#version 330 core

// Ouput data
layout(location = 0) out vec4 fragColor;

in vec4 fColor;

void main()
{
   fragColor = fColor;
}
