

#include "stdafx.h"

#include "ShuffleRoomNode.h"
#include "../MainProcess.h"
#include "../EtcHelpFunc.h"
#include "../ioEtcLogManager.h"
#include "ModeHelp.h"
#include "User.h"

bool IsWinTeam( WinTeamType eWinTeam, TeamType eTeam )
{
	switch( eWinTeam )
	{
	case WTT_RED_TEAM:
	case WTT_VICTORY_RED_TEAM:
		if( eTeam == TEAM_RED )
			return true;
		break;
	case WTT_BLUE_TEAM:
	case WTT_VICTORY_BLUE_TEAM:
		if( eTeam == TEAM_BLUE )
			return true;
		break;
	}

	return false;
}

bool IsRedWin( WinTeamType eWinTeam )
{
	if( eWinTeam == WTT_RED_TEAM || eWinTeam == WTT_VICTORY_RED_TEAM )
		return true;

	return false;
}

bool IsBlueWin( WinTeamType eWinTeam )
{
	if( eWinTeam == WTT_BLUE_TEAM || eWinTeam == WTT_VICTORY_BLUE_TEAM )
		return true;

	return false;
}

TeamType ConvertStringToTeamType( const char *szType )
{
	if( !strcmp( szType, "RED" ) )
		return TEAM_RED;
	else if( !strcmp( szType, "BLUE" ) )
		return TEAM_BLUE;

	return TEAM_NONE;
}

ModeRecord::ModeRecord()
{
	pUser							= NULL;
	eState							= RS_LOADING;

	dwEnterRoomTime					= TIMEGETTIME();
	dwPlayingStartTime				= 0;
	dwPlayingTime					= 0;
	iTotalDamage					= 0;

	dwClassPlayingStartTime			= 0;
	bClassPlayTimeSort				= false;

	iTotalPeso						= 0;
	iTotalAddPeso					= 0;
	iTotalExp						= 0;
	iTotalLadderPoint				= 0;

	bResultLevelUP					= false;

	iPreRank						= 0;
	iCurRank						= 0;

	dwCurDieTime					= 0;
	dwRevivalGap					= 0;
	dwDeathTime						= 0;
	iRevivalCnt						= 0;

	iContribute						= 0;
	fContributePer					= 1.0f;

	iUniqueTotalKill				= 0;
	iUniqueTotalDeath				= 0;
	iVictories						= 0;

	bRecvDamageList					= false;
	bCatchState						= false;	
	bDieState						= false;
	bChatModeState					= false;
	bFishingState					= false;
	bExperienceState				= false;
	iExperienceClassType			= 0;
	bWriteLog						= false;
	bBonusAlarmSend					= false;

	eLastAttackerTeam				= TEAM_NONE;
	dwLastAttackerWeaponItemCode	= 0;
	bClassPlayHireTimeCheck			= true;

	for (int i = 0; i < BA_MAX ; i++)
		fBonusArray[i] = 0.0f;

	iResultClassTypeList.clear();
	iResultClassPointList.clear();
}

int ModeRecord::GetAllTotalKill()
{
	int iAllTotalKill = 0;
	LOOP_GUARD();
	KillDeathInfoMap::iterator iter = iKillInfoMap.begin();
	while( iter != iKillInfoMap.end() )
	{
		iAllTotalKill += iter->second;

		++iter;
	}
	LOOP_GUARD_CLEAR();

	return iAllTotalKill;
}

int ModeRecord::GetAllTotalDeath()
{
	int iAllTotalDeath = 0;
	LOOP_GUARD();
	KillDeathInfoMap::iterator iter = iDeathInfoMap.begin();
	while( iter != iDeathInfoMap.end() )
	{
		iAllTotalDeath += iter->second;

		++iter;
	}
	LOOP_GUARD_CLEAR();

	return iAllTotalDeath;
}

DWORD ModeRecord::GetAllPlayingTime()
{
	return dwPlayingTime;
}

DWORD ModeRecord::GetCharHireCheckTime()
{
	if( dwPlayingStartTime == 0 )
		return dwPlayingTime;

	DWORD dwGapTime = TIMEGETTIME() - dwPlayingStartTime;
	return dwPlayingTime + dwGapTime;
}

