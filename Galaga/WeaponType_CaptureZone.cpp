#include "WeaponType_CaptureZone.h"
#include "CMovement2D.h"
#include "CCaptureZone.h"
#include <memory>
#include "Settings.h"

dae::WeaponType_CaptureZone::WeaponType_CaptureZone(const GameObjectHandle& target,float speed, const std::string& triggerName) : 
	m_Target(target),
	m_Speed(speed),
	m_TriggerName(triggerName)
{
}

void dae::WeaponType_CaptureZone::Create(const GameObjectHandle& gameObject)
{
	CaptureZoneData captureZoneData{};

	captureZoneData.OnActivateAudioData.File = "Sound/TractorBeam.wav";
	captureZoneData.OnActivateAudioData.LoopAmount = 0;

	captureZoneData.OnHitAudioData.File = "Sound/TractorBeamCaptured.wav";
	captureZoneData.OnHitAudioData.LoopAmount = 0;

	captureZoneData.RelativePos = glm::vec3{ 0,50,0 };
	captureZoneData.Target = m_Target;
	CCaptureZone captureZone{ captureZoneData };

	gameObject->AddComponent(captureZone);

	//---------
	std::string triggerName{ m_TriggerName };

	std::shared_ptr<CStateMachine> stateMachine{ gameObject->AddComponent(CStateMachine{})};
	stateMachine->AddTrigger(triggerName);
	stateMachine->AddState(std::make_shared<IdleState>		(m_Speed),		IdleState::Name());
	stateMachine->AddState(std::make_shared<DescendState>	(m_Speed),		DescendState::Name());
	stateMachine->AddState(std::make_shared<AscendState>	(m_Speed),		AscendState::Name());
	stateMachine->AddState(std::make_shared<CaptureState>	(),				CaptureState::Name());
	stateMachine->SetDefaultState(IdleState::Name());

	stateMachine->AddConnection(IdleState::Name(), DescendState::Name(), [triggerName](const CStateMachine& stateMachine, const GameObject& ) {

		return stateMachine.IsTriggerActive(triggerName);
		});
	stateMachine->AddConnection(DescendState::Name(), CaptureState::Name(), [](const CStateMachine& , const GameObject& gameObj) {

		return gameObj.TransformConst().GetWorldTransform().Position.y > (g_WindowHeight - 100);
		});
	stateMachine->AddConnection(CaptureState::Name(), AscendState::Name(), [triggerName](const CStateMachine& stateMachine, const GameObject& ) {

		return stateMachine.IsTriggerActive(triggerName);
		});
	stateMachine->AddConnection(AscendState::Name(), IdleState::Name(), [](const CStateMachine& , const GameObject& gameObj) {

		return gameObj.TransformConst().GetWorldTransform().Position.y < 50;
		});
}

void dae::WeaponType_CaptureZone::Execute(GameObject& obj)
{
	obj.GetComponent<CStateMachine>()->Trigger(m_TriggerName);
}

void dae::IdleState::Start(GameObject& actor)
{
	actor.GetComponent<CMovement2D>()->SetMaxSpeed(m_Speed);
}


void dae::DescendState::Start(GameObject& actor) {
	actor.GetComponent<CMovement2D>()->SetMaxSpeed(m_Speed);
}
void dae::DescendState::Update(GameObject& actor) 
{
	actor.GetComponent<CMovement2D>()->AddSingleFrameMovementInput(glm::vec2{ 0, 1 });
}

void dae::AscendState::Start(GameObject& actor) {
	actor.GetComponent<CMovement2D>()->SetMaxSpeed(m_Speed);
}
void dae::AscendState::Update(GameObject& actor) {
	actor.GetComponent<CMovement2D>()->AddSingleFrameMovementInput(glm::vec2{ 0,-1 });
}

//CaptureState

void dae::CaptureState::Start(GameObject& actor) {
	actor.GetComponent<CCaptureZone>()->SetActiveCaptureZone(true);
	actor.GetComponent<CMovement2D>()->SetMaxSpeed(0);
}


void dae::CaptureState::End(GameObject& actor) {

	actor.GetComponent<CCaptureZone>()->SetActiveCaptureZone(false);
}