#include "pch.h"
#include <fstream>

#include "CAnimator3D.h"
#include "CAnimation3D.h"

#include "CMeshRender.h"
#include "CStructuredBuffer.h"
#include "CMesh.h"
#include "CGameObject.h"
#include "CComponent.h"
#include "CSerializer.h"

namespace
{
	constexpr int DEFAULT_TARGET_ANIMATION_FRAME_RATE = 30;
}

CAnimator3D::CAnimator3D()
	: CComponent(COMPONENT_TYPE::ANIMATOR3D)
	, m_pVecBones(nullptr)
	, m_pVecClip(nullptr)
	, m_iFrameCount(DEFAULT_TARGET_ANIMATION_FRAME_RATE)
	, m_pBoneFinalMatBuffer(nullptr)
	, m_bUseSocket{false}
	, m_pPrevAnim(nullptr)
	, m_pCurAnim(nullptr)
	, m_pNextAnim(nullptr)
	, m_bRepeat(false)
	, m_bPlayWithChild{false}
	, m_eCreateMode(CREATE_ANIMATION_MODE::FRAME)
{
	m_pBoneFinalMatBuffer = new CStructuredBuffer{};
	m_pSocketBuffer       = new CStructuredBuffer{};
}

CAnimator3D::CAnimator3D(const CAnimator3D& _origin)
	: CComponent(COMPONENT_TYPE::ANIMATOR3D)
	, m_pVecBones(_origin.m_pVecBones)
	, m_pVecClip(_origin.m_pVecClip)
	, m_iFrameCount(_origin.m_iFrameCount)
	, m_pBoneFinalMatBuffer(nullptr)
	, m_bUseSocket{_origin.m_bUseSocket}
	, m_bRepeat(_origin.m_bRepeat)
	, m_bPlayWithChild{_origin.m_bPlayWithChild}
	, m_eCreateMode{_origin.m_eCreateMode}
{
	m_pBoneFinalMatBuffer = new CStructuredBuffer{};
	m_pSocketBuffer       = new CStructuredBuffer{};

	for (const auto& [animationName, pAnimation] : _origin.m_mapAnim)
	{
		CAnimation3D* pCloneAnimation = pAnimation->Clone();
		pCloneAnimation->SetName(animationName);
		pCloneAnimation->SetOwner(this);
		m_mapAnim.insert({animationName, pCloneAnimation});
	}
	m_pPrevAnim = _origin.m_pPrevAnim ? m_mapAnim.find(_origin.m_pPrevAnim->GetName())->second : nullptr;
	m_pCurAnim  = _origin.m_pCurAnim ? m_mapAnim.find(_origin.m_pCurAnim->GetName())->second : nullptr;
	m_pNextAnim = _origin.m_pNextAnim ? m_mapAnim.find(_origin.m_pNextAnim->GetName())->second : nullptr;
}

CAnimator3D::~CAnimator3D()
{
	SAFE_DELETE(m_pBoneFinalMatBuffer);
	Safe_Del_Map(m_mapAnim);
	SAFE_DELETE(m_pSocketBuffer);
}

void CAnimator3D::finalupdate()
{
	if (nullptr == m_pCurAnim || false == m_pCurAnim->IsPlay())
	{
		return;
	}

	m_pCurAnim->finalupdate();

	if (m_pCurAnim->IsFinish() && m_bRepeat)
	{
		m_pCurAnim->Reset();
	}
}

void CAnimator3D::SetAnimClip(const vector<tMTAnimClip>* _vecAnimClip)
{
	m_pVecClip = _vecAnimClip;
}

void CAnimator3D::UpdateData()
{
	if (m_pCurAnim != nullptr)
	{
		m_pCurAnim->UpdateData();
		if (m_bUseSocket)
		{
			//계산 셰이더에서 계산한 결과를 CPU쪽으로 들고옴 (GPU->CPU)
			m_pSocketBuffer->GetData(m_vecSocketMatrix.data());
		}
	}
}

void CAnimator3D::ClearData()
{
	if (m_pCurAnim != nullptr)
	{
		m_pCurAnim->ClearData();
	}
}

