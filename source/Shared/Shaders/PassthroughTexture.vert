#version 330 core

layout(location = 0) in vec3 aPosition;
layout(location = 1) in vec2 aUV;

out vec2 vUV;

uniform vec3 uOffset = vec3(0,0,0);

void main()
{
    gl_Position = vec4(aPosition + uOffset, 1);
	vUV = aUV;
}