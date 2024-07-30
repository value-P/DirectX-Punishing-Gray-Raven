#pragma once
#include "PrimitiveComponent.h"

namespace Engine
{
    class CPointProp;

    class ENGINE_DLL CPointCom final : public CPrimitiveComponent
    {
    public:
        CPointCom(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext);
        virtual ~CPointCom() = default;

    public:
        HRESULT Initialize();
        HRESULT Setting_Texture(SpriteTexType eTexType, shared_ptr<CTexture> pTexture);

    public:
        HRESULT Bind_SRV(const _char* pConstantName, SpriteTexType eTexType, _uint iTextureIndex);
        HRESULT Bind_SRVs(const _char* pConstantName, SpriteTexType eTexType);

    private: // 해당 컴포넌트에서만 할 수 있는 기능에 사용하기 위해서
        shared_ptr<CPointProp> m_pPointProperty = nullptr;
        shared_ptr<CTexture> m_pTextures[(_uint)SpriteTexType::TEX_END];

    public:
        static shared_ptr<CPointCom> Create(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext);

    };
}


