#include "stdafx.h"
#include "ProcessMgrImpl.h"
#include <Audiopolicy.h>
#include <Functiondiscoverykeys_devpkey.h>
#include <tchar.h>
#include <dsound.h>
#include "detours.h"
#pragma comment(lib,"Dsound.lib")
#pragma comment(lib,"Winmm.lib")
#pragma comment(lib,"detours.lib")
#include <math.h>


static DWORD (WINAPI* Sys_timeGetTime)()=timeGetTime;

static DWORD (WINAPI* Sys_GetTickCount)()=GetTickCount;

static BOOL (WINAPI* Sys_QueryPerformanceCounter)(LARGE_INTEGER *lpPerformanceCount)=QueryPerformanceCounter;

static MMRESULT (WINAPI* Sys_midiStreamOut)(HMIDISTRM hms, LPMIDIHDR pmh, UINT cbmh)=midiStreamOut;

static MMRESULT (WINAPI* Sys_waveOutWrite)(HWAVEOUT hwo,LPWAVEHDR pwh,UINT cbwh)=waveOutWrite;

static HRESULT (WINAPI* Sys_DirectSoundCreate)(LPCGUID pcGuidDevice, LPDIRECTSOUND *ppDS, LPUNKNOWN pUnkOuter)=DirectSoundCreate;

BOOL WINAPI BF_QueryPerformanceCounter(LARGE_INTEGER *lpPerformanceCount)
{  
    LONGLONG llTime=0;
    BOOL bRet=TRUE;
	::EnterCriticalSection(&CProcessMgrImpl::m_csQueryPerformanceCounter);
    bRet=Sys_QueryPerformanceCounter(lpPerformanceCount);
    if (bRet)
    {
        LONGLONG newCount=(*lpPerformanceCount).QuadPart;
        if (newCount<CProcessMgrImpl::m_llQueryPerformanceCounter1)
        {
            newCount=CProcessMgrImpl::m_llQueryPerformanceCounter1;
        }
        llTime=(LONGLONG)(floor((newCount-CProcessMgrImpl::m_llQueryPerformanceCounter1)*CProcessMgrImpl::m_dwSpeed+0.5)+CProcessMgrImpl::m_llQueryPerformanceCounter);
        (*lpPerformanceCount).QuadPart=llTime;
    }
	::LeaveCriticalSection(&CProcessMgrImpl::m_csQueryPerformanceCounter);
	return bRet;
}

DWORD WINAPI BF_GetTickCount()
{
    DWORD dwRet=0;
    DWORD dwTimeDiff=0;
	::EnterCriticalSection(&CProcessMgrImpl::m_csGetTimeCount);
	dwRet=Sys_GetTickCount();
	if (dwRet<CProcessMgrImpl::m_dwRealGetTickCount1)
	{
		dwRet=CProcessMgrImpl::m_dwRealGetTickCount1;
	}
	dwTimeDiff=(DWORD)(floor((dwRet-CProcessMgrImpl::m_dwRealGetTickCount1)*CProcessMgrImpl::m_dwSpeed)+0.5)+CProcessMgrImpl::m_dwRealGetTickCount;
	::LeaveCriticalSection(&CProcessMgrImpl::m_csGetTimeCount);
	return dwTimeDiff;
}

DWORD WINAPI BF_timeGetTime(void)
{
    DWORD dwRet=0;
    DWORD dwTime=0;
	::EnterCriticalSection(&CProcessMgrImpl::m_csTimeGetTime);
	dwRet=Sys_timeGetTime();
    if (dwRet<CProcessMgrImpl::m_dwRealTimeGetTime1)
    {
        dwRet=CProcessMgrImpl::m_dwRealTimeGetTime1;
    }
    dwTime=(DWORD)(floor((dwRet-CProcessMgrImpl::m_dwRealTimeGetTime1)*CProcessMgrImpl::m_dwSpeed+0.5)+CProcessMgrImpl::m_dwRealTimeGetTime);
	::LeaveCriticalSection(&CProcessMgrImpl::m_csTimeGetTime);
	return dwTime;
}

MMRESULT WINAPI BF_midiStreamOut(HMIDISTRM hms, LPMIDIHDR pmh, UINT cbmh)
{
	if (CProcessMgrImpl::IsMute())
	{
		if (NULL!=pmh)
		{
			memset(pmh->lpData,0,pmh->dwBufferLength);
		}
	}
	return Sys_midiStreamOut(hms,pmh,cbmh);
}

MMRESULT WINAPI BF_waveOutWrite(HWAVEOUT hwo,LPWAVEHDR pwh,UINT cbwh)
{
	if (CProcessMgrImpl::IsMute())
	{
		if (NULL!=pwh)
		{
			memset(pwh->lpData,0,pwh->dwBufferLength);
		}
	}
	return Sys_waveOutWrite(hwo,pwh,cbwh);
}

