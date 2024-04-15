#include "pch.h"
#include "CMesh.h"

#include "CDevice.h"
#include "CStructuredBuffer.h"
#undef max

CMesh::CMesh()
	: CRes(RES_TYPE::MESH)
	, m_tVBDesc{}
	, m_iVtxCount(0)
	, m_pVtxSys(nullptr)
	, m_pBoneFrameData{nullptr}
	, m_pBoneOffset{nullptr} {}

CMesh::~CMesh()
{
	SAFE_DELETE(m_pVtxSys);

	for (size_t i = 0; i < m_vecIdxInfo.size(); ++i)
	{
		SAFE_DELETE(m_vecIdxInfo[i].pIdxSysMem);
	}

	// Animation3D 정보
	SAFE_DELETE(m_pBoneFrameData);
	SAFE_DELETE(m_pBoneOffset);
}

int CMesh::Create(const void* _pVtxSys, UINT _iVtxCount, const void* _pIdxSys, UINT _iIdxCount)
{
	m_iVtxCount = _iVtxCount;

	tIndexInfo IndexInfo = {};
	IndexInfo.iIdxCount  = _iIdxCount;

	// 정점 데이터를 저장할 버텍스 버퍼를 생성한다.	
	m_tVBDesc.ByteWidth = sizeof(Vertex) * _iVtxCount;

	// 정점 버퍼는 처음 생성이후에 버퍼를 수정하지 않는다.
	m_tVBDesc.CPUAccessFlags = 0;
	m_tVBDesc.Usage          = D3D11_USAGE_DEFAULT;

	// 정점을 저장하는 목적의 버퍼 임을 알림
	m_tVBDesc.BindFlags           = D3D11_BIND_VERTEX_BUFFER;
	m_tVBDesc.MiscFlags           = 0;
	m_tVBDesc.StructureByteStride = 0;

	// 초기 데이터를 넘겨주기 위한 정보 구조체
	D3D11_SUBRESOURCE_DATA tSubDesc = {};
	tSubDesc.pSysMem                = _pVtxSys;

	if (FAILED(DEVICE->CreateBuffer(&m_tVBDesc, &tSubDesc, m_VB.GetAddressOf())))
	{
		return E_FAIL;
	}


	IndexInfo.tIBDesc.ByteWidth = sizeof(UINT) * _iIdxCount;

	// 버퍼 생성 이후에도, 버퍼의 내용을 수정 할 수 있는 옵션
	IndexInfo.tIBDesc.CPUAccessFlags = 0;
	IndexInfo.tIBDesc.Usage          = D3D11_USAGE_DEFAULT;

	// 정점을 저장하는 목적의 버퍼 임을 알림
	IndexInfo.tIBDesc.BindFlags           = D3D11_BIND_INDEX_BUFFER;
	IndexInfo.tIBDesc.MiscFlags           = 0;
	IndexInfo.tIBDesc.StructureByteStride = 0;

	// 초기 데이터를 넘겨주기 위한 정보 구조체
	tSubDesc         = {};
	tSubDesc.pSysMem = _pIdxSys;

	if (FAILED(DEVICE->CreateBuffer(&IndexInfo.tIBDesc, &tSubDesc, IndexInfo.pIB.GetAddressOf())))
	{
		return E_FAIL;
	}

	// 시스템 메모리 할당
	m_pVtxSys = new Vtx[m_iVtxCount];
	memcpy(m_pVtxSys, _pVtxSys, sizeof(Vtx) * m_iVtxCount);

	IndexInfo.pIdxSysMem = new UINT[IndexInfo.iIdxCount];
	memcpy(IndexInfo.pIdxSysMem, _pIdxSys, sizeof(UINT) * IndexInfo.iIdxCount);

	m_vecIdxInfo.push_back(IndexInfo);

	return S_OK;
}


