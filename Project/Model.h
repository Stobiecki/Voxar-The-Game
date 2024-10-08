#pragma once

#include <iostream>
#include <vector>
#include <map>
#include <fstream>

#include "light.h"
#include "Utilities.h"
#include "obj_loader.hpp"

#include <glew.h>
#include <freeglut.h>
#include <glm.hpp>
#include <gtc/matrix_transform.hpp>
#include <glm/gtx/euler_angles.hpp>
#include <gtc/type_ptr.hpp>

class Model;

class ProgramID
{
public:
    GLuint idProgram;	// programu
    GLuint idVAO;		// tablic wierzcholkow
    GLuint idVBO_coord;	// id bufora wspolrzednych
    GLuint idVBO_color; // id bufora na kolory
    GLuint idVBO_uv;    // id bufora textury
    GLuint idTexture;   // id textury
    //ProgramID();
};

class VertTextNorm
{
public:
    std::string name;
    std::vector<glm::vec3> vert;
    std::vector<glm::vec2> texture;
    std::vector<glm::vec3> normals;
};

struct MaterialParam
{
public:
    glm::vec3 Ambient;
    glm::vec3 Diffuse;
    glm::vec3 Specular;
    float Shininess;
};

extern MaterialParam myMaterial;


class TexturesData
{
public:
    std::string texturedModel;
    ProgramID* programPtr;
    std::string texName;
    int texWidth;
    int texHeigh;
    int texN;
    unsigned char* texData;
    MaterialParam parametersData;

    static std::map<std::string, TexturesData*>* LoadedTextures;

    TexturesData() = default;
    TexturesData(std::string texturedModel, ProgramID* programPtr, std::string _texName, int _texWidth, int _texHeigh, int _texN, unsigned char* _texData);
    static void FreeAllTexturesDataAfterLinking();
    static void SendMaterialParam(Model* m);
};

class ModelInstanceInfo;

class Model
{
private:
    static void LoadModelsInfoFromFile(std::ifstream& ifs);
    static void LoadModelsPlacementInfoFromFile(std::ifstream& ifs);

    //enum class typeOfModel
    //{
    //    StandardModel_,
    //    TransparentModel_,
    //    ReflectiveModel_,
    //    TransparentAndReflectiveModel_,
    //    _2DModel_,
    //    _2DCrossModel_,
    //    _Animated3DModel
    //};
    //friend typeOfModel;

public:
    Model() = default;
    Model(std::string _name, std::string _fileName, std::string _textureName, std::string _programName, std::string _vertexShader, std::string _vertexFragment);

    void LoadTextures(std::string _name, std::string _fileName, std::string _textureName, std::string _programName, std::string _vertexShader, std::string _vertexFragment);
    static void InitAllModels();
    virtual void InitModel();
    virtual void Render();
    static void InitModelsData();
    static void InitModelPlacement();
    void LoadMtlFile(std::string modelName);

    std::string name;
    std::string fileName;
    std::string textureName;
    std::string programName;
    std::string vertexShader;   //mo¿e byæ kilka
    std::string vertexFragment; //mo¿e byæ kilka
    std::string mtlName;

    glm::vec3 ambient = glm::vec3(0.0f);
    glm::vec3 diffuse = glm::vec3(0.0f);
    glm::vec3 specular = glm::vec3(0.0f);
    float shininess = 5.0f;

    bool visableModel = true;

    VertTextNorm* vertTextNorm;
    ProgramID* program;
    std::vector<ModelInstanceInfo> modelInstanceInfo;

    static std::map<std::string, VertTextNorm>* InfoForModels;
    static std::map<std::string, ProgramID*>* Textures;
    static std::map<std::string, ProgramID>* Programs;
    static std::map<std::string, std::unique_ptr<Model>>* Models;
    static std::map<std::string, std::string>* VertexShaders;
    static std::map<std::string, std::string>* FragmentShaders;
    static std::map<std::string, TexturesData*>* LoadedTextures;

    //////////////////////////////////////////////////////////////////////

};

class StandardModel : public Model
{
public:
    StandardModel(std::string _name, std::string _fileName, std::string _textureName, std::string _programName, std::string _vertexShader, std::string _vertexFragment);
    void Render() override;
    void InitModel() override;
};

class TransparentModel : public Model
{
public:

};

class ReflectiveModel : public Model
{
public:

};

class TransparentAndReflectiveModel : public Model
{
public:

};

class _2DModel : public Model
{
public:
    _2DModel(std::string _name, std::string _fileName, std::string _textureName, std::string _programName, std::string _vertexShader, std::string _vertexFragment);
    void Render() override;
    void InitModel() override;
};

class _2DCrossModel : public Model
{
public:
    _2DCrossModel(std::string _name, std::string _fileName, std::string _textureName, std::string _programName, std::string _vertexShader, std::string _vertexFragment);
    void Render() override;
    void InitModel() override;
};

class _Animated3DModel : public Model
{
public:

};

class ModelInstanceInfo
{
public:
    ModelInstanceInfo();
    ModelInstanceInfo(glm::vec3 _pos, glm::vec3 _rot, int id);

    glm::mat4 matMaterial = glm::mat4(1.0);

    unsigned int id = 0;
    bool visableInstance = true;
    glm::vec3 pos;
    glm::vec3 rot;
};