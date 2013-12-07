#pragma once
#include <winhttp.h>

namespace DevToolkit
{
    class EXPORTS_CLASS CHttpClient
    {
        typedef void ( *CallBackDataRecv )( BYTE* pData, DWORD dwDataSize );
    public:
        CHttpClient( LPCTSTR lpszURL, CallBackDataRecv callBack = NULL );
        virtual ~CHttpClient( void );
        
        BOOL SendHttpRequest();
        LPCTSTR GetHttpHeader();
        LPCTSTR GetHttpResponse();
        BOOL SaveResponseToFile( LPCTSTR lpszFileName );
    private:
        TCHAR* m_lpszURL;			//	完整的URL
        TCHAR* m_lpszHttpHeader;	//	Http头
        BYTE* m_lpszHttpResponse;	//	Http响应信息
        TCHAR* m_lpszHostName;		//	主机名
        TCHAR* m_lpszUrlPath;		//	Url相对路径
        INTERNET_PORT m_wPort;		//	端口号
        DWORD m_dwResponseLength;	//	响应信息长度
    private:
        CallBackDataRecv m_callBackDataRecv;
    };
}

