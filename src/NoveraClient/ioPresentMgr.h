#pragma once
#include "DataHeaders/LSC_PresentReward_info.h"
enum 
{
	PRESENT_DEFAULT            = 0,
	PRESENT_SOLDIER            = 1,
	PRESENT_DECORATION         = 2,
	PRESENT_ETC_ITEM           = 3,
	PRESENT_PESO               = 4,
	PRESENT_EXTRAITEM          = 5,
	PRESENT_EXTRAITEM_BOX      = 6,
	PRESENT_RANDOM_DECO		   = 7,
	PRESENT_GRADE_EXP          = 8,
	PRESENT_MEDALITEM          = 9,
	PRESENT_ALCHEMIC_ITEM	   = 10,
	PRESENT_PET				   = 11,
	PRESENT_COSTUME			   = 12,
	PRESENT_BONUS_CASH		   = 13,
	PRESENT_ACCESSORY		   = 14,
	PRESENT_COSTUME_BOX		   = 15,
	PRESENT_SPIRIT = 16,
	PRESENT_MAX				   = 17,
};

// 특별히 체크해야할 멘트 타입만 지정
#define PRESENT_SPECIAL_MENT        2
#define PRESENT_LOSA_SEARCH_MENT	4

#define PRESENT_FISHING_ITEM_SUCCESS	7

#define PRESENT_TRADE_BUY_MENT		37
#define PRESENT_TRADE_SELL_MENT		38
#define PRESENT_TRADE_CANCEL_MENT	39
#define PRESENT_TRADE_TIMEOUT_MENT	40

#define	PRESENT_POPUPITEM_NO_MENT	9999


// PresetType2 나눠 쓰기
#define PRESENT_EXTRAITEM_DIVISION_1      100000000
#define PRESENT_EXTRAITEM_DIVISION_2      10000

#define PRESENT_INDEX_MEMORY            0

class SP2Packet;
class ioComplexStringPrinter;
class ioPlayStage;

class ioPresentMgr : public Singleton< ioPresentMgr >
{
public:
	enum
	{
		PRESENT_STATE_NORMAL = 0,      //
		PRESENT_STATE_NEW    = 1,      //
		PRESENT_STATE_DELETE = 2,      //
	};
	enum
	{
		REQUEST_CHANGE_MODE,
		REQUEST_BTN,
	};

protected:
	struct PresentData
	{
		DWORD m_dwIndex;                   // 선물 DB 인덱스.         // 
		DWORD m_dwSlotIndex;               // 선물 SLOT 인덱스.       //
		ioHashString m_szSendID;           // 보낸 유저
		short   m_iPresentType;            // ( 1.용병 - 2.치장 - 3.권한 - 4.페소 - 5.장비 )
		short   m_iPresentMent;            // ( 멘트타입(클라이언트에서 정의) )
		short   m_iPresentState;           // 새로운 선물 1, 이미 받은 선물 0, 삭제된 선물 2,
		int     m_iPresentValue1;          // 용병:(용병타입), 치장,랜덤치장(ITEMSLOT의 m_item_type), 권한(ETCITEMSLOT의 m_iType), 페소(페소금액), 장비(장비코드)
		int     m_iPresentValue2;          // 용병:(용병기간), 치장,랜덤치장(ITEMSLOT의 m_item_code), 권한(ETCITEMSLOT의 m_iValue1), 페소(NONE), 장비(( 장비 성장값 * 10000 ) + 장비기간)
		                                   // 장비상자(00[00]:1골드,0페소 | [00]00:페소Array 1부터99 -1 빼서 실제 array 구함 )
		int     m_iPresentValue3;          // 장비:(남성스킨)
		int     m_iPresentValue4;          // 장비:(여성스킨)
		DWORD   m_dwLimitDate;             // 선물 기간 제한 : 0909141200(2009년9월14일12시까지 보관 이후 삭제), 0이면 삭제된 선물
		PresentData()
		{
			m_dwIndex = m_dwSlotIndex = 0;
			m_iPresentType = m_iPresentMent = m_iPresentState = 0;
			m_iPresentValue1 = m_iPresentValue2 = m_iPresentValue3 = m_iPresentValue4 = 0;
			m_dwLimitDate  = 0;
		}
	};
	typedef std::vector< PresentData > vPresentData;
	vPresentData m_vPresentList;
	vPresentData m_vDeletePresentList;

