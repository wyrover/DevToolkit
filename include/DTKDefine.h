#define _WTL_NO_CSTRING  
#define _WTL_NO_WTYPES  
#include <atlstr.h>  
#include <atltypes.h>  

#include <atlbase.h>  
#include <atlapp.h>  

//extern CAppModule _Module;  

#include <atlwin.h> 

#ifdef _USRDLL
#ifndef DEVTOOLKIT_EXPORTS
#define EXPORTS_FUNC extern "C" __declspec(dllimport)
#define EXPORTS_CLASS __declspec(dllimport)
#else
#define EXPORTS_FUNC extern "C" __declspec(dllexport)
#define EXPORTS_CLASS __declspec(dllexport)
#endif
#else
#define EXPORTS_FUNC
#define EXPORTS_CLASS
#endif