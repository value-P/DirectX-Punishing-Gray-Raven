#pragma once
#include "GameObject.h"

namespace Engine
{
    class CTerrainCom;
}

namespace Tool
{
    class CTool_Terrain : public CGameObject
    {
	public:
		CTool_Terrain(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext);
		CTool_Terrain(const CTool_Terrain& rhs);
		virtual ~CTool_Terrain() = default;

	public:
		virtual HRESULT Initialize(const wstring& strHeightMapKey, _uint iSizeX, _uint iSizeZ, Vector3 vInitPos);
		virtual void Priority_Tick(_float fTimeDelta);
		virtual _int Tick(_float fTimeDelta);
		virtual void Late_Tick(_float fTimeDelta);
		virtual HRESULT Render();

	private:
		HRESULT Add_Component(_uint iSizeX, _uint iSizeZ);
		HRESULT Bind_ShaderResources();

	private:
		shared_ptr<CTerrainCom> m_pTerrainCom = nullptr;
		
	private:
		wstring m_strHeightMapKey = L"";

	public:
		static shared_ptr<CTool_Terrain> Create(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext, const wstring& strHeightMapKey, _uint iSizeX, _uint iSizeZ, Vector3 vInitPos);
    };
}


