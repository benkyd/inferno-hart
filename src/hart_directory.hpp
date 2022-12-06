#pragma once

// INFERNO-HART Modules
// _GET returns derived HARTModule
// _DESTROY returns void but takes derived HARTModule
// _CREDIT returns ModuleCredit

#include <filesystem>
#include <string>
#include <vector>
#include <unordered_map>

namespace inferno {

class HARTModule;
struct ModuleCredit;

#ifdef _WIN32
#include <Windows.h>
#define HART_EXTENSION ".dll"
#define HART_INTERFACE extern "C" __declspec(dllexport)
#else // UNIX-Like
#include <dlfcn.h>
#define HART_EXTENSION ".so"
#define HART_INTERFACE extern "C"
#endif

HART_INTERFACE typedef void* (*HART_INIT_F)(void);
HART_INTERFACE typedef void (*HART_DESTROY_F)(void*);
HART_INTERFACE typedef void* (*HART_CREDIT_F)(void);

typedef void (*HART_HIT_CALLBACK)(HitInfo* hit);

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

