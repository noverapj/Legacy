

#include "stdafx.h"

#include "ioEtcBuff.h"
#include "WeaponDefine.h"
#include "BuffDefine.h"
#include "ioGrowthUpInfo.h"

// CatchBuff
ioCatchBuff::ioCatchBuff()
{
	m_dwExtraRopeDuration = 0;
}

ioCatchBuff::ioCatchBuff( const ioCatchBuff &rhs )
: ioBuff( rhs ),
 m_CatchEffect( rhs.m_CatchEffect ),
 m_CatchStateEffect( rhs.m_CatchStateEffect ),
 m_CatchEscapeEffect( rhs.m_CatchEscapeEffect ),
 m_CatchRope( rhs.m_CatchRope )
{
	m_dwExtraRopeDuration = 0;
}

ioCatchBuff::~ioCatchBuff()
{
}

ioBuff* ioCatchBuff::Clone()
{
	return new ioCatchBuff( *this );
}

void ioCatchBuff::LoadProperty( ioINILoader &rkLoader )
{
	ioBuff::LoadProperty( rkLoader );

	char szBuff[MAX_PATH];
	rkLoader.LoadString_e( "catch_tie_effect", "", szBuff, MAX_PATH );
	m_CatchEffect = szBuff;

	rkLoader.LoadString_e( "catch_state_effect", "", szBuff, MAX_PATH );
	m_CatchStateEffect = szBuff;

	rkLoader.LoadString_e( "catch_escape_effect", "", szBuff, MAX_PATH );
	m_CatchEscapeEffect = szBuff;

	rkLoader.LoadString_e( "catch_rope", "", szBuff, MAX_PATH );
	m_CatchRope = szBuff;
}

void ioCatchBuff::StartBuff( ioBaseChar *pOwner )
{
	ioBuff::StartBuff( pOwner );

	if( !m_ItemName.IsEmpty() )
	{
		ioGrowthUpInfo *pUpInfo = GetGrowthUpInfoForItem(GT_ROPE_DURATION);
		ioGrowthNormalUpInfo *pNormalUpInfo = ToNormalUpInfo(pUpInfo);
		if( pNormalUpInfo )
		{
			ioPlayStage *pStage = g_ItemMaker.GetPlayStage();
			m_dwExtraRopeDuration = (DWORD)pNormalUpInfo->GetValue(pStage);
		}
	}

	pOwner->SetCatchState( GetBuffDuration()+m_dwExtraRopeDuration,
						   m_CatchEffect,
						   m_CatchStateEffect,
						   m_CatchEscapeEffect,
						   m_CatchRope, 
						   m_CreateChar );
	SetReserveEndBuff();
}

bool ioCatchBuff::ApplyStartBuff( ioBaseChar *pOwner, DWORD dwGlobalTime, DWORD dwGapTime )
{
	if( !ioBuff::ApplyStartBuff( pOwner, dwGlobalTime, dwGapTime ) )
		return false;

	pOwner->ApplyCatchState( GetBuffDuration()+m_dwExtraRopeDuration,
							 m_CatchEffect,
							 m_CatchStateEffect,
							 m_CatchEscapeEffect,
							 m_CatchRope,
							 m_CreateChar );
	SetReserveEndBuff();
	return true;
}

void ioCatchBuff::ProcessBuff( float fTimePerSec )
{
	if( m_pOwner && !m_pOwner->IsCatchMode() )
	{
		SetReserveEndBuff();
		return;
	}

	switch( m_iOperationType )
	{
	case OT_DELAY:
	case OT_SWITCH:
	case OT_ACTIVE:
	case OT_PASSIVE:
		ioBuff::ProcessBuff( fTimePerSec );
		break;
	}
}

void ioCatchBuff::EndBuff()
{
	ioBuff::EndBuff();
}

void ioCatchBuff::GetBuffInfo( SP2Packet &rkPacket )
{
	ioBuff::GetBuffInfo( rkPacket );

	rkPacket << m_dwExtraRopeDuration;
}

void ioCatchBuff::ApplyBuffInfo( SP2Packet &rkPacket )
{
	rkPacket >> m_dwExtraRopeDuration;
}
//////////////////////////////////////////////////////////////////////////////////////////////////////
// GaugeClearBuff
ioGaugeClearBuff::ioGaugeClearBuff()
{
}

ioGaugeClearBuff::ioGaugeClearBuff( const ioGaugeClearBuff &rhs )
: ioBuff( rhs ),
 m_GaugeClearType( rhs.m_GaugeClearType ),
 m_eGaugeClearSlot( rhs.m_eGaugeClearSlot ),
 m_fClearActiveGaugePercent( rhs.m_fClearActiveGaugePercent ),
 m_fClearPassiveGaugePercent( rhs.m_fClearPassiveGaugePercent ),
 m_ClearBuffEffect( rhs.m_ClearBuffEffect )
{
}

ioGaugeClearBuff::~ioGaugeClearBuff()
{
}

ioBuff* ioGaugeClearBuff::Clone()
{
	return new ioGaugeClearBuff( *this );
}

void ioGaugeClearBuff::LoadProperty( ioINILoader &rkLoader )
{
	ioBuff::LoadProperty( rkLoader );

	char szBuff[MAX_PATH];

	m_GaugeClearType			= (GaugeClearType)rkLoader.LoadInt_e( "gauge_clear_type", GCT_NORMAL );
	m_eGaugeClearSlot			= (EquipSlot)rkLoader.LoadInt_e( "gauge_clear_slot", ES_UNKNOWN );
	m_fClearActiveGaugePercent	= rkLoader.LoadFloat_e( "active_gauge_clear_percent", 0.0f );
	m_fClearPassiveGaugePercent	= rkLoader.LoadFloat_e( "passive_gauge_clear_percent", 0.0f );

	rkLoader.LoadString_e( "clear_effect", "", szBuff, MAX_PATH );
	m_ClearBuffEffect = szBuff;
}

void ioGaugeClearBuff::StartBuff( ioBaseChar *pOwner )
{
	ioBuff::StartBuff( pOwner );
	
	if( m_GaugeClearType == GCT_PERCENT )
		pOwner->ClearEquipItemGaugeByPercent( m_eGaugeClearSlot, m_fClearActiveGaugePercent, m_fClearPassiveGaugePercent );
	else
		pOwner->ClearEquipItemGauge( m_GaugeClearType );

	pOwner->AttachEffect( m_ClearBuffEffect, NULL );

	SetBuffDuration( 0 );
}

bool ioGaugeClearBuff::ApplyStartBuff( ioBaseChar *pOwner, DWORD dwGlobalTime, DWORD dwGapTime )
{
	if( !ioBuff::ApplyStartBuff( pOwner, dwGlobalTime, dwGapTime ) )
		return false;

	if( m_GaugeClearType == GCT_PERCENT )
		pOwner->ClearEquipItemGaugeByPercent( m_eGaugeClearSlot, m_fClearActiveGaugePercent, m_fClearPassiveGaugePercent );
	else
		pOwner->ClearEquipItemGauge( m_GaugeClearType );

	pOwner->AttachEffect( m_ClearBuffEffect, NULL );

	SetBuffDuration( 0 );
	return true;
}

void ioGaugeClearBuff::ProcessBuff( float fTimePerSec )
{
	switch( m_iOperationType )
	{
	case OT_DELAY:
	case OT_SWITCH:
	case OT_ACTIVE:
	case OT_PASSIVE:
		SetReserveEndBuff();
		break;
	}
}
//////////////////////////////////////////////////////////////////////////////////////////////////////
// FearBuff
ioFearBuff::ioFearBuff()
{
}

ioFearBuff::ioFearBuff( const ioFearBuff &rhs )
: ioBuff( rhs ),
 m_bMovingDrop( rhs.m_bMovingDrop ),
 m_fMovingDropRate( rhs.m_fMovingDropRate ),
 m_fMovingSpeed( rhs.m_fMovingSpeed ),
 m_fMovingOffSet( rhs.m_fMovingOffSet ),
 m_MovingSound( rhs.m_MovingSound ),
 m_bOnlyArmorDrop( rhs.m_bOnlyArmorDrop ),
 m_fMovingDropNormalRate( rhs.m_fMovingDropNormalRate )
{
}

ioFearBuff::~ioFearBuff()
{
}

void ioFearBuff::LoadProperty( ioINILoader &rkLoader )
{
	ioBuff::LoadProperty( rkLoader );

	char szBuf[MAX_PATH] = "";
	m_bMovingDrop = rkLoader.LoadBool_e( "moving_drop", false );
	m_fMovingDropRate = rkLoader.LoadFloat_e( "moving_drop_rate", 50.0f );
	m_fMovingDropNormalRate = rkLoader.LoadFloat_e( "moving_drop_normal_rate", 0.0f );

	m_fMovingSpeed = rkLoader.LoadFloat_e( "moving_speed", 0.0f );
	m_fMovingOffSet = rkLoader.LoadFloat_e( "moving_offset", 0.0f );

	rkLoader.LoadString_e( "moving_sound", "", szBuf, MAX_PATH );
	m_MovingSound = szBuf;

	m_bOnlyArmorDrop = rkLoader.LoadBool_e( "only_armor_drop", false );
}

ioBuff* ioFearBuff::Clone()
{
	return new ioFearBuff( *this );
}

void ioFearBuff::StartBuff( ioBaseChar *pOwner )
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

	if( m_iOperationType != OT_DELAY )
	{
		SetAction();
	}
}

bool ioFearBuff::ApplyStartBuff( ioBaseChar *pOwner, DWORD dwGlobalTime, DWORD dwGapTime )
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

	m_dwCheckTime = FRAMEGETTIME();
	m_bLive = true;

	if( m_iOperationType != OT_DELAY )
	{
		SetAction();
	}

	return true;
}

void ioFearBuff::ProcessBuff( float fTimePerSec )
{
	switch( m_iOperationType )
	{
	case OT_ACTIVE:
	case OT_PASSIVE:
	case OT_SWITCH:
		SetReserveEndBuff();
		break;
	case OT_DELAY:
		ProcessDelay( fTimePerSec );
		break;
	}
}

void ioFearBuff::EndBuff()
{
	ioBuff::EndBuff();
}

void ioFearBuff::SetAction()
{
	ioBaseChar *pOwner = GetOwner();
	if( !pOwner )
	{
		SetReserveEndBuff();
		return;
	}

	CheckInvisibleWeapon( false );
	CheckInvisibleCloak( false );
	CheckInvisibleHelmet( false );
	CheckChangeColor( true );

	CheckChangeSkill( true );

	CheckBuffDesc();

	if( m_CreatorLookType != CLT_NONE && !m_CreateChar.IsEmpty() )
	{
		if( m_CreateChar != pOwner->GetCharName() )
			pOwner->RotateByBuffCollision( m_CreateChar, m_CreatorLookType );
	}

	pOwner->AttachEffect( m_BuffEffect, GetEffectEventOwner() );

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
		if( pOwner->IsHasCrownByAllMode() )
		{
			pOwner->CheckWearItemDrop();
		}
		else
		{
			IORandom eRandom;
			
			bool bEnableDrop = true;
			if( m_bMovingDrop )
			{
				eRandom.SetRandomSeed( timeGetTime() );

				int iRand = eRandom.Random( 100 );
				if( iRand >= m_fMovingDropRate )
					bEnableDrop = false;
			}

			int iSlot = -1;
			int iRate = (int)(pOwner->GetMemberBalanceRateForBuffItemDrop() * FLOAT100);

			if( m_bOnlyArmorDrop )
			{
				iSlot = pOwner->CheckArmorItemDrop();
			}
			else if( bEnableDrop && iRate >= 100 )
			{
				if ( m_fMovingDropNormalRate > 0.0f )
				{
					eRandom.SetRandomSeed( timeGetTime() );

					int iRand = eRandom.Random( 100 );
					if( iRand < m_fMovingDropNormalRate )
						iSlot = pOwner->CheckItemDrop( m_bMovingDrop, true );
				}
				else
					iSlot = pOwner->CheckItemDrop( m_bMovingDrop, true );
			}
			else if( bEnableDrop )
			{
				eRandom.SetRandomSeed( timeGetTime()+1 );

				int iRand = eRandom.Random( 100 );
				if( iRand <= iRate )
				{
					iSlot = pOwner->CheckItemDrop( m_bMovingDrop, true );
				}
			}

			if( (m_bOnlyArmorDrop && iSlot != -1) || (m_bMovingDrop && iSlot != -1) )	// 벗겨진후 옮겨지는 타입
			{
				pOwner->ItemMoveDrop( (EquipSlot)iSlot, m_CreateChar, m_SkillName, m_fMovingOffSet, m_fMovingSpeed );

				if( !m_MovingSound.IsEmpty() )
					pOwner->PlaySoundA( m_MovingSound );
			}
		}
	}

	m_bBuffStart = true;
}

