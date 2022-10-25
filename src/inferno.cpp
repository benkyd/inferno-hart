#include "inferno.hpp"

#include <version.hpp>
#include "gui/layout.hpp"
#include "window.hpp"

#include <graphics.hpp>
#include <spdlog/spdlog.h>

#include <iostream>
#include <memory>
#include <chrono>

namespace inferno {

Inferno::Inferno(int argc, char** argv) 
{
    // MOTD
    spdlog::info("INFERNO HART v" INFERNO_VERSION);

    // Create window
    mWin = &Window::GetInstance();
    mWin->init("Inferno v" INFERNO_VERSION, 1280, 720);
}

Inferno::~Inferno()
{

}

void Inferno::uiPreset() 
{
    ImGuiID dockspace_id = ImGui::GetID("main");
    ImGui::DockBuilderRemoveNode(dockspace_id); // Clear out existing layout
    ImGui::DockBuilderAddNode(dockspace_id, ImGuiDockNodeFlags_DockSpace); // Add empty node
    ImGui::DockBuilderSetNodeSize(dockspace_id, {1000, 1000});
    
    ImGuiID dock_main_id = dockspace_id; // This variable will track the document node, however we are not using it here as we aren't docking anything into it.
    ImGuiID dock_left = ImGui::DockBuilderSplitNode(dock_main_id, ImGuiDir_Left, 0.5f, NULL, &dock_main_id);
    ImGui::DockBuilderDockWindow("Preview", dock_left);
    ImGui::DockBuilderDockWindow("Render", dock_main_id);
    ImGui::DockBuilderFinish(dockspace_id);

    spdlog::info("LAYOUT SET TO DEFAULT");
}

int Inferno::run() 
{

    while (true) {
        if (!mWin->newFrame()) { break; }
        ImGuiID dockspace_id = ImGui::GetID("main");

        // set the main window to the dockspace and then on the first launch set the preset
        static ImGuiDockNodeFlags dockspace_flags = ImGuiDockNodeFlags_PassthruCentralNode;
        if (ImGui::DockBuilderGetNode(dockspace_id) == NULL) { this->uiPreset(); }
        ImGui::DockSpace(dockspace_id, ImVec2(0.0f, 0.0f), dockspace_flags);

        // Menu Bar
        // if (ImGui::BeginMenuBar())
        // {
        //     if (ImGui::BeginMenu("Menu"))
        //     {
        //         if (ImGui::MenuItem("New")) {}
        //         if (ImGui::MenuItem("Open", "Ctrl+O")) {}
        //         if (ImGui::BeginMenu("Open Recent"))
        //         {
        //             ImGui::EndMenu();
        //         }
        //         if (ImGui::MenuItem("Save", "Ctrl+S")) {}
        //         if (ImGui::MenuItem("Save As..")) {}

        //         ImGui::Separator();
        //         if (ImGui::BeginMenu("Options"))
        //         {
        //             ImGui::EndMenu();
        //         }
        //     }
        //     ImGui::EndMenuBar();
        // }

        ImGui::Begin("Preview");
        ImGui::End();

        ImGui::Begin("Render");
        ImGui::End();

        ImGui::Begin("Inferno HART");
        ImGui::End();

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
            spdlog::error("[GL]: ", error);
        }    
        
        mWin->render();
    }

    delete mWin;
    return 0;
}

}
