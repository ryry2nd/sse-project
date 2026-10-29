#pragma once

#include <glad/gl.h>
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <spdlog/spdlog.h>

#include <engine/Rendering/Rendering.hpp>

#include "../internal/Scope.hpp"

#ifdef DEBUG
#include <tracy/TracyOpenGL.hpp>
#define GPUTRACE(name) TracyGpuZone(name);
#else
#define GPUTRACE(name)
#endif

typedef struct SDL_GLContextState *SDL_GLContext;

using namespace Engine::Rendering;

struct SDL_Window;

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
	void setBackgroundColor(glm::vec4 color);
	void updateScreenRes();
	void setWindow();
	~GlWindow();

	SDL_Window *getWindow() {return window;}
	SDL_GLContext getContext() {return glContext;}
private:
	SDL_GLContext glContext;
	SDL_Window *window;
};

class GlImage : public Image
{
public:
	GlImage(const char *filePath, ImageSettings settings);
	GlImage(GLuint id, glm::vec2 size);
	GlImage(glm::vec2 size, ImageSettings settings);
	GlImage(Surface *surface, ImageSettings settings);
	~GlImage();
	GLuint getID() const {return textureID;}
	GLuint getSID() const {return samplerID;}

	void updateImage(const Surface *surface);
	void clearTransparent();

private:
	void setupObject(Surface *surface, ImageSettings settings);
	GLuint textureID = 0;
	GLuint samplerID = 0;
	bool useMip = false;
};

class GlFrameBuffer : public FrameBuffer {
public:
	GlFrameBuffer(glm::vec2 res, uint32_t settings);
	virtual ~GlFrameBuffer();

	void setSize(glm::vec2 res);

	virtual void setBackgroundColor(glm::vec4 color);
	virtual void clearBackground();

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
	GlShader(const char *path, bool isCompute);
	~GlShader();

	GLuint getID() {return id;}

private:
	GLuint id;
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
}

using namespace OpenGL;