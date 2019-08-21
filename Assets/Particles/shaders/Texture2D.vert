#version 330

layout (location = 0) in float aType;
layout (location = 1) in vec3 aPosition;
layout (location = 2) in vec4 aRotation;
layout (location = 4) in float aAge;

out float gType;
out vec4 gRotation;
out float gAge;

uniform mat4 uProjMatrix;
uniform mat4 uViewMatrix;

void main()
{
   gl_Position = uProjMatrix * uViewMatrix * vec4(aPosition, 1);
   gType = aType;
   gRotation = aRotation;
   gAge = aAge;
}
