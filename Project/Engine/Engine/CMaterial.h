#pragma once
#include "CRes.h"

#include "CGraphicsShader.h"
#include "CTexture.h"
#include "CSerializer.h"
class CMaterial : public CRes
{
private:
	tScalarParam         m_Param;
	Ptr<CTexture>        m_arrTex[static_cast<UINT>(TEX_PARAM::END)];
	Ptr<CGraphicsShader> m_pShader;

	vector<tScalarParamInfo> m_vecScalarParamInfo;
	vector<tTexParamInfo>    m_vecTexParamInfo;

	CMaterial*       m_pMasterMtrl;
	CGraphicsShader* m_pDynamicShader;
	bool             m_bUseDynamicShader;

public:
	void                 SetShader(Ptr<CGraphicsShader> _pShader);
	Ptr<CGraphicsShader> GetShader() { return m_pShader; }
	CGraphicsShader*     GetDynamicShader();
	CGraphicsShader*     CreateDynamicShader();
	void                 SetUseDynamicShader(bool _bUseDynamicShader) { m_bUseDynamicShader = _bUseDynamicShader; }
	bool                 IsUseDynamicShader() const { return m_bUseDynamicShader; }

	void SetMaterialCoefficient(Vec4 _vDiff, Vec4 _vSpec, Vec4 _vAmb, Vec4 _vEmis)
	{
		m_Param.mtrl.vDiff = _vDiff;
		m_Param.mtrl.vAmb  = _vAmb;
		m_Param.mtrl.vSpec = _vSpec;
		m_Param.mtrl.vEmv  = _vEmis;
	}

	void SetScalarParam(SCALAR_PARAM _eType, void* _pData);
	void SetScalarParam(const wstring& _strParamName, void* _pData);

	void SetTexParam(TEX_PARAM _eType, Ptr<CTexture> _pTex);
	void SetTexParam(const wstring& _strParamName, Ptr<CTexture> _pTex);

	void SetAnim3D(bool _bTrue) { m_Param.arrAnimData[0] = (int)_bTrue; }
	void SetBoneCount(int _iBoneCount) { m_Param.arrAnimData[1] = _iBoneCount; }

	const void*   GetScalarParam(SCALAR_PARAM _eType);
	Ptr<CTexture> GetTexParam(TEX_PARAM _eType);

	void       UpdateData() override;
	CMaterial* GetMtrlInst();
	CMaterial* GetMasterMtrl() const { return m_pMasterMtrl; }

public:
	void Serialize(YAML::Emitter& emitter) override;
	void Deserialize(const YAML::Node& node) override;

public:
	int Save(const wstring& _strFilePath) override;
	int Load(const wstring& _strFilePath) override;

public:
	CLONE(CMaterial);
	CMaterial();
	virtual ~CMaterial();
};
