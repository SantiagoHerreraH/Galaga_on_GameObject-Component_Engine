#pragma once

#include <random>
#include <SDL.h>
#include <glm.hpp>

namespace dae {

	class Random {

	public:
		static int GetRandomBetweenRange(int min, int max)
		{
			//if you don't add the + 1, the range can never reach max
			return (std::rand() % (max - min + 1)) + min;
		}

		static float GetRandomBetweenRange(float min, float max)
		{
			float scale = static_cast<float>(std::rand()) / static_cast<float>(RAND_MAX);
			return min + scale * (max - min);
		}

		static SDL_Color GetRandomBetweenRange(SDL_Color min, SDL_Color max)
		{
			SDL_Color color{};
			 color.r = Uint8(Random::GetRandomBetweenRange(min.r, max.r));
			 color.g = Uint8(Random::GetRandomBetweenRange(min.g, max.g));
			 color.b = Uint8(Random::GetRandomBetweenRange(min.b, max.b));
			 color.a = Uint8(Random::GetRandomBetweenRange(min.a, max.a));
			 return color;
		}

		static glm::vec2 GetRandomBetweenRange(glm::vec2 min, glm::vec2 max)
		{
			//if you don't add the + 1, the range can never reach max
			return glm::vec2{ Random::GetRandomBetweenRange(min.x, max.x), Random::GetRandomBetweenRange(min.y, max.y) };
		}
	};
}
