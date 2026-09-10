

#include "stdafx.h"

#include "ioPlayStage.h"
#include "ioBaseChar.h"

#include "iolupinItem.h"

#include "FindPredicateImpl.h"

#include "WeaponDefine.h"
#include "ItemDefine.h"
#include "BuffDefine.h"
#include "DummyCharDefine.h"
#include "ioFireSkill.h"

iolupinItem::iolupinItem()
{
	m_ChargeState = CS_NONE;
	m_BuffState = BS_NONE;
	m_SpecialState = SS_NONE;
	m_dwAttackStartTime = 0;
	m_dwMotionEndTime = 0;
	m_iCurCombo = 0;

	m_iCurAirJump = 0;
	m_fAirJumpPowerRate = 0.0f;

	m_bCharged = false;

	m_bSpecialLupinAttackState = false;
	m_bBlitzBack = false;
	m_iCurLupinCombo = 0;

	m_fCurChangeGauge = 0.0f;
	m_iCurBullet	= 0;
	m_fTempBullet	= 0.0f;

	m_dwPreSpecialMoveStartTime = 0;

	m_fSpecialAttackJumpPower = 0.0f;
	m_fSpecialAttackGravityRate = 0.0f;

	m_dwPreActionEndTime = 0;

	m_iWeaponWay = 0;

	m_fScatterAngle = 0.0f;
	m_iMaxWeaponWay = 1;

	m_dwLoopDurationStart = 0;
	m_dwLoopDurationEnd = 0;

	m_dwCurMapEffect = -1;
	m_dwWeaponBaseIndex = 0;

	m_dwSpecialSecAttDurationEnd = 0;
	m_dwFireMotionEndTime = 0;

	m_vTargetPos = ioMath::VEC3_ZERO;

	m_dwEnableHideTime = 0;

	m_dwSpecialFirstAttackMoveTime = 0;

	m_vMagicCirclePos = ioMath::VEC3_ZERO;

	m_pTargetChar = NULL;
}

iolupinItem::iolupinItem( const iolupinItem &rhs )
: ioWeaponItem( rhs ),
  m_AttackReadyAniList( rhs.m_AttackReadyAniList ),
  m_vExtendAttributeList( rhs.m_vExtendAttributeList ),
  m_LupinAttribute( rhs.m_LupinAttribute ),
  m_SpecialFirstAttribute( rhs.m_SpecialFirstAttribute ),
  m_vLupinList( rhs.m_vLupinList ),
  m_fBlitzRange( rhs.m_fBlitzRange ),
  m_fBlitzMaxAngle( rhs.m_fBlitzMaxAngle ),
  m_fBlitzMinAngle( rhs.m_fBlitzMinAngle ),
  m_fMaxChangeGauge( rhs.m_fMaxChangeGauge ),
  m_fIncreaseChangeGauge( rhs.m_fIncreaseChangeGauge ),
  m_fDecreaseChangeGauge( rhs.m_fDecreaseChangeGauge ),
  m_fBulletIncreaseRate( rhs.m_fBulletIncreaseRate ),
  m_iCurBullet( rhs.m_iCurBullet ), 
  m_iMaxBullet( rhs.m_iMaxBullet ),
  m_vNeedBullet( rhs.m_vNeedBullet ),
  m_SpecialLoopMotion( rhs.m_SpecialLoopMotion ),
  m_fSpecialLoopMotionRate( rhs.m_fSpecialLoopMotionRate ),
  m_dwFixedTimeInAir( rhs.m_dwFixedTimeInAir ),
  m_dwPreDelayTime( rhs.m_dwPreDelayTime ),
  m_bPreDelayProtect( rhs.m_bPreDelayProtect ),
  m_PreDelayAnimation( rhs.m_PreDelayAnimation ),
  m_SpecialAttackMotion( rhs.m_SpecialAttackMotion ),
  m_dwAnimationEndDelay( rhs.m_dwAnimationEndDelay ),
  m_fSpecialAttackAniTimeRate( rhs.m_fSpecialAttackAniTimeRate ),
  m_fSpecialAttackJumpPower( rhs.m_fSpecialAttackJumpPower ),
  m_fSpecialAttackGravityRate( rhs.m_fSpecialAttackGravityRate ),
  m_fScatterAngle( rhs.m_fScatterAngle ),
  m_iMaxWeaponWay( rhs.m_iMaxWeaponWay ),
  m_fCircleRange( rhs.m_fCircleRange ),
  m_fCircleMoveSpeed( rhs.m_fCircleMoveSpeed ),
  m_fCircleHeightGap( rhs.m_fCircleHeightGap ),
  m_fCircleOffSet( rhs.m_fCircleOffSet ),
  m_ReadyCircle( rhs.m_ReadyCircle ),
  m_RedEffect( rhs.m_RedEffect ),
  m_BlueEffect( rhs.m_BlueEffect ),
  m_SpecialSecondAttribute( rhs.m_SpecialSecondAttribute ),
  m_fActionJumpPower( rhs.m_fActionJumpPower ),
  m_fActionJumpMaxSpeed( rhs.m_fActionJumpMaxSpeed ),
  m_vWeaponOffset( rhs.m_vWeaponOffset ),
  m_vSummonDummyInfo( rhs.m_vSummonDummyInfo ),
  m_dwHideCoolTime( rhs.m_dwHideCoolTime ),
  m_StartBuffByWeaponSkill( rhs.m_StartBuffByWeaponSkill )
{
	m_ChargeState				= CS_NONE;
	m_BuffState					= BS_NONE;
	m_SpecialState				= SS_NONE;
	m_dwAttackStartTime			= 0;
	m_dwMotionEndTime			= 0;
	m_iCurCombo					= 0;

	m_iCurAirJump				= 0;
	m_fAirJumpPowerRate			= 0.0f;

	m_bCharged					= false;

	m_bSpecialLupinAttackState	= false;
	m_bBlitzBack				= false;
	m_iCurLupinCombo			= 0;

	m_fCurChangeGauge			= 0.0f;
	m_iCurBullet				= 0;
	m_fTempBullet				= 0.0f;

	m_dwPreSpecialMoveStartTime = 0;

	m_dwPreActionEndTime		= 0;

	m_iWeaponWay = 0;

	m_dwLoopDurationStart = 0;
	m_dwLoopDurationEnd = 0;

	m_dwCurMapEffect = -1;
	m_dwWeaponBaseIndex = 0;

	m_dwSpecialSecAttDurationEnd = 0;
	m_dwFireMotionEndTime = 0;

	m_vTargetPos = ioMath::VEC3_ZERO;

	m_dwEnableHideTime = m_dwHideCoolTime + FRAMEGETTIME();

	m_dwSpecialFirstAttackMoveTime = 0;

	m_vMagicCirclePos = ioMath::VEC3_ZERO;

	m_pTargetChar = NULL;
}

iolupinItem::~iolupinItem()
{
	m_vExtendAttributeList.clear();
	m_vLupinList.clear();

	m_SpecialFirstAttribute.clear();
}

