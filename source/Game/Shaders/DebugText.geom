#version 330 core

layout (lines) in;
layout (triangle_strip, max_vertices=4) out;

in vec2 gUV[];
out vec2 fUV;

void main()
{
   // Two input vertices will be the top left and bottom right
   vec4 vertices[4];
   vertices[0] = gl_in[0].gl_Position;
   vertices[3] = gl_in[1].gl_Position;
   
   vec2 uv[4];
   uv[0] = gUV[0];
   uv[3] = gUV[1];
   
   // Calculate the middle two vertices of the quad
   vertices[1] = vertices[2] = vertices[0];
   uv[1]       = uv[2]       = uv[0];
   
   // Top right
   vertices[1].x = vertices[3].x;
   uv[1].x       = uv[3].x;
   
   // Bottom left
   vertices[2].y = vertices[3].y;
   uv[2].y       = uv[3].y;
   
   float zVal = 0;
   //if (zVal >= 1) zVal -= 0.0000001;
   
   // Emit the vertices of the quad
   for(int i = 0; i < 4; i ++) {
      fUV = uv[i];
      gl_Position = vec4(vertices[i].xy, zVal, 1);
      EmitVertex();
   }
   
   EndPrimitive();
}

/*#version 330 core

layout (lines) in;
layout (triangle_strip, max_vertices=4) out;

// Input UVs coming from the vertex shader.
flat in vec2 gUV[];

// Output UVs going to the fragment shader.
flat out vec2 fUV;

void SendVertex(float posX, float posY, float uvX, float uvY)
{
   fUV = vec2(uvX, uvY);
   gl_Position = vec4(posX, posY, 0, 1);
   EmitVertex();
}

void main()
{
   float l = gl_in[0].gl_Position.x;
   float r = gl_in[1].gl_Position.x;
   float t = gl_in[0].gl_Position.y;
   float b = gl_in[1].gl_Position.y;

   float lUV = gUV[0].x;
   float rUV = gUV[1].x;
   float tUV = gUV[0].y;
   float bUV = gUV[1].y;

   SendVertex(r, b, rUV, bUV);
   SendVertex(r, t, rUV, tUV);
   SendVertex(l, b, lUV, bUV);
   SendVertex(l, t, lUV, tUV);

   EndPrimitive();
}*/