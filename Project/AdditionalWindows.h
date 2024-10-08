#pragma once

#include "imgui.h"
#include "imgui_impl_glut.h"
#include "imgui_impl_opengl3.h"
#include "imgui_stdlib.h"
#include "imgui_internal.h"

#include "Utilities.h"
#include "GUI.h"
#include "Camera.h"
#include "Light.h"
#include "Model.h"
#include <sstream>

static void HelpMarker(const char* desc)
{
	ImGui::TextDisabled("(?)");
	if (ImGui::IsItemHovered())
	{
		ImGui::BeginTooltip();
		ImGui::PushTextWrapPos(ImGui::GetFontSize() * 35.0f);
		ImGui::TextUnformatted(desc);
		ImGui::PopTextWrapPos();
		ImGui::EndTooltip();
	}
}

class SugestionsWindow
{
public:
	static void Draw();
};

class CtrlDispWindow
{
public:
	enum class ResizingDocking
	{
		Resizing_Docking_UP,
		Resizing_Docking_DOWN,
		Resizing_Docking_NONE
	};

	static float dockspaceMagnitude; //	dockspaceMagnitude = size/dockspaceMagnitude
	static float dockspaceRatioXSize;
	static bool dockspaceToggled;
	static bool DockspaceTurnedOnOff;
	static float dockspaceCurrentResize;
	static ResizingDocking resizingDirection;

	void DockspaceInitialization(ImGuiID dockspaceID, bool &ImGuiDockingWindowsInitFlag);
	void Draw();
};