void ModeRecord::SetDieLastAttackerInfo( const ioHashString &szName, TeamType eTeam, DWORD dwItemCode )
{
	szLastAttackerName = szName;
	eLastAttackerTeam  = eTeam;
	dwLastAttackerWeaponItemCode = dwItemCode;
}

void ModeRecord::AddKillCount( RoomStyle eRoomStyle, ModeType eModeType, int iKill )
{
	int iClassType = pUser->GetCharClassType( pUser->GetSelectChar() );
	if( iClassType > 0 )
	{
		KillDeathInfoMap::iterator iter = iKillInfoMap.find( iClassType );
		if( iter != iKillInfoMap.end() )
		{
			iter->second += iKill;
			pUser->AddKillCount( eRoomStyle, eModeType, iKill );
		}
		else
		{
			iKillInfoMap.insert( KillDeathInfoMap::value_type( iClassType, iKill ) );
			pUser->AddKillCount( eRoomStyle, eModeType, iKill );
		}
	}
}

void ModeRecord::AddDeathCount( RoomStyle eRoomStyle, ModeType eModeType, int iDeath )
{
	int iClassType = pUser->GetCharClassType( pUser->GetSelectChar() );
	if( iClassType > 0 )
	{
		KillDeathInfoMap::iterator iter = iDeathInfoMap.find( iClassType );
		if( iter != iDeathInfoMap.end() )
		{
			iter->second += iDeath;
			pUser->AddDeathCount( eRoomStyle, eModeType, iDeath );
		}
		else
		{
			iDeathInfoMap.insert( KillDeathInfoMap::value_type( iClassType, iDeath ) );
			pUser->AddDeathCount( eRoomStyle, eModeType, iDeath );
		}
	}
}

void ModeRecord::PlayTimeInit()
{
	dwPlayingTime = 0;	
	vClassPlayTime.clear();
}

void ModeRecord::AddPlayingTime( DWORD addTime )
{
	if( dwPlayingStartTime == 0 ) 
		return;

	DWORD dwGapTime = TIMEGETTIME() - dwPlayingStartTime;
	dwPlayingTime += dwGapTime;
	dwPlayingTime += addTime;

	dwPlayingStartTime = 0;
}

void ModeRecord::StartPlaying()
{
	if( dwPlayingStartTime != 0 ) 
		return;

	dwPlayingStartTime = TIMEGETTIME();
}

ClassPlayTimeInfo &ModeRecord::GetClassPlayTimeInfo( DWORD dwClassCode )
{
	vClassPlayTimeInfo::iterator iter,iEnd;
	iEnd = vClassPlayTime.end();
	for(iter = vClassPlayTime.begin();iter != iEnd;iter++)
	{
		ClassPlayTimeInfo &kTimeInfo = *iter;
		if( kTimeInfo.dwClassCode == dwClassCode )
			return kTimeInfo;
	}

	ClassPlayTimeInfo kTimeInfo;
	kTimeInfo.dwClassCode   = dwClassCode;
	kTimeInfo.dwPlayingTime = 0;
	vClassPlayTime.push_back( kTimeInfo );

	int iSize = (int)vClassPlayTime.size();

	return vClassPlayTime[iSize - 1];
}

DWORD ModeRecord::GetClassPlayingTime( DWORD dwClassCode )
{
	ClassPlayTimeInfo &kPlayTimeInfo = GetClassPlayTimeInfo( dwClassCode );
	return kPlayTimeInfo.dwPlayingTime;
}

void ModeRecord::ClassPlayTimeInfoSort()
{
	if( bClassPlayTimeSort ) return; //이미 정렬했다. 최종결과시 한번만 정렬.
	
	std::sort( vClassPlayTime.begin(), vClassPlayTime.end(), ClassPlayTimeSort() );
	bClassPlayTimeSort = true;
}

