#include "GlRendering.hpp"

#define ENTRY_POINT "main"

namespace fs = std::filesystem;

void compileShader(std::vector<Uint32> data, GLuint shader)
{
	glShaderBinary(
		1,
		&shader,
		GL_SHADER_BINARY_FORMAT_SPIR_V_ARB,
		data.data(),
		data.size() * sizeof(Uint32)
	);

	glSpecializeShaderARB(
		shader,
		ENTRY_POINT,
		0,
		nullptr,
		nullptr
	);

	GLint success = 0;
	glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
	if (!success) {
		char log[1024];
		glGetShaderInfoLog(shader, 1024, nullptr, log);
		spdlog::critical("SPIR-V shader failed: {}", log);
		std::exit(1);
	}
}

GlShader::GlShader(std::string path)
{
	auto [vertRaw, fragRaw] = getRaw(path);

	GLuint vertex = glCreateShader(GL_VERTEX_SHADER);
	GLuint fragment = glCreateShader(GL_FRAGMENT_SHADER);

	// ---- load binary files ----
	compileShader(vertRaw, vertex);
	compileShader(fragRaw, fragment);

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