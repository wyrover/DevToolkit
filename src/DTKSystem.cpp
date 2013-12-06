#include "stdafx.h"
#include "DTKSystem.h"
#include <shellapi.h>
#include <wininet.h>
#include <Sensapi.h>
#include <commdlg.h>
#include <tlhelp32.h>

BOOL IsAdmin()
{
    BOOL bIsElevated = FALSE;
    HANDLE hToken    = NULL;
    UINT16 uWinVer   = LOWORD( GetVersion() );
    uWinVer = MAKEWORD( HIBYTE( uWinVer ), LOBYTE( uWinVer ) );
    if ( uWinVer < 0x0600 ) return TRUE;
    if ( OpenProcessToken( GetCurrentProcess(), TOKEN_QUERY, &hToken ) )
    {
        struct
        {
            DWORD TokenIsElevated;
        } te;
        DWORD dwReturnLength = 0;
        if ( GetTokenInformation( hToken, TokenElevation, &te, sizeof( te ), &dwReturnLength ) )
        {
            if ( dwReturnLength == sizeof( te ) )
                bIsElevated = te.TokenIsElevated;
        }
        CloseHandle( hToken );
    }
    return bIsElevated;
}

LPCTSTR GetLastErrMsg()
{
    return FormatErrMsg( GetLastError() );
}

LPCTSTR FormatErrMsg( const DWORD dwErrID )
{
    LPTSTR lpBuffer = NULL;
    FormatMessage(
        FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS,
        NULL,
        dwErrID,
        MAKELANGID( LANG_NEUTRAL, SUBLANG_DEFAULT ),
        lpBuffer,
        MAX_PATH, NULL );
    return lpBuffer;
}

BOOL runAsAdmin( LPCTSTR lpExeFile )
{
    SHELLEXECUTEINFO SEI = {sizeof( SHELLEXECUTEINFO )};
    SEI.lpVerb = _T( "runas" );
    SEI.lpFile = lpExeFile;
    SEI.nShow = SW_SHOWNORMAL;
    return ShellExecuteEx( &SEI );
}

BOOL IsVistaOrLater()
{
    OSVERSIONINFO osvi;
    ZeroMemory( &osvi, sizeof( OSVERSIONINFO ) );
    osvi.dwOSVersionInfoSize = sizeof( OSVERSIONINFO );
    GetVersionEx( &osvi );
    if ( osvi.dwMajorVersion >= 6 )
        return TRUE;
    return FALSE;
}

HINSTANCE OpenURL( const LPCTSTR szURL, BOOL bPriorityIE )
{
    HINSTANCE hIn = NULL;
    if ( bPriorityIE )
    {
        hIn = ShellExecute( NULL, _T( "open" ), _T( "explorer.exe" ), szURL, NULL, SW_SHOWNORMAL );
    }
    else
        hIn = ShellExecute( NULL, _T( "open" ), szURL, NULL, NULL, SW_SHOWNORMAL );
    return hIn;
}

BOOL IsIA64()
{
    typedef void ( WINAPI * LPFN_PGNSI )( LPSYSTEM_INFO );
    typedef BOOL ( WINAPI * LPFN_ISWOW64PROCESS )( HANDLE, PBOOL );
    LPFN_PGNSI pGNSI = ( LPFN_PGNSI ) GetProcAddress( GetModuleHandle( TEXT( "kernel32.dll" ) ), "GetNativeSystemInfo" );
    LPFN_ISWOW64PROCESS fnIsWow64Process = ( LPFN_ISWOW64PROCESS ) GetProcAddress( GetModuleHandle( TEXT( "kernel32" ) ), "IsWow64Process" );
    
    BOOL bIsWow64 = FALSE;
    fnIsWow64Process( GetCurrentProcess(), &bIsWow64 );
    
    return bIsWow64;
}

HINSTANCE OpenFileInExplorer( __in const LPCTSTR lpszFile )
{
    TCHAR sCmd[MAX_PATH] = {0};
    _stprintf_s( sCmd, _T( "%s, %s" ), _T( "/select" ), lpszFile );
    return ShellExecute( NULL, _T( "open" ), _T( "explorer" ), sCmd, NULL, SW_SHOW );
}

