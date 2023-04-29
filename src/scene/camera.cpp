#include <scene/camera.hpp>

namespace inferno::graphics {

typedef struct _CameraImpl {
    glm::ivec2 Viewport = { 100, 100 };
    glm::ivec2 RayViewport = { 200, 200 };
    bool DidUpdate;
    std::mutex CamMutex;
} _CameraImpl;

std::unique_ptr<Camera> camera_create()
{
    std::unique_ptr<Camera> camera = std::make_unique<Camera>();
    camera->_impl = std::make_unique<_CameraImpl>();

    camera->ProjectionMatrix = glm::perspective(
        glm::radians(camera->FOV),
        1.0f,
        0.1f,
        1000.0f);
    camera->ViewMatrix = {};

    camera->Roll = 0.0f;
    camera->Pitch = 0.0f;
    camera->Yaw = 0.0f;

    camera->Position = {};
    camera->LookDirection = {};

    camera_update(camera);
    return camera;
}

void camera_cleanup(std::unique_ptr<Camera>& camera)
{
    camera->_impl.reset();
    camera.reset();
}

void camera_update(std::unique_ptr<Camera>& camera)
{
    glm::mat4 matRoll = glm::mat4(1.0f);
    glm::mat4 matPitch = glm::mat4(1.0f);
    glm::mat4 matYaw = glm::mat4(1.0f);

    matRoll = glm::rotate(matRoll, Roll, glm::vec3(0.0f, 0.0f, 1.0f));
    matPitch = glm::rotate(matPitch, Pitch, glm::vec3(1.0f, 0.0f, 0.0f));
    matYaw = glm::rotate(matYaw, Yaw, glm::vec3(0.0f, 1.0f, 0.0f));

    glm::mat4 rotate = matRoll * matPitch * matYaw;
    mCameraLook = rotate;

    glm::mat4 translate = glm::mat4(1.0f);
    translate = glm::translate(translate, -Position);

    std::lock_guard<std::mutex> lock(this->_mCam);

    mViewMatrix = rotate * translate;
    mProjMatrix = glm::perspective(glm::radians(FOV), mViewport.x / mViewport.y, 0.1f, 1000.0f);

    // Work out Look Vector
    glm::mat4 inverseView = glm::inverse(mViewMatrix);

    LookDirection.x = inverseView[2][0];
    LookDirection.y = inverseView[2][1];
    LookDirection.z = inverseView[2][2];

    mDidUpdate = true;
}

bool Camera::didUpdate()
{
    std::lock_guard<std::mutex> lock(this->_mCam);
    return mDidUpdate;
}

void Camera::newFrame()
{
    std::lock_guard<std::mutex> lock(this->_mCam);
    mDidUpdate = false;
}

glm::mat4 Camera::getViewMatrix()
{
    std::lock_guard<std::mutex> lock(this->_mCam);
    return mViewMatrix;
}

glm::mat4 Camera::getProjectionMatrix()
{
    std::lock_guard<std::mutex> lock(this->_mCam);
    return mProjMatrix;
}

glm::mat4 Camera::getCameraLook()
{
    std::lock_guard<std::mutex> lock(this->_mCam);
    return mCameraLook;
}

void Camera::setRasterViewport(glm::vec2 viewport)
{
    std::lock_guard<std::mutex> lock(this->_mCam);
    mViewport = viewport;
    mProjMatrix = glm::perspective(glm::radians(FOV), (float)viewport.x / (float)viewport.y, 0.1f, 1000.0f);
}

void Camera::moveCamera(uint8_t posDelta)
{
    if (posDelta == 0)
        return;

    // Rotate by camera direction
    glm::vec3 delta(0.0f);

    glm::mat2 rotate {
        cos(Yaw), -sin(Yaw),
        sin(Yaw), cos(Yaw)
    };

    glm::vec2 f(0.0, 1.0);
    f = f * rotate;

    if (posDelta & 0x80) {
        delta.z -= f.y;
        delta.x -= f.x;
    }
    if (posDelta & 0x20) {
        delta.z += f.y;
        delta.x += f.x;
    }
    if (posDelta & 0x40) {
        delta.z += f.x;
        delta.x += -f.y;
    }
    if (posDelta & 0x10) {
        delta.z -= f.x;
        delta.x -= -f.y;
    }
    if (posDelta & 0x8) {
        delta.y += 1;
    }
    if (posDelta & 0x4) {
        delta.y -= 1;
    }

    // get current view matrix
    glm::mat4 mat = getViewMatrix();
    glm::vec3 forward(mat[0][2], mat[1][2], mat[2][2]);
    glm::vec3 strafe(mat[0][0], mat[1][0], mat[2][0]);

    // forward vector must be negative to look forward.
    // read :http://in2gpu.com/2015/05/17/view-matrix/
    Position += delta * CameraSpeed;

    // update the view matrix
    update();
}

void Camera::mouseMoved(glm::vec2 mouseDelta)
{
    if (glm::length(mouseDelta) == 0)
        return;
    // note that yaw and pitch must be converted to radians.
    // this is done in update() by glm::rotate
    Yaw += MouseSensitivity * (mouseDelta.x / 100);
    Pitch += MouseSensitivity * (mouseDelta.y / 100);
    Pitch = glm::clamp<float>(Pitch, -M_PI / 2, M_PI / 2);

    update();
}

void Camera::setPosition(glm::vec3 position)
{
    Position = position;

    update();
}

void Camera::setEulerLook(float roll, float pitch, float yaw)
{
    Roll = roll;
    Pitch = pitch;
    Yaw = yaw;
    LookDirection.x = cos(Yaw) * cos(Pitch);
    LookDirection.y = sin(Yaw) * cos(Pitch);
    LookDirection.z = sin(Pitch);

    update();
}

void Camera::setLook(glm::vec3 lookDirection)
{
    LookDirection = lookDirection;
    Pitch = asin(-lookDirection.y);
    Yaw = atan2(lookDirection.x, lookDirection.z);

    update();
}

void Camera::setRayViewport(glm::vec2 viewport)
{
    std::lock_guard<std::mutex> lock(this->_mCam);
    mRayViewport = viewport;
}

glm::vec2 Camera::getRayViewport()
{
    std::lock_guard<std::mutex> lock(this->_mCam);
    return mRayViewport;
}

}
