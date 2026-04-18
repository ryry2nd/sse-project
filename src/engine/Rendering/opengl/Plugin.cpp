#include "GlRendering.hpp"

#define NAME "OpenGl4.6"

using namespace OpenGl;

extern "C" std::unique_ptr<Rendering::Shader> createShader(const char* vertex, const char* fragment) {
    return std::make_unique<GlShader>(vertex, fragment);
}

extern "C" std::unique_ptr<Rendering::Mesh> createMesh(Rendering::Shader *shady, const float *vertices, const size_t vert_size, const unsigned int *indices, const size_t ind_size, const short *vertLogic, const size_t vert_logic_size, const Rendering::MeshTypes &meshType = Rendering::MeshTypes::Triangles) {
    return std::make_unique<GlMesh>(shady, vertices, vert_size, indices, ind_size, vertLogic, vert_logic_size, meshType);
}

extern "C" std::unique_ptr<Rendering::Image> createImageFromFile(const std::string &filePath) {
    return std::make_unique<GlImage>(filePath);
}

extern "C" std::unique_ptr<Rendering::Image> createImageFromSurface(SDL_Surface *surface) {
    return std::make_unique<GlImage>(surface);
}

extern "C" std::unique_ptr<Rendering::Window> createWindow(glm::vec2 res, const char *name, Uint32 flags, Uint32 aa = 0, bool fullscreen = false, int vsync = 0, bool hideMouse = true) {
    return std::make_unique<GlWindow>(res, name, flags, aa, fullscreen, vsync, hideMouse);
}

extern "C" std::string getName() {return NAME;}