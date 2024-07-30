#pragma once
#include "Camera.h"

namespace Client
{
    class CActionCamera final : public CCamera
    {
	public:
		struct ActionCamKeyframe
		{
			Vector3 vPosition;
			Vector3 vLookPosition;
			_float fKeyframeTime;

			// 다음 키프레임까지 보간하며 이동할지 
			// false 라면 다음키프레임까지 현재위치에서 있다가 바로 이동하도록
			_bool bMoveLerp;
		};

		struct INIT_DESC
		{
			vector<ActionCamKeyframe> keyFrames;
			shared_ptr<CGameObject> pRefObject;
		};

	public:
		CActionCamera(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext);
		virtual ~CActionCamera() = default;

	public:
		virtual HRESULT Initialize(_float fFov, _float fNear, _float fFar, _float fAspect, const INIT_DESC& initDesc, LEVEL eCurrentLevel, const wstring& DestChangeCameraTag, LEVEL eNextLevel = LEVEL_END, _bool ChangeScene = false);
		virtual void Priority_Tick(_float fTimeDelta) override;
		virtual _int Tick(_float fTimeDelta) override;
	
	private:
		_float m_fCurrentKeyFrame = 0.f;
		_float m_fCameraActionDuration = 0.f;
		_int m_iCurrentKeyFrameIndex = 0;
		_bool m_bChangeScene = false;

		LEVEL m_eNextScene = LEVEL_END;
		LEVEL m_eCurrentLevel = LEVEL_END;

		wstring m_strDestChangeCamera = L""; // 연출카메라가 종s료되고 메인카메라로 변경할 카메라태그

		vector<ActionCamKeyframe> m_vecKeyFrames;

		// 연출 카메라가 참조할 오브젝트(없다면 상수 포지션을 받도록 한다)
		// 참조오브젝트가 있다면 keyframe의 position은 참조 포지션 기준 look : z, right : x만큼 떨어지도록 한다, y는 상수 값 그대로
		// at같은 경우 위와 같이 참조 포지션에서 얼마만큼 떨어진지에 대한 참고값으로 사용한다
		shared_ptr<CGameObject> m_pReferenceObejct = nullptr;

	public:
		static shared_ptr<CActionCamera> Create(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext,_float fFov, _float fNear, _float fFar, _float fAspect, const INIT_DESC& initDesc, LEVEL eCurrentLevel, const wstring& DestChangeCameraTag, LEVEL eNextLevel = LEVEL_END, _bool ChangeScene = false);

    };
}


