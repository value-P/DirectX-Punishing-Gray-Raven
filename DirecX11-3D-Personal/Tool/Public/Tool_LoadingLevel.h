#pragma once
#include "Level.h"

namespace Tool
{
	class CTool_Loader;

	class CTool_LoadingLevel final:	public CLevel
	{
	public:
		CTool_LoadingLevel(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext,LEVEL eNextLevel);
		virtual ~CTool_LoadingLevel() = default;

	public:
		virtual HRESULT Initialize();
		virtual void Tick(_float fTimeDelta) override;
		virtual HRESULT Render() override;

	public:
		HRESULT Init_Layer_Tool_BackGround();
		HRESULT Init_Layer_UI();

	private:
		HRESULT Ready_Layer_Tool_BackGround(const wstring& strLayerTag);

	private:
		shared_ptr<CTool_Loader>	m_pLoader = nullptr;
		LEVEL						m_eNextLevel = LEVEL_END;

	public:
		static shared_ptr<CTool_LoadingLevel> Create(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext, LEVEL eNextLevel);
	};
}
