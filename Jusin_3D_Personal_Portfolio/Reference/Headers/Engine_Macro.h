#ifndef Engine_Macro_h__
#define Engine_Macro_h__

#ifndef PI
#define PI 3.141592f
#endif

#define OBJ_DESTROY -1
#define OBJ_ALIVE 0

#ifndef			MSG_BOX
#define			MSG_BOX(_message)			MessageBox(NULL, TEXT(_message), L"System Message", MB_OK)
#endif

#define			BEGIN(NAMESPACE)		namespace NAMESPACE {
#define			END						}

#define			USING(NAMESPACE)	using namespace NAMESPACE;

#define MATERIAL_TEXTURE_TYPE_MAX MaterialTexType_TRANSMISSION

#ifdef	ENGINE_EXPORTS
#define ENGINE_DLL		_declspec(dllexport)
#else
#define ENGINE_DLL		_declspec(dllimport)
#endif

// 싱글턴 매크로
#define DELETE_COPY(classType)								 \
	public:													 \
		classType() = default;								 \
		~classType() = default;								 \
		classType(const classType&) = delete;				 \
		classType(classType&&) = delete;					 \
															 \
		classType& operator=(const classType&) = delete;	 \
		classType& operator=(classType&&) = delete;			 \

#define DECLARE_SINGLETON(classType)						 \
	DELETE_COPY(classType)									 \
	private:												 \
		static shared_ptr<classType> m_pInstance;			 \
															 \
	public:													 \
		static shared_ptr<classType>& GetInstance();		 \
		static void DestroyInstance();						 \

#define IMPLEMENT_SINGLETON(classType)								 \
	shared_ptr<classType> classType::m_pInstance = nullptr;			 \
																	 \
	shared_ptr<classType>& classType::GetInstance()					 \
	{																 \
		if (m_pInstance == nullptr)									 \
		{															 \
			m_pInstance = make_shared<classType>();					 \
		}															 \
		return m_pInstance;											 \
	}																 \
																	 \
	void classType::DestroyInstance()								 \
	{																 \
		if (m_pInstance != nullptr)									 \
		{															 \
			m_pInstance.reset();									 \
		}															 \
	}																 \

#endif // Engine_Macro_h__
