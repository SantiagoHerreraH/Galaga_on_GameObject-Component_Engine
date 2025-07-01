#pragma once
#include <string>
#include "Font.h"
#include "Texture.h"
#include "SDL_pixels.h"

namespace dae {

	struct CText final{
	public:
		CText() {}
		~CText();
		CText(const std::string& fontFullPath, uint8_t fontSize);
		void SetText(std::string text); 
		void AddText(const std::string& text,bool center = true);
		void SetColor(const SDL_Color& color);
		bool IsFontValid();
		bool SetFont(const std::string& fullPath, uint8_t size);
		const TextureData* Data()const;
		void SetTextTransform(const TransformData& transform) { m_TransformData = transform; }
		const TransformData& GetTextureTransform() const { return m_TransformData; }
		void Center();

	private:
		std::string m_Text;
		SDL_Color m_Color{255 ,255 , 255 , 255 };
		std::string m_FontPath;
		std::shared_ptr<FontData> m_FontData;
		mutable std::shared_ptr<TextureData> m_TextTexture{};// needs to be muttable for dirty pattern on Data() call
		TransformData m_TransformData;

		bool m_IsFontValid{ false };
		mutable bool m_TextureNeedsUpdate{ true }; // needs to be muttable for dirty pattern on Data() call
	};
}