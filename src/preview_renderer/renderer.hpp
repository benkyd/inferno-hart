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
    void setTarget(GLuint renderTarget);

    void prepare();
    void draw();

private:
    Scene* mCurrentScene;
};

}