CMesh* CMesh::CreateFromContainer(CFBXLoader& _loader, int ContainerIdx)
{
	const tContainer* container = &_loader.GetContainer(ContainerIdx);

	UINT iVtxCount = static_cast<UINT>(container->vecPos.size());

	D3D11_BUFFER_DESC tVtxDesc = {};

	tVtxDesc.ByteWidth = sizeof(Vtx) * iVtxCount;
	tVtxDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;

	tVtxDesc.Usage = D3D11_USAGE_DEFAULT;
	if (D3D11_USAGE_DYNAMIC == tVtxDesc.Usage)
	{
		tVtxDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	}

	D3D11_SUBRESOURCE_DATA tSub = {};
	tSub.pSysMem                = malloc(tVtxDesc.ByteWidth);
	auto pSys                   = (Vtx*)tSub.pSysMem;
	for (UINT i = 0; i < iVtxCount; ++i)
	{
		pSys[i].vPos      = container->vecPos[i];
		pSys[i].vUV       = container->vecUV[i];
		pSys[i].vColor    = Vec4(1.f, 0.f, 1.f, 1.f);
		pSys[i].vNormal   = container->vecNormal[i];
		pSys[i].vTangent  = container->vecTangent[i];
		pSys[i].vBinormal = container->vecBinormal[i];
		pSys[i].vWeights  = container->vecWeights[i];
		pSys[i].vIndices  = container->vecIndices[i];
	}
	ComPtr<ID3D11Buffer> pVB = nullptr;
	if (FAILED(DEVICE->CreateBuffer(&tVtxDesc, &tSub, pVB.GetAddressOf())))
	{
		return nullptr;
	}

	auto pMesh       = new CMesh;
	pMesh->m_VB      = pVB;
	pMesh->m_tVBDesc = tVtxDesc;
	pMesh->m_pVtxSys = pSys;

	// 인덱스 정보
	UINT              iIdxBufferCount = static_cast<UINT>(container->vecIdx.size());
	D3D11_BUFFER_DESC tIdxDesc        = {};

	for (UINT i = 0; i < iIdxBufferCount; ++i)
	{
		tIdxDesc.ByteWidth = static_cast<UINT>(container->vecIdx[i].size()) * sizeof(UINT);
		// Index Format 이 R32_UINT 이기 때문
		tIdxDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
		tIdxDesc.Usage     = D3D11_USAGE_DEFAULT;
		if (D3D11_USAGE_DYNAMIC == tIdxDesc.Usage)
		{
			tIdxDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
		}

		void* pSysMem = malloc(tIdxDesc.ByteWidth);
		memcpy(pSysMem, container->vecIdx[i].data(), tIdxDesc.ByteWidth);
		tSub.pSysMem = pSysMem;

		ComPtr<ID3D11Buffer> pIB = nullptr;
		if (FAILED(DEVICE->CreateBuffer(&tIdxDesc, &tSub, pIB.GetAddressOf())))
		{
			return nullptr;
		}

		tIndexInfo info = {};
		info.tIBDesc    = tIdxDesc;
		info.iIdxCount  = static_cast<UINT>(container->vecIdx[i].size());
		info.pIdxSysMem = pSysMem;
		info.pIB        = pIB;

		pMesh->m_vecIdxInfo.push_back(info);
	}

	// Animation3D
	if (!container->bAnimation)
	{
		return pMesh;
	}

	vector<tBone*>& vecBone     = _loader.GetBones();
	UINT            iFrameCount = 0;
	for (UINT i = 0; i < vecBone.size(); ++i)
	{
		tMTBone bone     = {};
		bone.iDepth      = vecBone[i]->iDepth;
		bone.iParentIndx = vecBone[i]->iParentIndx;
		bone.matBone     = GetMatrixFromFbxMatrix(vecBone[i]->matBone);
		bone.matOffset   = GetMatrixFromFbxMatrix(vecBone[i]->matOffset);
		bone.strBoneName = vecBone[i]->strBoneName;

		for (UINT j = 0; j < vecBone[i]->vecKeyFrame.size(); ++j)
		{
			tMTKeyFrame tKeyframe  = {};
			tKeyframe.dTime        = vecBone[i]->vecKeyFrame[j].dTime;
			tKeyframe.iFrame       = j;
			tKeyframe.vTranslate.x = static_cast<float>(vecBone[i]->vecKeyFrame[j].matTransform.GetT().mData[0]);
			tKeyframe.vTranslate.y = static_cast<float>(vecBone[i]->vecKeyFrame[j].matTransform.GetT().mData[1]);
			tKeyframe.vTranslate.z = static_cast<float>(vecBone[i]->vecKeyFrame[j].matTransform.GetT().mData[2]);

			tKeyframe.vScale.x = static_cast<float>(vecBone[i]->vecKeyFrame[j].matTransform.GetS().mData[0]);
			tKeyframe.vScale.y = static_cast<float>(vecBone[i]->vecKeyFrame[j].matTransform.GetS().mData[1]);
			tKeyframe.vScale.z = static_cast<float>(vecBone[i]->vecKeyFrame[j].matTransform.GetS().mData[2]);

			tKeyframe.qRot.x = static_cast<float>(vecBone[i]->vecKeyFrame[j].matTransform.GetQ().mData[0]);
			tKeyframe.qRot.y = static_cast<float>(vecBone[i]->vecKeyFrame[j].matTransform.GetQ().mData[1]);
			tKeyframe.qRot.z = static_cast<float>(vecBone[i]->vecKeyFrame[j].matTransform.GetQ().mData[2]);
			tKeyframe.qRot.w = static_cast<float>(vecBone[i]->vecKeyFrame[j].matTransform.GetQ().mData[3]);

			bone.vecKeyFrame.push_back(tKeyframe);
		}

		iFrameCount = std::max(iFrameCount, static_cast<UINT>(bone.vecKeyFrame.size()));

		pMesh->m_vecBones.push_back(bone);
	}

	vector<tAnimClip*>& vecAnimClip = _loader.GetAnimClip();

	for (UINT i = 0; i < vecAnimClip.size(); ++i)
	{
		tMTAnimClip tClip = {};

		tClip.strAnimName = vecAnimClip[i]->strName;
		tClip.dStartTime  = vecAnimClip[i]->tStartTime.GetSecondDouble();
		tClip.dEndTime    = vecAnimClip[i]->tEndTime.GetSecondDouble();
		tClip.dTimeLength = tClip.dEndTime - tClip.dStartTime;

		tClip.iStartFrame  = static_cast<int>(vecAnimClip[i]->tStartTime.GetFrameCount(vecAnimClip[i]->eMode));
		tClip.iEndFrame    = static_cast<int>(vecAnimClip[i]->tEndTime.GetFrameCount(vecAnimClip[i]->eMode));
		tClip.iFrameLength = tClip.iEndFrame - tClip.iStartFrame;
		tClip.eMode        = vecAnimClip[i]->eMode;

		pMesh->m_vecAnimClip.push_back(tClip);
	}

	// Animation 이 있는 Mesh 경우 structuredbuffer 만들어두기
	if (pMesh->IsAnimMesh())
	{
		// BoneOffet 행렬
		vector<Matrix>      vecOffset;
		vector<tFrameTrans> vecFrameTrans;
		vecFrameTrans.resize(static_cast<UINT>(pMesh->m_vecBones.size()) * iFrameCount);

		for (size_t i = 0; i < pMesh->m_vecBones.size(); ++i)
		{
			vecOffset.push_back(pMesh->m_vecBones[i].matOffset);

			for (size_t j = 0; j < pMesh->m_vecBones[i].vecKeyFrame.size(); ++j)
			{
				vecFrameTrans[static_cast<UINT>(pMesh->m_vecBones.size()) * j + i]
					= tFrameTrans
					{
						Vec4(pMesh->m_vecBones[i].vecKeyFrame[j].vTranslate, 0.f),
						Vec4(pMesh->m_vecBones[i].vecKeyFrame[j].vScale, 0.f), pMesh->m_vecBones[i].vecKeyFrame[j].qRot
					};
			}
		}

		pMesh->m_pBoneOffset = new CStructuredBuffer;
		pMesh->m_pBoneOffset->Create(sizeof(Matrix),
		                             static_cast<UINT>(vecOffset.size()),
		                             SB_TYPE::READ_ONLY,
		                             false,
		                             vecOffset.data());

		pMesh->m_pBoneFrameData = new CStructuredBuffer;
		pMesh->m_pBoneFrameData->Create(sizeof(tFrameTrans),
		                                static_cast<UINT>(vecOffset.size()) * iFrameCount,
		                                SB_TYPE::READ_ONLY,
		                                false,
		                                vecFrameTrans.data());
	}

	return pMesh;
}


