#version 330 core
out vec4 FragColor;
  
in vec2 texCoords;

uniform sampler2D screenTexture;

void main()
{ 
    float depthValue = texture(screenTexture, texCoords).r;
    FragColor = vec4(vec3(depthValue), 1.0);
}