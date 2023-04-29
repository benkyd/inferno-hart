#pragma once

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include <memory>
#include <mutex>

namespace inferno::graphics {

struct _CameraImpl;

typedef struct Camera {
    glm::mat4 ViewMatrix;
    glm::mat4 ProjectionMatrix;
    glm::mat4 LookMatrix;
    float MouseSensitivity = 0.4f;
    float CameraSpeed = 0.1f;
    float Roll, Pitch, Yaw;
    float FOV = 45.0f;
    glm::vec3 Position = {};
    glm::vec3 LookDirection = {};

    std::unique_ptr<_CameraImpl> _impl;
} Camera;

std::unique_ptr<Camera> camera_create();
void camera_cleanup(std::unique_ptr<Camera>& camera);

void camera_update(std::unique_ptr<Camera>& camera);
void camera_did_update(std::unique_ptr<Camera>& camera);
void camera_new_frame(std::unique_ptr<Camera>& camera);

glm::mat4 camera_get_view(std::unique_ptr<Camera>& camera);
glm::mat4 camera_get_projection(std::unique_ptr<Camera>& camera);
glm::mat4 camera_get_look(std::unique_ptr<Camera>& camera);

void raster_set_viewport(std::unique_ptr<Camera>& camera, glm::ivec2 viewport);
glm::ivec2 raster_get_viewport(std::unique_ptr<Camera>& camera);

void ray_set_viewport(std::unique_ptr<Camera>& camera, glm::ivec2 viewport);
glm::ivec2 ray_get_viewport(std::unique_ptr<Camera>& camera);

void camera_move(std::unique_ptr<Camera>& camera, uint8_t movement_delta);
void camera_mouse_move(std::unique_ptr<Camera>& camera, glm::vec2 mouse_delta);

void camera_set_position(std::unique_ptr<Camera>& camera, glm::vec3 position);
void camera_set_euler_look(std::unique_ptr<Camera>& camera, float roll,
    float pitch, float yaw);
void camera_set_look(std::unique_ptr<Camera>& camera,
    glm::vec3 look_direction);

} // namespace inferno::graphics
