#include "EnemyFormation.h"
#include "MainMenu.h"

dae::CEnemyFormation::CEnemyFormation(
	const glm::vec2& position,
	float secondsBetweenSpawning,
	const GalagaPlayer& player,
	const EnemyFormationType& enemyManagerType) :
	m_StartPosition(position),
	m_SecondsBetweenSpawning(secondsBetweenSpawning),
	m_Player(player)
	
{	
	m_EnemyUnitSwirlData = enemyManagerType.EnemyUnitSwirlData;
	m_EnemyGridData = enemyManagerType.EnemyGridData;
	m_EnemyType = enemyManagerType.EnemyType;
}

void dae::CEnemyFormation::Start()
{
	CreateFormationTimer();
	CreateGrid();
	CreateEnemies();
	CreatePlayerReactions();

	if (m_CalledStartFormationBeforeStart)
	{
		StartEnemySwirlBehaviour();
	}
}

void dae::CEnemyFormation::StopSendingTroops()
{
	m_CanSendTroops = false;
}

void dae::CEnemyFormation::StartSendingTroops()
{
	m_CanSendTroops = true;

	SendNextTroops(nullptr);
}

void dae::CEnemyFormation::StartEnemySwirlBehaviour(bool startActingBehaviourAfterEndSwirls)
{
	if (!m_CreatedFormationTimer)
	{
		m_CalledStartFormationBeforeStart = true;
		return;
	}

	Scene& scene = SceneManager::GetInstance().GetCurrentScene();
	TimerSystem* TIMERSYSTEM = &TimerSystem::GetFromScene(&scene);
	CEnemyFormation* self{ this };
	bool startActingAfterFormation = startActingBehaviourAfterEndSwirls;

	int currentSpawningIndex = 0;
	TimerKey timerKey = m_FormationTimerKey;

	TIMERSYSTEM->ClearTimerEvents(timerKey);

	TIMERSYSTEM->TimerAt(timerKey).GetOnRestartEvent().Subscribe([currentSpawningIndex, self, TIMERSYSTEM, timerKey, startActingAfterFormation]() mutable
		{
			self->m_Enemies[currentSpawningIndex].StartFormation();

			if (currentSpawningIndex < (self->m_Enemies.size() - 1))
			{
				++currentSpawningIndex;
				TIMERSYSTEM->RestartTimer(timerKey);
			}
			else
			{
				if (startActingAfterFormation)
				{
					self->SendNextTroops(&self->m_Enemies[currentSpawningIndex].GetGameObject());
				}

				self->m_OnEndSwirlFormation.Invoke();
			}

		});

	TIMERSYSTEM->RestartTimer(m_FormationTimerKey);
}

bool dae::CEnemyFormation::SendNextTroops(GameObject* incomingTroop)
{
	if (!m_CanSendTroops)
	{
		return false;
	}
	std::vector<int> permittedIndices{};

	for (int enemyIdx = 0; enemyIdx < m_Enemies.size(); enemyIdx++)
	{
		if (!m_Enemies[enemyIdx].GetGameObject().IsActive())
		{
			continue;
		}
		else if (incomingTroop  && &m_Enemies[enemyIdx].GetGameObject() == incomingTroop)
		{
			permittedIndices.push_back(enemyIdx);
		}
		else if (!m_Enemies[enemyIdx].IsActing())
		{
			permittedIndices.push_back(enemyIdx);
		}
	}

	if (!permittedIndices.empty())
	{
		int indexOfArray = Random::GetRandomBetweenRange(0, (int)permittedIndices.size());
		int chosenIndex = permittedIndices[indexOfArray];

		m_Enemies[chosenIndex].Act();
		return true;
	}

	return false;
}

void dae::CEnemyFormation::SubscribeOnFormationDeath(const std::function<void()>& func)
{
	m_OnFormationDeath.Subscribe(func);
}

void dae::CEnemyFormation::SubscribeOnEndFormationSwirlBehaviour(const std::function<void()>& func)
{
	m_OnEndSwirlFormation.Subscribe(func);
}

void dae::CEnemyFormation::CreatePlayerReactions()
{
	CEnemyFormation* self{this};
	auto stopSendingTroops = [self]() { self->StopSendingTroops(); };
	auto startSendingTroops = [self]() { self->StartSendingTroops(); };

	m_Player.SubscribeOnPlayerDespawnFromDamage(stopSendingTroops);
	m_Player.SubscribeOnPlayerRespawnFromDamage(startSendingTroops);

}

void dae::CEnemyFormation::CreateFormationTimer()
{
	Scene& scene = SceneManager::GetInstance().GetCurrentScene();
	TimerSystem* TIMERSYSTEM = &TimerSystem::GetFromScene(&scene);

	m_FormationTimerKey = TIMERSYSTEM->AddTimer(Timer{ m_SecondsBetweenSpawning , true, "Enemy Manager Beginning Spawning" });
	m_CreatedFormationTimer = true;
}

