#pragma once

#include <graphics.hpp>

namespace inferno {

class Scene;
class Colour;

class RayRenderer 
{
public:
    RayRenderer();
    ~RayRenderer();

    void setScene(Scene* scene);

    void setTargetSize(glm::ivec2 size);
    glm::ivec2 getTargetSize();
    GLuint getRenderedTexture();

    void prepare();
    void draw();

private:
    GLuint mRenderTargetTexture = 0;
    
    Colour* mTarget;
    glm::ivec2 mRenderTargetSize = {1920, 1080};

    Scene* mCurrentScene;
};

}
