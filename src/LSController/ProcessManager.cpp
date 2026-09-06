#include "pch.h"
#include "ProcessManager.h"

CString ProcessManager::BuildCommandLine(const ServerEntry& entry)
{
    CString strCmd;
    strCmd.Format(_T("\"%s\" -c %s"), entry.m_strExePath.GetString(), entry.m_strIni.GetString());
    return strCmd;
}

bool ProcessManager::Start(ServerEntry& entry, CString* pstrError /* = nullptr */)
{
    if (UpdateStatus(entry))
    {
        if (pstrError)
            pstrError->Format(_T("%s is already running (PID %u)."), entry.m_strName.GetString(), entry.m_dwPid);
        return false;
    }

    CString strCmd = BuildCommandLine(entry);

    STARTUPINFOW si = {};
    si.cb = sizeof(si);
    PROCESS_INFORMATION pi = {};

    LPTSTR pszCmd = strCmd.GetBuffer();
    BOOL bOk = ::CreateProcessW(nullptr, pszCmd, nullptr, nullptr, FALSE,
                                CREATE_NO_WINDOW | CREATE_NEW_PROCESS_GROUP,
                                nullptr,
                                entry.m_strDir.IsEmpty() ? nullptr : entry.m_strDir.GetString(),
                                &si, &pi);
    strCmd.ReleaseBuffer();

    if (!bOk)
    {
        if (pstrError)
            pstrError->Format(_T("CreateProcess failed (error %u): %s"), ::GetLastError(), strCmd.GetString());
        return false;
    }

    ::CloseHandle(pi.hThread);
    entry.m_hProcess = pi.hProcess;
    entry.m_dwPid = pi.dwProcessId;
    entry.m_bRunning = true;
    return true;
}

bool ProcessManager::UpdateStatus(ServerEntry& entry)
{
    if (entry.m_hProcess == nullptr)
    {
        entry.m_bRunning = false;
        return false;
    }

    if (::WaitForSingleObject(entry.m_hProcess, 0) == WAIT_OBJECT_0)
    {
        ::CloseHandle(entry.m_hProcess);
        entry.m_hProcess = nullptr;
        entry.m_dwPid = 0;
        entry.m_bRunning = false;
        return false;
    }

    entry.m_bRunning = true;
    return true;
}

bool ProcessManager::SendCtrlBreak(DWORD dwPid)
{
    // Ctrl events are routed through console infrastructure. A GUI process has no
    // console, so GenerateConsoleCtrlEvent alone silently does nothing. Attach to
    // the child's (hidden) console first, then send.
    if (!::AttachConsole(dwPid))
        return false;

    // Ignore the signal ourselves while sharing the child's console
    ::SetConsoleCtrlHandler(nullptr, TRUE);

    BOOL bOk = ::GenerateConsoleCtrlEvent(CTRL_BREAK_EVENT, dwPid);

    ::SetConsoleCtrlHandler(nullptr, FALSE);
    ::FreeConsole();
    return bOk != FALSE;
}

bool ProcessManager::StopGraceful(ServerEntry& entry, DWORD dwTimeoutMs /* = 10000 */)
{
    if (!UpdateStatus(entry))
        return true;    // already stopped

    if (!SendCtrlBreak(entry.m_dwPid))
        return false;   // could not deliver the signal

    for (DWORD dwElapsed = 0; dwElapsed < dwTimeoutMs; dwElapsed += 100)
    {
        if (::WaitForSingleObject(entry.m_hProcess, 100) == WAIT_OBJECT_0)
            break;
    }

    return !UpdateStatus(entry);   // true when exited
}
