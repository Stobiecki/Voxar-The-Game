#pragma once

#include <iostream>
#include <vector>
#include <map>
#include <fstream>
#include <functional>
#include <string>
#include <memory>

#include <glew.h>
#include <freeglut.h>
#include <glm.hpp>
#include <gtc/matrix_transform.hpp>
#include <glm/gtx/euler_angles.hpp>
#include <gtc/type_ptr.hpp>

#include "Utilities.h"

# define __CHECK_FOR_ERRORS { GLenum errCode ; if (( errCode = glGetError ()) != GL_NO_ERROR) printf(" Error (\% d): \%s in file \%s at line \%d !\n", errCode, gluErrorString(errCode), __FILE__, __LINE__); }


using std::vector;
using std::string;
using std::cout;
using std::endl;

class LightParam;

class Light
{
private:
    static void LoadLightingInfoFromFile(std::ifstream& ifs);
public:
    static bool lightMode;
    static bool shaderMode; // vertex = 0, frament = 1;
    static void InitLightData();
    static std::map<std::string, std::unique_ptr<LightParam>>* LoadedLight;
    static void SendLightParam();
};


class LightParam
{
private:
    //static LightParam defaultLight;
public:
    static LightParam defaultLight;
    LightParam() = default;
    LightParam(std::string _name, glm::vec3 _pos);
    LightParam(std::string _name, glm::vec3 _ambient, glm::vec3 _diffuse, glm::vec3 _specular, glm::vec3 _attenuation, glm::vec3 _position);
    virtual void SendLightParam(GLuint program, int idInArray, int id);
    std::string name;
    glm::vec3 Ambient;
    glm::vec3 Diffuse;
    glm::vec3 Specular;
    glm::vec3 Attenuation;
    glm::vec3 Position; // Direction dla kierunkowego
    bool IsTurnedOn = true;
};

class PointLightParam : public LightParam
{
public:
    // Identyfikatory obiektow
    GLuint idProgram;			// program generujacy mape cieni
    GLuint idTextureCube;		// tekstura
    GLuint idFrameBuffer[6];	// 6 x FBO

    // Rozmiar mapy cieni
    int DepthMap_Width = 1024 * 2;
    int DepthMap_Height = 1024 * 2;

    // Bryla obcinania (frustum)
    float frustumNear = 0.0001f;
    float frustumFar = 30.0f;
    // macierz projection oraz view
    // do renderingu kazdej z szesciu scian
    glm::mat4 matProj;
    glm::mat4 matViews[6];

    void SendLightParam(GLuint idProgram, int idInArray, int id) override;
    static void ShadowMapPointInit();
    static void RenderSceneToShadowCubeMap();
    void UpdateViewMat();
    void BeginRenderOfShadowMapFace(int face);
    static void RenderRenderOfShadowMapFaces();

    PointLightParam() = default;
    PointLightParam(std::string _name, glm::vec3 _pos);
    PointLightParam(std::string _name, glm::vec3 _ambient, glm::vec3 _diffuse, glm::vec3 _specular, glm::vec3 _attenuation, glm::vec3 _position);
};

class GlobalLightParam : public LightParam
{
public:
    // Identyfikatory obiektow
    GLuint DepthMap_idProgram;
    GLuint DepthMap_idFrameBuffer;
    GLuint DepthMap_idTexture;

    // Rozdzielczosc depth mapy
    static int DepthMap_Width;
    static int DepthMap_Height;

    static float Ortholeft;
    static float OrthoRight;
    static float OrthoBottom;
    static float OrthoTop;
    static float OrthoNear;
    static float OrthoFar;

    static float shadowBias;
    static float biasMultiplier;

    // Parametry swiatla kierunkowego, ale znacznie lepiej ubrac
    // to w strukture, ktora uzywalismy podczas zajec z oswietlenia
    glm::vec3 Light_Direction;
    glm::vec3 Light_Position;

    // Macierze rzutowania dla kamery patrzacej z punktu widzenia oswietlenia
    glm::mat4 lightProj;
    glm::mat4 lightView;

    // Pozycja kamery potrzebna do oswietlenia
    glm::vec3 cameraPos;

    static GlobalLightParam* globalLight;

    void SendLightParam(GLuint idProgram, int idInArray, int id) override;
    GlobalLightParam() = default;
    GlobalLightParam(std::string _name, glm::vec3 _pos);
    GlobalLightParam(std::string _name, glm::vec3 _ambient, glm::vec3 _diffuse, glm::vec3 _specular, glm::vec3 _attenuation, glm::vec3 _position);

    static void ShadowMapDirInit();
    static void ShadowMapDirUpdate();
    static void RenderSceneToShadowMap();
};

class RelativeLightParam : public LightParam
{
public:
    void SendLightParam(GLuint idProgram, int idInArray, int id) override;
    RelativeLightParam() = default;
    RelativeLightParam(std::string _name, glm::vec3 _pos);
    RelativeLightParam(std::string _name, glm::vec3 _ambient, glm::vec3 _diffuse, glm::vec3 _specular, glm::vec3 _attenuation, glm::vec3 _position);
};