void ioFearBuff::ProcessDelay( float fTimePerSec )
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

	if( m_bBuffStart )
	{
		SetReserveEndBuff();
		return;
	}
}
//////////////////////////////////////////////////////////////////////////////////////////////////////
// JumpBuff
ioDoubleJumpBuff::ioDoubleJumpBuff()
{
}

ioDoubleJumpBuff::ioDoubleJumpBuff( const ioDoubleJumpBuff &rhs )
: ioBuff( rhs ),
 m_fAniRate( rhs.m_fAniRate ),
 m_fJumpPower( rhs.m_fJumpPower ),
 m_fJumpForce( rhs.m_fJumpForce ),
 m_fJumpSpeedRate( rhs.m_fJumpSpeedRate ),
 m_fEnableHeight( rhs.m_fEnableHeight ),
 m_dwJumpEnableTime( rhs.m_dwJumpEnableTime ),
 m_iMaxJumpCnt( rhs.m_iMaxJumpCnt ),
 m_bJumpAttack( rhs.m_bJumpAttack ),
 m_bEnableRotate( rhs.m_bEnableRotate ),
 m_fGravityRate( rhs.m_fGravityRate ),
 m_fLandingRate( rhs.m_fLandingRate ),
 m_pJumpTypeLevelList( rhs.m_pJumpTypeLevelList ),
 m_pExcpetMercenaryList( rhs.m_pExcpetMercenaryList ),
 m_vBuffOnJumping( rhs.m_vBuffOnJumping ),
 m_fBuffOnJumpingEnableGauge( rhs.m_fBuffOnJumpingEnableGauge ),
 m_bInvisibleWeaponMeshOnJump( rhs.m_bInvisibleWeaponMeshOnJump ),
 m_fJumpMaxHeight( rhs.m_fJumpMaxHeight),
 m_fJumpEnableGauge( rhs.m_fJumpEnableGauge ),
 m_fGaugePerTickByDropZone( rhs.m_fGaugePerTickByDropZone ),
 m_fBuffAirWalkStartGauge( rhs.m_fBuffAirWalkStartGauge )
{
	m_vJumpAnimationList.clear();
	int iCnt = rhs.m_vJumpAnimationList.size();
	for( int i=0; i < iCnt; i++ )
		m_vJumpAnimationList.push_back( rhs.m_vJumpAnimationList[i] );
}

ioDoubleJumpBuff::~ioDoubleJumpBuff()
{
}

ioBuff* ioDoubleJumpBuff::Clone()
{
	return new ioDoubleJumpBuff( *this );
}

void ioDoubleJumpBuff::LoadProperty( ioINILoader &rkLoader )
{
	ioBuff::LoadProperty( rkLoader );

	char szBuf[MAX_PATH];
	char szKey[MAX_PATH];

	m_bEnableRotate = rkLoader.LoadBool_e( "enable_rotate", false );

	m_fAniRate = rkLoader.LoadFloat_e( "jump_ani_rate", FLOAT1 );
	m_fJumpPower = rkLoader.LoadFloat_e( "jump_power", 0.0f );
	m_fJumpForce = rkLoader.LoadFloat_e( "jump_force", 0.0f );
	m_fJumpSpeedRate = rkLoader.LoadFloat_e( "jump_speed_rate", FLOAT1 );
	m_fGravityRate = rkLoader.LoadFloat_e( "jump_gravity_rate", FLOAT1 );

	m_fLandingRate = rkLoader.LoadFloat_e( "jump_landing_rate", FLOAT1 );

	m_fEnableHeight = rkLoader.LoadFloat_e( "jump_enable_height", 0.0f );
	m_dwJumpEnableTime = rkLoader.LoadInt_e( "jump_enable_time", 0 );

	m_iMaxJumpCnt = rkLoader.LoadInt_e( "jump_max_cnt", 2 );

	m_bJumpAttack = rkLoader.LoadBool_e( "jump_attack_enable", false );

	m_vJumpAnimationList.clear();
	m_vJumpAnimationList.reserve( 4 );

	rkLoader.LoadString_e( "jump_animation_front", "", szBuf, MAX_PATH );
	m_vJumpAnimationList.push_back( ioHashString(szBuf) );
	rkLoader.LoadString_e( "jump_animation_right", "", szBuf, MAX_PATH );
	m_vJumpAnimationList.push_back( ioHashString(szBuf) );
	rkLoader.LoadString_e( "jump_animation_back", "", szBuf, MAX_PATH );
	m_vJumpAnimationList.push_back( ioHashString(szBuf) );
	rkLoader.LoadString_e( "jump_animation_left", "", szBuf, MAX_PATH );
	m_vJumpAnimationList.push_back( ioHashString(szBuf) );

	int iJumpTypeCnt = rkLoader.LoadInt_e( "jump_type_count", 0 );

	m_pJumpTypeLevelList.clear();
	m_pJumpTypeLevelList.reserve(iJumpTypeCnt);

	for( int i = 0; i < iJumpTypeCnt; ++i )
	{
		wsprintf_e( szKey, "jump_type%d_reduce_level", i+1 );
		DWORD dwLevel = rkLoader.LoadInt( szKey, 1 );

		JumpTypeInfo Info;
		Info.m_eJumpType = (JumpType)i;
		Info.m_dwJumpLevel = dwLevel;
				
		m_pJumpTypeLevelList.push_back( Info );
	}

	int iExceptMercenaryCnt = rkLoader.LoadInt_e( "except_mercenary_count", 0 );

	m_pExcpetMercenaryList.clear();
	m_pExcpetMercenaryList.reserve( iExceptMercenaryCnt );

	for( int i = 0; i < iJumpTypeCnt; ++i )
	{
		ExcpetMercenaryInfo Info;
		wsprintf_e( szKey, "except_mercenary%d_weapon_item_code", i+1 );
		Info.m_dwMercenaryWeaponType = rkLoader.LoadInt( szKey, 1 );

		wsprintf_e( szKey, "except_mercenary%d_jump_level", i+1 );
		Info.m_dwJumpLevel = rkLoader.LoadInt( szKey, 1 );
				
		m_pExcpetMercenaryList.push_back( Info );
	}

	int iBuffCnt = rkLoader.LoadInt_e( "buff_on_jumping_cnt", 0 );
	for( int i=0; i<iBuffCnt; ++i )
	{
		wsprintf_e( szKey, "buff_on_jumping%d", i+1 );
		rkLoader.LoadString( szKey, "", szBuf, MAX_PATH );
		ioHashString szBuffName = szBuf;
		m_vBuffOnJumping.push_back( szBuffName );
	}

	m_fBuffOnJumpingEnableGauge = rkLoader.LoadFloat_e( "buff_on_jumping_enable_gauge", 0.0f );
	m_bInvisibleWeaponMeshOnJump = rkLoader.LoadBool_e( "invisible_weapon_mesh_on_jump", false );

	m_fJumpMaxHeight = rkLoader.LoadFloat_e( "jump_max_height", 0.0f );
	m_fJumpEnableGauge = rkLoader.LoadFloat_e( "jump_enable_gauge", 0.0f );
	m_fGaugePerTickByDropZone = rkLoader.LoadFloat_e( "gauge_per_tick_by_drop_zone", 0.0f );

	m_fBuffAirWalkStartGauge = rkLoader.LoadFloat_e( "buff_walk_start_gauge", 0.0f );
}

void ioDoubleJumpBuff::StartBuff( ioBaseChar *pOwner )
{
	ioBuff::StartBuff( pOwner );

	CharBuffJumpInfo kInfo;
	kInfo.m_BuffJumpType = BJT_MULTI;
	kInfo.m_Name = GetName();
	kInfo.m_bEnableRotate = m_bEnableRotate;
	kInfo.m_fBuffJumpPower = m_fJumpPower;
	kInfo.m_fBuffJumpForce = m_fJumpForce;
	kInfo.m_fBuffJumpSpeedRate = m_fJumpSpeedRate;
	kInfo.m_fBuffJumpGravityRate = m_fGravityRate;
	kInfo.m_fBuffJumpLandingRate = m_fLandingRate;

	kInfo.m_fBuffJumpHeight = m_fEnableHeight;
	kInfo.m_iBuffJumpMaxCnt = max( 2, m_iMaxJumpCnt);

	kInfo.m_dwBuffJumpEnableTime = m_dwJumpEnableTime;

	kInfo.m_fBuffJumpAniRate = m_fAniRate;
	kInfo.m_vBuffJumpAnimationList = m_vJumpAnimationList;

	kInfo.m_fBuffJumpMaxHeight = m_fJumpMaxHeight;
	kInfo.m_fBuffJumpEnableGauge = m_fJumpEnableGauge;
	kInfo.m_fBuffAirWalkStartGauge = m_fBuffAirWalkStartGauge;

	pOwner->SetBuffJumpValue( kInfo );

	if( !m_vBuffOnJumping.empty() )
	{
		pOwner->SetJumpBuffOnDoubleJumpBuffName( m_vBuffOnJumping );
	}

	m_iRandomSeed = 0;
	m_iOwnerRandomSeed = 0;
	m_bHideMesh = false;
}

bool ioDoubleJumpBuff::ApplyStartBuff( ioBaseChar *pOwner, DWORD dwGlobalTime, DWORD dwGapTime )
{
	if( !ioBuff::ApplyStartBuff( pOwner, dwGlobalTime, dwGapTime ) )
		return false;

	CharBuffJumpInfo kInfo;
	kInfo.m_BuffJumpType = BJT_MULTI;
	kInfo.m_Name = GetName();
	kInfo.m_bEnableRotate = m_bEnableRotate;
	kInfo.m_fBuffJumpPower = m_fJumpPower;
	kInfo.m_fBuffJumpForce = m_fJumpForce;
	kInfo.m_fBuffJumpSpeedRate = m_fJumpSpeedRate;
	kInfo.m_fBuffJumpGravityRate = m_fGravityRate;
	kInfo.m_fBuffJumpLandingRate = m_fLandingRate;

	kInfo.m_fBuffJumpHeight = m_fEnableHeight;
	kInfo.m_iBuffJumpMaxCnt = max( 2, m_iMaxJumpCnt);

	kInfo.m_dwBuffJumpEnableTime = m_dwJumpEnableTime;

	kInfo.m_fBuffJumpAniRate = m_fAniRate;
	kInfo.m_vBuffJumpAnimationList = m_vJumpAnimationList;

	kInfo.m_fBuffJumpMaxHeight = m_fJumpMaxHeight;
	kInfo.m_fBuffJumpEnableGauge = m_fJumpEnableGauge;

	pOwner->SetBuffJumpValue( kInfo );

	if( !m_vBuffOnJumping.empty() )
	{
		pOwner->SetJumpBuffOnDoubleJumpBuffName( m_vBuffOnJumping );
	}

	m_bHideMesh = false;
	return true;
}

void ioDoubleJumpBuff::ProcessBuff( float fTimePerSec )
{
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
	case OT_ACTIVE:
		ioBuff::ProcessBuff( fTimePerSec );
		break;
	case OT_PASSIVE:
		CheckCallingAreaWeapon();
		ProcessPassive();
		break;
	}
}

bool ioDoubleJumpBuff::IsCanJumpAttack() const
{
	return m_bJumpAttack;
}

void ioDoubleJumpBuff::GetBuffInfo( SP2Packet &rkPacket )
{
	ioBuff::GetBuffInfo( rkPacket );
	rkPacket << m_iOwnerRandomSeed;
	rkPacket << m_iRandomSeed;
}

void ioDoubleJumpBuff::ApplyBuffInfo( SP2Packet &rkPacket )
{
	rkPacket >> m_iOwnerRandomSeed;
	rkPacket >> m_iRandomSeed;
}

void ioDoubleJumpBuff::EndBuff()
{
	ioBaseChar *pOwner = GetOwner();
	if( pOwner )
	{
		pOwner->RemoveBuffJumpValue( GetName() );
		pOwner->SetJumpBuffOnDoubleJumpBuffEnd();
	}
	
	ioBuff::EndBuff();
}

DWORD ioDoubleJumpBuff::CheckJumpTypeLevel( DWORD dwWeaponItemCode, JumpType eCheckType )
{
	if( m_pJumpTypeLevelList.empty() )
		return 1;

	JumpTypeLevelList::iterator iter;
	ExcpetMercenaryList::iterator mercenary_iter;
	for( iter = m_pJumpTypeLevelList.begin(); iter != m_pJumpTypeLevelList.end(); ++iter )
	{
		JumpTypeInfo Info = (*iter);
		if( Info.m_eJumpType == eCheckType )
		{			
			if( !m_pExcpetMercenaryList.empty() )
			{
				for( mercenary_iter = m_pExcpetMercenaryList.begin(); 
					 mercenary_iter != m_pExcpetMercenaryList.end();
					 ++mercenary_iter )
				{			
					ExcpetMercenaryInfo Info = (*mercenary_iter);

					if( Info.m_dwMercenaryWeaponType == dwWeaponItemCode )
						return Info.m_dwJumpLevel;					
				}
			}	
			return Info.m_dwJumpLevel;
		}
	}
	
	return 1;
}

