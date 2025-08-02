#include "CNameDisplayer.h"
#include "GameObject.h"

void dae::CNameDisplayer::Start()
{
	if (!m_Text)
	{
		m_Text = Owner().AddComponent<CText>(m_TextData);

	}

	std::string text = m_TextData.Text + Owner().GetName();
	m_Text->SetText(text);
	m_Text->Center();
	TransformData textTransform = m_TextTransform + m_Text->GetTextureTransform();
	m_Text->SetTextTransform(textTransform);
}
