
#include "stdafx.h"

#include "ioAbuseManager.h"

template<> ioAbuseManager* Singleton< ioAbuseManager >::ms_Singleton = 0;

ioAbuseManager::ioAbuseManager()
{
	m_dwKeyInputTimer = 0;
	m_dwMaxKeyInputTime = 0;
	m_dwOpenSpaceMaxKeyInputTime = 0;

	m_dwDamageTimer = 0;
	m_dwMaxDamageTime =0;

	m_dwCurAbuseTimer = 0;
	m_bAbuseSend = false;
}

ioAbuseManager::~ioAbuseManager()
{	
}

ioAbuseManager& ioAbuseManager::GetSingleton()
{
	return Singleton< ioAbuseManager >::GetSingleton();
}

void ioAbuseManager::LoadINIInfo( ioINILoader &rkLoader )
{
	rkLoader.SetTitle_e( "Abuse" );
	m_dwMaxKeyInputTime = rkLoader.LoadInt_e( "key_input_time", 180000 );
	m_dwOpenSpaceMaxKeyInputTime = rkLoader.LoadInt_e( "openspace_key_input_time", 600000 );
	
	m_dwMaxDamageTime   = rkLoader.LoadInt_e( "damage_time", 180000 ); 
}

void ioAbuseManager::Initialize()
{
	m_dwCurAbuseTimer = REALGETTIME();
	m_dwKeyInputTimer = 0;
	m_dwDamageTimer   = 0;
	m_bAbuseSend      = false;
}

void ioAbuseManager::CheckInputKey( DWORD dwDirKey, DWORD dwActionKey )
{
	if( dwDirKey == ioUserKeyInput::DKI_NONE && dwActionKey == ioUserKeyInput::AKI_NONE )
		return;     // 아무 키도 눌리지 않았다.

	m_dwKeyInputTimer = 0;
}

void ioAbuseManager::AddDamage( int iDamage )
{
	m_dwDamageTimer = 0;
}

void ioAbuseManager::ProcessLobbyStage()
{
}

void ioAbuseManager::ProcessPlayStage( ioPlayStage *pPlayStage )
{
	if( !m_bAbuseSend && pPlayStage )
	{
		if( !IsAbuseNoCheck( pPlayStage ) )
		{
			CheckAbuseKey( pPlayStage );

			if( pPlayStage->GetModeType() != MT_TRAINING && pPlayStage->GetModeType() != MT_HEADQUARTERS && pPlayStage->GetModeType() != MT_HOUSE )
			{
				CheckAbuseDamage( pPlayStage );
			}
		}
	}

	m_dwCurAbuseTimer = REALGETTIME();	
}

bool ioAbuseManager::IsAbuseNoCheck( ioPlayStage *pPlayStage )
{
#ifndef SHIPPING
	return true;
#endif

	if( pPlayStage->GetModeType() == MT_NONE ||
		pPlayStage->GetModeType() == MT_MYROOM )
		return true;

	if( g_GUIMgr.IsShow( ROUND_RESULT_MAIN_WND ) ||
		g_GUIMgr.IsShow( BRIEFING_WND ) ||
		pPlayStage->IsRoundSetEnd() )
		return true;

	ioPlayMode *pPlayMode = pPlayStage->GetPlayMode();
	if( pPlayMode == NULL )
		return true;
	if( pPlayMode->GetModeState() != ioPlayMode::MS_PLAY )
		return true;

	if( pPlayMode->GetModeType() == MT_BATTLE )
	{
		if( !g_BattleRoomMgr.CheckUserBattleFightFlag( g_MyInfo.GetPublicID() ) )
			return true;
	}

	return false;
}

void ioAbuseManager::CheckAbuseKey( ioPlayStage *pPlayStage )
{
	if( pPlayStage->GetModeType() == MT_HEADQUARTERS )
		return;

	ioBaseChar *pOwnerChar = pPlayStage->GetOwnerChar();
	if( pOwnerChar )      //예외 처리
	{
		if( pOwnerChar->GetState() == CS_VIEW ||
			pOwnerChar->GetState() == CS_LOADING ||
			pOwnerChar->GetState() == CS_DIE  ||
			pOwnerChar->GetState() == CS_READY||
			pOwnerChar->GetState() == CS_OBSERVER ||
			pOwnerChar->IsPrisonerMode() )
			return;
	}

	// 키 체크
	m_dwKeyInputTimer += REALGETTIME() - m_dwCurAbuseTimer;

	DWORD dwMaxInputTime = m_dwMaxKeyInputTime;
	if( pPlayStage->GetModeType() == MT_TRAINING || pPlayStage->GetModeType() == MT_HOUSE )
		dwMaxInputTime = m_dwOpenSpaceMaxKeyInputTime;

	if( m_dwKeyInputTimer > dwMaxInputTime )
	{
		if( dwMaxInputTime != 0 )
		{
			if( g_App.IsMouseBusy() )
				return;

			// 어뷰즈
			if( P2PNetwork::IsNetworkPlaying() )
			{
				SP2Packet kPacket( CTPK_ABUSE_QUIZ_START );
				kPacket << AT_KEY;
				TCPNetwork::SendToServer( kPacket );

				m_bAbuseSend = true;
			}			
		}
	}
}

void ioAbuseManager::CheckAbuseDamage( ioPlayStage *pPlayStage )
{
	ioBaseChar *pOwnerChar = pPlayStage->GetOwnerChar();
	if( pOwnerChar )     //예외 처리
	{
		if( pOwnerChar->GetState() == CS_VIEW ||
			pOwnerChar->GetState() == CS_LOADING ||
			pOwnerChar->GetState() == CS_DIE  ||
			pOwnerChar->GetState() == CS_READY||
			pOwnerChar->GetState() == CS_OBSERVER ||
			pOwnerChar->IsPrisonerMode() )
			return;
	}

	//데미지 체크
	m_dwDamageTimer += REALGETTIME() - m_dwCurAbuseTimer;
	if( m_dwDamageTimer > m_dwMaxDamageTime )
	{
		if( m_dwMaxDamageTime != 0 )
		{
			if( g_App.IsMouseBusy() )
				return;

			// 어뷰즈
			if( P2PNetwork::IsNetworkPlaying() )
			{
				SP2Packet kPacket( CTPK_ABUSE_QUIZ_START );
				kPacket << AT_DAMAGE;
				TCPNetwork::SendToServer( kPacket );

				m_bAbuseSend = true;
			}
		}
	}
}