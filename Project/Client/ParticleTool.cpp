#include "pch.h"
#include "ParticleTool.h"

#include <Engine/CRenderMgr.h>
#include <Engine/CMRT.h>
#include <Engine/CSceneMgr.h>
#include <Engine/CScene.h>
#include <Engine/CLayer.h>
#include <Engine/define.h>
#include <Engine/CGameObject.h>
#include <Engine/CParticleSystem.h>
#include <Engine/CTransform.h>
#include <Engine/CGraphicsShader.h>
#include <Engine/CMaterial.h>
#include <Engine/CCore.h>

#include "ListUI.h"
#include "CImGuiMgr.h"
#include "ParticleSystemUI.h"

ParticleTool::ParticleTool()
	: UI("##ParticleToolUI")
	, m_pParticleTarget(nullptr)
	, m_pEmissiveTarget(nullptr)
	, m_vecParticleSystem()
	, m_pTargetParticle(nullptr)
	, m_strvecParticleName()
	, m_vecTargetResolution(0.f)
	, m_iMaxCount{ 5 }
	, m_bPosInherit{ false }
	, m_iAliveCount{ 1 }
	, m_fArrMinMaxTime{ {0.f}, {5.f} }
	, m_fArrMinMaxSpeed{ {10.f}, {100.f} }
	, m_fRange{ 10.f }
	, m_fTerm{ 3.f }
	, m_fAngle(180.f)
	, m_bShaderUseEmissive{ false }
	, m_bShaderUseSpeedDetail(false)
	, m_fArrDirection{ {1.f}, {0.f} }
	, m_iParticleComboIDX(0)
	, m_bLinerParticle(false)
	, m_bUseSoftParticle(false)
	, m_strSoundName()
	, m_pSound(nullptr)
{}

ParticleTool::~ParticleTool() {}

