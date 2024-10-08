// Model
#include "Model.h"

using std::vector;
using std::string;
using std::cout;
using std::endl;

//class RenderData;

//ProgramID::ProgramID(){}

std::map<string, VertTextNorm>* Model::InfoForModels = &RenderData::InfoForModels;
std::map<string, ProgramID*>* Model::Textures = &RenderData::Textures;
std::map<string, ProgramID>* Model::Programs = &RenderData::Programs;
std::map<string, std::unique_ptr<Model>>* Model::Models = &RenderData::Models;
std::map<string, string>* Model::VertexShaders = &RenderData::VertexShaders;
std::map<string, string>* Model::FragmentShaders = &RenderData::FragmentShaders;
std::map<string, TexturesData*>* Model::LoadedTextures = &RenderData::LoadedTextures;
std::map<string, TexturesData*>* TexturesData::LoadedTextures = &RenderData::LoadedTextures;

Model::Model(std::string _name, std::string _fileName, std::string _textureName, std::string _programName, std::string _vertexShader, std::string _vertexFragment) : Model()
{
	name = _name;
	fileName = _fileName;
	textureName = _textureName;
	programName = _programName;

	ambient = myMaterial.Ambient;
	diffuse = myMaterial.Diffuse;
	specular = myMaterial.Specular;
	shininess = myMaterial.Shininess;

	LoadTextures(_name, _fileName, _textureName, _programName, _vertexShader, _vertexFragment);
}
void Model::LoadTextures(string _name, string _fileName, string _textureName, string _programName, string _vertexShader, string _vertexFragment)
{
	// InfoForModels
	if (InfoForModels->find(_fileName) == InfoForModels->end())
	{
		InfoForModels->emplace(_fileName, VertTextNorm());
		vertTextNorm = &(*InfoForModels)[_fileName];
		if (!loadOBJ(_fileName.c_str(), vertTextNorm->vert, vertTextNorm->texture, vertTextNorm->normals))
			cout << "Can't load obj file!\n";
		cout << "\nnew material: \"" << _fileName << "\" created and loaded!\n";
	}
	else
	{
		vertTextNorm = &(*InfoForModels)[_fileName];
		cout << "\nmaterial: \"" << _fileName << "\" loaded!\n";
	}

	// Programs
	if (Programs->find(_programName) == Programs->end())
	{
		Programs->emplace(_programName, ProgramID());
		program = &(*Programs)[_programName];
		cout << "new program: \"" << _programName << "\" created and loaded!\n";
	}
	else
	{
		program = &(*Programs)[_programName];
		cout << "program: \"" << _programName << "\" loaded!\n";
	}

	// Textures
	if (Textures->find(_textureName) == Textures->end())
	{
		ProgramID* programPtr = &(*Programs)[_programName];
		Textures->emplace(_textureName, programPtr);
		int texWidth, texHeigh, texN;
		unsigned char* texData = nullptr;

		stbi_set_flip_vertically_on_load(true);
		glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

		texData = stbi_load(_textureName.c_str(), &texWidth, &texHeigh, &texN, 0);
		if (texData == NULL)
			cout << "Texture can't be loaded!\n";
		else
			cout << "new texture: \"" << _textureName << "\" loaded!\n";

		glGenTextures(1, &programPtr->idTexture);
		glBindTexture(GL_TEXTURE_2D, programPtr->idTexture);
		if (_textureName.find(".png") == string::npos)					// could throw error here (non-transparent bg related)
			glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, texWidth, texHeigh, 0, GL_RGB, GL_UNSIGNED_BYTE, texData);
		else
			glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, texWidth, texHeigh, 0, GL_RGBA, GL_UNSIGNED_BYTE, texData);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);

		//to mo¿e byæ Ÿle
		RenderData::LoadedTextures[name] = new TexturesData(name, programPtr, _textureName, texWidth, texHeigh, texN, texData);

	}
	else // when texture is loaded but you want to reuse it for other objects
	{
		ProgramID* programPtr = &(*Programs)[_programName];
		TexturesData* dataModelThatAlreadyHasTexture = nullptr;
		// searching for model that uses this texture (at this point there MUST be one like this)
		for (auto& usedTex : *LoadedTextures)
		{
			if (usedTex.second->texName == textureName)
				dataModelThatAlreadyHasTexture = usedTex.second;
		}
		cout << "\nreusing texture: \"" << _textureName << "\"!\n";
		glGenTextures(1, &program->idTexture);
		glBindTexture(GL_TEXTURE_2D, program->idTexture);
		if (_textureName.find(".png") == string::npos)					// could throw error here
			glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, dataModelThatAlreadyHasTexture->texWidth, dataModelThatAlreadyHasTexture->texHeigh, 0, GL_RGB, GL_UNSIGNED_BYTE, dataModelThatAlreadyHasTexture->texData);
		else
			glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, dataModelThatAlreadyHasTexture->texWidth, dataModelThatAlreadyHasTexture->texHeigh, 0, GL_RGBA, GL_UNSIGNED_BYTE, dataModelThatAlreadyHasTexture->texData);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	}

	// if possible try load .mtl file
	LoadMtlFile(_fileName);

	int count = 0;
	// Vertex shaders
	if (VertexShaders->find(_vertexShader) != VertexShaders->end())
	{
		vertexShader = _vertexShader;
		cout << "\nVertex shaders loaded to model!";
	}
	else
		count++;

	// Fragment shaders
	if (FragmentShaders->find(_vertexFragment) != FragmentShaders->end())
	{
		vertexFragment = _vertexFragment;
		cout << "\nFragment shaders loaded to model!";
	}
	else
		count++;

	if (count >= 2)
	{
		cout << "\nLoading fragment shader error! Exiting the program!";
		exit(1);
	}
}

