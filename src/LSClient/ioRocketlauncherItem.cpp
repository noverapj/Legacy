#include "StdAfx.h"
#include "ioRocketLauncherItem.h"
#include "ioZoneEffectWeapon.h"

#include "ioOwnerControlDummy6.h"
#include "ioTraceWeapon.h"

ioRocketLauncherItem::ioRocketLauncherItem(void)
{
	ClearData();

	m_iCurCombo = 0;
	m_iCurBullet							= 0;
	m_fReloadTimeModifyRate					= FLOAT1;
	m_bSetReloadMoveAni						= false;

	m_LevelTimeRate.clear();
	m_dwReloadStartTime						= 0;
	m_dwReloadEndTime						= 0;
	m_dwNextReloadTime						= 0;
	m_dwReloadGapTime						= 0;

	m_dwChargeFirstActionTime				= 0;

	m_iDummyIndex							= 0;
	m_vCreatePos							= ioMath::VEC3_ZERO;
	m_vDummyCharOffset						= ioMath::VEC3_ZERO;

	m_dwWeaponBaseIndex						= 0;

	m_dwFireStartTime						= 0;
	m_dwSpecialStateReleaseStartTime		= 0;
	m_dwSpecialStateReleaseEndime			= 0;

	m_dwSpecialAttackStartTime				= 0;
	m_dwSpecialAttackEndTime				= 0;

	m_dwSpecialAttackDelayTime				= 0;

	m_iSpecialAttackLevel					= 0;

	m_vAttactStepAimEffectList.clear();

	m_bReleaseBlock							= false;
}

ioRocketLauncherItem::ioRocketLauncherItem( const ioRocketLauncherItem &rhs ):
ioWeaponItem( rhs ),
m_ChargeAttackLoop( rhs.m_ChargeAttackLoop ),
m_ChargeAttackStart( rhs.m_ChargeAttackStart ),
m_dwFireGapMaxTime( rhs.m_dwFireGapMaxTime ),
m_dwFireGapMinTime( rhs.m_dwFireGapMinTime ),
m_dwFireGapReduceTime( rhs.m_dwFireGapReduceTime ),
m_iFireGapReduceCount( rhs.m_iFireGapReduceCount ),
m_iMaxBullet( rhs.m_iMaxBullet ),
m_iNeedBullet( rhs.m_iNeedBullet ),
m_fReloadTimeRate( rhs.m_fReloadTimeRate ),
m_ReloadAnimation( rhs.m_ReloadAnimation ),
m_ReloadEffect( rhs.m_ReloadEffect ),
m_ReloadMoveAnimation( rhs.m_ReloadMoveAnimation ),
m_DummyCharName( rhs.m_DummyCharName ),		// D꾹 특별 상태 관련
m_vDummyCharOffset( rhs.m_vDummyCharOffset ),
m_vDestroyDummyCharList( rhs.m_vDestroyDummyCharList ),
m_ChargeSpecialAttack( rhs.m_ChargeSpecialAttack ),
m_ChargeSpecialLastAttack( rhs.m_ChargeSpecialLastAttack ),
m_vAttactStepAimEffectList( rhs.m_vAttactStepAimEffectList ),
m_LevelTimeRate( rhs.m_LevelTimeRate ),
m_dwChargeTimeForDummyRelease( rhs.m_dwChargeTimeForDummyRelease ),
m_dwSpecialAttackDelayByLv( rhs.m_dwSpecialAttackDelayByLv )
{
	ClearData();

	m_iCurCombo							= 0;
	m_iCurBullet						= rhs.m_iMaxBullet;
	m_fReloadTimeModifyRate				= FLOAT1;
	m_bSetReloadMoveAni					= false;

	m_dwReloadStartTime					= 0;
	m_dwReloadEndTime					= 0;
	m_dwNextReloadTime					= 0;
	m_dwReloadGapTime					= 0;

	m_dwChargeFirstActionTime			= 0;

	m_iDummyIndex						= 0;
	m_vCreatePos						= ioMath::VEC3_ZERO;

	m_dwWeaponBaseIndex					= 0;

	m_dwFireStartTime					= 0;
	m_dwSpecialStateReleaseStartTime	= 0;
	m_dwSpecialStateReleaseEndime		= 0;

	m_dwSpecialAttackStartTime			= 0;
	m_dwSpecialAttackEndTime			= 0;

	m_dwSpecialAttackDelayTime			= 0;

	m_iSpecialAttackLevel				= 0;

	m_bReleaseBlock						= false;
}

ioRocketLauncherItem::~ioRocketLauncherItem(void)
{
}

ioItem* ioRocketLauncherItem::Clone()
{
	return new ioRocketLauncherItem( *this );
}

ioWeaponItem::WeaponSubType ioRocketLauncherItem::GetSubType() const
{
	return ioWeaponItem::WST_ROCKET_LAUNCHER_ITEM;
}

void ioRocketLauncherItem::OnReleased( ioBaseChar *pOwner )
{
	ioDummyChar *pDummyChar = g_DummyCharMgr.FindDummyCharToName( pOwner, m_DummyCharName );
	if( pDummyChar )
		pDummyChar->SendDieState( false );

	// 에임 삭제
	ioTargetMarker* pTargetMarker = pOwner->GetTargetMarker();
	if( pTargetMarker && pTargetMarker->GetMarkerType() != ioTargetMarker::MT_NONE)
	{
		// 에임 삭제 (로켓 런처가 자신이라면)
		if( pOwner->GetCharName() == g_MyInfo.GetPublicID() )
			pOwner->SetTargetMarker( ioTargetMarker::MT_NONE, "", "", "", "","" );
	}

	ioAttackableItem::OnReleased( pOwner );
}

