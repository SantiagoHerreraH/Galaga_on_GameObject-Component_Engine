#include "StatDisplayer.h"

dae::CStatDisplayer::CStatDisplayer(const StatDisplayData& statDisplayData) : 
	m_StatDisplayData(statDisplayData)
{

	m_StatName = std::make_shared<GameObject>();

	m_StatName->Transform().SetLocalPositionX(m_StatDisplayData.Where.x);
	m_StatName->Transform().SetLocalPositionY(m_StatDisplayData.Where.y);

	CText currentText{ m_StatDisplayData.StatNameTextData };
	currentText.Center();

	m_StatName->AddComponent(currentText);

	//---------------------

	m_StatValue = std::make_shared<GameObject>();


	m_StatValue->Transform().SetParent(*m_StatName, ETransformReparentType::KeepLocalTransform);

	glm::vec2 scoreOffset = currentText.GetScaledSize() + m_StatDisplayData.StatValueOffsetFromStatName;
	m_StatValue->Transform().SetLocalPositionX(scoreOffset.x);
	m_StatValue->Transform().SetLocalPositionY(scoreOffset.y);

	currentText.SetText("00");
	currentText.Center();
	m_StatValue->AddComponent(currentText);

}

void dae::CStatDisplayer::Start()
{
	if (!m_Subscribed)
	{
		m_Subscribed = true; //in case of multiple starts -> one gameobj in multiple scenes
		auto statValueGameObj = m_StatValue;

		GameObject* gameObjPtr = m_StatDisplayData.FromWho != nullptr ? m_StatDisplayData.FromWho.get() : &Owner();

		gameObjPtr->GetComponent<CStatController>()->OnCurrentStatChange(m_StatDisplayData.StatTypeToDisplay).Subscribe(
			[statValueGameObj](int statValue) mutable {
				statValueGameObj->GetComponent<CText>()->SetText(std::to_string(statValue));
			});
	}

	SceneManager::GetInstance().GetCurrentScene().AddGameObjectHandle(m_StatName);
	SceneManager::GetInstance().GetCurrentScene().AddGameObjectHandle(m_StatValue);
	
}

void dae::CStatDisplayer::StopDisplaying()
{
	m_StatName->SetActive(false);
	m_StatValue->SetActive(false);
}

void dae::CStatDisplayer::StartDisplaying()
{
	m_StatName->SetActive(true);
	m_StatValue->SetActive(true);
}
