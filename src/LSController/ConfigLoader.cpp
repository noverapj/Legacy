#include "pch.h"
#include "ConfigLoader.h"

CString ConfigLoader::ReadString(const CString& strSection, const CString& strKey, const CString& strPath)
{
    TCHAR buf[MAX_PATH] = {};
    ::GetPrivateProfileStringW(strSection, strKey, _T(""), buf, MAX_PATH, strPath);
    return CString(buf);
}

bool ConfigLoader::Load(const CString& strPath, std::vector<ServerEntry>& arrOut, CString& strError)
{
    arrOut.clear();

    const CString strMax = ReadString(_T("common"), _T("MaxService"), strPath);
    const int nMax = _ttoi(strMax);
    if (strMax.IsEmpty() || nMax <= 0 || nMax > 100)
    {
        strError.Format(_T("%s : [common] MaxService is missing or invalid."), strPath.GetString());
        return false;
    }

    for (int i = 1; i <= nMax; ++i)
    {
        CString strSection;
        strSection.Format(_T("Service%d"), i);

        CString strDir  = ReadString(strSection, _T("Dir"), strPath);
        CString strExe  = ReadString(strSection, _T("Exe"), strPath);
        CString strIni  = ReadString(strSection, _T("INI"), strPath);
        CString strName = ReadString(strSection, _T("Name"), strPath);
        const int nOrder = _ttoi(ReadString(strSection, _T("Order"), strPath));

        // Empty/skipped sections are tolerated
        if (strExe.IsEmpty())
            continue;

        ServerEntry entry;
        entry.m_nOrder = nOrder;
        entry.m_strIni = strIni;

        // Exe: absolute when it contains a drive colon, otherwise relative to Dir
        if (strExe.Find(':') >= 0)
            entry.m_strExePath = strExe;
        else if (!strDir.IsEmpty())
            entry.m_strExePath = strDir + _T("\\") + strExe;
        else
            entry.m_strExePath = strExe;

        // Dir: explicit, or derived from the exe location
        if (strDir.IsEmpty())
        {
            const int nSlash = entry.m_strExePath.ReverseFind(_T('\\'));
            if (nSlash > 0)
                strDir = entry.m_strExePath.Left(nSlash);
        }
        entry.m_strDir = strDir;

        // Display name: explicit > exe file name (without extension)
        if (strName.IsEmpty())
        {
            const int nSlash = entry.m_strExePath.ReverseFind(_T('\\'));
            CString strFile = (nSlash >= 0) ? entry.m_strExePath.Mid(nSlash + 1) : entry.m_strExePath;
            const int nDot = strFile.ReverseFind(_T('.'));
            strName = (nDot > 0) ? strFile.Left(nDot) : strFile;
        }
        entry.m_strName = strName;

        arrOut.push_back(entry);
    }

    if (arrOut.empty())
    {
        strError.Format(_T("%s : no [ServiceN] entries with Exe defined."), strPath.GetString());
        return false;
    }
    return true;
}
