#include "inferno.hpp"

#include "version.hpp"
#include "imgui/imgui.h"
#include "spdlog/spdlog.h"

#include <iostream>
#include <memory>
#include <chrono>

#ifdef _WIN32
// TODO: DLLs
#else
#include <dlfcn.h>
#endif
#include <inferno_hart.hpp>

using namespace core;

Inferno::Inferno(int argc, char** argv) {
    // MOTD
    spdlog::info("- INFERNO HART v" INFERNO_VERSION);

    // Create window
    win = new Window("Inferno v" INFERNO_VERSION, 1280, 720);
}

int Inferno::run() {

    /* on Linux, use "./myclass.so" */
    void* handle = dlopen("hart/inferno-hart-cpu/hart_cpu_accel.so", RTLD_LAZY);

    HART::Accelerator* (*get)();
    void (*destroy)(HART::Accelerator*);

    get = (HART::Accelerator* (*)())dlsym(handle, "get");
    destroy = (void (*)(HART::Accelerator*))dlsym(handle, "destroy");

    HART::Accelerator* myClass = (HART::Accelerator*)get();
    myClass->Init();
    destroy( myClass );

    while (true) {
        if (win->newFrame()) { break; }

        static bool demo = false;
        ImGui::Checkbox("ImGui Demo", &demo);
        if (demo) {
            ImGui::ShowDemoWindow();
        }

        // check err
        GLenum err;
        while((err = glGetError()) != GL_NO_ERROR) {
            std::string error;
            switch (err) {
                case GL_INVALID_ENUM:                  error = "INVALID_ENUM"; break;
                case GL_INVALID_VALUE:                 error = "INVALID_VALUE"; break;
                case GL_INVALID_OPERATION:             error = "INVALID_OPERATION"; break;
                case GL_STACK_OVERFLOW:                error = "STACK_OVERFLOW"; break;
                case GL_STACK_UNDERFLOW:               error = "STACK_UNDERFLOW"; break;
                case GL_OUT_OF_MEMORY:                 error = "OUT_OF_MEMORY"; break;
                case GL_INVALID_FRAMEBUFFER_OPERATION: error = "INVALID_FRAMEBUFFER_OPERATION"; break;
            }
            std::cout << "[GL]: " << error << std::endl;
        }    
        
        win->render();
    }

    delete win;
    return 0;
}
