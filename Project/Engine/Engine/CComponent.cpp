#include "pch.h"
#include "CComponent.h"

#include "CEventMgr.h"

#include "CAnimator2D.h"
#include "CAnimator3D.h"
#include "CCamera.h"
#include "CCollider2D.h"
#include "CDecal.h"
#include "CLandScape.h"
#include "CLight2D.h"
#include "CLight3D.h"
#include "CMeshRender.h"
#include "CParticleSystem.h"
#include "CSkyBox.h"
#include "CTileMap.h"
#include "CTransform.h"
#include "CCollider3D.h"
#include "CBoundingBox.h"
#include "CFSM.h"
#include "CRigidBody.h"
#include "CNaviAgent.h"
#include "CNaviMap.h"
#include "CRenderMgr.h"
#include "CUIButton.h"
#include "CUIImage.h"
#include "CUIPanel.h"
#include "CUIProgressBar.h"
#include "CUIText.h"

CComponent::CComponent(COMPONENT_TYPE _eType)
	: m_eComType(_eType)
	, m_pOwner{nullptr}
	, m_bActive(true)
	, m_pDebugObj(nullptr) {}

CComponent::~CComponent()
{
	if (nullptr != m_pDebugObj)
	{
		delete m_pDebugObj;
	}
}

void CComponent::Activate()
{
	tEventInfo info{};
	info.eType  = EVENT_TYPE::ACTIVATE_COMPONENT;
	info.lParam = (DWORD_PTR)this;
	CEventMgr::GetInst()->AddEvent(info);
}

void CComponent::Deactivate()
{
	tEventInfo info{};
	info.eType  = EVENT_TYPE::DEACTIVATE_COMOPNENT;
	info.lParam = (DWORD_PTR)this;
	CEventMgr::GetInst()->AddEvent(info);
}

CComponent* CComponent::MakeComponent(const std::wstring& name)
{
	const COMPONENT_TYPE type = magic_enum::enum_cast<COMPONENT_TYPE>(ToString(name)).value_or(COMPONENT_TYPE::END);
	switch (type)
	{
	case COMPONENT_TYPE::TRANSFORM: return new CTransform{};
	case COMPONENT_TYPE::CAMERA: return new CCamera{};
	case COMPONENT_TYPE::COLLIDER2D: return new CCollider2D{};
	case COMPONENT_TYPE::COLLIDER3D: return new CCollider3D{};
	case COMPONENT_TYPE::ANIMATOR2D: return new CAnimator2D{};
	case COMPONENT_TYPE::ANIMATOR3D: return new CAnimator3D{};
	case COMPONENT_TYPE::LIGHT2D: return new CLight2D{};
	case COMPONENT_TYPE::LIGHT3D: return new CLight3D{};
	case COMPONENT_TYPE::BOUNDINGBOX: return new CBoundingBox{};
	case COMPONENT_TYPE::FSM: return new CFSM{};
	case COMPONENT_TYPE::RIGIDBODY: return new CRigidBody{};
	case COMPONENT_TYPE::NAVIMAP: return new CNaviMap{};
	case COMPONENT_TYPE::NAVIAGENT: return new CNaviAgent{};
	case COMPONENT_TYPE::UIPANEL: return new CUIPanel{};
	case COMPONENT_TYPE::UIIMAGE: return new CUIImage{};
	case COMPONENT_TYPE::UITEXT: return new CUIText{};
	case COMPONENT_TYPE::UIPROGRESSBAR: return new CUIProgressBar{};
	case COMPONENT_TYPE::UIBUTTON: return new CUIButton{};
	case COMPONENT_TYPE::MESHRENDER: return new CMeshRender{};
	case COMPONENT_TYPE::TILEMAP: return new CTileMap{};
	case COMPONENT_TYPE::PARTICLESYSTEM: return new CParticleSystem{};
	case COMPONENT_TYPE::LANDSCAPE: return new CLandScape{};
	case COMPONENT_TYPE::DECAL: return new CDecal{};
	case COMPONENT_TYPE::SKYBOX: return new CSkyBox{};
	case COMPONENT_TYPE::END:
	case COMPONENT_TYPE::SCRIPT:
	default:
		return nullptr;;
	}
	//기존 방식
	/*
	if (L"TRANSFORM" == name) return new CTransform{};
	if (L"CAMERA" == name) return new CCamera{};
	if (L"COLLIDER2D" == name) return new CCollider2D{};
	if (L"COLLIDER3D" == name) return new CCollider3D{};
	if (L"ANIMATOR2D" == name) return new CAnimator2D{};
	if (L"ANIMATOR3D" == name) return new CAnimator3D{};
	if (L"LIGHT2D" == name) return new CLight2D{};
	if (L"LIGHT3D" == name) return new CLight3D{};
	if (L"BOUNDINGBOX" == name) return new CBoundingBox{};
	if (L"MESHRENDER" == name) return new CMeshRender{};
	if (L"TILEMAP" == name) return new CTileMap{};
	if (L"PARTICLESYSTEM" == name) return new CParticleSystem{};
	if (L"LANDSCAPE" == name) return new CLandScape{};
	if (L"DECAL" == name) return new CDecal{};
	if (L"SKYBOX" == name) return new CSkyBox{};
	if (L"FSM" == name) return new CFSM{};
	if (L"NAVIMAP" == name) return new CNaviMap{};
	if (L"NAVIAGENT" == name) return new CNaviAgent{};
	if (L"UIPANEL" == name) return new CUIPanel{};
	if (L"UIIMAGE" == name) return new CUIImage{};
	if (L"UITEXT" == name) return new CUIText{};
	if (L"UIPROGRESSBAR" == name) return new CUIProgressBar{};
	if (L"UIBUTTON" == name) return new CUIButton{};


	return nullptr;*/
}
