#include <stdexcept>
#include "Renderer.h"
#include "SceneManager.h"
#include "Texture.h"
#include <glm.hpp>

int GetOpenGLDriverIndex()
{
	auto openglIndex = -1;
	const auto driverCount = SDL_GetNumRenderDrivers();
	for (auto i = 0; i < driverCount; i++)
	{
		SDL_RendererInfo info;
		if (!SDL_GetRenderDriverInfo(i, &info))
			if (!strcmp(info.name, "opengl"))
				openglIndex = i;
	}
	return openglIndex;
}

void dae::Renderer::Init(SDL_Window* window)
{
	m_window = window;
	m_renderer = SDL_CreateRenderer(window, GetOpenGLDriverIndex(), SDL_RENDERER_ACCELERATED);
	if (m_renderer == nullptr) 
	{
		throw std::runtime_error(std::string("SDL_CreateRenderer Error: ") + SDL_GetError());
	}
}

void dae::Renderer::Render() const
{
	const auto& color = GetBackgroundColor();
	SDL_SetRenderDrawColor(m_renderer, color.r, color.g, color.b, color.a);
	SDL_RenderClear(m_renderer);

	SceneManager::GetInstance().Render();
	
	SDL_RenderPresent(m_renderer);
}

void dae::Renderer::Destroy()
{
	if (m_renderer != nullptr)
	{
		SDL_DestroyRenderer(m_renderer);
		m_renderer = nullptr;
	}
}

void dae::Renderer::RenderTexture(const TextureData& texture, glm::vec3 position, glm::vec3 rotation, glm::vec3 scale, const Rect* textureView)
{
	// Query texture dimensions
	int texWidth, texHeight;
	SDL_QueryTexture(texture.GetSDLTexture(), NULL, NULL, &texWidth, &texHeight);

	// Apply scaling
	texWidth = static_cast<int>(texWidth * scale.x);
	texHeight = static_cast<int>(texHeight * scale.y);

	SDL_Rect* srcRectPtr{nullptr};
	SDL_Rect srcRect{};

	if (textureView)
	{
		srcRect = { textureView->Left, textureView->Top, textureView->Width, textureView->Height };
		srcRectPtr = &srcRect;
	}

	SDL_Rect dstRect = { static_cast<int>(position.x), static_cast<int>(position.y), texWidth, texHeight };

	// Render with rotation (only using Z-axis rotation, as SDL is 2D)
	SDL_RenderCopyEx(GetSDLRenderer(), texture.GetSDLTexture(), srcRectPtr, &dstRect, rotation.z, NULL, SDL_FLIP_NONE);
}

void dae::Renderer::RenderTexture(const TextureData& texture, const float x, const float y) const
{
	SDL_Rect dst{};
	dst.x = static_cast<int>(x);
	dst.y = static_cast<int>(y);
	SDL_QueryTexture(texture.GetSDLTexture(), nullptr, nullptr, &dst.w, &dst.h);
	SDL_RenderCopy(GetSDLRenderer(), texture.GetSDLTexture(), nullptr, &dst);
}

void dae::Renderer::RenderTexture(const TextureData& texture, const float x, const float y, const float width, const float height) const
{
	SDL_Rect dst{};
	dst.x = static_cast<int>(x);
	dst.y = static_cast<int>(y);
	dst.w = static_cast<int>(width);
	dst.h = static_cast<int>(height);
	SDL_RenderCopy(GetSDLRenderer(), texture.GetSDLTexture(), nullptr, &dst);
}

void dae::Renderer::DrawRect(int left, int top, int width, int height, bool fill, const SDL_Color& color)const 
{
	SDL_Rect rect = { left, top, width, height };

	// Set draw color
	SDL_SetRenderDrawColor(GetSDLRenderer(), color.r, color.g, color.b, color.a);

	// Draw filled or outlined rectangle
	if (fill) {
		SDL_RenderFillRect(GetSDLRenderer(), &rect);
	}
	else {
		SDL_RenderDrawRect(GetSDLRenderer(), &rect);
	}
}

void dae::Renderer::DrawCross(const glm::ivec2& center, int size, const SDL_Color& color) const
{
	// Set draw color
	SDL_SetRenderDrawColor(GetSDLRenderer(), color.r, color.g, color.b, color.a);
	// Horizontal line
	SDL_RenderDrawLine(GetSDLRenderer(), center.x - size, center.y, center.x + size, center.y);
	// Vertical line
	SDL_RenderDrawLine(GetSDLRenderer(), center.x, center.y - size, center.x, center.y + size);
}

void dae::Renderer::DrawDot(const glm::ivec2& center, const SDL_Color& color) const
{

	// Set draw color
	SDL_SetRenderDrawColor(GetSDLRenderer(), color.r, color.g, color.b, color.a);
	
	SDL_RenderDrawLine(GetSDLRenderer(), center.x , center.y, center.x , center.y);
}

SDL_Renderer* dae::Renderer::GetSDLRenderer() const { return m_renderer; }
