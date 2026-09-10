#pragma once

#include "EncryptValue.h"

class SP2Packet;
class QuestParent;
class QuestDataParent
{
public:
	enum
	{
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
	virtual const DWORD GetIndexData() const { return m_dwIndexData; }
	virtual const DWORD GetDateData() const { return m_dwDateData; }
	virtual QuestParent *GetLinkQuest();

public:
	void SetIndexData( DWORD dwIndexData );
	void SetDateData( DWORD dwDateData );
	void SetLinkQuest( QuestParent *pQuest );
    
public:
	const int GetPerformType() const;

public:
	QuestDataParent();
	virtual ~QuestDataParent();
};

class QuestData : public QuestDataParent
{
protected:
	CEncrypt<DWORD> m_dwValueData;           // 달성값
	DWORD m_dwMagicData;           // (이벤트 퀘스트등... 여러가지 의미로 사용) + 알리미 등록 + 상태(진행,달성,완료)  

protected:
	bool m_bChangeData;

public:
	DWORD GetCurrentData();
	DWORD GetCurrentMagicData();
	DWORD GetState();

public:
	bool  IsAlarm();
	bool  IsChangeData() { return m_bChangeData; }

public:
	void SetMainIndex( DWORD dwMainIndex );
	void SetSubIndex( DWORD dwSubIndex );
	void SetCurrentData( DWORD dwCurrentData );
	void SetCurrentMagicData( DWORD dwCurrentMagicData );
	void SetAlarm( bool bAlarm );
	void SetState( DWORD dwState );

public:
	inline const DWORD GetValueData() const { return m_dwValueData; }
	inline const DWORD GetMagicData() const { return m_dwMagicData; }
	
	inline void SetValueData( DWORD dwData ){ m_dwValueData = dwData; }
	
public:
	void SyncComplete(){ m_bChangeData = false; }

public:
	void ApplyData( SP2Packet &rkPacket );

public:
	QuestData();
	virtual ~QuestData();
};

class QuestDataSort : public std::binary_function< const QuestData*, const QuestData*, bool >
{
public:
	bool operator()( const QuestData *lhs , const QuestData *rhs ) const
	{
		if( !lhs || !rhs )
			return false;

		if( lhs->GetPerformType() > rhs->GetPerformType() )
			return true;
		else if( lhs->GetPerformType() == rhs->GetPerformType() )
		{
			if( lhs->GetDateData() > rhs->GetDateData() )
			{
				return true;
			}
		}				
		return false;
	}
};

class QuestCompleteData : public QuestDataParent
{

public:
	void ApplyData( SP2Packet &rkPacket );

public:
	QuestCompleteData();
	virtual ~QuestCompleteData();
};

class QuestCompleteDataSort : public std::binary_function< const QuestCompleteData*, const QuestCompleteData*, bool >
{
public:
	bool operator()( const QuestCompleteData *lhs , const QuestCompleteData *rhs ) const
	{
		if( !lhs || !rhs )
			return false;

		if( lhs->GetPerformType() > rhs->GetPerformType() )
			return true;
		else if( lhs->GetPerformType() == rhs->GetPerformType() )
		{
			if( lhs->GetDateData() > rhs->GetDateData() )
			{
				return true;
			}				
		}
		return false;
	}
};
//////////////////////////////////////////////////////////////////////////
class ioQuest
{
protected:
	typedef std::vector< QuestData* > vQuestInfo;
	vQuestInfo m_vItemList;

	typedef std::vector< QuestCompleteData* > vQuestCompleteInfo;
	vQuestCompleteInfo m_vCompleteList;

public:
	void ApplyQuestData( SP2Packet &rkPacket );
	void ApplyQuestCompleteData( SP2Packet &rkPacket );
	void ApplyQuestOccurData( SP2Packet &rkPacket );
	void ApplyQuestRewardData( SP2Packet &rkPacket );
	void AddQuestData( QuestData &rkData );
	void AddQuestCompleteData( QuestCompleteData &rkData );
	
public:
	bool RemoveQuestData( DWORD dwMainIndex, DWORD dwSubIndex );
	bool RemoveQuestCompleteData( DWORD dwMainIndex, DWORD dwSubIndex );
	bool RemoveQuestOneDayAll();

public:
	bool IsQuestAttain( DWORD dwMainIndex, DWORD dwSubIndex );
	bool IsQuestComplete( DWORD dwMainIndex, DWORD dwSubIndex );
	bool IsQuestIndexCheck( DWORD dwMainIndex, DWORD dwSubIndex );

public:
	/************************************************************************/
	/* 서브퀘스트도 포함한 최대 갯수이다. m_vItemList.size()보다 값이 크다	*/
	/************************************************************************/
	int GetMaxQuest( DWORD dwState );
	int GetMaxAlarmQuest();

public:
	QuestData &GetQuestData( DWORD dwMainIndex, DWORD dwSubIndex );
	DWORD GetQuestState( DWORD dwMainIndex, DWORD dwSubIndex );
	QuestData GetProcessToArray( int iArray );
	QuestData GetAttainNCompleteToArray( int iArray );
	QuestData GetAlarmQuestToArray( int iArray );

public: //테스트용 메크로 
	void ClearQuestData();

public:
	ioQuest();
	virtual ~ioQuest();
};

