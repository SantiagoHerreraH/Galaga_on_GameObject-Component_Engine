#pragma once
#include <SDL.h>
#include "Singleton.h"
#include <glm.hpp>
#include "Rect.h"

namespace dae
{
	class TextureData;
	/**
	 * Simple RAII wrapper for the SDL renderer
	 */
	class Renderer final : public Singleton<Renderer>
	{
		SDL_Renderer* m_renderer{};
		SDL_Window* m_window{};
		SDL_Color m_clearColor{};	
	public:
		void Init(SDL_Window* window);
		void Render() const;
		void Destroy();

		void RenderTexture(const TextureData& texture, glm::vec3 position, glm::vec3 rotation, glm::vec3 scale, const Rect* textureView = nullptr);
		void RenderTexture(const TextureData& texture, float x, float y) const;
		void RenderTexture(const TextureData& texture, float x, float y, float width, float height) const;

		void DrawRect(int left, int top, int width, int height, bool fill = false, const SDL_Color& color = {255, 255, 255, 255})const;
		void DrawCross(const glm::ivec2& center, int size, const SDL_Color& color)const;
		void DrawDot(const glm::ivec2& center, const SDL_Color& color)const;

		SDL_Renderer* GetSDLRenderer() const;

		const SDL_Color& GetBackgroundColor() const { return m_clearColor; }
		void SetBackgroundColor(const SDL_Color& color) { m_clearColor = color; }
	};
}

