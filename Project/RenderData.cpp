#include "RenderData.h"

// RenderData
std::map<string, VertTextNorm> RenderData::InfoForModels;
std::map<string, ProgramID*> RenderData::Textures;
std::map<string, ProgramID> RenderData::Programs;
std::map<string, std::unique_ptr<Model>> RenderData::Models;
std::map<string, string> RenderData::VertexShaders;
std::map<string, string> RenderData::FragmentShaders;
std::map<std::string, TexturesData*> RenderData::LoadedTextures;
std::map<std::string, std::unique_ptr<LightParam>> RenderData::LoadedLight;

glm::mat4 RenderData::matProj;
glm::mat4 RenderData::matView;
GLfloat RenderData::cameraZoom = 3.f;

void RenderData::FreeData()
{
	for (auto& prog : Programs)
	{
		glDeleteProgram(prog.second.idProgram);
		glDeleteVertexArrays(1, &(prog.second.idVBO_coord));
		glDeleteVertexArrays(1, &(prog.second.idVBO_color));
		glDeleteVertexArrays(1, &(prog.second.idVBO_uv));
		glDeleteVertexArrays(1, &(prog.second.idVAO));
	}
}

void RenderData::FreeTexturesData()
{
	for (auto& tex : LoadedTextures)
	{
		if (tex.second != nullptr)
		{
			stbi_image_free(tex.second->texData);
			tex.second->texData = nullptr;
			delete tex.second;
		}
	}
}

void RenderData::RenderAll()
{
	for (auto& model : Models)
		model.second.get()->Render();
}