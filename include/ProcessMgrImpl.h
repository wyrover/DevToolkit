#pragma once
#include <mmdeviceapi.h>
#include "processmanager.h"

namespace DevToolkit
{
class EXPORTS_CLASS CProcessMgrImpl : public CProcessManager
{
public:

	virtual ~CProcessMgrImpl(void);

	BOOL SetXpMute(BOOL bMute/* =TRUE */);

	BOOL SetWin7Mute(BOOL bMute/* =TRUE */);

	BOOL SetXpSpeed(double dbSpeed/* =1.0 */);

	BOOL setWin7Speed(double dbSpeed/* =1.0 */);

	static CProcessManager* GetInstance();	

	static BOOL IsMute();

	BOOL HookSysFunc();

	static CRITICAL_SECTION m_csTimeGetTime;

	static CRITICAL_SECTION m_csGetTimeCount;

	static CRITICAL_SECTION m_csQueryPerformanceCounter;

	static DWORD m_dwOldByte_funcCreateSoundBuffer;	// CreateSoundBuffer函数前4个字节

	static DWORD m_dwRealGetTickCount;
	static DWORD m_dwRealGetTickCount1;

    static LONGLONG m_llQueryPerformanceCounter;
    static LONGLONG m_llQueryPerformanceCounter1;

	static DWORD m_dwRealTimeGetTime;
	static DWORD m_dwRealTimeGetTime1;

    static double m_dwSpeed;

protected:
	template<class T>
	void SafeRelease(T **ppT);

	BOOL HookFuncBegin(PVOID *ppPointer,PVOID pDetour);

	BOOL HookFuncEnd(PVOID *ppPointer,PVOID pDetour);

	BOOL VirtualProtectFunc(LPVOID lpAddr,DWORD dwProtected,DWORD* dwOldProtected);

private:

	CProcessMgrImpl(void);

	static CProcessManager* m_pProcessMgr;

	static BOOL m_bMute;

	static BOOL m_bIsHooked;
};
}

