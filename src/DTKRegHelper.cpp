#include "stdafx.h"
#include "DTKRegHelper.h"
#include <Shlwapi.h>
#pragma comment(lib,"Shlwapi.lib")
namespace DevToolkit
{

    CRegHelper::CRegHelper( void )
    {
    }
    
    
    CRegHelper::~CRegHelper( void )
    {
    }
    
    /**
      @param hkey A handle to the currently open key
      @param pszSubKey the name of the subkey
      @param pszValue The address of the value
      @param bValue the bool value
      @return TURE or FALSE
    */
    BOOL CRegHelper::GetBool( HKEY hkey, LPCTSTR pszSubKey, LPCTSTR pszValue, BOOL& bValue )
    {
        bValue = FALSE;
        DWORD dwValue = 0;
        DWORD dwType = REG_DWORD;
        DWORD dwSize = sizeof( DWORD );
        LSTATUS lRet =::SHGetValue( hkey, pszSubKey, pszValue, &dwType, &dwValue, &dwSize );
        
        if ( ERROR_SUCCESS == lRet )
        {
            bValue = ( BOOL )dwValue;
            return TRUE;
        }
        
        return FALSE;
    }
    
    /** 设置BOOL值
      @param hkey A handle to the currently open key
      @param pszSubKey the name of the subkey
      @param pszValue The address of the value
      @param bValue the bool value
      @return TURE or FALSE
    */
    BOOL CRegHelper::SetBool( HKEY hkey, LPCTSTR pszSubKey, LPCTSTR pszValue, BOOL bValue )
    {
        LSTATUS lRet =::SHSetValue( hkey, pszSubKey, pszValue, REG_DWORD, &bValue, sizeof( BOOL ) );
        return ( ERROR_SUCCESS == lRet );
    }
    
    BOOL CRegHelper::GetDword( HKEY hkey, LPCTSTR pszSubKey, LPCTSTR pszValue, DWORD& dwValue )
    {
        dwValue = 0;
        DWORD dwType = REG_DWORD;
        DWORD dwSize = sizeof( DWORD );
        LSTATUS lRet =::SHGetValue( hkey, pszSubKey, pszValue, &dwType, &dwValue, &dwSize );
        return ( ERROR_SUCCESS == lRet );
    }
    
    BOOL CRegHelper::SetDword( HKEY hkey, LPCTSTR pszSubKey, LPCTSTR pszValue, DWORD dwValue )
    {
        LSTATUS lRet =::SHSetValue( hkey, pszSubKey, pszValue, REG_DWORD, &dwValue, sizeof( DWORD ) );
        return ( ERROR_SUCCESS == lRet );
    }
    
    BOOL CRegHelper::GetString( HKEY hkey, LPCTSTR pszSubKey, LPCTSTR pszValue, LPTSTR lpszValue, DWORD dwSize )
    {
        DWORD dwType = REG_SZ;
        LSTATUS lRet =::SHGetValue( hkey, pszSubKey, pszValue, &dwType, lpszValue, &dwSize );
        return ( ERROR_SUCCESS == lRet );
    }
    
    BOOL CRegHelper::SetString( HKEY hkey, LPCTSTR pszSubKey, LPCTSTR pszValue, LPCTSTR lpszValue )
    {
        DWORD dwType = REG_SZ;
        DWORD dwSize = _tcslen( lpszValue ) * sizeof( TCHAR );
        LSTATUS lRet =::SHSetValue( hkey, pszSubKey, pszValue, dwType, ( LPCVOID )lpszValue, dwSize );
        return ( ERROR_SUCCESS == lRet );
    }
    
    /** 删除某个键值,如果键值为NULL,则删除整个项
      @return TURE or FALSE
    */
    BOOL CRegHelper::DelKeyValue( HKEY hkey, LPCTSTR pszSubKey, LPCTSTR pszValue )
    {
        LSTATUS lRet = -1;
        
        if ( NULL == pszValue )
        {
            lRet =::SHDeleteKey( hkey, pszSubKey );
        }
        else
            lRet =::SHDeleteValue( hkey, pszSubKey, pszValue );
            
        return ( ERROR_SUCCESS == lRet );
    }
    
    BOOL CRegHelper::IsKeyExist( HKEY hkey, LPCTSTR pszSubKey )
    {
        HKEY hRetKey = NULL;
        LSTATUS lRet =::RegOpenKey( hkey, pszSubKey, &hRetKey );
        
        if ( ERROR_SUCCESS == lRet )
        {
            ::RegCloseKey( hRetKey );
            return TRUE;
        }
        
        return FALSE;
    }
    
}