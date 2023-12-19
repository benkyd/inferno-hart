#version 450 core

layout(location = 0) in vec3 fFragColour;

layout(location = 0) out vec4 outColour;

void main() {
    outColour = vec4(fFragColour, 1.0);
}
