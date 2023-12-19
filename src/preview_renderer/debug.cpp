#include "debug.hpp"

#include "graphics/buffer.hpp"
#include "graphics/rendertarget.hpp"
#include "graphics/shader.hpp"
#include "graphics/vkrenderer.hpp"

#include "preview_renderer/renderer.hpp"

#include "scene/camera.hpp"
#include "scene/mesh.hpp"
#include "scene/scene.hpp"

#include <graphics.hpp>
#include <yolo/yolo.hpp>

namespace inferno::graphics {

struct DebugLine {
    glm::vec3 Start;
    glm::vec3 End;
    glm::vec3 Color;
};

struct DebugTextBillboard {
    glm::vec3 Position;
    glm::vec3 Color;
    std::string Text;
};

struct _DebugInternal {
    std::mutex DebugMutex;

    Buffer* LineBuffer = nullptr;
    Shader* LineShader;
};

static DebugDrawer* DebugDrawerInstance = nullptr;

void debug_init(PreviewRenderer* renderer)
{
    DebugDrawerInstance = new DebugDrawer;

    DebugDrawerInstance->Renderer = renderer;
    DebugDrawerInstance->LineElements = std::vector<DebugLine>();
    // DebugDrawerInstance->BillboardElements = std::vector<DebugTextBillboard>();

    DebugDrawerInstance->_Internal = new _DebugInternal;
    DebugDrawerInstance->_Internal->LineShader = shader_create(renderer->Renderer->Device,
        renderer->Renderer->Swap, SHADER_PROGRAM_TYPE_GRAPHICS_LINE);
    graphics::shader_load(
        DebugDrawerInstance->_Internal->LineShader, "res/shaders/lines_debug");
    graphics::shader_build(DebugDrawerInstance->_Internal->LineShader);

    yolo::debug("DebugDrawer initialized");
}

void debug_cleanup() { delete DebugDrawerInstance; }

void debug_do_depth_test(bool doDepthTest)
{
    DebugDrawerInstance->DoDepthTest = doDepthTest;
}

void debug_draw_line(glm::vec3 start, glm::vec3 end, glm::vec3 color)
{
    std::lock_guard<std::mutex> lock(DebugDrawerInstance->_Internal->DebugMutex);
    DebugDrawerInstance->LineElements.push_back({ start, end, color });
}

void debug_draw_ui() { ImGui::Checkbox("Show Overlay", &DebugDrawerInstance->DoShow); }

void debug_draw_to_preview(scene::Scene* scene)
{
    if (!DebugDrawerInstance->DoShow)
        return;

    uint32_t bufferSize
        = DebugDrawerInstance->LineElements.size() * sizeof(DebugLine) * 2;
    scene::DebugLineVert* bufferData = new scene::DebugLineVert[bufferSize];

    for (int i = 0; i < DebugDrawerInstance->LineElements.size(); i++) {
        bufferData[i * 2] = { DebugDrawerInstance->LineElements[i].Start,
            DebugDrawerInstance->LineElements[i].Color };
        bufferData[i * 2 + 1] = { DebugDrawerInstance->LineElements[i].End,
            DebugDrawerInstance->LineElements[i].Color };
    }

    if (bufferSize > 0 && DebugDrawerInstance->_Internal->LineBuffer == nullptr) {
        DebugDrawerInstance->_Internal->LineBuffer = vertex_buffer_create(
            DebugDrawerInstance->Renderer->Renderer->Device, bufferData, bufferSize);
    }

    auto backend = DebugDrawerInstance->Renderer->Renderer;
    VkCommandBuffer commandBuffer = *backend->CommandBufferInFlight;

    graphics::renderer_begin_pass(backend,
        DebugDrawerInstance->Renderer->PreviewRenderTarget,
        DebugDrawerInstance->Renderer->Viewport, false);

    graphics::shader_use(DebugDrawerInstance->_Internal->LineShader, commandBuffer,
        DebugDrawerInstance->Renderer->Viewport);

    scene::GlobalUniformObject GlobalUniformObject {
        .Projection = graphics::camera_get_projection(scene->Camera),
        .View = graphics::camera_get_view(scene->Camera),
    };

    graphics::shader_update_state(DebugDrawerInstance->_Internal->LineShader,
        commandBuffer, GlobalUniformObject, backend->CurrentFrameIndex);

    graphics::vertex_buffer_bind(
        DebugDrawerInstance->_Internal->LineBuffer, commandBuffer);

    if (DebugDrawerInstance->DoDepthTest)
        vkCmdSetDepthTestEnable(commandBuffer, VK_TRUE);
    else
        vkCmdSetDepthTestEnable(commandBuffer, VK_FALSE);

    vkCmdDraw(commandBuffer, DebugDrawerInstance->LineElements.size() * 2, 1, 0, 0);

    vkCmdSetDepthTestEnable(commandBuffer, VK_TRUE);

    graphics::renderer_end_pass(backend);

    DebugDrawerInstance->LineElements.clear();
    delete[] bufferData;
}

}
