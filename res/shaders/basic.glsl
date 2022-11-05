#type vertex
#version 450 core

layout (location = 0) in vec3 position;
layout (location = 1) in vec3 normal;

uniform mat4 model;
uniform mat4 view;
uniform mat4 proj;

out vec3 vNormal;
out vec3 vFragPos;

void main() {
    vFragPos = vec3(model * vec4(position, 1.0));
    vNormal = normal;
    gl_Position = proj * view * model * vec4(position, 1.0);
}

#type fragment
#version 450 core

layout(location = 0) out vec4 outColour;

in vec3 vNormal;
in vec3 vFragPos;

vec3 lightColour = {1.0, 1.0, 1.0};
vec3 lightPos = {12.0, 2.0, 4.0}; 
vec3 objectColour = {1.0, 0.5, 0.4};

void main() {
    // ambient
    float ambientStrength = 0.1;
    vec3 ambient = ambientStrength * lightColour;
  	
    // diffuse 
    vec3 norm = normalize(vNormal);
    vec3 lightDir = normalize(lightPos - vFragPos);
    float diff = max(dot(norm, lightDir), 0.0);
    vec3 diffuse = diff * lightColour;

    vec3 result = (ambient + diffuse) * objectColour;

    outColour = vec4(result, 1.0);
}
