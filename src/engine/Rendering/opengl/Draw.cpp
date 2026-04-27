#include "GlRendering.hpp"

#include <engine/Rendering/Rendering.hpp>
#include <spdlog/spdlog.h>
#include <glad/gl.h>

using namespace OpenGl;


void loadBuffs(GlShader *glshdr, std::unordered_map<std::string, Rendering::Buff *> buffs) {
    auto &map = glshdr->getBindingMap();

    for (auto& [name, buf] : buffs)
    {
        auto it = map.find(name);

        if (it == map.end()) {
            spdlog::warn("Missing binding for buffer: {}", name);
            continue;
        }

        auto *glbuf = static_cast<OpenGl::GlBuff*>(buf);

        glBindBufferBase(GL_UNIFORM_BUFFER,
                        it->second,
                        glbuf->getID());
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
    int texUnit = 0;

    for (const auto& [name, img] : mat->images)
    {
        auto *glimg = static_cast<OpenGl::GlImage*>(img);

        glActiveTexture(GL_TEXTURE0 + texUnit);
        glBindTexture(GL_TEXTURE_2D, glimg->getID());

        GLint loc = glGetUniformLocation(glshdr->getID(), name.c_str());
        if (loc != -1)
            glUniform1i(loc, texUnit);

        texUnit++;
    }

    // -------------------------
    // 2. BUFFERS (correct type-aware binding)
    // -------------------------

    loadBuffs(glshdr, mat->buffs);
    loadBuffs(glshdr, params->buffers);

    // -------------------------
    // 3. DRAW
    // -------------------------
    glDrawElements(
        GL_TRIANGLES,
        static_cast<GLsizei>(glmesh->getInd()),
        GL_UNSIGNED_INT,
        nullptr
    );

    glBindVertexArray(0);
}