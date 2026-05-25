#include "GlRendering.hpp"

#include <engine/Rendering/Rendering.hpp>
#include <spdlog/spdlog.h>
#include <glad/gl.h>

using namespace OpenGl;

using namespace Engine::Rendering;

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

void OpenGl::draw(Material *mat, Mesh *mesh, DrawParams *params = nullptr) {
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
	glBindVertexArray(glmesh->getVAO());


	// -------------------------
	// 1. TEXTURES
	// -------------------------
	setImages(glshdr, mat->images);


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

		if (params->instanceCount > 0)
		{
			glDrawElementsInstanced(
				glmesh->getMeshType(),
				(GLsizei)glmesh->getInd(),
				GL_UNSIGNED_INT,
				nullptr,
				params->instanceCount
			);
		}
		else
		{
			glDrawElements(
				glmesh->getMeshType(),
				(GLsizei)glmesh->getInd(),
				GL_UNSIGNED_INT,
				nullptr
			);
		}
	}

	// -------------------------
	// 4. RENDER TO SCREEN
	// -------------------------
	if (!disableScreen)
	{
		glBindFramebuffer(GL_FRAMEBUFFER, 0);

		// NOTE: ideally restore window viewport here

		if (params && params->instanceCount > 0)
		{
			glDrawElementsInstanced(
				glmesh->getMeshType(),
				(GLsizei)glmesh->getInd(),
				GL_UNSIGNED_INT,
				nullptr,
				params->instanceCount
			);
		}
		else
		{
			glDrawElements(
				glmesh->getMeshType(),
				(GLsizei)glmesh->getInd(),
				GL_UNSIGNED_INT,
				nullptr
			);
		}
	}

	glBindVertexArray(0);
}