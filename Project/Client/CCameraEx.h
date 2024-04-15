#pragma once
#include <Engine/CCamera.h>

class CCameraEx
	: public CCamera
{
public:
	void finalupdate() override;

public:
	CCameraEx();
	virtual ~CCameraEx();
};
