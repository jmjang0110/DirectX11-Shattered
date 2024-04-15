#include "pch.h"
#include "MainMenuScript.h"

#include <Engine/CUIBase.h>
#include <Engine/CUIButton.h>
#include <Engine/CRenderEffectMgr.h>
#include <Engine/CUIText.h>
#include <Engine/CUIImage.h>

#include "CObjectManager.h"
#include "Engine/CDevice.h"


namespace MAIN_MENU
{
	constexpr float DURATION = 1.5f;
}

MainMenuScript::MainMenuScript()
	: CScript{static_cast<int>(SCRIPT_TYPE::MAINMENUSCRIPT)}
	, m_pPlayerUI{nullptr} {}

MainMenuScript::~MainMenuScript() = default;


void MainMenuScript::start()
{
	FindChild();
	CRenderEffectMgr::GetInst()->Enable_FadeOutPaperBurn(MAIN_MENU::DURATION);
	m_pPlayerUI = CSceneMgr::GetInst()->FindObjectByName(L"PLAYER_UI_PANEL");
	m_pPlayerUI->Deactivate();

	//마우스를 제외한 인풋 일단 막기
	CKeyMgr::GetInst()->EnableAll(false);
	CKeyMgr::GetInst()->SetEnableInput(KEY::LBTN, true);

	wstring soundpath = L"sound\\opening.wav";
	CSound* pSound    = CResMgr::GetInst()->Load<CSound>(soundpath, soundpath).Get();
	pSound->Play(0, 0.3f, false);
}

void MainMenuScript::update()
{
	//Load
	MainLoading();
	OptionLoading();

	if (m_bitSelected.any() && CRenderEffectMgr::GetInst()->IsFadeInFinished())
	{
		if (m_bitSelected[E_START])
		{
			GetOwner()->Destroy();
			m_pPlayerUI->Activate();
			CObjectManager::GetInst()->GetPlayer()->Activate();
			CKeyMgr::GetInst()->EnableAll(true);
			/*	SetCursorPos(1600, 900);
				ShowCursor(SW_HIDE);*/

			wstring soundpath = L"sound\\opening.wav";
			CSound* pSound    = CResMgr::GetInst()->Load<CSound>(soundpath, soundpath).Get();
			pSound->Stop();

			static bool isSound = false;
			if (isSound == false)
			{
				const MAP_TYPE type = CObjectManager::GetInst()->GetCurrentMapType();
				if (type == MAP_TYPE::_01)
				{
					wstring soundpath = L"sound\\map01_sound.wav";
					CSound* pSound    = nullptr;
					pSound            = CResMgr::GetInst()->Load<CSound>(soundpath, soundpath).Get();
					pSound->Play(0, 0.3f, false);
				}
				isSound = true;
			}
		}
		else if (m_bitSelected[E_SETTING])
		{
			m_bitSelected.reset();
			m_bitSelected[E_OPTION] = true;
			GetOwner()->GetChild(L"OPTION")->Activate();
			GetOwner()->GetChild(L"TITLE")->Deactivate();
			GetOwner()->GetChild(L"SELECTION")->Deactivate();
			CRenderEffectMgr::GetInst()->Enable_FadeOutPaperBurn(MAIN_MENU::DURATION);

			for (const auto& pMenu : m_vecOptionMenu)
			{
				if (pMenu->GetName() != L"TITLE")
				{
					pMenu->GetChild(L"SELECTOR")->MeshRender()->Deactivate();
				}
			}
		}
		else if (m_bitSelected[E_LEAVE])
		{
			PostQuitMessage(0);
		}
		else if (m_bitSelected[E_BACK])
		{
			m_bitSelected.reset();
			GetOwner()->GetChild(L"OPTION")->Deactivate();
			GetOwner()->GetChild(L"TITLE")->Activate();
			GetOwner()->GetChild(L"SELECTION")->Activate();
			CRenderEffectMgr::GetInst()->Enable_FadeOutPaperBurn(MAIN_MENU::DURATION);
			for (const auto& pMenu : m_vecOptionMenu)
			{
				if (pMenu->GetName() != L"TITLE")
				{
					pMenu->GetChild(L"SELECTOR")->MeshRender()->Deactivate();
				}
			}
			for (const auto& pMenu : m_vecSelectionMenu)
			{
				pMenu->GetChild(L"SELECTOR")->MeshRender()->Deactivate();
			}
		}
	}
	else if (m_bitSelected.any() && false == CRenderEffectMgr::GetInst()->IsFadeInFinished())
	{
		//IN MAIN MENU
		if (false == m_bitSelected[E_BACK])
		{
			CGameObject* pTitleText = GetOwner()->GetChild(L"TITLE")->GetChild(L"TEXT");
			pTitleText->UIText()->SetOpacity(pTitleText->UIText()->GetOpacity() - DT / MAIN_MENU::DURATION);

			for (const auto& pMenu : m_vecSelectionMenu)
			{
				CGameObject* pText = pMenu->GetChild(L"TEXT");
				pText->UIText()->SetOpacity(pText->UIText()->GetOpacity() - DT / MAIN_MENU::DURATION);
			}
		}
		//OPTION -> MAIN
		else
		{
			for (const auto& pMenu : m_vecOptionMenu)
			{
				if (pMenu->GetName() != L"TITLE")
				{
					CGameObject* pText = pMenu->GetChild(L"TEXT");
					pText->UIText()->SetOpacity(pText->UIText()->GetOpacity() - DT / MAIN_MENU::DURATION);
				}
				else
				{
					pMenu->UIText()->SetOpacity(pMenu->UIText()->GetOpacity() - DT / MAIN_MENU::DURATION);
				}
			}
		}
	}
}

