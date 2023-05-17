#pragma once

#include <graphics.hpp>

#include <memory>

namespace inferno::scene {
struct Scene;
}

namespace inferno::graphics {

struct Viewport;

typedef struct PreviewRenderer {
    GLuint RenderTarget = 0;
    GLuint RenderTargetTexture = 0;
    GLuint RenderTargetDepthTexture = 0;
    std::shared_ptr<scene::Scene> CurrentScene;
    std::shared_ptr<const Viewport> RenderTargetSize;
} PreviewRenderer;

std::unique_ptr<PreviewRenderer> preview_create();
void preview_cleanup(std::unique_ptr<PreviewRenderer>& renderer);

void preview_set_scene(std::unique_ptr<PreviewRenderer>& renderer, std::shared_ptr<scene::Scene> scene);
void preview_set_viewport(std::unique_ptr<PreviewRenderer>& renderer, std::shared_ptr<const Viewport> viewports);
const glm::ivec2& preview_get_viewport(std::unique_ptr<PreviewRenderer>& renderer);
GLuint preview_get_rendered_texture(std::unique_ptr<PreviewRenderer>& renderer);

void preview_prepare(std::unique_ptr<PreviewRenderer>& renderer);
void preview_draw(std::unique_ptr<PreviewRenderer>& renderer);

} // namespace inferno::graphics
