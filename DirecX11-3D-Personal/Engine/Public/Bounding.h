#pragma once
#include "DebugDraw.h"
#include "Collider.h"

namespace Engine
{
	class CBounding abstract
	{
	public:
		CBounding(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext);
		virtual ~CBounding() = default;

	public:
		virtual shared_ptr<void> Get_Bounding() = 0;
		virtual _float Get_Radius() = 0;
		virtual Vector3 Get_Center() = 0;
		void OnCollide() { m_isCollided = true; }
		void ExitCollide() { m_isCollided = false; }

	public:
		virtual void Tick(const Matrix& worldMatrix) = 0;
		virtual HRESULT Render(shared_ptr<PrimitiveBatch<VertexPositionColor>> pBatch) = 0;
		virtual _bool Intersect(CCollider::TYPE eType, shared_ptr<CBounding> pBounding) = 0;

	protected:
		ComPtr<ID3D11Device> m_pDevice = nullptr;
		ComPtr<ID3D11DeviceContext> m_pContext = nullptr;

		_bool						m_isCollided = { false };
	};
}


