#include "Player.h"
#include "CPlayerController.h"
#include "CCollider.h"
#include "CRigidbody2D.h"
#include "CMovement2D.h"
#include "CTextureHandle.h"
#include "CStatController.h"
#include "CText.h"
#include "TimerSystem.h"
#include "EventTriggerCommand.h"

#include "CPlayerLife.h"
#include "CGameStatController.h"
#include "CollisionLayers.h"
#include "CStatDisplayer.h"
#include "Create_Bullet.h"
#include "CGun.h"
#include "CNameDisplayer.h"
#include "Create_Explosion.h"
#include "CAudio.h"
#include "ServiceLocator.h"
#include "MoveCommand.h"

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
	textureTransform.Scale = { playerType.TextureScale, playerType.TextureScale, playerType.TextureScale };
	dae::CTextureHandle currentTexture{ playerType.TextureName };// { "galaga.png" };
	currentTexture.SetTextureTransform(textureTransform);
	currentTexture.Center();

	currentPlayer->AddComponent(currentTexture);

	//-----

	Rect rect{};

	auto textureSize = currentTexture.GetScaledSize();

	rect.Height = textureSize.y;
	rect.Width = textureSize.x;

	dae::CCollider collider{ rect, (int)playerType.PlayerCollisionLayer };
	collider.CenterRect();

	currentPlayer->AddComponent(collider);

	//-----

	auto playerControllerRef = currentPlayer->AddComponent(CPlayerController{});

	//-----

	CGameStatController statController{};

	GameStat initialHealth{ 3, 3, 3 };


	constexpr int maxInt = std::numeric_limits<int>::max();

	GameStat initialPoints{ 0, maxInt, maxInt };
	GameStat initialShotsFired{ 0, maxInt, maxInt };
	GameStat initialNumberOfHits{ 0, maxInt, maxInt };

	statController.CreateStat(GameStatType::Health, initialHealth);

	statController.CreateStat(GameStatType::Points, initialPoints);
	statController.CreateStat(GameStatType::ShotsFired, initialShotsFired);
	statController.CreateStat(GameStatType::NumberOfHits, initialNumberOfHits);

	currentPlayer->AddComponent(statController);

	//----

	StatDisplayData statDisplayData{};
	statDisplayData.StatNameTextData.FontData.FontFile = "Emulogic-zrEw.ttf";
	statDisplayData.StatNameTextData.FontData.FontSize = 13;
	statDisplayData.StatNameTextData.Color = {255, 0, 0 };
	statDisplayData.StatNameTextData.Text = "1UP ";
	statDisplayData.StatTypeToDisplay = GameStatType::Points;
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

	 Rect movementBounds{};
	 movementBounds.Left = 50;
	 movementBounds.Width = (g_WindowWidth - movementBounds.Left) - 200;
	 movementBounds.Top = 50;
	 movementBounds.Height = g_WindowHeight - movementBounds.Top - movementBounds.Top;

	 auto moveLeft	{ std::make_shared<MoveCommand>( glm::vec2{-1, 0}, movementBounds ) };
	 auto moveRight	{ std::make_shared<MoveCommand>(glm::vec2{1, 0}, movementBounds ) };
	 auto moveUp	{ std::make_shared<MoveCommand>(glm::vec2{0, -1}, movementBounds ) };
	 auto moveDown	{ std::make_shared<MoveCommand>(glm::vec2{0, 1}, movementBounds ) };


	playerControllerRef->BindKey(dae::PlayerKeyboardKeyData{ ButtonState::BUTTON_PRESSED, SDL_SCANCODE_SPACE,	weaponType });
	playerControllerRef->BindKey(dae::PlayerKeyboardKeyData{ ButtonState::BUTTON_PRESSED, SDL_SCANCODE_LEFT,	moveLeft });
	playerControllerRef->BindKey(dae::PlayerKeyboardKeyData{ ButtonState::BUTTON_PRESSED, SDL_SCANCODE_RIGHT,	moveRight });

	if (playerType.HasVerticalMovement)
	{
		playerControllerRef->BindKey(dae::PlayerKeyboardKeyData{ ButtonState::BUTTON_PRESSED, SDL_SCANCODE_UP,		moveUp	 });
		playerControllerRef->BindKey(dae::PlayerKeyboardKeyData{ ButtonState::BUTTON_PRESSED, SDL_SCANCODE_DOWN,	moveDown });
	}

	playerControllerRef->BindKey(dae::PlayerKeyboardKeyData{ ButtonState::BUTTON_PRESSED, SDL_SCANCODE_A,	 moveLeft });
	playerControllerRef->BindKey(dae::PlayerKeyboardKeyData{ ButtonState::BUTTON_PRESSED, SDL_SCANCODE_D,	 moveRight });

	if (playerType.HasVerticalMovement)
	{
		playerControllerRef->BindKey(dae::PlayerKeyboardKeyData{ ButtonState::BUTTON_PRESSED, SDL_SCANCODE_W,	 moveUp });
		playerControllerRef->BindKey(dae::PlayerKeyboardKeyData{ ButtonState::BUTTON_PRESSED, SDL_SCANCODE_S,	 moveDown });
	}


	playerControllerRef->BindKey(dae::PlayerGamepadKeyData{ ButtonState::BUTTON_PRESSED, GamepadButton::ButtonX,	 weaponType });
	playerControllerRef->BindKey(dae::PlayerGamepadKeyData{ ButtonState::BUTTON_PRESSED, GamepadButton::DpadLeft,	 moveLeft });
	playerControllerRef->BindKey(dae::PlayerGamepadKeyData{ ButtonState::BUTTON_PRESSED, GamepadButton::DpadRight,	 moveRight });
	
	if (playerType.HasVerticalMovement)
	{
		playerControllerRef->BindKey(dae::PlayerGamepadKeyData{ ButtonState::BUTTON_PRESSED, GamepadButton::DpadUp,		 moveUp });
		playerControllerRef->BindKey(dae::PlayerGamepadKeyData{ ButtonState::BUTTON_PRESSED, GamepadButton::DpadDown,	 moveDown });
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

	m_CurrentExplosion = Create_Explosion("playerExplosion.png");
	 

	
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