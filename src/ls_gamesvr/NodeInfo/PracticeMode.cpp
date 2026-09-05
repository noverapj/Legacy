#include <stdafx.h>
#include "../MainProcess.h"
#include "PracticeMode.h"
#include "Room.h"
#include "User.h"

#include "PracticeManager.h"
#include "ioHeroManager.h"
#include "ioSetItemInfo.h"
#include "../DataBase/DBClient.h"
#include "../DataBase/LogDBClient.h"
#include "../MainServerNode/MainServerNode.h"

extern CLog TradeLOG;

PracticeMode::PracticeMode( Room *pCreator ) : Mode( pCreator )
{
	m_iPracticeIndex = 0;
	m_vRecordList.clear();
}

PracticeMode::~PracticeMode()
{

}

void PracticeMode::InitMode()
{
	m_dwModeStartTime = TIMEGETTIME();

	LoadINIValue();

	SetModeState( MS_READY );
}

void PracticeMode::AddNewRecord( User *pUser )
{
	if(pUser)
	{
		pUser->SetTeam(TEAM_BLUE);
		ModeRecord kRecord;
		kRecord.pUser = pUser;
		m_vRecordList.push_back( kRecord );
	}
}

void PracticeMode::RemoveRecord( User *pUser, bool bRoomDestroy /*= false */ )
{
	if(!pUser) return;

	int iCharCnt = m_vRecordList.size();
	for( int i=0 ; i<iCharCnt ; i++ )
	{
		if( m_vRecordList[i].pUser == pUser )
		{
			m_vRecordList.erase( m_vRecordList.begin() + i );
			break;
		}
	}

	UpdateUserRank();

	if( pUser )
	{
		m_KickOutVote.RemoveVoteUserList( pUser->GetPublicID() );
		//RemoveRecordChangeCharacterSync( pUser->GetPublicID() );
	}
}

ModeRecord* PracticeMode::FindModeRecord( const ioHashString &rkName )
{
	if( rkName.IsEmpty() )	return NULL;

	CRASH_GUARD();
	int iRecordCnt = m_vRecordList.size();
	for( int i=0 ; i<iRecordCnt ; i++ )
	{
		if( m_vRecordList[i].pUser->GetPublicID() == rkName )
			return &m_vRecordList[i];
	}

	return NULL;
}

ModeRecord* PracticeMode::FindModeRecord( User *pUser )
{
	if( !pUser )	return NULL;

	int iRecordCnt = m_vRecordList.size();
	for( int i=0 ; i<iRecordCnt ; i++ )
	{
		if( m_vRecordList[i].pUser == pUser )
			return &m_vRecordList[i];
	}

	return NULL;
}

ModeRecord* PracticeMode::FindModeRecord( int iIdx )
{
	if( COMPARE( iIdx, 0, GetRecordCnt() ) )
		return &m_vRecordList[iIdx];

	return NULL;
}

ModeRecord* PracticeMode::FindModeRecordIgnoreCase( const ioHashString &rkName )
{
	if( rkName.IsEmpty() )	return NULL;

	CRASH_GUARD();
	int iRecordCnt = m_vRecordList.size();
	for( int i=0 ; i<iRecordCnt ; i++ )
	{
		if( _stricmp(m_vRecordList[i].pUser->GetPublicID().c_str(), rkName.c_str() ) == 0 )
			return &m_vRecordList[i];
	}

	return NULL;
}

bool PracticeMode::CheckTCPPacket( SP2Packet &rkPacket )
{
	if( Mode::CheckTCPPacket(rkPacket) )
		return true;

	switch( rkPacket.GetPacketID() )
	{
	case CTPK_PRISONER_ESCAPE:
	case CTPK_PRISONER_DROP:
	case CTPK_PRISONERMODE:
		if(GetModeState() != MS_PLAY)
			return true;
		break;
	}

	return false;
}

