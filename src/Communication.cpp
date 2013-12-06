#include "StdAfx.h"
#include <Dbt.h>
#include "Communication.h"
const DWORD dwInQueue = 4096;
const DWORD dwOutQueue = 4096;
const DWORD EvtMask = EV_RXCHAR;
HANDLE CSerial::m_hFile = NULL;
CRITICAL_SECTION CSerial::m_Cs;


//默认构造函数
CSerial::CSerial( void )
{
    m_hPort = INVALID_HANDLE_VALUE;
    m_pMainWnd = NULL;
    m_WantRead = 0;
    //	m_bOpened=FALSE;
    m_pReadThread = NULL;

    m_ovWait.hEvent = NULL;
    m_ovRead.hEvent = NULL;
    m_ovWrite.hEvent = NULL;

    ZeroMemory( &m_ovWait, sizeof( m_ovWait ) );
    ZeroMemory( &m_ovRead, sizeof( m_ovRead ) );
    ZeroMemory( &m_ovWrite, sizeof( m_ovWrite ) );
    m_ovRead.hEvent = CreateEvent( NULL, FALSE, FALSE, NULL );
    m_ovWrite.hEvent = CreateEvent( NULL, FALSE, FALSE, NULL );
    m_ovWait.hEvent = CreateEvent( NULL, TRUE, FALSE, 0 );
    m_bThreadAlive = FALSE;
    ZeroMemory( m_RecvedBuf, 2048 );
    m_CmdID = 0;
}

//默认析构函数
CSerial::~CSerial( void )
{
    ClosePort();
    DeleteCriticalSection( &m_Cs );
}

//串口是否已经打开
BOOL CSerial::PortIsOpen()
{
    return m_hPort != INVALID_HANDLE_VALUE;
}

//设置回调函数,当接收到串口事件时由CSerial调用,通知主程序窗口处理消息
void CSerial::SetCallBackFunc( LPVOID CallBackFunc, LPVOID pParam )
{
    //	ThreadParam* param=(ThreadParam*)pParam;
    m_CallBackFunc = ( ReadFunc )CallBackFunc;
    m_pWnd = pParam;
}

HANDLE CSerial::GetPortHandle()
{
    return m_hPort;
}

