#pragma once


/**
* \brief HOOK IAT
* \param hModule EXE程序基址
* \param pImageName EXE中导入的DLL模块名称
* \param pTargetFuncAddr 要HOOK的函数名称
* \param pReplaceFuncAddr HOOK成功后，调用的自定义函数
* \return HOOK成功返回TRUE,失败返回FALSE
*/
EXPORTS_FUNC BOOL IATHook( IN HMODULE hModule, IN LPCTSTR pImageName, IN LPCVOID pTargetFuncAddr, IN LPCVOID pReplaceFuncAddr );

/**
* \brief 提升应用程序权限为DEBUG权限
* \param PrivilegeName 要提升的权限名称 如SE_DEBUG
* \return 成功提升为DEBUG权限返回TRUE,否则返回FALSE
*/
EXPORTS_FUNC BOOL EnablePrivilege( LPCTSTR PrivilegeName );

/**
* \brief 远线程注入DLL
* \param dwPID 要注入DLL的进程PID
* \param lpszDllName 要注入的DLL的路径
* \return 注入成功返回TRUE,否则返回FALSE
*/
EXPORTS_FUNC BOOL RemoteInjectDll( DWORD dwPID, LPCTSTR lpszDllName );

/**
* \brief 远线程卸载DLL
* \param dwPID 要卸载的进程PID
* \param lpszDllName 要卸载的DLL的名称或者路径
* \return 卸载成功返回TRUE,否则返回FALSE
*/
EXPORTS_FUNC BOOL RemoteUnInjectDll( DWORD dwPID, LPCTSTR lpszDllName );

EXPORTS_FUNC BOOL HookAPI(PVOID *ppSystemFunction, PVOID pHookFunction);

EXPORTS_FUNC BOOL UnHookAPI(PVOID *ppHookedFunction);