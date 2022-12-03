#pragma once

namespace inferno {

// Thread dispatcher keeps track of the thread that calls the
// copy function because OpenGL is a nightmare

#include <graphics.hpp>

class HHM;
class Scene;
class RayRenderer;

class RenderDispatcher 
{
public:
    RenderDispatcher();
    ~RenderDispatcher();

    RayRenderer* getRenderer();
    HHM* getTopModule();

    void startProgression();
    void stopProgression();
    GLuint getLatestTexture();
    
private:
    HHM* mHHM;
    RayRenderer* mRenderer;
};

}
