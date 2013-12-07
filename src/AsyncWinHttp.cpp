#include "AsyncWinHttp1.h"
#include <assert.h>
#include <tchar.h>
#include <locale.h>
#include <io.h>
using namespace CODEHELPER;

CWinHttpHandle::CWinHttpHandle(): m_handle( NULL )
{

}

CWinHttpHandle::~CWinHttpHandle()
{
    Close();
}

bool CWinHttpHandle::Attach( HINTERNET handle )
{
    assert( NULL == m_handle );
    m_handle = handle;
    return NULL != m_handle;
}

HINTERNET CWinHttpHandle::Detach()
{
    HANDLE handle = m_handle;
    m_handle = NULL;
    return handle;
}

void CWinHttpHandle::Close()
{
    if ( NULL != m_handle )
    {
        ::WinHttpCloseHandle( m_handle );
        m_handle = NULL;
    }
}

HRESULT CWinHttpHandle::SetOption( DWORD option, const void* value, DWORD length )
{
    if ( !::WinHttpSetOption( m_handle, option, const_cast<void*>( value ), length ) )
    {
        return HRESULT_FROM_WIN32( ::GetLastError() );
    }

    return S_OK;
}

HRESULT CWinHttpHandle::QueryOption( DWORD option, void* value, DWORD& length ) const
{
    if ( !::WinHttpQueryOption( m_handle, option, value, &length ) )
    {
        return HRESULT_FROM_WIN32( ::GetLastError() );
    }

    return S_OK;
}

HINTERNET CWinHttpHandle::GetHandle() const
{
    return m_handle;
}

CWinHttpSession::CWinHttpSession()
{

}

CWinHttpSession::~CWinHttpSession()
{
    Close();
}

HRESULT CWinHttpSession::Open()
{
    if ( !Attach( ::WinHttpOpen( _T( "A WinHTTP Example Program" ), WINHTTP_ACCESS_TYPE_DEFAULT_PROXY,
                                 WINHTTP_NO_PROXY_NAME, WINHTTP_NO_PROXY_BYPASS, WINHTTP_FLAG_ASYNC ) ) )
    {
        return HRESULT_FROM_WIN32( ::GetLastError() );
    }

    return S_OK;
}

CWinHttpConnection::CWinHttpConnection( CWinHttpSession* pSession, CWinHttpUrl* pUrl )
{
    m_pUrl = pUrl;
    m_pSession = pSession;
}

CWinHttpConnection::~CWinHttpConnection()
{
    Close();
}

HRESULT CWinHttpConnection::Open()
{
    if ( NULL == m_pSession )
    {
        return S_FALSE;
    }

    if ( !Attach( ::WinHttpConnect( m_pSession->GetHandle(), m_pUrl->GetHostName(), m_pUrl->GetHostPort(), 0 ) ) )
    {
        return HRESULT_FROM_WIN32( ::GetLastError() );
    }

    return S_OK;
}

CWinHttpSession* CWinHttpConnection::GetSession() const
{
    return m_pSession;
}

CWinHttpRequest::CWinHttpRequest( CWinHttpConnection* pConnect, CWinHttpUrl* pUrl )
{
    m_pUrl = pUrl;
    m_pConnect = pConnect;
    m_dwTotalSize = 0;
    m_dwReadedSize = 0;
	m_hRunEvent=CreateEvent(NULL,TRUE,TRUE,NULL);
}

CWinHttpRequest::~CWinHttpRequest()
{
	if (NULL!=m_hRunEvent)
	{
		CloseHandle(m_hRunEvent);
		m_hRunEvent=NULL;
	}
    WinHttpSetStatusCallback( GetHandle(), NULL, NULL, NULL );

    Close();
}

