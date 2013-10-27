#include "stdafx.h"
#include "DevToolkit.h"
#include "DTKSystem.h"

BOOL IsAdmin()
{
    return DevToolkit::CSystem::IsAdmin();
}

LPCTSTR GetLastErrMsg()
{
    return DevToolkit::CSystem::GetLastErrMsg();
}

LPCTSTR FormatErrMsg( const DWORD dwErrID )
{
    return DevToolkit::CSystem::FormatErrMsg( dwErrID );
}

BOOL runAsAdmin( LPCTSTR lpExeFile )
{
    return DevToolkit::CSystem::runAsAdmin( lpExeFile );
}

BOOL IsVistaOrLater()
{
    return DevToolkit::CSystem::IsVistaOrLater();
}

BOOL EnablePrivilege( LPCTSTR PrivilegeName )
{
    return DevToolkit::CSystem::EnablePrivilege( PrivilegeName );
}
