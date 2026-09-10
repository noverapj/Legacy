#include "stdafx.h"
#include "ioFrozenItem.h"
#include "ioBaseChar.h"
#include "WeaponDefine.h"
#include "ItemDefine.h"

ioFrozenItem::ioFrozenItem()
{
	m_FrozenState					= FS_NONE;
	m_iCurCombo						= 0;
	m_dwChargeStartTime				= 0;
	m_fCurGauge						= 0.0f;
	m_dwChargeReadyAnimationTime	= 0;
	m_bChargeLoopAniStart			= false;
	m_dwChargeLoopStartTime			= 0;
	m_bLoopAttackKeyDown			= false;
	m_dwChargeAttackStartTime		= 0;
	m_iCurMultiShot					= 0;
	m_CurrentChargeAttack			= NULL;
	m_CurrentChargeAttackMax		= NULL;
	m_bChargeLoopLoopAniStart		= false;
	m_iMultShotGaugeMax				= 0;
	m_dwLoopLoopChargeCheckTime		= 0;
	m_dwFrozenDummyIndex			= -1;
	m_bDefenseFirstMoveMotion		= true;
	m_bDefenseMotionSetted			= false;
	m_dwFrozenDummyStartTime		= 0;
	m_bFrozenDummyStart				= false;
	m_iCurFrozenDummyGauge			= 0;
	m_vWeaponMoveInfoList.clear();
	m_fMoveRange					= 0;
	m_vAttackDir					= ioMath::VEC3_ZERO;	
	m_iCurShotGaugeMax				= 0;
	m_dwFireEndTime					= 0;
	m_dwAniEndTime					= 0;
	m_dwArrowSpecialEffectID		= 0;
}

ioFrozenItem::ioFrozenItem( const ioFrozenItem &rhs )
: ioWeaponItem( rhs ),
m_fMaxGauge( rhs.m_fMaxGauge ),
m_fIncreaseGauge( rhs.m_fIncreaseGauge ),
m_fNeedGauge( rhs.m_fNeedGauge ),
m_fAddNeedGauge( rhs.m_fAddNeedGauge ),
m_szDelayAnimation( rhs.m_szDelayAnimation ),
m_fRecoverGaugeDelay( rhs.m_fRecoverGaugeDelay ),
m_fRecoverGaugeRun( rhs.m_fRecoverGaugeRun ),
m_fRecoverGaugeDash( rhs.m_fRecoverGaugeDash ),
m_fRecoverGaugeWound( rhs.m_fRecoverGaugeWound ),
m_bBulletGaugeInit( rhs.m_bBulletGaugeInit ),
m_szChargeReadyAnimation( rhs.m_szChargeReadyAnimation ),
m_szChargeLoopAnimation( rhs.m_szChargeLoopAnimation ),
m_szChargeLoopReadyAnimation( rhs.m_szChargeLoopReadyAnimation ),
m_szChargeLoopLoopAnimation( rhs.m_szChargeLoopLoopAnimation ),
m_szReadyCircleEffect( rhs.m_szReadyCircleEffect ),
m_szEnableCircleEffect( rhs.m_szEnableCircleEffect ),
m_fCircleMoveSpeed( rhs.m_fCircleMoveSpeed ),
m_fCircleRange( rhs.m_fCircleRange ),
m_fCircleHeightGap( rhs.m_fCircleHeightGap ),
m_fCircleOffset(rhs.m_fCircleOffset),
m_dwChargeAttackTerm( rhs.m_dwChargeAttackTerm ),
m_FullAttackMin( rhs.m_FullAttackMin ),
m_FullAttackMax( rhs.m_FullAttackMax ),
m_fWeaponGravityRate( rhs.m_fWeaponGravityRate ),
m_fWeaponFloatPower( rhs.m_fWeaponFloatPower ),
m_fRevisionRate( rhs.m_fRevisionRate ),
m_fUpLimitRange( rhs.m_fUpLimitRange ),
m_fWeaponFloatMinPower( rhs.m_fWeaponFloatMinPower ),
m_fUpLimitHeight( rhs.m_fUpLimitHeight ),
m_fWeaponMinSpeed( rhs.m_fWeaponMinSpeed ),
m_iMultiShotMin( rhs.m_iMultiShotMin ),
m_iMultiShotMax( rhs.m_iMultiShotMax ),
m_dwMultiShotGaugeTime( rhs.m_dwMultiShotGaugeTime ),
m_dwHalfChargeTime( rhs.m_dwHalfChargeTime ),		
m_szFrozenDummyName( rhs.m_szFrozenDummyName ),
m_vFrozenDummyOffset( rhs.m_vFrozenDummyOffset ),
m_AniDefenseFrontMove( rhs.m_AniDefenseFrontMove ),
m_AniDefenseBackMove( rhs.m_AniDefenseBackMove ),
m_AniDefenseRightMove( rhs.m_AniDefenseRightMove ),
m_AniDefenseLeftMove( rhs.m_AniDefenseLeftMove ),
m_HalfAttack( rhs.m_HalfAttack ),
m_iFrozenDummyMaxGauge( rhs.m_iFrozenDummyMaxGauge ),
m_fFrozenDummyChargingTime( rhs.m_fFrozenDummyChargingTime ), 
m_FrozenSpecialAttack( rhs.m_FrozenSpecialAttack ),
m_FrozenChargeSpecialEffect( rhs.m_FrozenChargeSpecialEffect ),
m_fFrozenChargeSpecialOffset( rhs.m_fFrozenChargeSpecialOffset ),
m_vFrozenChargeSpecialEffectAngle( rhs.m_vFrozenChargeSpecialEffectAngle ),
m_RedEffect( rhs.m_RedEffect ),
m_BlueEffect( rhs.m_BlueEffect ),
m_fRandomRange( rhs.m_fRandomRange ),
m_iFrozenEffectlimit( rhs.m_iFrozenEffectlimit ),
m_fChargeNeedGauge( rhs.m_fChargeNeedGauge ),
m_ArrowChargeSpecialEffect( rhs.m_ArrowChargeSpecialEffect ),
m_fArrowChargeSpecialOffset( rhs.m_fArrowChargeSpecialOffset ),
m_fArrowChargeSpecialEffectAngle( rhs.m_fArrowChargeSpecialEffectAngle )
{
	m_FrozenState					= FS_NONE;
	m_iCurCombo						= 0;
	m_dwChargeStartTime				= 0;
	m_fCurGauge						= 0.0f;
	m_dwChargeReadyAnimationTime	= 0;
	m_bChargeLoopAniStart			= false;
	m_dwChargeLoopStartTime			= 0;
	m_bLoopAttackKeyDown			= false;
	m_dwChargeAttackStartTime		= 0;
	m_iCurMultiShot					= 0;
	m_CurrentChargeAttack			= NULL;
	m_CurrentChargeAttackMax		= NULL;
	m_bChargeLoopLoopAniStart		= false;
	m_iMultShotGaugeMax				= 0;
	m_dwLoopLoopChargeCheckTime		= 0;
	m_dwFrozenDummyIndex			= -1;
	m_bDefenseFirstMoveMotion		= true;
	m_bDefenseMotionSetted			= false;
	m_dwFrozenDummyStartTime		= 0;
	m_bFrozenDummyStart				= false;
	m_iCurFrozenDummyGauge			= 0;
	m_vWeaponMoveInfoList.clear();
	m_fMoveRange					= 0;
	m_vAttackDir					= ioMath::VEC3_ZERO;	
	m_iCurShotGaugeMax				= 0;
	m_dwFireEndTime					= 0;
	m_dwAniEndTime					= 0;
	m_dwArrowSpecialEffectID		= 0;
}

ioFrozenItem::~ioFrozenItem()
{
}

