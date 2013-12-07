#pragma once
#include <atlwin.h>
#include <atltypes.h>
#pragma warning(disable:4192)

#import "PROGID:ShockwaveFlash.ShockwaveFlash"  \
	raw_interfaces_only,       /* Don't add raw_ to method names */ \
	named_guids,           /* Named guids and declspecs */    \
	rename("IDispatchEx","IMyDispatchEx")	/* fix conflicting with IDispatchEx ant dispex.h */
using namespace ShockwaveFlashObjects;

namespace DevToolkit
{
    class CFlashControl : public ShockwaveFlashObjects::_IShockwaveFlashEvents , public CAxWindow
    {
    private:
        HWND m_hWnd;
        CComPtr<IShockwaveFlash> pFlash;
    public:
        LPCONNECTIONPOINT       m_ConnectionPoint;
        DWORD                   m_dwCookie;
        int                     m_nRefCount;
        
    public:
        CFlashControl();
        
        virtual ~CFlashControl();
        
        BOOL Create( HWND hWnd, CRect rc );
        
        
        void Destroy();
        
        BOOL LoadFlash( BSTR lpszFlash );
        
        
        BOOL Start();
        
        
        BOOL Stop();
        
        
        BOOL IsPlaying();
        
        
        HRESULT Back( );
        
        
        HRESULT Forward( );
        
        HRESULT Rewind( );
        
        HRESULT GotoFrame( long FrameNum );
        
        
    protected:
        HRESULT Init( CComPtr<IShockwaveFlash> ptrFlash );
        
        HRESULT Shutdown();
        
        
        HRESULT STDMETHODCALLTYPE QueryInterface( REFIID riid, LPVOID* ppv );
        
        ULONG STDMETHODCALLTYPE AddRef();
        
        
        ULONG STDMETHODCALLTYPE Release();
        
        
        // IDispatch method
        virtual HRESULT STDMETHODCALLTYPE GetTypeInfoCount( UINT* pctinfo );
        
        virtual HRESULT STDMETHODCALLTYPE GetTypeInfo( UINT iTInfo, LCID lcid, ITypeInfo** ppTInfo );
        
        virtual HRESULT STDMETHODCALLTYPE GetIDsOfNames( REFIID riid, LPOLESTR* rgszNames,
                UINT cNames, LCID lcid, DISPID* rgDispId );
                
                
        virtual HRESULT STDMETHODCALLTYPE Invoke( DISPID dispIdMember, REFIID riid, LCID lcid,
                WORD wFlags, ::DISPPARAMS __RPC_FAR *pDispParams, VARIANT __RPC_FAR *pVarResult,
                ::EXCEPINFO __RPC_FAR *pExcepInfo, UINT __RPC_FAR *puArgErr );
                
        HRESULT OnReadyStateChange( long newState );
        
        
        HRESULT OnProgress( long percentDone );
        
        HRESULT FSCommand( _bstr_t command, _bstr_t args );
        
    };
}
