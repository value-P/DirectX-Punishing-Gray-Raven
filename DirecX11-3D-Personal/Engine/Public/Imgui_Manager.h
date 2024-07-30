#pragma once
#include "Engine_Defines.h"

namespace Engine
{
	class CImguiBase;

	class CImgui_Manager final
	{

	public:
		CImgui_Manager(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext);
		virtual ~CImgui_Manager();

	public:
		HRESULT Initialize(HWND hWnd, ImGuiContext* ctx);
		void Tick();
		void Render();

	public:
		HRESULT Add_Imgui(const wstring& strImguiTag, shared_ptr<CImguiBase> pImguiObject);

	private:
		ComPtr<ID3D11Device> m_pDevice = nullptr;
		ComPtr<ID3D11DeviceContext> m_pContext = nullptr;

		map<const wstring, shared_ptr<CImguiBase>> m_mapImgui;

	public:
		static shared_ptr<CImgui_Manager> Create(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext, HWND hWnd, ImGuiContext* ctx);
	};
}


