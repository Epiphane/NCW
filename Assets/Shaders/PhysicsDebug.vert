#version 330 core

layout(location = 0) in vec2 aPosition;
layout(location = 1) in vec2 aUV;

uniform vec3 uPosition;

uniform mat4 uProjMatrix;
uniform mat4 uViewMatrix;
uniform mat4 uModelMatrix;

void main()
{
   gl_Position = uProjMatrix * uViewMatrix * uModelMatrix * vec4(uPosition, 1);
}
