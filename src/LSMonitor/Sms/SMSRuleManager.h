#pragma once

#include "../stdafx.h"
#include "ioINILoaderAU.h"
#include "SMSSender.h"

#include <vector>

using namespace std;

#define DEFAULT_SMS_MENT "서버 이상 발생. SMS발송 중지후 조치부탁드립니다."
#define MAX_VECTOR_SIZE 100

#define WM_TO_PARENT_NORMAL (WM_USER + 900)
#define WM_TO_PARENT_STOP   (WM_USER + 901)

typedef vector<CString> vCString;

enum SMSRULESTATE
{
	COUNTING_ERROR,
	SENDING_SMS,
	STOPING,
	NORMAL
};

typedef struct SMSRULEINFO
{
	int m_iCurSentSMSCnt;
	int m_iCurSendFailCnt;

	DWORD m_dwWaitTimeForReStart;
	DWORD m_dwSetCheckTime;

	int m_iCurCellPhoneArray;

	SMSRULESTATE m_eState;

	SMSRULEINFO()
	{
		m_iCurSentSMSCnt = 0;
		m_iCurSendFailCnt = 0;

		m_dwWaitTimeForReStart = 3600000;// 1시간
		m_dwSetCheckTime = 0;
		m_iCurCellPhoneArray = 0;
		
		m_eState = NORMAL;
	}
}SMSRULEINFO;

typedef vector<SMSRULEINFO> vSMSRULEINFO;

class CSMSRuleManager
{
protected:
	ioINILoaderAU m_INILoader;
	CSMSSender m_SMSSender;

	// ini
	int m_MaxSentSMSCnt;
	int m_MaxSendFailCnt;
	
	DWORD m_dwWaitTimeForError;
	DWORD m_dwWaitTimeForSendSMS;

	CString m_szSMSMent;

	vCString m_vCStringPhoneNumber;
	//

	int m_iReservedWeek;
	int m_iReservedStartHour;
	int m_iReservedEndHour;
	bool m_bReserveStart;

	CString m_szErrorInfo;
	
	vSMSRULEINFO m_vSMSRuleInfo;
	int m_iMaxSmsRuleInfo;
	
	HWND m_ParentHwnd;

	int m_iPreYearMonth;

protected:
	void SendSMS(const int iArray);
	
	void SetSequenceSendSMS(const int iArray);

	void CheckReservedStop();
	void _OnTimerSub(const int iArray);

	void NextCellPhoneAndCheckStop( const int iArray );
	void LoadINI();

	CString& GetCurNumber(const int iCurCellPhoneArray);
public:
	void _OnTimer();

	void SetInfo(const int iMaxArray, HWND parentHwnd);

	void ClearStates(const int iArray);
	void SetFirstError(const int iArray);
	void SetStopMonitering(DWORD dwStopTime, const int iArray);
	void SetErrorToNormal(const int iArray);

	void SetErrorInfo(CString &szErrorInfo);

public:
	SMSRULESTATE GetState(const int iArray) const;
	DWORD GetWaitTimeForStop(const int iArray) const;
	DWORD GetRemainWaitTime(const int iArray) const;
	int GetSize() const;
public:
	static void GlobalInit();
	static void GlobalEnd();

public:
	CSMSRuleManager(void);
	~CSMSRuleManager(void);
};
