#include "GlRendering.hpp"
#include <filesystem>

#define COMPILED_OUT_PATH "cache/compiled_shaders/OpenGl"
#define SLANG_PATH "bin-tools/slangc"

#ifdef _WIN32
#define POPEN _popen
#define PCLOSE _pclose
#define EXE_EXT ".exe"
#define BIN_PREFIX ""
#else
#define POPEN popen
#define PCLOSE pclose
#define EXE_EXT ""
#define BIN_PREFIX "./"
#endif

#define ASSET_SHADER "/assets/shaders"

#define VERT_EXT ".vert.spv"
#define FRAG_EXT ".frag.spv"

namespace fs = std::filesystem;

// scarry code, watch out, can execute random commands. DO NOT GIVE THIS TO THE HEADER
std::string compileShaderCode(const std::string& arguments) {
	std::string command = std::string(BIN_PREFIX) + SLANG_PATH + EXE_EXT + " " + arguments + " 2>&1";

	std::unique_ptr<FILE, int(*)(FILE*)> pipe(
		POPEN(command.c_str(), "r"),
		static_cast<int(*)(FILE*)>(PCLOSE)
	);

	if (!pipe) {
		return "Failed to run command";
	}

	std::string output;
	char buffer[256];

	while (fgets(buffer, sizeof(buffer), pipe.get()) != nullptr) {
		output += buffer;
	}
	return output;
}

int OpenGl::GlShader::compileShaders(std::string path, std::string &vertPath, std::string &fragPath) {
	fs::create_directories(COMPILED_OUT_PATH);

	std::filesystem::path entry(path);

	std::string command = path + " -target spirv -stage vertex -entry vertMain -O3 -line-directive-mode none -o " + COMPILED_OUT_PATH + "/" + entry.stem().string() + VERT_EXT;
	std::string out = compileShaderCode(command);
	if (out.size()) {
		spdlog::error("Program: {} failed to compile vertex shader and gave the error:\n{}", id, out);
		return 1;
	}

	vertPath = std::string() + COMPILED_OUT_PATH + "/" + entry.stem().string() + VERT_EXT;

	command = path + " -target spirv -stage fragment -entry fragMain -O3 -line-directive-mode none -o " + COMPILED_OUT_PATH + "/" + entry.stem().string() + FRAG_EXT;
	out = compileShaderCode(command);
	if (out.size()) {
		spdlog::error("Program: {} failed to compile fragment shader and gave the error:\n{}", id, out);
		return 1;
	}

	fragPath = std::string() + COMPILED_OUT_PATH + "/" + entry.stem().string() + FRAG_EXT;

	spdlog::debug("Successfully compiled shader {}", entry.stem().string());

	return 0;
}