void Model::InitAllModels()
{
	for (auto& model : *Models)
		model.second.get()->InitModel();
}

void Model::InitModel()
{
	// propably it will be changed later on
	for (auto& modelInstances : modelInstanceInfo)
	{
		modelInstances.matMaterial = glm::translate(modelInstances.matMaterial, modelInstances.pos);
		if (modelInstances.rot.x != 0.0f)
			modelInstances.matMaterial = glm::rotate(modelInstances.matMaterial, modelInstances.rot.x, glm::vec3(1.0, 0.0, 0.0));
		if (modelInstances.rot.y != 0.0f)
			modelInstances.matMaterial = glm::rotate(modelInstances.matMaterial, modelInstances.rot.y, glm::vec3(0.0, 0.0, 1.0));
		if (modelInstances.rot.z != 0.0f)
			modelInstances.matMaterial = glm::rotate(modelInstances.matMaterial, modelInstances.rot.z, glm::vec3(0.0, 1.0, 0.0));
	}

	// potok
	program->idProgram = glCreateProgram();
	glAttachShader(program->idProgram, ShaderLoader::LoadShader(GL_VERTEX_SHADER, vertexShader.c_str()));
	glAttachShader(program->idProgram, ShaderLoader::LoadShader(GL_FRAGMENT_SHADER, vertexFragment.c_str()));
	ShaderLoader::LinkAndValidateProgram(program->idProgram);

	// VAO
	glGenVertexArrays(1, &program->idVAO);
	glBindVertexArray(program->idVAO);

	// vertex buffor
	glGenBuffers(1, &(program->idVBO_coord));
	glBindBuffer(GL_ARRAY_BUFFER, program->idVBO_coord);
	glBufferData(GL_ARRAY_BUFFER, vertTextNorm->vert.size() * sizeof(glm::vec3), &(vertTextNorm->vert[0]), GL_STATIC_DRAW);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, NULL);
	glEnableVertexAttribArray(0);

	// texture buffor
	glGenBuffers(1, &(program->idVBO_uv));
	glBindBuffer(GL_ARRAY_BUFFER, program->idVBO_uv);
	glBufferData(GL_ARRAY_BUFFER, vertTextNorm->texture.size() * sizeof(glm::vec2), &(vertTextNorm->texture[0]), GL_STATIC_DRAW);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 0, NULL);
	glEnableVertexAttribArray(1);

	// color buffor
	glGenBuffers(1, &(program->idVBO_color));
	glBindBuffer(GL_ARRAY_BUFFER, program->idVBO_color);
	glBufferData(GL_ARRAY_BUFFER, vertTextNorm->normals.size() * sizeof(glm::vec3), &(vertTextNorm->normals[0]), GL_STATIC_DRAW);
	glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, 0, NULL);
	glEnableVertexAttribArray(2);

	glBindVertexArray(0);
}

