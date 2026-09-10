#pragma once

using namespace std;

typedef vector<string> vString;

typedef void (*PFUNC_DISPLAY) (LPVOID caller, bool isMaxCnt, int iCurrentCnt);

class CCopyFolder
{
private:
	vString    m_vFileList;
	vString    m_vNotCopyList;

private:
	void SetCreateDirectory(const char *szDir);
	bool CopyFileWithCreateFolder(const char *szSourceFile, const char *szDestFile);
	void FindFileRecursive(const char *szPath);

	bool IsNotCopyFile(const char* szFileName);

public:
	void AddNotCopyFileName(const char * szFileName);

	// CopyFolder("c:\\Program Files\\SurvivalProject\\sounds", "c:\\Program Files\\SurvivalProject\\new_sounds", ....);
	bool CopyFolder(const char *szSourceFolder, const char *szDestFolder ,const bool &bActive    
		          , PFUNC_DISPLAY pFunc_Display = NULL, LPVOID caller = NULL);
public:
	CCopyFolder(void);
	~CCopyFolder(void);
};
