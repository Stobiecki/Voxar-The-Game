#version 330 core

in vec4 inPosition;
in vec2 inUV;

out vec2 fragUV;
out vec2 fragPos;

uniform float scale;

void main()
{
	fragUV = inUV;
	vec4 finalPos = inPosition;
	finalPos.z = -1.0;
	//fragPos = inPosition.xy * scale;

	finalPos.x *= scale;
	gl_Position = finalPos;
}
