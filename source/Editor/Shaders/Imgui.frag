#version 330 core

in vec2 fUV;
in vec4 fColor;

uniform sampler2D uTexture;
layout (location = 0) out vec4 oColor;

void main()
{
   oColor = fColor * texture(uTexture, fUV.st);
}
