#include "EffectMeshCom.h"
#include "AnimEffectProp.h"
#include "Shader.h"
#include "Texture.h"

Engine::CEffectMeshCom::CEffectMeshCom(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext)
    :CPrimitiveComponent(pDevice, pContext)
{
}

_bool Engine::CEffectMeshCom::isAnimEffect()
{
    return m_pEffectProp->isAnimMesh();
}

_int Engine::CEffectMeshCom::Get_MeshCount()
{
    return m_pEffectProp->Get_MeshCount();
}

void Engine::CEffectMeshCom::Set_KeyFrame(_int iKeyFrame)
{
    m_pEffectProp->SetKeyFrame(iKeyFrame);
}

HRESULT Engine::CEffectMeshCom::Initialize(shared_ptr<CAnimEffectProp> pEffectProp)
{
    if (nullptr == pEffectProp)
        return E_FAIL;

    m_pEffectProp = pEffectProp->Clone();

    return S_OK;
}

HRESULT Engine::CEffectMeshCom::Render(_uint iMeshIndex, _uint iPassIndex, _uint iTechniqueIndex)
{
    if (iMeshIndex >= m_pEffectProp->Get_MeshCount())
        return E_FAIL;

    if (FAILED(m_pShader->Begin(iPassIndex, iTechniqueIndex)))
        return E_FAIL;

    m_pEffectProp->Render(iMeshIndex);

    return S_OK;
}
 

HRESULT Engine::CEffectMeshCom::Bind_SRV(const _char* pConstantName, _uint iMeshIndex, MaterialTexType eMatrialType)
{
    if (nullptr == m_pShader || nullptr == m_pEffectProp) return E_FAIL;

    ComPtr<ID3D11ShaderResourceView> pSRV = m_pEffectProp->Get_MaterialSRV(iMeshIndex, eMatrialType);

    if (nullptr == pSRV) return S_OK;

    return m_pShader->Bind_SRV(pConstantName, pSRV);
}

HRESULT Engine::CEffectMeshCom::Bind_SRV(const _char* pConstantName, shared_ptr<CTexture> pTex)
{
    if (nullptr == m_pShader || nullptr == m_pEffectProp) return E_FAIL;

    ComPtr<ID3D11ShaderResourceView> pSRV = pTex->GetSRV(0);

    if (nullptr == pSRV) return S_OK;

    return m_pShader->Bind_SRV(pConstantName, pSRV);
}

HRESULT Engine::CEffectMeshCom::Bind_BoneMatrices(const _char* pConstantName, _uint iMeshIndex)
{
    return m_pShader->Bind_Matrices(pConstantName, m_pEffectProp->Get_BoneMatrices(iMeshIndex).data(), 512);
}

void Engine::CEffectMeshCom::Play_Animation(_float fTimeDelta, _bool& lastFrameCalled, _double& fFrameRate, _bool isLoop)
{
    m_pEffectProp->Play_Animation(fTimeDelta, lastFrameCalled, fFrameRate, isLoop);
}

void Engine::CEffectMeshCom::UV_Move(_float fTimeDelta)
{
    m_vUVMove += m_vUVSpeed * fTimeDelta;

    if (m_vUVMove.x > 1.f)
        m_vUVMove.x - 1.f;

    if (m_vUVMove.y > 1.f)
        m_vUVMove.y - 1.f;
}

shared_ptr<CEffectMeshCom> Engine::CEffectMeshCom::Create(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext, shared_ptr<CAnimEffectProp> pEffectProp)
{
    shared_ptr<CEffectMeshCom> pInstance = make_shared<CEffectMeshCom>(pDevice, pContext);

    if (FAILED(pInstance->Initialize(pEffectProp)))
    {
        MSG_BOX("Failed to Created : CEffectMeshCom");
        pInstance.reset();
    }

    return pInstance;
}
