#pragma once

class CSystemInfo
{
protected:
	CString m_szSystemInfo;
	
protected:
	void SetMemoryInfo();
	void SetOS();
	void SetCPU();
	void SetHDD();

public:
	void SetupSystemInfo();
	CString &GetSystemInfo() { return m_szSystemInfo; }

public:
	enum OSType
	{
		OT_NONE  = 0, 
		OT_XP    = 1, 
		OT_VISTA = 2,
	};

	OSType GetOS(OUT OSVERSIONINFOEX &kVersionInfo,OUT char *szOSName, IN int iOSNameSize );
public:
	CSystemInfo(void);
	virtual ~CSystemInfo(void);
};
