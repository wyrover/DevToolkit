#pragma once
#include <Windows.h>

namespace DevToolkit
{
	enum INTEGRITYLEVEL {
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
        BOOL Open(DWORD dwID);

		// 以SE_DEBUG权限打开进程
		BOOL OpenWithPrivileged(DWORD dwID);
		BOOL OpenWithAccess(DWORD dwID,DWORD access_flags );
        void Close();
        BOOL Terminal();
        BOOL ReadMemory( LPCVOID lpAddr, LPVOID lpBuf, DWORD dwReadSize );
        BOOL WriteMemory( LPVOID lpAddr, LPCVOID lpBuf, DWORD dwWriteSize );
        
        DWORD GetPID()const;
        HANDLE GetHandle();
        DWORD GetThreadCount();
        DWORD GetParentPID();
        DWORD GetPriClassBase();

		// 获取当前进程的完整路径
        LPCTSTR GetFullPathName();

		// 获取当前进程在进程管理器中的短名称
		LPCTSTR GetName();
        BOOL GetIntegrityLevel(INTEGRITYLEVEL* pLevel);
    private:
        DWORD m_dwPID;
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