void Model::Render()
{
	//__CHECK_FOR_ERRORS
		ProgramID* programPtr = &(*Programs)[programName];
	TexturesData* texData = RenderData::LoadedTextures[name];
	Model* m = RenderData::Models["sun_moon"].get();
	//TexturesData* texData = (*LoadedTextures)[textureName];

	if (visableModel == false)
		return;

	for (auto& modelInstances : modelInstanceInfo)
	{
		if (modelInstances.visableInstance)
		{
			//__CHECK_FOR_ERRORS
				glUseProgram(program->idProgram);
			glUniformMatrix4fv(glGetUniformLocation(program->idProgram, "matProj"), 1, GL_FALSE, glm::value_ptr(RenderData::matProj));
			glUniformMatrix4fv(glGetUniformLocation(program->idProgram, "matView"), 1, GL_FALSE, glm::value_ptr(RenderData::matView));
			glUniformMatrix4fv(glGetUniformLocation(program->idProgram, "matModel"), 1, GL_FALSE, glm::value_ptr(modelInstances.matMaterial));
			glUniform3fv(glGetUniformLocation(program->idProgram, "cameraPos"), 1, &Camera::cameraPos[0]);
			//__CHECK_FOR_ERRORS
				// global light param
				glUniformMatrix4fv(glGetUniformLocation(program->idProgram, "lightProj"), 1, GL_FALSE, glm::value_ptr(GlobalLightParam::globalLight->lightProj));
			glUniformMatrix4fv(glGetUniformLocation(program->idProgram, "lightView"), 1, GL_FALSE, glm::value_ptr(GlobalLightParam::globalLight->lightView));
			glUniform3fv(glGetUniformLocation(program->idProgram, "lightDirection"), 1, glm::value_ptr(GlobalLightParam::globalLight->Light_Direction));
			glUniform1f(glGetUniformLocation(program->idProgram, "BIAS"), GlobalLightParam::shadowBias);
			//__CHECK_FOR_ERRORS
				// mist
				if (StaticData::mistOn)
				{
					glUniform3fv(glGetUniformLocation(program->idProgram, "fogColor"), 1, &StaticData::mistShade[0]);
					glUniform1f(glGetUniformLocation(program->idProgram, "fogDensity"), StaticData::mistDensity);
				}
				else
				{
					glUniform3fv(glGetUniformLocation(program->idProgram, "fogColor"), 1, &StaticData::mistShade[0]);
					glUniform1f(glGetUniformLocation(program->idProgram, "fogDensity"), 0.0f);
				}
			//__CHECK_FOR_ERRORS
			Light::SendLightParam();
			TexturesData::SendMaterialParam(this);
			//__CHECK_FOR_ERRORS
			if (name == "street_lamp_bulb")
			{
				if (modelInstances.id == 0)
				{
					//static glm::vec3 red = glm::vec3(1.f, 0.f, 0.f);
					glUniform3fv(glGetUniformLocation(program->idProgram, "staticColor"), 1, glm::value_ptr(RenderData::LoadedLight["street_light1"].get()->Diffuse));
				}
				else if (modelInstances.id == 1)
				{
					//static glm::vec3 green = glm::vec3(0.f, 1.f, 0.f);
					glUniform3fv(glGetUniformLocation(program->idProgram, "staticColor"), 1, glm::value_ptr(RenderData::LoadedLight["street_light2"].get()->Diffuse));
				}
				else // modelInstances.id == 3
				{
					//static glm::vec3 blue = glm::vec3(0.f, 0.f, 1.f);
					glUniform3fv(glGetUniformLocation(program->idProgram, "staticColor"), 1, glm::value_ptr(RenderData::LoadedLight["street_light3"].get()->Diffuse));
				}
			}

			if (name == "monkey")
			{
				static bool leftRight = true; // true - left, false - right

				modelInstanceInfo[0].matMaterial = glm::mat4(1.0);
				if (leftRight == true)
					modelInstanceInfo[0].pos.x -= 0.02f;
				else if (leftRight == false)
					modelInstanceInfo[0].pos.x += 0.02f;

				if (modelInstanceInfo[0].pos.x < -10.f)
				{
					leftRight = false;
					modelInstanceInfo[0].pos.x = -10.f + 0.01f;
				}
				else if (modelInstanceInfo[0].pos.x > 10.f)
				{
					leftRight = true;
					modelInstanceInfo[0].pos.x = 10.f - 0.01f;
				}

				modelInstanceInfo[0].rot.x += 0.005;
				modelInstanceInfo[0].rot.z += 0.005;

				modelInstanceInfo[0].matMaterial = glm::translate(modelInstanceInfo[0].matMaterial, modelInstanceInfo[0].pos);
				modelInstanceInfo[0].matMaterial = glm::rotate(modelInstanceInfo[0].matMaterial, modelInstanceInfo[0].rot.x, glm::vec3(1, 0, 0));
				modelInstanceInfo[0].matMaterial = glm::rotate(modelInstanceInfo[0].matMaterial, modelInstanceInfo[0].rot.z, glm::vec3(0, 0, 1));
			}

			if (name == "sun_moon")
			{
				glm::vec3 currentPos = modelInstanceInfo[0].pos;
				modelInstanceInfo[0].matMaterial = glm::mat4(1.0);

				modelInstanceInfo[0].matMaterial = glm::rotate(modelInstanceInfo[0].matMaterial, 0.002f, glm::vec3(0.0f, 0.0f, 1.0f));
				modelInstanceInfo[0].matMaterial = glm::rotate(modelInstanceInfo[0].matMaterial, 0.0005f, glm::vec3(0.0f, 1.0f, 0.0f));

				modelInstanceInfo[0].matMaterial = glm::translate(modelInstanceInfo[0].matMaterial, glm::vec3(currentPos.x, currentPos.y, currentPos.z));
				modelInstanceInfo[0].pos = glm::vec3(modelInstanceInfo[0].matMaterial[3]);
			}
			//__CHECK_FOR_ERRORS
			glActiveTexture(GL_TEXTURE0);
				glBindTexture(GL_TEXTURE_2D, programPtr->idTexture);
				glUniform1i(glGetUniformLocation(programPtr->idProgram, "tex"), 0);
			// global light shadow map
			glActiveTexture(GL_TEXTURE2);
			//__CHECK_FOR_ERRORS
			glBindTexture(GL_TEXTURE_2D, GlobalLightParam::globalLight->DepthMap_idTexture);
			//__CHECK_FOR_ERRORS
			glUniform1i(glGetUniformLocation(programPtr->idProgram, "tex_shadowMap"), 2);
			//__CHECK_FOR_ERRORS
				glBindVertexArray(program->idVAO);
			//__CHECK_FOR_ERRORS
				glDrawArrays(GL_TRIANGLES, 0, vertTextNorm->vert.size());
			//__CHECK_FOR_ERRORS
				glBindVertexArray(0);
			//__CHECK_FOR_ERRORS
			glUseProgram(0);
			//__CHECK_FOR_ERRORS
		}
	}

	//unbinding texture
	//glBindTexture(GL_TEXTURE_2D, 0);
}

