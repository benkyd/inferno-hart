#include "hart_directory.hpp"

#include "inferno_hart.hpp"

#include <yolo/yolo.hpp>

#include <iostream>

using namespace inferno;

HARTModuleDirectory::HARTModuleDirectory()
{

}

HARTModuleDirectory::~HARTModuleDirectory()
{

}

std::vector<HARTModuleDirectory::discoveryEntry> HARTModuleDirectory::discoverModules(std::filesystem::path folder, bool recurse)
{
    if (!std::filesystem::exists(folder)) return { };
    if (!std::filesystem::is_directory(folder))
    {
        return { this->registerModule(folder) };
    }

    std::error_code err;
    if (recurse)
    {
        for (const auto& file : std::filesystem::recursive_directory_iterator(folder, err))
        {
            if (file.path().extension() == HART_EXTENSION)
            {
                this->registerModule(file);   
            }
        }
    } else 
    {
        for (const auto& file : std::filesystem::directory_iterator(folder, err))
        {
            if (file.path().extension() == HART_EXTENSION)
            {
                this->registerModule(file);   
            }
        }
    }

    return { };
}

HARTModuleDirectory::discoveryEntry HARTModuleDirectory::registerModule(std::filesystem::path file)
{
    yolo::info("Registering module at {}", file.c_str());

    discoveryEntry entry;
    entry.Location = file;

#ifdef _WIN32
    entry.Handle = LoadLibraryA(file.c_str());
    if (entry.Handle == NULL) 
    {
        yolo::error("Cannot load module at {}.", file.c_str());
        entry.Handle = NULL; entry.DidLink = false;
        return entry;
    }
    HART_CREDIT_F credit = (HART_CREDIT_F)GetProcAddress(entry.Handle, "_CREDIT");
    entry.InitCallback = (HART_INIT_F)GetProcAddress(entry.Handle, "_GET");
    entry.DestroyCallback = (HART_DESTROY_F)GetProcAddress(entry.Handle, "_DESTROY");
#else // UNIX-Like
    entry.Handle = dlopen(file.c_str(), RTLD_NOW | RTLD_LOCAL);
    if (entry.Handle == NULL) 
    {
        yolo::error("Cannot load module at ", dlerror());
        entry.Handle = NULL; entry.DidLink = false;
        return entry;
    }
    HART_CREDIT_F credit = (HART_CREDIT_F)dlsym(entry.Handle, "_CREDIT");
    entry.InitCallback = (HART_INIT_F)dlsym(entry.Handle, "_GET");
    entry.DestroyCallback = (HART_DESTROY_F)dlsym(entry.Handle, "_DESTROY");
#endif

    if (credit == NULL)
    {
        yolo::error("Cannot load module at {}... No credit...", file.c_str());
        entry.Handle = NULL; entry.DidLink = false;
        return entry;
    }
    if (entry.InitCallback == NULL)
    {
        yolo::error("Cannot load module at {}... No get...", file.c_str());
        entry.Handle = NULL; entry.DidLink = false;
        return entry;
    }
    if (entry.DestroyCallback == NULL)
    {
        yolo::error("Cannot load module at {}... No destroy...", file.c_str());
        entry.Handle = NULL; entry.DidLink = false;
        return entry;
    }

    entry.Credit = (ModuleCredit*)credit();

    yolo::info("Module {} v{}.{}.{} by {}", entry.Credit->ModuleName,
                                            entry.Credit->VersionMajor,
                                            entry.Credit->VersionMinor,
                                            entry.Credit->VersionBuild,
                                            entry.Credit->AuthorName);

    entry.DidLink = true;
    mEntries[entry.Credit->ModuleName] = { entry, nullptr };

    if (mEntries.size() == 1)
    {
        // this is the first, make it active - or else
        mActiveModule = entry.Credit->ModuleName;
        this->load(mActiveModule);
    }

    return entry;
}

std::vector<std::string> HARTModuleDirectory::getModules()
{
    std::vector<std::string> keys;
    for(auto kv : mEntries)
    {
        keys.push_back(kv.first);
    }
    return keys;
}

void HARTModuleDirectory::setActive(std::string moduleName)
{
    if (moduleName == mActiveModule) return;
    this->destroy(mActiveModule);
    mActiveModule = moduleName;
    this->load(mActiveModule);
}

void HARTModuleDirectory::setActiveIndex(int index)
{
    std::vector<std::string> keys = this->getModules();
    this->setActive(keys[index]);
}

HARTModule* HARTModuleDirectory::getActiveModule()
{
    return mEntries[mActiveModule].Module;
}

std::string HARTModuleDirectory::getActive()
{
    return mActiveModule;
}

int HARTModuleDirectory::getActiveIndex()
{
    std::vector<std::string> keys = this->getModules();
    for (int i = 0; i < keys.size(); i++)
    {
        if (keys[i] == mActiveModule) return i;
    }
    return -1; // this should never happen
}

ModuleCredit* HARTModuleDirectory::getActiveCredit()
{
    return mEntries[mActiveModule].Discovery.Credit;
}

void HARTModuleDirectory::load(std::string moduleName)
{
    HARTModule* mod = (HARTModule*)mEntries[moduleName].Discovery.InitCallback();
    mEntries[moduleName].Module = mod;
    mEntries[moduleName].Active = true;
}

void HARTModuleDirectory::destroy(std::string moduleName)
{
    mEntries[moduleName].Discovery.DestroyCallback(mEntries[moduleName].Module);
    mEntries[moduleName].Module = nullptr;
    mEntries[moduleName].Active = false;
}
