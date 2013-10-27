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
