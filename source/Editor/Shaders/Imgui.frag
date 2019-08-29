#version 330 core

in vec2 fUV;
in vec4 fColor;

uniform sampler2D uTexture;
layout (location = 0) out vec4 Out_Color;

void main()
{
   Out_Color = Frag_Color * texture(uTexture, Frag_UV.st);
}