void ioRocketLauncherItem::LoadProperty( ioINILoader &rkLoader )
{
	ioWeaponItem::LoadProperty( rkLoader );

	char szBuf[MAX_PATH] = "";
	char szKey[MAX_PATH] = "";

	m_iMaxBullet = rkLoader.LoadInt_e( "max_bullet", 0 );
	m_iNeedBullet = rkLoader.LoadInt_e( "need_bullet", 1 );
	m_fReloadTimeRate  = rkLoader.LoadFloat_e( "reload_time_rate", FLOAT1 );

	rkLoader.LoadString_e( "reload_animation", "", szBuf, MAX_PATH );
	m_ReloadAnimation = szBuf;
	rkLoader.LoadString_e( "reload_move_animation", "", szBuf, MAX_PATH );
	m_ReloadMoveAnimation = szBuf;
	rkLoader.LoadString_e( "reload_effect", "", szBuf, MAX_PATH );
	m_ReloadEffect = szBuf;

	//Charge Attack Ani
	LoadAttackAttribute( "charge_special_attack_start", m_ChargeAttackStart, rkLoader );
	LoadAttackAttribute( "charge_special_attack_loop", m_ChargeAttackLoop, rkLoader );

	// 구체 더미 셋팅 값
	rkLoader.LoadString_e( "dummy_char_name", "", szBuf, MAX_PATH );
	m_DummyCharName = szBuf;

	m_vDummyCharOffset.x = rkLoader.LoadFloat_e( "dummy_char_width_offset", 0.0f );
	m_vDummyCharOffset.y = rkLoader.LoadFloat_e( "dummy_char_height_offset", 0.0f );
	m_vDummyCharOffset.z = rkLoader.LoadFloat_e( "dummy_char_offset", 0.0f );

	int iCnt = rkLoader.LoadInt_e( "destroy_dummy_cnt", 0 );
	m_vDestroyDummyCharList.clear();
	for( int i=0; i < iCnt; ++i )
	{
		wsprintf_e( szKey, "destroy_dummy%d_name", i+1 );
		rkLoader.LoadString( szKey, "", szBuf, MAX_PATH );

		m_vDestroyDummyCharList.push_back( szBuf );
	}

	int iLevelCount = rkLoader.LoadInt_e( "level_count", 0 );
	m_LevelTimeRate.clear();
	for( int i=0; i<iLevelCount; i++ )
	{
		wsprintf_e( szBuf, "level_time_rate%d", i);

		float fRate = rkLoader.LoadFloat( szBuf, -FLOAT1 );
		if( fRate == -FLOAT1 && i > 0 )
		{
			if( i>0 )
				fRate = m_LevelTimeRate[i-1];
			else
				fRate = FLOAT1;
		}
		m_LevelTimeRate.push_back( fRate );
	}

	//Attack
	LoadAttackAttribute( "charge_special_attack", m_ChargeSpecialAttack, rkLoader );
	LoadAttackAttribute( "charge_special_last_attack", m_ChargeSpecialLastAttack, rkLoader );

	int iMaxAimCount = rkLoader.LoadInt_e( "aim_marker_count", 0 );
	if ( iMaxAimCount > 0 )
	{
		m_vAttactStepAimEffectList.reserve( iMaxAimCount );
		for( int i=0; i < iMaxAimCount; ++i )
		{
			wsprintf_e( szBuf, "aim_marker%d", i+1 );
			rkLoader.LoadString( szBuf, "", szBuf, MAX_PATH );
			m_vAttactStepAimEffectList.push_back( ioHashString(szBuf) );
		}
	}
	m_dwChargeTimeForDummyRelease = (DWORD)rkLoader.LoadInt_e( "charge_time_for_dummy_release", 1000 );
	m_dwSpecialAttackDelayByLv		= (DWORD)rkLoader.LoadInt_e( "charge_special_delay", 0 );
}

void ioRocketLauncherItem::ClearData()
{
	m_ChargeState = CS_NONE;
	m_dwMotionStartTime = 0;
	m_dwMotionEndTime = 0;
}

void ioRocketLauncherItem::InitChargeState( ioBaseChar* pOwner, bool bInitTargetMark, bool bSend )
{
	m_ChargeState = CS_NONE;
	m_AimedTarget.Clear();

	if( bInitTargetMark )
		pOwner->SetTargetMarker( ioTargetMarker::MT_NONE, "", "", "", "","" );		// 에임 삭제

	if( bSend && pOwner->IsNeedSendNetwork() )
	{
		SP2Packet kPacket( CUPK_ITEM_GAUGE_OK );
		FillItemGaugePacketHeader( pOwner, kPacket );
		kPacket << ST_NONE;
		kPacket << pOwner->GetWorldPosition();
		P2PNetwork::SendToAllPlayingUser( kPacket );
	}
}

void ioRocketLauncherItem::SetNormalAttack( ioBaseChar *pOwner, int iCurCombo )
{
	if( !pOwner )
		return;

	m_fCurChargeRate = 0.0f;
	m_fCurChargeRateForSkill = 0.0f;

	ClearCancelInfo();
	ClearData();

	int iComboCnt = GetMaxCombo();
	if( !COMPARE( iCurCombo, 0, iComboCnt ) )
	{
		ioWeaponItem::SetNormalAttack( pOwner, iCurCombo );

		m_ChargeState = CS_NONE;
		m_iCurCombo = iCurCombo;

		pOwner->SetAttackMoveEnable( false );
		pOwner->SetChargingState( false );
	}
	else
	{
		ioEntityGroup *pGrp = pOwner->GetGroup();
		pGrp->ClearAllLoopAni( FLOAT100, true );
		pGrp->ClearAllActionAni( FLOAT100, true );

		m_ChargeState = CS_CHARGING;
		m_dwMotionStartTime = FRAMEGETTIME();
		m_iCurCombo = iCurCombo;
		pOwner->SetState( CS_ATTACK );
		pOwner->SetAttackMoveEnable( false );
		pOwner->SetChargingState( true );
	}
}

void ioRocketLauncherItem::SetExtraInfoToWeapon( ioWeapon *pWeapon, bool bJumpAttack )
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
			pZone->SetZoneValue( true, false );
		}
	}
}

void ioRocketLauncherItem::CheckNormalAttack( ioBaseChar *pOwner )
{
	switch( m_ChargeState )
	{
	case CS_CHARGING:
		ProcessCharging( pOwner );
		break;
	case CS_NORMAL_ATTACK:
		ProcessNormalAttack( pOwner );
		break;
	case CS_CHARGE_ATTACK_START:
		{
			ProcessChargeAttackStart( pOwner );
			break;
		}
	case CS_CHARGE_ATTACK_LOOP:
		{
			ProcessChargeAttackLoop( pOwner );
			ProcessLookatCircleDummy( pOwner );
			break;
		}
	case CS_CHARGE_ATTACK_END:
		{
			ProcessExtendAttack( pOwner );
			ProcessLookatCircleDummy( pOwner );
			break;
		}
	}
}

