

#include "stdafx.h"

#include "../io3DEngine/ioStream.h"
#include "../io3DEngine/ioResourceLoader.h"

#include "../GameEnumType.h"
#include "MathManager.h"
#include "../ioRankBattleManager.h"

template<> MathManager* Singleton< MathManager >::ms_Singleton = 0;

MathManager::MathManager()
{
	m_LuaState = NULL;
}

MathManager::~MathManager()
{
}

bool MathManager::Init( cLua *pContext )
{
	m_LuaState = pContext;

	std::string szPath = "Math.lua";
	if( g_ResourceLoader.IsLoadInPack() )
	{
		szPath.insert(0, "Scripts\\");

		ioBinaryStream kStream;
		if( !g_ResourceLoader.LoadStream( szPath.c_str(), &kStream ) )
		{
			LOG.PrintTimeAndLog( 0, "MathManager::Init - %s Load Failed", szPath.c_str() );
			return false;
		}
		
		if( !m_LuaState->RunMemory( (const char*)kStream.GetPtr() , kStream.GetSize()) )
		{
			LOG.PrintTimeAndLog( 0, "MathManager::Init - %s RunMemory is fail", szPath.c_str() );
			return false;
		}
	}
	else
	{
		if( !m_LuaState->RunScript( szPath.c_str() ) )
		{
			LOG.PrintTimeAndLog( 0, "MathManager::Init - %s RunScript is fail!", szPath.c_str() );
			return false;
		}
	}

	return true;
}

float MathManager::GetScoreBalanceDamage( float fCurDamage, float fBaseDamage, 
										  int iOwnerWinCnt, int iEnemyWinCnt,
										  int iModeType )
{
	if( g_BattleRoomMgr.IsTournamentModeType() || iModeType == MT_BATTLE )
		return fCurDamage;			// 경기방, 배틀모드 보정 없음

	lua_pushstring_e( m_LuaState->GetScriptContext(), "ScoreBalanceCalc" );
	lua_gettable( m_LuaState->GetScriptContext(), LUA_GLOBALSINDEX );
	lua_pushnumber( m_LuaState->GetScriptContext(), fCurDamage );
	lua_pushnumber( m_LuaState->GetScriptContext(), fBaseDamage );
	lua_pushnumber( m_LuaState->GetScriptContext(), iOwnerWinCnt );
	lua_pushnumber( m_LuaState->GetScriptContext(), iEnemyWinCnt );
	lua_pushnumber( m_LuaState->GetScriptContext(), iModeType );

	lua_call( m_LuaState->GetScriptContext(), 5, 1 );

	float fResult = (float)lua_tonumber( m_LuaState->GetScriptContext(), -1 );

	int iTop = lua_gettop( m_LuaState->GetScriptContext() );
	if( iTop > 0 )
	{
		lua_pop( m_LuaState->GetScriptContext(), iTop );
	}

	return fResult;
}

float MathManager::GetScoreBalanceGauge( int iOwnerWinCnt, int iEnemyWinCnt, int iModeType )
{
	if( g_BattleRoomMgr.IsTournamentModeType() || iModeType == MT_BATTLE )
		return FLOAT1;			// 경기방, 배틀모드 보정 없음


	lua_pushstring_e( m_LuaState->GetScriptContext(), "ScoreBalanceCalcForGauge" );
	lua_gettable( m_LuaState->GetScriptContext(), LUA_GLOBALSINDEX );
	lua_pushnumber( m_LuaState->GetScriptContext(), iOwnerWinCnt );
	lua_pushnumber( m_LuaState->GetScriptContext(), iEnemyWinCnt );
	lua_pushnumber( m_LuaState->GetScriptContext(), iModeType );

	lua_call( m_LuaState->GetScriptContext(), 3, 1 );

	float fResult = (float)lua_tonumber( m_LuaState->GetScriptContext(), -1 );

	int iTop = lua_gettop( m_LuaState->GetScriptContext() );
	if( iTop > 0 )
	{
		lua_pop( m_LuaState->GetScriptContext(), iTop );
	}

	return fResult;
}

