#pragma once
#include "Engine_Defines.h"

class ENGINE_DLL CObject abstract : public enable_shared_from_this<CObject>
{
public:
	CObject() = default;
	virtual ~CObject();

private:
	virtual void Release() {}
};

