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
};
