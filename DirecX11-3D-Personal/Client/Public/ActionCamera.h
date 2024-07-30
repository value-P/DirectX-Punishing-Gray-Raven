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

			// ���� Ű�����ӱ��� �����ϸ� �̵����� 
			// false ��� ����Ű�����ӱ��� ������ġ���� �ִٰ� �ٷ� �̵��ϵ���
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

		wstring m_strDestChangeCamera = L""; // ����ī�޶� ��s��ǰ� ����ī�޶�� ������ ī�޶��±�

		vector<ActionCamKeyframe> m_vecKeyFrames;

		// ���� ī�޶� ������ ������Ʈ(���ٸ� ��� �������� �޵��� �Ѵ�)
		// ����������Ʈ�� �ִٸ� keyframe�� position�� ���� ������ ���� look : z, right : x��ŭ ���������� �Ѵ�, y�� ��� �� �״��
		// at���� ��� ���� ���� ���� �����ǿ��� �󸶸�ŭ ���������� ���� �������� ����Ѵ�
		shared_ptr<CGameObject> m_pReferenceObejct = nullptr;

	public:
		static shared_ptr<CActionCamera> Create(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext,_float fFov, _float fNear, _float fFar, _float fAspect, const INIT_DESC& initDesc, LEVEL eCurrentLevel, const wstring& DestChangeCameraTag, LEVEL eNextLevel = LEVEL_END, _bool ChangeScene = false);

    };
}


