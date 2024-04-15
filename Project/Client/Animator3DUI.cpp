#include "pch.h"
#include "Animator3DUI.h"

#include <fstream>
#include <sstream>


#include <Engine/CAnimation3D.h>
#include <Engine/CAnimator3D.h>
#include <Engine/CPathMgr.h>


#include "CImGuiMgr.h"
#include "IconsFontAwesome5.h"

#include <Script/SocketColliderScript.h>



Animator3DUI::Animator3DUI()
	: ComponentUI("Animator3D", COMPONENT_TYPE::ANIMATOR3D)
	, m_bPlay(false)
	, m_fCommonLerpTime(1.f)

{
	SetSize(Vec2(0.f, 150.f));
	m_FrameTimeLine.m_FrameMin = 0;
	m_FrameTimeLine.m_FrameMax = 0;
}

Animator3DUI::~Animator3DUI()
{
}

void Animator3DUI::update()
{
	ComponentUI::update();
}

void Animator3DUI::render_update()
{
	ComponentUI::render_update();
	if (ComponentUI::IsFold())
	{
		SetSize(Vec2(0.f, 25.f));
		return;
	}
	else { SetSize(Vec2(0.f, 150.f)); }

	if (m_FrameTimeLine.GetOwner() == nullptr)
		m_FrameTimeLine.SetOwner(this);

	CGameObject* pTargetObject = GetTargetObject();
	CAnimator3D* pAnimator3D = pTargetObject->Animator3D();
	// 애니메이션3D 를 보유한 객체를  처음 선택했을 때 갱신 
	if (pAnimator3D != m_pAnimator3D)
	{
		m_pAnimator3D = pAnimator3D;
		m_FrameTimeLine.m_pCurAnimator3D = m_pAnimator3D;
		m_FrameTimeLine.Reset();
		Reset((DWORD_PTR)nullptr);
	}
	m_pCurAnim3D = m_pAnimator3D->GetCurAnim();
	const map<wstring, CAnimation3D*> MapAnim = m_pAnimator3D->GetAllAnim();

	if (MapAnim.size() != m_FrameTimeLine.m_vecAnimItem.size())
	{
		Reset((DWORD_PTR)nullptr);
	}

	RenderAnim3DClipWindow();
	RenderComponentWindow();
}

