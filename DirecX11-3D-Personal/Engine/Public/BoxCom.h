#pragma once
#include "PrimitiveComponent.h"

namespace Engine
{
    class CCubeProp;

    class ENGINE_DLL CBoxCom final: public CPrimitiveComponent
    {
    public:
        CBoxCom(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext);
        virtual ~CBoxCom() = default;

    public:
        HRESULT Initialize();
        HRESULT Setting_Texture(SpriteTexType eTexType, shared_ptr<CTexture> pTexture);

    public:
        HRESULT Bind_SRV(const _char* pConstantName, SpriteTexType eTexType, _uint iTextureIndex);
        HRESULT Bind_SRVs(const _char* pConstantName, SpriteTexType eTexType);

    private: // �ش� ������Ʈ������ �� �� �ִ� ��ɿ� ����ϱ� ���ؼ�
        shared_ptr<CCubeProp> m_pCubeProperty = nullptr;
        shared_ptr<CTexture> m_pTextures[(_uint)SpriteTexType::TEX_END];

    public:
        static shared_ptr<CBoxCom> Create(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext);
    };
}


