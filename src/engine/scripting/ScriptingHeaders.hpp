#pragma once
#include <string>
#include <vector>

namespace ScriptingHeaders
{
    class Package
    {
    public:
        std::string name;
        std::string id;
        std::string version;

        static std::vector<Package *> packages;

        Package(const std::string &path);
        ~Package();
        static void init();
        static void deinit();

    private:
        // static RuntimeInitArgs init_args;
        // wasm_module_inst_t module_inst;
        
        void includeGlm();
        void includeRendering();
        void includeBigVars();
        void includeObjects();
    };
}