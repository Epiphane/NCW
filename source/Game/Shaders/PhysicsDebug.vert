#version 330 core

layout(location = 0) in vec2 aPosition;
layout(location = 1) in vec2 aUV;

uniform vec3 uPosition;

uniform mat4 uProjMatrix;
uniform mat4 uViewMatrix;
uniform mat4 uModelMatrix;

out vec3 gColor;

void main()
{
   gl_Position = uProjMatrix * uViewMatrix * vec4(uPosition, 1);
   gColor = vec3(0, 0, 1);
}
