#include "pch.h"
#include "LogTailer.h"

#include <string>
#include <process.h>

namespace
{
	// Maximum initial tail (bytes) loaded when a file is opened.
	const ULONGLONG kInitialTailBytes = 32 * 1024;
}

CLogTailer::CLogTailer()
{
}

CLogTailer::~CLogTailer()
{
	Stop();
}

bool CLogTailer::Start(const CString& strFilePath, HWND hWndNotify, UINT nMessage)
{
	Stop();

	m_strFilePath = strFilePath;
	m_hWndNotify = hWndNotify;
	m_nMessage = nMessage;

	m_hStopEvent = ::CreateEventW(nullptr, TRUE, FALSE, nullptr);
	if (m_hStopEvent == nullptr)
		return false;

	m_hThread = reinterpret_cast<HANDLE>(::_beginthreadex(nullptr, 0, &CLogTailer::ThreadProc, this, 0, nullptr));
	if (m_hThread == nullptr)
	{
		::CloseHandle(m_hStopEvent);
		m_hStopEvent = nullptr;
		return false;
	}
	return true;
}

void CLogTailer::Stop()
{
	if (m_hStopEvent != nullptr)
		::SetEvent(m_hStopEvent);

	if (m_hThread != nullptr)
	{
		::WaitForSingleObject(m_hThread, 5000);
		::CloseHandle(m_hThread);
		m_hThread = nullptr;
	}

	if (m_hStopEvent != nullptr)
	{
		::CloseHandle(m_hStopEvent);
		m_hStopEvent = nullptr;
	}
}

unsigned int __stdcall CLogTailer::ThreadProc(LPVOID pParam)
{
	static_cast<CLogTailer*>(pParam)->Run();
	return 0;
}

void CLogTailer::Run()
{
	ULONGLONG nPosition = 0;
	bool bFirstOpen = true;

	for (;;)
	{
		if (::WaitForSingleObject(m_hStopEvent, 1000) != WAIT_TIMEOUT)
			break;	// stop signaled

		HANDLE hFile = ::CreateFileW(m_strFilePath, GENERIC_READ,
		                             FILE_SHARE_READ | FILE_SHARE_WRITE | FILE_SHARE_DELETE,
		                             nullptr, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, nullptr);
		if (hFile == INVALID_HANDLE_VALUE)
		{
			bFirstOpen = true;	// not there (yet) -> re-arm the initial tail
			continue;
		}

		LARGE_INTEGER liSize;
		if (!::GetFileSizeEx(hFile, &liSize))
		{
			::CloseHandle(hFile);
			continue;
		}

		if (bFirstOpen)
		{
			// On open: dump only the last kInitialTailBytes of the file
			nPosition = (liSize.QuadPart > static_cast<LONGLONG>(kInitialTailBytes))
				? static_cast<ULONGLONG>(liSize.QuadPart) - kInitialTailBytes
				: 0;
			bFirstOpen = false;
		}
		else if (static_cast<ULONGLONG>(liSize.QuadPart) < nPosition)
		{
			// File shrank (the server recreated it) -> restart from the top
			nPosition = 0;
		}

		std::string strData;
		LARGE_INTEGER liPos;
		liPos.QuadPart = static_cast<LONGLONG>(nPosition);
		if (::SetFilePointerEx(hFile, liPos, nullptr, FILE_BEGIN))
		{
			char buffer[8192];
			DWORD dwRead = 0;
			for (;;)
			{
				if (!::ReadFile(hFile, buffer, sizeof(buffer), &dwRead, nullptr) || dwRead == 0)
					break;
				strData.append(buffer, dwRead);
				if (strData.size() > 1024 * 1024)	// per-chunk cap
					break;
			}
			nPosition += strData.size();
		}

		if (!strData.empty() && m_hWndNotify != nullptr)
		{
			auto* pStr = new std::string(strData);
			if (!::PostMessageW(m_hWndNotify, m_nMessage, reinterpret_cast<WPARAM>(pStr), 0))
				delete pStr;
		}

		::CloseHandle(hFile);
	}
}
