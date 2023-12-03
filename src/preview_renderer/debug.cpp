#include "debug.hpp"

#include "preview_renderer/renderer.hpp"
#include "graphics/shader.hpp"
#include "graphics/buffer.hpp"
#include "scene/camera.hpp"
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

    Buffer* LineBuffer;
    Shader* LineShader;
};

static DebugDrawer* DebugDrawerInstance = nullptr;

void debug_init()
{
    DebugDrawerInstance = new DebugDrawer;

    DebugDrawerInstance->LineElements = std::vector<DebugLine>();
    DebugDrawerInstance->BillboardElements = std::vector<DebugTextBillboard>();

    DebugDrawerInstance->_Internal = new _DebugInternal;
    // DebugDrawerInstance->_Internal->LineShader = shader_create();

    yolo::debug("DebugDrawer initialized");
}

void debug_cleanup()
{
    delete DebugDrawerInstance;
}

void debug_attach_renderer(PreviewRenderer* renderer)
{
    DebugDrawerInstance->Renderer = renderer;
}

void debug_draw_line(glm::vec3 start, glm::vec3 end, glm::vec3 color)
{
    std::lock_guard<std::mutex> lock(DebugDrawerInstance->_Internal->DebugMutex);
    DebugDrawerInstance->LineElements.push_back({ start, end, color });
}

void debug_draw_text_billboard(glm::vec3 position, glm::vec3 color, std::string text)
{
    std::lock_guard<std::mutex> lock(DebugDrawerInstance->_Internal->DebugMutex);
    DebugDrawerInstance->BillboardElements.push_back({ position, color, text });
}

void debug_draw_ui()
{
    ImGui::Checkbox("Show Overlay", &DebugDrawerInstance->DoShow);
}

void debug_draw_to_target(scene::Scene* scene)
{
    if (!DebugDrawerInstance->DoShow)
        return;

    auto renderer = DebugDrawerInstance->Renderer;

    GLuint vertex_position_arr_size = DebugDrawerInstance->LineElements.size() * 2 * 3;
    GLfloat vertex_position_arr[vertex_position_arr_size];
    for (int i = 0; i < DebugDrawerInstance->LineElements.size(); i++) {
        auto line = DebugDrawerInstance->LineElements[i];
        vertex_position_arr[i * 6 + 0] = line.Start.x;
        vertex_position_arr[i * 6 + 1] = line.Start.y;
        vertex_position_arr[i * 6 + 2] = line.Start.z;
        vertex_position_arr[i * 6 + 3] = line.End.x;
        vertex_position_arr[i * 6 + 4] = line.End.y;
        vertex_position_arr[i * 6 + 5] = line.End.z;
    }


    // glDrawArrays(GL_LINES, 0, vertex_position_arr_size);

    DebugDrawerInstance->LineElements.clear();

    // glEnable(GL_TEXTURE_2D);
    // glEnable(GL_BLEND);
    // glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    //
    // for (auto& billboard : DebugDrawerInstance->BillboardElements) {
    //     glColor3f(billboard.Color.x, billboard.Color.y, billboard.Color.z);
    //     glRasterPos3f(billboard.Position.x, billboard.Position.y, billboard.Position.z);
    //
    //     for (auto& c : billboard.Text) {
    //         glutBitmapCharacter(GL_BITMAP, c);
    //     }
    // }

    DebugDrawerInstance->BillboardElements.clear();
}

}
