#pragma once
//#define NDEBUG
#include "Utilities.h"


using std::vector;
using std::string;
using std::cout;
using std::endl;
#define __CHECK_FOR_ERRORS { GLenum errCode ; if (( errCode = glGetError ()) != GL_NO_ERROR) printf(" Error (\% d): \%s in file \%s at line \%d !\n", errCode, gluErrorString(errCode), __FILE__, __LINE__); }


// loadOBJ
bool loadOBJ(
	const char* path,
	std::vector<glm::vec3>& out_vertices,
	std::vector<glm::vec2>& out_uvs,
	std::vector<glm::vec3>& out_normals
)
{
	//printf("Loading OBJ file %s ... ", path);

	std::vector<unsigned int> vertexIndices, uvIndices, normalIndices;
	std::vector<glm::vec3> temp_vertices;
	std::vector<glm::vec2> temp_uvs;
	std::vector<glm::vec3> temp_normals;


	FILE* file = fopen(path, "r");
	if (file == NULL) {
		printf("can't open the file.\n");
		return false;
	}


	while (1)
	{

		char lineHeader[128];

		// read the first word of the line
		int res = fscanf(file, "%s", lineHeader);

		if (res == EOF)
			break; // EOF = End Of File. Quit the loop.

		// read vertex coordinates
		if (strcmp(lineHeader, "v") == 0)
		{
			glm::vec3 vertex;
			fscanf(file, "%f %f %f\n", &vertex.x, &vertex.y, &vertex.z);
			temp_vertices.push_back(vertex);

		}
		// read texture coordinates
		else if (strcmp(lineHeader, "vt") == 0)
		{
			glm::vec2 uv;
			fscanf(file, "%f %f\n", &uv.x, &uv.y);
			temp_uvs.push_back(uv);
		}
		// read normal vectors
		else if (strcmp(lineHeader, "vn") == 0)
		{
			glm::vec3 normal;
			fscanf(file, "%f %f %f\n", &normal.x, &normal.y, &normal.z);
			temp_normals.push_back(normal);
		}
		// read faces (triangles)
		else if (strcmp(lineHeader, "f") == 0)
		{
			std::string vertex1, vertex2, vertex3;
			unsigned int vertexIndex[3], uvIndex[3], normalIndex[3];
			int matches = fscanf(file, "%d/%d/%d %d/%d/%d %d/%d/%d\n", &vertexIndex[0], &uvIndex[0], &normalIndex[0],
				&vertexIndex[1], &uvIndex[1], &normalIndex[1], &vertexIndex[2], &uvIndex[2], &normalIndex[2]);

			if (matches != 9)
			{
				printf("\nFile can't be read by our simple parser. ");
				printf("Try exporting with other options and make sure you export normals and uvs.\n");
				fclose(file);
				return false;
			}

			// Create new triangle
			vertexIndices.push_back(vertexIndex[0]);
			vertexIndices.push_back(vertexIndex[1]);
			vertexIndices.push_back(vertexIndex[2]);
			uvIndices.push_back(uvIndex[0]);
			uvIndices.push_back(uvIndex[1]);
			uvIndices.push_back(uvIndex[2]);
			normalIndices.push_back(normalIndex[0]);
			normalIndices.push_back(normalIndex[1]);
			normalIndices.push_back(normalIndex[2]);

		}
		else
		{
			// Probably a comment, eat up the rest of the line
			char stupidBuffer[1000];
			fgets(stupidBuffer, 1000, file);
		}

	}

	// For each vertex of each triangle
	for (unsigned int i = 0; i < vertexIndices.size(); i++) {

		// Get the indices of its attributes
		unsigned int vertexIndex = vertexIndices[i];
		unsigned int uvIndex = uvIndices[i];
		unsigned int normalIndex = normalIndices[i];

		// Get the attributes thanks to the index
		glm::vec3 vertex = temp_vertices[vertexIndex - 1];
		glm::vec2 uv = temp_uvs[uvIndex - 1];
		glm::vec3 normal = temp_normals[normalIndex - 1];

		// Put the attributes in buffers
		out_vertices.push_back(vertex);
		out_uvs.push_back(uv);
		out_normals.push_back(normal);

	}
	fclose(file);
	//printf(" done.\n");
	return true;
}


