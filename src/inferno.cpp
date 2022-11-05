#include "inferno.hpp"

#include <version.hpp>
#include "gui/layout.hpp"
#include "window.hpp"

#include "preview_renderer/renderer.hpp"
#include "preview_renderer/shader.hpp"
#include "scene/camera.hpp"
#include "scene/scene.hpp"
#include "scene/material.hpp"
#include "scene/mesh.hpp"

#include <spdlog/spdlog.h>

#include <iostream>
#include <memory>
#include <chrono>
#include <numeric>

using namespace inferno;

Inferno::Inferno()
{
    // MOTD
    spdlog::set_level(spdlog::level::trace);
    spdlog::info("INFERNO HART v" INFERNO_VERSION);

    // Create window
    mWin = &Window::GetInstance();
    mWin->init("Inferno v" INFERNO_VERSION, 1280, 720);

    mRasterRenderer = new RasterizeRenderer();
    mScene = new Scene();
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

void Inferno::input()
{
    // KBD & MOUSE
    static glm::dvec2 dMouseDelta;
    glm::dvec2 tempMousePos;
    glfwGetCursorPos(mWin->getGLFWWindow(), &tempMousePos.x, &tempMousePos.y);

    mouseDelta = dMouseDelta - tempMousePos;
    dMouseDelta = tempMousePos;

    movementDelta = 0b00000000;
    if (glfwGetKey(mWin->getGLFWWindow(), GLFW_KEY_W) == GLFW_PRESS)
        movementDelta |= 0b10000000;
    if (glfwGetKey(mWin->getGLFWWindow(), GLFW_KEY_A) == GLFW_PRESS)
        movementDelta |= 0b01000000;
    if (glfwGetKey(mWin->getGLFWWindow(), GLFW_KEY_S) == GLFW_PRESS)
        movementDelta |= 0b00100000;
    if (glfwGetKey(mWin->getGLFWWindow(), GLFW_KEY_D) == GLFW_PRESS)
        movementDelta |= 0b00010000;
    if (glfwGetKey(mWin->getGLFWWindow(), GLFW_KEY_SPACE) == GLFW_PRESS)
        movementDelta |= 0b00001000;
    if (glfwGetKey(mWin->getGLFWWindow(), GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS) 
        movementDelta |= 0b00000100;
}

int Inferno::run() 
{
    // mWin->setFPSMode();

    Camera camera;
    Mesh cornell;
    cornell.loadOBJ("res/cornell.obj");
    cornell.ready();

    Material basicMaterial("basic");
    Shader basicShader;
    basicShader.load("res/shaders/basic.glsl")->link();
    basicMaterial.setGlShader(&basicShader);
    cornell.setMaterial(&basicMaterial);

    mScene->addMesh(&cornell);
    mScene->setCamera(&camera);

    mRasterRenderer->setScene(mScene);

    while (true) 
    {
        if (!mWin->newFrame()) { break; }
        
        // UI
        ImGuiID dockspace_id = ImGui::GetID("main");

        // set the main window to the dockspace and then on the first launch set the preset
        static ImGuiDockNodeFlags dockspace_flags = ImGuiDockNodeFlags_PassthruCentralNode;
        if (ImGui::DockBuilderGetNode(dockspace_id) == NULL) { this->uiPreset(); }
        ImGui::DockSpace(dockspace_id, ImVec2(0.0f, 0.0f), dockspace_flags);

        this->input();
        camera.MouseMoved(mouseDelta);
        camera.MoveCamera(movementDelta);

        mRasterRenderer->setTargetSize({mWin->getSize()});
        mRasterRenderer->prepare();
        mRasterRenderer->draw();


        ImGui::Begin("Preview");
        ImGui::Image((ImTextureID)mRasterRenderer->getRenderedTexture(),
             { mRasterRenderer->getTargetSize().x, mRasterRenderer->getTargetSize().y },
             ImVec2(0,1), ImVec2(1,0));
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
                default:                               error = std::to_string((uint32_t)err); break;
            }
            spdlog::error("[GL]: ", err, " ", error);
        }    
        
        mWin->render();
    }

    delete mWin;
    return 0;
}