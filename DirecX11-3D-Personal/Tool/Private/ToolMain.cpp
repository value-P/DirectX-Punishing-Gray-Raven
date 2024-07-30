#include "pch.h"
#include "ToolMain.h"
#include "SampleUI.h"
#include "EffectTool.h"

Tool::CToolMain::CToolMain(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext)
	:CImguiBase(pDevice, pContext)
{
}

HRESULT Tool::CToolMain::Initialize()
{
	m_pSampleUI = CSampleUI::Create(m_pDevice, m_pContext, shared_from_this());
	if (nullptr == m_pSampleUI)
		return E_FAIL;

	m_pEffectTool = CEffectTool::Create(m_pDevice, m_pContext, shared_from_this());
	if (nullptr == m_pEffectTool)
		return E_FAIL;

	return S_OK;
}

void Tool::CToolMain::Tick()
{
	ImGui::Begin("ToolSelect");

	ImGui::Checkbox("SampleUI", &UIFlags[Sample]);
	ImGui::Checkbox("EffectUI", &UIFlags[Effect]);

	ImGui::End();

	if (UIFlags[Sample])
		m_pSampleUI->Tick();
	if (UIFlags[Effect])
		m_pEffectTool->Tick();
}

void Tool::CToolMain::Render()
{
	if (UIFlags[Sample])
		m_pSampleUI->Render();
	if (UIFlags[Effect])
		m_pEffectTool->Render();
}

shared_ptr<CToolMain> Tool::CToolMain::Create(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext)
{
	shared_ptr<CToolMain> pInstance = make_shared<CToolMain>(pDevice, pContext);

	if (FAILED(pInstance->Initialize()))
	{
		MSG_BOX("Failed to Created : CToolMain");
		pInstance.reset();
	}

	return pInstance;
}
