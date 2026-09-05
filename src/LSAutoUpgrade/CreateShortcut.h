#pragma once


//CreateShortcut(_T("C:\\Program Files\\Internet Explorer\\IEXPLORE.EXE"), 
//			   _T("C:\\IEXPLORE.lnk"));
//CreateShortcut(_T("http://www.kilojuliet.com"), _T("C:\\MyBlog.url"));

BOOL CreateShortcut(LPCTSTR szObjPath, LPCTSTR szLinkPath, LPCTSTR szDesc = NULL, LPCTSTR szArgument = NULL,
					LPCTSTR szIconPath = NULL, int iIconIndex = -1, WORD wHotkey = 0, int nShowCmd = SW_SHOW);