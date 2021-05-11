#version 330 core

// Interpolated values from the geometry shaders
in vec2 fUV;

// Ouput data
layout(location = 0) out vec4 fragColor;

uniform sampler2D uTexture;
uniform vec4 uTransparentColor = vec4(0, 0, 0.4, 1);

void main()
{
   fragColor = vec4(texture(uTexture, fUV));
   if (fragColor == uTransparentColor)
   {
      discard;
   }
}