void ModeRecord::AddClassPlayingTime( int iPrevCharType )
{
	if( iPrevCharType <= 0 )
	{
		// 현재 용병 타입 세팅
		iPrevCharType = pUser->GetSelectClassType();
		if( iPrevCharType == 0 )
		{
			LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL, "ModeRecord::AddClassPlayingTime Not Char Type: %s", pUser->GetPublicID().c_str() );
			return;
		}
	}

	if( dwClassPlayingStartTime == 0 ) 
	{
		ClassPlayTimeInfo &kPlayTimeInfo = GetClassPlayTimeInfo( iPrevCharType );	//생성만한다.
		return;	
	}
	ClassPlayTimeInfo &kPlayTimeInfo = GetClassPlayTimeInfo( iPrevCharType );	
	kPlayTimeInfo.dwPlayingTime += TIMEGETTIME() - dwClassPlayingStartTime;
	dwClassPlayingStartTime = 0;
}

DWORD ModeRecord::GetHighPlayingTime( IN int iCount, OUT IntVec &rkClassTypeList,OUT DWORDVec &rkPlayTimeList )
{
	if( vClassPlayTime.empty() ) return 0;
	if( !pUser ) return 0;

	int iCurMaxCnt = pUser->GetCurMaxCharSlot();
	if( !COMPARE( iCount , 1, iCurMaxCnt + 1 ) ) return 0;

	ClassPlayTimeInfoSort();
	
	int i = 0;
    int iLoop = min( (int)vClassPlayTime.size(), iCount ); 
	//예외 처리 : 경험치 받을 용병이 모두 재고용 상태인지 확인
	bool bAllInActive = true;
	for(i = 0;i < iLoop;i++)
	{
		ClassPlayTimeInfo &kPlayTimeInfo = vClassPlayTime[i];
		if( kPlayTimeInfo.dwPlayingTime <= 0 ) continue;
		
		if( pUser->IsActiveChar( kPlayTimeInfo.dwClassCode ) )
		{
			bAllInActive = false;
			break;
		}
	}

	//예외 처리 : 재고용중인 용병은 플레이시간 처리.
	for(i = 0;i < iLoop;i++)
	{
		ClassPlayTimeInfo &kPlayTimeInfo = vClassPlayTime[i];
		if( kPlayTimeInfo.dwPlayingTime <= 0 ) continue;
		
		if( !pUser->IsActiveChar( kPlayTimeInfo.dwClassCode ) )
		{
			// 재고용중인 용병은 플레이시간을 0으로 만든다. 
			// 단, 모두 재고용이면 첫번째 용병만 0으로 만들지 않는다.
			if( !bAllInActive || ( bAllInActive && i != 0 ) )
				kPlayTimeInfo.dwPlayingTime = 0;				
		}
	}

	int iPlayTimeCnt = 0;
	DWORD dwTotalTime = 0;
	for(i = 0;i < iLoop;i++)
	{
		ClassPlayTimeInfo &kPlayTimeInfo = vClassPlayTime[i];
		if( kPlayTimeInfo.dwPlayingTime <= 0 ) continue;

		rkClassTypeList.push_back(kPlayTimeInfo.dwClassCode);
		rkPlayTimeList.push_back(kPlayTimeInfo.dwPlayingTime);

		dwTotalTime  += kPlayTimeInfo.dwPlayingTime;
		iPlayTimeCnt++;
	}
	return dwTotalTime;
}

