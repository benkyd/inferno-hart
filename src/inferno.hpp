#pragma once

#include "graphics.hpp"
#include "preview_renderer/renderer.hpp"
#include "scene/scene.hpp"

#include <singleton.hpp>

namespace inferno {

class Window;


class Inferno : public helpers::Singleton<Inferno>
{
public:
    Inferno();
    ~Inferno();

    void uiPreset();
    
    void input();
    int run();

public:
    glm::vec2 mouseDelta;
    // 0x00000000
    // 0xFLBRUDxx
    uint8_t movementDelta;

private:
    RasterizeRenderer mRasterRenderer;
    Scene mScene;

private:
    Window* mWin;
};

}
