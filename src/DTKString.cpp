#include "stdafx.h"
#include "DTKString.h"
using namespace DevToolkit;

CString::CString()
{

}

CString::~CString()
{

}

wchar_t* CString::AnsiToUnicode( const char* szAnsiStr )
{
    int nLen = MultiByteToWideChar( CP_ACP, MB_PRECOMPOSED, szAnsiStr, -1, NULL, 0 );
    if ( nLen == 0 )
    {
        return NULL;
    }
    wchar_t* pResult = new wchar_t[nLen];
    MultiByteToWideChar( CP_ACP, MB_PRECOMPOSED, szAnsiStr, -1, pResult, nLen );
    return pResult;
}

