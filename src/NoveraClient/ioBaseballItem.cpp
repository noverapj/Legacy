#include "StdAfx.h"

#include "ioBaseballItem.h"
#include "WeaponDefine.h"

ioBaseballItem::ioBaseballItem(void)
{
	ClearData();
}

ioBaseballItem::ioBaseballItem( const ioBaseballItem &rhs ):
ioWeaponItem( rhs ),
	m_AttackReadyAniList( rhs.m_AttackReadyAniList ),
	m_szGatheringAni( rhs.m_szGatheringAni ),
	m_fGatheringAniRate( rhs.m_fGatheringAniRate ),
	m_szSpecialDelayAni( rhs.m_szSpecialDelayAni ),
	m_fSpecialDelayAniRate( rhs.m_fSpecialDelayAniRate ),
	m_szSpecialEndAni( rhs.m_szSpecialEndAni ),
	m_fSpecialEndAniRate( rhs.m_fSpecialEndAniRate ),
	m_dwGatheringTime( rhs.m_dwGatheringTime ),
	m_dwSpecialDelayTime( rhs.m_dwSpecialDelayTime ),
	m_LevelSwing( rhs.m_LevelSwing ),
	m_UpperSwing( rhs.m_UpperSwing ),
	m_PitchingAttack( rhs.m_PitchingAttack ),
	m_dwBattingWeapon( rhs.m_dwBattingWeapon ),
	m_fBattingMinAngle( rhs.m_fBattingMinAngle ),
	m_fBattingMaxAngle( rhs.m_fBattingMaxAngle ),
	m_fBattingMinRange( rhs.m_fBattingMinRange ),
	m_fBattingMaxRange( rhs.m_fBattingMaxRange ),
	m_szBattingSound( rhs.m_szBattingSound ),
	m_fMaxGauge( rhs.m_fMaxGauge ),
	m_fPitchingNeedGauge( rhs.m_fPitchingNeedGauge ),
	m_fStandNeedGauge( rhs.m_fStandNeedGauge ),
	m_fEndReturnGauge( rhs.m_fEndReturnGauge ),
	m_fIncreaseGauge( rhs.m_fIncreaseGauge )
{
	m_fCurGauge = 0.0f;

	ClearData();
}

ioBaseballItem::~ioBaseballItem(void)
{
	ClearData();
}

void ioBaseballItem::ClearData()
{
	m_ChargeState = CS_NONE;
	
	m_iCurCombo = 0;
	m_dwMotionStartTime = 0;
	m_dwMotionEndTime = 0;
	m_dwFireTime = 0;
}

ioItem* ioBaseballItem::Clone()
{
	return new ioBaseballItem( *this );
}

ioWeaponItem::WeaponSubType ioBaseballItem::GetSubType() const
{
	return WST_BASEBALL_ITEM;
}

void ioBaseballItem::SetNoneState( ioBaseChar* pOwner, int eNewState /* = 0 */ )
{
	ClearData();
}