HRESULT CWinHttpRequest::Open( LPCTSTR lpszVerb )
{
    if ( NULL == m_pUrl )
    {
        return S_FALSE;
    }

    DWORD dwOpenRequestFlag = ( INTERNET_SCHEME_HTTPS == m_pUrl->GetScheme() ) ? WINHTTP_FLAG_SECURE : 0;
    HINTERNET request = ::WinHttpOpenRequest( m_pConnect->GetHandle(), lpszVerb, m_pUrl->GetUrlPath(), NULL
                        , WINHTTP_NO_REFERER, WINHTTP_DEFAULT_ACCEPT_TYPES, dwOpenRequestFlag );

    if ( !Attach( request ) )
    {
        return HRESULT_FROM_WIN32( ::GetLastError() );
    }

    if ( WINHTTP_INVALID_STATUS_CALLBACK == WinHttpSetStatusCallback( GetHandle(),
            &CWinHttpRequest::Callback, WINHTTP_CALLBACK_FLAG_ALL_NOTIFICATIONS | WINHTTP_CALLBACK_FLAG_REDIRECT , NULL ) )
    {
        return HRESULT_FROM_WIN32( ::GetLastError() );
    }

    return S_OK;
}

HRESULT CWinHttpRequest::Send( LPCTSTR pwszHeaders/*=WINHTTP_NO_ADDITIONAL_HEADERS*/, DWORD dwHeadersLength/*=0*/, LPVOID lpOptional/*=WINHTTP_NO_REQUEST_DATA*/, DWORD dwOptionalLength/*=0*/, DWORD dwTotalLength/*=0*/ )
{
    if ( !WinHttpSendRequest( GetHandle(), pwszHeaders, dwHeadersLength, lpOptional,
                              dwOptionalLength, dwTotalLength, ( DWORD_PTR )this ) )
    {
        return HRESULT_FROM_WIN32( ::GetLastError() );
    }

    return S_OK;
}

void CALLBACK CWinHttpRequest::Callback( HINTERNET hInternet, DWORD_PTR dwContext, DWORD dwInternetStatus, LPVOID lpvStatusInformation, DWORD dwStatusInformationLength )
{
    CWinHttpRequest* pRequest = ( CWinHttpRequest* )dwContext;
    HRESULT hr = S_OK;

    if ( NULL == pRequest )
    {
        return ;
    }

	WaitForSingleObject(pRequest->m_hRunEvent,INFINITE);

    switch ( dwInternetStatus )
    {
    case WINHTTP_CALLBACK_STATUS_REQUEST_ERROR:
    {
        break;
    }

    case WINHTTP_CALLBACK_STATUS_SENDREQUEST_COMPLETE:
    {
        hr = pRequest->OnSendRequestComplete( lpvStatusInformation, dwStatusInformationLength );
        break;
    }

    case WINHTTP_CALLBACK_STATUS_HEADERS_AVAILABLE:
    {
        hr = pRequest->OnHeaderAvailable( lpvStatusInformation, dwStatusInformationLength );
        break;
    }

    case WINHTTP_CALLBACK_STATUS_DATA_AVAILABLE:
    {
        hr = pRequest->OnDataAvailable( lpvStatusInformation, dwStatusInformationLength );
        break;
    }

    case WINHTTP_CALLBACK_STATUS_READ_COMPLETE:
    {
        hr = pRequest->OnReadComplete( lpvStatusInformation, dwStatusInformationLength );
        break;
    }

    default:
        break;
    }

    if ( S_FALSE == hr )
    {
        if ( NULL != pRequest->m_funcUserCallback )
        {
            pRequest->m_funcUserCallback( pRequest->m_pParam, DOWN_STATUS_ERROR, 0, 0, 0, 0 );
        }
    }
}

HRESULT CWinHttpRequest::OnSendRequestComplete( LPVOID lpvStatusInformation, DWORD dwStatusInformationLength )
{
    if ( WinHttpReceiveResponse( GetHandle(), NULL ) == FALSE )
    {
        if ( !ERROR_IO_PENDING == GetLastError() )
        {
            return HRESULT_FROM_WIN32( ::GetLastError() );
        }
    }

    return S_OK;
}

