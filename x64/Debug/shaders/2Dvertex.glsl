#version 330 core

// Wspolrzedne wierzcholkow
layout( location = 0 ) in vec4 inPosition;
layout( location = 1 ) in vec3 inColor;
layout( location = 2 ) in vec2 inUV;

// Macierz rzutowania
uniform mat4 matProj;
uniform mat4 matView;
uniform mat4 matModel;

out vec4 inoutPos;
out vec3 inoutColor;
out vec2 inoutUV;

void main()
{
	inoutPos = inPosition;
	inoutColor = inColor;
	inoutUV = inUV;
	
	gl_Position = matProj * matView * matModel * inPosition;
}
