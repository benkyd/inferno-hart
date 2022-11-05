#include "renderer.hpp"

#include "shader.hpp"

#include <scene/camera.hpp>
#include <scene/scene.hpp>
#include <scene/material.hpp>
#include <scene/mesh.hpp>

#include <iostream>

using namespace inferno;

RasterizeRenderer::RasterizeRenderer()
{
    glGenFramebuffers(1, &mRenderTarget);
    glBindFramebuffer(GL_FRAMEBUFFER, mRenderTarget);

    glGenTextures(1, &mRenderTargetTexture);
    glBindTexture(GL_TEXTURE_2D, mRenderTargetTexture);

	glEnable(GL_DEPTH_TEST);
    glDisable(GL_CULL_FACE);

    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, 800, 600, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);  

    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, mRenderTargetTexture, 0);  

    assert(glCheckFramebufferStatus(GL_FRAMEBUFFER) == GL_FRAMEBUFFER_COMPLETE);
    glBindFramebuffer(GL_FRAMEBUFFER, 0);  
}

RasterizeRenderer::~RasterizeRenderer()
{

}

void RasterizeRenderer::setScene(Scene* scene)
{
    mCurrentScene = scene;
}

void RasterizeRenderer::setTargetSize(glm::vec2 size)
{
    glBindFramebuffer(GL_FRAMEBUFFER, mRenderTarget);
    glBindTexture(GL_TEXTURE_2D, mRenderTargetTexture);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, size.x, size.y, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    mRenderTargetSize = size;
}

GLuint RasterizeRenderer::getRenderedTexture()
{
    return mRenderTarget;
}

glm::vec2 RasterizeRenderer::getTargetSize()
{
    return mRenderTargetSize;
}

void RasterizeRenderer::prepare()
{
    glBindFramebuffer(GL_FRAMEBUFFER, mRenderTarget);
    glClearColor(0.1, 0.1, 0.1, 1.0);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void RasterizeRenderer::draw()
{
    glBindFramebuffer(GL_FRAMEBUFFER, mRenderTarget);
    glViewport(0, 0, mRenderTargetSize.x, mRenderTargetSize.y);

    for (Mesh* m : mCurrentScene->getRenderables())
    {
	    m->getMaterial()->getGlShader()->use();
        GLint uniTrans = glGetUniformLocation(m->getMaterial()->getGlShader()->getProgram(), "model");
        glUniformMatrix4fv(uniTrans, 1, GL_FALSE, glm::value_ptr(glm::mat4(1.0f)));

        GLint uniView = glGetUniformLocation(m->getMaterial()->getGlShader()->getProgram(), "view");
        glUniformMatrix4fv(uniView, 1, GL_FALSE, glm::value_ptr(mCurrentScene->getCamera()->GetViewMatrix()));

        GLint uniProj = glGetUniformLocation(m->getMaterial()->getGlShader()->getProgram(), "proj");
        glUniformMatrix4fv(uniProj, 1, GL_FALSE, glm::value_ptr(mCurrentScene->getCamera()->GetProjectionMatrix()));
        
        glBindVertexArray(m->getVAO());
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m->getEBO());

        glDrawElements(GL_TRIANGLES, m->getIndexCount() * sizeof(uint32_t), GL_UNSIGNED_INT, 0);
    }
    
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}