HRESULT BF_CreateSoundBuffer(LPCDSBUFFERDESC pcDSBufferDesc, LPDIRECTSOUNDBUFFER *ppDSBuffer, LPUNKNOWN pUnkOuter)
{
	typedef HRESULT (__stdcall *Sys_CreateSoundBuffer) (LPCDSBUFFERDESC pcDSBufferDesc,LPDIRECTSOUNDBUFFER *ppDSBuffer,LPUNKNOWN pUnkOuter);
	Sys_CreateSoundBuffer func_Sys_CreateSoundBuffer=(Sys_CreateSoundBuffer)(DWORD*)CProcessMgrImpl::m_dwOldByte_funcCreateSoundBuffer;
	HRESULT hr=func_Sys_CreateSoundBuffer(pcDSBufferDesc,ppDSBuffer,pUnkOuter);
	return hr;
}

HRESULT __stdcall BF_DirectSoundCreate(LPCGUID pcGuidDevice, LPDIRECTSOUND *ppDS, LPUNKNOWN pUnkOuter)
{
	HRESULT hr=Sys_DirectSoundCreate(pcGuidDevice,ppDS,pUnkOuter);
	DWORD dwOldProtect=0;
	if (NULL!=ppDS)
	{
		DWORD** vptr=(DWORD**)(*ppDS);
		DWORD* addrCreateSoundBuffer=*vptr+0x0C;
		CProcessMgrImpl::m_dwOldByte_funcCreateSoundBuffer=*(addrCreateSoundBuffer);
		VirtualProtectEx(NULL,addrCreateSoundBuffer,sizeof(DWORD),PAGE_EXECUTE_READWRITE,&dwOldProtect);
		*addrCreateSoundBuffer=*((DWORD*)&BF_CreateSoundBuffer);
	}
	return hr;
}

HRESULT __stdcall BF_Stop()
{
	HRESULT hr=S_OK;
	return hr;
}

CProcessManager* CProcessMgrImpl::m_pProcessMgr=NULL;
BOOL CProcessMgrImpl::m_bMute=FALSE;
CRITICAL_SECTION CProcessMgrImpl::m_csGetTimeCount;
CRITICAL_SECTION CProcessMgrImpl::m_csTimeGetTime;
CRITICAL_SECTION CProcessMgrImpl::m_csQueryPerformanceCounter;
DWORD CProcessMgrImpl::m_dwOldByte_funcCreateSoundBuffer=0;
BOOL CProcessMgrImpl::m_bIsHooked=FALSE;

DWORD CProcessMgrImpl::m_dwRealGetTickCount=0;
DWORD CProcessMgrImpl::m_dwRealGetTickCount1=0;

DWORD CProcessMgrImpl::m_dwRealTimeGetTime=0;
DWORD CProcessMgrImpl::m_dwRealTimeGetTime1=0;

LONGLONG CProcessMgrImpl::m_llQueryPerformanceCounter=0;
LONGLONG CProcessMgrImpl::m_llQueryPerformanceCounter1=0;

double CProcessMgrImpl::m_dwSpeed=1.0;

CProcessMgrImpl::CProcessMgrImpl(void)
{
	::InitializeCriticalSection(&m_csGetTimeCount);
	::InitializeCriticalSection(&m_csTimeGetTime);
	::InitializeCriticalSection(&m_csQueryPerformanceCounter);
}


CProcessMgrImpl::~CProcessMgrImpl(void)
{
	::DeleteCriticalSection(&m_csGetTimeCount);
	::DeleteCriticalSection(&m_csTimeGetTime);
	::DeleteCriticalSection(&m_csQueryPerformanceCounter);
}

CProcessManager* CProcessMgrImpl::GetInstance()
{
	if (NULL==m_pProcessMgr)
	{
		m_pProcessMgr=new CProcessMgrImpl();
	}
	return m_pProcessMgr;
}

BOOL CProcessMgrImpl::SetXpMute(BOOL bMute/* =TRUE */)
{
	return FALSE;
}

