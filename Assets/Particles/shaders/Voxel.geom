#version 330

// Lots of this is stolen from Shared/Shaders/Voxel.geom, enjoy

layout(points) in;
layout(triangle_strip, max_vertices=24) out;

uniform mat4 uProjMatrix;
uniform mat4 uViewMatrix;
uniform vec3 uCameraPos;
uniform float uParticleLifetime;

// Custom parameters
uniform float uVoxelSize;
uniform vec4 uColor0;
uniform vec4 uColor1;
uniform vec4 uColor2;
uniform vec4 uColor3;

in float gType[];
in vec4 gRotation[];
in float gAge[];

flat out vec3 fNormal;
flat out vec3 fColor;

#define TYPE_EMITTER 1.0f
#define TYPE_PARTICLE 2.0f

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
   if (gType[0] == TYPE_EMITTER) {
      return;
   }

   vec4 center = gl_in[0].gl_Position;
   float age = gAge[0];
   mat4 mvp = uProjMatrix * uViewMatrix;
   mat4 model = mat4(1, 0, 0, 0,
                     0, 1, 0, 0,
                     0, 0, 1, 0,
                     0, 0, 0, 1);

   float angle = gRotation[0][3];
   if (angle != 0)
   {
      model = Rotate(gRotation[0].xyz, angle);
      mvp = mvp * model;
   }

   vec3 color = uColor3.xyz;
   float agePercent = age / uParticleLifetime;
   if (agePercent < uColor1[3]) {
      float progress = agePercent / uColor1[3];
      color = uColor1.xyz * progress + uColor0.xyz * (1 - progress);
   }
   else if (agePercent < uColor2[3]) {
      float progress = (agePercent - uColor1[3]) / (uColor2[3] - uColor1[3]);
      color = uColor2.xyz * progress + uColor1.xyz * (1 - progress);
   }
   else if (agePercent < 1) {
      float progress = (agePercent - uColor2[3]) / (uColor3[3] - uColor2[3]);
      color = uColor3.xyz * progress + uColor2.xyz * (1 - progress);
   }

   float size = uVoxelSize * (1 - age / (2 * uParticleLifetime));

   // vec4 directions, in camera space, for computing the other corners
   vec4 dx = mvp[0] / 2.0f * size;
   vec4 dy = mvp[1] / 2.0f * size;
   vec4 dz = mvp[2] / 2.0f * size;
   
   // Normals are computed from just the model matrix
   vec4 nx = normalize(model[0] / 2.0f * size);
   vec4 ny = normalize(model[1] / 2.0f * size);
   vec4 nz = normalize(model[2] / 2.0f * size);

   EmitQuad(center - dz, dy, dx, -nz, color);   // Back
   EmitQuad(center - dy, dx, dz, -ny, color);   // Bottom
   EmitQuad(center - dx, dz, dy, -nx, color);   // Left
   EmitQuad(center + dz, dx, dy, nz, color);    // Front
   EmitQuad(center + dy, dz, dx, ny, color);    // Top
   EmitQuad(center + dx, dy, dz, nx, color);    // Right
}