LPCTSTR GetModulePathByHwnd( const HWND hWnd, LPTSTR lpszBuf, DWORD dwBufSize )
{
    DWORD dwPid = 0x0;
    GetWindowThreadProcessId( hWnd, &dwPid );
    HANDLE handle = OpenProcess( PROCESS_ALL_ACCESS, FALSE, dwPid );
    GetModuleFileName( ( HMODULE )handle, lpszBuf, dwBufSize );
    CloseHandle( handle );
    return lpszBuf;
}

BOOL ParseURLs( LPCTSTR pwszURL, LPTSTR pwszHostName, LPTSTR pwszPath, WORD& wPort )
{
    if ( pwszURL == NULL )
    {
        return FALSE;
    }
    
    WCHAR wszCanonicalizedURL[INTERNET_MAX_URL_LENGTH];
    int iLen = 0;
    BOOL bOk = EncodeURL( pwszURL, wcslen( pwszURL ), wszCanonicalizedURL, iLen );
    if ( bOk )
    {
        URL_COMPONENTS urlComponents = {0};
        urlComponents.dwStructSize = sizeof( URL_COMPONENTS );
        urlComponents.dwHostNameLength = INTERNET_MAX_URL_LENGTH;
        urlComponents.lpszHostName = pwszHostName;
        urlComponents.dwUrlPathLength = INTERNET_MAX_URL_LENGTH;
        urlComponents.lpszUrlPath = pwszPath;
        bOk = InternetCrackUrl( wszCanonicalizedURL, 0, ICU_ESCAPE, &urlComponents );
        bOk = ( bOk && urlComponents.nScheme == INTERNET_SERVICE_HTTP );
        if ( bOk )
        {
            wPort = urlComponents.nPort;
        }
    }
    
    return bOk;
}


BOOL EncodeURL( LPCTSTR pwszURL, int iLen, LPTSTR pwszRetURL, int& iRetLen )
{
    const WCHAR HEX_MAP[] =
    {
        _T( '0' ), _T( '1' ), _T( '2' ), _T( '3' ), _T( '4' ), _T( '5' ), _T( '6' ), _T( '7' ),
        _T( '8' ), _T( '9' ), _T( 'A' ), _T( 'B' ), _T( 'C' ), _T( 'D' ), _T( 'E' ), _T( 'F' )
    };
    
    char* pszBuff  = new char[iLen * 4];
    if ( pszBuff == NULL )
    {
        return FALSE;
    }
    
    int iCnt = WideCharToMultiByte( CP_UTF8, 0, pwszURL, iLen, pszBuff, iLen * 4, NULL, NULL );
    iRetLen = 0;
    
    for ( int i = 0; i < iCnt; i++ )
    {
        char ch = pszBuff[i];
        if ( ch > _T( ' ' ) && ch != _T( '%' ) )
        {
            pwszRetURL[iRetLen++] = ch;
        }
        else
        {
            int iVal = ch;
            pwszRetURL[iRetLen++] = _T( '%' );
            pwszRetURL[iRetLen++] = HEX_MAP[( iVal >> 4 ) & 0x0f];
            pwszRetURL[iRetLen++] = HEX_MAP[iVal & 0xf];
        }
    }
    pwszRetURL[iRetLen] = _T( '\0' );
    
    delete [] pszBuff;
    return TRUE;
}


