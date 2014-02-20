#include "stdafx.h"
#include "FileMap.h"

void Test_FileMap()
{
    const TCHAR* sFile = _T("E:\\Project\\bin\\Debug\\BugReport.exe");
    CFileMap fileMap;

    if(fileMap.Create(sFile, PAGE_READWRITE))
    {
        DWORD dwFileSize = fileMap.GetFileSize();
        LPVOID lpAddr = fileMap.GetMapAddress();
        memset(lpAddr, 0, dwFileSize);
        fileMap.Close();
    }
}

int main()
{
    Test_FileMap();
    return 0;
}