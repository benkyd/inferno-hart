#include <scene/scene.hpp>

#include <scene/object.hpp>
#include <scene/camera.hpp>
#include <scene/mesh.hpp>

using namespace inferno;

Scene::Scene()
{

}

Scene::~Scene()
{

}

void Scene::setCamera(Camera* camera)
{
    mCurrentCamera = camera;
}

Camera* Scene::getCamera()
{
    return mCurrentCamera;
}

void Scene::addMesh(Mesh* mesh)
{
    mMeshs.push_back(mesh);
}

const std::vector<Mesh*>& Scene::getRenderables()
{
    return mMeshs;
}
