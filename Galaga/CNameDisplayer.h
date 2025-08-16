#pragma once

#include "CText.h"

namespace dae {

	class CNameDisplayer : public Component
	{
	public:
		CNameDisplayer(const TextData& textData, const TransformData& transformData) :
			m_TextData(textData),
			m_TextTransform(transformData){}
		void Start()override;
	private:
		std::shared_ptr<CText> m_Text{};
		TextData m_TextData{};
		TransformData m_TextTransform{};
	};
}

