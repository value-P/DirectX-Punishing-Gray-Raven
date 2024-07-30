#pragma once
#include "Level.h"

namespace Client
{
	class CPlayableObject;
	class CPlayableVera;
	class CBoss_StarKnight;
 
	class CLevel_Boss2 : public CLevel
    {
	public:
		CLevel_Boss2(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext);
		virtual ~CLevel_Boss2() = default;

	public:
		virtual HRESULT Initialize() override;
		virtual void Tick(_float fTimeDelta) override;
		virtual HRESULT Render() override;

	private:
		HRESULT Ready_Layer_Camera(const wstring& strLayerTag);
		HRESULT Ready_Layer_BackGround(const wstring& strLayerTag);
		HRESULT Ready_Layer_Player(const wstring& strLayerTag);
		HRESULT Ready_Layer_Monster(const wstring& strLayerTag);
		HRESULT Ready_Layer_SkyBox(const wstring& strLayerTag);

	private:
		shared_ptr<CPlayableVera> m_pPlayableChar = nullptr;
		shared_ptr<CBoss_StarKnight> m_pBoss = nullptr;

	public:
		static shared_ptr<CLevel_Boss2> Create(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext);
	};
}

