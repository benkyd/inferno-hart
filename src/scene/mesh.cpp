#include <scene/mesh.hpp>

#include <spdlog/spdlog.h>

#include <scene/objloader.hpp>

using namespace inferno;

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
    for (int i = 0; i < vertCount; i += 3)
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
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, mObjLoader->getIndexCount() * sizeof(float), &mObjLoader->getFaces()[0], GL_STATIC_DRAW);

    // set the vertex attribute pointers
    // vertex Positions
    glEnableVertexAttribArray(0);	
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vert), (void*)0);
    // vertex normals
    glEnableVertexAttribArray(1);	
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vert), (void*)offsetof(Vert, Normal));

    glBindVertexArray(0);

    spdlog::debug("MESH READY");
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
