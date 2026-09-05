#include "stdafx.h"

#include "ioInfectionBuff.h"

#include "ioPlayStage.h"
#include "ioBaseChar.h"


ioInfectionBuff::ioInfectionBuff()
{
}

ioInfectionBuff::ioInfectionBuff( const ioInfectionBuff &rhs )
: ioBuff( rhs ),
m_bDisableDownState( rhs.m_bDisableDownState ),
m_fInfectionRange( rhs.m_fInfectionRange ),
m_TargetCallBuffList( rhs.m_TargetCallBuffList ),
m_TargetCallExceptBuffList( rhs.m_TargetCallExceptBuffList )
{	
	m_TargetList.clear();
}

ioInfectionBuff::~ioInfectionBuff()
{
}

void ioInfectionBuff::LoadProperty( ioINILoader &rkLoader )
{
	ioBuff::LoadProperty( rkLoader );
	m_bDisableDownState = rkLoader.LoadBool_e( "disable_down_state", false );

	m_fInfectionRange = rkLoader.LoadFloat_e( "infection_range", FLOAT100 );
	
	LoadCallBuffList( rkLoader );	
}

void ioInfectionBuff::LoadCallBuffList( ioINILoader &rkLoader )
{
	m_TargetCallBuffList.clear();
	m_TargetCallExceptBuffList.clear();

	char szBuf[MAX_PATH], szKey[MAX_PATH];

	int iCnt = rkLoader.LoadInt_e( "target_call_buff_cnt", 0 );
	if( iCnt > 0 )
	{
		m_TargetCallBuffList.reserve( iCnt );

		for( int i=0; i<iCnt; ++i )
		{
			CallBuff kCallBuff;

			wsprintf_e( szKey, "target_call_buff%d_name", i+1 );
			rkLoader.LoadString( szKey, "", szBuf, MAX_PATH );
			kCallBuff.m_szBuffName = szBuf;

			wsprintf_e( szKey, "target_call_buff%d_time", i+1 );
			kCallBuff.m_dwBuffCallTime = (DWORD)rkLoader.LoadInt( szKey, 0 );

			m_TargetCallBuffList.push_back( kCallBuff );
		}

		if( !m_TargetCallBuffList.empty() )
			std::sort( m_TargetCallBuffList.begin(), m_TargetCallBuffList.end(), CallBuffSort() );
	}

	int iExceptCnt = rkLoader.LoadInt_e( "target_call_except_buff_cnt", 0 );
	if( iExceptCnt > 0 )
	{
		m_TargetCallExceptBuffList.reserve( iExceptCnt );

		for( int i=0; i < iExceptCnt; ++i )
		{
			wsprintf_e( szKey, "target_call_except_buff%d_name", i+1 );
			rkLoader.LoadString( szKey, "", szBuf, MAX_PATH );

			m_TargetCallExceptBuffList.push_back( szBuf );
		}
	}
}

ioBuff* ioInfectionBuff::Clone()
{
	return new ioInfectionBuff( *this );
}

void ioInfectionBuff::StartBuff( ioBaseChar *pOwner )
{
#ifndef SHIPPING 
	if( Setting::ShowExtraInfo() )
	{
		LOG.PrintTimeAndLog( 0, "BuffCheck - %s, StartTime: %d", m_Name.c_str(), FRAMEGETTIME() );
	}
#endif

	if( !pOwner )
		return;

	DWORD dwCurTime = FRAMEGETTIME();
	m_pOwner = pOwner;
	m_OwnerName = pOwner->GetCharName();

	m_dwBuffStartTime = dwCurTime;
	if( m_iOperationType == OT_DELAY )
	{
		m_dwBuffStartTime += m_dwDelayDuration;
	}

	DWORD dwCurDuration = GetBuffDuration() * pOwner->GetMemberBalanceRateForStateProtectionTime( GetTypeID() );
	m_dwBuffEndTime = m_dwBuffStartTime + dwCurDuration;
	m_dwCheckTime = dwCurTime;

	m_bLive = true;

	if( m_iOperationType != OT_DELAY )
	{
		SetAction();
	}	
}

bool ioInfectionBuff::ApplyStartBuff( ioBaseChar *pOwner, DWORD dwGlobalTime, DWORD dwGapTime )
{
	m_pOwner = pOwner;
	DWORD dwGlobalTimeGap = g_GlobalTimer.CalcGlobalGapTime( dwGlobalTime );
	
	if( m_iOperationType != OT_SWITCH )
	{
		if( dwGlobalTimeGap > dwGapTime )
			return false;

		dwGapTime -= dwGlobalTimeGap;
		m_dwBuffEndTime = FRAMEGETTIME() + dwGapTime;

		DWORD dwCurDuration = GetBuffDuration() * pOwner->GetMemberBalanceRateForStateProtectionTime( GetTypeID() );
		m_dwBuffStartTime = m_dwBuffEndTime - dwCurDuration;
	}

	m_dwCheckTime = FRAMEGETTIME();
	m_bLive = true;

	if( m_iOperationType != OT_DELAY )
	{
		SetAction();
	}

	return true;
}

