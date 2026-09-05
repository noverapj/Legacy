// LSMonitor.h : PROJECT_NAME 응용 프로그램에 대한 주 헤더 파일입니다.
//

#pragma once

#ifndef __AFXWIN_H__
	#error PCH에서 이 파일을 포함하기 전에 'stdafx.h'를 포함하십시오.
#endif

#include "resource.h"		// 주 기호


// CLSMonitorApp:
// 이 클래스의 구현에 대해서는 LSMonitor.cpp을 참조하십시오.
//

// Global 인자를 붙이면 사용자 전환등 계정이 변경되어도 중복 실행을 방지 한다.
#define APPMUTEXNAME                  "Global\\_LOSTSAGA_MONITOR_859494948392020949"
class ioLocalManager;
class CLSMonitorApp : public CWinApp
{
	HANDLE			m_h_mutex;
	ioLocalManager  *m_pLocalMgr;
public:
	CLSMonitorApp();

// 재정의
	public:
	virtual BOOL InitInstance();

// 구현

	DECLARE_MESSAGE_MAP()
	virtual int ExitInstance();
};

extern CLSMonitorApp theApp;
