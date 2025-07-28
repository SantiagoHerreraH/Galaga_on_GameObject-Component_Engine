#include "MovementActionSequence.h"
#include "MathTools.h"

dae::MovementActionSequence::MovementActionSequence(const std::string& sequenceName) :
	m_SequenceName(sequenceName)
{
}

void dae::MovementActionSequence::Start()
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

bool dae::MovementActionSequence::StartSequence()
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

bool dae::MovementActionSequence::RestartSequence()
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

void dae::MovementActionSequence::StopSequence()
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


bool dae::MovementActionSequence::IsActing()const 
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

bool dae::MovementActionSequence::CanStartSequence() const
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

void dae::MovementActionSequence::AddConditionToStartSequence(const std::function<bool()>& condition)
{
	m_ConditionsToStartSequence.push_back(condition);
}

void dae::MovementActionSequence::AddAction(const std::string& actionName)
{
	std::shared_ptr<MovementAction> movementAction{ std::make_shared<MovementAction>() };
	movementAction->SetName(actionName);
	m_MovementActions.emplace_back(movementAction);
}

void dae::MovementActionSequence::AddConditionToStartOrRestartAction(const std::function<bool()>& condition)
{

	m_MovementActions.back()->AddConditionToStartOrRestartAction(condition);

}

void dae::MovementActionSequence::SetActionDependsOnDuration(bool dependsOnTime, float time)
{
	m_MovementActions.back()->SetActionDependsOnDuration(dependsOnTime, time);

}

void dae::MovementActionSequence::SetActionDuration(float time)
{
	m_MovementActions.back()->SetDuration(time);
}

void dae::MovementActionSequence::AddConditionToStopAction(const std::function<bool()>& condition)
{
	m_MovementActions.back()->AddConditionToStopAction(condition);

	//auto timerKey = m_MovementActions.back().TimerKey;
	//Scene* scenePtr = m_Scene;
	//auto conditionFunction = [condition, timerKey, scenePtr]()mutable {
	//	
	//	if (condition())
	//	{
	//		TimerSystem::GetFromScene(scenePtr).EndTimer(timerKey);
	//	}
	//	
	//	};

	//TimerSystem::GetFromScene(m_Scene).TimerAt(m_MovementActions.back().TimerKey).GetOnUpdateEvent().Subscribe(conditionFunction);
}

void dae::MovementActionSequence::SetMovementPathDecidingFunction(std::function<glm::vec2()> functionThatReturnsVectorWithMovementDirectionAndMagnitude)
{
	m_MovementActions.back()->SetMovementPathDecidingFunction(functionThatReturnsVectorWithMovementDirectionAndMagnitude);

	/*std::shared_ptr<MovementData> movementData = m_MovementActions.back().MovementData;
	GameObjectHandle self = m_Self;
	auto function = [functionThatReturnsVectorWithMovementDirectionAndMagnitude, movementData, self]()mutable {

		movementData.get()->BeginningDeltaTowardsTarget = functionThatReturnsVectorWithMovementDirectionAndMagnitude();
		movementData.get()->BeginningPosition = self->Transform().GetWorldTransform().Position;

		};

	TimerSystem::GetFromScene(m_Scene).TimerAt(m_MovementActions.back().TimerKey).GetOnStartEvent().Subscribe(function);*/
}

void dae::MovementActionSequence::AddActionFunction(std::function<bool(float timeSinceActionStarted, const MovementData&)> movementFuncThatReturnsWhenItIsDone)
{
	m_MovementActions.back()->AddActionFunction(movementFuncThatReturnsWhenItIsDone);

	/*auto timerKey = m_MovementActions.back().TimerKey;
	std::shared_ptr<MovementData> movementData = m_MovementActions.back().MovementData;

	Scene* scenePtr = m_Scene;

	auto function = [movementFuncThatReturnsWhenItIsDone, movementData, timerKey, scenePtr]()mutable {

		if (movementFuncThatReturnsWhenItIsDone(
			TimerSystem::GetFromScene(scenePtr).TimerAt(timerKey).GetTime(),
			*movementData.get()))
		{

			TimerSystem::GetFromScene(scenePtr).EndTimer(timerKey);
		}

		};

	TimerSystem::GetFromScene(m_Scene).TimerAt(m_MovementActions.back().TimerKey).GetOnUpdateEvent().Subscribe(function);*/
}

void dae::MovementActionSequence::AddStartSubAction(const std::function<void()>& subAction)
{
	m_MovementActions.back()->AddStartSubAction(subAction);

	/*TimerSystem::GetFromScene(m_Scene).TimerAt(m_MovementActions.back().TimerKey).GetOnStartEvent().Subscribe(subAction);*/
}

void dae::MovementActionSequence::AddUpdateSubAction(const std::function<void()>& subAction)
{
	m_MovementActions.back()->AddPostActionUpdateSubAction(subAction);

	/*TimerSystem::GetFromScene(m_Scene).TimerAt(m_MovementActions.back().TimerKey).GetOnUpdateEvent().Subscribe(subAction);*/
}

