#pragma once

#include "ServerEntry.h"
#include <vector>

class ConfigLoader
{
public:
    // Parses config.ini. Tolerates empty/skipped [ServiceN] sections.
    // Returns false (with strError) on fatal problems (missing file / bad MaxService / no entries).
    static bool Load(const CString& strPath, std::vector<ServerEntry>& arrOut, CString& strError);

private:
    static CString ReadString(const CString& strSection, const CString& strKey, const CString& strPath);
};
