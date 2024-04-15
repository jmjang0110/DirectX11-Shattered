#include "pch.h"
#include "CEventMgr.h"

#include "CSceneMgr.h"
#include "CScene.h"
#include "CLayer.h"
#include "CGameObject.h"

#include "CRenderMgr.h"
#include "CComponent.h"
#include "CDevice.h"
#include "CFSM.h"
#include "CMgrScript.h"
#include "CUIText.h"

CEventMgr::CEventMgr()
	: m_bObjEvn(false) {}

CEventMgr::~CEventMgr() = default;

void CEventMgr::update()
{
	m_bObjEvn = false;

	// Dead Object 삭제
	for (const auto& pGameObject : m_vecDead)
	{
		LOG_ASSERT(pGameObject, "DELETING INVALID GAME OBJECT");

		// 삭제되는 오브젝트가 부모가 있다면(자식 오브젝트라면)
		if (pGameObject->GetParent())
		{
			pGameObject->DisconnectBetweenParent();
		}
		delete pGameObject;

		m_bObjEvn = true;
	}
	m_vecDead.clear();

	// Event 처리
	for (size_t i = 0; i < m_vecEvent.size(); ++i)
	{
		tEventInfo eventInfo = m_vecEvent[i];
		switch (eventInfo.eType)
		{
		case EVENT_TYPE::CREATE_OBJ:
			{
				const auto    pGameObject = reinterpret_cast<CGameObject*>(eventInfo.lParam);
				const int     layerIndex  = static_cast<int>(eventInfo.wParam);
				const CScene* pCurScene   = CSceneMgr::GetInst()->GetCurScene();
				pCurScene->AddObject(pGameObject, layerIndex);

				pGameObject->start();

				m_bObjEvn = true;
				break;
			}
		case EVENT_TYPE::DELETE_OBJ:
			{
				auto pDeleteObject = reinterpret_cast<CGameObject*>(eventInfo.lParam);

				if (false == pDeleteObject->m_bDead)
				{
					m_vecDead.push_back(pDeleteObject);

					// 보유하고 있는 자식 오브젝트도 전부 Dead 체크 해둔다.
					static list<CGameObject*> queue{};
					queue.clear();
					queue.push_back(pDeleteObject);

					while (false == queue.empty())
					{
						CGameObject* pObj = queue.front();
						queue.pop_front();
						pObj->m_bDead = true;

						for (auto& pChild : pObj->GetChild())
						{
							queue.push_back(pChild);
						}
					}
				}
				break;
			}
		case EVENT_TYPE::ADD_CHILD:
			{
				const auto pParent = reinterpret_cast<CGameObject*>(eventInfo.lParam);
				const auto pChild  = reinterpret_cast<CGameObject*>(eventInfo.wParam);

				const int parentLayer = pParent->GetLayerIndex();
				const int childLayer  = pChild->GetLayerIndex();

				if (parentLayer != childLayer)
				{
					const CScene* pCurScene = CSceneMgr::GetInst()->GetCurScene();
					if (childLayer != -1)
					{
						pCurScene->GetLayer(childLayer)->DeregisterObject(pChild);
					}
					pChild->RenewLayerIndex(parentLayer);
				}
				pParent->AddChild(pChild);
				m_bObjEvn = true;
				break;
			}
		case EVENT_TYPE::DISCONNECT_PARENT:
			{
				const auto pObject = reinterpret_cast<CGameObject*>(eventInfo.lParam);
				pObject->DisconnectBetweenParent();
				const int     layerIndex = pObject->GetLayerIndex();
				const CScene* pCurScene  = CSceneMgr::GetInst()->GetCurScene();
				pCurScene->GetLayer(layerIndex)->RegisterObjectAsRoot(pObject);

				m_bObjEvn = true;
				break;
			}
		case EVENT_TYPE::SET_CAMEAR_INDEX:
			{
				const auto pCamera    = reinterpret_cast<CCamera*>(eventInfo.lParam);
				const int  iChangeIdx = static_cast<int>(eventInfo.wParam);
				CRenderMgr::GetInst()->SwapCameraIndex(pCamera, iChangeIdx);
				break;
			}
		case EVENT_TYPE::ACTIVATE_OBJECT:
			{
				const auto pObject = reinterpret_cast<CGameObject*>(eventInfo.lParam);
				pObject->m_bActive = true;
				pObject->active();
				break;
			}
		case EVENT_TYPE::DEACTIVATE_OBJECT:
			{
				const auto pObject = reinterpret_cast<CGameObject*>(eventInfo.lParam);
				pObject->m_bActive = false;
				pObject->deactive();
				break;
			}
		case EVENT_TYPE::ACTIVATE_COMPONENT:
			{
				const auto pCom = reinterpret_cast<CComponent*>(eventInfo.lParam);
				pCom->active();
				break;
			}
		case EVENT_TYPE::DEACTIVATE_COMOPNENT:
			{
				const auto pCom = reinterpret_cast<CComponent*>(eventInfo.lParam);
				pCom->deactive();
				break;
			}
		case EVENT_TYPE::SCENE_CHANGE:
			{
				m_bObjEvn = true;
				break;
			}
		case EVENT_TYPE::DELETE_RES:
			{
				const auto pRes = reinterpret_cast<CRes*>(eventInfo.lParam);
				CResMgr::GetInst()->DeleteRes(pRes->GetKey());

				m_bObjEvn = true;
				break;
			}
		case EVENT_TYPE::CHANGE_FSM_STATE:
			{
				const auto pFSM       = reinterpret_cast<CFSM*>(eventInfo.lParam);
				CState*    pNextState = reinterpret_cast<CState*>(eventInfo.wParam);

				// 기존 상태 Exit 
				CState* pPrevState = pFSM->GetCurState();
				if (pPrevState != nullptr)
				{
					pPrevState->Exit();
				}

				// 현재 상태 Enter 
				pNextState->Enter();
				pFSM->SetCurState(pNextState);

				break;
			}
		case EVENT_TYPE::CHANGE_OBJ_LAYER_INDEX:
			{
				const auto pObject      = reinterpret_cast<CGameObject*>(eventInfo.lParam);
				const UINT iNewLayerIdx = static_cast<UINT>(eventInfo.wParam);

				const UINT    iCurLayerIdx = pObject->GetLayerIndex();
				const CScene* pCurScene    = CSceneMgr::GetInst()->GetCurScene();
				pCurScene->GetLayer(iCurLayerIdx)->DeregisterObject(pObject);
				pCurScene->AddObject(pObject, iNewLayerIdx);

				m_bObjEvn = true;
				break;
			}
		case EVENT_TYPE::SWAP_LAYER:
			{
				const UINT iLayerIdx_1 = static_cast<UINT>(eventInfo.lParam);
				const UINT iLayerIdx_2 = static_cast<UINT>(eventInfo.wParam);

				CScene* pCurScene = CSceneMgr::GetInst()->GetCurScene();
				pCurScene->SwapLayer(iLayerIdx_1, iLayerIdx_2);
				m_bObjEvn = true;
				break;
			}
		case EVENT_TYPE::RENDER_TEXT:
			{
				auto pUIText = reinterpret_cast<CUIText*>(eventInfo.lParam);

				if (pUIText->GetText().empty())
				{
					break;
				}
				CDevice::GetInst()->GetRtv2D()->BeginDraw();
				const Vec3 worldPos       = pUIText->Transform()->GetWorldPos();
				const Vec3 halfWorldScale = pUIText->Transform()->GetWorldScale() * 0.5f;
				const Vec2 halfResolution = CDevice::GetInst()->GetRenderResolution() * 0.5f;

				if (pUIText->IsAlphaEnable())
				{
					pUIText->GetBrush()->SetOpacity(pUIText->GetOpacity());
				}
				else
				{
					pUIText->GetBrush()->SetOpacity(1.f);
				}

				//화면의 좌표계에 그려야 한다.
				D2D1_POINT_2F ptSize{};
				const float   yOffset = worldPos.y > 0 ? -fabs(worldPos.y) : fabs(worldPos.y);
				ptSize.x              = worldPos.x + halfResolution.x - halfWorldScale.x;
				ptSize.y              = yOffset + halfResolution.y - halfWorldScale.y;

				CDevice::GetInst()->GetRtv2D()->DrawTextLayout(ptSize,
				                                               pUIText->GetTextureLayout().Get(),
				                                               pUIText->GetBrush().Get(),
				                                               D2D1_DRAW_TEXT_OPTIONS_NONE);

				CDevice::GetInst()->GetRtv2D()->EndDraw();
				break;
			}
		}
	}
	m_vecEvent.clear();

	//Custom Event
	if (false == CSingletonScript::s_vecFunc.empty())
	{
		CSingletonScript::FireScriptEvents();
	}
}