void Model::InitModelsData()
{
	std::ifstream ifs;
	ifs.open("data/models.txt");
	if (ifs.is_open())
		LoadModelsInfoFromFile(ifs);
	ifs.close();
}

void Model::LoadModelsInfoFromFile(std::ifstream& ifs)
{
	string modelType;
	string nameTag;
	string path;
	string texture;
	string programUsed;
	string vertexShader;
	string vertexFragment;
	string newLineSign;
	if (ifs.is_open())
	{
		// preambule ends with @
		string buff;
		std::getline(ifs, buff, '@');
		std::getline(ifs, buff, '\n');
		while (std::getline(ifs, modelType, ';') &&
			std::getline(ifs, nameTag, ';') &&
			std::getline(ifs, path, ';') &&
			std::getline(ifs, texture, ';') &&
			std::getline(ifs, programUsed, ';') &&
			std::getline(ifs, vertexShader, ';') &&
			std::getline(ifs, vertexFragment, ';'))
		{
			//RenderData::Models[nameTag.c_str()] = Model(nameTag, path, texture, programUsed, vertexShader, vertexFragment);
			if (modelType == "SM")
			{
				Models->emplace(nameTag.c_str(), std::make_unique<Model>(nameTag, path, texture, programUsed, vertexShader, vertexFragment));
			}
			else if (modelType == "TM")
			{

			}
			else if (modelType == "RM")
			{

			}
			else if (modelType == "TRM")
			{

			}
			else if (modelType == "2D")
			{
				Models->emplace(nameTag.c_str(), std::make_unique<_2DModel>(nameTag, path, texture, programUsed, vertexShader, vertexFragment));
			}
			else if (modelType == "2DC")
			{
				Models->emplace(nameTag.c_str(), std::make_unique<_2DCrossModel>(nameTag, path, texture, programUsed, vertexShader, vertexFragment));
			}
			else if (modelType == "3DA")
			{

			}

			std::getline(ifs, newLineSign, '\n');
		}
	}
	else
		exit(1);
}

