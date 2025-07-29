#include "Button.h"
#include "PlayerController.h"
#include "EventTriggerCommand.h"

dae::CButton::CButton(const ButtonData& buttonData) : m_ButtonData(buttonData)
{
}

void dae::CButton::Start()
{
	CText currentText{ m_ButtonData.FontData };
	currentText.SetText(m_ButtonData.Name);
	currentText.SetColor(m_ButtonData.UnselectedColor);
	currentText.Center();

	m_CText = Owner().AddComponent(currentText);
	if (m_IsSelected)
	{
		Select();
	}
	else
	{
		Unselect();
	}
}


void dae::CButton::Select()
{
	m_IsSelected = true;
	if (!m_CText)
	{
		return;
	}
	m_CText->SetColor(m_ButtonData.SelectedColor);
}

void dae::CButton::Unselect()
{
	m_IsSelected = false;
	if (!m_CText)
	{
		return;
	}
	m_CText->SetColor(m_ButtonData.UnselectedColor);
}

void dae::CButton::Press()
{
	m_ButtonData.OnPress.Invoke();
}

void dae::CButton::SubscribeOnPress(const std::function<void()>& func)
{
	m_ButtonData.OnPress.Subscribe(func);
}



