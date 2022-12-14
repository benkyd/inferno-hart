#pragma once

#include <graphics.hpp>

namespace inferno {

class Scene;

class RasterizeRenderer 
{
public:
    RasterizeRenderer();
    ~RasterizeRenderer();

    void setScene(Scene* scene);

    void setTargetSize(glm::ivec2 size);
    glm::ivec2 getTargetSize();
    GLuint getRenderedTexture();

    void prepare();
    void draw();

private:
    GLuint mRenderTarget = 0;
    GLuint mRenderTargetTexture = 0;
    GLuint mRenderTargetDepthTexture = 0;
    glm::ivec2 mRenderTargetSize = {1920, 1080};

    Scene* mCurrentScene;
};

}
