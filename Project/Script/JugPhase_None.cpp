#include "pch.h"
#include "JugPhase_None.h"

JugPhase_None::JugPhase_None()
	: CState(L"JUG_PHASE_NONE")
{
}

JugPhase_None::JugPhase_None(const JugPhase_None& _origin)
	: CState(_origin.m_sStateType)
{
}

JugPhase_None::~JugPhase_None()
{
}

void JugPhase_None::Enter()
{
}

void JugPhase_None::Exit()
{
}

void JugPhase_None::Update()
{
	CState::Update();
}

void JugPhase_None::LateUpdate()
{
	CState::LateUpdate();
}

