#include "StdAfx.h"
#include "cpu_info.h"
#include "HTTPManager.h"
#include ".\systeminfo.h"

#include <strsafe.h>

CSystemInfo::CSystemInfo(void)
{
}

CSystemInfo::~CSystemInfo(void)
{
}

void CSystemInfo::SetupSystemInfo()
{
	__try
	{
		m_szSystemInfo="";
		SetOS();
		m_szSystemInfo += "\r\n";
		SetCPU();
		m_szSystemInfo += "\r\n";
		SetMemoryInfo();
		m_szSystemInfo += "\r\n";
		SetHDD();
	}
	__except(1)
	{

	}
}

void CSystemInfo::SetMemoryInfo()
{
	MEMORYSTATUS memoryStatus;
	ZeroMemory(&memoryStatus,sizeof(MEMORYSTATUS));
	memoryStatus.dwLength = sizeof (MEMORYSTATUS);
	::GlobalMemoryStatus (&memoryStatus);

	m_szSystemInfo += "Ram :";

	char szTemp[MAX_PATH]="";
	StringCbPrintf(szTemp, sizeof(szTemp), " Total:%ldMB / Avail:%ldMB / Load:%ld%% "
		                                ,(DWORD) (memoryStatus.dwTotalPhys/MEGA) 
										,(DWORD) (memoryStatus.dwAvailPhys/MEGA)
										,memoryStatus.dwMemoryLoad);
	m_szSystemInfo += szTemp;
}



void CSystemInfo::SetOS()
{
	m_szSystemInfo += "OS :";

	OSVERSIONINFOEX versionInfo;
	char szOSName[MAX_PATH]="";
	GetOS( versionInfo, szOSName, sizeof(szOSName) );
	
	char szTemp[MAX_PATH*2]="";
	memset(szTemp, 0 , sizeof(szTemp));
	StringCbPrintf(szTemp, sizeof(szTemp),  " %s / BuildNumber:%d/ MajorVersion:%d/ MinorVersion:%d/ PlatformId:%d/ "
		                                    "CSDVersion:%s/ ServicePackMajor:%d/ ServicePackMinor:%d/ ProductType:%d"
					, szOSName
					, versionInfo.dwBuildNumber
					, versionInfo.dwMajorVersion
					, versionInfo.dwMinorVersion
					, versionInfo.dwPlatformId
					, versionInfo.szCSDVersion
					, versionInfo.wServicePackMajor
					, versionInfo.wServicePackMinor
					, versionInfo.wProductType);

	m_szSystemInfo += szTemp;
}



void CSystemInfo::SetCPU()
{
	m_szSystemInfo += "CPU :";
	char szTemp[MAX_PATH*2]="";
	CPUInfo cpu;
	if (cpu.DoesCPUSupportCPUID()) 
	{
		StringCbPrintf(szTemp, sizeof(szTemp), " %s / ID:%s /TYPE:%s /FamilyID:%s /ModelID:%s /Code:%s"
			          ,cpu.GetExtendedProcessorName()
			          ,cpu.GetVendorID()
					  ,cpu.GetTypeID()
					  ,cpu.GetFamilyID()
					  ,cpu.GetModelID()
					  ,cpu.GetSteppingCode());
	}

	m_szSystemInfo += szTemp;
}

void CSystemInfo::SetHDD()
{
	m_szSystemInfo += "HDD :";
	ULARGE_INTEGER availDisk;
	ULARGE_INTEGER total;

	if(!GetDiskFreeSpaceEx(NULL, &availDisk, &total, NULL))
		return;

	char szTemp[MAX_PATH]="";
	StringCbPrintf(szTemp, sizeof(szTemp), " Total:%dMB / Remain:%dMB", (int)(total.QuadPart/MEGA), (int)(availDisk.QuadPart/MEGA));
	m_szSystemInfo += szTemp;
}

CSystemInfo::OSType CSystemInfo::GetOS(OUT OSVERSIONINFOEX &kVersionInfo, OUT char *szOSName, IN int iOSNameSize )
{
	OSType kOSType = OT_NONE;

	ZeroMemory(&kVersionInfo, sizeof(OSVERSIONINFOEX));
	kVersionInfo.dwOSVersionInfoSize = sizeof (OSVERSIONINFOEX);
	GetVersionEx ((OSVERSIONINFO *)&kVersionInfo);

	if(kVersionInfo.dwPlatformId == VER_PLATFORM_WIN32_WINDOWS)	// 98, Me
	{
		if(kVersionInfo.dwMinorVersion == 0)
		{
			if ( kVersionInfo.szCSDVersion[1] == 'C' || kVersionInfo.szCSDVersion[1] == 'B' )
				StringCbCopy(szOSName,iOSNameSize, "Microsoft Windows 95 OSR2 ");	
			else
				StringCbCopy(szOSName,iOSNameSize, "Microsoft Windows 95 ");
		}
		else if(kVersionInfo.dwMinorVersion == 10)
		{
			if ( kVersionInfo.szCSDVersion[1] == 'A' )
				StringCbCopy(szOSName,iOSNameSize, "Microsoft Windows 98 SE ");			
			else
				StringCbCopy(szOSName,iOSNameSize, "Microsoft Windows 98");			
		}
		else if(kVersionInfo.dwMinorVersion == 90)
			StringCbCopy(szOSName,iOSNameSize, "Microsoft Windows Millennium Edition");
	}
	else if( kVersionInfo.dwPlatformId == VER_PLATFORM_WIN32_NT ) //	NT, 2000, XP, Server
	{
		if( kVersionInfo.dwMajorVersion == 5)
		{
			if(kVersionInfo.dwMinorVersion == 0)
				StringCbCopy(szOSName,iOSNameSize,"Microsoft Windows 2000");
			else if(kVersionInfo.dwMinorVersion == 1)
			{
				StringCbCopy(szOSName,iOSNameSize,"Microsoft Windows XP");
				kOSType = OT_XP;
			}
			else if(kVersionInfo.dwMinorVersion == 2)
				StringCbCopy(szOSName,iOSNameSize,"Microsoft Windows Server 2003 Family");
		}
		else if( kVersionInfo.dwMajorVersion == 6  && kVersionInfo.dwMinorVersion == 0 )
		{
			StringCbCopy(szOSName,iOSNameSize,"Microsoft Windows Vista");
			kOSType = OT_VISTA;
		}
		else		
			StringCbCopy(szOSName,iOSNameSize,"Microsoft Windows NT");
	}
	else		// Maybe Win 3.1
	{
		StringCbCopy(szOSName,iOSNameSize, "Microsoft Win32s");
	}

	return kOSType;
}