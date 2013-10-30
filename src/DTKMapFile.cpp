#include "stdafx.h"
#include "DTKMapFile.h"
#include <Shlwapi.h>
#pragma comment(lib,"Shlwapi.lib")
using namespace DevToolkit;

CMapFile::CMapFile()
{
    m_hFile = INVALID_HANDLE_VALUE;
    m_hFileMap = NULL;
    m_pMapData = NULL;
}

CMapFile::~CMapFile()
{
    Close();
}

BOOL CMapFile::Open( LPCTSTR lpszFile )
{
    if ( !PathFileExists( lpszFile ) )
    {
        return FALSE;
    }
    if ( INVALID_HANDLE_VALUE == m_hFile )
    {
        m_hFile = CreateFile( lpszFile, GENERIC_READ | GENERIC_WRITE, 0, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL , NULL );
        if ( INVALID_HANDLE_VALUE == m_hFile )
        {
            return FALSE;
        }
        if ( INVALID_HANDLE_VALUE == m_hFileMap )
        {
            m_hFileMap = CreateFileMapping( m_hFile, NULL, PAGE_READWRITE, 0, 0, NULL );
            if ( NULL == m_hFileMap )
            {
                CloseFileHandle();
                return FALSE;
            }
            if ( NULL == m_pMapData )
            {
                m_pMapData = MapViewOfFile( m_hFileMap, FILE_MAP_WRITE | FILE_MAP_READ, 0, 0, 0 );
                if ( NULL == m_pMapData )
                {
                    CloseMapFileHandle();
                    CloseFileHandle();
                    return FALSE;
                }
                return TRUE;
            }
        }
    }
    return FALSE;
}

LPVOID CMapFile::GetData() const
{
    return m_pMapData;
}

void CMapFile::Close()
{
    if ( NULL != m_pMapData )
    {
        UnmapViewOfFile( m_pMapData );
        m_pMapData = NULL;
    }
    
    CloseMapFileHandle();
    CloseFileHandle();
}

void CMapFile::CloseFileHandle()
{
    if ( INVALID_HANDLE_VALUE != m_hFile )
    {
        CloseHandle( m_hFile );
        m_hFile = INVALID_HANDLE_VALUE;
    }
}

void CMapFile::CloseMapFileHandle()
{
    if ( NULL != m_hFileMap )
    {
        CloseHandle( m_hFileMap );
        m_hFileMap = NULL;
    }
}
