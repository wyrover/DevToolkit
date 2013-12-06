#include "stdafx.h"
#include "DTKIniReader.h"
#include <Shlwapi.h>
#include <assert.h>
using namespace DevToolkit;

CIniReader::CIniReader( LPCTSTR lpszIniPath )
{
    assert( NULL != lpszIniPath );
    memcpy_s( m_IniPath, MAX_PATH, lpszIniPath, _tcslen( lpszIniPath )*sizeof( TCHAR ) );
    ZeroMemory( m_IniPath, MAX_PATH );
    ZeroMemory( m_result, MAX_PATH );
}

CIniReader::~CIniReader( void )
{
}

BOOL CIniReader::WriteDword( LPCTSTR Section, LPCTSTR key, DWORD Value )
{
    _stprintf_s( m_result, MAX_PATH, _T( "%u" ), Value );
    return WritePrivateProfileString( Section, key, m_result, m_IniPath );
}

BOOL CIniReader::WriteDouble( LPCTSTR Section, LPCTSTR key, double Value )
{
    _stprintf_s( m_result, MAX_PATH, _T( "%f" ), Value );
    return WritePrivateProfileString( Section, key, m_result, m_IniPath );
}

BOOL CIniReader::WriteString( LPCTSTR Section, LPCTSTR key, LPCTSTR Value )
{
    return WritePrivateProfileString( Section, key, Value, m_IniPath );
}

DWORD CIniReader::ReadDword( LPCTSTR Section, LPCTSTR key ) const
{
    return GetPrivateProfileInt( Section, key, 0, m_IniPath );
}

DOUBLE CIniReader::ReadDouble( LPCTSTR Section, LPCTSTR key )
{
    ReadString( Section, key, m_result );
    return _tstof( m_result );
}

DWORD CIniReader::ReadString( LPCTSTR Section, LPCTSTR key, TCHAR* Buf ) const
{
    DWORD dwReaded = GetPrivateProfileString( Section, key, 0, Buf, MAX_PATH, m_IniPath );
    return dwReaded + 1;	// +terminating null character
}

LPCTSTR CIniReader::GetIniFilePath() const
{
    return m_IniPath;
}
