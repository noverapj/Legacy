
#include "stdafx.h"
#include ".\smsrulemanager.h"

CSMSRuleManager::CSMSRuleManager(void)
{
	// ini
	m_MaxSentSMSCnt = 5;
	m_MaxSendFailCnt = 5;
	
	m_dwWaitTimeForError = 120000; // 2Ка
	m_dwWaitTimeForSendSMS = 300000; // 5Ка
	
	m_szSMSMent=DEFAULT_SMS_MENT;

	m_vCStringPhoneNumber.reserve(20);
	//

	m_iReservedWeek = 0;
	m_iReservedStartHour = 4;
	m_iReservedEndHour = 4;
	m_bReserveStart = false;

	m_szErrorInfo = "";

	m_ParentHwnd = NULL;

	m_iPreYearMonth = 0;

	m_SMSSender.Init();
	LoadINI();
}

CSMSRuleManager::~CSMSRuleManager(void)
{
	m_vCStringPhoneNumber.clear();
	m_vSMSRuleInfo.clear();
}

void CSMSRuleManager::_OnTimer()
{
	CheckReservedStop();

	for (int i = 0; i < m_iMaxSmsRuleInfo; i++)
	{
		_OnTimerSub(i);
	}
}

void CSMSRuleManager::SetSequenceSendSMS( const int iArray )
{
	if(!COMPARE(iArray, 0, m_iMaxSmsRuleInfo))
		return;

	SYSTEMTIME st;
	GetLocalTime(&st);

	int iCurYearMonth = ( st.wYear*100 ) + st.wMonth;
	if( m_iPreYearMonth == iCurYearMonth ) return;
	m_iPreYearMonth = iCurYearMonth;

	CString szKeyName;
	if( st.wMonth%2 == 1 ) // ШІМіДо
		szKeyName = "CellPhoneForm1";
	else // ТІМіДо
		szKeyName = "CellPhoneForm2";

	m_vCStringPhoneNumber.clear();

	for (int i = 0; i < MAX_VECTOR_SIZE;i ++)
	{
		char szTemp[MAX_PATH]="";
		char szWholeKeyName[MAX_PATH]="";
		sprintf(szWholeKeyName, "%s_%d", (LPCTSTR)szKeyName, i+1);
		m_INILoader.LoadString(szWholeKeyName, "", szTemp, sizeof(szTemp));
		if(strcmp(szTemp, "") == 0)
			break;
		m_vCStringPhoneNumber.push_back(szTemp);
	}
	m_vSMSRuleInfo[iArray].m_iCurCellPhoneArray = 0;
	m_vSMSRuleInfo[iArray].m_iCurSentSMSCnt = 1;
}

void CSMSRuleManager::LoadINI()
{
	char szPath[MAX_PATH*2]="";
	GetCurrentDirectory(sizeof(szPath), szPath);
	strcat(szPath, "\\config.ini");
	m_INILoader.SetFileName(szPath);

	m_INILoader.SetTitle("SMSRule");

	m_MaxSentSMSCnt = m_INILoader.LoadInt("MaxSentSMSCnt", 5);
	m_MaxSendFailCnt = m_INILoader.LoadInt("MaxSendFailCnt", 5);
	m_dwWaitTimeForError = m_INILoader.LoadInt("WaitTimeForError", 120000);
	m_dwWaitTimeForSendSMS = m_INILoader.LoadInt("WaitTimeForSendSMS", 300000);

	char szMent[MAX_PATH*2]="";
	m_INILoader.LoadString("SMSMent", DEFAULT_SMS_MENT , szMent, sizeof(szMent));
	
	m_szSMSMent = szMent;
	const int iMaxINIMent = 50;
	if(m_szSMSMent.GetLength() >= iMaxINIMent)
	{
		m_szSMSMent = m_szSMSMent.Left( m_szSMSMent.GetLength() - ( m_szSMSMent.GetLength() - iMaxINIMent));
	}
	m_iReservedWeek = m_INILoader.LoadInt("ReservedWeek", 2);
	m_iReservedStartHour = m_INILoader.LoadInt("ReservedStartHour", 4);
	m_iReservedEndHour = m_INILoader.LoadInt("ReservedEndHour", 4);
}

void CSMSRuleManager::ClearStates( const int iArray )
{
	if(!COMPARE(iArray, 0, m_iMaxSmsRuleInfo))
		return;

	m_vSMSRuleInfo[iArray].m_iCurSentSMSCnt = 0;
	m_vSMSRuleInfo[iArray].m_iCurSendFailCnt = 0;
	m_vSMSRuleInfo[iArray].m_iCurCellPhoneArray = 0;
	m_vSMSRuleInfo[iArray].m_eState = NORMAL;
	m_vSMSRuleInfo[iArray].m_dwSetCheckTime = 0;
}