void dae::CEnemyFormation::CreateGrid()
{
	Scene& scene = SceneManager::GetInstance().GetCurrentScene();
	TimerSystem* TIMERSYSTEM = &TimerSystem::GetFromScene(&scene);
	GameObjectHandle grid = scene.CreateGameObject();
	m_Grid = grid;

	// ----- GRID MOVEMENT -----

	float movementSpeed{ 2 };
	float movementRange{ 1 };
	float currentTime{ };
	float maxTime{ 100.f };

	auto moveHorizontal = [grid, movementSpeed, movementRange, currentTime, maxTime]()mutable
		{
			currentTime += movementSpeed * Time::GetInstance().GetElapsedSeconds();

			if (currentTime > (maxTime * movementSpeed))
			{
				currentTime -= maxTime;
			}

			float value = std::sin(currentTime) * movementRange;


			grid->Transform().MoveLocalPositionX(value);
		};

	float scaleSpeed{ 2 };
	float scaleRange{ 0.1f };
	float scaleMidPoint{ 1 };

	auto scale = [grid, scaleSpeed, scaleRange, scaleMidPoint, currentTime, maxTime]()mutable
		{
			currentTime += scaleSpeed * Time::GetInstance().GetElapsedSeconds();

			if (currentTime > (maxTime * scaleSpeed))
			{
				currentTime -= maxTime;
			}

			float value = std::sin(currentTime) * scaleRange;


			grid->Transform().SetLocalScaleX(scaleMidPoint + value);
		};

	size_t movementTimerKey = TIMERSYSTEM->AddTimer(Timer{ maxTime,false, "GridMovement" });
	Timer& movementTimer = TIMERSYSTEM->TimerAt(movementTimerKey);

	auto restartMovementTimer = [TIMERSYSTEM, movementTimerKey]
		{
			TIMERSYSTEM->RestartTimer(movementTimerKey);
		};

	Event<> movementTimerUpdate{};
	movementTimerUpdate.Subscribe(moveHorizontal);
	movementTimerUpdate.Subscribe(scale);

	Event<> movementTimerEnd{};
	movementTimerEnd.Subscribe(restartMovementTimer);

	movementTimer.SetOnUpdateEvent(movementTimerUpdate);
	movementTimer.SetOnEndEvent(movementTimerEnd);
}

dae::CButtonGrid* dae::MainMenu::MainMenuCreator::GetGrid()
{
	return m_ButtonGrid->GetComponent<CButtonGrid>();
}

void dae::CEnemyFormation::CreateEnemies()
{
	Scene& scene = SceneManager::GetInstance().GetCurrentScene();
	m_Grid->Transform().SetLocalPositionX(m_StartPosition.x);
	m_Grid->Transform().SetLocalPositionY(m_StartPosition.y);

	const float spaceBetweenEnemies{ m_EnemyGridData.SpaceBetweenEnemies };
	const float spaceBetweenRows{ m_EnemyGridData.SpaceBetweenRows };

	glm::vec3 currentRelativePos{};
	const int totalEnemyAmount{ m_EnemyGridData.NumberOfEnemyRows * m_EnemyGridData.NumberOfEnemiesPerRow };

	//---------- SWIRL DATA

	const int maxEnemiesPerSwirlType{ m_EnemyUnitSwirlData.EnemiesInAUnit };
	int currentEnemyCountPerSwirlType{ 0 };
	int currentEnemyUnit{ 0 };


	//--------- CREATE ENEMIES

	EnemyInstanceData currentEnemyInstanceData{};

	currentEnemyInstanceData.EnemyManager = this;
	currentEnemyInstanceData.Grid = m_Grid;
	currentEnemyInstanceData.Scene = &scene;
	currentEnemyInstanceData.Player = m_Player.GetGameObjectHandle();

	CEnemyFormation* self = this;
	auto checkEnemyDeaths = [self]() {self->CheckEnemyDeaths(); };

	int currentEnemyIndex = 0;
	for (int rowIdx = 0; rowIdx < m_EnemyGridData.NumberOfEnemyRows; rowIdx++)
	{
		for (int colIdx = 0; colIdx < m_EnemyGridData.NumberOfEnemiesPerRow; colIdx++)
		{
			currentRelativePos.y = spaceBetweenRows * rowIdx;
			currentRelativePos.x = spaceBetweenEnemies * colIdx;

			currentEnemyInstanceData.RelativePos = currentRelativePos;
			currentEnemyInstanceData.MoveTowardsLeft = colIdx > (m_EnemyGridData.NumberOfEnemiesPerRow / 2);

			Enemy enemy{ currentEnemyInstanceData , m_EnemyType };
			SwirlBehaviour swirlBehaviour{ m_EnemyUnitSwirlData.SwirlDataPerUnit[currentEnemyUnit] };
			enemy.SetStartingFormationBehaviour(swirlBehaviour);

			m_Enemies.push_back(enemy);

			++currentEnemyCountPerSwirlType;

			if (currentEnemyCountPerSwirlType >= maxEnemiesPerSwirlType)
			{
				currentEnemyCountPerSwirlType = 0;
				currentEnemyUnit = (currentEnemyUnit + 1) % (int)m_EnemyUnitSwirlData.SwirlDataPerUnit.size();
			}

			m_Enemies.back().GetGameObject().SetActive(false);
			m_Enemies.back().OnDie().Subscribe(checkEnemyDeaths);

			++currentEnemyIndex;
		}
	}
}

void dae::CEnemyFormation::CheckEnemyDeaths()
{
	for (size_t i = 0; i < m_Enemies.size(); i++)
	{
		if (m_Enemies[i].GetGameObject().IsActive())
		{
			return;
		}
	}

	m_OnFormationDeath.Invoke();
}
