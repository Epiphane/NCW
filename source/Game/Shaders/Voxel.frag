#version 330 core

// Value from the geometry shader.
flat in vec4 fColor;

// Ouput data
out vec4 fragColor;

void main() {
   fragColor = fColor;
}