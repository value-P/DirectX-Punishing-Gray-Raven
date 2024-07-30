#pragma once
#include "PrimitiveComponent.h"

namespace Engine
{
    class CTrailBufferPop;

	class ENGINE_DLL CTrailCom final : public CPrimitiveComponent
	{
    public:
        CTrailCom(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext);
        virtual ~CTrailCom() = default;

    public:
        HRESULT Initialize(_int iMaxTrailRectNum);
        void Tick(const Vector3& vPosUpperWorld, const Vector3& vPosDownWorld);
        HRESULT Render(_uint iPassIndex, _uint iTechniqueIndex = 0);

    public:
        HRESULT Setting_Texture(shared_ptr<CTexture> pTexture);
        HRESULT Bind_SRV(const _char* pConstantName);

    private: // �ش� ������Ʈ������ �� �� �ִ� ��ɿ� ����ϱ� ���ؼ�
        shared_ptr<CTrailBufferPop> m_pTrailBuffer = nullptr;
        shared_ptr<CTexture> m_pTexture = nullptr;

    public:
        static shared_ptr<CTrailCom> Create(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext, _int iMaxTrailRectNum);
	};
}


