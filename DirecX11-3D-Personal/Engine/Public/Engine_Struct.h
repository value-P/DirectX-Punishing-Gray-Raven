#ifndef Engine_Struct_h__
#define Engine_Struct_h__

namespace Engine
{
	struct GRAPHIC_DESC
	{
		HWND			hWnd;
		unsigned int	iWinSizeX, iWinSizeY;
		unsigned int	iUpScalingSizeX, iUpScalingSizeY;
		bool			isWindowed;
	};

	struct ENGINE_DLL VTXPOS
	{
		Vector3 vPosition;

		static const unsigned int iNumElements = 1;
		static const D3D11_INPUT_ELEMENT_DESC Elements[1];
	};

	struct ENGINE_DLL VTXPOINT
	{
		Vector3 vPosition;
		Vector2 vPSize;

		static const unsigned int iNumElements = 2;
		static const D3D11_INPUT_ELEMENT_DESC Elements[2];
	};

	struct ENGINE_DLL VTXPOSTEX
	{
		Vector3	vPosition;
		Vector2 vTexCoord;

		static const unsigned int iNumElements = 2;
		static const D3D11_INPUT_ELEMENT_DESC Elements[2];
	};

	struct ENGINE_DLL VTXCUBE
	{
		Vector3	vPosition;
		Vector3 vTexCoord;

		static const unsigned int iNumElements = 2;
		static const D3D11_INPUT_ELEMENT_DESC Elements[2];
	};

	struct ENGINE_DLL VTXNORTEX
	{
		Vector3	vPosition;
		Vector3	vNormal;
		Vector2	vTexCoord;

		static const unsigned int iNumElements = 3;
		static const D3D11_INPUT_ELEMENT_DESC Elements[3];
	};

	struct ENGINE_DLL VTXMESH
	{
		Vector3	vPosition;
		Vector3	vNormal;
		Vector2	vTexCoord;
		Vector3 vTangent;

		static const unsigned int iNumElements = 4;
		static const D3D11_INPUT_ELEMENT_DESC Elements[4];
	};

	struct ENGINE_DLL VTXRECTINSTANCE
	{
		Vector4 vRight, vUp, vLook, vTranslation;
		Vector4 vColor;

		static const unsigned int iNumElements = 7;
		static const D3D11_INPUT_ELEMENT_DESC Elements[7];
	};

	struct ENGINE_DLL VTXANIMMESH
	{
		Vector3	vPosition;
		Vector3	vNormal;
		Vector2	vTexCoord;
		Vector3 vTangent;

		// 이 정점이 어떤 뼈들의 상태에 영향을 받는지
		// 각각의 메시가 사용하는 뼈들의 인덱스
		XMUINT4 vBlendIndices = {};

		// 각각의 뼈에 몇퍼센트의 영향을 받는지 나타내는 가중치(0~1)
		Vector4 vBlendWeights;

		static const unsigned int iNumElements = 6;
		static const D3D11_INPUT_ELEMENT_DESC Elements[6];
	};

	struct ENGINE_DLL VTXINSTANCE
	{
		Vector4 vRight, vUp, vLook, vTranslation;
		Vector4 vColor;
	};

	struct ENGINE_DLL VTX_POINT_INSTANCE
	{
		static const unsigned int iNumElements = 7;
		static const D3D11_INPUT_ELEMENT_DESC Elements[7];
	};

	struct ENGINE_DLL VTX_RECT_INSTANCE
	{
		static const unsigned int iNumElements = 7;
		static const D3D11_INPUT_ELEMENT_DESC Elements[7];
	};

	struct LIGHT_DESC
	{
		enum TYPE {TYPE_DIRECTIONAL, TYPE_POINT, TYPE_END};

		TYPE		eType;
		Vector4		vDirection;
		Vector4		vPosition;
		float		fRange;
		Vector4		vDiffuse;
		Vector4		vAmbient;
		Vector4		vSpecular;
		Vector4		vAt;
	};

	struct FX_EVENT_DATA
	{
		enum FXType{FX_PARTICLE, FX_MESH};

		FXType eFxType;
		string strPrefabName;
		Vector3 vPivot;
	};
}


#endif // Engine_Struct_h__
