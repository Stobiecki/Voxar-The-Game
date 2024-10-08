#include "Light.h"

std::map<std::string, std::unique_ptr<LightParam>>* Light::LoadedLight = &RenderData::LoadedLight;

// LightParam
LightParam LightParam::defaultLight = LightParam(
	std::string("default"),
	glm::vec3(0.0, 0.0, 0.0), // ambient
	glm::vec3(1.0, 1.0, 1.0), // diffuse
	glm::vec3(1.0, 1.0, 1.0), // specular
	glm::vec3(0.01, 0.01, 0.01), // attenuation
	glm::vec3(0.0, 0.0, 0.0)  // position
);

void LightParam::SendLightParam(GLuint idProgram, int id, int counter = 0)
{
	glUniform3fv(glGetUniformLocation(idProgram, (std::string("myLight[") + std::to_string(id) + "].Ambient").c_str()), 1, glm::value_ptr(Ambient));
	glUniform3fv(glGetUniformLocation(idProgram, (std::string("myLight[") + std::to_string(id) + "].Diffuse").c_str()), 1, glm::value_ptr(Diffuse));
	glUniform3fv(glGetUniformLocation(idProgram, (std::string("myLight[") + std::to_string(id) + "].Specular").c_str()), 1, glm::value_ptr(Specular));
	glUniform3fv(glGetUniformLocation(idProgram, (std::string("myLight[") + std::to_string(id) + "].Attenuation").c_str()), 1, glm::value_ptr(Attenuation));
	glUniform3fv(glGetUniformLocation(idProgram, (std::string("myLight[") + std::to_string(id) + "].Position").c_str()), 1, glm::value_ptr(Position));
}

LightParam::LightParam(std::string _name, glm::vec3 _pos) :
	name{ _name }, Ambient{ defaultLight.Ambient },
	Diffuse{ defaultLight.Diffuse }, Specular{ defaultLight.Specular },
	Attenuation{ defaultLight.Attenuation }, Position{ _pos } {}

LightParam::LightParam(std::string _name, glm::vec3 _ambient, glm::vec3 _diffuse, glm::vec3 _specular, glm::vec3 _attenuation, glm::vec3 _position) :
	name{ _name }, Ambient{ _ambient },
	Diffuse{ _diffuse }, Specular{ _specular },
	Attenuation{ _attenuation }, Position{ _position } {}

// Light
bool Light::lightMode = 0;
bool Light::shaderMode = 1;

