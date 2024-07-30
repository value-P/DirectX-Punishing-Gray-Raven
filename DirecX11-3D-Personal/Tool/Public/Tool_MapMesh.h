#pragma once
#include "GameObject.h"

namespace Tool
{
	class CTool_MapMesh final : public CGameObject
	{
	public:
		CTool_MapMesh(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext);
		virtual ~CTool_MapMesh() = default;

	public:
		virtual HRESULT Initialize(LEVEL eLevel, const wstring& strModelKey, Vector3 vInitPos);
		virtual void Priority_Tick(_float fTimeDelta);
		virtual _int Tick(_float fTimeDelta);
		virtual void Late_Tick(_float fTimeDelta);
		virtual HRESULT Render();

	private:
		HRESULT Add_Component(LEVEL eLevel, const wstring& strModelKey);
		HRESULT Bind_ShaderResources();

	private:
		shared_ptr<CStaticMeshCom> m_pMeshCom = nullptr;

	public:
		static shared_ptr<CTool_MapMesh> Create(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext, LEVEL eLevel, const wstring& strModelKey, Vector3 vInitPos);

	};
}