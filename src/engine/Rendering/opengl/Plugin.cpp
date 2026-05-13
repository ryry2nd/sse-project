#include "GlRendering.hpp"
#include "engine/Rendering/Rendering.hpp"
#include <memory>

#define NAME "OpenGl4.6"

using namespace OpenGl;

extern "C" std::unique_ptr<Rendering::Shader> createShader(std::string path) {
    return std::make_unique<GlShader>(path);
}

extern "C" std::unique_ptr<Rendering::Mesh> createMesh(const float *vertices, const size_t vert_size, const unsigned int *indices, const size_t ind_size, const short *vertLogic, const size_t vert_logic_size, Rendering::Mesh::MeshTypes meshType = Rendering::Mesh::MeshTypes::Triangles) {
    return std::make_unique<GlMesh>(vertices, vert_size, indices, ind_size, vertLogic, vert_logic_size, meshType);
}

extern "C" std::unique_ptr<Rendering::Image> createImageFromFile(const char *filePath) {
    return std::make_unique<GlImage>(filePath);
}

extern "C" std::unique_ptr<Rendering::Image> createImageFromSurface(SDL_Surface *surface) {
    return std::make_unique<GlImage>(surface);
}

extern "C" std::unique_ptr<Rendering::Window> createWindow(glm::vec2 res, const char *name, Uint32 flags, Uint32 aa = 0, bool fullscreen = false, int vsync = 0, bool hideMouse = true) {
    return std::make_unique<GlWindow>(res, name, flags, aa, fullscreen, vsync, hideMouse);
}

extern "C" std::unique_ptr<Rendering::Buff> createBuff(Rendering::Buff::Type type, Rendering::Buff::Frequency freq, std::size_t size, const void* data = nullptr) {
    return std::make_unique<GlBuff>(type, freq, size, data);
}

extern "C" std::unique_ptr<Rendering::FrameBuffer> createFrameBuffer(glm::vec2 size, uint32_t settings) {
    return std::make_unique<GlFrameBuffer>(size, settings);
}

extern "C" void draw(Rendering::Material* mat, Rendering::Mesh* mesh, Rendering::DrawParams* params) {
    OpenGl::draw(mat, mesh, params);
}

extern "C" std::string getName() {return NAME;}