bool PracticeMode::ProcessTCPPacket( User *pSend, SP2Packet &rkPacket )
{
	if( Mode::ProcessTCPPacket( pSend, rkPacket ) )
		return true;

	switch( rkPacket.GetPacketID() )
	{
	case CTPK_PRISONER_ESCAPE:
		OnPrisonerEscape( pSend, rkPacket );
		return true;
	case CTPK_PRISONER_DROP:
		OnPrisonerDrop( pSend, rkPacket );
		return true;
	case CTPK_PRISONERMODE:
		OnPrisonerMode( pSend, rkPacket );
		return true;
	case CTPK_PRACTICE_SELECT_REGULAR:
		OnSelectRegular( pSend, rkPacket );
		return true;
	case CTPK_PRACTICE_GAME_START:
		OnPractice_GameStart( pSend, rkPacket );
		return true;
	case CTPK_PRACTICE_RESULT:
		OnPracticeResult( pSend, rkPacket );
		return true;
	}

	return false;
}

void PracticeMode::ProcessReady()
{
	DWORD dwCurTime = TIMEGETTIME();
	if( m_dwStateChangeTime + m_dwReadyStateTime >= dwCurTime )
		return;

	// 유저 입장 대기중이면 플레이 상태로 전환하지 않는다.
	if( !m_pCreator )
		return;
	if( m_pCreator->IsRoomEnterUserDelay() )
		return;

	SP2Packet kPacket( STPK_ROUND_START );
	PACKET_GUARD_VOID( kPacket.Write(m_iCurRound) );
	PACKET_GUARD_VOID( kPacket.Write(m_iPracticeIndex) );
	SendRoomAllUser( kPacket );
	SetModeState( MS_PLAY );

	m_dwRoundStartTime = TIMEGETTIME();
	// 레디 상태에서 이탈한 유저에 대한 체크
	CheckUserLeaveEnd();
}

int PracticeMode::SetRegularSoldierItem( int iSlot, OUT int &iItemCode )
{
	if( m_vRecordList[0].pUser )
	{
		if( iItemCode == 0 )
		{
			int iClass = g_PracticeMgr.GetRegularSoldierClass( m_iPracticeIndex );
			iItemCode = (iSlot * 100000) + iClass;
		}		

		ioCharacter *pCharater = m_vRecordList[0].pUser->GetCharacter(0);
		if( pCharater )
		{
			ioItem *pItem = NULL;
			pItem = m_pCreator->CreateItemByCode( iItemCode );

			if( pItem )
			{
				pItem->SetOwnerName( m_vRecordList[0].pUser->GetPublicID() );
				pCharater->EquipItem( iSlot, pItem );
				
				return pItem->GetGameIndex();
			}
		}
	}

	return 0;
}

void PracticeMode::ProcessPlay()
{
	ProcessRevival();
	CheckFieldItemLiveTime();
	CheckItemSupply( m_dwStateChangeTime );
	CheckBallSupply( m_dwStateChangeTime );
	CheckMachineSupply( m_dwStateChangeTime );
	CheckNeedSendPushStruct();
	CheckRoundEnd( true );
	ProcessEvent();
	ProcessBonusAlarm();
}

void PracticeMode::ProcessRevival()
{
}

void PracticeMode::InitObjectGroupList()
{
	Mode::InitObjectGroupList();
}

int PracticeMode::GetRecordCnt() const
{
	return m_vRecordList.size();
}

void PracticeMode::OnPrisonerEscape( User *pUser, SP2Packet &rkPacket )
{
	if( !pUser ) return;

	DWORD dwAttackerIndex = 0;
	ioHashString szPrisonerName, szAttackerName, szLastAttackerSkill;

	PACKET_GUARD_VOID( rkPacket.Read(szPrisonerName) );
	PACKET_GUARD_VOID( rkPacket.Read(szAttackerName) );
	PACKET_GUARD_VOID( rkPacket.Read(dwAttackerIndex) );	
	PACKET_GUARD_VOID( rkPacket.Read(szLastAttackerSkill) );

	SP2Packet kReturn( STPK_PRISONER_ESCAPE );
	PACKET_GUARD_VOID( kReturn.Write(szPrisonerName) );
	PACKET_GUARD_VOID( kReturn.Write(dwAttackerIndex) );
	PACKET_GUARD_VOID( kReturn.Write(szLastAttackerSkill) );
	SendRoomAllUser( kReturn );
}

void PracticeMode::OnPrisonerDrop( User *pUser, SP2Packet &rkPacket )
{

}

void PracticeMode::OnPrisonerMode( User *pUser, SP2Packet &rkPacket )
{

}