//打开串口
BOOL CSerial::OpenPort( LPTSTR portnum, DWORD dwBaudRate, BYTE byParity, BYTE byStopBits, BYTE byByteSize, HWND hWnd )
{
    m_pMainWnd = pWnd;

    if ( PortIsOpen() )
    {
        ClosePort();
    }

    CString strport;
    strport.Format( _T( "\\\\.\\%s" ), portnum );
    m_hPort = CreateFile( strport, GENERIC_WRITE | GENERIC_READ,
                          0, NULL, OPEN_EXISTING,
                          FILE_ATTRIBUTE_NORMAL | FILE_FLAG_OVERLAPPED, NULL );

    if ( INVALID_HANDLE_VALUE == m_hPort )
    {
        TRACE( _T( "CreateFile Failed" ) );
        return FALSE;
    }

    //设置串口输入输出缓冲区
    if ( !SetupComm( m_hPort, dwInQueue, dwOutQueue ) )
    {
        TRACE( _T( "SetupComm Failed" ) );
        return FALSE;
    }

    //取得串口属性
    DCB dcb;

    if ( !GetCommState( m_hPort, &dcb ) )
    {
        TRACE( _T( "GetCommState Failed" ) );
        return FALSE;
    }

    //设置串口属性
    dcb.BaudRate = dwBaudRate;	//设置波特率

    if ( byParity == 0 )			//设置奇偶校验位
    {
        dcb.Parity = NOPARITY;
    }
    else if ( byParity == 1 )
    {
        dcb.Parity = ODDPARITY;
    }
    else if ( byParity == 2 )
    {
        dcb.Parity = EVENPARITY;
    }
    else if ( byParity == 3 )
    {
        dcb.Parity = MARKPARITY;
    }
    else if ( byParity == 4 )
    {
        dcb.Parity = SPACEPARITY;
    }
    else
        dcb.Parity = NOPARITY;

    dcb.ByteSize = byByteSize;		//设置数据位

    if ( byStopBits == 1 )			//设置停止位
        dcb.StopBits	= ONESTOPBIT;
    else if ( byStopBits == 2 )
        dcb.StopBits	= TWOSTOPBITS;
    else
        dcb.StopBits	= ONE5STOPBITS;

    //流控
    // 	dcb.fDsrSensitivity = 0;
    // 	dcb.fDtrControl = DTR_CONTROL_ENABLE;
    // 	dcb.fOutxDsrFlow = 0;
    if ( !SetCommState( m_hPort, &dcb ) )
    {
        TRACE( _T( "SetCommState Failed" ) );
        return FALSE;
    }

    //设置超时
    COMMTIMEOUTS timeouts;
    //设定读超时
    timeouts.ReadIntervalTimeout = MAXWORD;
    timeouts.ReadTotalTimeoutMultiplier = 0;
    timeouts.ReadTotalTimeoutConstant = 0;
    //在读一次输入缓冲区的内容后读操作就立即返回，
    //而不管是否读入了要求的字符。


    //设定写超时
    timeouts.WriteTotalTimeoutMultiplier = 0;
    timeouts.WriteTotalTimeoutConstant = 0;

    if ( !SetCommTimeouts( m_hPort, &timeouts ) )
    {
        TRACE( _T( "SetCommTimeouts Failed" ) );
        return FALSE;
    }

    //设置串口事件
    if ( !SetCommMask( m_hPort, EvtMask ) )
    {
        TRACE( _T( "SetCommMask Failed" ) );
        return FALSE;
    }

    if ( !PurgeComm( m_hPort, PURGE_RXCLEAR | PURGE_TXCLEAR | PURGE_RXABORT | PURGE_TXABORT ) )
    {
        TRACE( _T( "PurgeComm Failed" ) );
        return FALSE;
    }

    return StartMonitor();
}

//开启监视线程
BOOL CSerial::StartMonitor()
{
    if ( NULL == m_pReadThread )
        m_pReadThread = AfxBeginThread( AFX_THREADPROC( ReadThread ), this, THREAD_PRIORITY_NORMAL, 0, 0, 0 );

    if ( NULL == m_pReadThread )
    {
        return FALSE;
    }
    else
        return TRUE;
}


//挂起监视线程
void CSerial::PauseMonitor()
{
    if ( NULL != m_pReadThread )
    {
        m_pReadThread->SuspendThread();
    }
}

//恢复监视线程
void CSerial::RestoreMonitor()
{
    if ( NULL != m_pReadThread )
    {
        m_pReadThread->ResumeThread();
    }
}

//停止监视线程
void CSerial::StopMonitor()
{
    if ( NULL != m_pReadThread )
    {
        SetCommMask( m_hPort, 0 );
        SetEvent( m_ovWrite.hEvent );
        SetEvent( m_ovRead.hEvent );
        SetEvent( m_ovWait.hEvent );
        m_bThreadAlive = FALSE;

        if ( WaitForSingleObject( m_pReadThread->m_hThread, 5000 ) != WAIT_OBJECT_0 )
            TerminateThread( m_pReadThread, 0 );

        m_pReadThread = NULL;
    }
}

