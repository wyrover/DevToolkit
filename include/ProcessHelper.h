#pragma once
#include <Windows.h>

class EXPORTS_CLASS CProcessHelper
{
public:
    CProcessHelper(DWORD dwID);
    virtual ~CProcessHelper(void);

public:
    BOOL Open();
    BOOL Close();
    BOOL Terminal();
    BOOL ReadMemory(LPCVOID lpAddr,LPVOID lpBuf,DWORD dwReadSize);
    BOOL WriteMemory(LPVOID lpAddr,LPCVOID lpBuf,DWORD dwWriteSize);

    DWORD GetPID()const;
    HANDLE GetHandle();
    DWORD GetThreadCount();
    DWORD GetParentPID();
    DWORD GetPriClassBase();
    LPCTSTR GetFileName();

private:
    DWORD m_dwID;
    HANDLE m_hProcess;
    DWORD m_dwThreadCount;
    DWORD m_dwParentProcessID;
    DWORD m_dwPriClassBase;
    TCHAR m_sExeFile[MAX_PATH];
};

