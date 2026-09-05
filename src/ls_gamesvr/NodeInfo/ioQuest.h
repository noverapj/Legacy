
#ifndef _ioQuest_h_
#define _ioQuest_h_

#include "NodeHelpStructDefine.h"
#include "ioDBDataController.h"
#include "ioQuestManager.h"

class QuestParent;
class QuestDataParent
{
public:
	enum
	{
		DEFAULT_YEAR    = 2010,			// 2010년은 DB에 저장하지 않는다. 즉 DateData의 년도가 0이면 2010이란 뜻이다. 1이면 2011년
		DATE_YEAR_VALUE = 100000000,    // 년까지 나눈다.
		DATE_MONTH_VALUE= 1000000,      // 월까지 나눈다.
		DATE_DAY_VALUE =  10000,        // 일까지 나눈다.
		DATE_HOUR_VALUE = 100,          // 시까지 나눈다.

		INDEX_HALF_VALUE = 10000,		// IndexData값을 잘라서 2개의 정보를 넣는다.
		MAGIC_VALUE      = 100,			// MagicData값을 잘라서 3개의 정보를 넣는다.
		ALARM_VALUE      = 10,			// MagicData값을 잘라서 3개의 정보를 넣는다.
	};

protected:
	DWORD m_dwIndexData;           // 메인 인덱스 + 서브 인덱스
	DWORD m_dwDateData;            // 수락/완료 시간값 : 00년 01월 01일 00시 00분  = 2010(Default) << 년 << 월 << 일 << 시 << 분

protected:
	QuestParent *m_pLinkQuest;

public:
	DWORD GetMainIndex();
	DWORD GetSubIndex();

public:
	DWORD GetYear();
	DWORD GetMonth();
	DWORD GetDay();
	DWORD GetHour();
	DWORD GetMinute();

public:
	virtual DWORD GetIndexData(){ return m_dwIndexData; }
	virtual DWORD GetDateData(){ return m_dwDateData; }
	virtual QuestParent *GetLinkQuest();
	DWORD GetCompleteValue();

public:
	void SetIndexData( DWORD dwIndexData );
	void SetDateData( DWORD dwDateData );
	void SetDate( WORD wYear, WORD wMonth, WORD wDay, WORD wHour, WORD wMinute );
	void SetLinkQuest( QuestParent *pQuest );

public:
	bool CheckDelete();
	bool CheckOneDayDelete( DWORD dwCheckTime );
	bool CheckGuildQuestDelete( bool bGuild );

public:
	QuestDataParent();
	virtual ~QuestDataParent();
};

class QuestData : public QuestDataParent
{
protected:
	DWORD m_dwValueData;           // 달성값
	DWORD m_dwMagicData;           // 추가값(2Byte사용가능) + 알리미 등록 + 상태(진행,달성,완료)

public:
	DWORD GetCurrentData();
	DWORD GetCurrentMagicData();
	DWORD GetState();

public:
	bool  IsAlarm();

public:
	inline DWORD GetValueData(){ return m_dwValueData; }
	inline DWORD GetMagicData(){ return m_dwMagicData; }

public:
	void PresetData( QuestParent *pQuest ); 
	void SetCurrentData( DWORD dwCurrentData );
	void SetCurrentMagicData( DWORD dwCurrentMagicData );
	void SetAlarm( bool bAlarm );
	void SetState( DWORD dwState );

public:
	void AttainIntegrity( User *pUser );

public:
	void ApplyData( CQueryResultData *query_data );	
	void ApplyData( SP2Packet &rkPacket );

public:
	void ClearData();

public:
	QuestData();
	virtual ~QuestData();
};

class QuestCompleteData : public QuestDataParent
{
public:
	void PresetData( QuestParent *pQuest ); 

public:
	void ApplyData( CQueryResultData *query_data );	
	void ApplyData( SP2Packet &rkPacket );

public:
	void ClearData();

public:
	QuestCompleteData();
	virtual ~QuestCompleteData();
};
//////////////////////////////////////////////////////////////////////////
class ioQuest : public ioDBDataController
{
public:
	enum
	{
		MAX_SLOT   = 20,	
	};

protected:
	// 진행 & 달성 목록
	struct QuestInfo   
	{
		bool      m_bChange;
		DWORD     m_dwIndex;
		QuestData m_Data[MAX_SLOT];

		QuestInfo()
		{
			m_bChange = false;
			m_dwIndex = NEW_INDEX;
		}
	};
	typedef std::vector< QuestInfo > vQuestInfo;
	vQuestInfo m_vItemList;

