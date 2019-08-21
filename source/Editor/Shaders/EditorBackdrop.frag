#version 330 core

in vec3 fColor;

// Ouput data
layout(location = 0) out vec4 fragColor;

void main() {
   fragColor = vec4(fColor, 1);
}