void ioDoubleJumpBuff::ProcessPassive()
{
	if( m_fGaugePerTick <= 0.0f || m_dwTickTime <= 0 )
		return;

	if( GetName() != m_pOwner->GetBuffJumpName() )
		return;
	
	float fGaugePerTick = m_fGaugePerTick;
	if ( m_fGaugePerTickByDropZone > 0.0f && m_pOwner->GetBottomHeight() == 0.0f )
		fGaugePerTick = m_fGaugePerTickByDropZone;

	float fCurTicGauge = fGaugePerTick / m_fTicGaugeRateByGrowth;
	DWORD dwCurTime = FRAMEGETTIME();
	int iTickGap = (int)( dwCurTime - m_dwCheckTime );
	int iCurTicTime = m_dwTickTime + m_dwTicTimeByGrowth;

	// 더블 점프중에만 스킬 게이지가 감소한다.
	if( iTickGap >= iCurTicTime )
	{
		m_dwCheckTime = dwCurTime - ( iTickGap - iCurTicTime );
		if( fCurTicGauge != 0.0f )
		{
			if( (m_pOwner->GetState() == CS_JUMP && m_pOwner->GetJumpState() == JS_JUMPPING) &&
				m_pOwner->GetUsedBuffJump() == BJUT_CUR )
			{
				for( int i=ES_WEAPON; i < MAX_EQUIP_SLOT; ++i )
				{
					ioSkill *pSkill = m_pOwner->GetEquipedSkill(i);
					ioPassiveSkill *pPassiveSkill = ToPassiveSkill(pSkill);

					if( !pPassiveSkill )
						continue;

					if( GetSkillName().IsEmpty() ||  pPassiveSkill->GetName() != GetSkillName() )
						continue;

					if( pPassiveSkill->UsePassiveGauge() )
					{
						pPassiveSkill->ReduceCurPassiveGauge( fCurTicGauge );

						if( m_bInvisibleWeaponMeshOnJump && !m_bHideMesh )
						{
							m_pOwner->ShowEquipItemMesh( false, ES_WEAPON );
							m_bHideMesh = true;
						}	
					}
				}
			}
			else
			{
				if( m_bInvisibleWeaponMeshOnJump && m_bHideMesh )
				{
					m_pOwner->ShowEquipItemMesh( true, ES_WEAPON );
					m_bHideMesh = false;
				}	
			}
		}
	}
}

void ioDoubleJumpBuff::CheckRandomSeed( DWORD dwRandomSeed )
{
	m_iRandomSeed++;
	m_iOwnerRandomSeed = dwRandomSeed;
}
//////////////////////////////////////////////////////////////////////////////////////////////////////
// AbsoluteProtection Buff
ioAbsoluteProtectionBuff::ioAbsoluteProtectionBuff()
{
	m_bSetLoopEffect = false;
}

ioAbsoluteProtectionBuff::ioAbsoluteProtectionBuff( const ioAbsoluteProtectionBuff &rhs )
: ioBuff( rhs ),
m_StartEffect( rhs.m_StartEffect )
{
	m_bSetLoopEffect = false;
}

ioAbsoluteProtectionBuff::~ioAbsoluteProtectionBuff()
{
}

ioBuff* ioAbsoluteProtectionBuff::Clone()
{
	return new ioAbsoluteProtectionBuff( *this );
}

void ioAbsoluteProtectionBuff::LoadProperty( ioINILoader &rkLoader )
{
	ioBuff::LoadProperty( rkLoader );

	char szBuf[MAX_PATH];

	rkLoader.LoadString_e( "start_buff", "", szBuf, MAX_PATH );
	m_StartEffect = szBuf;
}

void ioAbsoluteProtectionBuff::StartBuffWithAniTime( ioBaseChar *pOwner, DWORD dwAniTime )
{
#ifndef SHIPPING 
	if( Setting::ShowExtraInfo() )
	{
		LOG.PrintTimeAndLog( 0, "BuffCheck - %s, StartTime: %d", m_Name.c_str(), FRAMEGETTIME() );
	}
#endif

	m_pOwner = pOwner;
	m_OwnerName = pOwner->GetCharName();

	CheckInvisibleWeapon( false );
	CheckInvisibleCloak( false );
	CheckInvisibleHelmet( false );
	CheckChangeColor( true );
	CheckChangeSkill( true );
	CheckBuffDesc();

	DWORD dwCurTime = FRAMEGETTIME();
	m_dwBuffEndTime = dwCurTime + GetBuffDuration();
	m_dwCheckTime = dwCurTime;

	if( m_CreatorLookType != CLT_NONE && !m_CreateChar.IsEmpty() )
	{
		if( m_CreateChar != m_pOwner->GetCharName() )
			m_pOwner->RotateByBuffCollision( m_CreateChar, m_CreatorLookType );
	}
	
	m_bLive = true;

	m_pOwner->AttachEffect( m_StartEffect );

	if( !m_LoopSound.IsEmpty() )
	{
		m_LoopSoundID = m_pOwner->PlaySound( m_LoopSound, true );
		m_bLoopSoundPlayed = true;
	}
}

bool ioAbsoluteProtectionBuff::ApplyStartBuff( ioBaseChar *pOwner, DWORD dwGlobalTime, DWORD dwGapTime )
{
	m_pOwner = pOwner;

	DWORD dwGlobalTimeGap = g_GlobalTimer.CalcGlobalGapTime( dwGlobalTime );
	if( m_iOperationType != OT_SWITCH )
	{
		if( dwGlobalTimeGap > dwGapTime )
			return false;

		dwGapTime -= dwGlobalTimeGap;
		m_dwBuffEndTime = FRAMEGETTIME() + dwGapTime;
	}

	m_dwCheckTime = FRAMEGETTIME();
	m_bLive = true;

	if( m_CreatorLookType != CLT_NONE && !m_CreateChar.IsEmpty() )
	{
		if( m_CreateChar != m_pOwner->GetCharName() )
			m_pOwner->RotateByBuffCollision( m_CreateChar, m_CreatorLookType );
	}
	
	m_pOwner->AttachEffect( m_StartEffect );

	if( !m_LoopSound.IsEmpty() )
	{
		m_LoopSoundID = m_pOwner->PlaySound( m_LoopSound, true );
		m_bLoopSoundPlayed = true;
	}

	CheckInvisibleWeapon( false );
	CheckInvisibleCloak( false );
	CheckInvisibleHelmet( false );
	CheckChangeSkill( true );
	CheckChangeColor( true );
	CheckBuffDesc();

	return true;
}

void ioAbsoluteProtectionBuff::ProcessBuff( float fTimePerSec )
{
	if( !m_bSetLoopEffect )
	{
		ioEffect *pEffect = m_pOwner->GetEffectByFileName( m_StartEffect );
		if( !pEffect || !pEffect->IsLiveAtNextFrame( fTimePerSec * FLOAT1000 ) )
		{
			m_pOwner->AttachEffect( m_BuffEffect, GetEffectEventOwner() );
			m_bSetLoopEffect = true;
		}
	}

	CharState eState = m_pOwner->GetState();
	if( eState != CS_PROTECTION && eState != CS_USING_SKILL )
	{
		SetReserveEndBuff();
		return;
	}

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
	case OT_ACTIVE:
		ioBuff::ProcessBuff( fTimePerSec );
		break;
	case OT_PASSIVE:
		CheckCallingAreaWeapon();
		break;
	}
}

void ioAbsoluteProtectionBuff::EndBuff()
{
#ifndef SHIPPING 
	if( Setting::ShowExtraInfo() )
	{
		LOG.PrintTimeAndLog( 0, "BuffCheck - %s, EndTime: %d", m_Name.c_str(), FRAMEGETTIME() );
	}
#endif

	CheckInvisibleWeapon( true );
	CheckInvisibleCloak( true );
	CheckInvisibleHelmet( true );
	CheckChangeSkill( false );
	CheckChangeColor( false );

	m_pOwner->EndEffect( m_StartEffect, false );
	m_pOwner->EndEffect( m_BuffEffect, false );

	if( m_bLoopSoundPlayed && !m_LoopSound.IsEmpty() )
	{
		g_SoundMgr.StopSound( m_LoopSound, m_LoopSoundID );
	}

	if( IsBadStateBuff() )
	{
		if( !m_pOwner->IsOwnerChar() )
			m_pOwner->SetEmoticon( m_EndEmoticon, EMT_BAD_STATE );
	}
	else
	{
		if( !m_pOwner->IsOwnerChar() )
			m_pOwner->SetEmoticon( m_EndEmoticon, EMT_SKILL_BUFF );
	}

	m_pOwner->AttachEffect( m_BuffEndEffect );

	DestroyAreaWeapon();
	CheckBuffEndBuff();
}
//////////////////////////////////////////////////////////////////////////////////////////////////////
// Eletric Buff
ioEletricBuff::ioEletricBuff()
{
	m_bBlowWounded = false;
}

ioEletricBuff::ioEletricBuff( const ioEletricBuff &rhs )
: ioBuff( rhs ),
 m_EletricAnimation( rhs.m_EletricAnimation ),
 m_EletricAnimationDown( rhs.m_EletricAnimationDown ),
 m_EletricAnimationDownBack( rhs.m_EletricAnimationDownBack ),
 m_EletricCatchAnimation( rhs.m_EletricCatchAnimation ),
 m_EletricCatchAnimationDown( rhs.m_EletricCatchAnimationDown ),
 m_EletricCatchAnimationDownBack( rhs.m_EletricCatchAnimationDownBack ),
 m_AddEntity( rhs.m_AddEntity )
{
	m_iEtcStateCnt = 0;
	m_bBlowWounded = false;
}

ioEletricBuff::~ioEletricBuff()
{
}

void ioEletricBuff::LoadProperty( ioINILoader &rkLoader )
{
	ioBuff::LoadProperty( rkLoader );

	char szBuf[MAX_PATH];
	rkLoader.LoadString_e( "eletric_animation", "", szBuf, MAX_PATH );
	m_EletricAnimation = szBuf;
	rkLoader.LoadString_e( "eletric_animation_down", "", szBuf, MAX_PATH );
	m_EletricAnimationDown = szBuf;
	rkLoader.LoadString_e( "eletric_animation_down_back", "", szBuf, MAX_PATH );
	m_EletricAnimationDownBack = szBuf;

	rkLoader.LoadString_e( "eletric_catch_animation", "", szBuf, MAX_PATH );
	m_EletricCatchAnimation = szBuf;
	rkLoader.LoadString_e( "eletric_catch_animation_down", "", szBuf, MAX_PATH );
	m_EletricCatchAnimationDown = szBuf;
	rkLoader.LoadString_e( "eletric_catch_animation_down_back", "", szBuf, MAX_PATH );
	m_EletricCatchAnimationDownBack = szBuf;

	rkLoader.LoadString_e( "add_mesh", "", szBuf, MAX_PATH );
	m_AddEntity = szBuf;
}

ioBuff* ioEletricBuff::Clone()
{
	return new ioEletricBuff( *this );
}

void ioEletricBuff::StartBuff(ioBaseChar *pOwner )
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

	m_bRseveSetEtcState = false; 

	if( m_iOperationType != OT_DELAY )
	{
		SetAction();
	}
}

bool ioEletricBuff::ApplyStartBuff(ioBaseChar *pOwner, DWORD dwGlobalTime, DWORD dwGapTime )
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

	m_dwCheckTime = FRAMEGETTIME();
	m_bLive = true;

	if( m_iOperationType != OT_DELAY )
	{
		SetAction();
	}

	return true;
}

