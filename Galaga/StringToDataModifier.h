#pragma once

#include <functional>
#include <unordered_map>
#include <string>

namespace dae {

	template<typename DataTypeToModify>
	class StringToDataModifier {

		using DataModFunction = std::function<void(DataTypeToModify&)>;

		static inline std::unordered_map<std::string, DataModFunction> m_Name_To_DataConversionFunction;

	public:

		static void AddDataModificationFunction(const std::string& name, const DataModFunction& func) {

			m_Name_To_DataConversionFunction[name] = func;
		}
		static bool ApplyDataModification(const std::string& name, DataTypeToModify& outData) {

			if (m_Name_To_DataConversionFunction.contains(name))
			{
				m_Name_To_DataConversionFunction[name](outData);

				return true;
			}

			return false;
		}

	};
}