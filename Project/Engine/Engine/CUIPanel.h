#pragma once
#include "CUIBase.h"

class CUIPanel final
	: public CUIBase
{
private:
	std::vector<CUIBase*> m_vecUIChild;
	bool                  m_isSorted;
	tImageInfo            m_info;
	int                   m_useInfo;
	bool                  m_preserveRatio;
	float                 m_ratio;

private:
	void SortGameObject();

public:
	bool CollisionMouse() override;
	void SetSorted(bool enable);

	void       SetImageInfo(const tImageInfo& info) { m_info = info; }
	tImageInfo GetInfo() const { return m_info; }

	void SetUseInfo(bool enable) { m_useInfo = enable; }
	bool IsUseInfo() const { return m_useInfo; }

	void SetPreserveRatio(bool enable) { m_preserveRatio = enable; }
	bool IsPreserveRatio() const { return m_preserveRatio; }

public:
	void finalupdate() override;

	void Serialize(YAML::Emitter& emitter) override;
	void Deserialize(const YAML::Node& node) override;

public:
	CLONE(CUIPanel);
	CUIPanel();
	virtual ~CUIPanel();
};
