#include "pch.h"
#include "CNaviMapData.h"

#include <iostream>
#include <fstream>

#include "CResMgr.h"
#include "CSerializer.h"

CNaviCell::CNaviCell()
	: m_iIDX(-1)
{
}

CNaviCell::~CNaviCell()
{
}

CNaviMapData::CNaviMapData()
	: CRes(RES_TYPE::NAVIMAPDATA)
	, m_pNavMesh(nullptr)
{
}

CNaviMapData::~CNaviMapData()
{
	Safe_Del_Vec(m_vecNaviCell);
}

bool CNaviMapData::CreateFromObj(const wstring& _strRelativePath, const wstring& _strMeshPath)
{
	wstring strContentPath = CPathMgr::GetInst()->GetContentPath();
	wstring strFilePath    = strContentPath + _strRelativePath;

	std::ifstream file(strFilePath);
	if (file.is_open())
	{
		m_pNavMesh = new CMesh;
		string       s;
		float        x, y, z = 0.f;
		vector<Vtx>  vecVtx;
		vector<UINT> vecIdx;
		Vtx          v;

		while (!file.eof())
		{
			file >> s >> x >> y >> z;

			if ("v" == s)
			{
				v.vPos      = Vec3(x, y, z);
				v.vColor    = Vec4(0.2f, 0.2f, 0.2f, 1.f);
				v.vUV       = Vec2(0.f, 0.f);
				v.vTangent  = Vec3(1.f, 0.f, 0.f);
				v.vNormal   = Vec3(0.f, 0.f, -1.f);
				v.vBinormal = Vec3(0.f, 1.f, 0.f);

				vecVtx.push_back(v);
			}
			if ("f" == s)
			{
				vecIdx.push_back((UINT)x - 1);
				vecIdx.push_back((UINT)y - 1);
				vecIdx.push_back((UINT)z - 1);
			}
		}
		file.close();


		m_pNavMesh->Create(vecVtx.data(), (UINT)vecVtx.size(), vecIdx.data(), (UINT)vecIdx.size());
		CResMgr::GetInst()->AddRes<CMesh>(_strMeshPath, m_pNavMesh, true);
		m_pNavMesh->Save(CPathMgr::GetInst()->GetContentPath() + _strMeshPath);

		Bake(vecVtx, vecIdx);
		vecVtx.clear();
		vecIdx.clear();

		return true;
	}

	return false;
}

void CNaviMapData::Bake(const vector<Vtx>& _vVTXBuffer, const vector<UINT>& _vIDXBuffer)
{
	if (0 != _vIDXBuffer.size() % 3)
		assert(nullptr && "잘못된 인덱스 버퍼 크기입니다.");


	// NaviMesh의 Ploygon들을 Cell로 만들어 동적배열에 추가하기
	for (size_t i = 0; i < _vIDXBuffer.size(); i += 3)
	{
		CNaviCell* NewCell = new CNaviCell;

		NewCell->m_iIDX      = (UINT)m_vecNaviCell.size();
		NewCell->m_arrVTX[0] = _vVTXBuffer[_vIDXBuffer[i + 0]];
		NewCell->m_arrVTX[1] = _vVTXBuffer[_vIDXBuffer[i + 1]];
		NewCell->m_arrVTX[2] = _vVTXBuffer[_vIDXBuffer[i + 2]];

		m_vecNaviCell.push_back(NewCell);
	}

	// 각 Cell들의 인접셀 찾기
	for (size_t i = 0; i < m_vecNaviCell.size(); ++i)
	{
		CNaviCell* pCurCell = m_vecNaviCell[i];

		// Map의 모든 Cell을 돌면서 자신과 1개 이상 같은 정점을 공유한 Cell을 등록한다.
		for (size_t j = 0; j < m_vecNaviCell.size(); ++j)
		{
			if (pCurCell->m_iIDX == m_vecNaviCell[j]->m_iIDX)
				continue;

			for (size_t c = 0; c < 3; ++c)
			{
				for (size_t v = 0; v < 3; ++v)
				{
					if (pCurCell->m_arrVTX[c].vPos == m_vecNaviCell[j]->m_arrVTX[v].vPos)
					{
						bool overlap = false;

						// 이미 등록된 셀이면 넘어간다
						vector<int>::iterator iter = pCurCell->m_vecLinkedCell.begin();
						for (; iter != pCurCell->m_vecLinkedCell.end(); ++iter)
						{
							if (m_vecNaviCell[j]->m_iIDX == (*iter))
							{
								overlap = true;
								break;
							}
						}

						if (!overlap)
							pCurCell->m_vecLinkedCell.push_back(m_vecNaviCell[j]->m_iIDX);
					}
				}
			}
		}
	}
}