void ParticleTool::ParticleOptionSetting()
{
	ImGui::Separator();
	ImGui::Text("PARTICLE OPTION SETTING");

	CParticleSystem* pTargetParticleSystem = m_pTargetParticle->ParticleSystem();

	if (nullptr == pTargetParticleSystem)
		return;

	ImGui::PushItemWidth(200); // Float3 위젯 간격 설정

	ImGui::BulletText("Pos Inherit");
	ImGui::SameLine(200);
	ImGui::Checkbox("##POS_INHERIT", &m_bPosInherit);
	pTargetParticleSystem->SetPosInherit(m_bPosInherit);

	ImGui::BulletText("Max Count");
	ImGui::SameLine(200);
	ImGui::InputInt("##MAX_COUNT", &m_iMaxCount);
	pTargetParticleSystem->SetMaxParticleCount(m_iMaxCount);

	ImGui::BulletText("Alive Count");
	ImGui::SameLine(200);
	ImGui::InputInt("##ALIVE_COUNT", &m_iAliveCount);
	pTargetParticleSystem->SetAliveCount(m_iAliveCount);

	ImGui::BulletText("MIN/MAX Time");
	ImGui::SameLine(200);
	ImGui::InputFloat2("##MIN_MAX_TIME", m_fArrMinMaxTime);
	pTargetParticleSystem->SetMinMaxLifeTime(m_fArrMinMaxTime[0], m_fArrMinMaxTime[1]);

	ImGui::BulletText("MIN/MAX Speed");
	ImGui::SameLine(200);
	ImGui::InputFloat2("##MIN_MAX_SPEED", m_fArrMinMaxSpeed);
	pTargetParticleSystem->SetMinMaxSpeed(m_fArrMinMaxSpeed[0], m_fArrMinMaxSpeed[1]);

	ImGui::BulletText("SPEED DETAIL");
	ImGui::SameLine();
	ImGui::Checkbox("##SHADER_USE_SPEED_DETAIL", &m_bShaderUseSpeedDetail);
	if (m_bShaderUseSpeedDetail)
	{
		static const char* s_arrSpeedDetailName[] = {
			"y = x",
			"y = log(2)(x)",
			"y = (2) ^ (x)",
		};

		int iSpeedDetailIndex = pTargetParticleSystem->GetSpeedDetailFunc();

		ImGui::SameLine(200);
		if (ImGui::BeginCombo("##PARTICLE_SPEED_DETAIL_COMBO", s_arrSpeedDetailName[iSpeedDetailIndex]))
		{
			for (size_t i = 0; i < std::size(s_arrSpeedDetailName); ++i)
			{
				if (ImGui::Selectable(s_arrSpeedDetailName[i]))
				{
					pTargetParticleSystem->SetSpeedDetailFunc(static_cast<UINT>(i));
				}
			}
			ImGui::EndCombo();
		}
	}

	CParticleSystem* pCurPSys = m_pTargetParticle->ParticleSystem();

	ImGui::BulletText("Liner Particle Use");
	ImGui::SameLine(200);
	ImGui::Checkbox("##LINER_PARTICLE_USE", &m_bLinerParticle);
	float liner = (float)m_bLinerParticle;
	pCurPSys->GetMaterial(0)->SetScalarParam(SCALAR_PARAM::FLOAT_0, &liner);
	pTargetParticleSystem->SetLinearParticle(m_bLinerParticle);

	ImGui::BulletText("Soft Particle Use");
	ImGui::SameLine(200);
	ImGui::Checkbox("##SOFT_PARTICLE_USE", &m_bUseSoftParticle);
	float soft = (float)m_bUseSoftParticle;
	pCurPSys->GetMaterial(0)->SetScalarParam(SCALAR_PARAM::FLOAT_1, &soft);
	pTargetParticleSystem->SetSoftParticle(m_bUseSoftParticle);
	if (m_bUseSoftParticle)
	{
		if (nullptr == m_pDepthTarget)
		{
			m_pDepthTarget = CResMgr::GetInst()->FindRes<CTexture>(L"PositionTargetTex");
		}
		pCurPSys->GetMaterial(0)->SetTexParam(TEX_PARAM::TEX_1, m_pDepthTarget);
	}


	ImGui::BulletText("Start Color");
	ImGui::SameLine(200);
	ImGui::ColorEdit4("##START_COLOR", m_vArrStartEndColor[0]);
	ImGui::BulletText("End Color");
	ImGui::SameLine(200);
	ImGui::ColorEdit4("##END_COLOR", m_vArrStartEndColor[1]);
	pTargetParticleSystem->SetStartEndColor(m_vArrStartEndColor[0], m_vArrStartEndColor[1]);

	ImGui::BulletText("Use Emissive"); ImGui::SameLine(200);
	ImGui::Checkbox("##EmissiveCheckBox", &m_bShaderUseEmissive);
	pTargetParticleSystem->SetUseEmissive(m_bShaderUseEmissive);

	if (m_bShaderUseEmissive)
	{
		ImGui::Indent();
		ImGui::BulletText("Emissive");
		ImGui::SameLine(200);
		ImGui::ColorEdit4("##EMISSIVE", m_vArrStartEndEmissive[0]);
		pTargetParticleSystem->SetStartEndEmissiveColor(m_vArrStartEndEmissive[0], m_vArrStartEndEmissive[0]);
		ImGui::Unindent();
	}

	ImGui::BulletText("Start Scale");
	ImGui::SameLine(200);
	ImGui::InputFloat3("##START_SCALE", m_vArrStartEndScale[0]);
	ImGui::BulletText("End Scale");
	ImGui::SameLine(200);
	ImGui::InputFloat3("##END_SCALE", m_vArrStartEndScale[1]);
	pTargetParticleSystem->SetStartEndScale(m_vArrStartEndScale[0], m_vArrStartEndScale[1]);

	ImGui::BulletText("Creation Range");
	ImGui::SameLine(200);
	ImGui::InputFloat("##CREATION_RANGE", &m_fRange);
	pTargetParticleSystem->SetRange(m_fRange);

	ImGui::BulletText("Creation Term");
	ImGui::SameLine(200);
	ImGui::InputFloat("##CREATION_TERM", &m_fTerm);
	pTargetParticleSystem->SetTerm(m_fTerm);

	ImGui::BulletText("Direction");
	ImGui::Indent();
	ImGui::BulletText("Dir X  :");
	ImGui::SameLine(200);
	ImGui::DragFloat("##PARTICLE_DIR_X", &m_fArrDirection[0], 0.1f, -1.0f, 1.0f, "%.1f");
	ImGui::BulletText("Dir Y  :");
	ImGui::SameLine(200);
	ImGui::DragFloat("##PARTICLE_DIR_Y", &m_fArrDirection[1], 0.1f, -1.0f, 1.0f, "%.1f");
	ImGui::Unindent();

	Vec2 dirNormal = Vec2(m_fArrDirection[0], m_fArrDirection[1]);
	dirNormal.Normalize();
	m_fArrDirection[0] = dirNormal.x;
	m_fArrDirection[1] = dirNormal.y;
	pTargetParticleSystem->SetDirection(Vec2(m_fArrDirection[0], m_fArrDirection[1]));

	ImGui::BulletText("Angle");
	ImGui::SameLine((200));
	ImGui::DragFloat("##PARTICLE_ANGLE_FLOAT", &m_fAngle, 1.f, 0, 360, "%.1f");
	pTargetParticleSystem->SetAngle((m_fAngle));


	ImGui::BulletText("PARTICLE IMAGE");
	ImGui::SameLine(200);
	Ptr<CTexture> pTexture = pTargetParticleSystem->GetMaterial(0)->GetTexParam(TEX_PARAM::TEX_0);
	if (nullptr != pTexture)
	{
		ImGui::BulletText(ToString(pTexture->GetKey()).c_str());
	}
	else
		ImGui::BulletText("NONE");
	ImGui::SameLine(200);
	if (ImGui::Button("##PARTICLE_TEXTURE_BUTTON", Vec2(15, 15)))
	{
		// ListUI 활성화한다.
		const auto& mapRes = CResMgr::GetInst()->GetResList(RES_TYPE::TEXTURE);
		ListUI* pListUI = static_cast<ListUI*>(CImGuiMgr::GetInst()->FindUI("##ListUI"));
		pListUI->Clear();
		pListUI->SetTitle("TEXTURE_LIST");

		for (const auto& pairData : mapRes)
		{
			pListUI->AddList(ToString(pairData.first));
		}

		pListUI->Activate();
		pListUI->SetDBCEvent(this, (DBCLKED)&ParticleTool::TextureSelect);
	}

	SetSound();

}

