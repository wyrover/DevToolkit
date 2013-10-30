#include "stdafx.h"
#include "DTKWebBrowser.h"
#include <mshtmhst.h>
#include <atlstr.h>
#include <winnt.h>
#include <atlapp.h>
#include <atlwin.h>
#include <atlframe.h>


CWebBrowser::CWebBrowser( void )
{
    m_ref = 0;
    m_hParent = NULL;
}


CWebBrowser::~CWebBrowser( void )
{

}

HRESULT STDMETHODCALLTYPE CWebBrowser::ShowContextMenu( DWORD dwID, DWORD x, DWORD y, IUnknown *pcmdtReserved, IDispatch *pdispReserved, HRESULT *dwRetVal )
{
    // 不显示菜单
    return S_OK;
}

HRESULT STDMETHODCALLTYPE CWebBrowser::GetHostInfo( DWORD *pdwFlags, DWORD *pdwDoubleClick )
{
    // 禁用上下文菜单,3D边框
    DWORD dwFlag = *pdwFlags;
    dwFlag |= DOCHOSTUIFLAG_DISABLE_HELP_MENU | DOCHOSTUIFLAG_NO3DBORDER | DOCHOSTUIFLAG_NO3DOUTERBORDER | DOCHOSTUIFLAG_FLAT_SCROLLBAR | DOCHOSTUIFLAG_THEME;
    *pdwFlags = dwFlag;
    return S_OK;
}

HRESULT STDMETHODCALLTYPE CWebBrowser::ShowUI( DWORD dwID, IUnknown *pActiveObject, IUnknown *pCommandTarget, IUnknown *pFrame, IUnknown *pDoc, HRESULT *dwRetVal )
{
    return E_NOTIMPL;
}

HRESULT STDMETHODCALLTYPE CWebBrowser::HideUI( void )
{
    return E_NOTIMPL;
}

HRESULT STDMETHODCALLTYPE CWebBrowser::UpdateUI( void )
{
    return E_NOTIMPL;
}

HRESULT STDMETHODCALLTYPE CWebBrowser::EnableModeless( VARIANT_BOOL fEnable )
{
    fEnable = TRUE;
    return E_NOTIMPL;
}

HRESULT STDMETHODCALLTYPE CWebBrowser::OnDocWindowActivate( VARIANT_BOOL fActivate )
{
    return E_NOTIMPL;
}

HRESULT STDMETHODCALLTYPE CWebBrowser::OnFrameWindowActivate( VARIANT_BOOL fActivate )
{
    return E_NOTIMPL;
}

HRESULT STDMETHODCALLTYPE CWebBrowser::ResizeBorder( long left, long top, long right, long bottom, IUnknown *pUIWindow, VARIANT_BOOL fFrameWindow )
{
    return E_NOTIMPL;
}

HRESULT STDMETHODCALLTYPE CWebBrowser::TranslateAccelerator( DWORD_PTR hWnd, DWORD nMessage, DWORD_PTR wParam, DWORD_PTR lParam, BSTR bstrGuidCmdGroup, DWORD nCmdID, HRESULT *dwRetVal )
{
    return E_NOTIMPL;
}

HRESULT STDMETHODCALLTYPE CWebBrowser::GetOptionKeyPath( BSTR *pbstrKey, DWORD dw )
{
    return E_NOTIMPL;
}

HRESULT STDMETHODCALLTYPE CWebBrowser::GetDropTarget( IUnknown *pDropTarget, IUnknown **ppDropTarget )
{
    return QueryInterface( IID_IUnknown, ( void ** )ppDropTarget );
}

HRESULT STDMETHODCALLTYPE CWebBrowser::GetExternal( IDispatch **ppDispatch )
{
    return QueryInterface( IID_IDispatch, ( void** )ppDispatch );
}

HRESULT STDMETHODCALLTYPE CWebBrowser::TranslateUrl( DWORD dwTranslate, BSTR bstrURLIn, BSTR *pbstrURLOut )
{
    return E_NOTIMPL;
}

HRESULT STDMETHODCALLTYPE CWebBrowser::FilterDataObject( IUnknown *pDO, IUnknown **ppDORet )
{
    return E_NOTIMPL;
}

HRESULT STDMETHODCALLTYPE CWebBrowser::GetIDsOfNames( REFIID riid, LPOLESTR __RPC_FAR *rgszNames, UINT cNames, LCID lcid, DISPID __RPC_FAR *rgDispId )
{
    return S_OK;
}

