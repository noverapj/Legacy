#ifndef __ioLocalKorea_h__
#define __ioLocalKorea_h__

#include "ioLocalParent.h"

class ioLocalKorea : public ioLocalParent
{
public:
	virtual ioLocalManager::LocalType GetType();
	virtual const char *GetTextListFileName();
	virtual const char *GetMemTextList();
	virtual void GetPacPassword( OUT char *szPassword, IN const int iPasswordSize, IN const int iPasswordType = 0 );
	virtual DWORD GetResourceIDBackBmp();
	virtual DWORD GetResourceIDOptimizeBtnBmp();
	virtual DWORD GetResourceIDStartBtnBmp();
	virtual DWORD GetResourceIDErrorBackBtnBmp();
	virtual DWORD GetResourceIDErrorSolutionBtnBmp();
	virtual DWORD GetResourceIDErrorExitBtnBmp();

	virtual const char *GetRegKey();
	virtual const char *GetFontName();
	virtual const char *GetErrorSolutionURL();

	virtual void  CreateShortcuts( const CString &rszRootDir, const CString &rszShortcutName, const CString &rszLoginURL );

	virtual bool IsFine();

public:
	ioLocalKorea(void);
	virtual ~ioLocalKorea(void);
};

#endif // __ioLocalKorea_h__