//接收事件线程函数
UINT CSerial::ReadThread( LPVOID pParam )
{
    CSerial* pThis = ( CSerial* )pParam;
    DWORD dwErrors = 0;
    DWORD dwEvtMask = 0;
    DWORD Transferred = 0;
    DWORD Readed = 0;
    COMSTAT ComStat;
    BYTE pBuff[4096] = {0};
    pThis->m_bThreadAlive = TRUE;

    if ( NULL == pThis->m_ovWait.hEvent )
    {
        MessageBox( NULL, _T( "创建等待事件失败" ), _T( "错误" ), 0 );
        return -1;
    }

    while ( pThis->m_bThreadAlive )
    {
        if ( !WaitCommEvent( pThis->m_hPort, &dwEvtMask, &pThis->m_ovWait ) )
        {
            if ( ERROR_IO_PENDING == GetLastError() )
            {
                WaitForSingleObject( pThis->m_ovWait.hEvent, INFINITE );

                if ( !GetOverlappedResult( pThis->m_hPort, &pThis->m_ovWait, &Transferred, FALSE ) )
                {
                    if ( ERROR_IO_INCOMPLETE != GetLastError() )
                    {
                        //						AfxMessageBox(_T("GetOverlappedResult Failed"));
                        SetEvent( pThis->m_ovWait.hEvent );
                        //						return -1;
                    }
                }
            }
        }

        switch ( dwEvtMask )
        {
        case EV_RXCHAR:
        {
            if ( !ClearCommError( pThis->m_hPort, &dwErrors, &ComStat ) )
            {
                PurgeComm( pThis->m_hPort, PURGE_RXABORT | PURGE_RXCLEAR );
                //					return -1;
            }

            // 				if (ComStat.cbInQue)
            // 					pThis->OnRecevie();
            if ( ComStat.cbInQue )
            {
                Readed = pThis->ReadData( pBuff, pThis->m_WantRead );
                pThis->m_CallBackFunc( pBuff, Readed, pThis->m_pWnd );
                TRACE( _T( "%dEV_RXCHAR\r\n" ), Readed );
            }
        }
        break;

        case EV_TXEMPTY:
            pThis->OnSend();
            break;

        case EV_BREAK:
            TRACE( _T( "EV_BREAK\r\n" ) );
            break;

        default:
            break;
        }

        ResetEvent( pThis->m_ovWait.hEvent );
    }

    TRACE( _T( "线程退出\r\n" ) );

    if ( NULL != pThis->m_ovWait.hEvent )
    {
        CloseHandle( pThis->m_ovWait.hEvent );
        pThis->m_ovWait.hEvent = NULL;
    }

    return 0;
}

//关闭串口
void CSerial::ClosePort()
{
    if ( !PortIsOpen() )
        return ;


    if ( NULL != m_pReadThread || m_bThreadAlive )
    {
        StopMonitor();
    }

    if ( m_ovRead.hEvent != NULL )
    {
        CloseHandle( m_ovRead.hEvent );
        m_ovRead.hEvent = NULL;
    }

    if ( m_ovWrite.hEvent != NULL )
    {
        CloseHandle( m_ovWrite.hEvent );
        m_ovWrite.hEvent = NULL;
    }

    if ( m_hPort != INVALID_HANDLE_VALUE )
    {
        CloseHandle( m_hPort );
        m_hPort = INVALID_HANDLE_VALUE;
    }
}

//读取数据
DWORD CSerial::ReadData( LPVOID pRecvBuf, DWORD WantReadLen, DWORD dwWaitTime/* =10 */ )
{
    DWORD Readed = 0;
    DWORD dwErrorFlags = 0;
    COMSTAT ComStat;
    DWORD ret = 0;

    if ( !ClearCommError( m_hPort, &dwErrorFlags, &ComStat ) && dwErrorFlags > 0 )
    {
        PurgeComm( m_hPort, PURGE_RXABORT | PURGE_RXCLEAR );
        AfxMessageBox( _T( "ClearCommError Failed" ) );
        return 0;
    }

    WantReadLen = WantReadLen > ComStat.cbInQue ? ComStat.cbInQue : WantReadLen;

    if ( !ReadFile( m_hPort, pRecvBuf, WantReadLen, &Readed, &m_ovRead ) )
    {
        if ( ERROR_IO_PENDING == GetLastError() )
        {
            if ( WAIT_OBJECT_0 != WaitForSingleObject( m_ovRead.hEvent, dwWaitTime ) )
            {
                if ( !GetOverlappedResult( m_hPort, &m_ovRead, &Readed, TRUE ) )
                {
                    AfxMessageBox( _T( "读取失败" ) );
                    return 0;
                }
            }
        }
    }

    //	ResetEvent(m_ovRead.hEvent);
    return Readed;
}

