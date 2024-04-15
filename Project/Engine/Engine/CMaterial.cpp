#include "pch.h"
#include "CMaterial.h"

#include "CGraphicsShader.h"
#include "CDevice.h"
#include "CConstBuffer.h"

#include "CSceneMgr.h"
#include "CScene.h"


CMaterial::CMaterial()
	: CRes(RES_TYPE::MATERIAL)
	, m_Param{}
	, m_arrTex{}
	, m_pShader(nullptr)
	, m_pMasterMtrl(nullptr)
	, m_pDynamicShader{nullptr}
	, m_bUseDynamicShader{false} {}

CMaterial::~CMaterial()
{
	if (m_pDynamicShader != nullptr)
	{
		SAFE_DELETE(m_pDynamicShader);
	}
}


void CMaterial::UpdateData()
{
	for (size_t i = 0; i < std::size(m_arrTex); ++i)
	{
		if (nullptr != m_arrTex[i])
		{
			m_arrTex[i]->UpdateData(ALL, static_cast<UINT>(i));
			m_Param.bTex[i] = true;
		}
		else
		{
			CTexture::Clear(static_cast<UINT>(i));
			m_Param.bTex[i] = false;
		}
	}


	CConstBuffer* pCB = CDevice::GetInst()->GetCB(CB_TYPE::SCALAR_PARAM);
	pCB->SetData(&m_Param, sizeof(tScalarParam));
	pCB->UpdateData();

	// Dynamic Shader 
	if (m_bUseDynamicShader && m_pDynamicShader)
	{
		m_pDynamicShader->UpdateData();
	}
	// Common Shader
	else
	{
		if (nullptr != m_pShader)
		{
			m_pShader->UpdateData();
		}
	}
}

CMaterial* CMaterial::GetMtrlInst()
{
	CMaterial* pCloneMtrl     = Clone();
	pCloneMtrl->m_pMasterMtrl = this;
	return pCloneMtrl;
}

void CMaterial::SetShader(Ptr<CGraphicsShader> _pShader)
{
	m_pShader = _pShader;

	if (nullptr != m_pShader)
	{
		m_vecScalarParamInfo = m_pShader->GetScalarParamInfo();
		m_vecTexParamInfo    = m_pShader->GetTexParamInfo();
	}
}

void CMaterial::SetScalarParam(SCALAR_PARAM _eType, void* _pData)
{
	switch (_eType)
	{
	case SCALAR_PARAM::INT_0:
	case SCALAR_PARAM::INT_1:
	case SCALAR_PARAM::INT_2:
	case SCALAR_PARAM::INT_3:
		m_Param.iArr[static_cast<UINT>(_eType) - static_cast<UINT>(SCALAR_PARAM::INT_0)] = *static_cast<int*>(_pData);
		break;
	case SCALAR_PARAM::FLOAT_0:
	case SCALAR_PARAM::FLOAT_1:
	case SCALAR_PARAM::FLOAT_2:
	case SCALAR_PARAM::FLOAT_3:
		m_Param.fArr[static_cast<UINT>(_eType) - static_cast<UINT>(SCALAR_PARAM::FLOAT_0)] = *static_cast<float*>(
			_pData);
		break;
	case SCALAR_PARAM::VEC2_0:
	case SCALAR_PARAM::VEC2_1:
	case SCALAR_PARAM::VEC2_2:
	case SCALAR_PARAM::VEC2_3:
		m_Param.v2Arr[static_cast<UINT>(_eType) - static_cast<UINT>(SCALAR_PARAM::VEC2_0)] = *static_cast<Vec2*>(
			_pData);
		break;
	case SCALAR_PARAM::VEC4_0:
	case SCALAR_PARAM::VEC4_1:
	case SCALAR_PARAM::VEC4_2:
	case SCALAR_PARAM::VEC4_3:
		m_Param.v4Arr[static_cast<UINT>(_eType) - static_cast<UINT>(SCALAR_PARAM::VEC4_0)] = *static_cast<Vec4*>(
			_pData);
		break;
	case SCALAR_PARAM::MAT_0:
	case SCALAR_PARAM::MAT_1:
	case SCALAR_PARAM::MAT_2:
	case SCALAR_PARAM::MAT_3:
		m_Param.matArr[static_cast<UINT>(_eType) - static_cast<UINT>(SCALAR_PARAM::MAT_0)] = *static_cast<Matrix*>(
			_pData);
		break;
	}
	Changed();
}

