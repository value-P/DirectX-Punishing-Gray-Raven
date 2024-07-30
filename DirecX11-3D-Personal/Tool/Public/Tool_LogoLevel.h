#pragma once

#include "Level.h"

namespace Tool
{
	class CTool_LogoLevel final : public CLevel
	{
	public:
		CTool_LogoLevel(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext);
		virtual ~CTool_LogoLevel() =default;

	public:
		virtual HRESULT Initialize() override;
		virtual void Tick(_float fTimeDelta) override;
		virtual HRESULT Render() override;

	private:
		HRESULT Ready_Layer_Tool_BackGround(const wstring& strLayerTag);

	public:
		static shared_ptr<CTool_LogoLevel> Create(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext);
	};
}