void Model::InitModelPlacement()
{
	std::ifstream ifs;
	ifs.open("data/map.txt");
	if (ifs.is_open())
		LoadModelsPlacementInfoFromFile(ifs);
	ifs.close();
}

void Model::LoadMtlFile(std::string modelName)
{
	std::string mtlName = modelName;
	bool readyToLoadMtl = false;
	if (mtlName.length() >= 3)
	{
		mtlName.erase(mtlName.length() - 3, 3);
		mtlName += "mtl";
		this->mtlName = mtlName;
		readyToLoadMtl = true;
	}

	std::ifstream ifs;
	ifs.open(mtlName);
	if (ifs.is_open() && readyToLoadMtl == true)
	{
		string dump;
		string posX, posY, posZ;
		string newLineSign;

		while (std::getline(ifs, dump))
		{
			if (dump.length() <= 2)
				continue;
			std::string tag = dump.substr(0, 2);
			std::string rest = dump.substr(3, dump.length() - 3);
			std::istringstream iss(rest);
			std::vector<std::string> tokens;
			std::string token;

			while (std::getline(iss, token, ' '))
				tokens.push_back(token);

			if (tag == "Ka")
				ambient = glm::vec3(stof(tokens[0]), stof(tokens[1]), stof(tokens[2]));
			else if (tag == "Kd")
				diffuse = glm::vec3(stof(tokens[0]), stof(tokens[1]), stof(tokens[2]));
			else if (tag == "Ks")
				specular = glm::vec3(stof(tokens[0]), stof(tokens[1]), stof(tokens[2]));
			else if (tag == "Ns")
				shininess = stof(tokens[0]);
		}

	}
	ifs.close();
}