CAnimation3D* CAnimator3D::FindAnim(const wstring& _strName)
{
	const auto iter = m_mapAnim.find(_strName);
	if (iter == m_mapAnim.end())
	{
		return nullptr;
	}
	return iter->second;
}

void CAnimator3D::CopyAllAnim(const map<wstring, CAnimation3D*> _mapAnim)
{
	Clear();
	for (auto& [animationName,pAnimation] : _mapAnim)
	{
		if (m_eCreateMode == CREATE_ANIMATION_MODE::FRAME)
		{
			CreateAnimByFrame(pAnimation->GetName(),
			                  pAnimation->GetClipNum(),
			                  pAnimation->GetStartFrameIdx(),
			                  pAnimation->GetEndFrameIdx()
			                 );
		}
		else if (m_eCreateMode == CREATE_ANIMATION_MODE::TIME)
		{
			CreateAnimByTime(pAnimation->GetName(),
			                 pAnimation->GetClipNum(),
			                 pAnimation->GetStartTime(),
			                 pAnimation->GetEndTime()
			                );
		}
	}
}

CAnimation3D* CAnimator3D::CreateAnimByFrame(const wstring& _strName, int _clipNum, int _startFrame, int _EndFrame)
{
	if (FindAnim(_strName) != nullptr)
	{
		LOG_WARN("CreateAnimByFrame : ANIMATION ALREADY EXISTS!");
		return nullptr;
	}

	CAnimation3D* pAnim = new CAnimation3D{};
	pAnim->SetName(_strName);
	pAnim->SetOwner(this);
	pAnim->SetClipUpdateTime((int)m_pVecClip->size());
	pAnim->SetFrameInfoByFrame(_clipNum, _startFrame, _EndFrame);

	m_mapAnim.insert(make_pair(_strName, pAnim));

	if (m_bPlayWithChild)
	{
		for (auto& pChild : GetOwner()->GetChild())
		{
			CAnimator3D* pChildAnimator3D = pChild->Animator3D();
			if (pChildAnimator3D != nullptr)
			{
				pChildAnimator3D->CreateAnimByFrame(_strName, _clipNum, _startFrame, _EndFrame);
			}
		}
	}

	return pAnim;
}

void CAnimator3D::MakeAnimationFromTXT(string _txtName)
{
	const std::string FileName = "AnimationInfo\\" + _txtName;
	const std::string strPath  = ToString(CPathMgr::GetInst()->GetContentPath()) + FileName;

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

			if (vecWords.empty() ||
			    vecWords[0] == "\n" ||
			    vecWords[0] == " " ||
			    vecWords[0] == "**" ||
			    vecWords[0] == "*")
			{
				continue;
			}

			string AnimName      = vecWords[0];						// 1. 애니메이션 이름 
			int    FrameStartIdx = std::stoi(vecWords[1]);		// 2. 시작 프레임 인덱스 
			int    FrameEndIdx   = std::stoi(vecWords[2]);		// 3. 끝   프레임 인덱스 

			wstring wstrName = ToWString(AnimName);
			if (m_mapAnim.find(wstrName) == m_mapAnim.end())
			{
				// 컴포넌트에 적용 
				CreateAnimByFrame(wstrName, 0, FrameStartIdx, FrameEndIdx);
			}
		}
	}
}

void CAnimator3D::MakeAnimationFromTXT_Extended(string _txtName)
{
	const std::string FileName = "AnimationInfo\\" + _txtName;
	const std::string strPath  = ToString(CPathMgr::GetInst()->GetContentPath()) + FileName;

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

			if (vecWords.empty() ||
			    vecWords[0] == "\n" ||
			    vecWords[0] == " " ||
			    vecWords[0] == "**" ||
			    vecWords[0] == "*")
			{
				continue;
			}

			string AnimName      = vecWords[0];					// 1. 애니메이션 이름 
			int    FrameStartIdx = std::stoi(vecWords[1]);		// 2. 시작 프레임 인덱스 
			int    FrameEndIdx   = std::stoi(vecWords[2]);		// 3. 끝   프레임 인덱스 
			float  LerpTime      = std::stof(vecWords[3]);		// 4. 보간 시간 

			wstring wstrName = ToWString(AnimName);
			if (m_mapAnim.find(wstrName) == m_mapAnim.end())
			{
				// 컴포넌트에 적용 
				CAnimation3D* pAnim = CreateAnimByFrame(wstrName, 0, FrameStartIdx, FrameEndIdx);
				pAnim->SetLerpTime(LerpTime);
			}
		}
	}
}