void iolupinItem::LoadProperty( ioINILoader &rkLoader )
{
	ioWeaponItem::LoadProperty( rkLoader );

	int i;
	char szBuf[MAX_PATH];
	char szKey[MAX_PATH];

	int iExtendCnt = rkLoader.LoadInt_e( "extend_attack_cnt", 0 );
	m_vExtendAttributeList.clear();
	m_vExtendAttributeList.reserve( iExtendCnt );
	for( i=0 ; i<iExtendCnt ; i++ )
	{
		AttackAttribute kAttribute;
		wsprintf_e( szBuf, "extend_attack0%d", i+1 );
		LoadAttackAttribute( szBuf, kAttribute, rkLoader );
		m_vExtendAttributeList.push_back( kAttribute );
	}

	m_AttackReadyAniList.clear();
	for( i=0 ; i<iExtendCnt ; i++ )
	{
		wsprintf_e( szBuf, "attack_ready_animation%d", i+1 );
		rkLoader.LoadString( szBuf, "", szBuf, MAX_PATH );
		m_AttackReadyAniList.push_back( ioHashString(szBuf) );
	}

	//
	LoadAttackAttribute_e( "buff_add_attack", m_LupinAttribute, rkLoader );

	m_vLupinList.clear();
	int iBuffCnt = rkLoader.LoadInt_e( "buff_add_cnt", 0 );
	for( i=0; i < iBuffCnt; ++i )
	{
		wsprintf_e( szKey, "buff_add%d", i+1 );
		rkLoader.LoadString( szKey, "", szBuf, MAX_PATH );

		ioHashString szBuffName = szBuf;
		m_vLupinList.push_back( szBuffName );
	}

	m_dwHideCoolTime = rkLoader.LoadInt_e( "hide_cool_time", 0 );

	// 위장 상태에서 D꾹하면 점프 후 사방으로 선물 폭턴 투척 웨폰
	int iSpecialFirstCnt = rkLoader.LoadInt_e( "special_first_attack_max", 0 );
	m_SpecialFirstAttribute.clear();
	m_SpecialFirstAttribute.reserve( iSpecialFirstCnt );
	for( i=0; i < iSpecialFirstCnt; i++ )
	{
		AttackAttribute kAttribute;
		wsprintf_e( szKey, "special_first_attack%d", i+1 );
		LoadAttackAttribute( szKey, kAttribute, rkLoader );
		m_SpecialFirstAttribute.push_back( kAttribute );
	}

	m_fBlitzRange = rkLoader.LoadFloat_e( "blitz_target_range", 0.0f );
	m_fBlitzMaxAngle = rkLoader.LoadFloat_e( "blitz_target_max_angle", 0.0f );
	m_fBlitzMinAngle = rkLoader.LoadFloat_e( "blitz_target_min_angle", 0.0f );

	m_fMaxChangeGauge = rkLoader.LoadFloat_e( "max_change_gauge", 0.0f );
	m_fIncreaseChangeGauge = rkLoader.LoadFloat_e( "increase_change_gauge", 0.0f );
	m_fDecreaseChangeGauge = rkLoader.LoadFloat_e( "decrease_change_gauge", 0.0f );

	m_iMaxBullet		= rkLoader.LoadInt_e( "max_bullet", 0 );
	m_iNeedBulletCnt	= rkLoader.LoadInt_e( "need_bullet_cnt", 0 );
	for ( int iNeedBulletCnt = 0; iNeedBulletCnt < m_iNeedBulletCnt; iNeedBulletCnt++ )
	{
		wsprintf_e( szKey, "need_bullet%d", iNeedBulletCnt + 1 );
		int iNeedBullet = rkLoader.LoadInt( szKey, 0 );
		m_vNeedBullet.push_back( iNeedBullet );
	}
	
	m_fBulletIncreaseRate	= rkLoader.LoadFloat_e( "bullet_increase_rate", FLOAT1 );

	// 특별 동작 관련
	m_bPreDelayProtect  = rkLoader.LoadBool_e( "pre_delay_protect", false );
	rkLoader.LoadString_e( "pre_delay_animation", "", szBuf, MAX_PATH );
	m_PreDelayAnimation = szBuf;

	m_dwPreDelayTime = rkLoader.LoadInt_e( "pre_delay_time", 0 );
	m_dwAnimationEndDelay = rkLoader.LoadInt_e( "animation_end_delay", 0 );

	rkLoader.LoadString_e( "special_attack_motion", "", szBuf, MAX_PATH );
	m_SpecialAttackMotion = szBuf;

	m_fSpecialAttackAniTimeRate = rkLoader.LoadFloat_e( "special_attack_animation_time_rate", FLOAT1 );

	m_fSpecialAttackJumpPower = rkLoader.LoadFloat_e( "special_char_jump_power", 0.0f );

	m_fSpecialAttackGravityRate = rkLoader.LoadFloat_e( "skill_gravity_rate", FLOAT1 );

	m_fScatterAngle = rkLoader.LoadFloat_e( "scatter_angle",FLOAT90 );
	m_iMaxWeaponWay = rkLoader.LoadInt_e( "max_bomb_cnt", 1 );

	rkLoader.LoadString_e( "special_loop_motion", "", szBuf, MAX_PATH );
	m_SpecialLoopMotion = szBuf;

	m_fSpecialLoopMotionRate = rkLoader.LoadFloat_e( "special_loop_motion_rate", FLOAT1 );
	m_dwFixedTimeInAir = rkLoader.LoadInt_e( "air_fixed_time", 0 );

	m_fCircleRange = rkLoader.LoadFloat_e( "magic_circle_range", 0.0f );
	m_fCircleMoveSpeed = rkLoader.LoadFloat_e( "magic_circle_move_speed", 0.0f );
	m_fCircleHeightGap = rkLoader.LoadFloat_e( "magic_circle_height_gap", 0.0f );
	m_fCircleOffSet = rkLoader.LoadFloat_e( "magic_circle_offset", 0.0f );

	rkLoader.LoadString_e( "ready_circle", "", szBuf, MAX_PATH );
	m_ReadyCircle = szBuf;

	rkLoader.LoadString_e( "red_team_circle", "", szBuf, MAX_PATH );
	m_RedEffect = szBuf;
	rkLoader.LoadString_e( "blue_team_circle", "", szBuf, MAX_PATH );
	m_BlueEffect = szBuf;

	wsprintf_e( szBuf, "special_second_attack");	
	LoadAttackAttribute( szBuf, m_SpecialSecondAttribute, rkLoader );	

	m_fActionJumpPower = rkLoader.LoadFloat_e( "action_jump_power", 0.0f );
	m_fActionJumpMaxSpeed = rkLoader.LoadFloat_e( "action_jump_max_speed", 0.0f );

	// 기본 공격
	m_vWeaponOffset.x = rkLoader.LoadFloat_e( "weapon_offset_x", 0.0f );
	m_vWeaponOffset.y = rkLoader.LoadFloat_e( "weapon_offset_y", 0.0f );
	m_vWeaponOffset.z = rkLoader.LoadFloat_e( "weapon_offset_z", 0.0f );

	// 기본 공격 더미
	int iDummyCharCnt = rkLoader.LoadInt_e( "dummy_char_cnt", 0 );
	for( int iDummyCharIdx = 0; iDummyCharIdx < iDummyCharCnt; iDummyCharIdx++ )
	{
		SummonDummyInfo dummyInfo;
		wsprintf_e( szKey, "dummy%d_char_name", iDummyCharIdx + 1 );
		rkLoader.LoadString( szKey, "", szBuf, MAX_PATH );
		dummyInfo.m_DummyCharName = szBuf;

		dummyInfo.m_vDummyAttCode.clear();
		wsprintf_e( szKey, "dummy%d_attack_cnt", iDummyCharIdx + 1 );
		int iDummyAttackCnt = rkLoader.LoadInt( szKey, 0 );
		for( int iDummyAttackIdx = 0; iDummyAttackIdx < iDummyAttackCnt; iDummyAttackIdx++ )
		{
			wsprintf_e( szKey, "dummy%d_attack%d_code", iDummyCharIdx + 1, iDummyAttackIdx + 1 );
			int iDummyAttackCode = rkLoader.LoadInt( szKey, 0 );
			dummyInfo.m_vDummyAttCode.push_back( iDummyAttackCode );
		}

		wsprintf_e( szKey, "dummy%d_char_offset_x", iDummyCharIdx + 1 );
		dummyInfo.m_vDummyCharOffset.x = rkLoader.LoadFloat( szKey, 0.0f );
		wsprintf_e( szKey, "dummy%d_char_offset_y", iDummyCharIdx + 1 );
		dummyInfo.m_vDummyCharOffset.y = rkLoader.LoadFloat( szKey, 0.0f );
		wsprintf_e( szKey, "dummy%d_char_offset_z", iDummyCharIdx + 1 );
		dummyInfo.m_vDummyCharOffset.z = rkLoader.LoadFloat( szKey, 0.0f );

		wsprintf_e( szKey, "destroy_dummy%d_cnt", iDummyCharIdx + 1 );
		int iCnt = rkLoader.LoadInt( szKey, 0 );
		dummyInfo.m_vDestroyDummyCharList.clear();
		for( int i=0; i < iCnt; ++i )
		{
			wsprintf_e( szKey, "destroy_dummy%d_%d_name", iDummyCharIdx + 1, i+1 );

			rkLoader.LoadString( szKey, "", szBuf, MAX_PATH );

			dummyInfo.m_vDestroyDummyCharList.push_back( szBuf );
		}

		wsprintf_e( szKey, "dummy%d_move_cnt", iDummyCharIdx + 1 );
		iCnt = rkLoader.LoadInt( szKey, 0 );
		for( int i = 0; i < iCnt; ++i )
		{
			StTopMoveInfo info;
			wsprintf( szKey, "dummy%d_rotate_x%d", iDummyCharIdx + 1, i+1 );
			info.fRotate_x = rkLoader.LoadFloat( szKey, 0.f );

			wsprintf( szKey, "dummy%d_rotate_y%d", iDummyCharIdx + 1, i+1 );
			info.fRotate_y = rkLoader.LoadFloat( szKey, 0.f );

			wsprintf( szKey, "dummy%d_move_length%d", iDummyCharIdx + 1, i+1 );
			info.fLength = rkLoader.LoadFloat( szKey, 0.f );

			wsprintf( szKey, "dummy%d_create%d", iDummyCharIdx + 1, i+1 );
			info.bCreate = rkLoader.LoadBool( szKey, false );

			dummyInfo.m_vDummyMoveInfo.push_back( info );
		}

		m_vSummonDummyInfo.push_back( dummyInfo );
	}
	
	rkLoader.LoadString_e( "skill_buff", "", szBuf, MAX_PATH );
	m_StartBuffByWeaponSkill = szBuf;
}

ioItem* iolupinItem::Clone()
{
	return new iolupinItem( *this );
}

ioWeaponItem::WeaponSubType iolupinItem::GetSubType() const
{
	return WST_LUPIN_ITEM;
}

void iolupinItem::ClearData()
{
	m_ChargeState = CS_NONE;
	m_SpecialState = SS_NONE;
	m_dwAttackStartTime = 0;
	m_dwMotionEndTime = 0;
	m_dwKeyReserveTime = 0;

	m_CurDirKey = ioUserKeyInput::DKI_NONE;
}

void iolupinItem::OnReleased( ioBaseChar *pOwner )
{
	int iBuffCnt = m_vLupinList.size();
	for( int i=0; i < iBuffCnt; ++i )
	{
		pOwner->RemoveBuff( m_vLupinList[i] );
	}

	DestroyLupinDummy( pOwner );

	ioAttackableItem::OnReleased( pOwner );
}

void iolupinItem::SetNormalAttack( ioBaseChar *pOwner, int iCurCombo )
{
	DWORD dwCurTime = FRAMEGETTIME();

	ClearCancelInfo();

	m_bCharged = false;

	int iExtendCnt = m_vExtendAttributeList.size();
	if( !COMPARE( iCurCombo, 0, ACT_GROUND_ATT_2 + 1 ) )
	{
		ioWeaponItem::SetNormalAttack( pOwner, iCurCombo );
		
		m_ChargeState = CS_NONE;
		m_iCurCombo = iCurCombo;
	}
	else
	{
		ioEntityGroup *pGrp = pOwner->GetGroup();
		pGrp->ClearAllLoopAni( FLOAT100, true );
		pGrp->ClearAllActionAni( FLOAT100, true );

		int iAniID = pGrp->GetAnimationIdx( m_AttackReadyAniList[iCurCombo] );

		float fTimeRate = (float)m_dwChargeTime / pGrp->GetAnimationFullTime( iAniID );
		pGrp->SetActionAni( iAniID, FLOAT100, FLOAT100, FLOAT1, FLOAT1/fTimeRate, 0.0f, 0.0f, true );

		m_ChargeState = CS_CHARGING;
		m_dwAttackStartTime = dwCurTime;
		m_iCurCombo = iCurCombo;
		pOwner->SetState( CS_ATTACK );

		DWORD dwTrackingTime = dwCurTime;
		dwTrackingTime += pGrp->GetAnimationEventTime_e( iAniID, "tracking_e" ) * fTimeRate;
		pOwner->SetAutoTargetTrackingEndTime( dwTrackingTime );
	}
}

void iolupinItem::CheckNormalAttack( ioBaseChar *pOwner )
{
	switch( m_ChargeState )
	{
	case CS_CHARGING:
		{
			OnCharging( pOwner );

			if( pOwner->IsNeedProcess() )
			{
				CheckCancelReserveByConditionalSpecialDash( pOwner );

				if( ProcessCancelBySpeicalDash(pOwner) )
					return;

				CheckCancelReserve( pOwner );

				if( ProcessCancel(pOwner) )
					return;
			}
		}
		break;
	case CS_ATTACK_FIRE:
		if( m_bCharged )
			pOwner->CheckExtraAniJump();

		if( pOwner->IsNeedProcess() )
		{
			CheckCancelReserveByConditionalSpecialDash( pOwner );

			if( ProcessCancelBySpeicalDash(pOwner) )
				return;

			CheckCancelReserve( pOwner );

			if( ProcessCancel(pOwner) )
				return;
		}

		ioWeaponItem::CheckNormalAttack( pOwner );
		break;
	case CS_NORMAL_ATTACK:
		if( pOwner->IsNeedProcess() )
		{
			CheckCancelReserveByConditionalSpecialDash( pOwner );

			if( ProcessCancelBySpeicalDash(pOwner) )
				return;

			CheckCancelReserve( pOwner );

			if( ProcessCancel(pOwner) )
				return;
		}

		ioWeaponItem::CheckNormalAttack( pOwner );
		break;
	case CS_SPECIAL_LUPIN_ATTACK:
		{
			// 현재 걸린 은신 해제 
			if( m_BuffState == BS_HIDE || m_BuffState == BS_CAMOUFLAGE )
			{
				ClearLupinState( pOwner, m_BuffState );
			}
		}
		break;
	}
}

void iolupinItem::OnCharging( ioBaseChar *pOwner )
{
	if( !pOwner->IsNeedProcess() )	return;

	if( pOwner->IsAttackKeyDown() )
	{
		DWORD dwTemp = m_dwChargeTime;
		if( m_dwAttackStartTime + m_dwChargeTime <= FRAMEGETTIME() )
		{
			if( m_iCurBullet < m_iMaxBullet && m_BuffState == BS_HIDE )
			{
				ChangeToNormalAttack( pOwner );
			}
			else if( m_iCurBullet >= m_iMaxBullet && m_BuffState == BS_NONE && ( m_dwEnableHideTime > 0 && m_dwEnableHideTime < FRAMEGETTIME() ) )		// 아무 버프도 없는 상태일때 D꾹
			{
				SetLupinState( pOwner, BS_HIDE );
				m_dwEnableHideTime = 0;
			}
			else if( m_BuffState == BS_CAMOUFLAGE && m_SpecialState == SS_NONE )	// 위장 상태일때 D꾹
			{
				if( IsEnoughBullet( SAB_FIRST_ACTION ) )
					StartPreDelayAnimation( pOwner );
				else
					ChangeToNormalAttack( pOwner );
			}
			else
				ChangeToNormalAttack( pOwner );
		}
	}
	else
	{
		ChangeToNormalAttack( pOwner );
	}
}

