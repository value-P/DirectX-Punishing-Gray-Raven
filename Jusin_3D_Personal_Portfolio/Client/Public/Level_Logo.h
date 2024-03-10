#pragma once

#include "Level.h"

namespace Client
{
	class CLevel_Logo final : public CLevel
	{
	public:
		CLevel_Logo(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext);
		virtual ~CLevel_Logo() =default;

	public:
		virtual HRESULT Initialize() override;
		virtual void Tick(_float fTimeDelta) override;
		virtual HRESULT Render() override;

	private:
		HRESULT Ready_Layer_BackGround(const wstring& strLayerTag);

	public:
		static shared_ptr<CLevel_Logo> Create(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext);
	};
}
