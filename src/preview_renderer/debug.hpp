#pragma once

#include <graphics.hpp>

#include <string>
#include <optional>
#include <vector>
#include <mutex>

namespace inferno::scene {
struct Scene;
}

namespace inferno::graphics {

struct PreviewRenderer;

struct DebugLine;
struct DebugTextBillboard;

struct _DebugInternal;

struct DebugDrawer {
    _DebugInternal* _Internal = nullptr;

    bool DoShow = true;
    PreviewRenderer* Renderer = nullptr;
    std::vector<DebugLine> LineElements;
    std::vector<DebugTextBillboard> BillboardElements;
};

void debug_init();
void debug_cleanup();

void debug_attach_renderer(PreviewRenderer* renderer);

void debug_draw_line(glm::vec3 start, glm::vec3 end, glm::vec3 color);
void debug_draw_text_billboard(glm::vec3 position, glm::vec3 color, std::string text);

void debug_draw_ui();

void debug_draw_to_target(scene::Scene* scene);

}
