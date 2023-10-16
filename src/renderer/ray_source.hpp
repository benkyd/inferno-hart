// #pragma once
//
// #include <vector>
// #include <unordered_map>
//
// #include <graphics.hpp>
//
// #include <tracing/ray.hpp>
//
// namespace inferno {
//
// class Camera;
//
// struct ReferencedRayField {
//     RayField Field;
//     std::unordered_map<uint32_t, glm::ivec2> Reference;
// };
//
//
// class RaySource
// {
// public:
//     RaySource(Camera* camera);
//     ~RaySource();
//
//     void generate();
//     ReferencedRayField getInitialRays(bool MSAA);
//
// private:
//     Camera* mReferenceCamera;
// };
//
// }
