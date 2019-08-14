#version 330

layout(points) in;
layout(triangle_strip) out;
layout(max_vertices = 4) out;

uniform mat4 uProjMatrix;
uniform mat4 uViewMatrix;
uniform vec3 uCameraPos;
uniform float uBillboardSize;

in float gType[];
in float gAge[];

out vec2 fTexCoord;
out float fParticleAge;

#define PARTICLE_TYPE_LAUNCHER 0.0f
#define PARTICLE_TYPE_SHELL 1.0f
#define PARTICLE_TYPE_SECONDARY_SHELL 2.0f

void main()
{
   // if (gType[0] != PARTICLE_TYPE_LAUNCHER) {
      fParticleAge = gAge[0];
      vec3 Pos = gl_in[0].gl_Position.xyz; // position of the particle in world space
      
      vec3 look = normalize(uCameraPos - Pos);
      vec3 right = normalize(cross(look, vec3(0.0, 1.0, 0.0)));
      vec3 up = cross(right, look);
      
      vec4 iPos = uProjMatrix * uViewMatrix * vec4(Pos, 1.0);//vec4(0, 0, 0, 1);
      float bSize = 0.1;

      // Bottom left corner of the quad
      vec3 vertexA = Pos - (right + up) * uBillboardSize;
      gl_Position = uProjMatrix * uViewMatrix * vec4(vertexA, 1.0);
      // gl_Position = vec4(-1.0, -1.0, 0.5, 1.0);
      gl_Position = iPos + vec4(-bSize, -bSize, 0, 0);
      fTexCoord = vec2(0.0, 0.0);
      EmitVertex();
      
      // Upper left corner of the quad
      vec3 vertexB = Pos - (right - up) * uBillboardSize;
      gl_Position = uProjMatrix * uViewMatrix * vec4(vertexB, 1.0);
      // gl_Position = vec4(1.0, -1.0, 0.5, 1.0);
      gl_Position = iPos + vec4(bSize, -bSize, 0, 0);
      fTexCoord = vec2(0.0, 1.0);
      EmitVertex();
      
      // Bottom right corner of the quad
      vec3 vertexC = Pos + (right - up) * uBillboardSize;
      gl_Position = uProjMatrix * uViewMatrix * vec4(vertexC, 1.0);
      // gl_Position = vec4(-1.0, 1.0, 0.5, 1.0);
      gl_Position = iPos + vec4(-bSize, bSize, 0, 0);
      fTexCoord = vec2(1.0, 0.0);
      EmitVertex();
      
      // Make the second triangle by creating a vertex at upper right corner
      vec3 vertexD = Pos + (right + up) * uBillboardSize;
      gl_Position = uProjMatrix * uViewMatrix * vec4(vertexD, 1.0);
      // gl_Position = vec4(1.0, 1.0, 0.5, 1.0);
      gl_Position = iPos + vec4(bSize, bSize, 0, 0);
      fTexCoord = vec2(1.0, 1.0);
      EmitVertex();
      
      EndPrimitive();
   // }
}