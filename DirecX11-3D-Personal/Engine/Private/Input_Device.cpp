#include "Input_Device.h"

Engine::CInput_Device::CInput_Device()
{
}

_bool Engine::CInput_Device::GetKeyDown(_ubyte byKeyID)
{
	_bool result = m_bKeyState[ButtonDown][byKeyID];
	
	m_bKeyState[ButtonDown][byKeyID] = false;

	return result;
}

_bool Engine::CInput_Device::GetKeyUp(_ubyte byKeyID)
{
	_bool result = m_bKeyState[ButtonUp][byKeyID];

	m_bKeyState[ButtonUp][byKeyID] = false;

	return result;
}

_bool Engine::CInput_Device::GetMouseButtonDown(MOUSEKEYSTATE eMouseState)
{
	_bool result = m_bMouseState[ButtonDown][eMouseState];

	m_bMouseState[ButtonDown][eMouseState] = false;

	return result;
}

_bool Engine::CInput_Device::GetMouseButtonUp(MOUSEKEYSTATE eMouseState)
{
	_bool result = m_bMouseState[ButtonUp][eMouseState];

	m_bMouseState[ButtonUp][eMouseState] = false;

	return result;
}

_long Engine::CInput_Device::Get_DIMouseMove(MOUSEMOVESTATE eMouseState)
{
	switch (eMouseState)
	{
	case Engine::DIMS_X:
		return m_tMouseState.lX;
	case Engine::DIMS_Y:
		return m_tMouseState.lY;
	case Engine::DIMS_Z:
		return m_tMouseState.lZ;
	}

    return (_long)0;
}

HRESULT Engine::CInput_Device::Initialize(HINSTANCE hInst, HWND hWnd)
{
	// DInput 컴객체를 생성하는 함수
	if (FAILED(DirectInput8Create(hInst, DIRECTINPUT_VERSION, IID_IDirectInput8, (void**)m_pInputSDK.GetAddressOf(), nullptr)))
		return E_FAIL;

	// 키보드 객체 생성
	if (FAILED(m_pInputSDK->CreateDevice(GUID_SysKeyboard, m_pKeyBoard.GetAddressOf(), nullptr)))
		return E_FAIL;

	// 생성된 키보드 객체의 대한 정보를 컴 객체에게 전달하는 함수
	m_pKeyBoard->SetDataFormat(&c_dfDIKeyboard);

	// 장치에 대한 독점권을 설정해주는 함수, (클라이언트가 떠있는 상태에서 키 입력을 받을지 말지를 결정하는 함수)
	m_pKeyBoard->SetCooperativeLevel(hWnd, DISCL_BACKGROUND | DISCL_NONEXCLUSIVE);
	
	// 장치에 대한 access 버전을 받아오는 함수
	m_pKeyBoard->Acquire();

	// 마우스 객체 생성
	if (FAILED(m_pInputSDK->CreateDevice(GUID_SysMouse, m_pMouse.GetAddressOf(), nullptr)))
		return E_FAIL;

	// 생성된 마우스 객체의 대한 정보를 컴 객체에게 전달하는 함수
	m_pMouse->SetDataFormat(&c_dfDIMouse);

	// 장치에 대한 독점권을 설정해주는 함수, 클라이언트가 떠있는 상태에서 키 입력을 받을지 말지를 결정하는 함수
	m_pMouse->SetCooperativeLevel(hWnd, DISCL_BACKGROUND | DISCL_NONEXCLUSIVE);

	// 장치에 대한 access 버전을 받아오는 함수
	m_pMouse->Acquire();

    return S_OK;
}

void Engine::CInput_Device::Tick()
{
	m_pKeyBoard->GetDeviceState(sizeof(_byte) * 256, m_byKeyState);
	m_pMouse->GetDeviceState(sizeof(m_tMouseState), &m_tMouseState);

	ButtonDownTick();
	ButtonUpTick();
}

void Engine::CInput_Device::ButtonDownTick()
{
	// 키보드
	for (size_t i = 0; i < 256; i++)
	{
		if (m_byKeyState[i] & 0x80)
		{
			if (true == m_bKeyRefresh[ButtonDown][i])
			{
				m_bKeyState[ButtonDown][i] = true;
				m_bKeyRefresh[ButtonDown][i] = false;
			}
		}
		else
			m_bKeyRefresh[ButtonDown][i] = true;
	}

	// 마우스
	for (size_t i = 0; i < DIM_END; i++)
	{
		if (m_tMouseState.rgbButtons[i] & 0x80)
		{
			if (true == m_bMouseRefresh[ButtonDown][i])
			{
				m_bMouseState[ButtonDown][i] = true;
				m_bMouseRefresh[ButtonDown][i] = false;
			}
		}
		else
			m_bMouseRefresh[ButtonDown][i] = true;
	}
}

void Engine::CInput_Device::ButtonUpTick()
{
	// 키보드
	for (size_t i = 0; i < 256; i++)
	{
		if (0 == (m_byKeyState[i] & 0x80))
		{
			if (true == m_bKeyRefresh[ButtonUp][i])
			{
				m_bKeyState[ButtonUp][i] = true;
				m_bKeyRefresh[ButtonUp][i] = false;
			}
		}
		else
			m_bKeyRefresh[ButtonUp][i] = true;
	}

	// 마우스
	for (size_t i = 0; i < DIM_END; i++)
	{
		if (!(m_tMouseState.rgbButtons[i] & 0x80))
		{
			if (true == m_bMouseRefresh[ButtonUp][i])
			{
				m_bMouseState[ButtonUp][i] = true;
				m_bMouseRefresh[ButtonUp][i] = false;
			}
		}
		else
			m_bMouseRefresh[ButtonUp][i] = true;
	}
}

shared_ptr<CInput_Device> Engine::CInput_Device::Create(HINSTANCE hInst, HWND hWnd)
{
	shared_ptr<CInput_Device> pInstance = make_shared<CInput_Device>();

	if (FAILED(pInstance->Initialize(hInst, hWnd)))
	{
		MSG_BOX("Failed to Created : CInput_Device");
		pInstance.reset();
	}

	return pInstance;
}