void dae::MovementActionSequence::AddEndSubAction(const std::function<void()>& subAction)
{
	m_MovementActions.back()->AddEndSubAction(subAction);

	//TimerSystem::GetFromScene(m_Scene).TimerAt(m_MovementActions.back().TimerKey).GetOnEndEvent().Subscribe(subAction);
}

#pragma region MovementAction

void dae::MovementActionSequence::MovementAction::Initialize(MovementActionSequence* movementActionSequence)
{
	m_MovementActionSequence = movementActionSequence;
	m_Scene = &SceneManager::GetInstance().GetCurrentScene();
	m_TimerKey = TimerSystem::GetFromScene(m_Scene).AddTimer(Timer{ m_Duration, true, movementActionSequence->GetName() + " - " + m_ActionName});

}

bool dae::MovementActionSequence::MovementAction::WasInitilized() const
{
	return m_Scene != nullptr;
}

void dae::MovementActionSequence::MovementAction::SetName(const std::string& actionName) {
	m_ActionName = actionName;
}
const std::string& dae::MovementActionSequence::MovementAction::GetName()const {
	return m_ActionName;
}

dae::TimerKey dae::MovementActionSequence::MovementAction::GetTimerKey()
{
	return m_TimerKey;
}

float dae::MovementActionSequence::MovementAction::GetDuration() {
	return m_Duration;
}
void dae::MovementActionSequence::MovementAction::SetDuration(float duration) {
	
	m_Duration = duration;

	if (WasInitilized())
	{
		TimerSystem::GetFromScene(m_Scene).SetTimerDuration(m_TimerKey, m_Duration, false);
	}
}

void dae::MovementActionSequence::MovementAction::SetActionDependsOnDuration(bool dependsOnTime, float time) {

	m_DependsOnTime = dependsOnTime;
	time = dependsOnTime ? time : -1; //-1 == duration is infinite

	m_Duration = time;

	if (WasInitilized())
	{
		TimerSystem::GetFromScene(m_Scene).SetTimerDuration(m_TimerKey, time, false);
	}


}

bool dae::MovementActionSequence::MovementAction::HasNextTimerKey() const
{
	return m_HasNextTimerKey;
}

dae::TimerKey dae::MovementActionSequence::MovementAction::GetNextTimerKey()
{
	return m_NextActionTimerKey;
}

void dae::MovementActionSequence::MovementAction::SetNextTimerKey(bool hasNextTimerKey, TimerKey nextTimerKey)
{
	m_HasNextTimerKey = hasNextTimerKey;
	m_NextActionTimerKey = nextTimerKey;
}

void dae::MovementActionSequence::MovementAction::AddConditionToStartOrRestartAction(const std::function<bool()>& condition) {
	m_ConditionsToStartOrRestart.push_back(condition);
}
bool dae::MovementActionSequence::MovementAction::CanStartOrRestartAction() const
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
void dae::MovementActionSequence::MovementAction::AddConditionToStopAction(const std::function<bool()>& condition) {
	m_ConditionsToStopAction.push_back(condition);
}

bool dae::MovementActionSequence::MovementAction::CanStopAction() const
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

void dae::MovementActionSequence::MovementAction::SetMovementPathDecidingFunction(std::function<glm::vec2()> functionThatReturnsVectorWithMovementDirectionAndMagnitude) {
	m_MovementPathDecidingFunc = functionThatReturnsVectorWithMovementDirectionAndMagnitude;
}
void dae::MovementActionSequence::MovementAction::AddActionFunction(std::function<bool(float timeSinceActionStarted, const dae::MovementData&)> movementFuncThatReturnsWhenItIsDone) {
	m_ActionFunctions.push_back(movementFuncThatReturnsWhenItIsDone);
}

void dae::MovementActionSequence::MovementAction::RestartAction()
{
	if (!CanStartOrRestartAction())
	{
		TimerSystem::GetFromScene(m_Scene).EndTimer(m_TimerKey);
	}
}

void dae::MovementActionSequence::MovementAction::StartAction()
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

void dae::MovementActionSequence::MovementAction::UpdateAction()
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

void dae::MovementActionSequence::MovementAction::EndAction()
{
	m_EndSubActions.Invoke();

	if (m_HasNextTimerKey)
	{
		TimerSystem::GetFromScene(m_Scene).RestartTimer(m_NextActionTimerKey);
	}
}

void dae::MovementActionSequence::MovementAction::AddStartSubAction(const std::function<void()>& subAction) {
	m_StartSubActions.Subscribe(subAction);
}
void dae::MovementActionSequence::MovementAction::AddPostActionUpdateSubAction(const std::function<void()>& subAction) {
	m_UpdateSubActions.Subscribe(subAction);
}
void dae::MovementActionSequence::MovementAction::AddEndSubAction(const std::function<void()>& subAction) {
	m_EndSubActions.Subscribe(subAction);
}

#pragma endregion