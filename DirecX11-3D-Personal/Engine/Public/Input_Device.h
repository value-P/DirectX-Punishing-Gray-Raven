#pragma once
#include "Engine_Defines.h"

namespace Engine
{
	class CInput_Device
	{
	private:
		enum ClickEvent{ButtonDown, ButtonUp, ButtonEnd};

	public:
		CInput_Device();
		virtual ~CInput_Device() = default;

	public:
		_bool GetKeyDown(_ubyte byKeyID);
		_bool GetKey(_ubyte byKeyID) { return (m_byKeyState[byKeyID] & 0x80); }
		_bool GetKeyUp(_ubyte byKeyID);

		_bool GetMouseButtonDown(MOUSEKEYSTATE eMouseState);
		_bool GetMouseButton(MOUSEKEYSTATE eMouseState) { return (m_tMouseState.rgbButtons[eMouseState] & 0x80); }
		_bool GetMouseButtonUp(MOUSEKEYSTATE eMouseState);

		_long Get_DIMouseMove(MOUSEMOVESTATE eMouseState);

	public:
		HRESULT Initialize(HINSTANCE hInst, HWND hWnd);
		void Tick();

	private:
		void ButtonDownTick();
		void ButtonUpTick();

	private:
		ComPtr<IDirectInput8> m_pInputSDK = nullptr;
		ComPtr<IDirectInputDevice8> m_pKeyBoard = nullptr;
		ComPtr<IDirectInputDevice8> m_pMouse = nullptr;

	private:
		_bool			m_bKeyState[ButtonEnd][256] = {};
		_bool			m_bKeyRefresh[ButtonEnd][256] = {};
		_bool			m_bMouseState[ButtonEnd][MOUSEKEYSTATE::DIM_END] = {};
		_bool			m_bMouseRefresh[ButtonEnd][MOUSEKEYSTATE::DIM_END] = {};
		
		_ubyte			m_byKeyState[256] = {};
		DIMOUSESTATE	m_tMouseState = {};

	public:
		static shared_ptr<CInput_Device> Create(HINSTANCE hInst, HWND hWnd);
	};
}


