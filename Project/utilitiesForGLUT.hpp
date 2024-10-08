#pragma once

//#ifndef __SHADER_STUFF
//#define __SHADER_STUFF

// References:
// https://www.opengl.org/sdk/docs/tutorials/ClockworkCoders/loading.php

#include <fstream>
#include <iostream>
#include <cmath>
#include <glew.h>
#include <freeglut.h>
#include <glm.hpp>
#include <gtc/matrix_transform.hpp>
#include "Utilities.h"
#include "Keyboard.h"

#include "imgui.h"
#include "imgui_impl_glut.h"
#include "imgui_impl_opengl3.h"

#include "AdditionalWindows.h"

// Zmienne do kontroli stanu myszy
int _mouse_buttonState = GLUT_UP;
int _mouse_buttonX;
int _mouse_buttonY;

// --------------------------------------------------------------
// Funkcja zwraca macierz widoku dla kamery
// --------------------------------------------------------------
glm::mat4 UpdateViewMatrix()
{
	ImGuiIO& io = ImGui::GetIO();
	if (!io.WantCaptureMouse)
	{
		RenderData::matView = glm::mat4x4(1.0);
		//RenderData::matView = glm::translate(RenderData::matView, glm::vec3(Camera::CameraTranslate_x, Camera::CameraTranslate_y, Camera::CameraTranslate_z));
		RenderData::matView = glm::rotate(RenderData::matView, Camera::CameraRotate_x, glm::vec3(1.0f, 0.0f, 0.0f));
		RenderData::matView = glm::rotate(RenderData::matView, Camera::CameraRotate_y, glm::vec3(0.0f, 1.0f, 0.0f));
		RenderData::matView = glm::translate(RenderData::matView, glm::vec3(Camera::CameraTranslate_x, Camera::CameraTranslate_y, Camera::CameraTranslate_z));
	}

	return RenderData::matView;
}

glm::mat4 UpdateViewMatrixForPlayer(glm::vec3 pos)
{
	RenderData::matView = glm::mat4x4(1.0);
	RenderData::matView = glm::rotate(RenderData::matView, Camera::CameraRotate_x, glm::vec3(1.0f, 0.0f, 0.0f));
	RenderData::matView = glm::rotate(RenderData::matView, Camera::CameraRotate_y, glm::vec3(0.0f, 1.0f, 0.0f));
	RenderData::matView = glm::translate(RenderData::matView, glm::vec3(-pos.x, -pos.y - StaticData::myPlayer.height, -pos.z));

	return RenderData::matView;
}

// ---------------------------------------
void Reshape(int width, int height)
{
	glViewport(0, 0, width, height);
	StaticData::windowWidth = width;
	StaticData::windowHeight = height;
	RenderData::matProj = glm::perspective(3.1415 / (RenderData::cameraZoom), (double)width / (double)height, 0.001, Camera::lengthOfCameraView);

	// ImGui
	ImGuiIO& io = ImGui::GetIO();
	io.DisplaySize = ImVec2((float)width, (float)height);
}