void ioEletricBuff::ProcessBuff( float fTimePerSec )
{
	if( m_bRseveSetEtcState )
	{
		ioEntityGroup *pGrp = m_pOwner->GetGroup();
		pGrp->ClearAllActionAni( FLOAT100, true );
		pGrp->ClearAllLoopAni( FLOAT100, true );

		if( m_pOwner->IsFloatingState() ||
			m_pOwner->IsApplyDownState() )
		{
			if( m_pOwner->GetCurAttackedSide() == AS_FRONT )
			{
				if( !m_pOwner->IsPrisonerMode() && !m_pOwner->IsCatchMode() )
					pGrp->SetLoopAni( m_EletricAnimationDown, FLOAT100 );
				else
					pGrp->SetLoopAni( m_EletricCatchAnimationDown, FLOAT100 );
			}
			else
			{
				if( !m_pOwner->IsPrisonerMode() && !m_pOwner->IsCatchMode() )
					pGrp->SetLoopAni( m_EletricAnimationDownBack, FLOAT100 );
				else
					pGrp->SetLoopAni( m_EletricCatchAnimationDownBack, FLOAT100 );
			}

			m_pOwner->CheckWearItemDrop();
			
			m_bBlowWounded = true;
		}
		else
		{
			if( !m_pOwner->IsPrisonerMode() && !m_pOwner->IsCatchMode() )
				pGrp->SetLoopAni( m_EletricAnimation, FLOAT100 );
			else
				pGrp->SetLoopAni( m_EletricCatchAnimation, FLOAT100 );
		}

		m_pOwner->SetState( CS_ETC_STATE );

		m_iEtcStateCnt = m_pOwner->GetCurEtcStateCnt();
		
		if( m_bBlowWounded )
			m_pOwner->SetBlowEtcState( true );

		m_bRseveSetEtcState = false;
	}

	if( m_iOperationType == OT_DELAY )
	{
		if( m_bBuffStart )
		{
			if( m_pOwner->GetState() != CS_ETC_STATE ||
				m_pOwner->GetCurEtcStateCnt() != m_iEtcStateCnt )
			{
				SetReserveEndBuff();
				return;
			}
		}
	}
	else
	{
		if( m_pOwner->GetState() != CS_ETC_STATE ||
			m_pOwner->GetCurEtcStateCnt() != m_iEtcStateCnt )
		{
			SetReserveEndBuff();
			return;
		}
	}
	
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
}

void ioEletricBuff::EndBuff()
{
	if( !m_AddEntity.IsEmpty() )
	{
		m_pOwner->DestroyEntity( m_AddEntity );
		m_pOwner->RemoveMaterialModifyExcept( m_AddEntity );
	}

	if( m_pOwner->GetState() == CS_ETC_STATE && m_pOwner->GetCurEtcStateCnt() == m_iEtcStateCnt )
	{
		if( !m_bBlowWounded )
		{
			m_pOwner->SetState( CS_DELAY, m_bSendDelayNetWork );
			m_pOwner->SetDashFailStart();
		}
		else
		{
			if( !m_pOwner->IsFloating() )
			{
				m_pOwner->SetState( CS_BLOW_WOUNDED );
				m_pOwner->SetBlowWoundedGetUp();
			}
			else
			{
				BlowWoundAniInfo kInfo;
				m_pOwner->SetBlowWoundedState( true, kInfo, true );
			}
		}
	}

	ioBuff::EndBuff();
}

void ioEletricBuff::SetAction()
{
	CheckInvisibleWeapon( false );
	CheckInvisibleCloak( false );
	CheckInvisibleHelmet( false );
	
	CheckChangeSkill( true );	
	CheckChangeColor( true );
	CheckBuffDesc();

	if( m_CreatorLookType != CLT_NONE && !m_CreateChar.IsEmpty() )
	{
		if( m_CreateChar != m_pOwner->GetCharName() )
			m_pOwner->RotateByBuffCollision( m_CreateChar, m_CreatorLookType );
	}

	m_pOwner->AttachEffect( m_BuffEffect, GetEffectEventOwner() );

	if( !m_LoopSound.IsEmpty() )
	{
		m_LoopSoundID = m_pOwner->PlaySound( m_LoopSound, true );
		m_bLoopSoundPlayed = true;
	}

	if( !m_AddEntity.IsEmpty() )
	{
		m_pOwner->AddEntity( m_AddEntity );
		m_pOwner->AddMaterialModifyExcept( m_AddEntity );
	}

	if( m_pOwner->GetState() != CS_DIE &&
		m_pOwner->GetState() != CS_READY &&
		m_pOwner->GetState() != CS_LOADING &&
		m_pOwner->GetState() != CS_OBSERVER &&
		m_pOwner->GetState() != CS_VIEW )
	{
		m_bRseveSetEtcState = true;
	}

	m_bBuffStart = true;
}

void ioEletricBuff::ProcessDelay( float fTimePerSec )
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
//////////////////////////////////////////////////////////////////////////////////////////////////////
// ManaShield Buff
ioManaShieldBuff::ioManaShieldBuff()
{
	m_fStartGauge = 0.0f;
	m_fPreGauge = 0.0f;
}

ioManaShieldBuff::ioManaShieldBuff( const ioManaShieldBuff &rhs )
: ioBuff( rhs ),
 m_bDisableDownState( rhs.m_bDisableDownState )
{
	m_fStartGauge = 0.0f;
	m_fPreGauge = 0.0f;
}

void ioManaShieldBuff::LoadProperty( ioINILoader &rkLoader )
{
	ioBuff::LoadProperty( rkLoader );

	m_bDisableDownState  = rkLoader.LoadBool_e( "disable_down_state", false );
}

ioManaShieldBuff::~ioManaShieldBuff()
{
}

ioBuff* ioManaShieldBuff::Clone()
{
	return new ioManaShieldBuff( *this );
}

void ioManaShieldBuff::StartBuff( ioBaseChar *pOwner )
{
	ioBuff::StartBuff( pOwner );

	//m_fStartGauge = m_pOwner->GetCurItemSkillGauge(m_ItemName);
	ioItem *pItem = GetOwnerItem();
	if( pItem && pItem->HasSkillGauge() )
		m_fStartGauge = pItem->GetMaxSkillGauge( m_pOwner->GetCharName() );
	else
		m_fStartGauge = m_pOwner->GetCurItemSkillGauge(m_ItemName);
	m_fPreGauge = m_fStartGauge;
	m_pOwner->CheckManaShield( true );
}

bool ioManaShieldBuff::ApplyStartBuff( ioBaseChar *pOwner, DWORD dwGlobalTime, DWORD dwGapTime )
{
	if( !ioBuff::ApplyStartBuff( pOwner, dwGlobalTime, dwGapTime ) )
		return false;	

	//m_fStartGauge = m_pOwner->GetCurItemSkillGauge(m_ItemName);
	ioItem *pItem = GetOwnerItem();
	if( pItem && pItem->HasSkillGauge() )
		m_fStartGauge = pItem->GetMaxSkillGauge( m_pOwner->GetCharName() );
	else
		m_fStartGauge = m_pOwner->GetCurItemSkillGauge(m_ItemName);
	m_fPreGauge = m_fStartGauge;
	m_pOwner->CheckManaShield( true );
	return true;
}

void ioManaShieldBuff::ProcessBuff( float fTimePerSec )
{
	ioBaseChar *pOwner = GetOwner();
	if( !pOwner )
	{
		SetReserveEndBuff();
		return;
	}

	CheckOwnerStateCheck();

	switch( m_iOperationType )
	{
	case OT_SWITCH:
		if( !CheckSwitchType( fTimePerSec ) )
		{
			m_pOwner->CheckUseBuffSkill( m_Name, false );
			SetReserveEndBuff();
			return;
		}

		CheckCallingProcess();
		break;
	case OT_PASSIVE:
		CheckCallingAreaWeapon();
		break;
	case OT_DELAY:
	case OT_ACTIVE:
		ioBuff::ProcessBuff( fTimePerSec );
		break;
	}

	if( !IsRightOwnerItem() )
	{
		pOwner->CheckUseBuffSkill( m_Name, false );
		SetReserveEndBuff();
		return;
	}

	float fCurGauge = m_pOwner->GetCurItemSkillGauge(m_ItemName);
	if( m_fStartGauge <= 0.99f || fCurGauge <= 0.01f )
	{
		pOwner->CheckUseBuffSkill( m_Name, false );
		SetReserveEndBuff();
		return;
	}

	if( fCurGauge < m_fPreGauge )
	{
		ioEffect *pEffect = m_pOwner->GetEffectByFileName( m_BuffEffect );
		if( pEffect )
		{
			pEffect->SetExtendColorRate( fCurGauge / m_fStartGauge );
		}

		m_fPreGauge = fCurGauge;
	}
}

void ioManaShieldBuff::EndBuff()
{
	ioBuff::EndBuff();
	m_pOwner->CheckManaShield( false );
}

void ioManaShieldBuff::CheckOwnerStateCheck()
{
	ioBaseChar *pOwner = GetOwner();
	if( !pOwner ) return;

	CharState eState = pOwner->GetState();

	bool bDown = false;
	if( m_bDisableDownState )
	{
		if( eState == CS_FROZEN && m_pOwner->GetBlowFrozenState() )
			bDown = true;
		else if( eState == CS_BLOW_WOUNDED && !m_pOwner->IsFloatingState() )
			bDown = true;
		else if( eState == CS_BOUND_BLOW_WOUNDED && !m_pOwner->IsFloatingState() )
			bDown = true;
		else if( eState == CS_ETC_STATE && m_pOwner->IsApplyDownState(false) )
			bDown = true;
		else if( eState == CS_STOP_MOTION && m_pOwner->GetBlowStopMotionState() )
			bDown = true;
		else if( eState == CS_DROP_ZONE_DOWN )
			bDown = true;
		else if( eState == CS_ICE_STATE && m_pOwner->GetBlowStopMotionState() )
			bDown = true;

		if( bDown )
		{
			pOwner->CheckUseBuffSkill( m_Name, false );
			SetReserveEndBuff();
		}
	}
}
//////////////////////////////////////////////////////////////////////////////////////////////////////
// WarpState Buff
ioWarpStateBuff::ioWarpStateBuff()
{
	m_bWarpStateCancel = false;
}

ioWarpStateBuff::ioWarpStateBuff( const ioWarpStateBuff &rhs )
: ioBuff( rhs ),
m_PositionType(rhs.m_PositionType),
m_WarpAnimation( rhs.m_WarpAnimation ),
m_WarpCatchAnimation( rhs.m_WarpCatchAnimation ),
m_iEnablePositionCnt( rhs.m_iEnablePositionCnt ),
m_fEnablePositionRange( rhs.m_fEnablePositionRange ),
m_fEnableMaxPositionRange( rhs.m_fEnableMaxPositionRange ),
m_bCheckDropZone( rhs.m_bCheckDropZone ),
m_bChangeDummy( rhs.m_bChangeDummy ),
m_fExtraHeight( rhs.m_fExtraHeight ),
m_fExtraHeightMax( rhs.m_fExtraHeightMax ),
m_bEnableEndJump( rhs.m_bEnableEndJump ),
m_bCheckHeight( rhs.m_bCheckHeight ),
m_fCheckLimitHeight( rhs.m_fCheckLimitHeight ),
m_bSkillStartPos( rhs.m_bSkillStartPos ),
m_fWarpOffsetNearMyChar( rhs.m_fWarpOffsetNearMyChar )
{
	m_bWarpStateCancel = false;
}

ioWarpStateBuff::~ioWarpStateBuff()
{
}

ioBuff* ioWarpStateBuff::Clone()
{
	return new ioWarpStateBuff( *this );
}

void ioWarpStateBuff::LoadProperty( ioINILoader &rkLoader )
{
	ioBuff::LoadProperty( rkLoader );

	m_PositionType = (PositionType)rkLoader.LoadInt_e( "warp_position_type", 0 );

	char szBuf[MAX_PATH];
	rkLoader.LoadString_e( "warp_animation", "", szBuf, MAX_PATH );
	m_WarpAnimation = szBuf;

	rkLoader.LoadString_e( "warp_catch_animation", "", szBuf, MAX_PATH );
	m_WarpCatchAnimation = szBuf;

	m_iEnablePositionCnt = rkLoader.LoadInt_e( "enable_warp_position_cnt", 0 );
	m_fEnablePositionRange = rkLoader.LoadFloat_e( "enable_warp_position_range", 0.0f );
	m_fEnableMaxPositionRange = rkLoader.LoadFloat_e( "enable_warp_position_range_max", 0.0f );
	m_fExtraHeight = rkLoader.LoadFloat_e( "extra_height", 0.0f );
	m_fExtraHeightMax = rkLoader.LoadFloat_e( "extra_height_max", 0.0f );
	
	m_bCheckDropZone = rkLoader.LoadBool_e( "check_drop_zone", false );
	m_bChangeDummy = rkLoader.LoadBool_e( "change_dummy_state", false );
	m_bEnableEndJump = rkLoader.LoadBool_e( "enable_end_jump", false );

	m_bCheckHeight = rkLoader.LoadBool_e( "check_height", false );
	m_fCheckLimitHeight = rkLoader.LoadFloat_e( "check_limit_height", FLOAT100 );
	m_bSkillStartPos = rkLoader.LoadBool_e( "set_skill_start_position", false );

	m_fWarpOffsetNearMyChar = rkLoader.LoadFloat_e( "warp_enemy_near_my_char_offset", FLOAT100 );
}

