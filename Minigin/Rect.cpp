#include "Rect.h"
#include "Renderer.h"

dae::SDebugDrawRectTrigger::SDebugDrawRectTrigger(Pillar::URegistry& registry)
{
	registry.GetComponents(m_RectTriggers);
}


void dae::SDebugDrawRectTrigger::Render() 
{
	SDL_Color color{};

	for (size_t archIdx = 0; archIdx < m_RectTriggers.NumOfArchetypes(); archIdx++)
	{
		for (size_t compIdx = 0; compIdx < m_RectTriggers.NumOfActivatedComponents(archIdx); compIdx++)
		{
			const Rect& rect = m_RectTriggers.ConstAt(archIdx, compIdx).Rect;

			color = m_RectTriggers.ConstAt(archIdx, compIdx).StayOverlapEntities.size() == 0 ? 
				SDL_Color{0,255,0,255} : SDL_Color{255, 0, 0, 255};

			Renderer::GetInstance().DrawRect(rect.Left, rect.Top, rect.Width, rect.Height, false, color);
		}
	}
}

dae::SCheckRectTriggerOverlap::SCheckRectTriggerOverlap(Pillar::URegistry& registry) :
	m_Registry(registry)
{
	registry.GetComponents(m_RectTriggers);
}

void dae::SCheckRectTriggerOverlap::Start()
{
	Pillar::FEntity currentEntity{};
	for (size_t archIdx = 0; archIdx < m_RectTriggers.NumOfArchetypes(); archIdx++)
	{
		for (size_t compIdx = 0; compIdx < m_RectTriggers.NumOfComponents(archIdx); compIdx++)
		{
			m_Registry.GetEntity<CRectTrigger>(currentEntity, m_RectTriggers.GetArchetype(archIdx), compIdx);
			m_RectTriggers.At(archIdx, compIdx).Self = currentEntity;
		}
	}
}

