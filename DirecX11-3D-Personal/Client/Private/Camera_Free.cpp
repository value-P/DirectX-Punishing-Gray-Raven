#include "pch.h"
#include "Camera_Free.h"

Client::CCamera_Free::CCamera_Free()
	:CCamera(nullptr, nullptr)
{
}

Client::CCamera_Free::CCamera_Free(const CCamera_Free& rhs)
    :CCamera(rhs)
{
}

HRESULT Client::CCamera_Free::Initialize(_float fFov, _float fNear, _float fFar, _float fAspect, _float fMouseSensor, Vector3 vInitPos)
{
	if (FAILED(__super::Initialize(fFov, fNear, fFar, fAspect, vInitPos)))
		return E_FAIL;

	m_fMouseSensor = fMouseSensor;

    return S_OK;
}

void Client::CCamera_Free::Priority_Tick(_float fTimeDelta)
{
	if (m_pGameInstance.lock()->GetKey(DIK_W))
		m_pTransformCom->Go_Straight(fTimeDelta * m_fCamSpeed);

	if (m_pGameInstance.lock()->GetKey(DIK_S))
		m_pTransformCom->Go_Backward(fTimeDelta * m_fCamSpeed);

	if (m_pGameInstance.lock()->GetKey(DIK_A))
		m_pTransformCom->Go_Left(fTimeDelta * m_fCamSpeed);

	if (m_pGameInstance.lock()->GetKey(DIK_D))
		m_pTransformCom->Go_Right(fTimeDelta * m_fCamSpeed);

	_long	MouseMove = { 0 };

	if (MouseMove = m_pGameInstance.lock()->Get_DIMouseMove(DIMS_X))
	{
		m_pTransformCom->Turn(XMVectorSet(0.f, 1.f, 0.f, 0.f), fTimeDelta * MouseMove * m_fMouseSensor);
	}


	if (MouseMove = m_pGameInstance.lock()->Get_DIMouseMove(DIMS_Y))
	{
		m_pTransformCom->Turn(Vector4(m_pTransformCom->Get_Right()), fTimeDelta * MouseMove * m_fMouseSensor);
	}


	if (FAILED(SetUp_Matrices()))
		return;
}

_int Client::CCamera_Free::Tick(_float fTimeDelta)
{
	return __super::Tick(fTimeDelta);
}

void Client::CCamera_Free::Late_Tick(_float fTimeDelta)
{
}

HRESULT Client::CCamera_Free::Render()
{
    return S_OK;
}

shared_ptr<CCamera_Free> Client::CCamera_Free::Create(_float fFov, _float fNear, _float fFar, _float fAspect, Vector3 vInitPos, _float fMouseSensor)
{
	shared_ptr<CCamera_Free> pInstance = make_shared<CCamera_Free>();

	if (FAILED(pInstance->Initialize(fFov, fNear, fFar, fAspect, fMouseSensor, vInitPos)))
	{
		MSG_BOX("Failed to Created : CCamera_Free");
		pInstance.reset();
	}

	return pInstance;
}