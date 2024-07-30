#pragma once
#include "GameObject.h"

namespace Client
{
    class CMapObject : public CGameObject
    {
	public:
		struct INIT_DESC
		{
			LEVEL eLevel;
			wstring strModelKey;
			Vector3 vInitPos;
		};

	public:
		CMapObject(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext);
		virtual ~CMapObject() = default;

	public:
		virtual HRESULT Initialize(const INIT_DESC& initDesc);
		virtual void Priority_Tick(_float fTimeDelta) override;
		virtual _int Tick(_float fTimeDelta) override;
		virtual void Late_Tick(_float fTimeDelta) override;
		virtual HRESULT Render() override;
		virtual HRESULT Render_Shadow() override;

	private:
		HRESULT Add_Component(LEVEL eLevel, const wstring& strModelKey);
		HRESULT Bind_ShaderResources();

	private:
		shared_ptr<CStaticMeshCom> m_pMeshCom = nullptr;

	public:
		static shared_ptr<CMapObject> Create(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext, const INIT_DESC& initDesc);

    };
}