void ioFrozenItem::LoadProperty( ioINILoader &rkLoader )
{
	ioWeaponItem::LoadProperty( rkLoader );

	char szBuf[MAX_PATH];

	// 기본공격은 ioAttackableItem 불러온다.

	// 공격 단계 시작을 알린다. ( 빈 애니 )
	rkLoader.LoadString( "attack_ready_animation", "", szBuf, MAX_PATH );
	m_szDelayAnimation		= szBuf;

	// Gauge
	m_fMaxGauge				= rkLoader.LoadFloat_e( "max_gauge", FLOAT100 );			// 게이지 총 카운트
	m_fNeedGauge			= rkLoader.LoadFloat_e( "need_gauge", FLOAT100 );			// 게이지 사용
	m_fIncreaseGauge		= rkLoader.LoadFloat_e( "increase_gauge", FLOAT1 );			// 게이지 증가 폭 
	m_fAddNeedGauge			= rkLoader.LoadFloat_e( "add_need_gauge", FLOAT1 );			// 추가 게이지 사용 
	m_fChargeNeedGauge		= rkLoader.LoadFloat_e( "add_charge_need_gauge", FLOAT1 );	// 추가 게이지 사용 2020-03-17

	// Gauge 캐릭터 상태마다 게이지 증가 값을 다르게 한다. 2020-03-04
	m_fRecoverGaugeDelay	= rkLoader.LoadFloat_e( "recover_gauge_delay", 0.0f );
	m_fRecoverGaugeRun		= rkLoader.LoadFloat_e( "recover_gauge_run", 0.0f );
	m_fRecoverGaugeDash		= rkLoader.LoadFloat_e( "recover_gauge_dash", 0.0f );
	m_fRecoverGaugeWound	= rkLoader.LoadFloat_e( "recover_gauge_wound", 0.0f );

	// 용병 교체 시 불렛 게이지 초기화 사용 유무 2020-03-05
	m_bBulletGaugeInit  = rkLoader.LoadBool_e( "bullet_gauage_init_by_change_char", false );

	// +D, D~ 
	rkLoader.LoadString( "charge_ready_animation", "", szBuf, MAX_PATH );
	m_szChargeReadyAnimation		= szBuf;
	rkLoader.LoadString( "charge_loop_animation", "", szBuf, MAX_PATH );
	m_szChargeLoopAnimation			= szBuf;
	rkLoader.LoadString( "charge_loop_ready_animation", "", szBuf, MAX_PATH );
	m_szChargeLoopReadyAnimation	= szBuf;
	rkLoader.LoadString( "charge_loop_loop_animation", "", szBuf, MAX_PATH );
	m_szChargeLoopLoopAnimation	= szBuf;	

	// 매직서클 생성
	rkLoader.LoadString( "ready_circle", "", szBuf, MAX_PATH );
	m_szReadyCircleEffect	= szBuf;
	rkLoader.LoadString( "enable_circle", "", szBuf, MAX_PATH );
	m_szEnableCircleEffect	= szBuf;
	m_fCircleMoveSpeed		= rkLoader.LoadFloat_e( "magic_circle_move_speed", FLOAT100 );
	m_fCircleRange			= rkLoader.LoadFloat_e( "magic_circle_range", FLOAT100 );
	m_fCircleHeightGap		= rkLoader.LoadFloat_e( "magic_circle_height_gap", FLOAT100 );
	m_fCircleOffset			= rkLoader.LoadFloat_e( "magic_circle_offset", FLOAT1 );


	m_fWeaponGravityRate	= rkLoader.LoadFloat_e( "weapon_gravity_rate", FLOAT1 );
	m_fWeaponFloatPower		= rkLoader.LoadFloat_e( "weapon_float_power", FLOAT1 );
	m_fRevisionRate			= rkLoader.LoadFloat_e( "weapon_revision_rate", 0.0f );
	m_fUpLimitRange			= rkLoader.LoadFloat_e( "up_limit_range", 0.0f );
	m_fWeaponFloatMinPower	= rkLoader.LoadFloat_e( "weapon_float_min_power", FLOAT1 );
	m_fUpLimitHeight		= rkLoader.LoadFloat_e( "up_limit_height", 30.0f );
	m_fWeaponMinSpeed		= rkLoader.LoadFloat_e( "weapon_min_speed", 0.0f );
	m_iMultiShotMin			= rkLoader.LoadInt_e( "multi_shot_min", 0 );
	m_iMultiShotMax			= rkLoader.LoadInt_e( "multi_shot_max", 0 );

	LoadAttackAttribute( "full_attack_min", m_FullAttackMin, rkLoader );
	LoadAttackAttribute( "full_attack_max", m_FullAttackMax, rkLoader );

	m_dwMultiShotGaugeTime	= rkLoader.LoadInt_e( "multi_shot_gauge_time", 1000 );

	m_dwChargeAttackTerm	= rkLoader.LoadInt_e( "frozen_charge_special_attack_term", 0 );

	// Half 차지
	LoadAttackAttribute( "half_attack", m_HalfAttack, rkLoader );
	m_dwHalfChargeTime		= rkLoader.LoadInt_e( "half_charging_time", 0 );

	rkLoader.LoadString_e( "frozen_dummy_name", "", szBuf, MAX_PATH );
	m_szFrozenDummyName = szBuf;
	m_vFrozenDummyOffset.x = rkLoader.LoadFloat_e( "frozen_dummy_offset_x", 0.0f );
	m_vFrozenDummyOffset.y = rkLoader.LoadFloat_e( "frozen_dummy_offset_y", 0.0f );
	m_vFrozenDummyOffset.z = rkLoader.LoadFloat_e( "frozen_dummy_offset_z", 0.0f );

	m_iFrozenDummyMaxGauge		= rkLoader.LoadInt_e( "frozen_dummy_max_gauge", 0 );
	m_fFrozenDummyChargingTime	= rkLoader.LoadFloat_e( "frozen_dummy_charging_time", 0 );	

	rkLoader.LoadString_e( "ani_defense_front_move", "victory_defense_move_fr.ani", szBuf, MAX_PATH );
	m_AniDefenseFrontMove = szBuf;
	rkLoader.LoadString_e( "ani_defense_back_move", "victory_defense_move_bk.ani", szBuf, MAX_PATH );
	m_AniDefenseBackMove = szBuf;
	rkLoader.LoadString_e( "ani_defense_right_move", "victory_defense_move_rt.ani", szBuf, MAX_PATH );
	m_AniDefenseRightMove = szBuf;
	rkLoader.LoadString_e( "ani_defense_left_move", "victory_defense_move_lt.ani", szBuf, MAX_PATH );
	m_AniDefenseLeftMove = szBuf;

	// D~ + D~ 떼기 시 공격
	LoadAttackAttribute( "frozen_charge_special_attack", m_FrozenSpecialAttack, rkLoader );
	rkLoader.LoadString_e( "frozen_charge_special_effect", "", szBuf, MAX_PATH );
	m_FrozenChargeSpecialEffect  = szBuf;
	m_fFrozenChargeSpecialOffset = rkLoader.LoadFloat_e( "frozen_charge_special_effect_offset", FLOAT1 );
	int iCnt = m_FrozenSpecialAttack.m_vWeaponInfoList.size();
	for( int i=0; i<iCnt; ++i )
	{
		char szKey[MAX_PATH] = "";
		wsprintf( szKey, "frozen_charge_special_effect_angle%d", i+1 );
		float fAngle = rkLoader.LoadFloat( szKey, 0.0f );
		m_vFrozenChargeSpecialEffectAngle.push_back( fAngle );
	}

	rkLoader.LoadString_e( "red_team_circle", "", szBuf, MAX_PATH );
	m_RedEffect = szBuf;
	rkLoader.LoadString_e( "blue_team_circle", "", szBuf, MAX_PATH );
	m_BlueEffect = szBuf;
	m_fRandomRange = rkLoader.LoadFloat_e( "frozen_charge_special_random_range", 0.0f );

	m_iFrozenEffectlimit = rkLoader.LoadInt( "frozen_charge_special_effect_limit", 0.0f );	

	rkLoader.LoadString_e( "arrow_charge_special_effect", "", szBuf, MAX_PATH );
	m_ArrowChargeSpecialEffect  = szBuf;
	m_fArrowChargeSpecialOffset = rkLoader.LoadFloat_e( "arrow_charge_special_effect_offset", FLOAT1 );
	m_fArrowChargeSpecialEffectAngle = rkLoader.LoadFloat_e( "arrow_charge_special_effect_angle", FLOAT1 );
}

ioItem* ioFrozenItem::Clone()
{
	return new ioFrozenItem( *this );
}

ioWeaponItem::WeaponSubType ioFrozenItem::GetSubType() const
{
	return WST_FROZEN_ITEM;
}

void ioFrozenItem::SetNormalAttack( ioBaseChar *pOwner, int iCurCombo )
{
	if( NULL == pOwner ) return;

	ClearCancelInfo();

	m_bChargeLoopAniStart		= false;
	m_dwLoopLoopChargeCheckTime	= 0;

	DWORD dwCurTime = FRAMEGETTIME();
	int nComboCnt = GetMaxCombo();
	if( !COMPARE( iCurCombo, 0, nComboCnt ) )
	{
		ioWeaponItem::SetNormalAttack( pOwner, iCurCombo );

		pOwner->SetAttackMoveEnable( false );
		pOwner->SetChargingState( false );

		m_iCurCombo		= 0;
		m_FrozenState	= FS_NONE;
	}
	else
	{
		m_iCurCombo	= iCurCombo;
		ChangeToCharging( pOwner );
	}
}

void ioFrozenItem::ChangeToCharging( ioBaseChar *pOwner )
{
	if( NULL == pOwner ) return;

	// 공격 준비 애니메이션 설정
	ioEntityGroup *pGrp = pOwner->GetGroup();
	if( NULL == pGrp )	{	return;		}

	pGrp->ClearAllLoopAni( FLOAT100, true );
	pGrp->ClearAllActionAni( FLOAT100, true );

	// 공격 준비 시작
	int iAniID = pGrp->GetAnimationIdx( m_szDelayAnimation );

	float fTimeRate = (float)m_dwChargeTime / pGrp->GetAnimationFullTime( iAniID );
	pGrp->SetActionAni( iAniID, FLOAT100, FLOAT100, FLOAT1, FLOAT1/fTimeRate, 0.0f, 0.0f, true );

	pOwner->SetState( CS_ATTACK );
	pOwner->SetAttackMoveEnable( false );
	pOwner->SetChargingState( true );

	m_FrozenState = FS_CHARGING;
	m_dwChargeStartTime	= FRAMEGETTIME();

	if( pOwner->IsNeedSendNetwork() )
	{
		SP2Packet kPacket( CUPK_ITEM_GAUGE_OK );
		FillItemGaugePacketHeader( pOwner, kPacket );
		kPacket << (int)m_FrozenState;
		kPacket << m_iCurCombo;
		P2PNetwork::SendToAllPlayingUser( kPacket );
	}
}

void ioFrozenItem::OnCharging( ioBaseChar* pOwner )
{
	if( !pOwner || !pOwner->IsNeedProcess() )
	{
		return;
	}

	if( pOwner->IsAttackKeyDown() )
	{
		if( m_dwChargeStartTime + m_dwChargeTime <= FRAMEGETTIME() )
		{
			if( m_fCurGauge > m_fChargeNeedGauge )
			{
				int iCurGauge = m_fCurGauge - m_fChargeNeedGauge;
				SetCurBullet( iCurGauge );
				ChangeToReadyAttackFire( pOwner );
			}
			else
			{
				// 기본 공격 1타,2타,3타
				ChangeToAttackFire( pOwner );
			}
		}
	}
	else
	{
		if( m_dwChargeStartTime + m_dwHalfChargeTime <= FRAMEGETTIME() )
		{
			// 게이지 사용 2020-03-23
			if( (GetCurBullet() >= GetNeedBullet()) && (false == m_bFrozenDummyStart) )
			{
				ChangeToHalfAttackFire( pOwner );

				// 게이지 감소 
				CheckDecreaseGauge();
			}
			else	// 2020-03-23
			{
				m_FrozenState	= FS_NONE;
			}
		}
		else
		{
			// 기본 공격 1타,2타,3타
			ChangeToAttackFire( pOwner );
		}
	}
}

void ioFrozenItem::ChangeToAttackFire( ioBaseChar *pOwner )
{
	if( !pOwner )	{	return;	}

	DWORD dwCurTime = FRAMEGETTIME();
	pOwner->InitExtendAttackTagInfo();
	ClearCancelInfo();

	const AttackAttribute *pAttribute = m_FormalAttack.GetAttribute( AT_NORMAL, m_iCurCombo );
	if( pAttribute )
	{
		pOwner->SetNormalAttackByAttribute( *pAttribute );

		m_szCurAni					= pAttribute->m_AttackAnimation;
		m_fCurAniRate				= pAttribute->m_fAttackAniRate;
		m_dwInputCancelCheckTime	= dwCurTime;
		m_dwCancelCheckTime			= dwCurTime;
	}
	pOwner->ApplyNormalAttackResult( m_iCurCombo );
	m_FrozenState = FS_NORMAL_ATTACK_FIRE;

	if( pOwner->IsNeedSendNetwork() )
	{
		SP2Packet kPacket( CUPK_ITEM_GAUGE_OK );
		FillItemGaugePacketHeader( pOwner, kPacket );
		kPacket << (int)m_FrozenState;
		kPacket << m_iCurCombo;
		P2PNetwork::SendToAllPlayingUser( kPacket );
	}
}

void ioFrozenItem::ChangeToReadyAttackFire( ioBaseChar *pOwner )
{
	if( !pOwner )	{	return;	}

	ioEntityGroup *pGrp = pOwner->GetGroup();
	if( !pGrp )	{	return;	}

	m_FrozenState = FS_CHARGING_LOOP_ATTACK_FIRE;

	int iAniID = pGrp->GetAnimationIdx( m_szChargeReadyAnimation );
	pGrp->ClearAllActionAni( FLOAT10, true );
	pGrp->SetActionAni( iAniID, FLOAT100, FLOAT100 );
	m_dwChargeReadyAnimationTime = FRAMEGETTIME() + pGrp->GetAnimationFullTime( iAniID );

	// 전방에 매직 서클이 생성 
	ioHashString szDummyCircle;
	pOwner->CreateMagicCircle( m_szReadyCircleEffect, m_szEnableCircleEffect, szDummyCircle, m_fMagicCircleOffSet );

	if( pOwner->IsNeedSendNetwork() )
	{
		SP2Packet kPacket( CUPK_ITEM_GAUGE_OK );
		FillItemGaugePacketHeader( pOwner, kPacket );
		kPacket << (int)FS_CHARGING_READY_ATTACK_FIRE;
		kPacket << m_iCurCombo;
		P2PNetwork::SendToAllPlayingUser( kPacket );
	}
}