void Animator3DUI::RenderComponentWindow()
{
	// 1. All Animation Same Lerp Time 
	const map<wstring, CAnimation3D*> MapAnim = m_pAnimator3D->GetAllAnim();
	string                            ButtonName = ICON_FA_CIRCLE_DOWN;
	//ButtonName += " LerpTime";
	if (ImGui::Button(ButtonName.c_str(), ImVec2(20.f, 20.f)))
		ImGui::OpenPopup("Apply All LerpTime");

	if (ImGui::BeginPopupModal("Apply All LerpTime", NULL, ImGuiWindowFlags_AlwaysAutoResize))
	{
		ImGui::TextColored(ImVec4(1.f, 1.f, 1.f, 1.f)
			, u8"모든 애니메이션에 [ %.3f ] 보간시간이 적용됩니다. \n\n", m_fCommonLerpTime);

		if (ImGui::Button("OK", ImVec2(100, 0)))
		{
			// 전체 애니메이션에 공통 보간시간 적용 
			m_pAnimator3D->SetLerpTimeOnAllAnim(m_fCommonLerpTime);
			ImGui::CloseCurrentPopup();
		}
		ImGui::SetItemDefaultFocus();
		ImGui::SameLine();
		if (ImGui::Button("Cancel", ImVec2(120, 0)))
		{
			ImGui::CloseCurrentPopup();
		}

		ImGui::EndPopup();
	}
	ImGui::SameLine();
	ImGui::PushItemWidth(125.f);
	ImGui::DragFloat("##LerpTimeSetting", &m_fCommonLerpTime, 0.1f, 0.f, 100.f);
	ImGui::PopItemWidth();

	ImGui::SameLine();
	ImGui::Text(" LerpTime On All Anim");


	// 2. Clear All List
	ButtonName = ICON_FA_BROOM;
	//ButtonName += " Clear";
	if (ImGui::Button(ButtonName.c_str(), ImVec2(20.f, 20.f)))
		ImGui::OpenPopup("Clear All Animation");

	ImGui::SameLine();
	ImGui::Text("Clear All Animation");

	if (ImGui::BeginPopupModal("Clear All Animation", NULL, ImGuiWindowFlags_AlwaysAutoResize))
	{
		ImGui::TextColored(ImVec4(1.f, 0.f, 0.f, 1.f)
			, u8"모든 애니메이션을 삭제하시겠습니까?\n\n");

		if (ImGui::Button("OK", ImVec2(120, 0)))
		{
			Clear();
			ImGui::CloseCurrentPopup();
		}
		ImGui::SetItemDefaultFocus();
		ImGui::SameLine();
		if (ImGui::Button("Cancel", ImVec2(120, 0)))
		{
			ImGui::CloseCurrentPopup();
		}

		ImGui::EndPopup();
	}

	// 3. play all Animation list 
	static bool bPlayAllList = false;
	if (bPlayAllList)
		ButtonName = ICON_FA_STOP;
	else
		ButtonName = ICON_FA_PLAY;

	ButtonName += " Play All Anim";
	ImGui::Checkbox(ButtonName.c_str(), &bPlayAllList);
	if (bPlayAllList)
	{
		m_pAnimator3D->SetRepeat(false);

		if (m_pCurAnim3D != nullptr)
		{
			if (m_pCurAnim3D->IsFinish())
			{
				CAnimation3D* pNextAnim3D = GetNextAnim();
				if (pNextAnim3D != nullptr)
				{
					m_pAnimator3D->Play(pNextAnim3D->GetName(), false);
				}
			}
		}
	}

	ButtonName = ICON_FA_CHILD;
	//ButtonName += "Apply To Child Object";
	if (ImGui::Button(ButtonName.c_str(), ImVec2(20.f, 20.f)))
		ImGui::OpenPopup("Apply To Child Object");

	ImGui::SameLine();
	ImGui::Text("Apply To Child Object");


	if (ImGui::BeginPopupModal("Apply To Child Object", NULL, ImGuiWindowFlags_Modal))
	{
		wstring wstrName = GetTargetObject()->GetName();
		string  name = ToString(wstrName);
		ImGui::TextWrapped(
			u8"하위 객체에 동일한 애니메이션을 적용합니다.\n\n"
			u8"하위 객체의 개별 애니메이션은 사라지며\n\n"
			u8"%s 객체의 애니메이션과 동일한 애니메이션을 갖게됩니다.", name.c_str());

		if (ImGui::Button("OK", ImVec2(120, 0)))
		{
			ApplyAnim3DToChild(GetTargetObject());
			ImGui::CloseCurrentPopup();
		}
		ImGui::SetItemDefaultFocus();
		ImGui::SameLine();
		if (ImGui::Button("Cancel", ImVec2(120, 0)))
		{
			ImGui::CloseCurrentPopup();
		}

		ImGui::EndPopup();
	}


	// 4. Play With Child Animator3D
	bool bPlayWithChild = m_pAnimator3D->GetPlayWithChild();
	ButtonName = ICON_FA_CHILD;
	ButtonName += " Play With Child";
	ImGui::Checkbox(ButtonName.c_str(), &bPlayWithChild);
	m_pAnimator3D->SetPlayWithChild(bPlayWithChild);



}


