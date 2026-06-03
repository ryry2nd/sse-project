#pragma once

#include <glad/gl.h>
#include <string>
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <spdlog/spdlog.h>

#include <engine/Rendering/Rendering.hpp>

typedef struct SDL_GLContextState *SDL_GLContext;

using namespace Engine::Rendering;

namespace OpenGL {
class GlBuff : public Buff {
public:
	GlBuff(Type type, Frequency freq, std::size_t size, const void* data = nullptr);
	~GlBuff();
	void write(const std::size_t offset, const std::size_t size, const void* data);
	void read(const std::size_t offset, const std::size_t size, void* data);

	GLuint getID() {return id;}
	GLenum getTarget() {return target;}
	GLenum getUsage() {return usage;}
private:
	GLuint id;
	GLenum target;
	GLenum usage;
};

class GlWindow : public Window
{
public:
	GlWindow(glm::vec2 res, const char *name, Uint32 flags, Uint32 aa = 0, bool fullscreen = false, int vsync = 0, bool hideMouse = true);
	void clearBackground();
	void swapBuffer();
	void updateScreenRes();
	void setBackgroundColor(glm::vec4 color);
	void setWindow();
	~GlWindow();

	SDL_Window *getWindow() {return window;}
	SDL_GLContext getContext() {return glContext;}
private:
	SDL_GLContext glContext;
};

class GlImage : public Image
{
public:
	GlImage(const char *filePath);
	GlImage(GLuint id, glm::vec2 size);
	GlImage(SDL_Surface *surface);
	~GlImage();
	GLuint getID() const {return textureID;}
	GLuint getSID() const {return samplerID;}

	void clearTransparent();

private:
	void setupObject(SDL_Surface *surface);
	GLuint textureID = 0;
	GLuint samplerID = 0;
};

class GlFrameBuffer : public FrameBuffer {
public:
	GlFrameBuffer(glm::vec2 size, uint32_t settings);
	~GlFrameBuffer();

	void setSize(glm::vec2 size);

	Image* getColorImage() {return colorImage;}
	Image* getDepthImage() {return depthImage;}
	Image* getStencilImage() {return stencilImage;}

	void updateRes();

	GLuint getID() {return id;}

private:
	GLuint id = 0;

	GlImage* colorImage = nullptr;
	GlImage* depthImage = nullptr;
	GlImage* stencilImage = nullptr;
};

class GlShader : public Shader
{
public:
	GlShader(std::string path);
	~GlShader();

	GLuint getID() {return id;}

private:
	GLuint id;
	void compileShaders(std::string path, std::string &vertPath, std::string &fragPath);
};

class GlMesh : public Mesh
{
public:
	GlMesh(const Vert *verts, const size_t vert_size, const Ind *indices, const size_t ind_size);
	~GlMesh();

	void updateVerts(const Vert *verts, const size_t vert_size, const Ind *indices, const size_t ind_size);

	GLuint getVAO() {return VAO;}
	GLuint getVBO() {return VBO;}
	GLuint getEBO() {return EBO;}

	size_t getInd() {return ind_size;}
	size_t getVert() {return vert_size;}
private:
	size_t ind_size;
	size_t vert_size;
	GLuint VAO, VBO, EBO;
};

void draw(Window *win, Mesh *mesh, InternalParams *params, size_t size);
}

using namespace OpenGL;