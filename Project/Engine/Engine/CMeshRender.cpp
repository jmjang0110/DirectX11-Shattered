#include "pch.h"
#include "CMeshRender.h"

#include "CMesh.h"
#include "CMaterial.h"

#include "CTransform.h"
#include "CAnimator2D.h"
#include "CAnimator3D.h"

CMeshRender::CMeshRender()
	: CRenderComponent(COMPONENT_TYPE::MESHRENDER) {}

CMeshRender::~CMeshRender() = default;

void CMeshRender::finalupdate() {}

void CMeshRender::render()
{
	if (nullptr == GetMesh() || nullptr == GetMaterial(0))
	{
		return;
	}

	// ==========
	// UpdateData
	// ==========

	Transform()->UpdateData();

	if (Animator2D())
	{
		Animator2D()->UpdateData();
	}

	// Animator3D 업데이트
	if (Animator3D())
	{
		Animator3D()->UpdateData();

		for (UINT i = 0; i < GetMtrlCount(); ++i)
		{
			if (nullptr == GetMaterial(i))
				continue;
			GetMaterial(i)->SetAnim3D(true); // Animation Mesh 알리기
			GetMaterial(i)->SetBoneCount(Animator3D()->GetBoneCount());
		}
	}

	// ======
	// Render
	// ======
	UINT iSubsetCount = GetMesh()->GetSubsetCount();
	for (UINT i = 0; i < iSubsetCount; ++i)
	{
		if (nullptr == GetMaterial(i))
			continue;

		GetMaterial(i)->UpdateData();
		GetMesh()->render(i);
	}

	// =====
	// Clear
	// =====
	if (Animator2D())
		CAnimator2D::Clear();

	if (Animator3D())
		Animator3D()->ClearData();
}

void CMeshRender::SaveToScene(FILE* _pFile)
{
	CRenderComponent::SaveToScene(_pFile);
}

void CMeshRender::LoadFromScene(FILE* _pFile)
{
	CRenderComponent::LoadFromScene(_pFile);
}
