#pragma once
#include "CRes.h"
#include "CFBXLoader.h"

class CStructuredBuffer;

struct tIndexInfo
{
	ComPtr<ID3D11Buffer> pIB;
	D3D11_BUFFER_DESC    tIBDesc;
	UINT                 iIdxCount;
	void*                pIdxSysMem;
};

class CMesh : public CRes
{
private:
	ComPtr<ID3D11Buffer> m_VB;
	D3D11_BUFFER_DESC    m_tVBDesc;
	UINT                 m_iVtxCount;
	void*                m_pVtxSys;

	// 하나의 버텍스버퍼에 여러개의 인덱스버퍼가 연결
	vector<tIndexInfo> m_vecIdxInfo;

	// Animation3D 정보
	vector<tMTAnimClip> m_vecAnimClip;
	vector<tMTBone>     m_vecBones;

	CStructuredBuffer* m_pBoneFrameData; // 전체 본 프레임 정보(크기, 이동, 회전) (프레임 개수만큼)
	CStructuredBuffer* m_pBoneOffset;    // 각 뼈의 offset 행렬(각 뼈의 위치를 되돌리는 행렬) (1행 짜리)

public:
	void render(UINT _iSubset);
	void render_particle(UINT _iCount);

	void UpdateData(UINT _iSubset);
	void UpdateVertexBuffer_Dynamic();

public:
	int           Create(const void* _pVtxSys, UINT _iVtxCount, const void* _pIdxSys, UINT _iIdxCount);
	static CMesh* CreateFromContainer(CFBXLoader& _loader, int ContainerIdx);;
	int           CreateBuffer_Dynamic(void* _pVtxSys, UINT _iVtxCount, void* _pIdxSys, UINT _iIdxCount);

public:
	int                        GetVtxCount() const { return m_iVtxCount; }
	Vtx*                       GetVtxSysMem() const { return static_cast<Vtx*>(m_pVtxSys); }
	UINT                       GetSubsetCount() const { return static_cast<UINT>(m_vecIdxInfo.size()); }
	const tIndexInfo&          GetIndexInfo(int index) const { return m_vecIdxInfo[index]; }
	const vector<tMTBone>*     GetBones() const { return &m_vecBones; }
	UINT                       GetBoneCount() const { return static_cast<UINT>(m_vecBones.size()); }
	const vector<tMTAnimClip>* GetAnimClip() const { return &m_vecAnimClip; }
	bool                       IsAnimMesh() const { return !m_vecAnimClip.empty(); }

	CStructuredBuffer* GetBoneFrameDataBuffer() const { return m_pBoneFrameData; } // 전체 본 프레임 정보
	CStructuredBuffer* GetBoneOffsetBuffer() const { return m_pBoneOffset; }       // 각 뼈의 offset 행렬	

public:
	int Save(const wstring& _strFilePath) override;
	int Load(const wstring& _strFilePath) override;

public:
	CMesh();
	virtual ~CMesh();
};
