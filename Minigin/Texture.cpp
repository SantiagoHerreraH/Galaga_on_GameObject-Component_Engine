#include <SDL.h>
#include <SDL_image.h>
#include "Texture.h"
#include "Renderer.h"
#include "ResourceManager.h"
#include <stdexcept>

#pragma region Texture Data

dae::TextureData::~TextureData()
{
	SDL_DestroyTexture(m_texture);
}

glm::ivec2 dae::TextureData::GetPixelSize() const
{
	SDL_Rect dst;
	SDL_QueryTexture(GetSDLTexture(), nullptr, nullptr, &dst.w, &dst.h);
	return { dst.w,dst.h };
}

SDL_Texture* dae::TextureData::GetSDLTexture() const
{
	return m_texture;
}

dae::TextureData::TextureData(const std::string &fullPath)
{
	m_texture = IMG_LoadTexture(Renderer::GetInstance().GetSDLRenderer(), fullPath.c_str());
	if (m_texture == nullptr)
		throw std::runtime_error(std::string("Failed to load texture: ") + SDL_GetError());
}

dae::TextureData::TextureData(SDL_Texture* texture)	: m_texture{ texture } 
{
	assert(m_texture != nullptr);
}

#pragma endregion

#pragma region CTextureHandle

dae::CTextureHandle::CTextureHandle(const std::string& path)
{
	SetTexture(path);
}

bool dae::CTextureHandle::IsValid()
{
	return m_IsValid;
}

bool dae::CTextureHandle::SetTexture(const std::string& path, bool resetTextureTransform)
{

	m_TextureData = ResourceManager::GetInstance().GetTextureData(path);
	m_IsValid = m_TextureData->GetSDLTexture() != nullptr;

	if (m_IsValid && resetTextureTransform)
	{
		m_TransformData = TransformData{};
	}

	return m_IsValid;
}

void dae::CTextureHandle::SetTextureView(const Rect& rect)
{
	m_TextureView = rect;
	m_SetTextureView = true;
}

const dae::Rect* dae::CTextureHandle::GetTextureView() const
{
	return m_SetTextureView ? &m_TextureView : nullptr;
}

const dae::TextureData* dae::CTextureHandle::Data() const
{
	return m_TextureData.get();
}

void dae::CTextureHandle::Center()
{
	glm::vec2 size = m_TextureData.get()->GetPixelSize();
	m_TransformData.Position.x = (-size.x/2.f) * m_TransformData.Scale.x;
	m_TransformData.Position.y = (-size.y/2.f) * m_TransformData.Scale.y;
}

void dae::CTextureHandle::SetTextureTransform(TransformData& transformData)
{
	m_TransformData = transformData;
}

#pragma endregion

#pragma region Sprite Sheet

namespace dae {

	SpriteSheet::SpriteSheet(const CTextureHandle& texture, unsigned int numberOfRows, unsigned int numberOfColumns, unsigned int numberOfEmptyPixeBetweenColumns, unsigned int numberOfEmptyPixeBetweenRows) :
		m_Texture(texture),
		m_NumberOfRows(numberOfRows),
		m_NumberOfColumns(numberOfColumns),
		m_NumberOfEmptyPixelsBetweenColumns(numberOfEmptyPixeBetweenColumns),
		m_NumberOfEmptyPixelsBetweenRows(numberOfEmptyPixeBetweenRows)
	{
		m_Texture.Center();
		m_CellWidth =  int(m_Texture.Data()->GetPixelSize().x / (float)m_NumberOfColumns);
		m_CellHeight = int(m_Texture.Data()->GetPixelSize().y / (float)m_NumberOfRows	);
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
			int((rowIndex		* m_CellHeight)	+ (m_NumberOfEmptyPixelsBetweenRows		/ 2.f)		),
			int((columnIndex	* m_CellWidth)	+ (m_NumberOfEmptyPixelsBetweenColumns	/ 2.f)		),
			m_CellWidth - m_NumberOfEmptyPixelsBetweenColumns,
			m_CellHeight - m_NumberOfEmptyPixelsBetweenRows });
	}
}



#pragma endregion