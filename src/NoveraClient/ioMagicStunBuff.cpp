
#include "stdafx.h"

#include "ioMagicStunBuff.h"

#include "ioBaseChar.h"


ioMagicStunBuff::ioMagicStunBuff()
{
	m_bEnableKeyState = false;
	m_bSuccess = false;
	m_bAddBuff = false;

	m_dwEnableTime = 0;
}

ioMagicStunBuff::ioMagicStunBuff( const ioMagicStunBuff &rhs )
: ioBuff( rhs ),
m_StunAnimation( rhs.m_StunAnimation ),
m_CatchAnimation( rhs.m_CatchAnimation ),
m_AttackSuccess( rhs.m_AttackSuccess ),
m_DefenseSuccess( rhs.m_DefenseSuccess ),
m_JumpSuccess( rhs.m_JumpSuccess ),
m_AttackFail( rhs.m_AttackFail ),
m_DefenseFail( rhs.m_DefenseFail ),
m_JumpFail( rhs.m_JumpFail ),
m_fSuccessRate( rhs.m_fSuccessRate ),
m_dwDisableTime( rhs.m_dwDisableTime ),
m_DisableEffect( rhs.m_DisableEffect ),
m_EnableEffect( rhs.m_EnableEffect )
{
	m_bEnableKeyState = false;
	m_bSuccess = false;
	m_bAddBuff = false;

	m_dwEnableTime = 0;
}

ioMagicStunBuff::~ioMagicStunBuff()
{
}

void ioMagicStunBuff::LoadProperty( ioINILoader &rkLoader )
{
	ioBuff::LoadProperty( rkLoader );

	char szBuf[MAX_PATH];
	char szKey[MAX_PATH];

	rkLoader.LoadString_e( "stun_animation", "", szBuf, MAX_PATH );
	m_StunAnimation = szBuf;

	rkLoader.LoadString_e( "stun_catch_animation", "", szBuf, MAX_PATH );
	m_CatchAnimation = szBuf;

	m_dwDisableTime = (DWORD)rkLoader.LoadInt_e( "disable_key_duration", 0 );

	rkLoader.LoadString_e( "disable_key_effect", "", szBuf, MAX_PATH );
	m_DisableEffect = szBuf;
	rkLoader.LoadString_e( "enable_key_effect", "", szBuf, MAX_PATH );
	m_EnableEffect = szBuf;

	int iBuffCnt = 0;
	m_AttackSuccess.clear();
	m_DefenseSuccess.clear();
	m_JumpSuccess.clear();
	m_AttackFail.clear();
	m_DefenseFail.clear();
	m_JumpFail.clear();

	// success
	iBuffCnt = rkLoader.LoadInt_e( "attack_success_buff_cnt", 0 );
	if( iBuffCnt > 0 )
	{
		m_AttackSuccess.reserve( iBuffCnt );

		for( int i=0; i < iBuffCnt; ++i )
		{
			wsprintf_e( szKey, "attack_success_buff%d", i+1 );
			rkLoader.LoadString( szKey, "", szBuf, MAX_PATH );
			
			m_AttackSuccess.push_back( szBuf );
		}
	}

	iBuffCnt = rkLoader.LoadInt_e( "defense_success_buff_cnt", 0 );
	if( iBuffCnt > 0 )
	{
		m_DefenseSuccess.reserve( iBuffCnt );

		for( int i=0; i < iBuffCnt; ++i )
		{
			wsprintf_e( szKey, "defense_success_buff%d", i+1 );
			rkLoader.LoadString( szKey, "", szBuf, MAX_PATH );

			m_DefenseSuccess.push_back( szBuf );
		}
	}

	iBuffCnt = rkLoader.LoadInt_e( "jump_success_buff_cnt", 0 );
	if( iBuffCnt > 0 )
	{
		m_JumpSuccess.reserve( iBuffCnt );

		for( int i=0; i < iBuffCnt; ++i )
		{
			wsprintf_e( szKey, "jump_success_buff%d", i+1 );
			rkLoader.LoadString( szKey, "", szBuf, MAX_PATH );

			m_JumpSuccess.push_back( szBuf );
		}
	}

	// fail
	iBuffCnt = rkLoader.LoadInt_e( "attack_fail_buff_cnt", 0 );
	if( iBuffCnt > 0 )
	{
		m_AttackFail.reserve( iBuffCnt );

		for( int i=0; i < iBuffCnt; ++i )
		{
			wsprintf_e( szKey, "attack_fail_buff%d", i+1 );
			rkLoader.LoadString( szKey, "", szBuf, MAX_PATH );

			m_AttackFail.push_back( szBuf );
		}
	}

	iBuffCnt = rkLoader.LoadInt_e( "defense_fail_buff_cnt", 0 );
	if( iBuffCnt > 0 )
	{
		m_DefenseFail.reserve( iBuffCnt );

		for( int i=0; i < iBuffCnt; ++i )
		{
			wsprintf_e( szKey, "defense_fail_buff%d", i+1 );
			rkLoader.LoadString( szKey, "", szBuf, MAX_PATH );

			m_DefenseFail.push_back( szBuf );
		}
	}

	iBuffCnt = rkLoader.LoadInt_e( "jump_fail_buff_cnt", 0 );
	if( iBuffCnt > 0 )
	{
		m_JumpFail.reserve( iBuffCnt );

		for( int i=0; i < iBuffCnt; ++i )
		{
			wsprintf_e( szKey, "jump_fail_buff%d", i+1 );
			rkLoader.LoadString( szKey, "", szBuf, MAX_PATH );

			m_JumpFail.push_back( szBuf );
		}
	}

	m_fSuccessRate = rkLoader.LoadFloat_e( "magic_success_rate", FLOAT05 );
	m_fSuccessRate = max( 0.0f, min(m_fSuccessRate, FLOAT1) );
}

