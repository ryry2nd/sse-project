#include "GlRendering.hpp"
#include <engine/Rendering/Rendering.hpp>
#include <memory>
#include <spdlog/spdlog.h>

#define NAME "OpenGl4.6"

using namespace OpenGl;
using namespace Engine::Rendering;

extern "C" std::unique_ptr<Shader> createShader(std::string path) {
    return std::make_unique<GlShader>(path);
}

extern "C" std::unique_ptr<Mesh> createMesh(const float *vertices, const size_t vert_size, const unsigned int *indices, const size_t ind_size, const short *vertLogic, const size_t vert_logic_size, Mesh::MeshTypes meshType = Mesh::MeshTypes::Triangles) {
    return std::make_unique<GlMesh>(vertices, vert_size, indices, ind_size, vertLogic, vert_logic_size, meshType);
}

extern "C" std::unique_ptr<Image> createImageFromFile(const char *filePath) {
    return std::make_unique<GlImage>(filePath);
}

extern "C" std::unique_ptr<Image> createImageFromSurface(SDL_Surface *surface) {
    return std::make_unique<GlImage>(surface);
}

extern "C" std::unique_ptr<Window> createWindow(glm::vec2 res, const char *name, Uint32 flags, Uint32 aa = 0, bool fullscreen = false, int vsync = 0, bool hideMouse = true) {
    return std::make_unique<GlWindow>(res, name, flags, aa, fullscreen, vsync, hideMouse);
}

extern "C" std::unique_ptr<Buff> createBuff(Buff::Type type, Buff::Frequency freq, std::size_t size, const void* data = nullptr) {
    return std::make_unique<GlBuff>(type, freq, size, data);
}

extern "C" std::unique_ptr<FrameBuffer> createFrameBuffer(glm::vec2 size, uint32_t settings) {
    return std::make_unique<GlFrameBuffer>(size, settings);
}

extern "C" void draw(Material* mat, Mesh* mesh, DrawParams* params) {
    OpenGl::draw(mat, mesh, params);
}

extern "C" const char *getName() {return NAME;}