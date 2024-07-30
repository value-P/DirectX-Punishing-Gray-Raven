#include "NavMeshProp.h"
#include "Cell.h"
#include "GameInstance.h"
#include "GameObject.h"
#include "Shader.h"

Matrix CNavMeshProp::m_AttachedWorldMatrix = XMMatrixIdentity();

Engine::CNavMeshProp::CNavMeshProp(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext)
    :CProperty(pDevice, pContext)
{
}

HRESULT Engine::CNavMeshProp::Setting_AttachedObject(shared_ptr<CGameObject> pObject)
{
    if (pObject == nullptr)
        return E_FAIL;
    
    m_pAttachedObject = pObject;

    return S_OK;
}

HRESULT Engine::CNavMeshProp::Add_Cell(shared_ptr<CCell> pCell)
{
    if (nullptr == pCell)
        return E_FAIL;

    m_Cells.push_back(pCell);

    return S_OK;
}

Vector3 Engine::CNavMeshProp::Get_NearestPoint(const Vector3 vInput)
{

    for (auto& Cell : m_Cells)
    {
        Vector3 vCellPoint;
        _float fDist;

        vCellPoint = Cell->Get_Point(CCell::POINT_A);
        fDist = (vCellPoint - vInput).Length();
        if (fDist <= 1.f) return vCellPoint;

        vCellPoint = Cell->Get_Point(CCell::POINT_B);
        fDist = (vCellPoint - vInput).Length();
        if (fDist <= 1.f) return vCellPoint;

        vCellPoint = Cell->Get_Point(CCell::POINT_C);
        fDist = (vCellPoint - vInput).Length();
        if (fDist <= 1.f) return vCellPoint;
    }

    return vInput;
}

HRESULT Engine::CNavMeshProp::Initialize(const wstring& strFilePath)
{
    ifstream istream(strFilePath);
    if (istream.fail())
        return E_FAIL;

    Vector3 vPoints[CCell::POINT_END];
    
    while (!istream.eof())
    {
        istream.read((_char*)vPoints, sizeof(Vector3) * CCell::POINT_END);
        shared_ptr<CCell> pCell = CCell::Create(m_pDevice, m_pContext, vPoints[0], vPoints[1], vPoints[2], m_Cells.size());
        m_Cells.push_back(pCell);
    }

    m_iNumCells = m_Cells.size();

    istream.close();

    if(FAILED(Make_Neighbors()))
        return E_FAIL;

#ifdef _DEBUG
    m_pShader = CShader::Create(m_pDevice, m_pContext, TEXT("../Bin/ShaderFiles/Shader_Cell.hlsl"), VTXPOS::Elements, VTXPOS::iNumElements);
    if (nullptr == m_pShader)
        return E_FAIL;
#endif

    return S_OK;
}

HRESULT Engine::CNavMeshProp::Initialize()
{
#ifdef _DEBUG
    m_pShader = CShader::Create(m_pDevice, m_pContext, TEXT("../Bin/ShaderFiles/Shader_Cell.hlsl"), VTXPOS::Elements, VTXPOS::iNumElements);
    if (nullptr == m_pShader)
        return E_FAIL;
#endif

    return S_OK;
}

void Engine::CNavMeshProp::Tick()
{
    if (m_pAttachedObject.lock() != nullptr)
        m_AttachedWorldMatrix = m_pAttachedObject.lock()->Get_Transform()->Get_WorldMatrix();
}

_bool Engine::CNavMeshProp::isMove(const Vector3& vPosition, _int& iCurrentIndex)
{
    _int iNeighborIndex = -1;

    // 현재 존재하고 있는 셀 안에서 움직인다면
    if(true == m_Cells[iCurrentIndex]->isIn(vPosition, iNeighborIndex))
        return true;
    // 현재 셀을 나갔다면
    else
    {
        if (-1 != iNeighborIndex) // 이웃 인덱스가 있다면
        {
            while (true)
            {
                if (-1 == iNeighborIndex)
                    return false;

                if (true == m_Cells[iNeighborIndex]->isIn(vPosition, iNeighborIndex))
                {
                    iCurrentIndex = iNeighborIndex;
                    return true;
                }
            }
        }
        else
            return false;
    }
}

_int Engine::CNavMeshProp::Get_IndexFromPos(const Vector3& vPosition)
{
    int iTemp;
    for (size_t i = 0; i < m_Cells.size(); ++i)
    {
        if (m_Cells[i]->isIn(vPosition, iTemp))
        {
            return m_Cells[i]->Get_Index();
        }
    }

    return -1;
}

