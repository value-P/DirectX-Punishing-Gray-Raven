#include "Level_Manager.h"
#include "Level.h"

Engine::CLevel_Manager::CLevel_Manager()
    :m_pGameInstance(CGameInstance::GetInstance())
{
}

HRESULT Engine::CLevel_Manager::Initialize()
{
    return S_OK;
}

void Engine::CLevel_Manager::Tick(_float fTimeDelta)
{
    if (m_pCurrentLevel == nullptr)
        return;

    m_pCurrentLevel->Tick(fTimeDelta);
}

HRESULT Engine::CLevel_Manager::Render()
{
    if (m_pCurrentLevel == nullptr)
        return E_FAIL;

    return m_pCurrentLevel->Render();
}

HRESULT Engine::CLevel_Manager::Open_Level(_uint iLevelIndex, shared_ptr<CLevel> pNewLevel)
{
    if (m_pCurrentLevel != nullptr)
    {
        m_pCurrentLevel.reset();
        m_pGameInstance.lock()->Clear(m_iLevelIndex);
    }

    m_pCurrentLevel = pNewLevel;

    m_iLevelIndex = iLevelIndex;

    return S_OK;
}

shared_ptr<CLevel_Manager> Engine::CLevel_Manager::Create()
{
    shared_ptr<CLevel_Manager> pInstance = make_shared<CLevel_Manager>();

    if (FAILED(pInstance->Initialize()))
    {
        MSG_BOX("Failed to Created : CLevel_Manager");
        pInstance.reset();
    }

    return pInstance;
}