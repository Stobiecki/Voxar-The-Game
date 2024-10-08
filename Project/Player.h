#pragma once

#include <iostream>
#include <vector>
#include <map>
#include <fstream>

#include "Ground.h"

#include <glew.h>
#include <freeglut.h>
#include <glm.hpp>
#include <gtc/matrix_transform.hpp>
#include <glm/gtx/euler_angles.hpp>
#include <gtc/type_ptr.hpp>

class Player
{
public:
	float walkingSpeed;
	float runningSpeed;
	float movementSpeed;

	float height;

	glm::vec3 Position;

	float Angle;

	glm::vec3 DirectionFront;
	glm::vec3 DirectionSide;

	GLuint idVAO;

	glm::mat4x4 matModel;

	std::vector<glm::vec3> PlayerOBJ_vertices;
	std::vector<glm::vec2> PlayerOBJ_uvs;
	std::vector<glm::vec3> PlayerOBJ_normals;

	Ground* myGround;

	Player();

	void Setup(std::string name);
	static bool PlayerModelLoader(const char* path, std::vector<glm::vec3>& out_vertices, std::vector<glm::vec2>& out_uvs, std::vector<glm::vec3>& out_normals);
	void Init(Ground* ground);
	void Draw();
	void Update();
	void UpdateForRotation();
	void SetPosition(glm::vec3 pos);
	bool TryMove(float valFront, float valSide);
	void Move(float valFront, float valSide);
	void Rotate(float angle);
};


