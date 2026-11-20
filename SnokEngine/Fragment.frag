#version 460 core

out vec4 FragColor;

in vec3 ourNormal;
in vec2 TexCoord;
in vec3 WorldPos;

uniform sampler2D ourTexture;
uniform vec3 lightPosition;

void main()
{
   vec3 lightDirection = normalize(lightPosition - WorldPos);
   vec3 diffuseLight = step(0.2, max(dot(ourNormal,lightDirection), 0.0)) * vec3(1.0,1.0,1.0);
   FragColor = texture(ourTexture, TexCoord) * vec4(diffuseLight + vec3(0.2,0.2,0.2), 1.0);
}