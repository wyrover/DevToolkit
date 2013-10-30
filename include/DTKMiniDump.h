#pragma once
#include <Windows.h>

namespace DevToolkit
{
	class EXPORTS_CLASS CMiniDump
	{
	public:
		CMiniDump( void );
		virtual ~CMiniDump( void );

	protected:
		static BOOL GetModulePath( LPTSTR lpBuf, DWORD dwBufSize );
		static LONG WINAPI MyUnhandledExceptionFilter( EXCEPTION_POINTERS* ExceptionInfo );
	};
}


