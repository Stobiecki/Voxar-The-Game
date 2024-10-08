#pragma once

#define _CRT_SECURE_NO_WARNINGS
#include "Player.h"

// ----------------------------------------------
// Klasa do reprezentacji postaci
// - obiektu, ktory porusza sie po scenie
// ----------------------------------------------

Player::Player() 
{
	walkingSpeed = 0.16f;
	runningSpeed = 0.4f;
	movementSpeed = 0.0f;
	//Position = glm::vec3(-18.337f, -3.37499, -19.6992f);
	height = 1.72f;
	//Position = glm::vec3(0.0f, 0.8f, 0.0f);
	Position = glm::vec3(13.8f, -2.73f, 20.93f);
	Angle = 0.0;
	DirectionFront = glm::vec3(1.0, 0.0, 0.0);
	DirectionSide = glm::vec3(0.0, 0.0, 1.0);
	matModel = glm::mat4(1.0);
	myGround = nullptr;
}

void Player::Setup(std::string name)
{
	// VAO z pliku OBJ
	if (!PlayerModelLoader(name.c_str(), PlayerOBJ_vertices, PlayerOBJ_uvs, PlayerOBJ_normals))
	{
		printf("Not loaded!\n");
		exit(1);
	}

	glGenVertexArrays(1, &idVAO);
	glBindVertexArray(idVAO);
	GLuint vBuffer_pos, vBuffer_uv;
	glGenBuffers(1, &vBuffer_pos);
	glBindBuffer(GL_ARRAY_BUFFER, vBuffer_pos);
	glBufferData(GL_ARRAY_BUFFER, PlayerOBJ_vertices.size() * sizeof(glm::vec3), &(PlayerOBJ_vertices)[0], GL_STATIC_DRAW);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, NULL);
	glEnableVertexAttribArray(0);

	glGenBuffers(1, &vBuffer_uv);
	glBindBuffer(GL_ARRAY_BUFFER, vBuffer_uv);
	glBufferData(GL_ARRAY_BUFFER, PlayerOBJ_uvs.size() * sizeof(glm::vec2), &(PlayerOBJ_uvs)[0], GL_STATIC_DRAW);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 0, NULL);
	glEnableVertexAttribArray(1);
	glBindVertexArray(0);
}

bool Player::PlayerModelLoader(
	const char* path,
	std::vector<glm::vec3>& out_vertices,
	std::vector<glm::vec2>& out_uvs,
	std::vector<glm::vec3>& out_normals
)
{
	//printf("Loading OBJ file %s ... ", path);

	std::vector<unsigned int> vertexIndices, uvIndices, normalIndices;
	std::vector<glm::vec3> temp_vertices;
	std::vector<glm::vec2> temp_uvs;
	std::vector<glm::vec3> temp_normals;


	FILE* file = fopen(path, "r");
	if (file == NULL) {
		printf("can't open the file.\n");
		return false;
	}


	while (1)
	{

		char lineHeader[128];

		// read the first word of the line
		int res = fscanf(file, "%s", lineHeader);

		if (res == EOF)
			break; // EOF = End Of File. Quit the loop.

		// read vertex coordinates
		if (strcmp(lineHeader, "v") == 0)
		{
			glm::vec3 vertex;
			fscanf(file, "%f %f %f\n", &vertex.x, &vertex.y, &vertex.z);
			temp_vertices.push_back(vertex);

		}
		// read texture coordinates
		else if (strcmp(lineHeader, "vt") == 0)
		{
			glm::vec2 uv;
			fscanf(file, "%f %f\n", &uv.x, &uv.y);
			temp_uvs.push_back(uv);
		}
		// read normal vectors
		else if (strcmp(lineHeader, "vn") == 0)
		{
			glm::vec3 normal;
			fscanf(file, "%f %f %f\n", &normal.x, &normal.y, &normal.z);
			temp_normals.push_back(normal);
		}
		// read faces (triangles)
		else if (strcmp(lineHeader, "f") == 0)
		{
			std::string vertex1, vertex2, vertex3;
			unsigned int vertexIndex[3], uvIndex[3], normalIndex[3];
			int matches = fscanf(file, "%d/%d/%d %d/%d/%d %d/%d/%d\n", &vertexIndex[0], &uvIndex[0], &normalIndex[0],
				&vertexIndex[1], &uvIndex[1], &normalIndex[1], &vertexIndex[2], &uvIndex[2], &normalIndex[2]);

			if (matches != 9)
			{
				printf("\nFile can't be read by our simple parser. ");
				printf("Try exporting with other options and make sure you export normals and uvs.\n");
				fclose(file);
				return false;
			}

			// Create new triangle
			vertexIndices.push_back(vertexIndex[0]);
			vertexIndices.push_back(vertexIndex[1]);
			vertexIndices.push_back(vertexIndex[2]);
			uvIndices.push_back(uvIndex[0]);
			uvIndices.push_back(uvIndex[1]);
			uvIndices.push_back(uvIndex[2]);
			normalIndices.push_back(normalIndex[0]);
			normalIndices.push_back(normalIndex[1]);
			normalIndices.push_back(normalIndex[2]);

		}
		else
		{
			// Probably a comment, eat up the rest of the line
			char stupidBuffer[1000];
			fgets(stupidBuffer, 1000, file);
		}

	}

	// For each vertex of each triangle
	for (unsigned int i = 0; i < vertexIndices.size(); i++) {

		// Get the indices of its attributes
		unsigned int vertexIndex = vertexIndices[i];
		unsigned int uvIndex = uvIndices[i];
		unsigned int normalIndex = normalIndices[i];

		// Get the attributes thanks to the index
		glm::vec3 vertex = temp_vertices[vertexIndex - 1];
		glm::vec2 uv = temp_uvs[uvIndex - 1];
		glm::vec3 normal = temp_normals[normalIndex - 1];

		// Put the attributes in buffers
		out_vertices.push_back(vertex);
		out_uvs.push_back(uv);
		out_normals.push_back(normal);

	}
	fclose(file);
	//printf(" done.\n");
	return true;
}

