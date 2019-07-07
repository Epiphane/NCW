#version 330

layout (location = 0) in float aType;
layout (location = 1) in vec3 aPosition;
layout (location = 2) in float aAge;

out float gType;
out float gAge;

void main()
{
   gl_Position = vec4(aPosition, 1.0);
   gType = aType;
   gAge = aAge;
}