void CAnimator3D::SetLerpTimeOnAnim(wstring _AnimName, float _fLerpTime)
{
	CAnimation3D* pAnim = FindAnim(_AnimName);
	pAnim->SetLerpTime(_fLerpTime);

	if (m_bPlayWithChild)
	{
		for (auto& pChild : GetOwner()->GetChild())
		{
			CAnimator3D* pChildAnimator3D = pChild->Animator3D();
			if (pChildAnimator3D != nullptr)
			{
				pChildAnimator3D->SetLerpTimeOnAnim(_AnimName, _fLerpTime);
			}
		}
	}
}

void CAnimator3D::CopyAllAnimToChild()
{
	// 하위 객체 적용 
	for (auto& pChild : GetOwner()->GetChild())
	{
		CAnimator3D* pA3D = pChild->Animator3D();
		if (pA3D == nullptr)
		{
			continue;
		}
		pA3D->CopyAllAnim(m_mapAnim);
		pA3D->CopyAllAnimToChild();
	}
}

CAnimation3D* CAnimator3D::CreateAnimByTime(const wstring& _strName, int _clipNum, double _StartTime, double _EndTime)
{
	if (FindAnim(_strName) != nullptr)
	{
		LOG_WARN("CreateAnimByTime : ANIMATION ALREADY EXISTS!");
		return nullptr;
	}
	//assert(!FindAnim(_strName));

	CAnimation3D* pAnim = new CAnimation3D{};
	pAnim->SetName(_strName);
	pAnim->SetOwner(this);
	pAnim->SetClipUpdateTime((int)m_pVecClip->size());
	pAnim->SetFrameInfoByTime(_clipNum, _StartTime, _EndTime);

	m_mapAnim.insert(make_pair(_strName, pAnim));

	if (m_bPlayWithChild)
	{
		for (auto& pChild : GetOwner()->GetChild())
		{
			CAnimator3D* pChildAnimator3D = pChild->Animator3D();
			if (pChildAnimator3D != nullptr)
			{
				pChildAnimator3D->CreateAnimByTime(_strName, _clipNum, _StartTime, _EndTime);
			}
		}
	}
	return pAnim;
}

void CAnimator3D::Play(const wstring& _strName, bool _bRepeat)
{
	CAnimation3D* pAnim = FindAnim(_strName);
	LOG_ASSERT(pAnim, "ANIMATION NOT FOUND");

	m_pPrevAnim = m_pCurAnim;
	m_bRepeat   = _bRepeat;

	if (m_pPrevAnim != nullptr)
	{
		// 애니메이션 종료 후 다른 애니메이션을 재생한 경우 
		if (m_pPrevAnim->IsFinish())
		{
			pAnim->SetPrevFrameEndIdx(m_pPrevAnim->GetEndFrameIdx());
		}
		// 애니메이션 진행 중 다른 애니메이션을 재생한 경우 
		else
		{
			pAnim->SetPrevFrameEndIdx(m_pPrevAnim->GetCurFrameIdx());
		}
	}

	m_pCurAnim = pAnim;
	m_pCurAnim->Play(true);
	m_pCurAnim->SetAnimState(ANIMATION_STATE::BEFORE_PLAY);
	m_pCurAnim->Play(true);

	if (m_pPrevAnim != nullptr)
	{
		m_pPrevAnim->Reset();
	}

	if (m_bPlayWithChild)
	{
		// 하위 객체들도 같이 애니메이션 Play 
		for (auto& pChild : GetOwner()->GetChild())
		{
			CAnimator3D* pChildAnimator3D = pChild->Animator3D();
			if (pChildAnimator3D != nullptr)
			{
				CAnimation3D* pChildAnim = pChildAnimator3D->FindAnim(_strName);
				if (pChildAnim != nullptr)
				{
					pAnim->CopyInfo(&pChildAnim); // pChildAnim 에 pAnim 의 정보를 복사한다. 
					pChildAnimator3D->Play(_strName, _bRepeat);
				}
			}
		}
	}
}

