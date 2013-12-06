// stdafx.h : 标准系统包含文件的包含文件，
// 或是经常使用但不常更改的
// 特定于项目的包含文件
//

#pragma once

#include "targetver.h"

#define WIN32_LEAN_AND_MEAN             //  从 Windows 头文件中排除极少使用的信息
// Windows 头文件:
#include <windows.h>
#include <tchar.h>
#include "DevToolkit.h"

#define _WTL_NO_CSTRING  
#define _WTL_NO_WTYPES  
#include <atlstr.h>  
#include <atltypes.h>  

#include <atlbase.h>  
#include <atlapp.h>  

extern CAppModule _Module;  

#include <atlwin.h> 

// TODO: 在此处引用程序需要的其他头文件