void CMesh::render(UINT _iSubset)
{
	UpdateData(_iSubset);

	CONTEXT->DrawIndexed(m_vecIdxInfo[_iSubset].iIdxCount, 0, 0);
}

void CMesh::render_particle(UINT _iCount)
{
	UpdateData(0);

	CONTEXT->DrawIndexedInstanced(m_vecIdxInfo[0].iIdxCount, _iCount, 0, 0, 0);
}

void CMesh::UpdateData(UINT _iSubset)
{
	UINT iStride = sizeof(Vtx);
	UINT iOffset = 0;

	CONTEXT->IASetVertexBuffers(0, 1, m_VB.GetAddressOf(), &iStride, &iOffset);
	CONTEXT->IASetIndexBuffer(m_vecIdxInfo[_iSubset].pIB.Get(), DXGI_FORMAT_R32_UINT, 0);
}


int CMesh::Save(const wstring& _strFilePath)
{
	SetRelativePath(CPathMgr::GetInst()->GetRelativePath(_strFilePath));

	FILE*   pFile = nullptr;
	errno_t err   = _wfopen_s(&pFile, _strFilePath.c_str(), L"wb");

	assert(pFile);

	// 키값, 상대 경로	
	SaveWStringToFile(GetName(), pFile);
	SaveWStringToFile(GetKey(), pFile);
	SaveWStringToFile(GetRelativePath(), pFile);

	// 정점 데이터 저장				
	int iByteSize = m_tVBDesc.ByteWidth;
	fwrite(&iByteSize, sizeof(int), 1, pFile);
	fwrite(m_pVtxSys, iByteSize, 1, pFile);

	// 인덱스 정보
	UINT iMtrlCount = static_cast<UINT>(m_vecIdxInfo.size());
	fwrite(&iMtrlCount, sizeof(int), 1, pFile);

	UINT iIdxBuffSize = 0;
	for (UINT i = 0; i < iMtrlCount; ++i)
	{
		fwrite(&m_vecIdxInfo[i], sizeof(tIndexInfo), 1, pFile);
		fwrite(m_vecIdxInfo[i].pIdxSysMem, m_vecIdxInfo[i].iIdxCount * sizeof(UINT), 1, pFile);
	}


	// Animation3D 정보 
	UINT iCount = static_cast<UINT>(m_vecAnimClip.size());
	fwrite(&iCount, sizeof(int), 1, pFile);
	for (UINT i = 0; i < iCount; ++i)
	{
		SaveWStringToFile(m_vecAnimClip[i].strAnimName, pFile);
		fwrite(&m_vecAnimClip[i].dStartTime, sizeof(double), 1, pFile);
		fwrite(&m_vecAnimClip[i].dEndTime, sizeof(double), 1, pFile);
		fwrite(&m_vecAnimClip[i].dTimeLength, sizeof(double), 1, pFile);
		fwrite(&m_vecAnimClip[i].eMode, sizeof(int), 1, pFile);
		fwrite(&m_vecAnimClip[i].fUpdateTime, sizeof(float), 1, pFile);
		fwrite(&m_vecAnimClip[i].iStartFrame, sizeof(int), 1, pFile);
		fwrite(&m_vecAnimClip[i].iEndFrame, sizeof(int), 1, pFile);
		fwrite(&m_vecAnimClip[i].iFrameLength, sizeof(int), 1, pFile);
	}

	iCount = static_cast<UINT>(m_vecBones.size());
	fwrite(&iCount, sizeof(int), 1, pFile);

	for (UINT i = 0; i < iCount; ++i)
	{
		SaveWStringToFile(m_vecBones[i].strBoneName, pFile);
		fwrite(&m_vecBones[i].iDepth, sizeof(int), 1, pFile);
		fwrite(&m_vecBones[i].iParentIndx, sizeof(int), 1, pFile);
		fwrite(&m_vecBones[i].matBone, sizeof(Matrix), 1, pFile);
		fwrite(&m_vecBones[i].matOffset, sizeof(Matrix), 1, pFile);

		int iFrameCount = static_cast<int>(m_vecBones[i].vecKeyFrame.size());
		fwrite(&iFrameCount, sizeof(int), 1, pFile);

		for (int j = 0; j < m_vecBones[i].vecKeyFrame.size(); ++j)
		{
			fwrite(&m_vecBones[i].vecKeyFrame[j], sizeof(tMTKeyFrame), 1, pFile);
		}
	}


	fclose(pFile);

	return S_OK;
}

