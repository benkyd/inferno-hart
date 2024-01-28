#version 450 core

in vec4 vFragPos;

layout(location = 0) out vec4 outColour;

void main()
{
    outColour = vec4(vFragPos.xyz, 1.0);
}

