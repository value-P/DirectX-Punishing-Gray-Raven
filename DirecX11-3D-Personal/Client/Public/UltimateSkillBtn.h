#pragma once
#include "UIBase.h"

namespace Client
{
    class CPlayableVera;

    class CUltimateSkillBtn final : public CUIBase
    {
	public:
		CUltimateSkillBtn(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext);
		virtual ~CUltimateSkillBtn() = default;

	public:
		void Set_Player(shared_ptr<CPlayableVera> pPlayer);

	public:
		virtual HRESULT Initialize(const INIT_DESC& initDesc);
		virtual _int Tick(_float fTimeDelta) override;
		virtual HRESULT Render() override;

	public:
		virtual void OnClickEnter() override;

	private:
		weak_ptr<CPlayableVera>		m_pPlayer;
		shared_ptr<CTexture>		m_pBgTexture = nullptr;
		_float						m_fBgRotationSpeed = 60.f;
		_float						m_fBgRotationAcc = 0.f;

	public:
		static shared_ptr<CUltimateSkillBtn> Create(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext, const INIT_DESC& initDesc);

    };
}


