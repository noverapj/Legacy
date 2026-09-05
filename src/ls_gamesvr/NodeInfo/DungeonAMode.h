

#ifndef _DungeonAMode_h_
#define _DungeonAMode_h_

#include "MonsterSurvivalMode.h"
#include "../Util/IORandom.h"

class SP2Packet;
class User;
class DungeonAMode : public MonsterSurvivalMode
{
protected:
	DWORD m_dwTreasureCardIndex;

public:
	virtual void InitObjectGroupList();
	virtual const char* GetModeINIFileName() const;

protected:
	virtual void LoadMapINIValue();

protected:
	virtual void CheckTurnMonster();
	virtual void NextTurnCheck();

public:
	virtual ModeType GetModeType() const;

protected:
	virtual void ProcessResultWait();
	virtual void ProcessResult();

protected:
	virtual void OnLastPlayRecordInfo( User *pUser, SP2Packet &rkPacket );

public:
	DungeonAMode( Room *pCreator );
	virtual ~DungeonAMode();
};

inline DungeonAMode* ToDungeonAMode( Mode *pMode )
{
	if( !pMode || pMode->GetModeType() != MT_DUNGEON_A )
		return NULL;

	return static_cast< DungeonAMode* >( pMode );
}
#endif

