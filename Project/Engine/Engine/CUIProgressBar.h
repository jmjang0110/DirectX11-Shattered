#pragma once
#include "CComponent.h"

class CUIImage;

enum class PROGRESS_DIR
{
	RIGHT_TO_LEFT,
	LEFT_TO_RIGHT,
	TOP_TO_BOTTOM,
	BOTTOM_TO_TOP,

	END
};

class CUIProgressBar final
	: public CComponent
{
private:
	float        m_percentage;
	PROGRESS_DIR m_dir;

public:
	float GetPercent() const { return m_percentage; }
	void  SetPercent(float value);

	void         SetDir(PROGRESS_DIR dir) { m_dir = dir; }
	PROGRESS_DIR GetDir() const { return m_dir; }

public:
	void finalupdate() override;

public:
	void Serialize(YAML::Emitter& emitter) override;
	void Deserialize(const YAML::Node& node) override;

public:
	CLONE(CUIProgressBar);
	CUIProgressBar();
	CUIProgressBar(const CUIProgressBar& _origin);
	virtual ~CUIProgressBar();
};
