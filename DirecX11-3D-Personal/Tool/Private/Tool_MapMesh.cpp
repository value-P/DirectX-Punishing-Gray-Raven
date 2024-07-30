#include "pch.h"
#include "Tool_MapMesh.h"

Tool::CTool_MapMesh::CTool_MapMesh(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext)
	:CGameObject(pDevice, pContext)
{
}

HRESULT Tool::CTool_MapMesh::Initialize(LEVEL eLevel, const wstring& strModelKey, Vector3 vInitPos)
{
	if (FAILED(__super::Initialize(vInitPos)))
		return E_FAIL;

	if (FAILED(Add_Component(eLevel, strModelKey))) return E_FAIL;

	return S_OK;
}

void Tool::CTool_MapMesh::Priority_Tick(_float fTimeDelta)
{
}

_int Tool::CTool_MapMesh::Tick(_float fTimeDelta)
{
	return OBJ_ALIVE;
}

void Tool::CTool_MapMesh::Late_Tick(_float fTimeDelta)
{
	if (FAILED(m_pGameInstance.lock()->Add_RenderGroup(CRenderer::RENDER_NONBLEND, shared_from_this())))
		return;
}

HRESULT Tool::CTool_MapMesh::Render()
{
	if (FAILED(Bind_ShaderResources()))
		return E_FAIL;

	for (_uint i = 0; i < m_pMeshCom->Get_MeshCount(); ++i)
	{
		// 텍스처 바인딩
		if (FAILED(m_pMeshCom->Bind_SRV("g_DiffuseTexture", i, MaterialTexType_DIFFUSE))) return E_FAIL;

		m_pMeshCom->Render(i, 0);
	}

	return S_OK;
}

HRESULT Tool::CTool_MapMesh::Add_Component(LEVEL eLevel, const wstring& strModelKey)
{
	auto pModelProp = m_pGameInstance.lock()->Get_NonAnimModel(eLevel, strModelKey);
	m_pBufferCom = m_pMeshCom = CStaticMeshCom::Create(m_pDevice, m_pContext, pModelProp);

	if (nullptr == m_pMeshCom)
		return E_FAIL;

	// 쉐이더 세팅
	if (FAILED(m_pMeshCom->Setting_Shader(m_pGameInstance.lock()->Get_Shader(L"Shader_VtxMesh"))))
		return E_FAIL;

	return S_OK;
}

HRESULT Tool::CTool_MapMesh::Bind_ShaderResources()
{
	Matrix WorldMatrix = m_pTransformCom->Get_WorldMatrix();
	Matrix ViewMatrix = m_pGameInstance.lock()->Get_ViewMatrix();
	Matrix ProjMatrix = m_pGameInstance.lock()->Get_ProjMatrix();

	if (FAILED(m_pMeshCom->Bind_Matrix("g_WorldMatrix", &WorldMatrix)))
		return E_FAIL;
	if (FAILED(m_pMeshCom->Bind_Matrix("g_ViewMatrix", &ViewMatrix)))
		return E_FAIL;
	if (FAILED(m_pMeshCom->Bind_Matrix("g_ProjMatrix", &ProjMatrix)))
		return E_FAIL;

	return S_OK;
}

shared_ptr<CTool_MapMesh> Tool::CTool_MapMesh::Create(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext, LEVEL eLevel, const wstring& strModelKey, Vector3 vInitPos)
{
	shared_ptr<CTool_MapMesh> pInstance = make_shared<CTool_MapMesh>(pDevice, pContext);

	if (FAILED(pInstance->Initialize(eLevel, strModelKey, vInitPos)))
	{
		MSG_BOX("Failed to Created : CTool_MapMesh");
		pInstance.reset();
	}

	return pInstance;
}