void ParticleTool::SaveParticle()
{
	wchar_t      szName[256] = {};
	OPENFILENAME ofn = {};

	ofn.lStructSize = sizeof(OPENFILENAME);
	ofn.hwndOwner = CCore::GetInst()->GetMainHwnd();
	ofn.lpstrFile = szName;
	ofn.nMaxFile = sizeof(szName);
	ofn.lpstrFilter = L"ALL\0*.*\0Particle\0*.prtcl\0";
	ofn.nFilterIndex = 0;
	ofn.lpstrFileTitle = nullptr;
	ofn.nMaxFileTitle = 0;

	wstring strTileFolder = CPathMgr::GetInst()->GetContentPath();
	strTileFolder += L"prtcl";

	ofn.lpstrInitialDir = strTileFolder.c_str();
	ofn.Flags = OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST;

	// Modal
	if (GetSaveFileName(&ofn))
	{
		FILE* pFile = nullptr;
		_wfopen_s(&pFile, szName, L"wb");

		assert(pFile);
		if (nullptr == pFile)
			return;

		m_pTargetParticle->SaveToScene(pFile);

		fclose(pFile);
	}
}

void ParticleTool::LoadParticle(CGameObject* _load)
{
	wchar_t szName[256] = {};

	OPENFILENAME ofn = {};

	ofn.lStructSize = sizeof(OPENFILENAME);
	ofn.hwndOwner = CCore::GetInst()->GetMainHwnd();
	ofn.lpstrFile = szName;
	ofn.nMaxFile = sizeof(szName);
	ofn.lpstrFilter = L"ALL\0*.*\0Particle\0*.prtcl\0";
	ofn.nFilterIndex = 0;
	ofn.lpstrFileTitle = nullptr;
	ofn.nMaxFileTitle = 0;

	wstring strTileFolder = CPathMgr::GetInst()->GetContentPath();
	strTileFolder += L"prtcl";

	ofn.lpstrInitialDir = strTileFolder.c_str();
	ofn.Flags = OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST;

	// Modal
	if (GetOpenFileName(&ofn))
	{
		FILE* pFile = nullptr;
		_wfopen_s(&pFile, szName, L"rb");

		assert(pFile);
		if (nullptr == pFile)
			return;

		_load->LoadFromScene(pFile);

		fclose(pFile);
	}
}

