#version 330 core

layout(location = 0) in vec3 aPosition;
layout(location = 1) in vec3 aColor;
layout(location = 2) in int aEnabledFaces;

flat out vec4 gColor;
flat out vec4 gPosition;
flat out int gEnabledFaces;

uniform mat4 uProjMatrix;
uniform mat4 uViewMatrix;
uniform mat4 uModelMatrix;

void main()
{
   gl_Position = uProjMatrix * uViewMatrix * uModelMatrix * vec4(aPosition, 1);
   gPosition = vec4(aPosition, 1);
   
	gColor = vec4(aColor, 1);
   gEnabledFaces = aEnabledFaces;
}
