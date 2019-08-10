#version 330

layout(points) in;
layout(triangle_strip, max_vertices=16) out;

uniform mat4 uProjMatrix;
uniform mat4 uViewMatrix;
uniform float uParticleLifetime;

// Custom parameters
uniform float uSize;
uniform float uSprites;
uniform float uSheetWidth;
uniform float uSheetHeight;

in float gType[];
in vec4 gRotation[];
in float gAge[];

out vec2 fUV;

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

mat4 RotateEuler(vec3 eulerAngles)
{
   return Rotate(vec3(1, 0, 0), eulerAngles.x) * 
      Rotate(vec3(0, 1, 0), eulerAngles.y) * 
      Rotate(vec3(0, 0, 1), eulerAngles.z);
}

void EmitQuad(vec4 origin, vec4 dy, vec4 dx, vec2 bottomLeft, vec2 topRight)
{
   gl_Position = origin - dx - dy;
   fUV = bottomLeft;
   EmitVertex();

   gl_Position = origin - dx + dy;
   fUV = vec2(topRight.x, bottomLeft.y);
   EmitVertex();

   gl_Position = origin + dx - dy;
   fUV = vec2(bottomLeft.x, topRight.y);
   EmitVertex();

   gl_Position = origin + dx + dy;
   fUV = topRight;
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
      model = RotateEuler(gRotation[0].xyz);//, angle);
      model = RotateEuler(vec3(0, 180, 0));//, angle);
      mvp = mvp * model;
   }

   float agePercent = age / uParticleLifetime;
   float size = uSize;

   // vec4 directions, in camera space, for computing the other corners
   // mat4 mvp = uProjMatrix * uViewMatrix;
   vec4 dx = mvp[0] / 2.0f * size;
   vec4 dy = mvp[1] / 2.0f * size;
   vec4 dz = mvp[2] / 2.0f * size;

   float frame = floor(uSprites * age / uParticleLifetime);
   float frameX = mod(frame, uSheetWidth);
   float frameY = floor(frame / uSheetWidth);
   float frameW = 1.0f / uSheetWidth;
   float frameH = 1.0f / uSheetHeight;
   vec2 baseUV = vec2(frameX * frameW, frameY * frameH);
   vec2 nextUV = baseUV + vec2(frameW, frameH);

   EmitQuad(center, dx, dy, vec2(baseUV.x, nextUV.y), vec2(nextUV.x, baseUV.y)); // Front
   EmitQuad(center, dx, -dy, baseUV, nextUV); // Back
}
