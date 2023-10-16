#include <scene/camera.hpp>

#include <graphics.hpp>

namespace inferno::graphics {

Camera* camera_create()
{
    Camera* camera = new Camera;
    camera->_impl = new _CameraImpl;

    camera->Views = Viewports();
    camera->Views.Raster = glm::ivec2(800, 600);
    camera->Views.Ray = glm::ivec2(800, 600);

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

void camera_cleanup(Camera* camera)
{
    delete camera->_impl;
    delete camera;
}

void camera_draw_ui(Camera* camera)
{
    ImGui::PushItemWidth(100);
    ImGui::Text("Camera Position X,Y,Z");

    bool positionUpdated = false;
    ImGui::DragFloat("X", &camera->Position.x, 0.01f, -FLT_MAX, FLT_MAX, "%.2f", ImGuiSliderFlags_None);
    ImGui::SameLine();
    if (ImGui::IsItemEdited())
        positionUpdated = true;
    ImGui::DragFloat("Y", &camera->Position.y, 0.01f, -FLT_MAX, FLT_MAX, "%.2f", ImGuiSliderFlags_None);
    ImGui::SameLine();
    if (ImGui::IsItemEdited())
        positionUpdated = true;
    ImGui::DragFloat("Z", &camera->Position.z, 0.01f, -FLT_MAX, FLT_MAX, "%.2f", ImGuiSliderFlags_None);
    if (ImGui::IsItemEdited())
        positionUpdated = true;
    if (positionUpdated)
        graphics::camera_set_position(camera, graphics::camera_get_position(camera));

    bool viewUpdated = false;
    ImGui::Text("Camera Look Yaw, Pitch, Roll");
    ImGui::DragFloat("Yaw", &camera->Yaw, 0.01f, -FLT_MAX, FLT_MAX, "%.2f", ImGuiSliderFlags_None);
    ImGui::SameLine();
    if (ImGui::IsItemEdited())
        viewUpdated = true;
    ImGui::DragFloat("Pitch", &camera->Pitch, 0.01f, -FLT_MAX, FLT_MAX, "%.2f", ImGuiSliderFlags_None);
    ImGui::SameLine();
    if (ImGui::IsItemEdited())
        viewUpdated = true;
    ImGui::DragFloat("Roll", &camera->Roll, 0.01f, -FLT_MAX, FLT_MAX, "%.2f", ImGuiSliderFlags_None);
    if (ImGui::IsItemEdited())
        viewUpdated = true;

    ImGui::PopItemWidth();
    ImGui::PushItemWidth(300);

    ImGui::Text("Camera Zoom");
    ImGui::DragFloat("Zoom", &camera->FOV, -0.1f, 0.01f, 180.0f, "%.2f", ImGuiSliderFlags_None);
    ImGui::SameLine();
    if (ImGui::IsItemEdited())
        viewUpdated = true;
    if (viewUpdated)
        graphics::camera_update(camera);

    ImGui::PopItemWidth();
}

void camera_update(Camera* camera)
{
    glm::mat4 matRoll = glm::mat4(1.0f);
    glm::mat4 matPitch = glm::mat4(1.0f);
    glm::mat4 matYaw = glm::mat4(1.0f);

    matRoll = glm::rotate(matRoll, camera->Roll, glm::vec3(0.0f, 0.0f, 1.0f));
    matPitch = glm::rotate(matPitch, camera->Pitch, glm::vec3(1.0f, 0.0f, 0.0f));
    matYaw = glm::rotate(matYaw, camera->Yaw, glm::vec3(0.0f, 1.0f, 0.0f));

    glm::mat4 rotate = matRoll * matPitch * matYaw;
    camera->LookMatrix = rotate;

    glm::mat4 translate = glm::mat4(1.0f);
    translate = glm::translate(translate, -camera->Position);

    std::lock_guard<std::mutex> lock(camera->_impl->CamMutex);

    camera->ViewMatrix = rotate * translate;
    camera->ProjectionMatrix = glm::perspective(
        glm::radians(camera->FOV),
        static_cast<float>(camera->Views.Raster.x) / static_cast<float>(camera->Views.Raster.y),
        0.1f,
        1000.0f);

    // Work out Look Vector
    glm::mat4 inverseView = glm::inverse(camera->ViewMatrix);

    camera->LookDirection.x = inverseView[2][0];
    camera->LookDirection.y = inverseView[2][1];
    camera->LookDirection.z = inverseView[2][2];

    camera->_impl->DidUpdate = true;
}

bool camera_did_update(Camera* camera)
{
    std::lock_guard<std::mutex> lock(camera->_impl->CamMutex);
    return camera->_impl->DidUpdate;
}

void camera_new_frame(Camera* camera)
{
    std::lock_guard<std::mutex> lock(camera->_impl->CamMutex);
    camera->_impl->DidUpdate = false;
}

glm::mat4 camera_get_view(Camera* camera)
{
    std::lock_guard<std::mutex> lock(camera->_impl->CamMutex);
    return camera->ViewMatrix;
}

glm::mat4 camera_get_projection(Camera* camera)
{
    std::lock_guard<std::mutex> lock(camera->_impl->CamMutex);
    return camera->ProjectionMatrix;
}

glm::mat4 camera_get_look(Camera* camera)
{
    std::lock_guard<std::mutex> lock(camera->_impl->CamMutex);
    return camera->LookMatrix;
}

void camera_raster_set_viewport(Camera* camera, glm::ivec2 viewport)
{
    std::lock_guard<std::mutex> lock(camera->_impl->CamMutex);
    camera->Views.Raster = viewport;
    camera->ProjectionMatrix = glm::perspective(
        glm::radians(camera->FOV),
        static_cast<float>(viewport.x) / static_cast<float>(viewport.y),
        0.1f,
        1000.0f);
}

glm::ivec2 camera_raster_get_viewport(Camera* camera)
{
    std::lock_guard<std::mutex> lock(camera->_impl->CamMutex);
    return camera->Views.Raster;
}

void camera_ray_set_viewport(Camera* camera, glm::ivec2 viewport)
{
    std::lock_guard<std::mutex> lock(camera->_impl->CamMutex);
    camera->Views.Ray = viewport;
}

glm::ivec2 camera_ray_get_viewport(Camera* camera)
{
    std::lock_guard<std::mutex> lock(camera->_impl->CamMutex);
    return camera->Views.Ray;
}

void camera_move(Camera* camera, uint8_t movement_delta)
{
    if (movement_delta == 0)
        return;

    // Rotate by camera direction
    glm::vec3 delta(0.0f);

    glm::mat2 rotate {
        cos(camera->Yaw), -sin(camera->Yaw),
        sin(camera->Yaw), cos(camera->Yaw)
    };

    glm::vec2 f(0.0, 1.0);
    f = f * rotate;

    if (movement_delta & 0x80) {
        delta.z -= f.y;
        delta.x -= f.x;
    }
    if (movement_delta & 0x20) {
        delta.z += f.y;
        delta.x += f.x;
    }
    if (movement_delta & 0x40) {
        delta.z += f.x;
        delta.x += -f.y;
    }
    if (movement_delta & 0x10) {
        delta.z -= f.x;
        delta.x -= -f.y;
    }
    if (movement_delta & 0x8) {
        delta.y += 1;
    }
    if (movement_delta & 0x4) {
        delta.y -= 1;
    }

    // get current view matrix
    glm::mat4 mat = camera_get_view(camera);
    glm::vec3 forward(mat[0][2], mat[1][2], mat[2][2]);
    glm::vec3 strafe(mat[0][0], mat[1][0], mat[2][0]);

    // forward vector must be negative to look forward.
    // read :http://in2gpu.com/2015/05/17/view-matrix/
    camera->Position += delta * camera->Speed;

    // update the view matrix
    camera_update(camera);
}

void camera_mouse_move(Camera* camera, glm::vec2 mouse_delta)
{
    if (glm::length(mouse_delta) == 0)
        return;
    // note that yaw and pitch must be converted to radians.
    // this is done in update() by glm::rotate
    camera->Yaw += camera->MouseSensitivity * (mouse_delta.x / 100);
    camera->Pitch += camera->MouseSensitivity * (mouse_delta.y / 100);
    camera->Pitch = glm::clamp<float>(camera->Pitch, -M_PI / 2, M_PI / 2);

    camera_update(camera);
}

void camera_set_position(Camera* camera, glm::vec3 position)
{
    camera->Position = position;
    camera_update(camera);
}

void camera_set_euler_look(Camera* camera, float roll, float pitch, float yaw)
{
    camera->Roll = roll;
    camera->Pitch = pitch;
    camera->Yaw = yaw;
    camera->LookDirection.x = cos(camera->Yaw) * cos(camera->Pitch);
    camera->LookDirection.y = sin(camera->Yaw) * cos(camera->Pitch);
    camera->LookDirection.z = sin(camera->Pitch);

    camera_update(camera);
}

void camera_set_look(Camera* camera, glm::vec3 look_direction)
{
    camera->LookDirection = look_direction;
    camera->Pitch = asin(-look_direction.y);
    camera->Yaw = atan2(look_direction.x, look_direction.z);

    camera_update(camera);
}

glm::vec3 camera_get_position(Camera* camera)
{
    return camera->Position;
}

}
