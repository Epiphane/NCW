#version 330 core

layout(location = 0) in vec3 aPosition;
layout(location = 1) in vec3 aColor;
layout(location = 2) in vec3 aNormal;
layout(location = 3) in float aOcclusion;

uniform mat4 uProjMatrix;
uniform mat4 uViewMatrix;
uniform mat4 uModelMatrix;

flat out vec3 fNormal;
out vec4 fColor;
out float fOcclusion;
out float fDepth;

void main()
{
   gl_Position = uProjMatrix * uViewMatrix * uModelMatrix * vec4(aPosition, 1);
   fColor = vec4(aColor, 1.0);
   fNormal = aNormal;
   fOcclusion = aOcclusion;
   fDepth = -(uViewMatrix * uModelMatrix * vec4(aPosition, 1)).z;
}
