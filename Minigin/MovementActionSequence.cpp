#include "MovementActionSequence.h"
#include "MathTools.h"

dae::MovementActionSequence::MovementActionSequence(dae::Scene& scene, dae::GameObjectHandle self, const std::string& sequenceName) :
m_Scene(scene),
m_Self(self),
m_SequenceName(sequenceName)
{
}

bool dae::MovementActionSequence::StartSequence()
{
	if (CanStartSequence())
	{
		TimerSystem::GetFromScene(&m_Scene).RestartTimer(m_MovementActions.front().TimerKey);
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

	for (size_t i = 0; i < m_MovementActions.size(); i++)
	{
		TimerSystem::GetFromScene(&m_Scene).PauseTimer(m_MovementActions[i].TimerKey);
	}

	TimerSystem::GetFromScene(&m_Scene).RestartTimer(m_MovementActions.front().TimerKey);

	return true;
}

void dae::MovementActionSequence::StopSequence()
{
	for (size_t i = 0; i < m_MovementActions.size(); i++)
	{
		TimerSystem::GetFromScene(&m_Scene).PauseTimer(m_MovementActions[i].TimerKey);
	}
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

dae::TimerKey dae::MovementActionSequence::GetFirstActionTimerKey() const
{
	return m_MovementActions.front().TimerKey;
}

std::vector<dae::TimerKey> dae::MovementActionSequence::GetActionTimerKeys() const
{
	std::vector<TimerKey> timerKeys{};
	timerKeys.reserve(m_MovementActions.size());

	for (size_t i = 0; i < m_MovementActions.size(); i++)
	{
		timerKeys.push_back(m_MovementActions[i].TimerKey);
	}

	return timerKeys;
}

void dae::MovementActionSequence::AddAction(const std::string& actionName)
{
	MovementAction GalagaEnemyAction{};
	GalagaEnemyAction.TimerKey = TimerSystem::GetFromScene(&m_Scene).AddTimer(Timer{10, true, m_SequenceName + " - " + actionName });
	GalagaEnemyAction.MovementData = std::make_shared<MovementData>();
	m_MovementActions.emplace_back(GalagaEnemyAction);
	Scene* scenePtr = &m_Scene;

	if (m_MovementActions.size() > 1)
	{
		TimerKey currentTimerKey = GalagaEnemyAction.TimerKey;
		TimerKey prevTimerKey = m_MovementActions[m_MovementActions.size() - 2].TimerKey;
		TimerSystem::GetFromScene(&m_Scene).TimerAt(prevTimerKey).GetOnEndEvent().Subscribe([currentTimerKey, scenePtr](){
			
			TimerSystem::GetFromScene(scenePtr).RestartTimer(currentTimerKey);

			});
	}
}

void dae::MovementActionSequence::AddConditionToStartOrRestartAction(const std::function<bool()>& condition)
{
	auto timerKey = m_MovementActions.back().TimerKey;
	Scene* scenePtr = &m_Scene;
	auto conditionFunction = [condition, timerKey, scenePtr]()mutable {

		if (!condition())
		{
			TimerSystem::GetFromScene(scenePtr).EndTimer(timerKey);
		}

		};

	TimerSystem::GetFromScene(&m_Scene).TimerAt(m_MovementActions.back().TimerKey).GetOnStartEvent().Subscribe(conditionFunction);
	TimerSystem::GetFromScene(&m_Scene).TimerAt(m_MovementActions.back().TimerKey).GetOnRestartEvent().Subscribe(conditionFunction);

}

void dae::MovementActionSequence::SetActionDependsOnDuration(bool dependsOnTime, float time)
{

	time = dependsOnTime ? time : -1; //-1 == duration is infinite

	TimerSystem::GetFromScene(&m_Scene).SetTimerDuration(m_MovementActions.back().TimerKey, time, false);

}

void dae::MovementActionSequence::SetActionDuration(float time)
{
	TimerSystem::GetFromScene(&m_Scene).SetTimerDuration(m_MovementActions.back().TimerKey, time, false);
}

void dae::MovementActionSequence::AddConditionToStopAction(const std::function<bool()>& condition)
{
	auto timerKey = m_MovementActions.back().TimerKey;
	Scene* scenePtr = &m_Scene;
	auto conditionFunction = [condition, timerKey, scenePtr]()mutable {
		
		if (condition())
		{
			TimerSystem::GetFromScene(scenePtr).EndTimer(timerKey);
		}
		
		};

	TimerSystem::GetFromScene(&m_Scene).TimerAt(m_MovementActions.back().TimerKey).GetOnUpdateEvent().Subscribe(conditionFunction);
}

void dae::MovementActionSequence::SetMovementPathDecidingFunction(std::function<glm::vec2()> functionThatReturnsVectorWithMovementDirectionAndMagnitude)
{
	std::shared_ptr<MovementData> movementData = m_MovementActions.back().MovementData;
	GameObjectHandle self = m_Self;
	auto function = [functionThatReturnsVectorWithMovementDirectionAndMagnitude, movementData, self]()mutable {

		movementData.get()->BeginningDeltaTowardsTarget = functionThatReturnsVectorWithMovementDirectionAndMagnitude();
		movementData.get()->BeginningPosition = self->Transform().GetWorldTransform().Position;

		};

	TimerSystem::GetFromScene(&m_Scene).TimerAt(m_MovementActions.back().TimerKey).GetOnStartEvent().Subscribe(function);
}

void dae::MovementActionSequence::AddActionFunction(std::function<bool(float timeSinceActionStarted, const MovementData&)> movementFuncThatReturnsWhenItIsDone)
{

	auto timerKey = m_MovementActions.back().TimerKey;
	std::shared_ptr<MovementData> movementData = m_MovementActions.back().MovementData;

	Scene* scenePtr = &m_Scene;

	auto function = [movementFuncThatReturnsWhenItIsDone, movementData, timerKey, scenePtr]()mutable {

		if (movementFuncThatReturnsWhenItIsDone(
			TimerSystem::GetFromScene(scenePtr).TimerAt(timerKey).GetTime(),
			*movementData.get()))
		{

			TimerSystem::GetFromScene(scenePtr).EndTimer(timerKey);
		}

		};

	TimerSystem::GetFromScene(&m_Scene).TimerAt(m_MovementActions.back().TimerKey).GetOnUpdateEvent().Subscribe(function);
}

void dae::MovementActionSequence::AddStartSubAction(const std::function<void()>& subAction)
{
	TimerSystem::GetFromScene(&m_Scene).TimerAt(m_MovementActions.back().TimerKey).GetOnStartEvent().Subscribe(subAction);
}

void dae::MovementActionSequence::AddUpdateSubAction(const std::function<void()>& subAction)
{
	TimerSystem::GetFromScene(&m_Scene).TimerAt(m_MovementActions.back().TimerKey).GetOnUpdateEvent().Subscribe(subAction);
}

void dae::MovementActionSequence::AddEndSubAction(const std::function<void()>& subAction)
{
	TimerSystem::GetFromScene(&m_Scene).TimerAt(m_MovementActions.back().TimerKey).GetOnEndEvent().Subscribe(subAction);
}
