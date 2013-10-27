#pragma once

#ifndef DEVTOOLKIT_EXPORTS
#define EXPORTS_FUNC1 extern "C" __declspec(dllimport)
#else
#define EXPORTS_FUNC1 extern "C" __declspec(dllexport)
#endif

///////////////////////////////////系统相关函数///////////////////////////////////////
EXPORTS_FUNC1 BOOL IsAdmin();

EXPORTS_FUNC1 LPCTSTR GetLastErrMsg();

EXPORTS_FUNC1 LPCTSTR FormatErrMsg( const DWORD dwErrID );

EXPORTS_FUNC1 BOOL runAsAdmin( LPCTSTR lpExeFile );

EXPORTS_FUNC1 BOOL IsVistaOrLater();

EXPORTS_FUNC1 BOOL EnablePrivilege( LPCTSTR PrivilegeName );

///////////////////////////////////内存相关函数///////////////////////////////////////
