#pragma once
#include "GameObject.h"

namespace Client
{
    class CTerrain final : public CGameObject
    {
	public:
		CTerrain(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext);
		virtual ~CTerrain() = default;

	public:
		virtual HRESULT Initialize(const wstring& strHeightMapName,Vector3 vInitPos);
		virtual void Priority_Tick(_float fTimeDelta);
		virtual _int Tick(_float fTimeDelta);
		virtual void Late_Tick(_float fTimeDelta);
		virtual HRESULT Render();

	private:
		HRESULT Add_Component(const wstring& strHeightMapName);
		HRESULT Bind_ShaderResources();

	private:
		shared_ptr<CTerrainCom> m_pTerrainCom = nullptr;

	public:
		static shared_ptr<CTerrain> Create(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext, const wstring& strHeightMapName, Vector3 vInitPos);

    };
}


