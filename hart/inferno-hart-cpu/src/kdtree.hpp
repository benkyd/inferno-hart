#pragma once

#include <vector>
#include <algorithm>
#include <limits>

#include <hart_graphics.hpp>

#include <tracing/ray.hpp>
#include <yolo/yolo.hpp>

using namespace inferno;

struct Triangle
{
    unsigned int indices[3];
};

struct AABB
{
    glm::vec3 min;
    glm::vec3 max;
};

struct KDNode
{
    AABB aabb;
    unsigned int splitAxis;
    std::vector<Triangle> triangles;
    KDNode *left;
    KDNode *right;
};

inline AABB createAABB(const std::vector<glm::vec3>& vertices, const std::vector<Triangle>& triangles)
{
    glm::vec3 min(std::numeric_limits<float>::max());
    glm::vec3 max(std::numeric_limits<float>::min());

    for (const auto& triangle : triangles)
    {
        for (int i = 0; i < 3; ++i)
        {
            min = glm::min(min, vertices[triangle.indices[i]]);
            max = glm::max(max, vertices[triangle.indices[i]]);
        }
    }

    return { min, max };
}

inline bool intersectAABB(const AABB& aabb, const glm::vec3& origin, const glm::vec3& direction, float& tNear, float& tFar)
{
    for (int i = 0; i < 3; ++i)
    {
        float invDirection = 1.0f / direction[i];
        float t0 = (aabb.min[i] - origin[i]) * invDirection;
        float t1 = (aabb.max[i] - origin[i]) * invDirection;

        if (invDirection < 0.0f)
        {
            std::swap(t0, t1);
        }

        tNear = t0 > tNear ? t0 : tNear;
        tFar = t1 < tFar ? t1 : tFar;

        if (tNear > tFar)
        {
            return false;
        }
    }

    return true;
}

inline KDNode* buildKDTree(const std::vector<glm::vec3>& vertices, std::vector<Triangle>& triangles, unsigned int depth = 0)
{
    if (triangles.empty())
    {
        return nullptr;
    }

    unsigned int splitAxis = depth % 3;
    std::sort(triangles.begin(), triangles.end(), [&](const Triangle& a, const Triangle& b) {
        return vertices[a.indices[0]][splitAxis] < vertices[b.indices[0]][splitAxis];
    });

    size_t midIdx = triangles.size() / 2;
    KDNode* node = new KDNode;
    node->splitAxis = splitAxis;
    node->aabb = createAABB(vertices, triangles);
    node->triangles.push_back(triangles[midIdx]);

    std::vector<Triangle> leftTriangles(triangles.begin(), triangles.begin() + midIdx);
    std::vector<Triangle> rightTriangles(triangles.begin() + midIdx + 1, triangles.end());

    node->left = buildKDTree(vertices, leftTriangles, depth + 1);
    node->right = buildKDTree(vertices, rightTriangles, depth + 1);

    return node;
}

inline void intersectKDTree(KDNode* node, const glm::vec3& origin, const glm::vec3& direction, std::vector<Triangle>& hitCandidates) {
    if (!node)
    {
        return;
    }

    float tNear = -std::numeric_limits<float>::max();
    float tFar = std::numeric_limits<float>::max();
    if (!intersectAABB(node->aabb, origin, direction, tNear, tFar))
    {
        return;
    }

    if (!node->left && !node->right)
    {
        hitCandidates.insert(hitCandidates.end(), node->triangles.begin(), node->triangles.end());
        return;
    }

    intersectKDTree(node->left, origin, direction, hitCandidates);
    intersectKDTree(node->right, origin, direction, hitCandidates);
}

inline void deleteKDTree(KDNode* node)
{
    if (node)
    {
        deleteKDTree(node->left);
        deleteKDTree(node->right);
        delete node;
    }
}