void PracticeMode::OnSelectRegular( User *pUser, SP2Packet &rkPacket )
{
	if( GetRecordCnt() == 1 )
	{
		if( m_vRecordList[0].pUser )
		{
			int iCloak		= g_PracticeMgr.GetRegularSoldierCloak( m_iPracticeIndex );
			int iCloakIndex = SetRegularSoldierItem( EQUIP_CLOAK, iCloak);

			int iHelmet		= g_PracticeMgr.GetRegularSoldierHelmet( m_iPracticeIndex );
			int iHelmetIndex= SetRegularSoldierItem( EQUIP_HELM, iHelmet);

			int iArmor		= g_PracticeMgr.GetRegularSoldierArmor( m_iPracticeIndex );
			int iArmorIndex = SetRegularSoldierItem( EQUIP_ARMOR, iArmor);

			int iWeapon		= g_PracticeMgr.GetRegularSoldierWeapon( m_iPracticeIndex );
			int iWeaponIndex= SetRegularSoldierItem( EQUIP_WEAPON, iWeapon);
			
			int iClass		= g_PracticeMgr.GetRegularSoldierClass( m_iPracticeIndex );
			BYTE byGender	= g_PracticeMgr.GetRegularSoldierGender( m_iPracticeIndex );

			int iUnderwear	= g_PracticeMgr.GetRegularSoldierUnderwear( m_iPracticeIndex );
			int iHair		= g_PracticeMgr.GetRegularSoldierHair( m_iPracticeIndex );
			int iHairColor	= g_PracticeMgr.GetRegularSoldierHairColor( m_iPracticeIndex );
			int iFace		= g_PracticeMgr.GetRegularSoldierFace( m_iPracticeIndex );
			int iSkinColor	= g_PracticeMgr.GetRegularSoldierSkinColor( m_iPracticeIndex );

			const ioSetItemInfo *pSetInfo = g_ioHeroManager.GetHeroInfo( iClass + SET_ITEM_CODE );
			
			if( pSetInfo )
			{
				if( iUnderwear <= 0 )
					iUnderwear	= pSetInfo->GetPresetDecoCode( byGender, ioSetItemInfo::IDT_UNDERWEAR );
				if( iHair <= 0 )
					iHair		= pSetInfo->GetPresetDecoCode( byGender, ioSetItemInfo::IDT_HAIR );
				if( iHairColor <= 0 )
					iHairColor	= pSetInfo->GetPresetDecoCode( byGender, ioSetItemInfo::IDT_HAIR_COLOR );
				if( iFace <= 0 )
					iFace		= pSetInfo->GetPresetDecoCode( byGender, ioSetItemInfo::IDT_FACE );
				if( iSkinColor <= 0 )
					iSkinColor	= pSetInfo->GetPresetDecoCode( byGender, ioSetItemInfo::IDT_SKIN );
			}

			SP2Packet kPacket( STPK_PRACTICE_SELECT_REGULAR );
			PACKET_GUARD_VOID( kPacket.Write(byGender) );
			PACKET_GUARD_VOID( kPacket.Write(iClass) );
			PACKET_GUARD_VOID( kPacket.Write(iUnderwear) );
			PACKET_GUARD_VOID( kPacket.Write(iHair) );
			PACKET_GUARD_VOID( kPacket.Write(iHairColor) );
			PACKET_GUARD_VOID( kPacket.Write(iFace) );
			PACKET_GUARD_VOID( kPacket.Write(iSkinColor) )

			PACKET_GUARD_VOID( kPacket.Write(iCloak) );
			PACKET_GUARD_VOID( kPacket.Write(iCloakIndex) );
			PACKET_GUARD_VOID( kPacket.Write(iHelmet) );
			PACKET_GUARD_VOID( kPacket.Write(iHelmetIndex) );
			PACKET_GUARD_VOID( kPacket.Write(iArmor) );
			PACKET_GUARD_VOID( kPacket.Write(iArmorIndex) );
			PACKET_GUARD_VOID( kPacket.Write(iWeapon) );
			PACKET_GUARD_VOID( kPacket.Write(iWeaponIndex) );

			m_vRecordList[0].pUser->SendMessage( kPacket );
		}
	}
}

