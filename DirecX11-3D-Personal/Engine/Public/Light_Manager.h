#pragma once
#include "Engine_Defines.h"

namespace Engine
{
	class CLight;
	class CShader;

	class CLight_Manager final
	{
	public:
		CLight_Manager(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext);
		virtual ~CLight_Manager() = default;

	public:
		const shared_ptr<LIGHT_DESC> Get_LightDesc(_uint iIndex);
		const Matrix* Get_LightViewMatrix(_uint iIndex);

	public:
		HRESULT Initialize();
		HRESULT Add_Light(const LIGHT_DESC& LightDesc);
		HRESULT Render_Light(shared_ptr<CShader> pShader);
		void Clear() { m_pLightList.clear(); }

	private:
		ComPtr<ID3D11Device> m_pDevice = nullptr;
		ComPtr<ID3D11DeviceContext> m_pContext = nullptr;

		list <shared_ptr<CLight>>	m_pLightList;

	public:
		static shared_ptr<CLight_Manager> Create(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext);
	};
}