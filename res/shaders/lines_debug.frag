#version 450 core

layout (location = 0) in vec3 vFragPos;
layout (location = 1) in vec3 vColour;

layout(location = 0) out vec4 outColour;

void main()
{
    outColour = vec4(vColour, 1.0);
}