float MathManager::GetTeamSurvivalBalanceDamage( float fCurDamage, float fBaseDamage,
												 float fOwnerRate, float fEnemyRate )
{
	if( g_BattleRoomMgr.IsTournamentModeType() || ioPlayMode::GetModeType() == MT_BATTLE )
		return fCurDamage;			// 경기방 보정 없음


	lua_pushstring_e( m_LuaState->GetScriptContext(), "TeamSurvivalBalanceDamage" );
	lua_gettable( m_LuaState->GetScriptContext(), LUA_GLOBALSINDEX );
	lua_pushnumber( m_LuaState->GetScriptContext(), fCurDamage );
	lua_pushnumber( m_LuaState->GetScriptContext(), fBaseDamage );
	lua_pushnumber( m_LuaState->GetScriptContext(), fOwnerRate );
	lua_pushnumber( m_LuaState->GetScriptContext(), fEnemyRate );

	lua_call( m_LuaState->GetScriptContext(), 4, 1 );

	float fResult = (float)lua_tonumber( m_LuaState->GetScriptContext(), -1 );

	int iTop = lua_gettop( m_LuaState->GetScriptContext() );
	if( iTop > 0 )
	{
		lua_pop( m_LuaState->GetScriptContext(), iTop );
	}

	return fResult;
}

float MathManager::GetTeamSurvivalBalanceGauge( float fOwner, float fEnemy )
{
	if( g_BattleRoomMgr.IsTournamentModeType() || ioPlayMode::GetModeType() == MT_BATTLE )
		return FLOAT1;			// 경기방 보정 없음

	lua_pushstring_e( m_LuaState->GetScriptContext(), "TeamSurvivalBalanceGauge" );
	lua_gettable( m_LuaState->GetScriptContext(), LUA_GLOBALSINDEX );
	lua_pushnumber( m_LuaState->GetScriptContext(), fOwner );
	lua_pushnumber( m_LuaState->GetScriptContext(), fEnemy );

	lua_call( m_LuaState->GetScriptContext(), 2, 1 );

	float fResult = (float)lua_tonumber( m_LuaState->GetScriptContext(), -1 );

	int iTop = lua_gettop( m_LuaState->GetScriptContext() );
	if( iTop > 0 )
	{
		lua_pop( m_LuaState->GetScriptContext(), iTop );
	}

	return fResult;
}

float MathManager::GetFightClubTimeHPDamage( int iCurTime, int iMaxTime, float fOwnerHP, float fEnemyHP, float fCurDamage, float fBaseDamage )
{
	if( g_BattleRoomMgr.IsTournamentModeType() || ioPlayMode::GetModeType() == MT_BATTLE )
		return fCurDamage;			// 경기방 보정 없음

	lua_pushstring_e( m_LuaState->GetScriptContext(), "FightClubTimeHPDamage" );
	lua_gettable( m_LuaState->GetScriptContext(), LUA_GLOBALSINDEX );
	lua_pushnumber( m_LuaState->GetScriptContext(), iCurTime );
	lua_pushnumber( m_LuaState->GetScriptContext(), iMaxTime );
	lua_pushnumber( m_LuaState->GetScriptContext(), fOwnerHP );
	lua_pushnumber( m_LuaState->GetScriptContext(), fEnemyHP );
	lua_pushnumber( m_LuaState->GetScriptContext(), fCurDamage );
	lua_pushnumber( m_LuaState->GetScriptContext(), fBaseDamage );

	lua_call( m_LuaState->GetScriptContext(), 6, 1 );

	float fResult = (float)lua_tonumber( m_LuaState->GetScriptContext(), -1 );

	int iTop = lua_gettop( m_LuaState->GetScriptContext() );
	if( iTop > 0 )
	{
		lua_pop( m_LuaState->GetScriptContext(), iTop );
	}

	return fResult;
}

float MathManager::GetFightClubTimeHPGauge( int iCurTime, int iMaxTime, float fOwnerHP, float fEnemyHP )
{
	if( g_BattleRoomMgr.IsTournamentModeType() || ioPlayMode::GetModeType() == MT_BATTLE )
		return FLOAT1;			// 경기방 보정 없음

	lua_pushstring_e( m_LuaState->GetScriptContext(), "FightClubTimeHPGauge" );
	lua_gettable( m_LuaState->GetScriptContext(), LUA_GLOBALSINDEX );
	lua_pushnumber( m_LuaState->GetScriptContext(), iCurTime );
	lua_pushnumber( m_LuaState->GetScriptContext(), iMaxTime );
	lua_pushnumber( m_LuaState->GetScriptContext(), fOwnerHP );
	lua_pushnumber( m_LuaState->GetScriptContext(), fEnemyHP );

	lua_call( m_LuaState->GetScriptContext(), 4, 1 );

	float fResult = (float)lua_tonumber( m_LuaState->GetScriptContext(), -1 );

	int iTop = lua_gettop( m_LuaState->GetScriptContext() );
	if( iTop > 0 )
	{
		lua_pop( m_LuaState->GetScriptContext(), iTop );
	}

	return fResult;
}

