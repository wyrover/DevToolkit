#include "stdafx.h"
#include "DTKSystem.h"
#include <shellapi.h>
using namespace DevToolkit;

BOOL CSystem::IsAdmin()
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

LPCTSTR CSystem::GetLastErrMsg()
{
    return FormatErrMsg( GetLastError() );
}

LPCTSTR CSystem::FormatErrMsg( const DWORD dwErrID )
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

BOOL CSystem::runAsAdmin( LPCTSTR lpExeFile )
{
    SHELLEXECUTEINFO SEI = {sizeof( SHELLEXECUTEINFO )};
    SEI.lpVerb = _T( "runas" );
    SEI.lpFile = lpExeFile;
    SEI.nShow = SW_SHOWNORMAL;
    return ShellExecuteEx( &SEI );
}

BOOL CSystem::IsVistaOrLater()
{
    OSVERSIONINFO osvi;
    ZeroMemory( &osvi, sizeof( OSVERSIONINFO ) );
    osvi.dwOSVersionInfoSize = sizeof( OSVERSIONINFO );
    GetVersionEx( &osvi );
    if ( osvi.dwMajorVersion >= 6 )
        return TRUE;
    return FALSE;
}

BOOL CSystem::EnablePrivilege( LPCTSTR PrivilegeName )
{
    HANDLE tokenhandle;
    TOKEN_PRIVILEGES tp;
    tp.PrivilegeCount = 1;
    tp.Privileges[0].Attributes = SE_PRIVILEGE_ENABLED;
    
    if ( !OpenProcessToken( GetCurrentProcess(), TOKEN_ADJUST_PRIVILEGES, &tokenhandle ) )
    {
        return FALSE;
    }
    
    if ( !LookupPrivilegeValue( NULL, PrivilegeName, &tp.Privileges[0].Luid ) )
    {
        CloseHandle( tokenhandle );
        return FALSE;
    }
    
    if ( !AdjustTokenPrivileges( tokenhandle, FALSE, &tp, sizeof( tp ), NULL, NULL ) )
    {
        CloseHandle( tokenhandle );
        return FALSE;
    }
    
    CloseHandle( tokenhandle );
    return TRUE;
}

HINSTANCE CSystem::OpenURL( const LPCTSTR szURL, BOOL bPriorityIE )
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

BOOL CSystem::IsIA64()
{
    typedef void ( WINAPI * LPFN_PGNSI )( LPSYSTEM_INFO );
    typedef BOOL ( WINAPI * LPFN_ISWOW64PROCESS )( HANDLE, PBOOL );
    LPFN_PGNSI pGNSI = ( LPFN_PGNSI ) GetProcAddress( GetModuleHandle( TEXT( "kernel32.dll" ) ), "GetNativeSystemInfo" );
    LPFN_ISWOW64PROCESS fnIsWow64Process = ( LPFN_ISWOW64PROCESS ) GetProcAddress( GetModuleHandle( TEXT( "kernel32" ) ), "IsWow64Process" );
    
    BOOL bIsWow64 = FALSE;
    fnIsWow64Process( GetCurrentProcess(), &bIsWow64 );
    
    return bIsWow64;
}

HINSTANCE CSystem::OpenFileInExplorer( __in const LPCTSTR lpszFile )
{
    TCHAR sCmd[MAX_PATH] = {0};
    _stprintf_s( sCmd, _T( "%s, %s" ), _T( "/select" ), lpszFile );
    return ShellExecute( NULL, _T( "open" ), _T( "explorer" ), sCmd, NULL, SW_SHOW );
}

LPCTSTR CSystem::GetModulePathByHwnd( const HWND hWnd, LPTSTR lpszBuf, DWORD dwBufSize )
{
    DWORD dwPid = 0x0;
    GetWindowThreadProcessId( hWnd, &dwPid );
    HANDLE handle = OpenProcess( PROCESS_ALL_ACCESS, FALSE, dwPid );
    GetModuleFileName( ( HMODULE )handle, lpszBuf, dwBufSize );
    CloseHandle( handle );
    return lpszBuf;
}