void MyKeyboard(unsigned char key, int x, int y)
{
	bool reshape = false;

	Keyboard::KeyboardTable[key] = true;
	//std::cout << "shitf: " << (glutGetModifiers() & GLUT_ACTIVE_SHIFT) << "\n\n\n\n\n\n\n\n";
	Keyboard::ShiftKey = (glutGetModifiers() & GLUT_ACTIVE_SHIFT);

	if (!Camera::ImGuiOn)
		switch (key)
		{
		//case 27:	// ESC key
		//	exit(0);
		//	break;

		//case 'a':
		//case 'A':
		//	Camera::CameraTranslate_x += 0.3f * cos(Camera::CameraRotate_y);
		//	Camera::CameraTranslate_z += 0.3f * sin(Camera::CameraRotate_y);
		//	break;

		//case 'd':
		//case 'D':
		//	Camera::CameraTranslate_x -= 0.3f * cos(Camera::CameraRotate_y);
		//	Camera::CameraTranslate_z -= 0.3f * sin(Camera::CameraRotate_y);
		//	break;

		//case 'w':
		//case 'W':
		//	Camera::CameraTranslate_x += 0.3f * -sin(Camera::CameraRotate_y);
		//	Camera::CameraTranslate_y += 0.3f * sin(Camera::CameraRotate_x);
		//	Camera::CameraTranslate_z += 0.3f * cos(Camera::CameraRotate_y);
		//	break;

		//case 's':
		//case 'S':
		//	Camera::CameraTranslate_x -= 0.3f * -sin(Camera::CameraRotate_y);
		//	Camera::CameraTranslate_y -= 0.3f * sin(Camera::CameraRotate_x);
		//	Camera::CameraTranslate_z -= 0.3f * cos(Camera::CameraRotate_y);
		//	break;

		case '+':
			RenderData::cameraZoom += 1.f;
			if (RenderData::cameraZoom > 10.f)
				RenderData::cameraZoom = 10.f;
			reshape = true;
			break;

		case '-':
			RenderData::cameraZoom -= 1.f;
			if (RenderData::cameraZoom < 2.f)
				RenderData::cameraZoom = 2.f;
			reshape = true;
			break;
		case 'h':
			DitheringFBO::isTurnedOn = !DitheringFBO::isTurnedOn;
			break;
		case 'm':
			if (CtrlDispWindow::DockspaceTurnedOnOff == true)
			{
				CtrlDispWindow::resizingDirection = CtrlDispWindow::ResizingDocking::Resizing_Docking_DOWN;
				CtrlDispWindow::dockspaceToggled = true;
				CtrlDispWindow::DockspaceTurnedOnOff = false;
			}
			else if (CtrlDispWindow::DockspaceTurnedOnOff == false)
			{
				CtrlDispWindow::resizingDirection = CtrlDispWindow::ResizingDocking::Resizing_Docking_UP;
				CtrlDispWindow::dockspaceToggled = true;
				CtrlDispWindow::DockspaceTurnedOnOff = true;
			}
			break;
		}
	if (reshape = true)
		Reshape(StaticData::windowWidth, StaticData::windowHeight);

	glutPostRedisplay();

	// ImGui
	ImGuiIO& io = ImGui::GetIO();
	if (key >= 32)
		io.AddInputCharacter((unsigned int)key);

	ImGuiKey c = ImGui_ImplGLUT_KeyToImGuiKey(key);
	ImGui_ImplGLUT_AddKeyEvent(c, true, key);
	ImGui_ImplGLUT_UpdateKeyModifiers();
	(void)x; (void)y; // Unused
}

void MyKeyboardUp(unsigned char c, int x, int y)
{
	Keyboard::KeyboardTable[c] = false;
	Keyboard::ShiftKey = (glutGetModifiers() & GLUT_ACTIVE_SHIFT);

	if (c == 'w' || c == 's' || c == 'd' || c == 'a')
	{
		Keyboard::ShiftKey = false;
	}


	ImGuiKey key = ImGui_ImplGLUT_KeyToImGuiKey(c);
	ImGui_ImplGLUT_AddKeyEvent(key, false, c);
	ImGui_ImplGLUT_UpdateKeyModifiers();
	(void)x; (void)y; // Unused
}

// --------------------------------------------------------------
// Funkcja wywolywana przy nacisnieciu tzw. specjalnych
// klawiszy (klawiszy spoza tablicy ASCII)
// --------------------------------------------------------------
void MySpecialKeys( int key, int x, int y )
{
	if (!Camera::ImGuiOn)
		switch( key )
		{
			case GLUT_KEY_LEFT:
				Camera::CameraTranslate_x += 0.2f * cos(Camera::CameraRotate_y);
				Camera::CameraTranslate_z += 0.2f * sin(Camera::CameraRotate_y);
				break;

			case GLUT_KEY_RIGHT:
				Camera::CameraTranslate_x -= 0.2f * cos(Camera::CameraRotate_y);
				Camera::CameraTranslate_z -= 0.2f * sin(Camera::CameraRotate_y);
				break;

			 case GLUT_KEY_UP:
				 Camera::CameraTranslate_x += 0.2f * -sin(Camera::CameraRotate_y);
				 Camera::CameraTranslate_y += 0.2f * sin(Camera::CameraRotate_x);
				 Camera::CameraTranslate_z += 0.2f * cos(Camera::CameraRotate_y);
				break;

			case GLUT_KEY_DOWN:
				Camera::CameraTranslate_x -= 0.2f * -sin(Camera::CameraRotate_y);
				Camera::CameraTranslate_y -= 0.2f * sin(Camera::CameraRotate_x);
				Camera::CameraTranslate_z -= 0.2f * cos(Camera::CameraRotate_y);
				break;
		}

    glutPostRedisplay();

	// ImGui
	ImGuiKey imgui_key = ImGui_ImplGLUT_KeyToImGuiKey(key + 256);
	ImGui_ImplGLUT_AddKeyEvent(imgui_key, true, key + 256);
	ImGui_ImplGLUT_UpdateKeyModifiers();
	(void)x; (void)y; // Unused
}



