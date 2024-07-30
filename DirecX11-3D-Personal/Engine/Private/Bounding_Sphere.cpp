#include "Bounding_Sphere.h"

Engine::CBounding_Sphere::CBounding_Sphere(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext)
	:CBounding(pDevice, pContext)
{
}

HRESULT Engine::CBounding_Sphere::Initialize(const INIT_DESC& InitDesc)
{
	m_pOriginalShpere = make_shared<BoundingSphere>(InitDesc.vCenter, InitDesc.fRadius);
	m_pShpere = make_shared<BoundingSphere>(*m_pOriginalShpere);

	return S_OK;
}

void Engine::CBounding_Sphere::Tick(const Matrix& worldMatrix)
{
	m_pOriginalShpere->Transform(*m_pShpere, worldMatrix);
}

HRESULT Engine::CBounding_Sphere::Render(shared_ptr<PrimitiveBatch<VertexPositionColor>> pBatch)
{    
	DX::Draw(pBatch.get(), *m_pShpere, m_isCollided == true ? XMVectorSet(1.f, 0.f, 0.f, 1.f) : XMVectorSet(0.f, 1.f, 0.f, 1.f));

	return S_OK;
}

_bool Engine::CBounding_Sphere::Intersect(CCollider::TYPE eType, shared_ptr<CBounding> pBounding)
{
    m_isCollided = false;

    shared_ptr<void> pTargetBounding = pBounding->Get_Bounding();

    switch (eType)
    {
    case Engine::CCollider::TYPE_SPHERE:
        m_isCollided = m_pShpere->Intersects(*reinterpret_pointer_cast<BoundingSphere>(pTargetBounding));
        break;
    case Engine::CCollider::TYPE_AABB:
        m_isCollided = m_pShpere->Intersects(*reinterpret_pointer_cast<BoundingBox>(pTargetBounding));
        break;
    case Engine::CCollider::TYPE_OBB:
        m_isCollided = m_pShpere->Intersects(*reinterpret_pointer_cast<BoundingOrientedBox>(pTargetBounding));
        break;
    }

    return m_isCollided;
}

shared_ptr<CBounding_Sphere> Engine::CBounding_Sphere::Create(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext, const INIT_DESC& InitDesc)
{
    shared_ptr<CBounding_Sphere> pInstance = make_shared<CBounding_Sphere>(pDevice, pContext);

    if (FAILED(pInstance->Initialize(InitDesc)))
    {
        MSG_BOX("Failed to Created : CBounding_Sphere");
        pInstance.reset();
    }

    return pInstance;
}
