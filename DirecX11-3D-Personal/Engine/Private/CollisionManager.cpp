#include "CollisionManager.h"
#include "Collider.h"

Engine::CCollisionManager::CCollisionManager()
{
}

HRESULT Engine::CCollisionManager::Initialize(_uint iNumLevel, vector<_uint>& layer_Collision_Matrix)
{
    m_iNumCollisionLayers = static_cast<_uint>(layer_Collision_Matrix.size());
    Layer_Collision_Matrix.assign(layer_Collision_Matrix.begin(), layer_Collision_Matrix.end());
    
    m_Colliders.resize(iNumLevel);
    for (_uint i = 0; i < iNumLevel; ++i)
        m_Colliders[i].resize(m_iNumCollisionLayers);

    m_OnCollidedLists.resize(m_iNumCollisionLayers);
    m_StayCollisionLists.resize(m_iNumCollisionLayers);

    return S_OK;
}

void Engine::CCollisionManager::Tick(_uint iCurrentLevelIndex)
{
    ComputeCollision(iCurrentLevelIndex);

    for (_uint i = 0; i < m_iNumCollisionLayers; ++i)
    {
        for (auto& colliderInfo : m_OnCollidedLists[i])
        {
            auto iter = find_if(m_StayCollisionLists[i].begin(), m_StayCollisionLists[i].end(), [&](CollideInfo ColInfo)->_bool {
                return colliderInfo == ColInfo; });

            // �̹� ƽ�� ó������ �浹�� �ݶ��̴��� OnCollisionEnter�� ȣ���ϰ� StayCollisionList�� ����
            if (iter == m_StayCollisionLists[i].end())
            {
                colliderInfo.pSrcCol->OnCollisionEnter(colliderInfo.pDstCol);
                colliderInfo.pDstCol->OnCollisionEnter(colliderInfo.pSrcCol);
                m_StayCollisionLists[i].push_back(colliderInfo);
            }
        }

        for (auto colliderInfo = m_StayCollisionLists[i].begin(); colliderInfo != m_StayCollisionLists[i].end();)
        {
            auto iter = find_if(m_OnCollidedLists[i].begin(), m_OnCollidedLists[i].end(), [&](CollideInfo ColInfo)->_bool
                {
                return (*colliderInfo) == ColInfo; 
                });

            // �̹� ƽ�� �浹���� �������� �浹���̴� ����Ʈ�� ����ִٸ� OnCollisionExit�� ȣ���ϰ� ����Ʈ���� ����
            if (iter == m_OnCollidedLists[i].end())
            {
                colliderInfo->pSrcCol->OnCollisionExit(colliderInfo->pDstCol);
                colliderInfo->pDstCol->OnCollisionExit(colliderInfo->pSrcCol);
                colliderInfo = m_StayCollisionLists[i].erase(colliderInfo);
            }
            // �̹�ƽ���� �浹�� �Ǿ��ٸ� OnCollisionStay ȣ��
            else
            {
                colliderInfo->pSrcCol->OnCollisionStay(colliderInfo->pDstCol);
                colliderInfo->pDstCol->OnCollisionStay(colliderInfo->pSrcCol);
                ++colliderInfo;
            }
        }

        // �� ƽ���� �浹�� �ֵ鸸 ��Ƴ��� ���� ����ֱ�
        m_OnCollidedLists[i].clear();
    }
}

void Engine::CCollisionManager::AddCollider(_uint iLevelIndex, shared_ptr<CCollider> pCollider)
{
    _uint iColLayer = pCollider->Get_ColliderLayer();

    m_Colliders[iLevelIndex][iColLayer].push_back(pCollider);
}

void Engine::CCollisionManager::Clear(_uint iLevel)
{
    for (_uint i = 0; i < m_iNumCollisionLayers; ++i)
    {
        m_Colliders[iLevel][i].clear();
        m_OnCollidedLists[i].clear();
        m_StayCollisionLists[i].clear();
    }
}

void Engine::CCollisionManager::ComputeCollision(_uint iCurrentLevelIndex)
{
    for (_uint i = 0; i < m_iNumCollisionLayers; ++i)
    {
        for (_uint j = i; j < m_iNumCollisionLayers; ++j)
        {
            // i ��° ���̾�� j��° ���̾ �浹������ �����̷� ��ϵǾ����� Ȯ��
            if (Layer_Collision_Matrix[i] & (1 << j))
            {
                // ���� ���̾� �ȿ��� �浹���� ��
                if (i == j)
                {
                    for (auto iter = m_Colliders[iCurrentLevelIndex][i].begin(); iter != m_Colliders[iCurrentLevelIndex][i].end(); ++iter)
                    {
                        for (auto riter = m_Colliders[iCurrentLevelIndex][j].rbegin(); riter != m_Colliders[iCurrentLevelIndex][j].rend(); ++riter)
                        {
                            // Ż������ 
                            if ((*iter) == (*riter))
                                break;
                            // ���� �� �� ��Ȱ��ȭ ���¶�� �Ѿ��
                            else if (false == (*iter)->IsActivate() || false == (*riter)->IsActivate())
                                continue;
                            else
                            {
                                if ((*iter)->Intersect(*riter))
                                    m_OnCollidedLists[i].push_back(CollideInfo(*iter, *riter));
                            }
                        }
                    }
                }
                // ���� �ٸ� ���̾ �浹���� ��
                else
                {
                    for (auto iter = m_Colliders[iCurrentLevelIndex][i].begin(); iter != m_Colliders[iCurrentLevelIndex][i].end(); ++iter)
                    {
                        for (auto iter2 = m_Colliders[iCurrentLevelIndex][j].begin(); iter2 != m_Colliders[iCurrentLevelIndex][j].end(); ++iter2)
                        {
                            // ���� �ϳ��� ��Ȱ��ȭ ���¶�� üũ���� �ʵ���
                            if (false == (*iter)->IsActivate() || false == (*iter2)->IsActivate())
                                continue;
                            else if((*iter)->Intersect(*iter2))
                                m_OnCollidedLists[i].push_back(CollideInfo(*iter, *iter2));
                        }
                    }
                }
            }
        }
    }
}

shared_ptr<CCollisionManager> Engine::CCollisionManager::Create(_uint iNumLevel, vector<_uint>& layer_Collision_Matrix)
{
    shared_ptr<CCollisionManager> pInstance = make_shared<CCollisionManager>();

    if (FAILED(pInstance->Initialize(iNumLevel, layer_Collision_Matrix)))
    {
        MSG_BOX("Failed to Created : CCollisionManager");
        pInstance.reset();
    }

    return pInstance;
}
