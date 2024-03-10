#include "pch.h"
#include "Terrain.h"

Client::CTerrain::CTerrain(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext)
    :CGameObject(pDevice, pContext)
{
}

HRESULT Client::CTerrain::Initialize(const wstring& strHeightMapName, Vector3 vInitPos)
{
	if (FAILED(__super::Initialize(vInitPos)))
		return E_FAIL;

	if (FAILED(Add_Component(strHeightMapName))) return E_FAIL;

	return S_OK;
}

void Client::CTerrain::Priority_Tick(_float fTimeDelta)
{
}

_int Client::CTerrain::Tick(_float fTimeDelta)
{
	return __super::Tick(fTimeDelta);
}

void Client::CTerrain::Late_Tick(_float fTimeDelta)
{
	if (FAILED(m_pGameInstance.lock()->Add_RenderGroup(CRenderer::RENDER_PRIORITY, shared_from_this())))
		return;
}

HRESULT Client::CTerrain::Render()
{
	if (FAILED(Bind_ShaderResources()))
		return E_FAIL;	

	return S_OK;
}

HRESULT Client::CTerrain::Add_Component(const wstring& strHeightMapName)
{
	m_pBufferCom = m_pTerrainCom = CTerrainCom::Create(m_pDevice, m_pContext,m_pGameInstance.lock()->Get_TerrainProp(LEVEL_GAMEPLAY, strHeightMapName));
	if (nullptr == m_pTerrainCom) 
		return E_FAIL;

	// 쉐이더 세팅
	if(FAILED(m_pTerrainCom->Setting_Shader(m_pGameInstance.lock()->Get_Shader(L"Shader_VtxNorTex"))))
		return E_FAIL;

	// 텍스처 세팅
	if(FAILED(m_pTerrainCom->Setting_Texture(TerrainTexType::TEX_DIFFUSE, m_pGameInstance.lock()->Get_Texture(LEVEL_STATIC, L"Tile"))))
		return E_FAIL;
	if(FAILED(m_pTerrainCom->Setting_Texture(TerrainTexType::TEX_MASK, m_pGameInstance.lock()->Get_Texture(LEVEL_STATIC, L"Mask"))))
		return E_FAIL;

	return S_OK;
}

HRESULT Client::CTerrain::Bind_ShaderResources()
{
	Matrix WorldMatrix = m_pTransformCom->Get_WorldMatrix();
	Matrix ViewMatrix = m_pGameInstance.lock()->Get_ViewMatrix();
	Matrix ProjMatrix = m_pGameInstance.lock()->Get_ProjMatrix();
	Vector4 CamPos(m_pGameInstance.lock()->Get_CamPos());

	if(FAILED(m_pTerrainCom->Bind_Matrix("g_WorldMatrix", &WorldMatrix))) 
		return E_FAIL;
	if(FAILED(m_pTerrainCom->Bind_Matrix("g_ViewMatrix", &ViewMatrix)))
		return E_FAIL;
	if(FAILED(m_pTerrainCom->Bind_Matrix("g_ProjMatrix", &ProjMatrix))) 
		return E_FAIL;
	if(FAILED(m_pTerrainCom->Bind_SRV("g_MaskTexture", TerrainTexType::TEX_MASK, 0)))
		return E_FAIL;
	if(FAILED(m_pTerrainCom->Bind_SRVs("g_DiffuseTexture", TerrainTexType::TEX_DIFFUSE)))
		return E_FAIL;
	if(FAILED(m_pTerrainCom->Bind_RawValue("g_vCamPosition", &CamPos, sizeof(Vector4)))) 
		return E_FAIL;

	const shared_ptr<LIGHT_DESC> pLightDesc = m_pGameInstance.lock()->Get_LightDesc(0);
	if (nullptr == pLightDesc) return E_FAIL;

	if (FAILED(m_pTerrainCom->Bind_RawValue("g_vLightDiffuse", &pLightDesc->vDiffuse, sizeof(Vector4))))
		return E_FAIL;
	if (FAILED(m_pTerrainCom->Bind_RawValue("g_vLightAmbient", &pLightDesc->vAmbient, sizeof(Vector4))))
		return E_FAIL;
	if (FAILED(m_pTerrainCom->Bind_RawValue("g_vLightSpecular", &pLightDesc->vSpecular, sizeof(Vector4))))
		return E_FAIL;

	_uint iPassIndex = 0;

	switch (pLightDesc->eType)
	{
	case LIGHT_DESC::TYPE_POINT:
	{
		if (FAILED(m_pTerrainCom->Bind_RawValue("g_vLightPos", &pLightDesc->vPosition, sizeof(Vector4))))
			return E_FAIL;
		if (FAILED(m_pTerrainCom->Bind_RawValue("g_fLightRange", &pLightDesc->fRange, sizeof(_float))))
			return E_FAIL;
		iPassIndex = 0;
	}
	break;
	case LIGHT_DESC::TYPE_DIRECTIONAL:
	{
		if (FAILED(m_pTerrainCom->Bind_RawValue("g_vLightDir", &pLightDesc->vDirection, sizeof(Vector4))))
			return E_FAIL;
		iPassIndex = 1;
	}
	break;
	}

	if (FAILED(m_pTerrainCom->Render(iPassIndex)))
		return E_FAIL;

	return S_OK;
}

shared_ptr<CTerrain> Client::CTerrain::Create(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext, const wstring& strHeightMapName, Vector3 vInitPos)
{
	shared_ptr<CTerrain> pInstance = make_shared<CTerrain>(pDevice, pContext);

	if (FAILED(pInstance->Initialize(strHeightMapName, vInitPos)))
	{
		MSG_BOX("Failed to Created : CTerrain");
		pInstance.reset();
	}

	return pInstance;
}