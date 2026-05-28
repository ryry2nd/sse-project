#include "GlRendering.hpp"

#include <engine/Rendering/Rendering.hpp>
#include <SDL3/SDL_video.h>
#include <spdlog/spdlog.h>
#include <glad/gl.h>

using namespace OpenGl;

using namespace Engine::Rendering;

#define DRAW_CALL(mesh, params) \
    { \
		if (mesh) \
		{ \
			if (params && params->instanceCount > 0) \
				glDrawElementsInstanced( \
					mesh->getMeshType(), \
					(GLsizei)mesh->getInd(), \
					GL_UNSIGNED_INT, \
					nullptr, \
					params->instanceCount \
				); \
			else \
				glDrawElements( \
					mesh->getMeshType(), \
					(GLsizei)mesh->getInd(), \
					GL_UNSIGNED_INT, \
					nullptr \
				); \
		} \
		else { \
			if (params) glDrawArrays(GL_LINES, params->vertFirst, params->vertCount); \
			else glDrawArrays(GL_LINES, 0, 6); \
		} \
    }

void loadUBO(GlShader *glshdr, std::unordered_map<size_t, Buff*> &ubo)
{
	if (ubo.empty()) return;

	for (auto &[binding, buf] : ubo)
	{
		if (!buf) continue;

		auto glbuf = static_cast<GlBuff*>(buf);

		glBindBufferBase(GL_UNIFORM_BUFFER,
						 (GLuint)binding,
						 glbuf->getID());
	}
}
void loadSSBO(GlShader *glshdr, std::unordered_map<size_t, Buff*> &ssbo)
{
	if (ssbo.empty()) return;

	for (auto &[binding, buf] : ssbo)
	{
		if (!buf) continue;

		auto *glbuf = static_cast<GlBuff*>(buf);

		glBindBufferBase(GL_SHADER_STORAGE_BUFFER,
						 (GLuint)binding,
						 glbuf->getID());
	}
}
void setImages(GlShader *glshdr, std::unordered_map<size_t, Image*> &images)
{
    if (images.empty()) return;

    for (auto &[binding, img] : images)
    {
		if (!img) continue;

        auto *glimg = static_cast<GlImage*>(img);

		glActiveTexture(GL_TEXTURE0 + (GLuint)binding);
		glBindTexture(GL_TEXTURE_2D, glimg->getID());
		glBindSampler((GLuint)binding, glimg->getSID());
    }
}

void OpenGl::draw(Window *win, Material *mat, Mesh *mesh, DrawParams *params = nullptr) {
	auto *glwin = static_cast<GlWindow*>(win);
	auto *glshdr = static_cast<GlShader*>(mat->shader);

	auto *glmesh = static_cast<GlMesh*>(mesh);

	bool disableScreen;
	bool useFBO;
	bool hasFBO;
	FrameBuffer *fbo = nullptr;

	if (params) {
		fbo = params->fbo;

		disableScreen = params->settings & DrawParams::DisableScreen;
		useFBO        = params->settings & DrawParams::EnableFBO;
		hasFBO        = (fbo != nullptr);
	}
	else {
		disableScreen = false;
		useFBO = false;
		hasFBO = false;
	}

	glUseProgram(glshdr->getID());

	if (glmesh)
		glBindVertexArray(glmesh->getVAO());
	else {
		GLuint defaultVAO;
		glGenVertexArrays(1, &defaultVAO);
		glBindVertexArray(defaultVAO);
	}


	// -------------------------
	// 1. TEXTURES
	// -------------------------
	setImages(glshdr, mat->images);

	if (params) {
		setImages(glshdr, params->images);
	}


	// -------------------------
	// 2. BUFFERS Global
	// -------------------------
	loadUBO(glshdr, mat->ubo);
	loadSSBO(glshdr, mat->ssbo);

	if (params) {
		loadUBO(glshdr, params->ubo);
		loadSSBO(glshdr, params->ssbo);
	}


	// -------------------------
	// 3. Render To FBO
	// -------------------------
	if (useFBO && hasFBO) {
		auto glFBO = static_cast<GlFrameBuffer*>(fbo);
		glBindFramebuffer(GL_FRAMEBUFFER, glFBO->getID());

		glViewport(0, 0, (GLsizei)fbo->getSize().x, (GLsizei)fbo->getSize().y);

		DRAW_CALL(glmesh, params);

		auto res = glwin->getRes();
		glViewport(0, 0, res.x, res.y);
	}

	// -------------------------
	// 4. RENDER TO SCREEN
	// -------------------------
	if (!disableScreen)
	{
		glBindFramebuffer(GL_FRAMEBUFFER, 0);

		DRAW_CALL(glmesh, params);
	}

	glBindVertexArray(0);
}