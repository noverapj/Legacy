#pragma once

// One managed server, loaded from config.ini ([ServiceN] section).
struct ServerEntry
{
    CString m_strName;      // display name
    CString m_strDir;       // working directory (spawn cwd; the server resolves its INI from here)
    CString m_strExePath;   // full path to the exe
    CString m_strIni;       // config file passed via "-c"
    int     m_nOrder = 0;   // Start All ordering (ascending)

    // Runtime state
    HANDLE  m_hProcess = nullptr;
    DWORD   m_dwPid = 0;
    bool    m_bRunning = false;

    // Watchdog
    bool    m_bWatchdog = true;         // eligible for auto-restart (per-service config)
    bool    m_bExpectStop = false;      // intentional stop in progress (not a crash)
    bool    m_bWatchdogDisabled = false; // tripped by crash-loop protection
    int     m_nCrashCount = 0;          // crashes this session (display column)
    DWORD   m_arrCrashTicks[3] = {};   // last 3 crash times (loop detection window)
    int     m_nCrashTickCount = 0;
};