void ioFrozenItem::CheckNormalAttack( ioBaseChar *pOwner )
{
	switch( m_FrozenState )
	{
		case FS_NONE:
		{
			// 2020-03-24
			ioEntityGroup *pGrp = pOwner->GetGroup();
			if( pGrp )
			{
				pGrp->ClearAllLoopAni( FLOAT100, true );
				pGrp->ClearAllActionAni( FLOAT100, true );
			}
			pOwner->SetState( CS_DELAY );
		}
		break;
		case FS_DELAY:
		{
			if( m_dwAniEndTime < FRAMEGETTIME() )
			{
				ioEntityGroup *pGrp = pOwner->GetGroup();
				if( pGrp )
				{
					pGrp->ClearAllLoopAni( FLOAT100, true );
					pGrp->ClearAllActionAni( FLOAT100, true );
				}				
				pOwner->SetState( CS_DELAY );				
			}
		}
		break;
		case FS_CHARGING:
		{
			OnCharging( pOwner );
		}
		break;
		case FS_NORMAL_ATTACK_FIRE:
		{
			ioAttackableItem::CheckNormalAttack( pOwner );
		}
		break;
		case FS_CHARGING_LOOP_ATTACK_FIRE:
		{
			ChangeToLoopAttackFire( pOwner );
		}
		break;
		case FS_CHARGING_LOOP_CHARGING_ATTACK_FIRE:
		{
			OnChargingLoopChargingAttackFire( pOwner );
		}
		break;
		case FS_CHARGING_LOOP_NORMAL_CIRCLE_ACTION:
		{
			ChangeToChargingNormalCircleAction( pOwner );
		}
		break;
		case FS_CHARGING_LOOP_LOOP_FULL_CIRCLE_ACTION:
		{
			ChangeToChargingFullCircleAction( pOwner );
		}
		break;
		case FS_DUMMY_PROCESS:
		{
			ProcessFrozenDummy( pOwner );
		}
		break;
		case FS_HALF_ATTACK_FIRE:
		{
			if( pOwner->IsNeedProcess() )
			{
				CheckCancelReserve( pOwner );

				if( ProcessCancel(pOwner) )
					return;
			}

			ioWeaponItem::CheckNormalAttack( pOwner );
		}
		break;
	}
	
}

void ioFrozenItem::ChangeToLoopAttackFire( ioBaseChar* pOwner )
{
	if( !pOwner )	{	return;	}

	ioEntityGroup *pGrp = pOwner->GetGroup();
	if( !pGrp )	{	return;	}

	// 매직 서클
	if( pOwner->CheckMagicCircle() )
	{
		pOwner->CheckChangeDirectionForMagicCircle();
		pOwner->UpdateMagicCirclePosAll( m_fCircleMoveSpeed, m_fCircleRange, m_fCircleHeightGap, false );

		D3DXVECTOR3 vCharPos = pOwner->GetWorldPosition();
		D3DXVECTOR3 vCharDir = pOwner->GetMagicCirclePos() - vCharPos;
		vCharDir.y = 0.0f;

		D3DXVec3Normalize( &vCharDir, &vCharDir );

		pOwner->SetTargetRotToDir( vCharDir, true );
	}

	// loop ani
	if( !m_bChargeLoopAniStart && m_dwChargeReadyAnimationTime < FRAMEGETTIME() )
	{
		int iAniID = pGrp->GetAnimationIdx( m_szChargeLoopAnimation );

		pGrp->ClearAllLoopAniExceptOne( iAniID, FLOAT100, true );
		pGrp->ClearAllActionAni( FLOAT10, true );
		pGrp->SetLoopAni( iAniID, FLOAT100 );
		pOwner->SetAttackMoveEnable( false );
		pOwner->ChangeDirectionByInputDir( true );

		m_bChargeLoopAniStart	= true;
	}

	if( !pOwner->IsNeedProcess() )
	{
		return;
	}

	m_iCurMultiShot				= m_iMultiShotMin;	// +D 일반공격 할때 몇발 
	m_CurrentChargeAttack		= &m_FullAttackMin;	// +D 공격
	m_CurrentChargeAttackMax	= &m_FullAttackMax;	// D~ 공격


	// D~ 누른 상태에서 떼기 시 2020-03-06
	if( !pOwner->IsAttackKeyDown() && !m_bLoopAttackKeyDown )
	{
		// true로 변경하여 +D, D~ 키를 사용할수 있다. 
		m_bLoopAttackKeyDown = true;
	}

	if( pOwner->IsAttackKeyDown() && m_bLoopAttackKeyDown )
	{
		ChangeToLoopReadyAttackFire( pOwner );
	}
}

void ioFrozenItem::ChangeToLoopReadyAttackFire( ioBaseChar* pOwner )
{
	if( !pOwner )	{	return;	}

	ioEntityGroup *pGrp = pOwner->GetGroup();
	if( !pGrp )	{	return;	}

	m_FrozenState				= FS_CHARGING_LOOP_CHARGING_ATTACK_FIRE;
	m_dwLoopLoopChargeCheckTime = 0;

	// 2020-03-24
	pGrp->ClearAllLoopAni( FLOAT100, true );
	pGrp->ClearAllActionAni( FLOAT100, true );

	int iAniID = pGrp->GetAnimationIdx( m_szChargeLoopReadyAnimation );
	float fTimeRate = (float)m_dwChargeTime / pGrp->GetAnimationFullTime( iAniID );
	pGrp->SetActionAni( iAniID, FLOAT100, FLOAT100, FLOAT1, FLOAT1/fTimeRate, 0.0f, 0.0f, true );
	m_dwChargeLoopStartTime = FRAMEGETTIME() + pGrp->GetAnimationFullTime( iAniID );

	if( pOwner->IsNeedSendNetwork() )
	{
		SP2Packet kPacket( CUPK_ITEM_GAUGE_OK );
		FillItemGaugePacketHeader( pOwner, kPacket );
		kPacket << (int)FS_CHARGING_LOOP_READY_ATTACK_FIRE;
		kPacket << m_iCurCombo;
		P2PNetwork::SendToAllPlayingUser( kPacket );
	}
}

void ioFrozenItem::OnChargingLoopChargingAttackFire( ioBaseChar* pOwner )
{
	if( !pOwner || !pOwner->IsNeedProcess() )
	{
		return;
	}

	if( pOwner->IsAttackKeyDown() )
	{
		if( m_dwChargeLoopStartTime + m_dwChargeTime <= FRAMEGETTIME() )
		{
			ChangeToChargingFullAttackFire( pOwner );
		}
	}
	else
	{
		// 2020-03-09
		if( m_bChargeLoopLoopAniStart )
		{
			m_bChargeLoopLoopAniStart = false;
			OnChangeToChargingFullAttackFire( pOwner );
		}
		else
		{
			ChangeToChargingNormalAttackFire( pOwner );
		}
	}
}


void ioFrozenItem::ChangeToChargingFullAttackFire( ioBaseChar* pOwner )
{
	if( !pOwner )	{	return;		}

	ioEntityGroup *pGrp = pOwner->GetGroup();
	if( !pGrp )	{	return;		}

	if( pOwner->CheckMagicCircle() )
	{
		pOwner->CheckChangeDirectionForMagicCircle();
		pOwner->UpdateMagicCirclePosAll( m_fCircleMoveSpeed, m_fCircleRange, m_fCircleHeightGap, false );

		D3DXVECTOR3 vCharPos = pOwner->GetWorldPosition();
		D3DXVECTOR3 vCharDir = pOwner->GetMagicCirclePos() - vCharPos;
		vCharDir.y = 0.0f;

		D3DXVec3Normalize( &vCharDir, &vCharDir );

		pOwner->SetTargetRotToDir( vCharDir, true );
		// 뱡향 변화 체크
		pOwner->CheckChangeDirectionForMagicCircle();
	}

	// loop ani
	if( !m_bChargeLoopLoopAniStart && m_dwChargeLoopStartTime < FRAMEGETTIME() )
	{
		int iAniID = pGrp->GetAnimationIdx( m_szChargeLoopLoopAnimation );

		pGrp->ClearAllLoopAniExceptOne( iAniID, FLOAT100, true );
		pGrp->ClearAllActionAni( FLOAT10, true );
		pGrp->SetLoopAni( iAniID, FLOAT100 );
		pOwner->SetAttackMoveEnable( false );
		pOwner->ChangeDirectionByInputDir( true );

		m_bChargeLoopLoopAniStart	= true;
		m_vWeaponMoveInfoList.clear();

		if( pOwner->IsNeedProcess() )
		{
			m_dwLoopLoopChargeCheckTime = FRAMEGETTIME() + m_dwMultiShotGaugeTime;
			m_iCurShotGaugeMax	= 0;
			m_iMultShotGaugeMax = 1;	// 처음 한발 저장
			// 한발 증가 될때 마다 게이지 소멸
			int iCurGauge = m_fCurGauge - m_iMultShotGaugeMax;
			SetCurBullet( iCurGauge );

			// WeaponMove
			DWORD dwSeed = pOwner->GetRandomSeed();			
			m_random.SetRandomSeed( timeGetTime() );
	
			int iFrozenEffectlimit = m_iMultShotGaugeMax % m_iFrozenEffectlimit;
			if( iFrozenEffectlimit <= 0 )
			{
				CreateChargeSpecialEffect( pOwner );
			}
		}


		if( pOwner->IsNeedSendNetwork() )
		{
			SP2Packet kPacket( CUPK_ITEM_GAUGE_OK );
			FillItemGaugePacketHeader( pOwner, kPacket );
			kPacket << (int)FS_CHARGING_LOOP_LOOP_FIRE;
			kPacket << m_iCurCombo;
			P2PNetwork::SendToAllPlayingUser( kPacket );
		}
	}

	m_bLoopAttackKeyDown = false;
	UpdateChargeSpecialEffect( pOwner );

	if( !pOwner->IsNeedProcess() )	{	return;	}

	if( m_fCurGauge > 0 )
	{
		// D~ 루프중 D~ 루프일때 추가 게이지 tic을 사용하여 얼음 화실 개수를 증가 시킨다.
		if( m_dwLoopLoopChargeCheckTime < FRAMEGETTIME() )
		{
			if( m_iMultShotGaugeMax >= (int)m_FrozenSpecialAttack.m_vWeaponInfoList.size() )
			{
				m_iMultShotGaugeMax = (int)m_FrozenSpecialAttack.m_vWeaponInfoList.size();
				OnChangeToChargingFullAttackFire( pOwner );
				return;
			}

			m_dwLoopLoopChargeCheckTime = FRAMEGETTIME() + m_dwMultiShotGaugeTime;

			// 게이지가 소멸되면 자동으로 공격한다. 
			++m_iMultShotGaugeMax; // 총몇발 
			int iCurGauge = m_fCurGauge - 1;
			SetCurBullet( iCurGauge  );

			// 2020-03-17
			int iFrozenEffectlimit = m_iMultShotGaugeMax % m_iFrozenEffectlimit;
			if( iFrozenEffectlimit <= 0 )
			{
				CreateChargeSpecialEffect( pOwner );
			}
		}
	}

	// 게이지가 소멸 되거나, 키를 떼기 시 공격한다.
	if( m_fCurGauge <= 0  )
	{
		OnChangeToChargingFullAttackFire( pOwner );
	}
}

