#include "MovementActionSequence.h"
#include "MathTools.h"

dae::CMovementActionSequence::CMovementActionSequence(const std::string& sequenceName) :
	m_SequenceName(sequenceName)
{
}

void dae::CMovementActionSequence::Start()
{
	m_Scene = &SceneManager::GetInstance().GetCurrentScene();

	for (size_t i = 0; i < m_MovementActions.size(); i++)
	{
		auto currentAction = m_MovementActions[i];
		m_MovementActions[i]->Initialize(this);
		m_MovementActions[i]->SetNextTimerKey(false);

		auto reStart	= [currentAction]() {currentAction->RestartAction(); };
		auto start		= [currentAction]() {currentAction->StartAction();};
		auto update		= [currentAction]() {currentAction->UpdateAction();};
		auto end		= [currentAction]() {currentAction->EndAction();};

		TimerSystem::GetFromScene(m_Scene).TimerAt(currentAction->GetTimerKey()).GetOnRestartEvent().Subscribe(reStart);
		TimerSystem::GetFromScene(m_Scene).TimerAt(currentAction->GetTimerKey()).GetOnStartEvent().Subscribe(start);
		TimerSystem::GetFromScene(m_Scene).TimerAt(currentAction->GetTimerKey()).GetOnUpdateEvent().Subscribe(update);
		TimerSystem::GetFromScene(m_Scene).TimerAt(currentAction->GetTimerKey()).GetOnEndEvent().Subscribe(end);

		if (i > 0)
		{
			m_MovementActions[i - 1]->SetNextTimerKey(true, m_MovementActions[i]->GetTimerKey());
		}
	}

	if (m_StartSequenceOnStart)
	{
		m_StartSequenceOnStart = false;
		StartSequence();
	}
	if (m_RestartSequenceOnStart)
	{
		m_RestartSequenceOnStart = false;
		RestartSequence();
	}
	if (m_StopSequenceOnStart)
	{
		m_StopSequenceOnStart = false;
		StopSequence();
	}
}

void dae::CMovementActionSequence::SetActive(bool isActive)
{
	Component::SetActive(isActive);

	if (!isActive && IsActing())
	{
		EndSequence();
	}
}

bool dae::CMovementActionSequence::StartSequence()
{

	if (CanStartSequence())
	{
		if (!m_Scene)
		{
			m_StartSequenceOnStart = true;
			return true;
		}

		TimerSystem::GetFromScene(m_Scene).RestartTimer(m_MovementActions.front()->GetTimerKey());
		return true;
	}

	return false;
}

bool dae::CMovementActionSequence::RestartSequence()
{
	if (!CanStartSequence())
	{
		return false;
	}

	if (!m_Scene)
	{
		m_RestartSequenceOnStart = true;
		return true;
	}

	for (size_t i = 0; i < m_MovementActions.size(); i++)
	{
		TimerSystem::GetFromScene(m_Scene).PauseTimer(m_MovementActions[i]->GetTimerKey());
	}

	TimerSystem::GetFromScene(m_Scene).RestartTimer(m_MovementActions.front()->GetTimerKey());

	return true;
}

void dae::CMovementActionSequence::StopSequence()
{
	if (!m_Scene)
	{
		m_StopSequenceOnStart = true;
		return;
	}

	for (size_t i = 0; i < m_MovementActions.size(); i++)
	{
		TimerSystem::GetFromScene(m_Scene).PauseTimer(m_MovementActions[i]->GetTimerKey());
	}
}

void dae::CMovementActionSequence::EndSequence()
{
	if (!m_Scene)
	{
		m_StopSequenceOnStart = true;
		return;
	}

	for (size_t i = 0; i < m_MovementActions.size(); i++)
	{
		TimerSystem::GetFromScene(m_Scene).PauseTimer(m_MovementActions[i]->GetTimerKey());
	}

	TimerSystem::GetFromScene(m_Scene).EndTimer(m_MovementActions.back()->GetTimerKey());
}


bool dae::CMovementActionSequence::IsActing()const 
{
	if (!m_Scene)
	{
		return false;
	}

	for (size_t i = 0; i < m_MovementActions.size(); i++)
	{
		if (TimerSystem::GetFromScene(m_Scene).IsTimerActive(m_MovementActions[i]->GetTimerKey()))
		{
			return true;
		}
	}

	return false;
}