void CMaterial::SetScalarParam(const wstring& _strParamName, void* _pData)
{
	for (size_t i = 0; i < m_vecScalarParamInfo.size(); ++i)
	{
		if (m_vecScalarParamInfo[i].strDesc == _strParamName)
		{
			SetScalarParam(m_vecScalarParamInfo[i].eScalarParam, _pData);
			break;
		}
	}
	Changed();
}

CGraphicsShader* CMaterial::GetDynamicShader()
{
	if (m_pDynamicShader)
	{
		return m_pDynamicShader;
	}

	m_pDynamicShader = CreateDynamicShader();
	return m_pDynamicShader;
}


CGraphicsShader* CMaterial::CreateDynamicShader()
{
	if (m_pShader == nullptr)
	{
		return nullptr;
	}
	if (m_pDynamicShader != nullptr)
		return nullptr;

	auto                      pDynamicShader    = new CGraphicsShader;
	const tShaderCompileInfo* ShaderCompileInfo = m_pShader->GetCompileShaderInfo();

	// [ CREATE SHADER ]
	if (ShaderCompileInfo[static_cast<UINT>(SHADER_TYPE::VERTEX_SHADER)].bExist)
	{
		wstring strRelPath = ShaderCompileInfo[static_cast<UINT>(SHADER_TYPE::VERTEX_SHADER)].strShaderRelativePath;
		string  strFunc    = ShaderCompileInfo[static_cast<UINT>(SHADER_TYPE::VERTEX_SHADER)].strShaderFunc;
		pDynamicShader->CreateVertexShader(strRelPath, strFunc);
	}
	if (ShaderCompileInfo[static_cast<UINT>(SHADER_TYPE::HULL_SHADER)].bExist)
	{
		wstring strRelPath = ShaderCompileInfo[static_cast<UINT>(SHADER_TYPE::HULL_SHADER)].strShaderRelativePath;
		string  strFunc    = ShaderCompileInfo[static_cast<UINT>(SHADER_TYPE::HULL_SHADER)].strShaderFunc;
		pDynamicShader->CreateHullShader(strRelPath, strFunc);
	}

	if (ShaderCompileInfo[static_cast<UINT>(SHADER_TYPE::DOMAIN_SHADER)].bExist)
	{
		wstring strRelPath = ShaderCompileInfo[static_cast<UINT>(SHADER_TYPE::DOMAIN_SHADER)].strShaderRelativePath;
		string  strFunc    = ShaderCompileInfo[static_cast<UINT>(SHADER_TYPE::DOMAIN_SHADER)].strShaderFunc;
		pDynamicShader->CreateDomainShader(strRelPath, strFunc);
	}
	if (ShaderCompileInfo[static_cast<UINT>(SHADER_TYPE::GEOMETRY_SHADER)].bExist)
	{
		wstring strRelPath = ShaderCompileInfo[static_cast<UINT>(SHADER_TYPE::GEOMETRY_SHADER)].strShaderRelativePath;
		string  strFunc    = ShaderCompileInfo[static_cast<UINT>(SHADER_TYPE::GEOMETRY_SHADER)].strShaderFunc;
		pDynamicShader->CreateGeometryShader(strRelPath, strFunc);
	}
	if (ShaderCompileInfo[static_cast<UINT>(SHADER_TYPE::PIXEL_SHADER)].bExist)
	{
		wstring strRelPath = ShaderCompileInfo[static_cast<UINT>(SHADER_TYPE::PIXEL_SHADER)].strShaderRelativePath;
		string  strFunc    = ShaderCompileInfo[static_cast<UINT>(SHADER_TYPE::PIXEL_SHADER)].strShaderFunc;
		pDynamicShader->CreatePixelShader(strRelPath, strFunc);
	}

	pDynamicShader->SetRSType(m_pShader->GetRSType());
	pDynamicShader->SetDSType(m_pShader->GetDSType());
	pDynamicShader->SetBSType(m_pShader->GetBSType());
	pDynamicShader->SetShaderDomain(m_pShader->GetShaderDomain());
	pDynamicShader->CopyScalarParamInfo(m_vecScalarParamInfo);
	pDynamicShader->CopyTexParamInfo(m_vecTexParamInfo);

	m_pDynamicShader = pDynamicShader;

	return pDynamicShader;
}

