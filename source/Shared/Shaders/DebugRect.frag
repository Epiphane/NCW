#version 330 core

// Ouput data
layout(location = 0) out vec4 fragColor;

uniform vec4 uColor;

void main()
{
   fragColor = uColor;
}
