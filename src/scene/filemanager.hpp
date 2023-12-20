
#include <vector>
#include <filesystem>

#include <assimp/Importer.hpp>
#include <assimp/scene.h>

#include <hart_graphics.hpp>

namespace inferno::scene {

typedef struct FileManager {
    Assimp::Importer Importer;
} File;

FileManager* filemanager_create();
void filemanager_cleanup();

aiScene* filemanager_load

}
