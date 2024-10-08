#version 330 core
layout (location = 0) in vec4 inPosition;

uniform mat4 matProj;
uniform mat4 matView;
uniform mat4 matModel;

out vec3 fragPos;

void main()
{
	fragPos = vec3(matModel * inPosition);
    gl_Position = matProj * matView * matModel * inPosition;
}
