#version 330 core

layout(location = 0) in vec3 aPosition;
layout(location = 1) in vec3 aColor;

uniform mat4 uProjMatrix;
uniform mat4 uViewMatrix;
uniform mat4 uModelMatrix;

out vec4 vColor;

void main()
{
   gl_Position = uProjMatrix * uViewMatrix * uModelMatrix * vec4(aPosition, 1);
   vColor = vec4(aColor, 1.0);
}
