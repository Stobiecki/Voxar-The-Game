#pragma once

#include <glm.hpp>
#include <glew.h>
#include <freeglut.h>
#include "Utilities.h"

class Skybox
{
public:
	static GLuint SkyBox_VAO;
	static GLuint SkyBox_Program;
	static GLuint SkyBox_Texture;

	static GLfloat positions[8*3];
	static GLuint indices[12*3];
	static const char files[6][30];
	static const GLenum targets[6];

	static glm::vec3 skyboxScale;
	static glm::mat4 matSkybox;

	static float lightDim;

	static void CreateSkyBox();
	static void DrawSkyBox(glm::mat4 matProj, glm::mat4 matView);
	static void RecalculatePosRelativeToCamera();
	static void CalculateLightDim();
};

