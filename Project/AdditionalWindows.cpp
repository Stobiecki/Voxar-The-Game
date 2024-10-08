#include <iostream>
#include <cmath>

#include "AdditionalWindows.h"


//# define __CHECK_FOR_ERRORS { GLenum errCode ; if (( errCode = glGetError ()) != GL_NO_ERROR) printf(" Error (\% d): \%s in file \%s at line \%d !\n", errCode, gluErrorString(errCode), __FILE__, __LINE__); }


// SugestionsWindow
void SugestionsWindow::Draw()
{
	ImGuiStyle& style = ImGui::GetStyle();
	static ImGuiStyle refSavedStyle;
	static ImGuiWindowFlags windowFlags = ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoScrollbar;
	ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 10.f);
	ImGui::SetNextWindowBgAlpha(0.2f);
	ImGui::Begin("##HelperWindow", 0, windowFlags);
	ImGui::PopStyleVar();
	ImGui::TextUnformatted("Usable keys:");
	ImGui::BulletText("+ to zoom");
	ImGui::BulletText("- to zoom out");
	ImGui::BulletText("m to toggle on/off\n  this window");
	ImGui::Separator();
	{
		std::stringstream ss;
		ss << "Camera position: " << -Camera::CameraTranslate_x << " " << -Camera::CameraTranslate_y << " " << -Camera::CameraTranslate_z;
		ImGui::TextUnformatted(ss.str().c_str());
	}
	{
		std::stringstream ss;
		ss << "Camera rotation: " << Camera::CameraRotate_x << " " << Camera::CameraRotate_y;
		ImGui::TextUnformatted(ss.str().c_str());
		ImGui::SetNextItemWidth(70.f);
	}
	static glm::vec3 newCameraPos = glm::vec3(0.0f);
	ImGui::SliderFloat("##Set camera pos x", &newCameraPos.x, -100.0f, 100.0f); ImGui::SameLine();
	ImGui::SetNextItemWidth(70.f);
	ImGui::SliderFloat("##Set camera pos y", &newCameraPos.y, -100.0f, 100.0f); ImGui::SameLine();
	ImGui::SetNextItemWidth(70.f);
	ImGui::SliderFloat("##Set camera pos z", &newCameraPos.z, -100.0f, 100.0f);
	ImGui::TextUnformatted("Set camera position"); ImGui::SameLine();
	if(ImGui::Button("##Set camera position button", ImVec2(0.0f,0.0f)))
	{
		RenderData::matView = glm::translate(glm::mat4(1.0), newCameraPos);
	}

	ImGui::End();
	
	ImGui::SetNextWindowBgAlpha(0.2f);
	ImGui::Begin("##ControlPanel", 0, windowFlags);
	static ImGuiTabBarFlags tab_bar_flags = ImGuiTabBarFlags_None;

	static LightParam* l = RenderData::LoadedLight.rbegin()->second.get();
	static Model* m = RenderData::Models.rbegin()->second.get();
	static ModelInstanceInfo* i = &(m->modelInstanceInfo[0]);
	static const char* current_item_light = l->name.c_str();
	static const char* current_item_model = m->name.c_str();
	static std::string current_item_instance = "0"; // has to be std::string, 'cos it has no name, only id inside

	static float WholePi = 3.1415f * 8;
	static bool debugWindow = false;
	if (ImGui::BeginTable("TableForSettings", 1, ImGuiTableFlags_BordersInnerV))
	{
		ImGui::TableNextRow();
		ImGui::TableSetColumnIndex(0);
		if (ImGui::BeginTabBar("OptionTabBar", tab_bar_flags))
		{
			if (ImGui::BeginTabItem("Light"))
			{
				ImGui::Checkbox("shader type -", &Light::shaderMode); ImGui::SameLine();
				if(Light::shaderMode)
					ImGui::TextUnformatted("fragment");
				else
					ImGui::TextUnformatted("vertex");
				ImGui::Checkbox("light type -", &Light::lightMode); ImGui::SameLine();
				if (Light::lightMode)
					ImGui::TextUnformatted("Blinna-Phong");
				else
					ImGui::TextUnformatted("Phong");

				// light 1
				if (ImGui::BeginCombo("##lightCombo", current_item_light)) // The second parameter is the label previewed before opening the combo.
				{
					//int n = 0;
					for (auto& light : RenderData::LoadedLight)
					{
						bool is_selected = (light.second.get()->name == current_item_light); // You can store your selection however you want, outside or inside your objects
						if (ImGui::Selectable(light.second.get()->name.c_str(), is_selected))
						{
							current_item_light = light.second.get()->name.c_str();
							l = light.second.get();
						}
						if (is_selected)
							ImGui::SetItemDefaultFocus();   // You may set the initial focus when opening the combo (scrolling + for keyboard navigation support)
						//n++;
					}
					ImGui::EndCombo();
				}; ImGui::SameLine();
				ImGui::TextUnformatted("- Light"); ImGui::SameLine();
				ImGui::Checkbox("##Switch for light", &l->IsTurnedOn); ImGui::SameLine();
				HelpMarker("ctrl + LMB = direct change of value");

				// Ambient
				ImGui::SetNextItemWidth(70.f);
				ImGui::SliderFloat("##Light1 Ambient x", &l->Ambient.x, 0.0f, 1.0f); ImGui::SameLine();
				ImGui::SetNextItemWidth(70.f);
				ImGui::SliderFloat("##Light1 Ambient y", &l->Ambient.y, 0.0f, 1.0f); ImGui::SameLine();
				ImGui::SetNextItemWidth(70.f);
				ImGui::SliderFloat("##Light1 Ambient z", &l->Ambient.z, 0.0f, 1.0f); ImGui::SameLine();
				ImGui::TextUnformatted("Ambient");

				// Diffuse
				ImGui::SetNextItemWidth(70.f);
				ImGui::SliderFloat("##Light1 Diffuse x", &l->Diffuse.x, 0.0f, 1.0f); ImGui::SameLine();
				ImGui::SetNextItemWidth(70.f);
				ImGui::SliderFloat("##Light1 Diffuse y", &l->Diffuse.y, 0.0f, 1.0f); ImGui::SameLine();
				ImGui::SetNextItemWidth(70.f);
				ImGui::SliderFloat("##Light1 Diffuse z", &l->Diffuse.z, 0.0f, 1.0f); ImGui::SameLine();
				ImGui::TextUnformatted("Diffuse");

				// Specular
				ImGui::SetNextItemWidth(70.f);
				ImGui::SliderFloat("##Light1 Specular x", &l->Specular.x, 0.0f, 1.0f); ImGui::SameLine();
				ImGui::SetNextItemWidth(70.f);
				ImGui::SliderFloat("##Light1 Specular y", &l->Specular.y, 0.0f, 1.0f); ImGui::SameLine();
				ImGui::SetNextItemWidth(70.f);
				ImGui::SliderFloat("##Light1 Specular z", &l->Specular.z, 0.0f, 1.0f); ImGui::SameLine();
				ImGui::TextUnformatted("Specular");

				// Attenuation
				ImGui::SetNextItemWidth(70.f);
				ImGui::SliderFloat("##Light1 Attenuation x", &l->Attenuation.x, 0.0f, 1.0f); ImGui::SameLine();
				ImGui::SetNextItemWidth(70.f);
				ImGui::SliderFloat("##Light1 Attenuation y", &l->Attenuation.y, 0.0f, 1.0f); ImGui::SameLine();
				ImGui::SetNextItemWidth(70.f);
				ImGui::SliderFloat("##Light1 Attenuation z", &l->Attenuation.z, 0.0f, 1.0f); ImGui::SameLine();
				ImGui::TextUnformatted("Attenuation");

				// Position
				if (dynamic_cast<PointLightParam*>(l) || dynamic_cast<RelativeLightParam*>(l))
				{
					ImGui::SetNextItemWidth(70.f);
					ImGui::SliderFloat("##Light1 Position x", &l->Position.x, -40.0f, 40.0f); ImGui::SameLine();
					ImGui::SetNextItemWidth(70.f);
					ImGui::SliderFloat("##Light1 Position y", &l->Position.y, -40.0f, 40.0f); ImGui::SameLine();
					ImGui::SetNextItemWidth(70.f);
					ImGui::SliderFloat("##Light1 Position z", &l->Position.z, -40.0f, 40.0f); ImGui::SameLine();
					ImGui::TextUnformatted("Position");
				}
				else if (dynamic_cast<GlobalLightParam*>(l))
				{
					ImGui::SetNextItemWidth(70.f);
					ImGui::SliderFloat("##Light1 Angle x", &l->Position.x, -WholePi, WholePi); ImGui::SameLine();
					ImGui::SetNextItemWidth(70.f);
					ImGui::SliderFloat("##Light1 Angle y", &l->Position.y, -WholePi, WholePi); ImGui::SameLine();
					ImGui::SetNextItemWidth(70.f);
					ImGui::SliderFloat("##Light1 Angle z", &l->Position.z, -WholePi, WholePi); ImGui::SameLine();
					ImGui::TextUnformatted("Angle");
				}
				ImGui::Dummy(ImVec2(0.0, 10.f)); // Esthetics :>
				ImGui::EndTabItem();
			}
			if (ImGui::BeginTabItem("Shadows"))
			{
				static GlobalLightParam* gl = GlobalLightParam::globalLight;
				static PointLightParam* l = dynamic_cast<PointLightParam*>(RenderData::LoadedLight["light1"].get());
				ImGui::TextUnformatted("Global Light Shadow");

				ImGui::SetNextItemWidth(70.f);
				ImGui::SliderFloat("##gls Direction x", &gl->Position.x, -WholePi, WholePi); ImGui::SameLine();
				ImGui::SetNextItemWidth(70.f);
				ImGui::SliderFloat("##gls Direction y", &gl->Position.y, -WholePi, WholePi); ImGui::SameLine();
				ImGui::SetNextItemWidth(70.f);
				ImGui::SliderFloat("##gls Direction z", &gl->Position.z, -WholePi, WholePi); ImGui::SameLine();
				ImGui::TextUnformatted("Position");

				ImGui::SetNextItemWidth(70.f);
				ImGui::SliderFloat("##gls Position x", &gl->Light_Position.x, -400.0f, 400.0f); ImGui::SameLine();
				ImGui::SetNextItemWidth(70.f);
				ImGui::SliderFloat("##gls Position y", &gl->Light_Position.y, -400.0f, 400.0f); ImGui::SameLine();
				ImGui::SetNextItemWidth(70.f);
				ImGui::SliderFloat("##gls Position z", &gl->Light_Position.z, -400.0f, 400.0f); ImGui::SameLine();
				ImGui::TextUnformatted("Angle");
				ImGui::Separator();
				ImGui::Checkbox("Display global ligh shadow map", &StaticData::globalLightShadowMapOn);
				ImGui::SetNextItemWidth(70.f);
				ImGui::SliderFloat("Ortholeft", &GlobalLightParam::Ortholeft, -1000.0f, 1000.0f); ImGui::SameLine();
				ImGui::SetNextItemWidth(70.f);
				ImGui::SliderFloat("OrthoRight", &GlobalLightParam::OrthoRight, -1000.0f, 1000.0f);
				ImGui::SetNextItemWidth(70.f);
				ImGui::SliderFloat("OrthoBottom", &GlobalLightParam::OrthoBottom, -1000.0f, 1000.0f); ImGui::SameLine();
				ImGui::SetNextItemWidth(70.f);
				ImGui::SliderFloat("OrthoTop", &GlobalLightParam::OrthoTop, -1000.0f, 1000.0f);
				ImGui::SetNextItemWidth(70.f);
				ImGui::SliderFloat("OrthoNear", &GlobalLightParam::OrthoNear, -140.f, 80.f); ImGui::SameLine();
				ImGui::SetNextItemWidth(70.f);
				ImGui::SliderFloat("OrthoFar", &GlobalLightParam::OrthoFar, -80.f, 180.f);
				ImGui::Separator();
				ImGui::SetNextItemWidth(70.f);
				ImGui::SliderFloat("Shadow Bias", &GlobalLightParam::shadowBias, 0.0f, 1.0f);
				ImGui::InputFloat("##Shadow Bias non-slider", &GlobalLightParam::shadowBias, 0.0001f, 1.0f, "%.6f");
				ImGui::SetNextItemWidth(70.f);
				ImGui::SliderFloat("Bias Multiplier", &GlobalLightParam::biasMultiplier, 0.0001f, 1.0f, "%.6f");
				ImGui::Separator();
				ImGui::SetNextItemWidth(70.f);
				ImGui::SliderInt("DepthMap_Width", &GlobalLightParam::DepthMap_Width, 1, 50000); ImGui::SameLine();
				ImGui::SetNextItemWidth(70.f);
				ImGui::SliderInt("DepthMap_Height", &GlobalLightParam::DepthMap_Height, 1, 50000);
				ImGui::Separator();
				// pint light shadow
				ImGui::SetNextItemWidth(70.f);
				ImGui::SliderFloat("frustumNear", &l->frustumNear, -10.0f, 100.0f); ImGui::SameLine();
				ImGui::SetNextItemWidth(70.f);
				ImGui::SliderFloat("frustumFar", &l->frustumFar, 0.0f, 200.f);
				ImGui::SetNextItemWidth(70.f);
				ImGui::SliderInt("DepthMap Width", &l->DepthMap_Width, 0, 5000); ImGui::SameLine();
				ImGui::SetNextItemWidth(70.f);
				ImGui::SliderInt("DepthMap Height", &l->DepthMap_Height, 0, 5000);

				ImGui::EndTabItem();
			}
			// model m
			if (ImGui::BeginTabItem("Models"))
			{
				if (ImGui::BeginCombo("##ModelCombo", current_item_model)) // The second parameter is the label previewed before opening the combo.
				{
					//int n = 0;
					for (auto& models : RenderData::Models)
					{
						bool is_selected = (models.second.get()->name == current_item_model); // You can store your selection however you want, outside or inside your objects
						if (ImGui::Selectable(models.second.get()->name.c_str(), is_selected))
						{
							current_item_model = models.second.get()->name.c_str();
							m = models.second.get();
							i = &(m->modelInstanceInfo[0]);
						}
						if (is_selected)
							ImGui::SetItemDefaultFocus();   // You may set the initial focus when opening the combo (scrolling + for keyboard navigation support)
						//n++;
					}
					ImGui::EndCombo();
				}; ImGui::SameLine();
				// material
				ImGui::TextUnformatted(" - Material"); ImGui::SameLine();
				ImGui::Checkbox("##Switch for model", &m->visableModel);

				// Ambient
				ImGui::SetNextItemWidth(70.f);
				ImGui::SliderFloat("##Material Ambient x", &m->ambient.x, 0.0f, 1.0f); ImGui::SameLine();
				ImGui::SetNextItemWidth(70.f);
				ImGui::SliderFloat("##Material Ambient y", &m->ambient.y, 0.0f, 1.0f); ImGui::SameLine();
				ImGui::SetNextItemWidth(70.f);
				ImGui::SliderFloat("##Material Ambient z", &m->ambient.z, 0.0f, 1.0f); ImGui::SameLine();
				ImGui::TextUnformatted("Ambient");

				// Diffuse
				ImGui::SetNextItemWidth(70.f);
				ImGui::SliderFloat("##Material Diffuse x", &m->diffuse.x, 0.0f, 1.0f); ImGui::SameLine();
				ImGui::SetNextItemWidth(70.f);
				ImGui::SliderFloat("##Material Diffuse y", &m->diffuse.y, 0.0f, 1.0f); ImGui::SameLine();
				ImGui::SetNextItemWidth(70.f);
				ImGui::SliderFloat("##Material Diffuse z", &m->diffuse.z, 0.0f, 1.0f); ImGui::SameLine();
				ImGui::TextUnformatted("Diffuse");

				// Specular
				ImGui::SetNextItemWidth(70.f);
				ImGui::SliderFloat("##Material Specular x", &m->specular.x, 0.0f, 1.0f); ImGui::SameLine();
				ImGui::SetNextItemWidth(70.f);
				ImGui::SliderFloat("##Material Specular y", &m->specular.y, 0.0f, 1.0f); ImGui::SameLine();
				ImGui::SetNextItemWidth(70.f);
				ImGui::SliderFloat("##Material Specular z", &m->specular.z, 0.0f, 1.0f); ImGui::SameLine();
				ImGui::TextUnformatted("Specular");

				// Shininess
				ImGui::SetNextItemWidth(226.f);
				ImGui::SliderFloat("##Material Shininess", &m->shininess, 0.01f, 128.f); ImGui::SameLine();
				ImGui::TextUnformatted("Shininess");
				ImGui::Separator();

				// list of instances
				// instance i
				ImGui::TextUnformatted("List of model instances:");
				if (ImGui::BeginCombo("##ModelInstanceCombo", current_item_instance.c_str())) // The second parameter is the label previewed before opening the combo.
				{
					//int n = 0;
					for (auto& instance : m->modelInstanceInfo)
					{
						std::cout << "\ninstance.id: " << instance.id;
						bool is_selected = (instance.id == i->id); // You can store your selection however you want, outside or inside your objects
						if (ImGui::Selectable(std::to_string(instance.id).c_str(), is_selected))
						{
							current_item_instance = std::to_string(instance.id).c_str();
							i = &instance;
						}
						if (is_selected)
							ImGui::SetItemDefaultFocus();   // You may set the initial focus when opening the combo (scrolling + for keyboard navigation support)
						//n++;
					}
					ImGui::EndCombo();
				}; ImGui::SameLine();
				ImGui::TextUnformatted("- Disable");

				ImGui::SetNextItemWidth(70.f);
				ImGui::SliderFloat("##Model instance pos x", &i->pos.x, -100.0f, 100.0f); ImGui::SameLine();
				ImGui::SetNextItemWidth(70.f);
				ImGui::SliderFloat("##Model instance pos y", &i->pos.y, -100.0f, 100.0f); ImGui::SameLine();
				ImGui::SetNextItemWidth(70.f);
				ImGui::SliderFloat("##Model instance pos z", &i->pos.z, -100.0f, 100.0f); ImGui::SameLine();
				ImGui::TextUnformatted("Position");

				ImGui::SetNextItemWidth(70.f);
				ImGui::SliderFloat("##Model instance rot x", &i->rot.x, 0.0f, 360.0f); ImGui::SameLine();
				ImGui::SetNextItemWidth(70.f);
				ImGui::SliderFloat("##Model instance rot y", &i->rot.y, 0.0f, 360.0f); ImGui::SameLine();
				ImGui::SetNextItemWidth(70.f);
				ImGui::SliderFloat("##Model instance rot z", &i->rot.z, 0.0f, 360.0f); ImGui::SameLine();
				ImGui::TextUnformatted("Rotation");

				ImGui::EndTabItem();
			}
			if (ImGui::BeginTabItem("Audio"))
			{
				if (ImGui::Checkbox("Background audio", &Audio::IsBgAudioOn))
				{
					Audio::BgAudioChangeStatus = true;
				}
				if (Audio::BgAudioChangeStatus)
				{
					if (Audio::IsBgAudioOn == false)
						Audio::PauseBgMusic();
					else if (Audio::IsBgAudioOn == true)
						Audio::PlayBgMusic();
				}
				ImGui::SetNextItemWidth(226.f);
				if (ImGui::SliderFloat("##Material Shininess", &Audio::bgAudioVolume, 1.0f, 100.0f))
					Audio::SetBgMusicVolume(Audio::bgAudioVolume);

				ImGui::EndTabItem();
			}
			if (ImGui::BeginTabItem("Misc."))
			{
				ImGui::Checkbox("Debug window", &debugWindow);
				ImGui::Separator();

				//ImGui::TextUnformatted("Mist: "); ImGui::SameLine();
				ImGui::Checkbox("Mist: ", &StaticData::mistOn); ImGui::SameLine();
				ImGui::ColorEdit3("##MistShade", &StaticData::mistShade.x);
				ImGui::SliderFloat("##MistDensity", &StaticData::mistDensity, 0.0f, 0.9f);
				ImGui::Separator();
				ImGui::Checkbox("Dithering", &DitheringFBO::isTurnedOn);

				ImGui::EndTabItem();
			}			
			if (ImGui::BeginTabItem("Rendering"))
			{
				//ImGui::Checkbox("Debug window", &debugWindow);
				auto distance = glm::length(StaticData::myPlayer.matModel[3]);
				auto distance2 = glm::length(glm::vec3(StaticData::myPlayer.matModel[3]) - glm::vec3(-425.85f, -345.7f, 125.7f));
				ImGui::Text("distance: "); ImGui::SameLine();
				ImGui::Text(std::to_string(distance).c_str());
				ImGui::Text("distance from plant: "); ImGui::SameLine();
				ImGui::Text(std::to_string(distance2).c_str());
				ImGui::EndTabItem();
			}
		}
		ImGui::EndTable();

		if (debugWindow)
			ImGui::ShowDemoWindow();
	}

	ImGui::End();
	if (StaticData::globalLightShadowMapOn)
	{
		//CtrlDispWindow::BindFramebuffer();
		ImGui::Begin("Global light shadow map");
		ImGui::BeginChild("Global light shadow map Child");

		// we access the ImGui window size
		const float window_width = ImGui::GetContentRegionAvail().x;
		const float window_height = ImGui::GetContentRegionAvail().y;

		ImVec2 pos = ImGui::GetCursorScreenPos();

		ImGui::GetWindowDrawList()->AddImage(
			(void*)GlobalLightParam::globalLight->DepthMap_idTexture,
			ImVec2(pos.x, pos.y),
			ImVec2(pos.x + window_width, pos.y + window_height),
			ImVec2(0, 1),
			ImVec2(1, 0)
		);

		ImGui::EndChild();
		ImGui::End();
	}
	//if (StaticData::shadowMap3DOn)
	//{
	//	ImGui::Begin("Point light shadow map");
	//	ImGui::BeginChild("Point light shadow map Child");

	//	const float window_width = ImGui::GetContentRegionAvail().x;
	//	const float window_height = ImGui::GetContentRegionAvail().y;

	//	ImVec2 pos = ImGui::GetCursorScreenPos();
	//	PointLightParam* l = dynamic_cast<PointLightParam*>(RenderData::LoadedLight["light1"].get());
	//	ImGui::GetWindowDrawList()->AddImage(
	//		(void*)l->idTextureCube,
	//		ImVec2(pos.x, pos.y),
	//		ImVec2(pos.x + window_width, pos.y + window_height),
	//		ImVec2(0, 1),
	//		ImVec2(1, 0)
	//	);

	//	ImGui::EndChild();
	//	ImGui::End();
	//}
	//__CHECK_FOR_ERRORS
}


