#version 330 core

layout( location = 0 ) in vec4 inPosition;
layout( location = 1 ) in vec2 inUV;

out vec4 inoutPos;
out vec2 inoutUV;

void main()
{
	vec4 finalPos = inPosition;
	finalPos.z -= 0.95f;

	inoutUV = inUV;
	inoutPos = finalPos;
	gl_Position = finalPos;
}
