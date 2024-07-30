#include "VIBuffer.h"

Engine::CVIBuffer::CVIBuffer(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext)
    :CProperty(pDevice, pContext)
{
}

HRESULT Engine::CVIBuffer::Initialize()
{
    return S_OK;
}

HRESULT Engine::CVIBuffer::Render()
{
    m_pContext->DrawIndexed(m_iNumIndices, 0, 0);

    return S_OK;
}

HRESULT Engine::CVIBuffer::Bind_Buffers()
{
    ID3D11Buffer* pVertexBuffers[] = {
        m_pVB.Get(),
    };

    _uint iVertexStrides[] = {
        m_iVertexStride,
    };

    _uint iOffsets[] = {
        0,
    };
    
    m_pContext->IASetVertexBuffers(0, m_iNumVertexBuffers, pVertexBuffers, iVertexStrides, iOffsets);
    m_pContext->IASetIndexBuffer(m_pIB.Get(), m_eIndexFormat, 0);
    m_pContext->IASetPrimitiveTopology(m_ePrimitiveTopology);

    return S_OK;
}

_bool Engine::CVIBuffer::CheckRayPick(const Ray& ray, _float& fDist)
{
    _float tempDist = 0.f;
    for (size_t i = 0; i < m_vecIndex.size(); i += 3)
    {
        if (ray.Intersects(m_vecVertexPos[m_vecIndex[i]], m_vecVertexPos[m_vecIndex[i + 1]], m_vecVertexPos[m_vecIndex[i + 2]], tempDist))
        {
            fDist = tempDist;
            return true;
        }
    }

    return false;
}

HRESULT Engine::CVIBuffer::Create_Buffer(ID3D11Buffer** ppBuffer)
{
    return m_pDevice->CreateBuffer(&m_BufferDesc,&m_InitialData,ppBuffer);
}