void ioRocketLauncherItem::ProcessCharging( ioBaseChar *pOwner )
{
	if( !pOwner || !pOwner->IsNeedProcess() )
		return;

	if( pOwner->IsAttackKeyDown() && !pOwner->IsHasCrown() )
	{
		if( m_dwMotionStartTime + m_dwChargeTime <= FRAMEGETTIME() )
		{
			if( m_iCurBullet >= m_iNeedBullet )
				ChangeToChargeAttackStart( pOwner );
			else
			{
				pOwner->NormalAttackOver();

				if( m_iCurBullet < m_iNeedBullet && SetExtendDefenseState( pOwner ) )
				{
					if( pOwner->IsNeedSendNetwork() )
					{
						SP2Packet kPacket( CUPK_EXTEND_DEFENSE );
						kPacket << pOwner->GetCharName();
						kPacket << pOwner->GetHP().m_fCurValue;
						kPacket << m_iCurBullet;
						P2PNetwork::SendToAllPlayingUser( kPacket );
					}
				}
			}
		}
	}
	else
		ChangeToNormalAttack( pOwner );
}

void ioRocketLauncherItem::ChangeToNormalAttack( ioBaseChar *pOwner )
{
	DWORD dwCurTime = FRAMEGETTIME();

	pOwner->InitExtendAttackTagInfo();
	pOwner->InitActionStopDelay( true );
	ClearCancelInfo();

	pOwner->SetAttackMoveEnable( false );
	pOwner->SetChargingState( false );

	m_fCurChargeRate = 0.0f;
	m_fCurChargeRateForSkill = 0.0f;

	const AttackAttribute *pAttribute = m_FormalAttack.GetAttribute( AT_NORMAL, m_iCurCombo );
	if( pAttribute )
	{
		if( !m_bChargeAutoTarget )
			pOwner->SetAutoTarget( ATT_NORMAL );

		pOwner->SetNormalAttackByAttribute( *pAttribute );

		SetCurAttackAniForInputJump( pAttribute->m_AttackAnimation, pAttribute->m_fAttackAniRate );
	}

	pOwner->ApplyNormalAttackResult( m_iCurCombo );

	m_ChargeState = CS_NORMAL_ATTACK;

	if( pOwner->IsNeedSendNetwork() )
	{
		SP2Packet kPacket( CUPK_ITEM_GAUGE_OK );
		FillItemGaugePacketHeader( pOwner, kPacket );
		kPacket << ST_NORMAL_ATTACK;
		kPacket << pOwner->GetWorldPosition();
		kPacket << m_iCurCombo;
		if( !m_bChargeAutoTarget )
			pOwner->FillAutoTargetInfo( kPacket );
		P2PNetwork::SendToAllPlayingUser( kPacket );
	}
}

void ioRocketLauncherItem::ProcessNormalAttack( ioBaseChar *pOwner )
{
	if ( !pOwner )
		return;

	if( pOwner->IsNeedProcess() )
	{
		CheckCancelReserve( pOwner );		
		if( ProcessCancel(pOwner) )
			return;

		CheckCancelReserveByConditionalSpecialDash( pOwner );
		//Enable GaugeCheck
		ProcessCancelBySpeicalDash( pOwner );
	}

	ioWeaponItem::CheckNormalAttack( pOwner );
}


void ioRocketLauncherItem::ChangeToChargeAttackStart( ioBaseChar *pOwner )
{
	if ( !pOwner )
		return;

	ioEntityGroup *pGrp = pOwner->GetGroup();
	if( !pGrp )
		return;

	if( pOwner->GetState() != CS_ATTACK )
	{
		pOwner->SetState( CS_ATTACK );
		pOwner->SetCurNormalAttackItem( this );
	}

	int iAniID = pGrp->GetAnimationIdx( m_ChargeAttackStart.m_AttackAnimation );
	if ( iAniID == -1 )
	{
		LOG.PrintTimeAndLog( 0, "%s - ani Name is not exist", __FUNCTION__ );
		return;
	}

	DWORD dwPreDelay = m_ChargeAttackStart.m_dwPreDelay;
	float fTimeRate = m_ChargeAttackStart.m_fAttackAniRate;
	if( fTimeRate <= 0.0f )
		fTimeRate = FLOAT1;

	pGrp->ClearAllActionAni( 0.0f, true );
	pGrp->ClearAllLoopAni( 0.0f, true );
	pGrp->SetActionAni( iAniID, FLOAT100, FLOAT100, FLOAT1, FLOAT1/fTimeRate );

	m_dwChargeFirstActionTime = FRAMEGETTIME();
	m_dwChargeFirstActionTime += pGrp->GetAnimationEventTime_e( iAniID, "fire_s" ) * m_ChargeAttackStart.m_fAttackAniRate;

	pOwner->RefreshFireTimeListWithBase( m_dwWeaponBaseIndex,
		iAniID,
		m_ChargeAttackStart.m_vWeaponInfoList,
		FTT_NORMAL_ATTACK,
		fTimeRate,
		dwPreDelay );
	pOwner->IncreaseWeaponIndexBase();
	pOwner->SetReservedSliding( m_ChargeAttackStart.m_vForceInfoList, iAniID, fTimeRate, 0 );
	pOwner->CheckCharColSkipTime( iAniID, fTimeRate, dwPreDelay );

	m_dwMotionEndTime = FRAMEGETTIME() + pGrp->GetAnimationFullTime(iAniID) * fTimeRate;
	m_ChargeState = CS_CHARGE_ATTACK_START;

	if( !m_bReleaseBlock )
		m_bReleaseBlock = true;

	if( pOwner->IsNeedSendNetwork() )
	{
		SP2Packet kPacket( CUPK_ITEM_GAUGE_OK );
		FillItemGaugePacketHeader( pOwner, kPacket );
		kPacket << ST_CHARGE_ATTACK_START;
		kPacket << pOwner->GetWorldPosition();
		kPacket << pOwner->GetTargetRot();
		P2PNetwork::SendToAllPlayingUser( kPacket );
	}
}