BOOL CProcessMgrImpl::SetWin7Mute(BOOL bMute/* =TRUE */)
{
	if (m_bMute == bMute)
		return S_OK;


	IMMDeviceEnumerator* pEnumerator;

	HRESULT hr = E_FAIL;
	CoInitialize(NULL);
	hr = CoCreateInstance(__uuidof(MMDeviceEnumerator), NULL,
		CLSCTX_ALL, __uuidof(IMMDeviceEnumerator), (void**)&pEnumerator);

	IMMDevice* pDevice;
	hr = pEnumerator->GetDefaultAudioEndpoint(eRender, eConsole, &pDevice);
	if (FAILED(hr))
		return hr;

	IAudioSessionManager2* pasm = NULL;
	hr = pDevice->Activate(__uuidof(IAudioSessionManager2), CLSCTX_ALL, NULL, (void**)&pasm);
	if (FAILED(hr))
		return hr;

	IAudioSessionEnumerator* audio_session_enumerator;
	if(SUCCEEDED(pasm->GetSessionEnumerator(&audio_session_enumerator)))
	{
		int count;

		if(SUCCEEDED(audio_session_enumerator->GetCount(&count)))
		{
			for (int i = 0; i < count; i++)  
			{
				IAudioSessionControl* audio_session_control;
				IAudioSessionControl2* audio_session_control2;

				if(SUCCEEDED(audio_session_enumerator->GetSession(i, &audio_session_control)))
				{
					if(SUCCEEDED(audio_session_control->QueryInterface(__uuidof(IAudioSessionControl2), (void**)&audio_session_control2)))
					{
						DWORD processid;

						if(SUCCEEDED(audio_session_control2->GetProcessId(&processid)))
						{
							if (processid == GetCurrentProcessId())
							{
								ISimpleAudioVolume* pSAV;
								hr = audio_session_control2->QueryInterface(__uuidof(ISimpleAudioVolume), (void**) &pSAV);
								if (SUCCEEDED(hr))
								{
									hr = pSAV->SetMute(bMute, NULL);
									if (SUCCEEDED(hr))
									{
										m_bMute = bMute;
									}
									pSAV->Release();
								}
							}
							audio_session_control->Release();
							audio_session_control2->Release();
						}
					}
				}
			}
			audio_session_enumerator->Release();
		}
	} 

	pasm->Release();

	SafeRelease(&pEnumerator);

	::CoUninitialize();

	return hr;
}

BOOL CProcessMgrImpl::SetXpSpeed(double dbSpeed/* =1.0 */)
{
    HookSysFunc();
	return FALSE;
}

BOOL CProcessMgrImpl::setWin7Speed(double dbSpeed/* =1.0 */)
{
    m_dwRealGetTickCount=GetTickCount();
    m_dwRealGetTickCount1=Sys_GetTickCount();

    LARGE_INTEGER qpc;
    QueryPerformanceCounter(&qpc);
    m_llQueryPerformanceCounter=qpc.QuadPart;
    Sys_QueryPerformanceCounter(&qpc);
    m_llQueryPerformanceCounter1=qpc.QuadPart;

    m_dwRealTimeGetTime=timeGetTime();
    m_dwRealTimeGetTime1=Sys_timeGetTime();

    m_dwSpeed=dbSpeed;	
    
    HookSysFunc();
	return FALSE;
}

template <class T> 
void CProcessMgrImpl::SafeRelease(T **ppT)
{
	if (*ppT)
	{
		(*ppT)->Release();
		*ppT = NULL;
	}
}

BOOL CProcessMgrImpl::IsMute()
{
	return m_bMute;
}

/*
BOOL CProcessMgrImpl::SetWin7Mute(BOOL bMute)
{
	HMIXER hMixer;
	MIXERCONTROL mxc;
	MIXERLINE mxl;
	MIXERLINECONTROLS mxlc;
	MIXERCONTROLDETAILS mxcd;
	MIXERCONTROLDETAILS_SIGNED volStruct;
	MMRESULT mmr;

	//Sound Setting
	mmr = mixerOpen(&hMixer, 0, 0, 0, 0);
	if (mmr != MMSYSERR_NOERROR) return FALSE;

	// 初始化MIXERLINE结构体.
	ZeroMemory(&mxl, sizeof(MIXERLINE));
	mxl.cbStruct = sizeof(MIXERLINE);

	// 指出需要获取的通道，扬声器用MIXERLINE_COMPONENTTYPE_DST_SPEAKERS
	mxl.dwComponentType = MIXERLINE_COMPONENTTYPE_DST_SPEAKERS;

	mmr = mixerGetLineInfo((HMIXEROBJ)hMixer, &mxl, MIXER_GETLINEINFOF_COMPONENTTYPE);
	if (mmr != MMSYSERR_NOERROR) return FALSE;

	// 取得控制器.
	ZeroMemory(&mxlc, sizeof(MIXERLINECONTROLS));
	mxlc.cbStruct = sizeof(MIXERLINECONTROLS);
	mxlc.dwLineID = mxl.dwLineID;
	mxlc.dwControlType = MIXERCONTROL_CONTROLTYPE_MUTE;
	mxlc.cControls = 1;
	mxlc.cbmxctrl = sizeof(mxc);
	mxlc.pamxctrl = &mxc;
	//mxc.cMultipleItems=0;

	ZeroMemory(&mxc, sizeof(MIXERCONTROL));
	mxc.cbStruct = sizeof(MIXERCONTROL);
	mmr = mixerGetLineControls((HMIXEROBJ)hMixer, &mxlc, MIXER_GETLINECONTROLSF_ONEBYTYPE);
	if (mmr != MMSYSERR_NOERROR) return FALSE;

	// 初始化MIXERCONTROLDETAILS结构体
	ZeroMemory(&mxcd, sizeof(MIXERCONTROLDETAILS));
	mxcd.cbStruct = sizeof(MIXERCONTROLDETAILS);
	mxcd.cbDetails = sizeof(MIXERCONTROLDETAILS_SIGNED);
	mxcd.dwControlID = mxc.dwControlID;
	mxcd.paDetails = &volStruct;
	mxcd.cChannels = 1;

	// 获得音量值
	volStruct.lValue = bMute;
	mmr = mixerSetControlDetails((HMIXEROBJ)hMixer, &mxcd, MIXER_SETCONTROLDETAILSF_VALUE);

	mixerClose(hMixer);
	return TRUE;
}
*/