void ParticleTool::SetSound()
{
	ImGui::Text("Particle Sound : "); ImGui::SameLine(200);
	if (L"" == m_strSoundName)
	{
		ImGui::Text("-SOUND IS NOT EXIST-");
	}
	else
	{
		string name = ToString(m_strSoundName);
		ImGui::Text(name.c_str());
	}
	ImGui::SameLine();
	if (ImGui::Button("LOAD SOUND"))
	{
		SoundLoad();

		if (L"" == m_strSoundName)
			return;

		wstring soundpath = L"sound\\";
		soundpath += m_strSoundName;
		m_pSound = CResMgr::GetInst()->Load<CSound>(soundpath, soundpath).Get();

	}
}

void ParticleTool::SoundLoad()
{
	wchar_t szName[256] = {};

	OPENFILENAME ofn = {};

	ofn.lStructSize = sizeof(OPENFILENAME);
	ofn.hwndOwner = CCore::GetInst()->GetMainHwnd();
	ofn.lpstrFile = szName;
	ofn.nMaxFile = sizeof(szName);
	ofn.lpstrFilter = L"ALL\0*.*\0Wav\0*.wav\0";
	ofn.nFilterIndex = 0;
	ofn.lpstrFileTitle = nullptr;
	ofn.nMaxFileTitle = 0;

	wstring strTileFolder = CPathMgr::GetInst()->GetContentPath();
	strTileFolder += L"prtcl";

	ofn.lpstrInitialDir = strTileFolder.c_str();
	ofn.Flags = OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST;

	wstring SoundPath = L"";

	// Modal
	if (GetOpenFileName(&ofn))
	{
		FILE* pFile = nullptr;
		_wfopen_s(&pFile, szName, L"rb");

		assert(pFile);
		if (nullptr == pFile)
			return;

		fclose(pFile);
		int i = 0;
		while (true)
		{
			if (L'\0' == szName[i])
				break;
			else
			{
				SoundPath += szName[i];
				i++;
			}
		}

		int pathLength = (int)SoundPath.size();
		wstring reversedName = L"";


		if (0 == pathLength)
			return;
		else
		{
			m_strSoundName = L"";
			while (true)
			{
				if (L'\\' == SoundPath[pathLength])
				{
					break;
				}
				else
				{
					if (L'\0' != SoundPath[pathLength])
					{
						reversedName += SoundPath[pathLength];
					}
					pathLength--;
				}
			}
			int reverseSize = (int)reversedName.size() - 1;
			while (true)
			{
				if (-1 == reverseSize)
					break;
				m_strSoundName += reversedName[reverseSize];
				reverseSize--;
			}
		}

		CParticleSystem* pTargetParticleSystem = m_pTargetParticle->ParticleSystem();
		pTargetParticleSystem->SetSoundName(m_strSoundName);
	}
}

