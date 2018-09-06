#version 330 core

// Interpolated values from the geometry shaders
in vec2 fUV;

// Ouput data
out vec4 fragColor;

uniform sampler2D uTexture;

void main()
{
   fragColor = vec4(1.0, 1.0, 1.0, texture(uTexture, fUV).r);
   //fragColor = texture(uTexture, fUV);
}