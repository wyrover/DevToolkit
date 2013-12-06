#pragma once
// 共享内存（内存文件映射）类

namespace DevToolkit
{
    typedef struct SharedMemoryCreateOptions
    {
        SharedMemoryCreateOptions(): dwSize( 0 ), bOpenExisting( FALSE ), bExcutable( FALSE ) {}
        CString sName;
        DWORD dwSize;
        BOOL bOpenExisting;
        BOOL bExcutable;
    } SHAREDMEMORYCREATEOPTIONS, *PSHAREDMEMORYCREATEOPTIONS;
    
    class EXPORTS_CLASS CSharedMemory
    {
    public:
        explicit CSharedMemory( void );
        virtual ~CSharedMemory( void );
        
        BOOL Create( const SHAREDMEMORYCREATEOPTIONS& options );
        BOOL Open( LPCTSTR lpszName, BOOL bReadOnly );
        BOOL MapAt( DWORD dwOffset, DWORD dwSize );
        BOOL UnMap();
        void Close();
        void Lock();
        BOOL Lock( DWORD dwTimeout, LPSECURITY_ATTRIBUTES lpAttributes );
        void UnLock();
        HANDLE GetHandle()const;
        BOOL IsHandleValid()const;
    private:
        HANDLE m_hFileMap;
        DWORD m_dwMapedSize;
        BOOL m_bReadOnly;
        HANDLE m_hShareMemLock;
        LPVOID m_lpMemory;
        TCHAR m_sName[MAX_PATH];
        static const int MAP_MINIMUM_ALIGNMENT = 32;
    };
}