HRESULT OpenUrlWithDefaultBrowser( LPCTSTR lpUrl )
{
    HRESULT hr = S_OK;
    // 	HKEY hkRoot,hSubKey;
    // 	TCHAR ValueName[256];
    // 	TCHAR DataValue[256];
    // 	unsigned long cbValueName=256;
    // 	unsigned long cbDataValue=256;
    // 	TCHAR ShellChar[256];
    // 	DWORD dwType;
    // 	CString str = _T("\"");
    //
    // 	if(RegOpenKey(HKEY_CLASSES_ROOT,NULL,&hkRoot)!=ERROR_SUCCESS)
    // 	{
    // 		return S_FALSE;
    // 	}
    // 	if (RegOpenKeyEx(hkRoot,_T("htmlfile\\shell\\open\\command"),0,KEY_READ,&hSubKey)!=ERROR_SUCCESS)
    // 	{
    // 		return S_FALSE;
    // 	}
    // 	RegEnumValue(hSubKey,0,ValueName,&cbValueName,NULL,&dwType,(LPBYTE)DataValue,&cbDataValue);
    // 	lstrcpy(ShellChar,(TCHAR *)DataValue);
    // 	TCHAR * pTemp=ShellChar;
    // 	++pTemp;
    // 	while(*pTemp != _T('"'))
    // 	{
    // 		str+=*pTemp;
    // 		++pTemp;
    // 	}
    // 	str+=_T("\"");
    // 	ShellExecute(NULL,_T("open"),str.GetBuffer(),lpUrl,NULL,SW_NORMAL);
    // 	str.ReleaseBuffer();
    //
    // 	RegCloseKey(hSubKey);
    // 	RegCloseKey(hkRoot);
    return hr;
}

BOOL IsHasNetConnect()
{
    DWORD dwFlag = 0;
    if ( !IsNetworkAlive( &dwFlag ) )
    {
        DWORD dwErr =::GetLastError();
        if ( 0 == dwErr )
        {
            return FALSE;
        }
        dwFlag = 0;
        if ( InternetGetConnectedState( &dwFlag, 0 ) )
        {
            if ( INTERNET_CONNECTION_OFFLINE == dwFlag )
            {
                return FALSE;
            }
        }
    }
    return TRUE;
}

BOOL OpenUrlWithIE( LPCTSTR lpszURL )
{
    typedef HINSTANCE( *ShellExecuteFunc )( HWND, LPCTSTR , LPCTSTR , LPCTSTR , LPCTSTR , int );
    ShellExecuteFunc mShellExecuteW = NULL;
    try
    {
        HINSTANCE hShell32 =::LoadLibrary( _T( "Shell32.dll" ) );
        if ( NULL != hShell32 )
        {
            mShellExecuteW = ( ShellExecuteFunc )::GetProcAddress( hShell32, "ShellExecuteA" );
            if ( NULL != mShellExecuteW )
            {
                HINSTANCE hins = mShellExecuteW( NULL, _T( "open" ), _T( "iexplore" ), lpszURL, NULL, SW_SHOWNORMAL );
                if ( 32 < reinterpret_cast<int>( hins ) )
                {
                    ::FreeLibrary( hShell32 );
                    return TRUE;
                }
            }
            ::FreeLibrary( hShell32 );
        }
    }
    catch ( ... )
    {
        return FALSE;
    }
    
    return FALSE;
}

BOOL IsWin7()
{
    OSVERSIONINFOEXW osverexw;
    memset( &osverexw, 0, sizeof( osverexw ) );
    osverexw.dwOSVersionInfoSize = sizeof( osverexw ); //sizeof(OSVERSIONINFOEXW)
    if ( !::GetVersionExW( ( OSVERSIONINFOW* )&osverexw ) )
    {
        return FALSE;
    }
    
    if ( osverexw.dwMajorVersion == 6 && osverexw.dwMinorVersion == 1 ) //Win 7 Version : 6.1
        return TRUE;
        
    return FALSE;
}

BOOL IsXP()
{
    OSVERSIONINFOEXW osverexw;
    memset( &osverexw, 0, sizeof( osverexw ) );
    osverexw.dwOSVersionInfoSize = sizeof( osverexw ); //sizeof(OSVERSIONINFOEXW)
    if ( !::GetVersionExW( ( OSVERSIONINFOW* )&osverexw ) )
    {
        return FALSE;
    }
    
    if ( osverexw.dwMajorVersion == 5 && osverexw.dwMinorVersion == 1 ) //Win XP Version : 5.1
        return TRUE;
        
    return FALSE;
}

BOOL IsVista()
{
    OSVERSIONINFOEXW osverexw;
    memset( &osverexw, 0, sizeof( osverexw ) );
    osverexw.dwOSVersionInfoSize = sizeof( osverexw ); //sizeof(OSVERSIONINFOEXW)
    if ( !::GetVersionExW( ( OSVERSIONINFOW* )&osverexw ) )
    {
        return FALSE;
    }
    
    if ( osverexw.dwMajorVersion == 6 && osverexw.dwMinorVersion == 0 ) //Win Vista Version : 6.0
        return TRUE;
        
    return FALSE;
}

