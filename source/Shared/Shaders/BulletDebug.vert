#version 330 core

layout(location = 0) in vec3 aPosition;

uniform mat4 uProjMatrix;
uniform mat4 uViewMatrix;
uniform mat4 uModelMatrix;
uniform vec3 uColor;

flat out vec4 fColor;

void main()
{
   gl_Position = uProjMatrix * uViewMatrix * uModelMatrix * vec4(aPosition, 1);
   fColor = vec4(uColor, 1);
}