void ioRocketLauncherItem::ChangeToExtendAttack( ioBaseChar *pOwner )
{
	if ( !pOwner )
		return;

	ioEntityGroup *pGrp = pOwner->GetGroup();
	if ( !pGrp )
		return;

	AttackAttribute attAttr = m_ChargeSpecialAttack;
	// 공격 레벨 1이면 마지막 공격 애니, 웨폰 나와야하기 때문에...
	if( m_iSpecialAttackLevel == 1 )
		attAttr = m_ChargeSpecialLastAttack;

	int iAniID = -1;
	iAniID = pGrp->GetAnimationIdx( attAttr.m_AttackAnimation );

	if ( iAniID == -1 )
		return;

	if ( m_AimedTarget.IsEmpty() )
		return;
	
	m_dwMotionEndTime = 0;
	ClearKeyReserve();

	m_ChargeState = CS_CHARGE_ATTACK_END;

	pOwner->SetAttackMoveEnable( false );

	pGrp->ClearAllLoopAni( FLOAT100, true );
	pGrp->ClearAllActionAni( FLOAT100, true );
	pOwner->ClearReservedSliding();

	DWORD dwPreDelay = attAttr.m_dwPreDelay;
	float fTimeRate = 1.0f;
	if ( attAttr.m_fAttackAniRate > 0.0f )
		fTimeRate = attAttr.m_fAttackAniRate;

	DWORD dwStartTime			= FRAMEGETTIME() + dwPreDelay;
	m_dwSpecialAttackEndTime	= dwStartTime + pGrp->GetAnimationFullTime( iAniID ) * fTimeRate + attAttr.m_dwEndDelay;
	m_dwFireStartTime			= dwStartTime + pGrp->GetAnimationEventTime_e( iAniID, "fire_s" ) * fTimeRate;

	// Col Skip
	pOwner->CheckCharColSkipTime( iAniID, fTimeRate, dwPreDelay );
	//Force
	pOwner->SetReservedSliding( attAttr.m_vForceInfoList, iAniID, fTimeRate, dwPreDelay );

	pGrp->SetActionAni( iAniID, FLOAT100, FLOAT100, FLOAT1, FLOAT1/fTimeRate, 0.0f, dwPreDelay );

	if( pOwner->IsNeedSendNetwork() )
	{
		SP2Packet kPacket( CUPK_ITEM_GAUGE_OK );
		FillItemGaugePacketHeader( pOwner, kPacket );
		kPacket << ST_CHARGE_ATTACK_FIRE_START;
		kPacket << pOwner->GetWorldPosition();
		kPacket << pOwner->GetTargetRot();
		kPacket << m_AimedTarget;
		kPacket << m_iSpecialAttackLevel;
		P2PNetwork::SendToAllPlayingUser( kPacket );
	}
}

void ioRocketLauncherItem::ProcessExtendAttack( ioBaseChar *pOwner )
{
	if ( !pOwner || !pOwner->IsNeedProcess() )
		return;

	DWORD dwCurTime = FRAMEGETTIME();

	if ( m_dwFireStartTime && m_dwFireStartTime < dwCurTime )
	{
		m_dwFireStartTime = 0;
		bool bLastFire = false;
		if( m_iSpecialAttackLevel == 0 )
			bLastFire = true;
		ExtendAttackFire( pOwner, bLastFire );
	}

	if( m_dwSpecialAttackEndTime && m_dwSpecialAttackEndTime < dwCurTime )
	{
		m_dwSpecialAttackEndTime = 0;
		if( m_iSpecialAttackLevel > 0 )
		{
			m_dwSpecialAttackDelayTime = m_dwSpecialAttackDelayByLv + dwCurTime;
		}
		else
			m_dwSpecialAttackDelayTime = 0;
	}

	if( m_dwSpecialAttackDelayTime && m_dwSpecialAttackDelayTime < dwCurTime )
	{
		m_dwSpecialAttackDelayTime = 0;

		if( m_iSpecialAttackLevel > 0 )
		{
			ChangeToExtendAttack( pOwner );
			m_iSpecialAttackLevel--;
		}
	}
	
	if( m_iSpecialAttackLevel <= 0 && m_dwFireStartTime == 0 && m_dwSpecialAttackEndTime == 0 && m_dwSpecialAttackDelayTime == 0 )
	{
		pOwner->SetState( CS_DELAY );
		InitChargeState( pOwner, false, true );
	}
}

void ioRocketLauncherItem::ExtendAttackFire( ioBaseChar *pOwner, bool bLastFire )
{
	if ( !pOwner )
		return;

	AttackAttribute attAttr = m_ChargeSpecialAttack;
	// 공격 레벨 1이면 마지막 공격 애니, 웨폰 나와야하기 때문에...
	if( m_iSpecialAttackLevel == 0 )
		attAttr = m_ChargeSpecialLastAttack;

	int iAttackSize = attAttr.m_vWeaponInfoList.size();
	if ( iAttackSize <= 0 )
		return;

	DWORD dwWeaponBaseIndex = pOwner->IncreaseWeaponIndexBase();
	D3DXVECTOR3 vPos = pOwner->GetMidPositionByRate();
	D3DXVECTOR3 vDir = pOwner->GetWorldOrientation() * D3DXVECTOR3(0.0f,0.0f,-FLOAT1);
	D3DXVec3Normalize( &vDir, &vDir );

	for ( int i = 0; i<iAttackSize ; ++i )
	{
		WeaponInfo sWeaponInfo = attAttr.m_vWeaponInfoList[i];

		FireTime kFireTime;
		kFireTime.dwStart = FRAMEGETTIME();

		kFireTime.iAttributeIdx = sWeaponInfo.m_iWeaponIdx;
		kFireTime.iResistanceIdx = sWeaponInfo.m_iResistanceIdx;
		kFireTime.szWoundedAni = sWeaponInfo.m_WoundedAnimation;
		kFireTime.dwWoundedAniDuration = sWeaponInfo.m_dwWoundedDuration;
		kFireTime.bLoopWoundedAni = sWeaponInfo.m_bWoundedLoopAni;
		kFireTime.dwWeaponIdx = dwWeaponBaseIndex++;
		kFireTime.eFireTimeType = FTT_NORMAL_ATTACK;

		ioWeapon *pWeapon = pOwner->ExtendFireExplicit( kFireTime, vPos, vDir, "" );
		if ( !m_AimedTarget.IsEmpty() )
		{
			ioTraceWeapon *pTraceWeapon = ToTraceWeapon( pWeapon );
			if ( pTraceWeapon )
			{
				pTraceWeapon->SetTraceTarget( m_AimedTarget );
				if( bLastFire && i == ( iAttackSize - 1 ) )
					pTraceWeapon->SetLastWeapon();
			}
		}
	}

	// 특별 상태 끝나서 더미 삭제
	g_DummyCharMgr.DestroyDummyCharByNameList( pOwner, m_vDestroyDummyCharList );
	// 에임 삭제 (로켓 런처가 자신이라면)
	if( pOwner->GetCharName() == g_MyInfo.GetPublicID() )
		pOwner->SetTargetMarker( ioTargetMarker::MT_NONE, "", "", "", "","" );

	WasteBullet( pOwner );

	if( pOwner->IsNeedSendNetwork() )
	{
		SP2Packet kPacket( CUPK_ITEM_GAUGE_OK );
		FillItemGaugePacketHeader( pOwner, kPacket );
		kPacket << ST_CHARGE_ATTACK_FIRE_END;
		kPacket << pOwner->GetWorldPosition();
		kPacket << pOwner->GetTargetRot();
		kPacket << bLastFire;
		P2PNetwork::SendToAllPlayingUser( kPacket );
	}
}

