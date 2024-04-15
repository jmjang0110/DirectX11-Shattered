#include "pch.h"
#include "CUIImage.h"

#include "CMeshRender.h"
#include "CTransform.h"
#include "CSerializer.h"

CUIImage::CUIImage()
	: CUIBase{COMPONENT_TYPE::UIIMAGE}
	, m_currentFrameName{"NO DATA"}
	, m_useInfo{0}
	, m_preserveRatio{false}
	, m_ratio{1.f} {}

CUIImage::~CUIImage() = default;

void CUIImage::finalupdate()
{
	if (m_preserveRatio)
	{
		if (m_useInfo && m_mapInfo.size() > 0)
		{
			m_ratio           = m_currentInfo.imageSize.x / m_currentInfo.imageSize.y;
			Vec3 currentScale = Transform()->GetRelativeScale();
			currentScale.y    = currentScale.x / m_ratio;
			Transform()->SetRelativeScale(currentScale);
		}
		else
		{
			Ptr<CTexture> pTexture = MeshRender()->GetMaterial(0)->GetTexParam(TEX_PARAM::TEX_0);
			if (nullptr != pTexture)
			{
				m_ratio           = pTexture->Width() / pTexture->Height();
				Vec3 currentScale = Transform()->GetRelativeScale();
				currentScale.y    = currentScale.x / m_ratio;
				Transform()->SetRelativeScale(currentScale);
			}
		}
		Transform()->finalupdate_module();
	}
	CUIBase::finalupdate();
	if (nullptr == MeshRender()
	    || false == MeshRender()->IsActive()
	    || nullptr == MeshRender()->GetMaterial(0)
	    || nullptr == MeshRender()->GetMesh())
	{
		return;
	}

	if (m_useInfo)
	{
		MeshRender()->GetMaterial(0)->SetScalarParam(SCALAR_PARAM::INT_1, &m_useInfo);
		MeshRender()->GetMaterial(0)->SetScalarParam(SCALAR_PARAM::VEC4_0, &m_currentInfo);
	}
}

void CUIImage::Clear()
{
	m_mapInfo.clear();
	m_useInfo = false;
}

void CUIImage::AddImageInfo(const std::wstring& frameName, const Vec2& pos, const Vec2& size)
{
	const auto iter = FindImageInfo(frameName);
	if (iter != m_mapInfo.end())
	{
		iter->second.imagePos  = pos;
		iter->second.imageSize = size;
	}
	else
	{
		if (m_mapInfo.empty())
		{
			m_currentFrameName = ToString(frameName);
			m_currentInfo      = {pos, size};
		}
		m_mapInfo.insert({frameName, tImageInfo{pos, size}});
	}
}

tImageInfo CUIImage::GetImageInfo(const std::wstring& frameName)
{
	const auto iter = FindImageInfo(frameName);
	assert(iter != m_mapInfo.end());
	return iter->second;
}

void CUIImage::RemoveInfo(const std::wstring& frameName)
{
	auto iter = FindImageInfo(frameName);
	if (iter != m_mapInfo.end())
	{
		iter = m_mapInfo.erase(iter);
	}

	if (m_mapInfo.empty())
	{
		m_currentInfo = {};
		m_currentFrameName.clear();
		m_useInfo = false;
	}
	else
	{
		if (iter != m_mapInfo.end())
		{
			m_currentFrameName = ToString(iter->first);
			m_currentInfo      = iter->second;
		}
	}
}

void CUIImage::SetImageInfo(const std::wstring& frameName)
{
	m_currentInfo      = GetImageInfo(frameName);
	m_currentFrameName = ToString(frameName);
	m_useInfo          = true;
}

void CUIImage::SetPreserveRatio(bool enable)
{
	m_preserveRatio = enable;
}

void CUIImage::Serialize(YAML::Emitter& emitter)
{
	CUIBase::Serialize(emitter);
	emitter << YAML::Key << "IMAGE INFO LIST" << YAML::Value << YAML::BeginMap;
	for (auto& [infoName, info] : m_mapInfo)
	{
		emitter << YAML::Key << ToString(infoName) << YAML::Value << YAML::BeginMap;
		emitter << YAML::Key << NAME_OF(info.imagePos) << YAML::Value << info.imagePos;
		emitter << YAML::Key << NAME_OF(info.imageSize) << YAML::Value << info.imageSize;
		emitter << YAML::EndMap;
	}
	emitter << YAML::EndMap;

	emitter << YAML::Key << NAME_OF(m_useInfo) << YAML::Value << m_useInfo;
	emitter << YAML::Key << NAME_OF(m_currentFrameName) << YAML::Value << m_currentFrameName;
	emitter << YAML::Key << NAME_OF(m_currentInfo.imagePos) << YAML::Value << m_currentInfo.imagePos;
	emitter << YAML::Key << NAME_OF(m_currentInfo.imageSize) << YAML::Value << m_currentInfo.imageSize;
	emitter << YAML::Key << NAME_OF(m_preserveRatio) << YAML::Value << m_preserveRatio;
}

void CUIImage::Deserialize(const YAML::Node& node)
{
	CUIBase::Deserialize(node);
	for (auto& infoNameNode : node["IMAGE INFO LIST"])
	{
		tImageInfo        info{};
		const std::string infoName = infoNameNode.first.as<std::string>();
		const YAML::Node& mapData  = infoNameNode.second;
		info.imagePos              = mapData[NAME_OF(info.imagePos)].as<Vec2>();
		info.imageSize             = mapData[NAME_OF(info.imageSize)].as<Vec2>();
		m_mapInfo.insert({ToWString(infoName), info});
	}
	m_useInfo               = node[NAME_OF(m_useInfo)].as<int>();
	m_currentFrameName      = node[NAME_OF(m_currentFrameName)].as<std::string>();
	m_currentInfo.imagePos  = node[NAME_OF(m_currentInfo.imagePos)].as<Vec2>();
	m_currentInfo.imageSize = node[NAME_OF(m_currentInfo.imageSize)].as<Vec2>();
	m_preserveRatio         = node[NAME_OF(m_preserveRatio)].as<bool>();
}
