#pragma once

#include "Keyboard.h"

bool Keyboard::KeyboardTable[128];
bool Keyboard::ShiftKey = false;

void Keyboard::Update()
{

	if (!ShiftKey)
		StaticData::myPlayer.movementSpeed = StaticData::myPlayer.walkingSpeed;
	else
		StaticData::myPlayer.movementSpeed = StaticData::myPlayer.runningSpeed;


	if (KeyboardTable[27]) //exit
	{
		exit(0);
	}
	if (KeyboardTable['w'])
	{
		if (StaticData::myPlayer.TryMove(StaticData::myPlayer.movementSpeed, 0.0f))
			StaticData::myPlayer.Move(StaticData::myPlayer.movementSpeed, 0.0f);
	}
	if (KeyboardTable['s'])
	{
		if (StaticData::myPlayer.TryMove(-StaticData::myPlayer.movementSpeed, 0.0f))
			StaticData::myPlayer.Move(-StaticData::myPlayer.movementSpeed, 0.0f);
	}
	if (KeyboardTable['d'])
	{
		if (StaticData::myPlayer.TryMove(0.0f, -StaticData::myPlayer.movementSpeed))
			StaticData::myPlayer.Move(0.0f, -StaticData::myPlayer.movementSpeed);
	}
	if (KeyboardTable['a'])
	{
		if (StaticData::myPlayer.TryMove(0.0f, StaticData::myPlayer.movementSpeed))
			StaticData::myPlayer.Move(0.0f, StaticData::myPlayer.movementSpeed);
	}

	glm::vec3 cameraPos = glm::vec3(StaticData::myPlayer.matModel[3]);
	Camera::CameraTranslate_x = -cameraPos.x;
	Camera::CameraTranslate_y = -cameraPos.y;
	Camera::CameraTranslate_z = -cameraPos.z;

	StaticData::myPlayer.Rotate(Camera::CameraRotate_y);
	StaticData::myPlayer.Draw();
}

void Keyboard::ButtonPressed()
{

}

void Keyboard::ButtonUp()
{

}