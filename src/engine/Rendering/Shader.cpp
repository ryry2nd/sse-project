#include "Shader.hpp"
#include <fstream>
#include <filesystem>
#include <spdlog/spdlog.h>

namespace fs = std::filesystem;

#define VERT_EXT ".vert.spv"
#define FRAG_EXT ".frag.spv"
#define COMP_EXT ".comp.spv"
#define SHADERS_PATH "/gl/"

using namespace Engine::Rendering;

std::pair<std::vector<Uint32>, std::vector<Uint32>> Shader::getRaw(std::string prePath) {
	fs::path fsPath = prePath;
	std::string path = fsPath.parent_path().string() + SHADERS_PATH + fsPath.stem().string();
	std::string vertexPath = path + VERT_EXT;
	std::string fragmentPath = path + FRAG_EXT;

	if (!(fs::exists(vertexPath) && fs::exists(fragmentPath))) {
		spdlog::critical("Shader path {} does not exist.", path);
		std::exit(1);
	}

	spdlog::debug("creating shader with paths: \n{}\n{}", vertexPath, fragmentPath);

	size_t size;

	std::ifstream vertFile(vertexPath, std::ios::binary | std::ios::ate);
	size = vertFile.tellg();
	vertFile.seekg(0);
	std::vector<uint32_t> vertData(size / sizeof(uint32_t));
	vertFile.read(reinterpret_cast<char*>(vertData.data()), size);

	std::ifstream fragFile(fragmentPath, std::ios::binary | std::ios::ate);
	size = fragFile.tellg();
	fragFile.seekg(0);
	std::vector<uint32_t> fragData(size / sizeof(uint32_t));
	fragFile.read(reinterpret_cast<char*>(fragData.data()), size);

	return std::pair(vertData, fragData);
}

std::vector<Uint32> Shader::getRawCompute(std::string prePath) {
	fs::path fsPath = prePath;
	std::string path = fsPath.parent_path().string() + SHADERS_PATH + fsPath.stem().string();
	std::string compPath = path + COMP_EXT;

	if (!(fs::exists(compPath))) {
		spdlog::critical("Shader path {} does not exist.", path);
		std::exit(1);
	}

	spdlog::debug("creating shader with path: \n{}", compPath);

	size_t size;

	std::ifstream compFile(compPath, std::ios::binary | std::ios::ate);
	size = compFile.tellg();
	compFile.seekg(0);
	std::vector<uint32_t> compData(size / sizeof(uint32_t));
	compFile.read(reinterpret_cast<char*>(compData.data()), size);

	return compData;
}