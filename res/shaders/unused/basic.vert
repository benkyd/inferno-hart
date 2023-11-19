#version 450 core

layout (location = 0) in vec3 position;
layout (location = 1) in vec3 normal;
layout (location = 2) in vec2 uv;

uniform mat4 model;
uniform mat4 view;
uniform mat4 proj;

out vec3 vNormal;
out vec3 vFragPos;

void main() {
    vFragPos = vec3(model * vec4(position, 1.0));
    vNormal = normal;
    gl_Position = proj * view * model * vec4(position, 1.0);
}
