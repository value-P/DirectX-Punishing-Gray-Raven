#pragma once
#include "PrimitiveComponent.h"

namespace Engine
{
    class CTerrainProp;

    class ENGINE_DLL CTerrainCom final : public CPrimitiveComponent
    {
    public:
        CTerrainCom(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext);
        virtual ~CTerrainCom() = default;

    public:
        HRESULT Initialize(shared_ptr<CTerrainProp> pBufferProperty);
        HRESULT Setting_Texture(TerrainTexType eTexType, shared_ptr<CTexture> pTexture);

    public:
        HRESULT Bind_SRV(const _char* pConstantName, TerrainTexType eTexType, _uint iTextureIndex);
        HRESULT Bind_SRVs(const _char* pConstantName, TerrainTexType eTexType);

    private: // 터레인 전용 기능을 위해 포인터 보관용
        shared_ptr<CTerrainProp> m_pTerrainProperty = nullptr;
        shared_ptr<CTexture> m_pTextures[(_uint)TerrainTexType::TEX_END];

    public:
        static shared_ptr<CTerrainCom> Create(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext, shared_ptr<CTerrainProp> pBufferProperty);
    };
}

