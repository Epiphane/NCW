#version 330 core

layout (lines) in;
layout (triangle_strip, max_vertices=4) out;

void main()
{
   // Two input vertices will be the top left and bottom right
   vec4 vertices[4];
   vertices[0] = gl_in[0].gl_Position;
   vertices[3] = gl_in[1].gl_Position;
   
   // Calculate the middle two vertices of the quad
   vertices[1] = vertices[2] = vertices[0];
   
   // Bottom right
   vertices[1].x = vertices[3].x;
   
   // Top left
   vertices[2].y = vertices[3].y;
   
   float zVal = 0; // TODO: me :3
   
   // Emit the vertices of the quad
   for(int i = 0; i < 4; i ++) {
      gl_Position = vec4(vertices[i].xy, zVal, 1);
      EmitVertex();
   }
   
   EndPrimitive();
}
