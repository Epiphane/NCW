#version 330 core

layout (location = 0) in vec2 aPosition;
layout (location = 1) in vec2 aUV;
layout (location = 2) in vec4 aColor;

uniform mat4 uProjMatrix;
out vec2 fUV;
out vec4 fColor;

void main()
{
   fUV = aUV;
   fColor = aColor;
   gl_Position = uProjMatrix * vec4(aPosition.xy, 0, 1);
}
