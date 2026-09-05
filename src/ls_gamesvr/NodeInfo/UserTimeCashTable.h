#pragma once

class User;

struct UserCashTable
{
	DWORD m_dwItemCode;
	DWORD m_dwReceiveDate;	//해당 테이블 위치에 보상받은 날짜저장.
	DWORD m_dwEndDate;		//
	DWORD m_dwBonusCashEndDate;		//
	
	UserCashTable()
	{
		m_dwItemCode	= 0;
		m_dwReceiveDate	= 0;
		m_dwEndDate		= 0;
		m_dwBonusCashEndDate = 0;
	}
};

class UserTimeCashTable
{
public:
	UserTimeCashTable();
	virtual ~UserTimeCashTable();

	void Init(User* pUser);
	void Destroy();

public:
	void DBToData(CQueryResultData *query_data);
	void FillMoveData(SP2Packet& kPacket);
	void ApplyMoveData(SP2Packet& kPacket);

public:
	void SQLUpdateReceiveDate(const DWORD dwItemCode, BOOL bRequestCash);
	
	void ResultSQLInsertCashTable(CQueryResultData *query_data);

	BOOL IsRenewalDate(const DWORD dwItemCode);
	BOOL IsExpired(const DWORD dwEndDate);

public:
	DWORD GetReceiveDate();
	DWORD GetEndDate();
	void GetCashTable(const DWORD dwCode, UserCashTable& stInfo);
	BOOL IsActive() { return m_bActive; }

	void SetReceiveDate(CTime& cDate);
	void SetEndDate(CTime& cDate);
	void SetActive(BOOL bVal) { m_bActive = bVal; }

public:
	BOOL UpdateCashTable(const DWORD dwCode, const DWORD dwReceiveDate);
	BOOL InsertCashTable(const DWORD dwCode, const DWORD dwReceiveDate);

	void DisposeUpdateReceiveData(const DWORD dwCode, const int iResult, const DWORD dwReceiveDate, ioHashString& szBillingGUID);

	void CheckUpdateInfo();

protected:
	void DeleteExpiredData(const DWORD dwCode);
	

protected:
	typedef std::vector<UserCashTable> vTimeCashTable;
	typedef vTimeCashTable::iterator vTimeCashTable_iter;

protected:
	vTimeCashTable m_vTimeCashTable;

	User* m_pUser;
	BOOL m_bActive;
};