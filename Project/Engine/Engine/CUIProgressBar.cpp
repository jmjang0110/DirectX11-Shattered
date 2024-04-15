#include "pch.h"
#include "CUIProgressBar.h"

#include "CMeshRender.h"
#include "CSerializer.h"
#include "CUIBase.h"
#include "CUIImage.h"

CUIProgressBar::CUIProgressBar()
	: CComponent{COMPONENT_TYPE::UIPROGRESSBAR}
	, m_percentage{1.f}
	, m_dir{PROGRESS_DIR::RIGHT_TO_LEFT} {}

CUIProgressBar::CUIProgressBar(const CUIProgressBar& _origin)
	: CComponent{_origin}
	, m_percentage{_origin.m_percentage}
	, m_dir{_origin.m_dir} {}

CUIProgressBar::~CUIProgressBar() = default;

void CUIProgressBar::SetPercent(float value)
{
	m_percentage = ClampData(value, 0.f, 1.f);
}


void CUIProgressBar::finalupdate()
{
	if (nullptr == MeshRender()
	    || false == MeshRender()->IsActive()
	    || nullptr == MeshRender()->GetMaterial(0)
	    || nullptr == MeshRender()->GetMesh())
	{
		return;
	}

	const CUIBase* pUIBase = GetOwner()->GetUIBaseComponenent();
	if (nullptr != pUIBase)
	{
		int usePercentage = true;
		MeshRender()->GetMaterial(0)->SetScalarParam(SCALAR_PARAM::INT_2, &usePercentage);
		MeshRender()->GetMaterial(0)->SetScalarParam(SCALAR_PARAM::INT_3, &m_dir);
		MeshRender()->GetMaterial(0)->SetScalarParam(SCALAR_PARAM::FLOAT_0, &m_percentage);
	}
}

void CUIProgressBar::Serialize(YAML::Emitter& emitter)
{
	emitter << YAML::Key << NAME_OF(m_dir) << YAML::Value << static_cast<int>(m_dir);
}

void CUIProgressBar::Deserialize(const YAML::Node& node)
{
	m_dir = (PROGRESS_DIR)node[NAME_OF(m_dir)].as<int>();
}