	DWORD m_dwPresentCurTimer;
	DWORD m_dwPresentRequestTime;
	DWORD m_dwPresentRequestBtnTime;
	DWORD m_dwPresentRequestCount;

protected:
	LSC_PresentReward_info_Manager* m_pPresentRewardInfoMgr;//수련장용
protected:
	struct MentData
	{
		DWORD m_dwMentColor;
		char  m_szMentText[MAX_PATH];
		MentData()
		{
			m_dwMentColor = 0;
			memset( m_szMentText, 0, sizeof( m_szMentText ) );
		}
	};
	typedef std::vector < MentData > vMentData;
	struct MentList
	{
		vMentData m_MentList;
	};
	typedef std::vector < MentList > vMentList;
	typedef std::map< short, vMentList > MentMap;
	MentMap m_MentList;

protected:
	bool m_bPresentChange;

protected:
	bool IsAlreadyPresent( DWORD dwIndex, DWORD dwSlotIndex );
	void DeletePresent( DWORD dwIndex, DWORD dwSlotIndex );
	void SendPresentData();

	bool RecvEtcItem( PresentData &rkData, int &iItemType, ComplexStringPrinterVec& vTitle, ComplexStringPrinterVec& vDesc, int &nGradeType );

public:
	void LoadINIInfo();

public:
	void SendPresentDataToTime( int iType );
	bool IsEnableSendPresentDataToTime();

public:
	void ApplyPresentData( SP2Packet &rkPacket, ioPlayStage *pStage );
	void ApplyPresentRecv( SP2Packet &rkPacket, ioPlayStage *pStage );
	void ApplyPresentSell( SP2Packet &rkPacket );

public:
	bool IsAlarmPresentData( bool bSpecialAwardBlock );
	bool IsDeletePresentData();
	bool IsPresentChange();

public:
	void ClearAlarmPresentData( DWORD dwIndex, DWORD dwSlotIndex );
	DWORD FirstAlarmPresentData( bool bSpecialAwardBlock, DWORD &rSlotIndex );

public:
	void AllRemoveDeletePresentData();
	void AllDeletePresentData();
	void RemoveDeletePresentData( DWORD dwIndex, DWORD dwSlotIndex );
	DWORD FirstDeletePresentData( DWORD &rSlotIndex );

public:
	void GetPresentDataToIndex( DWORD dwIndex, DWORD dwSlotIndex, ioHashString &rSendID, short &rPresentType, short &rPresentMent, 
								int &rPresentValue1, int &rPresentValue2, int &rPresentValue3, int &rPresentValue4, DWORD &rLimitDate );
	void GetDeletePresentDataToIndex( DWORD dwIndex, DWORD dwSlotIndex, ioHashString &rSendID, short &rPresentType, short &rPresentMent, 
						 			  int &rPresentValue1, int &rPresentValue2, int &rPresentValue3, int &rPresentValue4, DWORD &rLimitDate );

	DWORD GetPresentIndexToArray( int iArray, DWORD &rSlotIndex );

public:
	int  GetMentMaxLine( short iPresentMent );
	void GetMentLineText( short iPresentMent, int iLine, ioComplexStringPrinter &rkPrinter );

public:
	LSC_PresentReward_info* ioPresentMgr::GetRewardInfo(DWORD dwPresentRewardIndex);
	const ioHashString &GetPresentValue1Text(DWORD dwPresentRewardIndex);
	const ioHashString &GetPresentValue2Text(DWORD dwPresentRewardIndex);
	const ioHashString &GetPresentIconText(DWORD dwPresentRewardIndex);
	const ioHashString &GetPresentSubIconText(DWORD dwPresentRewardIndex);
	int GetPresentItemType(DWORD dwPresentRewardIndex);

	const ioHashString &GetPresentValue1Text( short iPresentType, int iPresentValue1, int iPresentValue2 );
	const ioHashString &GetPresentValue2Text( short iPresentType, int iPresentValue1, int iPresentValue2 );
	const ioHashString &GetPresentIconText( short iPresentType, int iPresentValue1, int iPresentValue2 );
	const ioHashString &GetPresentSubIconText( short iPresentType, int iPresentValue1, int iPresentValue2 );
	const ioHashString &GetLmitDateText( DWORD dwLimitDate );
	const int GetGradeType( int nPresentType, int nPresentCode );
public:
	int GetMaxPresent(){ return m_vPresentList.size(); }
	inline const DWORD& GetLimitPresentSize() const { return m_dwPresentRequestCount; }

public:
	void _DebugLogTreasureCardItem();

public:
	static ioPresentMgr& GetSingleton();

public:
	ioPresentMgr();
	virtual ~ioPresentMgr();
};

#define g_PresentMgr ioPresentMgr::GetSingleton()

