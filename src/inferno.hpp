#pragma once

#include "graphics.hpp"

#include <singleton.hpp>

namespace inferno {

class Window;
class HHM;

class RasterizeRenderer;
class RayRenderer;
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
    // 0x00000000
    // 0xFLBRUDxx
    uint8_t movementDelta;

private:
    // need deffered init as they need an OpenGL context
    // could and should be fixed with a static window
    RasterizeRenderer* mRasterRenderer;
    RayRenderer* mRayRenderer;
    Scene* mScene;

    HHM* mHeadHartModule;
    
private:
    Window* mWin;
};

}
