#pragma once

namespace DevToolkit
{
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
    
    /**
    * \brief 内联APIHook
    * \param ppSystemFunction 要HOOK的API地址
    * \param pHookFunction 自定义的HOOK函数地址，成功后调用原始API时会先调用该函数
    * \return HOOK成功返回TRUE,失败返回FALSE
    */
    EXPORTS_FUNC BOOL HookAPI( PVOID *ppSystemFunction, PVOID pHookFunction );
    
    /**
    * \brief 卸载内联APIHook
    * \param ppHookedFunction 要卸载的API地址
    * \return 卸载成功返回TRUE,失败返回FALSE
    */
    EXPORTS_FUNC BOOL UnHookAPI( PVOID *ppHookedFunction );
    
    /**
    * \brief 返回一个地址所在的DLL的句柄，这个DLL必须已经被调用进程加载到内存中
    * \param address
    * \return 返回address这个地址所在的DLL的句柄
    */
    HMODULE GetModuleFromAddress( LPCVOID address );
    
    /**
    * \brief RVA转换为指针
    * \param pbImage
    * \param dwRva
    * \return
    */
    PBYTE RvaToPointer( PBYTE pbImage, DWORD dwRva );
    
    /**
    * \brief 文件偏移转换为RVA
    * \param pbImage
    * \param dwOffset
    * \return
    */
    DWORD OffsetToRva( PBYTE pbImage, DWORD dwOffset );

	/**
    * \brief 模拟实现GetProcAddress函数
    * \param phModule 要获取函数的模块句柄
    * \param pProcName 要获取的函数的名称
    * \return 要获取的函数在phModule中的地址
    */
    DWORD GetFunctionAddress( HMODULE phModule, TCHAR* pProcName );
    
}