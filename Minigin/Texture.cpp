#include <SDL.h>
#include <SDL_image.h>
#include "Texture.h"
#include "Renderer.h"
#include "ResourceManager.h"
#include "GameObject.h"
#include <stdexcept>

#pragma region Texture Data

dae::Texture::~Texture()
{
	SDL_DestroyTexture(m_texture);
}

glm::ivec2 dae::Texture::GetPixelSize() const
{
	return m_PixelSize;
}

SDL_Texture* dae::Texture::GetSDLTexture() const
{
	return m_texture;
}

dae::Texture::Texture(const std::string &fullPath)
{
	m_texture = IMG_LoadTexture(Renderer::GetInstance().GetSDLRenderer(), fullPath.c_str());
	if (m_texture == nullptr)
		throw std::runtime_error(std::string("Failed to load texture: ") + SDL_GetError());
		

	SDL_Rect dst;
	SDL_QueryTexture(GetSDLTexture(), nullptr, nullptr, &dst.w, &dst.h);
	m_PixelSize = glm::ivec2{ dst.w,dst.h };
}

dae::Texture::Texture(SDL_Texture* texture)	: m_texture{ texture } 
{
	assert(m_texture != nullptr);

	SDL_Rect dst;
	SDL_QueryTexture(GetSDLTexture(), nullptr, nullptr, &dst.w, &dst.h);
	m_PixelSize = glm::ivec2{ dst.w,dst.h };
}

#pragma endregion
