#version 150 core

in vec4 fragPos;
out vec4 outColor;

uniform samplerCube texCubeMap;
uniform int depthOnly = 0;

void main()
{
	vec4 textured = texture(texCubeMap, fragPos.xyz);
	if(depthOnly==1)
	{
		textured = vec4(textured.r, textured.r, textured.r, textured.a);
		if(textured.r >=0.99f)
			discard;
	}
	outColor = textured;
}
