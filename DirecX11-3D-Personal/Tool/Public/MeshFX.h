#pragma once
#include "GameObject.h"

namespace Tool
{
    class CMeshFX final : public CGameObject
    {
	public:
		CMeshFX(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext);
		virtual ~CMeshFX() = default;

	public:
		void Set_UVSpeed(Vector2 UVSpeed);
		void Set_AnimSpeed(_float fSpeed) { m_fAnimSpeed = fSpeed; }
		void Set_Duration(_float fDuration) { m_fDuration = fDuration; }

		void UsePrimeColor(_bool usePrimeColor) { m_bUsePrimeColor = usePrimeColor; }
		void Set_PrimeColor(Vector4 vPrimeColor) { m_vPrimeColor = vPrimeColor; }
		void Set_MaskTexture(shared_ptr<CTexture> pTex) { m_pMaskTex = pTex; }
	public:
		virtual HRESULT Initialize(LEVEL eLevel, const wstring& strOwnerKey, const wstring& strEffectModelKey, Vector3 vInitPos);
		virtual void Priority_Tick(_float fTimeDelta);
		virtual _int Tick(_float fTimeDelta);
		virtual void Late_Tick(_float fTimeDelta);
		virtual HRESULT Render();

		void Activate() {
			m_bActivate = true;
		}

	private:
		HRESULT Add_Component(LEVEL eLevel, const wstring& strOwnerKey, const wstring& strEffectModelKey);
		HRESULT Bind_ShaderResources();

	private:
		shared_ptr<CEffectMeshCom> m_pMeshCom = nullptr;
		_bool m_bActivate = false;
		_bool m_bLastFrameCalled = false;
		_float m_fAnimSpeed = 1.f;
		_float m_fDuration = 0.f;
		_float m_fTimeCheck = 0.f;
		_bool m_bUsePrimeColor = false;
		shared_ptr<CTexture> m_pMaskTex = nullptr;
		Vector4 m_vPrimeColor;
		_double m_fFrameRate = 0.0;
	public:
		static shared_ptr<CMeshFX> Create(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext, LEVEL eLevel, const wstring& strOwnerKey, const wstring& strEffectModelKey, Vector3 vInitPos);
	};
}