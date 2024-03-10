#pragma once
#include "Level.h"

namespace Client
{
	class CLoader;

	class CLevel_Loading final:	public CLevel
	{
	public:
		CLevel_Loading(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext,LEVEL eNextLevel);
		virtual ~CLevel_Loading() = default;

	public:
		virtual HRESULT Initialize();
		virtual void Tick(_float fTimeDelta) override;
		virtual HRESULT Render() override;

	private:
		HRESULT Ready_Layer_BackGround(const wstring& strLayerTag, LEVEL eLevel);
		HRESULT Init_Layer_UI(const wstring& strLayerTag, LEVEL eLevel);

	private:
		shared_ptr<CLoader>	m_pLoader = nullptr;
		LEVEL						m_eNextLevel = LEVEL_END;

	public:
		static shared_ptr<CLevel_Loading> Create(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext, LEVEL eNextLevel);
	};
}
