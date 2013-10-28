#include "stdafx.h"
#include "DevToolkit.h"
#include "DTKSystem.h"
using namespace DevToolkit;

BOOL IsAdmin()
{
    return CSystem::IsAdmin();
}

LPCTSTR GetLastErrMsg()
{
    return CSystem::GetLastErrMsg();
}

LPCTSTR FormatErrMsg( const DWORD dwErrID )
{
    return CSystem::FormatErrMsg( dwErrID );
}

BOOL runAsAdmin( LPCTSTR lpExeFile )
{
    return CSystem::runAsAdmin( lpExeFile );
}

BOOL IsVistaOrLater()
{
    return CSystem::IsVistaOrLater();
}

BOOL EnablePrivilege( LPCTSTR PrivilegeName )
{
    return CSystem::EnablePrivilege( PrivilegeName );
}

HINSTANCE OpenURL( const LPCTSTR szURL, BOOL bPriorityIE )
{
    return CSystem::OpenURL( szURL, bPriorityIE );
}

BOOL IsIA64()
{
    return CSystem::IsIA64();
}

HINSTANCE OpenFileInExplorer( const LPCTSTR lpszFile )
{
    return CSystem::OpenFileInExplorer( lpszFile );
}

LPCTSTR GetModulePathByHwnd( const HWND hWnd, LPTSTR lpszBuf, DWORD dwBufSize )
{
    return CSystem::GetModulePathByHwnd( hWnd, lpszBuf, dwBufSize );
}
