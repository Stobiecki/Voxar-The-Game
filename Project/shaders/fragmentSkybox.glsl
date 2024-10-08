#version 150 core

in vec4 fragPos;
out vec4 outColor;

uniform float globalLightYPos = 1.0f;
uniform samplerCube tex_skybox;

void main()
{
	vec4 textured = texture(tex_skybox, fragPos.xyz);
	outColor = textured * globalLightYPos;
}
