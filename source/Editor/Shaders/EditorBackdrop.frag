#version 330 core

uniform vec3 uColor;

// Ouput data
layout(location = 0) out vec4 fragColor;

void main() {
	fragColor = vec4(uColor, 1);
}