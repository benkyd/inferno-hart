#type vertex 
#version 330 

in vec3 position;

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
    outColour = vec4(0.58, 0.61, 0.627, 1.0);
}
