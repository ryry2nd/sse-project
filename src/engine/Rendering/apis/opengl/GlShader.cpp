#include "GlRendering.hpp"

#include <fstream>
#include <filesystem>

#define SHADERS_PATH "/gl/"
#define VERT_EXT ".vert.spv"
#define FRAG_EXT ".frag.spv"
#define ENTRY_POINT "main"

namespace fs = std::filesystem;

#define compileSpirV(path, shader) \
{ \
	std::ifstream file(path, std::ios::binary | std::ios::ate); \
	size_t size = file.tellg(); \
	file.seekg(0); \
	std::vector<uint32_t> data(size / sizeof(uint32_t)); \
	file.read(reinterpret_cast<char*>(data.data()), size); \
	\
	glShaderBinary( \
		1, \
		&shader, \
		GL_SHADER_BINARY_FORMAT_SPIR_V_ARB, \
		data.data(), \
		data.size() * sizeof(uint32_t) \
	); \
	\
	glSpecializeShaderARB( \
		shader, \
		ENTRY_POINT, \
		0, \
		nullptr, \
		nullptr \
	); \
	\
	GLint success = 0; \
	glGetShaderiv(shader, GL_COMPILE_STATUS, &success); \
	if (!success) { \
		char log[1024]; \
		glGetShaderInfoLog(shader, 1024, nullptr, log); \
		spdlog::critical("SPIR-V shader failed: {}", log); \
		std::exit(1); \
	} \
}

GlShader::GlShader(std::string prePath)
{
	fs::path fsPath = prePath;
	std::string path = fsPath.parent_path().string() + SHADERS_PATH + fsPath.stem().string();
	std::string vertexPath = path + VERT_EXT;
	std::string fragmentPath = path + FRAG_EXT;

	if (!(fs::exists(vertexPath) && fs::exists(fragmentPath))) {
		spdlog::critical("Shader path {} does not exist.", path);
		std::exit(1);
	}

	spdlog::debug("creating shader with paths: \n{}\n{}", vertexPath, fragmentPath);

	GLuint vertex = glCreateShader(GL_VERTEX_SHADER);
	GLuint fragment = glCreateShader(GL_FRAGMENT_SHADER);

	// ---- load binary files ----
	compileSpirV(vertexPath.c_str(), vertex);
	compileSpirV(fragmentPath.c_str(), fragment);

	// ---- link program ----
	id = glCreateProgram();
	glAttachShader(id, vertex);
	glAttachShader(id, fragment);
	glLinkProgram(id);

	// Delete shaders; linked into program now and no longer needed
	glDeleteShader(vertex);
	glDeleteShader(fragment);

	spdlog::debug("Set shader id to: 0x{:x}", id);
}

GlShader::~GlShader()
{
	spdlog::debug("Deleting shader with id: 0x{:x}", id);
	if (id != 0)
		glDeleteProgram(id);
}