#include "RoundUI.h"
#include "CLifetime.h"
#include "CText.h"

dae::RoundUI::RoundUI(dae::Scene& scene, int roundNumber)
{
	m_RoundNumBeginning = CreateRoundNumBeginning(scene, roundNumber);
	m_RoundNum = CreateRoundNum(scene, roundNumber, glm::vec2{g_WindowWidth - 100, g_WindowHeight - 50});
}

void dae::RoundUI::SubscribeOnRoundTextEnd(const std::function<void()>& onRoundTextEnd)
{
	m_RoundNumBeginning->GetComponent<CLifeTime>()->SubscribeOnEndLifetime(onRoundTextEnd);
}

dae::GameObjectHandle dae::RoundUI::CreateRoundNum(dae::Scene& scene, int roundNumber, const glm::vec2& startPos)
{
	//-----

	dae::CText currentText{ "Emulogic-zrEw.ttf", 13 };
	currentText.SetText("ROUND " + std::to_string(roundNumber));
	currentText.SetColor(SDL_Color{ 255, 0, 0 });
	currentText.Center();

	dae::GameObjectHandle title{ scene.CreateGameObject() }; 
	title->SetName("RoundText");
	title->Transform().SetLocalPositionX(startPos.x);
	title->Transform().SetLocalPositionY(startPos.y);
	title->AddComponent(currentText);

	return title;

}

dae::GameObjectHandle dae::RoundUI::CreateRoundNumBeginning(dae::Scene& scene, int roundNumber)
{

	TransformData currentTransformData{};
	currentTransformData.Position.x = g_WindowWidth / 2.f;
	currentTransformData.Position.y = g_WindowHeight / 2.f;

	CText currentText{ "Emulogic-zrEw.ttf", 36 };
	currentText.SetText("Round " + std::to_string(roundNumber));
	currentText.Center();


	CLifeTime lifeTime{ 3.f, false };

	dae::GameObjectHandle roundText{ scene.CreateGameObject() };// , currentTransformData, currentText
	roundText->SetName("RoundStartText");
	roundText->Transform().SetLocalTransform(currentTransformData);
	roundText->AddComponent(currentText);
	roundText->AddComponent(lifeTime);

	return roundText;


}
