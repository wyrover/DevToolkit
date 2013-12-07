#include "stdafx.h"
#include "ProcessManager.h"
#include "ProcessMgrImpl.h"

namespace DevToolkit
{
    CProcessManager::CProcessManager( void )
    {
    }
    
    
    CProcessManager::~CProcessManager( void )
    {
    }
    
    CProcessManager* CProcessManager::GetInstance()
    {
        return CProcessMgrImpl::GetInstance();
    }
}