#include "GlRendering.hpp"

#include <engine/Rendering/Rendering.hpp>
#include <spdlog/spdlog.h>
#include <glad/gl.h>

using namespace OpenGl;


void loadBuffs(GlShader *glshdr, std::unordered_map<std::string, Rendering::Buff *> &buffs) {
    if (buffs.empty()) return;

    auto &map = glshdr->getBindingMap();

    for (auto& [name, buf] : buffs)
    {
        if (buf == nullptr) {
            spdlog::warn("Buffer: {} is null", name);
            continue;
        }

        auto it = map.find(name);

        if (it == map.end()) {
            spdlog::warn("Missing binding for buffer: {}", name);
            continue;
        }

        auto *glbuf = static_cast<OpenGl::GlBuff*>(buf);

        glBindBufferBase(glbuf->getTarget(),
                        it->second,
                        glbuf->getID());
    }
}

int bindInstanceBuffers(GlShader *glshdr, const std::unordered_map<std::string, std::vector<Rendering::Buff*>> &instanceBuffers)
{
    if (instanceBuffers.empty())
        return 0;

    int instanceCount = -1;

    for (const auto& [name, bufList] : instanceBuffers)
    {
        for (auto *buf : bufList)
        {
            if (!buf)
                continue;

            auto *glbuf = static_cast<OpenGl::GlBuff*>(buf);
            int size = glbuf->getSize();

            if (instanceCount == -1)
                instanceCount = size;
            else
                instanceCount = std::min(instanceCount, size);
        }
    }

    return instanceCount == -1 ? 0 : instanceCount;
}

void setImages(GlShader *glshdr, std::unordered_map<std::string, Rendering::Image *> &images) {
    if (images.empty()) return;

    int texUnit = 0;

    for (const auto& [name, img] : images)
    {
        auto *glimg = static_cast<OpenGl::GlImage*>(img);

        glActiveTexture(GL_TEXTURE0 + texUnit);
        glBindTexture(GL_TEXTURE_2D, glimg->getID());

        GLint loc = glGetUniformLocation(glshdr->getID(), name.c_str());
        if (loc != -1)
            glUniform1i(loc, texUnit);

        texUnit++;
    }
}

void OpenGl::draw(Rendering::Material *mat, Rendering::Mesh *mesh, Rendering::DrawParams *params) {
    auto *glshdr = static_cast<OpenGl::GlShader*>(mat->shader);
    auto *glmesh = static_cast<OpenGl::GlMesh*>(mesh);

    glUseProgram(glshdr->getID());
    glBindVertexArray(glmesh->getVAO());

    // -------------------------
    // 1. TEXTURES
    // -------------------------
    setImages(glshdr, mat->images);

    // -------------------------
    // 2. BUFFERS Global
    // -------------------------`
    loadBuffs(glshdr, mat->buffs);
    loadBuffs(glshdr, params->buffers);


    // -------------------------
    // 3. DRAW
    // -------------------------
    if (params->instanceCount > 0)
    {
        glDrawElementsInstanced(
            glmesh->getMeshType(),
            static_cast<GLsizei>(glmesh->getInd()),
            GL_UNSIGNED_INT,
            nullptr,
            params->instanceCount
        );
    }
    else
    {
        glDrawElements(
            glmesh->getMeshType(),
            static_cast<GLsizei>(glmesh->getInd()),
            GL_UNSIGNED_INT,
            nullptr
        );
    }

    glBindVertexArray(0);
}