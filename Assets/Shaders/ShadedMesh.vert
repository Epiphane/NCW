#version 330 core

layout(location = 0) in vec4 aPosition;
layout(location = 1) in vec4 aColor;
layout(location = 2) in vec4 aNormal;
layout(location = 3) in float aOcclusion;

uniform mat4 uProjMatrix;
uniform mat4 uViewMatrix;
uniform mat4 uModelMatrix;

flat out vec3 fNormal;
out vec4 fColor;
out float fDepth;

void main()
{
   gl_Position = uProjMatrix * uViewMatrix * uModelMatrix * vec4(aPosition);
   fColor = aColor;
   fNormal = aNormal.xyz;
   fDepth = -(uViewMatrix * uModelMatrix * aPosition).z;
}
