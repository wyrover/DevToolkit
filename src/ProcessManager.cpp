#include "stdafx.h"
#include "ProcessManager.h"
#include "ProcessMgrImpl.h"


CProcessManager::CProcessManager(void)
{
}


CProcessManager::~CProcessManager(void)
{
}

CProcessManager* CProcessManager::GetInstance()
{
	return CProcessMgrImpl::GetInstance();
}