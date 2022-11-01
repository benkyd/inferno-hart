#pragma once

#include "graphics.hpp"

#include <singleton.hpp>

namespace inferno {

class Window;

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
    RasterizeRenderer* mRasterRenderer;
    Scene* mScene;

private:
    Window* mWin;
};

}
