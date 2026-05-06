#include "GlRendering.hpp"

#include <glad/gl.h>
#include <fstream>
#include <filesystem>

using namespace OpenGl;
using namespace Rendering;

GlShader::GlShader(std::string path)
{
	std::string vertexPath;
	std::string fragmentPath;
	compileShaders(path, vertexPath, fragmentPath);

	spdlog::debug("creating shader with paths: \n{}\n{}", vertexPath, fragmentPath);

	if (!std::filesystem::exists(vertexPath)) {
		spdlog::error("Shader path: {} does not exist", vertexPath);
		return;
	}
	if (!std::filesystem::exists(fragmentPath)) {
		spdlog::error("Shader path: {} does not exist", fragmentPath);
		return;
	}

	GLuint vertex = glCreateShader(GL_VERTEX_SHADER);
	GLuint fragment = glCreateShader(GL_FRAGMENT_SHADER);

	// ---- load binary files ----
	auto loadSPV = [](const std::string& path) -> std::vector<uint32_t> {
		std::ifstream file(path, std::ios::binary | std::ios::ate);

		size_t size = file.tellg();
		file.seekg(0);

		std::vector<uint32_t> data(size / sizeof(uint32_t));
		file.read(reinterpret_cast<char*>(data.data()), size);

		return data;
	};

	auto vertSPV = loadSPV(vertexPath);
	auto fragSPV = loadSPV(fragmentPath);

	// ---- helper for OpenGL SPIR-V ----
	auto compileSPIRV = [](GLuint shader,
						const std::vector<uint32_t>& spirv,
						const char* entryPoint)
	{
		glShaderBinary(
			1,
			&shader,
			GL_SHADER_BINARY_FORMAT_SPIR_V_ARB,
			spirv.data(),
			spirv.size() * sizeof(uint32_t)
		);

		glSpecializeShaderARB(
			shader,
			entryPoint,
			0,
			nullptr,
			nullptr
		);

		GLint success = 0;
		glGetShaderiv(shader, GL_COMPILE_STATUS, &success);

		if (!success) {
			char log[1024];
			glGetShaderInfoLog(shader, 1024, nullptr, log);
			spdlog::error("SPIR-V shader failed: {}", log);
		}
	};

	// ---- compile ----
	compileSPIRV(vertex, vertSPV, "main");
	compileSPIRV(fragment, fragSPV, "main");

	// ---- link program ----
	id = glCreateProgram();
	glAttachShader(id, vertex);
	glAttachShader(id, fragment);
	glLinkProgram(id);

	// Delete shaders; linked into program now and no longer needed
	glDeleteShader(vertex);
	glDeleteShader(fragment);

	spdlog::debug("Set shader id to: 0x{:x}", id);

	// ============================
	// UBO REFLECTION
	// ============================

	GLuint nextBinding = 0;
	size_t nextCpuBinding = 0;
	GLint uniformCount = 0;

	glGetProgramInterfaceiv(id, GL_UNIFORM_BLOCK, GL_ACTIVE_RESOURCES, &uniformCount);

	for (GLint i = 0; i < uniformCount; i++)
	{
		size_t cpuBinding = nextCpuBinding++;
		GLuint gpuBinding = nextBinding++;

		glUniformBlockBinding(id, i, gpuBinding);

		uboMap[cpuBinding] = gpuBinding;

	#ifdef DEBUG
		spdlog::debug("UBO CPU {} -> GPU {}", cpuBinding, gpuBinding);
	#endif
	}

	// ============================
	// SSBO REFLECTION
	// ============================

	nextBinding = 0;
	nextCpuBinding = 0;
	uniformCount = 0;

	glGetProgramInterfaceiv(id, GL_SHADER_STORAGE_BLOCK, GL_ACTIVE_RESOURCES, &uniformCount);

	for (GLint i = 0; i < uniformCount; i++)
	{
		size_t cpuBinding = nextCpuBinding++;
		GLuint gpuBinding = nextBinding++;

		glShaderStorageBlockBinding(id, i, gpuBinding);

		ssboMap[cpuBinding] = gpuBinding;

	#ifdef DEBUG
		spdlog::debug("SSBO CPU {} -> GPU {}", cpuBinding, gpuBinding);
	#endif
	}

	// =========================
	// TEXTURE SLOTS (NEW)
	// =========================

	nextBinding = 0;
	nextCpuBinding = 0;
	uniformCount = 0;
	glGetProgramInterfaceiv(id, GL_UNIFORM, GL_ACTIVE_RESOURCES, &uniformCount);

	for (GLint i = 0; i < uniformCount; i++)
	{
		GLenum props[] = { GL_TYPE };
		GLint type = 0;

		glGetProgramResourceiv(id, GL_UNIFORM, i,
							1, props,
							1, nullptr,
							&type);

		if (type != GL_SAMPLER_2D)
			continue;

		imageMap[nextCpuBinding] = nextBinding;
		#ifdef DEBUG
		spdlog::debug("TEX CPU {} -> GPU {}", nextCpuBinding++, nextBinding);
		nextBinding += 2;
		#endif
	}
}

GlShader::~GlShader()
{
	spdlog::debug("Deleting shader with id: 0x{:x}", id);
	if (id != 0)
		glDeleteProgram(id);
}

GLuint GlShader::getID() {
	return id;
}