void ioInfectionBuff::SetExtraDurationByGrowth( float fDuration )
{
	m_dwExtraDurationByGrowth = 0;

	if( m_bDisableGrowth )
		return;

	if( GetBuffDuration() == 0 ) return;
	if( fDuration == 0.0f ) return;

	m_dwExtraDurationByGrowth = (DWORD)fDuration;

	DWORD dwCurDuration = GetBuffDuration() * m_pOwner->GetMemberBalanceRateForStateProtectionTime( GetTypeID() );
	m_dwBuffEndTime = m_dwBuffStartTime + dwCurDuration + m_dwExtraDurationByGrowth;
}

void ioInfectionBuff::SetAction()
{
	CheckInvisibleWeapon( false );
	CheckInvisibleCloak( false );
	CheckInvisibleHelmet( false );

	CheckChangeSkill( true );

	CheckChangeColor( true );
	CheckBuffDesc();

	m_pOwner->AttachEffect( m_BuffEffect, GetEffectEventOwner() );

	if( !m_LoopSound.IsEmpty() )
	{
		m_LoopSoundID = m_pOwner->PlaySound( m_LoopSound, true );
		m_bLoopSoundPlayed = true;
	}

	m_bBuffStart = true;
}

void ioInfectionBuff::EndBuff()
{	
	ioBuff::EndBuff();
}

void ioInfectionBuff::ProcessBuff( float fTimePerSec )
{
	CheckOwnerStateCheck();

	switch( m_iOperationType )
	{
	case OT_SWITCH:
		if( !CheckSwitchType( fTimePerSec ) )
		{
			SetReserveEndBuff();
			return;
		}

		CheckExtendCallingProcess();
		break;
	case OT_DELAY:
		ProcessDelay( fTimePerSec );
		break;
	case OT_ACTIVE:
		ProcessActive( fTimePerSec );
		break;
	case OT_PASSIVE:
		CheckCallingAreaWeapon();
		break;
	}
}

void ioInfectionBuff::ProcessDelay( float fTimePerSec )
{
	DWORD dwCurTime = FRAMEGETTIME();

	if( CheckEnableDelayState() )
		return;

	if( GetBuffDuration() == 0 )
	{
		if( !m_bBuffStart && m_dwBuffStartTime < dwCurTime )
			SetAction();
		return;
	}

	if( !m_bBuffStart && m_dwBuffStartTime < dwCurTime )
		SetAction();

	if( m_dwBuffEndTime <= dwCurTime )
	{		
		SetReserveEndBuff();
		return;
	}

	CheckExtendCallingProcess();
}

void ioInfectionBuff::ProcessActive( float fTimePerSec )
{

	if( CheckEnableDelayState() )
		return;

	CheckExtendCallingProcess();

	if( GetBuffDuration() == 0 )
		return;	

	if( m_dwBuffEndTime <= FRAMEGETTIME() )
	{		
		SetReserveEndBuff();
		return;
	}
}

void ioInfectionBuff::CheckExtendCallingProcess()
{	
	ioPlayStage *pStage = g_ItemMaker.GetPlayStage();
	if( pStage )
	{
		ioBaseChar *pInfectionHost = pStage->GetBaseChar( m_CreateChar );
		ioBaseChar *pInfectioner = pStage->GetBaseChar( m_OwnerName );
		if( !pInfectioner )
			return;

		if( pInfectionHost )
		{
			CheckNonInfectionList( pInfectionHost, pInfectioner, pStage );
			SetTargetInfection( m_CreateChar, m_ItemName );
		}
	}
	
	CheckExceptCallingBuff();

	CheckCallingAttribute();
	CheckCallingAreaWeapon();
}


