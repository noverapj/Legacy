#include "StdAfx.h"
#include <intshcut.h>
#include ".\createshortcut.h"

BOOL CreateShortcut(LPCTSTR szObjPath,  
					LPCTSTR szLinkPath,  
					LPCTSTR szDesc /*= NULL*/,  
					LPCTSTR szArgument /*= NULL*/,  
					LPCTSTR szIconPath /*= NULL*/,  
					int iIconIndex /*= -1*/,  
					WORD wHotkey /*= 0*/,
					int nShowCmd /*= SW_SHOW*/)
{	
	// Validate parameters
	ASSERT(szObjPath);
	ASSERT(szLinkPath);

	HRESULT hRes;
	IUniformResourceLocator* purl = NULL;
	IShellLink* psl = NULL;
	IPersistFile* ppf = NULL;
	BOOL bRet = FALSE;

	::CoInitialize(NULL);

	TCHAR szDrive[_MAX_DRIVE], szDir[_MAX_PATH], szExt[_MAX_EXT];
	_tsplitpath(szLinkPath, szDrive, szDir, NULL, szExt);

	CString sExt = szExt;

	// Shortcut
	if (0 == sExt.CollateNoCase(_T(".lnk")))
	{
		hRes = ::CoCreateInstance(CLSID_ShellLink, NULL, CLSCTX_INPROC_SERVER, 
			IID_IShellLink, (LPVOID*)&psl);
		hRes = psl->QueryInterface(IID_IPersistFile, (LPVOID*)&ppf);
	}

	// URL Shortcut
	else if (0 == sExt.CollateNoCase(_T(".url")))
	{
		hRes = ::CoCreateInstance(CLSID_InternetShortcut, NULL, CLSCTX_INPROC_SERVER, 
			IID_IUniformResourceLocator, (LPVOID*)&purl);
		if (SUCCEEDED(hRes))
		{
			hRes = purl->QueryInterface(IID_IShellLink, (LPVOID*)&psl);
			if (SUCCEEDED(hRes))
			{
				hRes = purl->QueryInterface(IID_IPersistFile, (LPVOID*) &ppf);
			}
		}
	}
	else
	{
		// Shortcut file extention must be .lnk or .url
		::CoUninitialize();
		return FALSE;
	}

	if (SUCCEEDED(hRes))
	{
		hRes = psl->SetPath(szObjPath);
		if (szDesc)
		{
			psl->SetDescription(szDesc);
		}
		if (szArgument)
		{
			psl->SetArguments(szArgument);
		}
		if (szIconPath && iIconIndex >= 0)
		{
			psl->SetIconLocation(szIconPath, iIconIndex);
		}
		if (wHotkey)
		{
			psl->SetHotkey(wHotkey);
		}

		TCHAR szPath[_MAX_PATH];		
		_stprintf(szPath, _T("%s%s"), szDrive, szDir);
		psl->SetWorkingDirectory(szPath);
		psl->SetShowCmd(nShowCmd);

		USES_CONVERSION;
		hRes = ppf->Save(T2COLE(szLinkPath), TRUE);
		if (hRes != S_OK)
		{
			TRACE(_T("IPersistFile->Save() Error\n"));
		}
		else
		{
			bRet = TRUE;
		}
		ppf->Release();
		psl->Release();
		if (purl)	purl->Release();

		::CoUninitialize();
	}
	return bRet;
}
