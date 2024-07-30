#include "pch.h"
#include "FX.h"

Client::CFX::CFX(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext)
	:CGameObject(pDevice, pContext)
{
}

void Client::CFX::PlayOnce()
{
	m_bActivate = true;
}

void Client::CFX::PlayLoop()
{
	m_bActivate = true;
}

void Client::CFX::Stop()
{
	m_bActivate = false;
}