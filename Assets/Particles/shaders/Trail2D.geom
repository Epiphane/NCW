#version 330

layout(triangles) in;
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
out float fAge;

#define TYPE_EMITTER 1.0f
#define TYPE_PARTICLE 2.0f

void main()
{
   if (gType[0] == TYPE_EMITTER) {
      return;
   }

   if (gAge[0] > uParticleLifetime) {
      return;
   }

   // Rendering for front side:
   // 1--3  1
   // | / / |
   // 2  2--3

   // Front side
   gl_Position = gl_in[0].gl_Position;
   fUV = vec2(gRotation[2].y, 0);
   fAge = gAge[0];
   EmitVertex();

   gl_Position = gl_in[1].gl_Position;
   fUV = vec2(0, 1);
   fAge = gAge[1];
   EmitVertex();

   gl_Position = gl_in[2].gl_Position;
   fUV = vec2(1, gRotation[2].y);
   fAge = gAge[2];
   EmitVertex();
   EndPrimitive();

   // Rendering for front side:
   // 3  1--3
   // | \ \ |
   // 1--2  2

   // 1  3--1
   // | \ \ |
   // 3--2  2

   // Reverse side.
   gl_Position = gl_in[2].gl_Position;
   fUV = vec2(0, gRotation[2].y);
   fAge = gAge[2];
   EmitVertex();

   gl_Position = gl_in[1].gl_Position;
   fUV = vec2(1, 1);
   fAge = gAge[1];
   EmitVertex();

   gl_Position = gl_in[0].gl_Position;
   fUV = vec2(gRotation[2].x, 0);
   fAge = gAge[0];
   EmitVertex();

   EndPrimitive();
}
