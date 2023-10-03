#pragma once

#include <graphics.hpp>

#include <memory>

namespace inferno::scene {
struct Scene;
}

namespace inferno::graphics {

struct Viewport;

typedef struct PreviewRenderer {
    glm::ivec2 Viewport;

    GLuint RenderTarget = 0;
    GLuint RenderTargetTexture = 0;
    GLuint RenderTargetDepthTexture = 0;
} PreviewRenderer;

PreviewRenderer* preview_create();
void preview_cleanup(PreviewRenderer* renderer);

GLuint preview_get_rendered_texture(PreviewRenderer* renderer);

void preview_draw(PreviewRenderer* renderer, scene::Scene* scene);

} // namespace inferno::graphics
