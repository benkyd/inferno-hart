#pragma once

#include <string>

#include "hardware_accelerator.hpp"

namespace inferno {

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

struct ModuleCredit
{
    const std::string ModuleName;
    const std::string AuthorName;
    const std::string ModuleDesc;
    const int VersionMajor;
    const int VersionMinor;
    const int VersionBuild;
};

class HARTModule
{
public:
    // Constructor & destructor is done in the module
    // virtual void takeScene() = 0;
};
    
}
