#type vertex
#version 450 core

layout(location = 0) in vec4 position;

uniform mat4 model;
uniform mat4 view;
uniform mat4 proj;

out vec4 vFragPos;

void main()
{
    vFragPos = model * position;
    gl_Position = proj * view * model * position;
}

#type fragment
#version 450 core

in vec4 vFragPos;

layout(location = 0) out vec4 outColour;

void main()
{
    outColour = vec4(vFragPos.xyz, 1.0);
}

