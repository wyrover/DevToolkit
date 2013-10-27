#pragma once

namespace DevToolkit
{
    class CString
    {
    public:
        CString();
        virtual ~CString();
        
        /**
        * \brief Ansi转换为Unicode
        * \param szAnsiStr 要转换的Ansi字符串
        * \return 转换之后的Unicode字符串
        * \warning 必须手动释放返回的字符串,否则会造成内存泄露
        */
        static wchar_t* AnsiToUnicode( const char* szAnsiStr );
    protected:
    private:
    };
}