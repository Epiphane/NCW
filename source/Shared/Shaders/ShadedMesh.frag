#version 330 core

flat in vec3 fNormal;
in vec4 fColor;
in float fOcclusion;
in float fDepth;

layout(location = 0) out vec4 fragColor;

vec3 lightDir = normalize(vec3(1, -10, -1));

void main() {
	vec3 rgb = clamp( fColor.xyz, vec3(0.0, 0.0, 0.0), vec3(1.0, 1.0, 1.0) );
	float da = 0.6;
	float di = clamp( dot( fNormal, -lightDir ), 0.0, 1.0 );
	float occFactor = clamp( fOcclusion, 0.0, 1.0 );
	float occ = clamp( (da + di) * occFactor, 0.0, 1.0 );

	vec4 fogColor = vec4(0.529, 0.808, 0.922, 1.0);
	float fog = smoothstep(200.0, 1000.0, fDepth);

	fragColor = vec4( occ * rgb, 1.0 );

	fragColor = mix(fragColor, fogColor, fog);
}