	// 완료 목록
	struct QuestCompleteInfo
	{
		bool      m_bChange;
		DWORD     m_dwIndex;
		QuestCompleteData m_Data[MAX_SLOT];

		QuestCompleteInfo()
		{
			m_bChange = false;
			m_dwIndex = NEW_INDEX;
		}
	};
	typedef std::vector< QuestCompleteInfo > vQuestCompleteInfo;
	vQuestCompleteInfo m_vCompleteList;

	// 삭제된 목록
	struct QuestDeleteInfo
	{
		DWORD m_dwMainIndex;
		DWORD m_dwSubIndex;
		QuestDeleteInfo()
		{
			m_dwMainIndex = m_dwSubIndex = 0;
		}
	};
	typedef std::vector< QuestDeleteInfo > vQuestDeleteInfo;
	vQuestDeleteInfo m_vDeleteList;

	// 어뷰즈 체크
	struct QuestAbuseInfo
	{
		WORD m_dwMainIndex;
		WORD m_dwSubIndex;
		DWORD m_dwCurrentTime;
		bool m_bServerMoving;

		QuestAbuseInfo()
		{
			m_dwMainIndex = m_dwSubIndex = 0;
			m_dwCurrentTime = 0;
			m_bServerMoving = false;
		}
	};
	typedef std::vector< QuestAbuseInfo > vQuestAbuseInfo;
	vQuestAbuseInfo m_AbuseList;

public:
	virtual void Initialize( User *pUser );

	// 진행 & 달성 목록
protected:
	void InsertDB( QuestInfo &kQuestDB );     

public:
	virtual bool DBtoNewIndex( DWORD dwIndex );
	virtual void DBtoData( CQueryResultData *query_data );	

public:
	QuestData GetQuestData( ioHashString szClass );
	QuestData GetQuestData( DWORD dwMainIndex, DWORD dwSubIndex );
	QuestData AddQuestData( DWORD dwMainIndex, DWORD dwSubIndex );
	QuestData SetQuestCurrentData( QuestData &kData );
	QuestData SetQuestAlarm( DWORD dwMainIndex, DWORD dwSubIndex, bool bAlarm );
	void SetQuestReward( DWORD dwMainIndex, DWORD dwSubIndex, bool isRewardSelectStyle, std::vector<BYTE>& vSelIndexes, SP2Packet &rkPacket );

protected:
	// return : 보상이 지급 되었으면 true / 보상이 지급되지 않았으면 false
	bool PackQuestReward( SP2Packet& rpacket, QuestParent* quest, BYTE* pindexes, int numindex );

	// 완료 목록
protected:
	void InsertCompleteDB( QuestCompleteInfo &kQuestDB );

public:
	virtual bool DBtoNewCompleteIndex( DWORD dwIndex );
	virtual void DBtoCompleteData( CQueryResultData *query_data );	

public:
	QuestCompleteData AddQuestCompleteData( DWORD dwMainIndex, DWORD dwSubIndex );

	// 공통
public:
	virtual void SaveData();
	virtual void FillMoveData( SP2Packet &rkPacket );
	virtual void ApplyMoveData( SP2Packet &rkPacket, bool bDummyNode = false  );

public:
	bool IsQuestComplete( DWORD dwMainIndex, DWORD dwSubIndex );
	bool IsQuestIndexCheck( DWORD dwMainIndex, DWORD dwSubIndex );

public:
	void CheckQuestReConnectSeconds( int iTotalSeconds );

public:
	void ClearQuestInfo( DWORD dwMainIndex, DWORD dwSubIndex );
	void ClearOneDayQuestCompleteAll();
	void ClearGuildQuestProgressAll();

public: // Abuse
	void AddAbuseCheckQuest( DWORD dwMainIndex, DWORD dwSubIndex );				// 어뷰즈 체크하는 퀘스트 등록
	void DeleteAbuseCheckQuest( DWORD dwMainIndex, DWORD dwSubIndex );			// 완료한 퀘스트는 어뷰즈 체크에서 제외
	bool IsQuestAbuse( QuestData &rkServerQuest, QuestData &rkClientData );		// 퀘스트 어뷰즈
	bool IsQuestAttainCheck( DWORD dwMainIndex, DWORD dwSubIndex );				// 퀘스트 완료 체크
	bool IsCheckAvailableQuest(  DWORD dwMainIndex, DWORD dwSubIndexin, int iStartDate, int iEndDate);
	
public:  //테스트용 매크로 
	void InitQuestData();

public:
	ioQuest();
	virtual ~ioQuest();
};

#endif