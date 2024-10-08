#include "Camera.h"

// Camera
bool Camera::ImGuiOn = false;
GLfloat Camera::CameraRotate_x = 0.0f;
GLfloat Camera::CameraRotate_y = 0.0f;
GLfloat Camera::CameraTranslate_x = 0.0f;
GLfloat Camera::CameraTranslate_y = 0.0f;
GLfloat Camera::CameraTranslate_z = -2.0f;
double Camera::lengthOfCameraView = 1000.0;

glm::vec3 Camera::cameraPos = glm::vec3(0.0f);

glm::vec3 Camera::ExtractCameraPos(const glm::mat4& a_modelView)
{
	glm::mat4 modelViewT = transpose(a_modelView);

	// Get plane normals
	glm::vec3 n1(modelViewT[0]);
	glm::vec3 n2(modelViewT[1]);
	glm::vec3 n3(modelViewT[2]);

	// Get plane distances
	float d1(modelViewT[0].w);
	float d2(modelViewT[1].w);
	float d3(modelViewT[2].w);

	// Get the intersection of these 3 planes
	glm::vec3 n2n3 = cross(n2, n3);
	glm::vec3 n3n1 = cross(n3, n1);
	glm::vec3 n1n2 = cross(n1, n2);

	glm::vec3 top = (n2n3 * d1) + (n3n1 * d2) + (n1n2 * d3);
	float denom = dot(n1, n2n3);

	return top / -denom;
}