void iolupinItem::OnBlitzAttack( ioBaseChar *pOwner )
{
	if( !pOwner )
		return;

	pOwner->CheckExtraAniJump();

	if( pOwner->IsNeedProcess() )
	{
		if( pOwner->CheckExtendAttackEndJump() )
			return;

		CheckCancelReserve( pOwner );

		if( ProcessCancel(pOwner) )
			return;
	}

	ioWeaponItem::CheckNormalAttack( pOwner );
}

void iolupinItem::ChangeToNormalAttack( ioBaseChar *pOwner )
{
	if( !pOwner ) return;

	// 특별 상태 공격 단계면 아래 실행되지 않음
	if( m_SpecialState != SS_NONE )
		return;

	DWORD dwCurTime = FRAMEGETTIME();
	m_bCharged = false;

	pOwner->InitExtendAttackTagInfo();

	ClearCancelInfo();

	const AttackAttribute *pAttribute = m_FormalAttack.GetAttribute( AT_NORMAL, m_iCurCombo );
	if( pAttribute )
	{
		pOwner->SetNormalAttackByAttribute( *pAttribute );

		m_szCurAni = pAttribute->m_AttackAnimation;
		m_fCurAniRate = pAttribute->m_fAttackAniRate;

		m_dwInputCancelCheckTime = dwCurTime;
		m_dwCancelCheckTime = dwCurTime;
	}

	pOwner->ApplyNormalAttackResult( m_iCurCombo );

	m_ChargeState = CS_NORMAL_ATTACK;

	// 버프 삭제
	if( m_BuffState == BS_HIDE || m_BuffState == BS_CAMOUFLAGE )
	{
		ClearLupinState( pOwner, m_BuffState );
	}

	if( pOwner->IsNeedSendNetwork() )
	{
		SP2Packet kPacket( CUPK_ITEM_GAUGE_OK );
		FillItemGaugePacketHeader( pOwner, kPacket );
		kPacket << m_ChargeState;
		kPacket << m_iCurCombo;
		kPacket << pOwner->GetTargetRot();
		P2PNetwork::SendToAllPlayingUser( kPacket );
	}

	if( pOwner->IsNeedProcess() )
	{
		CheckCancelReserveByConditionalSpecialDash( pOwner );

		if( ProcessCancelBySpeicalDash(pOwner) )
			return;
	}
}

void iolupinItem::ChangeToAttackFire( ioBaseChar *pOwner )
{
	if( !pOwner ) return;

	DWORD dwCurTime = FRAMEGETTIME();
	m_bCharged = true;

	pOwner->InitExtendAttackTagInfo();

	ClearCancelInfo();

	int iExtendCnt = m_vExtendAttributeList.size();
	if( COMPARE( m_iCurCombo, 0, iExtendCnt ) )
	{
		m_szCurAni = m_vExtendAttributeList[m_iCurCombo].m_AttackAnimation;
		m_fCurAniRate = m_vExtendAttributeList[m_iCurCombo].m_fAttackAniRate;

		m_dwInputCancelCheckTime = dwCurTime;
		m_dwCancelCheckTime = dwCurTime;

		pOwner->SetNormalAttackByAttribute( m_vExtendAttributeList[m_iCurCombo] );

		ioEntityGroup *pGrp = pOwner->GetGroup();
		int iAniID	     = pGrp->GetAnimationIdx( m_vExtendAttributeList[m_iCurCombo].m_AttackAnimation );
		float fTimeRate  = m_vExtendAttributeList[m_iCurCombo].m_fAttackAniRate;
		DWORD dwPreDelay = m_vExtendAttributeList[m_iCurCombo].m_dwPreDelay;

		pOwner->SetBaseAttackExtraAniJump( iAniID, fTimeRate, dwPreDelay );
		pOwner->SetExtendAttackEndJumpInfo( iAniID, fTimeRate, dwPreDelay );
	}

	if( iExtendCnt <= m_iCurCombo + 1 )
		pOwner->ApplyNormalAttackResult( m_iCurCombo, true );
	else
		pOwner->ApplyNormalAttackResult( m_iCurCombo );

	m_ChargeState = CS_ATTACK_FIRE;

	if( pOwner->IsNeedSendNetwork() )
	{
		SP2Packet kPacket( CUPK_ITEM_GAUGE_OK );
		FillItemGaugePacketHeader( pOwner, kPacket );
		kPacket << m_ChargeState;
		kPacket << m_iCurCombo;
		kPacket << pOwner->GetTargetRot();
		P2PNetwork::SendToAllPlayingUser( kPacket );
	}
}

void iolupinItem::SetLupinState( ioBaseChar *pOwner, BuffState eBuffState )
{
	if( !pOwner )
		return;

	// 은신 시작
	if( eBuffState == BS_HIDE )
	{
		// change state & motion
		pOwner->SetWereWolfState( m_LupinAttribute.m_AttackAnimation, m_LupinAttribute.m_fAttackAniRate );

		pOwner->SetCurJumpPowerAmt( 0.0f );
		pOwner->SetForcePowerAmt( 0.0f );
		pOwner->SetGravityAmt( 0.0f );

		ioEntityGroup *pGrp = pOwner->GetGroup();
		int iAniID = pGrp->GetAnimationIdx( m_LupinAttribute.m_AttackAnimation );
		float fTimeRate = m_LupinAttribute.m_fAttackAniRate;
		if( fTimeRate <= 0.0f )
			fTimeRate = FLOAT1;

		pOwner->RefreshFireTimeList( iAniID, m_LupinAttribute.m_vWeaponInfoList, FTT_NORMAL_ATTACK, fTimeRate, 0 );
		pOwner->IncreaseWeaponIndexBase();

		pOwner->CheckCharColSkipTime( iAniID, fTimeRate, m_LupinAttribute.m_dwPreDelay );
		pOwner->SetReservedSliding( m_LupinAttribute.m_vForceInfoList, iAniID, fTimeRate, 0 );
	}
	
	// add buff
	int iBuffIdx = (int)(eBuffState - (BS_NONE + 1));
	if(iBuffIdx >= 0 && iBuffIdx < (int)m_vLupinList.size())
	{
		ioHashString szBuffName = m_vLupinList[ iBuffIdx ];
		pOwner->AddNewBuff( szBuffName, pOwner->GetCharName(), GetName(), NULL );
	}

	if( m_BuffState != eBuffState)
		m_BuffState = eBuffState;

	if( pOwner->IsNeedSendNetwork() )
	{
		SP2Packet kPacket( CUPK_ITEM_GAUGE_OK );
		FillItemGaugePacketHeader( pOwner, kPacket );
		kPacket << eBuffState;
		kPacket << m_iCurCombo;
		kPacket << pOwner->GetTargetRot();
		P2PNetwork::SendToAllPlayingUser( kPacket );
	}
}

void iolupinItem::ChangeToSpecialLupinAttack( ioBaseChar *pOwner, bool bFirst )
{
	if( !pOwner ) return;

	DWORD dwCurTime = FRAMEGETTIME();
	m_bCharged = true;
	pOwner->InitExtendAttackTagInfo();

	m_iWeaponWay = 0;

	m_iMaxLupinCombo = m_SpecialFirstAttribute.size();

	ClearCancelInfo();

	if( pOwner->IsNeedProcess() && bFirst )
	{
		m_iCurLupinCombo = 0;
	}

	const AttackAttribute *pAttr = NULL;	
	if( COMPARE( m_iCurLupinCombo, 0, m_iMaxLupinCombo ) )
	{
		pAttr = &m_SpecialFirstAttribute[m_iCurLupinCombo];
	}

	if( pAttr )
	{
		pOwner->SetAutoTarget( ATT_NORMAL );

		int iAniID = -1;
		float fTimeRate = FLOAT1;
		DWORD dwPreDelay = 0;
		ioEntityGroup *pGrp = pOwner->GetGroup();

		pOwner->SetNormalAttackByAttribute( *pAttr );

		iAniID = pGrp->GetAnimationIdx( pAttr->m_AttackAnimation );
		fTimeRate = pAttr->m_fAttackAniRate;
		dwPreDelay = pAttr->m_dwPreDelay;

		m_szCurAni = pAttr->m_AttackAnimation;
		m_fCurAniRate = pAttr->m_fAttackAniRate;

		m_dwInputCancelCheckTime = dwCurTime;
		m_dwCancelCheckTime = dwCurTime;

		float fAniRate = pAttr->m_fAttackAniRate;
		
		m_dwSpecialFirstAttackMoveTime = dwCurTime + dwPreDelay;
		m_dwSpecialFirstAttackMoveTime += pGrp->GetAnimationEventTime_e( iAniID, "air_move" ) * fAniRate;

		pOwner->CheckCharColSkipTime( iAniID, fTimeRate, dwPreDelay );
		pOwner->SetBaseAttackExtraAniJump( iAniID, fTimeRate, dwPreDelay );
		pOwner->SetExtendAttackEndJumpInfo( iAniID, fTimeRate, dwPreDelay );

		if( m_iMaxLupinCombo <= m_iCurLupinCombo + 1 )
			pOwner->ApplyNormalAttackResult( 0, true );
		else
			pOwner->ApplyNormalAttackResult( 0 );
	}
	else
	{
		pOwner->ApplyNormalAttackResult( 0, true );
	}

	m_ChargeState = CS_SPECIAL_LUPIN_ATTACK;

	if( pOwner->IsNeedSendNetwork() )
	{
		SP2Packet kPacket( CUPK_ITEM_GAUGE_OK );
		FillItemGaugePacketHeader( pOwner, kPacket );
		kPacket << m_ChargeState;
		kPacket << m_iCurCombo;
		kPacket << pOwner->GetTargetRot();
		kPacket << bFirst;
		kPacket << m_iCurLupinCombo;
		kPacket << m_bBlitzBack;
		P2PNetwork::SendToAllPlayingUser( kPacket );
	}

	// send 후...
	m_iCurLupinCombo++;
}

