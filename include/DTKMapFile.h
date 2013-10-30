#pragma once

namespace DevToolkit
{
	class EXPORTS_CLASS CMapFile
	{
	public:
		CMapFile();
		virtual ~CMapFile();
	public:
		BOOL Open( LPCTSTR lpszFile );
		LPVOID GetData()const;
		void Close();
	protected:
		void CloseFileHandle();
		void CloseMapFileHandle();
	private:
		HANDLE m_hFile;
		HANDLE m_hFileMap;
		LPVOID m_pMapData;
	};
}
