#version 330 core

in vec2 vUV;

out vec4 color;

uniform sampler2D uTexture;
uniform vec4 uTransparentColor = vec4(0, 0, 0.4, 1);

void main()
{
    color = vec4(texture(uTexture, vUV));
	if (color == uTransparentColor)
	{
		discard;
	}
}