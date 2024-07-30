#include "Bounding_OBB.h"

Engine::CBounding_OBB::CBounding_OBB(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext)
    :CBounding(pDevice, pContext)
{
}

HRESULT Engine::CBounding_OBB::Initialize(const INIT_DESC& InitDesc)
{
    Quaternion vRotation = XMQuaternionRotationRollPitchYaw(InitDesc.vRadians.x, InitDesc.vRadians.y, InitDesc.vRadians.z);
    m_pOriginalOBB = make_shared<BoundingOrientedBox>(InitDesc.vCenter, InitDesc.vExtents, vRotation);
    m_pOBB = make_shared<BoundingOrientedBox>(*m_pOriginalOBB);

    return S_OK;
}

void Engine::CBounding_OBB::Tick(const Matrix& worldMatrix)
{
    m_pOriginalOBB->Transform(*m_pOBB, worldMatrix);
}

HRESULT Engine::CBounding_OBB::Render(shared_ptr<PrimitiveBatch<VertexPositionColor>> pBatch)
{
    DX::Draw(pBatch.get(), *m_pOBB, m_isCollided == true ? Vector4(1.f, 0.f, 0.f, 1.f) : Vector4(0.f, 1.f, 0.f, 1.f));

    return S_OK;
}

_bool Engine::CBounding_OBB::Intersect(CCollider::TYPE eType, shared_ptr<CBounding> pBounding)
{
    m_isCollided = false;

    shared_ptr<void> pTargetBounding = pBounding->Get_Bounding();

    switch (eType)
    {
    case Engine::CCollider::TYPE_SPHERE:
        m_isCollided = m_pOBB->Intersects(*reinterpret_pointer_cast<BoundingSphere>(pTargetBounding));
        break;
    case Engine::CCollider::TYPE_AABB:
        m_isCollided = m_pOBB->Intersects(*reinterpret_pointer_cast<BoundingBox>(pTargetBounding));
        break;
    case Engine::CCollider::TYPE_OBB:
        m_isCollided = m_pOBB->Intersects(*reinterpret_pointer_cast<BoundingOrientedBox>(pTargetBounding));
        break;
    }

    return m_isCollided;
}

shared_ptr<CBounding_OBB> Engine::CBounding_OBB::Create(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext, const INIT_DESC& InitDesc)
{
    shared_ptr<CBounding_OBB> pInstance = make_shared<CBounding_OBB>(pDevice, pContext);

    if (FAILED(pInstance->Initialize(InitDesc)))
    {
        MSG_BOX("Failed to Created : CBounding_OBB");
        pInstance.reset();
    }

    return pInstance;
}
