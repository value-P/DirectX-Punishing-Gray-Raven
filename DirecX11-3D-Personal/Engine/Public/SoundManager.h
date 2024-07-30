#pragma once
#include "Engine_Defines.h"

namespace Engine
{
	class CSoundManager
	{
	public:
		CSoundManager();
		virtual ~CSoundManager();

	public:
		HRESULT Initialize();

	public:
		CHANNELID Play_Sound(const wstring& strSoundKey, _float fVolume);
		_bool isPlaying(CHANNELID eChannel);
		void Play_BGM(const wstring& strSoundKey, _float fVolume);
		void Stop_Sound(CHANNELID eID);
		void Stop_All();
		void Set_ChannelVolume(CHANNELID eID, _float fVolume);

		void LoadSound(const wstring& strRootFile);

	private:
		FMOD::System* m_pSystem = nullptr;
		FMOD::Channel* m_Channels[CHANNEL_END];
		map<wstring, FMOD::Sound*> m_mapSound;

	public:
		static shared_ptr<CSoundManager> Create();
	};
}