void MainMenuScript::MainLoading()
{
	if (m_bitSelected.none() && false == CRenderEffectMgr::GetInst()->IsFadeOutFinished())
	{
		CGameObject* pTitleText = GetOwner()->GetChild(L"TITLE")->GetChild(L"TEXT");
		pTitleText->UIText()->SetOpacity(pTitleText->UIText()->GetOpacity() + DT / MAIN_MENU::DURATION);

		for (const auto& pMenu : m_vecSelectionMenu)
		{
			CGameObject* pText   = pMenu->GetChild(L"TEXT");
			const float  opacity = ClampData(pText->UIText()->GetOpacity() + DT / MAIN_MENU::DURATION, 0.f, 0.5f);
			pText->UIText()->SetOpacity(opacity);
		}
	}
}

void MainMenuScript::OptionLoading()
{
	if (m_bitSelected[E_OPTION] && false == CRenderEffectMgr::GetInst()->IsFadeOutFinished())
	{
		for (const auto& pMenu : m_vecOptionMenu)
		{
			if (pMenu->GetName() != L"TITLE")
			{
				CGameObject* pText   = pMenu->GetChild(L"TEXT");
				const float  opacity = ClampData(pText->UIText()->GetOpacity() + DT / MAIN_MENU::DURATION,
				                                 0.f,
				                                 0.5f);
				pText->UIText()->SetOpacity(opacity);
			}
			else
			{
				pMenu->UIText()->SetOpacity(pMenu->UIText()->GetOpacity() + DT / MAIN_MENU::DURATION);
			}
		}
	}
}

void MainMenuScript::FindChild()
{
	//TITLE
	GetOwner()->GetChild(L"TITLE")->GetChild(L"TEXT")->UIText()->SetAlphaEnable(true);
	GetOwner()->GetChild(L"TITLE")->GetChild(L"TEXT")->UIText()->SetOpacity(0.f);

	//SELECTION
	const CGameObject* pSelection = GetOwner()->GetChild(L"SELECTION");
	m_vecSelectionMenu            = pSelection->GetChild();
	for (const auto& pSelectionMenu : m_vecSelectionMenu)
	{
		pSelectionMenu->GetUIBaseComponenent()->SetMouseHoverCallback(this, &MainMenuScript::OnHover);
		pSelectionMenu->GetUIBaseComponenent()->SetMouseLeaveCallback(this, &MainMenuScript::OnHover);
		pSelectionMenu->GetChild(L"SELECTOR")->MeshRender()->Deactivate();
		pSelectionMenu->GetChild(L"TEXT")->UIText()->SetAlphaEnable(true);
		pSelectionMenu->GetChild(L"TEXT")->UIText()->SetOpacity(0.f);
	}
	CGameObject* pLeave = pSelection->GetChild(L"LEAVE");
	pLeave->UIButton()->SetCallback(this, &MainMenuScript::LeaveGame, MOUSE_TYPE::LEFT, CLICK_TYPE::ONE);

	CGameObject* pStart = pSelection->GetChild(L"NEW GAME");
	pStart->UIButton()->SetCallback(this, &MainMenuScript::StartGame, MOUSE_TYPE::LEFT, CLICK_TYPE::ONE);

	CGameObject* pSetting = pSelection->GetChild(L"SETTING");
	pSetting->UIButton()->SetCallback(this, &MainMenuScript::SettingGame, MOUSE_TYPE::LEFT, CLICK_TYPE::ONE);

	//OPTION
	CGameObject* pOption = GetOwner()->GetChild(L"OPTION");
	pOption->Deactivate();
	m_vecOptionMenu = pOption->GetChild();
	for (const auto& pOptionMenu : m_vecOptionMenu)
	{
		if (pOptionMenu->GetName() != L"TITLE")
		{
			pOptionMenu->GetUIBaseComponenent()->SetMouseHoverCallback(this, &MainMenuScript::OnHover);
			pOptionMenu->GetUIBaseComponenent()->SetMouseLeaveCallback(this, &MainMenuScript::OnHover);
			pOptionMenu->GetChild(L"TEXT")->UIText()->SetAlphaEnable(true);
			pOptionMenu->GetChild(L"TEXT")->UIText()->SetOpacity(0.f);
		}
		else
		{
			pOptionMenu->UIText()->SetAlphaEnable(true);
			pOptionMenu->UIText()->SetOpacity(0.f);
		}
	}
	CGameObject* pFXAA = pOption->GetChild(L"FXAA");
	OnFXAA(pFXAA);
	pFXAA->UIButton()->SetCallback(this, &MainMenuScript::OnFXAA, MOUSE_TYPE::LEFT, CLICK_TYPE::ONE);

	CGameObject* pSSAO = pOption->GetChild(L"SSAO");
	OnSSAO(pSSAO);
	pSSAO->UIButton()->SetCallback(this, &MainMenuScript::OnSSAO, MOUSE_TYPE::LEFT, CLICK_TYPE::ONE);

	CGameObject* pVsync = pOption->GetChild(L"VSYNC");
	pVsync->UIButton()->SetCallback(this, &MainMenuScript::OnVSync, MOUSE_TYPE::LEFT, CLICK_TYPE::ONE);

	CGameObject* pBack = pOption->GetChild(L"BACK");
	pBack->UIButton()->SetCallback(this, &MainMenuScript::OnBack, MOUSE_TYPE::LEFT, CLICK_TYPE::ONE);
}

