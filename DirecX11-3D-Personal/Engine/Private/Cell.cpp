#include "Cell.h"
#include "CellBufferProp.h"

Engine::CCell::CCell(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext)
    :m_pDevice(pDevice), m_pContext(pContext)
{
}

HRESULT Engine::CCell::Initialize(const Vector3& vPoint0, const Vector3& vPoint1, const Vector3& vPoint2, _uint iCellIndex)
{
    m_vPoints[0] = vPoint0;
    m_vPoints[1] = vPoint1;
    m_vPoints[2] = vPoint2;

    m_vNormals[LINE_AB] = m_vPoints[POINT_B] - m_vPoints[POINT_A];
    m_vNormals[LINE_AB] = Vector3(m_vNormals[LINE_AB].z * -1.f, 0.f, m_vNormals[LINE_AB].x);
    m_vNormals[LINE_AB].Normalize();

    m_vNormals[LINE_BC] = m_vPoints[POINT_C] - m_vPoints[POINT_B];
    m_vNormals[LINE_BC] = Vector3(m_vNormals[LINE_BC].z * -1.f, 0.f, m_vNormals[LINE_BC].x);
    m_vNormals[LINE_BC].Normalize();

    m_vNormals[LINE_CA] = m_vPoints[POINT_A] - m_vPoints[POINT_C];
    m_vNormals[LINE_CA] = Vector3(m_vNormals[LINE_CA].z * -1.f, 0.f, m_vNormals[LINE_CA].x);
    m_vNormals[LINE_CA].Normalize();


    m_iIndex = iCellIndex;

#ifdef _DEBUG
    m_pBufferProp = CCellBufferProp::Create(m_pDevice, m_pContext, m_vPoints[0], m_vPoints[1], m_vPoints[2]);
    if (nullptr == m_pBufferProp)
        return E_FAIL;
#endif

    return S_OK;
}

_bool Engine::CCell::Compare_Points(const Vector3& vSrcPoint, const Vector3& vDstPoint)
{
    if (m_vPoints[POINT_A] == vSrcPoint)
    {
        if (m_vPoints[POINT_B] == vDstPoint)
            return true;
        if (m_vPoints[POINT_C] == vDstPoint)
            return true;
    }
    if (m_vPoints[POINT_B] == vSrcPoint)
    {
        if (m_vPoints[POINT_C] == vDstPoint)
            return true;
        if (m_vPoints[POINT_A] == vDstPoint)
            return true;
    }
    if (m_vPoints[POINT_C] == vSrcPoint)
    {
        if (m_vPoints[POINT_A] == vDstPoint)
            return true;
        if (m_vPoints[POINT_B] == vDstPoint)
            return true;
    }

    return false;
}

_bool Engine::CCell::isIn(Vector3 vLocalPos, _int& iNeighborIndex)
{
    for (_int i = 0; i < LINE_END; i++)
    {
        Vector3 vDir = vLocalPos - m_vPoints[i];
        vDir.Normalize();

        // 안에 없다면
        if (0 < m_vNormals[i].Dot(vDir))
        {
            // 나의 다른 이웃으로 찾아가도록
            iNeighborIndex = m_iNeighborIndices[i];
            return false;
        }
    }

    return true;
}

#ifdef _DEBUG
HRESULT Engine::CCell::Render()
{
    m_pBufferProp->Bind_Buffers();
    m_pBufferProp->Render();

    return S_OK;
}
#endif

shared_ptr<CCell> Engine::CCell::Create(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext, const Vector3& vPoint0, const Vector3& vPoint1, const Vector3& vPoint2, _uint iCellIndex)
{
    shared_ptr<CCell> pInstance = make_shared<CCell>(pDevice, pContext);

    if (FAILED(pInstance->Initialize(vPoint0, vPoint1, vPoint2, iCellIndex)))
    {
        MSG_BOX("Failed to Created : CCell");
        pInstance.reset();
    }

    return pInstance;
}
