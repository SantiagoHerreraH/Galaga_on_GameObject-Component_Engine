#include "Player.h"
#include "PlayerController.h"
#include "Collider.h"
#include "Rigidbody.h"
#include "Movement.h"
#include "Texture.h"
#include "StatSystem.h"
#include "Text.h"
#include "TimerSystem.h"
#include "EventTriggerCommand.h"

#include "PlayerLife.h"
#include "GalagaStats.h"
#include "CollisionLayers.h"
#include "StatDisplayer.h"
#include "Bullet.h"
#include "Gun.h"
#include "CNameDisplayer.h"
#include "Misc_CreationFunctions.h"
#include "Audio.h"
#include "ServiceLocator.h"

#undef max
#undef min
#include <limits>

dae::Player::Player(const glm::vec2& startPos, float zRotation, int playerNum, const PlayerType& playerType)
{
	//------- PLAYER

	dae::GameObjectHandle currentPlayer = std::make_shared<GameObject>();

	//------- SHOOTING PIVOT

	dae::GameObjectHandle shootingPivot = std::make_shared<GameObject>();
	m_ShootingPivot = shootingPivot;
	shootingPivot->Transform().SetLocalPositionY(-10);
	shootingPivot->Transform().SetParent(*currentPlayer, dae::ETransformReparentType::KeepLocalTransform);

	//----- COMPONENTS -----

	Rect rect{};
	rect.Height = 32;
	rect.Width = 32;

	dae::CCollider collider{ rect, (int)playerType.PlayerCollisionLayer };
	collider.CenterRect();

	currentPlayer->AddComponent(collider);

	//-----

	dae::CRigidbody2D rigidBody{};
	currentPlayer->AddComponent(rigidBody);

	//-----

	dae::CMovement2D movement{};
	movement.SetMaxSpeed(300);
	currentPlayer->AddComponent(movement);

	//----

	TransformData textDisplayerOffset{};
	textDisplayerOffset.Position.x = 60;

	TextData textDisplayerTextData{};
	textDisplayerTextData.Color = { 255, 255, 255 };
	textDisplayerTextData.FontData.FontFile = "Emulogic-zrEw.ttf";
	textDisplayerTextData.FontData.FontSize = 15;

	CNameDisplayer nameDisplayer{ textDisplayerTextData , textDisplayerOffset };
	currentPlayer->AddComponent(nameDisplayer);

	//-----

	TransformData textureTransform{};
	textureTransform.Scale = { 0.5f, 0.5f , 0.5f };
	dae::CTextureHandle currentTexture{ playerType.TextureName };// { "galaga.png" };
	currentTexture.SetTextureTransform(textureTransform);
	currentTexture.Center();

	currentPlayer->AddComponent(currentTexture);

	//-----

	auto playerControllerRef = currentPlayer->AddComponent(CPlayerController{});

	//-----

	CStatController statController{};

	IntStat initialHealth{ 3, 3, 3 };


	constexpr int maxInt = std::numeric_limits<int>::max();

	IntStat initialPoints{ 0, maxInt, maxInt };
	IntStat initialShotsFired{ 0, maxInt, maxInt };
	IntStat initialNumberOfHits{ 0, maxInt, maxInt };

	statController.CreateStat(StatType::Health, initialHealth);

	statController.CreateStat(StatType::Points, initialPoints);
	statController.CreateStat(StatType::ShotsFired, initialShotsFired);
	statController.CreateStat(StatType::NumberOfHits, initialNumberOfHits);

	currentPlayer->AddComponent(statController);

	//----

	StatDisplayData statDisplayData{};
	statDisplayData.StatNameTextData.FontData.FontFile = "Emulogic-zrEw.ttf";
	statDisplayData.StatNameTextData.FontData.FontSize = 13;
	statDisplayData.StatNameTextData.Color = {255, 0, 0 };
	statDisplayData.StatNameTextData.Text = "1UP ";
	statDisplayData.StatTypeToDisplay = StatType::Points;
	statDisplayData.StatValueColor = { 255, 255, 255 };
	statDisplayData.StatValueBaseOffsetMultiplierX = 0;
	statDisplayData.StatValueBaseOffsetMultiplierY = 1;
	statDisplayData.StatValueOffsetFromStatName = {0,5};
	statDisplayData.UpdateAutomatically = true;
	statDisplayData.Where = { g_WindowWidth * 4.3f / 5.f , (g_WindowHeight * 1.6f / 5.f) + (50 * playerNum) };

	CStatDisplayer statDisplayer{ statDisplayData };

	currentPlayer->AddComponent(statDisplayer);

	//----

	CPlayerLife playerHealthComponent{ 3, startPos, 3.f, playerNum };

	currentPlayer->AddComponent(playerHealthComponent);

	//----

	AudioData dieAudioData{};
	dieAudioData.File = "Sound/PlayerDeathSound.wav";
	dieAudioData.LoopAmount = 0;


	m_DeathAudio = currentPlayer->AddComponent(CAudio{ dieAudioData });

	//----
	
	auto weaponType = playerType.WeaponType;

	weaponType->Create(currentPlayer);

	//-----

	currentPlayer->Transform().SetLocalPositionX(startPos.x);
	currentPlayer->Transform().SetLocalPositionY(startPos.y);
	currentPlayer->Transform().SetLocalRotationZ(zRotation);

	//----- ACTIONS -----

	 const float limitXLeft = 50;
	 const float limitXRight = 200;
	 const float limitY = 50;

	// - Movement Action
	auto moveLeft	= [limitXLeft](GameObject& gameObj) mutable
		{
			if (gameObj.Transform().GetWorldTransform().Position.x < limitXLeft)
			{
				gameObj.Transform().SetLocalPositionX(limitXLeft);
			}
			else
			{
				gameObj.GetComponent<CMovement2D>()->AddSingleFrameMovementInput(glm::vec2{ -1,  0 });
			}
		};
	auto moveRight	= [limitXRight](GameObject& gameObj) mutable
		{
			if (gameObj.Transform().GetWorldTransform().Position.x > (g_WindowWidth - limitXRight))
			{
				gameObj.Transform().SetLocalPositionX(g_WindowWidth - limitXRight);
			}
			else
			{
				gameObj.GetComponent<CMovement2D>()->AddSingleFrameMovementInput(glm::vec2{ 1,  0 });
			}		
		};
	auto moveUp		= [limitY](GameObject& gameObj) mutable
		{
			if (gameObj.Transform().GetWorldTransform().Position.y < limitY)
			{
				gameObj.Transform().SetLocalPositionY(limitY);
			}
			else
			{
				gameObj.GetComponent<CMovement2D>()->AddSingleFrameMovementInput(glm::vec2{ 0,  -1 });
			}	
		};
	auto moveDown	= [limitY](GameObject& gameObj) mutable
		{
			if (gameObj.Transform().GetWorldTransform().Position.y > g_WindowHeight - limitY)
			{
				gameObj.Transform().SetLocalPositionY(g_WindowHeight - limitY);
			}
			else
			{
				gameObj.GetComponent<CMovement2D>()->AddSingleFrameMovementInput(glm::vec2{ 0,  1 });
			}	
		};


	// - Shooting Action

	auto shootAction = [currentPlayer, weaponType](GameObject&) mutable { weaponType->Execute(*currentPlayer); };

	//Events

	Event<GameObject&> shootEvent{};
	Event<GameObject&> moveLeftEvent{};
	Event<GameObject&> moveRightEvent{};
	Event<GameObject&> moveUpEvent{};
	Event<GameObject&> moveDownEvent{};

	moveLeftEvent.Subscribe(moveLeft);
	moveRightEvent.Subscribe(moveRight);
	moveUpEvent.Subscribe(moveUp);
	moveDownEvent.Subscribe(moveDown);
	shootEvent.Subscribe(shootAction);

	playerControllerRef->BindKey(dae::PlayerKeyboardKeyData{ ButtonState::BUTTON_PRESSED, SDL_SCANCODE_SPACE,	std::make_shared<EventTriggerCommand>(shootEvent) });
	playerControllerRef->BindKey(dae::PlayerKeyboardKeyData{ ButtonState::BUTTON_PRESSED, SDL_SCANCODE_LEFT,	std::make_shared<EventTriggerCommand>(moveLeftEvent) });
	playerControllerRef->BindKey(dae::PlayerKeyboardKeyData{ ButtonState::BUTTON_PRESSED, SDL_SCANCODE_RIGHT,	std::make_shared<EventTriggerCommand>(moveRightEvent) });

	if (playerType.HasVerticalMovement)
	{
		playerControllerRef->BindKey(dae::PlayerKeyboardKeyData{ ButtonState::BUTTON_PRESSED, SDL_SCANCODE_UP,		std::make_shared<EventTriggerCommand>(moveUpEvent) });
		playerControllerRef->BindKey(dae::PlayerKeyboardKeyData{ ButtonState::BUTTON_PRESSED, SDL_SCANCODE_DOWN,	std::make_shared<EventTriggerCommand>(moveDownEvent) });
	}

	playerControllerRef->BindKey(dae::PlayerKeyboardKeyData{ ButtonState::BUTTON_PRESSED, SDL_SCANCODE_A,	 std::make_shared<EventTriggerCommand>(moveLeftEvent) });
	playerControllerRef->BindKey(dae::PlayerKeyboardKeyData{ ButtonState::BUTTON_PRESSED, SDL_SCANCODE_D,	 std::make_shared<EventTriggerCommand>(moveRightEvent) });

	if (playerType.HasVerticalMovement)
	{
		playerControllerRef->BindKey(dae::PlayerKeyboardKeyData{ ButtonState::BUTTON_PRESSED, SDL_SCANCODE_W,	 std::make_shared<EventTriggerCommand>(moveUpEvent) });
		playerControllerRef->BindKey(dae::PlayerKeyboardKeyData{ ButtonState::BUTTON_PRESSED, SDL_SCANCODE_S,	 std::make_shared<EventTriggerCommand>(moveDownEvent) });
	}


	playerControllerRef->BindKey(dae::PlayerGamepadKeyData{ ButtonState::BUTTON_PRESSED, GamepadButton::ButtonX,	 std::make_shared<EventTriggerCommand>(shootEvent) });
	playerControllerRef->BindKey(dae::PlayerGamepadKeyData{ ButtonState::BUTTON_PRESSED, GamepadButton::DpadLeft,	 std::make_shared<EventTriggerCommand>(moveLeftEvent) });
	playerControllerRef->BindKey(dae::PlayerGamepadKeyData{ ButtonState::BUTTON_PRESSED, GamepadButton::DpadRight,	 std::make_shared<EventTriggerCommand>(moveRightEvent) });
	
	if (playerType.HasVerticalMovement)
	{
		playerControllerRef->BindKey(dae::PlayerGamepadKeyData{ ButtonState::BUTTON_PRESSED, GamepadButton::DpadUp,		 std::make_shared<EventTriggerCommand>(moveUpEvent) });
		playerControllerRef->BindKey(dae::PlayerGamepadKeyData{ ButtonState::BUTTON_PRESSED, GamepadButton::DpadDown,	 std::make_shared<EventTriggerCommand>(moveDownEvent) });
	}


	auto toggleMute = [](GameObject&)

		{
			ServiceLocator::GetInstance().GetService<IAudioService>()->ToggleMute();
		};


	Event<GameObject&> toggleMuteEvent{};
	toggleMuteEvent.Subscribe(toggleMute);

	playerControllerRef->BindKey(dae::PlayerKeyboardKeyData{ ButtonState::BUTTON_DOWN, SDL_SCANCODE_F2,				std::make_shared<EventTriggerCommand>(toggleMuteEvent) });
	playerControllerRef->BindKey(dae::PlayerGamepadKeyData{ ButtonState::BUTTON_DOWN, GamepadButton::Back,	        std::make_shared<EventTriggerCommand>(toggleMuteEvent) });

	m_CurrentPlayer = currentPlayer;

	m_CurrentExplosion = CreateExplosion("playerExplosion.png");
	 

	
}

