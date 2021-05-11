#version 330 core

layout (lines) in;
layout (triangle_strip, max_vertices=4) out;

in vec4 gColor[];
out vec4 fColor;

void main()
{
   // Two input vertices will be the top left and bottom right
   vec4 vertices[4];
   vertices[0] = gl_in[0].gl_Position;
   vertices[3] = gl_in[1].gl_Position;

   vec4 color[4];
   color[0] = gColor[0];
   color[3] = gColor[1];
   color[1] = color[2] = (gColor[0] + gColor[1]) / 2;

   // Calculate the middle two vertices of the quad
   vertices[1] = vertices[2] = vertices[0];

   // Bottom right
   vertices[1].x = vertices[3].x;

   // Top left
   vertices[2].y = vertices[3].y;

   // Emit the vertices of the quad
   for(int i = 0; i < 4; i ++) {
     fColor = color[i];
      gl_Position = vertices[i];
      EmitVertex();
   }

   EndPrimitive();
}
