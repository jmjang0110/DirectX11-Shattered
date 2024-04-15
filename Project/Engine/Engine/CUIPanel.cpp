#include "pch.h"
#include "CUIPanel.h"

#include "CMeshRender.h"
#include "CTransform.h"
#include "CSerializer.h"

CUIPanel::CUIPanel()
	: CUIBase{COMPONENT_TYPE::UIPANEL}
	, m_isSorted{false}
	, m_useInfo{0}
	, m_preserveRatio{false}
	, m_ratio{1.f} {}

CUIPanel::~CUIPanel() = default;

void CUIPanel::finalupdate()
{
	if (m_vecUIChild.size() != GetOwner()->GetChild().size())
	{
		m_isSorted = false;
	}
	//Panel에 연결된 다른 UI에서 신호를 보내는 경우도 있으므로 분리해서 처리
	if (false == m_isSorted)
	{
		SortGameObject();
	}

	if (m_preserveRatio)
	{
		if (m_useInfo)
		{
			m_ratio           = m_info.imageSize.x / m_info.imageSize.y;
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
		MeshRender()->GetMaterial(0)->SetScalarParam(SCALAR_PARAM::VEC4_0, &m_info);
	}
}

void CUIPanel::SortGameObject()
{
	auto func = [](CGameObject* pLeft, CGameObject* pRight)
	{
		const CUIBase* pLeftUI = pLeft->GetUIBaseComponenent();
		assert(pLeftUI);
		const CUIBase* pRightUI = pRight->GetUIBaseComponenent();
		assert(pRightUI);

		return pLeftUI->GetOrderZ() > pRightUI->GetOrderZ();
	};
	GetOwner()->SortChild(func);

	m_vecUIChild.clear();
	for (auto& pUIObject : GetOwner()->GetChild())
	{
		assert(pUIObject->GetUIBaseComponenent());
		if (pUIObject->IsDead())
		{
			continue;
		}
		m_vecUIChild.push_back(pUIObject->GetUIBaseComponenent());
	}
	std::reverse(m_vecUIChild.begin(), m_vecUIChild.end());
	m_isSorted = true;
}

bool CUIPanel::CollisionMouse()
{
	if (false == m_mouseCollisionEnable)
	{
		return false;
	}
	//패널이 아닌 자식에서 점검하는 경우
	for (const auto& pUIWidget : m_vecUIChild)
	{
		if (pUIWidget->CollisionMouse())
		{
			return true;
		}
	}
	//패널 자체가 이벤트가 발생하는 경우
	if (CUIBase::CollisionMouse())
	{
		return true;
	}
	return false;
}

void CUIPanel::SetSorted(bool enable)
{
	m_isSorted = enable;
	m_vecUIChild.clear();
}

void CUIPanel::Serialize(YAML::Emitter& emitter)
{
	CUIBase::Serialize(emitter);
	emitter << YAML::Key << NAME_OF(m_useInfo) << YAML::Value << m_useInfo;
	emitter << YAML::Key << NAME_OF(m_info.imagePos) << YAML::Value << m_info.imagePos;
	emitter << YAML::Key << NAME_OF(m_info.imageSize) << YAML::Value << m_info.imageSize;
	emitter << YAML::Key << NAME_OF(m_preserveRatio) << YAML::Value << m_preserveRatio;
}

void CUIPanel::Deserialize(const YAML::Node& node)
{
	CUIBase::Deserialize(node);

	if (node[NAME_OF(m_useInfo)].IsDefined())
		m_useInfo = node[NAME_OF(m_useInfo)].as<int>();
	if (node[NAME_OF(m_info.imagePos)].IsDefined())
		m_info.imagePos = node[NAME_OF(m_info.imagePos)].as<Vec2>();
	if (node[NAME_OF(m_info.imageSize)].IsDefined())
		m_info.imageSize = node[NAME_OF(m_info.imageSize)].as<Vec2>();
	if (node[NAME_OF(m_preserveRatio)].IsDefined())
		m_preserveRatio = node[NAME_OF(m_preserveRatio)].as<bool>();
}
