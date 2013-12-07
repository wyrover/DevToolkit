#pragma once
#include <Windows.h>
#include <Mmsystem.h>
#include <mmdeviceapi.h>
#include <stdio.h>

namespace DevToolkit
{
class EXPORTS_CLASS CProcessManager
{
public:
	CProcessManager(void);

	virtual ~CProcessManager(void);

	virtual BOOL SetXpMute(BOOL bMute=TRUE)=0;

	virtual BOOL SetWin7Mute(BOOL bMute=TRUE)=0;

	virtual BOOL SetXpSpeed(double dbSpeed=1.0)=0;

	virtual BOOL setWin7Speed(double dbSpeed=1.0)=0;

	virtual BOOL HookSysFunc()=0;

	static CProcessManager* GetInstance();
};
}