// MaterialParam
MaterialParam myMaterial = MaterialParam
{
	glm::vec3(0.2, 0.2, 0.2), // ambient
	glm::vec3(1.0, 1.0, 1.0), // diffuse
	glm::vec3(1.0, 1.0, 1.0), // specular
	32.0 // shininess
};

// TexturesData
TexturesData::TexturesData(std::string _texturedModel, ProgramID* _programPtr, string _texName, int _texWidth, int _texHeigh, int _texN, unsigned char* _texData) :
	texturedModel{ _texturedModel }, programPtr{ _programPtr }, texName{ _texName }, texWidth{ _texWidth }, texHeigh{ _texHeigh }, texN{ _texN }, texData{ _texData }{}

void TexturesData::FreeAllTexturesDataAfterLinking()
{
	for (auto& tex : *LoadedTextures)
	{
		stbi_image_free(tex.second->texData);
		tex.second = nullptr;
	}
}

void TexturesData::SendMaterialParam(Model *m)
{
	GLint idProgram = 1;
	glGetIntegerv(GL_CURRENT_PROGRAM, &idProgram);
	glUniform3fv(glGetUniformLocation(idProgram, "myMaterial.Ambient"), 1, glm::value_ptr(m->ambient));
	glUniform3fv(glGetUniformLocation(idProgram, "myMaterial.Diffuse"), 1, glm::value_ptr(m->diffuse));
	glUniform3fv(glGetUniformLocation(idProgram, "myMaterial.Specular"), 1, glm::value_ptr(m->specular));
	glUniform1f(glGetUniformLocation(idProgram, "myMaterial.Shininess"), m->shininess);

	//glUniform3fv(glGetUniformLocation(idProgram, "myMaterial.Ambient"), 1, glm::value_ptr(myMaterial.Ambient));
	//glUniform3fv(glGetUniformLocation(idProgram, "myMaterial.Diffuse"), 1, glm::value_ptr(myMaterial.Diffuse));
	//glUniform3fv(glGetUniformLocation(idProgram, "myMaterial.Specular"), 1, glm::value_ptr(myMaterial.Specular));
	//glUniform1f(glGetUniformLocation(idProgram, "myMaterial.Shininess"), myMaterial.Shininess);
}

// ShaderLoader
void ShaderLoader::InitShadersData()
{
	std::ifstream ifs;
	ifs.open("data/shaders.txt");
	if (ifs.is_open())
		LoadShadersInfoFromFile(ifs);
	ifs.close();
}

void ShaderLoader::LinkAndValidateProgram(GLuint program)
{
	// Konsolidacja programu
	glLinkProgram(program);
	CheckForErrors_Program(program, GL_LINK_STATUS);

	// Walidacja programu
	glValidateProgram(program);
	CheckForErrors_Program(program, GL_VALIDATE_STATUS);
}

void ShaderLoader::CheckForErrors_Program(GLuint program, GLenum mode)
{
	GLint status;
	glGetProgramiv(program, mode, &status);
	if (status != GL_TRUE)
	{
		switch (mode)
		{
		case GL_LINK_STATUS:
			printf("Blad konsolidacji programu!\n");
			break;
		case GL_VALIDATE_STATUS:
			printf("Blad walidacji programu!\n");
			break;
		default:
			printf("Inny blad programu!\n");

		}
		GLint logLength;
		glGetProgramiv(program, GL_INFO_LOG_LENGTH, &logLength);
		char* log = new char[logLength];
		glGetProgramInfoLog(program, logLength, NULL, log);
		printf("LOG: %s\n", log);
		delete[] log;
	}
}

unsigned long ShaderLoader::getFileLength(std::ifstream& file)
{
	if (!file.good()) return 0;

	unsigned long pos = file.tellg();
	file.seekg(0, std::ios::end);
	unsigned long len = file.tellg();
	file.seekg(std::ios::beg);

	return len;
}

GLchar* ShaderLoader::LoadShaderFile(const char* filename)
{
	std::ifstream file;
	file.open(filename, std::ios::in); // opens as ASCII!
	if (!file) { printf("Can't open file %s !\n", filename); exit(1); }

	unsigned long len = getFileLength(file);

	if (len == 0) { printf("File %s is empty!\n", filename); exit(1); };   // Error: Empty File

	GLchar* ShaderSource = new GLchar[len + 1];
	if (ShaderSource == NULL) { printf("Can't reserve memory %d \n", len + 1); exit(1); }   // can't reserve memory

	// len isn't always strlen cause some characters are stripped in ascii read...
	// it is important to 0-terminate the real length later, len is just max possible value...
	ShaderSource[len] = 0;

	unsigned int i = 0;
	while (file.good())
	{
		ShaderSource[i] = file.get();
		if (!file.eof())
			i++;
	}

	ShaderSource[i] = 0;
	file.close();
	return ShaderSource;
}

