#version 330 core

layout (points) in;
// Maximum un-culled faces is 3, with 6 vertices per face.
// TODO be smart and figure out the normal and manually cull faces here, so we can limit vertices to 12.
layout (triangle_strip, max_vertices=72) out;

// Input colors coming from the vertex shader.
flat in vec4 gColor[];
flat in int gEnabledFaces[];
flat in int gOcclusion[];

// Output color going to the fragment shader.
flat out vec3 fNormal;
flat out vec4 fColor;
out float fOcclusion;

uniform float uVoxelSize = 1.0;
uniform mat4 uProjMatrix;
uniform mat4 uViewMatrix;
uniform mat4 uModelMatrix;

void DrawCorner(
    vec4 origin,
    vec4 dx, vec4 dy, vec4 dz,
    vec4 nx, vec4 ny, vec4 nz,
    float occA, float occB, float occC, float occD,
    float occE, float occF, float occG
)
{
    fColor = gColor[0];
    fNormal = ny.xyz;
    fOcclusion = (occA + occB) / 2.0;
    gl_Position = origin + dx + dy;
    EmitVertex();
    
    fColor = gColor[0];
    fNormal = ny.xyz;
    fOcclusion = (occA + occB + occC + occE) / 4.0;
    gl_Position = origin + dy;
    EmitVertex();
    
    fColor = gColor[0];
    //fColor = vec4(1, 0, 0, 1);
    fNormal = ny.xyz;
    fOcclusion = occA;
    gl_Position = origin + dx + dy + dz;
    EmitVertex();
    
    fColor = gColor[0];
    //fColor = vec4(0, 1, 0, 1);
    fNormal = ny.xyz;
    fOcclusion = (occA + occC) / 2.0;
    gl_Position = origin + dy + dz;
    EmitVertex();
    
    fColor = gColor[0];
    //fColor = vec4(0, 0, 1, 1);
    fNormal = -nz.xyz;
    fOcclusion = (occA + occC + occD + occF) / 4.0;
    gl_Position = origin + dz;
    EmitVertex();
    
    // Fake triangle
    fColor = gColor[0];
    //fColor = vec4(1, 1, 0, 1);
    fNormal = -nz.xyz;
    fOcclusion = (occA + occD) / 2.0;
    gl_Position = origin + dx + dz;
    EmitVertex();
    
    // Fake triangle
    fColor = gColor[0];
    //fColor = vec4(0, 1, 1, 1);
    fNormal = -nz.xyz;
    fOcclusion = occA;
    gl_Position = origin + dx + dy + dz;
    EmitVertex();
    
    fColor = gColor[0];
    //fColor = vec4(1, 0, 1, 1);
    fNormal = nx.xyz;
    fOcclusion = (occA + occB + occD + occG) / 4.0;
    gl_Position = origin + dx;
    EmitVertex();

    fColor = gColor[0];
    //fColor = vec4(0, 1, 0, 1);
    fNormal = nx.xyz;
    fOcclusion = (occA + occB) / 2.0;
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

    float o000 = 1.0 - ((gOcclusion[0] >> 28) & 0xf) / 16.0; // -dx, -dy, -dz
    float o001 = 1.0 - ((gOcclusion[0] >> 24) & 0xf) / 16.0; // -dx, -dy,  dz
    float o010 = 1.0 - ((gOcclusion[0] >> 20) & 0xf) / 16.0; // -dx,  dy, -dz
    float o011 = 1.0 - ((gOcclusion[0] >> 16) & 0xf) / 16.0; // -dx,  dy,  dz
    float o100 = 1.0 - ((gOcclusion[0] >> 12) & 0xf) / 16.0; //  dx, -dy, -dz
    float o101 = 1.0 - ((gOcclusion[0] >> 8 ) & 0xf) / 16.0; //  dx, -dy,  dz
    float o110 = 1.0 - ((gOcclusion[0] >> 4 ) & 0xf) / 16.0; //  dx,  dy, -dz
    float o111 = 1.0 - ((gOcclusion[0]      ) & 0xf) / 16.0; //  dx,  dy,  dz

    o101 = 0.6;

    DrawCorner(center,  dx,  dy,  dz,  nx,  ny,  nz, o111, o110, o011, o101, o010, o001, o100);
    DrawCorner(center,  dx, -dz,  dy, -nz, -nx, -ny, o110, o100, o010, o111, o000, o011, o101);
    DrawCorner(center, -dx,  dy, -dz, -nx,  ny, -nz, o010, o011, o110, o000, o111, o100, o001);
    DrawCorner(center, -dx,  dz,  dy,  nz,  nx, -ny, o011, o001, o111, o010, o101, o110, o000);

    DrawCorner(center, -dx, -dz, -dy,  nz, -nx,  ny, o000, o010, o100, o001, o110, o101, o011);
    DrawCorner(center, -dx, -dy,  dz, -nx, -ny,  nz, o001, o000, o101, o011, o100, o111, o010);
    DrawCorner(center,  dx, -dy, -dz,  nx, -ny, -nz, o100, o101, o000, o110, o001, o010, o111);
    DrawCorner(center,  dx,  dz, -dy, -nz,  nx,  ny, o101, o111, o001, o100, o011, o000, o110);

    if (gEnabledFaces[0] == 424)
    {
        DrawCorner(center, dx, dy, dz, nx, ny, nz, o110, o100, o010, o111, o000, o011, o101);
    }
}