#ifndef __ioLocalJapan_h__
#define __ioLocalJapan_h__

#include "ioLocalParent.h"

class ioLocalJapan : public ioLocalParent
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

public:
	ioLocalJapan(void);
	virtual ~ioLocalJapan(void);
};

#endif // __ioLocalJapan_h__