void ioRocketLauncherItem::CheckDummyChar( ioBaseChar *pChar, ioPlayStage *pStage )
{
	if( !pChar ) return;
	if( !pChar->IsNeedProcess() ) return;
	if( !pStage ) return;

	if( m_DummyCharName.IsEmpty() ) return;
	if( m_dwChargeFirstActionTime == 0 ) return;

	if( m_dwChargeFirstActionTime >= FRAMEGETTIME() )
		return;

	m_ChargeState = CS_CHARGE_ATTACK_LOOP;

	m_dwChargeFirstActionTime = 0;

	g_DummyCharMgr.DestroyDummyCharByNameList( pChar, m_vDestroyDummyCharList );

	m_vCreatePos = pChar->GetWorldPosition() + ( pChar->GetTargetRot() * m_vDummyCharOffset );

	int iCurIndex = g_DummyCharMgr.GetCurLocalIndex();
	m_iDummyIndex = iCurIndex;

	ioDummyChar *pDummy = pStage->CreateDummyChar( m_DummyCharName, m_iDummyIndex,
		pChar->GetCharName(),
		m_vCreatePos,
		0.0f,
		0, true );

	if( pDummy )
	{
		pDummy->SetControlSkillName( GetName() );
		ioOwnerControlDummy6* pOCDummy = ToOwnerControlDummy6( pDummy );
		if( pOCDummy )
		{
			pOCDummy->ConfirmCreatedSpecialState();
		}
	}

	if( pChar->IsNeedSendNetwork() )
	{
		SP2Packet kPacket( CUPK_SKILL_EXTRA_INFO );
		kPacket << pChar->GetCharName();
		kPacket << GetName();
		kPacket << pChar->GetTargetRot();
		kPacket << m_vCreatePos;
		kPacket << m_iDummyIndex;
		P2PNetwork::SendToAllPlayingUser( kPacket );
	}
}

bool ioRocketLauncherItem::IsDummyCharControl( ioBaseChar *pOwner )
{
	ioDummyChar *pDummyChar = g_DummyCharMgr.FindDummyCharToName( pOwner, m_DummyCharName );
	if ( pDummyChar && pDummyChar->GetState() != DCS_DIE )
		return true;

	// 에임 삭제
	ioTargetMarker* pTargetMarker = pOwner->GetTargetMarker();
	if( pTargetMarker && pTargetMarker->GetMarkerType() != ioTargetMarker::MT_NONE)
	{
		// 에임 삭제 (로켓 런처가 자신이라면)
		if( pOwner->GetCharName() == g_MyInfo.GetPublicID() )
			pOwner->SetTargetMarker( ioTargetMarker::MT_NONE, "", "", "", "","" );
	}
	
	return false;
}

D3DXVECTOR3 ioRocketLauncherItem::GetDummyCharControlPos( ioBaseChar *pOwner )
{
	if( !pOwner )
		return ioMath::VEC3_ZERO;

	ioDummyChar *pDummyChar = g_DummyCharMgr.FindDummyCharToName( pOwner, m_DummyCharName );
	if( pDummyChar )
		return pDummyChar->GetCameraPosition();

	return pOwner->GetWorldPosition();
}

void ioRocketLauncherItem::ProcessChargeAttackStart( ioBaseChar *pOwner )
{
	if ( !pOwner || !pOwner->IsNeedProcess() )
		return;

	if ( m_dwMotionEndTime && m_dwMotionEndTime < FRAMEGETTIME() )
	{
		m_dwMotionEndTime = 0;
		ChangeToChargeAttackLoop( pOwner );
		if( pOwner->GetCreator() )
			CheckDummyChar( pOwner, pOwner->GetCreator() );
		return;
	}

	if( pOwner->IsAttackKeyRelease() )
	{
		if( m_dwMotionStartTime > 0 )
			m_dwMotionStartTime = 0;
		if( m_dwSpecialStateReleaseStartTime > 0 )
			m_dwSpecialStateReleaseStartTime = 0;

		if( m_bReleaseBlock )
			m_bReleaseBlock = false;
	}
}

void ioRocketLauncherItem::ChangeToChargeAttackLoop( ioBaseChar *pOwner )
{
	if ( !pOwner )
		return;
	ioEntityGroup *pGrp = pOwner->GetGroup();
	if ( !pGrp )
		return;

	ioPlayStage *pStage = pOwner->GetCreator();
	if( !pStage ) return;

	pGrp->ClearAllActionAni( FLOAT100, true );
	pGrp->ClearAllLoopAni( FLOAT100, true );

	pGrp->SetLoopAni( m_ChargeAttackLoop.m_AttackAnimation, 0.0f, FLOAT1 );

	m_ChargeState = CS_CHARGE_ATTACK_LOOP;
	m_dwFireTime = FRAMEGETTIME();
	m_dwFireGapTime = m_dwFireGapMaxTime;
	m_iFireCount = 0;

	if( pOwner->IsNeedSendNetwork() )
	{
		SP2Packet kPacket( CUPK_ITEM_GAUGE_OK );
		FillItemGaugePacketHeader( pOwner, kPacket );
		kPacket << ST_CHARGE_ATTACK_LOOP;
		kPacket << pOwner->GetWorldPosition();
		P2PNetwork::SendToAllPlayingUser( kPacket );
	}
}

