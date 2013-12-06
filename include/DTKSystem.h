/**
 * 系统相关函数实现
 */
#pragma once


/**
* \brief 检测当前用户是否具有系统权限
* \return 当前用户具有系统权限,返回TRUE,否则返回FALSE
*/
EXPORTS_FUNC BOOL IsAdmin();

/**
* \brief 将GetLastError函数返回的ID格式化为字符串
* \return 格式化后的字符串
* \warning 必须手动释放返回的字符串
*/
EXPORTS_FUNC LPCTSTR GetLastErrMsg();

/**
* \brief 将系统定义的错误ID格式化为字符串
* \param dwErrID 系统定义的错误ID
* \return 格式化后的字符串
* \warning 必须手动释放返回的字符串
*/
EXPORTS_FUNC LPCTSTR FormatErrMsg( const DWORD dwErrID );

/**
* \brief 以管理员权限运行程序
* \param lpExeFile 要运行的程序路径
* \return 成功返回TRUE,失败返回FALSE
*/
EXPORTS_FUNC BOOL runAsAdmin( LPCTSTR lpExeFile );

/**
* \brief 当前系统版本是否是Vista之后的版本
* \return 是Vista之后版本返回TRUE,否则返回FALSE
*/
EXPORTS_FUNC BOOL IsVistaOrLater();


/**
* \brief 打开一个URL
* \param szURL URL地址
* \param bPriorityIE 是否优先使用IE打开
* \return HINSTANCE
*/
EXPORTS_FUNC HINSTANCE OpenURL( const LPCTSTR szURL, BOOL bPriorityIE );

/**
* \brief 当前系统是否是64位架构
* \return 是64位架构返回TRUE,否则返回FALSE
*/
EXPORTS_FUNC BOOL IsIA64();

/**
* \brief 在Explorer中定位并选择文件
* \param lpszFile 要定位的文件路径
* \return HINSTANCE
*/
EXPORTS_FUNC HINSTANCE OpenFileInExplorer( const LPCTSTR lpszFile );

/**
* \brief 根据窗口句柄获取模块路径
* \param hWnd 要获取路径的窗口句柄
* \param lpszBuf 路径缓冲区
* \param dwBufSize 缓冲区大小
* \return 模块路径
*/
EXPORTS_FUNC LPCTSTR GetModulePathByHwnd( const HWND hWnd, LPTSTR lpszBuf, DWORD dwBufSize );

EXPORTS_FUNC BOOL ParseURLs( LPCTSTR pwszURL, LPTSTR pwszHostName, LPTSTR pwszPath, WORD& wPort );

EXPORTS_FUNC BOOL EncodeURL( LPCTSTR pwszURL, int iLen, LPTSTR pwszRetURL, int& iRetLen ) ;

EXPORTS_FUNC HRESULT OpenUrlWithDefaultBrowser( LPCTSTR lpUrl );

EXPORTS_FUNC BOOL IsHasNetConnect();

EXPORTS_FUNC BOOL OpenUrlWithIE( LPCTSTR lpszURL );

EXPORTS_FUNC BOOL IsWin7();

EXPORTS_FUNC BOOL IsXP();

EXPORTS_FUNC BOOL IsVista();

EXPORTS_FUNC BOOL IsWin8();

EXPORTS_FUNC BOOL IsWin2000();

EXPORTS_FUNC BOOL IsWinNT();

EXPORTS_FUNC BOOL SetProcessAppUserModelID( LPCWSTR lpwsProcessUserModelID );

BOOL OpenFileDlg( HWND hWnd, LPTSTR lpFileName );

/**
 * 模拟实现GetProcAddress函数
 * @param phModule 要获取函数的模块句柄
 * @param pProcName 要获取的函数的名称
 * @return 要获取的函数在phModule中的地址
*/
DWORD GetFunctionAddress( HMODULE phModule, TCHAR* pProcName );

DWORD GetThreadIDByProcssID( DWORD dwProcessID );

HWND GetWindowHandleByPID( DWORD dwProcessID );