void Model::LoadModelsPlacementInfoFromFile(std::ifstream& ifs)
{
	string nameTag;
	string posX, posY, posZ;
	string rotX, rotY, rotZ;
	string newLineSign;

	if (ifs.is_open())
		while (std::getline(ifs, nameTag, ';') &&
			std::getline(ifs, posX, ';') &&
			std::getline(ifs, posY, ';') &&
			std::getline(ifs, posZ, ';') &&
			std::getline(ifs, rotX, ';') &&
			std::getline(ifs, rotY, ';') &&
			std::getline(ifs, rotZ, ';'))
		{
			// info generated form blender -> that's why these values are mixed up
			glm::vec3 modelPos = glm::vec3(stof(posX), stof(posZ), -stof(posY));
			glm::vec3 modelRot = glm::vec3(stof(rotX), stof(rotY), stof(rotZ));
			RenderData::Models[nameTag.c_str()].get()->modelInstanceInfo.push_back(ModelInstanceInfo(std::move(modelPos), std::move(modelRot), RenderData::Models[nameTag.c_str()].get()->modelInstanceInfo.size()));
			std::getline(ifs, newLineSign, '\n');
		}
}

// StandardModel
StandardModel::StandardModel(std::string _name, std::string _fileName, std::string _textureName, std::string _programName, std::string _vertexShader, std::string _vertexFragment) :
	Model(_name, _fileName, _textureName, _programName, _vertexShader, _vertexFragment)
{
	cout << "\noverrided constructor";
}


void StandardModel::Render()
{
	cout << "\noverrided render";
}

void StandardModel::InitModel()
{
	cout << "\noverrided InitModel";
}

// _2DModel
_2DModel::_2DModel(std::string _name, std::string _fileName, std::string _textureName, std::string _programName, std::string _vertexShader, std::string _vertexFragment) :
	Model(_name, _fileName, _textureName, _programName, _vertexShader, _vertexFragment)
{
	cout << "\noverrided constructor";
}

void _2DModel::Render()
{
	ProgramID* programPtr = &(*Programs)[programName];
	TexturesData* texData = RenderData::LoadedTextures[name];
	//TexturesData* texData = (*LoadedTextures)[textureName];

	//cout << "\nmodel name<" << name << ">";

	if (visableModel == false)
		return;

	for (auto& modelInstances : modelInstanceInfo)
	{
		if (modelInstances.visableInstance)
		{
			glUseProgram(program->idProgram);
			glUniformMatrix4fv(glGetUniformLocation(program->idProgram, "matProj"), 1, GL_FALSE, glm::value_ptr(RenderData::matProj));
			glUniformMatrix4fv(glGetUniformLocation(program->idProgram, "matView"), 1, GL_FALSE, glm::value_ptr(RenderData::matView));
			glUniformMatrix4fv(glGetUniformLocation(program->idProgram, "matModel"), 1, GL_FALSE, glm::value_ptr(modelInstances.matMaterial));
			glUniform3fv(glGetUniformLocation(program->idProgram, "cameraPos"), 1, &Camera::cameraPos[0]);

			Light::SendLightParam();
			TexturesData::SendMaterialParam(this);

			glActiveTexture(GL_TEXTURE0);
			glBindTexture(GL_TEXTURE_2D, programPtr->idTexture);
			glUniform1i(glGetUniformLocation(programPtr->idProgram, "tex"), 0);

			glBindVertexArray(program->idVAO);
			glDrawArrays(GL_TRIANGLES, 0, vertTextNorm->vert.size());
			glBindVertexArray(0);
			glUseProgram(0);
		}
	}
}

