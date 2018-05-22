#version 330 core

flat in int vMaterial;
in vec2 vUV;
in vec3 vNormal;
in vec4 vWorldSpace;

in vec3 vLightPos;
in vec3 vCameraVec;

#define MAX_MATERIALS 30

uniform vec3 UaColor[MAX_MATERIALS];
uniform vec3 UdColor[MAX_MATERIALS];
uniform vec3 UsColor[MAX_MATERIALS];
uniform float Ushine[MAX_MATERIALS];

uniform bool uHasTextures;
uniform vec2 uTexScale[MAX_MATERIALS];
uniform sampler2DArray uTexUnits;

uniform float uPowerup;

uniform vec3 uShipTint;

out vec4 fragColor;
in vec4 vShadowCoord;

uniform sampler2DShadow uShadowMap;

// Specular with cook-torrance calculation
float specular(in vec3 rd, in vec3 norm, in vec3 lightDir, float roughness, float fresnel) {

    float NdotL = dot(norm, lightDir);
    float NdotV = dot(norm, -rd);

    float spe = 0.0;
    if (NdotL > 0.0 && NdotV > 0.0) {

        vec3 h = normalize(-rd + lightDir);

        float NdotH = max(dot(norm, h), 0.0);
        float VdotH = max(dot(-rd, h), 0.000001);
        float LdotH = max(dot(lightDir, h), 0.000001);

        // Beckmann distrib
        float cos2a = NdotH * NdotH;
        float tan2a = (cos2a - 1.0) / cos2a;
        float r = max(roughness, 0.01);
        float r2 = r * r;
        float D = exp(tan2a / r2) / (r2 * cos2a * cos2a);

        // Fresnel term - Schlick approximation
        float F = fresnel + (1.0 - fresnel) * pow(1.0 - VdotH, 5.0);

        // Geometric attenuation term
        float G = min(1.0, (2.0 * NdotH / VdotH) * min(NdotV, NdotL));

        // Cook Torrance
        spe = D * F * G / (4.0 * NdotV * NdotL);
    }

    return spe;
}


void main() {
   vec3 aColor = vec3(0);
   vec3 dColor = vec3(0);
   vec3 sColor = vec3(0);
   float shine = 0;
   vec3 lightVector = normalize(vLightPos - vWorldSpace.xyz);
   vec3 cameraVec = normalize(vCameraVec);
   
   float Id = max(dot(vNormal, lightVector), 0.0f);
   
   // Calculate a visibility value
   float bias = 0.005 * tan(acos(Id));
   bias = clamp(bias, 0.0, 0.01);
   float visibility = clamp(texture(uShadowMap, vec3(vShadowCoord.xy, vShadowCoord.z - bias)), 0.1, 1.0);
   
   if (vMaterial < 0) {
      // Placeholder: Basic metal
      aColor = vec3(0.15, 0.15, 0.15);
      dColor = vec3(0.4, 0.4, 0.4);
      sColor = vec3(0.14, 0.14, 0.14);
      shine = 76.8;
	  
	  float Is = specular(cameraVec, vNormal, lightVector, 0.4f, 0.3f);
	  
	  fragColor = vec4(Is * sColor * visibility + Id * dColor * visibility + (aColor*1.5), 1);
   }
   else {
      aColor = UaColor[vMaterial];
      dColor = UdColor[vMaterial];
      sColor = UsColor[vMaterial];
      shine = Ushine[vMaterial];
      
      if (uHasTextures) {
         vec2 UV = vec2(vUV.x * uTexScale[vMaterial].x,
                     vUV.y * uTexScale[vMaterial].y);
		vec3 textureColor = texture(uTexUnits, vec3(UV, vMaterial)).xyz;

		float Is = specular(cameraVec, vNormal, lightVector, 0.35f, 0.2f);
         
         fragColor = vec4(Is * vec3(1)  * visibility + Id * textureColor * visibility + (textureColor*1.5), 1);
      }
      else {
         float Is = specular(cameraVec, vNormal, lightVector, 0.25f, 0.01f);
         
         fragColor = vec4(Is * sColor * visibility + Id * dColor * visibility + (aColor*1.5), 1);
      }
      
      if (fragColor.a == 0)
         discard;
   }

   vec3 tint = uShipTint;
   if (uPowerup != 0) {
      if (mod(uPowerup, 0.2) > 0.1 && uPowerup < 0) {
         tint = tint / 2 - vec3(0.1);
      }
   }
   fragColor += vec4(tint, 1);
   
   vec4 fogColor = vec4(0.4, 0.4, 0.4, 0.0);
   float fogDensity = 1 - clamp((vWorldSpace.z + 1600) / 800.0, 0.0, 1.0);
   fragColor = mix(fragColor, fogColor, fogDensity);
}
