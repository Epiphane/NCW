#version 330

// Lots of this is stolen from Shared/Shaders/Voxel.geom, enjoy

layout(points) in;
layout(triangle_strip, max_vertices=24) out;

uniform mat4 uProjMatrix;
uniform mat4 uViewMatrix;
uniform mat4 uModelMatrix;
uniform vec3 uCameraPos;
uniform float uBillboardSize;

in float gType[];
in vec4 gRotation[];
in float gAge[];

flat out vec3 fNormal;
flat out vec3 fColor;

#define EMITTER 0.0f
#define PARTICLE 1.0f
#define LIFE 0.5f
#define VOXEL_SIZE 0.05f

mat4 Rotate(vec3 axis, float angle)
{
   axis = normalize(axis);
   float s = sin(angle);
   float c = cos(angle);
   float oc = 1.0 - c;

   return mat4(oc * axis.x * axis.x + c,           oc * axis.x * axis.y - axis.z * s,  oc * axis.z * axis.x + axis.y * s,  0.0,
               oc * axis.x * axis.y + axis.z * s,  oc * axis.y * axis.y + c,           oc * axis.y * axis.z - axis.x * s,  0.0,
               oc * axis.z * axis.x - axis.y * s,  oc * axis.y * axis.z + axis.x * s,  oc * axis.z * axis.z + c,           0.0,
               0.0,                                0.0,                                0.0,                                1.0);
}

void EmitQuad(vec4 origin, vec4 dy, vec4 dx, vec4 normal, vec3 color)
{
   fColor = color;
   fNormal = normal.xyz;
   gl_Position = origin - dx - dy;
   EmitVertex();

   fColor = color;
   fNormal = normal.xyz;
   gl_Position = origin - dx + dy;
   EmitVertex();

   fColor = color;
   fNormal = normal.xyz;
   gl_Position = origin + dx - dy;
   EmitVertex();

   fColor = color;
   fNormal = normal.xyz;
   gl_Position = origin + dx + dy;
   EmitVertex();

   EndPrimitive();
}

void main()
{
   if (gType[0] == EMITTER) {
      return;
   }

   vec3 RED = vec3(134, 2, 0) / 255.0f;
   vec3 ORANGE = vec3(255, 119, 1) / 255.0f;
   vec3 YELLOW = vec3(255, 239, 36) / 255.0f;
   vec3 WHITE = vec3(255, 255, 255) / 255.0f;

   vec3 color = WHITE;

   vec4 center = gl_in[0].gl_Position;
   float age = gAge[0];
   mat4 mvp = uProjMatrix * uViewMatrix;

   float angle = gRotation[0][3];
   if (angle != 0)
   {
      mvp = mvp * Rotate(gRotation[0].xyz, angle);
   }

   float stageLength = LIFE / 3;
   if (age < stageLength) {
      float progress = age / stageLength;
      color = ORANGE * progress + RED * (1 - progress);
   }
   else if (age < 2 * stageLength) {
      float progress = (age - stageLength) / stageLength;
      color = YELLOW * progress + ORANGE * (1 - progress);
   }
   else if (age < LIFE) {
      float progress = (age - 2 * stageLength) / stageLength;
      color = WHITE * progress + YELLOW * (1 - progress);
   }

   float uVoxelSize = VOXEL_SIZE * (1 - age / (2 * LIFE));

   // vec4 directions, in camera space, for computing the other corners
   // mat4 mvp = uProjMatrix * uViewMatrix;
   vec4 dx = mvp[0] / 2.0f * uVoxelSize;
   vec4 dy = mvp[1] / 2.0f * uVoxelSize;
   vec4 dz = mvp[2] / 2.0f * uVoxelSize;
   
   // Normals are computed from just the model matrix
   vec4 nx = normalize(mvp[0] / 2.0f * uVoxelSize);
   vec4 ny = normalize(mvp[1] / 2.0f * uVoxelSize);
   vec4 nz = normalize(mvp[2] / 2.0f * uVoxelSize);

   EmitQuad(center - dz, dy, dx, -nz, color);   // Back
   EmitQuad(center - dy, dx, dz, -ny, color);   // Bottom
   EmitQuad(center - dx, dz, dy, -nx, color);   // Left
   EmitQuad(center + dz, dx, dy, nz, color);    // Front
   EmitQuad(center + dy, dz, dx, ny, color);    // Top
   EmitQuad(center + dx, dy, dz, nx, color);    // Right
}