void ioWarpStateBuff::StartBuff( ioBaseChar *pOwner )
{
	ioBuff::StartBuff( pOwner );

	m_bSetCreatorPos = false;
	ioBaseChar *pCreator = m_pOwner->GetBaseChar( m_CreateChar );
	if( pCreator && ( m_PositionType == PT_CREATOR || 
					m_PositionType == PT_STARTING_POINT || 
					m_PositionType == PT_ENEMY_STARTING_POINT ) )
	{
		m_vCreatorPos = pCreator->GetEnableWarpPosition( pOwner,
														 m_iEnablePositionCnt,
														 m_fEnablePositionRange,
														 m_fEnableMaxPositionRange,
														 m_bCheckDropZone,
														 m_bCheckHeight,
														 m_fCheckLimitHeight,
														 m_bSkillStartPos, 
														 m_PositionType );

		IORandom random;
		DWORD dwSeed = 0;
		int iRand = 0;
		dwSeed = pOwner->GetRandomSeed();
		random.SetRandomSeed( dwSeed );

		float fMax = max( m_fExtraHeight, m_fExtraHeightMax );
		float fGap = fabsf( m_fExtraHeight - fMax );
		float fCurHeight = m_fExtraHeight;

		if( fCurHeight != fMax )
		{
			int iRand = random.Random( (DWORD)fGap );
			fCurHeight = m_fExtraHeight + iRand;
		}

		m_vCreatorPos.y += fCurHeight;
		m_bSetCreatorPos = true;
	}
	else if( pCreator && m_PositionType == PT_DIR )
	{
		m_vCreatorPos = pCreator->GetWorldPosition();

		D3DXVECTOR3 vDir = pCreator->GetInputDir();
		m_vCreatorPos += m_fEnableMaxPositionRange * vDir;

		m_bSetCreatorPos = true;
	}
	// SetWarpState
	ioEntityGroup *pGrp = pOwner->GetGroup();
	pGrp->ClearAllActionAni( FLOAT100, true );
	pGrp->ClearAllLoopAni( FLOAT100, true );

	if( !pOwner->IsPrisonerMode() && !pOwner->IsCatchMode() )
		pGrp->SetLoopAni( m_WarpAnimation, FLOAT100 );
	else
		pGrp->SetLoopAni( m_WarpCatchAnimation, FLOAT100 );

	pOwner->SetWarpState();
	m_bWarpStateCancel = false;
}

bool ioWarpStateBuff::ApplyStartBuff( ioBaseChar *pOwner, DWORD dwGlobalTime, DWORD dwGapTime )
{
	if( !ioBuff::ApplyStartBuff( pOwner, dwGlobalTime, dwGapTime ) )
		return false;

	m_bSetCreatorPos = false;
	ioBaseChar *pCreator = m_pOwner->GetBaseChar( m_CreateChar );
	if( pCreator && m_PositionType == PT_CREATOR )
	{
		m_vCreatorPos = pCreator->GetEnableWarpPosition( pOwner,
														 m_iEnablePositionCnt,
														 m_fEnablePositionRange,
														 m_fEnableMaxPositionRange,
														 m_bCheckDropZone,
														 m_bCheckHeight,
														 m_fCheckLimitHeight,
														 m_bSkillStartPos );

		IORandom random;
		DWORD dwSeed = 0;
		int iRand = 0;
		dwSeed = pOwner->GetRandomSeed();
		random.SetRandomSeed( dwSeed );

		float fMax = max( m_fExtraHeight, m_fExtraHeightMax );
		float fGap = fabsf( m_fExtraHeight - fMax );
		float fCurHeight = m_fExtraHeight;

		if( fCurHeight != fMax )
		{
			int iRand = random.Random( (DWORD)fGap );
			fCurHeight = m_fExtraHeight + iRand;
		}

		m_vCreatorPos.y += m_fExtraHeight;
		m_bSetCreatorPos = true;
	}

	ioEntityGroup *pGrp = pOwner->GetGroup();
	pGrp->ClearAllActionAni( FLOAT100, true );
	pGrp->ClearAllLoopAni( FLOAT100, true );

	if( !pOwner->IsPrisonerMode() && !pOwner->IsCatchMode() )
		pGrp->SetLoopAni( m_WarpAnimation, FLOAT100 );
	else
		pGrp->SetLoopAni( m_WarpCatchAnimation, FLOAT100 );

	pOwner->SetWarpState();
	m_bWarpStateCancel = false;
	return true;
}

void ioWarpStateBuff::ProcessBuff( float fTimePerSec )
{
	if( m_pOwner->GetState() != CS_WARP_STATE )
	{
		m_bWarpStateCancel = true;
		SetReserveEndBuff();
		return;
	}
	
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
	case OT_PASSIVE:
		CheckCallingAreaWeapon();
		break;
	case OT_DELAY:
	case OT_ACTIVE:
		if( CheckEnableDelayState() )
			break;

		if( m_dwBuffEndTime <= FRAMEGETTIME() )
		{
			CheckWarpPosition();
			SetReserveEndBuff();
			break;
		}

		CheckCallingProcess();
		break;
	}
}

void ioWarpStateBuff::EndBuff()
{
	if( m_bChangeDummy )
	{
		m_pOwner->SetState( CS_DUMMY_STATE );
	}
	else if( m_bEnableEndJump )
	{
		m_pOwner->SetSKillEndJumpState( 0.1f, false, false, false );
	}
	else
	{
		if( !m_bWarpStateCancel )
		{
			m_pOwner->SetState( CS_DELAY, m_bSendDelayNetWork );
		}
	}

	ioBuff::EndBuff();
}

void ioWarpStateBuff::CheckWarpPosition()
{
	switch( m_PositionType )
	{
	case PT_START:
		m_pOwner->SetStartPos();
		break;
	case PT_CREATOR:
	case PT_STARTING_POINT:
	case PT_DIR:
	case PT_ENEMY_STARTING_POINT:
		if( m_bSetCreatorPos )
		{
			m_pOwner->SetWorldPosition( m_vCreatorPos );
		}
		break;
	case PT_ENEMY_NEAR_CASTER_POINT:
		{
			ioBaseChar *pCreator = m_pOwner->GetBaseChar( m_CreateChar );
			if( pCreator )
			{
				D3DXVECTOR3 vOffset = pCreator->GetMoveDir() * m_fWarpOffsetNearMyChar;
				D3DXVECTOR3 vWarpPos = pCreator->GetWorldPosition() + vOffset;
				m_pOwner->SetWorldPosition( vWarpPos );
			}
		}
		break;
	}
}
//////////////////////////////////////////////////////////////////////////////////////////////////////
// CloseOrder Buff
ioCloseOrderBuff::ioCloseOrderBuff()
{
	m_dwCloseOrderDuration = 0;

	m_dwTargetAreaWeaponIdx = 0;
	m_dwTargetWeaponIdx = 0;

	m_vTargetPos = ioMath::VEC3_ZERO;
}

ioCloseOrderBuff::ioCloseOrderBuff( const ioCloseOrderBuff &rhs )
: ioBuff( rhs ),
 m_bOnlyMoveDir( rhs.m_bOnlyMoveDir ),
 m_fDecreaseSpeed( rhs.m_fDecreaseSpeed ),
 m_fAniRate( rhs.m_fAniRate ),
 m_PreDelayAnimation( rhs.m_PreDelayAnimation ),
 m_MoveAnimation( rhs.m_MoveAnimation ),
 m_PreDelayCatchAnimation( rhs.m_PreDelayCatchAnimation ),
 m_MoveCatchAnimation( rhs.m_MoveCatchAnimation ),
 m_dwCloseOrderMinTime( rhs.m_dwCloseOrderMinTime ),
 m_dwCloseOrderMaxTime( rhs.m_dwCloseOrderMaxTime ),
 m_dwDelayTime( rhs.m_dwDelayTime ),
 m_eCloseOrderType( rhs.m_eCloseOrderType ),
 m_eCloseOrderSubType( rhs.m_eCloseOrderSubType ),
 m_eCloseOrderWeaponType( rhs.m_eCloseOrderWeaponType ),
 m_AngleList( rhs.m_AngleList ),
 m_bMustBlowWound( rhs.m_bMustBlowWound ),
 m_fCarefulStepHeight( rhs.m_fCarefulStepHeight ),
 m_fMoveRangeToStop( rhs.m_fMoveRangeToStop )
{
	m_dwCloseOrderDuration = 0;

	m_dwTargetAreaWeaponIdx = 0;
	m_dwTargetWeaponIdx = 0;

	m_vTargetPos = ioMath::VEC3_ZERO;
}

ioCloseOrderBuff::~ioCloseOrderBuff()
{
}

void ioCloseOrderBuff::LoadProperty( ioINILoader &rkLoader )
{
	ioBuff::LoadProperty( rkLoader );

	m_fCarefulStepHeight = rkLoader.LoadFloat_e( "careful_step_height", 0.0f );
	m_fDecreaseSpeed = rkLoader.LoadFloat_e( "decrease_move_speed", 0.0f );
	m_fAniRate = rkLoader.LoadFloat_e( "closeorder_ani_rate", FLOAT1 );

	m_dwCloseOrderMinTime = rkLoader.LoadInt_e( "closeorder_min_time", 0 );
	m_dwCloseOrderMaxTime = rkLoader.LoadInt_e( "closeorder_max_time", 0 );

	m_bMustBlowWound = rkLoader.LoadBool_e( "must_blow_wound", false );

	char szBuf[MAX_PATH];
	rkLoader.LoadString_e( "pre_delay_animation", "", szBuf, MAX_PATH );
	m_PreDelayAnimation = szBuf;
	rkLoader.LoadString_e( "move_animation", "", szBuf, MAX_PATH );
	m_MoveAnimation = szBuf;

	rkLoader.LoadString_e( "pre_delay_catch_animation", "", szBuf, MAX_PATH );
	m_PreDelayCatchAnimation = szBuf;
	rkLoader.LoadString_e( "move_catch_animation", "", szBuf, MAX_PATH );
	m_MoveCatchAnimation = szBuf;

	m_bOnlyMoveDir = rkLoader.LoadBool_e( "only_move_dir", false );
	m_dwDelayTime = rkLoader.LoadInt_e( "delay_time", 0 );
	m_eCloseOrderType = (CloseOrderType) rkLoader.LoadInt_e( "closeorder_type", 1 );
	m_eCloseOrderSubType = (CloseOrderTargetType)rkLoader.LoadInt_e( "closeorder_target_type", 0 );
	m_eCloseOrderWeaponType = (CloseOrderWeaponType)rkLoader.LoadInt_e( "closeorder_weapon_type", COWT_AREAWEAPON );
	int iAngleCnt = rkLoader.LoadInt_e( "angle_cnt", 0 );
	m_AngleList.clear();

	for( int i=0; i < iAngleCnt; ++i )
	{
		wsprintf_e( szBuf, "angle%d", i+1 );
		float fAngle = rkLoader.LoadFloat( szBuf, 0.0f );
		m_AngleList.push_back( fAngle );
	}

	m_fMoveRangeToStop = rkLoader.LoadFloat_e( "move_range_to_stop", 0.0f );
}

ioBuff* ioCloseOrderBuff::Clone()
{
	return new ioCloseOrderBuff( *this );
}

void ioCloseOrderBuff::StartBuff( ioBaseChar *pOwner )
{
	ioBuff::StartBuff( pOwner );

	m_bEndCloseOrder = false;

	DWORD dwStartTime = FRAMEGETTIME();
	dwStartTime += m_dwDelayTime;

	float fCurAngle = 0.0f;
	if( m_eCloseOrderType == COT_NORMAL )
	{
		if( !m_AngleList.empty() )
		{
			DWORD dwSeed = pOwner->GetRandomSeed();

			IORandom random;
			random.SetRandomSeed( dwSeed );
			int iRand = random.Random( m_AngleList.size() );

			if( COMPARE( iRand, 0, (int) m_AngleList.size() ) )
				fCurAngle = m_AngleList[iRand];
		}
	}

	if( !m_ItemName.IsEmpty() )
	{
		ioGrowthNormalUpInfo *pUpInfo = ToNormalUpInfo(GetGrowthUpInfoForItem(GT_CLOSE_ORDER_DURATION));
		if( pUpInfo )
		{
			ioPlayStage *pStage = g_ItemMaker.GetPlayStage();
			m_dwCloseOrderDuration = (DWORD)pUpInfo->GetValue(pStage);
		}
	}

	if( !pOwner->IsPrisonerMode() && !pOwner->IsCatchMode() )
		pOwner->SetCloseOrderAni( m_MoveAnimation, m_fAniRate, fCurAngle,
								  dwStartTime, m_dwCloseOrderMinTime+m_dwCloseOrderDuration, m_dwCloseOrderMaxTime+m_dwCloseOrderDuration, m_bOnlyMoveDir,
								  m_eCloseOrderType );
	else
		pOwner->SetCloseOrderAni( m_MoveCatchAnimation, m_fAniRate, fCurAngle,
								  dwStartTime, m_dwCloseOrderMinTime+m_dwCloseOrderDuration, m_dwCloseOrderMaxTime+m_dwCloseOrderDuration, m_bOnlyMoveDir,
								  m_eCloseOrderType );

	pOwner->SetExtraMoveSpeed( m_fDecreaseSpeed );

	pOwner->GetGroup()->ClearAllActionAni( FLOAT100, true );
	pOwner->GetGroup()->ClearAllLoopAni( FLOAT100, true );

	if( !pOwner->IsPrisonerMode() && !pOwner->IsCatchMode() )
		pOwner->GetGroup()->SetLoopAni( m_PreDelayAnimation, FLOAT100, FLOAT1, m_fAniRate );
	else
		pOwner->GetGroup()->SetLoopAni( m_PreDelayCatchAnimation, FLOAT100, FLOAT1, m_fAniRate );

	SetExtraDurationByGrowth( (float)m_dwCloseOrderDuration );

}

