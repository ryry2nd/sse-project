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
class APIBuff : public Buff {
public:
	APIBuff(Type type, Frequency freq, std::size_t size, const void* data = nullptr);
	APIBuff(GLuint existingID, Type type, std::size_t size);
	~APIBuff();
	virtual void write(const std::size_t offset, const std::size_t size, const void* data);
	virtual void read(const std::size_t offset, const std::size_t size, void* data);
	virtual void copy(const Buff *src, std::size_t sourceOffset, std::size_t destinationOffset, std::size_t size);

	GLuint getID() {return id;}
	GLenum getTarget() {return target;}
private:
	GLuint id;
	GLenum target;
};

class APIWindow : public Window
{
public:
	APIWindow(glm::vec2 res, const char *name, Uint32 flags, Uint32 aa, Uint8 mode, int vsync);
	virtual void clearBackground();
	virtual void swapBuffer();
	virtual void setBackgroundColor(glm::vec4 color);
	virtual void updateScreenRes();
	virtual void setMode(Uint8);
	void setWindow();
	~APIWindow();

	SDL_Window *getWindow() {return window;}
	SDL_GLContext getContext() {return glContext;}
private:
	SDL_GLContext glContext;
	SDL_Window *window;
};

class APIImage : public Image
{
public:
	APIImage(const char *filePath, ImageSettings settings);
	APIImage(GLuint id, glm::vec2 size);
	APIImage(glm::vec2 size, ImageSettings settings);
	APIImage(Surface *surface, ImageSettings settings);
	~APIImage();
	GLuint getID() const {return textureID;}
	GLuint getSID() const {return samplerID;}

	virtual void updateImage(const Surface *surface);
	virtual void clearTransparent();

private:
	void setupObject(Surface *surface, ImageSettings settings);
	GLuint textureID = 0;
	GLuint samplerID = 0;
	bool useMip = false;
};

class APIFrameBuffer : public FrameBuffer {
public:
	APIFrameBuffer(glm::vec2 res, uint32_t settings);
	virtual ~APIFrameBuffer();

	virtual void setSize(glm::vec2 res);

	virtual void setBackgroundColor(glm::vec4 color);
	virtual void clearBackground();

	Image* getColorImage() {return colorImage;}
	Image* getDepthImage() {return depthImage;}
	Image* getStencilImage() {return stencilImage;}

	void updateRes();

	GLuint getID() {return id;}
private:
	GLuint id = 0;

	APIImage* colorImage = nullptr;
	APIImage* depthImage = nullptr;
	APIImage* stencilImage = nullptr;
};

class APIShader : public Shader
{
public:
	APIShader(const char *path, bool isCompute);
	~APIShader();

	GLuint getID() {return id;}

private:
	GLuint id;
};

class APIMesh : public Mesh
{
public:
	APIMesh(const Vert *verts, const size_t vert_size, const Ind *indices, const size_t ind_size);
	~APIMesh();

	size_t getInd() {return ind_size;}
	size_t getVert() {return vert_size;}

	GLuint getVAO() {return VAO;}

	virtual Buff *vertBuff();
	virtual Buff *indBuff();
private:
	size_t ind_size;
	size_t vert_size;

	std::unique_ptr<APIBuff> VBO;
	std::unique_ptr<APIBuff> EBO;

	GLuint VAO;
};
}

using namespace OpenGL;