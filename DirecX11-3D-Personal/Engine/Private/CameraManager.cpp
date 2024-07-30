#include "CameraManager.h"
#include "Camera.h"

Engine::CCameraManager::CCameraManager(_uint iNumLevels)
    :m_iLevelNum(iNumLevels)
{
    m_vecCameras.resize(iNumLevels);
    m_MainCameras.resize(iNumLevels);
}

HRESULT Engine::CCameraManager::Initialize()
{
    return S_OK;
}

void Engine::CCameraManager::Priority_Tick(_float fTimeDelta)
{
    if (nullptr != m_MainCameras[m_iCurrentLevel])
        m_MainCameras[m_iCurrentLevel]->Priority_Tick(fTimeDelta);
}

void Engine::CCameraManager::Tick(_float fTimeDelta)
{
    if (nullptr != m_MainCameras[m_iCurrentLevel])
        m_MainCameras[m_iCurrentLevel]->Tick(fTimeDelta);
}

HRESULT Engine::CCameraManager::Add_Camera(_uint iLevelIndex, const wstring& strCameraTag, shared_ptr<CCamera> pCamera)
{
    auto Pair = m_vecCameras[iLevelIndex].find(strCameraTag);
    if (Pair != m_vecCameras[iLevelIndex].end())
    {
        MSG_BOX("CCameraManager::Add_Camera : 이미 존재하는 키값입니다");
        return E_FAIL;
    }

    m_vecCameras[iLevelIndex].emplace(strCameraTag, pCamera);

    return S_OK;
}

void Engine::CCameraManager::Change_Scene(_uint iLevelIndex)
{
    m_vecCameras[m_iCurrentLevel].clear();
    m_iCurrentLevel = iLevelIndex;
}

HRESULT Engine::CCameraManager::Set_MainCamera(_uint iLevelIndex, const wstring& strCameraTag)
{
    auto Pair = m_vecCameras[iLevelIndex].find(strCameraTag);
    if (Pair == m_vecCameras[iLevelIndex].end())
        return E_FAIL;

    m_MainCameras[iLevelIndex] = Pair->second;

    return S_OK;
}

shared_ptr<CCameraManager> Engine::CCameraManager::Create(_uint iNumLevels)
{
    shared_ptr<CCameraManager> pInstance = make_shared<CCameraManager>(iNumLevels);

    if (FAILED(pInstance->Initialize()))
    {
        MSG_BOX("Failed to Created : CCameraManager");
        pInstance.reset();
    }

    return pInstance;
}
