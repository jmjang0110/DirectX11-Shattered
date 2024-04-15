#include "pch.h"
#include "CUIButton.h"
#include "CTimeMgr.h"
#include "CUIImage.h"
#include "CKeyMgr.h"
#include "CSerializer.h"

namespace BUTTON
{
	inline constexpr float DEFAULT_TERM = 0.3f;
}

CUIButton::CUIButton()
	: CComponent{COMPONENT_TYPE::UIBUTTON}
	, m_clickTerm{BUTTON::DEFAULT_TERM}
	, m_elapsedTime{0.f} {}

CUIButton::CUIButton(const CUIButton& _origin)
	: CComponent{_origin}
	, m_clickTerm{_origin.m_clickTerm}
	, m_elapsedTime{0.f} {}

CUIButton::~CUIButton() = default;

void CUIButton::finalupdate()
{
	m_elapsedTime += DT;

	if (GetOwner()->GetUIBaseComponenent() && GetOwner()->GetUIBaseComponenent()->IsMouseHovered())
	{
		HandleClickEvent();
		HandleDragEvent();
		HandleReleaseEvent();
	}

	if (m_clickTerm <= m_elapsedTime)
	{
		m_elapsedTime -= m_clickTerm;
		m_prevClick.reset();
	}
}

bool CUIButton::DoubleClick(MOUSE_TYPE mouseType, CLICK_TYPE clickType)
{
	if (m_prevClick[static_cast<int>(mouseType)]
	    && m_arrCallback[static_cast<int>(mouseType)][static_cast<int>(clickType)])
	{
		m_arrCallback[static_cast<int>(mouseType)][static_cast<int>(clickType)](GetOwner());
		return true;
	}
	return false;
}

void CUIButton::OneClick(MOUSE_TYPE mouseType, CLICK_TYPE clickType) const
{
	if (m_arrCallback[static_cast<int>(mouseType)][static_cast<int>(clickType)])
	{
		m_arrCallback[static_cast<int>(mouseType)][static_cast<int>(clickType)](GetOwner());
	}
}

void CUIButton::HandleClickEvent()
{
	if (KEY_TAP(KEY::LBTN))
	{
		if (DoubleClick(MOUSE_TYPE::LEFT, CLICK_TYPE::DOUBLE))
		{
			return;
		}
		OneClick(MOUSE_TYPE::LEFT, CLICK_TYPE::ONE);

		m_prevClick.reset();
		m_prevClick[static_cast<int>(MOUSE_TYPE::LEFT)] = true;
		m_clickTerm                                     = BUTTON::DEFAULT_TERM;
	}
	else if (KEY_TAP(KEY::MBTN))
	{
		if (DoubleClick(MOUSE_TYPE::MIDDLE, CLICK_TYPE::DOUBLE))
		{
			return;
		}
		OneClick(MOUSE_TYPE::MIDDLE, CLICK_TYPE::ONE);

		m_prevClick.reset();
		m_prevClick[static_cast<int>(MOUSE_TYPE::MIDDLE)] = true;
		m_clickTerm                                       = BUTTON::DEFAULT_TERM;
	}
	else if (KEY_TAP(KEY::RBTN))
	{
		if (DoubleClick(MOUSE_TYPE::RIGHT, CLICK_TYPE::DOUBLE))
		{
			return;
		}
		OneClick(MOUSE_TYPE::RIGHT, CLICK_TYPE::ONE);

		m_prevClick.reset();
		m_prevClick[static_cast<int>(MOUSE_TYPE::RIGHT)] = true;
		m_clickTerm                                      = BUTTON::DEFAULT_TERM;
	}
}

void CUIButton::HandleDragEvent()
{
	if (KEY_PRESSED(KEY::LBTN))
	{
		if (m_arrCallback[static_cast<int>(MOUSE_TYPE::LEFT)][static_cast<int>(CLICK_TYPE::DRAG)])
		{
			m_arrCallback[static_cast<int>(MOUSE_TYPE::LEFT)][static_cast<int>(CLICK_TYPE::DRAG)](GetOwner());
		}
	}
	else if (KEY_PRESSED(KEY::MBTN))
	{
		if (m_arrCallback[static_cast<int>(MOUSE_TYPE::MIDDLE)][static_cast<int>(CLICK_TYPE::DRAG)])
		{
			m_arrCallback[static_cast<int>(MOUSE_TYPE::MIDDLE)][static_cast<int>(CLICK_TYPE::DRAG)](GetOwner());
		}
	}
	else if (KEY_PRESSED(KEY::RBTN))
	{
		if (m_arrCallback[static_cast<int>(MOUSE_TYPE::RIGHT)][static_cast<int>(CLICK_TYPE::DRAG)])
		{
			m_arrCallback[static_cast<int>(MOUSE_TYPE::RIGHT)][static_cast<int>(CLICK_TYPE::DRAG)](GetOwner());
		}
	}
}

void CUIButton::HandleReleaseEvent()
{
	if (KEY_AWAY(KEY::LBTN))
	{
		if (m_arrCallback[static_cast<int>(MOUSE_TYPE::LEFT)][static_cast<int>(CLICK_TYPE::RELEASE)])
		{
			m_arrCallback[static_cast<int>(MOUSE_TYPE::LEFT)][static_cast<int>(CLICK_TYPE::RELEASE)](GetOwner());
		}
	}
	else if (KEY_AWAY(KEY::MBTN))
	{
		if (m_arrCallback[static_cast<int>(MOUSE_TYPE::MIDDLE)][static_cast<int>(CLICK_TYPE::RELEASE)])
		{
			m_arrCallback[static_cast<int>(MOUSE_TYPE::MIDDLE)][static_cast<int>(CLICK_TYPE::RELEASE)](GetOwner());
		}
	}
	else if (KEY_AWAY(KEY::RBTN))
	{
		if (m_arrCallback[static_cast<int>(MOUSE_TYPE::RIGHT)][static_cast<int>(CLICK_TYPE::RELEASE)])
		{
			m_arrCallback[static_cast<int>(MOUSE_TYPE::RIGHT)][static_cast<int>(CLICK_TYPE::RELEASE)](GetOwner());
		}
	}
}

void CUIButton::Serialize(YAML::Emitter& emitter)
{
	emitter << YAML::Key << NAME_OF(m_clickTerm) << YAML::Value << m_clickTerm;
}

void CUIButton::Deserialize(const YAML::Node& node)
{
	m_clickTerm = node[NAME_OF(m_clickTerm)].as<float>();
}
