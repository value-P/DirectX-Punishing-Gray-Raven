#pragma once
#include "Level.h"

namespace Tool
{
	class CTool_GamePlayLevel final : public CLevel
	{
	public:
		CTool_GamePlayLevel(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext);
		virtual ~CTool_GamePlayLevel() = default;

	public:
		virtual HRESULT Initialize() override;
		virtual void Tick(_float fTimeDelta) override;
		virtual HRESULT Render() override;

	private:
		HRESULT Ready_Reource();
		HRESULT Ready_Lights();
		HRESULT Ready_Layer_Camera(const wstring& strLayerTag);

	private:
		wstring m_strLoadingText = L"";

	public:
		static shared_ptr<CTool_GamePlayLevel> Create(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext);
	};
}


