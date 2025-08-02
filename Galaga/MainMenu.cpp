#include "MainMenu.h"
#include "Texture.h"
#include "Text.h"
#include "Settings.h"
#include "TextCreator.h"
#include "ParticleSystem.h"
#include "ScoreSaver.h"
#include "Misc_CreationFunctions.h"

dae::MainMenu::MainMenu() : m_MainMenuCreator{ std::make_shared<MainMenuCreator>() }
{
	auto creator = m_MainMenuCreator;

	auto createSceneFunc = [creator](Scene& scene)
		{			
			CreateHighscore(scene, "HI-SCORE",{ int(g_WindowWidth / 2.f),   int(g_WindowHeight * 0.5 / 6.f) });
			creator->CreateParticles(scene);
			creator->CreateTexture(scene, "galagaLogo.png", { int(g_WindowWidth / 2.f),   int(g_WindowHeight / 2.f) - 100 }, { 0.5f, 0.5f });

			CreateLastScore(scene, "1_UP", "GameModes/OnePlayerVsEnemies.txt",{ int(g_WindowWidth * 1 / 4.f),      int(g_WindowHeight * 0.5 / 6.f) });
			CreateHighscoreInLastScores(scene, "2_UP", "GameModes/TwoPlayersVsEnemies.txt", 2, { int(g_WindowWidth * 3 / 4.f),      int(g_WindowHeight * 0.5 / 6.f) });
			
			TextCreator textCreatorOne("TM AND © 1981 1988 NAMCO LTD.", { int(g_WindowWidth / 2.f),   int(g_WindowHeight * 4.5 / 6.f) }, 13, SDL_Color{ 255, 255 , 255 });
			TextCreator textCreatorTwo("BANDAI CO. LTD. IS LICENSED", { int(g_WindowWidth / 2.f),   int(g_WindowHeight * 5.0 / 6.f) }, 13, SDL_Color{ 255, 255 , 255 });
			TextCreator textCreatorThree("BY NINTENDO OF AMERICA INC.", { int(g_WindowWidth / 2.f),   int(g_WindowHeight * 5.5 / 6.f) }, 13, SDL_Color{ 255, 255 , 255 });
			scene.AddGameObjectHandle(textCreatorOne.GetGameObjectHandle());
			scene.AddGameObjectHandle(textCreatorTwo.GetGameObjectHandle());
			scene.AddGameObjectHandle(textCreatorThree.GetGameObjectHandle());

			creator->AddGridToScene(scene);
		};

	SceneManager::GetInstance().AddScene(MainMenu::Name(), createSceneFunc);
	
}

void dae::MainMenu::AddSceneChangeButton(const std::string& sceneName, const std::string& buttonName)
{
	const std::string* buttonNamePtr = buttonName.empty() ? &sceneName : &buttonName;
	m_MainMenuCreator->CreateButton( *buttonNamePtr, sceneName);
}

#pragma region Main Menu Creator

dae::MainMenu::MainMenuCreator::MainMenuCreator()
{
	CreateGrid();
}

void dae::MainMenu::MainMenuCreator::CreateParticles(dae::Scene& scene)
{
	auto gameObj = scene.CreateGameObject();

	CParticleSystem particleSystem{};

	particleSystem.SetNumber(50);
	particleSystem.SetColorRange({ 0,0,0 }, { 255, 255, 255 });
	particleSystem.SetMovementDirectionRange({ 0,1 }, { 0,1 });
	particleSystem.SetPositionRange({ 0,0 }, { g_WindowWidth, g_WindowHeight });
	particleSystem.SetMovementSpeedRange(50, 300);
	particleSystem.SetBounds(true, Rect{ 0, 0, g_WindowWidth , g_WindowHeight });

	gameObj->AddComponent(particleSystem);
}

void  dae::MainMenu::MainMenuCreator::CreateTexture(dae::Scene& scene, const std::string& fileName, const glm::vec2& pos, const glm::vec2& scale) {

	TransformData currentTransformData{};
	currentTransformData.Position.x = pos.x;
	currentTransformData.Position.y = pos.y;

	TransformData textureTransform{};
	textureTransform.Scale.x = scale.x;
	textureTransform.Scale.y = scale.y;

	CTextureHandle currentTexture{ fileName };
	currentTexture.SetTextureTransform(textureTransform);
	currentTexture.Center();

	dae::GameObjectHandle textureObj{ scene.CreateGameObject() }; 

	textureObj->Transform().SetLocalTransform(currentTransformData);
	textureObj->AddComponent(currentTexture);

}


void dae::MainMenu::MainMenuCreator::CreateTitleAndSubTitle(dae::Scene& scene, const std::string& titleText, const std::string& subTitleText, const glm::vec2& pos)
{

	TransformData currentTransformData{};
	currentTransformData.Position.x = pos.x;
	currentTransformData.Position.y = pos.y;

	CText currentText{ "Emulogic-zrEw.ttf", 13 };
	currentText.SetText(titleText);
	currentText.SetColor(SDL_Color{ 255,0,0 });
	currentText.Center();

	dae::GameObjectHandle titleGO{ scene.CreateGameObject() }; 
	titleGO->Transform().SetLocalTransform(currentTransformData);
	titleGO->AddComponent(currentText);

	currentTransformData.Position.y += currentText.Data()->GetPixelSize().y + 5;

	currentText.SetText(subTitleText);
	currentText.SetColor(SDL_Color{ 255,255,255 });
	currentText.Center();

	dae::GameObjectHandle subTitleGO{ scene.CreateGameObject() };
	subTitleGO->Transform().SetLocalTransform(currentTransformData);
	subTitleGO->AddComponent(currentText);

}

void dae::MainMenu::MainMenuCreator::CreateButton(const std::string& buttonName, std::string sceneNameToTransition)
{
	std::string name = buttonName;
	ButtonData data{};
	data.FontData.FontFile = "Emulogic-zrEw.ttf";
	data.FontData.FontSize = 13;
	data.Name = name;
	data.OnPress.Subscribe([sceneNameToTransition]() 
		{ 
			SceneManager::GetInstance().ChangeCurrentScene(sceneNameToTransition); 
		});
	data.SelectedColor = {255, 0, 0};
	data.UnselectedColor = { 255, 255, 255 };

	CButton button{ data };
	auto gameObj = std::make_shared<GameObject>();
	gameObj->AddComponent(button);

	GetGrid()->AddButton(gameObj);
}

void dae::MainMenu::MainMenuCreator::AddGridToScene(dae::Scene& scene)
{
	scene.AddGameObjectHandle(m_ButtonGrid);
}


void dae::MainMenu::MainMenuCreator::CreateGrid() {
	m_ButtonGrid = std::make_shared<GameObject>();
	
	ButtonGridData buttonGridData{ };
	buttonGridData.ColumnNumber = 1;
	buttonGridData.OffsetBetweenCols = 0;
	buttonGridData.OffsetBetweenRows = 50;
	buttonGridData.StartPos = {int(g_WindowWidth/2.f), int(g_WindowHeight/2.f) - 30 };
	CButtonGrid buttonGrid{ buttonGridData };

	m_ButtonGrid->AddComponent(buttonGrid);
}


#pragma endregion