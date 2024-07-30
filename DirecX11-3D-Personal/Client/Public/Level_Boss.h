#pragma once
#include "Level.h"

namespace Client
{
	class CPlayableObject;
	class CPlayableVera;
	class CBoss_Pushin;
 
	class CLevel_Boss : public CLevel
    {
	public:
		CLevel_Boss(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext);
		virtual ~CLevel_Boss() = default;

	public:
		virtual HRESULT Initialize() override;
		virtual void Tick(_float fTimeDelta) override;
		virtual HRESULT Render() override;

	private:
		HRESULT Ready_Layer_Camera(const wstring& strLayerTag);
		HRESULT Ready_Layer_BackGround(const wstring& strLayerTag);
		HRESULT Ready_Layer_Player(const wstring& strLayerTag);
		HRESULT Ready_Layer_Monster(const wstring& strLayerTag);

	private:
		//shared_ptr<CPlayableObject> m_pPlayableChar = nullptr;
		shared_ptr<CPlayableVera> m_pPlayableChar = nullptr;
		shared_ptr<CBoss_Pushin> m_pBoss = nullptr;
	public:
		static shared_ptr<CLevel_Boss> Create(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext);
	};
}