void Light::LoadLightingInfoFromFile(std::ifstream& ifs)
{
	// if first sign will be:
	//		g - it means it will be global
	//		gc - it means it will be global custom
	//		d - it means it will be point default
	//		c - it means it will be point custom

	string mode;
	string lightName;

	string AmbientX, AmbientY, AmbientZ;
	string DiffuseX, DiffuseY, DiffuseZ;
	string SpecularX, SpecularY, SpecularZ;
	string AttenuationX, AttenuationY, AttenuationZ;
	string PositionX, PositionY, PositionZ;

	string newLineSign;

	for (bool exitFlag = false; std::getline(ifs, mode, ';');)
	{
		if (mode == "c")
		{
			if (std::getline(ifs, lightName, ';') &&
				std::getline(ifs, AmbientX, ';') &&
				std::getline(ifs, AmbientY, ';') &&
				std::getline(ifs, AmbientZ, ';') &&
				std::getline(ifs, DiffuseX, ';') &&
				std::getline(ifs, DiffuseY, ';') &&
				std::getline(ifs, DiffuseZ, ';') &&
				std::getline(ifs, SpecularX, ';') &&
				std::getline(ifs, SpecularY, ';') &&
				std::getline(ifs, SpecularZ, ';') &&
				std::getline(ifs, AttenuationX, ';') &&
				std::getline(ifs, AttenuationY, ';') &&
				std::getline(ifs, AttenuationZ, ';') &&
				std::getline(ifs, PositionX, ';') &&
				std::getline(ifs, PositionY, ';') &&
				std::getline(ifs, PositionZ, ';'))
			{
				glm::vec3 Ambient = glm::vec3(stof(AmbientX), stof(AmbientY), stof(AmbientZ));
				glm::vec3 Diffuse = glm::vec3(stof(DiffuseX), stof(DiffuseY), stof(DiffuseZ));
				glm::vec3 Specular = glm::vec3(stof(SpecularX), stof(SpecularY), stof(SpecularZ));
				glm::vec3 Attenuation = glm::vec3(stof(AttenuationX), stof(AttenuationY), stof(AttenuationZ));
				glm::vec3 Position = glm::vec3(stof(PositionX), stof(PositionY), stof(PositionZ));
				//(*LoadedLight)[lightName] = PointLightParam(lightName, std::move(Ambient), std::move(Diffuse), std::move(Specular), std::move(Attenuation), std::move(Position));
				LoadedLight->emplace(lightName, std::make_unique<PointLightParam>(lightName, std::move(Ambient), std::move(Diffuse), std::move(Specular), std::move(Attenuation), std::move(Position)));
			}
		}
		else if (mode == "d")
		{
			if (std::getline(ifs, lightName, ';') &&
				std::getline(ifs, PositionX, ';') &&
				std::getline(ifs, PositionY, ';') &&
				std::getline(ifs, PositionZ, ';'))
			{
				cout << "\n" << lightName;
				glm::vec3 Position = glm::vec3(stof(PositionX), stof(PositionY), stof(PositionZ));
				//(*LoadedLight)[lightName] = PointLightParam(lightName, std::move(Position));
				LoadedLight->emplace(lightName, std::make_unique<PointLightParam>(lightName, std::move(Position)));
			}
		}
		else if (mode == "g")
		{
			if (std::getline(ifs, lightName, ';') &&
				std::getline(ifs, PositionX, ';') &&
				std::getline(ifs, PositionY, ';') &&
				std::getline(ifs, PositionZ, ';'))
			{
				cout << "\n" << lightName;
				glm::vec3 Position = glm::vec3(stof(PositionX), stof(PositionY), stof(PositionZ));
				//(*LoadedLight)[lightName] = GlobalLightParam(lightName, std::move(Position));
				LoadedLight->emplace(lightName, std::make_unique<GlobalLightParam>(lightName, std::move(Position)));
			}
		}
		else if (mode == "gc")
		{
			if (std::getline(ifs, lightName, ';') &&
				std::getline(ifs, AmbientX, ';') &&
				std::getline(ifs, AmbientY, ';') &&
				std::getline(ifs, AmbientZ, ';') &&
				std::getline(ifs, DiffuseX, ';') &&
				std::getline(ifs, DiffuseY, ';') &&
				std::getline(ifs, DiffuseZ, ';') &&
				std::getline(ifs, SpecularX, ';') &&
				std::getline(ifs, SpecularY, ';') &&
				std::getline(ifs, SpecularZ, ';') &&
				std::getline(ifs, AttenuationX, ';') &&
				std::getline(ifs, AttenuationY, ';') &&
				std::getline(ifs, AttenuationZ, ';') &&
				std::getline(ifs, PositionX, ';') &&
				std::getline(ifs, PositionY, ';') &&
				std::getline(ifs, PositionZ, ';'))
			{
				cout << "\n" << lightName;
				glm::vec3 Ambient = glm::vec3(stof(AmbientX), stof(AmbientY), stof(AmbientZ));
				glm::vec3 Diffuse = glm::vec3(stof(DiffuseX), stof(DiffuseY), stof(DiffuseZ));
				glm::vec3 Specular = glm::vec3(stof(SpecularX), stof(SpecularY), stof(SpecularZ));
				glm::vec3 Attenuation = glm::vec3(stof(AttenuationX), stof(AttenuationY), stof(AttenuationZ));
				glm::vec3 Position = glm::vec3(stof(PositionX), stof(PositionY), stof(PositionZ));
				//(*LoadedLight)[lightName] = GlobalLightParam(lightName, std::move(Position));
				LoadedLight->emplace(lightName, std::make_unique<GlobalLightParam>(lightName, std::move(Ambient), std::move(Diffuse), std::move(Specular), std::move(Attenuation), std::move(Position)));
			}
		}
		else if (mode == "r")
		{
			if (std::getline(ifs, lightName, ';') &&
				std::getline(ifs, PositionX, ';') &&
				std::getline(ifs, PositionY, ';') &&
				std::getline(ifs, PositionZ, ';'))
			{
				cout << "\n" << lightName;
				glm::vec3 Position = glm::vec3(stof(PositionX), stof(PositionY), stof(PositionZ));
				//(*LoadedLight)[lightName] = GlobalLightParam(lightName, std::move(Position));
				LoadedLight->emplace(lightName, std::make_unique<RelativeLightParam>(lightName, std::move(Position)));
			}
		}
		else if (mode == "rc")
		{
			if (std::getline(ifs, lightName, ';') &&
				std::getline(ifs, AmbientX, ';') &&
				std::getline(ifs, AmbientY, ';') &&
				std::getline(ifs, AmbientZ, ';') &&
				std::getline(ifs, DiffuseX, ';') &&
				std::getline(ifs, DiffuseY, ';') &&
				std::getline(ifs, DiffuseZ, ';') &&
				std::getline(ifs, SpecularX, ';') &&
				std::getline(ifs, SpecularY, ';') &&
				std::getline(ifs, SpecularZ, ';') &&
				std::getline(ifs, AttenuationX, ';') &&
				std::getline(ifs, AttenuationY, ';') &&
				std::getline(ifs, AttenuationZ, ';') &&
				std::getline(ifs, PositionX, ';') &&
				std::getline(ifs, PositionY, ';') &&
				std::getline(ifs, PositionZ, ';'))
			{
				cout << "\n" << lightName;
				glm::vec3 Ambient = glm::vec3(stof(AmbientX), stof(AmbientY), stof(AmbientZ));
				glm::vec3 Diffuse = glm::vec3(stof(DiffuseX), stof(DiffuseY), stof(DiffuseZ));
				glm::vec3 Specular = glm::vec3(stof(SpecularX), stof(SpecularY), stof(SpecularZ));
				glm::vec3 Attenuation = glm::vec3(stof(AttenuationX), stof(AttenuationY), stof(AttenuationZ));
				glm::vec3 Position = glm::vec3(stof(PositionX), stof(PositionY), stof(PositionZ));
				//(*LoadedLight)[lightName] = GlobalLightParam(lightName, std::move(Position));
				LoadedLight->emplace(lightName, std::make_unique<RelativeLightParam>(lightName, std::move(Ambient), std::move(Diffuse), std::move(Specular), std::move(Attenuation), std::move(Position)));
			}
		}
		if (!std::getline(ifs, newLineSign, '\n'))
			exitFlag = true;
	}
}

