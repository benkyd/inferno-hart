#pragma once

#include "graphics.hpp"

#include <singleton.hpp>

namespace inferno {

class Window;
class GLFWwindow;

class RasterizeRenderer;
class Scene;

class Inferno : public helpers::Singleton<Inferno>
{
public:
    Inferno();
    ~Inferno();

    void uiPreset();
    int run();

public:
    glm::vec2 mouseDelta;
    glm::vec3 kbdDelta;
private:
    friend void handleKbd(int key, int scan, int action, int mod);
    friend void handlePtr(double x, double y);

private:
    RasterizeRenderer* mRasterRenderer;
    Scene* mScene;

private:
    Window* mWin;
};

void handleKbd(int key, int scan, int action, int mod);
void handlePtr(double x, double y);

}
