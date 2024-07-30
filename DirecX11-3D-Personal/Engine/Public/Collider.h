#pragma once
#include "Component.h"


namespace Engine
{
    class CBounding; 
    class CGameObject;

    class ENGINE_DLL CCollider : public CComponent, public enable_shared_from_this<CCollider>
    {
    public:
        enum TYPE { TYPE_SPHERE, TYPE_AABB, TYPE_OBB, TYPE_END };

    public:
        struct INIT_DESC
        {
            _uint iCreateLevelIndex;
            TYPE eType;
            _uint iLayerType;
            shared_ptr<void> pInitDesc;
            shared_ptr<CGameObject> pOwner;
        };

    public:
        CCollider(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext);
        virtual ~CCollider() = default;

    public:
        weak_ptr<CGameObject> Get_Owner() { return m_pOwner; }
        _uint Get_ColliderLayer() { return m_iColliderLayer; }
        void Set_Activate(_bool activate) { m_bActivate = activate; }
        _bool IsActivate() { return m_bActivate; }
        _float Get_Radius();
        Vector3 Get_Center();

    public:
        HRESULT Initialize(const INIT_DESC& InitDesc);
        
        void Tick(const Matrix& worldMatrix);

#ifdef _DEBUG
        HRESULT Render() override;
#endif // _DEBUG

    public:
        _bool Intersect(shared_ptr<CCollider> pTargetCollider);
        void OnCollisionEnter(shared_ptr<CCollider> pTargetCollider);
        void OnCollisionStay(shared_ptr<CCollider> pTargetCollider);
        void OnCollisionExit(shared_ptr<CCollider> pTargetCollider);

    private:
        TYPE        m_eType = TYPE_END;
        _uint       m_iColliderLayer = -1;
        _bool       m_bActivate = true;

    private:
        weak_ptr<CGameObject> m_pOwner;
        shared_ptr<CBounding> m_pBounding = nullptr;

#ifdef _DEBUG
        shared_ptr<PrimitiveBatch<VertexPositionColor>> m_pBatch = nullptr;
        shared_ptr<BasicEffect>                         m_pEffect = nullptr;
        ComPtr<ID3D11InputLayout>                       m_pInputLayout = nullptr;
#endif
        
    public:
        static shared_ptr<CCollider> Create(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext, const INIT_DESC& InitDesc);
    };
}


