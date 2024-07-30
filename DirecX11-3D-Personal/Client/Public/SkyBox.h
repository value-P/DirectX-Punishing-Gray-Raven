#pragma once
#include "GameObject.h"

namespace Client
{
	class CSkyBox final:public CGameObject
	{
	public:
		struct INIT_DESC
		{
			Vector3 vInitPos;
			wstring strTextureKey;
		};

	public:
		CSkyBox(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext);
		virtual ~CSkyBox() = default;

	public:
		virtual HRESULT Initialize(const INIT_DESC& initDesc);
		virtual void Priority_Tick(_float fTimeDelta);
		virtual _int Tick(_float fTimeDelta);
		virtual void Late_Tick(_float fTimeDelta);
		virtual HRESULT Render();

	private:
		HRESULT Add_Component(const wstring& strTextureKey);
		HRESULT Bind_ShaderResources();

	private:
		shared_ptr<CBoxCom> m_pBoxCom = nullptr;

	public:
		static shared_ptr<CSkyBox> Create(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext, const INIT_DESC& initDesc);

	};

}