void _2DModel::InitModel()
{
	// propably it will be changed later on
	for (auto& modelInstances : modelInstanceInfo)
	{
		modelInstances.matMaterial = glm::translate(modelInstances.matMaterial, modelInstances.pos);
		if (modelInstances.rot.x != 0.0f)
			modelInstances.matMaterial = glm::rotate(modelInstances.matMaterial, modelInstances.rot.x, glm::vec3(1.0, 0.0, 0.0));
		if (modelInstances.rot.y != 0.0f)
			modelInstances.matMaterial = glm::rotate(modelInstances.matMaterial, modelInstances.rot.y, glm::vec3(0.0, 0.0, 1.0));
		if (modelInstances.rot.z != 0.0f)
			modelInstances.matMaterial = glm::rotate(modelInstances.matMaterial, modelInstances.rot.z, glm::vec3(0.0, 1.0, 0.0));
	}

	// potok
	program->idProgram = glCreateProgram();
	glAttachShader(program->idProgram, ShaderLoader::LoadShader(GL_VERTEX_SHADER, vertexShader.c_str()));
	glAttachShader(program->idProgram, ShaderLoader::LoadShader(GL_FRAGMENT_SHADER, vertexFragment.c_str()));
	ShaderLoader::LinkAndValidateProgram(program->idProgram);

	// VAO
	glGenVertexArrays(1, &program->idVAO);
	glBindVertexArray(program->idVAO);

	// vertex buffor
	glGenBuffers(1, &(program->idVBO_coord));
	glBindBuffer(GL_ARRAY_BUFFER, program->idVBO_coord);
	glBufferData(GL_ARRAY_BUFFER, vertTextNorm->vert.size() * sizeof(glm::vec3), &(vertTextNorm->vert[0]), GL_STATIC_DRAW);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, NULL);
	glEnableVertexAttribArray(0);

	// color buffor
	glGenBuffers(1, &(program->idVBO_color));
	glBindBuffer(GL_ARRAY_BUFFER, program->idVBO_color);
	glBufferData(GL_ARRAY_BUFFER, vertTextNorm->normals.size() * sizeof(glm::vec3), &(vertTextNorm->normals[0]), GL_STATIC_DRAW);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, NULL);
	glEnableVertexAttribArray(1);

	// texture buffor
	glGenBuffers(1, &(program->idVBO_uv));
	glBindBuffer(GL_ARRAY_BUFFER, program->idVBO_uv);
	glBufferData(GL_ARRAY_BUFFER, vertTextNorm->texture.size() * sizeof(glm::vec2), &(vertTextNorm->texture[0]), GL_STATIC_DRAW);
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 0, NULL);
	glEnableVertexAttribArray(2);
	glBindVertexArray(0);
}

// _2DCrossModel
_2DCrossModel::_2DCrossModel(std::string _name, std::string _fileName, std::string _textureName, std::string _programName, std::string _vertexShader, std::string _vertexFragment) :
	Model(_name, _fileName, _textureName, _programName, _vertexShader, _vertexFragment)
{
	cout << "\noverrided constructor";
}

void _2DCrossModel::Render()
{
	ProgramID* programPtr = &(*Programs)[programName];
	TexturesData* texData = RenderData::LoadedTextures[name];
	//TexturesData* texData = (*LoadedTextures)[textureName];

	//cout << "\nmodel name<" << name << ">";

	if (visableModel == false)
		return;

	for (auto& modelInstances : modelInstanceInfo)
	{
		if (modelInstances.visableInstance)
		{
			glUseProgram(program->idProgram);

			glUniformMatrix4fv(glGetUniformLocation(program->idProgram, "matProj"), 1, GL_FALSE, glm::value_ptr(RenderData::matProj));
			glUniformMatrix4fv(glGetUniformLocation(program->idProgram, "matView"), 1, GL_FALSE, glm::value_ptr(RenderData::matView));
			glUniformMatrix4fv(glGetUniformLocation(program->idProgram, "matModel"), 1, GL_FALSE, glm::value_ptr(modelInstances.matMaterial));
			glUniform3fv(glGetUniformLocation(program->idProgram, "cameraPos"), 1, &Camera::cameraPos[0]);

			Light::SendLightParam();
			TexturesData::SendMaterialParam(this);

			glActiveTexture(GL_TEXTURE0);
			glBindTexture(GL_TEXTURE_2D, programPtr->idTexture);
			glUniform1i(glGetUniformLocation(programPtr->idProgram, "tex"), 0);

			glBindVertexArray(program->idVAO);
			glDrawArrays(GL_TRIANGLES, 0, vertTextNorm->vert.size());
			glBindVertexArray(0);

			glm::mat4 rotaded2DModel = glm::rotate(modelInstances.matMaterial, StaticData::one_degree * 90, glm::vec3(0, 1, 0));

			glUniformMatrix4fv(glGetUniformLocation(program->idProgram, "matProj"), 1, GL_FALSE, glm::value_ptr(RenderData::matProj));
			glUniformMatrix4fv(glGetUniformLocation(program->idProgram, "matView"), 1, GL_FALSE, glm::value_ptr(RenderData::matView));
			glUniformMatrix4fv(glGetUniformLocation(program->idProgram, "matModel"), 1, GL_FALSE, glm::value_ptr(rotaded2DModel));
			glUniform3fv(glGetUniformLocation(program->idProgram, "cameraPos"), 1, &Camera::cameraPos[0]);

			glActiveTexture(GL_TEXTURE0);
			glBindTexture(GL_TEXTURE_2D, programPtr->idTexture);
			glUniform1i(glGetUniformLocation(programPtr->idProgram, "tex"), 0);

			glBindVertexArray(program->idVAO);
			glDrawArrays(GL_TRIANGLES, 0, vertTextNorm->vert.size());
			glBindVertexArray(0);

			glUseProgram(0);
		}
	}
}

