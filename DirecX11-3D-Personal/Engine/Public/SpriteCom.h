#pragma once
#include "PrimitiveComponent.h"

namespace Engine
{
    class CRectProp;
    
    class ENGINE_DLL CSpriteCom final: public CPrimitiveComponent
    {
    public:
        CSpriteCom(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext);
        virtual ~CSpriteCom() = default;

    public:
        HRESULT Initialize(shared_ptr<CRectProp> pBufferProperty);
        HRESULT Setting_Texture(SpriteTexType eTexType, shared_ptr<CTexture> pTexture);

    public:
        HRESULT Bind_SRV(const _char* pConstantName, SpriteTexType eTexType, _uint iTextureIndex);
        HRESULT Bind_SRVs(const _char* pConstantName, SpriteTexType eTexType);
    
    private: // �ش� ������Ʈ������ �� �� �ִ� ��ɿ� ����ϱ� ���ؼ�
        shared_ptr<CRectProp> m_pRectProperty = nullptr;
        shared_ptr<CTexture> m_pTextures[(_uint)SpriteTexType::TEX_END];

    public:
        static shared_ptr<CSpriteCom> Create(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext, shared_ptr<CRectProp> pBufferProperty);
    };
}


