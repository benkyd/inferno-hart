#include "renderer.hpp"

#include <graphics.hpp>

#include "debug.hpp"

#include "graphics/buffer.hpp"
#include "graphics/rendertarget.hpp"
#include "graphics/shader.hpp"
#include "graphics/vkrenderer.hpp"

#include "scene/mesh.hpp"
#include "scene/object.hpp"
#include "scene/scene.hpp"

#include <yolo/yolo.hpp>

#include <algorithm>
#include <scene/camera.hpp>
#include <scene/mesh.hpp>
#include <scene/object.hpp>
#include <scene/scene.hpp>

#include <iostream>

namespace inferno::graphics {

PreviewRenderer* preview_create(VulkanRenderer* vkrenderer)
{
    PreviewRenderer* renderer = new PreviewRenderer;
    renderer->Renderer = vkrenderer;

    renderer->DrawShader = graphics::shader_create(
        vkrenderer->Device, vkrenderer->Swap, SHADER_PROGRAM_TYPE_GRAPHICS);

    graphics::shader_load(renderer->DrawShader, "res/shaders/basic");
    graphics::shader_build(renderer->DrawShader);

    renderer->PreviewRenderTarget = graphics::rendertarget_create(
        renderer->Renderer->Device, { 1920, 1080 }, VK_FORMAT_R8G8B8A8_UNORM, true);

    // bind preview renderer to debugdraw
    debug_init(renderer);

    return renderer;
}

void preview_cleanup(PreviewRenderer* renderer) {
    delete renderer;
}

void preview_draw_ui(PreviewRenderer* renderer) { }

void preview_set_viewport(PreviewRenderer* renderer, Camera* camera)
{
    auto viewport = camera_raster_get_viewport(camera);
    renderer->Viewport.offset.x = 0;
    renderer->Viewport.offset.y = 0;
    renderer->Viewport.extent.width = viewport.x;
    renderer->Viewport.extent.height = viewport.y;
    renderer->HasViewportChanged = true;
}

RenderTarget* preview_get_target(PreviewRenderer* renderer)
{
    return renderer->PreviewRenderTarget;
}

void preview_draw(PreviewRenderer* renderer, scene::Scene* scene)
{
    VkCommandBuffer commandBuffer = *renderer->Renderer->CommandBufferInFlight;
    // if changed
    if (renderer->HasViewportChanged) {
        graphics::rendertarget_recreate(renderer->PreviewRenderTarget,
            renderer->Viewport.extent, VK_FORMAT_R8G8B8A8_UNORM);
    }

    graphics::renderer_begin_pass(
        renderer->Renderer, renderer->PreviewRenderTarget, renderer->Viewport);

    graphics::shader_use(renderer->DrawShader, commandBuffer, renderer->Viewport);

    scene::GlobalUniformObject globalUniformObject {
        .Projection = graphics::camera_get_projection(scene->Camera),
        .View = graphics::camera_get_view(scene->Camera),
    };

    graphics::shader_update_state(renderer->DrawShader, commandBuffer,
        globalUniformObject, renderer->Renderer->CurrentFrameIndex);

    for (scene::SceneObject* o : scene::scene_get_renderables(scene)) {
        for (scene::Mesh* m : scene::scene_object_get_meshs(o)) {

            graphics::vertex_buffer_bind(m->VertexBuffer, commandBuffer);
            graphics::index_buffer_bind(m->IndexBuffer, commandBuffer);

            vkCmdDrawIndexed(
                commandBuffer, m->IndexBuffer->GenericBuffer->Count, 1, 0, 0, 0);
        }
    }
    graphics::renderer_end_pass(renderer->Renderer);
    renderer->HasViewportChanged = false;
}

}