BOOL CProcessMgrImpl::HookSysFunc()
{	
	BOOL bSuccess=FALSE;
	DWORD dwOldProtected=0;
    if (!m_bIsHooked)
    {
        m_bIsHooked=TRUE;

        bSuccess=HookFuncBegin(&(PVOID&)Sys_GetTickCount,BF_GetTickCount);
        if (!bSuccess)
        {
            HookFuncEnd(&(PVOID&)Sys_GetTickCount,BF_GetTickCount);
        }

        VirtualProtectFunc(&(PVOID&)Sys_GetTickCount,dwOldProtected,&dwOldProtected);

        bSuccess=HookFuncBegin(&(PVOID&)Sys_timeGetTime,BF_timeGetTime);
        if (!bSuccess)
        {
            HookFuncEnd(&(PVOID&)Sys_timeGetTime,BF_timeGetTime);
        }

        bSuccess=HookFuncBegin(&(PVOID&)Sys_QueryPerformanceCounter,BF_QueryPerformanceCounter);
        if (!bSuccess)
        {
            HookFuncEnd(&(PVOID&)Sys_QueryPerformanceCounter,BF_QueryPerformanceCounter);
        }

        bSuccess=HookFuncBegin(&(PVOID&)Sys_midiStreamOut,BF_midiStreamOut);
        if (!bSuccess)
        {
            HookFuncEnd(&(PVOID&)Sys_midiStreamOut,BF_midiStreamOut);
        }

        bSuccess=HookFuncBegin(&(PVOID&)Sys_waveOutWrite,BF_waveOutWrite);
        if (!bSuccess)
        {
            HookFuncEnd(&(PVOID&)Sys_waveOutWrite,BF_waveOutWrite);
        }

        bSuccess=HookFuncBegin(&(PVOID&)Sys_DirectSoundCreate,BF_DirectSoundCreate);
        if (!bSuccess)
        {
            HookFuncEnd(&(PVOID&)Sys_DirectSoundCreate,BF_DirectSoundCreate);
        }
    }

	return bSuccess;
}


BOOL CProcessMgrImpl::HookFuncBegin(PVOID *ppPointer,PVOID pDetour)
{
	DetourRestoreAfterWith();
	DetourTransactionBegin();
	DetourUpdateThread(GetCurrentThread());
	DetourAttach(ppPointer, pDetour);
	DWORD dwErr = DetourTransactionCommit();
	if (dwErr == NO_ERROR) {
		OutputDebugString(_T("HookSuccess"));
	}
	else {
		OutputDebugString(_T("HookFailed"));
	}
	return (dwErr==NO_ERROR);
}

BOOL CProcessMgrImpl::HookFuncEnd(PVOID *ppPointer,PVOID pDetour)
{
	DetourRestoreAfterWith();
	DetourTransactionBegin();
	DetourUpdateThread(GetCurrentThread());
	DetourDetach(ppPointer, pDetour);
	DWORD dwErr = DetourTransactionCommit();
	return (dwErr==NO_ERROR);
}

BOOL CProcessMgrImpl::VirtualProtectFunc(LPVOID lpAddr,DWORD dwProtected,DWORD* dwOldProtected)
{
	HANDLE hProcess=::OpenProcess(PROCESS_ALL_ACCESS,FALSE,GetCurrentProcessId());
	if (NULL==hProcess)
	{
		return FALSE;
	}
	BOOL bRet=::VirtualProtectEx(hProcess,lpAddr,sizeof(DWORD),dwProtected,dwOldProtected);
	if (0==bRet)
	{
		DWORD dwErr=GetLastError();
		TCHAR sErr[MAX_PATH]={0};
		_stprintf_s(sErr,_T("%u\r\n"),dwErr);
		OutputDebugString(sErr);
	}
	::CloseHandle(hProcess);
	return bRet;
}