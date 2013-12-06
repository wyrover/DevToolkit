#include "stdafx.h"
#include "DTKKernel.h"
#include <tlhelp32.h>
#include "mhook.h"

BOOL IATHook( IN HMODULE hModule,IN LPCTSTR pImageName,IN LPCVOID pTargetFuncAddr,IN LPCVOID pReplaceFuncAddr )
{
	IMAGE_DOS_HEADER* pImgDosHdr = ( IMAGE_DOS_HEADER* )hModule;
	IMAGE_OPTIONAL_HEADER* pImgOptHdr = ( IMAGE_OPTIONAL_HEADER* )( ( DWORD )hModule + pImgDosHdr->e_lfanew + 24 );
	IMAGE_IMPORT_DESCRIPTOR* pImgImportDes = ( IMAGE_IMPORT_DESCRIPTOR* )( ( DWORD )hModule + pImgOptHdr->DataDirectory[IMAGE_DIRECTORY_ENTRY_IMPORT].VirtualAddress );
	IMAGE_THUNK_DATA* pImgThunkData = NULL;
	CString TargetLibraryName;
	DWORD Value = 0;
	DWORD OldProtect = 0;
	DWORD NewProtect = 0;
	LPDWORD FuncAddress = NULL;

	while ( pImgImportDes->Characteristics  != 0 )
	{
		USES_CONVERSION;
		LPCTSTR lpszName = A2W( ( ( LPCSTR )( DWORD )hModule + pImgImportDes->Name ) );
		TargetLibraryName = lpszName;
		if ( TargetLibraryName.CompareNoCase( pImageName ) == 0 )
		{
			pImgThunkData = ( IMAGE_THUNK_DATA* )( ( DWORD )hModule + pImgImportDes->FirstThunk );
			break;
		}
		pImgImportDes++;
	}
	if ( pImgThunkData == NULL )
	{
		return FALSE;
	}
	while ( pImgThunkData->u1.Function )
	{
		//循环查找目标函数地址所在的位置
		FuncAddress = ( LPDWORD ) & ( pImgThunkData->u1.Function );
		if ( *FuncAddress == ( DWORD )pTargetFuncAddr )
		{
			//找到目标函数的地址，然后修改为钩子函数的地址
			VirtualProtect( FuncAddress, sizeof( DWORD ), PAGE_READWRITE, &OldProtect );
			if ( !WriteProcessMemory( ( HANDLE ) - 1, FuncAddress, &pReplaceFuncAddr, 4, NULL ) )
			{
				return FALSE;
			}
			VirtualProtect( FuncAddress, sizeof( DWORD ), OldProtect, 0 );
			return TRUE;
		}
		pImgThunkData++;
	}
	return FALSE;
}

BOOL EnablePrivilege( LPCTSTR PrivilegeName )
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


BOOL RemoteInjectDll( DWORD dwPID, LPCTSTR lpszDllName )
{
	if ( !PathFileExists( lpszDllName ) )
	{
		return FALSE;
	}

	if ( !EnablePrivilege( SE_DEBUG_NAME ) )
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


BOOL RemoteUnInjectDll( DWORD dwPID, LPCTSTR lpszDllName )
{
	if ( !EnablePrivilege( SE_DEBUG_NAME ) )
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

EXPORTS_FUNC BOOL HookAPI( PVOID *ppSystemFunction, PVOID pHookFunction )
{
	return Mhook_SetHook(ppSystemFunction,pHookFunction);
}

EXPORTS_FUNC BOOL UnHookAPI( PVOID *ppHookedFunction )
{
	return Mhook_Unhook(ppHookedFunction);
}