DWORD ModeRecord::GetCurrentHighPlayingTime( IN int iPrevCharType, IN int iCount, OUT IntVec &rkClassTypeList,OUT DWORDVec &rkPlayTimeList )
{
	if( vClassPlayTime.empty() ) return 0;
	if( !pUser ) return 0;

	int iCurMaxCnt = pUser->GetCurMaxCharSlot();
	if( !COMPARE( iCount , 1, iCurMaxCnt + 1 ) ) return 0;

	if( iPrevCharType <= 0 )
	{
		// 현재 용병 타입 세팅
		iPrevCharType = pUser->GetSelectClassType();
	}
	
    if( iPrevCharType > 0 )
	{
		ClassPlayTimeInfo &kPlayTimeInfo = GetClassPlayTimeInfo( iPrevCharType );	
	}

	//
	vClassPlayTimeInfo vCurrentPlayTime;
	vCurrentPlayTime.resize( vClassPlayTime.size() );
	std::copy( vClassPlayTime.begin(), vClassPlayTime.end(), vCurrentPlayTime.begin() );

	int i = 0;
	// 현재 플레이중인 용병 시간 적용
	if( iPrevCharType > 0 && dwClassPlayingStartTime > 0 )
	{
		for(i = 0;i < (int)vCurrentPlayTime.size();i++)
		{
			ClassPlayTimeInfo &kPlayTimeInfo = vCurrentPlayTime[i];
			if( kPlayTimeInfo.dwClassCode != (DWORD)iPrevCharType ) continue;

			kPlayTimeInfo.dwPlayingTime += TIMEGETTIME() - dwClassPlayingStartTime;
			break;
		}
	}
	std::sort( vCurrentPlayTime.begin(), vCurrentPlayTime.end(), ClassPlayTimeSort() );

	int iLoop = min( (int)vCurrentPlayTime.size(), iCount ); 
	//예외 처리 : 경험치 받을 용병이 모두 재고용 상태인지 확인
	bool bAllInActive = true;
	for(i = 0;i < iLoop;i++)
	{
		ClassPlayTimeInfo &kPlayTimeInfo = vCurrentPlayTime[i];
		if( kPlayTimeInfo.dwPlayingTime <= 0 ) continue;

		if( pUser->IsActiveChar( kPlayTimeInfo.dwClassCode ) )
		{
			bAllInActive = false;
			break;
		}
	}

	//예외 처리 : 재고용중인 용병은 플레이시간 처리.
	for(i = 0;i < iLoop;i++)
	{
		ClassPlayTimeInfo &kPlayTimeInfo = vCurrentPlayTime[i];
		if( kPlayTimeInfo.dwPlayingTime <= 0 ) continue;

		if( !pUser->IsActiveChar( kPlayTimeInfo.dwClassCode ) )
		{
			// 재고용중인 용병은 플레이시간을 0으로 만든다. 
			// 단, 모두 재고용이면 첫번째 용병만 0으로 만들지 않는다.
			if( !bAllInActive || ( bAllInActive && i != 0 ) )
				kPlayTimeInfo.dwPlayingTime = 0;				
		}
	}

	int iPlayTimeCnt = 0;
	DWORD dwTotalTime = 0;
	for(i = 0;i < iLoop;i++)
	{
		ClassPlayTimeInfo &kPlayTimeInfo = vCurrentPlayTime[i];
		if( kPlayTimeInfo.dwPlayingTime <= 0 ) continue;

		rkClassTypeList.push_back(kPlayTimeInfo.dwClassCode);
		rkPlayTimeList.push_back(kPlayTimeInfo.dwPlayingTime);

		dwTotalTime  += kPlayTimeInfo.dwPlayingTime;
		iPlayTimeCnt++;
	}
	return dwTotalTime;
}

int ModeRecord::GetHighPlayingClass()
{
	if( vClassPlayTime.empty() )
		return 0;

	ClassPlayTimeInfoSort();

	return vClassPlayTime[0].dwClassCode;
}

DWORD ModeRecord::GetDeathTime()
{
	return dwDeathTime;
}

void ModeRecord::AddDeathTime( DWORD dwCurRoundDeathTime )
{
	dwDeathTime += dwCurRoundDeathTime;
}

void ModeRecord::CheckLoadingTime()
{
	if( eState != RS_LOADING ) return;

	DWORD dwSec = ( TIMEGETTIME() - dwEnterRoomTime ) / 1000;
	g_EtcLogMgr.RoomEnterLoadTime( dwSec + 1 );
}

int ModeRecord::GetKillCount( int iClassType )
{
	KillDeathInfoMap::iterator iter = iKillInfoMap.find( iClassType );
	if( iter != iKillInfoMap.end() )
		return iter->second;

	return 0;
}

int ModeRecord::GetDeathCount( int iClassType )
{
	KillDeathInfoMap::iterator iter = iDeathInfoMap.find( iClassType );
	if( iter != iDeathInfoMap.end() )
		return iter->second;

	return 0;
}

