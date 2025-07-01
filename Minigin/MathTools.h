#pragma once
#include <glm.hpp>
#include <string>
#include <unordered_map>

namespace dae {

	class MathTools {

	public:
		static int Loop(int value, int min, int max);
		static bool IsOutOfBounds(int value, int min, int max);
		static std::pair<std::string, int> GetMaxEntry(const std::unordered_map<std::string, int>& nameToValue);
	};

}