/**
 * PE相关函数实现
 */

#pragma once

namespace DevToolkit
{
    class CPe
    {
    public:
        CPe();
        virtual ~CPe();

	public:
		static BOOL IATHook(IN HMODULE hModule,IN LPCTSTR pImageName,IN LPCVOID pTargetFuncAddr,IN LPCVOID pReplaceFuncAddr);
    protected:
    private:
    };
}