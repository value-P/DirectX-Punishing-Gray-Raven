#include "Navigation.h"
#include "GameInstance.h"
#include "Shader.h"
#include "NavMeshProp.h"

Engine::CNavigation::CNavigation(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext)
    :CComponent(pDevice, pContext)
{
}

HRESULT Engine::CNavigation::Initialize(_int iStartIndex)
{
    m_iCurrentIndex = iStartIndex;

    return S_OK;
}

HRESULT Engine::CNavigation::Initialize(_uint iLevelIndex, const Vector3& vStartPos)
{
    _int index = m_pGameInstance.lock()->GetIndexByPosition(iLevelIndex, vStartPos);

    if (index == -1)
        return E_FAIL;

    m_iCurrentIndex = index;
    
    return S_OK;
}

_bool Engine::CNavigation::isMove(const Vector3& vPosition)
{
    return m_pGameInstance.lock()->isMove(vPosition,m_iCurrentIndex);
}

_float Engine::CNavigation::Get_HeightOnNav(const Vector3& vPosition)
{
    return m_pGameInstance.lock()->Get_HeightByIndex(m_iCurrentIndex, vPosition);
}

shared_ptr<CNavigation> Engine::CNavigation::Create(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext, _int iStartIndex)
{
    shared_ptr<CNavigation> pInstance = make_shared<CNavigation>(pDevice, pContext);

    if (FAILED(pInstance->Initialize(iStartIndex)))
    {
        MSG_BOX("Failed to Created : CNavigation");
        pInstance.reset();
    }

    return pInstance;
}

shared_ptr<CNavigation> Engine::CNavigation::Create(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext, _uint iLevelIndex, const Vector3& vStartPos)
{
    shared_ptr<CNavigation> pInstance = make_shared<CNavigation>(pDevice, pContext);

    if (FAILED(pInstance->Initialize(iLevelIndex, vStartPos)))
    {
        MSG_BOX("startPos is out of NavMesh : CNavigation");
        pInstance.reset();
    }

    return pInstance;
}
