#include <Windows.h>
#include <winhttp.h>
#pragma comment(lib,"WinHttp.lib")
#include <atlstr.h>

namespace CODEHELPER
{
    class CWinHttpHandle
    {
    public:
        CWinHttpHandle();

        virtual ~CWinHttpHandle();

        bool Attach( HINTERNET handle );

        HINTERNET Detach();

        void Close();

        HRESULT SetOption( DWORD option, const void* value, DWORD length );

        HRESULT QueryOption( DWORD option, void* value, DWORD& length ) const;

        HINTERNET GetHandle()const;

    private:
        HINTERNET m_handle;
    };

    class CWinHttpSession : public CWinHttpHandle
    {
    public:
        CWinHttpSession();
        virtual ~CWinHttpSession();
    public:
        HRESULT Open();
    };

    class CWinHttpUrl
    {
    public:
        CWinHttpUrl( LPCTSTR lpszUrl );
        virtual ~CWinHttpUrl();
    public:
        BOOL CrackUrl();
        LPCTSTR GetHostName();
        WORD GetHostPort();
        LPCTSTR GetUrlPath()const;
        INTERNET_SCHEME GetScheme()const;
    protected:
    private:
        URL_COMPONENTS m_urlComp;
        TCHAR m_sUrl[MAX_PATH];
    };

    class CWinHttpConnection : public CWinHttpHandle
    {
    public:
        CWinHttpConnection( CWinHttpSession* pSession, CWinHttpUrl* pUrl );
        virtual ~CWinHttpConnection();
    public:
        HRESULT Open();
        CWinHttpSession* GetSession()const;
    private:
        CWinHttpSession* m_pSession;
        CWinHttpUrl* m_pUrl;
    };

    class CWinHttpHeader
    {
    public:
        CWinHttpHeader();
        virtual ~CWinHttpHeader();
    public:
        void Add( LPCTSTR lpszKey, LPCTSTR lpszValue );
        LPCTSTR GetHeader()const;
    private:
        CString m_sHeader;
    };

    class CWinHttpRequest : public CWinHttpHandle
    {
        typedef void ( __stdcall *UserCallback )( LPVOID pParam, DWORD dwStatus, LPCVOID lpData, DWORD dwDataSize, DWORD dwReaded, DWORD dwTotal );
    public:
        CWinHttpRequest( CWinHttpConnection* pConnect, CWinHttpUrl* pUrl );
        virtual ~CWinHttpRequest();
    public:
        HRESULT Open( LPCTSTR lpszVerb );
        HRESULT EnableRedirect( BOOL bEnable = TRUE );
        HRESULT AddHeader( CWinHttpHeader* pHeader );
        HRESULT Send( LPCTSTR pwszHeaders = WINHTTP_NO_ADDITIONAL_HEADERS, DWORD dwHeadersLength = 0,
                      LPVOID lpOptional = WINHTTP_NO_REQUEST_DATA, DWORD dwOptionalLength = 0, DWORD dwTotalLength = 0 );
        void SetUserCallback( UserCallback pUserCallback, LPVOID pParam );
        void SetReadedSize( DWORD dwSize );
		void Pause();
		void Resume();
    private:
		HANDLE m_hRunEvent;
        CWinHttpConnection* m_pConnect;
        DWORD m_dwTotalSize;
        DWORD m_dwReadedSize;
        CWinHttpUrl* m_pUrl;
        LPVOID m_pParam;
        UserCallback m_funcUserCallback;
        static void CALLBACK Callback( HINTERNET hInternet, DWORD_PTR dwContext, DWORD dwInternetStatus,
                                       LPVOID lpvStatusInformation, DWORD dwStatusInformationLength );
        HRESULT OnSendRequestComplete( LPVOID lpvStatusInformation, DWORD dwStatusInformationLength );
        HRESULT OnHeaderAvailable( LPVOID lpvStatusInformation, DWORD dwStatusInformationLength );
        HRESULT OnDataAvailable( LPVOID lpvStatusInformation, DWORD dwStatusInformationLength );
        HRESULT OnReadComplete( LPVOID lpvStatusInformation, DWORD dwStatusInformationLength );
    };

    typedef void ( *DownCallback )( LPVOID pParam, DWORD dwStatus, DWORD dwDownloaded, DWORD dwTotal );

    class __declspec( dllexport ) CDownload
    {
    public:
        CDownload( LPCTSTR lpszUrl, LPCTSTR lpszSavePath );
        virtual ~CDownload();
    public:
        void SetCallBack( DownCallback callBack, LPVOID lpcallBackParam );
        BOOL SendRequest();
        BOOL PostRequest();
		void Pause();
		void Resume();
		DWORD GetTaskID();  
	  void CloseAll();
    protected:
        void AddHeader( LPCTSTR lpszKey, LPCTSTR lpszValue );
        void EnableRedirect( BOOL bEnable = FALSE );
        static void __stdcall DownloadCallback( LPVOID pParam, DWORD dwStatus, LPCVOID lpData,
                                                DWORD dwDataSize, DWORD dwReaded, DWORD dwTotal );

    private:
        CWinHttpUrl* m_pHttpUrl;
        CWinHttpSession* m_pSession;
        CWinHttpConnection* m_pConnect;
        CWinHttpRequest* m_pRequest;
        CWinHttpHeader* m_pHeader;
        TCHAR m_sSavePath[MAX_PATH];
        BOOL m_bEnableRedirect;
        FILE* m_pFile;
        DownCallback m_downCallback;
        LPVOID m_callBackParam;
		static DWORD m_dwTaskID;
    };

    enum DOWNSTATUS
    {
        DOWN_STATUS_UKNOWN,
        DOWN_STATUS_ERROR,
        DOWN_STATUS_DOWNLOADING,
        DWON_STATUS_SUCCESS
    };

}