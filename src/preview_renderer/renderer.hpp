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

std::unique_ptr<PreviewRenderer> preview_create();
void preview_cleanup(std::unique_ptr<PreviewRenderer>& renderer);

GLuint preview_get_rendered_texture(std::unique_ptr<PreviewRenderer>& renderer);

void preview_draw(std::unique_ptr<PreviewRenderer>& renderer, std::unique_ptr<scene::Scene>& scene);

} // namespace inferno::graphics
