#include "pch.h"
#include "CCameraEx.h"

#include <Engine/CTransform.h>

CCameraEx::CCameraEx() {}

CCameraEx::~CCameraEx() {}

void CCameraEx::finalupdate()
{
	finalupdate_module();

	m_Frustum.finalupdate();
}
