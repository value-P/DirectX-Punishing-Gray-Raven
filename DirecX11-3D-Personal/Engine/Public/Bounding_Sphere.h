#pragma once
#include "Bounding.h"

namespace Engine
{
    class CBounding_Sphere final : public CBounding
    {
    public:
        struct INIT_DESC
        {
            Vector3 vCenter;
            _float fRadius;
        };

    public:
        CBounding_Sphere(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext);
        virtual ~CBounding_Sphere() = default;

    public:
        shared_ptr<void> Get_Bounding() override { return m_pShpere; }
        _float Get_Radius() override { return m_pShpere->Radius; }
        Vector3 Get_Center() override { return m_pShpere->Center; }
    public:
        HRESULT Initialize(const INIT_DESC& InitDesc);
        void Tick(const Matrix& worldMatrix) override;
        HRESULT Render(shared_ptr<PrimitiveBatch<VertexPositionColor>> pBatch) override;

    public:
        _bool Intersect(CCollider::TYPE eType, shared_ptr<CBounding> pBounding) override;

    private:
        shared_ptr<BoundingSphere>     m_pOriginalShpere = nullptr;
        shared_ptr<BoundingSphere>     m_pShpere = nullptr;

    public:
        static shared_ptr<CBounding_Sphere> Create(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext, const INIT_DESC& InitDesc);
    };
}

