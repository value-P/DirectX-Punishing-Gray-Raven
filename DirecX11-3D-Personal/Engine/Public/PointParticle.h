#pragma once
#include "VIBuffer.h"

namespace Engine
{
    class ENGINE_DLL CPointParticle final : public CVIBuffer
    {
	public:
		struct INSTANCE_DESC
		{
			Vector3			vPivot;
			Vector3			vCenter;
			Vector3			vRange;
			Vector2			vMinSize;
			Vector2			vMaxSize;
			Vector2			vSpeed;
			Vector2			vLifeTime;
			_bool			isLoop;
			Vector4			vColor;
			_float			fGravityAcceleration; // �߷°��ӵ� 0�̸� �߷� ���� x
			_float			fTerminalSpeed; // ���ܼӵ�(�߶� �ӵ�)
			_float			fDrag; // 0~1 -> speed * (1 - fDrag) �Ұ���
			_float			fDuration;
		};

	public:
		CPointParticle(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext);
		virtual ~CPointParticle() = default;

	public:
		void Set_InstanceData(const INSTANCE_DESC& instanceData) { m_InstanceData = instanceData; }
		_bool isFinished() { return m_isFinished; }
		void Start();
		Vector3 Get_PivotPos() { return m_InstanceData.vPivot; }

	public:
		virtual HRESULT Initialize(_uint iNumInstance, const INSTANCE_DESC& initData);
		virtual HRESULT Bind_Buffers() override;
		virtual HRESULT Render() override;

	 public:
		void Tick_Drop(_float fTimeDelta);
		void Tick_Spread(_float fTimeDelta);

	private:
		ComPtr<ID3D11Buffer>		m_pVBInstance = nullptr;
		_uint						m_iInstanceStride = 0;
		_uint						m_iNumInstance = 0;
		_uint						m_iIndexCountPerInstance = 0;

		INSTANCE_DESC				m_InstanceData;

		vector<_float>				m_vecInstanceSpeed;
		vector<_float>				m_vecInstanceLifeTime;
		vector<_float>				m_vecInstanceOriginLifeTime;

		_float						m_fTimeAcc = 0.f;
		_bool						m_isFinished = false;
		_bool						m_bDurationEnd = false;

	public:
		static shared_ptr<CPointParticle> Create(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext, _uint iNumInstance, const INSTANCE_DESC& initData);

	};
}


