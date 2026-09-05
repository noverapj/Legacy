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

#define NORMAL_EXCUTE_CHECK           "EDEW3940FVDP4950"

#define	APPNAME                       "_LOSTSAGA_AUTOUPGRADE_"
#define APPMUTEXNAME                  "Global\\_LOSTSAGA_AUTOUPGRADE_748569521365987415930392049"
#define APP_ERROR_NAME                "LostSaga_AutoUpgrade - ERROR"

#define CLIENT_CLASS_NAME             "Lost Saga in Timegate - Client" // 수정 금지 client 통신때 사용
#define CLIENTMUTEXNAME               "Global\\_LOSTSAGA_CLIENT_3894738393994848484939308770494"

/////////////////////////////////////////////////////////////////////////////
// CAutoUpgradeApp:
// See AutoUpgrade.cpp for the implementation of this class
//

class ioStringManager;
class ioLocalManager;
class ioChannelingNodeManager;
class ioHttpManager;
class CImpIDispatch;
class CAutoUpgradeApp : public CWinApp
{
	ioStringManager *m_pStringMgr;
	ioLocalManager  *m_pLocalMgr;
	ioHttpManager   *m_pHttpManager;
	ioChannelingNodeManager *m_pChannelingMgr;

	HANDLE			m_h_mutex;
	BOOL            IsDuplication();
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