void iolupinItem::OnItemGaugeOK( ioBaseChar *pOwner, SP2Packet &rkPacket )
{
	if( !pOwner ) return;

	int iState;
	rkPacket >> iState;
	rkPacket >> m_iCurCombo;

	if( iState != SS_REQ_ACTION && iState != SS_PRE_ACTION && iState != SS_FIXED && iState != SS_NONE && iState != SS_CREATE_DUMMY &&
		 iState != BS_NONE )
	{
		D3DXQUATERNION qtRot;
		rkPacket >> qtRot;
		pOwner->SetTargetRot( qtRot );
	}

	switch( iState )
	{
	case CS_NORMAL_ATTACK:
		ChangeToNormalAttack( pOwner );
		break;
	case CS_ATTACK_FIRE:
		ChangeToAttackFire( pOwner );
		break;
	case CS_SPECIAL_LUPIN_ATTACK:
		{
			bool bFirst;
			rkPacket >> bFirst;
			if( bFirst )
				m_bSpecialLupinAttackState = true;

			rkPacket >> m_iCurLupinCombo;
			rkPacket >> m_bBlitzBack;

			ChangeToSpecialLupinAttack( pOwner, false );
		}
		break;
	case BS_NONE:
		{
			ClearLupinState( pOwner, BS_NONE );
		}
		break;
	case BS_HIDE:
	case BS_CAMOUFLAGE:
		SetLupinState( pOwner, (BuffState)iState );
		break;
	case SS_REQ_ACTION:
		{
			StartPreDelayAnimation( pOwner );
		}
		break;
	case SS_PRE_ACTION:
		{
			SetPreActionState( pOwner );
		}
		break;
	case SS_FIXED:
		{
			D3DXVECTOR3 vPos;
			D3DXQUATERNION qtRot;

			rkPacket >> vPos >> qtRot;
			pOwner->SetWorldPosition( vPos );
			pOwner->SetTargetRotToRotate( qtRot, true, false );

			SetLoopState( pOwner, false );
		}
		break;
	case SS_ATTACK:
		{
			SetJumpActionState( pOwner, false );
		}
		break;
	case SS_CREATE_DUMMY:
		{
			DWORD dwAttackCode = 0;
			rkPacket >> dwAttackCode;
			if( dwAttackCode != 0 )
				CreateDummyImmediatelyAttack( pOwner, dwAttackCode, true );
		}
		break;
	case SS_DASH:
		{
			bool bGround = false;
			rkPacket >> bGround;
			SetMoveDummyPos( pOwner, bGround, true, true );
		}
		break;
	case SS_CANCEL_DASH:
		{
			int iDirKey = 0;
			rkPacket >> iDirKey;
			SetChangeConditionalSpecialDash( pOwner,  (ioUserKeyInput::DirKeyInput)iDirKey, ioAttackableItem::CCSD_NORMAL );
		}
		break;
	case SS_FALL:
		{
			SetFallState( pOwner );
		}
		break;
	case SS_NONE:
		{
			ClearLupinState( pOwner, m_BuffState );
			m_SpecialState = SS_NONE;
			m_dwPreActionEndTime = 0;
			pOwner->SetState( CS_DELAY );
		}
		break;
	}
}

void iolupinItem::SetExtraInfoToWeapon( ioWeapon *pWeapon, bool bJumpAttack )
{
	ioAttackableItem::SetExtraInfoToWeapon( pWeapon, bJumpAttack );

	ioBombWeapon *pBomb = ToBombWeapon( pWeapon );

	if( pBomb )
	{
		float fCurAngle = 0.0f;
		if( m_iMaxWeaponWay > 1 )
		{
			float fWayAngle = m_fScatterAngle / (float)m_iMaxWeaponWay;

			fCurAngle = fWayAngle * (float)m_iWeaponWay;
			fCurAngle -= m_fScatterAngle - fWayAngle;
			fCurAngle = ioMath::ArrangeHead( fCurAngle );
		}

		D3DXQUATERNION qtAngle;
		D3DXQuaternionRotationYawPitchRoll( &qtAngle, DEGtoRAD(fCurAngle), 0.0f, 0.0f );

		D3DXVECTOR3 vMoveDir = qtAngle * pWeapon->GetOwner()->GetMoveDir();
		pWeapon->SetMoveDir( vMoveDir );

		m_iWeaponWay++;
		m_iWeaponWay %= m_iMaxWeaponWay;
	}
	else if( m_SpecialState != SS_NONE )
	{
		ioBaseChar *pChar = pWeapon->GetOwner();

		if( pChar )
		{
			D3DXVECTOR3 vDiff = m_vCirclePos - pChar->GetMidPositionByRate();
			D3DXVec3Normalize( &vDiff, &vDiff );
			pWeapon->SetMoveDir( vDiff );		
		}
	}
	else if( m_SpecialState == SS_NONE )
	{
		ioArrowWeapon *pArrow = ToArrowWeapon( pWeapon );
		ioMissileWeapon *pMissile = ToMissileWeapon( pWeapon );
		ioBombMissileWeapon *pBombMissile = ToBombMissileWeapon( pWeapon );
		ioTraceWeaponType2 *pTrace2 = ToTraceWeaponType2( pWeapon );

		if( (pArrow || pMissile || pBombMissile || pTrace2) && m_vTargetPos != ioMath::VEC3_ZERO )
		{
			D3DXVECTOR3 vDir = m_vTargetPos - pWeapon->GetPosition();
			D3DXVec3Normalize( &vDir, &vDir );

			pWeapon->SetMoveDir( vDir );
		}

		ioSylphidMissileWeapon *pSylphid = ToSylphidMissileWeapon( pWeapon );
		if( pSylphid )
		{
			g_WeaponMgr.CheckSylphidMissileWeapon( pWeapon );
		}

		ioDestroyCallWeapon *pDestroyCallWeapon = ToDestroyCallWeapon( pWeapon );
		if ( pDestroyCallWeapon )
		{
			ioBaseChar *pOwner = pWeapon->GetOwner();
			if ( pOwner )
			{
				D3DXVECTOR3 vPos = pOwner->GetMidPositionByRate() + pOwner->GetWorldOrientation() * m_vWeaponOffset;
				D3DXVECTOR3 vMoveDir = pOwner->GetWorldOrientation() * D3DXVECTOR3( 0.0f, 0.0f, -FLOAT1 );
				D3DXVec3Normalize( &vMoveDir, &vMoveDir );

				pDestroyCallWeapon->SetPosition( vPos );
				pDestroyCallWeapon->SetMoveDir( vMoveDir );
			}
		}
	}
}

bool iolupinItem::IsChargeAttackState( ioBaseChar *pOwner )
{
	if( pOwner->GetState() == CS_ATTACK )
	{
		if( m_ChargeState == CS_CHARGING )
			return true;
	}

	return false;
}

bool iolupinItem::IsEnableAttackDashCheck( ioBaseChar *pOwner, bool bCheck )
{
	if( !IsChargeDash() ) return false;
	if( !m_bCharged ) return false;

	if( pOwner->GetState() == CS_ATTACK )
	{
		switch( m_ChargeState )
		{
		case CS_ATTACK_FIRE:
			if( !bCheck )
				return false;

			return true;
		}
	}

	return false;
}

void iolupinItem::CheckReserve( ioBaseChar *pOwner )
{
	bool bReserveSkill = false;
	int iSkillNum = pOwner->GetSkillKeyInput();
	if( iSkillNum >= 0 )
	{
		m_iSkillInput = iSkillNum;
		bReserveSkill = true;
	}

	if( pOwner->IsDefenseKeyDown() && !m_bEnableDefenseState && !bReserveSkill )
	{
		m_bEnableDefenseState = true;
		m_bEnableAttackState = false;
		m_bEnableJumpState = false;

		m_iSkillInput = -1;
	}
	else if( pOwner->IsAttackKey() && !m_bEnableAttackState && !bReserveSkill )
	{
		m_bEnableAttackState = true;
		m_bEnableDefenseState = false;
		m_bEnableJumpState = false;

		m_iSkillInput = -1;
	}
	else if( pOwner->IsJumpKey() && !m_bEnableJumpState && !bReserveSkill )
	{
		m_bEnableJumpState = true;
		m_bEnableAttackState = false;
		m_bEnableDefenseState = false;
		m_iSkillInput = -1;
	}
}

bool iolupinItem::ProcessCancel( ioBaseChar *pOwner )
{
	if( m_szCurAni.IsEmpty() )
		return false;

	if( !pOwner ) return false;

	ioEntityGroup *pGrp = pOwner->GetGroup();
	if( !pGrp ) return false;

	int iAniID = pGrp->GetAnimationIdx( m_szCurAni );

	

	return false;
}

bool iolupinItem::ProcessCancelBySpeicalDash( ioBaseChar *pOwner )
{
	if( m_szCurAni.IsEmpty() )
		return false;

	if( m_dwInputCancelCheckTime == 0 )
		return false;

	if( !pOwner )
		return false;

	ioEntityGroup *pGrp = pOwner->GetGroup();
	if( !pGrp )
		return false;

	DWORD dwCurTime = FRAMEGETTIME();
	int iAniID = pGrp->GetAnimationIdx( m_szCurAni );			
	DWORD dwInputSpecialDashTime = (DWORD)pGrp->GetAnimationEventTime_e( iAniID, "input_dash_s" );
	if( 0 < dwInputSpecialDashTime )
	{
		dwInputSpecialDashTime *= m_fCurAniRate;
		dwInputSpecialDashTime += m_dwInputCancelCheckTime;		
	}

	if( 0 < dwInputSpecialDashTime && dwCurTime > dwInputSpecialDashTime )
	{
		if( pOwner->IsDirKeyDoubleClick()/*m_bEnableDashState && IsConditionalSpecialDashReserve( pOwner )*/ )
		{
			SetChangeConditionalSpecialDash( pOwner, m_ConditionalSpecialDashKey, ioAttackableItem::CCSD_NORMAL );
			return true;
		}
	}

	return false;
}

bool iolupinItem::IsConditionalSpecialDashReserve( ioBaseChar *pOwner )
{
	switch( m_SpecialState )
	{
	case SS_CANCEL_DASH:
		return true;
	}

	return false;
}

void iolupinItem::SetChangeConditionalSpecialDash( ioBaseChar *pOwner, ioUserKeyInput::DirKeyInput eCurrDir, int eCallerType )
{
	if( !pOwner || eCurrDir == ioUserKeyInput::DKI_NONE )
		return;

	ioEntityGroup *pGrp = pOwner->GetGroup();
	if( !pGrp )
		return;

	ioExtendAssaultDash *pExtendAssaultDash = ToExtendAssaultDash( GetCurExtendDash() );
	if ( !pExtendAssaultDash )
		return;

	AttackAttribute DashAttackAttr = pExtendAssaultDash->GetDashAttackAttribute();

	int iAniID = pGrp->GetAnimationIdx( DashAttackAttr.m_AttackAnimation );
	if ( iAniID == -1 )
		return;

	DWORD dwPreDelay = DashAttackAttr.m_dwPreDelay;
	DWORD dwEndDelay = DashAttackAttr.m_dwEndDelay;
	DWORD dwStartTime = FRAMEGETTIME() + dwPreDelay;

	ClearCancelInfo();
	pOwner->ClearDirDoubleClick();
	pOwner->ClearReserveKeyInput();
	pOwner->ClearReservedSliding();
	pOwner->SetCurJumpPowerAmt( 0.0f );
	pOwner->SetForcePowerAmt( 0.0f );
	pOwner->SetGravityAmt( 0.0f );
	pOwner->SetChargingState( false );

	m_SpecialState = SS_CANCEL_DASH;
	float fYawD = 180.0f + 45.0f * ( eCurrDir - ioUserKeyInput::DKI_UP );
	fYawD = ioMath::ArrangeHead( fYawD );

	float fYawR = DEGtoRAD( fYawD );
	D3DXQUATERNION qtRotate;
	D3DXQuaternionRotationYawPitchRoll( &qtRotate, fYawR, 0.0f, 0.0f );

	pOwner->SetTargetRotToRotate( qtRotate, true, false );

	float fTimeRate = DashAttackAttr.m_fAttackAniRate;
	if( fTimeRate <= 0.0f )
		fTimeRate = FLOAT1;

	if ( pExtendAssaultDash )
		pExtendAssaultDash->StartDash(pOwner);

	pOwner->CheckCharColSkipTime( iAniID, fTimeRate, dwPreDelay );

	m_dwMotionEndTime = dwStartTime + pGrp->GetAnimationFullTime(iAniID) * fTimeRate + dwEndDelay;

	DWORD dwKeyReserve = pGrp->GetAnimationEventTime_e( iAniID, "key_reserve" ) * fTimeRate;
	if( dwKeyReserve > 0 )
		m_dwKeyReserveTime += dwStartTime;

	if ( pOwner->IsNeedSendNetwork() )
	{
		SP2Packet kPacket( CUPK_ITEM_GAUGE_OK );
		FillItemGaugePacketHeader( pOwner, kPacket );
		kPacket << SS_CANCEL_DASH;
		kPacket << m_iCurCombo;
		kPacket << pOwner->GetTargetRot();
		kPacket << (int)eCurrDir;
		P2PNetwork::SendToAllPlayingUser( kPacket );
	}
}

