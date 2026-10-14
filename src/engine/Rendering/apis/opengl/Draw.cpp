#include "GlRendering.hpp"

#include <engine/Rendering/Rendering.hpp>
#include <SDL3/SDL_video.h>

#include "../internal/DrawAgnostic.hpp"

inline GLenum convertMeshType(InternalParams::MeshTypes meshType) {
	switch (meshType)
	{
		case InternalParams::MeshTypes::Points:     return GL_POINTS;
		case InternalParams::MeshTypes::Lines:      return GL_LINES;
		case InternalParams::MeshTypes::Triangles:  return GL_TRIANGLES;
	}
}

#define DRAW_CALL(mesh, params) \
{ \
	if (params.useArray || !mesh) \
	{ \
		glDrawArrays( \
			convertMeshType(params.meshType), \
			params.arrayFirst, params.arrayCount \
		); \
	} \
	else \
	{ \
		if (params.instanceCount) \
			glDrawElementsInstanced( \
				convertMeshType(params.meshType), \
				(GLsizei)mesh->getInd(), \
				GL_UNSIGNED_INT, \
				nullptr, \
				params.instanceCount \
			); \
		else \
			glDrawElements( \
				convertMeshType(params.meshType), \
				(GLsizei)mesh->getInd(), \
				GL_UNSIGNED_INT, \
				nullptr \
			); \
	} \
}

#define loadBuffer(glshdr, buffs, type) \
{ \
	if (!buffs.empty()) \
		for (auto &[binding, buf] : buffs) \
		{ \
			if (!buf) continue; \
			auto glbuf = static_cast<GlBuff*>(buf); \
			GLuint unit = (GLuint)(binding); \
			glBindBufferBase(type, unit, glbuf->getID()); \
		} \
}

#define setImages(glshdr, images) \
{ \
    if (!images.empty()) \
        for (auto &[binding, img] : images) \
        { \
            if (!img) continue; \
            auto *glimg = static_cast<GlImage*>(img); \
            GLuint unit = (GLuint)(binding); \
            glActiveTexture(GL_TEXTURE0 + unit); \
            glBindTexture(GL_TEXTURE_2D, glimg->getID()); \
            glBindSampler(unit, glimg->getSID()); \
        } \
}

extern "C" void draw(Window *win, Mesh* mesh, DrawParams* pms, size_t size) {
	InternalParams params;

	bool renderScreen;
	bool renderFBO;
	FrameBuffer *fbo;

	drawinit(win, pms, size, params, fbo, renderScreen, renderFBO);

	auto *glwin = static_cast<GlWindow*>(win);
	auto *glshdr = static_cast<GlShader*>(params.shader);
	auto *glmesh = static_cast<GlMesh*>(mesh);

	glwin->setWindow();

	glUseProgram(glshdr->getID());

	static GLuint defaultVAO = 0;

	if (!defaultVAO)
		glGenVertexArrays(1, &defaultVAO);

	glBindVertexArray(mesh ? glmesh->getVAO() : defaultVAO);


	// -------------------------
	// 1. TEXTURES
	// -------------------------
	setImages(glshdr, params.images);


	// -------------------------
	// 2. BUFFERS Global
	// -------------------------
	loadBuffer(glshdr, params.ubo, GL_UNIFORM_BUFFER);
	loadBuffer(glshdr, params.ssbo, GL_SHADER_STORAGE_BUFFER);


	if (params.settings & InternalParams::EnableClear)
		glEnable(GL_BLEND);
	else
		glDisable(GL_BLEND);

	if (params.settings & InternalParams::DisableCulling)
		glDisable(GL_CULL_FACE);
	else
		glEnable(GL_CULL_FACE);

	if (params.settings & InternalParams::DisableDepth)
		glDisable(GL_DEPTH_TEST);
	else
		glEnable(GL_DEPTH_TEST);


	// -------------------------
	// 3. Render To FBO
	// -------------------------
	if (renderFBO) {
		auto glFBO = static_cast<GlFrameBuffer*>(fbo);
		glBindFramebuffer(GL_FRAMEBUFFER, glFBO->getID());
		glFBO->updateRes();
		DRAW_CALL(glmesh, params);
	}

	// -------------------------
	// 4. RENDER TO SCREEN
	// -------------------------

	if (renderScreen) {
		glBindFramebuffer(GL_FRAMEBUFFER, 0);
		glwin->updateScreenRes();
		DRAW_CALL(glmesh, params);
	}

	glBindVertexArray(0);
}