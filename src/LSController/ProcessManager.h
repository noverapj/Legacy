#pragma once

#include "ServerEntry.h"

class ProcessManager
{
public:
    // Spawns "<Exe> -c <INI>" hidden, with the server's folder as working directory.
    // CREATE_NO_WINDOW        : hidden console (child still owns one; required for ctrl events)
    // CREATE_NEW_PROCESS_GROUP: allows GenerateConsoleCtrlEvent(CTRL_BREAK_EVENT, pid)
    static bool Start(ServerEntry& entry, CString* pstrError = nullptr);

    // Graceful stop: attaches to the child's console (required for ctrl events
    // to be routed from a GUI process), sends CTRL_BREAK_EVENT (all ls_ servers
    // handle it in their ConsoleHandler), and waits up to dwTimeoutMs.
    // Returns true when the process exited; false when it is still running.
    // No force termination - a still-running server is reported, not killed.
    static bool StopGraceful(ServerEntry& entry, DWORD dwTimeoutMs = 10000);

    // Refreshes m_bRunning from the live process handle; closes the handle once exited.
    static bool UpdateStatus(ServerEntry& entry);

private:
    static CString BuildCommandLine(const ServerEntry& entry);

    // AttachConsole dance: ctrl events can only be delivered through a console,
    // and the caller (a GUI app) has none. Attaches to the child's hidden console,
    // ignores the signal for ourselves, sends CTRL_BREAK, detaches.
    static bool SendCtrlBreak(DWORD dwPid);
};
