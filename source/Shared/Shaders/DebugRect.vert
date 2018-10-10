#version 330 core

layout(location = 0) in vec2 aPosition;
layout(location = 1) in vec4 aColor;

out vec4 gColor;

uniform vec2 uWindowSize;

void main()
{
   vec2 pos = 2 * aPosition / uWindowSize - 1;

   gl_Position = vec4(pos.x, pos.y, 0, 1);
   gColor = aColor;
}