float MathManager::RecalculateDamageByArmorClass( float fCurDamage, float fCharArmorClass )
{
	lua_pushstring_e( m_LuaState->GetScriptContext(), "ArmorClassCalc" );
	lua_gettable( m_LuaState->GetScriptContext(), LUA_GLOBALSINDEX );
	lua_pushnumber( m_LuaState->GetScriptContext(), fCurDamage );
	lua_pushnumber( m_LuaState->GetScriptContext(), fCharArmorClass );

	lua_call( m_LuaState->GetScriptContext(), 2, 1 );

	float fResult = (float)lua_tonumber( m_LuaState->GetScriptContext(), -1 );

	int iTop = lua_gettop( m_LuaState->GetScriptContext() );
	if( iTop > 0 )
	{
		lua_pop( m_LuaState->GetScriptContext(), iTop );
	}

	return fResult;
}

float MathManager::RecalculateSpeedBySpeedClass( float fCurMaxSpeed, float fCharSpeedClass )
{
	lua_pushstring_e( m_LuaState->GetScriptContext(), "SpeedClassCalc" );
	lua_gettable( m_LuaState->GetScriptContext(), LUA_GLOBALSINDEX );
	lua_pushnumber( m_LuaState->GetScriptContext(), fCurMaxSpeed );
	lua_pushnumber( m_LuaState->GetScriptContext(), fCharSpeedClass );

	lua_call( m_LuaState->GetScriptContext(), 2, 1 );

	float fResult = (float)lua_tonumber( m_LuaState->GetScriptContext(), -1 );

	int iTop = lua_gettop( m_LuaState->GetScriptContext() );
	if( iTop > 0 )
	{
		lua_pop( m_LuaState->GetScriptContext(), iTop );
	}

	return fResult;
}

float MathManager::GetStatAttribute( float fTarget, float fAttributeLev, float fChangeWith )
{
	lua_pushstring_e( m_LuaState->GetScriptContext(), "StatAttributeCalc" );
	lua_gettable( m_LuaState->GetScriptContext(), LUA_GLOBALSINDEX );
	lua_pushnumber( m_LuaState->GetScriptContext(), fTarget );
	lua_pushnumber( m_LuaState->GetScriptContext(), fAttributeLev );
	lua_pushnumber( m_LuaState->GetScriptContext(), fChangeWith );

	lua_call( m_LuaState->GetScriptContext(), 3, 1 );

	float fResult = (float)lua_tonumber( m_LuaState->GetScriptContext(), -1 );

	int iTop = lua_gettop( m_LuaState->GetScriptContext() );
	if( iTop > 0 )
	{
		lua_pop( m_LuaState->GetScriptContext(), iTop );
	}

	return fResult;
}

float MathManager::GetContributeBalanceForGauge( int iContribute, int iModeStyle )
{
	if( g_BattleRoomMgr.IsTournamentModeType() || ioPlayMode::GetModeType() == MT_BATTLE )
		return FLOAT1;			// 경기방 보정 없음

	lua_pushstring_e( m_LuaState->GetScriptContext(), "ContributeBalanceForGauge" );
	lua_gettable( m_LuaState->GetScriptContext(), LUA_GLOBALSINDEX );
	lua_pushnumber( m_LuaState->GetScriptContext(), iContribute );
	lua_pushnumber( m_LuaState->GetScriptContext(), iModeStyle );

	lua_call( m_LuaState->GetScriptContext(), 2, 1 );

	float fResult = (float)lua_tonumber( m_LuaState->GetScriptContext(), -1 );

	int iTop = lua_gettop( m_LuaState->GetScriptContext() );
	if( iTop > 0 )
	{
		lua_pop( m_LuaState->GetScriptContext(), iTop );
	}

	return fResult;
}

