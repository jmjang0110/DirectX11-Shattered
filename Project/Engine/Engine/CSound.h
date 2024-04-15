#pragma once
#include "CRes.h"

#include <FMOD/fmod.h>
#include <FMOD/fmod.hpp>
#include <FMOD/fmod_codec.h>

#ifdef _DEBUG
#pragma comment(lib, "FMOD/fmodL64_vc.lib")
#else
#pragma comment(lib, "FMOD/fmod64_vc.lib")
#endif

class CSound : public CRes
{
public:
	static FMOD::System* g_pFMOD;

private:
	FMOD::Sound*         m_pSound;
	list<FMOD::Channel*> m_listChannel;

public:
	// 0 (무한반복) 0 ~ 1(Volume)
	int  Play(int _iRoopCount, float _fVolume = 1.f, bool _bOverlap = false);
	void Stop();

	// 0 ~ 1
	void SetVolume(float _volume, int _iChannelIndex);

private:
	void               RemoveChannel(const FMOD::Channel* _pTargetChannel);
	friend FMOD_RESULT CHANNEL_CALLBACK(FMOD_CHANNELCONTROL*              channelcontrol,
	                                    FMOD_CHANNELCONTROL_TYPE          controltype,
	                                    FMOD_CHANNELCONTROL_CALLBACK_TYPE callbacktype,
	                                    void*                             commanddata1,
	                                    void*                             commanddata2);

public:
	void UpdateData() override {}
	int  Save(const wstring& _strRelativePath) override { return 0; }
	int  Load(const wstring& _strFilePath) override;

public:
	CSound();
	virtual ~CSound();
};
