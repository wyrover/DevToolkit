#include "stdafx.h"
#include "FlashControl.h"

CFlashControl::CFlashControl()
{
    m_dwCookie = 0;
    m_ConnectionPoint = NULL;
    m_nRefCount = 0;
}

CFlashControl::~CFlashControl()
{

}

BOOL CFlashControl::Create( HWND hWnd, CRect rc )
{
    m_hWnd = CAxWindow::Create( hWnd, rc, _T( "ShockwaveFlash.ShockwaveFlash" ), WS_CHILD | WS_VISIBLE );
    if ( NULL == m_hWnd )
    {
        return FALSE;
    }
    HRESULT hr = QueryControl( __uuidof( IShockwaveFlash ), ( void** )&pFlash );
    if ( FAILED( hr ) )
    {
        return FALSE;
    }
    hr = Init( pFlash );
    if ( FAILED( hr ) )
    {
        return FALSE;
    }
    return TRUE;
}

void CFlashControl::Destroy()
{
    DestroyWindow();
}

BOOL CFlashControl::LoadFlash( BSTR lpszFlash )
{
    HRESULT hr = pFlash->put_Movie( lpszFlash );
    return SUCCEEDED( hr );
}

BOOL CFlashControl::Start()
{
    HRESULT hr = pFlash->Play();
    return SUCCEEDED( hr );
}

BOOL CFlashControl::Stop()
{
    HRESULT hr = pFlash->Stop();
    return SUCCEEDED( hr );
}

BOOL CFlashControl::IsPlaying()
{
    VARIANT_BOOL isPlayIng;
    HRESULT hr = pFlash->get_Playing( &isPlayIng );
    return SUCCEEDED( hr );
}

HRESULT CFlashControl::Back( )
{
    HRESULT hr = pFlash->Back();
    return SUCCEEDED( hr );
}

HRESULT CFlashControl::Forward( )
{
    HRESULT hr = pFlash->Forward();
    return SUCCEEDED( hr );
}

HRESULT CFlashControl::Rewind( )
{
    HRESULT hr = pFlash->Rewind();
    return SUCCEEDED( hr );
}
HRESULT CFlashControl::GotoFrame( long FrameNum )
{
    HRESULT hr = pFlash->GotoFrame( FrameNum );
    return SUCCEEDED( hr );
}

HRESULT CFlashControl::Init( CComPtr<IShockwaveFlash> ptrFlash )
{
    HRESULT aResult = NOERROR;
    LPCONNECTIONPOINTCONTAINER aConnectionPoint = NULL;
    if ( ( ptrFlash->QueryInterface( IID_IConnectionPointContainer, ( void** ) &aConnectionPoint ) == S_OK ) &&
            ( aConnectionPoint->FindConnectionPoint( __uuidof( ShockwaveFlashObjects::_IShockwaveFlashEvents ), &m_ConnectionPoint ) == S_OK ) )
    {
        IDispatch* aDispatch = NULL;
        QueryInterface( __uuidof( IDispatch ), ( void** ) &aDispatch );
        if ( aDispatch != NULL )
        {
            aResult = m_ConnectionPoint->Advise( ( LPUNKNOWN )aDispatch, &m_dwCookie );
            aDispatch->Release();
        }
    }
    
    if ( aConnectionPoint != NULL )
        aConnectionPoint->Release();
        
    return aResult;
}

HRESULT CFlashControl::Shutdown()
{
    HRESULT aResult = S_OK;
    
    if ( m_ConnectionPoint )
    {
        if ( m_dwCookie )
        {
            aResult = m_ConnectionPoint->Unadvise( m_dwCookie );
            m_dwCookie = 0;
        }
        
        m_ConnectionPoint->Release();
        m_ConnectionPoint = NULL;
    }
    
    return aResult;
}

HRESULT STDMETHODCALLTYPE CFlashControl::QueryInterface( REFIID riid, LPVOID* ppv )
{
    *ppv = NULL;
    
    if ( riid == IID_IUnknown )
    {
        *ppv = ( LPUNKNOWN )this;
        AddRef();
        return S_OK;
    }
    else if ( riid == IID_IDispatch )
    {
        *ppv = ( IDispatch* )this;
        AddRef();
        return S_OK;
    }
    else if ( riid == __uuidof( ShockwaveFlashObjects::_IShockwaveFlashEvents ) )
    {
        *ppv = ( ShockwaveFlashObjects::_IShockwaveFlashEvents* ) this;
        AddRef();
        return S_OK;
    }
    else
    {
        return E_NOTIMPL;
    }
}

ULONG STDMETHODCALLTYPE CFlashControl::AddRef()
{
    return ++m_nRefCount;
}

ULONG STDMETHODCALLTYPE CFlashControl::Release()
{
    int aRefCount = --m_nRefCount;
    if ( aRefCount == 0 )
        delete this;
        
    return aRefCount;
}

// IDispatch method
HRESULT STDMETHODCALLTYPE CFlashControl::GetTypeInfoCount( UINT* pctinfo )
{
    return E_NOTIMPL;
}

HRESULT STDMETHODCALLTYPE CFlashControl::GetTypeInfo( UINT iTInfo, LCID lcid, ITypeInfo** ppTInfo )
{
    return E_NOTIMPL;
}

HRESULT STDMETHODCALLTYPE CFlashControl::GetIDsOfNames( REFIID riid, LPOLESTR* rgszNames,
        UINT cNames, LCID lcid, DISPID* rgDispId )
{
    return E_NOTIMPL;
}

HRESULT STDMETHODCALLTYPE CFlashControl::Invoke( DISPID dispIdMember, REFIID riid, LCID lcid,
        WORD wFlags, ::DISPPARAMS __RPC_FAR *pDispParams, VARIANT __RPC_FAR *pVarResult,
        ::EXCEPINFO __RPC_FAR *pExcepInfo, UINT __RPC_FAR *puArgErr )
{
    switch ( dispIdMember )
    {
        case 0x7a6:
            break;
        case 0x96:
            if ( ( pDispParams->cArgs == 2 ) &&
                    ( pDispParams->rgvarg[0].vt == VT_BSTR ) &&
                    ( pDispParams->rgvarg[1].vt == VT_BSTR ) )
            {
                FSCommand( pDispParams->rgvarg[1].bstrVal, pDispParams->rgvarg[0].bstrVal );
            }
            break;
        case DISPID_READYSTATECHANGE:
            break;
        default:
            return DISP_E_MEMBERNOTFOUND;
    }
    
    return NOERROR;
}

HRESULT CFlashControl::OnReadyStateChange( long newState )
{
    MessageBox( L"", L"Read", MB_OK );
    return S_OK;
}

HRESULT CFlashControl::OnProgress( long percentDone )
{
    MessageBox( L"", L"Done", MB_OK );
    return S_OK;
}

HRESULT CFlashControl::FSCommand( _bstr_t command, _bstr_t args )
{
    //         if (m_pFlashWidget != NULL)
    //             m_pFlashWidget->FlashCommand((char*) command, (char*) args);
    return S_OK;
}