void ioBaseballItem::LoadProperty( ioINILoader &rkLoader )
{
	ioWeaponItem::LoadProperty( rkLoader );

	char szBuf[MAX_PATH];

	int iMaxCombo = GetMaxCombo();
	m_AttackReadyAniList.clear();
	for( int i=0; i < iMaxCombo; ++i )
	{
		wsprintf_e( szBuf, "attack_ready_animation%d", i+1 );
		rkLoader.LoadString( szBuf, "", szBuf, MAX_PATH );
		m_AttackReadyAniList.push_back( ioHashString(szBuf) );
	}

	rkLoader.LoadString_e( "gathering_animation", "", szBuf, MAX_PATH );
	m_szGatheringAni = szBuf;
	m_fGatheringAniRate = rkLoader.LoadFloat_e( "gathering_ani_rate", FLOAT1 );

	rkLoader.LoadString_e( "special_delay_animation", "", szBuf, MAX_PATH );
	m_szSpecialDelayAni = szBuf;
	m_fSpecialDelayAniRate = rkLoader.LoadFloat_e( "special_delay_ani_rate", FLOAT1 );
	rkLoader.LoadString_e( "special_end_animation", "", szBuf, MAX_PATH );
	m_szSpecialEndAni = szBuf;
	m_fSpecialEndAniRate = rkLoader.LoadFloat_e( "special_end_ani_rate", FLOAT1 );

	m_dwGatheringTime = (DWORD)rkLoader.LoadInt_e( "gathering_time", 0 );
	m_dwSpecialDelayTime = (DWORD)rkLoader.LoadInt_e( "special_delay_time", 0 );

	LoadAttackAttribute_e( "level_swing_attack", m_LevelSwing, rkLoader );
	LoadAttackAttribute_e( "upper_swing_attack", m_UpperSwing, rkLoader );
	LoadAttackAttribute_e( "pitching_attack", m_PitchingAttack, rkLoader );
	m_dwBattingWeapon = (DWORD)rkLoader.LoadInt_e( "batting_weapon_type", 0 );

	m_fBattingMinAngle = rkLoader.LoadFloat_e( "batting_min_angle", 0.f );
	m_fBattingMaxAngle = rkLoader.LoadFloat_e( "batting_max_angle", FLOAT360 );
	m_fBattingMinRange = rkLoader.LoadFloat_e( "batting_min_range", FLOAT1 );
	m_fBattingMaxRange = rkLoader.LoadFloat_e( "batting_max_range", FLOAT1 );
	rkLoader.LoadString_e( "batting_sound", "", szBuf, MAX_PATH );
	m_szBattingSound = szBuf;

	m_fMaxGauge = rkLoader.LoadFloat_e( "max_gauge", FLOAT100 );
	m_fPitchingNeedGauge = rkLoader.LoadFloat_e( "pitching_need_gauge", FLOAT1 );
	m_fStandNeedGauge = rkLoader.LoadFloat_e( "stand_need_gauge", FLOAT1 );
	m_fEndReturnGauge = rkLoader.LoadFloat_e( "end_return_gauge", FLOAT1 );
	m_fIncreaseGauge = rkLoader.LoadFloat_e( "increase_gauge", FLOAT1 );
}

//Normal Attack
void ioBaseballItem::SetNormalAttack( ioBaseChar *pOwner, int iCurCombo )
{
	if( !pOwner ) return;

	m_fCurChargeRate = 0.0f;
	m_fCurChargeRateForSkill = 0.0f;

	m_dwKeyReserveTime = 0;
	m_dwMotionStartTime = 0;
	m_dwMotionEndTime = 0;
	m_dwFireTime = 0;

	m_iSkillInput = -1;
	m_bEnableDefenseState = false;
	m_bEnableAttackState = false;
	m_bEnableJumpState = false;

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

		int iAniID = pGrp->GetAnimationIdx( m_AttackReadyAniList[iCurCombo] );
		float fTimeRate = (float)m_dwChargeTime / pGrp->GetAnimationFullTime( iAniID );
		pGrp->SetActionAni( iAniID, FLOAT100, FLOAT100, FLOAT1, FLOAT1/fTimeRate, 0.0f, 0.0f, true );

		m_ChargeState = CS_CHARGING;
		m_dwMotionStartTime = FRAMEGETTIME();
		m_iCurCombo = iCurCombo;
		pOwner->SetState( CS_ATTACK );
		pOwner->SetAttackMoveEnable( false );
		pOwner->SetChargingState( true );

		if( m_bChargeAutoTarget )
		{
			DWORD dwTrackingTime = FRAMEGETTIME();
			dwTrackingTime += pGrp->GetAnimationEventTime_e( iAniID, "tracking_e" ) * fTimeRate;
			pOwner->SetAutoTargetTrackingEndTime( dwTrackingTime );
		}
		else
		{
			pOwner->StopAutoTargetTracking();
		}
	}
}