BOOL IsWin8()
{
    OSVERSIONINFOEXW osverexw;
    memset( &osverexw, 0, sizeof( osverexw ) );
    osverexw.dwOSVersionInfoSize = sizeof( osverexw ); //sizeof(OSVERSIONINFOEXW)
    if ( !::GetVersionExW( ( OSVERSIONINFOW* )&osverexw ) )
    {
        return FALSE;
    }
    
    if ( osverexw.dwMajorVersion == 6 && osverexw.dwMinorVersion == 1 ) //Win 8 Version : 6.2
        return TRUE;
        
    return FALSE;
}

BOOL IsWin2000()
{
    OSVERSIONINFOEXW osverexw;
    memset( &osverexw, 0, sizeof( osverexw ) );
    osverexw.dwOSVersionInfoSize = sizeof( osverexw ); //sizeof(OSVERSIONINFOEXW)
    if ( !::GetVersionExW( ( OSVERSIONINFOW* )&osverexw ) )
    {
        return FALSE;
    }
    
    if ( osverexw.dwMajorVersion == 5 && osverexw.dwMinorVersion == 0 ) //Win 2000 Version : 5.0
        return TRUE;
        
    return FALSE;
}

BOOL IsWinNT()
{
    OSVERSIONINFOEXW osverexw;
    memset( &osverexw, 0, sizeof( osverexw ) );
    osverexw.dwOSVersionInfoSize = sizeof( osverexw ); //sizeof(OSVERSIONINFOEXW)
    if ( !::GetVersionExW( ( OSVERSIONINFOW* )&osverexw ) )
    {
        return FALSE;
    }
    
    if ( osverexw.dwMajorVersion >= 4 ) //Win NT Version : <=4
        return TRUE;
        
    return FALSE;
}


BOOL SetProcessAppUserModelID( LPCWSTR lpwsProcessUserModelID )
{
    if ( !IsWin7() )
        return FALSE;
        
    //适应非win7及以后系统
    //HRESULT hr = SetCurrentProcessExplicitAppUserModelID(lpwsProcessUserModelID);
    
    HRESULT hr = E_FAIL;
    HMODULE hMod = LoadLibraryW( L"shell32.dll" );
    if ( hMod )
    {
        typedef HRESULT( __stdcall * PFNSetCurrentProcessExplicitAppUserModelID )( PCWSTR pwsID );
        PFNSetCurrentProcessExplicitAppUserModelID pfnSetID = ( PFNSetCurrentProcessExplicitAppUserModelID )GetProcAddress( hMod, "SetCurrentProcessExplicitAppUserModelID" );
        if ( pfnSetID )
        {
            hr = pfnSetID( lpwsProcessUserModelID );
        }
        FreeLibrary( hMod );
    }
    
    return SUCCEEDED( hr );
}

BOOL OpenFileDlg( HWND hWnd, LPTSTR lpFileName )
{
    if ( NULL == lpFileName )
    {
        return FALSE;
    }
    
    BOOL bRet = FALSE;
    OPENFILENAME    ofn = {0};
    // 开始设置OPENFILENAME结构成员
    ofn.lStructSize   = sizeof( OPENFILENAME );
    ofn.hwndOwner   =   hWnd;
    ofn.lpstrFile   =   lpFileName;
    ofn.nMaxFile   =   MAX_PATH;
    ofn.lpstrFilter    =  _T( "DLL文件|*.DLL||" );
    ofn.nFilterIndex   =  1;
    ofn.lpstrTitle   =  _T( "选择要注入的DLL" );
    ofn.lpstrInitialDir = 0;
    ofn.Flags = OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST;
    
    // 指派给"打开"对话框, 并显示对话框
    if ( ::GetOpenFileName( &ofn ) )
    {
        bRet = TRUE;
    }
    
    return bRet;
}