void CAnimator3D::ResizeSocketMatrix()
{
	m_pSocketBuffer->Create(sizeof(Matrix), GetBoneCount(), SB_TYPE::READ_WRITE, true, nullptr);
	m_vecSocketMatrix.clear();
	m_vecSocketMatrix.shrink_to_fit();
	m_vecSocketMatrix.resize(GetBoneCount());
}

const Matrix& CAnimator3D::GetSocket(int index)
{
	index        = ClampData(index, 0, static_cast<int>(m_vecSocketMatrix.size()));
	m_bUseSocket = true;

	return m_vecSocketMatrix[index];
}

void CAnimator3D::RegisterNextAnim(CAnimation3D* _pNextAnim)
{
	m_pNextAnim = _pNextAnim;
	if (m_pCurAnim == m_pNextAnim || _pNextAnim == nullptr)
	{
		m_pNextAnim = nullptr;
	}
}


void CAnimator3D::Clear()
{
	Safe_Del_Map(m_mapAnim);
	m_pPrevAnim = nullptr;
	m_pCurAnim  = nullptr;
	m_pNextAnim = nullptr;

	m_bRepeat = false;
}

void CAnimator3D::DeleteAnim(const wstring& _wstrName)
{
	if (m_mapAnim.find(_wstrName) == m_mapAnim.end())
	{
		return;
	}

	if (m_pCurAnim == m_mapAnim.find(_wstrName)->second)
	{
		m_pCurAnim = (m_pNextAnim != nullptr) ? m_pNextAnim : nullptr;
		m_bRepeat  = false;
	}

	CAnimation3D* pDelAnim = m_mapAnim.find(_wstrName)->second;
	SAFE_DELETE(pDelAnim);
	m_mapAnim.erase(_wstrName);

	if (m_bPlayWithChild)
	{
		// [ DELETE PARENT OBJ / ANIMATION ] - 부모 애니 삭제 
		CGameObject* pParentObj = GetOwner()->GetParent();
		if (pParentObj != nullptr)
		{
			CAnimator3D* pAnimator3D = pParentObj->Animator3D();
			if (pAnimator3D != nullptr)
			{
				pAnimator3D->DeleteAnim(_wstrName);
			}
		}
		// [ DELETE CHILD OBJ / ANIMTION ] - 자식 애니 삭제 
		for (auto& pChild : GetOwner()->GetChild())
		{
			CAnimator3D* pAnimator3D = pChild->Animator3D();
			if (pAnimator3D != nullptr)
			{
				pAnimator3D->DeleteAnim(_wstrName);
			}
		}
	}
}


void CAnimator3D::SetLerpTimeOnAllAnim(float _fLerpTime)
{
	for (auto& [animationName,pAnimation] : m_mapAnim)
	{
		pAnimation->SetLerpTime(_fLerpTime);
	}

	if (m_bPlayWithChild)
	{
		for (auto& pChild : GetOwner()->GetChild())
		{
			CAnimator3D* pChildAnimator3D = pChild->Animator3D();
			if (pChildAnimator3D != nullptr)
			{
				pChildAnimator3D->SetLerpTimeOnAllAnim(_fLerpTime);
			}
		}
	}
}

