#pragma once

#include "CTextureHandle.h"

namespace dae {
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