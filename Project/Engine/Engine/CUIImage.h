#pragma once
#include "CUIBase.h"

class CUIImage final
	: public CUIBase
{
private:
	std::map<std::wstring, tImageInfo> m_mapInfo;
	tImageInfo                         m_currentInfo;
	std::string                        m_currentFrameName;
	int                                m_useInfo;
	bool                               m_preserveRatio;
	float                              m_ratio;

public:
	void finalupdate() override;

public:
	void Clear();

	const std::map<std::wstring, tImageInfo>& GetInfoList() const { return m_mapInfo; }
	void AddImageInfo(const std::wstring& frameName, const Vec2& pos, const Vec2& size);
	tImageInfo GetImageInfo(const std::wstring& frameName);
	auto FindImageInfo(const std::wstring& frameName) { return m_mapInfo.find(frameName); };

	void SetImageInfo(const std::wstring& frameName);
	void RemoveInfo(const std::wstring& frameName);

	const std::string& CurrentInfoName() const { return m_currentFrameName; }
	bool               HasCurrentInfo() const { return false == m_mapInfo.empty(); }

	bool IsUseInfo() const { return m_useInfo; }

	void SetPreserveRatio(bool enable);
	bool IsPreserveRatio() const { return m_preserveRatio; }

public:
	void Serialize(YAML::Emitter& emitter) override;
	void Deserialize(const YAML::Node& node) override;

public:
	CLONE(CUIImage);
	CUIImage();
	virtual ~CUIImage();
};
