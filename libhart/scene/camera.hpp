#pragma once

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include <memory>
#include <mutex>

namespace inferno::graphics {

typedef struct _CameraImpl {
    bool DidUpdate;
    std::mutex CamMutex;
} _CameraImpl;

typedef struct Viewport {
    glm::ivec2 Raster;
    glm::ivec2 Ray;
} Viewports;

typedef struct Camera {
    glm::mat4 ViewMatrix;
    glm::mat4 ProjectionMatrix;
    glm::mat4 LookMatrix;

    Viewport Views;

    float MouseSensitivity = 0.4f;
    float Speed = 0.1f;
    float Roll, Pitch, Yaw;
    float FOV = 45.0f;

    glm::vec3 Position = {};
    glm::vec3 LookDirection = {};

    _CameraImpl* _impl;
} Camera;

Camera* camera_create();
void camera_cleanup(Camera* camera);

void camera_update(Camera* camera);
bool camera_did_update(Camera* camera);
void camera_new_frame(Camera* camera);

glm::mat4 camera_get_view(Camera* camera);
glm::mat4 camera_get_projection(Camera* camera);
glm::mat4 camera_get_look(Camera* camera);

void raster_set_viewport(Camera* camera, glm::ivec2 viewport);
glm::ivec2 raster_get_viewport(Camera* camera);

void ray_set_viewport(Camera* camera, glm::ivec2 viewport);
glm::ivec2 ray_get_viewport(Camera* camera);

void camera_move(Camera* camera, uint8_t movement_delta);
void camera_mouse_move(Camera* camera, glm::vec2 mouse_delta);

void camera_set_position(Camera* camera, glm::vec3 position);
void camera_set_euler_look(Camera* camera, float roll,
    float pitch, float yaw);
void camera_set_look(Camera* camera,
    glm::vec3 look_direction);

glm::vec3 camera_get_position(Camera* camera);

} // namespace inferno::graphics