DWORD GetFunctionAddress( HMODULE phModule, TCHAR* pProcName )
{
    if ( !phModule )
        return        0;
    PIMAGE_DOS_HEADER pimDH = ( PIMAGE_DOS_HEADER )phModule;
    PIMAGE_NT_HEADERS pimNH = ( PIMAGE_NT_HEADERS )( ( TCHAR* )phModule + pimDH->e_lfanew );
    PIMAGE_EXPORT_DIRECTORY pimED = ( PIMAGE_EXPORT_DIRECTORY )( ( DWORD )phModule + pimNH->OptionalHeader.DataDirectory[IMAGE_DIRECTORY_ENTRY_EXPORT].VirtualAddress );
    DWORD pExportSize = pimNH->OptionalHeader.DataDirectory[IMAGE_DIRECTORY_ENTRY_EXPORT].Size;
    DWORD pResult = 0;
    
    if ( ( DWORD )pProcName < 0x10000 )
    {
        if ( ( DWORD )pProcName >= pimED->NumberOfFunctions + pimED->Base || ( DWORD )pProcName < pimED->Base )
            return 0;
        pResult = ( DWORD )phModule + ( ( DWORD* )( ( DWORD )phModule + pimED->AddressOfFunctions ) )[( DWORD )pProcName - pimED->Base];
    }
    else
    {
        DWORD* pAddressOfNames = ( DWORD* )( ( DWORD )phModule + pimED->AddressOfNames );
        for ( DWORD i = 0; i < pimED->NumberOfNames; i++ )
        {
            TCHAR* pExportName = ( TCHAR* )( pAddressOfNames[i] + ( DWORD )phModule );
            if ( _tcscmp( pProcName, pExportName ) == 0 )
            {
                WORD* pAddressOfNameOrdinals = ( WORD* )( ( DWORD )phModule + pimED->AddressOfNameOrdinals );
                pResult = ( DWORD )phModule + ( ( DWORD* )( ( DWORD )phModule + pimED->AddressOfFunctions ) )[pAddressOfNameOrdinals[i]];
                break;
            }
        }
    }
    if ( pResult != 0 && pResult >= ( DWORD )pimED && pResult < ( DWORD )pimED + pExportSize )
    {
        TCHAR* pDirectStr = ( TCHAR* )pResult;
        bool pstrok = false;
        while ( *pDirectStr )
        {
            if ( *pDirectStr == '.' )
            {
                pstrok = TRUE;
                break;
            }
            pDirectStr++;
        }
        if ( !pstrok )
            return 0;
        TCHAR pdllname[MAX_PATH];
        int  pnamelen = pDirectStr - ( TCHAR* )pResult;
        if ( pnamelen <= 0 )
            return 0;
        memcpy( pdllname, ( TCHAR* )pResult, pnamelen );
        pdllname[pnamelen] = 0;
        HMODULE phexmodule = GetModuleHandle( pdllname );
        pResult  = GetFunctionAddress( phexmodule, pDirectStr + 1 );
    }
    
    return pResult;
}

HWND GetWindowHandleByPID( DWORD dwProcessID )
{
    HWND h = GetTopWindow( 0 );
    while ( h )
    {
        DWORD pid = 0;
        DWORD dwTheardId = GetWindowThreadProcessId( h, &pid );
        if ( dwTheardId != 0 )
        {
            if ( pid == dwProcessID )
            {
                return h;
            }
        }
        h = GetNextWindow( h , GW_HWNDNEXT );
    }
    return NULL;
}

DWORD GetThreadIDByProcssID( DWORD dwProcessID )
{
    DWORD dwThreadID = 0;
    THREADENTRY32 te32 = {sizeof( te32 )};
    HANDLE hThreadSnap = CreateToolhelp32Snapshot( TH32CS_SNAPTHREAD, dwProcessID );
    if ( Thread32First( hThreadSnap, &te32 ) )
    {
        do
        {
            if ( dwProcessID == te32.th32OwnerProcessID )
            {
                dwThreadID = te32.th32ThreadID;
                break;
            }
        }
        while ( Thread32Next( hThreadSnap, &te32 ) );
    }
    
    return dwThreadID;
}
