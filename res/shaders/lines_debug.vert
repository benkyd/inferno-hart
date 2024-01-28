#version 450 core

layout (location = 0) in vec3 position;
layout (location = 1) in vec3 colour;

layout (binding = 0) uniform SceneUniformBufferObject {
    mat4 proj;
    mat4 view;
    mat4 unused0;
    mat4 unused1;
} ubo;

// mat4 model;

layout (location = 0) out vec3 vFragPos;
layout (location = 1) out vec3 vColour;

void main() {
    vFragPos = vec3(vec4(position, 1.0));
    vColour = colour;
    gl_Position = ubo.proj * ubo.view * vec4(position, 1.0);
}