ioBuff* ioMagicStunBuff::Clone()
{
	return new ioMagicStunBuff( *this );
}

void ioMagicStunBuff::StartBuff( ioBaseChar *pOwner )
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
		m_dwBuffStartTime += m_dwDelayDuration;

	m_dwBuffEndTime = m_dwBuffStartTime + GetBuffDuration();
	m_dwCheckTime = dwCurTime;

	m_bLive = true;

	m_bEnableKeyState = false;
	m_dwEnableTime = 0;

	//
	IORandom random;
	random.SetRandomSeed( pOwner->GetRandomSeed() );

	int iRand = random.Random( 100 );
	float fRandRate = (float)iRand / FLOAT100;
	
	m_bSuccess = false;
	if( m_fSuccessRate > fRandRate )
		m_bSuccess = true;

	if( m_iOperationType != OT_DELAY )
	{
		SetAction();
	}
}

bool ioMagicStunBuff::ApplyStartBuff( ioBaseChar *pOwner, DWORD dwGlobalTime, DWORD dwGapTime )
{
	m_pOwner = pOwner;
	DWORD dwGlobalTimeGap = g_GlobalTimer.CalcGlobalGapTime( dwGlobalTime );

	if( m_iOperationType != OT_SWITCH )
	{
		if( dwGlobalTimeGap > dwGapTime )
			return false;

		dwGapTime -= dwGlobalTimeGap;
		m_dwBuffEndTime = FRAMEGETTIME() + dwGapTime;
		m_dwBuffStartTime = m_dwBuffEndTime - GetBuffDuration();
	}

	DWORD dwCurTime = FRAMEGETTIME();
	m_dwCheckTime = dwCurTime;
	m_bLive = true;

	//
	IORandom random;
	random.SetRandomSeed( pOwner->GetRandomSeed() );

	int iRand = random.Random( 100 );
	float fRandRate = (float)iRand / FLOAT100;

	m_bSuccess = false;
	if( m_fSuccessRate > fRandRate )
		m_bSuccess = true;

	if( m_iOperationType != OT_DELAY )
	{
		SetAction();
	}

	return true;
}