HRESULT STDMETHODCALLTYPE CWebBrowser::GetTypeInfoCount( UINT __RPC_FAR *pctinfo )
{
    return E_NOTIMPL;
}

HRESULT STDMETHODCALLTYPE CWebBrowser::GetTypeInfo( UINT iTInfo, LCID lcid, ITypeInfo __RPC_FAR *__RPC_FAR *ppTInfo )
{
    return E_NOTIMPL;
}

HRESULT STDMETHODCALLTYPE CWebBrowser::Invoke( DISPID dispIdMember, REFIID riid, LCID lcid, WORD wFlags, DISPPARAMS __RPC_FAR *pDispParams, VARIANT __RPC_FAR *pVarResult, EXCEPINFO __RPC_FAR *pExcepInfo, UINT __RPC_FAR *puArgErr )
{
    return IDispEventSimpleImpl<0, CWebBrowser, &DIID_DWebBrowserEvents2>::Invoke(
               dispIdMember, riid, lcid, wFlags, pDispParams, pVarResult, pExcepInfo, puArgErr );
}

void __stdcall CWebBrowser::OnNewWindow( IDispatch ** pObj, VARIANT_BOOL* Cannel, DWORD dwFlags, BSTR urlContext, BSTR url )
{
    // 	if (NULL!=Cannel)
    // 	{
    // 		Stop();
    // 		*Cannel=TRUE;
    // 	}
    // 	Navigate(url);
}

void __stdcall CWebBrowser::OnDocumentComplete( IDispatch *pDisp, VARIANT * pvarURL )
{

}

void __stdcall CWebBrowser::CommandStateChanged( long lCommand, BOOL bReady )
{

}

BOOL CWebBrowser::Create( HWND hParent, RECT rcWnd )
{
    m_hParent = hParent;
    m_rcClient = rcWnd;
    
    DWORD dwStyle = WS_CHILD | WS_VISIBLE | WS_CLIPCHILDREN ;
    DWORD dwExStyle = WS_EX_NOPARENTNOTIFY ;
    
    HWND hWnd = CAxWindow::Create( hParent, &rcWnd, NULL, dwStyle, dwExStyle );
    
    if ( hWnd == NULL )
        return FALSE;
        
    m_hHost = hWnd;
    
    return TRUE;
}

ULONG STDMETHODCALLTYPE CWebBrowser::AddRef()
{
    return InterlockedIncrement( &m_ref );
}

HRESULT STDMETHODCALLTYPE CWebBrowser::QueryInterface( REFIID riid, void **ppv )
{
    if ( ppv == NULL )
        return E_INVALIDARG;
        
    HRESULT hr = E_NOINTERFACE;
    void *pInterface = NULL;
    
    if ( riid == IID_IUnknown || riid == IID_IDispatch )
    {
        pInterface = static_cast<IDispatch *>( this );
    }
    else if ( riid == IID_IDocHostUIHandlerDispatch )
    {
        pInterface = static_cast<IDocHostUIHandlerDispatch *>( this );
    }
    
    if ( pInterface )
    {
        *ppv = pInterface;
        AddRef();
        hr = S_OK;
    }
    
    return hr;
}

ULONG STDMETHODCALLTYPE CWebBrowser::Release()
{
    return InterlockedDecrement( &m_ref );
}

BOOL CWebBrowser::Initialize()
{
    CComPtr<IUnknown> punkCtrl;
    
    HRESULT hr = CoCreateInstance( CLSID_WebBrowser, NULL, CLSCTX_INPROC_SERVER, IID_IUnknown, ( void ** )&punkCtrl );
    
    if ( SUCCEEDED( hr ) )
    {
        m_spWebBrowser2 = punkCtrl;
        
        if ( m_spWebBrowser2 )
        {
            CComPtr<IAxWinHostWindow> spWinHost;
            HRESULT hr = QueryHost( &spWinHost );
            
            if ( spWinHost )
            {
                spWinHost->AttachControl( m_spWebBrowser2, m_hHost );
            }
            
            CComQIPtr<IOleObject> spOle = m_spWebBrowser2;
            
            if ( spOle )
            {
                CComQIPtr<IOleClientSite> spClientSite = spWinHost;
                spOle->SetClientSite( spClientSite );
                RECT rc;
                ::GetClientRect( m_hHost, &rc );
                spOle->DoVerb( OLEIVERB_UIACTIVATE, NULL, spClientSite, 0, m_hWnd, &rc );
            }
            
            SetExternalUIHandler( this );
            DispEventAdvise( m_spWebBrowser2 );
            
            m_spWebBrowser2->put_Silent( VARIANT_TRUE );
            m_spWebBrowser2->put_RegisterAsDropTarget( VARIANT_FALSE );
            
            return TRUE;
        }
    }
    
    return FALSE;
}