void ioBaseballItem::CheckNormalAttack( ioBaseChar *pOwner )
{
	DWORD dwCurTime = FRAMEGETTIME();
	switch( m_ChargeState )
	{
	case CS_CHARGING:
		OnCharging( pOwner );
		break;
	case CS_GATHERING:
		OnGathering( pOwner );
		break;
	case CS_SPECIAL_DELAY:
		ProcessSpecialDelay( pOwner );
		break;
	case CS_UPPER_SWING:
	case CS_LEVEL_SWING:
		if( m_dwFireTime > 0 && m_dwFireTime < dwCurTime )
		{
			m_dwFireTime = 0;
			CheckBattingWeapon( pOwner );
		}
		else if( m_dwMotionEndTime > 0 && m_dwMotionEndTime < dwCurTime )
		{
			pOwner->SetState( CS_DELAY );
		}
		break;
	case CS_SPECIAL_END:
	case CS_PITCHING:
		if( m_dwMotionEndTime > 0 && m_dwMotionEndTime < dwCurTime )
		{
			pOwner->SetState( CS_DELAY );
		}
		break;
	case CS_NORMAL_ATTACK:
		if( pOwner->IsNeedProcess() )
		{
			CheckCancelReserve( pOwner );

			if( ProcessCancel(pOwner) )
				return;
		}
		ioWeaponItem::CheckNormalAttack( pOwner );
		break;
	}
}

void ioBaseballItem::OnReleased( ioBaseChar *pOwner )
{
	int iCount = m_vPitchingWeaponIndex.size();
	for( int i = 0; i < iCount; i++ )
	{
		ioWeapon *pWeapon = g_WeaponMgr.FindWeapon( pOwner, m_vPitchingWeaponIndex[i] );
		if( pWeapon && pWeapon->IsLive() )
			pWeapon->SetWeaponDead();
	}
	m_vPitchingWeaponIndex.clear();
	m_fCurGauge = 0.0f;

	ioAttackableItem::OnReleased( pOwner );
}

void ioBaseballItem::OnCharging( ioBaseChar *pOwner )
{
	if( !pOwner || !pOwner->IsNeedProcess() )	return;

	if( pOwner->IsAttackKeyDown() && !pOwner->IsHasCrown() )
	{
		if( m_dwMotionStartTime + m_dwChargeTime <= FRAMEGETTIME() )
		{
			ChangeGatheringState( pOwner );
		}
	}
	else
	{
		ChangeToNormalAttack( pOwner );
	}
}

void ioBaseballItem::OnGathering( ioBaseChar *pOwner )
{
	if( !pOwner )
		return;

	if( !pOwner->IsNeedProcess() )	
		return;

	if( pOwner->IsAttackKeyDown() && !pOwner->IsHasCrown() && IsEnableGauge( m_fStandNeedGauge ) )
	{
		if( m_dwMotionEndTime <= FRAMEGETTIME() )
			ChangeSpecialDelayState( pOwner );
	}
	else if( IsEnableGauge( m_fPitchingNeedGauge ) )
		ChangePitchingState( pOwner );
	else
		ChangeToNormalAttack( pOwner );
}

void ioBaseballItem::ProcessSpecialDelay( ioBaseChar *pOwner )
{
	if( !pOwner->IsNeedProcess() )	
		return;

	DWORD dwCurTinme = FRAMEGETTIME();

	if( pOwner->IsAttackKey() )
	{
		ChangeUpperSwingState( pOwner );
	}
	else if( pOwner->IsJumpKey() )
	{
		ChangeLevelSwingState( pOwner );
	}
	else if( pOwner->IsDefenseKey() )
	{
		CheckIncreaseGauge( m_fEndReturnGauge );
		ChangeSpecialEndState( pOwner );
	}
	else if( dwCurTinme > m_dwMotionEndTime )
	{
		ChangeSpecialEndState( pOwner );
	}
}

