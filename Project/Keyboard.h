#pragma once

#include "Utilities.h"
#include "Player.h"
#include "Ground.h"

class Keyboard
{
public:
	static bool KeyboardTable[128];
	static bool ShiftKey;

	static void Update();
	static void ButtonPressed();
	static void ButtonUp();
};