bool iolupinItem::SetJumpAttack( ioBaseChar *pOwner, int iAniID, float fTimeRate, AttackType eType )
{
	if( !pOwner ) return false;

	DWORD dwCurTime = FRAMEGETTIME();
	ClearAirJump();

	AttackType eExtendType = AT_JUMP_ATTACK;

	const AttackAttribute *pAttr = GetAttackAttribute( eExtendType );
	if( !pAttr )
	{
		LOG.PrintTimeAndLog( 0, "iolupinItem::SetJumpAttack - AttackAttribute Not Exist" );
		return false;
	}

	ioEntityGroup *pGrp = pOwner->GetGroup();
	iAniID = pGrp->GetAnimationIdx( pAttr->m_AttackAnimation );
	if( iAniID == -1 )
	{
		LOG.PrintTimeAndLog( 0, "iolupinItem::SetJumpAttack - JumpAttack Animation Not Exist" );
		return false;
	}

	fTimeRate   = pAttr->m_fAttackAniRate;
	DWORD dwPreDelay  = pAttr->m_dwPreDelay;

	SetAirJump( pOwner, iAniID, fTimeRate );

	return false;
}

void iolupinItem::ProcessJumpAttack( ioBaseChar *pOwner, float fHeightGap )
{
	if( m_pExtendJump )
	{
		m_pExtendJump->ProcessJumpAttack( pOwner, fHeightGap );
	}

	if( m_BuffState == BS_HIDE || m_BuffState == BS_CAMOUFLAGE )
	{
		ClearLupinState( pOwner, m_BuffState );
	}

	CheckAirJump(  pOwner );
}

bool iolupinItem::CheckLupinState( ioBaseChar *pOwner )
{
	if( !pOwner )
		return false;

	bool bExist = false;
	int iBuffCnt = m_vLupinList.size();
	for( int i=0; i < iBuffCnt; ++i )
	{
		if( pOwner->HasBuff( m_vLupinList[i] ) )
		{
			bExist = true;
			if( i == (int)(m_BuffState - (BS_NONE + 1)))
			{
				if( pOwner->CheckTargetAllWoundedState() || pOwner->GetState() == CS_WOUNDED_DEFENSE || 
					pOwner->GetState() == CS_USING_SKILL )
					ClearLupinState( pOwner, m_BuffState );
			}
		}
		else
		{
			if( i == (int)(m_BuffState - (BS_NONE + 1)))
			{
				if( (i + 1) < iBuffCnt)
				{
					if( pOwner->GetState() != CS_ATTACK && pOwner->GetState() != CS_WOUNDED_DEFENSE && 
						pOwner->GetState() != CS_USING_SKILL && !pOwner->CheckTargetAllWoundedState() )
					{
						if( m_BuffState == BS_HIDE )
							m_dwEnableHideTime = FRAMEGETTIME() + m_dwHideCoolTime;
						SetLupinState( pOwner, (BuffState)(m_BuffState + 1) );
						ioHashString szBuffName = m_vLupinList[ (i + 1) ];
						pOwner->AddNewBuff( szBuffName, pOwner->GetCharName(), GetName(), NULL );
					}
					else
					{
						if( m_BuffState == BS_HIDE )
							m_dwEnableHideTime = FRAMEGETTIME() + m_dwHideCoolTime;
						ClearLupinState( pOwner, m_BuffState );
						SetLupinState( pOwner, BS_NONE );
					}
				}
				else			// 마지막 버프
				{
					if( m_SpecialState == SS_NONE )
						ClearLupinState( pOwner, m_BuffState );
					SetLupinState( pOwner, BS_NONE );
				}
			}
		}
	}

	if( !pOwner->IsNeedSendNetwork() )
		return bExist;

	// 괴도 용병이 SS_FALL 특별 상태에서 지면에 있기 시작한 상태라면 버프(혹시나 있을...)와 게이지 초기화하고 특별 상태를 SS_NONE으로 수정
	float fHeightGap = pOwner->GetWorldPosition().y - pOwner->GetBottomHeight();
	if( ( fHeightGap <= 0.1f && m_BuffState == BS_NONE && ( m_SpecialState == SS_FALL || m_SpecialState == SS_ATTACK ) ) || CheckOwnerDownState( pOwner ) )		// 다운 상태 체크
	{
		ClearLupinState( pOwner, m_BuffState );
		m_SpecialState = SS_NONE;
		m_dwPreActionEndTime = 0;
		
		if( pOwner->IsNeedSendNetwork() && pOwner->GetState() != CS_DIE && pOwner->GetState() != CS_DROP_ZONE_DOWN )
		{
			pOwner->SetState( CS_DELAY );

			SP2Packet kPacket( CUPK_ITEM_GAUGE_OK );
			FillItemGaugePacketHeader( pOwner, kPacket );
			kPacket << SS_NONE;
			kPacket << m_iCurCombo;
			P2PNetwork::SendToAllPlayingUser( kPacket );
		}	
	}

	return bExist;
}

void iolupinItem::OnEndExtendAttack( ioBaseChar *pOwner, bool bReleaseItem )
{
	pOwner->SetAttackMoveEnable( false );
	pOwner->SetChargingState( false );

	m_ChargeState = CS_NONE;

	m_bSpecialLupinAttackState = false;
	m_bBlitzBack = false;
	m_iCurLupinCombo = 0;
}

bool iolupinItem::CheckBlitzBackTarget( ioBaseChar *pOwner )
{
	if( !pOwner ) return false;
	ioPlayStage *pStage = pOwner->GetCreator();
	if( !pStage ) return false;

	FD_GrapplingAutoTarget kAutoPred( pOwner, m_fBlitzMinAngle, m_fBlitzMaxAngle, m_fBlitzRange );
	ioBaseChar *pTarget = pStage->FindUser( kAutoPred );
	if( pTarget )
	{
		D3DXQUATERNION qtRot = pTarget->GetTargetRot();
		D3DXVECTOR3 vTargetDir = qtRot * D3DXVECTOR3( 0.0f, 0.0f, -FLOAT1 );
		D3DXVec3Normalize( &vTargetDir, &vTargetDir );

		qtRot = pOwner->GetTargetRot();
		D3DXVECTOR3 vAttackDir = qtRot * D3DXVECTOR3( 0.0f, 0.0f, -FLOAT1 );
		D3DXVec3Normalize( &vAttackDir, &vAttackDir );

		float fDotValue = D3DXVec3Dot( &vTargetDir, &vAttackDir );
		if( fDotValue >= 0.0f )	
			return true;
	}

	return false;
}

void iolupinItem::CheckIncreaseChangeDamage( ioBaseChar *pOwner )
{
	if( !pOwner )
		return;

	if( CheckLupinState( pOwner ) )
		return;

	if( m_SpecialState != SS_NONE )
		return;

	float fTimePerSec = g_FrameTimer.GetSecPerFrame();
	float fCurIncrease = m_fIncreaseChangeGauge * fTimePerSec;

	m_fCurChangeGauge += fCurIncrease;
	m_fCurChangeGauge = (float)max( 0, min( m_fCurChangeGauge, m_fMaxChangeGauge ) );
	if( m_fCurChangeGauge)
	SetCurExtraGauge( m_fCurChangeGauge );
}

void iolupinItem::DecreaseChangeDamage( ioBaseChar *pOwner )
{
	if( !pOwner )
		return;

	// 위장 상태가 아니면 감소되지 않음
	if( m_BuffState != BS_CAMOUFLAGE )
		return;

	float fExtraRate = FLOAT1;
	ioBuff* pBuff = pOwner->GetBuff(BT_CHANGE_TEAM, pOwner->GetCharName());
	if( pBuff && ToChangeTeamBuff(pBuff) )
	{
		fExtraRate = ToChangeTeamBuff(pBuff)->GetExtraRate();
	}

	float fTimePerSec = g_FrameTimer.GetSecPerFrame();
	float fCurIncrease = m_fDecreaseChangeGauge * fExtraRate * fTimePerSec;

	m_fCurChangeGauge -= fCurIncrease;
	m_fCurChangeGauge = (float)max( 0, min( m_fCurChangeGauge, m_fMaxChangeGauge ) );
	SetCurExtraGauge( m_fCurChangeGauge );
}


void iolupinItem::ClearLupinState( ioBaseChar *pOwner, BuffState eBuffState, bool bSpecialAction )
{
	if( !bSpecialAction )
	{
		if( m_SpecialState != SS_NONE || m_BuffState != BS_NONE )
			SetCurBullet( 0 );
	}

	if( m_SpecialState != SS_NONE || m_BuffState != BS_NONE )
		m_fCurChangeGauge = 0.0f;
	SetCurExtraGauge( m_fCurChangeGauge );
	ClearBuff( pOwner, eBuffState );
}

int iolupinItem::GetMaxBullet()
{
	return m_iMaxBullet;
}

int iolupinItem::GetCurBullet()
{
	return	m_iCurBullet;
}

void iolupinItem::SetCurBullet( int iCurBullet )
{
	m_iCurBullet	= iCurBullet;
	m_fTempBullet	= (float)iCurBullet;
}

int iolupinItem::GetNeedBullet()
{
	if( !m_vNeedBullet.empty() )
		return	m_vNeedBullet[ SAB_FIRST_ACTION ];
	return m_iMaxBullet;
}

bool iolupinItem::IsEnableLupinFall()
{
	if( m_SpecialState == SS_NONE)
		return true;
	return false;
}

bool iolupinItem::IsEnableLupinJumpAction()
{
	if( m_SpecialState != SS_NONE )
	{
		if( m_iCurBullet <= 0 )
			return true;
		else
			return false;
	}
	return true;
}

bool iolupinItem::IsLupinFall()
{
	if( m_SpecialState == SS_FALL)
		return true;
	return false;
}

void iolupinItem::SetExtraGaugeTypeByWeapon( ioBaseChar *pOwner, int iType, int iWGT, float fDamage )
{
	if( !pOwner )
		return;

	if( m_BuffState == BS_HIDE || m_BuffState == BS_CAMOUFLAGE )
	{
		ClearLupinState( pOwner, m_BuffState );
	}
}

void iolupinItem::UpdateExtraData( ioBaseChar *pOwner )
{
	// 여기에서 bullet 업데이트를 한다.
	if( !pOwner )
		return;

	if( m_iCurBullet >= m_iMaxBullet )
	{
		m_iCurBullet = m_iMaxBullet;
		return;
	}

	if( m_SpecialState != SS_NONE )
		return;

	// 각 상태에 따라 차는 속도가 다르게 하기 전까진 여기에 Bullet값 갱신
	float fTimePerSec = g_FrameTimer.GetSecPerFrame();
	m_fTempBullet	+= fTimePerSec * m_fBulletIncreaseRate;
	m_iCurBullet	= (int)m_fTempBullet;
}

