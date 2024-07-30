#pragma once
#include "FX.h"

namespace Client
{
	class CTrailFX final : public CFX
	{
	public:
		struct INIT_DESC
		{
			wstring trailTexKey;
			_uint iMaxTrailRectNum;
			Vector3 vLocalUpperPos;
			Vector3 vLocalDownPos;
			Vector4 vColor;
		};

	public:
		CTrailFX(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext);
		virtual ~CTrailFX() = default;

	public:
		virtual HRESULT Initialize(const INIT_DESC& InitDesc);
		virtual _int Tick(const Matrix& matWorld);
		virtual HRESULT Render() override;

	protected:
		shared_ptr<CTrailCom> m_pTrailCom = nullptr;
		Vector3 m_vUpperPos = {};
		Vector3 m_vDownPos = {};
		Vector4 m_vColor = {};

	public:
		static shared_ptr<CTrailFX> Create(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext, const INIT_DESC& InitDesc);
	};
}