void CSMSRuleManager::SendSMS( const int iArray )
{
	if(!COMPARE(iArray, 0, m_iMaxSmsRuleInfo))
		return;

	SYSTEMTIME st;
	GetLocalTime(&st);
	CString szAddDateMent;
	szAddDateMent.Format("(%s)%s %02d%02d%02d%02d%02d", m_szErrorInfo , m_szSMSMent
		                ,st.wMonth, st.wDay, st.wHour, st.wMinute, st.wSecond);


	SMSRULEINFO &kCurSMSRuleInfo = m_vSMSRuleInfo[iArray];
	bool isFailSend = true;
	for (int i = 0; i < m_MaxSendFailCnt; i++)
	{
		LOG.PrintTimeAndLog(0, "SEND SMS:%d|%s|%d|%d|%d", iArray, GetCurNumber(kCurSMSRuleInfo.m_iCurCellPhoneArray)
			, kCurSMSRuleInfo.m_iCurCellPhoneArray,kCurSMSRuleInfo.m_iCurSentSMSCnt, i);

		if( m_SMSSender.SendSMS("ioenter", "tjvmqwer", "lostsaga.com", "20820780"
			                    ,GetCurNumber(kCurSMSRuleInfo.m_iCurCellPhoneArray),szAddDateMent) )
		{
			isFailSend = false;
			break;
		}
		Sleep(100);
	}
	
	// next cellphone
	if(isFailSend)
	{
		kCurSMSRuleInfo.m_dwSetCheckTime = (timeGetTime() - m_dwWaitTimeForSendSMS);
		kCurSMSRuleInfo.m_iCurSentSMSCnt = m_MaxSentSMSCnt;
	}
}

void CSMSRuleManager::SetFirstError( const int iArray )
{
	if(!COMPARE(iArray, 0, m_iMaxSmsRuleInfo))
		return;

	if(m_vSMSRuleInfo[iArray].m_eState == NORMAL)
	{
		m_vSMSRuleInfo[iArray].m_dwSetCheckTime = timeGetTime();
		m_vSMSRuleInfo[iArray].m_eState = COUNTING_ERROR;
		LOG.PrintTimeAndLog(0, "Error %d", iArray);
	}
}

void CSMSRuleManager::SetStopMonitering( DWORD dwStopTime, const int iArray )
{
	if(!COMPARE(iArray, 0, m_iMaxSmsRuleInfo))
		return;
	m_vSMSRuleInfo[iArray].m_dwWaitTimeForReStart = dwStopTime;
	m_vSMSRuleInfo[iArray].m_dwSetCheckTime = timeGetTime();
	m_vSMSRuleInfo[iArray].m_eState = STOPING;
}

void CSMSRuleManager::SetErrorToNormal( const int iArray )
{
	if(!COMPARE(iArray, 0, m_iMaxSmsRuleInfo))
		return;
	if(m_vSMSRuleInfo[iArray].m_eState == COUNTING_ERROR || m_vSMSRuleInfo[iArray].m_eState == SENDING_SMS)
	{
		ClearStates(iArray);
		LOG.PrintTimeAndLog(0, "Auto Start-Network:%d", iArray);
	}
}

void CSMSRuleManager::SetInfo( const int iMaxArray, HWND parentHwnd )
{
	m_vSMSRuleInfo.clear();
	for (int i = 0; i < iMaxArray ; i++)
	{
		SMSRULEINFO createSMSRuleInfo;
		m_vSMSRuleInfo.push_back(createSMSRuleInfo);
	}

	m_iMaxSmsRuleInfo = (int)m_vSMSRuleInfo.size();
	m_ParentHwnd      = parentHwnd;
}

void CSMSRuleManager::CheckReservedStop()
{
	SYSTEMTIME st;
	GetLocalTime(&st);

	if(!m_bReserveStart 
	 && m_iReservedWeek == st.wDayOfWeek 
	 && m_iReservedStartHour == st.wHour)
	{
		m_bReserveStart = true;
		int iMSeconds = 3600000 * (m_iReservedEndHour - m_iReservedStartHour);
		if(iMSeconds > 0)
		{
			for (int i = 0; i < m_iMaxSmsRuleInfo; i++)
			{
				m_vSMSRuleInfo[i].m_dwWaitTimeForReStart = iMSeconds;
				m_vSMSRuleInfo[i].m_eState = STOPING;
				m_vSMSRuleInfo[i].m_dwSetCheckTime = timeGetTime();
				if(m_ParentHwnd != NULL)
					::SendMessage(m_ParentHwnd, WM_TO_PARENT_STOP, i, 0);
				LOG.PrintTimeAndLog(0, "Auto Stop:%d", i);
			}
		}
	}

	if(m_bReserveStart
	&& m_iReservedWeek == st.wDayOfWeek 
	&& m_iReservedEndHour == st.wHour)
	{
		m_bReserveStart = false;

		for (int i = 0; i < m_iMaxSmsRuleInfo; i++)
		{
			if(m_vSMSRuleInfo[i].m_eState == STOPING)
			{
				ClearStates(i);
				if(m_ParentHwnd != NULL)
					::SendMessage(m_ParentHwnd, WM_TO_PARENT_NORMAL, i, 0);
				LOG.PrintTimeAndLog(0, "Auto Start-Resrved:%d", i);
			}
		}
	}
}