void ioBaseballItem::ChangeGatheringState( ioBaseChar *pOwner )
{
	if( !pOwner ) return;

	ioEntityGroup *pGrp = pOwner->GetGroup();
	if( !pGrp )
		return;

	DWORD dwCurTime = FRAMEGETTIME();

	pGrp->ClearAllLoopAni( FLOAT100, true );
	pGrp->ClearAllActionAni( FLOAT100, true );

	int iAniID	     = pGrp->GetAnimationIdx( m_szGatheringAni );
	float fTimeRate  = m_fGatheringAniRate;

	pGrp->SetActionAni( iAniID, FLOAT100, FLOAT100, FLOAT1, FLOAT1/fTimeRate, 0.0f, 0.0f, true );

	m_ChargeState = CS_GATHERING;
	if( m_dwGatheringTime == 0 )
		m_dwMotionEndTime = dwCurTime + ( pGrp->GetAnimationFullTime( iAniID ) * fTimeRate );
	else
		m_dwMotionEndTime = dwCurTime + m_dwGatheringTime;

	if( pOwner->IsNeedSendNetwork() )
	{
		SP2Packet kPacket( CUPK_ITEM_GAUGE_CANCEL );
		FillItemGaugePacketHeader( pOwner, kPacket );
		kPacket << static_cast<byte>( m_ChargeState );
		kPacket << m_iCurCombo;
		kPacket << pOwner->GetWorldPosition();
		kPacket << pOwner->GetTargetRot();
		P2PNetwork::SendToAllPlayingUser( kPacket );
	}
}

void ioBaseballItem::ChangeSpecialDelayState( ioBaseChar *pOwner )
{
	if( !pOwner ) return;

	ioEntityGroup *pGrp = pOwner->GetGroup();
	if( !pGrp )
		return;

	DWORD dwCurTime = FRAMEGETTIME();

	pGrp->ClearAllLoopAni( FLOAT100, true );
	pGrp->ClearAllActionAni( FLOAT100, true );

	int iAniID	     = pGrp->GetAnimationIdx( m_szSpecialDelayAni );
	float fTimeRate  = m_fSpecialDelayAniRate;

	pGrp->SetLoopAni( iAniID, FLOAT100, FLOAT1, FLOAT1/fTimeRate );

	m_ChargeState = CS_SPECIAL_DELAY;
	if( m_dwSpecialDelayTime == 0 )
		m_dwMotionEndTime = dwCurTime + ( pGrp->GetAnimationFullTime( iAniID ) * fTimeRate );
	else
		m_dwMotionEndTime = dwCurTime + m_dwSpecialDelayTime;

	pOwner->ChangeDirectionByInputDir( true );
	CheckDecreaseGauge( m_fStandNeedGauge );
	if( pOwner->IsNeedSendNetwork() )
	{
		SP2Packet kPacket( CUPK_ITEM_GAUGE_CANCEL );
		FillItemGaugePacketHeader( pOwner, kPacket );
		kPacket << static_cast<byte>( m_ChargeState );
		kPacket << m_iCurCombo;
		kPacket << pOwner->GetWorldPosition();
		kPacket << pOwner->GetTargetRot();
		P2PNetwork::SendToAllPlayingUser( kPacket );
	}
}

void ioBaseballItem::ChangeUpperSwingState( ioBaseChar *pOwner )
{
	if( !pOwner ) return;

	m_ChargeState = CS_UPPER_SWING;
	CheckPitchingWeapon( pOwner );
	SetAttackAttribute( pOwner, m_UpperSwing );

	if( pOwner->IsNeedSendNetwork() )
	{
		SP2Packet kPacket( CUPK_ITEM_GAUGE_CANCEL );
		FillItemGaugePacketHeader( pOwner, kPacket );
		kPacket << static_cast<byte>( m_ChargeState );
		kPacket << m_iCurCombo;
		kPacket << pOwner->GetWorldPosition();
		kPacket << pOwner->GetTargetRot();
		P2PNetwork::SendToAllPlayingUser( kPacket );
	}
}

void ioBaseballItem::ChangeLevelSwingState( ioBaseChar *pOwner )
{
	if( !pOwner ) return;

	m_ChargeState = CS_LEVEL_SWING;
	CheckPitchingWeapon( pOwner );
	SetAttackAttribute( pOwner, m_LevelSwing );

	if( pOwner->IsNeedSendNetwork() )
	{
		SP2Packet kPacket( CUPK_ITEM_GAUGE_CANCEL );
		FillItemGaugePacketHeader( pOwner, kPacket );
		kPacket << static_cast<byte>( m_ChargeState );
		kPacket << m_iCurCombo;
		kPacket << pOwner->GetWorldPosition();
		kPacket << pOwner->GetTargetRot();
		P2PNetwork::SendToAllPlayingUser( kPacket );
	}
}

