#pragma once
#include "Component.h"

namespace Engine
{
    class CVIBuffer;
    class CShader;
    class CTexture;

    class ENGINE_DLL CPrimitiveComponent abstract : public CComponent
    {
    public:
        CPrimitiveComponent(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext);
        virtual ~CPrimitiveComponent() = default;

    public:
        virtual HRESULT Initialize() override;

    public:
        HRESULT Setting_Shader(shared_ptr<CShader> pShader);

        HRESULT Bind_Matrix(const _char* pConstantName, const Matrix* pMatrix);
        HRESULT Bind_Matrices(const _char* pConstantName, const Matrix* pMatrices, _uint iNumMatrices);
        HRESULT Bind_RawValue(const _char* pConstantName, const void* pData, _uint iLength);
        virtual HRESULT Render(_uint iPassIndex, _uint iTechniqueIndex = 0);

        virtual _bool Check_RayPicked(const Ray& ray, _float& fOutDist);

    protected:
        shared_ptr<CVIBuffer> m_pBuffer = nullptr;
        shared_ptr<CShader> m_pShader = nullptr;
    };
}