int CMesh::Load(const wstring& _strFilePath)
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

	// 정점데이터
	UINT iByteSize = 0;
	fread(&iByteSize, sizeof(int), 1, pFile);

	m_pVtxSys = static_cast<Vtx*>(malloc(iByteSize));
	fread(m_pVtxSys, 1, iByteSize, pFile);


	D3D11_BUFFER_DESC tDesc = {};
	tDesc.BindFlags         = D3D11_BIND_VERTEX_BUFFER;
	tDesc.ByteWidth         = iByteSize;
	tDesc.Usage             = D3D11_USAGE_DEFAULT;

	D3D11_SUBRESOURCE_DATA tSubData = {};
	tSubData.pSysMem                = m_pVtxSys;

	if (FAILED(DEVICE->CreateBuffer(&tDesc, &tSubData, m_VB.GetAddressOf())))
	{
		assert(nullptr);
	}

	// 인덱스 정보
	UINT iMtrlCount = 0;
	fread(&iMtrlCount, sizeof(int), 1, pFile);

	for (UINT i = 0; i < iMtrlCount; ++i)
	{
		tIndexInfo info = {};
		fread(&info, sizeof(tIndexInfo), 1, pFile);

		UINT iByteWidth = info.iIdxCount * sizeof(UINT);

		void* pSysMem   = malloc(iByteWidth);
		info.pIdxSysMem = pSysMem;
		fread(info.pIdxSysMem, iByteWidth, 1, pFile);

		tSubData.pSysMem = info.pIdxSysMem;

		if (FAILED(DEVICE->CreateBuffer(&info.tIBDesc, &tSubData, info.pIB.GetAddressOf())))
		{
			assert(nullptr);
		}

		m_vecIdxInfo.push_back(info);
	}

	// Animation3D 정보 읽기
	int iCount = 0;
	fread(&iCount, sizeof(int), 1, pFile);
	for (int i = 0; i < iCount; ++i)
	{
		tMTAnimClip tClip = {};

		LoadWStringFromFile(tClip.strAnimName, pFile);
		fread(&tClip.dStartTime, sizeof(double), 1, pFile);
		fread(&tClip.dEndTime, sizeof(double), 1, pFile);
		fread(&tClip.dTimeLength, sizeof(double), 1, pFile);
		fread(&tClip.eMode, sizeof(int), 1, pFile);
		fread(&tClip.fUpdateTime, sizeof(float), 1, pFile);
		fread(&tClip.iStartFrame, sizeof(int), 1, pFile);
		fread(&tClip.iEndFrame, sizeof(int), 1, pFile);
		fread(&tClip.iFrameLength, sizeof(int), 1, pFile);

		m_vecAnimClip.push_back(tClip);
	}

	iCount = 0;
	fread(&iCount, sizeof(int), 1, pFile);
	m_vecBones.resize(iCount);

	UINT _iFrameCount = 0;
	for (int i = 0; i < iCount; ++i)
	{
		LoadWStringFromFile(m_vecBones[i].strBoneName, pFile);
		fread(&m_vecBones[i].iDepth, sizeof(int), 1, pFile);
		fread(&m_vecBones[i].iParentIndx, sizeof(int), 1, pFile);
		fread(&m_vecBones[i].matBone, sizeof(Matrix), 1, pFile);
		fread(&m_vecBones[i].matOffset, sizeof(Matrix), 1, pFile);

		UINT iFrameCount = 0;
		fread(&iFrameCount, sizeof(int), 1, pFile);
		m_vecBones[i].vecKeyFrame.resize(iFrameCount);
		_iFrameCount = std::max(_iFrameCount, iFrameCount);
		for (UINT j = 0; j < iFrameCount; ++j)
		{
			fread(&m_vecBones[i].vecKeyFrame[j], sizeof(tMTKeyFrame), 1, pFile);
		}
	}

	// Animation 이 있는 Mesh 경우 Bone StructuredBuffer 만들기
	if (m_vecAnimClip.size() > 0 && m_vecBones.size() > 0)
	{
		wstring strBone = GetName();

		// BoneOffet 행렬
		vector<Matrix>      vecOffset;
		vector<tFrameTrans> vecFrameTrans;
		vecFrameTrans.resize(static_cast<UINT>(m_vecBones.size()) * _iFrameCount);

		for (size_t i = 0; i < m_vecBones.size(); ++i)
		{
			vecOffset.push_back(m_vecBones[i].matOffset);

			for (size_t j = 0; j < m_vecBones[i].vecKeyFrame.size(); ++j)
			{
				vecFrameTrans[static_cast<UINT>(m_vecBones.size()) * j + i]
					= tFrameTrans{
						Vec4(m_vecBones[i].vecKeyFrame[j].vTranslate, 0.f),
						Vec4(m_vecBones[i].vecKeyFrame[j].vScale, 0.f), Vec4(m_vecBones[i].vecKeyFrame[j].qRot)
					};
			}
		}

		m_pBoneOffset = new CStructuredBuffer;
		m_pBoneOffset->Create(sizeof(Matrix),
		                      static_cast<UINT>(vecOffset.size()),
		                      SB_TYPE::READ_ONLY,
		                      false,
		                      vecOffset.data());

		m_pBoneFrameData = new CStructuredBuffer;
		m_pBoneFrameData->Create(sizeof(tFrameTrans),
		                         static_cast<UINT>
		                         (vecOffset.size()) * _iFrameCount,
		                         SB_TYPE::READ_ONLY,
		                         false,
		                         vecFrameTrans.data());
	}

	fclose(pFile);

	return S_OK;
}

