#include "StaticMeshCom.h"
#include "NonAnimModelProp.h"
#include "Shader.h"

Engine::CStaticMeshCom::CStaticMeshCom(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext)
    :CPrimitiveComponent(pDevice, pContext)
{
}

_uint Engine::CStaticMeshCom::Get_MeshCount()
{
    return m_pModelProp->Get_NumMeshes();
}

HRESULT Engine::CStaticMeshCom::Initialize(shared_ptr<CNonAnimModelProp> pStaticModelProp)
{
    if (pStaticModelProp == nullptr)
        return E_FAIL;

    m_pModelProp = pStaticModelProp;

    return S_OK;
}

HRESULT Engine::CStaticMeshCom::Render(_uint iMeshIndex, _uint iPassIndex, _uint iTechniqueIndex)
{
    if (iMeshIndex >= m_pModelProp->Get_NumMeshes())
        return E_FAIL;

    if (FAILED(m_pShader->Begin(iPassIndex, iTechniqueIndex)))
        return E_FAIL;

    m_pModelProp->Render(iMeshIndex);

    return S_OK;
}

HRESULT Engine::CStaticMeshCom::Bind_SRV(const _char* pConstantName, _uint iMeshIndex, MaterialTexType eMatrialType)
{
    if (nullptr == m_pShader || nullptr == m_pModelProp) return E_FAIL;

    ComPtr<ID3D11ShaderResourceView> pSRV = m_pModelProp->Get_MaterialSRV(iMeshIndex, eMatrialType);

    if (nullptr == pSRV) return S_OK;

    return m_pShader->Bind_SRV(pConstantName, pSRV);
}

_bool Engine::CStaticMeshCom::Check_RayPicked(const Ray& ray, _float& fOutDist)
{
    return m_pModelProp->Check_RayPicked(ray, fOutDist);
}

shared_ptr<CStaticMeshCom> Engine::CStaticMeshCom::Create(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext, shared_ptr<CNonAnimModelProp> pStaticModelProp)
{
    shared_ptr<CStaticMeshCom> pInstance = make_shared<CStaticMeshCom>(pDevice, pContext);

    if (FAILED(pInstance->Initialize(pStaticModelProp)))
    {
        MSG_BOX("Failed to Created : CStaticMeshCom");
        pInstance.reset();
    }

    return pInstance;
}