// --------------------------------------------------------------
// Funkcja zwrotna wywolywana podczas nacisnieciu klawisza myszy
// --------------------------------------------------------------
void MyMouseButton( int glut_button, int state, int x, int y )
{
    if(glut_button == GLUT_LEFT_BUTTON && !Camera::ImGuiOn)
    {

        _mouse_buttonState = state;


        if( state == GLUT_DOWN )
        {
            _mouse_buttonX = x;
            _mouse_buttonY = y;
        }
    }
	// ImGui
	ImGuiIO& io = ImGui::GetIO();
	io.AddMousePosEvent((float)x, (float)y);
	int button = -1;
	if (glut_button == GLUT_LEFT_BUTTON) button = 0;
	if (glut_button == GLUT_RIGHT_BUTTON) button = 1;
	if (glut_button == GLUT_MIDDLE_BUTTON) button = 2;
	if (button != -1 && (state == GLUT_DOWN || state == GLUT_UP))
		io.AddMouseButtonEvent(button, state == GLUT_DOWN);
}

// --------------------------------------------------------------
// Funkcja zwrotna wywolywana podczas ruchu kursorem myszy
// --------------------------------------------------------------
void MyMouseMotion( int x, int y )
{
    if( _mouse_buttonState == GLUT_DOWN && !Camera::ImGuiOn)
    {
		Camera::CameraRotate_y += 2*(x - _mouse_buttonX)/(float)glutGet( GLUT_WINDOW_WIDTH );
        _mouse_buttonX = x;
		Camera::CameraRotate_x -= 2*(_mouse_buttonY - y)/(float)glutGet( GLUT_WINDOW_HEIGHT );
        _mouse_buttonY = y;
        glutPostRedisplay();
    }
	// ImGui
	ImGuiIO& io = ImGui::GetIO();
	io.AddMousePosEvent((float)x, (float)y);
}

// --------------------------------------------------------------
// Funkcja wywolywana podczas ruchu rolki myszy
// --------------------------------------------------------------
void MyMouseWheel(int button, int dir, int x, int y)
{
	if(!Camera::ImGuiOn)
		if (dir > 0)
		{
			// Zoom in
			Camera::CameraTranslate_x += 0.3f * -sin(Camera::CameraRotate_y);
			Camera::CameraTranslate_y += 0.3f * sin(Camera::CameraRotate_x);
			Camera::CameraTranslate_z += 0.3f * cos(Camera::CameraRotate_y);
		}
		else
		{
			// Zoom out
			Camera::CameraTranslate_x -= 0.3f * -sin(Camera::CameraRotate_y);
			Camera::CameraTranslate_y -= 0.3f * sin(Camera::CameraRotate_x);
			Camera::CameraTranslate_z -= 0.3f * cos(Camera::CameraRotate_y);
		}

    glutPostRedisplay();
	ImGuiIO& io = ImGui::GetIO();
	io.AddMousePosEvent((float)x, (float)y);
	if (dir != 0)
		io.AddMouseWheelEvent(0.0f, dir > 0 ? 1.0f : -1.0f);
	(void)button; // Unused
}


// cleanup
void myCleanup()
{
	// Audio Cleaning
	Audio::Cleanup();

	// Data Cleaning
	RenderData::FreeTexturesData();
	RenderData::FreeData();
	//TexturesData::FreeAllTexturesDataAfterLinking();
	GUI::freeData();

	// ImGui Cleanup
	ImGui_ImplOpenGL3_Shutdown();
	ImGui_ImplGLUT_Shutdown();
	ImGui::DestroyContext();
}


//#endif