//写入数据
DWORD CSerial::WriteData( LPCVOID pWriteBuf, DWORD WriteLen )
{
    DWORD Written = 0;
    DWORD dwErrorFlags = 0;
    COMSTAT ComStat;
    DWORD ret = 0;

    if ( !ClearCommError( m_hPort, &dwErrorFlags, &ComStat ) && dwErrorFlags > 0 )
    {
        PurgeComm( m_hPort, PURGE_TXABORT | PURGE_TXCLEAR );
        AfxMessageBox( _T( "ClearCommError Failed" ) );
        return -1;
    }

    if ( !WriteFile( m_hPort, pWriteBuf, WriteLen, &Written, &m_ovWrite ) )
    {
        if ( GetLastError() == ERROR_IO_PENDING )
        {
            ret = WaitForSingleObject( m_ovWrite.hEvent, 1000 );

            switch ( ret )
            {
            case WAIT_OBJECT_0:
                if ( !GetOverlappedResult( m_hPort, &m_ovWrite, &Written, TRUE ) )
                {
//					AfxMessageBox(_T("发送成功"));
                    SetEvent( m_ovWrite.hEvent );
                    Written = 0;
                }

                break;

            case WAIT_TIMEOUT:
            {
                AfxMessageBox( _T( "发送超时" ) );
                SetEvent( m_ovWrite.hEvent );
                break;
            }

            case WAIT_FAILED:
            {
                AfxMessageBox( _T( "发送失败" ) );
                return 0;
                break;
            }

            default:
            {
                Written = 0;
                break;
            }
            }
        }

    }

    //	ResetEvent(m_ovWrite.hEvent);
    return Written;
}

//锁定
void CSerial::Lock()
{
    EnterCriticalSection( &m_Cs );
}

//解锁
void CSerial::UnLock()
{
    LeaveCriticalSection( &m_Cs );
}

void CSerial::OnRecevie()
{

}

void CSerial::OnSend()
{

}

//设置保存数据文件路径
BOOL CSerial::OpenSaveFilePath( LPCTSTR SaveFilePath )
{
    InitializeCriticalSection( &m_Cs );

    if ( NULL == m_hFile )
    {
        m_hFile = CreateFile( SaveFilePath, GENERIC_WRITE | GENERIC_READ, FILE_SHARE_READ | FILE_SHARE_WRITE, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL );
    }

    if ( INVALID_HANDLE_VALUE != m_hFile )
    {
        return TRUE;
    }
    else
        return FALSE;
}

//文件保存数据
void CSerial::WriteSaveFile( LPVOID pData, DWORD dwWantWrite )
{
    DWORD dwWrited = 0;
    Lock();
    WriteFile( m_hFile, pData, dwWantWrite, &dwWrited, NULL );
    UnLock();
}

//关闭数据文件
void CSerial::CloseSaveWrite()
{
    if ( INVALID_HANDLE_VALUE != m_hFile || NULL != m_hFile )
    {
        CloseHandle( m_hFile );
        m_hFile = NULL;
    }

    DeleteCriticalSection( &m_Cs );
}

BOOL CSerial::RegisterHardWare( HWND hwnd )
{
    DEV_BROADCAST_HANDLE  NotificationFilter;
    ZeroMemory( &NotificationFilter, sizeof( NotificationFilter ) );
    NotificationFilter.dbch_size = sizeof( DEV_BROADCAST_HANDLE );
    NotificationFilter.dbch_devicetype = DBT_DEVTYP_HANDLE;
    NotificationFilter.dbch_handle = m_hPort;
    HDEVNOTIFY  hDevNotify = RegisterDeviceNotification( hwnd, &NotificationFilter, DEVICE_NOTIFY_WINDOW_HANDLE );

    if ( NULL == hDevNotify )
    {
        TRACE( _T( "注册硬件通知事件失败" ) );
        return FALSE;
    }

    return TRUE;
}