void ShaderLoader::CheckForErrors_Shader(GLuint shader)
{
	GLint status;
	glGetShaderiv(shader, GL_COMPILE_STATUS, &status);
	if (status != GL_TRUE)
	{
		printf("Blad!\n");
		GLint logLength;
		glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &logLength);
		char* log = new char[logLength];
		glGetShaderInfoLog(shader, logLength, NULL, log);
		printf("LOG: %s\n", log);
		delete[] log;
	}
}

GLuint ShaderLoader::LoadShader(GLuint MODE, const char* filename)
{
	// utworzenie obiektu shadera
	GLuint shader = glCreateShader(MODE);

	// Wczytanie kodu shadera z pliku
	GLchar* code = LoadShaderFile(filename);

	glShaderSource(shader, 1, &code, NULL);
	glCompileShader(shader);
	CheckForErrors_Shader(shader);

	delete[] code;
	return shader;
}

void ShaderLoader::LoadShadersInfoFromFile(std::ifstream& ifs)
{
	string shaderType;
	string path;
	string programUsed;
	string newLineSign;
	if (ifs.is_open())
		while (std::getline(ifs, shaderType, ';') &&
			std::getline(ifs, path, ';')&&
			std::getline(ifs, programUsed, ';'))
		{
			if (shaderType == "v") // vertex shader
			{
				RenderData::VertexShaders[path] = programUsed;
			}
			else if (shaderType == "f")
			{
				RenderData::FragmentShaders[path] = programUsed;
			}
			std::getline(ifs, newLineSign, '\n');
		}
}

// DitheringFBO
bool DitheringFBO::isTurnedOn = false;
GLuint DitheringFBO::idProgram[DitheringFBO::NUMBER_OF_OBJECTS];
GLuint DitheringFBO::idVAO[DitheringFBO::NUMBER_OF_OBJECTS];
//GLuint DitheringFBO::idVAOpos;
//GLuint DitheringFBO::idVAOuv;
GLuint DitheringFBO::idTexture[DitheringFBO::NUMBER_OF_OBJECTS];

GLuint DitheringFBO::idFrameBuffer;    // FBO
GLuint DitheringFBO::idDepthBuffer;    // Bufor na skladowa glebokosci
GLuint DitheringFBO::idTextureBuffer;  // Tekstura na skladowa koloru

int DitheringFBO::buffer_Width;
int DitheringFBO::buffer_Height;

float DitheringFBO::windowScale = 1.0f;

std::vector<GLfloat> DitheringFBO::vertices_pos;
std::vector<GLfloat> DitheringFBO::vertices_tex;


