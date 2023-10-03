#include "renderer.hpp"

#include "scene/object.hpp"
#include "shader.hpp"

#include <algorithm>
#include <scene/camera.hpp>
#include <scene/material.hpp>
#include <scene/scene.hpp>
#include <scene/object.hpp>
#include <scene/mesh.hpp>

#include <iostream>

namespace inferno::graphics {

std::unique_ptr<PreviewRenderer> preview_create()
{
    std::unique_ptr<PreviewRenderer> renderer = new PreviewRenderer;

    glGenFramebuffers(1, &renderer->RenderTarget);
    glBindFramebuffer(GL_FRAMEBUFFER, renderer->RenderTarget);

    glGenTextures(1, &renderer->RenderTargetTexture);
    glBindTexture(GL_TEXTURE_2D, renderer->RenderTargetTexture);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, 800, 600, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    glGenTextures(1, &renderer->RenderTargetDepthTexture);
    glBindTexture(GL_TEXTURE_2D, renderer->RenderTargetDepthTexture);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH24_STENCIL8, 800, 600, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

    // Attach the texture to the framebuffer.
    glFramebufferTexture2D(GL_DRAW_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, renderer->RenderTargetDepthTexture, 0);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, renderer->RenderTargetTexture, 0);

    assert(glCheckFramebufferStatus(GL_FRAMEBUFFER) == GL_FRAMEBUFFER_COMPLETE);
    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    return renderer;
}

void preview_cleanup(std::unique_ptr<PreviewRenderer>& renderer)
{
}

void preview_set_viewport(std::unique_ptr<PreviewRenderer>& renderer, std::unique_ptr<Viewport> viewport)
{
}

GLuint preview_get_rendered_texture(std::unique_ptr<PreviewRenderer>& renderer)
{
    glBindFramebuffer(GL_FRAMEBUFFER, renderer->RenderTarget);
    return renderer->RenderTargetTexture;
}

void preview_draw(std::unique_ptr<PreviewRenderer>& renderer, std::unique_ptr<scene::Scene>& scene)
{
    const glm::ivec2& viewport = graphics::raster_get_viewport(scene::scene_get_camera(scene));
    glBindFramebuffer(GL_FRAMEBUFFER, renderer->RenderTarget);

    glBindTexture(GL_TEXTURE_2D, renderer->RenderTargetTexture);
    glTexImage2D(
        GL_TEXTURE_2D,
        0,
        GL_RGB,
        viewport.x,
        viewport.y,
        0,
        GL_RGB,
        GL_UNSIGNED_BYTE,
        NULL);

    glBindTexture(GL_TEXTURE_2D, renderer->RenderTargetDepthTexture);
    glTexImage2D(GL_TEXTURE_2D,
        0,
        GL_DEPTH24_STENCIL8,
        viewport.x,
        viewport.y,
        0,
        GL_DEPTH_COMPONENT,
        GL_FLOAT,
        NULL);

    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    // clear

    glBindFramebuffer(GL_FRAMEBUFFER, renderer->RenderTarget);
    glClearColor(0.1, 0.1, 0.1, 1.0);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    // draw

    glBindFramebuffer(GL_FRAMEBUFFER, renderer->RenderTarget);
    glViewport(0,
        0,
        renderer->Viewport.x,
        renderer->Viewport.y);

    glEnable(GL_DEPTH_TEST);

    for (std::unique_ptr<scene::SceneObject>& o : scene::scene_get_renderables(scene)) {
        for (scene::Mesh* m : scene::scene_object_get_meshs(o)) {
            glBindVertexArray(m->getVAO());
            glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m->getEBO());

            glDrawElements(GL_TRIANGLES, m->getIndexCount() * sizeof(uint32_t), GL_UNSIGNED_INT, 0);

        }
    }

    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

}
