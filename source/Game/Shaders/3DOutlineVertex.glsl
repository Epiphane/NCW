#version 330 core

layout(location = 0) in vec4 aPosition;
layout(location = 1) in vec3 aNormal;

uniform mat4 uProjMatrix;
uniform mat4 uViewMatrix;
uniform mat4 uModelMatrix;

// Ship tint
uniform vec3 uShipTint;

out vec3 vNormal;
out vec4 vWorldSpace;

#define M_PI 3.1415926535897932384626433832795

void main()
{
   vWorldSpace = uModelMatrix * aPosition;
   
   float offset = -1.0f * (vWorldSpace.z - 0); // Offset z by current ship position
   vWorldSpace.y += 0.0002f * pow(offset, 2);

   gl_Position = uProjMatrix * uViewMatrix * vWorldSpace;

   vNormal = normalize((uModelMatrix * vec4(aNormal, 0)).xyz);
   
   // and now Ryan makes up normals...
   // This makes normals behave with the upward curve of the track
   vNormal.z += 0.000001f * offset * offset;
   vNormal = normalize(vNormal);
}