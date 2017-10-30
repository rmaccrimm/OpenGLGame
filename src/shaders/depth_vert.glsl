#version 330 core
layout (location = 0) in vec3 pos;
layout (location = 1) in vec3 norm;

uniform mat4 lightSpaceMatrix;
uniform mat4 model;

void main()
{
    gl_Position = lightSpaceMatrix * model * vec4(pos, 1.0);
}