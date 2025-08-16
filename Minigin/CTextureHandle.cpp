#include "CTextureHandle.h"
#include "GameObject.h"
#include "Renderer.h"
#include "ResourceManager.h"

dae::CTextureHandle::CTextureHandle(const std::string& path)
{
	SetTexture(path);
}

void dae::CTextureHandle::Render()const
{
	TransformData transformData{ OwnerConst().TransformConst().GetWorldTransform() };
	transformData += GetTextureTransform();

	Renderer::GetInstance().RenderTexture(
		*Data(),
		transformData.Position,
		transformData.Rotation,
		transformData.Scale);
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

const dae::Texture* dae::CTextureHandle::Data() const
{
	return m_TextureData.get();
}


glm::vec2 dae::CTextureHandle::GetScaledSize()const {

	glm::vec2 result = Data()->GetPixelSize();
	result.x *= m_TransformData.Scale.x;
	result.y *= m_TransformData.Scale.y;

	return result;
}

void dae::CTextureHandle::Center()
{
	glm::vec2 size = m_TextureData.get()->GetPixelSize();
	m_TransformData.Position.x = (-size.x / 2.f) * m_TransformData.Scale.x;
	m_TransformData.Position.y = (-size.y / 2.f) * m_TransformData.Scale.y;
}

void dae::CTextureHandle::SetTextureTransform(TransformData& transformData)
{
	m_TransformData = transformData;
}