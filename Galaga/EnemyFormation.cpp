#include "EnemyFormation.h"
#include "MainMenu.h"

dae::CEnemyFormation::CEnemyFormation(
	const glm::vec2& position,
	float secondsBetweenSpawning,
	float secondsBetweenUnitSpawning,
	const Player& player,
	const EnemyFormationType& enemyManagerType) :
	m_StartPosition(position),
	m_SecondsBetweenSpawning(secondsBetweenSpawning),
	m_SecondsBetweenUnitSpawning(secondsBetweenUnitSpawning),
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

bool dae::CEnemyFormation::IsEnemySwirlBehaviourFinished() const
{
	return m_IsEnemySwirlBehaviourFinished;
}

void dae::CEnemyFormation::StartEnemySwirlBehaviour()
{
	if (!m_CreatedFormationTimer)
	{
		m_CalledStartFormationBeforeStart = true;
		return;
	}

	Scene& scene = SceneManager::GetInstance().GetCurrentScene();
	TimerSystem* TIMERSYSTEM = &TimerSystem::GetFromScene(&scene);
	CEnemyFormation* self{ this };

	int currentSpawningIndex = 0;
	TimerKey timerKey = m_FormationTimerKey;

	TIMERSYSTEM->ClearTimerEvents(timerKey);

	TIMERSYSTEM->TimerAt(timerKey).GetOnEndEvent().Subscribe([currentSpawningIndex, self, TIMERSYSTEM, timerKey]() mutable
		{

			self->m_Enemies[currentSpawningIndex].StartFormation();

			if (currentSpawningIndex < (self->m_Enemies.size() - 1))
			{
				++currentSpawningIndex;

				bool nextUnitDuration{ (currentSpawningIndex % self->m_EnemyUnitSwirlData.EnemiesInAUnit) == 0};

				if (nextUnitDuration)
				{
					TIMERSYSTEM->SetTimerDuration(timerKey, self->m_SecondsBetweenUnitSpawning, false);
				}
				else
				{
					TIMERSYSTEM->SetTimerDuration(timerKey, self->m_SecondsBetweenSpawning, false);
				}
				
				TIMERSYSTEM->RestartTimer(timerKey);
			}

		});

	TIMERSYSTEM->RestartTimer(m_FormationTimerKey);
	m_IsEnemySwirlBehaviourFinished = false;
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
		int indexOfArray = Random::GetRandomBetweenRange(0, int(permittedIndices.size() - 1));
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
	auto startSendingTroops = [self]() 
		{
			if (self->IsEnemySwirlBehaviourFinished())
			{
				self->StartSendingTroops();
			}
		};

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
	float startGridX = m_StartPosition.x;
	Scene& scene = SceneManager::GetInstance().GetCurrentScene();
	TimerSystem* TIMERSYSTEM = &TimerSystem::GetFromScene(&scene);
	GameObjectHandle grid = scene.CreateGameObject();
	m_Grid = grid;

	m_Grid->Transform().SetLocalPositionX(m_StartPosition.x);
	m_Grid->Transform().SetLocalPositionY(m_StartPosition.y);

	// ----- GRID MOVEMENT -----

	float movementSpeed{ 2 };
	float movementRange{ 1 };
	float currentTime{ };

	auto moveHorizontal = [grid, movementSpeed, movementRange, currentTime, startGridX]()mutable
		{
			currentTime += movementSpeed * Time::GetInstance().GetElapsedSeconds();

			if (currentTime > M_PI)
			{
				currentTime -= M_PI;
			}

			float value = std::sin(currentTime) * movementRange;

			grid->Transform().SetLocalPositionX(startGridX + value);
		};

	float scaleSpeed{ 2 };
	float scaleRange{ 0.1f };
	float scaleMidPoint{ 1 };

	auto scale = [grid, scaleSpeed, scaleRange, scaleMidPoint, currentTime]()mutable
		{
			currentTime += scaleSpeed * Time::GetInstance().GetElapsedSeconds();

			if (currentTime > M_PI)
			{
				currentTime -= M_PI;
			}

			float value = std::sin(currentTime) * scaleRange;

			grid->Transform().SetLocalScaleX(scaleMidPoint + value);
		};

	TimerKey movementTimerKey = TIMERSYSTEM->AddTimer(Timer{ -1,false, "GridMovement" });
	Timer& movementTimer = TIMERSYSTEM->TimerAt(movementTimerKey);

	Event<> movementTimerUpdate{};
	movementTimerUpdate.Subscribe(moveHorizontal);
	movementTimerUpdate.Subscribe(scale);

	movementTimer.SetOnUpdateEvent(movementTimerUpdate);
}

dae::CButtonGrid* dae::MainMenu::MainMenuCreator::GetGrid()
{
	return m_ButtonGrid->GetComponent<CButtonGrid>();
}

void dae::CEnemyFormation::CreateEnemies()
{
	Scene& scene = SceneManager::GetInstance().GetCurrentScene();

	const float spaceBetweenEnemies{ m_EnemyGridData.SpaceBetweenEnemies };
	const float spaceBetweenRows{ m_EnemyGridData.SpaceBetweenRows };


	glm::vec2 startOffsetFromPos{};
	startOffsetFromPos.x = (m_EnemyGridData.NumberOfEnemiesPerRow * spaceBetweenEnemies)/2.f;
	startOffsetFromPos.y = (m_EnemyGridData.NumberOfEnemyRows * spaceBetweenRows)/2.f;


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
			currentRelativePos.y = (spaceBetweenRows * rowIdx) - startOffsetFromPos.y;
			currentRelativePos.x = (spaceBetweenEnemies * colIdx) - startOffsetFromPos.x;

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

	m_Enemies.back().SubscribeOnEndStartingFormationBehaviour([self](GameObject&){
		self->m_IsEnemySwirlBehaviourFinished = true;
		self->m_OnEndSwirlFormation.Invoke(); });
	
}

void dae::CEnemyFormation::CheckEnemyDeaths()
{
	++m_EnemyDeathCount;

	if (m_EnemyDeathCount >= m_Enemies.size())
	{

		m_EnemyDeathCount = 0;

		m_OnFormationDeath.Invoke();
	}

}
