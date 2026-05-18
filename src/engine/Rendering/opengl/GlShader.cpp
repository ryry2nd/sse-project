#include "GlRendering.hpp"

#include <glad/gl.h>
#include <fstream>
#include <filesystem>

using namespace OpenGl;
using namespace Rendering;

void compileSpirV(const char *path, GLuint shader, const char* entryPoint) {
	std::ifstream file(path, std::ios::binary | std::ios::ate);

	size_t size = file.tellg();
	file.seekg(0);

	std::vector<uint32_t> data(size / sizeof(uint32_t));
	file.read(reinterpret_cast<char*>(data.data()), size);

	glShaderBinary(
		1,
		&shader,
		GL_SHADER_BINARY_FORMAT_SPIR_V_ARB,
		data.data(),
		data.size() * sizeof(uint32_t)
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
}

void compileGlSl(const char *path, GLuint shader)
{
    std::ifstream file(path, std::ios::binary | std::ios::ate);

    if (!file.is_open()) {
        spdlog::error("Failed to open shader file: {}", path);
        return;
    }

    size_t size = file.tellg();
    file.seekg(0);

    std::string source(size, '\0');
    file.read(source.data(), size);

    const char* src = source.c_str();

    glShaderSource(shader, 1, &src, nullptr);
    glCompileShader(shader);

    GLint success = 0;
    glGetShaderiv(shader, GL_COMPILE_STATUS, &success);

    if (!success) {
        char log[1024];
        glGetShaderInfoLog(shader, 1024, nullptr, log);
        spdlog::error("Shader compile failed ({}): {}", path, log);
    }
}

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
	std::string vertext = std::filesystem::path(vertexPath).extension().string();
	std::string fragext = std::filesystem::path(fragmentPath).extension().string();

	if (vertext == ".spv")
		compileSpirV(vertexPath.c_str(), vertex, "main");
	else if (vertext == ".glsl")
		compileGlSl(vertexPath.c_str(), vertex);
	if (fragext == ".spv")
		compileSpirV(fragmentPath.c_str(), fragment, "main");
	else if (fragext == ".glsl")
		compileGlSl(fragmentPath.c_str(), fragment);

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