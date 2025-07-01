#pragma once
#include <vec2.hpp>
#include <string>
#include <glm.hpp>
#include <memory>
#include "Rect.h"
#include "Transform.h"

struct SDL_Texture;
namespace dae
{
	/**
	 * Simple RAII wrapper for an SDL_Texture
	 */
	class TextureData final
	{
	public:
		SDL_Texture* GetSDLTexture() const;
		explicit TextureData(SDL_Texture* texture);
		explicit TextureData(const std::string& fullPath);
		~TextureData();

		glm::ivec2 GetPixelSize() const;

		TextureData(const TextureData &) = delete;
		TextureData(TextureData &&) = delete;
		TextureData & operator= (const TextureData &) = delete;
		TextureData & operator= (const TextureData &&) = delete;
	private:
		SDL_Texture* m_texture;
	};

	class CTextureHandle final{

	public:
		CTextureHandle() {}
		CTextureHandle(const std::string& path);
		bool IsValid();
		bool SetTexture(const std::string& path, bool resetTextureTransform = true);
		void SetTextureView(const Rect& rect);
		const Rect* GetTextureView()const;
		const TextureData* Data()const;
		void Center();
		void SetTextureTransform(TransformData& transformData);
		const TransformData& GetTextureTransform()const { return m_TransformData; }

	private:
		bool m_SetTextureView{ false };
		Rect m_TextureView{};
		TransformData m_TransformData;
		std::string m_TexturePath;
		std::shared_ptr<TextureData> m_TextureData;
		bool m_IsValid{ false };

	};

	class SpriteSheet final {
	public:
		SpriteSheet(const CTextureHandle& texture, unsigned int numberOfRows, unsigned int numberOfColumns, unsigned int numberOfEmptyPixelsHorizontal = 0, unsigned int numberOfEmptyPixelsVertical = 0);

		const CTextureHandle& GetFrame(int rowIndex, int columnIndex);

		CTextureHandle& TextureHandle();

		int GetOriginalCellWidth()const;
		int GetOriginalCellHeight()const;

		float GetScaledCellWidth()const;
		float GetScaledCellHeight()const;

	private:

		void FrameTexture(int rowIndex, int columnIndex);
		CTextureHandle m_Texture;
		int m_NumberOfRows;
		int m_NumberOfColumns;
		int m_NumberOfEmptyPixelsBetweenColumns;
		int m_NumberOfEmptyPixelsBetweenRows;
		int m_CellWidth{};
		int m_CellHeight{};

	};
}
