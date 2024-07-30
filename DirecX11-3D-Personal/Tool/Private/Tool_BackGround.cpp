#include "pch.h"
#include "Tool_BackGround.h"
#include "GameInstance.h"

Tool::CTool_BackGround::CTool_BackGround(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext)
	:CGameObject(pDevice, pContext)
{
}

HRESULT CTool_BackGround::Initialize(const wstring& strTextureTag, Vector3 vInitPos)
{
	if (FAILED(__super::Initialize(vInitPos)))
		return E_FAIL;

	if (FAILED(Add_Component(strTextureTag))) return E_FAIL;


	return S_OK;
}

void CTool_BackGround::Priority_Tick(_float fTimeDelta)
{
}

_int CTool_BackGround::Tick(_float fTimeDelta)
{
	return OBJ_ALIVE;
}

void CTool_BackGround::Late_Tick(_float fTimeDelta)
{
	if (FAILED(m_pGameInstance.lock()->Add_RenderGroup(CRenderer::RENDER_PRIORITY, shared_from_this())))
		return;
}

HRESULT CTool_BackGround::Render()
{
	Matrix WorldMatrix = m_pTransformCom->Get_WorldMatrix();
	Matrix ViewMatrix = XMMatrixIdentity();
	Matrix ProjMatrix = XMMatrixOrthographicLH(g_iWinSizeX, g_iWinSizeY, 0.f, 1.f);

	m_pRectCom->Bind_Matrix("g_WorldMatrix", &WorldMatrix);
	m_pRectCom->Bind_Matrix("g_ViewMatrix", &ViewMatrix);
	m_pRectCom->Bind_Matrix("g_ProjMatrix", &ProjMatrix);
	m_pRectCom->Bind_SRV("g_Texture", SpriteTexType::TEX_DIFFUSE, 0);
	if (FAILED(m_pRectCom->Render(0)))
		return E_FAIL;

	return S_OK;
}

HRESULT Tool::CTool_BackGround::Add_Component(const wstring& strTextureTag)
{
	m_pBufferCom = m_pRectCom = CSpriteCom::Create(m_pDevice, m_pContext, m_pGameInstance.lock()->Get_RectProp());
	// 셰이더 세팅
	m_pRectCom->Setting_Shader(m_pGameInstance.lock()->Get_Shader(L"Shader_VtxPosTex"));
	// 텍스처 세팅
	m_pRectCom->Setting_Texture(SpriteTexType::TEX_DIFFUSE, m_pGameInstance.lock()->Get_Texture(LEVEL_STATIC, strTextureTag));

	return S_OK;
}

shared_ptr<CTool_BackGround> Tool::CTool_BackGround::Create(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext, const wstring& strTextureTag, Vector3 vInitPos)
{
	shared_ptr<CTool_BackGround> pInstance = make_shared<CTool_BackGround>(pDevice, pContext);

	if (FAILED(pInstance->Initialize(strTextureTag, vInitPos)))
	{
		MSG_BOX("Failed to Created : CTool_BackGround");
		pInstance.reset();
	}

	return pInstance;
}
