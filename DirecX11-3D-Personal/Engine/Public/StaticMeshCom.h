#pragma once
#include "PrimitiveComponent.h"

namespace Engine
{
    class CNonAnimModelProp;

    class ENGINE_DLL CStaticMeshCom final : public CPrimitiveComponent
    {
    public:
        CStaticMeshCom(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext);
        virtual ~CStaticMeshCom() = default;

    public:
        _uint Get_MeshCount();

    public:
        HRESULT Initialize(shared_ptr<CNonAnimModelProp> pStaticModelProp);
        HRESULT Render(_uint iMeshIndex, _uint iPassIndex, _uint iTechniqueIndex = 0);

    public:
        HRESULT Bind_SRV(const _char* pConstantName, _uint iMeshIndex, MaterialTexType eMatrialType);
       
        virtual _bool Check_RayPicked(const Ray& ray, _float& fOutDist) override;
    
    private:
        shared_ptr<CNonAnimModelProp> m_pModelProp;

    public:
        static shared_ptr<CStaticMeshCom> Create(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext, shared_ptr<CNonAnimModelProp> pStaticModelProp);
    };
};


