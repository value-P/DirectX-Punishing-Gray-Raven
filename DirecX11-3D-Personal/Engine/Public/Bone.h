#pragma once
#include "Engine_Defines.h"

namespace Engine
{
	class CBone final
	{
	public:
		CBone();
		CBone(const CBone& rhs);
		virtual ~CBone() = default;

	public:
		const _char* Get_BoneName() const { return m_szName; }
		Matrix Get_CombinedTransformationMatrix() const { return m_CombinedTransformationMatrix; }
		void Set_TransformationMatrix(const Matrix& TransformationMatrix) { m_TransformationMatrix = TransformationMatrix; }
		_bool IsRootBone() { return m_bIsRootBone; }
		Matrix Get_InitialTransformationMatrix() { return m_InitialTransformationMatrix; }

	public:
		HRESULT Initialize(const wstring& strFilePath, Matrix pivotMatrix, _uint& outFilePointer);
		void Invalidate_CombinedTransformationMatrix(const vector<shared_ptr<CBone>>& Bones);

	private:
		_int	m_iParentBoneIndex = 0;
		
		_char	m_szName[MAX_PATH] = "";

		Matrix	m_InitialTransformationMatrix; // ���� �ʱ������ �������
		Matrix	m_TransformationMatrix; // �θ� �������� ǥ���� ������ �������
		Matrix	m_CombinedTransformationMatrix; // ���� TransformationMatrix * �θ��� CombinedTransformationMatrix

		_bool	m_bIsRootBone = false;

	public:
		static shared_ptr<CBone> Create(const wstring& strFilePath, Matrix pivotMatrix, _uint& outFilePointer);
		shared_ptr<CBone> Clone();
	};
}


