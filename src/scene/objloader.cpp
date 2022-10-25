// Adapted from https://raw.githubusercontent.com/tamato/simple-obj-loader/master/objloader.cpp
#include <scene/objloader.hpp>

#include <spdlog/spdlog.h>

#include <iostream>
#include <cstdlib>
#include <fstream>
#include <cstring>
#include <assert.h>
#include <map>

using namespace inferno;

struct FaceVert
{
    FaceVert() : vert(-1), norm(-1), coord(-1) {}

    int vert;
    int norm;
    int coord;
};

struct vert_less {
    bool operator() (const FaceVert& lhs, const FaceVert& rhs) const
    {
        // handle any size mesh
        if (lhs.vert != rhs.vert) return (lhs.vert<rhs.vert);
        if (lhs.norm != rhs.norm) return (lhs.norm<rhs.norm);
        if (lhs.coord!=rhs.coord) return (lhs.coord<rhs.coord);
        return false;
        // the following breaks down on very large meshes
        // const unsigned long prime1 = 73856093;
        // const unsigned long prime2 = 19349663;
        // const unsigned long prime3 = 83492791;
        // unsigned long lh = (lhs.vert * prime1) ^ (lhs.norm * prime2) ^ (lhs.coord * prime3);
        // unsigned long rh = (rhs.vert * prime1) ^ (rhs.norm * prime2) ^ (rhs.coord * prime3);
        // return lh < rh;
    }
};

ObjLoader::ObjLoader()
    : mTexCoordLayers(1)
{

}

void ObjLoader::load(std::filesystem::path file)
{
    if (!std::filesystem::exists(file))
    {
        spdlog::error("OBJ File does not exist at ", file);
        return;
    }

    std::ifstream inf;
    inf.open(file.c_str(), std::ios_base::in);
    if (!inf.is_open()) 
    {
        spdlog::error("Failed to open OBJ file ", file);
        return;
    }

    mPositions.clear();
    mNormals.clear();
    mTexCoords.clear();
    mFaces.clear();

    char *delims = " \n\r";
    const unsigned int CHARACTER_COUNT = 500;
    char line[CHARACTER_COUNT] = {0};

    std::vector<glm::vec3> verts;
    std::vector<glm::vec3> norms;
    std::vector<glm::vec2> texcoords;

    std::map<FaceVert, int, vert_less> uniqueverts;
    unsigned int vert_count = 0;

    while (inf.good()) 
    {
        memset( (void*)line, 0, CHARACTER_COUNT);
        inf.getline(line, CHARACTER_COUNT);
        if (inf.eof()) break;

        char *token = strtok(line, delims);
        if (token == NULL || token[0] == '#' || token[0] == '$')
            continue;

        // verts look like:
        //	v float float float
        if (strcmp(token, "v") == 0) 
        {
            float x=0, y=0, z=0, w=1;
            sscanf(line+2, "%f %f %f %f", &x, &y, &z, &w);
            verts.push_back( glm::vec3(x/w,y/w,z/w) );
        }
        // normals:
        // 	nv float float float
        else if (strcmp(token, "vn") == 0) {

            float x=0, y=0, z=0;
            sscanf(line+3, "%f %f %f", &x, &y, &z);
            norms.push_back( glm::vec3(x,y,z) );
        }
        // texcoords:
        //	vt	float float
        else if (strcmp(token, "vt") == 0) 
        {
            float x=0, y=0, z=0;
            sscanf(line+3, "%f %f %f", &x, &y, &z);
            texcoords.push_back( glm::vec2(x, y) );
        }

        // keep track of smoothing groups
        // s [number|off]
        else if (strcmp(token, "s") == 0) 
        {

        }

        // faces start with:
        //	f
        else if (strcmp(token, "f") == 0) 
        {

            std::vector<int> vindices;
            std::vector<int> nindices;
            std::vector<int> tindices;

            // fill out a triangle from the line, it could have 3 or 4 edges
            char *lineptr = line + 2;
            while (lineptr[0] != 0) {
                while (lineptr[0] == ' ') ++lineptr;

                int vi=0, ni=0, ti=0;
                if (sscanf(lineptr, "%d/%d/%d", &vi, &ni, &ti) == 3) {
                    vindices.push_back(vi-1);
                    nindices.push_back(ni-1);
                    tindices.push_back(ti-1);
                }
                else
                if (sscanf(lineptr, "%d//%d", &vi, &ni) == 2) {
                    vindices.push_back(vi-1);
                    nindices.push_back(ni-1);
                }
                else
                if (sscanf(lineptr, "%d/%d", &vi, &ti) == 2) {
                    vindices.push_back(vi-1);
                    tindices.push_back(ti-1);
                }
                else
                if (sscanf(lineptr, "%d", &vi) == 1) {
                    vindices.push_back(vi-1);
                }

                while(lineptr[0] != ' ' && lineptr[0] != 0) ++lineptr;
            }

            // being that some exporters can export either 3 or 4 sided polygon's
            // convert what ever was exported into triangles
            for (size_t i=1; i<vindices.size()-1; ++i) 
            {
                Face face;
                FaceVert tri;

                tri.vert = vindices[0];
                if (!nindices.empty())
                    tri.norm = nindices[0];
                if (!tindices.empty())
                    tri.norm = tindices[0];

                if (uniqueverts.count(tri) == 0)
                    uniqueverts[tri] = vert_count++;
                face.a = uniqueverts[tri];

                tri.vert = vindices[i];
                if (!nindices.empty())
                    tri.norm = nindices[i];
                if (!tindices.empty())
                    tri.norm = tindices[i];

                if (uniqueverts.count(tri) == 0)
                    uniqueverts[tri] = vert_count++;
                face.b = uniqueverts[tri];

                tri.vert = vindices[i+1];
                if (!nindices.empty())
                    tri.norm = nindices[i+1];
                if (!tindices.empty())
                    tri.norm = tindices[i+1];

                if (uniqueverts.count(tri) == 0)
                    uniqueverts[tri] = vert_count++;
                face.c = uniqueverts[tri];
                mFaces.push_back(face);
            }
        }
    }
    inf.close();

    mPositions.resize(vert_count);
    if (norms.size() > 0)
        mNormals.resize(vert_count);
    if (texcoords.size() > 0)
        mTexCoords.resize(vert_count);

    std::map<FaceVert, int, vert_less>::iterator iter;
    for (iter = uniqueverts.begin(); iter != uniqueverts.end(); ++iter) 
    {

        mPositions[iter->second] = verts[iter->first.vert];

        if ( norms.size() > 0 ) 
        {
            mNormals[iter->second] = norms[iter->first.norm];
        }

        if ( texcoords.size() > 0) 
        {
            mTexCoords[iter->second] = texcoords[iter->first.coord];
        }
    }    
}

int ObjLoader::getIndexCount()
{
    return (int)mFaces.size() * 3;
}

int ObjLoader::getVertCount()
{
    return (int)mPositions.size();
}

const uint32_t* ObjLoader::getFaces()
{
    return (const uint32_t*)&mFaces[0];
}

const float* ObjLoader::getPositions()
{
    return (const float*)&mPositions[0];
}

const float* ObjLoader::getNormals()
{
    return (const float*)&mNormals[0];
}

uint32_t ObjLoader::getTexCoordLayers()
{
    return mTexCoordLayers;
}

const float* ObjLoader::getTexCoords(int multiTexCoordLayer)
{
    assert(multiTexCoordLayer < mTexCoordLayers);
    return (const float*)&mTexCoords[0];
}
