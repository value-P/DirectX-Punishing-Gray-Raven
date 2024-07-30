#pragma once
#include "EffectObject.h"

namespace Client
{
	class CMeshEffect;

    class CThrowObject :
        public CEffectObject
    {
	public:
		struct INIT_DESC : public CEffectObject::INIT_DESC
		{
			_float fColRadius;
			wstring strSkillPrefabPath_1;
			wstring strSkillPrefabPath_2;
		};

	public:
		CThrowObject(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext);
		virtual ~CThrowObject() = default;

	public:
		virtual HRESULT Initialize(const INIT_DESC& initDesc);
		virtual void Priority_Tick(_float fTimeDelta) override;
		virtual _int Tick(_float fTimeDelta) override;
		virtual void Late_Tick(_float fTimeDelta) override;
		virtual HRESULT Render() override;

	public:
		virtual void Throw(const Vector3& vStartPos, const Vector3& vTargetPos);
		virtual void InActivate() override;

	protected:
		virtual HRESULT Add_Component(const INIT_DESC& initDesc);
		HRESULT Bind_ShaderResources();

		virtual void OnCollisionEnter(const shared_ptr<CCollider> pSrcCol, const shared_ptr<CCollider> pCollider) override;

	protected:
		shared_ptr<CMeshEffect> m_pMeshEffect1 = nullptr;
		shared_ptr<CMeshEffect> m_pMeshEffect2 = nullptr;

		Vector3 m_vThrowDir = {};

	public:
		static shared_ptr<CThrowObject> Create(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext, const INIT_DESC& initDesc);
    };
}