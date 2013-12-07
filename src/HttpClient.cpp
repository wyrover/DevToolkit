#include "stdafx.h"
#include <stdio.h>
#include "HttpClient.h"
#include <mmsystem.h>
#pragma comment(lib,"WinHttp.lib")
#pragma comment(lib,"Winmm.lib")
namespace DevToolkit
{

    CHttpClient::CHttpClient( LPCTSTR lpszURL, CallBackDataRecv callBack )
    {
        m_callBackDataRecv = callBack;
        m_lpszURL = NULL;
        m_lpszHttpHeader = NULL;
        m_lpszHttpResponse = NULL;
        m_lpszHostName = new TCHAR[MAX_PATH];
        m_wPort = 80;
        m_dwResponseLength = 0;
        
        if ( NULL != lpszURL )
        {
            int iUrlLen = _tcslen( lpszURL ) * sizeof( TCHAR );
            
            if ( 0 != iUrlLen )
            {
                m_lpszURL = new TCHAR[iUrlLen];
                memset( m_lpszURL, 0, iUrlLen );
                _tcscpy_s( m_lpszURL, iUrlLen, lpszURL );
            }
        }
    }
    
    
    CHttpClient::~CHttpClient( void )
    {
        if ( NULL != m_lpszHttpResponse )
        {
            delete[] m_lpszHttpResponse;
            m_lpszHttpResponse = NULL;
        }
        
        if ( NULL != m_lpszHttpHeader )
        {
            delete[] m_lpszHttpHeader;
            m_lpszHttpHeader = NULL;
        }
        
        if ( NULL != m_lpszURL )
        {
            delete[] m_lpszURL;
            m_lpszURL = NULL;
        }
        
        if ( NULL != m_lpszHostName )
        {
            delete[] m_lpszHostName;
            m_lpszHostName = NULL;
        }
        
        if ( NULL != m_lpszUrlPath )
        {
            delete[] m_lpszUrlPath;
            m_lpszUrlPath = NULL;
        }
    }
    
