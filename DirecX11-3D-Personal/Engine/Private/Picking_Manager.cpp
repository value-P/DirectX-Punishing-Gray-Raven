#include "pch.h"
#include "Picking_Manager.h"
#include "GameInstance.h"

Engine::CPicking_Manager::CPicking_Manager(HWND hWnd, _uint iWinSizeX, _uint iWinSizeY)
	:m_hWnd(hWnd), m_iWinSizeX(iWinSizeX), m_iWinSizeY(iWinSizeY), m_pGameInstance(CGameInstance::GetInstance())
{
}

Ray Engine::CPicking_Manager::RayAtViewSpace()
{
	Ray ray = {};
	POINT pt;
	RECT rc;
	GetCursorPos(&pt);
	ScreenToClient(m_hWnd, &pt);
	
	GetClientRect(m_hWnd,&rc);

	// 투영스페이스 -> 뷰스페이스
	Vector3 vProj;
	vProj.x = (((2.f * (_float)pt.x) / (_float)(rc.right - rc.left)) - 1.f);
	vProj.y = (((-2.f * (_float)pt.y) / (_float)(rc.bottom - rc.top)) + 1.f);
	vProj.z = 0.f;

	Matrix matProjInverse = m_pGameInstance.lock()->Get_ProjMatrixInverse();

	vProj = XMVector3TransformCoord(vProj, matProjInverse);

	ray.position = { 0.f,0.f,0.f };
	ray.direction = vProj;
	ray.direction.Normalize();

	return ray;
}

Ray Engine::CPicking_Manager::RayAtWorldSpace()
{
	Ray ray = RayAtViewSpace();

	Matrix matViewInverse = m_pGameInstance.lock()->Get_ViewMatrixInverse();

	ray.position = XMVector3TransformCoord(ray.position, matViewInverse);
	ray.direction = XMVector3TransformNormal(ray.direction, matViewInverse);

	return ray;
}

shared_ptr<CPicking_Manager> Engine::CPicking_Manager::Create(HWND hWnd, _uint iWinSizeX, _uint iWinSizeY)
{
	return make_shared<CPicking_Manager>(hWnd, iWinSizeX, iWinSizeY);
}
