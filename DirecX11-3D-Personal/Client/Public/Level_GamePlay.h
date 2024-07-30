#pragma once
#include "Level.h"

namespace Client
{
	class CPlayableObject;
	class CPlayableVera;
	class CDistortEffect;
	class CDistortionPointRect;

    class CLevel_GamePlay final : public CLevel
    {
	public:
		CLevel_GamePlay(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext);
		virtual ~CLevel_GamePlay() = default;

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
		//shared_ptr<CPlayableObject> m_pPlayableChar = nullptr;
		shared_ptr<CPlayableVera> m_pPlayableChar = nullptr;
		shared_ptr<CDistortionPointRect> m_pTestEffect = nullptr;
	public:
		static shared_ptr<CLevel_GamePlay> Create(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext);
    };
}