#ifdef _DEBUG
HRESULT Engine::CNavMeshProp::Render()
{
    Matrix viewMatrix = CGameInstance::GetInstance()->Get_ViewMatrix();
    Matrix projMatrix = CGameInstance::GetInstance()->Get_ProjMatrix();
    if (FAILED(m_pShader->Bind_Matrix("g_ViewMatrix", &viewMatrix)))
        return E_FAIL;
    if (FAILED(m_pShader->Bind_Matrix("g_ProjMatrix", &projMatrix)))
        return E_FAIL;
    
    Vector4		vColor = Vector4(0.f, 0.f, 0.f, 1.f);
    Matrix	WorldMatrix = m_AttachedWorldMatrix;

    if (-1 == m_iCurrentIndex)
    {
        vColor = Vector4(0.f, 1.f, 0.f, 1.f);
    }
    else
    {
        WorldMatrix._42 += 0.1f;
        vColor = Vector4(1.f, 0.f, 0.f, 1.f);
    }

    if (FAILED(m_pShader->Bind_RawValue("g_vColor", &vColor, sizeof(Vector4))))
        return E_FAIL;

    if (FAILED(m_pShader->Bind_Matrix("g_WorldMatrix", &WorldMatrix)))
        return E_FAIL;

    m_pShader->Begin(0,0);

    if (-1 == m_iCurrentIndex)
    {
        for (auto& pCell : m_Cells)
        {
            if (nullptr != pCell)
                pCell->Render();
        }
    }
    else
        m_Cells[m_iCurrentIndex]->Render();

    return S_OK;
}
#endif

HRESULT Engine::CNavMeshProp::Make_Neighbors()
{
    for (size_t i = 0; i < m_Cells.size() - 1; ++i)
    {
        for (size_t j = 0; j < m_Cells.size() - 1; ++j)
        {
            if (i == j)
                continue;

            if (true == m_Cells[j]->Compare_Points(m_Cells[i]->Get_Point(CCell::POINT_A), m_Cells[i]->Get_Point(CCell::POINT_B)))
                m_Cells[i]->Set_Neighbor(CCell::LINE_AB, m_Cells[j]);
            if (true == m_Cells[j]->Compare_Points(m_Cells[i]->Get_Point(CCell::POINT_B), m_Cells[i]->Get_Point(CCell::POINT_C)))
                m_Cells[i]->Set_Neighbor(CCell::LINE_BC, m_Cells[j]);
            if (true == m_Cells[j]->Compare_Points(m_Cells[i]->Get_Point(CCell::POINT_C), m_Cells[i]->Get_Point(CCell::POINT_A)))
                m_Cells[i]->Set_Neighbor(CCell::LINE_CA, m_Cells[j]);

        }
    }

    /*for (auto& pSrcCell : m_Cells)
    {
        for (auto& pDstCell : m_Cells)
        {
            if (pSrcCell == pDstCell)
                continue;

            if (true == pDstCell->Compare_Points(pSrcCell->Get_Point(CCell::POINT_A), pSrcCell->Get_Point(CCell::POINT_B)))
                pSrcCell->Set_Neighbor(CCell::LINE_AB, pDstCell);
            if (true == pDstCell->Compare_Points(pSrcCell->Get_Point(CCell::POINT_B), pSrcCell->Get_Point(CCell::POINT_C)))
                pSrcCell->Set_Neighbor(CCell::LINE_BC, pDstCell);
            if (true == pDstCell->Compare_Points(pSrcCell->Get_Point(CCell::POINT_C), pSrcCell->Get_Point(CCell::POINT_A)))
                pSrcCell->Set_Neighbor(CCell::LINE_CA, pDstCell);
        }
    }*/

    return S_OK;
}

shared_ptr<CNavMeshProp> Engine::CNavMeshProp::Create(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext)
{
    shared_ptr<CNavMeshProp> pInstance = make_shared<CNavMeshProp>(pDevice, pContext);

    if (FAILED(pInstance->Initialize()))
    {
        MSG_BOX("Failed to Created : CNavMeshProp");
        pInstance.reset();
    }

    return pInstance;
}

shared_ptr<CNavMeshProp> Engine::CNavMeshProp::Create(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext, const wstring& strFilePath)
{
    shared_ptr<CNavMeshProp> pInstance = make_shared<CNavMeshProp>(pDevice, pContext);

    if (FAILED(pInstance->Initialize(strFilePath)))
    {
        MSG_BOX("Failed to Created : CNavMeshProp");
        pInstance.reset();
    }

    return pInstance;
}
