#include "stdafx.h"
#include "ProcessHelper.h"


CProcessHelper::CProcessHelper ( DWORD dwID )
{
    m_dwID = dwID;
    m_hProcess = NULL;
    m_hProcess = NULL;
    m_dwThreadCount = 0;
    m_dwParentProcessID = -1;
    m_dwPriClassBase = -1;
    ZeroMemory ( m_sExeFile, MAX_PATH );
}


CProcessHelper::~CProcessHelper ( void )
{
    Close ();
}

BOOL CProcessHelper::Open ()
{
    if ( NULL == m_hProcess )
    {
        m_hProcess =::OpenProcess ( PROCESS_ALL_ACCESS, FALSE, m_dwID );
    }

    return ( NULL != m_hProcess );
}

BOOL CProcessHelper::Close ()
{
    if ( NULL != m_hProcess )
    {
        ::CloseHandle ( m_hProcess );
        m_hProcess = NULL;
    }

    return TRUE;
}

BOOL CProcessHelper::Terminal ()
{
    BOOL bRet = FALSE;

    if ( NULL != m_hProcess )
    {
        bRet =::TerminateProcess ( m_hProcess, 0 );
    }

    return bRet;
}

BOOL CProcessHelper::ReadMemory ( LPCVOID lpAddr, LPVOID lpBuf, DWORD dwReadSize )
{
    BOOL bRet = FALSE;
    DWORD dwReaded = 0;

    if ( NULL != m_hProcess )
    {
        bRet =::ReadProcessMemory ( m_hProcess, lpAddr, lpBuf, dwReadSize, &dwReaded );
    }

    return ( ( TRUE == bRet ) && ( dwReadSize == dwReaded ) );
}

BOOL CProcessHelper::WriteMemory ( LPVOID lpAddr, LPCVOID lpBuf, DWORD dwWriteSize )
{
    BOOL bRet = FALSE;
    DWORD dwWrited = 0;

    if ( NULL != m_hProcess )
    {
        bRet =::WriteProcessMemory ( m_hProcess, lpAddr, lpBuf, dwWriteSize, &dwWrited );
    }

    return ( ( TRUE == bRet ) && ( dwWrited == dwWriteSize ) );
}

DWORD CProcessHelper::GetPID ()const
{
    return m_dwID;
}

HANDLE CProcessHelper::GetHandle ()
{
    return m_hProcess;
}

DWORD CProcessHelper::GetThreadCount ()
{
    return m_dwThreadCount;
}

DWORD CProcessHelper::GetParentPID ()
{
    return m_dwParentProcessID;
}

DWORD CProcessHelper::GetPriClassBase ()
{
    return m_dwPriClassBase;
}

LPCTSTR CProcessHelper::GetFileName ()
{
    return m_sExeFile;
}

