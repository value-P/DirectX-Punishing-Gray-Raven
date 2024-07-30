#pragma once
#include "Engine_Defines.h"

namespace Engine
{
	class CGraphic_Device final
	{
	public:
		CGraphic_Device() = default;
		virtual ~CGraphic_Device();

	public:
		// �׷��� ����̽� �ʱ�ȭ
		HRESULT Init_Graphic_Device(const GRAPHIC_DESC& GraphicDesc, ComPtr<ID3D11Device>& pDeviceOut, ComPtr<ID3D11DeviceContext>& pContextOut);

		// ����� �����
		HRESULT Clear_BackBuffer_View(Color vClearColor);

		// ���̹��� + ���ٽ� ���۸� �����
		HRESULT Clear_DepthStencil_View();

		// �ĸ� ���۸� ���� ���۷� ��ü
		HRESULT Present();

	private:
		// ����ü�ο��� �ʼ������� �ʿ��� �����ʹ� ����۰� �ʿ��Ͽ� ����۸� �����ϱ���/�� ������ �����ش�.
		// ����ü���� ������� == �����(�ؽ���)�� �����ȴ�.
		HRESULT Init_SwapChain(HWND hWnd, _bool isWindowed, _uint iWinCX, _uint iWinCY);
		HRESULT Init_BackBufferRenderTargetView();
		HRESULT Init_DepthStencilRenderTargetView(_uint iWinCX, _uint iWinCY);

	private:
		// �޸� �Ҵ�(��������, �ε�������, �ؽ��ķε�) �İ�ü�� ������ ���õ� ����
		ComPtr<ID3D11Device>			m_pDevice = nullptr;
		// ��� ����(.(���ε��۾�, �������� �׸���)
		ComPtr<ID3D11DeviceContext>		m_pDeviceContext = nullptr;
		// �ĸ� ���ۿ� ���� ���۸� ��ü�� ���鼭 ȭ�鿡 �����ִ� ����
		ComPtr<IDXGISwapChain>			m_pSwapChain = nullptr;
		// ID3D11RenderTargetView : ����Ÿ�ٿ����� ���� �� �ִ� �ؽ�ó Ÿ��.
		ComPtr<ID3D11RenderTargetView>	m_pBackBufferRTV = nullptr;
		// ID3D11DepthStencilView : ���̽��ٽ� ���۷μ� ���� �� �ִ� Ÿ��.
		ComPtr<ID3D11DepthStencilView>	m_pDepthStencilView = nullptr;

	public:
		static shared_ptr<CGraphic_Device> Create(const GRAPHIC_DESC& GraphicDesc, ComPtr<ID3D11Device>& pDeviceOut, ComPtr<ID3D11DeviceContext>& pContextOut);
	};
}