HRESULT CWinHttpRequest::OnHeaderAvailable( LPVOID lpvStatusInformation, DWORD dwStatusInformationLength )
{
    DWORD statusCodeSize = sizeof( DWORD );
    DWORD dwStatusCode = 0;

    if ( !::WinHttpQueryHeaders( GetHandle(), WINHTTP_QUERY_STATUS_CODE | WINHTTP_QUERY_FLAG_NUMBER, WINHTTP_HEADER_NAME_BY_INDEX,
                                 &dwStatusCode, &statusCodeSize, WINHTTP_NO_HEADER_INDEX ) )
    {
        return HRESULT_FROM_WIN32( ::GetLastError() );
    }

    if ( dwStatusCode == 416 )
    {
        if ( NULL != m_funcUserCallback )
        {
            m_funcUserCallback( m_pParam, DWON_STATUS_SUCCESS, 0, 0, m_dwReadedSize, m_dwReadedSize );
        }

        return S_OK;
    }

    DWORD dwSize = 0;

    // 查询头长度
    if ( !WinHttpQueryHeaders( GetHandle(),
                               WINHTTP_QUERY_RAW_HEADERS_CRLF,
                               WINHTTP_HEADER_NAME_BY_INDEX, NULL, &dwSize, WINHTTP_NO_HEADER_INDEX ) )
    {
        DWORD dwErr = GetLastError();

        if ( dwErr != ERROR_INSUFFICIENT_BUFFER )
        {
            return HRESULT_FROM_WIN32( ::GetLastError() );
        }
    }

    // 获取Http头内容
    TCHAR* pHeader = new TCHAR[dwSize];

    if ( !WinHttpQueryHeaders( GetHandle(),
                               WINHTTP_QUERY_RAW_HEADERS_CRLF,
                               WINHTTP_HEADER_NAME_BY_INDEX, pHeader, &dwSize, WINHTTP_NO_HEADER_INDEX ) )
    {
        return HRESULT_FROM_WIN32( ::GetLastError() );
    }

    delete[] pHeader;
    pHeader = NULL;

    // 查询文件长度
    m_dwTotalSize = 0;
    DWORD dwWantDownloadSize = 0;
    DWORD dwSizeContent = 4;
    DWORD dwIndex = 0;
    BOOL bRet = FALSE;
    bRet = WinHttpQueryHeaders( GetHandle(), WINHTTP_QUERY_CONTENT_LENGTH | WINHTTP_QUERY_FLAG_NUMBER, NULL, &dwWantDownloadSize, &dwSizeContent, &dwIndex );

    if ( !bRet )
    {
        return HRESULT_FROM_WIN32( ::GetLastError() );
    }

    m_dwTotalSize += dwWantDownloadSize + m_dwReadedSize;

    // 获取有效数据
    if ( WinHttpQueryDataAvailable( GetHandle(), NULL ) == FALSE )
    {
        return HRESULT_FROM_WIN32( ::GetLastError() );
    }

    return S_OK;
}

HRESULT CWinHttpRequest::OnDataAvailable( LPVOID lpvStatusInformation, DWORD dwStatusInformationLength )
{
    DWORD dwSize = *( ( LPDWORD )lpvStatusInformation );;

    if ( 0 == dwSize )
    {
        return OnReadComplete( NULL, 0 );
    }

    LPSTR lpOutBuffer = new char[dwSize + 1];
    ZeroMemory( lpOutBuffer, dwSize + 1 );

    if ( WinHttpReadData( GetHandle(), ( LPVOID )lpOutBuffer,
                          dwSize, 0 ) == FALSE )
    {
        DWORD dwErr = GetLastError();

        if ( dwErr != ERROR_IO_PENDING )
        {
            delete [] lpOutBuffer;
            return HRESULT_FROM_WIN32( ::GetLastError() );
        }
    }

    return S_OK;
}

