#include "mesh.hpp"

#include <yolo/yolo.hpp>

#include <scene/objloader.hpp>

#include <iostream>

namespace inferno::scene {

Mesh::Mesh()
{

}

Mesh::~Mesh()
{
    delete mObjLoader;
}

void Mesh::loadOBJ(std::filesystem::path file)
{
    mObjLoader = new ObjLoader();
    mObjLoader->load(file);

    int vertCount = mObjLoader->getVertCount();
    for (int i = 0; i < vertCount * 3; i += 3)
    {
        Vert vert;
        vert.Position = {
            mObjLoader->getPositions()[i],
            mObjLoader->getPositions()[i+1],
            mObjLoader->getPositions()[i+2],
        };
        vert.Normal = {
            mObjLoader->getNormals()[i],
            mObjLoader->getNormals()[i+1],
            mObjLoader->getNormals()[i+2],
        };

        mVerticies.push_back(vert);
    }
}

void Mesh::ready()
{
    // TODO: ready check
    glGenVertexArrays(1, &mVAO);
    glGenBuffers(1, &mVBO);
    glGenBuffers(1, &mEBO);

    glBindVertexArray(mVAO);
    // load data into vertex buffers

    glBindBuffer(GL_ARRAY_BUFFER, mVBO);
    glBufferData(GL_ARRAY_BUFFER, mVerticies.size() * sizeof(Vert), &mVerticies[0], GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mEBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, mObjLoader->getIndexCount() * sizeof(uint32_t), &mObjLoader->getFaces()[0], GL_STATIC_DRAW);

    // set the vertex attribute pointers
    // vertex Positions
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vert), (void*)0);
    // vertex normals
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vert), (void*)offsetof(Vert, Normal));
    // vertex UV
    glEnableVertexAttribArray(2);
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vert), (void*)offsetof(Vert, UV));

    glBindVertexArray(0);

    yolo::debug("Mesh for preview ready...");
}

int Mesh::getVerticies(const float** v, const float** n)
{
    *v = &mObjLoader->getPositions()[0];
    *n = &mObjLoader->getNormals()[0];
    return mObjLoader->getVertCount();
}

int Mesh::getIndicies(const uint32_t** i)
{
    *i = &mObjLoader->getFaces()[0];
    yolo::debug("{}", &mObjLoader->getFaces()[0]);
    return mObjLoader->getIndexCount();
}

int Mesh::getIndexCount()
{
    return mObjLoader->getIndexCount();
}

void Mesh::setMaterial(Material* mat)
{
    mMaterial = mat;
}

Material* Mesh::getMaterial()
{
    return mMaterial;
}

GLuint Mesh::getVAO()
{
    return mVAO;
}

GLuint Mesh::getVBO()
{
    return mVBO;
}

GLuint Mesh::getEBO()
{
    return mEBO;
}

}
