#pragma once

#include <string>

#include "singleton.hpp"
#include "graphics.hpp"

#define WINDOW_FLAGS ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoBringToFrontOnFocus | ImGuiWindowFlags_NoNavFocus | ImGuiWindowFlags_MenuBar | ImGuiWindowFlags_NoBackground | ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_NoCollapse

namespace inferno::core {

typedef void (*KeyCallback)(int key, int scan, int action, int mod);
typedef void (*MouseCallback)(double x, double y);

enum WINDOW_MODE
{
    WIN_MODE_DEFAULT,
    WIN_MODE_FPS,
};

class Window : public helpers::Singleton<Window>
{
public:
    Window();
    ~Window();

    void init(std::string title, int width, int height);

    bool newFrame();
    void render();

    void setTitle(std::string title);
    void setSize(int w, int h);
    void setPos(int x, int y);

    glm::vec2 getSize();
    void getPos(int& x, int& y);
    GLFWwindow* getGLFWWindow() { return window; }

    void setFPSMode();

    void setKeyCallback(KeyCallback callback);
    KeyCallback getKeyCallback();

private:
    WINDOW_MODE mWinMode = WIN_MODE_DEFAULT;

private:
    void setupGLFW(std::string title);
    void setupImGui();
    void shutdownImGui();
    void shutdownGLFW();

    static void glfwKeyCallback(GLFWwindow* window, int key, int scancode, int action, int mods);
    KeyCallback mKeyCallback = nullptr;

private:
    static void glfwErrorCallback(int error, const char* description);

    int width, height;
    const char* glslVersion;
    GLFWwindow* window;
};

}
