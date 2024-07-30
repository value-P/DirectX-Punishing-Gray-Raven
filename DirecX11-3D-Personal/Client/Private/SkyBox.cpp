#include "pch.h"
#include "SkyBox.h"

Client::CSkyBox::CSkyBox(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext)
	:CGameObject(pDevice,pContext)
{
}

HRESULT Client::CSkyBox::Initialize(const INIT_DESC& initDesc)
{
	if (FAILED(__super::Initialize(initDesc.vInitPos)))
		return E_FAIL;

	if (FAILED(Add_Component(initDesc.strTextureKey))) return E_FAIL;

	m_pTransformCom->Set_Scaling(3.f,3.f,3.f);

	return S_OK;
}

void Client::CSkyBox::Priority_Tick(_float fTimeDelta)
{
	__super::Priority_Tick(fTimeDelta);
}

_int Client::CSkyBox::Tick(_float fTimeDelta)
{
	return __super::Tick(fTimeDelta);
}

void Client::CSkyBox::Late_Tick(_float fTimeDelta)
{
	__super::Late_Tick(fTimeDelta);

	Vector3 vCamPos = m_pGameInstance.lock()->Get_CamPos();
	m_pTransformCom->Set_Pos(vCamPos);

	if (FAILED(m_pGameInstance.lock()->Add_RenderGroup(CRenderer::RENDER_PRIORITY, shared_from_this())))
		return;
}

HRESULT Client::CSkyBox::Render()
{
	if (FAILED(Bind_ShaderResources()))
		return E_FAIL;

	return S_OK;
}

HRESULT Client::CSkyBox::Add_Component(const wstring& strTextureKey)
{
	m_pBufferCom = m_pBoxCom = CBoxCom::Create(m_pDevice, m_pContext);

	if (nullptr == m_pBufferCom)
		return E_FAIL;

	// 텍스처 세팅
	shared_ptr<CTexture> pTex = m_pGameInstance.lock()->Get_Texture(LEVEL_STATIC, strTextureKey);
	if (pTex == nullptr)
		return E_FAIL;
	m_pBoxCom->Setting_Texture(SpriteTexType::TEX_DIFFUSE, pTex);

	// 쉐이더 세팅
	if (FAILED(m_pBoxCom->Setting_Shader(m_pGameInstance.lock()->Get_Shader(L"Shader_VtxCube"))))
		return E_FAIL;

	return S_OK;
}

HRESULT Client::CSkyBox::Bind_ShaderResources()
{
	Matrix WorldMatrix = m_pTransformCom->Get_WorldMatrix();
	Matrix ViewMatrix = m_pGameInstance.lock()->Get_ViewMatrix();
	Matrix ProjMatrix = m_pGameInstance.lock()->Get_ProjMatrix();

	m_pBoxCom->Bind_Matrix("g_WorldMatrix", &WorldMatrix);
	m_pBoxCom->Bind_Matrix("g_ViewMatrix", &ViewMatrix);
	m_pBoxCom->Bind_Matrix("g_ProjMatrix", &ProjMatrix);
	m_pBoxCom->Bind_SRV("g_Texture", SpriteTexType::TEX_DIFFUSE, 0);
  	if (FAILED(m_pBoxCom->Render(0)))
		return E_FAIL;

	return S_OK;
}

shared_ptr<CSkyBox> Client::CSkyBox::Create(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext, const INIT_DESC& initDesc)
{
	shared_ptr<CSkyBox> pInstance = make_shared<CSkyBox>(pDevice, pContext);

	if (FAILED(pInstance->Initialize(initDesc)))
	{
		MSG_BOX("Failed to Created : CSkyBox");
		pInstance.reset();
	}

	return pInstance;
}