    BOOL CHttpClient::SendHttpRequest()
    {
        BOOL bRet = TRUE;
        // 创建会话Session
        TCHAR sAgent[64] = {0};
        _stprintf_s( sAgent, _T( "Agent%ld" ), timeGetTime() );
        
        HINTERNET hSession = WinHttpOpen( sAgent, WINHTTP_ACCESS_TYPE_DEFAULT_PROXY, WINHTTP_NO_PROXY_NAME, WINHTTP_NO_PROXY_BYPASS, 0 );
        
        if ( NULL == hSession )
        {
            return FALSE;
        }
        
        // 解析URL
        URL_COMPONENTS url_Components = {0};
        url_Components.dwStructSize = sizeof( URL_COMPONENTS );
        url_Components.dwHostNameLength = MAX_PATH;
        url_Components.dwUrlPathLength = MAX_PATH * 5;
        m_lpszHostName = new TCHAR[url_Components.dwHostNameLength];
        m_lpszUrlPath = new TCHAR[url_Components.dwUrlPathLength];
        url_Components.lpszHostName = m_lpszHostName;
        url_Components.lpszUrlPath = m_lpszUrlPath;
        bRet = WinHttpCrackUrl( m_lpszURL, _tcslen( m_lpszURL ) * sizeof( TCHAR ), ICU_DECODE, &url_Components );
        
        if ( !bRet )
        {
            bRet = FALSE;
            goto HTTPERROR;
        }
        
        m_wPort = url_Components.nPort;
        
        // 建立连接
        HINTERNET hConnect = WinHttpConnect( hSession, url_Components.lpszHostName, INTERNET_DEFAULT_HTTP_PORT, 0 );
        
        if ( NULL == hConnect )
        {
            bRet = FALSE;
            goto HTTPERROR;
        }
        
        // 打开Request
        HINTERNET hRequest = WinHttpOpenRequest( hConnect, _T( "GET" ), url_Components.lpszUrlPath, NULL, WINHTTP_NO_REFERER, WINHTTP_DEFAULT_ACCEPT_TYPES, 0 );
        
        if ( NULL == hRequest )
        {
            bRet = FALSE;
            goto HTTPERROR;
        }
        
        // 发送Request
        bRet = WinHttpSendRequest( hRequest, WINHTTP_NO_ADDITIONAL_HEADERS, 0, WINHTTP_NO_REQUEST_DATA, 0, 0, NULL );
        
        if ( !bRet )
        {
            bRet = FALSE;
            goto HTTPERROR;
        }
        
        // 接收回应
        bRet = WinHttpReceiveResponse( hRequest, NULL );
        
        if ( !bRet )
        {
            bRet = FALSE;
            goto HTTPERROR;
        }
        
        // 查询HttpHeader
        DWORD dwHeaderSize = 0;
        bRet = WinHttpQueryHeaders( hRequest, WINHTTP_QUERY_RAW_HEADERS_CRLF, WINHTTP_HEADER_NAME_BY_INDEX, m_lpszHttpHeader, &dwHeaderSize, WINHTTP_NO_HEADER_INDEX );
        
        if ( !bRet )
        {
            if ( ERROR_INSUFFICIENT_BUFFER == GetLastError() )
            {
                m_lpszHttpHeader = new TCHAR[dwHeaderSize];
                bRet = WinHttpQueryHeaders( hRequest, WINHTTP_QUERY_RAW_HEADERS_CRLF, WINHTTP_HEADER_NAME_BY_INDEX, m_lpszHttpHeader, &dwHeaderSize, WINHTTP_NO_HEADER_INDEX );
                
                if ( !bRet )
                {
                    bRet = FALSE;
                    goto HTTPERROR;
                }
            }
        }
        
        // 查询文件长度
        DWORD dwSizeContent = 4;
        DWORD dwIndex = 0;
        WinHttpQueryHeaders( hRequest, WINHTTP_QUERY_CONTENT_LENGTH | WINHTTP_QUERY_FLAG_NUMBER, NULL, &m_dwResponseLength, &dwSizeContent, &dwIndex );
        
        // 接收数据
        m_lpszHttpResponse = new BYTE[m_dwResponseLength];
        DWORD dwReaded = 0;
        DWORD dwLeft = m_dwResponseLength;
        DWORD dwPos = 0;
        
        while ( dwLeft > 0 )
        {
            if ( WinHttpReadData( hRequest, &m_lpszHttpResponse[dwPos], 4096, &dwReaded ) )
            {
                if ( NULL != m_callBackDataRecv )
                {
                    m_callBackDataRecv( &m_lpszHttpResponse[dwPos], dwReaded );
                }
                
                dwLeft -= dwReaded;
                dwPos += dwReaded;
            }
            else
                break;
        }
        
        // 关闭会话
    HTTPERROR:
    
        if ( NULL != hRequest )
        {
            ::WinHttpCloseHandle( hRequest );
            hRequest = NULL;
        }
        
        if ( NULL != hConnect )
        {
            ::WinHttpCloseHandle( hConnect );
            hConnect = NULL;
        }
        
        if ( NULL != hSession )
        {
            ::WinHttpCloseHandle( hSession );
            hSession = NULL;
        }
        
        return TRUE;
    }
    
    LPCTSTR CHttpClient::GetHttpHeader()
    {
        return m_lpszHttpHeader;
    }
    
    LPCTSTR CHttpClient::GetHttpResponse()
    {
        return ( LPCTSTR )m_lpszHttpResponse;
    }
    
    BOOL CHttpClient::SaveResponseToFile( LPCTSTR lpszFileName )
    {
        FILE* pFile = NULL;
        _tfopen_s( &pFile, lpszFileName, _T( "wb" ) );
        
        if ( NULL != pFile )
        {
            fwrite( m_lpszHttpResponse, m_dwResponseLength, 1, pFile );
            fclose( pFile );
            pFile = NULL;
            return TRUE;
        }
        
        return FALSE;
    }
}