void DitheringFBO::InitFBO()
{

	buffer_Width = 500;
	buffer_Height = 500;

	vertices_pos =
	{
		-1.0f, -1.0f,
		 1.0f, -1.0f,
		 1.0f,  1.0f,
		 1.0f,  1.0f,
		-1.0f,  1.0f,
		-1.0f, -1.0f
	};

	vertices_tex =
	{
		0.0f, 0.0f,
		1.0f, 0.0f,
		1.0f, 1.0f,
		1.0f, 1.0f,
		0.0f, 1.0f,
		0.0f, 0.0f
	};

		for (int i = 0; i < 12; i++)
		vertices_pos[i] *= windowScale;

	// 1. Stworzenie obiektu FBO
	glGenFramebuffers(1, &idFrameBuffer);
	glBindFramebuffer(GL_FRAMEBUFFER, idFrameBuffer);

	// 2. Stworzenie obiektu tekstury na skladowa koloru
	glGenTextures(1, &idTextureBuffer);
	glBindTexture(GL_TEXTURE_2D, idTextureBuffer);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, buffer_Width, buffer_Height, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	// 3. Polaczenie tekstury ze skladowa koloru FBO
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D,
		idTextureBuffer, 0);

	// 4. Stworzenie obiektu render buffer dla skladowej glebokosci 
	glGenRenderbuffers(1, &idDepthBuffer);
	glBindRenderbuffer(GL_RENDERBUFFER, idDepthBuffer);
	glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT24, buffer_Width, buffer_Height);

	// 5. Polaczenie bufora glebokosci z aktualnym obiektem FBO
	glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, idDepthBuffer);

	// 6. Sprawdzenie czy pomyslnie zostal utworzony obiekt bufora ramki
	//    a nastepnie powrot do domyslnego bufora ramki
	if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
		printf("Error: Framebuffer is not complete!\n");
		exit(1);
	}

	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	//// Potok
	idProgram[SCREEN] = glCreateProgram();
	glAttachShader(idProgram[SCREEN], ShaderLoader::LoadShader(GL_VERTEX_SHADER, "shaders/vertexDithering.glsl"));
	glAttachShader(idProgram[SCREEN], ShaderLoader::LoadShader(GL_FRAGMENT_SHADER, "shaders/fragmentDithering.glsl"));
	ShaderLoader::LinkAndValidateProgram(idProgram[SCREEN]);

	// Ekran
	glGenVertexArrays(1, &idVAO[SCREEN]);
	glBindVertexArray(idVAO[SCREEN]);
	GLuint vao_pos, vao_uv;
	glGenBuffers(1, &vao_pos);
		glBindBuffer(GL_ARRAY_BUFFER, vao_pos);
		glBufferData(GL_ARRAY_BUFFER, vertices_pos.size() * sizeof(GLfloat), &vertices_pos[0], GL_STATIC_DRAW);
		glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 0, NULL);
	glEnableVertexAttribArray(0);
		glGenBuffers(1, &vao_uv);
		glBindBuffer(GL_ARRAY_BUFFER, vao_uv);
		glBufferData(GL_ARRAY_BUFFER, vertices_tex.size() * sizeof(GLfloat), &vertices_tex[0], GL_STATIC_DRAW);
		glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 0, NULL);
	glEnableVertexAttribArray(1);

	//glBindVertexArray(0);
}

void DitheringFBO::RenderToTexture()
{
	if (isTurnedOn)
	{
		glViewport(0, 0, buffer_Width, buffer_Height);
		glBindFramebuffer(GL_FRAMEBUFFER, idFrameBuffer);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		RenderData::RenderAll();
		Skybox::DrawSkyBox(RenderData::matProj, RenderData::matView);

		glBindFramebuffer(GL_FRAMEBUFFER, 0);
		glViewport(0, 0, StaticData::windowWidth, StaticData::windowHeight);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		DitheringFBO::RenderOnScreen();
	}
}

void DitheringFBO::RenderOnScreen()
{
	glUseProgram(idProgram[SCREEN]);

		// Tekstura sceny na screenie
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, idTextureBuffer);
		glUniform1i(glGetUniformLocation(idProgram[SCREEN], "tex0"), 0);

		// Ekran
		glBindVertexArray(idVAO[SCREEN]);
		glDrawArrays(GL_TRIANGLES, 0, 6);
		glBindVertexArray(0);

	glUseProgram(0);
}

// CubeMapDisplay
bool CubeMapDisplay::alreadyInitialized = false;
bool CubeMapDisplay::depthOnly = false;
GLuint *CubeMapDisplay::idFrameBuffer = nullptr;
GLuint *CubeMapDisplay::idTextureCube = nullptr;
GLuint CubeMapDisplay::CubeMap_VAO;
GLuint CubeMapDisplay::CubeMap_Program;


GLfloat CubeMapDisplay::positions[8 * 3] =
{
	1.0f, 1.0f, 1.0f,   // 0
	-1.0f, 1.0f, 1.0f,  // 1
	-1.0f, -1.0f, 1.0f, // 2
	1.0f, -1.0f, 1.0f,  // 3
	1.0f, 1.0f, -1.0f,  // 4
	-1.0f, 1.0f, -1.0f, // 5
	-1.0f, -1.0f, -1.0f,// 6
	1.0f, -1.0f, -1.0f  // 7
};

GLuint CubeMapDisplay::indices[12 * 3] =
{
	5, 0, 1,
	5, 4, 0,
	2, 0, 3,
	2, 1, 0,
	7, 0, 4,
	7, 3, 0,
	3, 6, 2,
	3, 7, 6,
	1, 2, 6,
	1, 6, 5,
	4, 5, 6,
	4, 6, 7
};

