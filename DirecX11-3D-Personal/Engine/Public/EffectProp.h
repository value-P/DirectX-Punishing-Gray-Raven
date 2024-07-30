#pragma once
#include "Property.h"

namespace Engine
{
    class CTexture;

    class ENGINE_DLL CEffectProp abstract: public CProperty
    {
    public:
        CEffectProp(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext);
        virtual ~CEffectProp() = default;

    public:
        vector<string>* Get_TextureKeys() { return &m_TextureKeys; }
        shared_ptr<CTexture> Get_Texture(const string& strTextureKey);

        HRESULT Add_Texture(const string& strTexFullPath);
        virtual HRESULT Add_Buffer(const wstring& strBufferPath, const Matrix& pivotMatrix) = 0;

    public:
        virtual HRESULT Initialize();

    private:
        map<string, shared_ptr<CTexture>> m_EffectTextures;
        vector<string> m_TextureKeys;
    };
}