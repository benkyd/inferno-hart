#include "inferno.hpp"

#include "version.hpp"
#include "imgui/imgui.h"
#include "spdlog/spdlog.h"

#include <iostream>
#include <memory>
#include <chrono>

using namespace inferno;

InfernoHART::InfernoHART(int argc, char** argv) {
    // MOTD
    spdlog::info("- INFERNO HART v" INFERNO_VERSION);

    // Create window
    win = new Window("Inferno v" INFERNO_VERSION, 1280, 720);
}

int InfernoHART::run() {

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
