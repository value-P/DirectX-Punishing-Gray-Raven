#include "pch.h"
#include "ActionCamera.h"
#include "Level_Loading.h"

Client::CActionCamera::CActionCamera(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext)
	:CCamera(pDevice, pContext)
{
}

HRESULT Client::CActionCamera::Initialize(_float fFov, _float fNear, _float fFar, _float fAspect, const INIT_DESC& initDesc, LEVEL eCurrentLevel, const wstring& DestChangeCameraTag, LEVEL eNextLevel , _bool ChangeScene)
{
	m_strDestChangeCamera = DestChangeCameraTag;

	if (initDesc.keyFrames.size() <= 0)
	{
		MSG_BOX("CActionCamera::Initialize : 키프레임이 존재하지 않습니다");
		return E_FAIL;
	}

	m_vecKeyFrames = initDesc.keyFrames;
	m_fCameraActionDuration = m_vecKeyFrames[m_vecKeyFrames.size() - 1].fKeyframeTime;
	m_bChangeScene = ChangeScene;
	m_eNextScene = eNextLevel;
	m_eCurrentLevel = eCurrentLevel;
	m_pReferenceObejct = initDesc.pRefObject;

	if (FAILED(__super::Initialize(fFov, fNear, fFar, fAspect, m_vecKeyFrames[0].vPosition)))
		return E_FAIL;

	m_pTransformCom->LookAt(XMVectorSetW(m_vecKeyFrames[0].vLookPosition, 1.f));

    return S_OK;
}

void Client::CActionCamera::Priority_Tick(_float fTimeDelta)
{
	m_pGameInstance.lock()->DisableUIs();

	__super::Priority_Tick(fTimeDelta);

	if (FAILED(SetUp_Matrices()))
		return;
}

_int Client::CActionCamera::Tick(_float fTimeDelta)
{
	m_fCurrentKeyFrame += fTimeDelta;

	if (m_fCurrentKeyFrame >= m_fCameraActionDuration)
	{
		if (m_bChangeScene)
		{
			m_pGameInstance.lock()->Open_Level(LEVEL_LOADING, CLevel_Loading::Create(m_pDevice, m_pContext, m_eNextScene));
			m_pGameInstance.lock()->EnableUIs();
			return OBJ_ALIVE;
		}

		if (m_strDestChangeCamera != L"")
		{
			m_fCurrentKeyFrame = 0.f;
			m_iCurrentKeyFrameIndex = 0;
			m_pGameInstance.lock()->Set_MainCamera(m_eCurrentLevel, m_strDestChangeCamera);
			m_pGameInstance.lock()->EnableUIs();
			return OBJ_ALIVE;
		}
		else
		{
			m_pGameInstance.lock()->EnableUIs();
			return OBJ_ALIVE;
		}
	}
	else
	{
		while (m_fCurrentKeyFrame >= m_vecKeyFrames[m_iCurrentKeyFrameIndex + 1].fKeyframeTime)
		{
			if (m_iCurrentKeyFrameIndex + 1 == m_vecKeyFrames.size())
				break;

			++m_iCurrentKeyFrameIndex;
		}
	}
	
	_float fRatio = (m_fCurrentKeyFrame - m_vecKeyFrames[m_iCurrentKeyFrameIndex].fKeyframeTime) /
		(m_vecKeyFrames[m_iCurrentKeyFrameIndex + 1].fKeyframeTime - m_vecKeyFrames[m_iCurrentKeyFrameIndex].fKeyframeTime);

	// position의 경우 lerp를 사용할지 안할지 정할 수 있다
	Vector3 vCurrentPos;
	if (m_vecKeyFrames[m_iCurrentKeyFrameIndex].bMoveLerp)
		vCurrentPos = Vector3::Lerp(m_vecKeyFrames[m_iCurrentKeyFrameIndex].vPosition, m_vecKeyFrames[m_iCurrentKeyFrameIndex + 1].vPosition, fRatio);
	else
		vCurrentPos = m_vecKeyFrames[m_iCurrentKeyFrameIndex].vPosition;

	// at의 경우 lerp를 사용할지 안할지 정할 수 없도록 되어있다(추가 가능)
	Vector3 vCurrentLook = Vector3::Lerp(m_vecKeyFrames[m_iCurrentKeyFrameIndex].vLookPosition, m_vecKeyFrames[m_iCurrentKeyFrameIndex + 1].vLookPosition, fRatio);

	if (nullptr == m_pReferenceObejct)
	{
		m_pTransformCom->Set_Pos(vCurrentPos);
		m_pTransformCom->LookAt(XMVectorSetW(vCurrentLook, 1.f));
	}
	else
	{
		Vector3 vLook = m_pReferenceObejct->Get_Transform()->Get_Look();
		Vector3 vRight = m_pReferenceObejct->Get_Transform()->Get_Right();
		Vector3 vPos = m_pReferenceObejct->Get_Transform()->Get_Pos();

		Vector3 vEye = vPos + vLook * vCurrentPos.z + vRight * vCurrentPos.x + Vector3(0.f, 1.f, 0.f) * vCurrentPos.y;
		m_pTransformCom->Set_Pos(vEye);

		Vector3 vAt = vPos + vLook * vCurrentLook.z + vRight * vCurrentLook.x + Vector3(0.f, 1.f, 0.f) * vCurrentLook.y;
		m_pTransformCom->LookAt(XMVectorSetW(vAt, 1.f));
	}

    return OBJ_ALIVE;
}

shared_ptr<CActionCamera> Client::CActionCamera::Create(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext,_float fFov, _float fNear, _float fFar, _float fAspect, const INIT_DESC& initDesc, LEVEL eCurrentLevel, const wstring& DestChangeCameraTag, LEVEL eNextLevel, _bool ChangeScene)
{
	shared_ptr<CActionCamera> pInstance = make_shared<CActionCamera>(pDevice,pContext);

	if (FAILED(pInstance->Initialize(fFov, fNear, fFar, fAspect, initDesc, eCurrentLevel, DestChangeCameraTag, eNextLevel, ChangeScene)))
	{
		MSG_BOX("Failed to Created : CActionCamera");
		pInstance.reset();
	}

	return pInstance;
}
