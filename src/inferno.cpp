#include "inferno.hpp"

#include <version.hpp>
#include "gui/layout.hpp"
#include "window.hpp"

#include "hart_module.hpp"
#include "hart_directory.hpp"

#include "preview_renderer/renderer.hpp"
#include "preview_renderer/shader.hpp"
#include "renderer/dispatcher.hpp"
#include "renderer/renderer.hpp"
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
    mRayRenderer = new RenderDispatcher();
    mScene = new Scene();
}

Inferno::~Inferno()
{

}

static void HelpMarker(const char* desc)
{
    ImGui::TextDisabled("(?)");
    if (ImGui::IsItemHovered(ImGuiHoveredFlags_DelayShort))
    {
        ImGui::BeginTooltip();
        ImGui::PushTextWrapPos(ImGui::GetFontSize() * 35.0f);
        ImGui::TextUnformatted(desc);
        ImGui::PopTextWrapPos();
        ImGui::EndTooltip();
    }
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

void Inferno::moveInput()
{
    static GLFWcursor* cursor = glfwCreateStandardCursor(GLFW_HAND_CURSOR);
    glfwSetCursor(mWin->getGLFWWindow(), cursor);

    // KBD & MOUSE
    // pan only get on hold
    static glm::dvec2 lastMousePos;
    static int firstClick = 0;
    if (glfwGetMouseButton(mWin->getGLFWWindow(), GLFW_MOUSE_BUTTON_1) == GLFW_PRESS)
    {
        firstClick++;
        if (firstClick == 1)
        {
            glfwGetCursorPos(mWin->getGLFWWindow(), &lastMousePos.x, &lastMousePos.y);
        }
        glm::dvec2 tempMousePos = { 0.0f, 0.0f };
        glfwGetCursorPos(mWin->getGLFWWindow(), &tempMousePos.x, &tempMousePos.y);
        mouseDelta = lastMousePos - tempMousePos;
        lastMousePos = tempMousePos;
    } else
    {
        firstClick = 0;
        mouseDelta = { 0.0f, 0.0f };
        lastMousePos = { 0.0f, 0.0f };
    }

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

void Inferno::stopMoveInput()
{
    movementDelta = 0x0;
    mouseDelta = { 0.0f, 0.0f };
}

int Inferno::run()
{
    Material basicMaterial("basic");
    Shader basicShader;
    basicShader.load("res/shaders/basic.glsl")->link();
    basicMaterial.setGlShader(&basicShader);

    Mesh cornell;
    cornell.loadOBJ("res/cornell-box.obj");
    // cornell.loadOBJ("res/sponza.obj");
    cornell.ready();
    cornell.setMaterial(&basicMaterial);
    mScene->addMesh(&cornell);

    Mesh dragon;
    dragon.loadOBJ("res/dragon-cornell-size.obj");
    dragon.ready();
    dragon.setMaterial(&basicMaterial);
    mScene->addMesh(&dragon);

    Camera camera;
    mScene->setCamera(&camera);

    mRasterRenderer->setScene(mScene);
    mRayRenderer->getRenderer()->setScene(mScene);

    while (true) 
    {
        if (!mWin->newFrame()) { break; }
        camera.newFrame();

        // set the main window to the dockspace and then on the first launch set the preset
        ImGuiID dockspace_id = ImGui::GetID("main");
        static ImGuiDockNodeFlags dockspace_flags = ImGuiDockNodeFlags_PassthruCentralNode;
        if (ImGui::DockBuilderGetNode(dockspace_id) == NULL) { this->uiPreset(); }
        ImGui::DockSpace(dockspace_id, ImVec2(0.0f, 0.0f), dockspace_flags);

        // Menu Bar
        static bool showPreview = true;
        static bool showRenderSettings = true;
        static bool showDemoWindow = false;
        if (ImGui::BeginMenuBar())
        {
            if (ImGui::BeginMenu("Menu"))
            {
                ImGui::EndMenu();
            }
            if (ImGui::BeginMenu("View"))
            {
                ImGui::Checkbox("Show Preview", &showPreview);
                ImGui::SameLine(); HelpMarker("Show the preview window");
                ImGui::Checkbox("Show Settings", &showRenderSettings);
                ImGui::SameLine(); HelpMarker("Show the Inferno HART settings window");
                ImGui::Checkbox("Show Demo", &showDemoWindow);

                ImGui::EndMenu();
            }
            ImGui::EndMenuBar();
        }

        if (showPreview && ImGui::Begin("Preview", nullptr, ImGuiWindowFlags_NoScrollbar))
        {
            if (ImGui::IsWindowHovered())
            {
                this->moveInput();
            } else
            {
                this->stopMoveInput();
            }
            if (glm::length(mouseDelta) > 0.0f) camera.mouseMoved(mouseDelta);
            if (movementDelta != 0b00000000) camera.moveCamera(movementDelta);

            camera.setRasterViewport({ImGui::GetWindowSize().x, ImGui::GetWindowSize().y});
            mRasterRenderer->setTargetSize({ImGui::GetWindowSize().x, ImGui::GetWindowSize().y});
            mRasterRenderer->prepare();
            mRasterRenderer->draw();
            ImGui::Image((ImTextureID)mRasterRenderer->getRenderedTexture(),
                { mRasterRenderer->getTargetSize().x, mRasterRenderer->getTargetSize().y },
                ImVec2(0,1), ImVec2(1,0));
            glBindFramebuffer(GL_FRAMEBUFFER, 0);
            ImGui::End();
        }

        if (ImGui::Begin("Render", nullptr, ImGuiWindowFlags_NoScrollbar))
        {
            ImGui::Image((ImTextureID)mRayRenderer->getLatestTexture(),
                { mRayRenderer->getRenderer()->getTargetSize().x,
                  mRayRenderer->getRenderer()->getTargetSize().y },
                ImVec2(0,1), ImVec2(1,0));
            glBindTexture(GL_TEXTURE_2D, 0);
            ImGui::End();
        }

        if (showRenderSettings && ImGui::Begin("Inferno HART"))
        {
            // start/stop
            bool isRenderRunning = mRayRenderer->progressionStatus();
            if (isRenderRunning)
            {
                ImGui::PushItemFlag(ImGuiItemFlags_Disabled, true); ImGui::PushStyleVar(ImGuiStyleVar_Alpha, ImGui::GetStyle().Alpha * 0.5f);
                ImGui::Button("Start Render"); ImGui::SameLine();
                ImGui::PopItemFlag(); ImGui::PopStyleVar();
                if (ImGui::Button("Stop Render"))
                    mRayRenderer->stopProgression();
            } else {
                if (ImGui::Button("Start Render"))
                    mRayRenderer->startProgression();
                ImGui::SameLine();
                ImGui::PushItemFlag(ImGuiItemFlags_Disabled, true); ImGui::PushStyleVar(ImGuiStyleVar_Alpha, ImGui::GetStyle().Alpha * 0.5f);
                ImGui::Button("Stop Render");
                ImGui::PopItemFlag(); ImGui::PopStyleVar();
            }

            if (ImGui::TreeNode("Render"))
            {
                // modules
                HHM* hhm = mRayRenderer->getTopModule();
                if (ImGui::TreeNode("Accelerator"))
                {
                    ImGui::Button("Find Accelerator...");
                    ImGui::Text("Select Accelerator:");
                    if (ImGui::BeginListBox("", ImVec2(-FLT_MIN, 3 * ImGui::GetTextLineHeightWithSpacing())))
                    {
                        std::vector<std::string> moduleNames = hhm->getModuleDirectory()->getModules();
                        int active = hhm->getModuleDirectory()->getActiveIndex();
                        for (int n = 0; n < moduleNames.size(); n++)
                        {
                            const bool isSelected = (active == n);
                            if (ImGui::Selectable(moduleNames[n].c_str(), isSelected))
                                 hhm->getModuleDirectory()->setActiveIndex(n);
                            if (isSelected)
                                ImGui::SetItemDefaultFocus();
                        }
                        ImGui::EndListBox();
                    }
                    auto* activeCredit = hhm->getModuleDirectory()->getActiveCredit();
                    ImGui::Text(activeCredit->ModuleName.c_str());
                    ImGui::SameLine();
                    ImGui::Text("v%i.%i.%i", activeCredit->VersionMajor,
                                             activeCredit->VersionMinor,
                                             activeCredit->VersionBuild);
                    ImGui::BulletText(activeCredit->ModuleDesc.c_str());
                    ImGui::BulletText("Authored by %s", activeCredit->AuthorName.c_str());

                    ImGui::TreePop();
                }
                ImGui::TreePop();
            }

            if (ImGui::TreeNode("Camera"))
            {
                ImGui::PushItemWidth(100);
                ImGui::Text("Camera Position X,Y,Z");

                bool positionUpdated = false;
                ImGui::DragFloat("X", &camera.Position.x, 0.01f, -FLT_MAX, FLT_MAX, "%.2f", ImGuiSliderFlags_None); ImGui::SameLine(); if (ImGui::IsItemEdited()) positionUpdated = true;
                ImGui::DragFloat("Y", &camera.Position.y, 0.01f, -FLT_MAX, FLT_MAX, "%.2f", ImGuiSliderFlags_None); ImGui::SameLine(); if (ImGui::IsItemEdited()) positionUpdated = true;
                ImGui::DragFloat("Z", &camera.Position.z, 0.01f, -FLT_MAX, FLT_MAX, "%.2f", ImGuiSliderFlags_None); if (ImGui::IsItemEdited()) positionUpdated = true;
                if (positionUpdated) camera.setPosition(camera.Position);

                bool viewUpdated = false;
                ImGui::Text("Camera Look Yaw, Pitch, Roll");
                ImGui::DragFloat("Yaw", &camera.Yaw, 0.01f, -FLT_MAX, FLT_MAX, "%.2f", ImGuiSliderFlags_None); ImGui::SameLine(); if (ImGui::IsItemEdited()) viewUpdated = true;
                ImGui::DragFloat("Pitch", &camera.Pitch, 0.01f, -FLT_MAX, FLT_MAX, "%.2f", ImGuiSliderFlags_None); ImGui::SameLine(); if (ImGui::IsItemEdited()) viewUpdated = true;
                ImGui::DragFloat("Roll", &camera.Roll, 0.01f, -FLT_MAX, FLT_MAX, "%.2f", ImGuiSliderFlags_None); if (ImGui::IsItemEdited()) viewUpdated = true;

                ImGui::PopItemWidth();
                ImGui::PushItemWidth(300);

                ImGui::Text("Camera Zoom");
                ImGui::DragFloat("Zoom", &camera.FOV, -0.1f, 0.01f, 180.0f, "%.2f", ImGuiSliderFlags_None); ImGui::SameLine(); if (ImGui::IsItemEdited()) viewUpdated = true;
                if (viewUpdated) camera.update();

                ImGui::PopItemWidth();
                ImGui::TreePop();
            }
            ImGui::End();
        }

        if (showDemoWindow)
        {
            ImGui::ShowDemoWindow();
        }

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
            spdlog::error("[GL]: {0} {1}", err, error);
        }    
        
        mWin->render();
    }

    return 0;
}