bool ioCloseOrderBuff::ApplyStartBuff( ioBaseChar *pOwner, DWORD dwGlobalTime, DWORD dwGapTime )
{
	if( !ioBuff::ApplyStartBuff( pOwner, dwGlobalTime, dwGapTime ) )
		return false;

	m_bEndCloseOrder = false;

	DWORD dwStartTime = FRAMEGETTIME();
	dwStartTime += m_dwDelayTime;

	float fCurAngle = 0.0f;
	if( m_eCloseOrderType == COT_NORMAL )
	{
		if( !m_AngleList.empty() )
		{
			DWORD dwSeed = pOwner->GetRandomSeed();

			IORandom random;
			random.SetRandomSeed( dwSeed );
			int iRand = random.Random( m_AngleList.size() );

			if( COMPARE( iRand, 0, (int) m_AngleList.size() ) )
				fCurAngle = m_AngleList[iRand];
		}
	}

	if( !pOwner->IsPrisonerMode() && !pOwner->IsCatchMode() )
		pOwner->SetCloseOrderAni( m_MoveAnimation, m_fAniRate, fCurAngle,
								  dwStartTime, m_dwCloseOrderMinTime+m_dwCloseOrderDuration, m_dwCloseOrderMaxTime+m_dwCloseOrderDuration, m_bOnlyMoveDir,
								  m_eCloseOrderType );
	else
		pOwner->SetCloseOrderAni( m_MoveCatchAnimation, m_fAniRate, fCurAngle,
								  dwStartTime, m_dwCloseOrderMinTime+m_dwCloseOrderDuration, m_dwCloseOrderMaxTime+m_dwCloseOrderDuration, m_bOnlyMoveDir,
								  m_eCloseOrderType );

	pOwner->SetExtraMoveSpeed( m_fDecreaseSpeed );

	pOwner->GetGroup()->ClearAllActionAni( FLOAT100, true );
	pOwner->GetGroup()->ClearAllLoopAni( FLOAT100, true );

	if( !pOwner->IsPrisonerMode() && !pOwner->IsCatchMode() )
		pOwner->GetGroup()->SetLoopAni( m_PreDelayAnimation, FLOAT100, FLOAT1, m_fAniRate );
	else
		pOwner->GetGroup()->SetLoopAni( m_PreDelayCatchAnimation, FLOAT100, FLOAT1, m_fAniRate );

	CheckInvisibleWeapon( false );
	CheckInvisibleCloak( false );
	CheckInvisibleHelmet( false );

	return true;
}

void ioCloseOrderBuff::ProcessBuff( float fTimePerSec )
{
	if( CheckEnableDelayState() )
		return;
	
	if( m_pOwner->GetState() != CS_CLOSEORDER )
	{
		SetReserveEndBuff();
		return;
	}
	
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
	case OT_ACTIVE:
		ioBuff::ProcessBuff( fTimePerSec );

		if( m_eCloseOrderType == COT_TRACE )
		{
			RotateToCreator();
		}
		else if( m_eCloseOrderType == COT_IMITATIION )
		{
			if( m_pOwner && !m_CreateChar.IsEmpty() )
			{
				ioBaseChar *pCreator = m_pOwner->GetBaseChar( m_CreateChar );
				if( pCreator )
					m_pOwner->SetTargetRotAndMoveDirChange( pCreator->GetTargetRot() );
			}
		}
		else if( m_eCloseOrderType == COT_AREA_TRACE )
		{
			RotateToWeapon();
		}
		
		if( m_eCloseOrderSubType == COTT_AIR )
		{
			D3DXVECTOR3 vCurPos = m_pOwner->GetWorldPosition();
			m_pOwner->SetWorldPosition( vCurPos );

			if( vCurPos.y <  m_vTargetPos.y )
			{
				m_pOwner->SetJumpPower( 250.0f );
			}
			else
			{
				m_pOwner->SetJumpPowerRateByBuff( 0.75f );				
			}
			
			m_pOwner->SetCurMoveSpeed( m_fDecreaseSpeed );
		}

		break;
		
	case OT_PASSIVE:
		CheckCallingAreaWeapon();
		break;
	}
}

void ioCloseOrderBuff::EndBuff()
{
	if( !m_bEndCloseOrder )
	{
		m_pOwner->RemoveExtraMoveSpeed( m_fDecreaseSpeed );
		m_pOwner->SetCloseOrderDir( 0.0f, false );
	}

	if( m_eCloseOrderSubType == COTT_AIR )
	{
		m_pOwner->SetCurMoveSpeed( 0 );
	}

	if( m_pOwner->GetState() == CS_DELAY )
	{
		m_pOwner->SetState( CS_DELAY, m_bSendDelayNetWork );
	}
	else if( m_pOwner->GetState() == CS_RUN )
	{
		m_pOwner->SetState( CS_RUN );
	}

	//무한스턴 방지
	if( m_pOwner->GetState() == CS_CLOSEORDER )
	{
		m_pOwner->SetState( CS_DELAY );
	}

	ioBuff::EndBuff();
}

// 2벡터간의 각도를 구한다.
float ioCloseOrderBuff::GetDegOwnerCreator()
{
	if( !m_pOwner )
		return 0.0f;
	if( m_CreateChar.IsEmpty() )
		return 0.0f;
	ioBaseChar *pCreator = m_pOwner->GetBaseChar( m_CreateChar );
	if( !pCreator )
		return 0.0f;

	D3DXVECTOR3 vOwnerDir   = m_pOwner->GetMoveDir();
	D3DXVECTOR3 vCreatorDir = pCreator->GetMoveDir();
	D3DXVec3Normalize( &vOwnerDir, &vOwnerDir );
	D3DXVec3Normalize( &vCreatorDir, &vCreatorDir );
	float fCos = D3DXVec3Dot( &vCreatorDir, &vOwnerDir );
	float fRadian = acos( fCos );
	return RADtoDEG(fRadian);
}

void ioCloseOrderBuff::RotateToCreator()
{
	if( !m_pOwner )
		return;
	if( m_CreateChar.IsEmpty() )
		return;
	ioBaseChar *pCreator = m_pOwner->GetBaseChar( m_CreateChar );
	if( !pCreator )
		return;

	D3DXVECTOR3 vTargetDir = pCreator->GetWorldPosition() - m_pOwner->GetWorldPosition();
	D3DXVec3Normalize( &vTargetDir, &vTargetDir );

	D3DXVECTOR3 vXZDir( vTargetDir.x, 0.0f, vTargetDir.z );
	D3DXVec3Normalize( &vXZDir, &vXZDir );
	D3DXQUATERNION qtTargetRot;
	ioMath::CalcDirectionQuaternion( qtTargetRot, -vXZDir );

	m_pOwner->SetTargetRotAndMoveDirChange( qtTargetRot );
}

void ioCloseOrderBuff::RotateToWeapon()
{
	if( !m_pOwner )
		return;

	D3DXVECTOR3 vCurPos = m_vTargetPos;

	if( m_eCloseOrderWeaponType == COWT_AREAWEAPON )
	{
		ioAreaWeapon *pAreaWeapon = g_AreaWeaponMgr.FindAreaWeaponByIndex( m_dwTargetAreaWeaponIdx );
		if( pAreaWeapon )
		{
			vCurPos = pAreaWeapon->GetStartPos();
			m_vTargetPos = vCurPos;
		}
		else
		{
			if( m_eCloseOrderSubType == COTT_AIR )
			{
				SetReserveEndBuff();
			}
			else
			{
				if( m_dwTargetAreaWeaponIdx != 0 && vCurPos == ioMath::VEC3_ZERO )
				{
					SetReserveEndBuff();
					return;
				}
			}
		}
	}
	else if( m_eCloseOrderWeaponType == COWT_WEAPON )
	{
		ioWeapon* pWeapon = g_WeaponMgr.FindWeapon( GetCreator(), m_dwTargetWeaponIdx );
		if( pWeapon )
		{
			vCurPos = pWeapon->GetPosition();
			m_vTargetPos = vCurPos;
			
		}
		else
		{
			if( m_eCloseOrderSubType == COTT_AIR )
			{
				SetReserveEndBuff();
			}
			else
			{
				if( m_dwTargetWeaponIdx != 0 && vCurPos == ioMath::VEC3_ZERO )
				{
					SetReserveEndBuff();
					return;
				}
			}
		}
	}
	else
	{
		//맞는 웨폰 타입이 없음으로 종료
		SetReserveEndBuff();
	}

	D3DXVECTOR3 vTargetDir = vCurPos - m_pOwner->GetWorldPosition();

	float fLength = D3DXVec3Length( &vTargetDir );
	
	if( !m_bEndCloseOrder && m_fMoveRangeToStop > 0.0f && fLength <= m_fMoveRangeToStop )
	{
		m_pOwner->RemoveExtraMoveSpeed( m_fDecreaseSpeed );
		m_pOwner->SetCloseOrderDir( 0.0f, false );
		m_bEndCloseOrder = true;
	}

	D3DXVec3Normalize( &vTargetDir, &vTargetDir );

	D3DXVECTOR3 vXZDir( vTargetDir.x, 0.0f, vTargetDir.z );
	D3DXVec3Normalize( &vXZDir, &vXZDir );
	D3DXQUATERNION qtTargetRot;
	ioMath::CalcDirectionQuaternion( qtTargetRot, -vXZDir );

	m_pOwner->SetTargetRotAndMoveDirChange( qtTargetRot );
}

void ioCloseOrderBuff::GetBuffInfo( SP2Packet &rkPacket )
{
	ioBuff::GetBuffInfo( rkPacket );

	rkPacket << m_dwCloseOrderDuration;
}

void ioCloseOrderBuff::ApplyBuffInfo( SP2Packet &rkPacket )
{
	rkPacket >> m_dwCloseOrderDuration;
}

void ioCloseOrderBuff::SetTargetAreaWeapon( DWORD dwIndex )
{
	m_dwTargetAreaWeaponIdx = dwIndex;
}

void ioCloseOrderBuff::SetTargetWeapon( DWORD dwIndex )
{
	m_dwTargetWeaponIdx = dwIndex;
}
/////////////////////////////////////////////////////////////////////////////////////////////////////
// Float Buff
ioFloatBuff::ioFloatBuff()
{
}

ioFloatBuff::ioFloatBuff( const ioFloatBuff &rhs )
: ioBuff( rhs ),
 m_FloatBuffInfo( rhs.m_FloatBuffInfo ),
 m_fFloatOffHeight( rhs.m_fFloatOffHeight ),
 m_FloatAnimation( rhs.m_FloatAnimation ),
 m_FloatCatchAnimation( rhs.m_FloatCatchAnimation ),
 m_WeaponAttachType( rhs.m_WeaponAttachType ),
 m_fFloatMoveSpeed( rhs.m_fFloatMoveSpeed ),
 m_bNoColCheck( rhs.m_bNoColCheck )
{
	m_vFloatMoveDir = D3DXVECTOR3( 0.f, 0.f, 0.f );
}

ioFloatBuff::~ioFloatBuff()
{
}

