#pragma once
#include <Windows.h>

namespace DevToolkit
{
    class CRegHelper
    {
    public:
        CRegHelper( void );
        virtual ~CRegHelper( void );

        static BOOL GetBool( HKEY hkey, LPCTSTR pszSubKey, LPCTSTR pszValue, BOOL& bValue );
        static BOOL SetBool( HKEY hkey, LPCTSTR pszSubKey, LPCTSTR pszValue, BOOL bValue );

        static BOOL GetDword( HKEY hkey, LPCTSTR pszSubKey, LPCTSTR pszValue, DWORD& dwValue );
        static BOOL SetDword( HKEY hkey, LPCTSTR pszSubKey, LPCTSTR pszValue, DWORD dwValue );

        static BOOL GetString( HKEY hkey, LPCTSTR pszSubKey, LPCTSTR pszValue, LPTSTR lpszValue, DWORD dwSize );
        static BOOL SetString( HKEY hkey, LPCTSTR pszSubKey, LPCTSTR pszValue, LPCTSTR lpszValue );

        static BOOL DelKeyValue( HKEY hkey, LPCTSTR pszSubKey, LPCTSTR pszValue );

        static BOOL IsKeyExist( HKEY hkey, LPCTSTR pszSubKey );
    };
}


