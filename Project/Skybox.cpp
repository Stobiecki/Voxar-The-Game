#pragma once
#include "Skybox.h"

GLuint Skybox::SkyBox_VAO;
GLuint Skybox::SkyBox_Program;
GLuint Skybox::SkyBox_Texture;

GLfloat Skybox::positions[8 * 3] =
{
	1.0f, 1.0f, 1.0f,   // 0
	-1.0f, 1.0f, 1.0f,  // 1
	-1.0f, -1.0f, 1.0f, // 2
	1.0f, -1.0f, 1.0f,  // 3
	1.0f, 1.0f, -1.0f,  // 4
	-1.0f, 1.0f, -1.0f, // 5
	-1.0f, -1.0f, -1.0f,// 6
	1.0f, -1.0f, -1.0f  // 7
};

GLuint Skybox::indices[12 * 3] =
{
	5, 0, 1,
	5, 4, 0,
	2, 0, 3,
	2, 1, 0,
	7, 0, 4,
	7, 3, 0,
	3, 6, 2,
	3, 7, 6,
	1, 2, 6,
	1, 6, 5,
	4, 5, 6,
	4, 6, 7
};

const char Skybox::files[6][30] =
{
	"src/textures/skybox/xpos.png",
	"src/textures/skybox/xneg.png",
	"src/textures/skybox/ypos.png",
	"src/textures/skybox/yneg.png",
	"src/textures/skybox/zpos.png",
	"src/textures/skybox/zneg.png",
};

const GLenum Skybox::targets[6] =
{
	GL_TEXTURE_CUBE_MAP_POSITIVE_X,
	GL_TEXTURE_CUBE_MAP_NEGATIVE_X,
	GL_TEXTURE_CUBE_MAP_POSITIVE_Y,
	GL_TEXTURE_CUBE_MAP_NEGATIVE_Y,
	GL_TEXTURE_CUBE_MAP_POSITIVE_Z,
	GL_TEXTURE_CUBE_MAP_NEGATIVE_Z
};


glm::vec3 Skybox::skyboxScale = glm::vec3(400.0f, 400.0f, 400.0f);
glm::mat4 Skybox::matSkybox = glm::mat4(1);
float Skybox::lightDim = 1.0f;

void Skybox::CreateSkyBox()
{
	// Program
	SkyBox_Program = glCreateProgram();
	glAttachShader(SkyBox_Program, ShaderLoader::LoadShader(GL_VERTEX_SHADER, "shaders/vertexSkybox.glsl"));
	glAttachShader(SkyBox_Program, ShaderLoader::LoadShader(GL_FRAGMENT_SHADER, "shaders/fragmentSkybox.glsl"));
	ShaderLoader::LinkAndValidateProgram(SkyBox_Program);

	// Vertex arrays
	glGenVertexArrays(1, &SkyBox_VAO);
	glBindVertexArray(SkyBox_VAO);
	// Wspolrzedne wierzchokow
	GLuint vBuffer_pos;
	glGenBuffers(1, &vBuffer_pos);
	glBindBuffer(GL_ARRAY_BUFFER, vBuffer_pos);
	glBufferData(GL_ARRAY_BUFFER, 8 * 3 * sizeof(GLfloat), positions, GL_STATIC_DRAW);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, NULL);
	glEnableVertexAttribArray(0);
	// Tablica indeksow
	GLuint vBuffer_idx;
	glGenBuffers(1, &vBuffer_idx);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, vBuffer_idx);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, 12 * 3 * sizeof(GLuint), indices, GL_STATIC_DRAW);
	glBindVertexArray(0);


	// Tekstura CUBE_MAP
	glGenTextures(1, &SkyBox_Texture);
	glBindTexture(GL_TEXTURE_CUBE_MAP, SkyBox_Texture);

	// Wylaczanie flipowania tekstury
	stbi_set_flip_vertically_on_load(false);

	// Utworzenie 6 tekstur dla kazdej sciany
	for (int i = 0; i < 6; ++i)
	{
		int tex_width, tex_height, n;
		unsigned char* tex_data;

		tex_data = stbi_load(files[i], &tex_width, &tex_height, &n, 0);
		if (tex_data == NULL)
		{
			printf("Image %s can't be loaded!\n", files[i]);
			exit(1);
		}
		// Zaladowanie danych do tekstury OpenGL
		glTexImage2D(targets[i], 0, GL_RGB, tex_width, tex_height, 0, GL_RGB, GL_UNSIGNED_BYTE, tex_data);

		// Zwolnienie pamieci pliku graficznego
		stbi_image_free(tex_data);
	}

	// Przykladowe opcje tekstury
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_NEAREST_MIPMAP_LINEAR);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_REPEAT);
	glGenerateMipmap(GL_TEXTURE_CUBE_MAP);

	// Powrot Flipowanie tekstury
	stbi_set_flip_vertically_on_load(true);
}

void Skybox::DrawSkyBox(glm::mat4 matProj, glm::mat4 matView)
{
	// Specjalny potok dla SkyBoxa (uproszczony)
	// Ten program nie ma oswietlenia/cieni itp.
	glUseProgram(SkyBox_Program);

	// Przeskalowanie boxa i przesuniêcie skyboxa wzglêdem kamery
	RecalculatePosRelativeToCamera();

	// Obliczanie przyciemnianie œwiat³a
	CalculateLightDim();

	//przeslanie macierzy rzutowania
	// korzystajac z macierzy Proj i View naszej sceny
	glm::mat4 matPVM = matProj * matView * matSkybox;
	glUniformMatrix4fv(glGetUniformLocation(SkyBox_Program, "matPVM"), 1, GL_FALSE, glm::value_ptr(matPVM));

	// Aktywacja tekstury CUBE_MAP
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_CUBE_MAP, SkyBox_Texture);
	//static PointLightParam* l = dynamic_cast<PointLightParam*>(RenderData::LoadedLight["light1"].get());
	//glBindTexture(GL_TEXTURE_CUBE_MAP, l->idTextureCube);
	glUniform1i(glGetUniformLocation(SkyBox_Program, "tex_skybox"), 0);
	glUniform1f(glGetUniformLocation(SkyBox_Program, "globalLightYPos"), lightDim);

	// Rendering boxa
	glBindVertexArray(SkyBox_VAO);
	glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, NULL);
	glBindVertexArray(SkyBox_VAO);

	glUseProgram(0);
}

void Skybox::RecalculatePosRelativeToCamera()
{
	matSkybox = glm::mat4(1);
	matSkybox = glm::translate(matSkybox, glm::vec3(-Camera::CameraTranslate_x, -Camera::CameraTranslate_y, -Camera::CameraTranslate_z));
	matSkybox = glm::scale(matSkybox, Skybox::skyboxScale);
}

void Skybox::CalculateLightDim()
{
	float lightPos = RenderData::LoadedLight["global"].get()->Position.y;
	if (lightPos < 0)
		lightDim = -7.8125 * pow(lightPos, 5) - 20.8333 * pow(lightPos, 4) - 18.2292 * pow(lightPos, 3) - 5.41667 * pow(lightPos, 2) - 0.758333 * lightPos + 0.45;
	else
		lightDim = (-0.05 * lightPos) + 0.45;
}