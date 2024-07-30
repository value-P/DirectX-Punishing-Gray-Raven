#pragma once
#include "Bounding.h"

namespace Engine
{
    class CBounding_AABB final : public CBounding
    {
    public:
        struct INIT_DESC
        {
            Vector3 vCenter;
            Vector3 vExtents;
        };

    public:
        CBounding_AABB(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext);
        virtual ~CBounding_AABB() = default;

    public:
        shared_ptr<void> Get_Bounding() override { return m_pAABB; }
        _float Get_Radius() override { return 0.f; }
        Vector3 Get_Center() override { return m_pAABB->Center; }

    public:
        HRESULT Initialize(const INIT_DESC& InitDesc);
        void Tick(const Matrix& worldMatrix) override;
        HRESULT Render(shared_ptr<PrimitiveBatch<VertexPositionColor>> pBatch) override;

    public:
        _bool Intersect(CCollider::TYPE eType, shared_ptr<CBounding> pBounding) override;

    private:
        shared_ptr<BoundingBox>     m_pOriginalAABB = nullptr;
        shared_ptr<BoundingBox>     m_pAABB = nullptr;

    public:
        static shared_ptr<CBounding_AABB> Create(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext, const INIT_DESC& InitDesc);
    };
}