void ioFrozenItem::OnChangeToChargingFullAttackFire( ioBaseChar* pOwner )
{
	if( !pOwner )	{	return;		}

	pOwner->SetAttackMoveEnable( false );
	pOwner->ChangeDirectionByInputDir( true );
	pOwner->InitExtendAttackTagInfo();

	m_bChargeLoopLoopAniStart	= false;

	// 마지막 좌표 위치 저장
	RandomPositionFullAttackFire( pOwner );

	DestroyChargeSpecialEffect( pOwner );
	pOwner->DestroyMagicCircle();

	m_dwFireEndTime	= 0;
	m_dwAniEndTime	= 0;

	if( !pOwner->IsNeedProcess() ) {		return;		}

	// D~ + D~ 누르고 있는 동안 얼음 화살에 이펙트 2020-03-20
	CreateArrowChargeSpecialEffect( pOwner );

	m_FrozenState	= FS_CHARGING_LOOP_LOOP_FULL_CIRCLE_ACTION;
	if( pOwner->IsNeedSendNetwork() )
	{
		SP2Packet kPacket( CUPK_ITEM_GAUGE_CANCEL );
		FillItemGaugePacketHeader( pOwner, kPacket );
		kPacket << (int)FSS_Full_ATTACK_FIRE;
		kPacket << m_iCurCombo;
		P2PNetwork::SendToAllPlayingUser( kPacket );
	}
}
 
void ioFrozenItem::ChangeToChargingFullCircleAction( ioBaseChar* pOwner )
{
	if( !pOwner || !pOwner->IsNeedProcess() )	{		return;		}

	ioEntityGroup *pGrp = pOwner->GetGroup();
	if( !pGrp )		{	return;		}


	// D~ + D~ 누르고 있는 동안 얼음 화살에 이펙트
	UpdateArrowChargeSpecialEffect( pOwner );

	DWORD dwCurTime = FRAMEGETTIME();
	if( m_iCurShotGaugeMax >= m_iMultShotGaugeMax  )
	{
		m_FrozenState	= FS_DELAY;

		// D~ + D~ 누르고 있는 동안 얼음 화살에 이펙트
		DestroyArrowChargeSpecialEffect( pOwner );
		m_CurrentChargeAttack		= NULL;
		m_CurrentChargeAttackMax	= NULL;
		m_bChargeLoopLoopAniStart	= false;

		if( pOwner->IsNeedSendNetwork() )
		{
			SP2Packet kPacket( CUPK_ITEM_GAUGE_OK );
			FillItemGaugePacketHeader( pOwner, kPacket );
			kPacket << (int)m_FrozenState;
			kPacket << m_iCurCombo;
			P2PNetwork::SendToAllPlayingUser( kPacket );
		}

		return;
	}

	DWORD dwWeaponBaseIndex = pOwner->IncreaseWeaponIndexBase();
	D3DXVECTOR3 vPos = pOwner->GetMidPositionByRate( 1.0f );

	if( m_dwLoopLoopChargeCheckTime < FRAMEGETTIME() && (m_iCurShotGaugeMax < m_iMultShotGaugeMax) )
	{
		m_dwLoopLoopChargeCheckTime = FRAMEGETTIME() + m_dwChargeAttackTerm;		

		if( COMPARE( m_iCurShotGaugeMax, 0, (int)m_FrozenSpecialAttack.m_vWeaponInfoList.size() ) )
		{
			m_dwAniEndTime = 0;

			D3DXVECTOR3 vDir	= ioMath::VEC3_ZERO;
			float fSpeed		= 0.0f;
			float fFloat		= 0.0f;
			if( COMPARE( m_iCurShotGaugeMax, 0, (int)m_vWeaponMoveInfoList.size() ) )
			{				
				vDir = m_vWeaponMoveInfoList[m_iCurShotGaugeMax].m_vMoveDir;
				fSpeed = m_vWeaponMoveInfoList[m_iCurShotGaugeMax].m_fMoveSpeed;
				fFloat = m_vWeaponMoveInfoList[m_iCurShotGaugeMax].m_fFloatPower;

				D3DXVECTOR3	vWeaponDir;
				D3DXVec3Normalize( &vWeaponDir, &vDir );	

				FireTime kFireTime;
				kFireTime.dwStart	= FRAMEGETTIME();
				kFireTime.iAttributeIdx			= m_FrozenSpecialAttack.m_vWeaponInfoList[m_iCurShotGaugeMax].m_iWeaponIdx;
				kFireTime.iResistanceIdx		= m_FrozenSpecialAttack.m_vWeaponInfoList[m_iCurShotGaugeMax].m_iResistanceIdx;
				kFireTime.szWoundedAni			= m_FrozenSpecialAttack.m_vWeaponInfoList[m_iCurShotGaugeMax].m_WoundedAnimation;
				kFireTime.dwWoundedAniDuration	= m_FrozenSpecialAttack.m_vWeaponInfoList[m_iCurShotGaugeMax].m_dwWoundedDuration;
				kFireTime.bLoopWoundedAni		= m_FrozenSpecialAttack.m_vWeaponInfoList[m_iCurShotGaugeMax].m_bWoundedLoopAni;

				kFireTime.dwWeaponIdx			= dwWeaponBaseIndex++;
				kFireTime.eFireTimeType			= FTT_NORMAL_ATTACK;

				ioWeapon*	pWeapon	= pOwner->ExtendFireExplicit( kFireTime, vPos, vWeaponDir, "" );
				if( pWeapon )
				{					
					pWeapon->SetMoveDir( vDir );
					pWeapon->SetMoveSpeed( fSpeed );
					pWeapon->SetFloatPower( fFloat );
				}
			}

			if( pOwner->IsNeedSendNetwork() )
			{
				SP2Packet kPacket( CUPK_ITEM_GAUGE_OK );
				FillItemGaugePacketHeader( pOwner, kPacket );
				kPacket << (int)m_FrozenState;
				kPacket << m_iCurCombo;		
				kPacket << vPos;
				kPacket << vDir;
				kPacket << pOwner->GetMagicCirclePos();
				kPacket << fSpeed;
				kPacket << fFloat;
				kPacket << m_iCurShotGaugeMax;
				P2PNetwork::SendToAllPlayingUser( kPacket );
			}

			++m_iCurShotGaugeMax;

			int	iAniID = pGrp->GetAnimationIdx( m_FrozenSpecialAttack.m_AttackAnimation );
			if( iAniID == -1 )
				return;

			float	fTimeRate   = m_FrozenSpecialAttack.m_fAttackAniRate;
			DWORD	dwPreDelay  = m_FrozenSpecialAttack.m_dwPreDelay;

			pGrp->ClearAllLoopAni( FLOAT100, true );
			pGrp->ClearAllActionAni( FLOAT100, true );
			

			m_dwAniEndTime = dwCurTime + pGrp->GetAnimationFullTime( iAniID ) * fTimeRate + dwPreDelay;

			if( m_FrozenSpecialAttack.m_AttackEndAnimation.IsEmpty() )
			{
				pGrp->SetActionAni( iAniID, FLOAT100, FLOAT100, FLOAT1, FLOAT1/fTimeRate, 0.0f, dwPreDelay, true );
			}
			else
			{
				pGrp->SetActionAni( iAniID, FLOAT100, 0.0f, FLOAT1, FLOAT1/fTimeRate, 0.0f, dwPreDelay );

				pGrp->SetActionAni( m_FrozenSpecialAttack.m_AttackEndAnimation,
					0.0f, FLOAT100,
					1.0f, FLOAT1/m_FrozenSpecialAttack.m_fAttackEndAniRate,
					m_dwAniEndTime, 0.0f, true );
			}
		}
	}
}

void ioFrozenItem::OnChangeToChargingFullCircleAction( ioBaseChar *pOwner, D3DXVECTOR3& target, D3DXVECTOR3& vPos, D3DXVECTOR3& vDir, float fSpeed, float fFloat, int iCurShotGaugeMax )
{
	ioEntityGroup *pGrp = pOwner->GetGroup();
	if( !pGrp )	{	return;		}	

	// D~ + D~ 누르고 있는 동안 얼음 화살에 이펙트
	UpdateArrowChargeSpecialEffect( pOwner );

	DWORD dwWeaponBaseIndex = pOwner->IncreaseWeaponIndexBase();

	D3DXVECTOR3	vWeaponDir;
	D3DXVec3Normalize( &vWeaponDir, &vDir );

	FireTime kFireTime;
	kFireTime.dwStart	= FRAMEGETTIME();
	kFireTime.iAttributeIdx			= m_FrozenSpecialAttack.m_vWeaponInfoList[iCurShotGaugeMax].m_iWeaponIdx;
	kFireTime.iResistanceIdx		= m_FrozenSpecialAttack.m_vWeaponInfoList[iCurShotGaugeMax].m_iResistanceIdx;
	kFireTime.szWoundedAni			= m_FrozenSpecialAttack.m_vWeaponInfoList[iCurShotGaugeMax].m_WoundedAnimation;
	kFireTime.dwWoundedAniDuration	= m_FrozenSpecialAttack.m_vWeaponInfoList[iCurShotGaugeMax].m_dwWoundedDuration;
	kFireTime.bLoopWoundedAni		= m_FrozenSpecialAttack.m_vWeaponInfoList[iCurShotGaugeMax].m_bWoundedLoopAni;

	kFireTime.dwWeaponIdx			= dwWeaponBaseIndex++;
	kFireTime.eFireTimeType			= FTT_NORMAL_ATTACK;

	ioWeapon*	pWeapon	= pOwner->ExtendFireExplicit( kFireTime, vPos, vWeaponDir, "" );
	ioFloatThrowWeapon*	pFloatWeapon	= ToFloatThrowWeapon( pWeapon );
	if( pFloatWeapon )
	{		
		pFloatWeapon->SetMoveSpeed( fSpeed );
		pFloatWeapon->SetFloatPower( fFloat );
		pFloatWeapon->SetGravityRate( m_fWeaponGravityRate );
	}	

	// 에니메이션 처리
	int	iAniID = pGrp->GetAnimationIdx( m_FrozenSpecialAttack.m_AttackAnimation );
	if( iAniID == -1 )
		return;

	float	fTimeRate   = m_FrozenSpecialAttack.m_fAttackAniRate;
	DWORD	dwPreDelay  = m_FrozenSpecialAttack.m_dwPreDelay;

	pGrp->ClearAllLoopAni( FLOAT100, true );
	pGrp->ClearAllActionAni( FLOAT100, true );

	DWORD	dwFireEndTime	= pGrp->GetAnimationFullTime( iAniID ) * fTimeRate + dwPreDelay;

	if( m_FrozenSpecialAttack.m_AttackEndAnimation.IsEmpty() )
	{
		pGrp->SetActionAni( iAniID, FLOAT100, FLOAT100, FLOAT1, FLOAT1/fTimeRate, 0.0f, dwPreDelay, true );
	}
	else
	{
		pGrp->SetActionAni( iAniID, FLOAT100, 0.0f, FLOAT1, FLOAT1/fTimeRate, 0.0f, dwPreDelay );

		pGrp->SetActionAni( m_FrozenSpecialAttack.m_AttackEndAnimation,
			0.0f, FLOAT100,
			1.0f, FLOAT1/m_FrozenSpecialAttack.m_fAttackEndAniRate,
			dwFireEndTime, 0.0f, true );
	}
}

