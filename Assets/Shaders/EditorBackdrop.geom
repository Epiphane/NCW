#version 330 core

layout (points) in;
layout (line_strip, max_vertices=6) out;

out vec3 fColor;

uniform mat4 uProjMatrix;
uniform mat4 uViewMatrix;
uniform mat4 uModelMatrix;
uniform bool uShowNegative;

void main()
{
   vec4 center = gl_in[0].gl_Position;

   mat4 mvp = uProjMatrix * uViewMatrix * uModelMatrix;

   // vec4 directions, in camera space, for computing the other corners
   vec4 dx = mvp[0] / 2.0f;
   vec4 dy = mvp[1] / 2.0f;
   vec4 dz = mvp[2] / 2.0f;

   float size = 500;

   // X
   gl_Position = center + size * dx;
   fColor = vec3(0, 0, 0);
   EmitVertex();
   gl_Position = center;
   fColor = vec3(1, 0, 0);
   EmitVertex();
   if (uShowNegative) {
      gl_Position = center - size * dx;
      fColor = vec3(0, 0, 0);
      EmitVertex();
   }
   EndPrimitive();

   // Y
   gl_Position = center + size * dy;
   fColor = vec3(0, 1, 0);
   EmitVertex();
   gl_Position = center;
   fColor = vec3(0, 1, 0);
   EmitVertex();
   if (uShowNegative) {
      gl_Position = center - size * dy;
      fColor = vec3(0, 0, 0);
      EmitVertex();
   }
   EndPrimitive();

   // Z
   gl_Position = center + size * dz;
      fColor = vec3(0, 0, 0);
   EmitVertex();
   gl_Position = center;
   fColor = vec3(0, 0, 1);
   EmitVertex();
   if (uShowNegative) {
      gl_Position = center - size * dz;
      fColor = vec3(0, 0, 0);
      EmitVertex();
   }
   EndPrimitive();
}
