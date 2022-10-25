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
    double mLastMouseX, mLastMouseY;

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
