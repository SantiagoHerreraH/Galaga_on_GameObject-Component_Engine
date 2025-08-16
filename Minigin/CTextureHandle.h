#pragma once

#include "Component.h"
#include "Texture.h"

namespace dae {

	class CTextureHandle final : public Component
	{

	public:
		CTextureHandle() {}
		CTextureHandle(const std::string& path);

		virtual void Render()const override;

		bool IsValid();
		bool SetTexture(const std::string& path, bool resetTextureTransform = true);
		void SetTextureView(const Rect& rect);
		const Rect* GetTextureView()const;
		const Texture* Data()const;
		glm::vec2 GetScaledSize()const;
		void Center();
		void SetTextureTransform(TransformData& transformData);
		const TransformData& GetTextureTransform()const { return m_TransformData; }

	private:
		bool m_SetTextureView{ false };
		Rect m_TextureView{};
		TransformData m_TransformData;
		std::string m_TexturePath;
		std::shared_ptr<Texture> m_TextureData;
		bool m_IsValid{ false };

	};
}