const void* CMaterial::GetScalarParam(SCALAR_PARAM _eType)
{
	switch (_eType)
	{
	case SCALAR_PARAM::INT_0:
	case SCALAR_PARAM::INT_1:
	case SCALAR_PARAM::INT_2:
	case SCALAR_PARAM::INT_3:
		return &m_Param.iArr[static_cast<UINT>(_eType) - static_cast<UINT>(SCALAR_PARAM::INT_0)];
		break;
	case SCALAR_PARAM::FLOAT_0:
	case SCALAR_PARAM::FLOAT_1:
	case SCALAR_PARAM::FLOAT_2:
	case SCALAR_PARAM::FLOAT_3:
		return &m_Param.fArr[static_cast<UINT>(_eType) - static_cast<UINT>(SCALAR_PARAM::FLOAT_0)];
		break;
	case SCALAR_PARAM::VEC2_0:
	case SCALAR_PARAM::VEC2_1:
	case SCALAR_PARAM::VEC2_2:
	case SCALAR_PARAM::VEC2_3:
		return &m_Param.v2Arr[static_cast<UINT>(_eType) - static_cast<UINT>(SCALAR_PARAM::VEC2_0)];
		break;
	case SCALAR_PARAM::VEC4_0:
	case SCALAR_PARAM::VEC4_1:
	case SCALAR_PARAM::VEC4_2:
	case SCALAR_PARAM::VEC4_3:
		return &m_Param.v4Arr[static_cast<UINT>(_eType) - static_cast<UINT>(SCALAR_PARAM::VEC4_0)];
		break;
	case SCALAR_PARAM::MAT_0:
	case SCALAR_PARAM::MAT_1:
	case SCALAR_PARAM::MAT_2:
	case SCALAR_PARAM::MAT_3:
		return &m_Param.matArr[static_cast<UINT>(_eType) - static_cast<UINT>(SCALAR_PARAM::MAT_0)];
		break;
	}

	return nullptr;
}

Ptr<CTexture> CMaterial::GetTexParam(TEX_PARAM _eType)
{
	switch (_eType)
	{
	case TEX_PARAM::TEX_0:
	case TEX_PARAM::TEX_1:
	case TEX_PARAM::TEX_2:
	case TEX_PARAM::TEX_3:
	case TEX_PARAM::TEX_4:
	case TEX_PARAM::TEX_5:
	case TEX_PARAM::TEX_CUBE_0:
	case TEX_PARAM::TEX_CUBE_1:
	case TEX_PARAM::TEX_ARR_0:
	case TEX_PARAM::TEX_ARR_1:
		return m_arrTex[static_cast<UINT>(_eType)];
		break;
	}


	return nullptr;
}


