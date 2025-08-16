#pragma once
#include <vec2.hpp>
#include <string>
#include <glm.hpp>
#include <memory>
#include "Rect.h"
#include "Transform.h"
#include "Component.h"

struct SDL_Texture;
namespace dae
{
	/**
	 * Simple RAII wrapper for an SDL_Texture
	 */
	class Texture final
	{
	public:
		SDL_Texture* GetSDLTexture() const;
		explicit Texture(SDL_Texture* texture);
		explicit Texture(const std::string& fullPath);
		~Texture();

		glm::ivec2 GetPixelSize() const;

		Texture(const Texture &) = delete;
		Texture(Texture &&) = delete;
		Texture & operator= (const Texture &) = delete;
		Texture & operator= (const Texture &&) = delete;
	private:
		glm::ivec2 m_PixelSize;
		SDL_Texture* m_texture;
	};

}
