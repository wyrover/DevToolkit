#pragma once
//#include <atlbase.h>
#include <exdispid.h>
#include <atlcom.h>
#include <exdisp.h>
// #include <atlwin.h>
// #include <atltypes.h>
// #include <atlapp.h>
namespace DevToolkit
{
    enum WEBBROWSER_TYPE
    {
        WEBBROWSER_NORMAL,
        WEBBROWSER_ASYNC
    };

    class IWEBBROWSER
    {
    public:
        virtual BOOL Init() = 0;
        virtual void UnInit() = 0;
        virtual BOOL Navigate( LPCTSTR lpszUrl ) = 0;
        virtual void Refresh() = 0;
        virtual void Stop() = 0;
        virtual void GoBack() = 0;
        virtual void GoForward() = 0;
        virtual void AdjustLayout() = 0;
        static IWEBBROWSER* Create( HWND hParent, const CRect& rcWnd, WEBBROWSER_TYPE webBrowserType = WEBBROWSER_ASYNC );
    };
    
    __declspec( selectany ) _ATL_FUNC_INFO PlayUGCUrlInfo = {CC_STDCALL, VT_I4, 3, { VT_BSTR, VT_BSTR, VT_BSTR,}};
    __declspec( selectany ) _ATL_FUNC_INFO GetAppVerInfo = {CC_STDCALL, VT_BSTR, 0};
    __declspec( selectany ) _ATL_FUNC_INFO GetUUIDInfo = {CC_STDCALL, VT_BSTR, 0};
    __declspec( selectany ) _ATL_FUNC_INFO LoadCompleteInfo = {CC_STDCALL, VT_I4, 0};
    __declspec( selectany ) _ATL_FUNC_INFO CommandStateChange = {CC_STDCALL, VT_EMPTY, 2, { VT_I4, VT_BOOL }};
    __declspec( selectany ) _ATL_FUNC_INFO NewWindow = {CC_STDCALL, VT_EMPTY, 5, { VT_DISPATCH, VT_BOOL | VT_BYREF, VT_I4, VT_BSTR, VT_BSTR }};
    __declspec( selectany ) _ATL_FUNC_INFO DocumentComplete = {CC_STDCALL, VT_EMPTY, 2, { VT_DISPATCH, VT_VARIANT | VT_BYREF}};
    class CWebBrowser : public IDocHostUIHandlerDispatch , public CAxWindow
        , public IDispEventSimpleImpl<0, CWebBrowser, &DIID_DWebBrowserEvents2>
        , public IWEBBROWSER
    {
    public:
        CWebBrowser( void );
        virtual ~CWebBrowser( void );
        
        BEGIN_SINK_MAP( CWebBrowser )
        SINK_ENTRY_INFO( 0, DIID_DWebBrowserEvents2, DISPID_COMMANDSTATECHANGE, CommandStateChanged, &CommandStateChange )
        SINK_ENTRY_INFO( 0, DIID_DWebBrowserEvents2, DISPID_NEWWINDOW3, OnNewWindow, &NewWindow )
        SINK_ENTRY_INFO( 0, DIID_DWebBrowserEvents2, DISPID_DOCUMENTCOMPLETE, OnDocumentComplete, &DocumentComplete )
        END_SINK_MAP()
        
        // IDocHostUIHandlerDispatch接口
        HRESULT STDMETHODCALLTYPE ShowContextMenu( DWORD dwID, DWORD x, DWORD y, IUnknown *pcmdtReserved, IDispatch *pdispReserved, HRESULT *dwRetVal );
        
        HRESULT STDMETHODCALLTYPE GetHostInfo( DWORD *pdwFlags, DWORD *pdwDoubleClick ) ;
        
        HRESULT STDMETHODCALLTYPE ShowUI( DWORD dwID, IUnknown *pActiveObject, IUnknown *pCommandTarget, IUnknown *pFrame, IUnknown *pDoc, HRESULT *dwRetVal ) ;
        
        HRESULT STDMETHODCALLTYPE HideUI( void ) ;
        
        HRESULT STDMETHODCALLTYPE UpdateUI( void );
        
        HRESULT STDMETHODCALLTYPE EnableModeless( VARIANT_BOOL fEnable ) ;
        
        HRESULT STDMETHODCALLTYPE OnDocWindowActivate( VARIANT_BOOL fActivate ) ;
        
        HRESULT STDMETHODCALLTYPE OnFrameWindowActivate( VARIANT_BOOL fActivate );
        
        HRESULT STDMETHODCALLTYPE ResizeBorder( long left, long top, long right, long bottom, IUnknown *pUIWindow, VARIANT_BOOL fFrameWindow );
        
        HRESULT STDMETHODCALLTYPE TranslateAccelerator( DWORD_PTR hWnd, DWORD nMessage, DWORD_PTR wParam, DWORD_PTR lParam, BSTR bstrGuidCmdGroup, DWORD nCmdID, HRESULT *dwRetVal );
        
        HRESULT STDMETHODCALLTYPE GetOptionKeyPath( BSTR *pbstrKey, DWORD dw );
        
        HRESULT STDMETHODCALLTYPE GetDropTarget( IUnknown *pDropTarget, IUnknown **ppDropTarget );
        
        HRESULT STDMETHODCALLTYPE GetExternal( IDispatch **ppDispatch );
        
        HRESULT STDMETHODCALLTYPE TranslateUrl( DWORD dwTranslate, BSTR bstrURLIn, BSTR *pbstrURLOut );
        
        HRESULT STDMETHODCALLTYPE FilterDataObject( IUnknown *pDO, IUnknown **ppDORet );
        
        //IUnknown 接口
        HRESULT STDMETHODCALLTYPE QueryInterface( REFIID riid, void **ppv );
        
        ULONG STDMETHODCALLTYPE AddRef();
        
        ULONG STDMETHODCALLTYPE Release();
        
        // IDispatch 接口
        // 	HRESULT Advise(IUnknown* pUnk);
        //
        // 	HRESULT DispEventAdvise(IUnknown* pUnk, const IID* piid );
        //
        // 	HRESULT DispEventUnadvise(IUnknown* pUnk, const IID* piid );
        //
        // 	HRESULT Unadvise(IUnknown* pUnk );
        
        HRESULT STDMETHODCALLTYPE GetTypeInfoCount( UINT __RPC_FAR *pctinfo );
        
        HRESULT STDMETHODCALLTYPE GetTypeInfo( UINT iTInfo, LCID lcid, ITypeInfo __RPC_FAR *__RPC_FAR *ppTInfo );
        
        HRESULT STDMETHODCALLTYPE GetIDsOfNames( REFIID riid, LPOLESTR __RPC_FAR *rgszNames, UINT cNames, LCID lcid, DISPID __RPC_FAR *rgDispId );
        
        HRESULT STDMETHODCALLTYPE Invoke( DISPID dispIdMember, REFIID riid, LCID lcid, WORD wFlags, DISPPARAMS __RPC_FAR *pDispParams, VARIANT __RPC_FAR *pVarResult, EXCEPINFO __RPC_FAR *pExcepInfo, UINT __RPC_FAR *puArgErr );
        
        virtual void __stdcall OnNewWindow( IDispatch ** pObj, VARIANT_BOOL* Cannel, DWORD dwFlags, BSTR urlContext, BSTR url );
        
        virtual void __stdcall OnDocumentComplete( IDispatch *pDisp, VARIANT * pvarURL );
        
        virtual void __stdcall CommandStateChanged( long lCommand, BOOL bReady );
        
    public:
        BOOL Create( HWND hParent, RECT rcWnd );
        BOOL Init();
        void UnInit();
        BOOL Navigate( LPCTSTR lpszUrl );
        void Refresh();
        void Stop();
        void GoBack();
        void GoForward();
        void AdjustLayout();
    protected:
        BOOL Initialize();
        void UnInitialize();
        HWND GetParent();
    private:
        HWND m_hParent;
        CRect m_rcClient;
        LONG m_ref;
        CComQIPtr<IWebBrowser2> m_spWebBrowser2;
        HWND m_hHost;
    };
    
