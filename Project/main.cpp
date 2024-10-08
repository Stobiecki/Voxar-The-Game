#pragma once

#define _CRT_SECURE_NO_WARNINGS

#include <stdio.h>
#include <iostream>
#include <glew.h>
#include <freeglut.h>
#include <vector>
#include <map>
#include <math.h>
#include <sstream>
#include <cstdlib>
#include <ctime>
#include <thread>
#include <chrono>

#include "shader_stuff.h"

// ---------------------------------------
#include <glm.hpp>
#include <gtc/matrix_transform.hpp>
#include <gtc/type_ptr.hpp>

//#include "imgui.h"
//#include "imgui_impl_glut.h"
//#include "imgui_impl_opengl3.h"

#include "Model.h"
#include "GUI.h"
#include "Utilities.h"
#include "AdditionalWindows.h"
#include "utilitiesForGLUT.hpp"
#include "Skybox.h"
#include "Player.h"
#include "Ground.h"
#include "Keyboard.h"

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

#include <SFML/Audio.hpp>

void DockspaceInitialization(ImGuiID dockspaceID, ImGuiID main_dock_id, bool* ImGuiDockingWindowsInitFlag);

using std::vector;
using std::string;
using std::cout;
using std::endl;

float angle = 0.f;


// ---------------------------------------
void DisplayScene()
{
	//__CHECK_FOR_ERRORS
	//glCullFace(GL_BACK);

	Camera::ImGuiOn = ImGui::GetIO().WantCaptureMouse;
	//RenderData::matView = UpdateViewMatrix();
	RenderData::matView = UpdateViewMatrixForPlayer(StaticData::myPlayer.Position);
	// Czyszczenie bufora koloru
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	// Update Camera position
	Camera::cameraPos = Camera::ExtractCameraPos(RenderData::matView);
	// Generate shadowmap from all objects (dir light)
	GlobalLightParam::ShadowMapDirUpdate();
	GlobalLightParam::RenderSceneToShadowMap();
	//__CHECK_FOR_ERRORS
	// Generate shadowmap from all objects (point light)
	PointLightParam::RenderSceneToShadowCubeMap();
	//__CHECK_FOR_ERRORS
	if(!DitheringFBO::isTurnedOn)
		RenderData::RenderAll(); // render all
	else
		DitheringFBO::RenderToTexture(); // render all as dithering
	//__CHECK_FOR_ERRORS

	//CubeMapDisplay::SetCubeMap(0, 0, 0, true);
	//CubeMapDisplay::Render(RenderData::matProj, RenderData::matView);
	//__CHECK_FOR_ERRORS
	Skybox::DrawSkyBox(RenderData::matProj, RenderData::matView);

	//checking for pressed keyes
	Keyboard::Update();

	// Potentially changing audio
	Audio::CheckForAudioSwap(glm::length(StaticData::myPlayer.matModel[3]), -Camera::CameraTranslate_y);
	Audio::PlaySoundEffect(glm::length(glm::vec3(StaticData::myPlayer.matModel[3]) - glm::vec3(-425.85f, -345.7f, 125.7f)), &StaticData::endGameTimerOn);

	// EndGameSequence
	if (StaticData::endGameTimerOn)
		StaticData::EndGameSequence();

	// render on screen
	GUI::RenderAll();

	// ImGui
	Camera::ImGuiOn = ImGui::GetIO().WantCaptureMouse;
	static CtrlDispWindow CDW;
	//CDW.Draw();

	glutSwapBuffers();
	//glCullFace(GL_FRONT);
	//__CHECK_FOR_ERRORS
}

// ---------------------------------------
void Initialize()
{
	// 0. Ustawienie maszyny stanów OpenGL
	//glEnable(GL_CULL_FACE);
	//glCullFace(GL_BACK);
	// Ustawianie domyslnego koloru, ktorym bedzie
	// czyszczony bufor koloru
	__CHECK_FOR_ERRORS
	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
	//__CHECK_FOR_ERRORS
	Camera::CameraTranslate_x = 0.0;
	Camera::CameraTranslate_y = -1.0;
	Camera::CameraTranslate_z = -12.0;
	//__CHECK_FOR_ERRORS
	// Init of all data needed
	ShaderLoader::InitShadersData();
	Model::InitModelsData();
	Model::InitModelPlacement();
	Model::InitAllModels();
	Light::InitLightData();
	GUI::InitGuiInfo();

	CubeMapDisplay::CubeMapDisplayInit();
	CubeMapDisplay::SetCubeMap(0, 0, 0, true);

	// Init Skybox
	Skybox::CreateSkyBox();

	//Player data setup
	StaticData::myPlayer.Setup("src/models/player.obj");

	// Ground collision
	StaticData::myGround.Init();

	// Player init with colision
	StaticData::myPlayer.Init(&StaticData::myGround);

	// Global light shadow map
	GlobalLightParam::ShadowMapDirInit();
	PointLightParam::ShadowMapPointInit();

	DitheringFBO::InitFBO();

	// Init Audio + play
	Audio::InitBgMusic();

	// Wlaczanie testu glebokosci
	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LESS);

	// Macierze kontrolujace polozenie kamery
	RenderData::matView = glm::mat4(1.0);

	// ImGui setup
	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGuiIO& io = ImGui::GetIO(); (void)io;
	io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;
	ImGui::StyleColorsDark();
	ImGui_ImplGLUT_Init();
	ImGui_ImplGLUT_InstallFuncs(); // problems fixed inside!!!
	ImGui_ImplOpenGL3_Init("#version 460");
}

// ---------------------------------------------------

void FrameCallback(int i)
{
	glutPostRedisplay();

	glutTimerFunc(1000.f / 120, FrameCallback, 0);
}


// ---------------------------------------------------
//int main(int argc, char* argv[])
int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
{	
	srand(time(NULL));
	//__CHECK_FOR_ERRORS
	// GLUT
	//glutInit(&argc, argv);
	int arc = 0;
	char* arv[1];
	glutInit(&arc, arv);
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);
	glutInitContextVersion(3, 2);
	glutInitContextProfile(GLUT_CORE_PROFILE);
	glutInitWindowSize(1200, 800);
	glutCreateWindow("Voxar - When the sky goes dark");

	//__CHECK_FOR_ERRORS
	glutDisplayFunc(DisplayScene);
	glutReshapeFunc(Reshape);
	glutMouseFunc(MyMouseButton);
	glutMotionFunc(MyMouseMotion);
	glutMouseWheelFunc(MyMouseWheel);
	glutKeyboardFunc(MyKeyboard);
	glutKeyboardUpFunc(MyKeyboardUp);
	glutSpecialFunc(MySpecialKeys);
	//__CHECK_FOR_ERRORS
	// cleanup
	glutCloseFunc(myCleanup);
	//glutSetOption(GLUT_ACTION_ON_WINDOW_CLOSE, GLUT_ACTION_CONTINUE_EXECUTION);
	//__CHECK_FOR_ERRORS
	// GLEW
	glewExperimental = GL_TRUE;
	GLenum err = glewInit();
	if (GLEW_OK != err)
	{
		printf("GLEW Error\n");
		exit(1);
	}
	//__CHECK_FOR_ERRORS
	// OpenGL
	if (!GLEW_VERSION_3_2)
	{
		printf("Brak OpenGL 3.2!\n");
		exit(1);
	}

	Initialize();
	glutTimerFunc(1000.f / 120, FrameCallback, 0);
	glutMainLoop();

	return 0;
}
