#include "pch.h"
#include "CUIBase.h"

#include "CDevice.h"
#include "CAnimator2D.h"
#include "CCamera.h"
#include "CKeyMgr.h"
#include "CMeshRender.h"
#include "CRenderMgr.h"
#include "CTransform.h"
#include "CUIPanel.h"
#include "CSerializer.h"

CUIBase::CUIBase(COMPONENT_TYPE type)
	: CComponent{type}
	, m_orderZ{0}
	, m_opacity{1.f}
	, m_isMouseHovered{false}
	, m_prevMouseHovered{false}
	, m_mouseCollisionEnable{true}
	, m_anchorV{ANCHOR_VERTICAL::MIDDLE}
	, m_anchorH{ANCHOR_HORIZONTAL::MIDDLE}
	, m_showDebugRect{true} {}

CUIBase::~CUIBase() = default;


void CUIBase::finalupdate()
{
	if (GetOwner()->IsDead())
	{
		CGameObject* pParent = GetOwner()->GetParent();
		if (pParent && pParent->UIPanel())
		{
			pParent->UIPanel()->SetSorted(false);
		}
		return;
	}

	m_opacity = ClampData(m_opacity, 0.f, 1.f);

	if (nullptr == m_pTarget)
	{
		Vec3 adjustPos = Transform()->GetRelativePos();
		adjustPos.x    = AdjustPositionWithAnchor(m_anchorH);
		adjustPos.y    = AdjustPositionWithAnchor(m_anchorV);
		Transform()->SetRelativePos(adjustPos);
		Transform()->finalupdate_module();
	}
	else
	{
		UpdatePositionByObject();
	}

	if (nullptr == GetOwner()->GetParent())
	{
		CollisionMouse();
	}

	if (nullptr == MeshRender()
	    || false == MeshRender()->IsActive()
	    || nullptr == MeshRender()->GetMaterial(0)
	    || nullptr == MeshRender()->GetMesh())
	{
		return;
	}

	MeshRender()->GetMaterial(0)->SetScalarParam(SCALAR_PARAM::INT_0, &m_showDebugRect);

	Ptr<CTexture> pTexture = MeshRender()->GetMaterial(0)->GetTexParam(TEX_PARAM::TEX_0);
	if (nullptr != pTexture)
	{
		Vec2 textureSize = {pTexture->Width(), pTexture->Height()};
		MeshRender()->GetMaterial(0)->SetScalarParam(SCALAR_PARAM::VEC2_0, &textureSize);
	}
}

bool CUIBase::CollisionMouse()
{
	if (false == m_mouseCollisionEnable || false == IsActive())
	{
		return false;
	}

	Vec2       mousePos   = CKeyMgr::GetInst()->GetMousePos();
	const Vec2 resolution = CDevice::GetInst()->GetRenderResolution();
	//마우스 자체가 화면 공간 밖에 있어도 안된다.
	const bool isMouseOutWindow = (0 <= mousePos.x && mousePos.x <= resolution.x
	                               && 0 <= mousePos.y && mousePos.y <= resolution.y);
	if (false == isMouseOutWindow)
	{
		return false;
	}
	//좌상단이 0,0이므로 DirectX와 좌표계를 맞추기 위한 보정.
	const Vec2 half = resolution * 0.5f;
	mousePos.x -= half.x;
	mousePos.y = (mousePos.y >= half.y) ? -mousePos.y + half.y : half.y - mousePos.y;

	const Vec3 uiPos      = Transform()->GetWorldPos();
	const Vec3 halfUISize = Transform()->GetWorldScale() * 0.5f;

	const Vec2 leftTop     = {uiPos.x - halfUISize.x, uiPos.y + halfUISize.y};
	const Vec2 rightBottom = {uiPos.x + halfUISize.x, uiPos.y - halfUISize.y};

	m_isMouseHovered = (leftTop.x < mousePos.x && mousePos.x < rightBottom.x
	                    && rightBottom.y < mousePos.y && mousePos.y < leftTop.y);

	bool isHandled = false;
	if (m_isMouseHovered && m_prevMouseHovered != m_isMouseHovered && m_hoverCallback)
	{
		m_hoverCallback(GetOwner());
		isHandled = true;
	}
	if (m_prevMouseHovered != m_isMouseHovered && m_isMouseHovered == false && m_leaveCallback)
	{
		m_leaveCallback(GetOwner());
		isHandled = true;
	}
	m_prevMouseHovered = m_isMouseHovered;

	return isHandled;
}

