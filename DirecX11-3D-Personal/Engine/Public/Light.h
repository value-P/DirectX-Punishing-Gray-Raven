#pragma once
#include "Engine_Defines.h"

namespace Engine
{
	class CShader;
	class CRectProp;

	class CLight final
	{
	public:
		CLight(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext);
		virtual ~CLight() = default;

	public:
		const shared_ptr<LIGHT_DESC> Get_LightDesc() { return make_shared<LIGHT_DESC>(m_LightDesc); }
		const Matrix* Get_ViewMatrix() { return &m_LightViewMatrix; }

	public:
		HRESULT Initialize(const LIGHT_DESC& LightDesc);
		HRESULT Render(shared_ptr<CShader> pShader, shared_ptr<CRectProp> pRectProp);

	private:
		ComPtr<ID3D11Device>		m_pDevice = nullptr;
		ComPtr<ID3D11DeviceContext> m_pContext = nullptr;
		LIGHT_DESC					m_LightDesc = {};
		Matrix						m_LightViewMatrix = {};

	public:
		static shared_ptr<CLight> Create(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext, const LIGHT_DESC& LightDesc);
	};
}
