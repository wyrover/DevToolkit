#include "stdafx.h"
#include "ProcessHelper.h"
#include "DTKKernel.h"
#include <Psapi.h>
#include <tchar.h>
#pragma comment(lib,"Psapi.lib")

namespace DevToolkit
{

    CProcessHelper::CProcessHelper()
    {
        m_hProcess = NULL;
        m_hProcess = NULL;
        m_dwThreadCount = 0;
        m_dwParentProcessID = -1;
        m_dwPriClassBase = -1;
        m_dwPID = 0;
    }
    
    
    CProcessHelper::~CProcessHelper( void )
    {
        Close();
    }
    
    BOOL CProcessHelper::Open( DWORD dwID )
    {
        return OpenWithAccess( dwID, PROCESS_ALL_ACCESS );
    }
    
    BOOL DevToolkit::CProcessHelper::OpenWithPrivileged( DWORD dwID )
    {
        if ( !EnablePrivilege( SE_DEBUG_NAME ) )
            return FALSE;
        return Open( dwID );
    }
    
    BOOL DevToolkit::CProcessHelper::OpenWithAccess( DWORD dwID, DWORD access_flags )
    {
        if ( NULL == m_hProcess )
        {
            m_hProcess =::OpenProcess( access_flags, FALSE, dwID );
            if ( NULL != m_hProcess )
                m_dwPID = dwID;
            return ( NULL != m_hProcess );
        }
        return FALSE;
    }
    
    void CProcessHelper::Close()
    {
        if ( NULL != m_hProcess )
        {
            ::CloseHandle( m_hProcess );
            m_hProcess = NULL;
        }
    }
    
    BOOL CProcessHelper::Terminal()
    {
        BOOL bRet = FALSE;
        
        if ( NULL != m_hProcess )
        {
            bRet =::TerminateProcess( m_hProcess, 0 );
        }
        
        return bRet;
    }
    
    BOOL CProcessHelper::ReadMemory( LPCVOID lpAddr, LPVOID lpBuf, DWORD dwReadSize )
    {
        BOOL bRet = FALSE;
        DWORD dwReaded = 0;
        
        if ( NULL != m_hProcess )
        {
            bRet =::ReadProcessMemory( m_hProcess, lpAddr, lpBuf, dwReadSize, &dwReaded );
        }
        
        return ( ( TRUE == bRet ) && ( dwReadSize == dwReaded ) );
    }
    
    BOOL CProcessHelper::WriteMemory( LPVOID lpAddr, LPCVOID lpBuf, DWORD dwWriteSize )
    {
        BOOL bRet = FALSE;
        DWORD dwWrited = 0;
        
        if ( NULL != m_hProcess )
        {
            bRet =::WriteProcessMemory( m_hProcess, lpAddr, lpBuf, dwWriteSize, &dwWrited );
        }
        
        return ( ( TRUE == bRet ) && ( dwWrited == dwWriteSize ) );
    }
    
    DWORD CProcessHelper::GetPID()const
    {
        return m_dwPID;
    }
    
    HANDLE CProcessHelper::GetHandle()
    {
        return m_hProcess;
    }
    
    DWORD CProcessHelper::GetThreadCount()
    {
        return m_dwThreadCount;
    }
    
    DWORD CProcessHelper::GetParentPID()
    {
        return m_dwParentProcessID;
    }
    
    DWORD CProcessHelper::GetPriClassBase()
    {
        return m_dwPriClassBase;
    }
    
    LPCTSTR CProcessHelper::GetFullPathName()
    {
        ZeroMemory( m_sFullPathName, MAX_PATH );
        if ( 0 != GetModuleFileNameEx( m_hProcess, NULL, m_sFullPathName, MAX_PATH ) )
        {
            return m_sFullPathName;
        }
        return NULL;
    }
    
    LPCTSTR CProcessHelper::GetName()
    {
        ZeroMemory( m_sName, MAX_PATH );
        if ( 0 != GetModuleBaseName( m_hProcess, NULL, m_sName, MAX_PATH ) )
            return m_sName;
        return NULL;
    }
    
    BOOL DevToolkit::CProcessHelper::GetIntegrityLevel( INTEGRITYLEVEL* pLevel )
    {
        if ( !pLevel )
            return FALSE;
            
        HANDLE process_token;
        if ( !OpenProcessToken( m_hProcess, TOKEN_QUERY | TOKEN_QUERY_SOURCE,
                                &process_token ) )
            return FALSE;
            
        DWORD token_info_length = 0;
        if ( GetTokenInformation( process_token, TokenIntegrityLevel, NULL, 0,
                                  &token_info_length ) ||
                GetLastError() != ERROR_INSUFFICIENT_BUFFER )
            return FALSE;
            
        char* tokenInfo = new char[token_info_length];
        
        TOKEN_MANDATORY_LABEL* token_label =
            reinterpret_cast<TOKEN_MANDATORY_LABEL*>( tokenInfo );
        if ( !token_label )
            return FALSE;
            
        if ( !GetTokenInformation( process_token, TokenIntegrityLevel, token_label,
                                   token_info_length, &token_info_length ) )
            return FALSE;
            
        DWORD integrity_level = *GetSidSubAuthority( token_label->Label.Sid,
                                ( DWORD )( UCHAR )( *GetSidSubAuthorityCount( token_label->Label.Sid ) - 1 ) );
                                
        if ( integrity_level < SECURITY_MANDATORY_MEDIUM_RID )
        {
            *pLevel = LOW_INTEGRITY;
        }
        else if ( integrity_level >= SECURITY_MANDATORY_MEDIUM_RID &&
                  integrity_level < SECURITY_MANDATORY_HIGH_RID )
        {
            *pLevel = MEDIUM_INTEGRITY;
        }
        else if ( integrity_level >= SECURITY_MANDATORY_HIGH_RID )
        {
            *pLevel = HIGH_INTEGRITY;
        }
        else
        {
            return FALSE;
        }
        
        delete[] tokenInfo;
        tokenInfo = NULL;
        
        return TRUE;
    }
    
}