bool dae::CMovementActionSequence::CanStartSequence() const
{

	for (size_t i = 0; i < m_ConditionsToStartSequence.size(); i++)
	{
		if (!m_ConditionsToStartSequence[i]())
		{
			return false;
		}
	}
	return true;
}

void dae::CMovementActionSequence::AddConditionToStartSequence(const std::function<bool()>& condition)
{
	m_ConditionsToStartSequence.push_back(condition);
}

void dae::CMovementActionSequence::AddAction(const std::string& actionName)
{
	std::shared_ptr<MovementAction> movementAction{ std::make_shared<MovementAction>() };
	movementAction->SetName(actionName);
	m_MovementActions.emplace_back(movementAction);
}

void dae::CMovementActionSequence::AddConditionToStartOrRestartAction(const std::function<bool()>& condition)
{

	m_MovementActions.back()->AddConditionToStartOrRestartAction(condition);

}

void dae::CMovementActionSequence::SetActionDependsOnDuration(bool dependsOnTime, float time)
{
	m_MovementActions.back()->SetActionDependsOnDuration(dependsOnTime, time);

}

void dae::CMovementActionSequence::SetActionDuration(float time)
{
	m_MovementActions.back()->SetDuration(time);
}

void dae::CMovementActionSequence::AddConditionToStopAction(const std::function<bool()>& condition)
{
	m_MovementActions.back()->AddConditionToStopAction(condition);
}

void dae::CMovementActionSequence::SetMovementPathDecidingFunction(std::function<glm::vec2()> functionThatReturnsVectorWithMovementDirectionAndMagnitude)
{
	m_MovementActions.back()->SetMovementPathDecidingFunction(functionThatReturnsVectorWithMovementDirectionAndMagnitude);
}

void dae::CMovementActionSequence::AddActionFunction(std::function<bool(float timeSinceActionStarted, const MovementData&)> movementFuncThatReturnsWhenItIsDone)
{
	m_MovementActions.back()->AddActionFunction(movementFuncThatReturnsWhenItIsDone);
}

void dae::CMovementActionSequence::AddStartSubAction(const std::function<void()>& subAction)
{
	m_MovementActions.back()->AddStartSubAction(subAction);
}

void dae::CMovementActionSequence::AddUpdateSubAction(const std::function<void()>& subAction)
{
	m_MovementActions.back()->AddPostActionUpdateSubAction(subAction);
}

void dae::CMovementActionSequence::AddEndSubAction(const std::function<void()>& subAction)
{
	m_MovementActions.back()->AddEndSubAction(subAction);
}

#pragma region MovementAction

dae::CMovementActionSequence::MovementAction::MovementAction()
{
	m_MovementPathDecidingFunc = []() {return glm::vec2{}; };
}

void dae::CMovementActionSequence::MovementAction::Initialize(CMovementActionSequence* movementActionSequence)
{
	m_MovementActionSequence = movementActionSequence;
	m_Scene = &SceneManager::GetInstance().GetCurrentScene();
	m_TimerKey = TimerSystem::GetFromScene(m_Scene).AddTimer(Timer{ m_Duration, true, movementActionSequence->GetName() + " - " + m_ActionName});

}

bool dae::CMovementActionSequence::MovementAction::WasInitilized() const
{
	return m_Scene != nullptr;
}

void dae::CMovementActionSequence::MovementAction::SetName(const std::string& actionName) {
	m_ActionName = actionName;
}
const std::string& dae::CMovementActionSequence::MovementAction::GetName()const {
	return m_ActionName;
}

dae::TimerKey dae::CMovementActionSequence::MovementAction::GetTimerKey()
{
	return m_TimerKey;
}

float dae::CMovementActionSequence::MovementAction::GetDuration() {
	return m_Duration;
}
void dae::CMovementActionSequence::MovementAction::SetDuration(float duration) {
	
	m_Duration = duration;

	if (WasInitilized())
	{
		TimerSystem::GetFromScene(m_Scene).SetTimerDuration(m_TimerKey, m_Duration, false);
	}
}

void dae::CMovementActionSequence::MovementAction::SetActionDependsOnDuration(bool dependsOnTime, float time) {

	m_DependsOnTime = dependsOnTime;
	time = dependsOnTime ? time : -1; //-1 == duration is infinite

	m_Duration = time;

	if (WasInitilized())
	{
		TimerSystem::GetFromScene(m_Scene).SetTimerDuration(m_TimerKey, time, false);
	}


}

