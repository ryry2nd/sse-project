#include "ScriptingHeaders.hpp"
#include <yaml-cpp/yaml.h>
#include <filesystem>

using namespace ScriptingHeaders;

void Package::init() {
    // // Initialize WAMR runtime
    // memset(&init_args, 0, sizeof(RuntimeInitArgs));
    // init_args.mem_alloc_type = Alloc_With_System_Allocator;

    // if(!wasm_runtime_full_init(&init_args)) {
    //     std::cerr << "Failed to init WAMR runtime\n";
    // }
}

void Package::deinit() {
    // wasm_runtime_destroy();
}

Package::Package(const std::string &path)
{
    // if(!wasm_runtime_full_init(&init_args)) {
    //     std::cerr << "You need to Initialise first. Run Package::init()\n";
    // }
    // packages.push_back(this);
    // YAML::Node config = YAML::LoadFile(path + "/config.yaml");

    // name = config["name"].as<std::string>();
    // id = config["id"].as<std::string>();
    // version = config["version"].as<std::string>();

    // // Load WASM file into memory
    // std::ifstream f(config["script_path"].as<std::string>(), std::ios::binary | std::ios::ate);
    // if(!f.is_open()) {
    //     std::cerr << "Failed to open WASM file: " << wasm_file << "\n";
    //     return -1;
    // }
    // std::streamsize size = f.tellg();
    // f.seekg(0, std::ios::beg);
    // std::vector<uint8_t> wasm_bytes(size);
    // if(!f.read((char*)wasm_bytes.data(), size)) {
    //     std::cerr << "Failed to read WASM file\n";
    //     return -1;
    // }

    // // Load module
    // wasm_module_t module = wasm_runtime_load(wasm_bytes.data(), (uint32_t)size, nullptr, 0);
    // if(!module) {
    //     std::cerr << "Failed to load WASM module\n";
    //     return -1;
    // }

    // // Instantiate module
    // module_inst = wasm_runtime_instantiate(
    //     module,
    //     64 * 1024,  // stack size
    //     64 * 1024,  // heap size
    //     nullptr,    // error buffer
    //     0           // error buffer size
    // );

    // if(!module_inst) {
    //     std::cerr << "Failed to instantiate WASM module\n";
    //     return;
    // }

    // // Cleanup
    // wasm_runtime_unload(module);

    // if (config["permissions"])
    // {
    //     if (config["permissions"]["include_glm"] && config["permissions"]["include_glm"].as<bool>())
    //     {
    //         includeGlm();
    //     }
    //     if (config["permissions"]["include_bigvars"] && config["permissions"]["include_bigvars"].as<bool>())
    //     {
    //         includeBigVars();
    //     }
    //     if (config["permissions"]["include_rendering"] && config["permissions"]["include_rendering"].as<bool>())
    //     {
    //         includeRendering();
    //     }
    //     if (config["permissions"]["include_objects"] && config["permissions"]["include_objects"].as<bool>())
    //     {
    //         includeObjects();
    //     }
    // }

    // // Execute _start or main function
    // if(!wasm_application_execute_main(module_inst, 0, nullptr)) {
    //     std::cerr << "WASM execution failed\n";
    // }
}

Package::~Package()
{
    // auto it = std::find(packages.begin(), packages.end(), this);
    // if (it != packages.end())
    //     packages.erase(it);
    // wasm_runtime_deinstantiate(module_inst);
}