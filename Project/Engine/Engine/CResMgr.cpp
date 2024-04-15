#include "pch.h"
#include "CResMgr.h"
#include "CPathMgr.h"

CResMgr::CResMgr() = default;

CResMgr::~CResMgr()
{
	for (auto& mapResource : m_Res)
	{
		Safe_Del_Map(mapResource);
	}
}

void CResMgr::SaveChangedRes()
{
	const wstring strContent = CPathMgr::GetInst()->GetContentPath();
	for (auto& mapResource : m_Res)
	{
		for (auto& [resourceKey,pResource] : mapResource)
		{
			if (pResource->IsChanged())
			{
				pResource->Save(strContent + pResource->GetRelativePath());
			}
		}
	}
}


Ptr<CTexture> CResMgr::CreateTexture(const wstring& _strKey,
                                     UINT           _iWidth,
                                     UINT           _iHeight,
                                     DXGI_FORMAT    _format,
                                     UINT           _flag,
                                     bool           _bEngineRes)
{
	LOG_ASSERT(nullptr == FindRes<CTexture>(_strKey), "TEXTURE ALREADY EXISTS!");

	CTexture* pTexture = new CTexture{};
	pTexture->Create(_iWidth, _iHeight, _format, _flag);
	pTexture->SetKey(_strKey);
	pTexture->m_bEngineRes = _bEngineRes;

	AddRes<CTexture>(_strKey, pTexture, _bEngineRes);

	return pTexture;
}

Ptr<CTexture> CResMgr::CreateTexture(const wstring& _strKey, ComPtr<ID3D11Texture2D> _pTex2D, bool _bEngineRes)
{
	LOG_ASSERT(nullptr == FindRes<CTexture>(_strKey), "TEXTURE ALREADY EXISTS!");

	CTexture* pTexture = new CTexture{};
	pTexture->Create(_pTex2D);
	pTexture->SetKey(_strKey);
	pTexture->m_bEngineRes = _bEngineRes;

	AddRes<CTexture>(_strKey, pTexture, _bEngineRes);

	return pTexture;
}

Ptr<CTexture> CResMgr::LoadTexture(const wstring& _strKey, const wstring& _strRelativePath, int _iMapLevel)
{
	CTexture* pResource = FindRes<CTexture>(_strKey).Get();
	if (nullptr != pResource)
	{
		return pResource;
	}

	pResource = new CTexture{};

	const wstring strFilePath = CPathMgr::GetInst()->GetContentPath() + _strRelativePath;

	if (FAILED(pResource->Load(strFilePath, _iMapLevel)))
	{
		return nullptr;
	}
	pResource->SetKey(_strKey);
	pResource->SetRelativePath(_strRelativePath);

	m_Res[(UINT)RES_TYPE::TEXTURE].insert({_strKey, pResource});

	return pResource;
}

vector<Ptr<CMeshData>> CResMgr::LoadFBX(const wstring& _strPath)
{
	const wstring strFileName = std::filesystem::path(_strPath).stem();
	const wstring strName     = L"meshdata\\" + strFileName + L".mdat";

	vector<Ptr<CMeshData>> ptrMeshData{};

	const vector<CMeshData*> vecMeshData = CMeshData::LoadFromFBX(_strPath);
	for (int i = 0; i < vecMeshData.size(); ++i)
	{
		ptrMeshData.push_back(vecMeshData[i]);

		std::wstring resourceName = std::filesystem::path(_strPath).stem().wstring() + std::to_wstring(i);
		std::wstring resourceKey  = L"meshdata\\" + resourceName + L".mdat";

		ptrMeshData[i]->SetKey(resourceKey);
		ptrMeshData[i]->SetRelativePath(resourceKey);

		m_Res[(UINT)RES_TYPE::MESHDATA].insert({resourceKey, ptrMeshData[i].Get()});

		std::wstring fullPath = CPathMgr::GetInst()->GetContentPath() + resourceKey;
		ptrMeshData[i]->Save(fullPath);
	}

	return ptrMeshData;
}

void CResMgr::DeleteRes(const wstring& _strKey)
{
	for (UINT i = 0; i < (UINT)RES_TYPE::END; ++i)
	{
		const auto iter = m_Res[i].find(_strKey);
		if (iter != m_Res[i].end())
		{
			SAFE_DELETE(iter->second);
			m_Res[i].erase(iter);
			return;
		}
	}
	LOG_WARN("TRY TO DELETE INVALID RESOURCE : %s", ToString(_strKey).c_str());
}
