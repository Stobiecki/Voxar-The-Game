#pragma once

#include <iostream>
#include <vector>
#include <map>
#include <fstream>
#include <functional>
#include <sstream>
#include <memory>
#include <chrono>

#include <glew.h>
#include <freeglut.h>
#include <glm.hpp>
#include <gtc/matrix_transform.hpp>
#include <glm/gtx/euler_angles.hpp>
#include <gtc/type_ptr.hpp>

#include "Model.h"
#include "Camera.h"
#include "Light.h"
#include "Skybox.h"
#include "RenderData.h"
#include "Audio.h"

#include "Player.h"
#include "Ground.h"

#include "stb_image.h"
#include <SFML/Audio.hpp>


// all the final data needed
class LightParam;
class Model;
class VertTextNorm;
class ProgramID;
class TexturesData;

bool loadOBJ(const char* path, std::vector<glm::vec3>& out_vertices, std::vector<glm::vec2>& out_uvs, std::vector<glm::vec3>& out_normals);

class ShaderLoader
{
private:
    static void LoadShadersInfoFromFile(std::ifstream& ifs);

public:
    static void InitShadersData();
    static void LinkAndValidateProgram(GLuint program);
    static void CheckForErrors_Program(GLuint program, GLenum mode);
    static unsigned long getFileLength(std::ifstream& file);
    static GLchar* LoadShaderFile(const char* filename);
    static void CheckForErrors_Shader(GLuint shader);
    static GLuint LoadShader(GLuint MODE, const char* filename);
};

class DitheringFBO
{
public:

    enum {
        SCENE = 0,
        SCREEN,
        NUMBER_OF_OBJECTS,
    };


    static bool isTurnedOn;

    static GLuint idProgram[NUMBER_OF_OBJECTS];
    static GLuint idVAO[NUMBER_OF_OBJECTS];
    //static GLuint idVAOpos[NUMBER_OF_OBJECTS];
    //static GLuint idVAOuv[NUMBER_OF_OBJECTS];
    static GLuint idTexture[NUMBER_OF_OBJECTS];

    static GLuint idFrameBuffer;    // FBO
    static GLuint idDepthBuffer;    // Bufor na skladowa glebokosci
    static GLuint idTextureBuffer;  // Tekstura na skladowa koloru

    static int buffer_Width;
    static int buffer_Height;

    static float windowScale;

    static std::vector<GLfloat> vertices_pos;
    static std::vector<GLfloat> vertices_tex;

    static void InitFBO();
    static void RenderToTexture();
    static void RenderOnScreen();
};

class CubeMapDisplay
{
public:
    static bool alreadyInitialized;
    static bool depthOnly;

    static GLuint *idFrameBuffer;  //unused
    static GLuint *idTextureCube;

    static GLuint CubeMap_VAO;
    static GLuint CubeMap_Program;

    static GLfloat positions[8 * 3];
    static GLuint indices[12 * 3];
    static const GLenum targets[6];

    static glm::mat4 matCubeMap;
    static int Width;
    static int Height;

    CubeMapDisplay() = delete;

    static void CubeMapDisplayInit();
    static void SetCubeMap(GLuint *_idTextureCube, int _Width, int _Height, bool depthOnly);
    static void Render(glm::mat4 matProj, glm::mat4 matView);
};

class StaticData
{
public:
    static constexpr float one_degree = 3.1415f / 180.f;
    static int windowWidth;
    static int windowHeight;

    // mist
    static bool mistOn;
    static glm::vec3 mistShade;
    static float mistDensity;

    // shadow maps
    static bool globalLightShadowMapOn;
    static bool shadowMap3DOn;

    static Ground myGround;
    static Player myPlayer;

    static bool endGameTimerOn;

    static void EndGameSequence();
};