void _2DCrossModel::InitModel()
{
	// propably it will be changed later on
	for (auto& modelInstances : modelInstanceInfo)
	{
		modelInstances.matMaterial = glm::translate(modelInstances.matMaterial, modelInstances.pos);
		if (modelInstances.rot.x != 0.0f)
			modelInstances.matMaterial = glm::rotate(modelInstances.matMaterial, modelInstances.rot.x, glm::vec3(1.0, 0.0, 0.0));
		if (modelInstances.rot.y != 0.0f)
			modelInstances.matMaterial = glm::rotate(modelInstances.matMaterial, modelInstances.rot.y, glm::vec3(0.0, 0.0, 1.0));
		if (modelInstances.rot.z != 0.0f)
			modelInstances.matMaterial = glm::rotate(modelInstances.matMaterial, modelInstances.rot.z, glm::vec3(0.0, 1.0, 0.0));
	}

	// potok
	program->idProgram = glCreateProgram();
	glAttachShader(program->idProgram, ShaderLoader::LoadShader(GL_VERTEX_SHADER, vertexShader.c_str()));
	glAttachShader(program->idProgram, ShaderLoader::LoadShader(GL_FRAGMENT_SHADER, vertexFragment.c_str()));
	ShaderLoader::LinkAndValidateProgram(program->idProgram);

	// VAO
	glGenVertexArrays(1, &program->idVAO);
	glBindVertexArray(program->idVAO);

	// vertex buffor
	glGenBuffers(1, &(program->idVBO_coord));
	glBindBuffer(GL_ARRAY_BUFFER, program->idVBO_coord);
	glBufferData(GL_ARRAY_BUFFER, vertTextNorm->vert.size() * sizeof(glm::vec3), &(vertTextNorm->vert[0]), GL_STATIC_DRAW);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, NULL);
	glEnableVertexAttribArray(0);

	// color buffor
	glGenBuffers(1, &(program->idVBO_color));
	glBindBuffer(GL_ARRAY_BUFFER, program->idVBO_color);
	glBufferData(GL_ARRAY_BUFFER, vertTextNorm->normals.size() * sizeof(glm::vec3), &(vertTextNorm->normals[0]), GL_STATIC_DRAW);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, NULL);
	glEnableVertexAttribArray(1);

	// texture buffor
	glGenBuffers(1, &(program->idVBO_uv));
	glBindBuffer(GL_ARRAY_BUFFER, program->idVBO_uv);
	glBufferData(GL_ARRAY_BUFFER, vertTextNorm->texture.size() * sizeof(glm::vec2), &(vertTextNorm->texture[0]), GL_STATIC_DRAW);
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 0, NULL);
	glEnableVertexAttribArray(2);
	glBindVertexArray(0);
}

// ModelInstanceInfo
ModelInstanceInfo::ModelInstanceInfo() : pos{ glm::vec3(0.f) }, rot{ glm::vec3(0.f) } {}
ModelInstanceInfo::ModelInstanceInfo(glm::vec3 _pos, glm::vec3 _rot, int _id) : pos{ _pos }, rot{ _rot }, id(_id) {}