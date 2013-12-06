#include "stdafx.h"
#include "SharedMemory.h"
#include <assert.h>
#include <limits>
#include <iostream>
using namespace DevToolkit;

// Returns the length of the memory section starting at the supplied address.
size_t GetMemorySectionSize( void* address )
{
    MEMORY_BASIC_INFORMATION memory_info;
    if ( !::VirtualQuery( address, &memory_info, sizeof( memory_info ) ) )
        return 0;
    return memory_info.RegionSize - ( static_cast<char*>( address ) -
                                      static_cast<char*>( memory_info.AllocationBase ) );
}

CSharedMemory::CSharedMemory( void )
    : m_bReadOnly( FALSE ), m_dwMapedSize( 0 ), m_hFileMap( NULL ), m_hShareMemLock( NULL ), m_lpMemory( NULL )
{
    memset( m_sName, 0, MAX_PATH );
}


CSharedMemory::~CSharedMemory( void )
{
    Close();
    if ( m_hShareMemLock != NULL )
    {
        CloseHandle( m_hShareMemLock );
        m_hShareMemLock = NULL;
    }
}

BOOL CSharedMemory::Create( const SHAREDMEMORYCREATEOPTIONS& options )
{
    assert( !options.bExcutable );
    assert( !m_lpMemory );
    
    static const size_t kSectionMask = 65536 - 1;
    if ( options.dwSize <= 0 )
        return FALSE;
        
    if ( options.dwSize > static_cast<size_t>( INT_MAX - kSectionMask ) )
        return false;
        
    _tcscpy_s( m_sName, MAX_PATH, options.sName );
    size_t rounded_size = ( options.dwSize + kSectionMask ) & ~kSectionMask;
    m_hFileMap = CreateFileMapping( INVALID_HANDLE_VALUE, NULL,
                                    PAGE_READWRITE, 0, static_cast<DWORD>( rounded_size ),
                                    m_sName );
    if ( NULL == m_hFileMap )
        return FALSE;
        
    if ( GetLastError() == ERROR_ALREADY_EXISTS )
    {
        if ( !options.bOpenExisting )
        {
            Close();
            return FALSE;
        }
    }
    return TRUE;
}

BOOL CSharedMemory::Open( LPCTSTR lpszName, BOOL bReadOnly )
{
    assert( !m_lpMemory );
    
    _tcscpy_s( m_sName, MAX_PATH, lpszName );
    m_bReadOnly = bReadOnly;
    m_hFileMap = OpenFileMapping( m_bReadOnly ? FILE_MAP_READ : FILE_MAP_READ | FILE_MAP_WRITE,
                                  FALSE, m_sName );
    if ( m_hFileMap != NULL )
    {
        return TRUE;
    }
    return FALSE;
}

void CSharedMemory::Close()
{
    if ( m_lpMemory != NULL )
    {
        UnmapViewOfFile( m_lpMemory );
        m_lpMemory = NULL;
    }
    
    if ( m_hFileMap != NULL )
    {
        CloseHandle( m_hFileMap );
        m_hFileMap = NULL;
    }
}

void CSharedMemory::Lock()
{
    Lock( INFINITE, NULL );
}

BOOL CSharedMemory::Lock( DWORD dwTimeout, LPSECURITY_ATTRIBUTES lpAttributes )
{
    if ( m_hShareMemLock == NULL )
    {
        _tcscat_s( m_sName, MAX_PATH, _T( "Lock" ) );
        m_hShareMemLock = CreateMutex( lpAttributes, FALSE, m_sName );
        if ( m_hShareMemLock == NULL )
        {
            return FALSE;
        }
    }
    DWORD result = WaitForSingleObject( m_hShareMemLock, dwTimeout );
    
    // Return false for WAIT_ABANDONED, WAIT_TIMEOUT or WAIT_FAILED.
    return ( result == WAIT_OBJECT_0 );
}

void CSharedMemory::UnLock()
{
    assert( NULL != m_hShareMemLock );
    ReleaseMutex( m_hShareMemLock );
    m_hShareMemLock = NULL;
}

BOOL CSharedMemory::MapAt( DWORD dwOffset, DWORD dwSize )
{
    if ( m_hFileMap == NULL )
        return FALSE;
        
    if ( dwSize > static_cast<size_t>( INT_MAX ) )
        return FALSE;
        
    m_lpMemory = MapViewOfFile( m_hFileMap,
                                m_bReadOnly ? FILE_MAP_READ : FILE_MAP_READ |
                                FILE_MAP_WRITE,
                                static_cast<UINT64>( dwOffset ) >> 32,
                                static_cast<DWORD>( dwOffset ),
                                dwSize );
    if ( m_lpMemory != NULL )
    {
        m_dwMapedSize = GetMemorySectionSize( m_lpMemory );
        return TRUE;
    }
    return FALSE;
}

BOOL CSharedMemory::UnMap()
{
    if ( m_lpMemory == NULL )
        return FALSE;
        
    UnmapViewOfFile( m_lpMemory );
    m_lpMemory = NULL;
    return TRUE;
}

HANDLE CSharedMemory::GetHandle() const
{
    return m_hFileMap;
}

BOOL CSharedMemory::IsHandleValid() const
{
    return ( NULL != m_hFileMap );
}