void ParticleTool::SetData()
{
	if (nullptr == m_pTargetParticle)
		return;
	if (true == m_pTargetParticle->IsDead())
		return;

	CParticleSystem* pTargetParticleSystem = m_pTargetParticle->ParticleSystem();

	if (nullptr == pTargetParticleSystem)
		return;

	m_iMaxCount = pTargetParticleSystem->GetMaxParticleCount();
	m_bPosInherit = pTargetParticleSystem->PosInherit();
	m_iAliveCount = pTargetParticleSystem->GetAliveCount();

	const Vec2& vLifeTime = pTargetParticleSystem->GetMinMaxLifeTime();
	m_fArrMinMaxTime[0] = vLifeTime.x;
	m_fArrMinMaxTime[1] = vLifeTime.y;

	const Vec2& vMinMaxSpeed = pTargetParticleSystem->GetMinMaxSpeed();
	m_fArrMinMaxSpeed[0] = vMinMaxSpeed.x;
	m_fArrMinMaxSpeed[1] = vMinMaxSpeed.y;

	const std::pair<Vec4, Vec4>& pairColor = pTargetParticleSystem->GetStartEndColor();
	m_vArrStartEndColor[0] = pairColor.first;
	m_vArrStartEndColor[1] = pairColor.second;

	const std::pair<Vec4, Vec4>& pairEColor = pTargetParticleSystem->GetStartEndEmissiveColor();
	m_vArrStartEndEmissive[0] = pairEColor.first;
	m_vArrStartEndEmissive[1] = pairEColor.second;

	const std::pair<Vec3, Vec3>& pairScale = pTargetParticleSystem->GetStartEndScale();
	m_vArrStartEndScale[0] = pairScale.first;
	m_vArrStartEndScale[1] = pairScale.second;

	m_fAngle = pTargetParticleSystem->GetAngle();

	Vec2 dir = pTargetParticleSystem->GetDirection();
	m_fArrDirection[0] = dir.x;
	m_fArrDirection[1] = dir.y;

	m_bLinerParticle = pTargetParticleSystem->GetLinearParticle();
	m_bShaderUseEmissive = pTargetParticleSystem->GetUseEmissive();

	m_fRange = pTargetParticleSystem->GetRange();
	m_fTerm = pTargetParticleSystem->GetTerm();

	m_strSoundName = pTargetParticleSystem->GetSoundName();

}

void ParticleTool::TextureSelect(void* _pTextureName)
{
	const std::wstring  key = ToWString(static_cast<char*>(_pTextureName));
	const Ptr<CTexture> pTexture = CResMgr::GetInst()->FindRes<CTexture>(key);

	CParticleSystem* pCurPSys = m_pTargetParticle->ParticleSystem();

	// 변경점이 있을 때만 세팅
	if (pCurPSys->GetMaterial(0)->GetTexParam(TEX_PARAM::TEX_0) != pTexture)
	{
		pCurPSys->GetMaterial(0)->SetTexParam(TEX_PARAM::TEX_0, pTexture);

		CMaterial* curMaterial = pCurPSys->GetMaterial(0).Get();
		wstring    contentPath = CPathMgr::GetInst()->GetContentPath();
		contentPath += curMaterial->GetKey();
		pCurPSys->GetMaterial(0)->Save(contentPath);
	}
}

void ParticleTool::ParticleCreate()
{
	ImGui::Text("Particle Menu");

	if (ImGui::Button("Create Particle", ImVec2(0, 0)))
	{
		ImGui::OpenPopup("Set Particle Name");
	}

	if (ImGui::BeginPopupModal("Set Particle Name", NULL, ImGuiWindowFlags_AlwaysAutoResize))
	{
		static char particleName[512];
		ImGui::Text("Set Particle Name : ");
		ImGui::SameLine();
		ImGui::InputText("Name", particleName, IM_ARRAYSIZE(particleName), ImGuiInputTextFlags_None);

		if (ImGui::Button("CREATE", ImVec2(0, 0)))
		{
			if ('\0' == particleName[0])
			{
				ImGui::EndPopup();
				return;
			}

			wstring name = L"";
			int     i = 0;
			while (true)
			{
				if ('\0' == particleName[i])
				{
					break;
				}
				name += particleName[i];
				particleName[i] = '\0';
				i++;
			}

			CGameObject* pParticle = new CGameObject;
			pParticle->SetName(name);
			pParticle->AddComponent(new CTransform);
			pParticle->Transform()->SetRelativePos(Vec3(0.f, 0.f, 0.f));
			pParticle->AddComponent(new CParticleSystem);
			CSceneMgr::GetInst()->SpawnObject(pParticle, L"BG");


			CMaterial* pMtrl = nullptr;
			wstring    materialName = L"material\\";
			materialName += name;
			materialName += L".mtrl";

			if (nullptr != CResMgr::GetInst()->FindRes<CMaterial>(materialName))
			{
				pMtrl = CResMgr::GetInst()->FindRes<CMaterial>(materialName).Get();
			}
			else
			{
				pMtrl = new CMaterial;
				pMtrl->SetShader(CResMgr::GetInst()->FindRes<CGraphicsShader>(L"ParticleRenderShader"));
				CResMgr::GetInst()->AddRes<CMaterial>(materialName, pMtrl);
			}

			pParticle->ParticleSystem()->SetMaterial(materialName);

			AddParticleCombo(pParticle);

			ImGui::CloseCurrentPopup();
		}
		ImGui::SameLine(200);
		if (ImGui::Button("CLOSE", ImVec2(0, 0)))
		{
			ImGui::CloseCurrentPopup();
		}
		ImGui::EndPopup();
	}
}

