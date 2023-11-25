#version 450 core

layout(location = 0) in vec3 vPosition;
layout(location = 1) in vec3 vColor;
layout(location = 2) in vec2 vUV;

layout(location = 0) out vec3 fFragColour;


void main() {
    gl_Position = vec4(vPosition, 1.0);
    fFragColour = vColor;
}