void ioRocketLauncherItem::ProcessChargeAttackLoop( ioBaseChar *pOwner )
{
	if( !pOwner )
		return;

	if( pOwner->GetState() != CS_ATTACK )
	{
		
	}

	// 더미 살아있는지 체크
	ioDummyChar* pSphereDummy = g_DummyCharMgr.FindDummyCharToName( pOwner, m_DummyCharName );

	if ( pOwner->IsNeedProcess() )
	{
		if( pOwner->IsAttackKeyDown() && m_dwMotionStartTime == 0 )
		{
			if( m_dwSpecialStateReleaseStartTime == 0 )
			{
				m_dwSpecialStateReleaseStartTime = FRAMEGETTIME();
				m_dwSpecialStateReleaseEndime = m_dwSpecialStateReleaseStartTime + m_dwChargeTimeForDummyRelease;
			}
			
			if( m_dwMotionStartTime == 0 )
			{
				if( m_dwSpecialStateReleaseEndime > 0 && m_dwSpecialStateReleaseEndime <= FRAMEGETTIME() && pSphereDummy )
				{
					m_dwSpecialStateReleaseEndime = 0;
					// 특별 상태 끝나서 더미 삭제
					g_DummyCharMgr.DestroyDummyCharByNameList( pOwner, m_vDestroyDummyCharList );
					// 에임 삭제 (로켓 런처가 자신이라면)
					if( pOwner->GetCharName() == g_MyInfo.GetPublicID() )
						pOwner->SetTargetMarker( ioTargetMarker::MT_NONE, "", "", "", "","" );
					pOwner->SetState( CS_DELAY );
					pOwner->ChangeDirection();
					InitChargeState( pOwner, true, true );
					m_bReleaseBlock = false;
				}
			}
		}
		else if( pOwner->IsAttackKeyRelease() )
		{
			if( m_dwMotionStartTime > 0 )
				m_dwMotionStartTime = 0;
			if( m_dwSpecialStateReleaseStartTime > 0 )
				m_dwSpecialStateReleaseStartTime = 0;
			if( pSphereDummy && !m_bReleaseBlock )
			{
				ioOwnerControlDummy6* pOwnerControlDummy6 = ToOwnerControlDummy6( pSphereDummy );
				if( pOwnerControlDummy6 )
				{
					m_iSpecialAttackLevel = pOwnerControlDummy6->GetCurAttackLevel();
					// 레벨 3이면 +1 해줌 3렙에는 4번 쏴야하기때문에
					if( m_iSpecialAttackLevel == 3 )
						m_iSpecialAttackLevel = 4;

					ChangeToExtendAttack( pOwner );
					m_iSpecialAttackLevel--;
				}
			}
			m_bReleaseBlock = false;
		}

		// Attack lv 1 일대는 바로 기본 모션으로... ( 0인 이유는 위에서 -- 해서...)
		if( m_iSpecialAttackLevel == 0 )
		{
			if( !pSphereDummy )
			{
				pOwner->SetState( CS_DELAY );
				m_AimedTarget.Clear();
			}
		}
	}

	pOwner->ChangeDirectionByInputDir( true );
}

void ioRocketLauncherItem::ProcessChargeAttackEnd( ioBaseChar *pOwner )
{
	if ( !pOwner || !pOwner->IsNeedProcess() )
		return;

	if ( m_dwMotionEndTime && m_dwMotionEndTime < FRAMEGETTIME() )
	{
		m_dwMotionEndTime = 0;
		pOwner->SetState( CS_DELAY );
		return;
	}

	if ( m_dwSpecialAttackEndTime && m_dwSpecialAttackEndTime < FRAMEGETTIME() )
	{
		m_dwSpecialAttackEndTime = 0;
		pOwner->SetState( CS_DELAY );
		return;
	}
}

void ioRocketLauncherItem::ProcessLookatCircleDummy( ioBaseChar *pOwner )
{
	if ( !pOwner )
		return;
	ioDummyChar *pDummy = g_DummyCharMgr.FindDummyCharToName( pOwner, m_DummyCharName );
	if ( pDummy )
	{
		D3DXVECTOR3 vDiff1 = pDummy->GetWorldPosition() - pOwner->GetWorldPosition();
		D3DXVec3Normalize( &vDiff1, &vDiff1 );

		pOwner->SetTargetRotToDir( vDiff1, true, true );
	}
	else if ( !m_AimedTarget.IsEmpty() )
	{
		ioPlayStage* pPlayStage = pOwner->GetCreator();
		if( pPlayStage )
		{
			ioBaseChar* pTarget = pPlayStage->GetBaseChar( m_AimedTarget );
			if( pTarget )
			{
				D3DXVECTOR3 vDiff2 = pTarget->GetWorldPosition() - pOwner->GetWorldPosition();
				D3DXVec3Normalize( &vDiff2, &vDiff2 );

				pOwner->SetTargetRotToDir( vDiff2, true, true );
			}
		}
	}
}

void ioRocketLauncherItem::DeleteSpecialAttackAim( ioBaseChar *pOwner )
{
	// 에임 삭제
	ioTargetMarker* pTargetMarker = pOwner->GetTargetMarker();
	if( pTargetMarker && pTargetMarker->GetMarkerType() != ioTargetMarker::MT_NONE)
	{
		// 에임 삭제 (로켓 런처가 자신이라면)
		if( pOwner->GetCharName() == g_MyInfo.GetPublicID() )
			pOwner->SetTargetMarker( ioTargetMarker::MT_NONE, "", "", "", "","" );
	}

	if( m_ChargeState != CS_CHARGE_ATTACK_END )
		m_AimedTarget.Clear();
}

void ioRocketLauncherItem::DeleteSpecialAttackAim( ioBaseChar *pOwner, ioBaseChar *pTargetChar )
{
	// 에임 삭제
	ioTargetMarker* pTargetMarker = pOwner->GetTargetMarker();
	if( pTargetMarker && pTargetMarker->GetMarkerType() != ioTargetMarker::MT_NONE && pTargetMarker->GetTargetChar() == pTargetChar )
	{
		// 에임 삭제 (로켓 런처가 자신이라면)
		if( pOwner->GetCharName() == g_MyInfo.GetPublicID() )
			pOwner->SetTargetMarker( ioTargetMarker::MT_NONE, "", "", "", "","" );
	}

	if( m_ChargeState != CS_CHARGE_ATTACK_END && pTargetChar->GetCharName() == m_AimedTarget )
		m_AimedTarget.Clear();
}

