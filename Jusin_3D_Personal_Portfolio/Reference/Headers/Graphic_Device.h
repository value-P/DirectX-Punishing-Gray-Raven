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
		// 그래픽 디바이스 초기화
		HRESULT Init_Graphic_Device(const GRAPHIC_DESC& GraphicDesc, ComPtr<ID3D11Device>& pDeviceOut, ComPtr<ID3D11DeviceContext>& pContextOut);

		// 백버퍼 지우기
		HRESULT Clear_BackBuffer_View(Color vClearColor);

		// 깊이버퍼 + 스텐실 버퍼를 지운다
		HRESULT Clear_DepthStencil_View();

		// 후면 버퍼를 전면 버퍼로 교체
		HRESULT Present();

	private:
		// 스왑체인에게 필수적으로 필요한 데이터는 백버퍼가 필요하여 백버퍼를 생성하기위/한 정보를 던져준다.
		// 스왑체인을 만들었다 == 백버퍼(텍스쳐)가 생성된다.
		HRESULT Init_SwapChain(HWND hWnd, _bool isWindowed, _uint iWinCX, _uint iWinCY);
		HRESULT Init_BackBufferRenderTargetView();
		HRESULT Init_DepthStencilRenderTargetView(_uint iWinCX, _uint iWinCY);

	private:
		// 메모리 할당(정점버퍼, 인덱스버퍼, 텍스쳐로드) 컴객체의 생성과 관련된 역할
		ComPtr<ID3D11Device>			m_pDevice = nullptr;
		// 기능 실행(.(바인딩작업, 정점버퍼 그리기)
		ComPtr<ID3D11DeviceContext>		m_pDeviceContext = nullptr;
		// 후면 버퍼와 전면 버퍼를 교체해 가면서 화면에 보여주는 역할
		ComPtr<IDXGISwapChain>			m_pSwapChain = nullptr;
		// ID3D11RenderTargetView : 렌더타겟용으로 사용될 수 있는 텍스처 타입.
		ComPtr<ID3D11RenderTargetView>	m_pBackBufferRTV = nullptr;
		// ID3D11DepthStencilView : 깊이스텐실 버퍼로서 사용될 수 있는 타입.
		ComPtr<ID3D11DepthStencilView>	m_pDepthStencilView = nullptr;

	public:
		static shared_ptr<CGraphic_Device> Create(const GRAPHIC_DESC& GraphicDesc, ComPtr<ID3D11Device>& pDeviceOut, ComPtr<ID3D11DeviceContext>& pContextOut);
	};
}
