#include "pch.h"
#include "Camera_Combined.h"
#include "PlayableObject.h"
#include "PlayableVera.h"

Client::CCamera_Combined::CCamera_Combined()
	:CCamera(nullptr, nullptr)
{
}

HRESULT Client::CCamera_Combined::Initialize(_float fFov, _float fNear, _float fFar, _float fAspect, shared_ptr<CPlayableVera> pCombinedObj, const Vector3& vInitialOffset)
{
	if (nullptr == pCombinedObj)
		return E_FAIL;

	m_pCombinedObj = pCombinedObj;

	m_vOffset = vInitialOffset;

	m_vCamReference = m_pCombinedObj.lock()->Get_Transform()->Get_Pos() + Vector3(0.f, 2.5f, 0.f);

    if (FAILED(__super::Initialize(fFov, fNear, fFar, fAspect, m_vCamReference + m_vOffset)))
        return E_FAIL;

	m_pTransformCom->LookAt(XMVectorSetW(m_vCamReference, 1.f));
	m_vPrevAt = m_vCamReference;

	m_fCombinedDist = m_fOriginDist = vInitialOffset.Length();
	m_fAzimuthRad = atan2f(vInitialOffset.z , vInitialOffset.x); 
	m_fElevationRad = asinf(vInitialOffset.y / m_fCombinedDist);

    return S_OK;
}

void Client::CCamera_Combined::Priority_Tick(_float fTimeDelta)
{
	__super::Priority_Tick(fTimeDelta);

	m_vCamReference = m_pCombinedObj.lock()->Get_Transform()->Get_Pos() + Vector3(0.f, 2.5f, 0.f);

	m_pTarget = m_pCombinedObj.lock()->Get_TargetPtr().lock();

	KeyInput(fTimeDelta);

	Compute_SphericalToOrthogonal();

	Vector3 curPos = m_pTransformCom->Get_Pos();
	Vector3 DestPos;

	if (m_pTarget == nullptr)
	{
		DestPos = Vector3::Lerp(curPos, m_vCamReference + m_vOffset, 0.1f);
		//DestPos = Vector3::Lerp(curPos, m_vCamReference + (m_vOffsetDir * m_fCombinedDist), 0.1f);
		m_pTransformCom->Set_Pos(DestPos);

		Vector3 tempAt = Vector3::Lerp(m_vPrevAt, m_vCamReference, 0.1f);
		m_pTransformCom->LookAt(XMVectorSetW(tempAt, 1.f));
		m_vPrevAt = tempAt;
	}
	else
	{
		Compute_TrackingOwner(fTimeDelta);

		Vector3 targetPos = m_pTarget->Get_CamRefPos();
		Vector3 tempDir = m_vCamReference - targetPos;
		tempDir.y = 0.f;
		tempDir.Normalize();
		tempDir = XMVector3TransformNormal(tempDir, XMMatrixRotationY(XMConvertToRadians(-20.f)));

		DestPos = Vector3::Lerp(curPos, m_vCamReference + (tempDir * m_fCombinedDist), 0.1f);
		m_pTransformCom->Set_Pos(DestPos);

		Vector3 tempAt = Vector3::Lerp(m_vPrevAt, targetPos, 0.1f);
		m_pTransformCom->LookAt(XMVectorSetW(tempAt, 1.f));

		m_vPrevAt = tempAt;

		Vector3 vCurrentOffset = DestPos - m_vCamReference;
		m_fAzimuthRad = atan2f(vCurrentOffset.z, vCurrentOffset.x);
		m_fElevationRad = asinf(vCurrentOffset.y / m_fCombinedDist);
	}

	if (FAILED(SetUp_Matrices()))
		return;
}

_int Client::CCamera_Combined::Tick(_float fTimeDelta)
{
	m_pTarget = m_pCombinedObj.lock()->Get_TargetPtr().lock();

	return OBJ_ALIVE;
}

void Client::CCamera_Combined::Late_Tick(_float fTimeDelta)
{
}

HRESULT Client::CCamera_Combined::Render()
{
    return S_OK;
}

void Client::CCamera_Combined::LockOn(_bool lockOn)
{
	if (lockOn)
	{
		if (m_pCombinedObj.lock()->Get_TargetPtr().lock() != nullptr)
		{
			m_pTarget = m_pCombinedObj.lock()->Get_TargetPtr().lock();
			m_bLockOn = true;
		}
	}
	else
	{
		m_pTarget = nullptr;
		m_bLockOn = true; 
	}
}

void Client::CCamera_Combined::Compute_SphericalToOrthogonal()
{
	_float t = m_fCombinedDist * cosf(m_fElevationRad);
	m_vOffset.x = t * cosf(m_fAzimuthRad);
	m_vOffset.y = m_fCombinedDist * sinf(m_fElevationRad);
	m_vOffset.z = t *  sinf(m_fAzimuthRad);
}

void Client::CCamera_Combined::Compute_TrackingOwner(_float fTimeDelta)
{
	Matrix matView = m_pGameInstance.lock()->Get_ViewMatrix();
	Matrix matProj = m_pGameInstance.lock()->Get_ProjMatrix();
	Matrix matVP = matView * matProj;

	Vector3 vOwnerPos = m_pCombinedObj.lock()->Get_Transform()->Get_Pos();
	Vector3 vOwnerProjPos = XMVector3TransformCoord(vOwnerPos, matVP);

	if (vOwnerProjPos.y <= -0.7f)
		m_fCombinedDist += 5.f * fTimeDelta;
	else
		m_fCombinedDist = m_fOriginDist;
}

void Client::CCamera_Combined::KeyInput(_float fTImeDelta)
{
	if (m_pGameInstance.lock()->GetMouseButton(MOUSEKEYSTATE::DIM_LB))
	{
		m_fAzimuthRad -= m_pGameInstance.lock()->Get_DIMouseMove(MOUSEMOVESTATE::DIMS_X) * fTImeDelta * 0.1f;
		if (m_fAzimuthRad >= PI)
		{
			m_fAzimuthRad -= PI;
		}
	}

}

shared_ptr<CCamera_Combined> Client::CCamera_Combined::Create(_float fFov, _float fNear, _float fFar, _float fAspect, shared_ptr<CPlayableVera> pCombinedObj, const Vector3& vInitialOffset)
{
	shared_ptr<CCamera_Combined> pInstance = make_shared<CCamera_Combined>();

	if (FAILED(pInstance->Initialize(fFov, fNear, fFar, fAspect, pCombinedObj, vInitialOffset)))
	{
		MSG_BOX("Failed to Created : CCamera_Combined");
		pInstance.reset();
	}

	return pInstance;
}
