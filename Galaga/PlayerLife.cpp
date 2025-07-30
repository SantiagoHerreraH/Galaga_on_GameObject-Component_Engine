#include "PlayerLife.h"
#include "GameObject.h"
#include "Settings.h"
#include "GameInput.h"
#include "PlayerController.h"
#include "TimerSystem.h"
#include "Text.h"

void dae::CPlayerLife::Start()
{
	CPlayerController* playerController = GetPlayerController();
	CStatController* currentStatController = GetStatController();


	Event<>& onHealthZeroOrLess = currentStatController->OnCurrentStatZeroOrLess(StatType::Health);
	Event<int>& onHealthChange = currentStatController->OnCurrentStatChange(StatType::Health);

	std::vector<dae::GameObjectHandle> healthIndicators =
		CreatePlayerHealthIndicator(
			m_PlayerNum,
			m_MaxHealth,
			glm::vec2{ g_WindowWidth * 4.3f / 5.f,
			(g_WindowHeight * 2.3f / 5.f) + (m_PlayerNum * 100) }, 1, 1);

	auto changeHealthIndicators = [healthIndicators](int currentHealth) mutable
		{
			if (currentHealth < 0)
			{
				currentHealth = 0;
			}
			for (int i = 0; i < currentHealth; i++)
			{
				healthIndicators[i]->SetActive(true);
			}
			for (int i = currentHealth; i < healthIndicators.size(); i++) //cast to size_t in case it is negative
			{
				healthIndicators[i]->SetActive(false);
			}
		};

	onHealthChange.Subscribe(changeHealthIndicators);

	CPlayerLife* self{ this };
	glm::vec2 respawnPoint{ m_RespawnPos };

	self->Owner().Transform().SetLocalPositionX(respawnPoint.x);
	self->Owner().Transform().SetLocalPositionY(respawnPoint.y);

	auto respawnPlayer = [self, respawnPoint, healthIndicators]() mutable
		{
			self->Owner().Transform().SetLocalPositionX(respawnPoint.x);
			self->Owner().Transform().SetLocalPositionY(respawnPoint.y);
			self->Owner().SetActive(true);
			self->m_OnRespawnFromDamage.Invoke();
		};

	TimerSystem* TIMERSYSTEM = &TimerSystem::GetCurrent();
	TimerKey respawnKey = TIMERSYSTEM->TriggerFunctionAfterSeconds(respawnPlayer, m_SecondsToRespawn, false, true);

	auto despawnAndRespawnAfterTime = [self, respawnKey, TIMERSYSTEM](int) mutable {
		
		self->Owner().SetActive(false);
		self->m_OnDespawnFromDamage.Invoke();

		if (TIMERSYSTEM->HasTimer(respawnKey))//because this might be called after scene reset
		{
			TIMERSYSTEM->RestartTimer(respawnKey);
		}

		};


	onHealthChange.Subscribe(despawnAndRespawnAfterTime); 
	onHealthZeroOrLess.Subscribe([self]() mutable {

		self->m_OnDie.Invoke();

		});

	for (size_t i = 0; i < healthIndicators.size(); i++)
	{
		SceneManager::GetInstance().GetCurrentScene().AddGameObjectHandle(healthIndicators[i]);
	}

}

void dae::CPlayerLife::SubscribeOnPlayerDie(const std::function<void()>& func)
{
	m_OnDie.Subscribe(func);
}

void dae::CPlayerLife::SubscribeOnPlayerDespawnFromDamage(const std::function<void()>& func)
{
	m_OnDespawnFromDamage.Subscribe(func);
}

void dae::CPlayerLife::SubscribeOnPlayerRespawnFromDamage(const std::function<void()>& func)
{
	m_OnRespawnFromDamage.Subscribe(func);
}

dae::CStatController* dae::CPlayerLife::GetStatController()
{
	CStatController* currentStatController = Owner().GetComponent<CStatController>();
	if (!currentStatController)
	{
		IntStat health{ m_MaxHealth, m_MaxHealth, m_MaxHealth };
		CStatController statController{};
		statController.CreateStat(StatType::Health, health);
		Owner().AddComponent(statController);
		currentStatController = Owner().GetComponent<CStatController>();
	}

	return currentStatController;
}

dae::CPlayerController* dae::CPlayerLife::GetPlayerController()
{
	CPlayerController* playerController = Owner().GetComponent<CPlayerController>();
	if (!playerController)
	{
		Owner().AddComponent(CPlayerController{});
		playerController = Owner().GetComponent<CPlayerController>();
	}
	return playerController;
}

std::vector<dae::GameObjectHandle> dae::CPlayerLife::CreatePlayerHealthIndicator(dae::PlayerId playerId, int playerHealth, const glm::vec2& startPos, float symbolOffset, float textOffset)
{


	//----- PLAYER INDICATOR

	dae::CText currentText{ "Emulogic-zrEw.ttf", 12 };
	currentText.SetText("PLAYER " + std::to_string(playerId + 1));
	currentText.SetColor(SDL_Color{ 255, 0, 0 });
	currentText.Center();

	//-----

	dae::GameObjectHandle healthWords = std::make_shared<GameObject>();
	healthWords->Transform().SetLocalPositionX(startPos.x);
	healthWords->Transform().SetLocalPositionY(startPos.y);

	healthWords->AddComponent(currentText);

	//----- HEALTH INDICATORS

	std::vector<dae::GameObjectHandle> healthIndicators{};

	//---texture
	dae::TransformData textureTransformData{};
	textureTransformData.Scale = { 0.5f, 0.5f , 0.5f };

	dae::CTextureHandle currentTexture{ "galaga.png" };
	currentTexture.SetTextureTransform(textureTransformData);
	glm::vec2 textureSize = currentTexture.Data()->GetPixelSize();

	textureSize.x *= textureTransformData.Scale.x;
	textureSize.y *= textureTransformData.Scale.y;

	float yOffset = textureSize.y + textOffset;
	float xOffset = startPos.x - (((textureSize.x + symbolOffset) * playerHealth) / 2.f);

	for (size_t i = 0; i < playerHealth; i++)
	{

		dae::GameObjectHandle currentHealthIndicator = std::make_shared<GameObject>();
		currentHealthIndicator->Transform().SetLocalPositionX(xOffset + ((textureSize.x + symbolOffset) * i));
		currentHealthIndicator->Transform().SetLocalPositionY(startPos.y + yOffset);

		currentHealthIndicator->AddComponent(currentTexture);

		healthIndicators.push_back(currentHealthIndicator);
	}

	return healthIndicators;
}
