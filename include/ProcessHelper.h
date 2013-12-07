#pragma once
#include <Windows.h>
#include "DTKKernel.h"

namespace DevToolkit
{
    enum INTEGRITYLEVEL
    {
        INTEGRITY_UNKNOWN,
        LOW_INTEGRITY,
        MEDIUM_INTEGRITY,
        HIGH_INTEGRITY,
    };
    
    class EXPORTS_CLASS CProcessHelper
    {
    public:
        CProcessHelper( );
        virtual ~CProcessHelper( void );
        
    public:
        // 以当前登录用户权限打开进程
        BOOL Open( DWORD dwID );
        
        // 以SE_DEBUG权限打开进程
        BOOL OpenWithPrivileged( DWORD dwID );
        
        // 以指定的权限打开进程
        BOOL OpenWithAccess( DWORD dwID, DWORD access_flags );
        
        // 释放资源
        void Close();
        
        // 结束进程
        BOOL Terminal();
        
        // 从进程读取内存
        BOOL ReadMemory( LPCVOID lpAddr, LPVOID lpBuf, DWORD dwReadSize );
        
        // 向进程写入数据
        BOOL WriteMemory( LPVOID lpAddr, LPCVOID lpBuf, DWORD dwWriteSize );
        
        // 返回进程PID
        DWORD GetPID()const;
        
        // 返回进程句柄
        HANDLE GetHandle();
        
        
        DWORD GetThreadCount();
        
        // 获取父进程PID
        DWORD GetParentPID();
        
        // 获取进程PEB基址
        DWORD GetPEBBase();
        
        // 获取当前进程的完整路径
        LPCTSTR GetFullPathName();
        
        // 获取当前进程在进程管理器中的短名称
        LPCTSTR GetName();
        
        // 获取进程完整性级别
        BOOL GetIntegrityLevel( INTEGRITYLEVEL* pLevel );
        
    protected:
        // 获取进程基本信息
        BOOL GetBaseInformation( LPPROCESS_BASIC_INFORMATION lpBaseInfo );
    private:
        // 进程PID
        DWORD m_dwPID;
        
        // 进程句柄
        HANDLE m_hProcess;
        DWORD m_dwThreadCount;
        DWORD m_dwParentProcessID;
        DWORD m_dwPriClassBase;
        
        // 进程全路径
        TCHAR m_sFullPathName[MAX_PATH];
        
        // 进程短名称
        TCHAR m_sName[MAX_PATH];
    };
}