bool dae::CMovementActionSequence::MovementAction::HasNextTimerKey() const
{
	return m_HasNextTimerKey;
}

dae::TimerKey dae::CMovementActionSequence::MovementAction::GetNextTimerKey()
{
	return m_NextActionTimerKey;
}

void dae::CMovementActionSequence::MovementAction::SetNextTimerKey(bool hasNextTimerKey, TimerKey nextTimerKey)
{
	m_HasNextTimerKey = hasNextTimerKey;
	m_NextActionTimerKey = nextTimerKey;
}

void dae::CMovementActionSequence::MovementAction::AddConditionToStartOrRestartAction(const std::function<bool()>& condition) {
	m_ConditionsToStartOrRestart.push_back(condition);
}
bool dae::CMovementActionSequence::MovementAction::CanStartOrRestartAction() const
{
	for (size_t i = 0; i < m_ConditionsToStartOrRestart.size(); i++)
	{
		if (!m_ConditionsToStartOrRestart[i]())
		{
			return false;
		}
	}
	return true;
}
void dae::CMovementActionSequence::MovementAction::AddConditionToStopAction(const std::function<bool()>& condition) {
	m_ConditionsToStopAction.push_back(condition);
}

bool dae::CMovementActionSequence::MovementAction::CanStopAction() const
{
	for (size_t i = 0; i < m_ConditionsToStopAction.size(); i++)
	{
		if (m_ConditionsToStopAction[i]())
		{
			return true;
		}
	}
	return false;
}

void dae::CMovementActionSequence::MovementAction::SetMovementPathDecidingFunction(std::function<glm::vec2()> functionThatReturnsVectorWithMovementDirectionAndMagnitude) {
	m_MovementPathDecidingFunc = functionThatReturnsVectorWithMovementDirectionAndMagnitude;
}
void dae::CMovementActionSequence::MovementAction::AddActionFunction(std::function<bool(float timeSinceActionStarted, const dae::MovementData&)> movementFuncThatReturnsWhenItIsDone) {
	m_ActionFunctions.push_back(movementFuncThatReturnsWhenItIsDone);
}

void dae::CMovementActionSequence::MovementAction::RestartAction()
{
	if (!CanStartOrRestartAction())
	{
		TimerSystem::GetFromScene(m_Scene).EndTimer(m_TimerKey);
	}
}

void dae::CMovementActionSequence::MovementAction::StartAction()
{
	m_MovementData.BeginningPosition = m_MovementActionSequence->Owner().Transform().GetWorldTransform().Position;
	m_MovementData.BeginningDeltaTowardsTarget = m_MovementPathDecidingFunc();

	if (!CanStartOrRestartAction())
	{
		TimerSystem::GetFromScene(m_Scene).EndTimer(m_TimerKey);
	}
	else
	{
		m_StartSubActions.Invoke();
	}
}

void dae::CMovementActionSequence::MovementAction::UpdateAction()
{
	if (CanStopAction())
	{
		TimerSystem::GetFromScene(m_Scene).EndTimer(m_TimerKey);
		return;
	}

	float timeSinceActionStarted = TimerSystem::GetFromScene(m_Scene).TimerAt(m_TimerKey).GetTime();

	for (size_t i = 0; i < m_ActionFunctions.size(); i++)
	{
		if (m_ActionFunctions[i](timeSinceActionStarted, m_MovementData))
		{
			TimerSystem::GetFromScene(m_Scene).EndTimer(m_TimerKey);
			return;
		}
	}

	m_UpdateSubActions.Invoke();
}

void dae::CMovementActionSequence::MovementAction::EndAction()
{
	m_EndSubActions.Invoke();

	if (m_HasNextTimerKey)
	{
		TimerSystem::GetFromScene(m_Scene).RestartTimer(m_NextActionTimerKey);
	}
}

void dae::CMovementActionSequence::MovementAction::AddStartSubAction(const std::function<void()>& subAction) {
	m_StartSubActions.Subscribe(subAction);
}
void dae::CMovementActionSequence::MovementAction::AddPostActionUpdateSubAction(const std::function<void()>& subAction) {
	m_UpdateSubActions.Subscribe(subAction);
}
void dae::CMovementActionSequence::MovementAction::AddEndSubAction(const std::function<void()>& subAction) {
	m_EndSubActions.Subscribe(subAction);
}

#pragma endregion