void MainMenuScript::StartGame(CGameObject* pGameObject)
{
	if (m_bitSelected.none())
	{
		CRenderEffectMgr::GetInst()->Enable_FadeInPaperBurn(MAIN_MENU::DURATION);
		m_bitSelected[E_START] = true;
	}
}

void MainMenuScript::SettingGame(CGameObject* pGameObject)
{
	if (m_bitSelected.none())
	{
		CRenderEffectMgr::GetInst()->Enable_FadeInPaperBurn(MAIN_MENU::DURATION);
		m_bitSelected[E_SETTING] = true;
	}
}

void MainMenuScript::LeaveGame(CGameObject* pGameObject)
{
	if (m_bitSelected.none())
	{
		CRenderEffectMgr::GetInst()->Enable_FadeInPaperBurn(MAIN_MENU::DURATION);
		m_bitSelected[E_LEAVE] = true;
	}
}

void MainMenuScript::OnHover(CGameObject* pGameObject)
{
	if (pGameObject->GetUIBaseComponenent()->IsMouseHovered())
	{
		if (false == pGameObject->GetChild(L"SELECTOR")->MeshRender()->IsActive())
		{
			pGameObject->GetChild(L"SELECTOR")->MeshRender()->Activate();
		}
		pGameObject->GetChild(L"TEXT")->UIText()->SetOpacity(1.f);
	}
	else
	{
		if (pGameObject->GetChild(L"SELECTOR")->MeshRender()->IsActive())
		{
			pGameObject->GetChild(L"SELECTOR")->MeshRender()->Deactivate();
		}
		pGameObject->GetChild(L"TEXT")->UIText()->SetOpacity(0.5f);
	}
}

void MainMenuScript::OnFXAA(CGameObject* pGameObject)
{
	const bool enabled = CRenderEffectMgr::GetInst()->IsEnable_FXAA();
	if (enabled)
	{
		CRenderEffectMgr::GetInst()->Disable_FXAA();
		pGameObject->GetChild(L"CHECK_BOX")->UIImage()->SetImageInfo(L"NOT_SELECTED");
	}
	else
	{
		CRenderEffectMgr::GetInst()->Enable_FXAA();
		pGameObject->GetChild(L"CHECK_BOX")->UIImage()->SetImageInfo(L"SELECTED");
	}
}

void MainMenuScript::OnSSAO(CGameObject* pGameObject)
{
	const bool enabled = CRenderEffectMgr::GetInst()->IsEnable_SSAO();
	if (enabled)
	{
		CRenderEffectMgr::GetInst()->Disable_SSAO();
		pGameObject->GetChild(L"CHECK_BOX")->UIImage()->SetImageInfo(L"NOT_SELECTED");
	}
	else
	{
		CRenderEffectMgr::GetInst()->Enable_SSAO();
		pGameObject->GetChild(L"CHECK_BOX")->UIImage()->SetImageInfo(L"SELECTED");
	}
}

void MainMenuScript::OnVSync(CGameObject* pGameObject)
{
	const bool enabled = CDevice::GetInst()->IsVSyncEnabled();
	if (enabled)
	{
		CDevice::GetInst()->SetVsync(false);
		pGameObject->GetChild(L"CHECK_BOX")->UIImage()->SetImageInfo(L"NOT_SELECTED");
	}
	else
	{
		CDevice::GetInst()->SetVsync(true);
		pGameObject->GetChild(L"CHECK_BOX")->UIImage()->SetImageInfo(L"SELECTED");
	}
}

void MainMenuScript::OnBack(CGameObject* pGameObject)
{
	if (m_bitSelected[E_BACK] == false)
	{
		CRenderEffectMgr::GetInst()->Enable_FadeInPaperBurn(MAIN_MENU::DURATION);
		m_bitSelected.reset();
		m_bitSelected[E_BACK] = true;
	}
}
