#include "pch.h"
#include "CAnimator2D.h"

#include "CAnimation2D.h"
#include "CDevice.h"
#include "CConstBuffer.h"
#include "CSerializer.h"

CAnimator2D::CAnimator2D()
	: CComponent(COMPONENT_TYPE::ANIMATOR2D)
	, m_pCurAnim(nullptr)
	, m_bRepeat(false) {}

CAnimator2D::CAnimator2D(const CAnimator2D& _origin)
	: CComponent(_origin)
	, m_pCurAnim(nullptr)
	, m_bRepeat(_origin.m_bRepeat)
{
	for (const auto& [animationName, pAnimation] : _origin.m_mapAnim)
	{
		CAnimation2D* pCloneAnim = pAnimation->Clone();
		m_mapAnim.insert({animationName, pCloneAnim});
		pCloneAnim->m_pOwner = this;
	}

	if (nullptr != _origin.m_pCurAnim)
	{
		m_pCurAnim = FindAnim(_origin.m_pCurAnim->GetName());
	}
}

CAnimator2D::~CAnimator2D()
{
	Safe_Del_Map(m_mapAnim);
}

void CAnimator2D::finalupdate()
{
	if (nullptr == m_pCurAnim)
	{
		return;
	}

	m_pCurAnim->finalupdate();

	if (m_pCurAnim->IsFinish() && m_bRepeat)
	{
		m_pCurAnim->Reset();
	}
}

void CAnimator2D::UpdateData()
{
	if (nullptr == m_pCurAnim)
		return;

	m_pCurAnim->UpdateData();
}

void CAnimator2D::Clear()
{
	static CConstBuffer* pBuffer = CDevice::GetInst()->GetCB(CB_TYPE::ANIM2D);
	const tAnim2D        info    = {};
	pBuffer->SetData(&info, sizeof(tAnim2D));
	pBuffer->UpdateData();
}

CAnimation2D* CAnimator2D::FindAnim(const wstring& _strName)
{
	const auto iter = m_mapAnim.find(_strName);
	if (iter == m_mapAnim.end())
	{
		return nullptr;
	}
	return iter->second;
}

void CAnimator2D::CreateAnim(const wstring& _strName,
                             Ptr<CTexture>  _pAtlas,
                             Vec2           _vBackgroundSizePixel,
                             Vec2           _vLeftTopPixel,
                             Vec2           _vSlicePixel,
                             Vec2           _vStepPixel,
                             float          _fDuration,
                             int            _iFrameCount)
{
	LOG_ASSERT(!FindAnim(_strName), "ANIMTION ALREADY EXISTS!");

	CAnimation2D* anim = new CAnimation2D;
	anim->SetName(_strName);
	anim->Create(_pAtlas, _vBackgroundSizePixel, _vLeftTopPixel, _vSlicePixel, _vStepPixel, _fDuration, _iFrameCount);
	anim->m_pOwner = this;

	m_mapAnim.insert({_strName, anim});
}

void CAnimator2D::Play(const wstring& _strName, bool _bRepeat)
{
	CAnimation2D* pAnim = FindAnim(_strName);
	LOG_ASSERT(pAnim, "ANIMATION NOT FOUND");

	m_bRepeat  = _bRepeat;
	m_pCurAnim = pAnim;
}


void CAnimator2D::SaveToScene(FILE* _pFile)
{
	CComponent::SaveToScene(_pFile);

	const size_t iAnimCount = m_mapAnim.size();
	fwrite(&iAnimCount, sizeof(size_t), 1, _pFile);

	for (auto& [animationName,pAnimation] : m_mapAnim)
	{
		pAnimation->SaveToScene(_pFile);
	}

	wstring strCurAnimName;
	if (nullptr != m_pCurAnim)
	{
		strCurAnimName = m_pCurAnim->GetName();
	}

	SaveWStringToFile(strCurAnimName, _pFile);
	fwrite(&m_bRepeat, sizeof(bool), 1, _pFile);
}

void CAnimator2D::LoadFromScene(FILE* _pFile)
{
	CComponent::LoadFromScene(_pFile);

	size_t iAnimCount = 0;
	fread(&iAnimCount, sizeof(size_t), 1, _pFile);

	for (size_t i = 0; i < iAnimCount; ++i)
	{
		CAnimation2D* pAnim = new CAnimation2D{};
		pAnim->LoadFromScene(_pFile);
		pAnim->m_pOwner = this;

		m_mapAnim.insert({pAnim->GetName(), pAnim});
	}

	wstring strCurAnimName;
	LoadWStringFromFile(strCurAnimName, _pFile);
	m_pCurAnim = FindAnim(strCurAnimName);

	fread(&m_bRepeat, sizeof(bool), 1, _pFile);
}


void CAnimator2D::Serialize(YAML::Emitter& emitter)
{
	for (auto& pairData : m_mapAnim)
	{
		emitter << YAML::Key << ToString(pairData.first) << YAML::Value << YAML::BeginSeq;
		pairData.second->Serialize(emitter);
		emitter << YAML::EndSeq;
	}
	std::string currentAnimationName = (m_pCurAnim) ? ToString(m_pCurAnim->GetName()) : "";
	emitter << YAML::Key << "CURRENT ANIMATION NAME" << YAML::Value << currentAnimationName;
	emitter << YAML::Key << NAME_OF(m_bRepeat) << YAML::Value << m_bRepeat;
}

void CAnimator2D::Deserialize(const YAML::Node& node)
{
	for (auto dataNode : node)
	{
		std::string key = dataNode.first.as<std::string>();
		if (key == "CURRENT ANIMATION NAME")
		{
			break;
		}
		YAML::Node    animationNode = dataNode.second;
		CAnimation2D* pAnimation2D  = new CAnimation2D{};
		pAnimation2D->Deserialize(animationNode);
		std::wstring animationName = ToWString(key);
		m_mapAnim.insert({animationName, pAnimation2D});
	}
	std::string currentAnimName = node["CURRENT ANIMATION NAME"].as<std::string>();
	if (false == currentAnimName.empty())
	{
		m_pCurAnim = m_mapAnim.find(ToWString(currentAnimName))->second;
	}
	m_bRepeat = node[NAME_OF(m_bRepeat)].as<bool>();
}