void CAnimator3D::SetAnimState(wstring _Name, ANIMATION_STATE _eState)
{
	switch (_eState)
	{
	case ANIMATION_STATE::BEFORE_PLAY:
		{
			CAnimation3D* pAnim = FindAnim(_Name);
			pAnim->SetAnimState(_eState);

			// CHILD OBJECT SETTING 
			if (m_bPlayWithChild)
			{
				for (auto& pChild : GetOwner()->GetChild())
				{
					CAnimator3D* pChildAnimator3D = pChild->Animator3D();
					if (pChildAnimator3D != nullptr)
					{
						CAnimation3D* pChildAnim3D = pChildAnimator3D->FindAnim(_Name);
						if (pChildAnim3D != nullptr)
						{
							pChildAnim3D->SetAnimState(_eState);
						}
					}
				}
			}
		}
		break;
	case ANIMATION_STATE::PLAY:
		{
			// OBJECT SETTING 
			CAnimation3D* pAnim = FindAnim(_Name);
			pAnim->SetAnimState(_eState);
			pAnim->Play(true);
			pAnim->SetFinish(false);

			// CHILD OBJECT SETTING 
			if (m_bPlayWithChild)
			{
				for (auto& pChild : GetOwner()->GetChild())
				{
					CAnimator3D* pChildAnimator3D = pChild->Animator3D();
					if (pChildAnimator3D != nullptr)
					{
						CAnimation3D* pChildAnim3D = pChildAnimator3D->FindAnim(_Name);
						if (pChildAnim3D != nullptr)
						{
							pChildAnim3D->SetAnimState(_eState);
							pChildAnim3D->Play(true);
							pChildAnim3D->SetFinish(false);
						}
					}
				}
			}
		}
		break;
	case ANIMATION_STATE::STOP:
		{
			// OBJECT SETTING 
			CAnimation3D* pAnim = FindAnim(_Name);
			pAnim->SetAnimState(_eState);
			pAnim->Play(false);

			// CHILD OBJECT SETTING 
			if (m_bPlayWithChild)
			{
				for (auto& pChild : GetOwner()->GetChild())
				{
					CAnimator3D* pChildAnimator3D = pChild->Animator3D();
					if (pChildAnimator3D != nullptr)
					{
						CAnimation3D* pChildAnim3D = pChildAnimator3D->FindAnim(_Name);
						if (pChildAnim3D != nullptr)
						{
							pChildAnim3D->SetAnimState(_eState);
							pChildAnim3D->Play(false);
						}
					}
				}
			}
		}
		break;
	case ANIMATION_STATE::FINISH:
		{
			CAnimation3D* pAnim = FindAnim(_Name);
			pAnim->SetAnimState(_eState);

			// CHILD OBJECT SETTING 
			if (m_bPlayWithChild)
			{
				for (auto& pChild : GetOwner()->GetChild())
				{
					CAnimator3D* pChildAnimator3D = pChild->Animator3D();
					if (pChildAnimator3D != nullptr)
					{
						CAnimation3D* pChildAnim3D = pChildAnimator3D->FindAnim(_Name);
						if (pChildAnim3D != nullptr)
						{
							pChildAnim3D->SetAnimState(_eState);
						}
					}
				}
			}
		}
		break;
	}
}

void CAnimator3D::SetSpeed(float _fSpeed)
{
	m_pCurAnim->SetSpeed(_fSpeed);
	if (m_bPlayWithChild)
	{
		for (auto& pChild : GetOwner()->GetChild())
		{
			const CAnimator3D* pChildAnimator3D = pChild->Animator3D();
			if (pChildAnimator3D != nullptr)
			{
				CAnimation3D* pAnim = pChildAnimator3D->GetCurAnim();
				if (pAnim != nullptr)
					pAnim->SetSpeed(_fSpeed);
			}
		}
	}
}

void CAnimator3D::SetSpeedOnAnim(wstring _AnimName, float _fSpeed)
{
	CAnimation3D* pAnim = FindAnim(_AnimName);
	pAnim->SetSpeed(_fSpeed);

	if (m_bPlayWithChild)
	{
		for (auto& pChild : GetOwner()->GetChild())
		{
			CAnimator3D* pChildAnimator3D = pChild->Animator3D();
			if (pChildAnimator3D != nullptr)
			{
				pChildAnimator3D->SetLerpTimeOnAnim(_AnimName, _fSpeed);
			}
		}
	}
}

