#type vertex
#version 450 core

layout (location = 0) in vec3 position;
layout (location = 1) in vec3 normal;

uniform mat4 model;
uniform mat4 view;
uniform mat4 proj;

out vec3 vNormal;

void main() {
    vNormal = normal;
    gl_Position = proj * view * model * vec4(position, 1.0);
}

#type fragment
#version 450 core

layout(location = 0) out vec4 outColour;

in vec3 vNormal;

void main() {
    outColour = vec4(vNormal, 1.0);
}
