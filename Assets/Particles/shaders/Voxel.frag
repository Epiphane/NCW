#version 330 core

// Stolen from Shared/Shaders/Voxel.frag
flat in vec3 fNormal;
flat in vec3 fColor;

// Ouput data
layout(location = 0) out vec4 fragColor;

vec3 lightDir = vec3(0, -5, -2);

void main() {
	vec3 rgb = fColor;
	float da = 0.7;
	float di = max(dot(-lightDir, fNormal), 0);

	fragColor = vec4(min(di + da, 1.0) * rgb, 1);
}