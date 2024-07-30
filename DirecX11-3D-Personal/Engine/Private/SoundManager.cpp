#include "SoundManager.h"

Engine::CSoundManager::CSoundManager()
{
}

Engine::CSoundManager::~CSoundManager()
{
    for (auto& Mypair : m_mapSound)
    {
        Mypair.second->release();
    }
    m_mapSound.clear();

    m_pSystem->release();
}

HRESULT Engine::CSoundManager::Initialize()
{
    FMOD_RESULT result;
    result = FMOD::System_Create(&m_pSystem);
    if (result != FMOD_OK) return E_FAIL;

    result = m_pSystem->init(32, FMOD_INIT_NORMAL, nullptr);
    if (result != FMOD_OK) return E_FAIL;

    return S_OK;
}

CHANNELID Engine::CSoundManager::Play_Sound(const wstring& strSoundKey, _float fVolume)
{
    auto iter = m_mapSound.find(strSoundKey);

    if (iter == m_mapSound.end()) return CHANNEL_END;

    _bool bPlay;

    for (_int ID = CHANNEL_PLAYER1; ID < CHANNEL_END; ++ID)
    {
        m_Channels[ID]->isPlaying(&bPlay);
        if (!bPlay)
        {
            m_pSystem->playSound(iter->second, nullptr, false, &m_Channels[ID]);
            m_Channels[ID]->setVolume(fVolume);
            m_pSystem->update();
            return (CHANNELID)ID;
        }
    }

    return CHANNEL_END;
}

_bool Engine::CSoundManager::isPlaying(CHANNELID eChannel)
{
    _bool bPlay;
    m_Channels[eChannel]->isPlaying(&bPlay);
    return bPlay;
}

void Engine::CSoundManager::Play_BGM(const wstring& strSoundKey, _float fVolume)
{
    auto iter = m_mapSound.find(strSoundKey);

    if (iter == m_mapSound.end()) return;

    m_Channels[CHANNEL_BGM]->stop();
    m_pSystem->playSound(iter->second, nullptr, false, &m_Channels[CHANNEL_BGM]);
    m_Channels[CHANNEL_BGM]->setMode(FMOD_LOOP_NORMAL);
    m_Channels[CHANNEL_BGM]->setVolume(fVolume);
    m_pSystem->update();
}

void Engine::CSoundManager::Stop_Sound(CHANNELID eID)
{
    m_Channels[eID]->stop();
}

void Engine::CSoundManager::Stop_All()
{
    for (int i = 0; i < CHANNEL_END; ++i)
        m_Channels[i]->stop();
}

void Engine::CSoundManager::Set_ChannelVolume(CHANNELID eID, _float fVolume)
{
    m_Channels[eID]->setVolume(fVolume);
    m_pSystem->update();
}

void Engine::CSoundManager::LoadSound(const wstring& strRootFile)
{
    filesystem::directory_iterator iter(strRootFile);

    FMOD::Sound* pSound = nullptr;

    while (iter != filesystem::end(iter))
    {
        if (iter->is_regular_file())
        {
            auto Pair = m_mapSound.find(iter->path().stem().wstring());
            if (Pair != m_mapSound.end())
                continue;

            string path = iter->path().string();

            FMOD_RESULT eRes = m_pSystem->createSound(path.c_str(), FMOD_LOOP_OFF, nullptr, &pSound);

            if (eRes == FMOD_OK)
            {
                // 사운드 파일의 확장자 명을 뗀 이름을 키값으로 사용
                m_mapSound.emplace(iter->path().stem().wstring(), pSound);
            }
        }
        else if(iter->is_directory())
        {
            LoadSound(iter->path().wstring());
        }

        ++iter;
    }
}

shared_ptr<CSoundManager> Engine::CSoundManager::Create()
{
    shared_ptr<CSoundManager> pInstance = make_shared<CSoundManager>();

    if (FAILED(pInstance->Initialize()))
    {
        MSG_BOX("Failed to Created : CSoundManager");
        pInstance.reset();
    }

    return pInstance;
}
