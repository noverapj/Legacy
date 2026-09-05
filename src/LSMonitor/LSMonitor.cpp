// LSMonitor.cpp : 응용 프로그램에 대한 클래스 동작을 정의합니다.
//

#include "stdafx.h"
#include "LSMonitor.h"
#include "LSMonitorDlg.h"
#include ".\lsmonitor.h"
#include "SMS/SMSRuleManager.h"
#include "local\iolocalmanager.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CLSMonitorApp

BEGIN_MESSAGE_MAP(CLSMonitorApp, CWinApp)
	ON_COMMAND(ID_HELP, CWinApp::OnHelp)
END_MESSAGE_MAP()


// CLSMonitorApp 생성

CLSMonitorApp::CLSMonitorApp()
{
	// TODO: 여기에 생성 코드를 추가합니다.
	// InitInstance에 모든 중요한 초기화 작업을 배치합니다.
	m_h_mutex        = NULL;
	m_pLocalMgr      = NULL;
}


// 유일한 CLSMonitorApp 개체입니다.

CLSMonitorApp theApp;


// CLSMonitorApp 초기화

BOOL CLSMonitorApp::InitInstance()
{
	CWinApp::InitInstance();

	// 표준 초기화
	// 이들 기능을 사용하지 않고 최종 실행 파일의 크기를 줄이려면
	// 아래에서 필요 없는 특정 초기화 루틴을 제거해야 합니다.
	// 해당 설정이 저장된 레지스트리 키를 변경하십시오.
	// TODO: 이 문자열을 회사 또는 조직의 이름과 같은
	// 적절한 내용으로 수정해야 합니다.
	SetRegistryKey(_T("로컬 응용 프로그램 마법사에서 생성한 응용 프로그램"));

	m_pLocalMgr = new ioLocalManager;
	if( m_pLocalMgr )
		m_pLocalMgr->Init();

	m_h_mutex = CreateMutex(NULL, true, APPMUTEXNAME);
	if(GetLastError() == ERROR_ALREADY_EXISTS)			
	{
		SAFEDELETE(m_pLocalMgr);
		return FALSE;
	}

	CSMSRuleManager::GlobalInit();

	CLSMonitorDlg dlg;
	m_pMainWnd = &dlg;
	INT_PTR nResponse = dlg.DoModal();
	if (nResponse == IDOK)
	{
		// TODO: 여기에 대화 상자가 확인을 눌러 없어지는 경우 처리할
		// 코드를 배치합니다.
	}
	else if (nResponse == IDCANCEL)
	{
		// TODO: 여기에 대화 상자가 취소를 눌러 없어지는 경우 처리할
		// 코드를 배치합니다.
	}

	if(m_h_mutex != NULL)
		ReleaseMutex(m_h_mutex);

	SAFEDELETE(m_pLocalMgr);

	// 대화 상자가 닫혔으므로 응용 프로그램의 메시지 펌프를 시작하지 않고
	// 응용 프로그램을 끝낼 수 있도록 FALSE를 반환합니다.
	return FALSE;
}

int CLSMonitorApp::ExitInstance()
{
	// TODO: 여기에 특수화된 코드를 추가 및/또는 기본 클래스를 호출합니다.
	CSMSRuleManager::GlobalEnd();
	return CWinApp::ExitInstance();
}