void PracticeMode::OnPractice_GameStart( User *pUser, SP2Packet &rkPacket )
{
	if( !pUser ) return;

	int  iPracticeIndex = 0;
	PACKET_GUARD_VOID( rkPacket.Read( iPracticeIndex ) );

	if(iPracticeIndex != GetPracticeIndex())
	{
		SP2Packet kPacket( STPK_PRACTICE_GAME_START );
		PACKET_GUARD_VOID( kPacket.Write( PRACTICE_RESULT_FAIL ) );
		PACKET_GUARD_VOID( kPacket.Write( iPracticeIndex ) );
		pUser->SendMessage( kPacket );
		return;
	}

	ioUserPractice* pUserPractice = pUser->GetUserPractice();
	if( !pUserPractice )
	{
		SP2Packet kPacket( STPK_PRACTICE_GAME_START );
		PACKET_GUARD_VOID( kPacket.Write( PRACTICE_RESULT_FAIL ) );
		PACKET_GUARD_VOID( kPacket.Write( iPracticeIndex ) );
		pUser->SendMessage( kPacket );
		return;
	}


	LSC_Practice* pkPractice = g_PracticeMgr.GetLSCPractice(iPracticeIndex);
	if(NULL == pkPractice)
	{
		SP2Packet kPacket( STPK_PRACTICE_GAME_START );
		PACKET_GUARD_VOID( kPacket.Write( PRACTICE_RESULT_FAIL ) );
		PACKET_GUARD_VOID( kPacket.Write( iPracticeIndex ) );
		pUser->SendMessage( kPacket );
		return;
	}

	boost::posix_time::ptime t = boost::posix_time::microsec_clock::local_time();

	SetPracticeIndex( iPracticeIndex );
	SetBoostPracticeStartTime(t);

	SP2Packet kPacket( STPK_PRACTICE_GAME_START );
	PACKET_GUARD_VOID( kPacket.Write( PRACTICE_RESULT_SUCCESS ) );
	PACKET_GUARD_VOID( kPacket.Write( GetPracticeIndex() ) );
	pUser->SendMessage( kPacket );
}


