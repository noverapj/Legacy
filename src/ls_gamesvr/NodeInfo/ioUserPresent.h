
#ifndef _ioUserPresent_h_
#define _ioUserPresent_h_

#define PRESENT_SOLDIER             1
#define PRESENT_DECORATION          2
#define PRESENT_ETC_ITEM            3
#define PRESENT_PESO                4
#define PRESENT_EXTRAITEM           5
#define PRESENT_EXTRAITEM_BOX       6
#define PRESENT_RANDOM_DECO			7
#define PRESENT_GRADE_EXP           8
#define PRESENT_MEDALITEM           9
#define PRESENT_ALCHEMIC_ITEM		10
#define PRESENT_PET_ITEM			11
#define PRESENT_COSTUME				12
#define PRESENT_BONUS_CASH			13
#define PRESENT_ACCESSORY			14
#define PRESENT_COSTUME_BOX			15
#define PRESENT_SPIRIT				16
#define MAX_PRESENT_TYPE            17

// 특별히 체크해야할 멘트 타입만 지정
#define PRESENT_SPECIAL_MENT        2
#define PRESENT_LOSA_SEARCH_MENT	4
#define PRESENT_TRADE_BUY_MENT		37
#define PRESENT_TRADE_SELL_MENT		38
#define PRESENT_TRADE_CANCEL_MENT	39
#define PRESENT_TRADE_TIMEOUT_MENT	40

// PresetType2 나눠 쓰기
#define PRESENT_EXTRAITEM_DIVISION_1      100000000
#define PRESENT_EXTRAITEM_DIVISION_2      10000

#define PRESENT_INDEX_MEMORY            0
class User;
class CQueryResultData;

class ioUserPresent
{
public:
	enum
	{
		PRESENT_STATE_NORMAL = 0,      //
		PRESENT_STATE_NEW    = 1,      //
		PRESENT_STATE_DELETE = 2,      //
	};

protected:
	User *m_pUser;

protected:
	struct PresentData
	{
		DWORD m_dwIndex;                   // 선물 DB 인덱스.         // 
		DWORD m_dwSlotIndex;               // 선물 SLOT 인덱스.       //
		DWORD m_dwSendUserIndex;           // 보낸 유저 인덱스.
		ioHashString m_szSendID;           // 보낸 유저
		short   m_iPresentType;            // ( 1.용병 - 2.치장 - 3.권한 - 4.페소 - 5.장비 )
		short   m_iPresentMent;            // ( 멘트타입(클라이언트에서 정의) )
		short   m_iPresentState;           // 새로운 선물 1, 이미 받은 선물 0, 삭제된 선물 2,
		int     m_iPresentValue1;          // 용병:(용병타입), 치장,랜덤치장(ITEMSLOT의 m_item_type), 권한(ETCITEMSLOT의 m_iType), 페소(페소금액), 장비(장비코드), 장비상자(뽑기번호)
		int     m_iPresentValue2;          // 용병:(용병기간), 치장,랜덤치장(ITEMSLOT의 m_item_code), 권한(ETCITEMSLOT의 m_iValue1), 페소(NONE), 장비((확장 타입 * 100000000) + ( 장비 성장값 * 10000 ) + 장비기간)
		                                   // 장비상자(00[00]:1골드,0페소 | [00]00:페소Array 1부터99 -1 빼서 실제 array 구함 )
		int     m_iPresentValue3;          // 장비:(남성스킨)
		int     m_iPresentValue4;          // 장비:(여성스킨)
		DWORD   m_dwLimitDate;             // 선물 기간 제한 : 0909141200(2009년9월14일12시까지 보관 이후 삭제), 0이면 삭제된 선물
		PresentData()
		{
			m_dwIndex = m_dwSlotIndex = 0;
			m_dwSendUserIndex = 0;
			m_iPresentType = m_iPresentMent = m_iPresentState = 0;
			m_iPresentValue1 = m_iPresentValue2 = m_iPresentValue3 = m_iPresentValue4 = 0;
			m_dwLimitDate  = 0;
		}
	};
	typedef std::vector< PresentData > vPresentData;
	vPresentData m_vPresentList;
	vPresentData m_vTempMemoryData;
	DWORD m_dwLastDBIndex;
	DWORD m_dwLastSlotIndex;
public:
	enum DeletePresentType
	{
		DPT_TIMEOVER = 1,
		DPT_RECV     = 2,
		DPT_SELL     = 3,
	};

public:
	void Initialize( User *pUser );

	// DB Load
public:
	void DBtoPresentData( CQueryResultData *query_data );	

public:
	DWORD GetLastPresentDBIndex();
	DWORD GetLastPresentSlotIndex();

protected:
	ioUserPresent::PresentData &GetPresentData( DWORD dwIndex, DWORD dwSlotIndex );
	bool ComparePresentData( ioUserPresent::PresentData &kData );

public:
	bool DeletePresentData( DWORD dwIndex, DWORD dwSlotIndex, DeletePresentType eDeletePresentType );
	bool PresentRecv( IN SP2Packet &rkRecvPacket, OUT SP2Packet &rkPacket );
	bool PresentDisassemble( IN SP2Packet &rkRecvPacket, OUT SP2Packet &rkPacket );
	void PresentSell( DWORD dwIndex, DWORD dwSlotIndex, SP2Packet &rkPacket );

	BOOL HaveAThisItem(DWORD dwType, DWORD dwItemCode);

public:
	void AddPresentMemory( const ioHashString &szSendName, short iPresentType, int iPresentValue1, int iPresentValue2, int iPresentValue3, 
						   int iPresentValue4, short iPresentMent, CTime &rkLimitTime, short iPresentState );
	void SendPresentMemory();
	void LogoutMemoryPresentInsert();     // 유저 로그 아웃시에 저장

public:
	void CheckPresentLimitDate();

public:
	void AllDeleteData();

public:
	void FillMoveData( SP2Packet &rkPacket );
	void ApplyMoveData( SP2Packet &rkPacket, bool bDummyNode = false  );

public:
	ioUserPresent();
	virtual ~ioUserPresent();
};

#endif