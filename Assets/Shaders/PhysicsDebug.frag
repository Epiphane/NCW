#version 330 core

// Interpolated values from the geometry shaders
in vec3 fColor;

// Ouput data
layout(location = 0) out vec4 fragColor;

void main()
{
   fragColor = vec4(fColor, 1.0);
}