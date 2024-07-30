#pragma once
#include "Bounding.h"

namespace Engine
{
    class CBounding_OBB final : public CBounding
    {
    public:
        struct INIT_DESC
        {
            Vector3 vCenter;
            Vector3 vExtents;
            Vector3 vRadians;
        };

    public:
        CBounding_OBB(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext);
        virtual ~CBounding_OBB() = default;

    public:
        shared_ptr<void> Get_Bounding() override { return m_pOBB; }
        _float Get_Radius() override { return 0.f; }
        Vector3 Get_Center() override { return m_pOBB->Center; }

    public:
        HRESULT Initialize(const INIT_DESC& InitDesc);
        void Tick(const Matrix& worldMatrix) override;
        HRESULT Render(shared_ptr<PrimitiveBatch<VertexPositionColor>> pBatch) override;

    public:
        _bool Intersect(CCollider::TYPE eType, shared_ptr<CBounding> pBounding) override;

    private:
        shared_ptr<BoundingOrientedBox>     m_pOriginalOBB = nullptr;
        shared_ptr<BoundingOrientedBox>     m_pOBB = nullptr;

    public:
        static shared_ptr<CBounding_OBB> Create(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext, const INIT_DESC& InitDesc);

    };
}


