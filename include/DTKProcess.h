#pragma once

namespace DevToolkit
{
    class CProcess
    {
    public:
        CProcess();
        virtual ~CProcess();
        
        /**
        * \brief 远线程注入DLL
        * \param dwPID 要注入DLL的进程PID
        * \param lpszDllName 要注入的DLL的路径
        * \return 注入成功返回TRUE,否则返回FALSE
        */
        static BOOL RemoteInjectDll( DWORD dwPID, LPCTSTR lpszDllName );
        
        /**
        * \brief 远线程卸载DLL
        * \param dwPID 要卸载的进程PID
        * \param lpszDllName 要卸载的DLL的名称或者路径
        * \return 卸载成功返回TRUE,否则返回FALSE
        */
        static BOOL RemoteUnInjectDll( DWORD dwPID, LPCTSTR lpszDllName );
    };
}