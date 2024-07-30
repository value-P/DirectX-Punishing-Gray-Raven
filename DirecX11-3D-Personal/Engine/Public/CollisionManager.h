#pragma once
#include "Engine_Defines.h"

namespace Engine
{
	class CCollider;

	class CCollisionManager final
	{
	public:
		struct INIT_DESC
		{
			vector<_uint> layer_Collision_Matrix; // Ŭ���̾�Ʈ���� ������ �޾ƿ�����
		};

		struct CollideInfo
		{
			CollideInfo(shared_ptr<CCollider> srcCol, shared_ptr<CCollider> dstCol) {
				pSrcCol = srcCol; pDstCol = dstCol; 
			}
			_bool operator ==(CollideInfo& colInfo) { 
				return (pSrcCol == colInfo.pSrcCol) && (pDstCol == colInfo.pDstCol); 
			}
			_bool operator !=(CollideInfo& colInfo) {
				return (pSrcCol != colInfo.pSrcCol) || (pDstCol != colInfo.pDstCol);
			}

			shared_ptr<CCollider> pSrcCol;
			shared_ptr<CCollider> pDstCol;
		};

	public:
		CCollisionManager();
		virtual ~CCollisionManager() = default;

	public:
		HRESULT Initialize(_uint iNumLevel, vector<_uint>& layer_Collision_Matrix);
		void Tick(_uint iCurrentLevelIndex);

	public:
		void AddCollider(_uint iLevelIndex, shared_ptr<CCollider> pCollider);
		void Clear(_uint iLevel);

	private:
		void ComputeCollision(_uint iCurrentLevelIndex);

	private:
		_uint m_iNumCollisionLayers = 0;
		// -n��° ���̾��� �ݶ��̴��� � �ݶ��̴����̾�� �浹������ ���� ������ or ��Ʈ������ ���� ������ �ֵ��� �Ѵ�.
		// -�̰��� ���� ������ ��� �ִ� �����̳� �̴�
		vector<_uint> Layer_Collision_Matrix;
		// ���� �������� �����ϴ� �ݶ��̴����� ������ �ִ´�
		using ColVector = vector<list<shared_ptr<CCollider>>>;
		vector<ColVector> m_Colliders;
		// �̹� �����ӿ� �浹ó���� �� �ݶ��̴����� ��Ƴ��� �����̳�
		vector<list<CollideInfo>> m_OnCollidedLists;
		// �ѹ� �浹ó���� �� �ݶ��̴����� ��Ƴ��� �����̳�, Enter, Stay, Exit�� �����ϱ� ���� ����Ѵ�
		vector<list<CollideInfo>> m_StayCollisionLists;

	public:
		static shared_ptr<CCollisionManager> Create(_uint iNumLevel, vector<_uint>& layer_Collision_Matrix);
	};
}


