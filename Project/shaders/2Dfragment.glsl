#version 150 core

// zmienne wejsciowe
in vec4 inoutPos;
in vec3 inoutColor;
in vec2 inoutUV;

// uchwyt tekstury
uniform sampler2D tex;

// zmienna wyjsciowa
out vec4 outColor;


void main()
{
	// pobranie wartosci tekstury dla zadanej
	// wspolrzednej UV

	vec4 texColor = texture(tex, inoutUV);
	//texColorOut = mix(texColor1, texColor2, 0.5);

	if(texColor.a < 0.5f)
		discard;
	
	outColor = texColor;
}
