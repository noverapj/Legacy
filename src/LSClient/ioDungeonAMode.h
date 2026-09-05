#pragma once

class ioPlayStage;
class ioPlayEntity;
class ioBaseChar;
class SP2Packet;

#include "ioMonsterSurvivalMode.h"
class ioDungeonAMode : public ioMonsterSurvivalMode
{
public:
	virtual void InitMode( int iMapIndex );

protected:
	void TurnStartEvent( DWORD dwPrevHighTurn, DWORD dwPrevLowTurn );

public:
	void OnTelepotation( ioBaseChar *pOwnerChar );


protected:
	virtual void SetTotalScoreWnd( IntVec &rvClassTypeList,
								   IntVec &rvClassPointList,
								   float fBlueGuildTeamBonus,
								   float fRedGuildTeamBonus,
								   float fBonusArray[BA_MAX] );

protected:
	virtual void UpdateDropDieRecord( ioBaseChar *pDieChar, ioBaseChar *pKillChar, ioBaseChar *pBestAttacker );
	virtual void UpdateKillDeathRecord( ioPlayEntity *pDieEntity, ioPlayEntity *pKiller, ioPlayEntity *pBestAttacker );

public:
	virtual void ContributeConvert();
	virtual int ContributeMyRank( int &rRankValue1, int &rRankValue2, int &rRankValue3, int &rRankValue4 );
	virtual int ContributeRank( const ioHashString &rkName, int &rRankValue1, int &rRankValue2, int &rRankValue3, int &rRankValue4 );
	virtual void AttackCntContribute( const ioHashString szAttacker, const ioHashString szTarget, DWORD dwItemCode, int iAttackValue = 100 );

	virtual void FillLastPlayRecordInfo( SP2Packet &rkPacket );
	virtual void FillPlayRecordInfo( SP2Packet &rkPacket );
	virtual void ApplyPlayRecordInfo( SP2Packet &rkPacket );

protected:
	virtual void OnTurnStart( SP2Packet &rkPacket );
	virtual void OnTurnEnd( SP2Packet &rkPacket );
	virtual void OnRoundEnd( SP2Packet &rkPacket );
	virtual void OnFinalResultUserInfo( SP2Packet &rkPacket );

public:
	ioDungeonAMode( ioPlayStage *pCreator );
	virtual ~ioDungeonAMode();
};

inline ioDungeonAMode* ToDungeonAMode( ioPlayMode *pMode )
{
	if( !pMode || pMode->GetModeType() != MT_DUNGEON_A )
		return NULL;

	return dynamic_cast< ioDungeonAMode* >( pMode );
}

