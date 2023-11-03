#pragma once

#include <graphics.hpp>

#include <memory>

namespace inferno::scene {
struct Scene;
}

namespace inferno::graphics {

struct Viewport;
struct Camera;
struct DebugDraw;

typedef struct PreviewRenderer {
    glm::ivec2 Viewport;

    GLuint RenderTarget = 0;
    GLuint RenderTargetTexture = 0;
    GLuint RenderTargetDepthTexture = 0;
} PreviewRenderer;

PreviewRenderer* preview_create();
void preview_cleanup(PreviewRenderer* renderer);

void preview_draw_debug_ui(PreviewRenderer* renderer);

void preview_draw_ui(PreviewRenderer* renderer);
void preview_set_viewport(PreviewRenderer* renderer, Camera* camera);

GLuint preview_get_rendered_texture(PreviewRenderer* renderer);

void preview_draw(PreviewRenderer* renderer, scene::Scene* scene);

} // namespace inferno::graphics
