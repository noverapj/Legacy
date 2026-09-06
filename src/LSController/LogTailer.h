#pragma once

#include <windows.h>

// Tails a single log file on a worker thread.
//  - Initial dump: the last 32 KB of the file.
//  - Polls for appended data every second.
//  - If the file shrinks (the server recreated it), restarts from the top.
//  - Posts nMessage (wParam = new'd std::string*) to the notify window for each chunk.
//    The receiver owns the buffer and must delete it.
class CLogTailer
{
public:
	CLogTailer();
	~CLogTailer();

	bool Start(const CString& strFilePath, HWND hWndNotify, UINT nMessage);
	void Stop();

private:
	static unsigned int __stdcall ThreadProc(LPVOID pParam);
	void Run();

	HANDLE m_hThread = nullptr;
	HANDLE m_hStopEvent = nullptr;
	CString m_strFilePath;
	HWND m_hWndNotify = nullptr;
	UINT m_nMessage = 0;
};
