#include "Scene_NameAssigner.h"
#include "Scene.h"
#include "CButtonGrid.h"
#include "Settings.h"
#include "TextCreator.h"
#include "EventTriggerCommand.h"
#include "CPlayerController.h"

namespace dae {

	Scene_NameAssigner::Scene_NameAssigner(const SceneData_NameAssigner& data)
	{
		m_NameAssignerSceneInternalData = std::make_shared<NameAssignerSceneInternalData>();
		auto internalData = m_NameAssignerSceneInternalData;
		internalData->NameAssignerSceneData = data;

		auto sceneCreationFunction = [internalData](Scene& scene)
			{
				for (size_t i = 0; i < internalData->NameAssignerSceneData.GameObjectsToAssignName.size(); i++)
				{
					internalData->NameAssignerSceneData.GameObjectsToAssignName[i]->SetName("");
				}
				TextCreator currentText{ "", {100, 50}, 25, {255, 255, 255} };
				scene.AddGameObjectHandle(currentText.GetGameObjectHandle());
				auto updateText = [internalData, currentText]() mutable
					{
						auto currentObj = internalData->NameAssignerSceneData.GameObjectsToAssignName[internalData->CurrentObjectIdx];
						const std::string& name = currentObj->GetName();
						currentText.SetText(name);
					};

				ButtonGridData buttonGridData{};
				buttonGridData.ColumnNumber = 8;
				buttonGridData.OffsetBetweenCols = 50;
				buttonGridData.OffsetBetweenRows = 50;
				buttonGridData.StartPos = {100, 150};

				auto buttonGridGO = scene.CreateGameObject();
				auto buttonGrid = buttonGridGO->AddComponent(CButtonGrid{ buttonGridData });
				auto playerController = buttonGridGO->AddComponent(CPlayerController{});
			

				ButtonData buttonData{};
				buttonData.FontData.FontFile = "Emulogic-zrEw.ttf";
				buttonData.FontData.FontSize = 40;

				const int maxCharsInName = 5;

				for (char currentLetter = 'A'; currentLetter <= 'Z'; currentLetter++)
				{
					buttonData.OnPress.UnsubscribeAll();
					buttonData.Name = currentLetter;
					buttonData.OnPress.Subscribe(
						[internalData, currentLetter, updateText, maxCharsInName]() mutable
						{
							auto currentObj = internalData->NameAssignerSceneData.GameObjectsToAssignName[internalData->CurrentObjectIdx];
							const std::string& name = currentObj->GetName();
							if (name.size() < maxCharsInName)
							{
								currentObj->SetName(name + currentLetter);
								updateText();
							}
						
						}); //add letter to name
					buttonData.SelectedColor = { 255,0,0 };
					buttonData.UnselectedColor = { 255, 255 , 255 };


					auto buttonGO = scene.CreateGameObject();
					buttonGO->AddComponent(CButton{ buttonData });
					buttonGrid->AddButton(buttonGO);
				}

				auto nextGameObject = [internalData, updateText, currentText](GameObject&) mutable
					{
						internalData->CurrentObjectIdx;

						if (internalData->CurrentObjectIdx < (internalData->NameAssignerSceneData.GameObjectsToAssignName.size() - 1))
						{
							++internalData->CurrentObjectIdx;

							updateText();
						}
						else
						{
							internalData->CurrentObjectIdx = 0;
							SceneManager::GetInstance().ChangeCurrentScene(internalData->NameAssignerSceneData.SceneNameUponCompletion);
						}
					};

				auto deleteLetter = [internalData, updateText](GameObject&) mutable
					{
						auto currentObj = internalData->NameAssignerSceneData.GameObjectsToAssignName[internalData->CurrentObjectIdx];
						std::string name = currentObj->GetName();
						if (!name.empty())
						{
							name.pop_back();
						}
						currentObj->SetName(name);
						updateText();
					};

				Event<GameObject&> nextGameObjectEvent{};
				nextGameObjectEvent.Subscribe(nextGameObject);

				Event<GameObject&> deleteLetterEvent{};
				deleteLetterEvent.Subscribe(deleteLetter);


				playerController->BindKey(dae::PlayerKeyboardKeyData{ ButtonState::BUTTON_DOWN, SDL_SCANCODE_RETURN,			 std::make_shared<EventTriggerCommand>(nextGameObjectEvent) });
				playerController->BindKey(dae::PlayerGamepadKeyData{  ButtonState::BUTTON_DOWN, GamepadButton::RightShoulder,	 std::make_shared<EventTriggerCommand>(nextGameObjectEvent) });

				playerController->BindKey(dae::PlayerKeyboardKeyData{ ButtonState::BUTTON_DOWN, SDL_SCANCODE_BACKSPACE,			std::make_shared<EventTriggerCommand>(deleteLetterEvent) });
				playerController->BindKey(dae::PlayerGamepadKeyData{ ButtonState::BUTTON_DOWN, GamepadButton::LeftShoulder,	 std::make_shared<EventTriggerCommand>(deleteLetterEvent) });

				

				TextCreator charsIndication{ "MAX " + std::to_string(maxCharsInName) + " CHARACTERS IN NAME", {g_WindowWidth / 2.f,  g_WindowHeight - 120}, 12, {255, 255, 255}};
				scene.AddGameObjectHandle(charsIndication.GetGameObjectHandle());

				TextCreator enterText{ "PRESS SPACE OR X TO INPUT LETTER", {g_WindowWidth / 2.f,  g_WindowHeight - 90}, 12, {255, 255, 255} };
				scene.AddGameObjectHandle(enterText.GetGameObjectHandle());

				TextCreator deleteText{ "PRESS LEFT SHOULDER OR BACKSPACE TO DELETE", {g_WindowWidth / 2.f,  g_WindowHeight - 60}, 12, {255, 255, 255} };
				scene.AddGameObjectHandle(deleteText.GetGameObjectHandle());

				TextCreator acceptText{ "PRESS RIGHT SHOULDER OR RETURN TO ACCEPT", {g_WindowWidth / 2.f,  g_WindowHeight - 30}, 12, {255, 255, 255} };
				scene.AddGameObjectHandle(acceptText.GetGameObjectHandle());

			};

		SceneManager::GetInstance().AddScene(data.SceneName, sceneCreationFunction);

	}

}