dae::GameObjectHandle dae::Player::GetGameObjectHandle()
{
	return m_CurrentPlayer;
}

void dae::Player::AddScene(Scene& scene)
{
	scene.AddGameObjectHandle(m_CurrentPlayer);
	scene.AddGameObjectHandle(m_ShootingPivot);
	scene.AddGameObjectHandle(m_CurrentExplosion);

	auto explosion = m_CurrentExplosion;
	auto currentPlayer = m_CurrentPlayer;
	auto dieAudio = m_DeathAudio;

	SubscribeOnPlayerDespawnFromDamage([explosion, currentPlayer, dieAudio]() mutable{
		dieAudio->Play();
		explosion->GetComponent<CLifeTime>()->Respawn(currentPlayer->Transform().GetWorldTransform().Position);
		});
}

void dae::Player::SubscribeOnPlayerDie(const std::function<void()>& func) {

	m_CurrentPlayer->GetComponent<CPlayerLife>()->SubscribeOnPlayerDie(func);
}
void dae::Player::SubscribeOnPlayerDespawnFromDamage(const std::function<void()>& func) {

	m_CurrentPlayer->GetComponent<CPlayerLife>()->SubscribeOnPlayerDespawnFromDamage(func);
}
void dae::Player::SubscribeOnPlayerRespawnFromDamage(const std::function<void()>& func) {

	m_CurrentPlayer->GetComponent<CPlayerLife>()->SubscribeOnPlayerRespawnFromDamage(func);
}