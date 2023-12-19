#version 450 core

layout (location = 0) in vec3 vNormal;
layout (location = 1) in vec3 vFragPos;

layout(location = 0) out vec4 outColour;

vec3 lightColour = {1.0, 1.0, 1.0};
vec3 lightPos = {12.0, 2.0, 4.0};
vec3 objectColour = {1.0, 0.74, 0.21};

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

    outColour = vec4(vNormal + 1.0 / 2.0, 1.0);
}
