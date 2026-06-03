#include "GlRendering.hpp"
#include <engine/Rendering/Rendering.hpp>
#include <memory>

#define NAME "OpenGl4.6"

extern "C" std::unique_ptr<Shader> createShader(std::string path) {
    return std::make_unique<GlShader>(path);
}

extern "C" std::unique_ptr<Mesh> createMesh(const Vert *vertices, const size_t vert_size, const Ind *indices, const size_t ind_size) {
    return std::make_unique<GlMesh>(vertices, vert_size, indices, ind_size);
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

extern "C" void draw(Window *win, Mesh* mesh, DrawParams* params, size_t size) {
    OpenGL::draw(win, mesh, params, size);
}

extern "C" const char *getName() {return NAME;}