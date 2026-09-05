#ifndef _ioItemInitializeControl_h_
#define _ioItemInitializeControl_h_

#include "../Util/Singleton.h"

enum INIT_TYPE
{
	ETC_COMMON		= 0,
	CLOVER_COMMON,
	MILEAGE_COMMON,
	MAX_COMMON,
};

// 멤버변수 값들을 구조체로..
struct stITEMINITCONTROL
{
	DWORD m_dwInitYear;
	DWORD m_dwInitMonth;
	DWORD m_dwInitDay;
	DWORD m_dwInitHour;
	DWORD m_dwRotateDay;
	CTime m_BeforeTime;
	CTime m_AfterTime;
	DWORDVec m_InitEtcItemList;

	void Init()
	{
		m_dwInitYear  = 0;
		m_dwInitMonth = 0;
		m_dwInitDay	  = 0;
		m_dwInitHour  = 0;
		m_dwRotateDay = 0;
		m_InitEtcItemList.clear();
	}
};

class User;
class ioItemInitializeControl : public Singleton< ioItemInitializeControl >
{
protected:
	DWORD m_dwInitProcessCheckMs;
	DWORD m_dwCurrentTime;

protected:
	stITEMINITCONTROL	m_stItemInitControl[ MAX_COMMON ];
	
protected:
	void CheckRotateDate( const int iType );

public:
	void LoadINIData();

public:
	void Process();
	
public:
	void CheckInitUserEtcItemByLogin( User *pUser );
	void CheckInitUserMileageByLogin( User *pUser );
	void CheckInitUserEtcItemByPlayer( User *pUser, const int iType );
	void CheckPresentFixedLimitDate( short iPresentType, int iPresentValue1, char *szReturn );
	CTime CheckPresentFixedLimitDate( short iPresentType, int iPresentValue1, CTime &rkLimitDate );

	// Init : clover item
	void CheckInitUserCloverItemByLogin( User* pUser );

	// Init : Before Receive Clover Count
	bool CheckInitUserBeforeReceiveCloverByLogin( User* pUser );

public:
	bool IsDeletePresentCheck( short iPresentType, int iPresentValue1 );

public:
	static ioItemInitializeControl& GetSingleton();

public:   
	ioItemInitializeControl();
	virtual ~ioItemInitializeControl();
};
#define g_ItemInitControl ioItemInitializeControl::GetSingleton()
#endif