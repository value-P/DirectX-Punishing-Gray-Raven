#include "pch.h"
#include "Tool_Terrain.h"
#include "TerrainProp.h"

Tool::CTool_Terrain::CTool_Terrain(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext)
    :CGameObject(pDevice, pContext)
{
}

Tool::CTool_Terrain::CTool_Terrain(const CTool_Terrain& rhs)
    :CGameObject(rhs)
{
}

HRESULT Tool::CTool_Terrain::Initialize(const wstring& strHeightMapKey, _uint iSizeX, _uint iSizeZ, Vector3 vInitPos)
{
	if (FAILED(__super::Initialize(vInitPos)))
		return E_FAIL;

	if (FAILED(Add_Component(iSizeX, iSizeZ))) return E_FAIL;

	m_strHeightMapKey = strHeightMapKey;

	return S_OK;
}

void Tool::CTool_Terrain::Priority_Tick(_float fTimeDelta)
{
}

_int Tool::CTool_Terrain::Tick(_float fTimeDelta)
{
	return OBJ_ALIVE;
}

void Tool::CTool_Terrain::Late_Tick(_float fTimeDelta)
{
	if (FAILED(m_pGameInstance.lock()->Add_RenderGroup(CRenderer::RENDER_NONBLEND, shared_from_this())))
		return;
}

HRESULT Tool::CTool_Terrain::Render()
{
	if (FAILED(Bind_ShaderResources()))
		return E_FAIL;

	if (FAILED(m_pTerrainCom->Render(0)))
		return E_FAIL;

	return S_OK;
}

HRESULT Tool::CTool_Terrain::Add_Component(_uint iSizeX, _uint iSizeZ)
{
	m_pBufferCom = m_pTerrainCom = CTerrainCom::Create(m_pDevice, m_pContext, CTerrainProp::Create(m_pDevice, m_pContext, iSizeX, iSizeZ));
	
	// 쉐이더 세팅
	m_pTerrainCom->Setting_Shader(m_pGameInstance.lock()->Get_Shader(L"Shader_VtxNorTex"));
	
	// 텍스처 세팅
	if (FAILED(m_pTerrainCom->Setting_Texture(TerrainTexType::TEX_DIFFUSE, m_pGameInstance.lock()->Get_Texture(LEVEL_STATIC, L"Tile"))))
		return E_FAIL;
	if (FAILED(m_pTerrainCom->Setting_Texture(TerrainTexType::TEX_MASK, m_pGameInstance.lock()->Get_Texture(LEVEL_STATIC, L"Mask"))))
		return E_FAIL;

	return S_OK;
}

HRESULT Tool::CTool_Terrain::Bind_ShaderResources()
{
	Matrix WorldMatrix = m_pTransformCom->Get_WorldMatrix();
	Matrix ViewMatrix = m_pGameInstance.lock()->Get_ViewMatrix();
	Matrix ProjMatrix = m_pGameInstance.lock()->Get_ProjMatrix();

	if (FAILED(m_pTerrainCom->Bind_Matrix("g_WorldMatrix", &WorldMatrix)))
		return E_FAIL;
	if (FAILED(m_pTerrainCom->Bind_Matrix("g_ViewMatrix", &ViewMatrix)))
		return E_FAIL;
	if (FAILED(m_pTerrainCom->Bind_Matrix("g_ProjMatrix", &ProjMatrix)))
		return E_FAIL;
	if (FAILED(m_pTerrainCom->Bind_SRVs("g_DiffuseTexture", TerrainTexType::TEX_DIFFUSE)))
		return E_FAIL;
	if (FAILED(m_pTerrainCom->Bind_SRV("g_MaskTexture", TerrainTexType::TEX_MASK, 0)))
		return E_FAIL;

	return S_OK;
}


shared_ptr<CTool_Terrain> Tool::CTool_Terrain::Create(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext, const wstring& strHeightMapKey, _uint iSizeX, _uint iSizeZ, Vector3 vInitPos)
{
	shared_ptr<CTool_Terrain> pInstance = make_shared<CTool_Terrain>(pDevice, pContext);

	if (FAILED(pInstance->Initialize(strHeightMapKey, iSizeX, iSizeZ, vInitPos)))
	{
		MSG_BOX("Failed to Created : CTool_Terrain");
		pInstance.reset();
	}

	return pInstance;
}