// Inicjalizacja obiektu
void Player::Init(Ground *ground)
{
	myGround = ground;

	glm::vec2 pos = glm::vec2{ Position.x, Position.z };
	Position.y = myGround->getAltitute(pos, Position.y);

	// Aktualizacja polozenia/macierzy itp.
	Player::Update();

}

// renderowanie obiektu
void Player::Draw()
{
	//// pobieranie aktualnego potoku
	//GLint idProgram;
	//glGetIntegerv(GL_CURRENT_PROGRAM, &idProgram);

	//// TO_DO: a moze wykorzystac metode Draw() z klasy CMesh?
	//glBindVertexArray(idVAO[LEGO]);
	//glUniformMatrix4fv(glGetUniformLocation(idProgram, "matModel"), 1, GL_FALSE, glm::value_ptr(this->matModel));
	//glBindTexture(GL_TEXTURE_2D, idTexture[LEGO]);
	//glDrawArrays(GL_TRIANGLES, 0, OBJ_vertices[LEGO].size());
	//glBindVertexArray(0);
}


// Obliczenie wysokosci nad ziemia
void Player::Update()
{
	Position.y = myGround->getAltitute( glm::vec2{ Position.x, Position.z }, Position.y);

	matModel = glm::translate(glm::mat4(1.0), Position);
	matModel = glm::rotate(matModel, Angle, glm::vec3(0.0, 1.0, 0.0));
}

void Player::UpdateForRotation()
{
	matModel = glm::translate(glm::mat4(1.0), Position);
	matModel = glm::rotate(matModel, Angle, glm::vec3(0.0, 1.0, 0.0));
}

// ustawienie pozycji na scenie
void Player::SetPosition(glm::vec3 pos)
{
	Position = pos;
	Update();
}

// próba wykonania ruchu w dan¹ stronê (sprawdzanie czy nie jest zbyt stromo + czy s¹ trójk¹ty)
// true  == mo¿na wykonaæ ruch
// false == nie mo¿na wykonaæ ruch
bool Player::TryMove(float valFront, float valSide)
{
	glm::vec3 tempPosition = Position + (DirectionFront * valFront) + (DirectionSide * valSide);
	tempPosition.y = myGround->getAltitute(glm::vec2{ tempPosition.x, tempPosition.z }, tempPosition.y);
	if (tempPosition.y != __ALTITUDE_ERROR)
		return true;
	else
		return false;
}

// zmiana pozycji na scenie
void Player::Move(float valFront, float valSide)
{
	Position += (DirectionFront * valFront) + (DirectionSide * valSide);

	// aktualizacja
	Update();
}

// zmiana orientacji obiektu
void Player::Rotate(float angle)
{
	Angle = -angle + 1.570796f;	// 2*pi/4 -> pi/2 = 90 stopni
	DirectionFront.x = cos(Angle);
	DirectionFront.z = -sin(Angle);

	DirectionSide.x = cos(Angle + 1.570796f);
	DirectionSide.z = -sin(Angle + 1.570796f);

	// aktualizacja
	UpdateForRotation();
}