// CtrlDispWindow
float CtrlDispWindow::dockspaceMagnitude = 3.6f;
float CtrlDispWindow::dockspaceRatioXSize = 0;
bool CtrlDispWindow::dockspaceToggled = false;
bool CtrlDispWindow::DockspaceTurnedOnOff = true;
float CtrlDispWindow::dockspaceCurrentResize = 1.0f;
CtrlDispWindow::ResizingDocking CtrlDispWindow::resizingDirection = CtrlDispWindow::ResizingDocking::Resizing_Docking_NONE;

void CtrlDispWindow::DockspaceInitialization(ImGuiID dockspaceID,  bool &ImGuiDockingWindowsInitFlag)
{
	ImGuiViewport* viewport = ImGui::GetMainViewport();
	ImVec2 viewportPos = ImVec2(viewport->Pos);
	ImVec2 viewportSize = ImVec2(viewport->Size);
	float viewportOriginalSizeX = viewportSize.x;


	// scrolling when turning off, and on
	if(resizingDirection == ResizingDocking::Resizing_Docking_NONE && DockspaceTurnedOnOff == true)
		viewportSize.x = dockspaceRatioXSize = viewportSize.x / dockspaceMagnitude;
	if (resizingDirection != ResizingDocking::Resizing_Docking_NONE)
	{
		viewportSize.x = dockspaceRatioXSize;
		if (resizingDirection == ResizingDocking::Resizing_Docking_UP)
		{
			dockspaceRatioXSize += 2.00f;
			if((viewportOriginalSizeX / dockspaceMagnitude) <= dockspaceRatioXSize)
			{
				viewportSize.x = dockspaceRatioXSize = viewportOriginalSizeX / dockspaceMagnitude;
				resizingDirection = ResizingDocking::Resizing_Docking_NONE;
				dockspaceToggled = false;
				DockspaceTurnedOnOff = true;
			}
		}
		else // resizingDirection == ResizingDocking::Resizing_Docking_DOWN
		{
			dockspaceRatioXSize -= 2.00f;
			if ((viewportSize.x / dockspaceMagnitude) <= 0.0f)
			{
				viewportSize.x = dockspaceRatioXSize = 0.0f;
				resizingDirection = ResizingDocking::Resizing_Docking_NONE;
				dockspaceToggled = false;
				DockspaceTurnedOnOff = false;
			}
		}
	}

	if (DockspaceTurnedOnOff == true)
		viewportPos.x = viewportOriginalSizeX - viewportSize.x + 1; // pos diffrence, aka "shift"
	else
		viewportPos.x = viewportOriginalSizeX - dockspaceRatioXSize + 1;


	// setting up windows
	ImGui::SetNextWindowPos(viewportPos);
	ImGui::SetNextWindowSize(viewportSize);
	ImGui::SetNextWindowViewport(viewport->ID);
	ImGui::SetNextWindowBgAlpha(0.8f);

	ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0.0f);
	ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, -1.f);
	ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.0f, 0.0f));

	ImGuiWindowFlags dock_window_flags = 1;
	dock_window_flags |= ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize;
	dock_window_flags |= ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoBringToFrontOnFocus | ImGuiWindowFlags_NoNavFocus;

	ImGui::Begin("MyDockspace", NULL, dock_window_flags);

	if (ImGui::DockBuilderGetNode(dockspaceID) == NULL || ImGuiDockingWindowsInitFlag == true)
	{
		ImGuiDockingWindowsInitFlag = false;

		ImGuiID dock_main_id = dockspaceID;

		ImGui::DockBuilderRemoveNode(dockspaceID);
		ImGui::DockBuilderAddNode(dockspaceID, ImGuiDockNodeFlags_DockSpace);
		ImGui::DockBuilderSetNodeSize(dock_main_id, viewport->Size);

		auto control_panel_id = ImGui::DockBuilderSplitNode(dock_main_id, ImGuiDir_Down, 0.85f, nullptr, &dock_main_id);

		ImGui::DockBuilderDockWindow("##ControlPanel", control_panel_id);
		ImGui::DockBuilderDockWindow("##HelperWindow", dock_main_id);


		ImGui::DockBuilderGetNode(control_panel_id)->LocalFlags |= ImGuiDockNodeFlags_NoTabBar | ImGuiDockNodeFlags_NoResizeX  // | ImGuiDockNodeFlags_NoResizeX 
			| ImGuiDockNodeFlags_NoWindowMenuButton | ImGuiDockNodeFlags_NoDockingOverMe | ImGuiDockNodeFlags_NoDockingSplitMe;

		ImGui::DockBuilderGetNode(dock_main_id)->LocalFlags |= ImGuiDockNodeFlags_NoTabBar // | ImGuiDockNodeFlags_NoResizeX 
			| ImGuiDockNodeFlags_NoWindowMenuButton | ImGuiDockNodeFlags_NoDockingOverMe | ImGuiDockNodeFlags_NoDockingSplitMe;


		ImGui::DockBuilderFinish(dockspaceID);
	}

	ImGui::PopStyleVar(3);
	ImGui::DockSpace(dockspaceID, ImVec2(0.0f, 0.0f), 0);
	ImGui::End();
}

void CtrlDispWindow::Draw()
{
	// ImGui
	ImGui_ImplOpenGL3_NewFrame();
	ImGui_ImplGLUT_NewFrame();

	static bool DockspaceInitFlag = true;
	ImGuiID dockspaceID = ImGui::GetID("MyDockspace");
	DockspaceInitialization(dockspaceID, DockspaceInitFlag);

	SugestionsWindow::Draw();

	//ImGui::ShowDemoWindow();
	ImGui::Render();
	//ImGuiIO& io = ImGui::GetIO();
	ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
}