void ioBaseballItem::ChangeSpecialEndState( ioBaseChar *pOwner )
{
	if( !pOwner ) return;

	ioEntityGroup *pGrp = pOwner->GetGroup();
	if( !pGrp )
		return;

	DWORD dwCurTime = FRAMEGETTIME();

	pGrp->ClearAllLoopAni( FLOAT100, true );
	pGrp->ClearAllActionAni( FLOAT100, true );

	int iAniID	     = pGrp->GetAnimationIdx( m_szSpecialEndAni );
	float fTimeRate  = m_fSpecialEndAniRate;

	pGrp->SetActionAni( iAniID, FLOAT100, FLOAT100, FLOAT1, FLOAT1/fTimeRate, 0.0f, 0.0f, true );

	m_ChargeState = CS_SPECIAL_END;
	m_dwMotionEndTime = dwCurTime + ( pGrp->GetAnimationFullTime( iAniID ) * fTimeRate );

	if( pOwner->IsNeedSendNetwork() )
	{
		SP2Packet kPacket( CUPK_ITEM_GAUGE_CANCEL );
		FillItemGaugePacketHeader( pOwner, kPacket );
		kPacket << static_cast<byte>( m_ChargeState );
		kPacket << m_iCurCombo;
		kPacket << pOwner->GetWorldPosition();
		kPacket << pOwner->GetTargetRot();
		P2PNetwork::SendToAllPlayingUser( kPacket );
	}
}

void ioBaseballItem::ChangePitchingState( ioBaseChar *pOwner )
{
	if( !pOwner ) return;
	
	m_ChargeState = CS_PITCHING;
	CheckPitchingWeapon( pOwner );

	SetAttackAttribute( pOwner, m_PitchingAttack );
	CheckDecreaseGauge( m_fPitchingNeedGauge );
	m_vPitchingWeaponIndex.push_back( pOwner->GetWeaponIndexBase() );

	if( pOwner->IsNeedSendNetwork() )
	{
		SP2Packet kPacket( CUPK_ITEM_GAUGE_CANCEL );
		FillItemGaugePacketHeader( pOwner, kPacket );
		kPacket << static_cast<byte>( m_ChargeState );
		kPacket << m_iCurCombo;
		kPacket << pOwner->GetWorldPosition();
		kPacket << pOwner->GetTargetRot();
		P2PNetwork::SendToAllPlayingUser( kPacket );
	}
}

void ioBaseballItem::CheckPitchingWeapon( ioBaseChar *pOwner )
{
	DWORDVec::iterator iter = m_vPitchingWeaponIndex.begin();
	while( iter != m_vPitchingWeaponIndex.end() )
	{
		ioWeapon *pWeapon = g_WeaponMgr.FindWeapon( pOwner, *iter );
		if( pWeapon && pWeapon->IsLive() )
			++iter;
		else
			m_vPitchingWeaponIndex.erase( iter );
	}
}

