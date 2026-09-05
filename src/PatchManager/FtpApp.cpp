// FtpApp.cpp: implementation of the CFtpApp class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "AutoUpgrade.h"
#include "FtpApp.h"


#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CFtpApp::CFtpApp() 
{

}

CFtpApp::~CFtpApp()
{

}

bool CFtpApp::InitSocket()
{
    WSADATA WOSAdata;
    if(WSAStartup(0x0002, &WOSAdata) != 0) 
        return false;
    return true;
}

void CFtpApp::CloseSocket()
{
	WSACleanup();
}


void CFtpApp::close()
{
	m_FTPClient.Logout();
	CloseSocket();
}

bool CFtpApp::connect( const char *iip, const char *id,const char *pw , int iPort )
{
	if(!InitSocket())
		return false;
	
	if( iPort == 0 ) iPort = 21;

	nsFTP::CLogonInfo logonInfo(iip, iPort, id, pw);
	if(!m_FTPClient.Login(logonInfo))
		return false;

	return true;
}

bool CFtpApp::isconnect()
{
	if(!m_FTPClient.IsConnected()) return false;

	return true;
}

bool CFtpApp::GetFileList( const CString &rszDir, OUT nsFTP::TSpFTPFileStatusVector &vList, bool bPassive )
{
	int iCnt = 100;
	while(iCnt > 0)
	{
		if(m_FTPClient.List((LPCTSTR)rszDir, vList, bPassive))
			break;
		iCnt--;
		Sleep(100);
	}

	if(iCnt == 0)
		return false;

	return true;
}


bool CFtpApp::UploadFile( const CString &rszLocalPath, CString szServerPath, bool bPassive )
{

 	if(!isconnect()) return false;
 
 	if(m_FTPClient.ChangeWorkingDirectory("/") != 0)
		return false;

	char dir[MAX_PATH]	= "";
	char fname[_MAX_FNAME] = "";
	char ext[_MAX_EXT] = "";
	::_splitpath((LPCTSTR)szServerPath,NULL,dir,fname,ext);

	CString szDir = dir;
	while(!szDir.IsEmpty())
	{
		CString szOneDir = szDir.SpanExcluding("\\");
		if(szOneDir.IsEmpty())
			continue;
		szDir = szDir.Right( szDir.GetLength() - ( szOneDir.GetLength() + 1));

		m_FTPClient.MakeDirectory((LPCTSTR)szOneDir);
		if(m_FTPClient.ChangeWorkingDirectory((LPCTSTR)szOneDir) != 0)
			return false;
	}

	szServerPath.Replace("\\", "/");

	int iCnt = 10;
	while(iCnt > 0)
	{
		m_FTPClient.Delete((LPCTSTR)szServerPath);
		m_FTPClient.UploadFile((LPCTSTR)rszLocalPath, (LPCTSTR)szServerPath, false, nsFTP::CRepresentation(nsFTP::CType::Image()), bPassive );
        long iSize = 0;
		if(m_FTPClient.FileSize((LPCTSTR)szServerPath, iSize) == 0) // ok
		{
			if(iSize == GetLocalFileSize((LPCTSTR)rszLocalPath) && iSize > 0)
				break;
		}
		iCnt--;
		Sleep(100);
	}

	if(iCnt == 0)
		return false;

	return true;
}


bool CFtpApp::DownloadFile( const CString &rszLocalPath, CString szServerPath , bool bPassive , OUT int* iRemoteSize  )
{
	if(!isconnect()) return false;

	szServerPath.Replace("\\", "/");

	int iCnt = 10;
	while(iCnt > 0)
	{
		m_FTPClient.DownloadFile((LPCTSTR)szServerPath, (LPCTSTR)rszLocalPath, nsFTP::CRepresentation(nsFTP::CType::Image()), bPassive);
		long iSize = 0;

		if(m_FTPClient.FileSize((LPCTSTR)szServerPath, iSize) == 0) // ok
		{
			if(iSize == GetLocalFileSize((LPCTSTR)rszLocalPath) && iSize > 0)
				break;
		}
		if( iSize == 0 )
		{
			*iRemoteSize = iSize;
			return false;
		}
		iCnt--;
		Sleep(100);
	}

	if(iCnt == 0)
		return false;

	return true;
}

int CFtpApp::GetLocalFileSize( const char *fname )
{
	HANDLE	handle;
	handle = CreateFile(fname, GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
	if(handle == INVALID_HANDLE_VALUE)
		return -1;

	int size = ::GetFileSize(handle, NULL);
	CloseHandle(handle);
	return size;
}

bool CFtpApp::GetTime( OUT CString &rszServerPath,OUT tm &rTime )
{
	if(!isconnect()) return false;

	rszServerPath.Replace("\\", "/");

	int iCnt = 100;
	while(iCnt > 0)
	{
		if(m_FTPClient.FileModificationTime((LPCTSTR)rszServerPath, rTime) == 0) // ok
			break;
		iCnt--;
		Sleep(100);
	}

	if(iCnt == 0)
		return false;

	return true;
}

int CFtpApp::Abort()
{
	return m_FTPClient.Abort();
}

bool CFtpApp::DeleteFile( const CString& rszServerPath )
{
	if(!isconnect()) return false;

	if(m_FTPClient.ChangeWorkingDirectory("/") != 0)
		return false;

	char dir[MAX_PATH]	= "";
	char fname[_MAX_FNAME] = "";
	char ext[_MAX_EXT] = "";
	::_splitpath((LPCTSTR)rszServerPath,NULL,dir,fname,ext);

	CString szDir = dir;
	while(!szDir.IsEmpty())
	{
		CString szOneDir = szDir.SpanExcluding("\\");
		if(szOneDir.IsEmpty())
			continue;
		szDir = szDir.Right( szDir.GetLength() - ( szOneDir.GetLength() + 1));

		m_FTPClient.MakeDirectory((LPCTSTR)szOneDir);
		if(m_FTPClient.ChangeWorkingDirectory((LPCTSTR)szOneDir) != 0)
			return false;
	}

	int iCnt = 10;
	while(iCnt > 0)
	{
		m_FTPClient.Delete((LPCTSTR)rszServerPath);

		long iSize = 0;
		int iReturn = m_FTPClient.FileSize((LPCTSTR)rszServerPath, iSize);
		if( iReturn == 1) // NOT_OK    The command was not accepted
			break;
		iCnt--;
		Sleep(100);
	}

	if(iCnt == 0)
		return false;

	return true;
}