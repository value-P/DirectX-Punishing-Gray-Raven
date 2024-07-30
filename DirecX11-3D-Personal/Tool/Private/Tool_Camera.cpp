#include "pch.h"
#include "Tool_Camera.h"

Tool::CTool_Camera::CTool_Camera()
	:CCamera(nullptr, nullptr)
{
}

HRESULT Tool::CTool_Camera::Initialize(_float fFov, _float fNear, _float fFar, _float fAspect, _float fMouseSensor, Vector3 vInitPos)
{
    if (FAILED(__super::Initialize(fFov, fNear, fFar, fAspect, vInitPos)))
        return E_FAIL;

	m_pTransformCom->Rotation(Vector4(1.f, 0.f, 0.f, 0.f), XMConvertToRadians(35.f));

	m_fMouseSensor = fMouseSensor;

    return S_OK;
}

void Tool::CTool_Camera::Priority_Tick(_float fTimeDelta)
{
	if (m_pGameInstance.lock()->GetKeyDown(DIK_GRAVE))
	{
		m_bFixCursor = !m_bFixCursor;

		if (m_bFixCursor)
			ShowCursor(FALSE);
		else
			ShowCursor(TRUE);
	}

	if (m_pGameInstance.lock()->GetKey(DIK_W))
		m_pTransformCom->Go_Straight(fTimeDelta * m_fCamSpeed);

	if (m_pGameInstance.lock()->GetKey(DIK_S))
		m_pTransformCom->Go_Backward(fTimeDelta * m_fCamSpeed);

	if (m_pGameInstance.lock()->GetKey(DIK_A))
		m_pTransformCom->Go_Left(fTimeDelta * m_fCamSpeed);

	if (m_pGameInstance.lock()->GetKey(DIK_D))
		m_pTransformCom->Go_Right(fTimeDelta * m_fCamSpeed);

	if (m_bFixCursor)
	{
		Limit_MouseMove();

		_long	MouseMove = { 0 };

		if (MouseMove = m_pGameInstance.lock()->Get_DIMouseMove(DIMS_X))
		{
			m_pTransformCom->Turn(XMVectorSet(0.f, 1.f, 0.f, 0.f), fTimeDelta * MouseMove * m_fMouseSensor);
		}


		if (MouseMove = m_pGameInstance.lock()->Get_DIMouseMove(DIMS_Y))
		{
			m_pTransformCom->Turn(Vector4(m_pTransformCom->Get_Right()), fTimeDelta * MouseMove * m_fMouseSensor);
		}
	}	

	if (FAILED(SetUp_Matrices()))
		return;
}

_int Tool::CTool_Camera::Tick(_float fTimeDelta)
{
	return OBJ_ALIVE;
}

void Tool::CTool_Camera::Late_Tick(_float fTimeDelta)
{
}

HRESULT Tool::CTool_Camera::Render()
{
	return S_OK;
}

void Tool::CTool_Camera::Limit_MouseMove()
{
	POINT ptMouse{ g_iWinSizeX >> 1, g_iWinSizeY >> 1 };

	ClientToScreen(g_hWnd, &ptMouse);
	SetCursorPos(ptMouse.x, ptMouse.y);
}

shared_ptr<CTool_Camera> Tool::CTool_Camera::Create(_float fFov, _float fNear, _float fFar, _float fAspect, Vector3 vInitPos, _float fMouseSensor)
{
	shared_ptr<CTool_Camera> pInstance = make_shared<CTool_Camera>();

	if (FAILED(pInstance->Initialize(fFov, fNear, fFar, fAspect, fMouseSensor, vInitPos)))
	{
		MSG_BOX("Failed to Created : CTool_Camera");
		pInstance.reset();
	}

	return pInstance;
}