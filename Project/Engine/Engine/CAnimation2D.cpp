#include "pch.h"
#include "CAnimation2D.h"

#include "CAnimator2D.h"
#include "CTimeMgr.h"

#include "CDevice.h"
#include "CConstBuffer.h"

#include "Ptr.h"
#include "CSerializer.h"

CAnimation2D::CAnimation2D()
	: m_pOwner(nullptr)
	, m_iCurFrmIdx(0)
	, m_fAccTime(false)
	, m_bFinish(false) {}

CAnimation2D::~CAnimation2D() = default;

void CAnimation2D::finalupdate()
{
	if (m_bFinish)
	{
		return;
	}

	m_fAccTime += DT;

	if (m_vecFrm[m_iCurFrmIdx].fDuration < m_fAccTime)
	{
		m_fAccTime -= m_vecFrm[m_iCurFrmIdx].fDuration;

		if (m_vecFrm.size() - 1 <= m_iCurFrmIdx)
		{
			m_bFinish = true;
		}
		else
		{
			++m_iCurFrmIdx;
		}
	}
}

void CAnimation2D::UpdateData()
{
	static CConstBuffer* pBuffer = CDevice::GetInst()->GetCB(CB_TYPE::ANIM2D);

	tAnim2D info      = {};
	info.useAnim2D    = 1;
	info.Atlas_Width  = m_pAtlasTex->Width();
	info.Atlas_Height = m_pAtlasTex->Height();

	info.vBackgroundSize = m_vBackgroundSize;
	info.vLT             = m_vecFrm[m_iCurFrmIdx].vLT;
	info.vSlice          = m_vecFrm[m_iCurFrmIdx].vSlice;
	info.vOffset         = m_vecFrm[m_iCurFrmIdx].vOffset;

	pBuffer->SetData(&info, sizeof(tAnim2D));
	pBuffer->UpdateData();


	m_pAtlasTex->UpdateData(PS, 10);
}

void CAnimation2D::Create(Ptr<CTexture> _Atlas,
                          Vec2          _vBackgroundSizePixel,
                          Vec2          _vLT,
                          Vec2          _vSlice,
                          Vec2          _vStep,
                          float         _fDuration,
                          int           _iFrameCount)
{
	LOG_ASSERT(_Atlas.Get(), "NO INPUT FOR ATALS TEXTURE");

	m_pAtlasTex            = _Atlas;
	const Vec2 textureSize = {m_pAtlasTex->Width(), m_pAtlasTex->Height()};

	// 픽셀 좌표를 0~1 UV 로 전환
	const Vec2 leftTop   = _vLT / textureSize;
	const Vec2 sliceSize = _vSlice / textureSize;
	const Vec2 stepSize  = _vStep / textureSize;

	m_vBackgroundSize = _vBackgroundSizePixel / textureSize;

	// 프레임 정보 생성
	for (int i = 0; i < _iFrameCount; ++i)
	{
		tAnim2DFrame frm = {};
		frm.vLT          = leftTop + (stepSize * static_cast<float>(i));
		frm.vSlice       = sliceSize;
		frm.fDuration    = _fDuration;

		m_vecFrm.push_back(frm);
	}
}

void CAnimation2D::SaveToScene(FILE* _pFile)
{
	CEntity::SaveToScene(_pFile);

	const size_t frameCount = m_vecFrm.size();
	fwrite(&frameCount, sizeof(size_t), 1, _pFile);
	fwrite(m_vecFrm.data(), sizeof(tAnim2DFrame), frameCount, _pFile);
	fwrite(&m_vBackgroundSize, sizeof(Vec2), 1, _pFile);

	SaveResPtr(m_pAtlasTex, _pFile);
}

void CAnimation2D::LoadFromScene(FILE* _pFile)
{
	CEntity::LoadFromScene(_pFile);

	size_t frameCount{};
	fread(&frameCount, sizeof(size_t), 1, _pFile);
	m_vecFrm.resize(frameCount);
	fread(m_vecFrm.data(), sizeof(tAnim2DFrame), frameCount, _pFile);
	fread(&m_vBackgroundSize, sizeof(Vec2), 1, _pFile);

	LoadResPtr(m_pAtlasTex, _pFile);
}

void CAnimation2D::Serialize(YAML::Emitter& emitter)
{
	const int frameCount = static_cast<int>(m_vecFrm.size());
	emitter << YAML::Key << "FRAME COUNT" << YAML::Value << frameCount;

	for (int i = 0; i < frameCount; ++i)
	{
		emitter << YAML::Key << i << YAML::Value << YAML::Flow << YAML::BeginMap;
		emitter << YAML::Key << "LEFT TOP" << YAML::Value << m_vecFrm[i].vLT;
		emitter << YAML::Key << "OFFSET" << YAML::Value << m_vecFrm[i].vOffset;
		emitter << YAML::Key << "DURATION" << YAML::Value << m_vecFrm[i].fDuration;
		emitter << YAML::Key << "SLICE" << YAML::Value << m_vecFrm[i].vSlice;
		emitter << YAML::EndMap;
	}
	emitter << YAML::Key << NAME_OF(m_vBackgroundSize) << YAML::Value << m_vBackgroundSize;
	CRes& atlasTex = *m_pAtlasTex.Get();
	emitter << YAML::Key << "ATLAS" << YAML::Value << atlasTex;
}

void CAnimation2D::Deserialize(const YAML::Node& node)
{
	const int frameCount = node["FRAME COUNT"].as<int>();
	for (int i = 0; i < frameCount; ++i)
	{
		tAnim2DFrame frame{};
		frame.vLT       = node[i]["LEFT TOP"].as<Vec2>();
		frame.vOffset   = node[i]["OFFSET"].as<Vec2>();
		frame.fDuration = node[i]["DURATION"].as<float>();
		frame.vSlice    = node[i]["SLICE"].as<Vec2>();

		m_vecFrm.push_back(frame);
	}
	m_vBackgroundSize = node[NAME_OF(m_vBackgroundSize)].as<Vec2>();
	m_pAtlasTex       = LoadAs<CTexture>(node["ATLAS"]);
}
