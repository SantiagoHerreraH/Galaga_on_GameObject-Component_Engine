#include "Text.h"
#include "ResourceManager.h"
#include <SDL_ttf.h>
#include <iostream>
#include "Renderer.h"

dae::CText::~CText()
{
}

dae::CText::CText(const std::string& fullPath, uint8_t size)
{
	SetFont(fullPath, size);
}

void dae::CText::SetText(std::string text)
{
	if (m_Text != text)
	{
		m_TextureNeedsUpdate = true;
		m_Text = std::move(text);
	}
}

void dae::CText::AddText(const std::string& text, bool center)
{
	if (m_Text != text)
	{
		m_TextureNeedsUpdate = true;
		m_Text += text;

		if (center)
		{
			Center();
		}
	}
}

void dae::CText::SetColor(const SDL_Color& color)
{
	m_TextureNeedsUpdate = true;
	m_Color = color;
}

bool dae::CText::IsFontValid()
{
	return m_IsFontValid;
}

bool dae::CText::SetFont(const std::string& fullPath, uint8_t size)
{
	m_FontData = ResourceManager::GetInstance().GetFontData(fullPath, size);
	m_IsFontValid = m_FontData->GetData() != nullptr;
	m_TextureNeedsUpdate = true;
	return m_IsFontValid;
}

const dae::TextureData* dae::CText::Data() const
{
	if (m_TextureNeedsUpdate)
	{
		const auto surf = TTF_RenderText_Blended(m_FontData->GetData(), m_Text.c_str(), m_Color);
		if (surf == nullptr)
		{
			std::cerr << "Render text failed: " << SDL_GetError();
			assert(surf);
		}
		auto texture = SDL_CreateTextureFromSurface(Renderer::GetInstance().GetSDLRenderer(), surf);
		if (texture == nullptr)
		{
			std::cerr << "Create text texture from surface failed: " << SDL_GetError();
			assert(texture);
		}
		SDL_FreeSurface(surf);
		m_TextTexture = std::make_shared<TextureData>(texture);
		m_TextureNeedsUpdate = false;
	}
	return m_TextTexture.get();
}

void dae::CText::Center()
{
	glm::vec2 size = Data()->GetPixelSize();
	m_TransformData.Position.x = (-size.x / 2.f) * m_TransformData.Scale.x;
	m_TransformData.Position.y = (-size.y / 2.f) * m_TransformData.Scale.y;
}
