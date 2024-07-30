#include "pch.h"
#include "Light.h"
#include "Shader.h"
#include "RectProp.h"

Engine::CLight::CLight(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext)
	:m_pDevice(pDevice), m_pContext(pContext)
{
}

HRESULT Engine::CLight::Initialize(const LIGHT_DESC& LightDesc)
{
    m_LightDesc = LightDesc;

	m_LightViewMatrix = XMMatrixLookAtLH(m_LightDesc.vPosition, m_LightDesc.vAt, XMVectorSet(0.f, 1.f, 0.f, 0.f));

	Matrix test = XMMatrixInverse(nullptr, m_LightViewMatrix);

    return S_OK;
}

HRESULT Engine::CLight::Render(shared_ptr<CShader> pShader, shared_ptr<CRectProp> pRectProp)
{
	_uint iPassIndex = 0;

	if (LIGHT_DESC::TYPE_DIRECTIONAL == m_LightDesc.eType)
	{
		iPassIndex = 1;

		if (FAILED(pShader->Bind_RawValue("g_LightDir", &m_LightDesc.vDirection, sizeof(Vector4))))
			return E_FAIL;
	}
	else if (LIGHT_DESC::TYPE_POINT == m_LightDesc.eType)
	{
		iPassIndex = 2;
	}

	if (FAILED(pShader->Bind_RawValue("g_vLightDiffuse", &m_LightDesc.vDiffuse, sizeof(Vector4))))
		return E_FAIL;
	if (FAILED(pShader->Bind_RawValue("g_vLightAmbient", &m_LightDesc.vAmbient, sizeof(Vector4))))
		return E_FAIL;
	//if (FAILED(pShader->Bind_RawValue("g_vLightSpecular", &m_LightDesc.vSpecular, sizeof(Vector4))))
	//	return E_FAIL;

	if (FAILED(pShader->Begin(iPassIndex, 0)))
		return E_FAIL;

	if (FAILED(pRectProp->Bind_Buffers()))
		return E_FAIL;

	if (FAILED(pRectProp->Render()))
		return E_FAIL;

	return S_OK;
}

shared_ptr<CLight> Engine::CLight::Create(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext, const LIGHT_DESC& LightDesc)
{
	shared_ptr<CLight> pInstance = make_shared<CLight>(pDevice, pContext);

	if (FAILED(pInstance->Initialize(LightDesc)))
	{
		MSG_BOX("Failed to Created : CLight");
		pInstance.reset();
	}

	return pInstance;
}