void Animator3DUI::RenderAnim3DClipWindow()
{
	const map<wstring, CAnimation3D*> MapAnim = m_pAnimator3D->GetAllAnim();

	// [ TIME LINE 변수들 ] 
	static int  firstFrame = 0;
	static bool expanded = true;


	ImGui::Begin("ANIMATION_3D CLIP");

	// [ PLAY/STOP UP_SPEED DOWN_SPEED BUTTON ]
	if (m_pCurAnim3D != nullptr)
	{
		float fSpeed = m_pCurAnim3D->GetSpeed();
		if (ImGui::Button(ICON_FA_ARROW_DOWN))
		{
			fSpeed -= 0.1f;
			if (fSpeed <= 0.f)
				fSpeed = 0.f;
			//m_pCurAnim3D->SetSpeed(fSpeed);
			m_pAnimator3D->SetSpeed(fSpeed);
		}

		ImGui::SameLine();
		ImGui::PushItemWidth(100.f);
		if (m_pCurAnim3D->IsPlay())
		{
			string ButtonName = "   ";
			ButtonName += ICON_FA_PAUSE;
			ButtonName += "  ";

			if (ImGui::Button(ButtonName.c_str()))
			{
				//m_pCurAnim3D->Play(false);
				m_pAnimator3D->SetAnimState(m_pCurAnim3D->GetName(), ANIMATION_STATE::STOP);
			}
		}
		else
		{
			string ButtonName = "   ";
			ButtonName += ICON_FA_PLAY;
			ButtonName += "  ";
			if (ImGui::Button(ButtonName.c_str()))
			{
				m_pAnimator3D->SetAnimState(m_pCurAnim3D->GetName(), ANIMATION_STATE::PLAY);
				//m_pCurAnim3D->Play(true);
				//m_pCurAnim3D->SetFinish(false);
			}
		}
		ImGui::PopItemWidth();

		ImGui::SameLine();
		if (ImGui::Button(ICON_FA_ARROW_UP))
		{
			fSpeed += 0.1f;
			if (fSpeed >= 3.f)
				fSpeed = 3.f;
			//m_pCurAnim3D->SetSpeed(fSpeed);
			m_pAnimator3D->SetSpeed(fSpeed);
		}

		ImGui::SameLine();
		bool bRepeatAnim = m_pAnimator3D->GetRepeat();
		if (ImGui::Checkbox(ICON_FA_REPEAT, &bRepeatAnim))
		{
			m_pAnimator3D->SetRepeat(bRepeatAnim);
		}

		ImGui::SameLine();
	}

	// [ CREATE NEW ANIMATION ]
	ImGui::SameLine();
	if (ImGui::Button("Create"))
		ImGui::OpenPopup("Create Animation");

	if (ImGui::BeginPopupModal("Create Animation", NULL, ImGuiWindowFlags_AlwaysAutoResize))
	{
		CREATE_ANIMATION_MODE eMode = m_pAnimator3D->GetCreateMode();
		string                ButtonName;
		if (eMode == CREATE_ANIMATION_MODE::FRAME)
		{
			ButtonName = "FRAME";
		}
		else if (eMode == CREATE_ANIMATION_MODE::TIME)
		{
			ButtonName = "TIME";
		}
		if (ImGui::Button(ButtonName.c_str(), ImVec2(75.f, 0.f)))
		{
			if (eMode == CREATE_ANIMATION_MODE::FRAME)
				eMode = CREATE_ANIMATION_MODE::TIME;
			else if (eMode == CREATE_ANIMATION_MODE::TIME)
				eMode = CREATE_ANIMATION_MODE::FRAME;
		}
		m_pAnimator3D->SetCreateMode(eMode);

		static int FrameStartIdx = 0;
		static int FrameEndIdx = 0;

		static double FrameStartTime = 0.0;
		static double FrameEndTime = 0.0;
		if (eMode == CREATE_ANIMATION_MODE::FRAME)
		{
			ImGui::Text("Frame");
			ImGui::PushItemWidth(100.f);
			ImGui::SameLine();
			ImGui::InputInt("##Start", &FrameStartIdx);
			ImGui::SameLine();
			ImGui::Text(" ~ ");
			ImGui::SameLine();
			ImGui::InputInt("##End", &FrameEndIdx);
			ImGui::PopItemWidth();
		}
		if (eMode == CREATE_ANIMATION_MODE::TIME)
		{
			ImGui::Text("Time");
			ImGui::PushItemWidth(100.f);
			ImGui::SameLine();
			ImGui::InputDouble("##Start", &FrameStartTime);
			ImGui::SameLine();
			ImGui::Text(" ~ ");
			ImGui::SameLine();
			ImGui::InputDouble("##End", &FrameEndTime);
			ImGui::PopItemWidth();
		}

		static char buf[512];
		ImGui::InputText("Name", buf, IM_ARRAYSIZE(buf), ImGuiInputTextFlags_None);

		if (ImGui::Button("OK", ImVec2(120, 0)))
		{
			string strName = buf;
			// [ NEW ANIMATION ] 
			wstring wstrName = wstring(strName.begin(), strName.end());
			if (MapAnim.find(wstrName) == MapAnim.end())
			{
				// 타임라인에 적용
				m_FrameTimeLine.AddAnimName(strName);
				int idx = (int)(m_FrameTimeLine.m_vecAnimName.size() - 1);
				int frameCnt = m_pAnimator3D->GetFrameCount();
				if (eMode == CREATE_ANIMATION_MODE::FRAME)
				{
					m_FrameTimeLine.m_vecAnimItem.push_back(AnimItem{ strName, idx, FrameStartIdx, FrameEndIdx });
					m_pAnimator3D->CreateAnimByFrame(wstrName, 0, FrameStartIdx, FrameEndIdx);
				}
				else if (eMode == CREATE_ANIMATION_MODE::TIME)
				{
					int sIdx = (int)(FrameStartTime * frameCnt);
					int eIdx = (int)(FrameEndTime * frameCnt);
					m_FrameTimeLine.m_vecAnimItem.push_back(AnimItem{ strName, idx, FrameStartIdx, FrameEndIdx });
					m_pAnimator3D->CreateAnimByTime(wstrName, 0, FrameStartTime, FrameEndTime);
				}

				m_pAnimator3D->Play(wstrName, false);
				m_pCurAnim3D = m_pAnimator3D->GetCurAnim();
				m_iSelectedIdx = idx;
			}
			FrameStartIdx = 0;
			FrameEndIdx = 0;
			FrameStartTime = 0.0;
			FrameEndTime = 0.0;

			buf[0] = NULL;
			ImGui::CloseCurrentPopup();
		}
		ImGui::SetItemDefaultFocus();
		ImGui::SameLine();
		if (ImGui::Button("Cancel", ImVec2(120, 0)))
		{
			FrameStartIdx = 0;
			FrameEndIdx = 0;
			FrameStartTime = 0.0;
			FrameEndTime = 0.0;

			buf[0] = NULL;
			ImGui::CloseCurrentPopup();
		}

		ImGui::EndPopup();
	}


	// [ CREATE NEW ANIMATION - LOAD FROM .txt File ]
	ImGui::SameLine();
	if (ImGui::Button("Load"))
		ImGui::OpenPopup("Load Animation");

	if (ImGui::BeginPopupModal("Load Animation", NULL, ImGuiWindowFlags_AlwaysAutoResize))
	{
		static char buf[512];
		ImGui::InputText("txt File Path", buf, IM_ARRAYSIZE(buf), ImGuiInputTextFlags_None);
		if (ImGui::Button("OK", ImVec2(120, 0)))
		{
			string FileName = "AnimationInfo\\";
			FileName += buf;
			wstring ContentPath = CPathMgr::GetInst()->GetContentPath();
			string  strPath = ToString(ContentPath);
			strPath += FileName;

			// 파일 입출력 
			std::ifstream FileStream(strPath);
			if (FileStream.is_open())
			{
				string strLine;
				while (std::getline(FileStream, strLine))
				{
					vector<string>    vecWords;
					string            strWord;
					std::stringstream SStream(strLine);
					while (std::getline(SStream, strWord, ' '))			// 공백에 따라 단어 분리 
					{
						vecWords.push_back(strWord);
					}
					string AnimName = vecWords[0];						// 1. 애니메이션 이름 
					int    FrameStartIdx = std::stoi(vecWords[1]);		// 2. 시작 프레임 인덱스 
					int    FrameEndIdx = std::stoi(vecWords[2]);		// 3. 끝   프레임 인덱스 


					wstring wstrName = wstring(AnimName.begin(), AnimName.end());
					if (MapAnim.find(wstrName) == MapAnim.end())
					{
						// 타임라인에 적용
						m_FrameTimeLine.AddAnimName(AnimName);
						int idx = (int)(m_FrameTimeLine.m_vecAnimName.size() - 1);
						m_FrameTimeLine.m_vecAnimItem.push_back(AnimItem{ AnimName, idx, FrameStartIdx, FrameEndIdx });

						// 컴포넌트에 적용 
						int    FrameCnt = m_pAnimator3D->GetFrameCount();
						double FrameStartTime = (double)FrameStartIdx / (double)FrameCnt;
						double FrameEndTime = (double)FrameEndIdx / (double)FrameCnt;

						m_pAnimator3D->CreateAnimByFrame(wstrName, 0, FrameStartIdx, FrameEndIdx);
						m_pAnimator3D->Play(wstrName, false);
						m_pCurAnim3D = m_pAnimator3D->GetCurAnim();
						m_iSelectedIdx = idx;
					}
				}

				FileStream.close();
			}
			buf[0] = NULL;
			ImGui::CloseCurrentPopup();
		}
		ImGui::SetItemDefaultFocus();
		ImGui::SameLine();
		if (ImGui::Button("Cancel", ImVec2(120, 0)))
		{
			buf[0] = NULL;
			ImGui::CloseCurrentPopup();
		}
		ImGui::EndPopup();
	}


	if (m_pCurAnim3D != nullptr)
	{
		ImGui::SameLine();
		if (ImGui::Button(ICON_FA_TRASH))
			ImGui::OpenPopup("Delete Animation");

		if (ImGui::BeginPopupModal("Delete Animation", NULL, ImGuiWindowFlags_AlwaysAutoResize))
		{
			wstring wstrName = m_pCurAnim3D->GetName();
			string  strName = ToString(wstrName);

			ImGui::TextColored(ImVec4(1.f, 0.f, 0.f, 1.f)
				, u8"[ %s ]\n 애니메이션을 삭제하시겠습니까?\n\n", strName.c_str());

			if (ImGui::Button("OK", ImVec2(120, 0)))
			{
				wstring wstrName = m_pCurAnim3D->GetName();
				string  strName = ToString(wstrName);

				// 1. 타임라인 정리 
				m_FrameTimeLine.SubAnimName(strName);
				m_FrameTimeLine.SubAnimItem(strName);

				// 2. 컴포넌트에서 정리 
				m_pAnimator3D->DeleteAnim(wstrName);
				m_iSelectedIdx = -1;
				m_iSelectedPrevIdx = -1;
				m_pCurAnim3D = nullptr;

				// 3. 타임라인 리셋
				m_FrameTimeLine.Reset();

				ImGui::CloseCurrentPopup();
			}
			ImGui::SetItemDefaultFocus();
			ImGui::SameLine();
			if (ImGui::Button("Cancel", ImVec2(120, 0)))
			{
				ImGui::CloseCurrentPopup();
			}

			ImGui::EndPopup();
		}
	}


	// [ CHANGE TIME / FRAME ]
	ImGui::Text("TImeLine (s)");
	ImGui::SameLine(100.f);

	if (m_pCurAnim3D != nullptr)
	{
		double StartTime = m_pCurAnim3D->GetStartTime(); // double 인데 float 으로 받아서 값이 조금 달라짐 
		float  StartDiffTime = (float)StartTime;
		ImGui::PushItemWidth(60);
		ImGui::DragFloat("##START_drag", &StartDiffTime, 0.01f);
		ImGui::PopItemWidth();
		StartTime = (double)StartDiffTime;
		m_pCurAnim3D->SetStartTime(StartTime);


		ImGui::SameLine();
		ImGui::Text("  ~  ");
		ImGui::SameLine();

		double EndTime = m_pCurAnim3D->GetEndTime();
		float  EndDiffTime = (float)EndTime;
		ImGui::PushItemWidth(60);
		ImGui::DragFloat("##END_drag", &EndDiffTime, 0.01f);
		ImGui::PopItemWidth();
		EndTime = (double)EndDiffTime;
		m_pCurAnim3D->SetEndTime(EndTime);

		ImGui::SameLine(270.f);
		ImGui::Text("Frame Range ");
		ImGui::SameLine(350.f);

		int StartFrameIdx = m_pCurAnim3D->GetStartFrameIdx();
		ImGui::PushItemWidth(60);
		ImGui::DragInt("##STARTIdx_drag", &StartFrameIdx);
		ImGui::PopItemWidth();
		m_pCurAnim3D->SetStartFrameIdx(StartFrameIdx);


		ImGui::SameLine();
		ImGui::Text("  ~  ");
		ImGui::SameLine();

		int EndFrameIdx = m_pCurAnim3D->GetEndFrameIdx();
		ImGui::PushItemWidth(60);
		ImGui::DragInt("##ENDIdx_drag", &EndFrameIdx);
		ImGui::PopItemWidth();
		m_pCurAnim3D->SetEndFrameIdx(EndFrameIdx);
	}


	// [ CURRENT FRAME ]
	m_FrameTimeLine.m_FrameMin = 0;
	m_FrameTimeLine.m_FrameMax = m_pAnimator3D->GetAnimClip(0).iFrameLength - 1;
	if (m_pCurAnim3D != nullptr)
		m_iCurFrameUI = m_pCurAnim3D->GetCurFrameIdx();
	else
		m_iCurFrameUI = 0;
	ImGui::Text("Frame");
	ImGui::SameLine(100.f);
	ImGui::SameLine();
	ImGui::Text("Min : %d - ", m_FrameTimeLine.m_FrameMin);
	ImGui::SameLine();

	ImGui::SameLine();
	if (ImGui::Button(ICON_FA_LEFT_LONG))
		m_iCurFrameUI -= 1;
	ImGui::SameLine();
	ImGui::PushItemWidth(130);
	ImGui::DragInt("##FrameDrag", &m_iCurFrameUI);
	ImGui::SameLine();
	ImGui::PopItemWidth();
	ImGui::SameLine();
	if (ImGui::Button(ICON_FA_RIGHT_LONG))
		m_iCurFrameUI += 1;
	if (m_pCurAnim3D != nullptr)
		m_pCurAnim3D->SetCurFrameIdx(m_iCurFrameUI);
	ImGui::SameLine();
	ImGui::Text("- Max : %d", m_FrameTimeLine.m_FrameMax);

	ImGui::SameLine();
	if (m_pCurAnim3D != nullptr)
		ImGui::TextColored(ImVec4(1.f, 155.f / 255.f, 0.f, 1.f), "  x%.2f", m_pCurAnim3D->GetSpeed());
	float FRMTime = (float)m_iCurFrameUI / (float)m_pAnimator3D->GetFrameCount();
	ImGui::SameLine();
	ImGui::TextColored(ImVec4(1.f, 155.f / 255.f, 0.f, 1.f), " [ %.2f (s) ]", FRMTime);

	if (m_pCurAnim3D != nullptr)
	{
		ImGui::SameLine(550.f);
		ImGui::PushItemWidth(100);
		float lerpTime = m_pCurAnim3D->GetLerpTime();
		ImGui::DragFloat("LerpTime", &lerpTime, 0.1f, 0.f, 100.f);
		//m_pCurAnim3D->SetLerpTime(lerpTime);
		m_pAnimator3D->SetLerpTimeOnAnim(m_pCurAnim3D->GetName(), lerpTime);

		ImGui::PopItemWidth();

		ANIMATION_STATE eState = m_pCurAnim3D->GetState();
		if (eState == ANIMATION_STATE::BEFORE_PLAY)
		{
			// [ 보간 시간 도중에 초록 원을 띄운다 ]
			ImGui::SameLine();
			ImGui::PushStyleColor(0, ImVec4(0.f / 255.f, 250.f / 255.f, 50.f / 255.f, 1.f));
			ImGui::Bullet();
			ImGui::SameLine();
			double LerpTimer = m_pCurAnim3D->GetAccTime();
			ImGui::TextColored(ImVec4(1.f, 155.f / 255.f, 0.f, 1.f), " [ %.2f (s) ]", (float)LerpTimer);

			ImGui::PopStyleColor();
		}
	}


	// [ SEQUENCER - TIMELINE ]
	//ImGui::BeginChild("Timeline", ImVec2(0, 200), false);

	Sequencer(&m_FrameTimeLine, &m_iCurFrameUI, &expanded, &m_iSelectedIdx, &firstFrame
		, ImSequencer::SEQUENCER_EDIT_STARTEND |
		ImSequencer::SEQUENCER_CHANGE_FRAME);
	// ImSequencer::SEQUENCER_DEL | ImSequencer::SEQUENCER_COPYPASTE |ImSequencer::SEQUENCER_ADD |


	// [ EDIT SELECT ENTRY ]
	if (m_iSelectedIdx != -1)
	{
		const AnimItem& item = m_FrameTimeLine.m_vecAnimItem[m_iSelectedIdx];
		string          SelectedEntryName = m_FrameTimeLine.m_vecAnimName[m_iSelectedIdx].c_str();
		wstring         wstrSelEntryName = wstring(SelectedEntryName.begin(), SelectedEntryName.end());

		// 처음 선택 했을 때 
		if (m_iSelectedIdx != m_iSelectedPrevIdx)
		{
			m_pAnimator3D->Play(wstrSelEntryName, true);
			m_iSelectedPrevIdx = m_iSelectedIdx;

			CAnimation3D* pAnim = m_pAnimator3D->FindAnim(wstrSelEntryName);
			if (pAnim != nullptr)
			{
				m_FrameTimeLine.m_vecAnimItem[m_iSelectedIdx].mFrameStart = pAnim->GetStartFrameIdx();
				m_FrameTimeLine.m_vecAnimItem[m_iSelectedIdx].mFrameEnd = pAnim->GetEndFrameIdx();
			}
		}
	}

	// 실시간 변경점을 적용한다.
	if (m_iSelectedIdx != -1)
	{
		if (m_pCurAnim3D != nullptr)
		{
			m_FrameTimeLine.m_vecAnimItem[m_iSelectedIdx].mFrameStart = m_pCurAnim3D->GetStartFrameIdx();
			m_FrameTimeLine.m_vecAnimItem[m_iSelectedIdx].mFrameEnd = m_pCurAnim3D->GetEndFrameIdx();
		}

		//pCurAnim->SetStartFrameIdx(m_FrameTimeLine.m_vecAnimItem[selectedEntry].mFrameStart);
		//pCurAnim->SetEndFrameIdx(m_FrameTimeLine.m_vecAnimItem[selectedEntry].mFrameEnd);
	}
	//ImGui::EndChild();

	ImGui::End();
}

