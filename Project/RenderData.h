#pragma once
#include <iostream>
#include <vector>
#include <map>
#include <memory>

#include <glew.h>
#include <freeglut.h>
#include <glm.hpp>

#include "Model.h"
#include "Camera.h"
#include "Light.h"

// all the final data needed
class LightParam;
class Model;
class VertTextNorm;
class ProgramID;
class TexturesData;

class RenderData
{
public:
    static void FreeData();
    static void FreeTexturesData();
    static void RenderAll();


    static std::map<std::string, VertTextNorm> InfoForModels;
    static std::map<std::string, ProgramID*> Textures;          // texture points to specific set of program with id of texture
    static std::map<std::string, ProgramID> Programs;
    static std::map<std::string, std::unique_ptr<Model>> Models;
    static std::map<std::string, std::string> VertexShaders;
    static std::map<std::string, std::string> FragmentShaders;
    static std::map<std::string, TexturesData*> LoadedTextures; // data might be freed so it could be empty
    static std::map<std::string, std::unique_ptr<LightParam>> LoadedLight;


    static glm::mat4 matProj;
    static glm::mat4 matView;
    static GLfloat cameraZoom;
};