#pragma once


namespace inferno {

class Window;
class GLFWwindow;


class RasterizeRenderer;
class Scene;


class Inferno
{
public:
    Inferno(int argc, char** argv);
    ~Inferno();

    void uiPreset();
    int run();

private:
    RasterizeRenderer* mRasterRenderer;
    
    Scene* mScene;

private:
    Window* mWin;
};

}
