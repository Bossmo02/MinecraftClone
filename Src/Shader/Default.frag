#version 400 core

in vec2 textureCoordinates;
in float lightLevel;
in float debugHighlight;

out vec4 FragColor;

uniform sampler2D u_texture;


void main()
{
	vec4 outTex;

	if(debugHighlight > 0.5f)
		outTex = texture(u_texture, textureCoordinates) * vec4(1.5, 0.4, 0.4, 1.0);
	else
		outTex = texture(u_texture, textureCoordinates);


	if(outTex.a <= 0.5)
		discard;

	FragColor = outTex * vec4(lightLevel, lightLevel, lightLevel, 1.0);
}