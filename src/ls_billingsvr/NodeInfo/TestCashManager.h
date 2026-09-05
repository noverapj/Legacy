#ifndef __TestCashManager_h__
#define __TestCashManager_h__

class TestCashManager
{
public:
	struct TestCashInfo
	{	
		ioHashString m_szPrivateID;
		int          m_iCash;
		DWORD        m_dwCashChargeAllDays; // 처음 캐쉬충전한 날짜 372*년+31*달+일
	};

protected:
	enum 
	{
		//CASH_CHARGE_DAYS = 30,     
		CASH_CHARGE_NUM  = 1000000,  
		CASH_MIN_NUM     = 50000,    
	};

	typedef std::vector< TestCashInfo* > vTestCashInfo;

	vTestCashInfo m_vTestCashInfoVec;

protected:
	bool  IsOverDays( DWORD dwCheckAllDays, int iAddDays );
	DWORD GetCurAllDays();

public:
	TestCashInfo *GetInfo( const ioHashString &rszPrivateID );
	void AddInfo( const ioHashString &rszPrivateID );
	void Clear();

	void CheckNChargeCash( OUT TestCashInfo *pInfo );

public:
	TestCashManager();
	virtual ~TestCashManager();
};

#endif // __TestCashManager_h__