void ioFrozenItem::OnChangeToChargingNormalAttackFire( ioBaseChar* pOwner )
{

	if( !pOwner )					{	return;		}

	ioEntityGroup *pGrp = pOwner->GetGroup();
	if( !pGrp )	{	return;		}

	pOwner->SetAttackMoveEnable( false );
	pOwner->ChangeDirectionByInputDir( true );
	pOwner->InitExtendAttackTagInfo();

	m_dwFireEndTime	= 0;
	m_dwAniEndTime	= 0;
}

void ioFrozenItem::OnChangeToChargingAniNormalAttackFire( ioBaseChar* pOwner )
{
	if( !pOwner )	{	return;		}

	ioEntityGroup *pGrp = pOwner->GetGroup();
	if( !pGrp )	{	return;		}

	m_CurrentChargeAttack = &m_FullAttackMin;	// +D 공격
	if( !m_CurrentChargeAttack )
	{
		return;
	}	

	// 에니메이션 처리
	int	iAniID = pGrp->GetAnimationIdx( m_CurrentChargeAttack->m_AttackAnimation );
	if( iAniID == -1 )
		return;

	float	fTimeRate   = m_CurrentChargeAttack->m_fAttackAniRate;
	DWORD	dwPreDelay  = m_CurrentChargeAttack->m_dwPreDelay;

	pGrp->ClearAllLoopAni( FLOAT100, true );
	pGrp->ClearAllActionAni( FLOAT100, true );

	m_dwAniEndTime = FRAMEGETTIME() + pGrp->GetAnimationFullTime( iAniID ) * fTimeRate + dwPreDelay;
	if( m_CurrentChargeAttack->m_AttackEndAnimation.IsEmpty() )
	{
		pGrp->SetActionAni( iAniID, FLOAT100, FLOAT100, FLOAT1, FLOAT1/fTimeRate, 0.0f, dwPreDelay, true );
	}
	else
	{
		pGrp->SetActionAni( iAniID, FLOAT100, 0.0f, FLOAT1, FLOAT1/fTimeRate, 0.0f, dwPreDelay );

		pGrp->SetActionAni( m_CurrentChargeAttack->m_AttackEndAnimation,
			0.0f, FLOAT100,
			1.0f, FLOAT1/m_CurrentChargeAttack->m_fAttackEndAniRate,
			m_dwAniEndTime, 0.0f, true );
	}
}

void ioFrozenItem::ChangeToChargingNormalAttackFire( ioBaseChar* pOwner )
{
	if( !pOwner )					{	return;		}
	if( !pOwner->IsNeedProcess() )	{	return;		}

	ioEntityGroup *pGrp = pOwner->GetGroup();
	if( !pGrp )	{	return;		}

	m_bLoopAttackKeyDown = false;
	m_FrozenState	= FS_CHARGING_LOOP_NORMAL_CIRCLE_ACTION;

	pOwner->SetAttackMoveEnable( false );
	pOwner->ChangeDirectionByInputDir( true );
	pOwner->InitExtendAttackTagInfo();

	m_dwChargeAttackStartTime	= FRAMEGETTIME();

	if( pOwner->IsNeedSendNetwork() )
	{
		SP2Packet kPacket( CUPK_ITEM_GAUGE_CANCEL );
		FillItemGaugePacketHeader( pOwner, kPacket );
		kPacket << (int)FSS_NORMAL_ATTACK_FIRE;
		kPacket << m_iCurCombo;
		P2PNetwork::SendToAllPlayingUser( kPacket );
	}

	m_bLoopAttackKeyDown = false;


	m_iMultShotGaugeMax = 1;	// 처음 한발 저장
	RandomPositionFullAttackFire( pOwner );
	m_iMultShotGaugeMax = 0;

	m_dwFireEndTime	= 0;
	m_dwAniEndTime	= 0;
}

void ioFrozenItem::OnChangeToChargingNormalCircleAction( ioBaseChar *pOwner, D3DXVECTOR3& target, D3DXVECTOR3& vPos, D3DXVECTOR3& vDir, float fSpeed, float fFloat, int iCurShotGaugeMax )
{
	if( !pOwner )	{	return;		}

	ioEntityGroup *pGrp = pOwner->GetGroup();
	if( !pGrp )	{	return;		}

	m_CurrentChargeAttack = &m_FullAttackMin;	// +D 공격
	if( !m_CurrentChargeAttack )
	{
		return;
	}	

	DWORD dwWeaponBaseIndex = pOwner->IncreaseWeaponIndexBase();

	D3DXVECTOR3	vWeaponDir;
	D3DXVec3Normalize( &vWeaponDir, &vDir );

	FireTime kFireTime;
	kFireTime.dwStart	= FRAMEGETTIME();

	kFireTime.iAttributeIdx			= m_CurrentChargeAttack->m_vWeaponInfoList[0].m_iWeaponIdx;
	kFireTime.iResistanceIdx		= m_CurrentChargeAttack->m_vWeaponInfoList[0].m_iResistanceIdx;
	kFireTime.szWoundedAni			= m_CurrentChargeAttack->m_vWeaponInfoList[0].m_WoundedAnimation;
	kFireTime.dwWoundedAniDuration	= m_CurrentChargeAttack->m_vWeaponInfoList[0].m_dwWoundedDuration;
	kFireTime.bLoopWoundedAni		= m_CurrentChargeAttack->m_vWeaponInfoList[0].m_bWoundedLoopAni;

	kFireTime.dwWeaponIdx			= dwWeaponBaseIndex++;
	kFireTime.eFireTimeType			= FTT_NORMAL_ATTACK;

	ioWeapon*	pWeapon	= pOwner->ExtendFireExplicit( kFireTime, vPos, vWeaponDir, "" );
	ioFloatThrowWeapon*	pFloatWeapon	= ToFloatThrowWeapon( pWeapon );
	if( pFloatWeapon )
	{
		GetChargingArrowSpeedNFloatingPower( pOwner, target, vDir, fSpeed, fFloat );
		pFloatWeapon->SetMoveSpeed( fSpeed );
		pFloatWeapon->SetFloatPower( fFloat );
		pFloatWeapon->SetGravityRate( m_fWeaponGravityRate );
	}

	m_FrozenState = FS_DELAY;
}

void ioFrozenItem::ChangeToChargingNormalCircleAction( ioBaseChar *pOwner )
{	
	if( !pOwner || !pOwner->IsNeedProcess() )
		return;

	if( !pOwner  )	{	return;	}

	ioEntityGroup *pGrp = pOwner->GetGroup();
	if( !pGrp )
		return;

	DWORD dwCurTime = FRAMEGETTIME();

	// 발사가 끝나면 
	if( m_iCurMultiShot <= 0 )
	{
		// 제자리 
		if( m_dwAniEndTime < dwCurTime )
		{
			m_FrozenState	= FS_NONE;

			pOwner->DestroyMagicCircle();
			pOwner->SetState( CS_DELAY );

			m_CurrentChargeAttack		= NULL;
			m_CurrentChargeAttackMax	= NULL;
			m_bChargeLoopAniStart		= false;
		}
		return;
	}

	if( m_dwChargeAttackStartTime < dwCurTime )
	{
		DWORD dwWeaponBaseIndex = pOwner->IncreaseWeaponIndexBase();
		D3DXVECTOR3 vPos = pOwner->GetMidPositionByRate( 1.0f );
		D3DXVECTOR3 vDir	= pOwner->GetMagicCircleRot() * D3DXVECTOR3( 0.0f, 0.0f, -FLOAT1 );

		vDir	= pOwner->GetMagicCirclePos() - pOwner->GetWorldPosition();

		D3DXVECTOR3	vWeaponDir;
		D3DXVec3Normalize( &vWeaponDir, &vDir );	

		if( !m_CurrentChargeAttack )
		{
			return;
		}	

		if( (m_dwFireEndTime > 0) && (m_dwFireEndTime < FRAMEGETTIME()) )
		{
			float fSpeed		= 0.0f;
			float fFloat		= 0.0f;		
			fSpeed = m_vWeaponMoveInfoList[m_iCurShotGaugeMax].m_fMoveSpeed;
			fFloat = m_vWeaponMoveInfoList[m_iCurShotGaugeMax].m_fFloatPower;

			FireTime kFireTime;
			kFireTime.dwStart	= FRAMEGETTIME();

			kFireTime.iAttributeIdx			= m_CurrentChargeAttack->m_vWeaponInfoList[0].m_iWeaponIdx;
			kFireTime.iResistanceIdx		= m_CurrentChargeAttack->m_vWeaponInfoList[0].m_iResistanceIdx;
			kFireTime.szWoundedAni			= m_CurrentChargeAttack->m_vWeaponInfoList[0].m_WoundedAnimation;
			kFireTime.dwWoundedAniDuration	= m_CurrentChargeAttack->m_vWeaponInfoList[0].m_dwWoundedDuration;
			kFireTime.bLoopWoundedAni		= m_CurrentChargeAttack->m_vWeaponInfoList[0].m_bWoundedLoopAni;

			kFireTime.dwWeaponIdx			= dwWeaponBaseIndex++;
			kFireTime.eFireTimeType			= FTT_NORMAL_ATTACK;

			ioWeapon*	pWeapon	= pOwner->ExtendFireExplicit( kFireTime, vPos, vWeaponDir, "" );
			ioFloatThrowWeapon*	pFloatWeapon	= ToFloatThrowWeapon( pWeapon );
			if( pFloatWeapon )
			{
				GetChargingArrowSpeedNFloatingPower( pOwner, pOwner->GetMagicCirclePos(), vDir, fSpeed, fFloat );
				pFloatWeapon->SetMoveSpeed( fSpeed );
				pFloatWeapon->SetFloatPower( fFloat );
				pFloatWeapon->SetGravityRate( m_fWeaponGravityRate );
			}

			if( pOwner->IsNeedSendNetwork() )
			{
				SP2Packet kPacket( CUPK_ITEM_GAUGE_OK );
				FillItemGaugePacketHeader( pOwner, kPacket );
				kPacket << (int)m_FrozenState;
				kPacket << m_iCurCombo;
				kPacket << vPos;
				kPacket << vDir;
				kPacket << pOwner->GetMagicCirclePos();
				kPacket << fSpeed;
				kPacket << fFloat;
				kPacket << m_iCurMultiShot;
				P2PNetwork::SendToAllPlayingUser( kPacket );
			}

			m_iCurMultiShot--;
		}

		if( m_dwFireEndTime <= 0 )
		{
			// 에니메이션 처리
			int	iAniID = pGrp->GetAnimationIdx( m_CurrentChargeAttack->m_AttackAnimation );
			if( iAniID == -1 )
				return;

			float	fTimeRate   = m_CurrentChargeAttack->m_fAttackAniRate;
			DWORD	dwPreDelay  = m_CurrentChargeAttack->m_dwPreDelay;

			pGrp->ClearAllLoopAni( FLOAT100, true );
			pGrp->ClearAllActionAni( FLOAT100, true );

			m_dwAniEndTime = dwCurTime + pGrp->GetAnimationFullTime( iAniID ) * fTimeRate + dwPreDelay;
			m_dwFireEndTime = pGrp->GetAnimationEventTime_e( iAniID, "fire_s" ) * fTimeRate + dwPreDelay;
			m_dwFireEndTime += dwCurTime;

			if( m_CurrentChargeAttack->m_AttackEndAnimation.IsEmpty() )
			{
				pGrp->SetActionAni( iAniID, FLOAT100, FLOAT100, FLOAT1, FLOAT1/fTimeRate, 0.0f, dwPreDelay, true );
			}
			else
			{
				pGrp->SetActionAni( iAniID, FLOAT100, 0.0f, FLOAT1, FLOAT1/fTimeRate, 0.0f, dwPreDelay );

				pGrp->SetActionAni( m_CurrentChargeAttack->m_AttackEndAnimation,
					0.0f, FLOAT100,
					1.0f, FLOAT1/m_CurrentChargeAttack->m_fAttackEndAniRate,
					m_dwAniEndTime, 0.0f, true );
			}

			SP2Packet kPacket( CUPK_ITEM_GAUGE_CANCEL );
			FillItemGaugePacketHeader( pOwner, kPacket );
			kPacket << (int)FSS_NORMAL_ANI_ATTACK_START;
			kPacket << m_iCurCombo;
			P2PNetwork::SendToAllPlayingUser( kPacket );
		}
	}
}