    const int WM_WEBBROWSER_METHORD = WM_USER + 100;
    enum WEBBROWSER_PARAM
    {
        WEBBROWSER_INIT,			// 初始化
        WEBBROWSER_NAVIGATE,			// 打开URL
        WEBBROWSER_STOP,			// 暂停
        WEBBROWSER_REFRESH,		// 刷新
        WEBBROWSER_FORWARD,		// 向前
        WEBBROWSER_BACK,			// 后退
        WEBBROWSER_UNINIT			// 反初始化
    };
    
    class CAsyncWebBrowser : public CWebBrowser, public CMessageFilter
    {
    public:
        CAsyncWebBrowser();
        virtual ~CAsyncWebBrowser();
    public:
        BOOL Create( HWND hParent, const CRect& rcClient );
        virtual BOOL PreTranslateMessage( MSG* pMsg );
        
        BOOL Init();
        void UnInit();
        BOOL Navigate( LPCTSTR lpszUrl ) ;
        void Refresh() ;
        void Stop() ;
        void GoBack() ;
        void GoForward() ;
        void AdjustLayout();
    protected:
        static UINT __stdcall _WebBrowserThread( LPVOID pParam );
        UINT WebBrowserThread();
        virtual LRESULT OnWebbrowserMethord( WPARAM wParam, LPARAM lParam );
    private:
        HANDLE m_hThread;
        UINT m_dwThreadID;
        HANDLE m_hEventThreadRun;
    };
}
