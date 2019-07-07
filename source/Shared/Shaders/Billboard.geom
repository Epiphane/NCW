#version 330

layout(points) in;
layout(triangle_strip) out;
layout(max_vertices = 4) out;

uniform mat4 uProjection;
uniform mat4 uView;
uniform vec3 uCameraPos;
uniform float uBillboardSize;

in float gType[];

out vec2 fTexCoord;

#define PARTICLE_TYPE_LAUNCHER 0.0f
#define PARTICLE_TYPE_SHELL 1.0f
#define PARTICLE_TYPE_SECONDARY_SHELL 2.0f

void main()
{
   if (Type0[0] != PARTICLE_TYPE_LAUNCHER) {
      vec3 Pos = gl_in[0].gl_Position.xyz; // position of the particle in world space
      
      vec3 look = normalize(uCameraPos - Pos);
      vec3 right = normalize(cross(look, vec3(0.0, 1.0, 0.0)));
      vec3 up = cross(right, look);
      
      // Bottom left corner of the quad
      vec3 vertexA = Pos - (right + up) * uBillboardSize;
      gl_Position = uProjection * uView * vec4(vertexA, 1.0);
      fTexCoord = vec2(0.0, 0.0);
      EmitVertex();
      
      // Upper left corner of the quad
      vec3 vertexB = Pos - (right - up) * uBillboardSize;
      gl_Position = uProjection * uView * vec4(vertexB, 1.0);
      fTexCoord = vec2(0.0, 1.0);
      EmitVertex();
      
      // Bottom right corner of the quad
      vec3 vertexC = Pos + (right - up) * uBillboardSize;
      gl_Position = uProjection * uView * vec4(vertexC, 1.0);
      fTexCoord = vec2(1.0, 0.0);
      EmitVertex();
      
      // Make the second triangle by creating a vertex at upper right corner
      vec3 vertexD = Pos + (right + up) * uBillboardSize;
      gl_Position = uProjection * uView * vec4(vertexD, 1.0);
      fTexCoord = vec2(1.0, 1.0);
      EmitVertex();
      
      EndPrimitive();
   }
}