void Light::InitLightData()
{
	std::ifstream ifs;
	ifs.open("data/lights.txt");
	if (ifs.is_open())
		LoadLightingInfoFromFile(ifs);
	ifs.close();
}


void Light::SendLightParam()
{
	GLint idProgram = 0;
	glGetIntegerv(GL_CURRENT_PROGRAM, &idProgram);
	glUniform1i(glGetUniformLocation(idProgram, "lightCount"), Light::LoadedLight->size());
	glUniform1i(glGetUniformLocation(idProgram, "lightMode"), Light::lightMode);
	glUniform1i(glGetUniformLocation(idProgram, "shaderMode"), Light::shaderMode); //bool
	int i = 0, pointLightCounter = 0, noOfActive = 0;
	for (auto& light : *LoadedLight)
	{
		//cout << "\nin loop name: " << light.second->name;
		if (PointLightParam* l = dynamic_cast<PointLightParam*>(light.second.get()))
		{
			light.second->SendLightParam(idProgram, i, pointLightCounter);
			if (light.second->IsTurnedOn)
				noOfActive++;
			pointLightCounter++;
		}
		else
			light.second->SendLightParam(idProgram, i, 0);
		i++;
	}
	glUniform1i(glGetUniformLocation(idProgram, "samplerCubeCount"), noOfActive);
}