void Animator3DUI::Clear()
{
	m_pAnimator3D->Clear();
	m_FrameTimeLine.ClearAnimItem();
	m_FrameTimeLine.ClearAnimName();
	m_pCurAnim3D = nullptr;
	m_iSelectedIdx = -1;
	m_iSelectedPrevIdx = -1;
	m_iCurFrameUI = -1;
}

void Animator3DUI::Reset(DWORD_PTR _ptr)
{
	m_FrameTimeLine.m_vecAnimName.clear();
	m_FrameTimeLine.m_vecAnimItem.clear();

	CGameObject* pTargetObject = GetTargetObject();
	CAnimator3D* pAnimator3D = pTargetObject->Animator3D();
	const map<wstring, CAnimation3D*> MapAnim = pAnimator3D->GetAllAnim();

	int                                         idx = 0;
	map<wstring, CAnimation3D*>::const_iterator iter = MapAnim.begin();
	for (; iter != MapAnim.end(); ++iter)
	{
		wstring       AnimName = iter->first;
		CAnimation3D* pAnim = iter->second;

		string name = ToString(AnimName);
		int    StartFrameIdx = pAnim->GetStartFrameIdx();
		int    EndFrameIdx = pAnim->GetEndFrameIdx();

		m_FrameTimeLine.m_vecAnimName.push_back(name);
		m_FrameTimeLine.m_vecAnimItem.push_back(AnimItem{ name, idx++, StartFrameIdx, EndFrameIdx });
	}

	m_iSelectedIdx = -1;
	m_iSelectedPrevIdx = -1;
	m_iCurFrameUI = 0;

	return;
}


