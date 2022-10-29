#version 400 core

in vec2 textureCoordinates;

out vec4 FragColor;
uniform sampler2D u_texture;


void main()
{
	vec4 outTex = texture(u_texture, textureCoordinates);

	if(outTex.a <= 0.5)
		discard;

	FragColor = outTex;
}