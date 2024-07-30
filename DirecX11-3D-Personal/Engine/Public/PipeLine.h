#pragma once
#include "Engine_Defines.h"

namespace Engine
{
	class CPipeLine
	{
	public:
		CPipeLine();
		virtual ~CPipeLine() = default;

	public:
		void Set_ViewMatrix(const Matrix& viewMatrix) { m_ViewMatrix = viewMatrix; }
		void Set_ProjMatrix(const Matrix& projMatrix) { m_ProjectionMatrix = projMatrix; }
		void Set_CamFar(const _float& fFar)			  { m_fCamFar = fFar; }
		Matrix Get_ViewMatrix()			{return m_ViewMatrix;}
		Matrix Get_ProjMatrix()			{return m_ProjectionMatrix;}
		Matrix Get_ViewMatrixInverse()	{return m_ViewMatrixInverse;}
		Matrix Get_ProjMatrixInverse()	{return m_ProjectionMatrixInverse;}
		Vector3 Get_CamPos()			{return m_vCamPos;}
		_float Get_CamFar()				{return m_fCamFar;}

	public:
		void Tick();

	private:
		Matrix	m_ViewMatrix = {};
		Matrix	m_ProjectionMatrix = {};
		Matrix	m_ViewMatrixInverse = {};
		Matrix	m_ProjectionMatrixInverse = {};
		Vector3 m_vCamPos = {};
		_float	m_fCamFar = {};

	public:
		static shared_ptr<CPipeLine> Create();
	};
}