CAnimation3D* Animator3DUI::GetNextAnim()
{
	CAnimation3D* pNextAnim = nullptr;
	const map<wstring, CAnimation3D*> MapAnim = m_pAnimator3D->GetAllAnim();
	if (m_pCurAnim3D == nullptr && MapAnim.size() != 0)
		return MapAnim.begin()->second;

	map<wstring, CAnimation3D*>::const_iterator iter = MapAnim.begin();

	for (; iter != MapAnim.end(); ++iter)
	{
		if (m_pCurAnim3D != nullptr)
		{
			if (m_pCurAnim3D == iter->second)
			{
				map<wstring, CAnimation3D*>::const_iterator checkIter = iter;
				checkIter++;
				// 현재 애니메이션의 다음 애니메이션이 있는 경우 ( 처음 ~ 중간 )
				if (checkIter != MapAnim.end())
				{
					pNextAnim = checkIter->second;
					break;
				}
				// 현재 애니메이션의 다음 애니메이션이 없는 경우 ( 끝 )
				else
				{
					pNextAnim = MapAnim.begin()->second;
					break;
				}
			}
		}
	}

	if (pNextAnim != nullptr)
	{
		wstring wstrName = pNextAnim->GetName();
		string  strName = ToString(wstrName);

		m_iSelectedIdx = m_FrameTimeLine.GetAnimNameIndex(strName);
		m_iSelectedPrevIdx = m_iSelectedIdx;
	}
	return pNextAnim;
}

void Animator3DUI::ApplyAnim3DToChild(CGameObject* pObj)
{
	CAnimator3D* pAnimator3D = pObj->Animator3D();
	if (pAnimator3D == nullptr)
		return;

	// 1. 애니메이션 복사
	const map<wstring, CAnimation3D*> MapAnim = pAnimator3D->GetAllAnim();

	// 2. 하위 객체에도 적용 
	vector<CGameObject*> vecChild = pObj->GetChild();
	for (int i = 0; i < vecChild.size(); ++i)
	{
		CAnimator3D* pA3D = vecChild[i]->Animator3D();
		if (pA3D == nullptr)
			continue;
		pA3D->CopyAllAnim(MapAnim);
		ApplyAnim3DToChild(vecChild[i]);
	}
}