void ioMagicStunBuff::ProcessBuff( float fTimePerSec )
{
	ioBaseChar *pOwner = GetOwner();
	if( !pOwner )
	{
		SetReserveEndBuff();
		return;
	}

	if( m_iOperationType == OT_DELAY )
	{
		if( m_bBuffStart )
		{
			if( pOwner->GetState() != CS_MAGIC_STUN )
			{
				SetReserveEndBuff();
				return;
			}
		}
	}
	else
	{
		if( pOwner->GetState() != CS_MAGIC_STUN )
		{
			SetReserveEndBuff();
			return;
		}
	}

	DWORD dwCurTime = FRAMEGETTIME();

	switch( m_iOperationType )
	{
	case OT_SWITCH:
		if( !CheckSwitchType( fTimePerSec ) )
		{
			SetReserveEndBuff();
			return;
		}

		CheckCallingProcess();
		break;
	case OT_DELAY:
		ProcessDelay( fTimePerSec );
		break;
	case OT_ACTIVE:
		ioBuff::ProcessBuff( fTimePerSec );
		break;
	case OT_PASSIVE:
		CheckCallingAreaWeapon();
		break;
	}

	if( !m_bEnableKeyState && m_dwEnableTime < dwCurTime )
	{
		m_bEnableKeyState = true;

		if( !m_EnableEffect.IsEmpty() )
			pOwner->AttachEffect( m_EnableEffect );

		if( !m_DisableEffect.IsEmpty() )
			pOwner->EndEffect( m_DisableEffect, false );
	}

	if( m_bEnableKeyState && pOwner->IsNeedProcess() && CheckSuccessFailBuff() )
	{
		SetReserveEndBuff();
	}
}

bool ioMagicStunBuff::CheckSuccessFailBuff()
{
	ioBaseChar *pOwner = GetOwner();
	if( !pOwner )
		return false;

	ioEntityGroup *pGrp = pOwner->GetGroup();
	if( !pGrp )
		return false;

	DWORD dwCurTime = FRAMEGETTIME();
	bool bKeyPress = false;
	int iRand = dwCurTime % 2;

	ioHashStringVec vSuccessList;
	ioHashStringVec vFailList;
	if( pOwner->IsAttackKey() )
	{
		bKeyPress = true;
		vSuccessList = m_AttackSuccess;

		if( iRand == 0 )
			vFailList = m_DefenseFail;
		else
			vFailList = m_JumpFail;
	}
	else if( pOwner->IsDefenseKey() )
	{
		bKeyPress = true;
		vSuccessList = m_DefenseSuccess;

		if( iRand == 0 )
			vFailList = m_AttackFail;
		else
			vFailList = m_JumpFail;
	}
	else if( pOwner->IsJumpKey() )
	{
		bKeyPress = true;
		vSuccessList = m_JumpSuccess;

		if( iRand == 0 )
			vFailList = m_AttackFail;
		else
			vFailList = m_DefenseFail;
	}

	if( bKeyPress )
	{
		ioHashStringVec vBuffList;
		m_bAddBuff = true;

		if( m_bSuccess && !vSuccessList.empty() )
		{
			int iSize = vSuccessList.size();
			for( int i=0; i < iSize; ++i )
			{
				ioHashString szBuffName = vSuccessList[i];

				if( !pOwner->IsProtectState() )
				{
					pOwner->ReserveAddNewBuff( szBuffName, m_CreateChar, m_ItemName, NULL );
					vBuffList.push_back( szBuffName );
				}
				else if( CheckIgnoreProtectCallingBuff( szBuffName ) )
				{
					pOwner->ReserveAddNewBuff( szBuffName, m_CreateChar, m_ItemName, NULL );
					vBuffList.push_back( szBuffName );
				}
			}
		}
		else if( !m_bSuccess && !vFailList.empty() )
		{
			int iSize = vFailList.size();
			for( int i=0; i < iSize; ++i )
			{
				ioHashString szBuffName = vFailList[i];

				if( !pOwner->IsProtectState() )
				{
					pOwner->ReserveAddNewBuff( szBuffName, m_CreateChar, m_ItemName, NULL );
					vBuffList.push_back( szBuffName );
				}
				else if( CheckIgnoreProtectCallingBuff( szBuffName ) )
				{
					pOwner->ReserveAddNewBuff( szBuffName, m_CreateChar, m_ItemName, NULL );
					vBuffList.push_back( szBuffName );
				}
			}
		}

		int iCurBuffCnt = vBuffList.size();
		if( pOwner->IsNeedSendNetwork() && iCurBuffCnt > 0 )
		{
			if( Help::CheckEnableTimeGapUser() )
			{
				LOG.PrintTimeAndLog( 0, "PC -BuffAddBuff3 : %s, %d", GetName().c_str(), FRAMEGETTIME() );
			}

			SP2Packet kPacket( CUPK_ADD_NEW_BUFF );
			kPacket << pOwner->GetCharName();
			kPacket << m_CreateChar;
			kPacket << "";
			kPacket << pOwner->GetRandomSeed();
			kPacket << false;					// 타격횟수 체크
			kPacket << 0.0f;
			kPacket << 0.0f;
			kPacket << 0.0f;
			kPacket << COWT_NONE;				// Check CloseOrderBuff(12.07.13 수정)
			kPacket << iCurBuffCnt;

			for( int i=0; i < iCurBuffCnt; i++ )
			{
				kPacket << vBuffList[i];
			}

			kPacket << false;					// Use Force

			P2PNetwork::SendToAllPlayingUser( kPacket );
		}

		return true;
	}

	return false;
}