HRESULT CWinHttpRequest::OnReadComplete( LPVOID lpvStatusInformation, DWORD dwStatusInformationLength )
{
    DWORD dwSize = dwStatusInformationLength;

    if ( 0 == dwSize )
    {
        if ( NULL != m_funcUserCallback )
        {
            m_funcUserCallback( m_pParam, DWON_STATUS_SUCCESS, lpvStatusInformation, 0, m_dwReadedSize, m_dwTotalSize );
        }

        return S_OK;
    }
    else
    {
        m_dwReadedSize += dwSize;

        if ( NULL != m_funcUserCallback )
        {
            m_funcUserCallback( m_pParam, DOWN_STATUS_DOWNLOADING, lpvStatusInformation, dwSize, m_dwReadedSize, m_dwTotalSize );
        }

        if ( WinHttpQueryDataAvailable( GetHandle(), NULL ) == FALSE )
        {
            return HRESULT_FROM_WIN32( ::GetLastError() );
        }
    }

    if ( NULL != lpvStatusInformation )
    {
        delete[] lpvStatusInformation;
        lpvStatusInformation = NULL;
    }

    return S_OK;
}

HRESULT CWinHttpRequest::AddHeader( CWinHttpHeader* pHeader )
{
    if ( ( NULL == pHeader ) || ( _tcslen( pHeader->GetHeader() ) == 0 ) )
    {
        return S_OK;
    }

    if ( !WinHttpAddRequestHeaders( GetHandle(), pHeader->GetHeader(), -1, WINHTTP_ADDREQ_FLAG_ADD | WINHTTP_ADDREQ_FLAG_REPLACE ) )
    {
        return S_FALSE;
    }

    return S_OK;
}

HRESULT CWinHttpRequest::EnableRedirect( BOOL bEnable/*=TRUE*/ )
{
    if ( !bEnable )
    {
        DWORD dwValue  = WINHTTP_DISABLE_REDIRECTS;
        DWORD dwOpSize  = 4;

        if ( S_FALSE == SetOption( WINHTTP_OPTION_DISABLE_FEATURE, &dwValue, dwOpSize ) )
        {
            return S_FALSE;
        }
    }

    return S_OK;
}

void CWinHttpRequest::SetUserCallback( UserCallback pUserCallback , LPVOID pParam )
{
    m_funcUserCallback = pUserCallback;
    m_pParam = pParam;
}

DWORD CDownload::m_dwTaskID=-1;
CDownload::CDownload( LPCTSTR lpszUrl, LPCTSTR lpszSavePath )
{
    m_pSession = NULL;
    m_pConnect = NULL;
    m_pRequest = NULL;
    m_pHeader = NULL;
    m_bEnableRedirect = FALSE;
    m_pHttpUrl = new CWinHttpUrl( lpszUrl );
    memcpy_s( m_sSavePath, MAX_PATH, lpszSavePath, MAX_PATH );
    m_pFile = NULL;
	m_dwTaskID++;
}

CDownload::~CDownload()
{
    CloseAll();
}

