#pragma once
#include "GameObject.h"

namespace Engine
{
	class CBone;
}

namespace Client
{
    class CCameraReference : public CGameObject
    {
	public:
		CCameraReference(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext);
		virtual ~CCameraReference() = default;

	public:
		Vector3 Get_Pos() { return m_WorldMatrix.Translation(); }
		Matrix Get_WorldMatrix() { return m_WorldMatrix; }

	public:
		virtual HRESULT Initialize(shared_ptr<CBone> socketBone, Vector3 vInitPos);
		virtual void Late_Tick(_float fTimeDelta) override;

	private:
		weak_ptr<CBone> m_SocketBone;
		Matrix m_WorldMatrix = {};

	public:
		static shared_ptr<CCameraReference> Create(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext, shared_ptr<CBone> socketBone, const Vector3& vInitPos);
	};
}