SMSRULESTATE CSMSRuleManager::GetState( const int iArray ) const
{
	if(!COMPARE(iArray, 0, m_iMaxSmsRuleInfo))
		return NORMAL;

	return m_vSMSRuleInfo[iArray].m_eState;
}

DWORD CSMSRuleManager::GetWaitTimeForStop( const int iArray ) const
{
	if(!COMPARE(iArray, 0, m_iMaxSmsRuleInfo))
		return 0;

	return m_vSMSRuleInfo[iArray].m_dwWaitTimeForReStart;
}

void CSMSRuleManager::_OnTimerSub( const int iArray )
{
	if(!COMPARE(iArray, 0, m_iMaxSmsRuleInfo))
		return;

	SMSRULEINFO &kCurSMSRuleInfo = m_vSMSRuleInfo[iArray];
	if(kCurSMSRuleInfo.m_eState == COUNTING_ERROR)
	{
		if( (timeGetTime() - kCurSMSRuleInfo.m_dwSetCheckTime) >= m_dwWaitTimeForError )
		{
			LOG.PrintTimeAndLog(0, "ERROR ON TIMER CHECK:%d", iArray);
			kCurSMSRuleInfo.m_dwSetCheckTime = timeGetTime();
			kCurSMSRuleInfo.m_iCurSentSMSCnt++;
			kCurSMSRuleInfo.m_eState = SENDING_SMS;
			SetSequenceSendSMS(iArray);
			SendSMS(iArray);
			if(m_MaxSentSMSCnt == 1)
				NextCellPhoneAndCheckStop(iArray);
		}
	}
	else if(kCurSMSRuleInfo.m_eState == SENDING_SMS)
	{
		if( (timeGetTime() - kCurSMSRuleInfo.m_dwSetCheckTime) >= m_dwWaitTimeForSendSMS )
		{
			kCurSMSRuleInfo.m_iCurSentSMSCnt++;
			kCurSMSRuleInfo.m_dwSetCheckTime = timeGetTime();
			SetSequenceSendSMS(iArray);
			SendSMS(iArray);

			if(kCurSMSRuleInfo.m_iCurSentSMSCnt >= m_MaxSentSMSCnt)
				NextCellPhoneAndCheckStop(iArray);
		}

	}
	else if(kCurSMSRuleInfo.m_eState == STOPING)
	{
		if( (timeGetTime() - kCurSMSRuleInfo.m_dwSetCheckTime) >= kCurSMSRuleInfo.m_dwWaitTimeForReStart )
		{
			ClearStates(iArray);
			if(m_ParentHwnd != NULL)
				::SendMessage(m_ParentHwnd, WM_TO_PARENT_NORMAL, iArray, 0);
			LOG.PrintTimeAndLog(0, "Auto Start-Sent SMS:%d", iArray);
		}
	}
}

void CSMSRuleManager::SetErrorInfo( CString &szErrorInfo )
{
	m_szErrorInfo = szErrorInfo;
}

void CSMSRuleManager::GlobalInit()
{
	CoInitialize(NULL);
}

void CSMSRuleManager::GlobalEnd()
{
	CoUninitialize();
}

DWORD CSMSRuleManager::GetRemainWaitTime( const int iArray ) const
{
	if(!COMPARE(iArray, 0, m_iMaxSmsRuleInfo))
		return 0;

	DWORD dwPassTime = timeGetTime() - m_vSMSRuleInfo[iArray].m_dwSetCheckTime;
	return m_vSMSRuleInfo[iArray].m_dwWaitTimeForReStart - dwPassTime;
}

void CSMSRuleManager::NextCellPhoneAndCheckStop( const int iArray )
{
	if(!COMPARE(iArray, 0, m_iMaxSmsRuleInfo))
		return;

	SMSRULEINFO &kCurSMSRuleInfo = m_vSMSRuleInfo[iArray];

	kCurSMSRuleInfo.m_iCurCellPhoneArray++;
	kCurSMSRuleInfo.m_iCurSentSMSCnt = 0;
	if(kCurSMSRuleInfo.m_iCurCellPhoneArray >= (int) m_vCStringPhoneNumber.size())
	{
		kCurSMSRuleInfo.m_iCurCellPhoneArray = 0;
		kCurSMSRuleInfo.m_eState = STOPING;
		kCurSMSRuleInfo.m_dwWaitTimeForReStart = m_INILoader.LoadInt("WaitTimeForReStart", 3600000);
		if(m_ParentHwnd != NULL)
			::SendMessage(m_ParentHwnd, WM_TO_PARENT_STOP, iArray, 0);
		LOG.PrintTimeAndLog(0, "Auto Stop-Sent SMS:%d", iArray);
	}
}

CString& CSMSRuleManager::GetCurNumber( const int iCurCellPhoneArray )
{
	if(COMPARE(iCurCellPhoneArray, 0, (int) m_vCStringPhoneNumber.size()))
		return m_vCStringPhoneNumber[iCurCellPhoneArray];

	static CString szError = "Error";
	return szError;
}

int CSMSRuleManager::GetSize() const
{
	return m_iMaxSmsRuleInfo;
}

