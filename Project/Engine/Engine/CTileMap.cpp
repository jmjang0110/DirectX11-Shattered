#include "pch.h"
#include "CTileMap.h"

#include "CTransform.h"
#include "CResMgr.h"
#include "CSerializer.h"
#include "CStructuredBuffer.h"

CTileMap::CTileMap()
	: CRenderComponent(COMPONENT_TYPE::TILEMAP)
	, m_iRowCount(0)
	, m_iColCount(0)
	, m_iTileCountX(0)
	, m_iTileCountY(0)
	, m_bBufferUpdated(false)
{
	// 메쉬, 재질
	SetMesh(CResMgr::GetInst()->FindRes<CMesh>(L"RectMesh"));
	SetSharedMaterial(CResMgr::GetInst()->FindRes<CMaterial>(L"TileMapMtrl"), 0);

	m_vecTileData.resize(m_iTileCountX * m_iTileCountY);
	m_pBuffer = new CStructuredBuffer;
}

CTileMap::CTileMap(const CTileMap& _origin)
	: CRenderComponent(_origin)
	, m_vSlicePixel(_origin.m_vSlicePixel)
	, m_vSliceUV(_origin.m_vSliceUV)
	, m_iRowCount(_origin.m_iRowCount)
	, m_iColCount(_origin.m_iColCount)
	, m_iTileCountX(_origin.m_iTileCountX)
	, m_iTileCountY(_origin.m_iTileCountY)
	, m_vecTileData(_origin.m_vecTileData)
	, m_pBuffer(nullptr)
	, m_bBufferUpdated(false)
{
	m_pBuffer = new CStructuredBuffer;
}

CTileMap::~CTileMap()
{
	SAFE_DELETE(m_pBuffer);
}

void CTileMap::finalupdate() { }

void CTileMap::UpdateData()
{
	if (nullptr == m_pAtlasTex)
		return;

	GetMaterial(0)->SetTexParam(TEX_PARAM::TEX_0, m_pAtlasTex);
	GetMaterial(0)->SetScalarParam(SCALAR_PARAM::INT_0, &m_iTileCountX);
	GetMaterial(0)->SetScalarParam(SCALAR_PARAM::INT_1, &m_iTileCountY);
	GetMaterial(0)->SetScalarParam(SCALAR_PARAM::VEC2_0, &m_vSliceUV);

	// 모든 타일 데이터(m_vecTileData) 를 구조화버퍼를 통해 t16 레지스터로 바인딩
	if (false == m_bBufferUpdated)
	{
		m_pBuffer->SetData(m_vecTileData.data(), m_iTileCountX * m_iTileCountY);
		m_bBufferUpdated = true;
	}

	m_pBuffer->UpdateData(PS, 16);
}


void CTileMap::render()
{
	if (nullptr == GetMesh() || nullptr == GetMaterial(0) || nullptr == m_pAtlasTex)
		return;

	UpdateData();

	Transform()->UpdateData();
	GetMaterial(0)->UpdateData();
	GetMesh()->render(0);
}


void CTileMap::SetTileMapCount(UINT _iCountX, UINT _iCountY)
{
	m_iTileCountX = _iCountX;
	m_iTileCountY = _iCountY;

	ClearTileData();
}

void CTileMap::SetTileData(int _iTileIdx, int _iImgIdx)
{
	if (nullptr == m_pAtlasTex)
	{
		return;
	}

	m_vecTileData[_iTileIdx].iImgIdx = _iImgIdx;

	// 아틀라스에서 타일의 행, 렬 개수 구하기
	m_iColCount = static_cast<UINT>(m_pAtlasTex->Width()) / static_cast<UINT>(m_vSlicePixel.x);
	m_iRowCount = static_cast<UINT>(m_pAtlasTex->Height()) / static_cast<UINT>(m_vSlicePixel.y);

	const int iRow = m_vecTileData[_iTileIdx].iImgIdx / m_iColCount;
	const int iCol = m_vecTileData[_iTileIdx].iImgIdx % m_iColCount;

	m_vecTileData[_iTileIdx].vLTUV = Vec2(m_vSliceUV.x * iCol, m_vSliceUV.y * iRow);

	m_bBufferUpdated = false;
}

void CTileMap::ClearTileData()
{
	m_vecTileData.shrink_to_fit();
	m_vecTileData.resize(m_iTileCountX * m_iTileCountY);

	// 구조화 버퍼도 크기에 대응한다.
	const size_t iBufferSize = (m_iTileCountX * m_iTileCountY) * sizeof(tTileData);
	if (m_pBuffer->GetBufferSize() < iBufferSize)
	{
		m_pBuffer->Create(sizeof(tTileData), m_iTileCountX * m_iTileCountY, SB_TYPE::READ_ONLY, true, nullptr);
	}

	m_bBufferUpdated = false;
}


void CTileMap::SaveToScene(FILE* _pFile)
{
	CRenderComponent::SaveToScene(_pFile);

	SaveResPtr(m_pAtlasTex, _pFile);

	fwrite(&m_vSlicePixel, sizeof(Vec2), 1, _pFile);
	fwrite(&m_vSliceUV, sizeof(Vec2), 1, _pFile);
	fwrite(&m_iRowCount, sizeof(UINT), 1, _pFile);
	fwrite(&m_iColCount, sizeof(UINT), 1, _pFile);
	fwrite(&m_iTileCountX, sizeof(UINT), 1, _pFile);
	fwrite(&m_iTileCountY, sizeof(UINT), 1, _pFile);
	fwrite(m_vecTileData.data(), sizeof(tTileData), m_vecTileData.size(), _pFile);
}

void CTileMap::LoadFromScene(FILE* _pFile)
{
	CRenderComponent::LoadFromScene(_pFile);

	LoadResPtr(m_pAtlasTex, _pFile);

	fread(&m_vSlicePixel, sizeof(Vec2), 1, _pFile);
	fread(&m_vSliceUV, sizeof(Vec2), 1, _pFile);
	fread(&m_iRowCount, sizeof(UINT), 1, _pFile);
	fread(&m_iColCount, sizeof(UINT), 1, _pFile);
	fread(&m_iTileCountX, sizeof(UINT), 1, _pFile);
	fread(&m_iTileCountY, sizeof(UINT), 1, _pFile);

	ClearTileData();
	fread(m_vecTileData.data(), sizeof(tTileData), m_iTileCountX * m_iTileCountY, _pFile);
}

void CTileMap::Serialize(YAML::Emitter& emitter)
{
	CRenderComponent::Serialize(emitter);

	const CRes& texture = *m_pAtlasTex.Get();
	emitter << YAML::Key << "SHADER" << YAML::Value << texture;
	emitter << YAML::Key << NAME_OF(m_vSlicePixel) << YAML::Value << m_vSlicePixel;
	emitter << YAML::Key << NAME_OF(m_vSliceUV) << YAML::Value << m_vSliceUV;
	emitter << YAML::Key << NAME_OF(m_iRowCount) << YAML::Value << m_iRowCount;
	emitter << YAML::Key << NAME_OF(m_iColCount) << YAML::Value << m_iColCount;
	emitter << YAML::Key << NAME_OF(m_iTileCountX) << YAML::Value << m_iTileCountX;
	emitter << YAML::Key << NAME_OF(m_iTileCountY) << YAML::Value << m_iTileCountY;
	emitter << YAML::Key << NAME_OF(m_vecTileData) << YAML::Value << m_vecTileData.data();
}

void CTileMap::Deserialize(const YAML::Node& node)
{
	CRenderComponent::Deserialize(node);
}
