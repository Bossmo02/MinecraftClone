#version 400 core

in vec2 textureCoordinates;
in float lightLevel;

out vec4 FragColor;
uniform sampler2D u_texture;


void main()
{
	vec4 outTex = texture(u_texture, textureCoordinates);

	FragColor = outTex * vec4(lightLevel, lightLevel, lightLevel, 1.0);
}