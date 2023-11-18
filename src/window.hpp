#pragma once

#include "graphics.hpp"

#include <optional>
#include <string>

#define WINDOW_FLAGS ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoBringToFrontOnFocus | ImGuiWindowFlags_NoNavFocus | ImGuiWindowFlags_MenuBar | ImGuiWindowFlags_NoBackground | ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_NoCollapse

namespace inferno::graphics {

struct GraphicsDevice;

typedef void (*KeyCallback)(int key, int scan, int action, int mod);
typedef void (*MouseCallback)(double x, double y);

enum WINDOW_MODE {
    WIN_MODE_DEFAULT,
    WIN_MODE_FPS,
};

void window_create(std::string title, int width, int height);
void window_cleanup();

void window_set_surface(GraphicsDevice* device);

void window_set_title(std::string title);

void window_set_size(int w, int h);
void window_set_pos(int x, int y);
glm::vec2 window_get_size();
void window_get_pos(int& x, int& y);

GLFWwindow* window_get_glfw_window();
void window_set_mode(WINDOW_MODE mode);

void window_set_key_callback(KeyCallback callback);
KeyCallback window_get_key_callback();

bool window_new_frame();
void window_render();

}