float MathManager::GetContributeBalanceForDamage( int iContribute, float fCurDamage, float fBaseDamage, int iModeStyle )
{
	if( g_BattleRoomMgr.IsTournamentModeType() || ioPlayMode::GetModeType() == MT_BATTLE )
		return fCurDamage;			// 경기방 보정 없음

	lua_pushstring_e( m_LuaState->GetScriptContext(), "ContributeBalanceForDamage" );
	lua_gettable( m_LuaState->GetScriptContext(), LUA_GLOBALSINDEX );

	lua_pushnumber( m_LuaState->GetScriptContext(), iContribute );
	lua_pushnumber( m_LuaState->GetScriptContext(), fCurDamage );
	lua_pushnumber( m_LuaState->GetScriptContext(), fBaseDamage );
	lua_pushnumber( m_LuaState->GetScriptContext(), iModeStyle );

	lua_call( m_LuaState->GetScriptContext(), 4, 1 );

	float fResult = (float)lua_tonumber( m_LuaState->GetScriptContext(), -1 );

	int iTop = lua_gettop( m_LuaState->GetScriptContext() );
	if( iTop > 0 )
	{
		lua_pop( m_LuaState->GetScriptContext(), iTop );
	}

	return fResult;
}

float MathManager::GetMemberBalance( int iOwnerMember, int iEnemyMember, int iModeStyle )
{
	if( g_BattleRoomMgr.IsTournamentModeType() || ioPlayMode::GetModeType() == MT_BATTLE )
		return FLOAT1;			// 경기방 보정 없음

	lua_pushstring_e( m_LuaState->GetScriptContext(), "TeamBalanceCalc" );
	lua_gettable( m_LuaState->GetScriptContext(), LUA_GLOBALSINDEX );
	lua_pushnumber( m_LuaState->GetScriptContext(), iOwnerMember );
	lua_pushnumber( m_LuaState->GetScriptContext(), iEnemyMember );
	lua_pushnumber( m_LuaState->GetScriptContext(), iModeStyle );

	lua_call( m_LuaState->GetScriptContext(), 3, 1 );

	float fResult = (float)lua_tonumber( m_LuaState->GetScriptContext(), -1 );

	int iTop = lua_gettop( m_LuaState->GetScriptContext() );
	if( iTop > 0 )
	{
		lua_pop( m_LuaState->GetScriptContext(), iTop );
	}

	return fResult;
}

float MathManager::GetMemberBalanceForBlowTime( int iOwnerMember, int iEnemyMember, int iModeType, int iModeStyle )
{
	if( g_BattleRoomMgr.IsTournamentModeType() || ioPlayMode::GetModeType() == MT_BATTLE )
		return FLOAT1;			// 경기방 보정 없음

	lua_pushstring_e( m_LuaState->GetScriptContext(), "TeamBalanceCalcForBlowTime" );
	lua_gettable( m_LuaState->GetScriptContext(), LUA_GLOBALSINDEX );
	lua_pushnumber( m_LuaState->GetScriptContext(), iOwnerMember );
	lua_pushnumber( m_LuaState->GetScriptContext(), iEnemyMember );
	lua_pushnumber( m_LuaState->GetScriptContext(), iModeType );
	lua_pushnumber( m_LuaState->GetScriptContext(), iModeStyle );

	lua_call( m_LuaState->GetScriptContext(), 4, 1 );

	float fResult = (float)lua_tonumber( m_LuaState->GetScriptContext(), -1 );

	int iTop = lua_gettop( m_LuaState->GetScriptContext() );
	if( iTop > 0 )
	{
		lua_pop( m_LuaState->GetScriptContext(), iTop );
	}

	return fResult;
}