void PracticeMode::OnPracticeResult( User *pUser, SP2Packet &rkPacket )
{
	if( !pUser ) return;

	DWORD dwGrade = 0, dwTempGrade = 0;
	
	PACKET_GUARD_VOID( rkPacket.Read( dwGrade ) );
	
	char szValue[MAX_PATH]="";
	StringCbPrintf( szValue, sizeof( szValue ), "%s", g_App.GetGameServerName().c_str());

	char szCipher[MAX_PATH]="";
	ioEncrypted::Encode15(std::to_string( static_cast<long long>(pUser->GetUserIndex())).c_str(), (char*)pUser->GetPacketGUID().c_str(), szCipher, ioEncrypted::NT_KOREA);

	char szPrivateID[DATA_LEN]="";
	ioHashString szEncLoginKeyAndID;
	PACKET_GUARD_VOID( rkPacket.Read(szEncLoginKeyAndID) );		//id 값이어야..
	if(!ioEncrypted::Decode15((char*)szEncLoginKeyAndID.c_str(), (char*)pUser->GetPacketGUID().c_str(), szPrivateID))
	{
		SP2Packet kPacket( STPK_PRACTICE_RESULT );
		PACKET_GUARD_VOID( kPacket.Write(PRACTICE_RESULT_FAIL) );
		pUser->SendMessage( kPacket );
		TradeLOG.PrintTimeAndLog( LOG_DEBUG_LEVEL, "[error][Practice] strcmp : [UserIndex %d] [ID %s] [%s] Encrypted [%s] [%s] EncrypKey[%s]", pUser->GetUserIndex(), pUser->GetPublicID().c_str(), szPrivateID, szCipher,  szEncLoginKeyAndID.c_str(), pUser->GetPacketGUID().c_str());
		g_LogDBClient.OnInsertGameLogInfo(LogDBClient::GLT_PRACTICE_RESULT, pUser, 0, 0, LogDBClient::LET_ETC, GetPracticeIndex(), dwGrade, 1, 0, szValue);
		return;
	}

	if( strcmp( std::to_string( static_cast<long long>(pUser->GetUserIndex())).c_str(), szPrivateID ) != 0 )
	{
		SP2Packet kPacket( STPK_PRACTICE_RESULT );
		PACKET_GUARD_VOID( kPacket.Write(PRACTICE_RESULT_FAIL) );
		TradeLOG.PrintTimeAndLog( LOG_DEBUG_LEVEL, "[error][Practice] strcmp : [UserIndex %d] [ID %s] [%s] Encrypted [%s] [%s] EncrypKey[%s]", pUser->GetUserIndex(), pUser->GetPublicID().c_str(), szPrivateID, szCipher,  szEncLoginKeyAndID.c_str(), pUser->GetPacketGUID().c_str());
		pUser->SendMessage( kPacket );
		g_LogDBClient.OnInsertGameLogInfo(LogDBClient::GLT_PRACTICE_RESULT, pUser, 0, 0, LogDBClient::LET_ETC, GetPracticeIndex(), dwGrade, 2, 0, szValue);
		return;
	}

	ioUserPractice* pUserPractice = pUser->GetUserPractice();
	if( !pUserPractice )
	{
		SP2Packet kPacket( STPK_PRACTICE_RESULT );
		PACKET_GUARD_VOID( kPacket.Write( PRACTICE_RESULT_FAIL ) );
		pUser->SendMessage( kPacket );
		TradeLOG.PrintTimeAndLog( LOG_DEBUG_LEVEL, "[error][Practice] ioUserPractice : [UserIndex %d] [ID %s] ", pUser->GetUserIndex(), pUser->GetPublicID().c_str());
		g_LogDBClient.OnInsertGameLogInfo(LogDBClient::GLT_PRACTICE_RESULT, pUser, 0, 0, LogDBClient::LET_ETC, GetPracticeIndex(), dwGrade, 3, 0, szValue);
		return;
	}
	
	LSC_Practice* pkPractice = g_PracticeMgr.GetLSCPractice(GetPracticeIndex());
	if(NULL == pkPractice)
	{
		SP2Packet kPacket( STPK_PRACTICE_RESULT );
		PACKET_GUARD_VOID( kPacket.Write(PRACTICE_RESULT_FAIL) );
		pUser->SendMessage( kPacket );
		TradeLOG.PrintTimeAndLog( LOG_DEBUG_LEVEL, "[error][Practice] LSC_Practice : [UserIndex %d] [ID %s]", pUser->GetUserIndex(), pUser->GetPublicID().c_str());
		g_LogDBClient.OnInsertGameLogInfo(LogDBClient::GLT_PRACTICE_RESULT, pUser, 0, 0, LogDBClient::LET_ETC, GetPracticeIndex(), dwGrade, 4, 0, szValue);
		return;
	}
	
	SPractice kPractice = pUserPractice->GetPractice(GetPracticeIndex());
	DWORDVec vGrade, vReward, vGiveReward;
	g_PracticeMgr.GetPracticeReward( GetPracticeIndex(), dwGrade, vGrade, vReward );

	if( vGrade.size() == 0 )
	{
		SP2Packet kPacket( STPK_PRACTICE_RESULT );
		PACKET_GUARD_VOID( kPacket.Write( PRACTICE_RESULT_FAIL ) );
		pUser->SendMessage( kPacket );
		TradeLOG.PrintTimeAndLog( LOG_DEBUG_LEVEL, "[error][Practice]GetPracticeReward - [UserIndex %d] [Index:%d] [m_dwGrade:%d] [m_dwCount:%d] [m_dwTime:%d] [m_dwRank:%d]",
								 pUser->GetUserIndex(), kPractice.m_dwID, kPractice.m_dwGrade, kPractice.m_dwCount, kPractice.m_dwTime, kPractice.m_dwRank);
		g_LogDBClient.OnInsertGameLogInfo(LogDBClient::GLT_PRACTICE_RESULT, pUser, 0, 0, LogDBClient::LET_ETC, GetPracticeIndex(), dwGrade, 5, kPractice.m_dwTime, szValue);
		return;
	}

	bool bUpdate = false;
	if( kPractice.m_dwGrade >= dwGrade )
	{
		dwTempGrade = kPractice.m_dwGrade;
	}
	else
	{
		dwTempGrade = dwGrade;
		bUpdate = true;
	}

	pUserPractice->SetBoostPracticeEndTime(boost::posix_time::microsec_clock::local_time());
	boost::posix_time::time_duration diff = boost::posix_time::microsec_clock::local_time() -  GetBoostPracticeStartTime();
	int ms = diff.total_milliseconds();

	if( ms < 0 )
	{
		SP2Packet kPacket( STPK_PRACTICE_RESULT );
		PACKET_GUARD_VOID( kPacket.Write( PRACTICE_RESULT_FAIL ) );
		pUser->SendMessage( kPacket );
		TradeLOG.PrintTimeAndLog( LOG_DEBUG_LEVEL, "[error][Practice]total_milliseconds - [UserIndex %d] [Index:%d] [m_dwGrade:%d] [m_dwCount:%d] [m_dwTime:%d] [m_dwRank:%d]",
								 pUser->GetUserIndex(), kPractice.m_dwID, kPractice.m_dwGrade, kPractice.m_dwCount, kPractice.m_dwTime, kPractice.m_dwRank);
		g_LogDBClient.OnInsertGameLogInfo(LogDBClient::GLT_PRACTICE_RESULT, pUser, 0, 0, LogDBClient::LET_ETC, GetPracticeIndex(), dwGrade, 6, kPractice.m_dwTime, szValue);
		return;
	}

	int iEndTime = 0;
	PACKET_GUARD_VOID(rkPacket.Read( iEndTime ));


	if(abs(ms -iEndTime) > PRACTICE_DIFF_TIME)
	{
		TradeLOG.PrintTimeAndLog( LOG_DEBUG_LEVEL, "[error][Practice]PRACTICE_DIFF_TIME : ID [%s] Practice [%d] Update [%d] Time Mintime [%d] CurrentTime [%d] ClientTime [%d] ServerTime [%d]", pUser->GetPublicID().c_str(), GetPracticeIndex(), bUpdate, pkPractice->MinTime, kPractice.m_dwTime, iEndTime, ms);
		iEndTime = ms;
	}

	if(iEndTime < kPractice.m_dwTime)
	{
		bUpdate = true;
	}
	else
	{
		iEndTime = kPractice.m_dwTime;
	}

	// OHTGTEST 이곳에서 퀘스트 체크해서 있으면 완료처리한다.
	QuestData kQuestData = pUser->GetQuest()->GetQuestData(QuestClass::QCN_PRACTICE_SUCCESS);
	if(NULL != kQuestData.GetLinkQuest())
	{
		kQuestData.GetLinkQuest()->lsCheckAttainTerm(pUser, kQuestData, pUser->GetGradeLevel(), kPractice.m_dwID, dwGrade);
	}

	if(pkPractice->MinTime > iEndTime)
	{
		SP2Packet kPacket( STPK_PRACTICE_RESULT );
		PACKET_GUARD_VOID( kPacket.Write( PRACTICE_RESULT_TIME_SHORT ) );
		pUser->SendMessage( kPacket );
		TradeLOG.PrintTimeAndLog( LOG_DEBUG_LEVEL, "[error][Practice]MinTime > iEndTime - [UserIndex %d] [Index:%d] [m_dwGrade:%d] [m_dwCount:%d] [m_dwTime:%d] [m_dwRank:%d]",
								 pUser->GetUserIndex(), kPractice.m_dwID, kPractice.m_dwGrade, kPractice.m_dwCount, kPractice.m_dwTime, kPractice.m_dwRank);
		g_LogDBClient.OnInsertGameLogInfo(LogDBClient::GLT_PRACTICE_RESULT, pUser, 0, 0, LogDBClient::LET_ETC, GetPracticeIndex(), dwGrade, 7, kPractice.m_dwTime, szValue);
		return ;
	}

	StringCbPrintf( szValue, sizeof( szValue ), "%d", kPractice.m_dwRank );
	g_LogDBClient.OnInsertGameLogInfo(LogDBClient::GLT_PRACTICE_RESULT, pUser, 0, 0, LogDBClient::LET_ETC, GetPracticeIndex(), dwTempGrade, kPractice.m_dwCount, iEndTime, szValue);

	if( bUpdate )
	{//물어볼거
		g_DBClient.OnUpdatePracticeTime(pUser->GetUserDBAgentID(), pUser->GetAgentThreadID(), pUser->GetUserIndex(), GetPracticeIndex(), dwTempGrade, dwGrade, kPractice.m_dwCount, iEndTime, kPractice.m_dwRank );
	}

    SHORT    RewardType = 0;
    INT      RewardValue = 0;
    INT      RewardCount = 0;

	// 무조건 보상을 준다
	ioHashString szSendID		= g_MainServer.GetSendID();
	CTimeSpan cPresentGapTime( pkPractice->PresentTime, 0, 0, 0 );
	CTime kPresentTime = CTime::GetCurrentTime() + cPresentGapTime;
	if( g_PracticeMgr.E_GRADE_A <= dwGrade )
	{
			RewardType = pkPractice->RewardAType;
			RewardValue = pkPractice->RewardAValue;
			RewardCount = pkPractice->RewardACount;

			pUser->AddPresentMemory( szSendID, RewardType, RewardValue, RewardCount, 0, 0, pkPractice->PRESENTMENT, kPresentTime, 0);
			g_LogDBClient.OnInsertPresent( 0, szSendID, g_App.GetPublicIP().c_str(), pUser->GetUserIndex(), RewardType, RewardValue, 
				RewardCount, 0, 0, LogDBClient::PST_PRACTICE, "PracticePresent" );
			pUser->SendPresentMemory();
	}
	if( g_PracticeMgr.E_GRADE_B <= dwGrade )
	{
			RewardType = pkPractice->RewardBType;
			RewardValue = pkPractice->RewardBValue;
			RewardCount = pkPractice->RewardBCount;
	
			pUser->AddPresentMemory( szSendID, RewardType, RewardValue, RewardCount, 0, 0, pkPractice->PRESENTMENT, kPresentTime, 0);
			g_LogDBClient.OnInsertPresent( 0, szSendID, g_App.GetPublicIP().c_str(), pUser->GetUserIndex(), RewardType, RewardValue, 
				RewardCount, 0, 0, LogDBClient::PST_PRACTICE, "PracticePresent" );
			pUser->SendPresentMemory();
	}
	if( g_PracticeMgr.E_GRADE_C <= dwGrade )
	{
			RewardType = pkPractice->RewardCType;
			RewardValue = pkPractice->RewardCValue;
			RewardCount = pkPractice->RewardCCount;

			pUser->AddPresentMemory( szSendID, RewardType, RewardValue, RewardCount, 0, 0, pkPractice->PRESENTMENT, kPresentTime, 0);
			g_LogDBClient.OnInsertPresent( 0, szSendID, g_App.GetPublicIP().c_str(), pUser->GetUserIndex(), RewardType, RewardValue, 
				RewardCount, 0, 0, LogDBClient::PST_PRACTICE, "PracticePresent" );
			pUser->SendPresentMemory();
	}

//	int iSize = vGiveReward.size();

	if(false == bUpdate)
	{
		SP2Packet kPacket( STPK_PRACTICE_RESULT );
		PACKET_GUARD_VOID( kPacket.Write( PRACTICE_RESULT_SUCCESS ) );
		PACKET_GUARD_VOID( kPacket.Write( GetPracticeIndex() ) );
		PACKET_GUARD_VOID( kPacket.Write( dwGrade ) );
		PACKET_GUARD_VOID( kPacket.Write( kPractice.m_dwRank ) );
		pUser->SendMessage( kPacket );

		TradeLOG.PrintTimeAndLog( LOG_DEBUG_LEVEL, "[error][Practice]OnPracticeResult - [UserIndex %d] [Index:%d] [m_dwGrade:%d] [m_dwCount:%d] [m_dwTime:%d] [m_dwRank:%d]",
								 pUser->GetUserIndex(), kPractice.m_dwID, kPractice.m_dwGrade, kPractice.m_dwCount, kPractice.m_dwTime, kPractice.m_dwRank);
		pUser->SendPracticeIndexRank(kPractice.m_dwID, kPractice.m_dwGrade, kPractice.m_dwCount, kPractice.m_dwTime, kPractice.m_dwRank);
	}
}

const char* PracticeMode::GetModeINIFileName() const
{
	return "config/mode/practicemode.ini";
}