int CMesh::CreateBuffer_Dynamic(void* _pVtxSys, UINT _iVtxCount, void* _pIdxSys, UINT _iIdxCount)
{
	m_iVtxCount = _iVtxCount;

	tIndexInfo IndexInfo = {};
	IndexInfo.iIdxCount  = _iIdxCount;

	// 정점 데이터를 저장할 버텍스 버퍼를 생성한다.	
	m_tVBDesc.ByteWidth = sizeof(Vertex) * _iVtxCount;

	// 정점 버퍼는 처음 생성이후에 버퍼를 수정하지 않는다.
	m_tVBDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	m_tVBDesc.Usage          = D3D11_USAGE_DYNAMIC;

	// 정점을 저장하는 목적의 버퍼 임을 알림
	m_tVBDesc.BindFlags           = D3D11_BIND_VERTEX_BUFFER;
	m_tVBDesc.MiscFlags           = 0;
	m_tVBDesc.StructureByteStride = 0;

	// 초기 데이터를 넘겨주기 위한 정보 구조체
	D3D11_SUBRESOURCE_DATA tSubDesc = {};
	tSubDesc.pSysMem                = _pVtxSys;

	if (FAILED(DEVICE->CreateBuffer(&m_tVBDesc, &tSubDesc, m_VB.GetAddressOf())))
	{
		return E_FAIL;
	}

	IndexInfo.tIBDesc.ByteWidth = sizeof(UINT) * _iIdxCount;

	// 버퍼 생성 이후에도, 버퍼의 내용을 수정 할 수 있는 옵션
	IndexInfo.tIBDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	IndexInfo.tIBDesc.Usage          = D3D11_USAGE_DYNAMIC;

	// 정점을 저장하는 목적의 버퍼 임을 알림
	IndexInfo.tIBDesc.BindFlags           = D3D11_BIND_INDEX_BUFFER;
	IndexInfo.tIBDesc.MiscFlags           = 0;
	IndexInfo.tIBDesc.StructureByteStride = 0;

	// 초기 데이터를 넘겨주기 위한 정보 구조체
	tSubDesc         = {};
	tSubDesc.pSysMem = _pIdxSys;

	if (FAILED(DEVICE->CreateBuffer(&IndexInfo.tIBDesc, &tSubDesc, IndexInfo.pIB.GetAddressOf())))
	{
		return E_FAIL;
	}

	// 시스템 메모리 할당
	m_pVtxSys = new Vtx[m_iVtxCount]{};
	memcpy(m_pVtxSys, _pVtxSys, sizeof(Vtx) * m_iVtxCount);

	IndexInfo.pIdxSysMem = new UINT[IndexInfo.iIdxCount]{};
	memcpy(IndexInfo.pIdxSysMem, _pIdxSys, sizeof(UINT) * IndexInfo.iIdxCount);

	m_vecIdxInfo.push_back(IndexInfo);

	return S_OK;
}

void CMesh::UpdateVertexBuffer_Dynamic()
{
	if (D3D11_USAGE_DYNAMIC == m_tVBDesc.Usage)
	{
		// 정점 버퍼 ( CPU -> GPU )
		D3D11_MAPPED_SUBRESOURCE mappedResource{};
		//  Disable GPU access to the vertex buffer data.
		CONTEXT->Map(m_VB.Get(), 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
		//  Update the vertex buffer here.
		memcpy(mappedResource.pData, m_pVtxSys, sizeof(Vtx) * m_iVtxCount);
		//  Reenable GPU access to the vertex buffer data.
		CONTEXT->Unmap(m_VB.Get(), 0);
	}
}
