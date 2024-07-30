#pragma once
#include "EffectObject.h"

namespace Engine
{
	class CBone;
}

namespace Client
{
	class CMeshEffect;

    class CLaserObject final : public CEffectObject
    {
	public:
		struct INIT_DESC : public CEffectObject::INIT_DESC
		{
			Vector3 vColOffset;
			Vector3 vColExtents;
			shared_ptr<CBone> pSocketBone;
			wstring strSkillPrefabPath;
		};

	public:
		CLaserObject(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext);
		virtual ~CLaserObject() = default;

	public:
		virtual HRESULT Initialize(const INIT_DESC& initDesc);
		virtual void Priority_Tick(_float fTimeDelta) override;
		virtual _int Tick(_float fTimeDelta) override;
		virtual void Late_Tick(_float fTimeDelta) override;
		virtual HRESULT Render() override;

	public:
		virtual void LookAt(const Vector3& vTargetPos);
		// 활성화 시키고 duration동안 start에서 Dest까지 LookAt을 시키는 함수
		virtual void ActiveRotationRage(_float fStartRad, _float fDestRad);

	protected:
		virtual HRESULT Add_Component(const INIT_DESC& initDesc);
		HRESULT Bind_ShaderResources();
		virtual void InActivate();

		virtual void OnCollisionEnter(const shared_ptr<CCollider> pSrcCol, const shared_ptr<CCollider> pCollider) override;

	private:
		shared_ptr<CMeshEffect> m_pMeshEffect = nullptr;
		weak_ptr<CBone> m_pSocketBone;
		_bool m_bUseRangeRotation = false;
		_float m_fStartLook = {};
		_float m_fDestLook = {};
		_float m_fCurrentLook = {};


	public:
		static shared_ptr<CLaserObject> Create(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext, const INIT_DESC& initDesc);
	};
}

