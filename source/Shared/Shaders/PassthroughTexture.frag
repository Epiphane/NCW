#version 330 core

in vec2 vUV;

out vec4 color;

uniform sampler2D uTexture;

void main()
{
    color = vec4(texture(uTexture, vUV).xyz, 1);
}