#pragma once

#include <mutex>

#include "Ptr.h"

#include "CPathMgr.h"

#include "CPrefab.h"
#include "CMaterial.h"
#include "CMesh.h"
#include "CTexture.h"
#include "CGraphicsShader.h"
#include "CComputeShader.h"
#include "CLog.h"
#include "CSceneFile.h"
#include "CSound.h"
#include "CMeshData.h"
#include "CNaviMapData.h"

class CResMgr
	: public CSingleton<CResMgr>
{
	SINGLE(CResMgr);
private:
	std::map<std::wstring, CRes*> m_Res[static_cast<UINT>(RES_TYPE::END)];

	std::mutex m_mutex;
public:
	void init();
	void update();
	void SaveChangedRes();

private:
	void InitSound();
	void CreateEngineMesh();
	void CreateEngineTexture();
	void CreateEngineShader();
	void CreateEngineComputeShader();
	void CreateEngineMaterial();
	void MakeInputLayoutInfo();

public:
	template <typename T>
	RES_TYPE GetResType() const;

	template <typename T>
	Ptr<T> Load(const wstring& _strKey, const wstring& _strRelativePath, bool _bEngineResource = false);


	template <typename type>
	Ptr<type> FindRes(const wstring& _strKey);

	vector<Ptr<CMeshData>> LoadFBX(const wstring& _strPath);

	const map<wstring, CRes*>& GetResList(RES_TYPE _eType) { return m_Res[static_cast<UINT>(_eType)]; }

	template <typename T>
	void AddRes(const wstring& _strKey, T* _pResource, bool _bEngineResource = false);

	// _flag : D3D11_BIND_FLAG
	Ptr<CTexture> CreateTexture(const wstring& _strKey,
	                            UINT           _iWidth,
	                            UINT           _iHeight,
	                            DXGI_FORMAT    _format,
	                            UINT           _flag,
	                            bool           _bEngineResource = false);

	Ptr<CTexture> CreateTexture(const wstring& _strKey, ComPtr<ID3D11Texture2D> _pTex2D, bool _bEngineResource = false);

	Ptr<CTexture> LoadTexture(const wstring& _strKey, const wstring& _strRelativePath, int _iMapLevel);


	template <typename T>
	void ForceDeleteRes(const wstring& _key);


private:
	void DeleteRes(const wstring& _strKey);

	friend class CEventMgr;
};

template <typename T>
RES_TYPE CResMgr::GetResType() const
{
	if (std::is_same_v<T, CPrefab>)
		return RES_TYPE::PREFAB;
	if (std::is_same_v<T, CMeshData>)
		return RES_TYPE::MESHDATA;
	if (std::is_same_v<T, CMesh>)
		return RES_TYPE::MESH;
	if (std::is_same_v<T, CSound>)
		return RES_TYPE::SOUND;
	if (std::is_same_v<T, CGraphicsShader>)
		return RES_TYPE::GRAPHICS_SHADER;
	if (std::is_same_v<T, CComputeShader>)
		return RES_TYPE::COMPUTE_SHADER;
	if (std::is_same_v<T, CMaterial>)
		return RES_TYPE::MATERIAL;
	if (std::is_same_v<T, CTexture>)
		return RES_TYPE::TEXTURE;
	if (std::is_same_v<T, CSceneFile>)
		return RES_TYPE::SCENEFILE;
	if (std::is_same_v<T, CNaviMapData>)
		return RES_TYPE::NAVIMAPDATA;

	return RES_TYPE::END;
}

template <typename T>
Ptr<T> CResMgr::Load(const wstring& _strKey, const wstring& _strRelativePath, bool _bEngineResource)
{
	RES_TYPE eType = GetResType<T>();
	{
		std::lock_guard<std::mutex> lock{m_mutex};
		CRes*                       pRes = FindRes<T>(_strKey).Get();
		if (nullptr != pRes)
		{
			return static_cast<T*>(pRes);
		}
	}

	const wstring strContentPath = CPathMgr::GetInst()->GetContentPath();
	const wstring strFullPath    = strContentPath + _strRelativePath;

	Ptr<T> pRes = new T{};
	pRes->SetKey(_strKey);
	pRes->SetRelativePath(_strRelativePath);
	pRes->m_bEngineRes = _bEngineResource;
	if (FAILED(pRes->Load(strFullPath)))
	{
		CRes* pDelete = pRes.Get();
		SAFE_DELETE(pDelete);
		//MessageBox(nullptr, L"리소스 로딩 실패", L"리소스 로딩 오류", MB_OK);
		LOG_ERROR("RESOURCE LOAD FAILED : %s", ToString(strFullPath).c_str());
		//MessageBox(nullptr, L"리소스 로딩 실패", L"리소스 로딩 오류", MB_OK);
		LOG_ERROR("RESOURCE LOAD FAILED :%s", ToString(strFullPath).c_str());
		return nullptr;
	}
	{
		std::lock_guard<std::mutex> lock{m_mutex};
		m_Res[static_cast<UINT>(eType)].insert({_strKey, pRes.Get()});
	}

	return static_cast<T*>(pRes.Get());
}

template <typename T>
Ptr<T> CResMgr::FindRes(const wstring& _strKey)
{
	RES_TYPE eType = GetResType<T>();

	const auto iter = m_Res[static_cast<UINT>(eType)].find(_strKey);
	if (iter == m_Res[static_cast<UINT>(eType)].end())
	{
		return nullptr;
	}

	return static_cast<T*>(iter->second);
}

template <typename T>
void CResMgr::AddRes(const wstring& _strKey, T* _pResource, bool _bEngineResource)
{
	RES_TYPE eType = GetResType<T>();
	Ptr<T>   pRes  = FindRes<T>(_strKey);
	assert(nullptr == pRes);

	_pResource->SetKey(_strKey);
	_pResource->SetRelativePath(_strKey);
	_pResource->m_bEngineRes = _bEngineResource;

	m_Res[static_cast<UINT>(eType)].insert({_strKey, _pResource});
	if (_bEngineResource)
	{
		return;
	}

	const std::wstring strContentPath = CPathMgr::GetInst()->GetContentPath();
	const std::wstring strFullPath    = strContentPath + _pResource->GetRelativePath();
	if (false == std::filesystem::exists(strFullPath))
	{
		_pResource->Save(strContentPath + _pResource->GetRelativePath());
	}
}

template <typename T>
void CResMgr::ForceDeleteRes(const wstring& _key)
{
	RES_TYPE eType = GetResType<T>();

	const auto iter = m_Res[static_cast<UINT>(eType)].find(_key);
	if (nullptr != iter->second)
	{
		if (nullptr != iter->second)
		{
			delete iter->second;
		}
		m_Res[static_cast<UINT>(eType)].erase(iter);
	}
}