// PointLightParam -> LightParam
PointLightParam::PointLightParam(std::string _name, glm::vec3 _pos)
{
	name = _name;
	Ambient = LightParam::defaultLight.Ambient;
	Diffuse = LightParam::defaultLight.Diffuse;
	Specular = LightParam::defaultLight.Specular;
	Attenuation = LightParam::defaultLight.Attenuation;
	Position = _pos;
}

PointLightParam::PointLightParam(std::string _name, glm::vec3 _ambient, glm::vec3 _diffuse, glm::vec3 _specular, glm::vec3 _attenuation, glm::vec3 _position)
{
	name = _name;
	Ambient = _ambient;
	Diffuse = _diffuse;
	Specular = _specular;
	Attenuation = _attenuation;
	Position = _position;
}

void PointLightParam::SendLightParam(GLuint idProgram, int id, int counter)
{
	if (name == "light1")
	{
		static float Shift = 0.0f;
		static glm::vec3 finalPos;

		Shift = Shift + 0.0002f;
		//cout << endl << Shift;

		finalPos.x = (Position.x) * cos(Shift) - (Position.z) * sin(Shift);
		finalPos.z = (Position.z) * cos(Shift) + (Position.x) * sin(Shift);
		finalPos.y = Position.y;
		glUniform3fv(glGetUniformLocation(idProgram, (std::string("myLight[") + std::to_string(id) + "].Position").c_str()), 1, glm::value_ptr(finalPos));
	}
	else
		glUniform3fv(glGetUniformLocation(idProgram, (std::string("myLight[") + std::to_string(id) + "].Position").c_str()), 1, glm::value_ptr(Position));

	glUniform3fv(glGetUniformLocation(idProgram, (std::string("myLight[") + std::to_string(id) + "].Ambient").c_str()), 1, glm::value_ptr(Ambient));
	glUniform3fv(glGetUniformLocation(idProgram, (std::string("myLight[") + std::to_string(id) + "].Diffuse").c_str()), 1, glm::value_ptr(Diffuse));
	glUniform3fv(glGetUniformLocation(idProgram, (std::string("myLight[") + std::to_string(id) + "].Specular").c_str()), 1, glm::value_ptr(Specular));
	glUniform3fv(glGetUniformLocation(idProgram, (std::string("myLight[") + std::to_string(id) + "].Attenuation").c_str()), 1, glm::value_ptr(Attenuation));
	glUniform3fv(glGetUniformLocation(idProgram, (std::string("myLight[") + std::to_string(id) + "].Position").c_str()), 1, glm::value_ptr(Position));
	glUniform1i(glGetUniformLocation(idProgram, (std::string("myLight[") + std::to_string(id) + "].IsTurnedOn").c_str()), IsTurnedOn);

	int texSlot = id + 6;
	if (texSlot > 31)
		texSlot = 31;
	//__CHECK_FOR_ERRORS
	glActiveTexture(GL_TEXTURE0 + texSlot);
	glBindTexture(GL_TEXTURE_CUBE_MAP, idTextureCube);
	//glUniform1i(glGetUniformLocation(idProgram, (std::string("myLight[") + std::to_string(id) + "].texShadowCubeMap").c_str()), texSlot);
	//glUniform1i(glGetUniformLocation(idProgram, (std::string("texShadowCubeMap[") + std::to_string(id - 1) + "]").c_str()), texSlot);
	if (0 <= counter && counter <= 5)
	{
		glUniform1i(glGetUniformLocation(idProgram, (std::string("texShadowCubeMap") + std::to_string(counter+1)).c_str()), texSlot);
		glUniform1i(glGetUniformLocation(idProgram, (std::string("lightShadowId") + std::to_string(counter+1)).c_str()), id);
	}

	// Parametry oswietlenia
	glUniform3fv(glGetUniformLocation(idProgram, (std::string("myLight[") + std::to_string(id) + "].lightPos").c_str()), 1, glm::value_ptr(Position));
	glUniform1f(glGetUniformLocation(idProgram, (std::string("myLight[") + std::to_string(id) + "].farPlane").c_str()), frustumFar);

	// Inne : pozycja kamery
	glUniform3fv(glGetUniformLocation(idProgram, (std::string("myLight[") + std::to_string(id) + "].cameraPos").c_str()), 1, glm::value_ptr(Camera::cameraPos));
	//__CHECK_FOR_ERRORS
}

