#include "stdafx.h"
#include "DTKProcess.h"
#include <Shlwapi.h>
#include <TlHelp32.h>
#include "DTKSystem.h"
#pragma comment(lib,"Shlwapi.lib")
using namespace DevToolkit;

CProcess::CProcess()
{

}

CProcess::~CProcess()
{

}

BOOL CProcess::RemoteInjectDll( DWORD dwPID, LPCTSTR lpszDllName )
{
    if ( !PathFileExists( lpszDllName ) )
    {
        return FALSE;
    }
    
    if ( !CSystem::EnablePrivilege( SE_DEBUG_NAME ) )
    {
        return FALSE;
    }
    
    BOOL bRet = FALSE;
    HANDLE hPro =::OpenProcess( PROCESS_ALL_ACCESS, FALSE, dwPID );
    
    if ( NULL == hPro )
    {
        return FALSE;
    }
    
    LPVOID pszLibFileRemote = NULL;
    HANDLE hRemoteThread = NULL;
    
    __try
    {
        int cb  = ( _tcslen( lpszDllName ) + 1 ) * sizeof( TCHAR );
        pszLibFileRemote = VirtualAllocEx( hPro, NULL, cb, MEM_COMMIT, PAGE_READWRITE );
        
        if ( pszLibFileRemote == NULL )
        {
            return FALSE;
        }
        
        if ( !WriteProcessMemory( hPro, pszLibFileRemote, ( PVOID ) lpszDllName, cb, NULL ) )
        {
            return FALSE;
        }
        
#ifdef _UNICODE
#define Libaray ("LoadLibraryW")
#else
#define Libaray ("LoadLibraryA")
#endif
        PTHREAD_START_ROUTINE pfnThreadRtn = ( PTHREAD_START_ROUTINE )GetProcAddress( GetModuleHandle( _T( "Kernel32" ) ), Libaray );
        
        if ( pfnThreadRtn == NULL )
        {
            return FALSE;
        }
        
        hRemoteThread = ::CreateRemoteThread( hPro, NULL, NULL, pfnThreadRtn, pszLibFileRemote, 0, NULL );
        if ( hRemoteThread == NULL )
        {
            return FALSE;
        }
        WaitForSingleObject( hRemoteThread, INFINITE );
        return TRUE;
    }
    __finally
    {
        if ( hRemoteThread  != NULL )
        {
            CloseHandle( hRemoteThread );
            hRemoteThread = NULL;
        }
        
        if ( pszLibFileRemote != NULL )
        {
            VirtualFreeEx( hPro, pszLibFileRemote, 0, MEM_RELEASE );
            pszLibFileRemote = NULL;
        }
        
        if ( NULL != hPro )
        {
            ::CloseHandle( hPro );
            hPro = NULL;
        }
    }
    
    return FALSE;
}


BOOL CProcess::RemoteUnInjectDll( DWORD dwPID, LPCTSTR lpszDllName )
{
    if ( !CSystem::EnablePrivilege( SE_DEBUG_NAME ) )
    {
        return FALSE;
    }
    
    BOOL bRet = FALSE;
    HANDLE hPro =::OpenProcess( PROCESS_ALL_ACCESS, FALSE, dwPID );
    
    if ( NULL == hPro )
    {
        return FALSE;
    }
    
    HANDLE hthSnapshot = NULL;
    HANDLE hThread = NULL;
    
    __try
    {
        // Grab a new snapshot of the process
        hthSnapshot = CreateToolhelp32Snapshot( TH32CS_SNAPMODULE, dwPID );
        
        if ( hthSnapshot == NULL )
        {
            return FALSE;
        }
        
        // Get the HMODULE of the desired library
        MODULEENTRY32 me = { sizeof( me ) };
        BOOL fFound = FALSE;
        BOOL fMoreMods = Module32First( hthSnapshot, &me );
        
        for ( ; fMoreMods; fMoreMods = Module32Next( hthSnapshot, &me ) )
        {
            fFound = ( _tcscmp( me.szModule,  lpszDllName ) == 0 ) ||
                     ( _tcscmp( me.szExePath, lpszDllName ) == 0 );
                     
            if ( fFound ) break;
        }
        
        if ( !fFound )
        {
            return FALSE;
        }
        
        // Get the real address of LoadLibraryW in Kernel32.dll
        PTHREAD_START_ROUTINE pfnThreadRtn = ( PTHREAD_START_ROUTINE )GetProcAddress( GetModuleHandle( _T( "Kernel32" ) ), "FreeLibrary" );
        
        if ( pfnThreadRtn == NULL )
        {
            return FALSE;
        }
        
        // Create a remote thread that calls LoadLibraryW(DLLPathname)
        hThread = ::CreateRemoteThread( hPro, NULL, 0, pfnThreadRtn, me.modBaseAddr, 0, NULL );
        if ( hThread == NULL )
        {
            return FALSE;
        }
        
        // Wait for the remote thread to terminate
        WaitForSingleObject( hThread, INFINITE );
        return TRUE;
    }
    __finally   // Now we can clean everything up
    {
        if ( hThread != NULL )
        {
            CloseHandle( hThread );
            hThread = NULL;
        }
        
        if ( hthSnapshot != NULL )
        {
            CloseHandle( hthSnapshot );
            hThread = NULL;
        }
        
        if ( NULL != hPro )
        {
            ::CloseHandle( hPro );
            hPro = NULL;
        }
    }
    
    return FALSE;
}
