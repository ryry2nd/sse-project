#include "GlRendering.hpp"

#include <engine/Rendering/Rendering.hpp>
#include <SDL3/SDL_video.h>
#include <spdlog/spdlog.h>

using namespace OpenGl;

using namespace Engine::Rendering;

#define DRAW_CALL(mesh, params) \
    { \
		if (params.useArray || !mesh) \
		{ \
			glDrawArrays( \
				GlMesh::convertMeshType(params.arrayType), \
				params.arrayFirst, params.arrayCount \
			); \
		} \
		else \
		{ \
			if (params.instanceCount) \
				glDrawElementsInstanced( \
					mesh->getMeshType(), \
					(GLsizei)mesh->getInd(), \
					GL_UNSIGNED_INT, \
					nullptr, \
					params.instanceCount \
				); \
			else \
				glDrawElements( \
					mesh->getMeshType(), \
					(GLsizei)mesh->getInd(), \
					GL_UNSIGNED_INT, \
					nullptr \
				); \
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

        GLuint unit = (GLuint)binding;

        glActiveTexture(GL_TEXTURE0 + unit);
        glBindTexture(GL_TEXTURE_2D, glimg->getID());
        glBindSampler(unit, glimg->getSID());
    }
}

InternalParams combineParams(InternalParams* params, size_t size)
{
    InternalParams out = *params;

    for (size_t i = 0; i < size; i++)
    {
        const auto& p = params[i];

		if (p.fieldMask & InternalParams::HasSettings) out.settings |= p.settings;
        if (p.fieldMask & InternalParams::HasInstanceCount) out.instanceCount = p.instanceCount;
        if (p.fbo) out.fbo = p.fbo;
        if (p.shader) out.shader = p.shader;
        if (p.fieldMask & InternalParams::HasUseArray) out.useArray = p.useArray;
        if (p.fieldMask & InternalParams::HasArrayData)
        {
            out.arrayType  = p.arrayType;
            out.arrayFirst = p.arrayFirst;
            out.arrayCount = p.arrayCount;
        }

        for (auto& [k, v] : p.images) out.images[k] = v;
        for (auto& [k, v] : p.ubo)    out.ubo[k] = v;
        for (auto& [k, v] : p.ssbo)   out.ssbo[k] = v;
    }

	return out;
}

void OpenGl::draw(Window *win, Mesh *mesh, InternalParams *pms, size_t size) {
	if (!pms) return;

	// auto params = *pms;
	auto params = combineParams(pms, size);

	if (!params.shader) return;

	auto *glwin = static_cast<GlWindow*>(win);
	auto *glshdr = static_cast<GlShader*>(params.shader);
	auto *glmesh = static_cast<GlMesh*>(mesh);

	FrameBuffer *fbo	= params.fbo;
	bool disableScreen	= params.settings & InternalParams::DisableScreen;
	bool useFBO       	= params.settings & InternalParams::EnableFBO;
	bool hasFBO       	= (fbo != nullptr);

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
	setImages(glshdr, params.images);


	// -------------------------
	// 2. BUFFERS Global
	// -------------------------
	loadUBO(glshdr, params.ubo);
	loadSSBO(glshdr, params.ssbo);


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