void PointLightParam::ShadowMapPointInit()
{
	//glEnable(GL_CULL_FACE);
	for (auto& light : RenderData::LoadedLight)
	{
		if (PointLightParam* l = dynamic_cast<PointLightParam*>(light.second.get()))
		{
			cout << "\nname: " << l->name;
			glGenTextures(1, &l->idTextureCube);
			glBindTexture(GL_TEXTURE_CUBE_MAP, l->idTextureCube);

			for (int i = 0; i < 6; ++i)
				glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_DEPTH_COMPONENT, l->DepthMap_Width, l->DepthMap_Height, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);

			glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
			glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
			glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
			glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
			glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

			// 3. 6 x FBO z buforem glebokosci na kazda sciane
			for (int i = 0; i < 6; i++)
			{
				glGenFramebuffers(1, &l->idFrameBuffer[i]);
				glBindFramebuffer(GL_FRAMEBUFFER, l->idFrameBuffer[i]);
				//glFramebufferTexture(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, idTextureCube, 0);
				glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, l->idTextureCube, 0);
				glDrawBuffer(GL_NONE);
				glReadBuffer(GL_NONE);

				if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
					printf("Error: Framebuffer %d is not complete!\n", i);
					exit(1);
				}
			}
			glBindFramebuffer(GL_FRAMEBUFFER, 0);

			// 4. Potok do generowania samych cieni
			l->idProgram = glCreateProgram();
			glAttachShader(l->idProgram, ShaderLoader::LoadShader(GL_VERTEX_SHADER, "shaders/depthmap-cube-vertex.glsl"));
			glAttachShader(l->idProgram, ShaderLoader::LoadShader(GL_FRAGMENT_SHADER, "shaders/depthmap-cube-fragment.glsl"));
			ShaderLoader::LinkAndValidateProgram(l->idProgram);

			// 5. Ustawienia macierzy rzutowania kamery oswietlenia
			l->matProj = glm::perspective(glm::radians(90.0f), (float)l->DepthMap_Width / l->DepthMap_Height, l->frustumNear, l->frustumFar);

			// aktualizacja macierzy view
			l->UpdateViewMat();
		}
	}
	//glDisable(GL_CULL_FACE);
}

void PointLightParam::UpdateViewMat()
{
	matProj = glm::perspective(glm::radians(90.0f), (float)DepthMap_Width / DepthMap_Height, frustumNear, frustumFar);
	// Obliczanie nowych macierzy widoku
	matViews[0] = glm::lookAt(Position, Position + glm::vec3(1.0, 0.0, 0.0), glm::vec3(0.0, -1.0, 0.0));
	matViews[1] = glm::lookAt(Position, Position + glm::vec3(-1.0, 0.0, 0.0), glm::vec3(0.0, -1.0, 0.0));
	matViews[2] = glm::lookAt(Position, Position + glm::vec3(0.0, 1.0, 0.0), glm::vec3(0.0, 0.0, 1.0));
	matViews[3] = glm::lookAt(Position, Position + glm::vec3(0.0, -1.0, 0.0), glm::vec3(0.0, 0.0, -1.0));
	matViews[4] = glm::lookAt(Position, Position + glm::vec3(0.0, 0.0, 1.0), glm::vec3(0.0, -1.0, 0.0));
	matViews[5] = glm::lookAt(Position, Position + glm::vec3(0.0, 0.0, -1.0), glm::vec3(0.0, -1.0, 0.0));
}