void ioRocketLauncherItem::OnItemGaugeOK( ioBaseChar *pOwner, SP2Packet &rkPacket )
{
	if ( !pOwner )
		return;

	int iState;
	rkPacket >> iState;

	D3DXVECTOR3 vPos;
	rkPacket >> vPos;
	pOwner->SetWorldPosition( vPos );

	if ( iState == ST_NORMAL_ATTACK )
	{
		rkPacket >> m_iCurCombo;
		if( !m_bChargeAutoTarget )
			pOwner->ApplyAutoTargetInfo( rkPacket );
		ChangeToNormalAttack( pOwner );
	}
	else if ( iState == ST_CHARGE_ATTACK_START )
	{
		D3DXQUATERNION qtRot;
		rkPacket >> qtRot;
		pOwner->SetTargetRotAndMoveDirChange( qtRot );

		ChangeToChargeAttackStart( pOwner );
	}
	else if ( iState == ST_CHARGE_ATTACK_LOOP )
	{
		ChangeToChargeAttackLoop( pOwner );
	}
	else if ( iState == ST_CHARGE_ATTACK_FIRE_START )
	{
		D3DXQUATERNION qtRot;
		rkPacket >> qtRot;
		pOwner->SetTargetRotAndMoveDirChange( qtRot );

		rkPacket >> m_AimedTarget;

		int iSpecialAttackLevel = 0;
		rkPacket >> iSpecialAttackLevel;
		
		// 4레벨 이상은 다 3레벨로 변경...
		if( iSpecialAttackLevel > 4 )
			iSpecialAttackLevel = 4;

		bool bSetAim = true;
		if( m_iSpecialAttackLevel >= iSpecialAttackLevel )
			bSetAim = false;

		m_iSpecialAttackLevel = iSpecialAttackLevel;

		if( m_iSpecialAttackLevel > 0 )
		{
			ChangeToExtendAttack( pOwner );
			if( bSetAim )
			{
				AttachDummyAimEffect( pOwner );
				SetDummyAimTarget( pOwner, m_iSpecialAttackLevel, m_AimedTarget, true );
			}
			
			m_iSpecialAttackLevel--;
		}
	}
	else if ( iState == ST_CHARGE_ATTACK_FIRE_END )
	{
		D3DXQUATERNION qtRot;
		bool bLastFire = false;
		rkPacket >> qtRot;
		rkPacket >> bLastFire;
		pOwner->SetTargetRotAndMoveDirChange( qtRot );

		ExtendAttackFire( pOwner, bLastFire );
	}
	else if ( iState == ST_CHARGE_ATTACK_END )
	{
		D3DXQUATERNION qtRot;
		rkPacket >> qtRot;
		pOwner->SetTargetRotAndMoveDirChange( qtRot );
	}
	else if ( iState == ST_NONE )
	{
		m_iSpecialAttackLevel = 0;
		m_ChargeState = CS_NONE;
	}
}

void ioRocketLauncherItem::OnItemGaugeCancel( ioBaseChar *pOwner, SP2Packet &rkPacket )
{
	int iState;
	rkPacket >> iState;

	if( iState == ST_DIR )
	{
		D3DXQUATERNION qtRot;
		rkPacket >> qtRot;
		if( pOwner )
			pOwner->SetTargetRotToRotate( qtRot, true );

	}
	else if( iState == ST_RELOAD )
	{
		m_ChargeState = CS_NONE;
		pOwner->SetAttackMoveEnable( false );
		pOwner->SetReloadMoveEnable( false );
		pOwner->SetState( CS_DELAY );
	}
}

void ioRocketLauncherItem::WasteBullet( ioBaseChar* pOwner )
{
	m_iCurBullet -= m_iNeedBullet;
	m_iCurBullet = max( m_iCurBullet, 0 );
}

void ioRocketLauncherItem::SetCurBullet( int iCurBullet )
{
	if( iCurBullet < 0 )
		return;

	int iBullet = min( iCurBullet, m_iMaxBullet );
	iBullet = max( 0, iBullet );

	m_iCurBullet = iBullet;
}

bool ioRocketLauncherItem::CheckExtendDefenseEnable( ioBaseChar *pChar )
{
	if( m_iCurBullet == m_iMaxBullet)
		return false;

	ioEntityGroup *pGrp = pChar->GetGroup();
	int iAniID = pGrp->GetAnimationIdx( m_ReloadAnimation );
	if( iAniID == -1 )	return false;

	return true;
}

bool ioRocketLauncherItem::SetExtendDefenseState( ioBaseChar *pOwner )
{
	if( m_iCurBullet == m_iMaxBullet)
		return false;

	ioEntityGroup *pGrp = pOwner->GetGroup();

	int iAniID = pGrp->GetAnimationIdx( m_ReloadAnimation );
	if( iAniID == -1 )	return false;

	int iMaxBullet = m_iMaxBullet;
	int iCurBullet = m_iCurBullet;

	float fTimeRate = m_fReloadTimeRate;

	int iLevelCount = m_LevelTimeRate.size();
	if( iLevelCount > 1 )
	{
		int iDividCnt, iNeedCnt;
		iDividCnt = iMaxBullet / iLevelCount;
		iNeedCnt = iMaxBullet - iCurBullet;

		LevelTimeRate::iterator iter = m_LevelTimeRate.begin();
		for( int i=1; i <= iLevelCount; i++ )
		{
			if( iNeedCnt <= iDividCnt * i )
			{
				fTimeRate *= *iter;
				break;
			}

			++iter;
		}
	}
	else
	{
		fTimeRate = m_fReloadTimeRate;
	}

	m_fReloadTimeModifyRate = m_fReloadTimeRate;

	m_bSetReloadMoveAni = false;
	pGrp->ClearAllLoopAni( FLOAT100, true );
	pGrp->ClearAllActionAni( FLOAT100, true );
	pGrp->SetLoopAni( iAniID, FLOAT100, FLOAT1, FLOAT1/m_fReloadTimeModifyRate );

	m_dwReloadStartTime = FRAMEGETTIME();
	m_dwReloadEndTime = m_dwReloadStartTime + pGrp->GetAnimationFullTime( iAniID ) * fTimeRate - FLOAT100;

	m_dwReloadGapTime = ( m_dwReloadEndTime - m_dwReloadStartTime ) / ( m_iMaxBullet / m_iNeedBullet );
	m_dwNextReloadTime = m_dwReloadStartTime + m_dwReloadGapTime;

	pOwner->SetState( CS_EXTEND_DEFENSE );
	pOwner->SetReloadMoveEnable( true );

	if( !m_ReloadEffect.IsEmpty() )
	{
		pOwner->ShowWeaponItemMesh( false );
		pOwner->AttachEffect( m_ReloadEffect );
	}

	return true;
}

