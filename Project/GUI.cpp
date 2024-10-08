#pragma once
#include "GUI.h"

// Crosshair
void Crosshair::InitCrossroad(ProgramID& program)
{
	stbi_set_flip_vertically_on_load(true);
	glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

	texData = stbi_load("src/textures/crosshair.png", &texWidth, &texHeigh, &texN, 0);
	if (texData == NULL)
		cout << "\ncrosshair can't be loaded!";
	else
		cout << "\ncrosshair loaded!";

	vert.push_back(glm::vec2(0.05, 0.05));
	vert.push_back(glm::vec2(0.05, -0.05));
	vert.push_back(glm::vec2(-0.05, -0.05));
	vert.push_back(glm::vec2(-0.05, -0.05));
	vert.push_back(glm::vec2(-0.05, 0.05));
	vert.push_back(glm::vec2(0.05, 0.05));

	text.push_back(glm::vec2(1.0, 1.0));
	text.push_back(glm::vec2(1.0, 0.0));
	text.push_back(glm::vec2(0.0, 0.0));
	text.push_back(glm::vec2(0.0, 0.0));
	text.push_back(glm::vec2(0.0, 1.0));
	text.push_back(glm::vec2(1.0, 1.0));

	glGenTextures(1, &program.idTexture);
	glBindTexture(GL_TEXTURE_2D, program.idTexture);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, texWidth, texHeigh, 0, GL_RGBA, GL_UNSIGNED_BYTE, texData);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
}

// GUI
GUI GUI::gui = GUI();

void GUI::RenderAll()
{
	glUseProgram(gui.program.idProgram);
	glUniform1f(glGetUniformLocation(gui.program.idProgram, "scale"), (1.f * glutGet(GLUT_WINDOW_HEIGHT) / glutGet(GLUT_WINDOW_WIDTH)));
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, gui.program.idTexture);
	glUniform1i(glGetUniformLocation(gui.program.idProgram, "tex"), 0);
	glBindVertexArray(gui.program.idVAO);
	glDrawArrays(GL_TRIANGLES, 0, gui.crosshair.vert.size());
	glBindVertexArray(0);
	glUseProgram(0);
}

void GUI::InitGuiInfo()
{
	gui.program.idProgram = glCreateProgram();
	gui.crosshair.InitCrossroad(gui.program);
	glGenVertexArrays(1, &gui.program.idVAO);

	glBindVertexArray(gui.program.idVAO);
	glGenBuffers(1, &gui.program.idVBO_coord);
	glBindBuffer(GL_ARRAY_BUFFER, gui.program.idVBO_coord);
	glBufferData(GL_ARRAY_BUFFER, gui.crosshair.vert.size() * sizeof(glm::vec2), &(gui.crosshair.vert[0]), GL_STATIC_DRAW);
	glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 0, NULL);
	glEnableVertexAttribArray(0);

	glGenBuffers(1, &(gui.program.idVBO_uv));
	glBindBuffer(GL_ARRAY_BUFFER, gui.program.idVBO_uv);
	glBufferData(GL_ARRAY_BUFFER, gui.crosshair.text.size() * sizeof(glm::vec2), &(gui.crosshair.text[0]), GL_STATIC_DRAW);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 0, NULL);
	glEnableVertexAttribArray(1);

	glBindVertexArray(0);

	glAttachShader(gui.program.idProgram, ShaderLoader::LoadShader(GL_VERTEX_SHADER, "shaders/vertexGUI.glsl"));
	glAttachShader(gui.program.idProgram, ShaderLoader::LoadShader(GL_FRAGMENT_SHADER, "shaders/fragmentGUI.glsl"));

	ShaderLoader::LinkAndValidateProgram(gui.program.idProgram);

}

void GUI::freeData()
{
	stbi_image_free(gui.crosshair.texData);
	glDeleteProgram(gui.program.idProgram);
	glDeleteVertexArrays(1, &(gui.program.idVBO_coord));
	glDeleteVertexArrays(1, &(gui.program.idVBO_color));
	glDeleteVertexArrays(1, &(gui.program.idVBO_uv));
	glDeleteVertexArrays(1, &(gui.program.idVAO));
}
