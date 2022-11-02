#include "renderer.hpp"

#include "shader.hpp"

#include <scene/camera.hpp>
#include <scene/scene.hpp>
#include <scene/material.hpp>
#include <scene/mesh.hpp>

using namespace inferno;

RasterizeRenderer::RasterizeRenderer()
{

}

RasterizeRenderer::~RasterizeRenderer()
{

}

void RasterizeRenderer::setScene(Scene* scene)
{
    mCurrentScene = scene;
}

void RasterizeRenderer::setTarget(GLuint renderTarget)
{

}

void RasterizeRenderer::prepare()
{

}

void RasterizeRenderer::draw()
{
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
}