void ioBaseballItem::CheckBattingWeapon( ioBaseChar *pOwner )
{
	DWORDVec::iterator iter = m_vPitchingWeaponIndex.begin();
	DWORDVec vWeaponIndex;
	while( iter != m_vPitchingWeaponIndex.end() )
	{
		ioWeapon *pWeapon = g_WeaponMgr.FindWeapon( pOwner, *iter );
		if( pWeapon && pWeapon->IsLive() )
		{
			D3DXVECTOR3 vDiff = pWeapon->GetPosition() - pOwner->GetWorldPosition();
			D3DXVECTOR3 vCharDir = pOwner->GetMoveDir();

			float fRangeMaxSq = m_fBattingMaxRange * m_fBattingMaxRange;
			float fRangeMinSq = m_fBattingMinRange * m_fBattingMinRange;
			float fTargetDist = D3DXVec3LengthSq( &vDiff );
			if( fTargetDist > fRangeMaxSq || fTargetDist < fRangeMinSq )
			{
				++iter;
				continue;
			}
			float fCurAngleRate = fTargetDist / fRangeMaxSq;
			float fCurAngle = m_fBattingMinAngle * fCurAngleRate + m_fBattingMaxAngle * (1.0f - fCurAngleRate );
			float fHalfCosine = cosf( DEGtoRAD( fCurAngle ) * FLOAT05 );

			vDiff.y = 0.0f;
			D3DXVec3Normalize( &vDiff, &vDiff );
			if( D3DXVec3Dot( &vCharDir, &vDiff ) >= fHalfCosine )
			{
				pWeapon->SetWeaponDead();

				FireTime kFireTime;
				kFireTime.dwStart = FRAMEGETTIME();
				kFireTime.iAttributeIdx = m_dwBattingWeapon;
				kFireTime.iResistanceIdx = 0;
				kFireTime.szWoundedAni  = "";
				kFireTime.dwWoundedAniDuration = 0;
				kFireTime.bLoopWoundedAni = false;
				kFireTime.dwWeaponIdx = pOwner->IncreaseWeaponIndexBase();
				kFireTime.eFireTimeType = FTT_NORMAL_ATTACK;

				D3DXVECTOR3 vPos = pOwner->GetMidPositionByRate();
				pOwner->ExtendFireExplicit( kFireTime, vPos, vCharDir, "" );

				m_vPitchingWeaponIndex.erase( iter );
				vWeaponIndex.push_back( kFireTime.dwWeaponIdx );

				if( !m_szBattingSound.IsEmpty() )
				{
					ioEntityGroup *pGrp = pOwner->GetGroup();
					if( pGrp )
						g_SoundMgr.PlaySound( m_szBattingSound, pGrp->GetParentSceneNode() );
				}
			}
			else
				++iter;
		}
		else
			m_vPitchingWeaponIndex.erase( iter );
	}
	if( vWeaponIndex.size() != 0 )
		m_vPitchingWeaponIndex.insert( m_vPitchingWeaponIndex.begin(), vWeaponIndex.begin(), vWeaponIndex.end() );
}

void ioBaseballItem::ChangeToNormalAttack( ioBaseChar *pOwner )
{
	DWORD dwCurTime = FRAMEGETTIME();

	pOwner->InitExtendAttackTagInfo();
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
		SP2Packet kPacket( CUPK_ITEM_GAUGE_CANCEL );
		FillItemGaugePacketHeader( pOwner, kPacket );
		kPacket << static_cast<byte>( m_ChargeState );
		kPacket << m_iCurCombo;
		kPacket << pOwner->GetWorldPosition();
		kPacket << pOwner->GetTargetRot();
		if( !m_bChargeAutoTarget )
			pOwner->FillAutoTargetInfo( kPacket );
		P2PNetwork::SendToAllPlayingUser( kPacket );
	}
}

void ioBaseballItem::SetAttackAttribute( ioBaseChar *pOwner, const AttackAttribute& Attr )
{
	if( !pOwner )
		return;

	ioEntityGroup *pGrp = pOwner->GetGroup();
	if( !pGrp )
		return;

	pOwner->InitExtendAttackTagInfo();
	pOwner->ClearReservedSliding();
	pOwner->SetForcePowerAmt( 0.0f );

	pGrp->ClearAllLoopAni( FLOAT100, true );
	pGrp->ClearAllActionAni( FLOAT100, true );

	int iAniID	     = pGrp->GetAnimationIdx( Attr.m_AttackAnimation );
	float fTimeRate  = Attr.m_fAttackAniRate;
	DWORD dwPreDelay = Attr.m_dwPreDelay;
	DWORD dwEndDelay = Attr.m_dwEndDelay;

	pGrp->SetActionAni( iAniID, FLOAT10, FLOAT10, FLOAT1, FLOAT1/fTimeRate, 0.0f, dwPreDelay, true );

	pOwner->RefreshFireTimeList( iAniID, Attr.m_vWeaponInfoList, FTT_NORMAL_ATTACK, fTimeRate, dwPreDelay );
	pOwner->CheckCharColSkipTime( iAniID, fTimeRate, dwPreDelay );
	pOwner->SetReservedSliding( Attr.m_vForceInfoList, iAniID, fTimeRate, dwPreDelay );

	DWORD dwCurTime = FRAMEGETTIME();
	m_dwMotionEndTime = dwCurTime + pGrp->GetAnimationFullTime( iAniID ) * fTimeRate;
	m_dwFireTime = dwCurTime + pGrp->GetAnimationEventTime_e( iAniID, "fire_s" ) * fTimeRate;
}