void CWebBrowser::UnInitialize()
{
    if ( m_spWebBrowser2 )
    {
        m_spWebBrowser2->Stop();
        
        SetExternalUIHandler( NULL );
        DispEventUnadvise( m_spWebBrowser2 );
        
        m_spWebBrowser2.Release();
        
        AttachControl( NULL, NULL );
    }
}

BOOL CWebBrowser::Navigate( LPCTSTR lpszUrl )
{
    CComBSTR url = lpszUrl;
    VARIANT vt;
    VariantInit( &vt );
    HRESULT hr = m_spWebBrowser2->Navigate( url, &vt, &vt, &vt, &vt );
    return ( hr == S_OK );
}

void CWebBrowser::Refresh()
{
    m_spWebBrowser2->Refresh();
}

void CWebBrowser::Stop()
{
    m_spWebBrowser2->Stop();
}

void CWebBrowser::GoBack()
{
    m_spWebBrowser2->GoBack();
}

void CWebBrowser::GoForward()
{
    m_spWebBrowser2->GoForward();
}

void CWebBrowser::AdjustLayout()
{
    CRect rcParent;
    ::GetClientRect( GetParent(), &rcParent );
    ::MoveWindow( m_hHost, rcParent.left, rcParent.top, rcParent.Width(), rcParent.Height(), TRUE );
}

HWND CWebBrowser::GetParent()
{
    return m_hParent;
}

BOOL CWebBrowser::Init()
{
    return Initialize();
}

void CWebBrowser::UnInit()
{
    UnInitialize();
}

BOOL CAsyncWebBrowser::PreTranslateMessage( MSG* pMsg )
{
    return FALSE;
}

CAsyncWebBrowser::CAsyncWebBrowser()
{
    m_hThread = NULL;
    m_dwThreadID = 0;
    m_hEventThreadRun = NULL;
}

CAsyncWebBrowser::~CAsyncWebBrowser()
{

}

BOOL CAsyncWebBrowser::Create( HWND hParent, const CRect& rcClient )
{
    //     CMessageLoop* pLoop = _Module.GetMessageLoop();
    //     ATLASSERT( pLoop != NULL );
    //     pLoop->AddMessageFilter( this );
    
    __super::Create( hParent, rcClient );
    
    if ( NULL == m_hEventThreadRun )
    {
        m_hEventThreadRun =::CreateEvent( NULL, FALSE, FALSE, NULL );
    }
    if ( NULL == m_hThread )
    {
        m_hThread = ( HANDLE )::_beginthreadex( NULL, 0, &CAsyncWebBrowser::_WebBrowserThread, this, 0, &m_dwThreadID );
    }
    WaitForSingleObject( m_hEventThreadRun, INFINITE );
    
    return ( NULL != m_hThread );
}

UINT CAsyncWebBrowser::_WebBrowserThread( LPVOID pParam )
{
    CAsyncWebBrowser* pThis = ( CAsyncWebBrowser* )pParam;
    if ( NULL == pThis )
    {
        return -1;
    }
    
    return pThis->WebBrowserThread();
}

UINT CAsyncWebBrowser::WebBrowserThread()
{
    OleInitialize( NULL );
    
    ::DefWindowProc( NULL, 0, 0, 0L );
    
    MSG msg;
    BOOL bRet = FALSE;
    
    PeekMessage( &msg, NULL, WM_USER, WM_USER, PM_NOREMOVE );
    
    ::SetEvent( m_hEventThreadRun );
    
    for ( ;; )
    {
        bRet = ::GetMessage( &msg, NULL, 0, 0 );
        
        if ( bRet == -1 )
        {
            ATLTRACE2( atlTraceUI, 0, _T( "::GetMessage returned -1 (error)\n" ) );
            continue;   // error, don't process
        }
        else if ( !bRet )
        {
            ATLTRACE2( atlTraceUI, 0, _T( "CMessageLoop::Run - exiting\n" ) );
            break;   // WM_QUIT, exit message loop
        }
        
        if ( msg.message == WM_WEBBROWSER_METHORD )
        {
            __try
            {
                OnWebbrowserMethord( msg.wParam, msg.lParam );
            }
            __except ( EXCEPTION_EXECUTE_HANDLER )
            {
            
            }
        }
        
        if ( !PreTranslateMessage( &msg ) )
        {
            ::TranslateMessage( &msg );
            ::DispatchMessage( &msg );
        }
    }
    
    OleUninitialize();
    if ( NULL != m_hThread )
    {
        CloseHandle( m_hThread );
        m_hThread = NULL;
    }
    return ( int )msg.wParam;
}