void ioFloatBuff::LoadProperty( ioINILoader &rkLoader )
{
	ioBuff::LoadProperty( rkLoader );

	char szBuf[MAX_PATH] = "";
	m_FloatBuffInfo.m_bStopAction = rkLoader.LoadBool_e( "float_action_stop", false );
	

	m_FloatBuffInfo.m_fFloatJumpPower = rkLoader.LoadFloat_e( "float_jump_power", 0.0f );
	m_FloatBuffInfo.m_fFloatGravityAmt = rkLoader.LoadFloat_e( "float_gravity_amt", 0.0f );
	m_FloatBuffInfo.m_fFloatFrictionAmt = rkLoader.LoadFloat_e( "float_friction_amt", 0.0f );
	m_FloatBuffInfo.m_fFloatBlowRate = rkLoader.LoadFloat_e( "float_blow_rate", FLOAT1 );
	m_FloatBuffInfo.m_fFloatSpeedRate = rkLoader.LoadFloat_e( "float_speed_rate", FLOAT1 );
	m_FloatBuffInfo.m_fFloatEndJumpPower = rkLoader.LoadFloat_e( "float_end_jump_power", 0.1f );

	m_FloatBuffInfo.m_bEnableDamage = rkLoader.LoadBool_e( "enable_damage", false );
	m_FloatBuffInfo.m_bEndJumpState = rkLoader.LoadBool_e( "enable_end_jump_state", false );
	m_FloatBuffInfo.m_bEnableTeamAttack = rkLoader.LoadBool_e( "enable_team_attack", false );

	m_FloatBuffInfo.m_iCancelType = rkLoader.LoadInt_e( "cancel_type", 0 );
	m_FloatBuffInfo.m_iMoveType = rkLoader.LoadInt_e( "move_type", 0 );
	m_FloatBuffInfo.m_iLandType = rkLoader.LoadInt_e( "land_type", 0 );

	m_FloatBuffInfo.m_dwFloatMinTime = rkLoader.LoadInt_e( "float_min_time", 0 );
	m_FloatBuffInfo.m_dwFloatMaxTime = rkLoader.LoadInt_e( "float_max_time", 0 );
	m_fFloatOffHeight = rkLoader.LoadFloat_e( "float_off_height", 0.0f );

	rkLoader.LoadString_e( "float_animation", "", szBuf, MAX_PATH );
	m_FloatAnimation = szBuf;

	rkLoader.LoadString_e( "float_catch_animation", "", szBuf, MAX_PATH );
	m_FloatCatchAnimation = szBuf;

	m_WeaponAttachType = (WeaponAttachType)rkLoader.LoadInt_e( "weapon_attach_type", WAT_NONE );
	m_fFloatMoveSpeed = rkLoader.LoadFloat_e( "float_move_speed", 0.0f );

	m_bNoColCheck = rkLoader.LoadBool_e( "no_col_check", false );
}

ioBuff* ioFloatBuff::Clone()
{
	return new ioFloatBuff( *this );
}

void ioFloatBuff::StartBuff( ioBaseChar *pOwner )
{
	ioBuff::StartBuff( pOwner );

	m_pOwner->SetState( CS_FLOAT_STATE );

	ioEntityGroup *pGrp = m_pOwner->GetGroup();
	if( !m_FloatAnimation.IsEmpty() )
	{
		pGrp->ClearAllLoopAni( 0.0f, true );
		pGrp->ClearAllActionAni( 0.0f, true );

		if( !pOwner->IsPrisonerMode() && !pOwner->IsCatchMode() )
			pGrp->SetLoopAni( m_FloatAnimation, FLOAT100 );
		else
			pGrp->SetLoopAni( m_FloatCatchAnimation, FLOAT100 );
	}

	ioAffectWoundedValueBuff *pBuff = ToAffectWoundedValue( pOwner->GetAffectWoundedValueBuff() );
	if( pBuff )
	{
		if( pBuff->CheckTeamAttack() )
		{
			if( GetOwner() && GetCreator() && GetOwner()->GetTeam() != GetCreator()->GetTeam() )
			{
				m_FloatBuffInfo.m_fFloatJumpPower *= pBuff->GetFloatJumpPowerRate();
				m_FloatBuffInfo.m_fFloatGravityAmt *= pBuff->GetFloatGravityRate();
				m_FloatBuffInfo.m_fFloatFrictionAmt *= pBuff->GetFloatFrictionRate();
			}
		}
		else
		{
			m_FloatBuffInfo.m_fFloatJumpPower *= pBuff->GetFloatJumpPowerRate();
			m_FloatBuffInfo.m_fFloatGravityAmt *= pBuff->GetFloatGravityRate();
			m_FloatBuffInfo.m_fFloatFrictionAmt *= pBuff->GetFloatFrictionRate();
		}
	}

	m_pOwner->SetFloatStateValue( m_FloatBuffInfo );
}

bool ioFloatBuff::ApplyStartBuff( ioBaseChar *pOwner, DWORD dwGlobalTime, DWORD dwGapTime )
{
	if( !ioBuff::ApplyStartBuff( pOwner, dwGlobalTime, dwGapTime ) )
		return false;

	m_pOwner->SetState( CS_FLOAT_STATE );

	ioEntityGroup *pGrp = m_pOwner->GetGroup();
	if( !m_FloatAnimation.IsEmpty() )
	{
		pGrp->ClearAllLoopAni( 0.0f, true );
		pGrp->ClearAllActionAni( 0.0f, true );

		if( !pOwner->IsPrisonerMode() && !pOwner->IsCatchMode() )
			pGrp->SetLoopAni( m_FloatAnimation, FLOAT100 );
		else
			pGrp->SetLoopAni( m_FloatCatchAnimation, FLOAT100 );
	}

	ioAffectWoundedValueBuff *pBuff = ToAffectWoundedValue( pOwner->GetAffectWoundedValueBuff() );
	if( pBuff )
	{
		if( pBuff->CheckTeamAttack() )
		{
			if( GetOwner() && GetCreator() && GetOwner()->GetTeam() != GetCreator()->GetTeam() )
			{
				m_FloatBuffInfo.m_fFloatJumpPower *= pBuff->GetFloatJumpPowerRate();
				m_FloatBuffInfo.m_fFloatGravityAmt *= pBuff->GetFloatGravityRate();
				m_FloatBuffInfo.m_fFloatFrictionAmt *= pBuff->GetFloatFrictionRate();
			}
		}
		else
		{
			m_FloatBuffInfo.m_fFloatJumpPower *= pBuff->GetFloatJumpPowerRate();
			m_FloatBuffInfo.m_fFloatGravityAmt *= pBuff->GetFloatGravityRate();
			m_FloatBuffInfo.m_fFloatFrictionAmt *= pBuff->GetFloatFrictionRate();
		}
	}

	m_pOwner->SetFloatStateValue( m_FloatBuffInfo );

	return true;
}

void ioFloatBuff::ProcessBuff( float fTimePerSec )
{
	if( m_pOwner->GetState() != CS_FLOAT_STATE )
	{
		SetReserveEndBuff();
		return;
	}

	float fGapHeight = m_pOwner->GetWorldPosition().y - m_pOwner->GetBottomHeight();
	float fCurJumpPower = m_pOwner->GetCurJumpPowerAmt();
	float fCurGravityAmt = m_pOwner->GetGravityAmt();

	if( m_FloatBuffInfo.m_iLandType == LET_NORMAL && !m_bNoColCheck &&
		fCurJumpPower <= fCurGravityAmt && fGapHeight <= m_fFloatOffHeight )
	{
		SetReserveEndBuff();
		return;
	}

	if( !CheckWeaponAttach() )
	{
		SetReserveEndBuff();
		return;
	}
	else if( !CheckAreaWeaponAttach() )
	{
		SetReserveEndBuff();
		return;
	}

	switch( m_iOperationType )
	{
	case OT_SWITCH:
	case OT_DELAY:
	case OT_ACTIVE:
		ioBuff::ProcessBuff( fTimePerSec );
		break;
	case OT_PASSIVE:
		CheckCallingAreaWeapon();
		break;
	}

	if( m_fFloatMoveSpeed != 0 )
	{
		D3DXVECTOR3 vCurPos = m_pOwner->GetWorldPosition();
		float fTimePerSec = g_FrameTimer.GetSecPerFrame();
		float fCurMoveSpeed = m_fFloatMoveSpeed * fTimePerSec;

		D3DXVECTOR3 vDir = m_vFloatMoveDir;
		if( vDir == D3DXVECTOR3( 0.f, 0.f, 0.f ) )
			vDir = m_pOwner->GetMoveDir();

		D3DXVECTOR3 vMoveAmt = vDir * fCurMoveSpeed;
		D3DXVECTOR3 vMovePos = vCurPos + vMoveAmt;

		m_pOwner->SetWorldPosition( vMovePos );
	}
}

bool ioFloatBuff::CheckWeaponAttach()
{
	if( !m_pOwner ) return true;

	if( (m_WeaponAttachType != WAT_XZ) && (m_WeaponAttachType != WAT_ALL) )
		return true;

	if( m_dwCreateWeaponIndex == 0 )
		return true;

	ioBaseChar *pCreator = m_pOwner->GetBaseChar( m_CreateChar );
	if( !pCreator )
		return false;

	ioWeapon *pWeapon = g_WeaponMgr.FindWeapon( pCreator, m_dwCreateWeaponIndex );
	if( !pWeapon )
		return false;

	if( !pWeapon->IsLive() )
		return false;

	ioPlayStage *pStage = g_ItemMaker.GetPlayStage();
	if( !pStage )
		return false;

	switch( m_WeaponAttachType )
	{
	case WAT_XZ:
		{
			D3DXVECTOR3 vPos = m_pOwner->GetWorldPosition();
			D3DXVECTOR3 vNewPos = vPos;
			vNewPos.x = pWeapon->GetPosition().x;
			vNewPos.z = pWeapon->GetPosition().z;

			D3DXVECTOR3 vMove = vNewPos - vPos;

			bool bCol = false;
			if( pStage->ReCalculateMoveVectorByTerrain( m_pOwner, &vMove, bCol ) )
			{
				m_pOwner->CheckAxisLimit( &vNewPos );
				m_pOwner->SetWorldPosition( vNewPos );
			}

			if( bCol && !m_bNoColCheck )
			{
				SetReserveEndBuff();
			}
		}
		return true;
	case WAT_ALL:
		{
			D3DXVECTOR3 vPos = m_pOwner->GetMidPositionByRate();
			D3DXVECTOR3 vNewPos = pWeapon->GetPosition();
			D3DXVECTOR3 vMove = vNewPos - vPos;

			bool bCol = false;
			if( pStage->ReCalculateMoveVectorByTerrain( m_pOwner, &vMove, bCol ) )
			{
				m_pOwner->Translate( vMove );

				vPos = m_pOwner->GetWorldPosition();

				if( m_pOwner->CheckAxisLimit( &vNewPos ) )
					m_pOwner->SetWorldPosition( vPos );
			}

			if( bCol && !m_bNoColCheck )
			{
				SetReserveEndBuff();
			}
		}
		return true;
	}

	return false;
}

bool ioFloatBuff::CheckAreaWeaponAttach()
{
	if( !m_pOwner ) return true;

	if( m_WeaponAttachType != WAT_AREA_XZ )
		return true;

	if( m_dwCreateAreawaponIndex == 0 )
		return true;

	ioBaseChar *pCreator = m_pOwner->GetBaseChar( m_CreateChar );
	if( !pCreator )
		return false;

	ioAreaWeapon *pAreaWeapon = g_AreaWeaponMgr.FindAreaWeaponByIndex( m_dwCreateAreawaponIndex );
	if( !pAreaWeapon || !pAreaWeapon->IsLive() )
		return false;

	ioPlayStage *pStage = g_ItemMaker.GetPlayStage();
	if( !pStage )
		return false;

	D3DXVECTOR3 vPos = m_pOwner->GetWorldPosition();
	switch( m_WeaponAttachType )
	{
	case WAT_AREA_XZ:
		{
			D3DXVECTOR3 vNewPos = vPos;
			vNewPos.x = pAreaWeapon->GetStartPos().x;
			vNewPos.z = pAreaWeapon->GetStartPos().z;

			D3DXVECTOR3 vMove = vNewPos - vPos;

			bool bCol = false;
			if( pStage->ReCalculateMoveVectorByTerrain( m_pOwner, &vMove, bCol ) )
			{
				m_pOwner->CheckAxisLimit( &vNewPos );
				m_pOwner->SetWorldPosition( vNewPos );
			}

			if( bCol && !m_bNoColCheck )
			{
				SetReserveEndBuff();
			}
		}
		return true;
	}

	return false;
}

void ioFloatBuff::EndBuff()
{
	CheckWeaponAttachEnd();

	if( m_pOwner->GetState() == CS_FLOAT_STATE )
	{
		if( m_FloatBuffInfo.m_bEndJumpState )
		{
			m_pOwner->SetCurMoveSpeed( 0.0f );
			m_pOwner->SetForcePowerAmt( 0.0f );
			m_pOwner->SetExtraMoveSpeed( 0.0f );

			// HARDCODE : 동기화 위해서 0.1f 부여
			m_pOwner->SetSKillEndJumpState( m_FloatBuffInfo.m_fFloatEndJumpPower, false, true );
		}
		else
		{
			BlowWoundAniInfo kInfo;
			m_pOwner->SetBlowWoundedState( true, kInfo );
		}
	}

	ioBuff::EndBuff();
}

