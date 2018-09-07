#version 330 core

// Value from the geometry shader.
flat in vec3 fNormal;
flat in vec4 fColor;

// Ouput data
layout(location = 0) out vec4 fragColor;

vec3 lightDir = vec3(0, -5, -1);

void main() {
	vec3 rgb = fColor.xyz;
	float da = 0.45;
	float di = max(dot(-lightDir, fNormal), 0);

	fragColor = vec4(min(di + da, 1.0) * rgb, 1);
}