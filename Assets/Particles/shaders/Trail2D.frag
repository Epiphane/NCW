#version 330 core

// Interpolated values from the geometry shaders
in vec2 fUV;
in float fAge;

// Ouput data
layout(location = 0) out vec4 fragColor;

uniform sampler2D uTexture;
uniform vec4 uTransparentColor = vec4(0, 0, 0.4, 1);
uniform float uParticleLifetime;

void main()
{
   vec2 uv = fUV;
   if (uv.y < 0.02) { uv.y = 0.02; }
   fragColor = vec4(texture(uTexture, uv));
   fragColor.a *= min((1 - fAge / uParticleLifetime), 1);
   if (fragColor == uTransparentColor)
   {
      discard;
   }
}
