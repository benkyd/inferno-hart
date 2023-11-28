#version 450 core

layout (location = 0) in vec3 position;
layout (location = 1) in vec3 normal;

layout (binding = 0) uniform SceneUniformBufferObject {
    mat4 proj;
    mat4 view;
} ubo;

// mat4 model;

layout (location = 0) out vec3 vNormal;
layout (location = 1) out vec3 vFragPos;

void main() {
    vFragPos = vec3(vec4(position, 1.0));
    vNormal = normal;
    gl_Position = ubo.proj * ubo.view * vec4(position, 1.0);
}
