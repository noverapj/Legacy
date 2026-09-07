#pragma once

#include "ServerEntry.h"
#include <vector>

class ConfigLoader
{
public:
    // Parses config.ini. Tolerates empty/skipped [ServiceN] sections.
    // Returns false (with strError) on fatal problems (missing file / bad MaxService / no entries).
    // rbWatchdogDefault: [common] Watchdog initial checkbox state.
    static bool Load(const CString& strPath, std::vector<ServerEntry>& arrOut, CString& strError,
                     bool& rbWatchdogDefault);

private:
    static CString ReadString(const CString& strSection, const CString& strKey, const CString& strPath);
};
