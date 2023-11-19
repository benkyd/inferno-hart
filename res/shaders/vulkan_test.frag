#version 450 core

layout(location = 0) out vec4 outColour;

layout(location = 0) in vec4 fFragColour;

void main() {
    outColour = fFragColour;
}
