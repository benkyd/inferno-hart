#include "renderer.hpp"

#include "shader.hpp"

#include <scene/camera.hpp>
#include <scene/material.hpp>
#include <scene/mesh.hpp>
#include <scene/scene.hpp>

#include <iostream>

namespace inferno::graphics {

std::unique_ptr<PreviewRenderer> preview_create()
{
    std::unique_ptr<PreviewRenderer> renderer = std::make_unique<PreviewRenderer>();

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

void preview_set_scene(std::unique_ptr<PreviewRenderer>& renderer, std::shared_ptr<Scene> scene)
{
    renderer->CurrentScene = scene;
}

void preview_set_viewport(std::unique_ptr<PreviewRenderer>& renderer, std::shared_ptr<const Viewport> viewport)
{
    renderer->RenderTargetSize = viewport;
    glBindFramebuffer(GL_FRAMEBUFFER, renderer->RenderTarget);

    glBindTexture(GL_TEXTURE_2D, renderer->RenderTargetTexture);
    glTexImage2D(
        GL_TEXTURE_2D,
        0,
        GL_RGB,
        viewport->Raster.x,
        viewport->Raster.y,
        0,
        GL_RGB,
        GL_UNSIGNED_BYTE,
        NULL);

    glBindTexture(GL_TEXTURE_2D, renderer->RenderTargetDepthTexture);
    glTexImage2D(GL_TEXTURE_2D,
        0,
        GL_DEPTH24_STENCIL8,
        viewport->Raster.x,
        viewport->Raster.y,
        0,
        GL_DEPTH_COMPONENT,
        GL_FLOAT,
        NULL);

    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

const glm::ivec2& preview_get_viewport(std::unique_ptr<PreviewRenderer>& renderer)
{
    return renderer->RenderTargetSize->Raster;
}

GLuint preview_get_rendered_texture(std::unique_ptr<PreviewRenderer>& renderer)
{
    glBindFramebuffer(GL_FRAMEBUFFER, renderer->RenderTarget);
    return renderer->RenderTargetTexture;
}

void preview_prepare(std::unique_ptr<PreviewRenderer>& renderer)
{
    glBindFramebuffer(GL_FRAMEBUFFER, renderer->RenderTarget);
    glClearColor(0.1, 0.1, 0.1, 1.0);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void preview_draw(std::unique_ptr<PreviewRenderer>& renderer)
{
    glBindFramebuffer(GL_FRAMEBUFFER, renderer->RenderTarget);
    glViewport(0,
        0,
        renderer->RenderTargetSize->Raster.x,
        renderer->RenderTargetSize->Raster.y);

    glEnable(GL_DEPTH_TEST);

    for (Mesh* m : renderer->CurrentScene->getRenderables()) {
        // m->getMaterial()->getGlShader()->use();
        // GLint uniTrans = glGetUniformLocation(m->getMaterial()->getGlShader()->getProgram(), "model");
        // glUniformMatrix4fv(uniTrans, 1, GL_FALSE, glm::value_ptr(glm::mat4(1.0f)));

        // GLint uniView = glGetUniformLocation(m->getMaterial()->getGlShader()->getProgram(), "view");
        // glUniformMatrix4fv(uniView, 1, GL_FALSE, glm::value_ptr(renderer->CurrentScene->getCamera()->getViewMatrix()));

        // GLint uniProj = glGetUniformLocation(m->getMaterial()->getGlShader()->getProgram(), "proj");
        // glUniformMatrix4fv(uniProj, 1, GL_FALSE, glm::value_ptr(renderer->CurrentScene->getCamera()->getProjectionMatrix()));

        glBindVertexArray(m->getVAO());
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m->getEBO());

        glDrawElements(GL_TRIANGLES, m->getIndexCount() * sizeof(uint32_t), GL_UNSIGNED_INT, 0);
    }

    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

}
