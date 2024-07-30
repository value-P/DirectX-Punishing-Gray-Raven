#include "pch.h"
#include "MapObject.h"

Client::CMapObject::CMapObject(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext)
    :CGameObject(pDevice, pContext)
{
}

HRESULT Client::CMapObject::Initialize(const INIT_DESC& initDesc)
{
	if (FAILED(__super::Initialize(initDesc.vInitPos)))
		return E_FAIL;

	if (FAILED(Add_Component(initDesc.eLevel, initDesc.strModelKey))) return E_FAIL;

	return S_OK;
}

void Client::CMapObject::Priority_Tick(_float fTimeDelta)
{
}

_int Client::CMapObject::Tick(_float fTimeDelta)
{
	return __super::Tick(fTimeDelta);
}

void Client::CMapObject::Late_Tick(_float fTimeDelta)
{
    if (FAILED(m_pGameInstance.lock()->Add_RenderGroup(CRenderer::RENDER_NONBLEND, shared_from_this())))
        return;
    //if (FAILED(m_pGameInstance.lock()->Add_RenderGroup(CRenderer::RENDER_SHADOW, shared_from_this())))
    //    return;
}

HRESULT Client::CMapObject::Render()
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

HRESULT Client::CMapObject::Render_Shadow()
{
	Matrix WorldMatrix = m_pTransformCom->Get_WorldMatrix();
	const Matrix* ViewMatrix = m_pGameInstance.lock()->Get_LightViewMatrix(0);
	Matrix ProjMatrix = m_pGameInstance.lock()->Get_ProjMatrix();

	if (FAILED(m_pMeshCom->Bind_Matrix("g_WorldMatrix", &WorldMatrix)))
		return E_FAIL;
	if (FAILED(m_pMeshCom->Bind_Matrix("g_ViewMatrix", ViewMatrix)))
		return E_FAIL;
	if (FAILED(m_pMeshCom->Bind_Matrix("g_ProjMatrix", &ProjMatrix)))
		return E_FAIL;

	for (_uint i = 0; i < m_pMeshCom->Get_MeshCount(); ++i)
	{
		// 텍스처 바인딩
		if (FAILED(m_pMeshCom->Bind_SRV("g_DiffuseTexture", i, MaterialTexType_DIFFUSE))) return E_FAIL;

		m_pMeshCom->Render(i, 1);
	}

	return S_OK;
}

HRESULT Client::CMapObject::Add_Component(LEVEL eLevel, const wstring& strModelKey)
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

HRESULT Client::CMapObject::Bind_ShaderResources()
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

shared_ptr<CMapObject> Client::CMapObject::Create(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext, const INIT_DESC& initDesc)
{
	shared_ptr<CMapObject> pInstance = make_shared<CMapObject>(pDevice, pContext);

	if (FAILED(pInstance->Initialize(initDesc)))
	{
		MSG_BOX("Failed to Created : CMapObject");
		pInstance.reset();
	}

	return pInstance;
}