bool iolupinItem::IsEnoughBullet( SpecialActionBullet eSpecialActionBullet )
{
	if( !m_vNeedBullet.empty() )
	{
		if( (int)eSpecialActionBullet >= 0 && (int)eSpecialActionBullet < (int)m_vNeedBullet.size())
		{
			if( m_vNeedBullet[ eSpecialActionBullet ] <= m_iCurBullet )
				return	true;
		}
	}
	else
	{
		if( m_iMaxBullet <= m_iCurBullet )
			return	true;
	}

	return	false;
}

void iolupinItem::DecreaseBullet( SpecialActionBullet eSpecialActionBullet )
{
	if( !m_vNeedBullet.empty() )
	{
		if( (int)eSpecialActionBullet >= 0 && (int)eSpecialActionBullet < (int)m_vNeedBullet.size())
		{
			if( m_vNeedBullet[ eSpecialActionBullet ] <= m_iCurBullet )
				SetCurBullet( m_iCurBullet - m_vNeedBullet[ eSpecialActionBullet ] );
		}
	}
	else if ( m_iMaxBullet <= m_iCurBullet )
	{
		SetCurBullet( m_iCurBullet - m_iMaxBullet );
	}
	else
		SetCurBullet( 0 );
}

void iolupinItem::ChangeToDummyChar( ioBaseChar *pOwner, bool bApplyGravity )
{
	if( !pOwner ) return;

	ioEntityGroup *pGrp = pOwner->GetGroup();
	if( !pGrp ) return;

	if( !bApplyGravity )
		m_iCurCombo = ACT_AIR_ATT_1;

	int iAniID = -1;
	float fAniRate = FLOAT1;
	DWORD dwCurTime = FRAMEGETTIME();

	g_DummyCharMgr.DestroyDummyCharByNameList( pOwner, m_vSummonDummyInfo[m_iCurCombo].m_vDestroyDummyCharList );

	int iIndex = -1;
	D3DXVECTOR3 vTargetPos = ioMath::VEC3_ZERO;

	CreateDummyCharState( pOwner, vTargetPos, iIndex, bApplyGravity );
}

void iolupinItem::CreateDummyCharState( IN ioBaseChar *pOwner, OUT D3DXVECTOR3& vPos, OUT int& iIndex, bool bApplyGravity )
{
	if( !bApplyGravity )
		m_iCurCombo = ACT_AIR_ATT_1;

	if( m_vSummonDummyInfo[m_iCurCombo].m_iCurDummyCharIndex != -1 )
	{
		int iCurDummyCharIndex = m_vSummonDummyInfo[m_iCurCombo].m_iCurDummyCharIndex;
		LOG.PrintTimeAndLog( 0, "%s - CreateIndex : %d", __FUNCTION__, iCurDummyCharIndex );
		return;
	}

	if( !pOwner ) return;
	ioPlayStage *pStage = pOwner->GetCreator();
	if( !pStage ) return;

	D3DXVECTOR3 vNewOffSet = pOwner->GetTargetRot() * m_vSummonDummyInfo[m_iCurCombo].m_vDummyCharOffset;
	D3DXVECTOR3 vTargetPos = pOwner->GetMidPositionByRate() + vNewOffSet;

	int iCurIndex = g_DummyCharMgr.GetCurLocalIndex();

	vPos = vTargetPos;
	iIndex = iCurIndex;

	ioDummyChar *pDummy = pStage->CreateDummyChar( m_vSummonDummyInfo[m_iCurCombo].m_DummyCharName, iCurIndex,
		pOwner->GetCharName(),
		vTargetPos,
		0.0f,
		0, true );

	if( pDummy )
		m_vSummonDummyInfo[m_iCurCombo].m_iCurDummyCharIndex = iCurIndex;

	if( !m_CurExtendEquipEffect.IsEmpty() )
	{
		pOwner->EndEffect( m_CurExtendEquipEffect, false );
		m_CurExtendEquipEffect.Clear();
	}
	ioMoveAttack4Dummy* pMoveAttack4Dummy = ToMoveAttack4Dummy( pDummy );
	if( pMoveAttack4Dummy )
		pMoveAttack4Dummy->ApplyGravity( bApplyGravity );
}

void iolupinItem::CheckDummyCharState( ioBaseChar *pOwner, int iIndex )
{
	if( !pOwner ) return;

	bool bDummyCheck = false;
	int i = 0;
	for ( i = 0; i < (int)m_vSummonDummyInfo.size(); i++ )
	{
		if( m_vSummonDummyInfo[i].m_iCurDummyCharIndex == iIndex )
		{
			bDummyCheck = true;
			break;
		}
	}
	if( !bDummyCheck )
		return;

	if( m_vSummonDummyInfo[i].m_iCurDummyCharIndex == iIndex )
	{
		m_vSummonDummyInfo[i].m_iCurDummyCharIndex = -1;

		if( !m_CurExtendEquipEffect.IsEmpty() )
			pOwner->EndEffect( m_CurExtendEquipEffect, false );

		if( pOwner->IsMale() )
		{
			pOwner->AttachEffect( m_ExtendEquipEffect );
			m_CurExtendEquipEffect = m_ExtendEquipEffect;
		}
		else
		{
			pOwner->AttachEffect( m_ExtendEquipEffectHw );
			m_CurExtendEquipEffect = m_ExtendEquipEffectHw;
		}
	}
}

void iolupinItem::InitDummyCharIndex( ioBaseChar *pOwner, int iIndex )
{
	if( m_vSummonDummyInfo[m_iCurCombo].m_iCurDummyCharIndex == -1 )
		return;

	if( !pOwner ) return;

	if( m_vSummonDummyInfo[m_iCurCombo].m_iCurDummyCharIndex == iIndex )
		m_vSummonDummyInfo[m_iCurCombo].m_iCurDummyCharIndex = -1;
}

void iolupinItem::SetMoveDummyPos( ioBaseChar *pOwner, const StTopMoveInfo& info, bool bGround )
{
	if( !bGround )
		m_iCurCombo = ACT_AIR_ATT_1;
	ioDummyChar *pDummy = g_DummyCharMgr.FindDummyChar( pOwner, m_vSummonDummyInfo[m_iCurCombo].m_iCurDummyCharIndex );
	if( pDummy && pOwner )
	{
		ioMoveAttack4Dummy* pMoveDummy4 = ToMoveAttack4Dummy( pDummy );
		if( pMoveDummy4 )
		{
			D3DXVECTOR3 vNewOffSet = pOwner->GetTargetRot() * m_vSummonDummyInfo[m_iCurCombo].m_vDummyCharOffset;
			D3DXVECTOR3 vPos = pOwner->GetMidPositionByRate() + vNewOffSet;

			D3DXVECTOR3 vDir = pOwner->GetTargetRot() * D3DXVECTOR3( 0.0f, 0.0f, -FLOAT1 );

			D3DXVec3Normalize(&vDir, &vDir);
			float fAngleH = ioMath::ArrangeHead( info.fRotate_x );
			float fAngleV = ioMath::ArrangeHead( info.fRotate_y );

			D3DXQUATERNION qtRot;
			D3DXVECTOR3 vAxis;
			D3DXVec3Cross( &vAxis, &vDir, &D3DXVECTOR3( 0.0f, -FLOAT1, 0.0f ) );

			D3DXQUATERNION qtOwnerRot = pOwner->GetTargetRot();


			// vertical
			D3DXQuaternionRotationAxis( &qtRot, &vAxis, DEGtoRAD(fAngleV) );
			vDir = qtRot * vDir;

			// horizon
			D3DXQuaternionRotationAxis( &qtRot, &ioMath::UNIT_Y, DEGtoRAD(fAngleH) );
			vDir = qtRot * vDir;


			/////
			DummyCharList vList;
			g_DummyCharMgr.FindAllDummyCharToName( pOwner, m_vSummonDummyInfo[m_iCurCombo].m_DummyCharName, vList );

			if( pDummy && !m_vSummonDummyInfo[m_iCurCombo].m_vDummyAttCode.empty() )
			{
				pDummy->SetReserveAttackCode( m_vSummonDummyInfo[m_iCurCombo].m_vDummyAttCode[0] );
				pDummy->SetAttackState();
				pMoveDummy4->IncreaseComboCount();
			}
		}
	}
}

void iolupinItem::SetMoveDummyPos( ioBaseChar *pOwner, bool bGround, bool bReceive, bool bDash )
{
	if( !pOwner->IsNeedProcess() && !bReceive )
		return;
	// 지상 대쉬와 공중 대쉬일때 각각 움직이는 더미가 다르므로 더미 존재 여부 체크부터 하고 MoveDummy함수에서 움직이게 함
	int iDummyCnt = 1;
	ioDummyChar *pCheckDummy = NULL;
	if(!bGround)
	{
		pCheckDummy = g_DummyCharMgr.FindDummyChar( pOwner, m_vSummonDummyInfo[ACT_AIR_ATT_1].m_iCurDummyCharIndex );
		if( pCheckDummy )
		{
			pCheckDummy->SetWorldOrientation( pOwner->GetTargetRot() );
			m_iCurCombo = ACT_AIR_ATT_1;
			MoveDummy( pOwner, ACT_AIR_ATT_1 );
		}
	}
	else
	{
		bool bGroundDummy = false;
		
		for( int i = 0; i <= ACT_GROUND_ATT_2; i++ )
		{
			pCheckDummy = g_DummyCharMgr.FindDummyChar( pOwner, m_vSummonDummyInfo[i].m_iCurDummyCharIndex );
			if( pCheckDummy )
			{
				bGroundDummy = true;
				pCheckDummy->SetWorldOrientation( pOwner->GetTargetRot() );
			}
		}

		if( !bGroundDummy )
			return;

		iDummyCnt += ACT_GROUND_ATT_2;
		for ( int j = 0; j < iDummyCnt; j++ )
			MoveDummy( pOwner, j );
	}

	if( pOwner->IsNeedProcess() && !bReceive && bDash )
	{
		SP2Packet kPacket( CUPK_ITEM_GAUGE_OK );
		FillItemGaugePacketHeader( pOwner, kPacket );
		kPacket << SS_DASH;
		kPacket << m_iCurCombo;
		kPacket << pOwner->GetTargetRot();
		kPacket << bGround;
		P2PNetwork::SendToAllPlayingUser( kPacket );
	}
}

void iolupinItem::MoveDummy( ioBaseChar *pOwner, int iAddIndex )
{
	ioDummyChar *pDummy = g_DummyCharMgr.FindDummyChar( pOwner, m_vSummonDummyInfo[iAddIndex].m_iCurDummyCharIndex );
	if( pDummy && pOwner )
	{
		ioMoveAttack4Dummy* pMoveDummy4 = ToMoveAttack4Dummy( pDummy );
		if( pMoveDummy4 )
		{
			D3DXVECTOR3 vDir = pOwner->GetMoveDir();

			D3DXVec3Normalize(&vDir, &vDir);
			DummyCharList vList;
			g_DummyCharMgr.FindAllDummyCharToName( pOwner, m_vSummonDummyInfo[iAddIndex].m_DummyCharName, vList );

			for( DummyCharList::iterator iter = vList.begin(); iter != vList.end();  ++iter )
			{
				ioDummyChar* pDummy = ToDummyChar( *iter );
				if( pDummy && !m_vSummonDummyInfo[iAddIndex].m_vDummyAttCode.empty() )
				{
					pDummy->SetReserveAttackCode( m_vSummonDummyInfo[iAddIndex].m_vDummyAttCode[1] );
					pDummy->SetAttackState();
					pMoveDummy4->IncreaseComboCount();
				}
			}
			pMoveDummy4->SetState( DCS_ATTACK );
		}
	}
}