float MathManager::GetMemberBalanceForGauge( int iOwnerMember, int iEnemyMember, int iModeType, int iModeStyle )
{
	if( g_BattleRoomMgr.IsTournamentModeType() || ioPlayMode::GetModeType() == MT_BATTLE )
		return FLOAT1;			// 경기방 보정 없음

	lua_pushstring_e( m_LuaState->GetScriptContext(), "TeamBalanceCalcForGauge" );
	lua_gettable( m_LuaState->GetScriptContext(), LUA_GLOBALSINDEX );
	lua_pushnumber( m_LuaState->GetScriptContext(), iOwnerMember );
	lua_pushnumber( m_LuaState->GetScriptContext(), iEnemyMember );
	lua_pushnumber( m_LuaState->GetScriptContext(), iModeType );
	lua_pushnumber( m_LuaState->GetScriptContext(), iModeStyle );

	lua_call( m_LuaState->GetScriptContext(), 4, 1 );

	float fResult = (float)lua_tonumber( m_LuaState->GetScriptContext(), -1 );

	int iTop = lua_gettop( m_LuaState->GetScriptContext() );
	if( iTop > 0 )
	{
		lua_pop( m_LuaState->GetScriptContext(), iTop );
	}

	return fResult;
}

float MathManager::GetMemberBalanceForDropDamage( int iOwnerMember, int iEnemyMember, float fBaseDamage )
{
	if( g_BattleRoomMgr.IsTournamentModeType() || ioPlayMode::GetModeType() == MT_BATTLE )
		return fBaseDamage;			// 경기방 보정 없음

	lua_pushstring_e( m_LuaState->GetScriptContext(), "TeamBalanceForDropDamage" );
	lua_gettable( m_LuaState->GetScriptContext(), LUA_GLOBALSINDEX );
	lua_pushnumber( m_LuaState->GetScriptContext(), iOwnerMember );
	lua_pushnumber( m_LuaState->GetScriptContext(), iEnemyMember );
	lua_pushnumber( m_LuaState->GetScriptContext(), fBaseDamage );

	lua_call( m_LuaState->GetScriptContext(), 3, 1 );

	float fResult = (float)lua_tonumber( m_LuaState->GetScriptContext(), -1 );

	int iTop = lua_gettop( m_LuaState->GetScriptContext() );
	if( iTop > 0 )
	{
		lua_pop( m_LuaState->GetScriptContext(), iTop );
	}

	return fResult;
}

float MathManager::GetMemberBalanceForMoveSpeed( int iOwnerMember, int iEnemyMember, int iModeType, int iModeStyle )
{
	if( g_BattleRoomMgr.IsTournamentModeType() || ioPlayMode::GetModeType() == MT_BATTLE )
		return FLOAT1;			// 경기방 보정 없음

	lua_pushstring_e( m_LuaState->GetScriptContext(), "TeamBalanceCalcForMoveSpeed" );
	lua_gettable( m_LuaState->GetScriptContext(), LUA_GLOBALSINDEX );
	lua_pushnumber( m_LuaState->GetScriptContext(), iOwnerMember );
	lua_pushnumber( m_LuaState->GetScriptContext(), iEnemyMember );
	lua_pushnumber( m_LuaState->GetScriptContext(), iModeType );
	lua_pushnumber( m_LuaState->GetScriptContext(), iModeStyle );

	lua_call( m_LuaState->GetScriptContext(), 4, 1 );

	float fResult = (float)lua_tonumber( m_LuaState->GetScriptContext(), -1 );

	int iTop = lua_gettop( m_LuaState->GetScriptContext() );
	if( iTop > 0 )
	{
		lua_pop( m_LuaState->GetScriptContext(), iTop );
	}

	return fResult;
}

float MathManager::GetSymbolExtraRate( int iOwnerTeamCnt, int iEnemyTeamCnt )
{
	if( g_BattleRoomMgr.IsTournamentModeType() || ioPlayMode::GetModeType() == MT_BATTLE )
		return FLOAT1;			// 경기방 보정 없음

	lua_pushstring_e( m_LuaState->GetScriptContext(), "SymbolBalance" );
	lua_gettable( m_LuaState->GetScriptContext(), LUA_GLOBALSINDEX );
	lua_pushnumber( m_LuaState->GetScriptContext(), iOwnerTeamCnt );
	lua_pushnumber( m_LuaState->GetScriptContext(), iEnemyTeamCnt );

	lua_call( m_LuaState->GetScriptContext(), 2, 1 );

	float fResult = (float)lua_tonumber( m_LuaState->GetScriptContext(), -1 );

	int iTop = lua_gettop( m_LuaState->GetScriptContext() );
	if( iTop > 0 )
	{
		lua_pop( m_LuaState->GetScriptContext(), iTop );
	}

	return fResult;
}

