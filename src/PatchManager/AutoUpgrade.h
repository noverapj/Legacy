// AutoUpgrade.h : main header file for the AUTOUPGRADE application
//

#if !defined(AFX_AUTOUPGRADE_H__13506E2A_C1B1_4280_8812_ABDF3B4426B0__INCLUDED_)
#define AFX_AUTOUPGRADE_H__13506E2A_C1B1_4280_8812_ABDF3B4426B0__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#ifndef __AFXWIN_H__
	#error include 'stdafx.h' before including this file for PCH
#endif

#include "resource.h"		// main symbols

#define	APPNAME                       "_LOSTSAGA_PATCHMANAGER_"
#define APPMUTEXNAME                  "_LOSTSAGA_PATCHMANAGER_748569521365987415930392049"
/////////////////////////////////////////////////////////////////////////////
// CAutoUpgradeApp:
// See AutoUpgrade.cpp for the implementation of this class
//

class ioLocalManager;

class CAutoUpgradeApp : public CWinApp
{
	HANDLE			m_h_mutex;
	ioLocalManager  *m_pLocalMgr;
public:
	CAutoUpgradeApp();
    
// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CAutoUpgradeApp)
	public:
	virtual BOOL InitInstance();
	//}}AFX_VIRTUAL

// Implementation

	//{{AFX_MSG(CAutoUpgradeApp)
		// NOTE - the ClassWizard will add and remove member functions here.
		//    DO NOT EDIT what you see in these blocks of generated code !
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};


/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_AUTOUPGRADE_H__13506E2A_C1B1_4280_8812_ABDF3B4426B0__INCLUDED_)