bool iolupinItem::CheckLupinDummy( DWORD dwAttackCode )
{
	bool bCompareAttackCode = false;
	for( int i = 0; i < (int)m_vSummonDummyInfo.size(); i++ )
	{
		for( int j = 0; j < (int)m_vSummonDummyInfo[ i ].m_vDummyAttCode.size(); j++ )
		{
			if( m_vSummonDummyInfo[ i ].m_vDummyAttCode[ j ] == dwAttackCode )
			{
				bCompareAttackCode = true;
				break;
			}
		}
	}
	return bCompareAttackCode;
}

void iolupinItem::CreateDummyImmediatelyAttack( ioBaseChar* pOwner, DWORD dwAttackCode, bool bReceive )
{
	if( !pOwner->IsNeedProcess() && !bReceive )
		return;

	int iCurCombo = m_iCurCombo;
	for( int i = 0; i < (int)m_vSummonDummyInfo.size(); i++ )
	{
		bool bCompareAttackCode = false;
		for( int j = 0; j < (int)m_vSummonDummyInfo[ i ].m_vDummyAttCode.size(); j++ )
		{
			if( m_vSummonDummyInfo[ i ].m_vDummyAttCode[ j ] == dwAttackCode )
			{
				bCompareAttackCode = true;
				break;
			}
		}
		if( !bCompareAttackCode )
			continue;

		m_iCurCombo = i;
		if( i == ACT_AIR_ATT_1 )
			ChangeToDummyChar(pOwner, false);
		else
			ChangeToDummyChar(pOwner, true);
		if( !m_vSummonDummyInfo[m_iCurCombo].m_vDummyMoveInfo.empty() )
		{
			StTopMoveInfo info = m_vSummonDummyInfo[m_iCurCombo].m_vDummyMoveInfo[0];
			SetMoveDummyPos( pOwner, info );
		}
	}
	m_iCurCombo = iCurCombo;

	if( pOwner->IsNeedProcess() && !bReceive )
	{
		SP2Packet kPacket( CUPK_ITEM_GAUGE_OK );
		FillItemGaugePacketHeader( pOwner, kPacket );
		kPacket << SS_CREATE_DUMMY;
		kPacket << m_iCurCombo;
		kPacket << dwAttackCode;
		P2PNetwork::SendToAllPlayingUser( kPacket );
	}
}

bool iolupinItem::CheckOwnerDownState( ioBaseChar *pOwner )
{
	if( !pOwner ) return false;

	CharState eState = pOwner->GetState();

	if( eState == CS_DROP_ZONE_DOWN )
		return true;
	else if( eState == CS_DIE )
		return true;

	return false;
}

void iolupinItem::ClearBuff( ioBaseChar* pOwner, BuffState eBuffState /* = BS_NONE */ )
{
	if( !pOwner )
		return;

	if( eBuffState != BS_NONE )
	{
		int iBuffIdx = (int)(eBuffState - (BS_NONE + 1));

		pOwner->RemoveBuff( m_vLupinList[ iBuffIdx ] );

		if( eBuffState == BS_HIDE )
			m_dwEnableHideTime = FRAMEGETTIME() + m_dwHideCoolTime;
	}
	else
	{
		int iBuffCnt = m_vLupinList.size();
		for( int i=0; i < iBuffCnt; ++i )
		{
			pOwner->RemoveBuff( m_vLupinList[i] );
		}
	}

	eBuffState = BS_NONE;
}

float iolupinItem::GetCurBuffTickTime( ioBaseChar *pOwner )
{
	return 0.0f;
}

bool iolupinItem::IsLupinSpecialFixedState()
{
	if( m_SpecialState == SS_FIXED)
		return true;
	return false;
}

bool iolupinItem::IsLupinSpecialFallState()
{
	if( m_SpecialState == SS_FALL)
		return true;
	return false;
}

void iolupinItem::CallItemProcess( ioBaseChar* pOwner )
{
	if( !pOwner->IsNeedProcess() )
		return;

	DWORD dwCurTime = FRAMEGETTIME();

	switch( m_SpecialState )
	{
	case SS_CANCEL_DASH:
		{
			ProcessCancelDashState( pOwner );
		}
		break;
	case SS_REQ_ACTION:
		{	
			if( pOwner->CheckTargetAllWoundedState() && pOwner->GetState() != CS_FALL )
			{
				m_SpecialState = SS_NONE;
				break;
			}

			if( m_dwPreSpecialMoveStartTime + m_dwPreDelayTime < dwCurTime )
			{
				m_dwPreSpecialMoveStartTime = 0;
				m_dwPreActionEndTime = 0;
				DecreaseBullet( SAB_FIRST_ACTION );
				SetPreActionState( pOwner );
			}
		}
		break;
	case SS_PRE_ACTION:
		{	
			if( pOwner->CheckTargetAllWoundedState() && pOwner->GetState() != CS_FALL )
			{
				m_SpecialState = SS_NONE;
				break;
			}

			if( m_dwPreActionEndTime > 0 && m_dwPreActionEndTime < dwCurTime )
			{	
				m_SpecialState = SS_FALL;
				ClearLupinState( pOwner, BS_CAMOUFLAGE, true );
				ChangeToSpecialLupinAttack( pOwner, false );
				return;
			}	
		}
		break;
	case SS_FIXED:
		{
			if( pOwner->CheckTargetAllWoundedState() && pOwner->GetState() != CS_FALL )
			{
				SetCurBullet( 0 );
				m_fCurChangeGauge = 0.0f;
				SetCurExtraGauge( m_fCurChangeGauge );
				ClearBuff( pOwner );
				m_SpecialState = SS_FALL;
				SetEndJumpState( pOwner );
				break;
			}

			if( COMPARE( dwCurTime, m_dwLoopDurationStart, m_dwLoopDurationEnd ) )
			{
				pOwner->SetState( CS_FALL );
				// 뱡향 변화 체크
				pOwner->CheckChangeDirectionForMagicCircle();

				// MagicCircle 위치 갱신
				pOwner->UpdateMagicCirclePosAll( m_fCircleMoveSpeed, m_fCircleRange, m_fCircleHeightGap, false );

				// MagicCircle 위치방향으로 캐릭터 회전
				m_vMagicCirclePos = pOwner->GetMagicCirclePos();
				pOwner->SetTargetRotToTargetPos( m_vMagicCirclePos, true, false );

				m_dwWeaponBaseIndex = pOwner->IncreaseWeaponIndexBase();

				CheckLoopState( pOwner );		
			}
			else
			{
				//SetCurBullet( 0 );
				m_fCurChangeGauge = 0.0f;
				SetCurExtraGauge( m_fCurChangeGauge );
				ClearBuff( pOwner );
				m_SpecialState = SS_FALL;
				SetEndJumpState( pOwner );
			}
		}
		break;

	case SS_LOOP: 
		{	
	
		}
		break;

	case SS_ATTACK:
		{
			if( 0 < m_dwSpecialSecAttDurationEnd && m_dwSpecialSecAttDurationEnd >= dwCurTime )
			{
				m_SpecialState = SS_FALL;
				pOwner->SetTargetRotToTargetPos( m_vMagicCirclePos, true, false );
				pOwner->ChangeDirectionByInputDir( false );
				SetFallState( pOwner );

				if( pOwner->IsNeedProcess() )
				{
					SP2Packet kPacket( CUPK_ITEM_GAUGE_OK );
					FillItemGaugePacketHeader( pOwner, kPacket );
					kPacket << SS_FALL;
					kPacket << m_iCurCombo;
					kPacket << pOwner->GetTargetRot();
					P2PNetwork::SendToAllPlayingUser( kPacket );
				}
			}
			
		}
		break;
	case SS_JUMP_ACTION:
		{

		}
		break;
	case SS_FALL:
		{
			if( pOwner->CheckTargetAllWoundedState() && pOwner->GetState() != CS_FALL )
				break;

			if( m_dwPreActionEndTime <= dwCurTime )
			{
				if( pOwner->IsJumpKey() )
				{
					if( IsEnoughBullet( SAB_SECOND_ACTION ) && m_SpecialState != SS_FIXED )
						SetLoopState( pOwner, true );
				}
			}

			if( m_dwSpecialFirstAttackMoveTime > 0 && m_dwSpecialFirstAttackMoveTime < dwCurTime )
			{
				m_dwSpecialFirstAttackMoveTime = 0;
				pOwner->SetChangeJumppingState( 0.0f, false, 0 );
			}
		}
		break;
	}

	// 무기 스킬 타겟이 있고 ioStealEquipomentBuff 버프를 가지고 있는 상태라면 타겟 넘겨줌
	if( m_pTargetChar )
	{
		ioBuff* pBuff = pOwner->GetBuff( BT_STEAL_EQUIPMENT_BUFF );
		ioStealEquipmentBuff* pStealEquipmentBuff = ToStealEquipmentBuff( pBuff );
		if( pStealEquipmentBuff )
		{
			pStealEquipmentBuff->SetStealTargetChar( m_pTargetChar );
			m_pTargetChar = NULL;
		}
	}
}

bool iolupinItem::StartPreDelayAnimation( ioBaseChar *pChar )
{
	ioHashString szPreDelay = m_PreDelayAnimation;

	if( szPreDelay.IsEmpty() )
	{
		m_dwPreSpecialMoveStartTime = 0;
		return false;
	}

	ioEntityGroup *pGrp = pChar->GetGroup();
	pGrp->ClearAllLoopAni( FLOAT100, true );
	pGrp->ClearAllActionAni( FLOAT100, true );
	pGrp->SetLoopAni( szPreDelay, FLOAT100 );

	m_dwPreSpecialMoveStartTime = FRAMEGETTIME();

	m_SpecialState = SS_REQ_ACTION;

	if( pChar->IsNeedSendNetwork() )
	{
		SP2Packet kPacket( CUPK_ITEM_GAUGE_OK );
		FillItemGaugePacketHeader( pChar, kPacket );
		kPacket << SS_REQ_ACTION;
		kPacket << szPreDelay;
		P2PNetwork::SendToAllPlayingUser( kPacket );
	}	

	return true;
}

void iolupinItem::SetPreActionState( ioBaseChar *pChar )
{
	m_SpecialState = SS_PRE_ACTION;
	pChar->SetJumpPower( m_fSpecialAttackJumpPower * m_fSpecialAttackGravityRate );

	ioEntityGroup *pGrp = pChar->GetGroup();
	ioHashString szSkillMotion = m_SpecialAttackMotion;
	int iCurSkillAniID = pGrp->GetAnimationIdx( szSkillMotion );
	if( iCurSkillAniID == -1 )	return;

	float fAniRate = m_fSpecialAttackAniTimeRate;
	float fTimeRate = fAniRate * FLOAT1;

	pGrp->ClearAllLoopAni( FLOAT100, true );
	pGrp->ClearAllActionAni( FLOAT100, true );
	pGrp->SetActionAni( iCurSkillAniID, FLOAT100, 5.0f, FLOAT1, FLOAT1/(fTimeRate), 0.0f, 0.0f, true );

	DWORD dwCurTime = FRAMEGETTIME();

	m_dwPreActionEndTime = dwCurTime;
	m_dwPreActionEndTime += pGrp->GetAnimationFullTime( iCurSkillAniID ) * fTimeRate;

	if( pChar->IsNeedSendNetwork() )
	{
		SP2Packet kPacket( CUPK_ITEM_GAUGE_OK );
		FillItemGaugePacketHeader( pChar, kPacket );
		kPacket << SS_PRE_ACTION;
		P2PNetwork::SendToAllPlayingUser( kPacket );
	}	
}

