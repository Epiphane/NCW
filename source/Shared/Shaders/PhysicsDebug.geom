#version 330 core

layout (points) in;
layout (line_strip, max_vertices=19) out;

out vec3 fColor;

uniform vec3 uPosition;
uniform vec3 uRotation;
uniform vec3 uSize;
uniform mat4 uProjMatrix;
uniform mat4 uViewMatrix;
uniform mat4 uModelMatrix;

void Point(vec4 origin, vec4 dx, vec4 dy, vec4 dz, vec3 color)
{
   fColor = color;
   gl_Position = origin + dx + dy + dz;
   EmitVertex();
}

void main()
{
   // gl_Position represents the center of the collision box.
   vec4 center = gl_in[0].gl_Position;

   mat4 mvp = uProjMatrix * uViewMatrix;

   // vec4 directions, in camera space, for computing the other corners
   vec4 dx = mvp[0] / 2.0f * uSize.x;
   vec4 dy = mvp[1] / 2.0f * uSize.y;
   vec4 dz = mvp[2] / 2.0f * uSize.z;
   
   Point(center, dx, dy, dz, vec3(1, 0, 1));
   Point(center, -dx, dy, -dz, vec3(0, 0, 0));
   Point(center, dx, dy, -dz, vec3(1, 0, 0));
   Point(center, dx, dy, dz, vec3(1, 0, 1));
   Point(center, -dx, dy, dz, vec3(0, 0, 1));
   Point(center, -dx, dy, -dz, vec3(0, 0, 0));

   Point(center, -dx, -dy, -dz, vec3(0, 1, 0));
   Point(center, dx, dy, -dz, vec3(1, 0, 0));
   Point(center, dx, -dy, -dz, vec3(1, 1, 0));
   Point(center, dx, dy, dz, vec3(1, 0, 1));
   Point(center, dx, -dy, dz, vec3(1, 1, 1));
   Point(center, -dx, dy, dz, vec3(0, 0, 1));
   Point(center, -dx, -dy, dz, vec3(0, 1, 1));

   Point(center, -dx, -dy, -dz, vec3(0, 1, 0));
   Point(center, dx, -dy, -dz, vec3(1, 1, 0));
   Point(center, dx, -dy, dz, vec3(1, 1, 1));
   Point(center, -dx, -dy, dz, vec3(0, 1, 1));
   Point(center, -dx, dy, -dz, vec3(0, 0, 0));

   EndPrimitive();
}