#include "TextCreator.h"
#include "Text.h"

dae::TextCreator::TextCreator(const std::string& text, const glm::vec2& pos, uint8_t fontSize, const SDL_Color& color)
{
	TransformData currentTransformData{};
	currentTransformData.Position.x = pos.x;
	currentTransformData.Position.y = pos.y;

	CText currentText{ "Emulogic-zrEw.ttf", fontSize };
	currentText.SetText(text);
	currentText.SetColor(color);
	currentText.Center();

	m_TextGameObject = std::make_shared<GameObject>();
	m_TextGameObject->Transform().SetLocalTransform(currentTransformData);
	m_TextGameObject->AddComponent(currentText);
}

void dae::TextCreator::SetText(const std::string& text) 
{
	m_TextGameObject->GetComponent<CText>()->SetText(text);
}
