#include "UIManager.h"
#include "UIBase.h"
#include "GameInstance.h"

Engine::CUIManager::CUIManager(_uint iNumLevels)
    :m_iLevelNum(iNumLevels), m_pGameInstance(CGameInstance::GetInstance())
{
    m_vecUIList.resize(iNumLevels);
}

HRESULT Engine::CUIManager::Initialize(HWND clientHandle)
{
    m_ClientHandle = clientHandle;

    return S_OK;
}

void Engine::CUIManager::Tick(_float fTimeDelta)
{
    if (false == m_bActiveUI)
        return;

    _bool bLButtonClicked = m_pGameInstance.lock()->GetMouseButtonDown(MOUSEKEYSTATE::DIM_LB);
    _bool bLButtonUpped = m_pGameInstance.lock()->GetMouseButtonUp(MOUSEKEYSTATE::DIM_LB);

    for(auto& iter : m_vecUIList)
    {
        for (auto& pUI : iter)
        {
            if (pUI->isActivate())
            {
                pUI->Tick(fTimeDelta);

                if (bLButtonClicked)
                {
                    if (pUI->isMouseIn(m_ClientHandle))
                    {
                        pUI->OnClickEnter();
                        m_clickedUIList.push_back(pUI);
                    }
                }
            }
        }
    }

    if (bLButtonUpped)
    {
        for (auto& pUI : m_clickedUIList)
            pUI->OnClickExit();

        m_clickedUIList.clear();
    }
    else
    {
        for (auto& pUI : m_clickedUIList)
            pUI->OnClickStay();
    }
}

HRESULT Engine::CUIManager::Add_UI(_uint iLevelIndex, shared_ptr<CUIBase> pUI)
{
    if (nullptr == pUI || iLevelIndex >= m_iLevelNum)
        return E_FAIL;

    m_vecUIList[iLevelIndex].push_back(pUI);

    return S_OK;
}

void Engine::CUIManager::Clear(_uint iLevelIndex)
{
    m_vecUIList[iLevelIndex].clear();
}

shared_ptr<CUIManager> Engine::CUIManager::Create(_uint iNumLevels, HWND clientHandle)
{
    shared_ptr<CUIManager> pInstance = make_shared<CUIManager>(iNumLevels);

    if (FAILED(pInstance->Initialize(clientHandle)))
    {
        MSG_BOX("Failed to Created : CUIManager");
        pInstance.reset();
    }

    return pInstance;
}