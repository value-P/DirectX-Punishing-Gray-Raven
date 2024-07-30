#include "PipeLine.h"

Engine::CPipeLine::CPipeLine()
{
}

void Engine::CPipeLine::Tick()
{
	m_ViewMatrixInverse = XMMatrixInverse(nullptr, m_ViewMatrix);
	m_ProjectionMatrixInverse = XMMatrixInverse(nullptr, m_ProjectionMatrix);

	m_vCamPos = m_ViewMatrixInverse.Translation();
}

shared_ptr<CPipeLine> Engine::CPipeLine::Create()
{
	shared_ptr<CPipeLine> pInstance = make_shared<CPipeLine>();

	return pInstance;
}
