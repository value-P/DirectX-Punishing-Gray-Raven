#include "UIBase.h"
#include "GameInstance.h"

Engine::CUIBase::CUIBase(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext)
    :CGameObject(pDevice,pContext)
{
}

_bool Engine::CUIBase::isMouseIn(HWND clientHandle)
{
    POINT pt{};
    GetCursorPos(&pt);
    ScreenToClient(clientHandle, &pt);

    _bool isInX = pt.x >= m_vPosition.x - m_vSize.x * 0.5f && pt.x <= m_vPosition.x + m_vSize.x * 0.5f;
    _bool isInY = pt.y >= m_vPosition.y - m_vSize.y * 0.5f && pt.y <= m_vPosition.y + m_vSize.y * 0.5f;

    return isInX && isInY;
}

HRESULT Engine::CUIBase::Initialize(const INIT_DESC& initDesc)
{
    D3D11_VIEWPORT			ViewPortDesc{};
    _uint		iNumViewports = 1;
    m_pContext->RSGetViewports(&iNumViewports, &ViewPortDesc);

    m_vViewPortSize = Vector2(ViewPortDesc.Width, ViewPortDesc.Height);

    m_vPosition = initDesc.vPos;

    Vector3 vPos;
    vPos.x = initDesc.vPos.x - m_vViewPortSize.x * 0.5f;
    vPos.y = -initDesc.vPos.y + m_vViewPortSize.y * 0.5f;
    vPos.z = 0.f;

    if (__super::Initialize(vPos))
        return E_FAIL;

    m_pPointBuffer = m_pGameInstance.lock()->Get_PointProp();

    m_vSize = initDesc.vSize;

    m_pTransformCom->Set_Scaling(m_vSize.x, m_vSize.y, 1.f);

    return S_OK;
}