void CMaterial::SetTexParam(TEX_PARAM _eType, Ptr<CTexture> _pTex)
{
	switch (_eType)
	{
	case TEX_PARAM::TEX_0:
	case TEX_PARAM::TEX_1:
	case TEX_PARAM::TEX_2:
	case TEX_PARAM::TEX_3:
	case TEX_PARAM::TEX_4:
	case TEX_PARAM::TEX_5:
	case TEX_PARAM::TEX_CUBE_0:
	case TEX_PARAM::TEX_CUBE_1:
	case TEX_PARAM::TEX_ARR_0:
	case TEX_PARAM::TEX_ARR_1:
		m_arrTex[static_cast<UINT>(_eType)] = _pTex;
		break;
	}

	Changed();
}

void CMaterial::SetTexParam(const wstring& _strParamName, Ptr<CTexture> _pTex)
{
	for (size_t i = 0; i < m_vecTexParamInfo.size(); ++i)
	{
		if (m_vecTexParamInfo[i].strDesc == _strParamName)
		{
			SetTexParam(m_vecTexParamInfo[i].eTexParam, _pTex);
			break;
		}
	}
	Changed();
}

int CMaterial::Save(const wstring& _strFilePath)
{
	const CScene* pCurScene = CSceneMgr::GetInst()->GetCurScene();
	assert(!((m_pMasterMtrl && pCurScene->GetSceneState() != SCENE_STATE::STOP)));

	if (m_pMasterMtrl && pCurScene->GetSceneState() != SCENE_STATE::STOP)
	{
		return E_FAIL;
	}
	// 변경체크 해제
	CRes::Save(_strFilePath);

	FILE* pFile = nullptr;
	_wfopen_s(&pFile, _strFilePath.c_str(), L"wb");

	if (nullptr == pFile)
	{
		return E_FAIL;
	}

	fwrite(&m_Param, sizeof(tScalarParam), 1, pFile);
	for (size_t i = 0; i < std::size(m_arrTex); ++i)
	{
		SaveResPtr(m_arrTex[i], pFile);
	}

	SaveResPtr(m_pShader, pFile);

	fclose(pFile);

	return S_OK;
}

int CMaterial::Load(const wstring& _strFilePath)
{
	const CScene* pCurScene = CSceneMgr::GetInst()->GetCurScene();
	assert(!((m_pMasterMtrl && pCurScene->GetSceneState() != SCENE_STATE::STOP)));

	if (m_pMasterMtrl && pCurScene->GetSceneState() != SCENE_STATE::STOP)
	{
		return E_FAIL;
	}

	FILE* pFile = nullptr;
	_wfopen_s(&pFile, _strFilePath.c_str(), L"rb");
	//assert(pFile);

	if (nullptr == pFile)
	{
		return E_FAIL;
	}

	fread(&m_Param, sizeof(tScalarParam), 1, pFile);
	for (size_t i = 0; i < std::size(m_arrTex); ++i)
	{
		LoadResPtr(m_arrTex[i], pFile);
	}

	Ptr<CGraphicsShader> pShader;
	LoadResPtr(pShader, pFile);
	SetShader(pShader);

	fclose(pFile);

	return S_OK;
}

void CMaterial::Serialize(YAML::Emitter& emitter)
{
	for (auto& texInfo : m_vecTexParamInfo)
	{
		Ptr<CTexture> pTexture = m_arrTex[static_cast<UINT>(texInfo.eTexParam)];
		if (nullptr != pTexture)
		{
			emitter << YAML::Key << static_cast<int>(texInfo.eTexParam)
				<< YAML::Value << ToString(pTexture->GetRelativePath());
		}
	}
}

void CMaterial::Deserialize(const YAML::Node& node)
{
	if (node.IsDefined())
	{
		for (const auto& texNode : node)
		{
			auto         param                 = static_cast<TEX_PARAM>(texNode.first.as<int>());
			std::wstring texturePath           = ToWString(texNode.second.as<std::string>());
			m_arrTex[static_cast<UINT>(param)] = CResMgr::GetInst()->Load<CTexture>(texturePath, texturePath);
		}
	}
}
