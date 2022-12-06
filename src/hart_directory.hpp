#pragma once

// INFERNO-HART Modules
// _GET returns derived HARTModule
// _DESTROY returns void but takes derived HARTModule
// _CREDIT returns ModuleCredit

// THIS IS SHARED DO __NOT__ REINCLUDE libhart/thirdparty
#include <inferno_hart.hpp>

#include <filesystem>
#include <string>
#include <vector>
#include <unordered_map>

namespace inferno {

class HARTModule;
struct ModuleCredit;

class HARTModuleDirectory
{
public:
    HARTModuleDirectory();
    ~HARTModuleDirectory();

    struct discoveryEntry
    {
        std::filesystem::path Location;
        bool DidLink = false;
#ifdef _WIN32
        HMODULE Handle;
#else // UNIX-Like
        void* Handle;
#endif
        ModuleCredit* Credit;
        HART_INIT_F InitCallback;
        HART_DESTROY_F DestroyCallback;
    };

    struct moduleEntry
    {
        discoveryEntry Discovery;
        HARTModule* Module;
        bool Active;
    };

    // This can take a file or a directory, and inside the directory discover recursively or not
    std::vector<discoveryEntry> discoverModules(std::filesystem::path path, bool recurse = false);
    discoveryEntry registerModule(std::filesystem::path file);

    std::vector<std::string> getModules();

    void setActive(std::string moduleName);
    void setActiveIndex(int index);
    
    HARTModule* getActiveModule();
    std::string getActive();
    int getActiveIndex();
    ModuleCredit* getActiveCredit();

    void load(std::string moduleName);
    void destroy(std::string moduleName);

private:
    std::string mActiveModule;
    std::unordered_map<std::string, moduleEntry> mEntries;
};

}

