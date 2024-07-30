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
			vector<_uint> layer_Collision_Matrix; // 클라이언트에서 정보를 받아오도록
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
		// -n번째 레이어의 콜라이더가 어떤 콜라이더레이어와 충돌할지에 대한 정보를 or 비트연산을 통해 가지고 있도록 한다.
		// -이것은 위의 정보를 들고 있는 컨테이너 이다
		vector<_uint> Layer_Collision_Matrix;
		// 현재 레벨에서 존재하는 콜라이더들을 가지고 있는다
		using ColVector = vector<list<shared_ptr<CCollider>>>;
		vector<ColVector> m_Colliders;
		// 이번 프레임에 충돌처리가 된 콜라이더들을 모아놓는 컨테이너
		vector<list<CollideInfo>> m_OnCollidedLists;
		// 한번 충돌처리가 된 콜라이더들을 모아놓는 컨테이너, Enter, Stay, Exit를 구분하기 위해 사용한다
		vector<list<CollideInfo>> m_StayCollisionLists;

	public:
		static shared_ptr<CCollisionManager> Create(_uint iNumLevel, vector<_uint>& layer_Collision_Matrix);
	};
}


