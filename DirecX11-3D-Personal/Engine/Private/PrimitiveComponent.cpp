#include "PrimitiveComponent.h"
#include "Texture.h"
#include "Shader.h"
#include "VIBuffer.h"

Engine::CPrimitiveComponent::CPrimitiveComponent(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext)
    :CComponent(pDevice, pContext)
{
}

HRESULT Engine::CPrimitiveComponent::Initialize()
{
    return S_OK;
}

HRESULT Engine::CPrimitiveComponent::Setting_Shader(shared_ptr<CShader> pShader)
{
    if (nullptr == pShader) return E_FAIL;

    m_pShader = pShader;

    return S_OK;
}

HRESULT Engine::CPrimitiveComponent::Bind_Matrix(const _char* pConstantName, const Matrix* pMatrix)
{
    if (nullptr == m_pShader) return E_FAIL;

    return m_pShader->Bind_Matrix(pConstantName, pMatrix);
}

HRESULT Engine::CPrimitiveComponent::Bind_Matrices(const _char* pConstantName, const Matrix* pMatrices, _uint iNumMatrices)
{
    if (nullptr == m_pShader) return E_FAIL;

    return m_pShader->Bind_Matrices(pConstantName, pMatrices, iNumMatrices);
}

HRESULT Engine::CPrimitiveComponent::Bind_RawValue(const _char* pConstantName, const void* pData, _uint iLength)
{
    if (nullptr == m_pShader) return E_FAIL;

    return m_pShader->Bind_RawValue(pConstantName, pData, iLength);
}

HRESULT Engine::CPrimitiveComponent::Render(_uint iPassIndex, _uint iTechniqueIndex)
{
    if (FAILED(m_pShader->Begin(iPassIndex, iTechniqueIndex)))
        return E_FAIL;

    if (FAILED(m_pBuffer->Bind_Buffers()))
        return E_FAIL;

    if (FAILED(m_pBuffer->Render()))
        return E_FAIL;

    return S_OK;
}

_bool Engine::CPrimitiveComponent::Check_RayPicked(const Ray& ray, _float& fOutDist)
{
    if (nullptr == m_pBuffer) return false;

    return m_pBuffer->CheckRayPick(ray,fOutDist);
}

