#include <hart_graphics.hpp>

#include <tracing/ray.hpp>

#include <vector>
#include <algorithm>
#include <limits>
#include <utility>
#include <array>
#include <iostream>

using namespace inferno;

bool AABBIntersection(glm::vec3 min, glm::vec3 max, const Ray* r)
{
    float tmin = 0.0, tmax = INFINITY;
    glm::vec3 invDir = 1.0f / r->Direction;

    for (int i = 0; i < 3; ++i) {
        float t1 = (min[i] - r->Origin[i]) * invDir[i];
        float t2 = (max[i] - r->Origin[i]) * invDir[i];

        tmin = std::max(tmin, std::min(t1, t2));
        tmax = std::min(tmax, std::max(t1, t2));
    }

    return tmin <= tmax;
}

struct KDNode {
    uint32_t TriIdx;
    glm::vec3 MinBounds;
    glm::vec3 MaxBounds;
    KDNode* LeftChild;
    KDNode* RightChild;

    KDNode(uint32_t triIdx, glm::vec3 minBounds, glm::vec3 maxBounds)
        : TriIdx(triIdx), MinBounds(minBounds), MaxBounds(maxBounds), LeftChild(nullptr), RightChild(nullptr) {}

    ~KDNode() {
        delete LeftChild;
        delete RightChild;
    }

    void setLeftChild(KDNode* child) {
        LeftChild = child;
        updateBounds();
    }

    void setRightChild(KDNode* child) {
        RightChild = child;
        updateBounds();
    }

    void updateBounds() {
        if (LeftChild && RightChild) {
            MinBounds = glm::min(LeftChild->MinBounds, RightChild->MinBounds);
            MaxBounds = glm::max(LeftChild->MaxBounds, RightChild->MaxBounds);
        }
    }
};

class KDTree {
    public:
        KDTree(float* vertices, uint32_t* indices, std::vector<uint32_t>& indicesToProcess, uint32_t startIdx, uint32_t endIdx, uint32_t depthLimit)
            : mVertices(vertices), mIndices(indices), mDepthLimit(depthLimit), mRoot(nullptr)
        {
            if (indicesToProcess.size() == 0) {
                return;
            }

            mRoot = buildNode(indicesToProcess, startIdx, endIdx, 0);
        }

        ~KDTree() {
            delete mRoot;
        }

        void intersect(const Ray* ray, std::vector<uint32_t>& outIndices) {
            intersect(mRoot, ray, outIndices);
        }

    private:
        KDNode* buildNode(std::vector<uint32_t>& indicesToProcess, uint32_t startIdx, uint32_t endIdx, uint32_t depth) {
            if (startIdx >= endIdx || depth >= mDepthLimit) {
                return nullptr;
            }

            if (endIdx - startIdx == 1) {
                return new KDNode(indicesToProcess[startIdx], getVertexBounds(mIndices[indicesToProcess[startIdx] * 3]), getVertexBounds(mIndices[indicesToProcess[startIdx] * 3]));
            }

            glm::vec3 minBounds(INFINITY), maxBounds(-INFINITY);
            for (uint32_t i = startIdx; i < endIdx; ++i) {
                const glm::vec3& v0 = getVertexBounds(mIndices[indicesToProcess[i] * 3]);
                const glm::vec3& v1 = getVertexBounds(mIndices[indicesToProcess[i] * 3 + 1]);
                const glm::vec3& v2 = getVertexBounds(mIndices[indicesToProcess[i] * 3 + 2]);
                minBounds = glm::min(minBounds, glm::min(v0, glm::min(v1, v2)));
                maxBounds = glm::max(maxBounds, glm::max(v0, glm::max(v1, v2)));
            }

            uint32_t axis = depth % 3;
            uint32_t median = partition(indicesToProcess, startIdx, endIdx, axis);

            KDNode* node = new KDNode(0, minBounds, maxBounds);

            std::vector<uint32_t> leftIndices(indicesToProcess.begin() + startIdx, indicesToProcess.begin() + median);
            std::vector<uint32_t> rightIndices(indicesToProcess.begin() + median, indicesToProcess.begin() + endIdx);

            node->setLeftChild(buildNode(leftIndices, startIdx, median, depth + 1));
            node->setRightChild(buildNode(rightIndices, 0, endIdx - median, depth + 1));

            return node;
        }

        void intersect(const KDNode* node, const Ray* ray, std::vector<uint32_t>& outIndices) {
            if (!node) {
                return;
            }

            if (AABBIntersection(node->MinBounds, node->MaxBounds, ray)) {
                if (node->LeftChild || node->RightChild) {
                    intersect(node->LeftChild, ray, outIndices);
                    intersect(node->RightChild, ray, outIndices);
                }
                else {
                    outIndices.push_back(node->TriIdx);
                }
            }
        }

        glm::vec3 getVertexBounds(uint32_t index) {
            return { mVertices[index * 3], mVertices[index * 3 + 1], mVertices[index * 3 + 2] };
        }

        uint32_t partition(std::vector<uint32_t>& indicesToProcess, uint32_t startIdx, uint32_t endIdx, uint32_t axis) {
            uint32_t medianIdx = (startIdx + endIdx) / 2;
            glm::vec3 pivot = getVertexBounds(mIndices[indicesToProcess[medianIdx] * 3]);
            std::nth_element(indicesToProcess.begin() + startIdx, indicesToProcess.begin() + medianIdx, indicesToProcess.begin() + endIdx, 
                    [this, &pivot, axis](uint32_t a, uint32_t b) { return getVertexBounds(mIndices[a * 3])[axis] < getVertexBounds(mIndices[b * 3])[axis]; });
            return medianIdx;
        }
    private:
        float* mVertices;
        uint32_t* mIndices;
        uint32_t mDepthLimit;
        KDNode* mRoot;
};
