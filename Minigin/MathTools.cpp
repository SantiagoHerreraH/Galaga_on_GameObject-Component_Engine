#include "MathTools.h"

int dae::MathTools::Loop(int value, int min, int max)
{
	return 
		value > max ?
		value % max :
		value < min ?
		max + (value % max) :
		value;
}

bool dae::MathTools::IsOutOfBounds(int value, int min, int max)
{
	return value > max || value < min;
}

std::pair<std::string, int> dae::MathTools::GetMaxEntry(const std::unordered_map<std::string, int>& nameToValue)
{
    if (nameToValue.empty())
        return std::pair{ "No Name", 0 };

    auto maxIt = std::max_element(
        nameToValue.begin(), nameToValue.end(),
        [](const auto& a, const auto& b) {
            return a.second < b.second;
        });

    return *maxIt;
}