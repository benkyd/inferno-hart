#pragma once

#include "graphics.hpp"

#include <singleton.hpp>

namespace inferno {

class Window;
class HHM;

class RasterizeRenderer;
class RenderDispatcher;
class Scene;

class Inferno : public helpers::Singleton<Inferno>
{
public:
    Inferno();
    ~Inferno();

    void uiPreset();
    
    void moveInput();
    void stopMoveInput();
    int run();

public:
    glm::vec2 mouseDelta;
    // 0b00000000
    // 0bFLBRUDxx
    uint8_t movementDelta;

private:
    // need deffered init as they need an OpenGL context
    // could and should be fixed with a static window
    RasterizeRenderer* mRasterRenderer;
    RenderDispatcher* mRayRenderer;
    Scene* mScene;
    
private:
    Window* mWin;
};

}
