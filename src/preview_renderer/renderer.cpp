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
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, 800, 600, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);  


	glGenTextures(1, &mRenderTargetDepthTexture );
	glBindTexture(GL_TEXTURE_2D, mRenderTargetDepthTexture );
	glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH24_STENCIL8, 800, 600, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

	// Attach the texture to the framebuffer.
	glFramebufferTexture2D( GL_DRAW_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, mRenderTargetDepthTexture, 0 );
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

    glBindTexture(GL_TEXTURE_2D, mRenderTargetDepthTexture);
	glTexImage2D( GL_TEXTURE_2D, 0, GL_DEPTH24_STENCIL8, size.x, size.y, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL );

    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    mRenderTargetSize = size;
}

GLuint RasterizeRenderer::getRenderedTexture()
{
    glBindFramebuffer(GL_FRAMEBUFFER, mRenderTarget);
    return mRenderTargetTexture;
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

	glEnable(GL_DEPTH_TEST);
    // glDepthFunc(GL_GREATER);

    // glDisable(GL_CULL_FACE);

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
