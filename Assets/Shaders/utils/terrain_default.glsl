//? #version 460
//? #include "noise.glsl"
//? #include "uniforms.glsl"

uniform uint uOctaves;
uniform float uFrequency;

float getElevation(ivec3 coords)
{
    // Read current global position
    float elevation = smoothstep(0.0, 1.5, float(coords.y - 8.0) / float(uChunkSize.y - 16));

    float py = float(coords.y) / float(uChunkSize.y);
    elevation = 0.5*pow(2.0*(0.5-abs(py-0.5)), 0.65);
    if (py > 0.5) elevation = 1.0 - elevation;

    vec3 chunkSize = uChunkSize;

    vec3 pt = coords + uWorldCoords;
    float noiseVal = 0;
    float div = 1.0;
    for (uint i = 0; i < uOctaves; i++)
    {
        noiseVal += snoise(pt * uFrequency * div + uWorldBase) / div;
        div *= 2.0;
    }
    noiseVal = (noiseVal + 1.0) / 2.0;
    noiseVal -= pow(elevation, 1);

    if (noiseVal < 0.3 && elevation > 0)
    {
        return -1;
    }

    return elevation;
}