BOOL CDownload::SendRequest()
{
    if ( !m_pHttpUrl->CrackUrl() )
    {
        return FALSE;
    }

    m_pSession = new CWinHttpSession();

    if ( S_FALSE == m_pSession->Open() )
        return FALSE;

    m_pConnect = new CWinHttpConnection( m_pSession, m_pHttpUrl );

    if ( S_FALSE == m_pConnect->Open() )
        return FALSE;

    m_pRequest = new CWinHttpRequest( m_pConnect, m_pHttpUrl );

    if ( S_FALSE == m_pRequest->Open( _T( "GET" ) ) )
        return FALSE;

    m_pRequest->SetUserCallback( &CDownload::DownloadCallback, this );

    m_pHeader = new CWinHttpHeader();
    m_pHeader->Add( _T( "Accept" ), _T( "*/*" ) );
    m_pHeader->Add( _T( "Pragma" ), _T( "no-cache" ) );
    m_pHeader->Add( _T( "Cache-Control" ), _T( "no-cache" ) );
    m_pHeader->Add( _T( "User-Agent" ), _T( "Mozilla/4.0 (compatible; MSIE 8.0; Windows NT 6.1; Trident/4.0; QQDownload 734; SLCC2; .NET CLR 2.0.50727; .NET CLR 3.5.30729; .NET CLR 3.0.30729; Media Center PC 6.0; .NET4.0C; .NET4.0E; Tablet PC 2.0)" ) );
    m_pHeader->Add( _T( "Connection" ), _T( "Keep-Alive" ) );

    // 断点续传
    TCHAR sFileLen[MAX_PATH] = {0};

    if ( PathFileExists( m_sSavePath ) )
    {
        _tfopen_s( &m_pFile, m_sSavePath, _T( "ab+" ) );
        fseek( m_pFile, 0, SEEK_END );
        DWORD dwFileSize = _filelength( _fileno( m_pFile ) );
        m_pRequest->SetReadedSize( dwFileSize );
        _stprintf_s( sFileLen, _T( "bytes=%u-" ), dwFileSize );
        m_pHeader->Add( _T( "Range" ), sFileLen );
    }
    else
    {
        _tfopen_s( &m_pFile, m_sSavePath, _T( "wb" ) );
        fseek( m_pFile, 0, SEEK_SET );
        m_pRequest->SetReadedSize( 0 );
        m_pHeader->Add( _T( "Range" ), _T( "bytes=0-" ) );
    }

    if ( NULL != m_pHeader )
    {
        if ( S_FALSE == m_pRequest->AddHeader( m_pHeader ) )
        {
            return S_FALSE;
        }
    }

    if ( m_bEnableRedirect )
    {
        if ( S_FALSE == m_pRequest->EnableRedirect( m_bEnableRedirect ) )
        {
            return S_FALSE;
        }
    }

    if ( S_FALSE == m_pRequest->Send() )
        return FALSE;

    return TRUE;
}

BOOL CDownload::PostRequest()
{
    return TRUE;
}

void CDownload::EnableRedirect( BOOL bEnable )
{
    m_bEnableRedirect = bEnable;
}

void CDownload::AddHeader( LPCTSTR lpszKey, LPCTSTR lpszValue )
{
    if ( NULL == m_pHeader )
    {
        m_pHeader = new CWinHttpHeader();
    }

    m_pHeader->Add( lpszKey, lpszValue );
}

void CDownload::CloseAll()
{
    if ( NULL != m_pHeader )
    {
        delete m_pHeader;
        m_pHeader = NULL;
    }

    if ( NULL != m_pRequest )
    {
        delete m_pRequest;
        m_pRequest = NULL;
    }

    if ( NULL != m_pConnect )
    {
        delete m_pConnect;
        m_pConnect = NULL;
    }

    if ( NULL != m_pSession )
    {
        delete m_pSession;
        m_pSession = NULL;
    }

    if ( NULL != m_pHttpUrl )
    {
        delete m_pHttpUrl;
        m_pHttpUrl = NULL;
    }

    if ( NULL != m_pFile )
    {
        fclose( m_pFile );
        m_pFile = NULL;
    }
}