int CNaviMapData::Save(const wstring& _strFilePath)
{
	SetRelativePath(CPathMgr::GetInst()->GetRelativePath(_strFilePath));

	FILE*   pFile = nullptr;
	errno_t err   = _wfopen_s(&pFile, _strFilePath.c_str(), L"wb");

	assert(pFile);

	// 키값, 상대 경로	
	SaveWStringToFile(GetName(), pFile);
	SaveWStringToFile(GetKey(), pFile);
	SaveWStringToFile(GetRelativePath(), pFile);

	// 메쉬 정보 저장
	int VtxCount = 0;
	if (nullptr == m_pNavMesh)
	{
		SaveWStringToFile(L"NULL", pFile);
	}
	else
	{
		SaveWStringToFile(m_pNavMesh->GetRelativePath(), pFile);
		VtxCount = m_pNavMesh->GetVtxCount();
	}
	fwrite(&VtxCount, sizeof(int), 1, pFile);

	// NeviCell로 구성된 맵 정보 저장
	UINT iCount = static_cast<UINT>(m_vecNaviCell.size());
	fwrite(&iCount, sizeof(int), 1, pFile);
	for (size_t i = 0; i < m_vecNaviCell.size(); i++)
	{
		// IDX
		fwrite(&m_vecNaviCell[i]->m_iIDX, sizeof(int), 1, pFile);

		// Vertex
		for (size_t j = 0; j < 3; j++)
		{
			fwrite(&m_vecNaviCell[i]->m_arrVTX[j].vPos.x, sizeof(float), 1, pFile);
			fwrite(&m_vecNaviCell[i]->m_arrVTX[j].vPos.y, sizeof(float), 1, pFile);
			fwrite(&m_vecNaviCell[i]->m_arrVTX[j].vPos.z, sizeof(float), 1, pFile);
		}

		// LinkedCell
		UINT iLinkedCellSize = static_cast<UINT>(m_vecNaviCell[i]->m_vecLinkedCell.size());
		fwrite(&iLinkedCellSize, sizeof(UINT), 1, pFile);
		for (size_t s = 0; s < m_vecNaviCell[i]->m_vecLinkedCell.size(); s++)
		{
			fwrite(&m_vecNaviCell[i]->m_vecLinkedCell[s], sizeof(int), 1, pFile);
		}
	}

	return S_OK;
}

int CNaviMapData::Load(const wstring& _strFilePath)
{
	FILE* pFile = nullptr;
	_wfopen_s(&pFile, _strFilePath.c_str(), L"rb");

	// 키값, 상대경로
	wstring strName, strKey, strRelativePath;
	LoadWStringFromFile(strName, pFile);
	LoadWStringFromFile(strKey, pFile);
	LoadWStringFromFile(strRelativePath, pFile);

	SetName(strName);
	SetKey(strKey);
	SetRelativePath(strRelativePath);


	// mesh
	wstring pMeshKey;
	int     iVtxCount;

	LoadWStringFromFile(pMeshKey, pFile);
	fread(&iVtxCount, sizeof(int), 1, pFile);

	if (L"NULL" == pMeshKey)
		m_pNavMesh = nullptr;
	else
		m_pNavMesh = CResMgr::GetInst()->Load<CMesh>(pMeshKey, pMeshKey).Get();

	UINT iCount;
	fread(&iCount, sizeof(int), 1, pFile);
	m_vecNaviCell.resize(iCount);
	for (size_t i = 0; i < iCount; i++)
	{
		// IDX
		m_vecNaviCell[i] = new CNaviCell;
		fread(&m_vecNaviCell[i]->m_iIDX, sizeof(int), 1, pFile);

		// Vertex
		for (size_t j = 0; j < 3; j++)
		{
			float x, y, z = 0;
			fread(&x, sizeof(float), 1, pFile);
			fread(&y, sizeof(float), 1, pFile);
			fread(&z, sizeof(float), 1, pFile);

			if (nullptr != m_pNavMesh)
			{
				Vtx* pVTXArr = m_pNavMesh->GetVtxSysMem();

				for (size_t k = 0; k < iVtxCount; k++)
				{
					if (pVTXArr[k].vPos == Vec3(x, y, z))
						m_vecNaviCell[i]->m_arrVTX[j] = pVTXArr[k];
				}
			}
		}

		// LinkedCell
		UINT iLinkedCellSize = 0;
		fread(&iLinkedCellSize, sizeof(int), 1, pFile);
		for (size_t s = 0; s < iLinkedCellSize; s++)
		{
			UINT idx;
			fread(&idx, sizeof(int), 1, pFile);
			m_vecNaviCell[i]->m_vecLinkedCell.push_back(idx);
		}
	}

	return S_OK;
}
