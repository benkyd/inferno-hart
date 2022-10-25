#pragma once

#include <string>

#include "graphics.hpp"

#define WINDOW_FLAGS ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoBringToFrontOnFocus | ImGuiWindowFlags_NoNavFocus | ImGuiWindowFlags_MenuBar | ImGuiWindowFlags_NoBackground | ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_NoCollapse

namespace core {

class Window
{
public:
    Window(std::string title, int width, int height);
    ~Window();

    bool newFrame();
    void render();

    void setTitle(std::string title);
    void setSize(int w, int h);
    void setPos(int x, int y);

    void getSize(int& w, int& h);
    void getPos(int& x, int& y);
    GLFWwindow* getGLFWWindow() { return window; }

private:
    void setupGLFW(std::string title);
    void setupImGui();
    void shutdownImGui();
    void shutdownGLFW();

    static void glfwErrorCallback(int error, const char* description);


    int width, height;
    const char* glslVersion;
    GLFWwindow* window;
};

}