void ioFloatBuff::CheckWeaponAttachEnd()
{
	if( !m_pOwner ) return;

	if( (m_WeaponAttachType != WAT_XZ) && (m_WeaponAttachType != WAT_ALL) )
		return;

	if( m_dwCreateWeaponIndex == 0 )
		return;

	ioBaseChar *pCreator = m_pOwner->GetBaseChar( m_CreateChar );
	if( !pCreator )
		return;

	ioWeapon *pWeapon = g_WeaponMgr.FindWeapon( pCreator, m_dwCreateWeaponIndex );
	if( !pWeapon )
		return;

	if( !pWeapon->IsLive() )
		return;

	ioUroborusWeapon2 *pUroborusWeapon2 = ToUroborusWeapon2( pWeapon );
	if( pUroborusWeapon2 )
		pUroborusWeapon2->SetEndState( true, false, false );
}

// Float Mode Event Buff
ioFloatModeEventBuff::ioFloatModeEventBuff()
{
}

ioFloatModeEventBuff::ioFloatModeEventBuff( const ioFloatModeEventBuff &rhs ) : ioFloatBuff( rhs )
{
}

ioFloatModeEventBuff::~ioFloatModeEventBuff()
{
}

ioBuff* ioFloatModeEventBuff::Clone()
{
	return new ioFloatModeEventBuff( *this );
}

void ioFloatModeEventBuff::EndBuff()
{
	ioBuff::EndBuff();

	if( GetOwner() )
		GetOwner()->FloatModeEventBuffEnd();
}
/////////////////////////////////////////////////////////////////////////////////////////////////////
// JumpPowerRateBuff
ioJumpPowerRateBuff::ioJumpPowerRateBuff()
{
}

ioJumpPowerRateBuff::ioJumpPowerRateBuff( const ioJumpPowerRateBuff &rhs )
: ioBuff( rhs ),
  m_fJumpPowerRate( rhs.m_fJumpPowerRate )
{
}

ioJumpPowerRateBuff::~ioJumpPowerRateBuff()
{
}

ioBuff* ioJumpPowerRateBuff::Clone()
{
	return new ioJumpPowerRateBuff( *this );
}

void ioJumpPowerRateBuff::LoadProperty( ioINILoader &rkLoader )
{
	ioBuff::LoadProperty( rkLoader );

	m_fJumpPowerRate = rkLoader.LoadFloat_e( "jump_power_rate", 0.0f );
}

void ioJumpPowerRateBuff::StartBuff( ioBaseChar *pOwner )
{
	ioBuff::StartBuff( pOwner );

	pOwner->SetJumpPowerRateByBuff( m_fJumpPowerRate );
}

bool ioJumpPowerRateBuff::ApplyStartBuff( ioBaseChar *pOwner, DWORD dwGlobalTime, DWORD dwGapTime )
{
	if( !ioBuff::ApplyStartBuff( pOwner, dwGlobalTime, dwGapTime ) )
		return false;

	pOwner->SetJumpPowerRateByBuff( m_fJumpPowerRate );

	return true;
}

void ioJumpPowerRateBuff::ProcessBuff( float fTimePerSec )
{
	switch( m_iOperationType )
	{
	case OT_SWITCH:
		break;
	case OT_DELAY:
	case OT_ACTIVE:
		ioBuff::ProcessBuff( fTimePerSec );
		break;
	case OT_PASSIVE:
		CheckCallingAreaWeapon();
		break;
	}
}

void ioJumpPowerRateBuff::EndBuff()
{
	m_pOwner->SetJumpPowerRateByBuff( FLOAT1 );

	ioBuff::EndBuff();
}
//////////////////////////////////////////////////////////////////////////////////////////////////////
// LandingRate Buff
ioLandingRateBuff::ioLandingRateBuff()
{
}

ioLandingRateBuff::ioLandingRateBuff( const ioLandingRateBuff &rhs )
: ioBuff( rhs ),
m_fLandingRate( rhs.m_fLandingRate ),
m_fExtendLandingRate( rhs.m_fExtendLandingRate )
{
}

ioLandingRateBuff::~ioLandingRateBuff()
{
}

ioBuff* ioLandingRateBuff::Clone()
{
	return new ioLandingRateBuff( *this );
}

void ioLandingRateBuff::LoadProperty( ioINILoader &rkLoader )
{
	ioBuff::LoadProperty( rkLoader );

	m_fLandingRate = rkLoader.LoadFloat_e( "landing_rate", FLOAT1 );
	m_fExtendLandingRate = rkLoader.LoadFloat_e( "extend_landing_rate", FLOAT1 );
}

void ioLandingRateBuff::StartBuff( ioBaseChar *pOwner )
{
	ioBuff::StartBuff( pOwner );

	pOwner->SetLandingRateByBuff( m_fLandingRate, m_fExtendLandingRate );
}

bool ioLandingRateBuff::ApplyStartBuff( ioBaseChar *pOwner, DWORD dwGlobalTime, DWORD dwGapTime )
{
	if( !ioBuff::ApplyStartBuff( pOwner, dwGlobalTime, dwGapTime ) )
		return false;

	pOwner->SetLandingRateByBuff( m_fLandingRate, m_fExtendLandingRate );

	return true;
}

void ioLandingRateBuff::ProcessBuff( float fTimePerSec )
{
	switch( m_iOperationType )
	{
	case OT_SWITCH:
		break;
	case OT_DELAY:
	case OT_ACTIVE:
		ioBuff::ProcessBuff( fTimePerSec );
		break;
	case OT_PASSIVE:
		CheckCallingAreaWeapon();
		break;
	}
}

void ioLandingRateBuff::EndBuff()
{
	m_pOwner->SetLandingRateByBuff( FLOAT1, FLOAT1 );

	ioBuff::EndBuff();
}
//////////////////////////////////////////////////////////////////////////////////////////////////////
// ScreenBlind Buff
ioScreenBlindBuff::ioScreenBlindBuff()
{
}

ioScreenBlindBuff::ioScreenBlindBuff( const ioScreenBlindBuff &rhs )
: ioBuff( rhs ),
 m_BlindTime( rhs.m_BlindTime ),
 m_szBlindName( rhs.m_szBlindName ),
 m_iMaxAlpha( rhs.m_iMaxAlpha )
{
}

ioScreenBlindBuff::~ioScreenBlindBuff()
{
}

ioBuff* ioScreenBlindBuff::Clone()
{
	return new ioScreenBlindBuff( *this );
}

void ioScreenBlindBuff::LoadProperty( ioINILoader &rkLoader )
{
	ioBuff::LoadProperty( rkLoader );

	char szBuf[MAX_PATH] = "";

	rkLoader.LoadString_e( "blind_name", "", szBuf, MAX_PATH );
	m_szBlindName = szBuf;

	m_iMaxAlpha = rkLoader.LoadInt_e( "blind_max_alpha", 0 );

	m_BlindTime.m_dwUpTime     = rkLoader.LoadInt_e( "blind_up_time", 0 );
	m_BlindTime.m_dwCenterTime = max( 1, rkLoader.LoadInt_e( "blind_center_time", 1 ) );
	m_BlindTime.m_dwDownTime   = max( 1, rkLoader.LoadInt_e( "blind_down_time", 1 ) );
}

void ioScreenBlindBuff::StartBuff( ioBaseChar *pOwner )
{
	ioBuff::StartBuff( pOwner );

	pOwner->StartBuffBlind( m_szBlindName, &m_BlindTime, m_iMaxAlpha );

	SetBuffDuration( 0 );
}

bool ioScreenBlindBuff::ApplyStartBuff( ioBaseChar *pOwner, DWORD dwGlobalTime, DWORD dwGapTime )
{
	if( !ioBuff::ApplyStartBuff( pOwner, dwGlobalTime, dwGapTime ) )
		return false;

	pOwner->StartBuffBlind( m_szBlindName, &m_BlindTime, m_iMaxAlpha );

	SetBuffDuration( 0 );
	return true;
}

void ioScreenBlindBuff::EndBuff()
{
	ioBuff::EndBuff();

	ioBaseChar *pOwner = GetOwner();
	if( pOwner )
		pOwner->EndBuffBlind();
}

void ioScreenBlindBuff::ProcessBuff( float fTimePerSec )
{
	ioBaseChar *pOwner = GetOwner();
	if( !pOwner )
	{
		SetReserveEndBuff();
		return;
	}

	ioBuff::ProcessBuff( fTimePerSec );
}
//////////////////////////////////////////////////////////////////////////////////////////////////////
// ioPushBlowEnhance Buff
ioPushBlowEnhanceBuff::ioPushBlowEnhanceBuff()
{
	m_dwPushID = 0;
	m_dwBlowID = 0;
	m_dwAirPushID = 0;
	m_dwAirBlowID = 0;
}

ioPushBlowEnhanceBuff::ioPushBlowEnhanceBuff( const ioPushBlowEnhanceBuff &rhs )
: ioBuff( rhs ),
 m_fPushRate( rhs.m_fPushRate ),
 m_fAirPushRate( rhs.m_fAirPushRate ),
 m_fBlowRate( rhs.m_fBlowRate ),
 m_fAirBlowRate( rhs.m_fAirBlowRate ),
 m_bDisableDownState( rhs.m_bDisableDownState ),
 m_bWoundEnhance( rhs.m_bWoundEnhance )
{
	m_dwPushID = 0;
	m_dwBlowID = 0;
	m_dwAirPushID = 0;
	m_dwAirBlowID = 0;
}

ioPushBlowEnhanceBuff::~ioPushBlowEnhanceBuff()
{
}

ioBuff* ioPushBlowEnhanceBuff::Clone()
{
	return new ioPushBlowEnhanceBuff( *this );
}

void ioPushBlowEnhanceBuff::LoadProperty( ioINILoader &rkLoader )
{
	ioBuff::LoadProperty( rkLoader );

	m_bWoundEnhance = rkLoader.LoadBool_e( "wound_enhance", false );

	m_fPushRate = rkLoader.LoadFloat_e( "push_rate", FLOAT1 );
	m_fAirPushRate = rkLoader.LoadFloat_e( "air_push_rate", FLOAT1 );
	m_fBlowRate = rkLoader.LoadFloat_e( "blow_rate", FLOAT1 );
	m_fAirBlowRate = rkLoader.LoadFloat_e( "air_blow_rate", FLOAT1 );
	m_bDisableDownState  = rkLoader.LoadBool_e( "disable_down_state", false );
}

void ioPushBlowEnhanceBuff::StartBuff( ioBaseChar *pOwner )
{
	ioBuff::StartBuff( pOwner );

	m_dwPushID = pOwner->AddPushPowerEnhanceRate( EOT_PUSH, m_fPushRate, m_bWoundEnhance );
	m_dwBlowID = pOwner->AddPushPowerEnhanceRate( EOT_BLOW, m_fBlowRate, m_bWoundEnhance );
	m_dwAirPushID = pOwner->AddPushPowerEnhanceRate( EOT_AIR_PUSH, m_fAirPushRate, m_bWoundEnhance );
	m_dwAirBlowID = pOwner->AddPushPowerEnhanceRate( EOT_AIR_BLOW, m_fAirBlowRate, m_bWoundEnhance );
}

bool ioPushBlowEnhanceBuff::ApplyStartBuff( ioBaseChar *pOwner, DWORD dwGlobalTime, DWORD dwGapTime )
{
	if( !ioBuff::ApplyStartBuff( pOwner, dwGlobalTime, dwGapTime ) )
		return false;

	m_dwPushID = pOwner->AddPushPowerEnhanceRate( EOT_PUSH, m_fPushRate, m_bWoundEnhance );
	m_dwBlowID = pOwner->AddPushPowerEnhanceRate( EOT_BLOW, m_fBlowRate, m_bWoundEnhance );
	m_dwAirPushID = pOwner->AddPushPowerEnhanceRate( EOT_AIR_PUSH, m_fAirPushRate, m_bWoundEnhance );
	m_dwAirBlowID = pOwner->AddPushPowerEnhanceRate( EOT_AIR_BLOW, m_fAirBlowRate, m_bWoundEnhance );

	return true;
}

void ioPushBlowEnhanceBuff::ProcessBuff( float fTimePerSec )
{
	CheckOwnerStateCheck();

	switch( m_iOperationType )
	{
	case OT_DELAY:
	case OT_SWITCH:
	case OT_ACTIVE:
		ioBuff::ProcessBuff( fTimePerSec );
	case OT_PASSIVE:
		CheckCallingAreaWeapon();
		break;
	}
}

void ioPushBlowEnhanceBuff::EndBuff()
{
	m_pOwner->RemovePushPowerEnhanceRate( EOT_PUSH, m_dwPushID, m_bWoundEnhance );
	m_pOwner->RemovePushPowerEnhanceRate( EOT_BLOW, m_dwBlowID, m_bWoundEnhance );
	m_pOwner->RemovePushPowerEnhanceRate( EOT_AIR_PUSH, m_dwAirPushID, m_bWoundEnhance );
	m_pOwner->RemovePushPowerEnhanceRate( EOT_AIR_BLOW, m_dwAirBlowID, m_bWoundEnhance );

	ioBuff::EndBuff();
}


void ioPushBlowEnhanceBuff::CheckOwnerStateCheck()
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