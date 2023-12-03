#pragma once

#include <graphics.hpp>

#include <memory>

namespace inferno::scene {
struct Scene;
}

namespace inferno::graphics {

struct VulkanRenderer;
struct Viewport;
struct Camera;
struct DebugDraw;
struct RenderTarget;
struct Shader;

typedef struct PreviewRenderer {
    VulkanRenderer* Renderer;

    RenderTarget* PreviewRenderTarget;

    Shader* DrawShader;

    VkRect2D Viewport;
    bool HasViewportChanged = false;
} PreviewRenderer;

PreviewRenderer* preview_create(VulkanRenderer* renderer);
void preview_cleanup(PreviewRenderer* renderer);

void preview_draw_debug_ui(PreviewRenderer* renderer);

void preview_draw_ui(PreviewRenderer* renderer);
void preview_set_viewport(PreviewRenderer* renderer, Camera* camera);

RenderTarget* preview_get_target(PreviewRenderer* renderer);

void preview_draw(PreviewRenderer* renderer, scene::Scene* scene);

} // namespace inferno::graphics
