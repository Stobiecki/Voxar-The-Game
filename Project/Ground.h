#pragma once

#define __ALTITUDE_ERROR	99999
#define __NEG_ALTITUDE_ERROR -99999

#include <iostream>
#include <vector>
#include <map>
#include <fstream>

#include <glew.h>
#include <freeglut.h>
#include <glm.hpp>
#include <gtc/matrix_transform.hpp>
#include <glm/gtx/euler_angles.hpp>
#include <gtc/type_ptr.hpp>
#include <gtx/normal.hpp>


// -------------------------------
// Klasa reprezentujaca trojkat
class CTriangle
{
public:

	// wspolrzedne punktow trojkata
	glm::vec3 p[3];

	// rownanie plaszczyzny Ax + By + Cz + D = 0
	float A,B,C,D;
	
	// nachylenie do osi Y
	float angleToYAxis = 0.0f;

	// konstruktory
	CTriangle();
	CTriangle(const glm::vec3& p1, const glm::vec3& p2, const glm::vec3& p3);
	void calculateEquation(void);

	// czy punkt p jest po lewej stronie odcinka (A, B)
	// obliczanie wyznacznika
	inline float det(glm::vec2 p, glm::vec3 A, glm::vec3 B);

	// czy punkt jest wewnatrz trojkata rzutowanego na plaszczyzne xy
	inline bool isInside(glm::vec2 point);

	// obliczamy wysokosc punktu w trojkacie z rownania plaszczyzny
	inline float calulateY(glm::vec2 point);

};



// ---------------------------------------
// Klasa reprezentujaca podloze
// ---------------------------------------
class Ground
{
public:

	std::vector<CTriangle> triangles;
	void Init();
	bool GroundModelLoader(const char* path, std::vector<glm::vec3>& out_vertices, std::vector<glm::vec2>& out_uvs, std::vector<glm::vec3>& out_normals);

	// ----------------------------------------
	// Glowna funkcja obliczajaca wysokosci nad podlozem w punkcie X Z
	// - przeszukanie po wszystkich trojkatach
	// - gdy zostanie znaleziony trojkat, obliczana jest wysokosc Y
	float getAltitute(glm::vec2 point, float currentY);
};