float MathManager::GetSymbolRecoveryRate( int iOwnerTeamCnt, int iEnemyTeamCnt )
{
	if( g_BattleRoomMgr.IsTournamentModeType() || ioPlayMode::GetModeType() == MT_BATTLE )
		return FLOAT1;			// 경기방 보정 없음

	lua_pushstring_e( m_LuaState->GetScriptContext(), "SymbolRecoveryRate" );
	lua_gettable( m_LuaState->GetScriptContext(), LUA_GLOBALSINDEX );
	lua_pushnumber( m_LuaState->GetScriptContext(), iOwnerTeamCnt );
	lua_pushnumber( m_LuaState->GetScriptContext(), iEnemyTeamCnt );

	lua_call( m_LuaState->GetScriptContext(), 2, 1 );

	float fResult = (float)lua_tonumber( m_LuaState->GetScriptContext(), -1 );

	int iTop = lua_gettop( m_LuaState->GetScriptContext() );
	if( iTop > 0 )
	{
		lua_pop( m_LuaState->GetScriptContext(), iTop );
	}

	return fResult;
}

float MathManager::GetRestorationValue( int iCharCnt, int iModeType )
{
	lua_pushstring_e( m_LuaState->GetScriptContext(), "RestorationValue" );
	lua_gettable( m_LuaState->GetScriptContext(), LUA_GLOBALSINDEX );
	lua_pushnumber( m_LuaState->GetScriptContext(), iCharCnt );
	lua_pushnumber( m_LuaState->GetScriptContext(), iModeType );

	lua_call( m_LuaState->GetScriptContext(), 2, 1 );

	float fResult = (float)lua_tonumber( m_LuaState->GetScriptContext(), -1 );

	int iTop = lua_gettop( m_LuaState->GetScriptContext() );
	if( iTop > 0 )
	{
		lua_pop( m_LuaState->GetScriptContext(), iTop );
	}

	return fResult;
}

float MathManager::GetStateProtectionTimeRate( int iOwnerTeamCnt, int iEnemyTeamCnt, int iType )
{
	if( g_BattleRoomMgr.IsTournamentModeType() || ioPlayMode::GetModeType() == MT_BATTLE )
		return FLOAT1;			// 경기방 보정 없음


	lua_pushstring_e( m_LuaState->GetScriptContext(), "StateProtectionTimeRate" );
	lua_gettable( m_LuaState->GetScriptContext(), LUA_GLOBALSINDEX );
	lua_pushnumber( m_LuaState->GetScriptContext(), iOwnerTeamCnt );
	lua_pushnumber( m_LuaState->GetScriptContext(), iEnemyTeamCnt );
	lua_pushnumber( m_LuaState->GetScriptContext(), iType );

	lua_call( m_LuaState->GetScriptContext(), 3, 1 );

	float fResult = (float)lua_tonumber( m_LuaState->GetScriptContext(), -1 );

	int iTop = lua_gettop( m_LuaState->GetScriptContext() );
	if( iTop > 0 )
	{
		lua_pop( m_LuaState->GetScriptContext(), iTop );
	}

	return fResult;
}

float MathManager::GetBuffItemDropRate( int iOwnerTeamCnt, int iEnemyTeamCnt )
{
	if( g_BattleRoomMgr.IsTournamentModeType() || ioPlayMode::GetModeType() == MT_BATTLE )
		return FLOAT1;			// 경기방 보정 없음

	lua_pushstring_e( m_LuaState->GetScriptContext(), "BuffItemDropRate" );
	lua_gettable( m_LuaState->GetScriptContext(), LUA_GLOBALSINDEX );
	lua_pushnumber( m_LuaState->GetScriptContext(), iOwnerTeamCnt );
	lua_pushnumber( m_LuaState->GetScriptContext(), iEnemyTeamCnt );

	lua_call( m_LuaState->GetScriptContext(), 2, 1 );

	float fResult = (float)lua_tonumber( m_LuaState->GetScriptContext(), -1 );

	int iTop = lua_gettop( m_LuaState->GetScriptContext() );
	if( iTop > 0 )
	{
		lua_pop( m_LuaState->GetScriptContext(), iTop );
	}

	return fResult;
}

