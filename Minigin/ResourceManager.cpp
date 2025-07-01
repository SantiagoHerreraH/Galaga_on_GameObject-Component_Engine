#include <stdexcept>
#include <SDL_image.h>
#include <SDL_ttf.h>
#include "ResourceManager.h"
#include "Renderer.h"
#include "Texture.h"
#include "Font.h"

namespace fs = std::filesystem;

void dae::ResourceManager::Init(const std::filesystem::path& dataPath)
{
	m_dataPath = dataPath;

	if (TTF_Init() != 0)
	{
		throw std::runtime_error(std::string("Failed to load support for fonts: ") + SDL_GetError());
	}
}

std::shared_ptr<dae::TextureData> dae::ResourceManager::GetTextureData(const std::string& file)
{
	const auto fullPath = m_dataPath/file;
	const auto filename = fs::path(fullPath).filename().string();
	if(m_TextureName_To_Texture.find(filename) == m_TextureName_To_Texture.end())
		m_TextureName_To_Texture.insert(std::pair(filename,std::make_shared<TextureData>(fullPath.string())));
	return m_TextureName_To_Texture.at(filename);
}

std::shared_ptr<dae::FontData> dae::ResourceManager::GetFontData(const std::string& file, uint8_t size)
{
	const auto fullPath = m_dataPath/file;
	const auto filename = fs::path(fullPath).filename().string();
	const auto key = std::pair<std::string, uint8_t>(filename, size);
	if(m_FontData_To_Font.find(key) == m_FontData_To_Font.end())
		m_FontData_To_Font.insert(std::pair(key,std::make_shared<FontData>(fullPath.string(), size)));
	return m_FontData_To_Font.at(key);
}

void dae::ResourceManager::UnloadUnusedResources()
{
	for (auto it = m_TextureName_To_Texture.begin(); it != m_TextureName_To_Texture.end();)
	{
		if (it->second.use_count() == 1)
			it = m_TextureName_To_Texture.erase(it);
		else
			++it;
	}

	for (auto it = m_FontData_To_Font.begin(); it != m_FontData_To_Font.end();)
	{
		if (it->second.use_count() == 1)
			it = m_FontData_To_Font.erase(it);
		else
			++it;
	}
}
