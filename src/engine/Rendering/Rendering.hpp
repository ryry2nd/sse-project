#pragma once

#include <cstdint>
#include <string>
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <memory>
#include <spdlog/spdlog.h>

struct SDL_Surface;
struct SDL_Window;
struct SDL_Color;

typedef uint32_t Uint32;
typedef uint64_t Uint64;
typedef uint8_t Uint8;

namespace Rendering
{
	SDL_Color Vec4ToSDLColor(const glm::vec4& color);

	class Window
	{
	public:
		Window(const Window&) = delete;
		Window& operator=(const Window&) = delete;
		Window(Window&&) noexcept = default;
		Window& operator=(Window&&) noexcept = default;

		Window(glm::vec2 res, const char *name, Uint32 flags, Uint32 aa = 0, bool fullscreen = false, bool hideMouse = false);
		virtual ~Window() = 0;


		virtual void clearBackground() = 0;
		virtual void swapBuffer() = 0;
		virtual void updateScreenRes() = 0;
		virtual void setBackgroundColor(glm::vec4 color) = 0;
		static void Update();
		void update();
		static const bool *getKeystates(int &numKeys);

		virtual void disableDepthTest() = 0;
		virtual void enableDepthTest() = 0;
		virtual void enableBackfaceCull() = 0;
		virtual void disableBackfaceCull() = 0;

		static Uint64 now;

		static void init();
		static void shutdown();

		float fps = 0;
		float deltaTime = 0;
		glm::vec2 res;

	protected:
		SDL_Window *window;

	private:
		Uint64 lastCounter;
	};

	class Buff {
	public:
		enum class Type {
			Uniform,
			Storage,
		};
		enum class Frequency {
			Static,
			Dynamic,
			Stream,
		};

		Buff(Type type, Frequency freq, std::size_t size) {
			this->buffType = type;
			this->freq = freq;
			this->allocSize = size;
		}
		virtual ~Buff() = default;

		Buff(const Buff&) = delete;
		Buff& operator=(const Buff&) = delete;
		Buff(Buff&&) noexcept = default;
		Buff& operator=(Buff&&) noexcept = default;

		std::size_t getSize() {return allocSize;}
		Type getType() {return buffType;}
		Frequency getFrequency() {return freq;}

		virtual void write(const std::size_t offset, const std::size_t size, const void* data) = 0;
		virtual void read(const std::size_t offset, const std::size_t size, void* data) = 0;

	protected:
		static bool canRead(Type t) {
			switch (t) {
				case Type::Uniform:      return 0;
				case Type::Storage:      return 1;
			}
			return 0;
		}
		static bool canWrite(Type t, Frequency freq) {
			if (freq == Frequency::Static) return 0;
			switch (t) {
				case Type::Uniform:      return 1;
				case Type::Storage:      return 1;
			}
			return 0;
		}

		Type buffType;
		Frequency freq;
		std::size_t allocSize;
	};

	class Image
	{
	public:
		Image& operator=(const Image&) = delete;
		Image& operator=(Image&&) noexcept = default;

		virtual ~Image() = default;
		static SDL_Surface *loadFile(const char *filePath);
		glm::vec2 getSizes() {return imageSizes;}
		virtual void clearTransparent() = 0;
	protected:
		glm::vec2 imageSizes;
	};

	class FrameBuffer {
	public:
		enum Settings : uint32_t {
			Color = 1 << 0,
			Depth = 1 << 1,
			Stencil = 1 << 2,
		};

		virtual ~FrameBuffer() = default;
		glm::vec2 getSize() const {return size;}
		uint32_t getSettings() const {return settings;}
		virtual void setSize(glm::vec2 size) = 0;

		virtual Image* getColorImage() = 0;
		virtual Image* getDepthImage() = 0;
		virtual Image* getStencilImage() = 0;

	protected:
		glm::vec2 size;
		uint32_t settings;
	};

	class Shader
	{
	public:
		Shader& operator=(const Shader&) = delete;
		Shader& operator=(Shader&&) noexcept = default;


		virtual ~Shader() = default;
	};

	class Mesh
	{
	public:
		enum class MeshTypes
		{
			Points,
			Lines,
			Triangles
		};

		virtual ~Mesh() = default;

		Mesh& operator=(const Mesh&) = delete;
		Mesh& operator=(Mesh&&) noexcept = default;

		MeshTypes getMeshType() {return meshType;}
		void setMeshType(MeshTypes meshType) {this->meshType = meshType;}

		bool getBackFaceCulling() {return backFaceCulling;}
		void toggleBackFaceCulling() {backFaceCulling = !backFaceCulling;}
	protected:
		MeshTypes meshType;
		bool backFaceCulling = true;
	};



	struct Material {
		Shader *shader;
		std::unordered_map<size_t, Image*> images;
		std::unordered_map<size_t, Buff*> ubo;
		std::unordered_map<size_t, Buff*> ssbo;
	};

	struct DrawParams {
		enum Settings : uint32_t {
			DisableScreen = 1 << 0,
			EnableFBO = 1 << 1,
		};

		std::unordered_map<size_t, Buff*> ubo;
		std::unordered_map<size_t, Buff*> ssbo;
		size_t instanceCount;
		uint32_t settings;
		FrameBuffer *fbo = nullptr;
	};

	namespace CreationFunctions {
		void initAPI(const std::string &apiName);

		// ToDo important. make it so draw is not pulled through the dll every draw call. that might introduce performance issues
		void draw(Material *mat, Mesh *mesh = nullptr, DrawParams *params = nullptr);
		std::unique_ptr<Shader> createShader(std::string path);
		std::unique_ptr<Mesh> createMesh(const float *vertices, const size_t vert_size, const unsigned int *indices, const size_t ind_size, const short *vertLogic, const size_t vert_logic_size, Rendering::Mesh::MeshTypes meshType = Rendering::Mesh::MeshTypes::Triangles);
		std::unique_ptr<Image> createImage(const char *filePath);
		std::unique_ptr<Image> createImage(SDL_Surface *surface);
		std::unique_ptr<Window> createWindow(glm::vec2 res, const char *name, Uint32 flags, Uint32 aa = 0, bool fullscreen = false, int vsync = 0, bool hideMouse = true);
		std::unique_ptr<Buff> createBuff(Buff::Type type, Buff::Frequency freq, std::size_t size, const void* data = nullptr);
		std::unique_ptr<FrameBuffer> createFrameBuffer(glm::vec2 size, uint32_t settings);
	}
}