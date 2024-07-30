#pragma once
#include "FX.h"

namespace Client
{
	class CChosanEffect final : public CFX
	{
	public:
		struct INIT_DESC
		{
			LEVEL eLevel;
			wstring strEffectBufferPath;
			_float fStartScale;
			_float fEndScale;
			_float fMoveSpeed;
		};

	public:
		CChosanEffect(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext);
		virtual ~CChosanEffect() = default;

	public:
		virtual HRESULT Initialize(const INIT_DESC& InitDesc);
		virtual _int Tick(_float fTimeDelta) override;
		virtual void Late_Tick(_float fTimeDelta) override;
		virtual HRESULT Render() override;
		virtual HRESULT Render_ExceptDark() override;

	public:
		virtual void PlayOnce() override;

	protected:
		shared_ptr<CTransform> m_EffectTransforms[6] = { nullptr };
		shared_ptr<CTexture> m_pTexture = nullptr;
	protected:
		shared_ptr<CEffectMeshCom> m_EffectComs[6] = { nullptr };

		_float m_fEffectMoveSpeed = 0.f;
		_float m_fChangableSpeed = 0.f;

		_float m_fDuration = 3.f;
		_float m_fDurationCheck = 0.f;

		_float m_fCurrentScale = 0.f;
		_float m_fMinScale = 1.f;
		_float m_fMaxScale = 1.f;
			

	public:
		static shared_ptr<CChosanEffect> Create(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext, const INIT_DESC& InitDesc);
	};
}