void CAnimator3D::SetLerpTime(float _fTime)
{
	if (m_pCurAnim == nullptr)
	{
		return;
	}

	m_pCurAnim->SetLerpTime(_fTime);
	if (m_bPlayWithChild)
	{
		for (auto& pChild : GetOwner()->GetChild())
		{
			const CAnimator3D* pChildAnimator3D = pChild->Animator3D();
			if (pChildAnimator3D != nullptr)
			{
				CAnimation3D* pAnim = pChildAnimator3D->GetCurAnim();
				if (pAnim != nullptr)
				{
					pAnim->SetLerpTime(_fTime);
				}
			}
		}
	}
}


void CAnimator3D::SetPlayWithChild(bool _bPlayWithChild)
{
	m_bPlayWithChild = _bPlayWithChild;

	// 하위 객체들도 같이 애니메이션 Play 
	for (auto& pChild : GetOwner()->GetChild())
	{
		CAnimator3D* pChildAnimator3D = pChild->Animator3D();
		if (pChildAnimator3D != nullptr)
		{
			pChildAnimator3D->SetPlayWithChild(_bPlayWithChild);
		}
	}
}


void CAnimator3D::SetRepeat(bool _bEnable)
{
	m_bRepeat = _bEnable;

	if (m_bPlayWithChild)
	{
		for (auto& pChild : GetOwner()->GetChild())
		{
			CAnimator3D* pChildAnimator3D = pChild->Animator3D();
			if (pChildAnimator3D != nullptr)
			{
				pChildAnimator3D->SetRepeat(_bEnable);
			}
		}
	}
}

void CAnimator3D::Serialize(YAML::Emitter& emitter)
{
	const CRes& mesh = *MeshRender()->GetMesh().Get();
	emitter << YAML::Key << "MESH" << YAML::Value << mesh;

	const std::string prevAnimName  = (m_pPrevAnim) ? ToString(m_pPrevAnim->GetName()) : "";
	const std::string curAnimName   = (m_pCurAnim) ? ToString(m_pCurAnim->GetName()) : "";
	const std::string nextvAnimName = (m_pNextAnim) ? ToString(m_pNextAnim->GetName()) : "";

	emitter << YAML::Key << NAME_OF(m_pPrevAnim) << YAML::Value << prevAnimName;
	emitter << YAML::Key << NAME_OF(m_pCurAnim) << YAML::Value << curAnimName;
	emitter << YAML::Key << NAME_OF(m_pNextAnim) << YAML::Value << nextvAnimName;

	emitter << YAML::Key << "ANIMATION LIST" << YAML::Value << YAML::BeginMap;
	for (auto& animData : m_mapAnim)
	{
		emitter << YAML::Key << ToString(animData.first) << YAML::Value << YAML::BeginMap;
		animData.second->Serialize(emitter);
		emitter << YAML::EndMap;
	}
	emitter << YAML::EndMap;
}

void CAnimator3D::Deserialize(const YAML::Node& node)
{
	Ptr<CMesh> pMesh = LoadAs<CMesh>(node["MESH"]);
	SetBones(pMesh->GetBones());
	SetAnimClip(pMesh->GetAnimClip());

	for (auto animNode : node["ANIMATION LIST"])
	{
		CAnimation3D* pAnimation = new CAnimation3D{};
		pAnimation->SetOwner(this);
		pAnimation->SetClipUpdateTime((int)m_pVecClip->size());
		const std::wstring animName = ToWString(animNode.first.as<std::string>());
		pAnimation->SetName(animName);
		pAnimation->Deserialize(animNode.second);

		m_mapAnim.insert({animName, pAnimation});
	}

	m_pPrevAnim = FindAnim(ToWString(node[NAME_OF(m_pPrevAnim)].as<std::string>()));
	m_pCurAnim  = FindAnim(ToWString(node[NAME_OF(m_pCurAnim)].as<std::string>()));
	m_pNextAnim = FindAnim(ToWString(node[NAME_OF(m_pNextAnim)].as<std::string>()));

	SetPlayWithChild(true);
}