void iolupinItem::SetLoopState( ioBaseChar *pChar, bool bSend )
{
	if( !pChar ) return;

	DWORD dwCurTime = FRAMEGETTIME();
	m_SpecialState = SS_FIXED;

	ioEntityGroup *pGrp = pChar->GetGroup();
	pGrp->ClearAllLoopAni( FLOAT100, true );
	pGrp->ClearAllActionAni( FLOAT100, true );

	int iAniID = pGrp->GetAnimationIdx( m_SpecialLoopMotion );
	float fAniRate = FLOAT1;
	if( m_fSpecialLoopMotionRate > 0.0f )
		fAniRate = m_fSpecialLoopMotionRate;

	pGrp->SetLoopAni( iAniID, FLOAT100, FLOAT1, FLOAT1/fAniRate );

	m_dwLoopDurationStart = dwCurTime;
	m_dwLoopDurationEnd = dwCurTime + m_dwFixedTimeInAir;


	if( !pChar->CheckMagicCircle() )
	{
		ioHashString szDummyCircle;
		pChar->CreateMagicCircle( m_ReadyCircle, m_ReadyCircle, szDummyCircle, m_fCircleOffSet );
	}


	if( bSend && pChar->IsNeedSendNetwork() )
	{
		SP2Packet kPacket( CUPK_ITEM_GAUGE_OK );
		FillItemGaugePacketHeader( pChar, kPacket );
		kPacket << SS_FIXED;
		kPacket << m_iCurCombo;
		kPacket << pChar->GetWorldPosition();
		kPacket << pChar->GetTargetRot();
		P2PNetwork::SendToAllPlayingUser( kPacket );
	}	
}

void iolupinItem::CheckLoopState( ioBaseChar *pChar )
{
	if( !pChar )
		return;	

	DWORD dwCurTime = FRAMEGETTIME();

	if( pChar->IsNeedProcess() )
	{
		//키입력 여부 검사
		bool bReserveSkill = false;
		int iSkillNum = pChar->GetSkillKeyInput();
		if( iSkillNum >= 0 )
		{
			bReserveSkill = true;
		}

		if( pChar->IsAttackKey() && !bReserveSkill )
		{
			DecreaseBullet( SAB_SECOND_ACTION );	// D키 입력하여 선물 폭턴 투척 시 게이지 사용
			SetJumpActionState( pChar, true );		
		}		
	}
}

bool iolupinItem::IsNoDropState()
{
	switch ( m_SpecialState )
	{
	case SS_JUMP_ACTION:
	case SS_FIXED:
		return true;
	}

	return false;
}

void iolupinItem::SetEndJumpState( ioBaseChar *pChar )
{

	if( !pChar ) return;

	m_dwLoopDurationEnd = 0;

	m_SpecialState = SS_FALL;

	pChar->SetMagicCircleTarget( "" );

	//DestroyMapEffect
	if( m_dwCurMapEffect != -1 )
		pChar->EndMapEffect( m_dwCurMapEffect, false );

	m_dwCurMapEffect = -1;

	// 동작 끝나서 초기화
	pChar->DestroyMagicCircle();

	m_dwLoopDurationStart = 0;
	m_dwLoopDurationEnd = 0;
}

void iolupinItem::SetJumpActionState( ioBaseChar *pChar, bool bSend )
{	
	if( !pChar )
		return;
	ioEntityGroup *pGrp = pChar->GetGroup();
	if ( !pGrp )
		return;

	pChar->SetJumpPower(0.0f);
	pChar->SetGravityAmt( 0.0f );
	pChar->SetJumpPower( m_fActionJumpPower );

	m_dwLoopDurationStart = 0;
	int iAniID = pGrp->GetAnimationIdx( m_SpecialSecondAttribute.m_AttackAnimation );
	float fAniRate = m_SpecialSecondAttribute.m_fAttackAniRate;
	if( fAniRate <= 0.0f )
		fAniRate = FLOAT1;

	DWORD dwPreDelay = m_SpecialSecondAttribute.m_dwPreDelay * fAniRate;

	pGrp->ClearAllActionAni( FLOAT100, true );
	pGrp->ClearAllLoopAni( FLOAT100, true );	

	pGrp->SetActionAni( iAniID, FLOAT100, FLOAT100, FLOAT1, FLOAT1/fAniRate, 0.0f, dwPreDelay, true );

	//Refresh
	if( !m_SpecialSecondAttribute.m_vWeaponInfoList.empty() )
	{
		pChar->RefreshFireTimeListWithBase( m_dwWeaponBaseIndex,
			iAniID,
			m_SpecialSecondAttribute.m_vWeaponInfoList,
			FTT_NORMAL_ATTACK,
			fAniRate,
			dwPreDelay 
			);
	}

	//모션 종료시간
	DWORD dwCurTime = FRAMEGETTIME();
	m_dwSpecialSecAttDurationEnd = dwCurTime;
	m_dwSpecialSecAttDurationEnd += pGrp->GetAnimationFullTime( iAniID ) * fAniRate;
	m_dwSpecialSecAttDurationEnd += dwPreDelay;

	//발사모션부분
	m_dwFireMotionEndTime = dwCurTime + dwPreDelay;
	m_dwFireMotionEndTime += pGrp->GetAnimationEventTime_e( iAniID, "firemotion_e" ) * fAniRate;

	if( pChar->CheckMagicCircle() )
		pChar->UpdateMagicCirclePos( m_fCircleMoveSpeed, m_fCircleRange, m_fCircleHeightGap, true, true );

	m_vCirclePos = pChar->GetMagicCirclePos( m_fCircleOffSet, false );

	//
	ioMapEffect *pMapEffect = NULL;
	if( pChar->GetTeam() == TEAM_RED )
	{
		pMapEffect = pChar->CreateMapEffectBySkill( m_RedEffect, m_vCirclePos );
	}
	else if( pChar->GetTeam() == TEAM_BLUE )
	{
		pMapEffect = pChar->CreateMapEffectBySkill( m_BlueEffect, m_vCirclePos );
	}
	else
	{
		if( pChar->GetSingleConvertTeam() == TEAM_BLUE )
		{
			pMapEffect = pChar->CreateMapEffectBySkill( m_BlueEffect, m_vCirclePos );
		}
		else
		{
			pMapEffect = pChar->CreateMapEffectBySkill( m_RedEffect, m_vCirclePos );
		}
	}
	pChar->DestroyMagicCircle();
	if( pChar->IsNeedSendNetwork() )
	{
		SP2Packet kPacket( CUPK_ITEM_GAUGE_OK );
		FillItemGaugePacketHeader( pChar, kPacket );
		kPacket << SS_ATTACK;
		P2PNetwork::SendToAllPlayingUser( kPacket );
	}
	m_SpecialState = SS_ATTACK;
}

void iolupinItem::SetFallState( ioBaseChar *pChar )
{
	m_SpecialState = SS_FALL;
	ClearLupinState( pChar, m_BuffState, true );
	pChar->SetState(CS_JUMP);
	pChar->SetJumppingState( false );
	SetEndJumpState( pChar );		
}

void iolupinItem::ProcessCancelDashState( ioBaseChar *pOwner )
{
	if( !pOwner || !pOwner->IsNeedProcess() )
		return;

	DWORD dwCurTime = FRAMEGETTIME();
	if ( m_dwMotionEndTime < dwCurTime )
	{
		float fHeightGap = pOwner->GetWorldPosition().y - pOwner->GetBottomHeight();

		if( m_dwMotionEndTime > 0 && m_dwMotionEndTime < dwCurTime )
			ClearData();
		ChangeReserveKeyInputState( pOwner );
		return;
	}

	if ( m_dwKeyReserveTime > 0 && m_dwKeyReserveTime < dwCurTime )
		CheckReserve( pOwner );
}

void iolupinItem::ChangeReserveKeyInputState( ioBaseChar *pOwner, int iCurCombo /* = 0 */ )
{
	// 필살기 예약되었으면 필살기 발동
	if( m_iSkillInput >= 0 && !pOwner->IsBuffLimitSkill() && pOwner->SetUseSkill( m_iSkillInput, SUT_NORMAL ) )
	{
		m_iSkillInput = -1;
		return;
	}

	// 방어
	if( m_bEnableDefenseState && pOwner->IsEnableDefenseByObject() && !pOwner->IsBuffLimitDefense() )
	{
		pOwner->SetDefenseState();
		return;
	}

	// 점프
	if( m_bEnableJumpState && pOwner->IsCanJumpState() && !pOwner->IsBuffLimitJump() )
	{
		pOwner->SetJumpState();
		return;
	}

	// 공격
	if( m_bEnableAttackState )
	{
		ioObjectItem *pItem = pOwner->GetObject();

		if( ToDrinkItem( pItem ) )
		{
			pOwner->SetDrinkState();
		}
		else
		{
			if( pOwner->IsCanNormalAttack() && !pOwner->IsBuffLimitAttack() )
			{
				if ( !COMPARE( iCurCombo, 0, GetMaxCombo() ) )
					iCurCombo = 0;
				pOwner->SetNormalAttack( iCurCombo );
			}
		}
		return;
	}

	// 대기
	pOwner->SetState( CS_DELAY );
}

void iolupinItem::DestroyLupinDummy( ioBaseChar *pOwner )
{
	if( !pOwner )
		return;

	for( int i = 0; i < (int)m_vSummonDummyInfo.size(); i++ )
		g_DummyCharMgr.DestroyDummyCharByNameList( pOwner, m_vSummonDummyInfo[ i ].m_vDestroyDummyCharList );
}

void iolupinItem::GetTargetCharByWeaponSkill( ioBaseChar* pOwner, ioBaseChar* pTarget, int iAttributeIndex )
{
	if( !m_pTargetChar )
	{
		ioBuff* pBuff = pOwner->GetBuff( BT_STEAL_EQUIPMENT_BUFF );
		ioStealEquipmentBuff* pStealEquipmentBuff = ToStealEquipmentBuff( pBuff );
		if( pStealEquipmentBuff )
		{
			m_pTargetChar = pTarget;
			pStealEquipmentBuff->SetStealTargetChar( pTarget );
		}
		else
		{
			// 피격자보다 공격자 쪽에서 스킬끝나는게 빨라 버프가 미리 실행된 상황이라면...
			ioSkill *pSkill = pOwner->GetEquipedSkill( m_SkillName );
			ioFireSkill* pFireSkill = ToFireSkill( pSkill );
			if( pFireSkill && pFireSkill->HasAttackAttributeByIndex( iAttributeIndex ) )
			{
				ioBuff* pAddBuff = pOwner->AddNewBuff( GetWeaponSkillBuffName(), pOwner->GetCharName(), "", NULL );
				ioStealEquipmentBuff* pStealEquipmentBuff2 = ToStealEquipmentBuff( pAddBuff );
				if( pStealEquipmentBuff2 )
				{
					ClearTargetCharByWeaponSkill();
					pStealEquipmentBuff2->SetStealTargetChar( pTarget );
					pStealEquipmentBuff2->ChangeEquipItem();
					pStealEquipmentBuff2->SetReserveEndBuff();
				}
			}
		}
	}
}