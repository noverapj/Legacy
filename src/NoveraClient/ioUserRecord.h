#pragma once

#include "CharacterInfo.h"

enum RecordType
{
	RECORD_BATTLE	 = 1,
	RECORD_LADDER	 = 2,
	RECORD_HEROMATCH = 3,
};

class SP2Packet;
class ioUserRecord
{
protected:
	typedef std::map< RecordType, RecordData > RecordMap;
	RecordMap m_UserRecord;
	RecordData m_HeroSeasonRecord;       // ¿µ¿õÀü ½ÃÁð ±â·Ï
	
public:
	void Initialize();
	void ApplyRecordData( SP2Packet &rkPacket );
	void ApplyHeroSeasonRecord( SP2Packet &rkPacket );

public:
	void AddRecordWin( RecordType eRecordType, int iCount );
	void AddRecordLose( RecordType eRecordType, int iCount );
	void AddRecordKill( RecordType eRecordType, int iCount );
	void AddRecordDeath( RecordType eRecordType, int iCount );

public:
	int GetTotalKill();
	int GetTotalDeath();
	float GetKillDeathPer();
	float GetWinLoseRate( RecordType eRecordType );
	float GetHeroSeasonWinLoseRate();
	__int64 GetRecordEXP( RecordType eRecordType );

public:
	void SetRecordInfo( RecordType eRecordType, int iWin, int iLose, int iKill, int iDeath );
	void SetHeroSeasonRecordInfo( int iWin, int iLose, int iKill, int iDeath );

public:
	int  GetRecordWin( RecordType eRecordType );
	int  GetRecordLose( RecordType eRecordType );
	int  GetHeroSeasonRecordWin();
	int  GetHeroSeasonRecordLose();

public:
	ioUserRecord();
	virtual ~ioUserRecord();
};

