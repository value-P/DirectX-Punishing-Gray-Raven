#include "Bounding_AABB.h"

Engine::CBounding_AABB::CBounding_AABB(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext)
	:CBounding(pDevice, pContext)
{
}

HRESULT Engine::CBounding_AABB::Initialize(const INIT_DESC& InitDesc)
{
	m_pOriginalAABB = make_shared<BoundingBox>(InitDesc.vCenter, InitDesc.vExtents);
	m_pAABB = make_shared<BoundingBox>(*m_pOriginalAABB);

	return S_OK;
}

void Engine::CBounding_AABB::Tick(const Matrix& worldMatrix)
{
    Matrix TransformMatrix = worldMatrix;

    TransformMatrix.Right(Vector3(1.f, 0.f, 0.f) * TransformMatrix.Right().Length());
    TransformMatrix.Up(Vector3( 0.f, 1.f, 0.f) * TransformMatrix.Up().Length());
    TransformMatrix.Backward(Vector3(0.f, 0.f, 1.f) * TransformMatrix.Backward().Length());

    m_pOriginalAABB->Transform(*m_pAABB, TransformMatrix);
}

HRESULT Engine::CBounding_AABB::Render(shared_ptr<PrimitiveBatch<VertexPositionColor>> pBatch)
{
    DX::Draw(pBatch.get(), *m_pAABB, m_isCollided == true ? Vector4(1.f, 0.f, 0.f, 1.f) : Vector4(0.f, 1.f, 0.f, 1.f));

	return S_OK;
}

_bool Engine::CBounding_AABB::Intersect(CCollider::TYPE eType, shared_ptr<CBounding> pBounding)
{
    m_isCollided = false;

    shared_ptr<void> pTargetBounding = pBounding->Get_Bounding();

    switch (eType)
    {
    case Engine::CCollider::TYPE_SPHERE:
        m_isCollided = m_pAABB->Intersects(*reinterpret_pointer_cast<BoundingSphere>(pTargetBounding));
        break;
    case Engine::CCollider::TYPE_AABB:
        m_isCollided = m_pAABB->Intersects(*reinterpret_pointer_cast<BoundingBox>(pTargetBounding));
        break;
    case Engine::CCollider::TYPE_OBB:
        m_isCollided = m_pAABB->Intersects(*reinterpret_pointer_cast<BoundingOrientedBox>(pTargetBounding));
        break;
    }

	return m_isCollided;
}

shared_ptr<CBounding_AABB> Engine::CBounding_AABB::Create(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext, const INIT_DESC& InitDesc)
{
    shared_ptr<CBounding_AABB> pInstance = make_shared<CBounding_AABB>(pDevice, pContext);

    if (FAILED(pInstance->Initialize(InitDesc)))
    {
        MSG_BOX("Failed to Created : CBounding_AABB");
        pInstance.reset();
    }

    return pInstance;
}