void ParticleTool::ParticleErase()
{
	if (ImGui::Button("Erase Particle", ImVec2(0, 0)))
	{
		if (nullptr == m_pTargetParticle)
			return;

		CGameObject* pDeleteParticle = m_pTargetParticle;
		m_vecParticleSystem = {};

		vector<CGameObject*> pVecBGObj = CSceneMgr::GetInst()->GetCurScene()->GetLayer(0)->GetObjects();
		for (size_t i = 0; i < pVecBGObj.size(); i++)
		{
			if (nullptr != pVecBGObj[i]->GetComponent(COMPONENT_TYPE::PARTICLESYSTEM))
			{
				if (pVecBGObj[i] != pDeleteParticle)
					m_vecParticleSystem.push_back(pVecBGObj[i]);
			}
		}

		m_pTargetParticle->Destroy();
		if (0 == m_vecParticleSystem.size())
		{
			m_pTargetParticle = nullptr;
		}
		else
		{
			m_pTargetParticle = m_vecParticleSystem[0];
			m_iParticleComboIDX = 0;
			SetData();
		}
	}
}

void ParticleTool::ParticleSaveNLoad()
{
	if (ImGui::Button("SAVE - CUR PARTICLE"))
	{
		SaveParticle();
	}
	ImGui::SameLine(150);
	if (ImGui::Button("LOAD"))
	{
		CGameObject* pLoadParticle = new CGameObject;
		LoadParticle(pLoadParticle);
		if (pLoadParticle->GetName() == L"")
		{
			delete pLoadParticle;
			return;
		}
		else
		{
			CSceneMgr::GetInst()->SpawnObject(pLoadParticle, L"BG");
			AddParticleCombo(pLoadParticle);
		}
	}
}

void ParticleTool::AddParticleCombo(CGameObject* _newparti)
{
	m_vecParticleSystem.push_back(_newparti);
	m_strvecParticleName.push_back(ToString(_newparti->GetName()));
}

void ParticleTool::MakeTargetParticleCombo()
{
	ImGui::Text("Change Target Particle : ");
	ImGui::SameLine();

	m_strvecParticleName = {};
	for (size_t i = 0; i < m_vecParticleSystem.size(); i++)
	{
		m_strvecParticleName.push_back(ToString(m_vecParticleSystem[i]->GetName()));
	}

	const char* combo_preview_value;
	if (0 == m_strvecParticleName.size())
	{
		combo_preview_value = "- Particle Is Not Existed-";

		ImGui::Text(combo_preview_value);
	}
	else
	{
		combo_preview_value = m_strvecParticleName[m_iParticleComboIDX].c_str();

		if (ImGui::BeginCombo("##SetTarget_ParticleCombo", combo_preview_value, 0))
		{
			for (int n = 0; n < m_vecParticleSystem.size(); n++)
			{
				const bool is_selected = (m_iParticleComboIDX == n);
				if (ImGui::Selectable(m_strvecParticleName[n].c_str(), is_selected))
				{
					m_iParticleComboIDX = n;
					m_pTargetParticle = m_vecParticleSystem[m_iParticleComboIDX];

					SetData();
				}

				if (is_selected)
					ImGui::SetItemDefaultFocus();
			}
			ImGui::EndCombo();
		}

		ParticleOptionSetting();
	}
}