void ioBaseballItem::OnItemGaugeCancel( ioBaseChar *pOwner, SP2Packet &rkPacket )
{
	BYTE btState;
	rkPacket >> btState;
	rkPacket >> m_iCurCombo;

	D3DXVECTOR3 vPos;
	rkPacket >> vPos;
	pOwner->SetWorldPosition( vPos );

	D3DXQUATERNION qtRot;
	rkPacket >> qtRot;
	pOwner->SetTargetRotToRotate( qtRot, true );

	switch( btState )
	{
	case CS_NORMAL_ATTACK:
		{
			if( !m_bChargeAutoTarget )
				pOwner->ApplyAutoTargetInfo( rkPacket );

			ChangeToNormalAttack( pOwner );
		}
		break;
	case CS_GATHERING:
		ChangeGatheringState( pOwner );
		break;
	case CS_SPECIAL_DELAY:
		ChangeSpecialDelayState( pOwner );
		break;
	case CS_UPPER_SWING:
		ChangeUpperSwingState( pOwner );
		break;
	case CS_LEVEL_SWING:
		ChangeLevelSwingState( pOwner );
		break;
	case CS_SPECIAL_END:
		ChangeSpecialEndState( pOwner );
		break;
	case CS_PITCHING:
		ChangePitchingState( pOwner );
		break;
	}
}

void ioBaseballItem::CheckDecreaseGauge( float fNeedGauge )
{
	m_fCurGauge -= fNeedGauge;
	m_fCurGauge = (float)max( 0, min( m_fCurGauge, m_fMaxGauge ) );
}

void ioBaseballItem::CheckIncreaseGauge( float fGauge )
{
	m_fCurGauge += fGauge;
	m_fCurGauge = (float)max( 0, min( m_fCurGauge, m_fMaxGauge ) );
}

int ioBaseballItem::GetMaxBullet()
{
	return (int)m_fMaxGauge;
}

int ioBaseballItem::GetNeedBullet()
{
	return (int)m_fStandNeedGauge;
}

int ioBaseballItem::GetCurBullet()
{
	float fCurCoolTime = m_fCurGauge;
	fCurCoolTime = max( 0, min( fCurCoolTime, m_fMaxGauge ) );

	return (int)fCurCoolTime;
}

void ioBaseballItem::SetCurBullet( int iCurBullet )
{
	if( iCurBullet < 0 )
		return;

	float fCurBullet = iCurBullet;
	fCurBullet = max( 0, min( fCurBullet, m_fMaxGauge ) );

	m_fCurGauge = fCurBullet;
}

bool ioBaseballItem::IsEnableGauge( float fNeedGauge )
{
	if( m_fCurGauge >= fNeedGauge )
		return true;

	return false;
}

void ioBaseballItem::UpdateExtraData( ioBaseChar *pOwner )
{
	if( !pOwner )
		return;

	float fTimePerSec = g_FrameTimer.GetSecPerFrame();
	switch( pOwner->GetState() )
	{
	case CS_DIE:
	case CS_DROP_ZONE_DOWN:
	case CS_READY:
	case CS_VIEW:
	case CS_OBSERVER:
	case CS_LOADING:
	case CS_FISHING:
	case CS_STARTMOTION:
	case CS_ENDMOTION:
	case CS_EXCAVATING:
	case CS_CREATEMOTION:
	case CS_ROULETTE:
		return;
	case CS_DELAY:
	case CS_RUN:
	case CS_DASH:
		if( m_fCurGauge < m_fMaxGauge )
		{
			float fGauge = m_fIncreaseGauge * fTimePerSec;

			m_fCurGauge += fGauge;
			m_fCurGauge = min( m_fCurGauge, m_fMaxGauge );
		}
		return;
	}
}

bool ioBaseballItem::IsEnableAttackRotateCheck( ioBaseChar *pOwner )
{
	if( pOwner->GetState() == CS_ATTACK )
	{
		switch( m_ChargeState )
		{
		case CS_NONE:
		case CS_SPECIAL_DELAY:
			return true;
		}
	}

	return false;
}