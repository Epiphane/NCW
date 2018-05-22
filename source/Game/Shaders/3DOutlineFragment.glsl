#version 330 core

in vec3 vNormal;
in vec4 vWorldSpace;

uniform vec3 uShipTint;

out vec4 fragColor;

void main() {
   fragColor = vec4(uShipTint * 3, 1.0);
   
   float direction = clamp(abs(dot(normalize(vNormal), normalize(-vWorldSpace.xyz))), 0.0, 1.0);
   if (direction > 0.4)
      discard;

   vec4 fogColor = vec4(0.4, 0.4, 0.4, 0.0);
   float fogDensity = 1 - clamp((vWorldSpace.z + 1600) / 800.0, 0.0, 1.0);
   fragColor = mix(fragColor, fogColor, fogDensity);
}