const GLenum CubeMapDisplay::targets[6] =
{
	GL_TEXTURE_CUBE_MAP_POSITIVE_X,
	GL_TEXTURE_CUBE_MAP_NEGATIVE_X,
	GL_TEXTURE_CUBE_MAP_POSITIVE_Y,
	GL_TEXTURE_CUBE_MAP_NEGATIVE_Y,
	GL_TEXTURE_CUBE_MAP_POSITIVE_Z,
	GL_TEXTURE_CUBE_MAP_NEGATIVE_Z
};

glm::mat4 CubeMapDisplay::matCubeMap = glm::mat4(1);
int CubeMapDisplay::Width = 1024;
int CubeMapDisplay::Height = 1024;

void CubeMapDisplay::CubeMapDisplayInit()
{
	if (alreadyInitialized == true)
		return;
	alreadyInitialized = true;

	matCubeMap = glm::translate(glm::mat4(1), glm::vec3(0.0f, 2.0f, 0.0f));

	// Program
	CubeMap_Program = glCreateProgram();
	glAttachShader(CubeMap_Program, ShaderLoader::LoadShader(GL_VERTEX_SHADER, "shaders/vertexCubeMap.glsl"));
	glAttachShader(CubeMap_Program, ShaderLoader::LoadShader(GL_FRAGMENT_SHADER, "shaders/fragmentCubeMap.glsl"));
	ShaderLoader::LinkAndValidateProgram(CubeMap_Program);

	// Vertex arrays
	glGenVertexArrays(1, &CubeMap_VAO);
	glBindVertexArray(CubeMap_VAO);
	// Wspolrzedne wierzchokow
	GLuint vBuffer_pos;
	glGenBuffers(1, &vBuffer_pos);
	glBindBuffer(GL_ARRAY_BUFFER, vBuffer_pos);
	glBufferData(GL_ARRAY_BUFFER, 8 * 3 * sizeof(GLfloat), positions, GL_STATIC_DRAW);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, NULL);
	glEnableVertexAttribArray(0);
	// Tablica indeksow
	GLuint vBuffer_idx;
	glGenBuffers(1, &vBuffer_idx);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, vBuffer_idx);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, 12 * 3 * sizeof(GLuint), indices, GL_STATIC_DRAW);
	glBindVertexArray(0);
}

void CubeMapDisplay::SetCubeMap(GLuint* _idTextureCube, int _Width, int _Height, bool _depthOnly)
{
	static PointLightParam* l = dynamic_cast<PointLightParam*>(RenderData::LoadedLight["street_light1"].get());

	glUseProgram(CubeMap_Program);
		idTextureCube = &l->idTextureCube;
		Width = l->DepthMap_Width;
		Height = l->DepthMap_Height;
		depthOnly = _depthOnly;
		//idTextureCube = _idTextureCube;
		//Width = _Width;
		//Height = _Height;
	glUseProgram(0);

}

void CubeMapDisplay::Render(glm::mat4 matProj, glm::mat4 matView)
{
	glUseProgram(CubeMap_Program);
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_CUBE_MAP, *idTextureCube);
		glm::mat4 matPVM = matProj * matView * matCubeMap;
		glUniformMatrix4fv(glGetUniformLocation(CubeMap_Program, "matPVM"), 1, GL_FALSE, glm::value_ptr(matPVM));
		glUniform1i(glGetUniformLocation(CubeMap_Program, "texCubeMap"), 0);
		glUniform1i(glGetUniformLocation(CubeMap_Program, "depthOnly"), depthOnly);
		glBindVertexArray(CubeMap_VAO);
		glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, NULL);
		glBindVertexArray(CubeMap_VAO);
	glUseProgram(0);
}

// StaticData
constexpr float StaticData::one_degree;
int StaticData::windowWidth = 0;
int StaticData::windowHeight = 0;
bool StaticData::mistOn = true;
glm::vec3 StaticData::mistShade = glm::vec3(0.520f, 0.520f, 1.0f);
float StaticData::mistDensity = 0.003f; //= 0.015f;
bool StaticData::globalLightShadowMapOn = false;
bool StaticData::shadowMap3DOn = false;
Ground StaticData::myGround;
Player StaticData::myPlayer;// = Player();
bool StaticData::endGameTimerOn = false;

void StaticData::EndGameSequence()
{
	static auto start_time = std::chrono::steady_clock::now();
	static auto end_time = start_time + std::chrono::seconds(20);

	if (end_time < std::chrono::steady_clock::now())
		exit(666);
}
