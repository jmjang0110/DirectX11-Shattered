#include "pch.h"
#include "CSound.h"

#include "CResMgr.h"

namespace SOUND
{
	constexpr int INVALID_INDEX = -1;
}

FMOD_RESULT CHANNEL_CALLBACK(FMOD_CHANNELCONTROL*              channelcontrol,
                             FMOD_CHANNELCONTROL_TYPE          controltype,
                             FMOD_CHANNELCONTROL_CALLBACK_TYPE callbacktype,
                             void*                             commanddata1,
                             void*                             commanddata2);

FMOD::System* CSound::g_pFMOD = nullptr;

CSound::CSound()
	: CRes(RES_TYPE::SOUND)
	, m_pSound(nullptr) {}

CSound::~CSound()
{
	if (nullptr != m_pSound)
	{
		m_pSound->release();
		m_pSound = nullptr;
	}
}

int CSound::Play(int _iRoopCount, float _fVolume, bool _bOverlap)
{
	if (_iRoopCount < 0)
	{
		assert(nullptr);
	}

	// 재생되고 있는 채널이 있는데, 중복재생을 허용하지 않았다 -> 재생 안함
	if (!_bOverlap && !m_listChannel.empty())
	{
		return SOUND::INVALID_INDEX;
	}
	--_iRoopCount;

	FMOD::Channel* pChannel{};
	g_pFMOD->playSound(m_pSound, nullptr, false, &pChannel);

	pChannel->setVolume(_fVolume);
	pChannel->setCallback(CHANNEL_CALLBACK);
	pChannel->setUserData(this);
	pChannel->setMode(FMOD_LOOP_NORMAL);
	pChannel->setLoopCount(_iRoopCount);

	m_listChannel.push_back(pChannel);

	int iIndex = SOUND::INVALID_INDEX;
	pChannel->getIndex(&iIndex);

	return iIndex;
}

void CSound::Stop()
{
	list<FMOD::Channel*>::iterator iter;

	while (!m_listChannel.empty())
	{
		iter = m_listChannel.begin();
		(*iter)->stop();
	}
}

void CSound::SetVolume(float _volume, int _iChannelIndex)
{
	const auto iter = std::find_if(m_listChannel.begin(),
	                               m_listChannel.end(),
	                               [_iChannelIndex](FMOD::Channel* pChannel)
	                               {
		                               int iIndex{};
		                               pChannel->getIndex(&iIndex);
		                               return _iChannelIndex == iIndex;
	                               });

	if (iter != m_listChannel.end())
	{
		(*iter)->setVolume(_volume);
	}
}

void CSound::RemoveChannel(const FMOD::Channel* _pTargetChannel)
{
	const auto iter = std::find(m_listChannel.begin(), m_listChannel.end(), _pTargetChannel);
	if (iter != m_listChannel.end())
	{
		m_listChannel.erase(iter);
	}
}

int CSound::Load(const wstring& _strFilePath)
{
	if (FMOD_OK != g_pFMOD->createSound(ToString(_strFilePath).c_str(), FMOD_DEFAULT, nullptr, &m_pSound))
	{
		assert(nullptr);
	}

	return S_OK;
}

FMOD_RESULT CHANNEL_CALLBACK(FMOD_CHANNELCONTROL*              channelcontrol,
                             FMOD_CHANNELCONTROL_TYPE          controltype,
                             FMOD_CHANNELCONTROL_CALLBACK_TYPE callbacktype,
                             void*                             commanddata1,
                             void*                             commanddata2)
{
	FMOD::Channel* cppchannel = reinterpret_cast<FMOD::Channel*>(channelcontrol);
	CSound*        pSound     = nullptr;

	switch (controltype)
	{
	case FMOD_CHANNELCONTROL_CALLBACK_END:
		{
			cppchannel->getUserData(reinterpret_cast<void**>(&pSound));
			pSound->RemoveChannel(cppchannel);
		}
		break;
	}

	return FMOD_OK;
}
