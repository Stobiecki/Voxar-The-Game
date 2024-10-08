#pragma once

#include <iostream>

#include <glew.h>
#include <freeglut.h>
#include <glm.hpp>
#include <gtc/matrix_transform.hpp>
#include <glm/gtx/euler_angles.hpp>
#include <gtc/type_ptr.hpp>

class Camera
{
public:
    static bool ImGuiOn;

    // Zmienne do kontroli obrotu kamery
    static GLfloat CameraRotate_x;
    static GLfloat CameraRotate_y;
    // Zmienne do kontroli pozycji kamery
    static GLfloat CameraTranslate_x;
    static GLfloat CameraTranslate_y;
    static GLfloat CameraTranslate_z;
    // D³ugoœæ bry³y obcinania
    static double lengthOfCameraView;

    static glm::vec3 cameraPos;

    static glm::vec3 ExtractCameraPos(const glm::mat4& a_modelView);
};