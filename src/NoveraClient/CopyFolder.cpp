#include "StdAfx.h"
#include ".\copyfolder.h"

CCopyFolder::CCopyFolder(void)
{
	m_vFileList.reserve(1000);
	m_vNotCopyList.reserve(1000);
}

CCopyFolder::~CCopyFolder(void)
{
	m_vFileList.clear();
	m_vNotCopyList.clear();
}

void CCopyFolder::SetCreateDirectory( const char *szDir )
{
	if(szDir == NULL) return;

	const char *pDir = szDir;
	char createDir[MAX_PATH] = "";
	int len = 0;
	while (*pDir != '\0') 
	{
		if( *pDir=='/' || 
			*pDir=='\\' )
		{
			CreateDirectory(createDir,NULL);			
		}
		if(len >= MAX_PATH)
			break;
		createDir[len++] = *pDir;
		pDir++;
	}
	CreateDirectory(createDir,NULL);
}

bool CCopyFolder::CopyFileWithCreateFolder( const char *szSourceFile, const char *szDestFile )
{
	char drive[MAX_PATH]="";
	char dir[MAX_PATH]	= "";
	::_splitpath(szDestFile,drive,dir,NULL,NULL);

	char szRealDir[MAX_PATH]="";
	StringCbPrintf(szRealDir, sizeof(szRealDir), "%s%s", drive, dir);
	SetCreateDirectory(szRealDir);

	SetFileAttributes(szDestFile,FILE_ATTRIBUTE_NORMAL);
	::DeleteFile(szDestFile);

	SetFileAttributes(szSourceFile,FILE_ATTRIBUTE_NORMAL);
	if(!::CopyFile(szSourceFile, szDestFile, FALSE)) 
		return false;

	return true;
}

void CCopyFolder::FindFileRecursive(const char *szPath)
{
	HANDLE			hSrch;
	WIN32_FIND_DATA wfd;
	char			fname[MAX_PATH];
	BOOL			bResult = TRUE;
	char			drive[_MAX_DRIVE];
	char			dir[MAX_PATH];
	char			newpath[MAX_PATH];	

	hSrch = FindFirstFile(szPath, &wfd);

	if(hSrch == INVALID_HANDLE_VALUE)
		return;

	while(bResult)
	{
		_splitpath(szPath, drive, dir, NULL, NULL);
		if(wfd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
		{
			if(wfd.cFileName[0] != '.')
			{
				StringCbPrintf(newpath, sizeof(newpath), "%s%s%s\\*.*",drive, dir, wfd.cFileName);
				FindFileRecursive(newpath);
			}
		}
		else
		{			
			StringCbPrintf(fname, sizeof(fname),  "%s%s%s", drive, dir, wfd.cFileName);
			m_vFileList.push_back(fname);	
		}
		bResult = FindNextFile(hSrch, &wfd);
	}
	FindClose(hSrch);
}

bool CCopyFolder::CopyFolder( const char *szSourceFolder, const char *szDestFolder ,const bool &bActive    
							 , PFUNC_DISPLAY pFunc_Display /*= NULL*/, LPVOID caller /*= NULL*/ )
{
	m_vFileList.clear();
	FindFileRecursive(szSourceFolder);

	if(m_vFileList.empty())
		return false;

	int iMax = m_vFileList.size();
	if(pFunc_Display != NULL)
		pFunc_Display(caller, true, iMax);
	for (int i = 0; i < iMax ; i++)
	{
		if(!bActive)
			return true;

		if(IsNotCopyFile(m_vFileList[i].c_str()))
			continue;

		// new name
		string szDestName = szDestFolder;
		int iSourceFileStart = strlen(szSourceFolder);
		int iSourceFileMax = m_vFileList[i].size();
		for (int iChar = iSourceFileStart; iChar < iSourceFileMax ; iChar++)
		{
			szDestName += m_vFileList[i].at(iChar);
		}

		if(!CopyFileWithCreateFolder(m_vFileList[i].c_str(), szDestName.c_str()))
			return false;
		if(pFunc_Display != NULL) 
			pFunc_Display(caller, false, i+1);
	}
	
	m_vNotCopyList.clear();
	return true;
}

void CCopyFolder::AddNotCopyFileName( const char * szFileName )
{
	if(strlen(szFileName) >= MAX_PATH*2)
		return;

	char szLowerCase[MAX_PATH*2]="";
	StringCbCopy(szLowerCase, sizeof(szLowerCase), szFileName);
	strlwr(szLowerCase);
	m_vNotCopyList.push_back(szLowerCase);
}

bool CCopyFolder::IsNotCopyFile( const char* szFileName )
{
	char fname[_MAX_FNAME]	= "";
	char ext[_MAX_EXT]		= "";		
	::_splitpath(szFileName,NULL,NULL,fname,ext);
	strlwr(fname);
	strlwr(ext);

	string szCheckFileName = fname;
	szCheckFileName += ext;

	for(vString::const_iterator it = m_vNotCopyList.begin(); it != m_vNotCopyList.end(); ++it)
	{
		if( (*it).find(szCheckFileName) != string::npos )
		{
			return true;
		}
	}

	return false;
}