void ioRocketLauncherItem::CheckExtendDefenseState( ioBaseChar *pOwner )
{
	if( !pOwner->IsNeedProcess() ) return;

	DWORD dwCurTime = FRAMEGETTIME();
	if( m_dwReloadEndTime + pOwner->GetActionStopTotalDelay() > dwCurTime )
	{
		if( m_dwNextReloadTime < dwCurTime )
		{
			m_iCurBullet += 4;
			m_iCurBullet = min( m_iCurBullet, m_iMaxBullet );
			m_dwNextReloadTime += m_dwReloadGapTime;
		}
	}
	else
	{
		m_iCurBullet = m_iMaxBullet;
		m_bSetReloadMoveAni = false;
		m_ChargeState = CS_NONE;
		pOwner->SetReloadMoveEnable( false );
		pOwner->SetState( CS_DELAY );

		if( pOwner->IsNeedSendNetwork() )
		{
			SP2Packet kPacket( CUPK_ITEM_GAUGE_CANCEL );
			FillItemGaugePacketHeader( pOwner, kPacket );
			kPacket << ST_RELOAD;
			P2PNetwork::SendToAllPlayingUser( kPacket );
		}
	}

	g_TutorialMgr.ActionWeaponCharge();
}

void ioRocketLauncherItem::OnEndExtendDefense( ioBaseChar *pOwner )
{
	if( !m_ReloadEffect.IsEmpty() )
	{
		pOwner->EndEffect( m_ReloadEffect, false );
		pOwner->ShowWeaponItemMesh( true );
	}
}

void ioRocketLauncherItem::CheckDefenseMoveAniState( ioBaseChar *pOwner )
{
	if( !pOwner->IsCanReloadMove() ) return;

	DWORD dwCurTime = FRAMEGETTIME();
	ioEntityGroup *pGrp = pOwner->GetGroup();
	int iAniID = 0;

	if( pOwner->IsSettedDirection() )
	{
		if( !m_bSetReloadMoveAni )
		{
			iAniID = pGrp->GetAnimationIdx(m_ReloadMoveAnimation);
			if( iAniID == -1 )
				return;

			DWORD dwPlayTime = 0;
			ioAnimation *pAni = pGrp->GetLoopInPlay( m_ReloadAnimation );
			
			if( pAni )
				dwPlayTime = pAni->GetCurTime();

			pGrp->ClearAllActionAni( FLOAT100, true );
			pGrp->ClearAllLoopAni( FLOAT100, true );
			pGrp->SetLoopAniInMid( iAniID, dwPlayTime, FLOAT100, FLOAT1, FLOAT1/m_fReloadTimeModifyRate );
			m_bSetReloadMoveAni = true;
		}
	}
	else
	{
		if( m_bSetReloadMoveAni )
		{
			iAniID = pGrp->GetAnimationIdx(m_ReloadAnimation);
			if( iAniID == -1 )
				return;

			DWORD dwPlayTime = 0;
			ioAnimation *pAni = pGrp->GetLoopInPlay( m_ReloadMoveAnimation );
			
			if( pAni )
				dwPlayTime = pAni->GetCurTime();

			pGrp->ClearAllActionAni( FLOAT100, true );
			pGrp->ClearAllLoopAni( FLOAT100, true );
			pGrp->SetLoopAniInMid( iAniID, dwPlayTime, FLOAT100, FLOAT1, FLOAT1/m_fReloadTimeModifyRate );
			m_bSetReloadMoveAni = false;
		}
	}
}

float ioRocketLauncherItem::GetAttackSpeedRate() const
{
	return m_fAttackSpeedRate;
}

void ioRocketLauncherItem::SetNoneState( ioBaseChar* pOwner, int eNewState /* = 0 */ )
{
	CharState eNewCharState = (CharState)eNewState;
	switch( eNewCharState )
	{
	case CS_WOUNDED:
	case CS_BLOW_WOUNDED:
	case CS_ETC_STATE:
	case CS_FALL:
	case CS_FROZEN:
	case CS_CLOSEORDER:
	case CS_FLOAT_STATE:
	case CS_STOP_MOTION:
	case CS_DUMMY_STATE:
	case CS_BOUND_BLOW_WOUNDED:
	case CS_ICE_STATE:
	case CS_BLOW_EXTEND_WOUND:
	case CS_PANIC_MOVE:
	case CS_MAGIC_STUN:
		InitChargeState( pOwner, true, true );
	}
}

void ioRocketLauncherItem::AttachDummyAimEffect( ioBaseChar* pOwner )
{
	if( !pOwner )
		return;

	if( (int)m_vAttactStepAimEffectList.size() >= (int)SAAS_MAX_LEVEL &&
		!m_vAttactStepAimEffectList[ SAAS_FIRST_LEVEL ].IsEmpty() && !m_vAttactStepAimEffectList[ SAAS_SECOND_LEVEL ].IsEmpty() && !m_vAttactStepAimEffectList[ SAAS_THIRD_LEVEL ].IsEmpty() )
	{
		pOwner->SetTargetMarker( ioTargetMarker::MT_OTHER_MARKER,
			"", "", m_vAttactStepAimEffectList[ SAAS_FIRST_LEVEL ], m_vAttactStepAimEffectList[ SAAS_SECOND_LEVEL ], m_vAttactStepAimEffectList[ SAAS_THIRD_LEVEL ] );
	}
}

void ioRocketLauncherItem::SetDummyAimTarget( ioBaseChar* pOwner, int iAttackStep, ioHashString stTargetName, bool bSync )
{
	if( !pOwner )
		return;
	ioDummyChar *pDummyChar = g_DummyCharMgr.FindDummyCharToName( pOwner, m_DummyCharName );
	ioOwnerControlDummy6* pOwnerControlDummy6 = ToOwnerControlDummy6( pDummyChar );
	if( !pOwnerControlDummy6 && !bSync )
		return;

	if( stTargetName.IsEmpty() )
		return;

	if( iAttackStep == 4 )
		iAttackStep = 3;

	if( (int)m_vAttactStepAimEffectList.size() > iAttackStep - 1 )
	{
		if( iAttackStep == 1 )
			pOwner->SetTargetMarkerState( ioTargetMarker::MS_AIMED );
		if( iAttackStep == 2 )
			pOwner->SetTargetMarkerState( ioTargetMarker::MS_ZERO_AIMED );
		if( iAttackStep == 3 )
			pOwner->SetTargetMarkerState( ioTargetMarker::MS_HEAD_AIMED);

		if( iAttackStep > 0 && iAttackStep <= 3 )
		{
			m_AimedTarget = stTargetName;
			pOwner->SetTargetMarkerTarget( stTargetName, false );
		}
	}
}