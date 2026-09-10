
#ifndef _MathManage_h_
#define _MathManage_h_

#include "../io3DEngine/io3DCommon.h"
#include "../io3DEngine/ioPrerequisites.h"
#include "../io3DEngine/ioSingleton.h"
#include "../PlayEntityHelpStruct.h"

class cLua;

class MathManager : public Singleton< MathManager >
{
private:
	cLua	*m_LuaState;

public:
	bool Init( cLua *pContext );
	cLua *GetLua() { return m_LuaState; }

	float GetStatAttribute( float fTarget,float fAttributeLev, float fChangeWith );
	float GetMemberBalance( int iOwnerMember, int iEnemyMember, int iModeStyle );
	float GetMemberBalanceForBlowTime( int iOwnerMember, int iEnemyMember, int iModeType, int iModeStyle );
	float GetMemberBalanceForGauge( int iOwnerMember, int iEnemyMember, int iModeType, int iModeStyle );
	float GetMemberBalanceForDropDamage( int iOwnerMember, int iEnemyMember, float fBaseDamage );
	float GetMemberBalanceForMoveSpeed( int iOwnerMember, int iEnemyMember, int iModeType, int iModeStyle );

	float GetScoreBalanceDamage( float fCurDamage, float fBaseDamage,
								 int iOwnerWinCnt, int iEnemyWinCnt,
								 int iModeType );
	float GetScoreBalanceGauge( int iOwnerWinCnt, int iEnemyWinCnt, int iModeType );

	float GetTeamSurvivalBalanceDamage( float fCurDamage, float fBaseDamage,
										float fOwnerRate, float fEnemyRate );
	float GetTeamSurvivalBalanceGauge( float fOwner, float fEnemy );

	float GetFightClubTimeHPDamage( int iCurTime, int iMaxTime,
									float fOwnerHP, float fEnemyHP,
									float fCurDamage, float fBaseDamage );

	float GetFightClubTimeHPGauge( int iCurTime, int iMaxTime,
								   float fOwnerHP, float fEnemyHP );

	float GetFightClubVictoryDamage( int iOwnerVictory, int iEnemyVictory,
									 float fCurDamage, float fBaseDamage );

	float GetFightClubVictoryGauge( int iOwnerVictory, int iEnemyVictory );

	float GetSymbolExtraRate( int iOwnerTeamCnt, int iEnemyTeamCnt );
	float GetSymbolRecoveryRate( int iOwnerTeamCnt, int iEnemyTeamCnt );
	float RecalculateDamageByArmorClass( float fCurDamage, float fCharArmorClass );
	float RecalculateSpeedBySpeedClass( float fCurMaxSpeed, float fCharSpeedClass );

	float GetContributeBalanceForGauge( int iContribute, int iModeStyle );
	float GetContributeBalanceForDamage( int iContribute, float fCurDamage, float fBaseDamage, int iModeStyle );

	float GetRestorationValue( int iCharCnt, int iModeType );
	float GetStateProtectionTimeRate( int iOwnerTeamCnt, int iEnemyTeamCnt, int iType );
	float GetBuffItemDropRate( int iOwnerTeamCnt, int iEnemyTeamCnt );

	float CalcBallDamageSpeedAmt( float fDamage, float fPushPower, int iOwnerTeamCnt, int iEnemyTeamCnt, bool bTimeOut );
	float CalcBallDamageBoundAmt( float fDamage, float fBlowPower, int iOwnerTeamCnt, int iEnemyTeamCnt, bool bTimeOut );

public:
	MathManager();
	virtual ~MathManager();
};

#define g_MathManager MathManager::GetSingleton()

#endif
