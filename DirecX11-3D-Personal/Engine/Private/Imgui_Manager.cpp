#include "Imgui_Manager.h"
#include "ImguiBase.h"

Engine::CImgui_Manager::CImgui_Manager(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext)
	:m_pDevice(pDevice), m_pContext(pContext)
{
}

Engine::CImgui_Manager::~CImgui_Manager()
{
	ImGui_ImplDX11_Shutdown();
	ImGui_ImplWin32_Shutdown();
	ImGui::DestroyContext();
}

HRESULT Engine::CImgui_Manager::Initialize(HWND hWnd, ImGuiContext* ctx)
{
	// Setup Dear ImGui context
	IMGUI_CHECKVERSION();
	ImGui::SetCurrentContext(ctx);
	ImGuiIO& io = ImGui::GetIO(); (void)io;
	io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;     // Enable Keyboard Controls
	io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;      // Enable Gamepad Controls

	// Setup Dear ImGui style
	ImGui::StyleColorsDark();

	ImGui_ImplWin32_Init(hWnd);
	HRESULT result = ImGui_ImplDX11_Init(m_pDevice.Get(), m_pContext.Get()) ? S_OK : E_FAIL;
		
	return result;

	//return S_OK;
}

void Engine::CImgui_Manager::Tick()
{
	// Start the Dear ImGui frame
	ImGui_ImplDX11_NewFrame();
	ImGui_ImplWin32_NewFrame();
	ImGui::NewFrame();

	for (auto& Pair : m_mapImgui)
		Pair.second->Tick();
}

void Engine::CImgui_Manager::Render()
{
	ImGui::Render();
	ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());

	for (auto& Pair : m_mapImgui)
		Pair.second->Render();
}

HRESULT Engine::CImgui_Manager::Add_Imgui(const wstring& strImguiTag, shared_ptr<CImguiBase> pImguiObject)
{
	auto iter = m_mapImgui.find(strImguiTag);

	if (iter != m_mapImgui.end()) return E_FAIL;

	m_mapImgui.emplace(strImguiTag, pImguiObject);

	return S_OK;
}

shared_ptr<CImgui_Manager> Engine::CImgui_Manager::Create(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext, HWND hWnd, ImGuiContext* ctx)
{
	shared_ptr<CImgui_Manager> pInstance = make_shared<CImgui_Manager>(pDevice, pContext);

	if (FAILED(pInstance->Initialize(hWnd, ctx)))
	{
		MSG_BOX("Failed to Created : CImgui_Manager");
		pInstance.reset();
	}

	return pInstance;
}