void dae::SCheckRectTriggerOverlap::FixedUpdate()
{
	std::vector<CRectTrigger> triggers;
	std::vector<CRectTrigger> otherTriggers;

	size_t firstActivatedSize{};
	size_t secondActivatedSize{};

	size_t compSize{};

	bool isAlreadyStayOverlapping{ false };

	for (size_t archIdx = 0; archIdx < m_RectTriggers.NumOfArchetypes(); archIdx++)
	{
		firstActivatedSize = m_RectTriggers.NumOfActivatedComponents(archIdx);
		compSize = m_RectTriggers.NumOfComponents(archIdx);
		triggers = m_RectTriggers.Move(archIdx);

		for (size_t compIdx = 0; compIdx < firstActivatedSize; compIdx++)
		{
			triggers[compIdx].BeginOverlapEntities.clear();
			triggers[compIdx].EndOverlapEntities.clear();
		}
		for (size_t deactivatedCompIdx = firstActivatedSize; deactivatedCompIdx < compSize; deactivatedCompIdx++)
		{
			triggers[deactivatedCompIdx].StayOverlapEntities.clear();
		}

		m_RectTriggers.MoveBack(archIdx, std::move(triggers));
	}

	for (size_t archIdx = 0; archIdx < m_RectTriggers.NumOfArchetypes(); archIdx++)
	{
		firstActivatedSize = m_RectTriggers.NumOfActivatedComponents(archIdx);
		triggers = m_RectTriggers.Move(archIdx);

		for (size_t compIdx = 0; compIdx < firstActivatedSize; compIdx++)
		{

			for (size_t otherArchIdx = archIdx; otherArchIdx < m_RectTriggers.NumOfArchetypes(); otherArchIdx++)
			{
				secondActivatedSize = m_RectTriggers.NumOfActivatedComponents(otherArchIdx);

				if (archIdx == otherArchIdx)
				{

					for (size_t otherCompIdx = 0; otherCompIdx < secondActivatedSize; otherCompIdx++)
					{
						if ((archIdx == otherArchIdx && compIdx == otherCompIdx) ||
							triggers.at(compIdx).LayerNumberToIgnore == triggers.at(otherCompIdx).LayerNumber)
						{
							continue;
						}

						isAlreadyStayOverlapping = triggers.at(compIdx).IsStayOverlappingWith(triggers.at(otherCompIdx).Self);

						if (IsColliding(triggers.at(compIdx).Rect, triggers.at(otherCompIdx).Rect))
						{
							if (!isAlreadyStayOverlapping)
							{

								triggers.at(compIdx).BeginOverlapEntities.push_back(triggers.at(otherCompIdx).Self);
								triggers.at(otherCompIdx).BeginOverlapEntities.push_back(triggers.at(compIdx).Self);

								triggers.at(compIdx).StayOverlapEntities.push_back(triggers.at(otherCompIdx).Self);
								triggers.at(otherCompIdx).StayOverlapEntities.push_back(triggers.at(compIdx).Self);

							}
						}
						else if(isAlreadyStayOverlapping)
						{

							triggers.at(compIdx).EndOverlapEntities.push_back(triggers.at(otherCompIdx).Self);
							triggers.at(otherCompIdx).EndOverlapEntities.push_back(triggers.at(compIdx).Self);

							std::erase(triggers.at(compIdx).StayOverlapEntities, triggers.at(otherCompIdx).Self);
							std::erase(triggers.at(otherCompIdx).StayOverlapEntities, triggers.at(compIdx).Self);
						}
					}
				}
				else
				{
					otherTriggers = m_RectTriggers.Move(otherArchIdx);

					for (size_t otherCompIdx = 0; otherCompIdx < secondActivatedSize; otherCompIdx++)
					{
						if ((archIdx == otherArchIdx && compIdx == otherCompIdx) ||
							triggers.at(compIdx).LayerNumberToIgnore == otherTriggers.at(otherCompIdx).LayerNumber)
						{
							continue;
						}

						isAlreadyStayOverlapping = triggers.at(compIdx).IsStayOverlappingWith(otherTriggers.at(otherCompIdx).Self);


						if (IsColliding(triggers.at(compIdx).Rect, otherTriggers.at(otherCompIdx).Rect))
						{
							if (!isAlreadyStayOverlapping)
							{

								triggers.at(compIdx).BeginOverlapEntities.push_back(otherTriggers.at(otherCompIdx).Self);
								otherTriggers.at(otherCompIdx).BeginOverlapEntities.push_back(triggers.at(compIdx).Self);

								triggers.at(compIdx).StayOverlapEntities.push_back(otherTriggers.at(otherCompIdx).Self);
								otherTriggers.at(otherCompIdx).StayOverlapEntities.push_back(triggers.at(compIdx).Self);

							}
						}
						else if (isAlreadyStayOverlapping)
						{

							triggers.at(compIdx).EndOverlapEntities.push_back(otherTriggers.at(otherCompIdx).Self);
							otherTriggers.at(otherCompIdx).EndOverlapEntities.push_back(triggers.at(compIdx).Self);

							std::erase(triggers.at(compIdx).StayOverlapEntities, otherTriggers.at(otherCompIdx).Self);
							std::erase(otherTriggers.at(otherCompIdx).StayOverlapEntities, triggers.at(compIdx).Self);
						}
					}


					m_RectTriggers.MoveBack(otherArchIdx, std::move(otherTriggers));
				}

				
			}
		}


		m_RectTriggers.MoveBack(archIdx, std::move(triggers));

	}


	for (size_t archIdx = 0; archIdx < m_RectTriggers.NumOfArchetypes(); archIdx++)
	{
		firstActivatedSize = m_RectTriggers.NumOfActivatedComponents(archIdx);
		triggers = m_RectTriggers.Move(archIdx);

		for (size_t compIdx = 0; compIdx < firstActivatedSize; compIdx++)
		{
			if (triggers[compIdx].OnCollisionBeginEvent.Size() > 0)
			{
				for (size_t i = 0; i < triggers[compIdx].BeginOverlapEntities.size(); i++)
				{
					triggers[compIdx].OnCollisionBeginEvent.Invoke(triggers[compIdx].Self, triggers[compIdx].BeginOverlapEntities[i], &m_Registry);
				}
			}

			if (triggers[compIdx].OnCollisionStayEvent.Size() > 0)
			{
				for (size_t i = 0; i < triggers[compIdx].StayOverlapEntities.size(); i++)
				{
					triggers[compIdx].OnCollisionStayEvent.Invoke(triggers[compIdx].Self, triggers[compIdx].StayOverlapEntities[i], &m_Registry);
				}
			}
			
			if (triggers[compIdx].OnCollisionEndEvent.Size() > 0)
			{
				for (size_t i = 0; i < triggers[compIdx].EndOverlapEntities.size(); i++)
				{
					triggers[compIdx].OnCollisionEndEvent.Invoke(triggers[compIdx].Self, triggers[compIdx].EndOverlapEntities[i], &m_Registry);
				}
			}
			
		}

		m_RectTriggers.MoveBack(archIdx, std::move(triggers));
	}
}

