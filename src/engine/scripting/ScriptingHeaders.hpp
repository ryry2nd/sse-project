#pragma once
#include <string>

struct RuntimeInitArgs;
struct wasm_module_inst_t;

namespace ScriptingHeaders
{
    struct Package
    {
        std::string name;
        std::string id;
        std::string version;

        static std::vector<Package *> packages;

        Package(const std::string &path);
        ~Package();
        static void init();
        static void deinit();

    private:
        static RuntimeInitArgs init_args;
        wasm_module_inst_t module_inst;
        
        void includeGlm();
        void includeRendering();
        void includeBigVars();
        void includeObjects();
    };
}