#include "GlRendering.hpp"

#include <engine/Rendering/Rendering.hpp>
#include <glad/gl.h>

using namespace OpenGl;


void loadUBO(GlShader *glshdr, std::unordered_map<size_t, Rendering::Buff*> &ubo)
{
	if (ubo.empty()) return;

	auto &map = glshdr->getUBOMap();

	for (auto &[cpuBinding, buf] : ubo)
	{
		if (!buf) continue;

		auto glbuf = static_cast<OpenGl::GlBuff*>(buf);

		auto it = map.find(cpuBinding);
		if (it == map.end()) continue;

		GLuint gpuBinding = it->second;

		glBindBufferBase(GL_UNIFORM_BUFFER,
						 gpuBinding,
						 glbuf->getID());
	}
}
void loadSSBO(GlShader *glshdr, std::unordered_map<size_t, Rendering::Buff*> &ssbo)
{
	if (ssbo.empty()) return;

	auto &map = glshdr->getSSBOMap();

	for (auto &[cpuBinding, buf] : ssbo)
	{
		if (!buf) continue;

		auto *glbuf = static_cast<OpenGl::GlBuff*>(buf);

		auto it = map.find(cpuBinding);
		if (it == map.end()) continue;

		GLuint gpuBinding = it->second;

		glBindBufferBase(GL_SHADER_STORAGE_BUFFER,
						 gpuBinding,
						 glbuf->getID());
	}
}
void setImages(GlShader *glshdr, std::unordered_map<size_t, Rendering::Image*> &images)
{
    if (images.empty()) return;

    auto &map = glshdr->getImageMap();

    for (auto &[cpuBinding, img] : images)
    {
        auto *glimg = static_cast<OpenGl::GlImage*>(img);

        auto it = map.find(cpuBinding);
        if (it == map.end()) continue;

        GLuint gpuBinding = it->second;

        glBindTextureUnit(gpuBinding, glimg->getID());
        glBindSampler(gpuBinding, glimg->getSID());
    }
}

void OpenGl::draw(Rendering::Material *mat, Rendering::Mesh *mesh, Rendering::DrawParams *params = nullptr) {
	auto *glshdr = static_cast<OpenGl::GlShader*>(mat->shader);
	auto *glmesh = static_cast<OpenGl::GlMesh*>(mesh);

	bool disableScreen;
	bool useFBO;
	bool hasFBO;
	Rendering::FrameBuffer *fbo = nullptr;

	if (params) {
		fbo = params->fbo;

		disableScreen = params->settings & Rendering::DrawParams::DisableScreen;
		useFBO        = params->settings & Rendering::DrawParams::EnableFBO;
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
	// setImages(glshdr, mat->images);
	auto *glimg = static_cast<OpenGl::GlImage*>(mat->images[0]);
	// HARD FORCE texture 0
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, glimg->getID());

	// FORCE sampler slot 0 (optional but safe)
	glBindSampler(0, glimg->getSID());

	// -------------------------
	// 2. BUFFERS Global
	// -------------------------`
	loadUBO(glshdr, mat->ubo);
	loadSSBO(glshdr, mat->ssbo);

	if (params) {
		loadUBO(glshdr, params->ubo);
		loadSSBO(glshdr, params->ssbo);
	}


	// -------------------------
	// 3. Render To FBO
	// -------------------------
	if (useFBO) {
		if (!hasFBO) {
			spdlog::warn("DrawParams: EnableFBO set but fbo is nullptr");

			if (disableScreen)
			{
				glBindVertexArray(0);
				return; // nowhere to render
			}
		}
		else {
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