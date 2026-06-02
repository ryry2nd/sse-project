#include "GlRendering.hpp"
#include <spdlog/spdlog.h>
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

#define SLANG_EXT ".slang"

#define VERT_EXT ".vert.spv"
#define FRAG_EXT ".frag.spv"
#define FLAGS \
	" -O3" \
	" -line-directive-mode none" \
	" -matrix-layout-column-major" \
	" -target spirv" \
	" -DGL"

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

void OpenGl::GlShader::compileShaders(std::string prePath, std::string &vertPath, std::string &fragPath) {
	std::string possibleVert;
	std::string possibleFrag;

	possibleVert = prePath + VERT_EXT;
	possibleFrag = prePath + FRAG_EXT;

	if (
		fs::exists(possibleVert) &&
		fs::exists(possibleFrag)
	) {
		vertPath = possibleVert;
		fragPath = possibleFrag;
		return;
	}

	#ifdef MINIMAL
	spdlog::critical("Shader path {} does not exist. Shaders in engine's minimal setting has to be precompiled", prePath);
	std::exit(1);
	#else
	if (!fs::exists(SLANG_PATH EXE_EXT)) {
		spdlog::critical("Could not find Slang executable. Executable has to exist in: {}", SLANG_PATH EXE_EXT);
		std::exit(1);
	}

	std::string path = prePath + SLANG_EXT;
	fs::path entry(path);

	if (!fs::exists(entry)) {
		spdlog::critical("Could not find: {}", path);
		std::exit(1);
	}


	possibleVert = COMPILED_OUT_PATH "/" + entry.stem().string() + VERT_EXT;
	possibleFrag = COMPILED_OUT_PATH "/" + entry.stem().string() + FRAG_EXT;

	if (
		fs::exists(possibleVert) &&
		fs::exists(possibleFrag)
	) {
		vertPath = possibleVert;
		fragPath = possibleFrag;
		return;
	}

	fs::create_directories(COMPILED_OUT_PATH);

	std::string command;

	command = path +
		FLAGS
		" -stage vertex"
		" -entry vertMain"
		" -o " + COMPILED_OUT_PATH + "/" + entry.stem().string() + VERT_EXT;
	std::string out = compileShaderCode(command);
	if (out.size()) {
		spdlog::critical("Program: {} failed to compile vertex shader and gave the error:\n{}", id, out);
		std::exit(1);
	}

	command = path +
		FLAGS
		" -stage fragment"
		" -entry fragMain"
		" -o " + COMPILED_OUT_PATH + "/" + entry.stem().string() + FRAG_EXT;
	out = compileShaderCode(command);
	if (out.size()) {
		spdlog::critical("Program: {} failed to compile fragment shader and gave the error:\n{}", id, out);
		std::exit(1);
	}

	if (!fs::exists(possibleVert) || !fs::exists(possibleFrag)) {
		spdlog::critical("Program: {} silently errored in the background. Outputted compiled shader path does not exist:\n{}\n", id, possibleVert, possibleFrag);
		std::exit(1);
	}

	vertPath = possibleVert;
	fragPath = possibleFrag;

	spdlog::debug("Successfully compiled shader {}", entry.stem().string());
	#endif
}