void PointLightParam::RenderSceneToShadowCubeMap()
{
	//glDisable(GL_DEPTH_TEST);
	//__CHECK_FOR_ERRORS
		for (auto& light : RenderData::LoadedLight)
		{
			if (PointLightParam* l = dynamic_cast<PointLightParam*>(light.second.get()))
				if (l->IsTurnedOn == true)
				{
					l->UpdateViewMat();
					for (int i = 0; i < 6; i++)
					{
						l->BeginRenderOfShadowMapFace(i);
						for (auto& model : RenderData::Models)
						{
							Model* m = model.second.get();
							if (m->name == "street_lamp_bulb")
							{
								continue;
								cout << endl << m->name;
							}
							for (auto& instance : m->modelInstanceInfo)
							{
								//cout << " " << instance.id;
								glUniformMatrix4fv(glGetUniformLocation(l->idProgram, "matModel"), 1, GL_FALSE, glm::value_ptr(instance.matMaterial));
								glBindVertexArray(m->program->idVAO);
								glDrawArrays(GL_TRIANGLES, 0, m->vertTextNorm->vert.size());
								glBindVertexArray(0);
							}
						}
					}
					RenderRenderOfShadowMapFaces();
				}
		}
	//glEnable(GL_DEPTH_TEST);
	//__CHECK_FOR_ERRORS
}

void PointLightParam::BeginRenderOfShadowMapFace(int face)
{
	glBindFramebuffer(GL_FRAMEBUFFER, idFrameBuffer[face]);
	glViewport(0, 0, DepthMap_Width, DepthMap_Height);
	glClear(GL_DEPTH_BUFFER_BIT);

	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_CUBE_MAP_POSITIVE_X + face, idTextureCube, 0);

	// AKTYWUJEMY program
	glUseProgram(idProgram);

	// Wysylanie macierzy
	glUniformMatrix4fv(glGetUniformLocation(idProgram, "matProj"), 1, GL_FALSE, glm::value_ptr(matProj));
	glUniformMatrix4fv(glGetUniformLocation(idProgram, "matView"), 1, GL_FALSE, glm::value_ptr(matViews[face]));
	glUniform3fv(glGetUniformLocation(idProgram, "lightPos"), 1, glm::value_ptr(Position));
	glUniform1f(glGetUniformLocation(idProgram, "farPlane"), frustumFar);
}

void PointLightParam::RenderRenderOfShadowMapFaces()
{
	// WYLACZAMY program, przechodzimy do domyslnego FBO
	glUseProgram(0);
	glViewport(0, 0, StaticData::windowWidth, StaticData::windowHeight);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	glBindVertexArray(0);
	glActiveTexture(GL_TEXTURE0);
	glClear(GL_DEPTH_BUFFER_BIT);
}

// GlobalLightParam -> LightParam
GlobalLightParam* GlobalLightParam::globalLight;

GlobalLightParam::GlobalLightParam(std::string _name, glm::vec3 _pos)
{
	name = _name;
	Ambient = LightParam::defaultLight.Ambient;
	Diffuse = LightParam::defaultLight.Diffuse;
	Specular = LightParam::defaultLight.Specular;
	Attenuation = LightParam::defaultLight.Attenuation;
	Position = _pos;
	globalLight = this;
}

GlobalLightParam::GlobalLightParam(std::string _name, glm::vec3 _ambient, glm::vec3 _diffuse, glm::vec3 _specular, glm::vec3 _attenuation, glm::vec3 _position)
{
	name = _name;
	Ambient = _ambient;
	Diffuse = _diffuse;
	Specular = _specular;
	Attenuation = _attenuation;
	Position = _position;
	globalLight = this;
}

void GlobalLightParam::SendLightParam(GLuint idProgram, int id, int counter = 0)
{
	//Position = RenderData::Models["sun_moon"].get()->modelInstanceInfo[0].rot;
	Position = GlobalLightParam::globalLight->Light_Direction;

	glUniform3fv(glGetUniformLocation(idProgram, "globalLightParam.Ambient"), 1, glm::value_ptr(Ambient));
	glUniform3fv(glGetUniformLocation(idProgram, "globalLightParam.Diffuse"), 1, glm::value_ptr(Diffuse));
	glUniform3fv(glGetUniformLocation(idProgram, "globalLightParam.Specular"), 1, glm::value_ptr(Specular));
	glUniform3fv(glGetUniformLocation(idProgram, "globalLightParam.Attenuation"), 1, glm::value_ptr(Attenuation));
	glUniform3fv(glGetUniformLocation(idProgram, "globalLightParam.Direction"), 1, glm::value_ptr(Position));
	glUniform1i(glGetUniformLocation(idProgram, "globalLightParam.IsTurnedOn"), IsTurnedOn);
}

