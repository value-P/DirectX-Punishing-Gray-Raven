#pragma once
#include "ImguiBase.h"
namespace Tool
{
	class CEffectTool;
	class CSampleUI;

	class CToolMain final : public CImguiBase, public enable_shared_from_this<CToolMain>
	{
	public:
		enum ToolType {Sample, Effect, TypeEnd};

	public:
		CToolMain(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext);
		virtual ~CToolMain() = default;

	public:
		virtual HRESULT Initialize() override;
		virtual void Tick() override;
		virtual void Render() override;

	public:
		shared_ptr<CEffectTool> m_pEffectTool = nullptr;
		shared_ptr<CSampleUI> m_pSampleUI = nullptr;

		_bool UIFlags[TypeEnd] = { true, true};

	public:
		static shared_ptr<CToolMain> Create(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext);

	};
}


