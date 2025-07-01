#pragma once

#include <unordered_map>
#include "EventSystem.h"

namespace dae {

	template<typename StatType>
	struct Stat
	{
		StatType CurrentStat;
		StatType MaxStat;
		StatType MaxLimitStat;

		Event<> OnResetCurrentStat;
		Event<> OnCurrentStatZeroOrLess;
		Event<StatType> OnCurrentStatChange;
	};

	enum class StatCategory {
		CurrentStat,
		MaxStat,
		MaxLimitStat 
	};

	template<typename StatKey, typename StatValueType>
	class StatController {
	public:

		void CreateStat(const StatKey& statKey, const Stat<StatValueType>& stat, bool* outChangedPrevious = nullptr);
		void RemoveStat(const StatKey& statKey);
		void ResetCurrentStat(const StatKey& statKey);

		bool HasStat(const StatKey& statKey)const;
		const Stat<StatValueType>& GetStat(const StatKey& statKey)const;
		const StatValueType& GetStat(const StatKey& statKey, StatCategory statType = StatCategory::CurrentStat)const;
		void SetStat(const StatKey& statKey, const StatValueType& value, StatCategory statType = StatCategory::CurrentStat);
		void OffsetStat(const StatKey& statKey, const StatValueType& offset, StatCategory statType = StatCategory::CurrentStat);
		void ScaleStat(const StatKey& statKey, const StatValueType& scalar, StatCategory statType = StatCategory::CurrentStat);

		Event<>& OnResetCurrentStatEvent(const StatKey& statKey);
		Event<>& OnCurrentStatZeroOrLess(const StatKey& statKey);
		Event<StatValueType>& OnCurrentStatChange(const StatKey& statKey);

	private:
		std::unordered_map<StatKey, Stat<StatValueType>> m_Stats;

	};

	template<typename StatKey, typename StatValueType>
	inline void StatController<StatKey, StatValueType>::CreateStat(const StatKey& statKey, const Stat<StatValueType>& stat, bool* outChangedPrevious)
	{
		if (m_Stats.contains(statKey))
		{
			m_Stats[statKey] = stat;
			if (outChangedPrevious)
			{
				*outChangedPrevious = true;
			}
		}
		else
		{
			m_Stats.emplace(std::pair(statKey, stat));
		}
	}

	template<typename StatKey, typename StatValueType>
	inline void StatController<StatKey, StatValueType>::RemoveStat(const StatKey& statKey)
	{
		m_Stats.erase(statKey);
	}

	template<typename StatKey, typename StatValueType>
	inline void StatController<StatKey, StatValueType>::ResetCurrentStat(const StatKey& statKey)
	{
		m_Stats.at(statKey).CurrentStat = m_Stats.at(statKey).MaxStat;
		m_Stats.at(statKey).OnResetCurrentStat.Invoke();

		m_Stats.at(statKey).OnCurrentStatChange.Invoke(m_Stats.at(statKey).CurrentStat);
	}

	template<typename StatKey, typename StatValueType>
	inline bool StatController<StatKey, StatValueType>::HasStat(const StatKey& statKey) const
	{
		return m_Stats.contains(statKey);
	}

	template<typename StatKey, typename StatValueType>
	inline const Stat<StatValueType>& StatController<StatKey, StatValueType>::GetStat(const StatKey& statKey) const
	{
		return m_Stats.at(statKey);
	}

	template<typename StatKey, typename StatValueType>
	inline const StatValueType& StatController<StatKey, StatValueType>::GetStat(const StatKey& statKey, StatCategory statCategory) const
	{
		switch (statCategory)
		{
		case dae::StatCategory::CurrentStat:
			return m_Stats.at(statKey).CurrentStat;

		case dae::StatCategory::MaxStat:

			return m_Stats.at(statKey).MaxStat;

		case dae::StatCategory::MaxLimitStat:

			return m_Stats.at(statKey).MaxLimitStat;

		default:
			assert(false);
			return m_Stats.at(statKey).CurrentStat;
		}
	}

	template<typename StatKey, typename StatValueType>
	inline void StatController<StatKey, StatValueType>::SetStat(const StatKey& statKey, const StatValueType& value, StatCategory statCategory)
	{
		switch (statCategory)
		{
		case StatCategory::CurrentStat:
			m_Stats.at(statKey).CurrentStat =
				value > m_Stats.at(statKey).MaxStat ?
				m_Stats.at(statKey).MaxStat :
				value < 0 ?
				0 : value;
			break;
		case StatCategory::MaxStat:

			m_Stats.at(statKey).MaxStat = 
				value > m_Stats.at(statKey).MaxLimitStat ?
				m_Stats.at(statKey).MaxLimitStat :
				value < 0 ?
				0 : value;

			m_Stats.at(statKey).CurrentStat = 
				m_Stats.at(statKey).CurrentStat > m_Stats.at(statKey).MaxStat ?
				m_Stats.at(statKey).MaxStat :
				m_Stats.at(statKey).CurrentStat;
			break;
		case StatCategory::MaxLimitStat:
			m_Stats.at(statKey).MaxLimitStat = value;
			m_Stats.at(statKey).MaxStat		= m_Stats.at(statKey).MaxStat		> value ? value : m_Stats.at(statKey).MaxStat;
			m_Stats.at(statKey).CurrentStat = m_Stats.at(statKey).CurrentStat	> value ? value : m_Stats.at(statKey).CurrentStat;

			break;
		}


		m_Stats.at(statKey).OnCurrentStatChange.Invoke(m_Stats.at(statKey).CurrentStat);

		if (m_Stats.at(statKey).CurrentStat <= 0)
		{
			m_Stats.at(statKey).OnCurrentStatZeroOrLess.Invoke();
		}
	}

	template<typename StatKey, typename StatValueType>
	inline void StatController<StatKey, StatValueType>::OffsetStat(const StatKey& statKey, const StatValueType& offset, StatCategory statType)
	{
		SetStat(statKey, GetStat(statKey, statType) + offset, statType);
	}

	template<typename StatKey, typename StatValueType>
	inline void StatController<StatKey, StatValueType>::ScaleStat(const StatKey& statKey, const StatValueType& scalar, StatCategory statType)
	{
		SetStat(statKey, GetStat(statKey, statType) * scalar, statType);
	}

	template<typename StatKey, typename StatValueType>
	inline Event<>& StatController<StatKey, StatValueType>::OnResetCurrentStatEvent(const StatKey& statKey)
	{
		return m_Stats.at(statKey).OnResetCurrentStatEvent;
	}

	template<typename StatKey, typename StatValueType>
	inline Event<>& StatController<StatKey, StatValueType>::OnCurrentStatZeroOrLess(const StatKey& statKey)
	{
		return m_Stats.at(statKey).OnCurrentStatZeroOrLess;
	}

	template<typename StatKey, typename StatValueType>
	inline Event<StatValueType>& StatController<StatKey, StatValueType>::OnCurrentStatChange(const StatKey& statKey)
	{
		return m_Stats.at(statKey).OnCurrentStatChange;
	}

}


