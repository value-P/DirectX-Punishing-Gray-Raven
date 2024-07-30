#include "pch.h"
#include "Slider.h"
#include "Shader.h"
#include "Texture.h"
#include "PointProp.h"

Client::CSlider::CSlider(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext)
	:CUIBase(pDevice, pContext)
{
}

HRESULT Client::CSlider::Initialize(const INIT_DESC& initDesc)
{
	if (FAILED(__super::Initialize(initDesc)))
		return E_FAIL;

	m_vColor = initDesc.vColor;

	D3D11_VIEWPORT			ViewPortDesc{};
	_uint		iNumViewports = 1;
	m_pContext->RSGetViewports(&iNumViewports, &ViewPortDesc);

	m_ProjMatrix = XMMatrixOrthographicLH(ViewPortDesc.Width, ViewPortDesc.Height, 0.f, 1.f);

	if (FAILED(Add_Component(initDesc)))
		return E_FAIL;

	return S_OK;
}

_int Client::CSlider::Tick(_float fTimeDelta)
{
	if (false == m_bActivate)
		return OBJ_ALIVE;

	__super::Tick(fTimeDelta);

	m_pGameInstance.lock()->Add_RenderGroup(CRenderer::RENDER_UI, shared_from_this());

	return OBJ_ALIVE;
}

HRESULT Client::CSlider::Render()
{
	Matrix WorldMatrix = m_pTransformCom->Get_WorldMatrix();

	if (FAILED(m_pShader->Bind_Matrix("g_WorldMatrix", &WorldMatrix)))
		return E_FAIL;
	if (FAILED(m_pShader->Bind_Matrix("g_ViewMatrix", &m_ViewMatrix)))
		return E_FAIL;
	if (FAILED(m_pShader->Bind_Matrix("g_ProjMatrix", &m_ProjMatrix)))
		return E_FAIL;
	if (FAILED(m_pShader->Bind_SRV("g_Texture", m_pTexture->GetSRV(0))))
		return E_FAIL;
	if (FAILED(m_pShader->Bind_RawValue("g_fSlideRate", &m_fSlideRate, sizeof(_float))))
		return E_FAIL;
	if (FAILED(m_pShader->Bind_RawValue("g_vColor", &m_vColor, sizeof(Vector4))))
		return E_FAIL;

	if (FAILED(m_pShader->Begin(1, 0)))
		return E_FAIL;

	m_pPointBuffer->Bind_Buffers();
	m_pPointBuffer->Render();

	return S_OK;
}

HRESULT Client::CSlider::Add_Component(const INIT_DESC& initDesc)
{
	m_pShader = m_pGameInstance.lock()->Get_Shader(L"Shader_VtxPoint");
	if (nullptr == m_pShader)
		return E_FAIL;

	m_pTexture = m_pGameInstance.lock()->Get_Texture(LEVEL_STATIC, initDesc.strTexKey);
	if (nullptr == m_pTexture)
		return E_FAIL;

	return S_OK;
}

shared_ptr<CSlider> Client::CSlider::Create(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext, const INIT_DESC& initDesc)
{
	shared_ptr<CSlider> pInstance = make_shared<CSlider>(pDevice, pContext);

	if (FAILED(pInstance->Initialize(initDesc)))
	{
		MSG_BOX("Failed to Created : CSlider");
		pInstance.reset();
	}

	return pInstance;
}
