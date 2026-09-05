#include "stdafx.h"
#include ".\systeminfoudpmsg.h"
#include "../LSMonitorDlg.h"

void CREQUEST_SYSTEM_INFO::Set()
{
	SetBasicHeader(sizeof(CREQUEST_SYSTEM_INFO), REQUEST_SYSTEM_INFO);
}

void CANSWER_SYSTEM_INFO::Set( int cpu, int memory, double cmemory, int c_hdd, int d_hdd )
{
	SetBasicHeader(sizeof(CANSWER_SYSTEM_INFO), ANSWER_SYSTEM_INFO);
	m_cpu     = cpu;
	m_memory  = memory;
	m_cmemory = cmemory;
	m_c_hdd   = c_hdd;
	m_d_hdd   = d_hdd;
}


void  CANSWER_SYSTEM_INFO::Process()
{
	if( CheckError() ) return;

	CLSMonitorDlg *pDlg = (CLSMonitorDlg*) AfxGetApp()->GetMainWnd();
	if(pDlg == NULL)
		return;

	char szIP[MAX_PATH]="";
	int iPort = 0;
	pDlg->GetUDPSocket().GetRecentIPPort(szIP, iPort);
	
	bool bUpdateList = false;
	SYSTEMINFO *pInfo = pDlg->GetSystemInfo( szIP , bUpdateList);
	if( !pInfo ) return;

	pInfo->iCPUUsed       = m_cpu;
	pInfo->iMemoryRemain  = m_memory;
	pInfo->dbMemoryUsed   = m_cmemory;
	pInfo->iCDriveRemain  = m_c_hdd;
	pInfo->iDDreiveRemain = m_d_hdd;
	pInfo->dwLastRcvTime  = timeGetTime();
	pInfo->bReaction      = true;

	if( bUpdateList )
		pDlg->UpdateSystemListCtrl(pInfo);
	pDlg->SetSystemSMSErrorToNormal( pInfo->wSMSArray );
}

void CANSWER_COPY::Process()
{
	if( CheckError() ) return;

	CLSMonitorDlg *pDlg = (CLSMonitorDlg*) AfxGetApp()->GetMainWnd();
	if(pDlg == NULL)
		return;

	char szIP[MAX_PATH]="";
	int iPort = 0;
	pDlg->GetUDPSocket().GetRecentIPPort(szIP, iPort);

	CControlDlg::CheckInfo kCheckInfo;
	kCheckInfo.m_szSystemIP  = szIP;
	kCheckInfo.m_iPort = iPort;
	pDlg->OnCopyResult( m_iType, m_iRequestType, m_iVersion, kCheckInfo );
}

void CANSWER_DELETE::Process()
{
	if( CheckError() ) return;

	CLSMonitorDlg *pDlg = (CLSMonitorDlg*) AfxGetApp()->GetMainWnd();
	if(pDlg == NULL)
		return;

	char szIP[MAX_PATH]="";
	int iPort = 0;
	pDlg->GetUDPSocket().GetRecentIPPort(szIP, iPort);

	CControlDlg::CheckInfo kCheckInfo;
	kCheckInfo.m_szSystemIP  = szIP;
	kCheckInfo.m_iPort = iPort;
	pDlg->OnDeleteResult( m_iType, m_iRequestType, m_iVersion, kCheckInfo );
}