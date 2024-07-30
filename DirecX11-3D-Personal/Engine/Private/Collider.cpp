#include "Collider.h"
#include "Bounding_AABB.h"
#include "Bounding_OBB.h"
#include "Bounding_Sphere.h"
#include "GameInstance.h"
#include "GameObject.h"

Engine::CCollider::CCollider(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext)
	:CComponent(pDevice, pContext)
{
}

_float Engine::CCollider::Get_Radius()
{
    return m_pBounding->Get_Radius();
}

Vector3 Engine::CCollider::Get_Center()
{
    return m_pBounding->Get_Center();
}

HRESULT Engine::CCollider::Initialize(const INIT_DESC& InitDesc)
{
    m_eType = InitDesc.eType;
    m_iColliderLayer = InitDesc.iLayerType;

    if (InitDesc.pOwner == nullptr)
        return E_FAIL;

    m_pOwner = InitDesc.pOwner;

    switch (m_eType)
    {
    case Engine::CCollider::TYPE_SPHERE:
        m_pBounding = CBounding_Sphere::Create(m_pDevice, m_pContext, *reinterpret_pointer_cast<CBounding_Sphere::INIT_DESC>(InitDesc.pInitDesc));
        break;
    case Engine::CCollider::TYPE_AABB:
        m_pBounding = CBounding_AABB::Create(m_pDevice, m_pContext, *reinterpret_pointer_cast<CBounding_AABB::INIT_DESC>(InitDesc.pInitDesc));
        break;
    case Engine::CCollider::TYPE_OBB:
        m_pBounding = CBounding_OBB::Create(m_pDevice, m_pContext, *reinterpret_pointer_cast<CBounding_OBB::INIT_DESC>(InitDesc.pInitDesc));
        break;
    }

#ifdef _DEBUG
    m_pBatch = make_shared<PrimitiveBatch<VertexPositionColor>>(m_pContext.Get());
    m_pEffect = make_shared<BasicEffect>(m_pDevice.Get());

    m_pEffect->SetVertexColorEnabled(true);

    const void* pShaderByteCode = nullptr;
    size_t      iByteCodeLength = 0;

    m_pEffect->GetVertexShaderBytecode(&pShaderByteCode, &iByteCodeLength);

    if (FAILED(m_pDevice->CreateInputLayout(VertexPositionColor::InputElements, VertexPositionColor::InputElementCount, pShaderByteCode, iByteCodeLength, m_pInputLayout.GetAddressOf())))
        return E_FAIL;
#endif

    m_pGameInstance.lock()->Add_Collider(InitDesc.iCreateLevelIndex, shared_from_this());

    return S_OK;
}

void Engine::CCollider::Tick(const Matrix& worldMatrix)
{
    m_pBounding->Tick(worldMatrix);
}

#ifdef _DEBUG
HRESULT Engine::CCollider::Render()
{
    m_pContext->GSSetShader(nullptr, nullptr, 0);

    m_pEffect->SetWorld(XMMatrixIdentity());
    m_pEffect->SetView(m_pGameInstance.lock()->Get_ViewMatrix());
    m_pEffect->SetProjection(m_pGameInstance.lock()->Get_ProjMatrix());

    m_pContext->IASetInputLayout(m_pInputLayout.Get());

    m_pEffect->Apply(m_pContext.Get());

    m_pBatch->Begin();

    m_pBounding->Render(m_pBatch);

    m_pBatch->End();

    return S_OK;
}
#endif // _DEBUG

_bool Engine::CCollider::Intersect(shared_ptr<CCollider> pTargetCollider)
{
    return m_pBounding->Intersect(pTargetCollider->m_eType, pTargetCollider->m_pBounding);
}

void Engine::CCollider::OnCollisionEnter(shared_ptr<CCollider> pTargetCollider)
{
    m_pBounding->OnCollide();
    m_pOwner.lock()->OnCollisionEnter(shared_from_this(), pTargetCollider);
}

void Engine::CCollider::OnCollisionStay(shared_ptr<CCollider> pTargetCollider)
{
    m_pOwner.lock()->OnCollisionStay(shared_from_this(), pTargetCollider);
}

void Engine::CCollider::OnCollisionExit(shared_ptr<CCollider> pTargetCollider)
{
    m_pBounding->ExitCollide();
    m_pOwner.lock()->OnCollisionExit(shared_from_this(), pTargetCollider);
}

shared_ptr<CCollider> Engine::CCollider::Create(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext, const INIT_DESC& InitDesc)
{
    shared_ptr<CCollider> pInstance = make_shared<CCollider>(pDevice, pContext);

    if (FAILED(pInstance->Initialize(InitDesc)))
    {
        MSG_BOX("Failed to Created : CCollider");
        pInstance.reset();
    }

    return pInstance;
}
