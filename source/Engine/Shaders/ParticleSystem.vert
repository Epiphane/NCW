#version 330

layout (location = 0) in float aType;
layout (location = 1) in vec3 aPosition;
layout (location = 2) in vec3 aVelocity;
layout (location = 3) in float aAge;

out float gType;
out vec3 gPosition;
out vec3 gVelocity;
out float gAge;

void main()
{
    gType = aType;
    gPosition = aPosition;
    gVelocity = aVelocity;
    gAge = aAge;
}