#include "Camera.h"
#include "GameInstance.h"

Engine::CCamera::CCamera(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext)
    :CGameObject(pDevice, pContext)
{
}

Engine::CCamera::CCamera(const CCamera& rhs)
    :CGameObject(rhs),
    m_fFov(rhs.m_fFov),
    m_fNear(rhs.m_fNear),
    m_fFar(rhs.m_fFar),
    m_fAspect(rhs.m_fAspect)
{
}

void Engine::CCamera::CameraShake(_float fDuration, _float fMinFov, _float fMaxFov)
{
    m_bOnShake = true;
    m_fShakeDuration = fDuration;
    m_fMinShakeFov = fMinFov;
    m_fMaxShakeFov = fMaxFov;
    m_fShakeCheck = 0.f;
}

HRESULT Engine::CCamera::Initialize(_float fFov, _float fNear, _float fFar, _float fAspect, Vector3 vInitPos)
{
    if (FAILED(__super::Initialize(vInitPos))) return E_FAIL;

    m_fFov = fFov;
    m_fOriginFov = fFov;
    m_fNear = fNear;
    m_fFar = fFar;
    m_fAspect = fAspect;

    return S_OK;
}

void Engine::CCamera::Priority_Tick(_float fTimeDelta)
{
    if (m_bOnShake)
    {
        m_fShakeCheck += fTimeDelta;

        m_fFov = m_pGameInstance.lock()->Rand_Num(m_fMinShakeFov, m_fMaxShakeFov);

        if (m_fShakeCheck >= m_fShakeDuration)
        {
            m_bOnShake = false;        
            m_fFov = m_fOriginFov;
        }
    }
}

_int Engine::CCamera::Tick(_float fTimeDelta)
{
    return OBJ_ALIVE;
}

void Engine::CCamera::Late_Tick(_float fTimeDelta)
{
}

HRESULT Engine::CCamera::Render()
{
    return S_OK;
}

HRESULT Engine::CCamera::SetUp_Matrices()
{
    m_pGameInstance.lock()->Set_ViewMatrix(XMMatrixInverse(nullptr, m_pTransformCom->Get_WorldMatrix()));
    m_pGameInstance.lock()->Set_ProjMatrix(XMMatrixPerspectiveFovLH(m_fFov, m_fAspect, m_fNear, m_fFar));
    m_pGameInstance.lock()->Set_CamFar(m_fFar);

    return S_OK;
}
