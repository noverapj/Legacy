#include "StdAfx.h"
#include "Utility.h"

using namespace std;

void Tokenize(const CString& str, TOKENS& tokens, const CString& delimiters)
{
	tokens.clear();

	int pos = 0;
	CString token;
	while((token  = str.Tokenize(delimiters, pos)) != _T(""))
	{
		tokens.push_back(token);
	}
}


static int CALLBACK BrowseForFolder_CallbackProc(HWND hwnd, UINT uMsg, LPARAM lParam, LPARAM lpData)
{
	if(uMsg == BFFM_INITIALIZED)
	{
		SendMessage(hwnd, BFFM_SETSELECTION, (WPARAM)TRUE, (LPARAM)lpData);
	}
	return 0;
}

BOOL SelectFolder(HWND hWnd, CString &folder)
{
	LPITEMIDLIST pidlBrowse;
	TCHAR         szFolder[MAX_PATH];

	BROWSEINFO brInfo;
	::ZeroMemory(&brInfo, sizeof(BROWSEINFO));
	brInfo.hwndOwner      = hWnd; // 윈도우 핸들
	brInfo.pidlRoot       = NULL;
	brInfo.pszDisplayName = NULL;
	brInfo.lpszTitle      = _T("설치파일이 있는 폴더를 선택하세요");
	brInfo.ulFlags        = BIF_RETURNONLYFSDIRS | BIF_STATUSTEXT; // 옵션 지정
	brInfo.lpfn           = BrowseForFolder_CallbackProc; //콜백함수
	brInfo.lParam         = (LPARAM)(LPCSTR)(LPCTSTR)(folder);

	::ZeroMemory(szFolder, sizeof(szFolder));

	pidlBrowse = ::SHBrowseForFolder(&brInfo); // 다이얼로그를 띄운다.
	if(pidlBrowse != NULL) 
	{
		::SHGetPathFromIDList(pidlBrowse, szFolder); // 패스를 얻어온다.
		folder = szFolder;
		return TRUE;
	}
	return FALSE;
}
