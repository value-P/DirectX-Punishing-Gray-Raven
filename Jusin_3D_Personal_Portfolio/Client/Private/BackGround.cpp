#include "pch.h"
#include "BackGround.h"

Client::CBackGround::CBackGround(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext)
	:CGameObject(pDevice, pContext)
{
}

HRESULT CBackGround::Initialize(const wstring& strTextureTag, Vector3 vInitPos)
{
	if (FAILED(__super::Initialize(vInitPos)))
		return E_FAIL;

	if (FAILED(Add_Component(strTextureTag))) return E_FAIL;

	m_fSizeX = (_float)g_iWinSizeX;
	m_fSizeY = (_float)g_iWinSizeY;
	m_fX = g_iWinSizeX >> 1;
	m_fY = g_iWinSizeY >> 1;

	m_pTransformCom->Set_Scaling(m_fSizeX, m_fSizeY, 1.f);

	matProj = XMMatrixOrthographicLH((_float)g_iWinSizeX, (_float)g_iWinSizeY, 0.f, 1.f);

	return S_OK;
}

void CBackGround::Priority_Tick(_float fTimeDelta)
{
}

_int CBackGround::Tick(_float fTimeDelta)
{
	return OBJ_ALIVE;
}

void CBackGround::Late_Tick(_float fTimeDelta)
{
	if (FAILED(m_pGameInstance.lock()->Add_RenderGroup(CRenderer::RENDER_PRIORITY, shared_from_this())))
		return;
}

HRESULT CBackGround::Render()
{
	Matrix WorldMatrix = m_pTransformCom->Get_WorldMatrix();
	Matrix ViewMatrix = XMMatrixIdentity();

	m_pRectCom->Bind_Matrix("g_WorldMatrix", &WorldMatrix);
	m_pRectCom->Bind_Matrix("g_ViewMatrix", &ViewMatrix);
	m_pRectCom->Bind_Matrix("g_ProjMatrix", &matProj);
	m_pRectCom->Bind_SRV("g_Texture", SpriteTexType::TEX_DIFFUSE, 0);
	if (FAILED(m_pRectCom->Render(0)))
		return E_FAIL;

	return S_OK;
}

HRESULT Client::CBackGround::Add_Component(const wstring& strTextureTag)
{
	m_pBufferCom = m_pRectCom = CSpriteCom::Create(m_pDevice, m_pContext, m_pGameInstance.lock()->Get_RectProp());
	// 셰이더 세팅
	m_pRectCom->Setting_Shader(m_pGameInstance.lock()->Get_Shader(L"Shader_VtxPosTex"));
	// 텍스처 세팅
	m_pRectCom->Setting_Texture(SpriteTexType::TEX_DIFFUSE, m_pGameInstance.lock()->Get_Texture(LEVEL_STATIC, strTextureTag));

	return S_OK;
}

shared_ptr<CBackGround> Client::CBackGround::Create(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext, const wstring& strTextureTag, Vector3 vInitPos)
{
	shared_ptr<CBackGround> pInstance = make_shared<CBackGround>(pDevice, pContext);

	if (FAILED(pInstance->Initialize(strTextureTag, vInitPos)))
	{
		MSG_BOX("Failed to Created : CBackGround");
		pInstance.reset();
	}

	return pInstance;
}