dae::STransformRectTriggers::STransformRectTriggers(Pillar::URegistry& registry) :
	m_Transforms(CTransform{registry})
{
	Pillar::UComponentFilter filter{};

	filter.SetIncludingFilter(CTransform{ registry }, CRectTrigger{});

	registry.GetComponents(m_Transforms, filter);
	registry.GetComponents(m_RectTriggers, filter);

}

void dae::STransformRectTriggers::FixedUpdate()
{

	for (size_t archIdx = 0; archIdx < m_RectTriggers.NumOfArchetypes(); archIdx++)
	{
		for (size_t compIdx = 0; compIdx < m_RectTriggers.NumOfActivatedComponents(archIdx); compIdx++)
		{
			const TransformData& transformData = m_Transforms.At(archIdx, compIdx).GetWorldTransform();
			m_RectTriggers.At(archIdx, compIdx).Rect.Left = (int)transformData.Position.x + (int)m_RectTriggers.At(archIdx, compIdx).Offset.x;
			m_RectTriggers.At(archIdx, compIdx).Rect.Top = (int)transformData.Position.y + (int) m_RectTriggers.At(archIdx, compIdx).Offset.y;
		}
	}

	//Can't use move semantics because GetWorldTransform calculates graph with getcomponent, so if there is a parent that is of the same archetype it will not retrieve it
	//std::vector<CTransform>		transforms;
	//std::vector<CRectTrigger>	rectTriggers;
	//size_t activatedSize{};
	// 
	//for (size_t archIdx = 0; archIdx < m_RectTriggers.NumOfArchetypes(); archIdx++)
	//{
	//	activatedSize = m_Transforms.NumOfActivatedComponents(archIdx);
	//	transforms = m_Transforms.Move(archIdx);
	//	rectTriggers = m_RectTriggers.Move(archIdx);
	//
	//	for (size_t compIdx = 0; compIdx < activatedSize; compIdx++)
	//	{
	//		const TransformData& transformData = transforms[compIdx].GetWorldTransform();
	//		rectTriggers[compIdx].Rect.Left = (int)transformData.Position.x + (int)rectTriggers[compIdx].Offset.x;
	//		rectTriggers[compIdx].Rect.Top = (int)transformData.Position.y + (int)rectTriggers[compIdx].Offset.y;
	//	}
	//
	//	m_Transforms.MoveBack(archIdx, std::move(transforms));
	//	m_RectTriggers.MoveBack(archIdx, std::move(rectTriggers));
	//}
}

dae::SDrawRect::SDrawRect(Pillar::URegistry* registry) : m_Registry(registry)
{
}

void dae::SDrawRect::Render()
{
	m_Registry->DoTaskOverAllActivated<CDrawRect, void>([](CDrawRect& rect, void*) {

		Renderer::GetInstance().DrawRect(rect.Rect.Left, rect.Rect.Top, rect.Rect.Width, rect.Rect.Height, rect.Fill, rect.Color);

		});
}