void __stdcall CDownload::DownloadCallback( LPVOID pParam, DWORD dwStatus, LPCVOID lpData, DWORD dwDataSize, DWORD dwReaded, DWORD dwTotal )
{
    CDownload* pThis = ( CDownload* )pParam;

    if ( NULL == pThis )
    {
        return ;
    }

    DownCallback callBack = pThis->m_downCallback;

    if ( dwStatus == DWON_STATUS_SUCCESS )
    {
		// 有回调函数的话,让调用者释放内存
        if ( NULL != callBack )
        {
            callBack( pThis->m_callBackParam, DWON_STATUS_SUCCESS, dwReaded, dwTotal );
        }
		else	// 否则,自己释放内存
		{
			delete pThis;
			pThis=NULL;
		}
    }
    else if ( dwStatus == DOWN_STATUS_DOWNLOADING )
    {
        if ( NULL != callBack )
        {
            callBack( pThis->m_callBackParam, DOWN_STATUS_DOWNLOADING, dwReaded, dwTotal );
        }

        if ( NULL != pThis->m_pFile )
        {
            fwrite( lpData, 1, dwDataSize, pThis->m_pFile );
            fflush( pThis->m_pFile );
        }
    }
    else if ( dwStatus == DOWN_STATUS_ERROR )
    {
        if ( NULL != callBack )
        {
            callBack( pThis->m_callBackParam, DOWN_STATUS_ERROR, dwReaded, dwTotal );
        }
		else
		{
			delete pThis;
			pThis = NULL;
		}
    }
}

CWinHttpUrl::CWinHttpUrl( LPCTSTR lpszUrl )
{
    memset( m_sUrl, 0, MAX_PATH );
    memcpy_s( m_sUrl, MAX_PATH, lpszUrl, MAX_PATH );
}

CWinHttpUrl::~CWinHttpUrl()
{
    if ( NULL != m_urlComp.lpszHostName )
    {
        delete[] m_urlComp.lpszHostName;
        m_urlComp.lpszHostName = NULL;
    }
}

LPCTSTR CWinHttpUrl::GetHostName()
{
    return m_urlComp.lpszHostName;
}

WORD CWinHttpUrl::GetHostPort()
{
    return m_urlComp.nPort;
}

LPCTSTR CWinHttpUrl::GetUrlPath() const
{
    return m_urlComp.lpszUrlPath;
}

BOOL CWinHttpUrl::CrackUrl()
{
    const int hostLen = MAX_PATH;
    memset( &m_urlComp, 0, sizeof( m_urlComp ) );
    m_urlComp.dwStructSize = sizeof( m_urlComp );
    m_urlComp.lpszHostName        = new TCHAR[hostLen];
    m_urlComp.dwHostNameLength    = hostLen;
    m_urlComp.dwUrlPathLength = ( DWORD ) - 1;
    m_urlComp.dwSchemeLength = ( DWORD ) - 1;

    if ( !WinHttpCrackUrl( m_sUrl, 0, 0, &m_urlComp ) )
    {
        throw _T( "ERROR" );
    }

    return TRUE;
}

INTERNET_SCHEME CWinHttpUrl::GetScheme() const
{
    return m_urlComp.nScheme;
}

CWinHttpHeader::CWinHttpHeader()
{

}

CWinHttpHeader::~CWinHttpHeader()
{

}

void CWinHttpHeader::Add( LPCTSTR lpszKey, LPCTSTR lpszValue )
{
    m_sHeader += lpszKey;
    m_sHeader += _T( ": " );
    m_sHeader += lpszValue;
    m_sHeader += _T( "\r\n" );
}

LPCTSTR CWinHttpHeader::GetHeader() const
{
    return m_sHeader;
}

void CWinHttpRequest::SetReadedSize( DWORD dwSize )
{
    m_dwReadedSize = dwSize;
}

void CODEHELPER::CDownload::SetCallBack( DownCallback callBack, LPVOID lpcallBackParam )
{
    m_downCallback = callBack;
    m_callBackParam = lpcallBackParam;
}

void CODEHELPER::CDownload::Pause()
{
	m_pRequest->Pause();
}

void CODEHELPER::CDownload::Resume()
{
	m_pRequest->Resume();
}

DWORD CODEHELPER::CDownload::GetTaskID()
{
	return m_dwTaskID;
}

void CODEHELPER::CWinHttpRequest::Pause()
{
	ResetEvent(m_hRunEvent);
}

void CODEHELPER::CWinHttpRequest::Resume()
{
	SetEvent(m_hRunEvent);
}