void ioFrozenItem::GetChargingArrowSpeedNFloatingPower( ioBaseChar *pOwner, D3DXVECTOR3& target, D3DXVECTOR3& dir, float& speed, float& floatingPower )
{
	// CharDir
	D3DXVECTOR3 vCharPos = pOwner->GetWorldPosition();

	// WeaponMove
	D3DXVECTOR3 vTargetPos = target;

	float fRate = FLOAT1;
	float fRange = D3DXVec3Length( &dir );

	float fCurWeaponFloatPower = 0.0f;
	float fCurWeaponMoveSpeed = 0.0f;
	if( fRange > 0.0f )
	{
		fRate = fRange / ( m_fCircleRange /*+ m_fWeaponGapDistance*/ );

		// CurFloat
		fCurWeaponFloatPower = m_fWeaponFloatPower * fRate;
		fCurWeaponFloatPower = max( fCurWeaponFloatPower, m_fWeaponFloatMinPower );

		float fCurGravity = 0.0f;
		float fWeaponMoveSpeed = 0.0f;
		ioPlayStage *pStage = pOwner->GetCreator();
		if( pStage )
			fCurGravity = pStage->GetRoomGravity() * m_fWeaponGravityRate;

		float fFlightDuration = 0.0f;
		float fGapHeight = pOwner->GetMidHeightByRate(m_fRevisionRate) - vCharPos.y;
		float fGapHeight2 = vCharPos.y - vTargetPos.y;
		if( fGapHeight2 > 0.0f )
		{
			fGapHeight += fGapHeight2;
			fFlightDuration = GetFlightDuration( fCurGravity, fCurWeaponFloatPower, fGapHeight );

			if( fFlightDuration > 0.0f )
				fWeaponMoveSpeed = ( fRange /*+ m_fWeaponGapDistance*/ ) / (fFlightDuration / FLOAT1000);

			fCurWeaponMoveSpeed = max( m_fWeaponMinSpeed, fWeaponMoveSpeed );
		}
		else if( COMPARE(fGapHeight2, -m_fUpLimitHeight, -20.0f) && fRange > m_fUpLimitRange )
		{
			float fNewGap = fabs( fGapHeight2 ) - fGapHeight;
			fFlightDuration = GetTargetTime( fCurGravity, fNewGap, fCurWeaponFloatPower );
			fRange -= 35.0f;

			if( fFlightDuration > 0.0f )
				fCurWeaponMoveSpeed = ( fRange /*+ m_fWeaponGapDistance*/ ) / (fFlightDuration / FLOAT1000);
		}
		else
		{
			fFlightDuration = GetFlightDuration( fCurGravity, fCurWeaponFloatPower, fGapHeight );

			if( fFlightDuration > 0.0f )
				fWeaponMoveSpeed = ( fRange /*+ m_fWeaponGapDistance*/ ) / (fFlightDuration / FLOAT1000);

			fCurWeaponMoveSpeed = max( m_fWeaponMinSpeed, fWeaponMoveSpeed );
		}
	}

	speed	= fCurWeaponMoveSpeed;
	floatingPower	= fCurWeaponFloatPower;
}

float ioFrozenItem::GetFlightDuration( float fCurGravity, float fCurWeaponFloatPower, float fGapHeight )
{
	float fTopTime, fTopHeight, fGapTime;
	fTopTime = fTopHeight = fGapTime = 0.0f;

	float fNewGravity = fCurGravity;
	if( fNewGravity > 0.0f )
		fTopTime = fCurWeaponFloatPower / fNewGravity;

	float fCurFloatPower = -fCurWeaponFloatPower;
	float fCurGapHeight = 0.0f;
	while( fCurGapHeight < fGapHeight )
	{
		fCurGapHeight += fabs(fCurFloatPower * 0.01f);
		fCurFloatPower -= fNewGravity * 0.01f;
		fGapTime += 0.01f;
	}

	float fResult = 2 * fTopTime * FLOAT1000;
	fResult += fGapTime * FLOAT1000;
	return fResult;
}

float ioFrozenItem::GetTargetTime( IN float fCurGravity, IN float fGapHeight, OUT float &fFloatPower )
{
	float fGapTime = 0.0f;
	float fCurFloatPower = 0;
	float fCurGapHeight = 0.0f;

	float fRevisionH = FLOAT100;
	float fTopHeight = fGapHeight + fRevisionH;

	// Top Check
	float fTopTime = 0.0f;
	do
	{
		fTopTime = fGapTime;

		fCurGapHeight += fabs(fCurFloatPower * 0.01f);
		fCurFloatPower += fCurGravity * 0.01;
		fGapTime += 0.01;
	} while( fCurGapHeight < fTopHeight );

	fFloatPower = fCurFloatPower;


	// Target Check
	fGapTime = 0.0f;
	fCurFloatPower = 0;
	fCurGapHeight = 0.0f;

	float fTargetTime = 0.18f;

	fTargetTime += fTopTime;
	fTargetTime *= FLOAT1000;
	return fTargetTime;
}

void ioFrozenItem::OnItemGaugeOK( ioBaseChar *pOwner, SP2Packet &rkPacket )
{
	int iState = 0;
	rkPacket >> iState;
	rkPacket >> m_iCurCombo;

	switch( iState )
	{
		case FS_DELAY:
		{
			ioEntityGroup *pGrp = pOwner->GetGroup();
			if( pGrp )
			{
				pGrp->ClearAllLoopAni( FLOAT100, true );
				pGrp->ClearAllActionAni( FLOAT100, true );
			}
			pOwner->SetState( CS_DELAY );
			m_CurrentChargeAttackMax	= NULL;
			m_CurrentChargeAttack		= NULL;
			m_bChargeLoopAniStart		= false;
		}
		break;

		case FS_CHARGING:
		{
			ChangeToCharging( pOwner );
		}
		break;

		case FS_CHARGING_READY_ATTACK_FIRE:
		{
			ChangeToReadyAttackFire( pOwner );
		}
		break;

		case FS_NORMAL_ATTACK_FIRE:
		{
			ChangeToAttackFire( pOwner );
		}
		break;
		case FS_CHARGING_LOOP_ATTACK_FIRE:
		{
			ChangeToLoopAttackFire( pOwner );
		}
		break;
		case FS_CHARGING_LOOP_READY_ATTACK_FIRE:
		{
			ChangeToLoopReadyAttackFire( pOwner );
		}
		break;
		case FS_CHARGING_LOOP_NORMAL_CIRCLE_ACTION:
		{
			D3DXVECTOR3 vPos, vDir, vTarget;
			D3DXQUATERNION	qtRot;
			float fSpeed, fFloat;					
			int iCurMultiShot;
			rkPacket >> vPos;
			rkPacket >> vDir;
			rkPacket >> vTarget;
			rkPacket >> fSpeed;
			rkPacket >> fFloat;
			rkPacket >> iCurMultiShot;
			OnChangeToChargingNormalCircleAction( pOwner, vTarget, vPos, vDir, fSpeed, fFloat, iCurMultiShot );
		}
		break;
		case FS_CHARGING_LOOP_LOOP_FIRE:
		{
			ChangeToChargingFullAttackFire( pOwner );
		}
		break;
		case FS_CHARGING_LOOP_LOOP_FULL_CIRCLE_ACTION:
		{
			D3DXVECTOR3 vPos, vDir, vTarget;
			D3DXQUATERNION	qtRot;
			float fSpeed, fFloat;
			int iCurShotGaugeMax;
			rkPacket >> vPos;
			rkPacket >> vDir;
			rkPacket >> vTarget;
			rkPacket >> fSpeed;
			rkPacket >> fFloat;
			rkPacket >> iCurShotGaugeMax;
			OnChangeToChargingFullCircleAction( pOwner, vTarget, vPos, vDir, fSpeed, fFloat, iCurShotGaugeMax );
		}
		break;
		case FS_DUMMY_CREATE:
		{
			ChangeToHalfAttackFire( pOwner );
		}
		break;
		case FS_DUMMY_DELETE:
		{
			bool bEffect;
			rkPacket >> bEffect;
			DestoryFrozenDummy( pOwner, bEffect );
		}
		break;
		case FS_HALF_ATTACK_FIRE:
		{
			ChangeToHalfAttackFire( pOwner );
		}
		break;
		case FS_SPECIAL_EFFECT_CREATE:
		{
			int iMultShotGaugeMax;
			rkPacket >> iMultShotGaugeMax;
			m_iMultShotGaugeMax = iMultShotGaugeMax;
			CreateChargeSpecialEffect( pOwner );
		}
		break;
		case FS_SPECIAL_EFFECT_DELETE:
		{
			DestroyChargeSpecialEffect( pOwner );
		}
		break;
		case FS_ARROW_EFFECT_CREATE:
		{
			CreateArrowChargeSpecialEffect( pOwner );
		}
		break;
		case FS_ARROW_EFFECT_DELETE:
		{
			DestroyArrowChargeSpecialEffect( pOwner );
		}
		break;

	}
}

void ioFrozenItem::OnItemGaugeCancel( ioBaseChar *pOwner, SP2Packet &rkPacket )
{
	int iState		= 0;
	int iCurCombo	= 0;
	rkPacket >> iState;
	rkPacket >> iCurCombo;

	switch( iState )
	{
		case FSS_NORMAL_ATTACK_FIRE:
		{
			OnChangeToChargingNormalAttackFire( pOwner );
		}
		break;

		case FSS_Full_ATTACK_FIRE:
		{
			OnChangeToChargingFullAttackFire( pOwner );
		}
		break;
		case FSS_NORMAL_ANI_ATTACK_START:
		{
			OnChangeToChargingAniNormalAttackFire( pOwner );
		}
		break;
	}
}


void ioFrozenItem::CheckDecreaseGauge()
{
	m_fCurGauge -= m_fNeedGauge;
	m_fCurGauge = (float)max( 0, min( m_fCurGauge, m_fMaxGauge ) );
}

void ioFrozenItem::CheckAddDecreaseGauge()
{
	m_fCurGauge -= m_fAddNeedGauge;
	m_fCurGauge = (float)max( 0, min( m_fCurGauge, m_fMaxGauge ) );
}

