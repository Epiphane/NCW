#version 330 core

layout (points) in;
// Maximum un-culled faces is 3, with 6 vertices per face.
// TODO be smart and figure out the normal and manually cull faces here, so we can limit vertices to 12.
layout (triangle_strip, max_vertices=24) out;

// Input colors coming from the vertex shader.
flat in vec4 gColor[];
flat in vec4 gPosition[];
flat in int gEnabledFaces[];

// Output color going to the fragment shader.
flat out vec3 fNormal;
flat out vec4 fColor;

uniform float uVoxelSize = 1.0;
uniform mat4 uProjMatrix;
uniform mat4 uViewMatrix;
uniform mat4 uModelMatrix;

void EmitQuad(vec4 origin, vec4 dy, vec4 dx, vec4 normal)
{
   fColor = gColor[0];
   fNormal = normal.xyz;
   gl_Position = origin - dx - dy;
   EmitVertex();

   fColor = gColor[0];
   fNormal = normal.xyz;
   gl_Position = origin - dx + dy;
   EmitVertex();

   fColor = gColor[0];
   fNormal = normal.xyz;
   gl_Position = origin + dx - dy;
   EmitVertex();

   fColor = gColor[0];
   fNormal = normal.xyz;
   gl_Position = origin + dx + dy;
   EmitVertex();

   EndPrimitive();
}

void main()
{
   // gl_Position represents the center of a voxel.
   vec4 center = gl_in[0].gl_Position;

   mat4 mvp = uProjMatrix * uViewMatrix * uModelMatrix;

   // vec4 directions, in camera space, for computing the other corners
   vec4 dx = mvp[0] / 2.0f * uVoxelSize;
   vec4 dy = mvp[1] / 2.0f * uVoxelSize;
   vec4 dz = mvp[2] / 2.0f * uVoxelSize;
   
   // Normals are computed from just the model matrix
   vec4 nx = uModelMatrix[0] / 2.0f * uVoxelSize;
   vec4 ny = uModelMatrix[1] / 2.0f * uVoxelSize;
   vec4 nz = uModelMatrix[2] / 2.0f * uVoxelSize;

   // Back
   if ((gEnabledFaces[0] & 0x01) != 0) {
      EmitQuad(center - dz, dy, dx, -nz);
   }

   // Bottom
   if ((gEnabledFaces[0] & 0x02) != 0) {
      EmitQuad(center - dy, dx, dz, -ny);
   }

   // Left
   if ((gEnabledFaces[0] & 0x04) != 0) {
      EmitQuad(center - dx, dz, dy, -nx);
   }

   // Front
   if ((gEnabledFaces[0] & 0x08) != 0) {
      EmitQuad(center + dz, dx, dy, nz);
   }

   // Top
   if ((gEnabledFaces[0] & 0x10) != 0) {
      EmitQuad(center + dy, dz, dx, ny);
   }

   // Right
   if ((gEnabledFaces[0] & 0x20) != 0) {
      EmitQuad(center + dx, dy, dz, nx);
   }
}