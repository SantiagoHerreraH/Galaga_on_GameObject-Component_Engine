#include "SpriteSheet.h"

namespace dae {

	SpriteSheet::SpriteSheet(const CTextureHandle& texture, unsigned int numberOfRows, unsigned int numberOfColumns, unsigned int numberOfEmptyPixeBetweenColumns, unsigned int numberOfEmptyPixeBetweenRows) :
		m_Texture(texture),
		m_NumberOfRows(numberOfRows),
		m_NumberOfColumns(numberOfColumns),
		m_NumberOfEmptyPixelsBetweenColumns(numberOfEmptyPixeBetweenColumns),
		m_NumberOfEmptyPixelsBetweenRows(numberOfEmptyPixeBetweenRows)
	{

		m_CellWidth = int(m_Texture.Data()->GetPixelSize().x / (float)m_NumberOfColumns);
		m_CellHeight = int(m_Texture.Data()->GetPixelSize().y / (float)m_NumberOfRows);

		glm::vec2 size = { GetOriginalCellWidth() , GetOriginalCellHeight() };
		TransformData transformData{ m_Texture.GetTextureTransform() };
		transformData.Position.x = (-size.x / 2.f) * m_Texture.GetTextureTransform().Scale.x;
		transformData.Position.y = (-size.y / 2.f) * m_Texture.GetTextureTransform().Scale.y;
		m_Texture.SetTextureTransform(transformData);
	}

	const CTextureHandle& SpriteSheet::GetFrame(int rowIndex, int columnIndex)
	{
		FrameTexture(rowIndex, columnIndex);
		return m_Texture;
	}

	CTextureHandle& SpriteSheet::TextureHandle()
	{
		return m_Texture;
	}

	int SpriteSheet::GetOriginalCellWidth() const
	{
		return m_CellWidth - m_NumberOfEmptyPixelsBetweenColumns;
	}

	int SpriteSheet::GetOriginalCellHeight() const
	{
		return m_CellHeight - m_NumberOfEmptyPixelsBetweenRows;
	}

	float SpriteSheet::GetScaledCellWidth() const
	{
		return (m_CellWidth - m_NumberOfEmptyPixelsBetweenColumns) * m_Texture.GetTextureTransform().Scale.x;
	}

	float SpriteSheet::GetScaledCellHeight() const
	{
		return (m_CellHeight - m_NumberOfEmptyPixelsBetweenRows) * m_Texture.GetTextureTransform().Scale.y;
	}

	void SpriteSheet::FrameTexture(int rowIndex, int columnIndex)
	{
		m_Texture.SetTextureView(Rect{
			int((rowIndex * m_CellHeight) + (m_NumberOfEmptyPixelsBetweenRows / 2.f)),
			int((columnIndex * m_CellWidth) + (m_NumberOfEmptyPixelsBetweenColumns / 2.f)),
			m_CellWidth - m_NumberOfEmptyPixelsBetweenColumns,
			m_CellHeight - m_NumberOfEmptyPixelsBetweenRows });
	}
}
