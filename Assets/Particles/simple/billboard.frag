#version 330

uniform sampler2D gColorMap;

in vec2 fTexCoord;
in float fParticleAge;

out vec4 FragColor;

#define SHELL_AGE 900.0

void main()
{
   //FragColor = vec4(0.39, 0.39, 0.39, 1.0) * texture(gColorMap, fTexCoord);
   /*
   FragColor =  texture(gColorMap, fTexCoord);
   
   if (FragColor.r >= 0.9 && FragColor.g >= 0.9 && FragColor.b >= 0.9 || FragColor.a < 0.4) {
      discard;
   } else {
      float ratio = fParticleAge / SHELL_AGE;
      FragColor = vec4(.98, 1.0 - (.70 * ratio), 0.0, 1.0);
      FragColor.a = .8 * (SHELL_AGE - fParticleAge) / SHELL_AGE;
   }
   */
   FragColor = vec4(fTexCoord.x, fTexCoord.y, 1, 1);
}