void ParticleTool::update()
{
	if (nullptr == m_pParticleTarget || nullptr == m_pEmissiveTarget)
	{
		CMRT* pParticleMrt = CRenderMgr::GetInst()->GetMRT(MRT_TYPE::PARTICLE);

		m_pParticleTarget = pParticleMrt->GetArrRt(0);
		m_pEmissiveTarget = pParticleMrt->GetArrRt(1);

		m_vecTargetResolution.x = m_pParticleTarget.Get()->Width();
		m_vecTargetResolution.y = m_pParticleTarget.Get()->Height();
	}

	// 현재 파티클을 vector안에다 넣어주어야함.
	// 일단 현재 씬에서 가져옴. 후에는...

	if (0 == m_vecParticleSystem.size())
	{
		vector<CGameObject*> pVecBGObj = CSceneMgr::GetInst()->GetCurScene()->GetLayer(0)->GetObjects();

		for (size_t i = 0; i < pVecBGObj.size(); i++)
		{
			if (nullptr != pVecBGObj[i]->GetComponent(COMPONENT_TYPE::PARTICLESYSTEM))
			{
				if (false == pVecBGObj[i]->IsDead())
					m_vecParticleSystem.push_back(pVecBGObj[i]);
			}
		}
	}

	if (0 != m_vecParticleSystem.size())
	{
		if (nullptr == m_pTargetParticle)
		{
			m_pTargetParticle = m_vecParticleSystem[0];
		}
	}
}

void ParticleTool::render_update()
{
	// 필요한것
	// 1. particle target
	// 3. 저장하기
	// 4. 불러오기
	// 5. 파티클 여러개 저장하는 기능.

	SetData();

	ParticleCreate();
	ImGui::SameLine();
	ParticleErase();
	ParticleSaveNLoad();
	ImGui::Separator();

	ImVec2 pTargetImageSize;
	pTargetImageSize.x = m_vecTargetResolution.x * 1.f / 3.f;
	pTargetImageSize.y = m_vecTargetResolution.y * 1.f / 3.f;


	static const char* s_arrTargetTextureName[] = {
		"Particle Target",
		"Emissive Target",
		"Particle + Emissive Target",
	};
	static int TargetTextureIDX = 0;
	ImGui::Text("Set Particle Target");
	ImGui::SameLine();
	if (ImGui::BeginCombo("##PARTICLE_TARGET_COMBO", s_arrTargetTextureName[TargetTextureIDX]))
	{
		for (size_t i = 0; i < std::size(s_arrTargetTextureName); ++i)
		{
			if (ImGui::Selectable(s_arrTargetTextureName[i]))
			{
				TargetTextureIDX = static_cast<int>(i);
			}
		}
		ImGui::EndCombo();
	}

	if (0 == TargetTextureIDX)
	{
		ImGui::Image(m_pParticleTarget.Get()->GetSRV().Get(),
			pTargetImageSize,
			ImVec2(0, 0),
			ImVec2(1, 1),
			ImVec4(1, 1, 1, 1),
			ImVec4(0, 0, 0, 0));
	}
	else if (1 == TargetTextureIDX)
	{
		ImGui::Image(m_pEmissiveTarget.Get()->GetSRV().Get(),
			pTargetImageSize,
			ImVec2(0, 0),
			ImVec2(1, 1),
			ImVec4(1, 1, 1, 1),
			ImVec4(0, 0, 0, 0));
	}
	else if (2 == TargetTextureIDX) {}


	if (nullptr == m_pTargetParticle)
	{
		return;
	}
	else
	{
		ImGui::Text("Cur Target Particle : ");
		ImGui::SameLine();
		ImGui::Text(ToString(m_pTargetParticle->GetName()).c_str());
	}

	MakeTargetParticleCombo();
}