float MathManager::CalcBallDamageSpeedAmt( float fDamage, float fPushPower, int iOwnerTeamCnt, int iEnemyTeamCnt, bool bTimeOut )
{
	lua_pushstring_e( m_LuaState->GetScriptContext(), "CalcBallDamageSpeedAmt" );
	lua_gettable( m_LuaState->GetScriptContext(), LUA_GLOBALSINDEX );
	lua_pushnumber( m_LuaState->GetScriptContext(), fDamage );
	lua_pushnumber( m_LuaState->GetScriptContext(), fPushPower );
	lua_pushnumber( m_LuaState->GetScriptContext(), iOwnerTeamCnt );
	lua_pushnumber( m_LuaState->GetScriptContext(), iEnemyTeamCnt );
	lua_pushboolean( m_LuaState->GetScriptContext(), bTimeOut );

	lua_call( m_LuaState->GetScriptContext(), 5, 1 );

	float fResult = (float)lua_tonumber( m_LuaState->GetScriptContext(), -1 );

	int iTop = lua_gettop( m_LuaState->GetScriptContext() );
	if( iTop > 0 )
	{
		lua_pop( m_LuaState->GetScriptContext(), iTop );
	}

	return fResult;
}

float MathManager::CalcBallDamageBoundAmt( float fDamage, float fBlowPower, int iOwnerTeamCnt, int iEnemyTeamCnt, bool bTimeOut )
{
	lua_pushstring_e( m_LuaState->GetScriptContext(), "CalcBallDamageBoundAmt" );
	lua_gettable( m_LuaState->GetScriptContext(), LUA_GLOBALSINDEX );
	lua_pushnumber( m_LuaState->GetScriptContext(), fDamage );
	lua_pushnumber( m_LuaState->GetScriptContext(), fBlowPower );
	lua_pushnumber( m_LuaState->GetScriptContext(), iOwnerTeamCnt );
	lua_pushnumber( m_LuaState->GetScriptContext(), iEnemyTeamCnt );
	lua_pushboolean( m_LuaState->GetScriptContext(), bTimeOut );

	lua_call( m_LuaState->GetScriptContext(), 5, 1 );

	float fResult = (float)lua_tonumber( m_LuaState->GetScriptContext(), -1 );

	int iTop = lua_gettop( m_LuaState->GetScriptContext() );
	if( iTop > 0 )
	{
		lua_pop( m_LuaState->GetScriptContext(), iTop );
	}

	return fResult;
}

float MathManager::GetFightClubVictoryDamage( int iOwnerVictory, int iEnemyVictory, float fCurDamage, float fBaseDamage )
{
	lua_pushstring_e( m_LuaState->GetScriptContext(), "FightClubVictoryDamage" );
	lua_gettable( m_LuaState->GetScriptContext(), LUA_GLOBALSINDEX );
	lua_pushnumber( m_LuaState->GetScriptContext(), iOwnerVictory );
	lua_pushnumber( m_LuaState->GetScriptContext(), iEnemyVictory );
	lua_pushnumber( m_LuaState->GetScriptContext(), fCurDamage );
	lua_pushnumber( m_LuaState->GetScriptContext(), fBaseDamage );

	lua_call( m_LuaState->GetScriptContext(), 4, 1 );

	float fResult = (float)lua_tonumber( m_LuaState->GetScriptContext(), -1 );

	int iTop = lua_gettop( m_LuaState->GetScriptContext() );
	if( iTop > 0 )
	{
		lua_pop( m_LuaState->GetScriptContext(), iTop );
	}

	return fResult;
}

float MathManager::GetFightClubVictoryGauge( int iOwnerVictory, int iEnemyVictory )
{
	lua_pushstring_e( m_LuaState->GetScriptContext(), "FightClubVictoryGauge" );
	lua_gettable( m_LuaState->GetScriptContext(), LUA_GLOBALSINDEX );
	lua_pushnumber( m_LuaState->GetScriptContext(), iOwnerVictory );
	lua_pushnumber( m_LuaState->GetScriptContext(), iEnemyVictory );

	lua_call( m_LuaState->GetScriptContext(), 2, 1 );

	float fResult = (float)lua_tonumber( m_LuaState->GetScriptContext(), -1 );

	int iTop = lua_gettop( m_LuaState->GetScriptContext() );
	if( iTop > 0 )
	{
		lua_pop( m_LuaState->GetScriptContext(), iTop );
	}

	return fResult;
}

////////////////////////////////////////////////////////////