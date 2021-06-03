//? #version 460
//? #include "uniforms.glsl"

uint getIndex(ivec3 coords)
{
	return (coords.y * uChunkSize.z + coords.z) * uChunkSize.x + coords.x;
}