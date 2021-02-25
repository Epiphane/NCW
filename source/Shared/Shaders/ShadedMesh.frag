#version 330 core

flat in vec3 fNormal;
in vec4 fColor;
in float fOcclusion;

layout(location = 0) out vec4 fragColor;

vec3 lightDir = normalize(vec3(1, -10, -1));

void main() {
	vec3 rgb = clamp( fColor.xyz, vec3(0.0, 0.0, 0.0), vec3(1.0, 1.0, 1.0) );
	float da = 0.6;
	float di = clamp( dot( fNormal, -lightDir ), 0.0, 1.0 );
	float occFactor = clamp( fOcclusion, 0.0, 1.0 );
	float occ = clamp( (da + di) * occFactor, 0.0, 1.0 );

	fragColor = vec4( occ * rgb, 1.0 );
}
