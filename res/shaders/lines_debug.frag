#version 450 core

layout (location = 0) in vec3 vFragPos;

layout(location = 0) out vec4 outColour;

void main()
{
    outColour = vec4(1., 0., 0., 1.0);
}