BOOL CAsyncWebBrowser::Navigate( LPCTSTR lpszUrl )
{
    BOOL bRet = FALSE;
    if ( NULL != m_hThread )
    {
        bRet = PostThreadMessage( m_dwThreadID, WM_WEBBROWSER_METHORD, WEBBROWSER_NAVIGATE, ( LPARAM )lpszUrl );
    }
    
    return bRet;
}

void CAsyncWebBrowser::Refresh()
{
    if ( NULL != m_hThread )
    {
        PostThreadMessage( m_dwThreadID, WM_WEBBROWSER_METHORD, WEBBROWSER_REFRESH, 0 );
    }
}

void CAsyncWebBrowser::Stop()
{
    if ( NULL != m_hThread )
    {
        PostThreadMessage( m_dwThreadID, WM_WEBBROWSER_METHORD, WEBBROWSER_STOP, 0 );
    }
}

void CAsyncWebBrowser::GoBack()
{
    if ( NULL != m_hThread )
    {
        PostThreadMessage( m_dwThreadID, WM_WEBBROWSER_METHORD, WEBBROWSER_BACK, 0 );
    }
}

void CAsyncWebBrowser::GoForward()
{
    if ( NULL != m_hThread )
    {
        PostThreadMessage( m_dwThreadID, WM_WEBBROWSER_METHORD, WEBBROWSER_FORWARD, 0 );
    }
}

void CAsyncWebBrowser::AdjustLayout()
{
    __super::AdjustLayout();
}

LRESULT CAsyncWebBrowser::OnWebbrowserMethord( WPARAM wParam, LPARAM lParam )
{
    WEBBROWSER_PARAM webParam = ( WEBBROWSER_PARAM )wParam;
    switch ( webParam )
    {
        case WEBBROWSER_INIT:
        {
            __super::Initialize();
            break;
        }
        case WEBBROWSER_UNINIT:
        {
            __super::UnInitialize();
            break;
        }
        case WEBBROWSER_NAVIGATE:
        {
            LPCTSTR lpszUrl = ( LPCTSTR )lParam;
            __super::Navigate( lpszUrl );
            break;
        }
        case WEBBROWSER_REFRESH:
        {
            __super::Refresh();
            break;
        }
        case WEBBROWSER_STOP:
        {
            __super::Stop();
            break;
        }
        case WEBBROWSER_FORWARD:
        {
            __super::GoForward();
            break;
        }
        case WEBBROWSER_BACK:
        {
            __super::GoBack();
            break;
        }
        default:
            break;
    }
    return S_OK;
}

BOOL CAsyncWebBrowser::Init()
{
    BOOL bRet = FALSE;
    if ( NULL != m_hThread )
    {
        bRet = PostThreadMessage( m_dwThreadID, WM_WEBBROWSER_METHORD, WEBBROWSER_INIT, 0 );
    }
    return bRet;
}

void CAsyncWebBrowser::UnInit()
{
    if ( NULL != m_hThread )
    {
        PostThreadMessage( m_dwThreadID, WM_WEBBROWSER_METHORD, WEBBROWSER_UNINIT, 0 );
        PostThreadMessage( m_dwThreadID, WM_QUIT, 0, 0 );
        //		WaitForSingleObject(m_hThread,INFINITE);
    }
}

IWEBBROWSER* IWEBBROWSER::Create( HWND hParent, const CRect& rcWnd, WEBBROWSER_TYPE webBrowserType )
{
    IWEBBROWSER* pWebBrowser = NULL;
    
    switch ( webBrowserType )
    {
        case WEBBROWSER_ASYNC:
        {
            CAsyncWebBrowser* pAsyncWebBrowser = new CAsyncWebBrowser();
            if ( NULL != pAsyncWebBrowser )
            {
                pAsyncWebBrowser->Create( hParent, rcWnd );
            }
            pWebBrowser = pAsyncWebBrowser;
            
            break;
        }
        case WEBBROWSER_NORMAL:
        {
            CWebBrowser* pWebBrowser1 = new CWebBrowser();
            if ( NULL != pWebBrowser1 )
            {
                pWebBrowser1->Create( hParent, rcWnd );
            }
            pWebBrowser = pWebBrowser1;
            
            break;
        }
        default:
            break;
    }
    
    return pWebBrowser;
}
