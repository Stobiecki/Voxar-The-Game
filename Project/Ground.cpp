#pragma once
#define _CRT_SECURE_NO_WARNINGS
#include "Ground.h"

//#include <stdio.h>
//#include <stdlib.h>
//#include <iostream>
//#include <algorithm>
//
//#include <glew.h>
//#include <freeglut.h>
//
//#include <glm.hpp>
//#include <gtc/matrix_transform.hpp>
//#include <gtc/type_ptr.hpp>
//#include <gtx/normal.hpp>
//
//#include "stb_image.h"
//#include "obj_loader.hpp"
//#include "utilities.hpp"



CTriangle::CTriangle() { }
CTriangle::CTriangle(const glm::vec3& p1, const glm::vec3& p2, const glm::vec3& p3)
{
	p[0] = p1;
	p[1] = p2;
	p[2] = p3;

	calculateEquation();
}

// obliczanie wspolczynnikow rownania
void CTriangle::calculateEquation(void)
{
	glm::vec3 v1, v2;

	v1 = p[1] - p[0];
	v2 = p[2] - p[0];

	A = v1.y * v2.z - v1.z * v2.y;
	B = v1.z * v2.x - v1.x * v2.z;
	C = v1.x * v2.y - v1.y * v2.x;

	D = -(A * p[0].x + B * p[0].y + C * p[0].z);

	if (C == 0)
	{
		//printf("Uwaga! Trojkat pionowy.\n");
	}

	// wyliczeie nachylenie do osi X
	glm::vec3 normal = glm::triangleNormal(p[0], p[1], p[2]);
	angleToYAxis = glm::degrees(glm::acos(glm::dot(glm::normalize(normal), glm::vec3(0.0f, 1.0f, 0.0f))));

	//std::cout << "angle: " << angleToYAxis << std::endl;
}

// czy punkt p jest po lewej stronie odcinka (A, B)
// obliczanie wyznacznika
inline float CTriangle::det(glm::vec2 p, glm::vec3 A, glm::vec3 B)
{
	return (p.x - B.x) * (A.z - B.z) - (A.x - B.x) * (p.y - B.z);
}

// czy punkt jest wewnatrz trojkata rzutowanego na plaszczyzne xy
inline bool CTriangle::isInside(glm::vec2 point)
{
	float d1, d2, d3;
	bool isNeg, isPos;

	d1 = det(point, p[0], p[1]);
	d2 = det(point, p[1], p[2]);
	d3 = det(point, p[2], p[0]);

	// czy jeden z wyznacznikow ma znak ujemny
	isNeg = (d1 < 0) || (d2 < 0) || (d3 < 0);
	// czy jeden z wyznacznikow ma znak dodatni
	isPos = (d1 > 0) || (d2 > 0) || (d3 > 0);

	// czy sa dodatnie i ujemne wartosci wyznacznikow
	// jesli tak, wtedy punkt nie lezy wewnatrz trojkata
	if (isNeg && isPos)
		return false;
	else
		return true;
}

// obliczamy wysokosc punktu w trojkacie z rownania plaszczyzny
inline float CTriangle::calulateY(glm::vec2 point)
{
	if (B) return -(A * point.x + C * point.y + D) / B;
	else { return __ALTITUDE_ERROR; }
}


void Ground::Init()
{
	std::vector<glm::vec3> verts;
	std::vector<glm::vec2> GroundOBJ_uvs;
	std::vector<glm::vec3> GroundOBJ_normals;

	if (!GroundModelLoader("src/models/ground.obj", verts, GroundOBJ_uvs, GroundOBJ_normals))
	{
		printf("Not loaded!\n");
		exit(1);
	}

	glm::vec3 p1, p2, p3;

	std::vector<glm::vec3>::iterator it = verts.begin();
	while (it != verts.end())
	{
		p1 = *it++;
		p2 = *it++;
		p3 = *it++;

		// utworzenie nowego obiektu CTriangle
		triangles.push_back(CTriangle(p1, p2, p3));

	}

	printf("Utworzono Ground zawierajacy %d trojkatow.\n", triangles.size());
}

bool Ground::GroundModelLoader(
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

// ----------------------------------------
// Glowna funkcja obliczajaca wysokosci nad podlozem w punkcie X Z
// - przeszukanie po wszystkich trojkatach
// - gdy zostanie znaleziony trojkat, obliczana jest wysokosc Y
float Ground::getAltitute(glm::vec2 point, float currentY)
{
	static const float step_size = 1.0f;
	static const float streight_error = 0.2f;
	static const float fluctuation_error = 0.8f;

	static int z = 0;

	std::vector<float> planeAgnels;
	std::vector<float> possibleLevels;
	std::vector<float> distances;

	int pos = 0;
	for (auto it = triangles.begin(); it != triangles.end(); it++)
	{
		if ((*it).isInside(point))
		{
			planeAgnels.push_back(abs(it->angleToYAxis));
			possibleLevels.push_back(it->calulateY(point));
			distances.push_back(abs(currentY - possibleLevels[pos]));
			pos++;
		}
	}

	if (!possibleLevels.empty())
	{
		float currentlyChosen = __NEG_ALTITUDE_ERROR; // -MAX
		float currentlyChosenDistance = __ALTITUDE_ERROR; // MAX
		float currentlyChosenAngle = 0;

		if (possibleLevels.size() == 1)
		{
			if (distances[0] < step_size)
			{
				currentlyChosen = possibleLevels[0];
				currentlyChosenAngle = planeAgnels[0];
			}
		}
		else
		{
			int size = distances.size();
			for (int i = 0; i < size; i++)
			{
				if (distances[i] < streight_error)
				{
					currentlyChosen = possibleLevels[i];
					currentlyChosenDistance = distances[i];
					currentlyChosenAngle = planeAgnels[i];
				}
			}
			// going up/down
			if (currentlyChosen == __NEG_ALTITUDE_ERROR || size > 1)
			{
				std::vector<float> planeAgnelsUpDown;
				std::vector<float> possibleLevelsUpDown;
				std::vector<float> distancesUpDown;

				for (int i = 0; i < size; i++)
				{
					if (distances[i] < fluctuation_error)
					{
						currentlyChosen = possibleLevels[i];
						currentlyChosenDistance = distances[i];
						currentlyChosenAngle = planeAgnels[i];

						possibleLevelsUpDown.push_back(possibleLevels[i]);
						distancesUpDown.push_back(distances[i]);
						planeAgnelsUpDown.push_back(planeAgnels[i]);
					}
				}
				if (size = possibleLevelsUpDown.size(); size > 1)
				{
					auto maxElement = std::max_element(possibleLevelsUpDown.begin(), possibleLevelsUpDown.end());
					int id = std::distance(possibleLevelsUpDown.begin(), maxElement);
					currentlyChosen = possibleLevelsUpDown[id];
					currentlyChosenAngle = planeAgnelsUpDown[id];
				}
			}
		}

		//std::cout<< currentlyChosenAngle <<std::endl;

		if (currentlyChosen == __NEG_ALTITUDE_ERROR)
		{
			//std::cout << z;
			return __ALTITUDE_ERROR;
		}

		// angle limit
		//if (currentlyChosenAngle > 45.f)
		//	return __ALTITUDE_ERROR;

		return currentlyChosen;
	}
	else
	{
		//printf("Brak ziemi pod nogami!\n");
		return __ALTITUDE_ERROR;
	}
}