int CSerial::EnumSerial( LPSTR lpszDest, LPCTSTR lpszFind, GUID SerialGUID )
{
    int iFindCount = 0;
    BOOL bFind = FALSE;
    HDEVINFO hDevinfo = SetupDiGetClassDevs( NULL, NULL, NULL, DIGCF_PRESENT | DIGCF_ALLCLASSES );
    //	DWORD i=0;

    SP_DEVINFO_DATA DevInfo_Data;
    DevInfo_Data.cbSize = sizeof( SP_DEVINFO_DATA );

    //	SetLastError(NO_ERROR);
    for ( DWORD i = 0; SetupDiEnumDeviceInfo( hDevinfo, i, &DevInfo_Data ) && ( GetLastError() != ERROR_NO_MORE_ITEMS ); i++ )
    {
        SP_DEVICE_INTERFACE_DATA Device_Interface_Data;
        Device_Interface_Data.cbSize = sizeof( SP_DEVICE_INTERFACE_DATA );
        DWORD j = 0;
        //		for (DWORD j=0;SetupDiEnumDeviceInterfaces(hDevinfo,&DevInfo_Data,NULL,j,&Device_Interface_Data) && (ERROR_NO_MORE_ITEMS!=GetLastError());j++)
        {
            //			PSP_DEVICE_INTERFACE_DETAIL_DATA Dev_Interface_Detail;
            DWORD RequiredSize = 0;
            // 			SetupDiGetDeviceInterfaceDetail(hDevinfo,&Device_Interface_Data,NULL,0,&RequiredSize,NULL);
            // 			Dev_Interface_Detail=(PSP_DEVICE_INTERFACE_DETAIL_DATA)new TCHAR[sizeof(SP_DEVICE_INTERFACE_DETAIL_DATA)+RequiredSize];
            // 			Dev_Interface_Detail->cbSize=sizeof(SP_DEVICE_INTERFACE_DETAIL_DATA);
            // 			SetupDiGetDeviceInterfaceDetail(hDevinfo,&Device_Interface_Data,Dev_Interface_Detail,RequiredSize,&RequiredSize,NULL);
            SetupDiGetDeviceRegistryProperty( hDevinfo, &DevInfo_Data, SPDRP_FRIENDLYNAME, NULL, NULL, 0, &RequiredSize );
            TCHAR *name = new TCHAR[RequiredSize + 10];
            SetupDiGetDeviceRegistryProperty( hDevinfo, &DevInfo_Data, SPDRP_FRIENDLYNAME, NULL, ( PBYTE )name, RequiredSize, &RequiredSize );
            int len = _tcslen( lpszFind ) * sizeof( TCHAR ) > _tcslen( name ) * sizeof( TCHAR ) ? _tcslen( name ) * sizeof( TCHAR ) : _tcslen( lpszFind ) * sizeof( TCHAR );
            bFind = TRUE;

            for ( int k = 0; k < len; k++ )
            {
                if ( lpszFind[k] != name[k] )
                {
                    bFind = FALSE;
                    break;
                }
            }

            // 			delete[] Dev_Interface_Detail;
            // 			Dev_Interface_Detail=NULL;
            if ( bFind )
            {
                TCHAR *posstart = NULL;
                posstart = strchr( name, 40 );
                TCHAR *posend = NULL;
                posend = strchr( name, 41 );

                if ( NULL == posstart || NULL == posend )
                {
                    //					AfxMessageBox(_T("该设备不匹配"));
                    continue;
                }

                int s = posstart - name + 1;
                int e = posend - name;
                memcpy_s( lpszDest, e - s, &name[s], e - s );
                iFindCount++;
            }

            delete name;
            name = NULL;
        }
    }

    SetupDiDestroyDeviceInfoList( hDevinfo );
    return iFindCount;
}