int ioFrozenItem::GetMaxBullet()
{
	return (int)m_fMaxGauge;
}

int ioFrozenItem::GetNeedBullet()
{
	return (int)m_fNeedGauge;
}

int ioFrozenItem::GetAddNeedBullet()
{
	return (int)m_fAddNeedGauge;
}

int ioFrozenItem::GetCurBullet()
{
	float fCurCoolTime = m_fCurGauge;
	fCurCoolTime = max( 0, min( fCurCoolTime, m_fMaxGauge ) );

	return (int)fCurCoolTime;
}

void ioFrozenItem::SetCurBullet( int iCurBullet )
{
	// 용병 교체 시 불렛 게이지 초기화 사용 유무
	if( m_bBulletGaugeInit )
	{
		return;
	}

	if( iCurBullet < 0 )
		return;

	float fCurBullet = iCurBullet;
	fCurBullet = max( 0, min( fCurBullet, m_fMaxGauge ) );

	m_fCurGauge = fCurBullet;
}

void ioFrozenItem::UpdateExtraData( ioBaseChar *pOwner )
{
	if( NULL == pOwner )	{	return;		}

	float fTimePerSec = g_FrameTimer.GetSecPerFrame();

	CharState cState = pOwner->GetState();
	float	fIncreaseGauge	= 0.0f;

	switch( cState )
	{
		case CS_DELAY:
		{
			fIncreaseGauge	= m_fRecoverGaugeDelay;
		}
		break;

		case CS_RUN:
		{
			fIncreaseGauge	= m_fRecoverGaugeRun;
		}
		break;

		case CS_DASH:
		{
			fIncreaseGauge	= m_fRecoverGaugeDash;
		}
		break;

		case CS_WOUNDED:
		{
			fIncreaseGauge	= m_fRecoverGaugeWound;
		}
		break;

		default:
		{
			fIncreaseGauge	= 0.0f;
		}
		break;
	}

	if( m_fCurGauge < m_fMaxGauge )
	{
		float	fGauge	= fIncreaseGauge * fTimePerSec;

		m_fCurGauge	+= fGauge;
		m_fCurGauge	= min( m_fCurGauge, m_fMaxGauge );
	}

	// 2020-03-24
	//if( (m_iCurFrozenDummyGauge <= 0) || (pOwner->GetState() == CS_VEHICLE_DRIVER) )	
	if( (m_iCurFrozenDummyGauge <= 0) || (pOwner->GetState() == CS_USING_SKILL) )		
	{
		DestoryFrozenDummy( pOwner );	
	}
}

void ioFrozenItem::ChangeToHalfAttackFire( ioBaseChar *pOwner )
{
	if( !pOwner )				{	return;		}
	if( m_bFrozenDummyStart )	
	{
		m_FrozenState	= FS_NONE;		
		pOwner->SetState( CS_DELAY );

		m_CurrentChargeAttack		= NULL;
		m_CurrentChargeAttackMax	= NULL;
		m_bChargeLoopLoopAniStart	= false;

		if( pOwner->IsNeedSendNetwork() )
		{
			SP2Packet kPacket( CUPK_ITEM_GAUGE_OK );
			FillItemGaugePacketHeader( pOwner, kPacket );
			kPacket << (int)m_FrozenState;
			kPacket << m_iCurCombo;
			P2PNetwork::SendToAllPlayingUser( kPacket );
		}

		return;
	}

	m_iCurCombo = 0;
	DWORD dwCurTime = FRAMEGETTIME();
	pOwner->InitExtendAttackTagInfo();
	ClearCancelInfo();

	pOwner->SetNormalAttackByAttribute( m_HalfAttack );
	m_szCurAni					= m_HalfAttack.m_AttackAnimation;
	m_fCurAniRate				= m_HalfAttack.m_fAttackAniRate;
	m_dwInputCancelCheckTime	= dwCurTime;
	m_dwCancelCheckTime			= dwCurTime;
	pOwner->ApplyNormalAttackResult( m_iCurCombo );
	m_FrozenState = FS_HALF_ATTACK_FIRE;

	if( pOwner->IsNeedSendNetwork() )
	{
		SP2Packet kPacket( CUPK_ITEM_GAUGE_OK );
		FillItemGaugePacketHeader( pOwner, kPacket );
		kPacket << (int)m_FrozenState;
		kPacket << m_iCurCombo;
		P2PNetwork::SendToAllPlayingUser( kPacket );
	}


	CreateFrozenDummy( pOwner );
}

void ioFrozenItem::CreateFrozenDummy( ioBaseChar *pOwner )
{
	if ( !pOwner )	{	return;	}
	if ( m_szFrozenDummyName.IsEmpty() )	{	return;		}

	ioPlayStage *pStage = pOwner->GetCreator();
	if ( !pStage )	{	return;		}


	D3DXQUATERNION qtRot = pOwner->GetWorldOrientation();
	D3DXVECTOR3 vPos = pOwner->GetWorldPosition() + ( qtRot * m_vFrozenDummyOffset );

	int iCurIndex = g_DummyCharMgr.GetCurLocalIndex();
	ioDummyChar *pDummy = pStage->CreateDummyChar( m_szFrozenDummyName, iCurIndex, pOwner->GetCharName(), vPos, 0.0f, 0, true );
	if ( !pDummy )
		return;

	m_dwFrozenDummyIndex		= pDummy->GetDummyCharIdx();
	m_dwFrozenDummyStartTime	= FRAMEGETTIME();
	m_bFrozenDummyStart			= true;   
	m_iCurFrozenDummyGauge		= m_iFrozenDummyMaxGauge;
}

void ioFrozenItem::DestoryFrozenDummy( ioBaseChar *pOwner, bool bEffect /*= true */ )
{
	if ( !pOwner )	{	return;		}

	ioDummyChar *pShield = g_DummyCharMgr.FindDummyChar( pOwner, m_dwFrozenDummyIndex );
	//if ( !pShield || pShield->GetState() == DCS_DIE )
	if ( !pShield )
		return;
	
	pShield->SetDieState( bEffect );

	m_dwFrozenDummyIndex		= -1;
	m_dwFrozenDummyStartTime	= 0;
	m_bFrozenDummyStart			= false;
	m_iCurFrozenDummyGauge		= 0;
	if ( pOwner->IsNeedSendNetwork() )
	{
		SP2Packet kPacket( CUPK_ITEM_GAUGE_OK );
		FillItemGaugePacketHeader( pOwner, kPacket );
		kPacket << FS_DUMMY_DELETE;
		kPacket << m_iCurCombo;
		kPacket << bEffect;
		P2PNetwork::SendToAllPlayingUser( kPacket );
	}
}

void ioFrozenItem::ProcessFrozenDummy( ioBaseChar *pOwner )
{
	ioDummyChar *pShield = g_DummyCharMgr.FindDummyChar( pOwner, m_dwFrozenDummyIndex );
	if ( pShield )
	{
		D3DXQUATERNION qtRot = pOwner->GetWorldOrientation();
		D3DXVECTOR3 vPos = pOwner->GetWorldPosition() + ( qtRot * m_vFrozenDummyOffset );
		pShield->SetWorldOrientation( qtRot );
		pShield->SetWorldPosition( vPos );
	}
}

// 특별공격 약 D~ 전방에 얼음 보호막이 생성 및 시전자 보호
void ioFrozenItem::CallItemProcess( ioBaseChar* pOwner )
{
	if( !pOwner )						{	return;		}
	if( GetdwFrozenDummyIndex() <= 0 )	{	return;		}
	if( !m_bFrozenDummyStart )			{	return;		}
	if( m_iCurFrozenDummyGauge <= 0 )	{	return;		}


	CharState eState = pOwner->GetState();	
	bool bShieldDie = false;
	ioDummyChar *pShield = g_DummyCharMgr.FindDummyChar( pOwner, m_dwFrozenDummyIndex );
	if( pShield && (pShield->GetState() == DCS_DIE ) )
	{
		bShieldDie = true;
	}

	if( (eState == CS_BLOW_WOUNDED) || (eState == CS_DROP_ZONE_DOWN) || (true == bShieldDie) )
	{		
		m_dwFrozenDummyIndex		= -1;
		m_dwFrozenDummyStartTime	= 0;
		m_bFrozenDummyStart			= false;
		m_iCurFrozenDummyGauge		= 0;	
		DestroyArrowChargeSpecialEffect( pOwner );	// 2020-03-24
		return;
	}

	ProcessFrozenDummy( pOwner );

	if( m_iCurFrozenDummyGauge > 0 )
	{
		if( m_dwFrozenDummyStartTime + m_fFrozenDummyChargingTime <= FRAMEGETTIME() )
		{
			m_dwFrozenDummyStartTime = FRAMEGETTIME();
			--m_iCurFrozenDummyGauge;
		}
	}

	if( m_iCurFrozenDummyGauge <= 0 )
	{
		// 일정 시간이 지나면 보호막이 사라져야한다.
		DestoryFrozenDummy( pOwner );
	}
}

void ioFrozenItem::RandomPositionFullAttackFire( ioBaseChar *pOwner )
{
	if( pOwner->CheckMagicCircle() )
	{
		pOwner->CheckChangeDirectionForMagicCircle();
		pOwner->UpdateMagicCirclePosAll( m_fCircleMoveSpeed, m_fCircleRange, m_fCircleHeightGap, false );

		D3DXVECTOR3 vCharPos = pOwner->GetWorldPosition();
		D3DXVECTOR3 vCharDir = pOwner->GetMagicCirclePos() - vCharPos;
		vCharDir.y = 0.0f;

		D3DXVec3Normalize( &vCharDir, &vCharDir );

		pOwner->SetTargetRotToDir( vCharDir, true );

		pOwner->UpdateMagicCirclePos( m_fCircleMoveSpeed, m_fCircleRange, m_fCircleHeightGap, true, true );
	}

	D3DXVECTOR3 vCirclePos = pOwner->GetMagicCirclePos( m_fCircleOffset, false );
	for( int i = 0; i < m_iMultShotGaugeMax; ++i )
	{
		if( COMPARE( i, 0, (int)m_FrozenSpecialAttack.m_vWeaponInfoList.size() ) )
		{
			D3DXVECTOR3 vPos = vCirclePos;
			if( m_fRandomRange > 0.0f )
			{
				int iAngle = m_random.Random( 360 );
				int iRange = (int)(m_fRandomRange);
				iRange = m_random.Random(iRange);

				int iXPos = cos((double) iAngle ) * iRange;
				int iZPos = sin((double) iAngle ) * iRange;
				vPos.x += iXPos;
				vPos.z += iZPos;
			}
			D3DXVECTOR3 vDir = vPos - pOwner->GetWorldPosition();
			vDir.y = 0.0f;

			float fRate = FLOAT1;
			float fRange = D3DXVec3Length( &vDir );
			D3DXVec3Normalize( &vDir, &vDir );

			float fCurWeaponFloatPower = 0.0f;
			float fCurWeaponMoveSpeed = 0.0f;
			if( fRange > 0.0f )
			{
				fRate = fRange / (m_fCircleRange+m_fRandomRange);

				// CurFloat
				fCurWeaponFloatPower = m_fWeaponFloatPower * fRate;
				fCurWeaponFloatPower = max( fCurWeaponFloatPower, m_fWeaponFloatMinPower );

				float fCurGravity = 0.0f;
				float fWeaponMoveSpeed = 0.0f;
				ioPlayStage *pStage = g_SkillMaker.GetPlayStage();
				if( pStage )
					fCurGravity = pStage->GetRoomGravity();

				float fFlightDuration = GetFlightDuration( fCurGravity, fCurWeaponFloatPower );
				if( fFlightDuration > 0.0f )
				{
					fWeaponMoveSpeed = (m_fCircleRange+m_fRandomRange) / (fFlightDuration / FLOAT1000);
				}

				fCurWeaponMoveSpeed = fWeaponMoveSpeed * fRate;
			}

			WeaponMoveInfo kInfo;
			kInfo.m_vMoveDir = vDir;
			kInfo.m_fMoveSpeed = fCurWeaponMoveSpeed;
			kInfo.m_fFloatPower = fCurWeaponFloatPower;

			m_vWeaponMoveInfoList.push_back( kInfo );
		}
	}
}

