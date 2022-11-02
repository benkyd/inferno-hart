#type vertex
#version 330 

layout (location = 0) in vec3 position;
layout (location = 1) in vec3 normal;

uniform mat4 model;
uniform mat4 view;
uniform mat4 proj;

void main() {
    gl_Position = proj * view * model * vec4(position, 1.0);
}

#type fragment
#version 330

out vec4 outColour;

void main() {
    outColour = vec4(1, 0, 0, 1);
}
