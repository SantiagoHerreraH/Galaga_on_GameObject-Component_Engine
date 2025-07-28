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

dae::Player::Player(const glm::vec2& startPos, float zRotation, const PlayerType& playerType)
{

	//------- PLAYER

	dae::GameObjectHandle currentPlayer = std::make_shared<GameObject>();

	//------- SHOOTING PIVOT

	dae::GameObjectHandle shootingPivot = std::make_shared<GameObject>();
	m_ShootingPivot = shootingPivot;
	shootingPivot->Transform().SetLocalPositionY(-10);
	shootingPivot->Transform().SetParent(*currentPlayer, dae::ETransformReparentType::KeepLocalTransform);

	auto weaponType = playerType.WeaponType;

	weaponType->Create(shootingPivot);

	//----- COMPONENTS -----

	Rect rect{};
	rect.Height = 32;
	rect.Width = 32;

	dae::CCollider collider{ rect, (int)GalagaCollisionLayers::Player };
	collider.AddCollisionTagToCollideWith((int)GalagaCollisionLayers::Enemies);
	collider.CenterRect();

	currentPlayer->AddComponent(collider);

	//-----

	dae::CRigidbody2D rigidBody{};
	currentPlayer->AddComponent(rigidBody);

	//-----

	dae::CMovement2D movement{};
	movement.SetMaxSpeed(300);
	currentPlayer->AddComponent(movement);

	//-----

	TransformData textureTransform{};
	textureTransform.Scale = { 0.5f, 0.5f , 0.5f };
	dae::CTextureHandle currentTexture{ playerType.TextureName };// { "galaga.png" };
	currentTexture.SetTextureTransform(textureTransform);
	currentTexture.Center();

	currentPlayer->AddComponent(currentTexture);

	//-----

	CPlayerController playerController{};
	PlayerId playerId = playerController.GetPlayerId();

	currentPlayer->AddComponent(playerController);

	//-----

	CStatController statController{};

	IntStat initialHealth{ 3, 3, 3 };


	const int maxInt = std::numeric_limits<int>::infinity();

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
	statDisplayData.StatNameTextData.FontData.FontFullPath = "Emulogic-zrEw.ttf";
	statDisplayData.StatNameTextData.FontData.FontSize = 13;
	statDisplayData.StatNameTextData.Color = {255, 0, 0 };
	statDisplayData.StatNameTextData.Text = "1UP ";
	statDisplayData.StatTypeToDisplay = StatType::Points;
	statDisplayData.StatValueColor = { 255, 255, 255 };
	statDisplayData.StatValueOffsetFromStatName = {0,5};
	statDisplayData.Where = { g_WindowWidth * 4.3f / 5.f , (g_WindowHeight * 1.6f / 5.f) + (50 * playerId) };

	CStatDisplayer statDisplayer{ statDisplayData };

	currentPlayer->AddComponent(statDisplayer);

	//----

	CPlayerLife playerHealthComponent{ 3, glm::vec3{g_WindowWidth/2.f, g_WindowHeight - 50, 0} };

	currentPlayer->AddComponent(playerHealthComponent);

	//-----

	currentPlayer->SetActive(false);

	currentPlayer->Transform().SetLocalPositionX(startPos.x);
	currentPlayer->Transform().SetLocalPositionY(startPos.y);
	currentPlayer->Transform().SetLocalRotationZ(zRotation);

	//----- ACTIONS -----

	// - Movement Action
	auto moveLeft	= [](GameObject& gameObj) mutable {gameObj.GetComponent<CMovement2D>()->AddSingleFrameMovementInput(glm::vec2{ -1,  0 }); };
	auto moveRight	= [](GameObject& gameObj) mutable {gameObj.GetComponent<CMovement2D>()->AddSingleFrameMovementInput(glm::vec2{ 1,  0 }); };
	auto moveUp		= [](GameObject& gameObj) mutable {gameObj.GetComponent<CMovement2D>()->AddSingleFrameMovementInput(glm::vec2{ 0, -1 }); };
	auto moveDown	= [](GameObject& gameObj) mutable {gameObj.GetComponent<CMovement2D>()->AddSingleFrameMovementInput(glm::vec2{ 0,  1 }); };


	// - Shooting Action

	auto shootAction = [shootingPivot, weaponType](GameObject&) mutable { weaponType->Execute(*shootingPivot); };

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

	playerController.BindKey(dae::PlayerKeyboardKeyData{ ButtonState::BUTTON_PRESSED, SDL_SCANCODE_SPACE,	std::make_unique<EventTriggerCommand>(shootEvent) });
	playerController.BindKey(dae::PlayerKeyboardKeyData{ ButtonState::BUTTON_PRESSED, SDL_SCANCODE_LEFT,	std::make_unique<EventTriggerCommand>(moveLeftEvent) });
	playerController.BindKey(dae::PlayerKeyboardKeyData{ ButtonState::BUTTON_PRESSED, SDL_SCANCODE_RIGHT,	std::make_unique<EventTriggerCommand>(moveRightEvent) });
	playerController.BindKey(dae::PlayerKeyboardKeyData{ ButtonState::BUTTON_PRESSED, SDL_SCANCODE_UP,		std::make_unique<EventTriggerCommand>(moveUpEvent) });
	playerController.BindKey(dae::PlayerKeyboardKeyData{ ButtonState::BUTTON_PRESSED, SDL_SCANCODE_DOWN,	std::make_unique<EventTriggerCommand>(moveDownEvent) });

	playerController.BindKey(dae::PlayerKeyboardKeyData{ ButtonState::BUTTON_PRESSED, SDL_SCANCODE_A,	 std::make_unique<EventTriggerCommand>(moveLeftEvent) });
	playerController.BindKey(dae::PlayerKeyboardKeyData{ ButtonState::BUTTON_PRESSED, SDL_SCANCODE_D,	 std::make_unique<EventTriggerCommand>(moveRightEvent) });
	playerController.BindKey(dae::PlayerKeyboardKeyData{ ButtonState::BUTTON_PRESSED, SDL_SCANCODE_W,	 std::make_unique<EventTriggerCommand>(moveUpEvent) });
	playerController.BindKey(dae::PlayerKeyboardKeyData{ ButtonState::BUTTON_PRESSED, SDL_SCANCODE_S,	 std::make_unique<EventTriggerCommand>(moveDownEvent) });


	playerController.BindKey(dae::PlayerGamepadKeyData{ ButtonState::BUTTON_PRESSED, GamepadButton::ButtonX,	 std::make_unique<EventTriggerCommand>(shootEvent) });
	playerController.BindKey(dae::PlayerGamepadKeyData{ ButtonState::BUTTON_PRESSED, GamepadButton::DpadLeft,	 std::make_unique<EventTriggerCommand>(moveLeftEvent) });
	playerController.BindKey(dae::PlayerGamepadKeyData{ ButtonState::BUTTON_PRESSED, GamepadButton::DpadRight,	 std::make_unique<EventTriggerCommand>(moveRightEvent) });
	playerController.BindKey(dae::PlayerGamepadKeyData{ ButtonState::BUTTON_PRESSED, GamepadButton::DpadUp,		 std::make_unique<EventTriggerCommand>(moveUpEvent) });
	playerController.BindKey(dae::PlayerGamepadKeyData{ ButtonState::BUTTON_PRESSED, GamepadButton::DpadDown,	 std::make_unique<EventTriggerCommand>(moveDownEvent) });

	m_CurrentPlayer = currentPlayer;
}

dae::GameObjectHandle dae::Player::GetGameObjectHandle()
{
	return m_CurrentPlayer;
}

void dae::Player::AddScene(Scene& scene)
{
	scene.AddGameObjectHandle(m_CurrentPlayer);
	scene.AddGameObjectHandle(m_ShootingPivot);
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