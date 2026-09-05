// FtpApp.h: interface for the CFtpApp class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_FTPAPP_H__33EB26BE_30EC_401B_82DF_CC7509C57524__INCLUDED_)
#define AFX_FTPAPP_H__33EB26BE_30EC_401B_82DF_CC7509C57524__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "ftp/FTPClient.h"

class CFtpApp  
{
private:	
	nsFTP::CFTPClient m_FTPClient;		

private:
	bool InitSocket();
	void CloseSocket();

public:
	bool connect(const char *iip,const char *id,const char *pw, int ip );	
	void close();

public:
	bool UploadFile(const CString &rszLocalPath, CString szServerPath, bool bPassive);
	bool DownloadFile( const CString &rszLocalPath, CString szServerPath , bool bPassive, OUT int* iRemoteSize = NULL );
	bool DeleteFile( const CString& rszServerPath );
	int  Abort();

public:
	bool isconnect();
	bool GetFileList( const CString &rszDir, OUT nsFTP::TSpFTPFileStatusVector &vList, bool bPassive);
	int GetLocalFileSize(const char *fname);
	bool GetTime(OUT CString &rszServerPath,OUT tm &rTime);

	public:
	CFtpApp();
	virtual ~CFtpApp();
};

#endif // !defined(AFX_FTPAPP_H__33EB26BE_30EC_401B_82DF_CC7509C57524__INCLUDED_)