void CUIBase::SetOrderZ(int iOrderZ)
{
	m_orderZ = iOrderZ;

	CGameObject* pParent = GetOwner()->GetParent();
	if (pParent && pParent->UIPanel())
	{
		pParent->UIPanel()->SetSorted(false);
	}
}

void CUIBase::SetShowDebugRect(bool enable)
{
	m_showDebugRect = enable;
}

float CUIBase::AdjustPositionWithAnchor(ANCHOR_HORIZONTAL type)
{
	Transform()->SetIgnoreParentScale(true);

	CGameObject* pParent = GetOwner()->GetParent();
	if (nullptr == pParent || (pParent && nullptr == pParent->UIPanel()))
	{
		return AdjustWithCamera(type);
	}
	return AdjustWithPanel(type);
}

float CUIBase::AdjustPositionWithAnchor(ANCHOR_VERTICAL type)
{
	Transform()->SetIgnoreParentScale(true);

	CGameObject* pParent = GetOwner()->GetParent();
	if (nullptr == pParent || (pParent && nullptr == pParent->UIPanel()))
	{
		return AdjustWithCamera(type);
	}
	return AdjustWithPanel(type);
}

float CUIBase::AdjustWithCamera(ANCHOR_HORIZONTAL type) const
{
	CCamera* pCamera = CRenderMgr::GetInst()->GetUICamera();
	if (nullptr == pCamera)
	{
		return m_offsetPos.x;
	}
	const float halfWidth         = pCamera->GetWidth() * 0.5f;
	const Vec3  cameraPos         = pCamera->Transform()->GetWorldPos();
	const Vec3  widgetUIHalfScale = GetOwner()->Transform()->GetRelativeScale() * 0.5f;

	switch (type)
	{
	case ANCHOR_HORIZONTAL::LEFT:
		return cameraPos.x - halfWidth + m_offsetPos.x + widgetUIHalfScale.x;
	case ANCHOR_HORIZONTAL::MIDDLE:
		return cameraPos.x + m_offsetPos.x;
	case ANCHOR_HORIZONTAL::RIGHT:
		return cameraPos.x + halfWidth + m_offsetPos.x - widgetUIHalfScale.x;
	}
	assert(nullptr && "INVALID POS");
	return m_offsetPos.x;
}

float CUIBase::AdjustWithCamera(ANCHOR_VERTICAL type) const
{
	CCamera* pCamera = CRenderMgr::GetInst()->GetUICamera();
	if (nullptr == pCamera)
	{
		return m_offsetPos.y;
	}
	const float halfHeight        = (pCamera->GetWidth() / pCamera->GetAspectRatio()) * 0.5f;
	const Vec3  cameraPos         = pCamera->Transform()->GetWorldPos();
	const Vec3  widgetUIHalfScale = GetOwner()->Transform()->GetRelativeScale() * 0.5f;

	switch (type)
	{
	case ANCHOR_VERTICAL::TOP:
		return cameraPos.y + halfHeight + m_offsetPos.y - widgetUIHalfScale.y;
	case ANCHOR_VERTICAL::MIDDLE:
		return cameraPos.y + m_offsetPos.y;
	case ANCHOR_VERTICAL::BOTTOM:
		return cameraPos.y - halfHeight + m_offsetPos.y + widgetUIHalfScale.y;
	}

	assert(nullptr && "INVALID POS");

	return m_offsetPos.y;
}

float CUIBase::AdjustWithPanel(ANCHOR_HORIZONTAL type) const
{
	//어차피 부모 윈도우 기준에서 자식 윈도우의 위치가 정해지므로(Transform에 의하여)
	//거기서 얼마나 떨어져야 하는지를 지정해주기만 하면 된다.
	const Vec3  panelScale        = GetOwner()->GetParent()->Transform()->GetRelativeScale();
	const float panelHalfWidth    = panelScale.x * 0.5f;
	const Vec3  widgetUIHalfScale = GetOwner()->Transform()->GetRelativeScale() * 0.5f;
	switch (type)
	{
	case ANCHOR_HORIZONTAL::LEFT:
		return (-panelHalfWidth) + m_offsetPos.x + widgetUIHalfScale.x;
	case ANCHOR_HORIZONTAL::MIDDLE:
		return m_offsetPos.x;
	case ANCHOR_HORIZONTAL::RIGHT:
		return panelHalfWidth + m_offsetPos.x - widgetUIHalfScale.x;
	}
	assert(nullptr && "INVALID POS");
	return m_offsetPos.x;
}

