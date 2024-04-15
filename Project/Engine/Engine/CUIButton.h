#pragma once
#include "CComponent.h"
#include <bitset>

class CUIBase;

enum class CLICK_TYPE
{
	ONE,
	DOUBLE,
	RELEASE,
	DRAG,

	END
};

enum class MOUSE_TYPE
{
	LEFT,
	MIDDLE,
	RIGHT,

	END
};

class CUIButton final
	: public CComponent
{
private:
	float m_clickTerm;
	float m_elapsedTime;

	std::bitset<(int)MOUSE_TYPE::END> m_prevClick;

	using ClickFunc = std::array<std::function<void(CGameObject*)>, (int)CLICK_TYPE::END>;
	std::array<ClickFunc, (int)MOUSE_TYPE::END> m_arrCallback;

private:
	bool DoubleClick(MOUSE_TYPE mouseType, CLICK_TYPE clickType);
	void OneClick(MOUSE_TYPE mouseType, CLICK_TYPE clickType) const;
	void HandleClickEvent();

	void HandleDragEvent();
	void HandleReleaseEvent();

public:
	void finalupdate() override;

public:
	template <typename T>
	void SetCallback(T* pInstance, void (T::*func)(CGameObject*), MOUSE_TYPE mouseType, CLICK_TYPE clickType)
	{
		m_arrCallback[static_cast<int>(mouseType)][static_cast<int>(clickType)]
			= std::bind(func, pInstance, std::placeholders::_1);
	}

public:
	void Serialize(YAML::Emitter& emitter) override;
	void Deserialize(const YAML::Node& node) override;

public:
	CLONE(CUIButton);
	CUIButton();
	CUIButton(const CUIButton& _origin);
	virtual ~CUIButton();
};