void ioMagicStunBuff::SetTimeOverBuff()
{
	ioBaseChar *pOwner = GetOwner();
	if( !pOwner )
		return;

	ioEntityGroup *pGrp = pOwner->GetGroup();
	if( !pGrp )
		return;

	DWORD dwCurTime = FRAMEGETTIME();
	
	ioHashStringVec vFailList;
	int iRand = dwCurTime % 3;

	switch( iRand )
	{
	case 0:
		vFailList = m_AttackFail;
		break;
	case 1:
		vFailList = m_DefenseFail;
		break;
	case 2:
		vFailList = m_JumpFail;
		break;
	}

	if( !vFailList.empty() )
	{
		int iSize = vFailList.size();
		for( int i=0; i < iSize; ++i )
		{
			ioHashString szBuffName = vFailList[i];

			if( !pOwner->IsProtectState() )
			{
				pOwner->ReserveAddNewBuff( szBuffName, m_CreateChar, m_ItemName, NULL );
			}
			else if( CheckIgnoreProtectCallingBuff( szBuffName ) )
			{
				pOwner->ReserveAddNewBuff( szBuffName, m_CreateChar, m_ItemName, NULL );
			}
		}
	}
}

void ioMagicStunBuff::EndBuff()
{
	ioBaseChar *pOwner = GetOwner();

	if( pOwner )
	{
		if( pOwner->GetState() == CS_MAGIC_STUN )
		{
			pOwner->SetState( CS_DELAY, m_bSendDelayNetWork );
			pOwner->SetDashFailStart();

			if( !m_bAddBuff )
			{
				SetTimeOverBuff();
			}
		}

		if( !m_DisableEffect.IsEmpty() )
			pOwner->EndEffect( m_DisableEffect, false );

		if( !m_EnableEffect.IsEmpty() )
			pOwner->EndEffect( m_EnableEffect, false );
	}

	ioBuff::EndBuff();
}

void ioMagicStunBuff::SetAction()
{
	CheckInvisibleWeapon( false );
	CheckInvisibleCloak( false );
	CheckInvisibleHelmet( false );

	CheckChangeSkill( true );
	CheckChangeColor( true );
	CheckBuffDesc();

	ioBaseChar *pOwner = GetOwner();
	if( !pOwner )
		return;

	DWORD dwCurTime = FRAMEGETTIME();

	if( m_CreatorLookType != CLT_NONE && !m_CreateChar.IsEmpty() )
	{
		if( m_CreateChar != pOwner->GetCharName() )
			pOwner->RotateByBuffCollision( m_CreateChar, m_CreatorLookType );
	}

	if( !m_LoopSound.IsEmpty() )
	{
		m_LoopSoundID = pOwner->PlaySound( m_LoopSound, true );
		m_bLoopSoundPlayed = true;
	}

	if( pOwner->GetState() != CS_DIE &&
		pOwner->GetState() != CS_LOADING &&
		pOwner->GetState() != CS_OBSERVER &&
		pOwner->GetState() != CS_VIEW )
	{
		ioEntityGroup *pGrp = pOwner->GetGroup();
		pGrp->ClearAllActionAni( FLOAT100, true );
		pGrp->ClearAllLoopAni( FLOAT100, true );

		if( !pOwner->IsPrisonerMode() && !pOwner->IsCatchMode() )
			pGrp->SetLoopAni( m_StunAnimation, FLOAT100 );
		else
			pGrp->SetLoopAni( m_CatchAnimation, FLOAT100 );

		pOwner->SetState( CS_MAGIC_STUN );

		if( !m_DisableEffect.IsEmpty() )
			pOwner->AttachEffect( m_DisableEffect );

		m_dwEnableTime = dwCurTime + m_dwDisableTime;
	}

	m_bBuffStart = true;
}

void ioMagicStunBuff::ProcessDelay( float fTimePerSec )
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

	CheckCallingProcess();
}

