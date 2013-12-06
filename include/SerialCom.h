#pragma once

class CSerialCom
{
public:
    CSerialCom( void );
    virtual ~CSerialCom( void );

    virtual BOOL RegisterHardWare( HWND hwnd ) = 0;
    virtual BOOL PortIsOpen() = 0;
    virtual BOOL OpenPort( LPTSTR portnum, DWORD dwBaudRate = 9600, BYTE byParity = NOPARITY, BYTE byStopBits = ONESTOPBIT, BYTE byByteSize = 8, HANDLE hWnd=NULL ) = 0;
    virtual void ClosePort() = 0;
    virtual DWORD ReadData( LPVOID pRecvBuf, DWORD ReadLen, DWORD dwWaitTime = 10 ) = 0;
    virtual DWORD WriteData( LPCVOID pWriteBuf, DWORD WriteLen ) = 0;
    virtual BOOL StartMonitor() = 0;
    virtual void PauseMonitor() = 0;
    virtual void RestoreMonitor() = 0;
    virtual void StopMonitor() = 0;
    virtual int EnumSerial( LPSTR lpszDest, LPCTSTR lpszFind, GUID SerialGUID ) = 0;
    virtual void SetCallBackFunc( LPVOID CallBackFunc, LPVOID pParam ) = 0;
    DWORD m_CmdID;
    DWORD m_WantRead;
    DWORD m_Recevied;
    BYTE m_RecvedBuf[2048];
};

class CSerialFactory
{
public:
    CSerialFactory();
    ~CSerialFactory();
    virtual CSerialCom* CreateSerial() = 0;
};

class ConcreteSerialFactory : public CSerialFactory
{
public:
    ConcreteSerialFactory();
    ~ConcreteSerialFactory();
    CSerialCom* CreateSerial();
};
