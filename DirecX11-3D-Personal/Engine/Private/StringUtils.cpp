#include "StringUtils.h"

wstring CStringUtils::toWstring(const _char* pStr)
{
    _int strLen = strlen(pStr);

    return wstring(pStr, &pStr[strLen]);
}

wstring CStringUtils::toWstring(const string& str)
{
    wstring wStr;
    wStr.assign(str.begin(), str.end());

    return wStr;
}
