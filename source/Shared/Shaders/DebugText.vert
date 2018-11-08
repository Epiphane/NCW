#version 330 core

layout(location = 0) in vec3 aPosition;
layout(location = 1) in vec2 aUV;

out vec2 gUV;

uniform float uMaxZValue = 1000;
uniform vec2 uOffset = vec2(0,0);
uniform vec2 uWindowSize;

void main()
{
   vec2 pos = 2 * (aPosition.xy + uOffset) / uWindowSize - 1;

   gl_Position = vec4(pos, aPosition.z / uMaxZValue, 1);
   gUV = aUV;
}