// D~ + D~ 공격 일때 이펙트
void ioFrozenItem::CreateChargeSpecialEffect( ioBaseChar *pOwner )
{
	if( !pOwner )	{		return;		}

	int iEffectAray = m_iMultShotGaugeMax - 1;
	iEffectAray = min( iEffectAray, (int)m_vFrozenChargeSpecialEffectAngle.size() );
	float fAngle = ioMath::ArrangeHead( m_vFrozenChargeSpecialEffectAngle[iEffectAray] );

	D3DXQUATERNION qtAngle;
	D3DXQuaternionRotationYawPitchRoll( &qtAngle, DEGtoRAD(fAngle), 0.0f, 0.0f );

	D3DXVECTOR3 vDir = qtAngle * pOwner->GetMoveDir();
	D3DXVec3Normalize( &vDir, &vDir );
	vDir *= m_fFrozenChargeSpecialOffset;
	D3DXVECTOR3 vPos = pOwner->GetMidPositionByRate() + vDir;
	ioMapEffect *pEffect = pOwner->CreateMapEffectBySkill( m_FrozenChargeSpecialEffect, vPos );
	if( pEffect )
	{
		m_vSpecialEffectID.push_back( pEffect->GetUniqueID() );
	}

	// 상대방에도 이펙트가 보여야 한다.
	if( pOwner->IsNeedSendNetwork() )
	{
		SP2Packet kPacket( CUPK_ITEM_GAUGE_OK );
		FillItemGaugePacketHeader( pOwner, kPacket );
		kPacket << (int)FS_SPECIAL_EFFECT_CREATE;
		kPacket << m_iCurCombo;
		kPacket << m_iMultShotGaugeMax;
		P2PNetwork::SendToAllPlayingUser( kPacket );
	}
}

void ioFrozenItem::DestroyChargeSpecialEffect( ioBaseChar *pOwner )
{
	ioPlayStage *pStage = pOwner->GetCreator();
	if( pStage )
	{
		int iCnt = m_vSpecialEffectID.size();
		for( int i=0; i<iCnt; ++i )
		{
			pStage->DestroyMapEffect( m_vSpecialEffectID[i] );
		}
		m_vSpecialEffectID.clear();
	}

	// 상대방 이펙트 삭제 
	if( pOwner->IsNeedSendNetwork() )
	{
		SP2Packet kPacket( CUPK_ITEM_GAUGE_OK );
		FillItemGaugePacketHeader( pOwner, kPacket );
		kPacket << (int)FS_SPECIAL_EFFECT_DELETE;
		kPacket << m_iCurCombo;
		P2PNetwork::SendToAllPlayingUser( kPacket );
	}
}

void ioFrozenItem::UpdateChargeSpecialEffect( ioBaseChar *pOwner )
{
	ioPlayStage *pStage = pOwner->GetCreator();
	if( !pStage )
		return;

	for( int i=0; i<(int)m_vSpecialEffectID.size(); ++i )
	{
		D3DXVECTOR3 vDir = pOwner->GetMoveDir() * m_fFrozenChargeSpecialOffset;
		D3DXVECTOR3 vPos = pOwner->GetWorldPosition() + vDir;

		ioMapEffect *pEffect = pStage->FindMapEffect( m_vSpecialEffectID[i] );
		if( pEffect )
		{
			float fAngle = ioMath::ArrangeHead( m_vFrozenChargeSpecialEffectAngle[i] );

			D3DXQUATERNION qtAngle;
			D3DXQuaternionRotationYawPitchRoll( &qtAngle, DEGtoRAD(fAngle), 0.0f, 0.0f );

			D3DXVECTOR3 vDir = qtAngle * pOwner->GetMoveDir();
			D3DXVec3Normalize( &vDir, &vDir );
			vDir *= m_fFrozenChargeSpecialOffset;
			D3DXVECTOR3 vPos = pOwner->GetMidPositionByRate() + vDir;

			pEffect->SetWorldPosition( vPos );
		}
	}
}

float ioFrozenItem::GetFlightDuration( float fCurGravity, float fCurWeaponFloatPower )
{
	float fTopTime, fTopHeight;
	fTopTime = fTopHeight = 0.0f;
	float fNewGravity = fCurGravity;
	if( fNewGravity > 0.0f )
	{
		fTopTime = fCurWeaponFloatPower / fNewGravity;
		fTopHeight = (fCurWeaponFloatPower*fTopTime) - (0.5f*fNewGravity*fTopTime*fTopTime);
	}

	float fResult = 2 * fTopTime * FLOAT1000;

#ifndef SHIPPING 
	if( Setting::ShowExtraInfo() )
	{
		LOG.PrintTimeAndLog( 0, "CirCleMoveWeapon - [ExpectionValue] TopHeight: %.2f, FlightDuration: %.2f", fTopHeight, fResult );
	}
#endif

	return fResult;
}

void ioFrozenItem::SetExtraInfoToWeapon( ioWeapon *pWeapon, bool bJumpAttack )
{
	ioAttackableItem::SetExtraInfoToWeapon( pWeapon, bJumpAttack );

	ioZoneEffectWeapon *pZone = ToZoneEffectWeapon( pWeapon );
	if( pZone )
	{
		ioBaseChar *pChar = pZone->GetOwner();
		if( pChar )
		{

			D3DXVECTOR3 vDir = pChar->GetTargetRot() * D3DXVECTOR3( 0.0f, 0.0f, -FLOAT1 );
			pZone->SetCollisionDir( vDir );
			pZone->SetZoneValue( false, false );
		}
	}
}

void ioFrozenItem::SetNoneState( ioBaseChar* pOwner, int eNewState /* = 0 */ )
{
	if( !pOwner )		{	return;		}

	m_AimedTarget.Clear();
	m_bAimedBall				= false;
	m_FrozenState				= FS_NONE;
	m_bLoopAttackKeyDown		= false;
	m_dwChargeAttackStartTime	= 0;
	m_iMultShotGaugeMax			= 0;
	m_iCurMultiShot				= 0;
	m_CurrentChargeAttack		= NULL;
	m_CurrentChargeAttackMax	= NULL;
	m_bChargeLoopLoopAniStart	= false;
	m_dwLoopLoopChargeCheckTime	= 0;
	m_iCurShotGaugeMax			= 0;

	m_vWeaponMoveInfoList.clear();

	DestroyChargeSpecialEffect( pOwner );
	DestroyArrowChargeSpecialEffect( pOwner );	
	pOwner->SetTargetMarkerState( ioTargetMarker::MS_NONE );
}

void ioFrozenItem::CreateArrowChargeSpecialEffect( ioBaseChar *pOwner )
{	if( !pOwner )	{		return;		}

	float fAngle = ioMath::ArrangeHead( m_fArrowChargeSpecialEffectAngle );

	D3DXQUATERNION qtAngle;
	D3DXQuaternionRotationYawPitchRoll( &qtAngle, DEGtoRAD(fAngle), 0.0f, 0.0f );

	D3DXVECTOR3 vDir = qtAngle * pOwner->GetMoveDir();
	D3DXVec3Normalize( &vDir, &vDir );
	vDir *= m_fFrozenChargeSpecialOffset;
	D3DXVECTOR3 vPos = pOwner->GetMidPositionByRate() + vDir;
	ioMapEffect *pEffect = pOwner->CreateMapEffectBySkill( m_ArrowChargeSpecialEffect, vPos );	
	if( pEffect )
	{
		m_dwArrowSpecialEffectID = pEffect->GetUniqueID();
	}

	// 상대방에도 이펙트가 보여야 한다.
	if( pOwner->IsNeedSendNetwork() )
	{
		SP2Packet kPacket( CUPK_ITEM_GAUGE_OK );
		FillItemGaugePacketHeader( pOwner, kPacket );
		kPacket << (int)FS_ARROW_EFFECT_CREATE;
		kPacket << m_iCurCombo;
		P2PNetwork::SendToAllPlayingUser( kPacket );
	}
}

void ioFrozenItem::DestroyArrowChargeSpecialEffect( ioBaseChar *pOwner )
{
	ioPlayStage *pStage = pOwner->GetCreator();
	if( !pStage )	{	return;	}

	pStage->DestroyMapEffect( m_dwArrowSpecialEffectID );
	m_dwArrowSpecialEffectID	= 0;

	// 상대방에도 이펙트가 보여야 한다.
	if( pOwner->IsNeedSendNetwork() )
	{
		SP2Packet kPacket( CUPK_ITEM_GAUGE_OK );
		FillItemGaugePacketHeader( pOwner, kPacket );
		kPacket << (int)FS_ARROW_EFFECT_DELETE;
		kPacket << m_iCurCombo;
		P2PNetwork::SendToAllPlayingUser( kPacket );
	}
}

void ioFrozenItem::UpdateArrowChargeSpecialEffect( ioBaseChar *pOwner )
{
	ioPlayStage *pStage = pOwner->GetCreator();
	if( !pStage )	{	return;	}

	D3DXVECTOR3 vDir = pOwner->GetMoveDir() * m_fArrowChargeSpecialOffset;
	D3DXVECTOR3 vPos = pOwner->GetWorldPosition() + vDir;

	ioMapEffect *pEffect = pStage->FindMapEffect( m_dwArrowSpecialEffectID );
	if( pEffect )
	{
		float fAngle = ioMath::ArrangeHead( m_fArrowChargeSpecialEffectAngle );

		D3DXQUATERNION qtAngle;
		D3DXQuaternionRotationYawPitchRoll( &qtAngle, DEGtoRAD(fAngle), 0.0f, 0.0f );

		D3DXVECTOR3 vDir = qtAngle * pOwner->GetMoveDir();
		D3DXVec3Normalize( &vDir, &vDir );
		vDir *= m_fArrowChargeSpecialOffset;
		D3DXVECTOR3 vPos = pOwner->GetMidPositionByRate() + vDir;
		pEffect->SetWorldPosition( vPos );
	}
}

void ioFrozenItem::OnReleased( ioBaseChar *pOwner )
{
	ioWeaponItem::OnReleased( pOwner );
	DestoryFrozenDummy( pOwner );	
}

void ioFrozenItem::CheckDummyCharState( ioBaseChar *pOwner, int iIndex )
{
	if( !pOwner ) 
		return;

	 // 일정 시간이 지나면 보호막이 사라져야한다. 2020-03-23
	if( m_dwFrozenDummyIndex != iIndex )
	{
		DestoryFrozenDummy( pOwner );
	}
}