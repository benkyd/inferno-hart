#pragma once

#include <thirdparty/glm/glm.hpp>

namespace inferno {

class Camera
{
public:
    Camera();
    ~Camera();


private:
    // View
    glm::vec3 Eye;
    glm::vec3 Target;
    glm::vec3 Up;
 
    // Projection
    float FovY;
    float Aspect;
    float ZNear;

};

}


