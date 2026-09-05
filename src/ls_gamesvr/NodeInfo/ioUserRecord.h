
#ifndef _ioUserRecord_h_
#define _ioUserRecord_h_

#include "NodeHelpStructDefine.h"

class User;
class CQueryResultData;

enum RecordType
{
	RECORD_BATTLE	 = 1,
	RECORD_LADDER	 = 2,
	RECORD_HEROMATCH = 3,
};

class ioUserRecord
{
protected:
	User *m_pUser;

protected:
	typedef std::map< RecordType, RecordData > RecordMap;
	RecordMap  m_UserRecord;
	RecordData m_HeroSeasonRecord;       // 영웅전 시즌 기록

	bool m_bChange;

public:
	void Initialize( User *pUser );

	// DB Load & Save
public:
	void DBtoRecordData( CQueryResultData *query_data );	
	void SaveRecordData();

public:
	void AddRecord( RecordType eRecordType, const RecordData &rkInRecord );
	void AddRecordWin( RecordType eRecordType, int iCount );
	void AddRecordLose( RecordType eRecordType, int iCount );
	void AddRecordKill( RecordType eRecordType, int iCount );
	void AddRecordDeath( RecordType eRecordType, int iCount );
	void AddRecordMaxContinuousKill( RecordType eRecordType, int iCount); //만들다가 말았습니다

public:
	void FillRecordData( SP2Packet &rkPacket );
	void FillMoveData( SP2Packet &rkPacket );
	void FillHeroSeasonData( SP2Packet &rkPacket );
	void ApplyMoveData( SP2Packet &rkPacket, bool bDummyNode = false );

public:
	void GetRecordInfo( RecordType eRecordType, int &iWin, int &iLose, int &iKill, int &iDeath );

public:
	bool InitRecordInfo( RecordType eRecordType );
	bool InitHeroSeasonRecord();
	
public:
	int GetRecordWin( RecordType eRecordType );
	int GetRecordLose( RecordType eRecordType );

public:
	int GetTotalKill();
	int GetTotalDeath();
	float GetKillDeathPer();

public:
	bool IsChange(){ return m_bChange; }

public:
	__int64 GetRecordEXP( RecordType eRecordType );

public:
	ioUserRecord();
	virtual ~ioUserRecord();
};

#endif