// FIX_ME
void GlobalLightParam::ShadowMapDirInit()
{
	//__CHECK_FOR_ERRORS
		GlobalLightParam* gl = GlobalLightParam::globalLight;
	// Ustawienie polozenia i kierunku swiatla
	gl->Light_Direction = glm::normalize(gl->Light_Direction); // glm::normalize(glm::vec3(0.2, -0.8f, 1.1f));
	gl->Light_Position = glm::vec3(0.0f, 20.0f, -10.0f);

	// Ustawienie macierzy kamery swiatla
	gl->lightProj = glm::ortho(-30.0f, 30.0f, -30.0f, 30.0f, -2.0f, 61.0f);
	gl->lightView = glm::lookAt(gl->Light_Position, gl->Light_Position + gl->Light_Direction, glm::vec3(0.0f, 1.0f, 0.0f));


	// 1. Stworzenie obiektu tekstury
	glGenTextures(1, &gl->DepthMap_idTexture);
	glBindTexture(GL_TEXTURE_2D, gl->DepthMap_idTexture);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, DepthMap_Width, DepthMap_Height, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
	float borderColor[] = { 0.0f, 0.0f, 0.0f, 1.0f };
	glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, borderColor);

	// 2. Stworzenie obiektu ramki bufora jedynie z buforem glebokosci
	// (skladowa koloru nie jest nam potrzebna)
	glGenFramebuffers(1, &gl->DepthMap_idFrameBuffer);

	// 3. Dolaczanie tekstury do ramki bufora
	glBindFramebuffer(GL_FRAMEBUFFER, gl->DepthMap_idFrameBuffer);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, gl->DepthMap_idTexture, 0);
	glDrawBuffer(GL_NONE);
	glReadBuffer(GL_NONE);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);


	// 4. Stworzenie oddzielnego/uproszczonego programu,
	// ktory bedzie generowal mape cieni
	gl->DepthMap_idProgram = glCreateProgram();
	glAttachShader(gl->DepthMap_idProgram, ShaderLoader::LoadShader(GL_VERTEX_SHADER, "shaders/depthmap-vertex.glsl"));
	glAttachShader(gl->DepthMap_idProgram, ShaderLoader::LoadShader(GL_FRAGMENT_SHADER, "shaders/depthmap-fragment.glsl"));
	ShaderLoader::LinkAndValidateProgram(gl->DepthMap_idProgram);

	//// Texture must be bound first
	//static GLint swizzle[4] = {
	//	GL_RED,   // Shader Red   channel source = Texture Red
	//	GL_GREEN, // Shader Green channel source = Texture Green
	//	GL_BLUE,  // Shader Blue  channel source = Texture Blue
	//	GL_ONE    // Shader Alpha channel source = One
	//};
	//glTexParameteriv(GL_TEXTURE_2D, GL_TEXTURE_SWIZZLE_RGBA, swizzle);
}

int GlobalLightParam::DepthMap_Width = 1024 * 16;
int GlobalLightParam::DepthMap_Height = 1024 * 16;

float GlobalLightParam::Ortholeft = -80.0f;
float GlobalLightParam::OrthoRight = 80.0f;
float GlobalLightParam::OrthoBottom = -80.0f;
float GlobalLightParam::OrthoTop = 80.0f;
float GlobalLightParam::OrthoNear = -400.0f;
float GlobalLightParam::OrthoFar = 400.0f;

float GlobalLightParam::shadowBias = 0.00015f;
float GlobalLightParam::biasMultiplier = 0.001f;

void GlobalLightParam::ShadowMapDirUpdate()
{
	GlobalLightParam* gl = GlobalLightParam::globalLight;
	Model* m = RenderData::Models["sun_moon"].get();

	glm::vec3 vectorBetweenPoints = glm::normalize(glm::vec3(0.0f) - m->modelInstanceInfo[0].pos);

	gl->Light_Direction = vectorBetweenPoints;
	gl->Light_Position = Camera::cameraPos;

	if (m->modelInstanceInfo[0].pos.y >= -5.0f)
	{
		//GlobalLightParam::shadowBias = 0.001f;
		//gl->IsTurnedOn = true;
	}
	else
	{
		//GlobalLightParam::shadowBias = 0.01f;
		//gl->IsTurnedOn = false;
	}

	// Ustawienie macierzy kamery swiatla
	gl->lightProj = glm::ortho(Ortholeft, OrthoRight, OrthoBottom, OrthoTop, OrthoNear, OrthoFar);
	gl->lightView = glm::lookAt(gl->Light_Position, gl->Light_Position + gl->Light_Direction, glm::vec3(0.0f, 1.0f, 0.0f));
}