void ioInfectionBuff::CheckNonInfectionList(ioBaseChar *pInfectionHost, ioBaseChar *pInfectioner, ioPlayStage *pStage )
{
	const BaseCharList &rkTargetList = pStage->GetBaseCharList();
	
	BaseCharList::const_iterator iter;
	m_TargetList.clear();
	
	if( m_TargetCallBuffList.empty() ) return;

	if(pInfectionHost && pInfectionHost->GetExperienceMode() == EMS_EXPERIENCE )
		return;

	int i = 0;
	for( iter = rkTargetList.begin(); iter != rkTargetList.end(); ++iter )
	{
		ioBaseChar *pTarget = (*iter);
		if( !pTarget )
			continue;
		
		if( !pTarget->IsNeedProcess() )
			continue;

		//최초 버프 생성한 캐릭터 대상은 제외
		if( pInfectionHost->GetTeam() == pTarget->GetTeam() )
		{				
			continue;
		}
	
		//시스템 상황에 따라 게임하지 못하는 대상 제외
		if( !pTarget->CheckGrapplingExceptionState() )
			continue;

		if( pTarget->GetState() == CS_SKIP_STUN || pTarget->IsSkipState() || pTarget->IsExcavating() )
			continue;

		if( Help::CheckTargetNoCollision( pTarget->GetState() ) )
			continue;

		if( !pTarget->HasCollisionAvailableBuff() )
			continue;

		//무적은 제외
		if( pTarget->IsProtectState() )
			continue;
		
		//기존 감염 대상자들은 제외
		if( pTarget->HasBuff( BT_INFECTION ) )
			continue;

		//예외처리
		int CallExCnt = 0;
		if( !m_TargetCallExceptBuffList.empty() )
		{
			ioHashStringVec::iterator callex_iter;
			for( callex_iter = m_TargetCallExceptBuffList.begin(); callex_iter != m_TargetCallExceptBuffList.end(); ++callex_iter )
			{
				ioHashString pBuffName = (*callex_iter);
				if( !pBuffName.IsEmpty() )
				{
					if( pTarget->HasBuff( pBuffName ) )
						CallExCnt++;
				}
			}
		}

		if( 0 < CallExCnt )
			continue;

		D3DXVECTOR3 vDiff = pTarget->GetWorldPosition() - pInfectioner->GetWorldPosition();

		float fDistSq = D3DXVec3LengthSq( &vDiff );
		float fRangeSq = m_fInfectionRange*m_fInfectionRange;

		if( fDistSq < fRangeSq )
		{
			m_TargetList.push_back(pTarget);
		}
	}
}

void ioInfectionBuff::SetTargetInfection( ioHashString pInfectionHost, ioHashString pInfectionHostItemName )
{
	
	if( m_TargetCallBuffList.empty() ) return;

	BaseCharList::iterator iter;
	for( iter = m_TargetList.begin(); iter != m_TargetList.end(); ++iter )
	{	
		ioBaseChar *pTarget = (*iter);
		if( !pTarget )
			continue;

		if( !pTarget->IsNeedProcess() )
			continue;

		//감염대상자에게 버프추가
		ioHashStringVec vBuffList;
		CallBuffList::iterator call_iter;
		for( call_iter = m_TargetCallBuffList.begin(); call_iter != m_TargetCallBuffList.end(); ++call_iter )
		{
			CallBuff CallInfo = (*call_iter);
			ioBuff* pTargetBuff = pTarget->AddNewBuff( CallInfo.m_szBuffName,
				pInfectionHost,
				pInfectionHostItemName,
				NULL );
			//pTarget->ReserveAddNewBuff( CallInfo.m_szBuffName, pInfectionHost, pInfectionHostItemName, NULL );

			if( pTargetBuff )
			{
				vBuffList.push_back( CallInfo.m_szBuffName );
			}
		}

		int iCurBuffCnt = vBuffList.size();
		if( pTarget->IsNeedSendNetwork() && iCurBuffCnt > 0 )
		{
			if( Help::CheckEnableTimeGapUser() )
			{
				LOG.PrintTimeAndLog( 0, "PC -BuffAddBuff2 : %s, %d", GetName().c_str(), FRAMEGETTIME() );
			}

			SP2Packet kPacket( CUPK_ADD_NEW_BUFF );
			kPacket << pTarget->GetCharName();
			kPacket << m_CreateChar;
			kPacket << m_ItemName;
			kPacket << pTarget->GetRandomSeed();
			kPacket << false;					// 타격횟수 체크
			kPacket << 0.0f;
			kPacket << 0.0f;
			kPacket << 0.0f;
			kPacket << COWT_NONE;				// Check CloseOrderBuff(12.07.13 수정)
			kPacket << iCurBuffCnt;

			for( int i=0; i < iCurBuffCnt; i++ )
			{				
				kPacket << vBuffList[i];
				//kPacket << m_TargetCallBuffList[i].m_szBuffName;
			}

			kPacket << false;					// Use Force

			P2PNetwork::SendToAllPlayingUser( kPacket );
		}
	}
}

void ioInfectionBuff::CheckOwnerStateCheck()
{
	if( !m_pOwner ) return;

	CharState eState = m_pOwner->GetState();

	if( m_bDisableDownState )
	{	
		if( eState == CS_FROZEN && m_pOwner->GetBlowFrozenState() )
			SetReserveEndBuff();
		else if( eState == CS_BLOW_WOUNDED && !m_pOwner->IsFloatingState() )
			SetReserveEndBuff();
		else if( eState == CS_BOUND_BLOW_WOUNDED && !m_pOwner->IsFloatingState() )
			SetReserveEndBuff();
		else if( eState == CS_ETC_STATE && m_pOwner->IsApplyDownState(false) )
			SetReserveEndBuff();
		else if( eState == CS_STOP_MOTION && m_pOwner->GetBlowStopMotionState() )
			SetReserveEndBuff();
		else if( eState == CS_DROP_ZONE_DOWN )
			SetReserveEndBuff();
		else if( eState == CS_ICE_STATE && m_pOwner->GetBlowStopMotionState() )
			SetReserveEndBuff();
	}
}
