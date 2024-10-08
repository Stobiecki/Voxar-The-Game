#version 150 core

// Kolor ostateczny
out vec4 outColor;

// Dane wejsciowe z shadera wierzcholkow
in vec2 fragUV;
in vec2 fragPos;

// Tekstura z aplikacji
uniform sampler2D tex;


void main()
{
	vec4 texColor = texture(tex, fragUV);

	texColor.a *= 0.65; 

	texColor.xyz *= 0.7;

	outColor = texColor;
	//outColor.rg = fragPos;
}