float CUIBase::AdjustWithPanel(ANCHOR_VERTICAL type) const
{
	const Vec3  panelScale        = GetOwner()->GetParent()->Transform()->GetRelativeScale();
	const float halfHeight        = panelScale.y * 0.5f;
	const Vec3  widgetUIHalfScale = GetOwner()->Transform()->GetRelativeScale() * 0.5f;

	switch (type)
	{
	case ANCHOR_VERTICAL::TOP:
		return halfHeight + m_offsetPos.y - widgetUIHalfScale.y;
	case ANCHOR_VERTICAL::MIDDLE:
		return m_offsetPos.y;
	case ANCHOR_VERTICAL::BOTTOM:
		return (-halfHeight) + m_offsetPos.y + widgetUIHalfScale.y;
	}
	assert(nullptr && "INVALID POS");
	return m_offsetPos.y;
}

void CUIBase::UpdatePositionByObject()
{
	const CCamera* pPlayCamera = CRenderMgr::GetInst()->GetMainCam();
	const CCamera* pUICamera   = CRenderMgr::GetInst()->GetUICamera();

	const Vec3 targetPos           = m_pTarget->Transform()->GetWorldPos();
	const Vec3 viewPosInPlayCamera = XMVector3TransformCoord(targetPos, pPlayCamera->GetViewMat());
	const Vec4 projPosInPlayCamera = XMVector3TransformCoord(viewPosInPlayCamera, pPlayCamera->GetProjMat());

	const Vec4 viewPosInUICamera = XMVector3TransformCoord(projPosInPlayCamera, pUICamera->GetProjInvMat());

	const Vec3 diff = {viewPosInUICamera.x, viewPosInUICamera.y, 0.f};

	Vec3 UIPos = Transform()->GetRelativePos();
	UIPos.x    = diff.x;
	UIPos.y    = diff.y;

	//반대 방향으로 바라보는 경우에도 출력되기 때문에, (x,y만 보정하므로) 이를 계산하지 않기 위함
	if (viewPosInUICamera.z > pUICamera->GetFar())
	{
		return;
	}
	Transform()->SetRelativePos(UIPos);
	Transform()->finalupdate();
}


void CUIBase::Serialize(YAML::Emitter& emitter)
{
	emitter << YAML::Key << NAME_OF(m_orderZ) << YAML::Value << m_orderZ;
	emitter << YAML::Key << NAME_OF(m_opacity) << YAML::Value << m_opacity;
	emitter << YAML::Key << NAME_OF(m_mouseCollisionEnable) << YAML::Value << m_mouseCollisionEnable;
	emitter << YAML::Key << NAME_OF(m_anchorH) << YAML::Value << static_cast<int>(m_anchorH);
	emitter << YAML::Key << NAME_OF(m_anchorV) << YAML::Value << static_cast<int>(m_anchorV);
	emitter << YAML::Key << NAME_OF(m_offsetPos) << YAML::Value << m_offsetPos;
	emitter << YAML::Key << NAME_OF(m_showDebugRect) << YAML::Value << m_showDebugRect;
}

void CUIBase::Deserialize(const YAML::Node& node)
{
	m_orderZ               = node[NAME_OF(m_orderZ)].as<int>();
	m_opacity              = node[NAME_OF(m_opacity)].as<float>();
	m_mouseCollisionEnable = node[NAME_OF(m_mouseCollisionEnable)].as<bool>();
	m_anchorH              = static_cast<ANCHOR_HORIZONTAL>(node[NAME_OF(m_anchorH)].as<int>());
	m_anchorV              = static_cast<ANCHOR_VERTICAL>(node[NAME_OF(m_anchorV)].as<int>());
	m_offsetPos            = node[NAME_OF(m_offsetPos)].as<Vec2>();
	if (node[NAME_OF(m_showDebugRect)].IsDefined())
	{
		m_showDebugRect = node[NAME_OF(m_showDebugRect)].as<int>();
	}
}