void GlobalLightParam::RenderSceneToShadowMap()
{
	GlobalLightParam* gl = GlobalLightParam::globalLight;

	// 1. Renderowanie z pozycji swiatla do textury DepthMap
	//__CHECK_FOR_ERRORS
		// FIX_ME 
		glViewport(0, 0, DepthMap_Width, DepthMap_Height);
	glBindFramebuffer(GL_FRAMEBUFFER, gl->DepthMap_idFrameBuffer);
	glClear(GL_DEPTH_BUFFER_BIT);
	// AKTYWUJEMY program
	glUseProgram(gl->DepthMap_idProgram);

	// Przesylamy macierze kamery z punktu
	// widzenia zrodla swiatla
	glUniformMatrix4fv(glGetUniformLocation(gl->DepthMap_idProgram, "lightProj"), 1, GL_FALSE, glm::value_ptr(gl->lightProj));
	glUniformMatrix4fv(glGetUniformLocation(gl->DepthMap_idProgram, "lightView"), 1, GL_FALSE, glm::value_ptr(gl->lightView));

	// Tutaj umieszczamy wszystkie obiekty
	// ktore maja rzucac cien
	for (auto& model : RenderData::Models)
	{
		Model* m = model.second.get();
		for (auto& instance : m->modelInstanceInfo)
		{
			if (m->name != "sun_moon")
				glUniformMatrix4fv(glGetUniformLocation(gl->DepthMap_idProgram, "matModel"), 1, GL_FALSE, glm::value_ptr(instance.matMaterial));
			glBindVertexArray(m->program->idVAO);
			glDrawArrays(GL_TRIANGLES, 0, m->vertTextNorm->vert.size());
			glBindVertexArray(0);
		}
	}
	// WYLACZAMY program
	glUseProgram(0);
	glViewport(0, 0, StaticData::windowWidth, StaticData::windowHeight);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}


// RelativeLightParam -> LightParam
RelativeLightParam::RelativeLightParam(std::string _name, glm::vec3 _pos)
{
	name = _name;
	Ambient = LightParam::defaultLight.Ambient;
	Diffuse = LightParam::defaultLight.Diffuse;
	Specular = LightParam::defaultLight.Specular;
	Attenuation = LightParam::defaultLight.Attenuation;
	Position = _pos;
}

RelativeLightParam::RelativeLightParam(std::string _name, glm::vec3 _ambient, glm::vec3 _diffuse, glm::vec3 _specular, glm::vec3 _attenuation, glm::vec3 _position)
{
	name = _name;
	Ambient = _ambient;
	Diffuse = _diffuse;
	Specular = _specular;
	Attenuation = _attenuation;
	Position = _position;
}

void RelativeLightParam::SendLightParam(GLuint idProgram, int id, int counter = 0)
{

	glUniform3fv(glGetUniformLocation(idProgram, "relativeLightParam.Ambient"), 1, glm::value_ptr(Ambient));
	glUniform3fv(glGetUniformLocation(idProgram, "relativeLightParam.Diffuse"), 1, glm::value_ptr(Diffuse));
	glUniform3fv(glGetUniformLocation(idProgram, "relativeLightParam.Specular"), 1, glm::value_ptr(Specular));
	glUniform3fv(glGetUniformLocation(idProgram, "relativeLightParam.Attenuation"), 1, glm::value_ptr(Attenuation));
	glUniform3fv(glGetUniformLocation(idProgram, "relativeLightParam.Position"), 1, glm::value_ptr(Position));
	glUniform1i(glGetUniformLocation(idProgram, "relativeLightParam.IsTurnedOn"), IsTurnedOn);
}