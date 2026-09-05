
#include "stdafx.h"

#include "ioPlayStage.h"
#include "ioBaseChar.h"

#include "ioDarkSeasItem.h"

#include "WeaponDefine.h"
#include "ItemDefine.h"

#include "ioFlyFollowStateBuff.h"

ioDarkSeasItem::ioDarkSeasItem()
{
	m_ChargeState = CS_NONE;

	m_dwAttackStartTime = 0;
	m_iCurCombo = 0;

	m_fCurChargeRate = 0.0f;
	m_fCurChargeRateForSkill = 0.0f;

	m_fCurGauge = 0.0f;

	m_DarkSeasState = CFS_NONE;

	m_dwStartChargingTime = 0;
	m_dwModeCancleTime = 0;
}

ioDarkSeasItem::ioDarkSeasItem( const ioDarkSeasItem &rhs )
	: ioWeaponItem( rhs ),
	m_AttackReadyAniList( rhs.m_AttackReadyAniList ),
	m_AttackInfo( rhs.m_AttackInfo ),
	m_fMaxGauge( rhs.m_fMaxGauge ),
	m_fNeedGauge( rhs.m_fNeedGauge ),
	m_fIncreaseGauge( rhs.m_fIncreaseGauge ),
	m_fDecreaseGauge( rhs.m_fDecreaseGauge ),
	m_fHighDecreaseHeight( rhs.m_fHighDecreaseHeight ),
	m_fHighDecreaseGauge( rhs.m_fHighDecreaseGauge ),	
	m_fGravity( rhs.m_fGravity ),
	m_GatherAuraEffect( rhs.m_GatherAuraEffect ),
	m_fAuraEffectScaleRate( rhs.m_fAuraEffectScaleRate ),
	m_SpecialCheckBuff( rhs.m_SpecialCheckBuff ),
	m_fSpecialAttackMaxRange( rhs.m_fSpecialAttackMaxRange ),
	m_dwSpecialAttakTicTime( rhs.m_dwSpecialAttakTicTime ),
	m_fSpecial_a_attack_end_jump_power( rhs.m_fSpecial_a_attack_end_jump_power ),
	m_stSpecial_a_attack_buff( rhs.m_stSpecial_a_attack_buff ),
	m_stSpecial_a_attack_motion( rhs.m_stSpecial_a_attack_motion ),
	m_fSpecial_a_attack_motion_rate( rhs.m_fSpecial_a_attack_motion_rate ),
	m_vChargeTimeInfo( rhs.m_vChargeTimeInfo ),
	m_nSpecialAttackIdx( rhs. m_nSpecialAttackIdx ),
	m_nSpecialAttackResist( rhs.m_nSpecialAttackResist ),
	m_fDecA_Attack( rhs.m_fDecA_Attack ),
	m_fDecD_Attack( rhs.m_fDecD_Attack )
{
	m_ChargeState = CS_NONE;

	m_dwAttackStartTime = 0;
	m_iCurCombo = 0;

	m_fCurChargeRate = 0.0f;
	m_fCurChargeRateForSkill = 0.0f;

	m_fCurGauge = 0.0f;

	m_DarkSeasState = CFS_NONE;

	m_dwGatherAuraEffectID = 0;
}

ioDarkSeasItem::~ioDarkSeasItem()
{
} 

void ioDarkSeasItem::LoadProperty( ioINILoader &rkLoader )
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

	LoadInfo( rkLoader );

	m_fMaxGauge = rkLoader.LoadFloat_e( "max_gauge", FLOAT100 );
	m_fNeedGauge = rkLoader.LoadFloat_e( "need_gauge", FLOAT100 );

	m_fIncreaseGauge = rkLoader.LoadFloat_e( "increase_gauge", FLOAT1 );
	m_fDecreaseGauge = rkLoader.LoadFloat_e( "decrease_gauge", 2.0f );

	m_fHighDecreaseHeight = rkLoader.LoadFloat_e( "high_decrease_height", FLOAT100 );
	m_fHighDecreaseGauge = rkLoader.LoadFloat_e( "high_decrease_gauge", 3.0f );

	//m_fShotDecreaseGauge = rkLoader.LoadFloat_e( "shot_decrease_gauge", FLOAT1 );
	m_fAttackSpeedRate = rkLoader.LoadFloat_e( "attack_max_speed_rate", FLOAT1 );
}

void ioDarkSeasItem::LoadInfo( ioINILoader &rkLoader )
{
	char szBuf[MAX_PATH];
	char szKey[MAX_PATH];
	m_AttackInfo.Init();

	//////////////////////////////////////////////////////////////////////////
	rkLoader.LoadString_e( "special_start_ani", "", szBuf, MAX_PATH );
	m_AttackInfo.m_StartAni = szBuf;
	m_AttackInfo.m_fStartAniRate = rkLoader.LoadFloat_e( "special_start_ani_rate", FLOAT1 );

	rkLoader.LoadString_e( "special_end_ani", "", szBuf, MAX_PATH );
	m_AttackInfo.m_EndAni = szBuf;
	m_AttackInfo.m_fEndAniRate = rkLoader.LoadFloat_e( "special_end_ani_rate", FLOAT1 );

	rkLoader.LoadString_e( "special_delay_ani", "", szBuf, MAX_PATH );
	m_AttackInfo.m_DelayAni = szBuf;
	m_AttackInfo.m_fDelayAniRate = rkLoader.LoadFloat_e( "special_delay_ani_rate", FLOAT1 );

	rkLoader.LoadString_e( "special_move_ani", "", szBuf, MAX_PATH );
	m_AttackInfo.m_MoveAniFr = szBuf;
	m_AttackInfo.m_fMoveAniFrRate = rkLoader.LoadFloat_e( "special_move_ani_rate", FLOAT1 );
	//////////////////////////////////////////////////////////////////////////
	m_fGravity = rkLoader.LoadFloat_e( "mode_gravity", 0.0f );

	m_nSpecialAttackIdx = rkLoader.LoadInt_e( "special_attack_idx", 0 );
	m_nSpecialAttackResist = rkLoader.LoadInt_e( "special_attack_idx_resist", 0 );
	//////////////////////////////////////////////////////////////////////////
	rkLoader.LoadString_e( "aura_effect", "", szBuf, MAX_PATH );
	m_GatherAuraEffect = szBuf;
	m_fAuraEffectScaleRate		= rkLoader.LoadFloat_e( "aura_effect_scale", FLOAT1 );

	rkLoader.LoadString_e( "special_check_buff", "", szBuf, MAX_PATH );
	m_SpecialCheckBuff = szBuf;
	m_fSpecialAttackMaxRange = rkLoader.LoadFloat_e( "special_max_range", FLOAT1 );
	m_dwSpecialAttakTicTime = (DWORD)rkLoader.LoadInt_e( "special_attack_tic_time", 10000 );

	rkLoader.LoadString_e( "special_a_attack_motion", "", szBuf, MAX_PATH );
	m_stSpecial_a_attack_motion = szBuf;
	m_fSpecial_a_attack_motion_rate = rkLoader.LoadFloat_e( "special_a_attack_motion_rate", FLOAT1 );

	m_fSpecial_a_attack_end_jump_power = rkLoader.LoadFloat_e( "special_a_attack_end_jump_powe", 0.0f );

	rkLoader.LoadString_e( "special_a_attack_buff", "", szBuf, MAX_PATH );
	m_stSpecial_a_attack_buff = szBuf;

	m_fDecA_Attack = rkLoader.LoadFloat_e( "special_a_decgauge", FLOAT100 );
	m_fDecD_Attack = rkLoader.LoadFloat_e( "special_d_decgauge", FLOAT100 );

	int nCnt = rkLoader.LoadInt_e( "charge_time_attack_cnt", 0 );
	for( int i = 0; i < nCnt; i++ )
	{
		ChargeTimeAttackInfo info;
		wsprintf( szKey, "charge_time%d_attack", i+1 );
		info.m_dwTime = (DWORD)rkLoader.LoadInt( szKey, 10000 );

		wsprintf( szKey, "special_d_attack%d_buff", i+1 );
		rkLoader.LoadString( szKey, "", szBuf, MAX_PATH );
		info.m_stAttackBuff = szBuf;
		m_vChargeTimeInfo.push_back( info );
	}
	//m_vChargeTimeInfo
}

ioItem* ioDarkSeasItem::Clone()
{
	return new ioDarkSeasItem( *this );
}

ioWeaponItem::WeaponSubType ioDarkSeasItem::GetSubType() const
{
	return WST_DARKSEAS_ITEM;
}

void ioDarkSeasItem::SetNormalAttack( ioBaseChar *pOwner, int iCurCombo )
{
	if( !pOwner ) 
		return;

	m_fCurChargeRate = 0.0f;
	m_fCurChargeRateForSkill = 0.0f;

	m_dwKeyReserveTime = 0;

	m_iSkillInput = -1;
	m_bEnableDefenseState = false;
	m_bEnableAttackState = false;
	m_bEnableJumpState = false;

	m_fCurAngle = 0;

	ClearDarkSeasState( pOwner );

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
		m_dwAttackStartTime = FRAMEGETTIME();
		m_iCurCombo = iCurCombo;
		pOwner->SetState( CS_ATTACK );
		pOwner->SetAttackMoveEnable( false );
		pOwner->SetChargingState( true );

		/*if( m_bChargeAutoTarget )
		{
			DWORD dwTrackingTime = FRAMEGETTIME();
			dwTrackingTime += pGrp->GetAnimationEventTime_e( iAniID, "tracking_e" ) * fTimeRate;
			pOwner->SetAutoTargetTrackingEndTime( dwTrackingTime );
		}
		else*/
		{
			pOwner->StopAutoTargetTracking();
		}
	}
}

void ioDarkSeasItem::CheckNormalAttack( ioBaseChar *pOwner )
{
	switch( m_ChargeState )
	{
	case CS_CHARGING:
		OnCharging( pOwner );
		break;
	case CS_NORMAL_ATTACK:
		ioWeaponItem::CheckNormalAttack( pOwner );
		break;
	}
}

void ioDarkSeasItem::OnCharging( ioBaseChar *pOwner )
{
	if( !pOwner ) return;
	if( !pOwner->IsNeedProcess() )	return;

	if( IsEnableGauge() && pOwner->IsAttackKeyDown() && !pOwner->IsHasCrown() )
	{
		if( m_dwAttackStartTime + m_dwChargeTime <= FRAMEGETTIME() )
		{
			SetDarkSeasState( pOwner, true );
			return;
		}
	}
	else
	{
		ChangeToNormalAttack( pOwner );
	}
}

void ioDarkSeasItem::ChangeToNormalAttack( ioBaseChar *pOwner )
{
	DWORD dwCurTime = FRAMEGETTIME();

	//
	pOwner->SetAttackMoveEnable( false );
	pOwner->SetChargingState( false );

	m_fCurChargeRate = 0.0f;
	m_fCurChargeRateForSkill = 0.0f;

	const AttackAttribute *pAttribute = m_FormalAttack.GetAttribute( AT_NORMAL, m_iCurCombo );
	if( pAttribute )
	{
		if( !m_bChargeAutoTarget )
		{
			pOwner->SetAutoTarget( ATT_NORMAL );
		}

		pOwner->SetNormalAttackByAttribute( *pAttribute );
	}

	pOwner->ApplyNormalAttackResult( m_iCurCombo );

	m_ChargeState = CS_NORMAL_ATTACK;

	if( pOwner->IsNeedSendNetwork() )
	{
		SP2Packet kPacket( CUPK_ITEM_GAUGE_OK );
		FillItemGaugePacketHeader( pOwner, kPacket );
		kPacket << m_ChargeState;
		kPacket << m_iCurCombo;
		if( !m_bChargeAutoTarget )
			pOwner->FillAutoTargetInfo( kPacket );
		P2PNetwork::SendToAllPlayingUser( kPacket );
	}
}

void ioDarkSeasItem::OnItemGaugeOK( ioBaseChar *pOwner, SP2Packet &rkPacket )
{
	int iState;
	rkPacket >> iState;
	rkPacket >> m_iCurCombo;

	switch( iState )
	{
	case CS_NONE:
		pOwner->SetAttackMoveEnable( false );
		pOwner->SetChargingState( false );

		m_ChargeState = CS_NONE;
		break;
	case CS_NORMAL_ATTACK:
		if( !m_bChargeAutoTarget )
			pOwner->ApplyAutoTargetInfo( rkPacket );
		ChangeToNormalAttack( pOwner );
		break;
	}
}

void ioDarkSeasItem::OnItemGaugeCancel( ioBaseChar *pOwner, SP2Packet &rkPacket )
{
	D3DXQUATERNION qtRot;
	D3DXVECTOR3 vPos;
	int nIndex = 0;

	int iSyncType;
	rkPacket >> iSyncType;

	switch( iSyncType )
	{
	case SST_EFFECT:
		{
			float fGauge;
			rkPacket >> fGauge;
			SetCurGauge( fGauge );
		}
		break;
	case SST_CHANGE_STATE:
		CheckDarkSeasState( pOwner, rkPacket );
		break;
	case SST_AUTO_FIRE:
		{
			ioHashString stTargetName;

			rkPacket >> qtRot;
			rkPacket >> vPos;
			rkPacket >> stTargetName;

			ioBaseChar* pTarget = pOwner->GetBaseChar( stTargetName );

			pOwner->SetWorldPosition( vPos );
			pOwner->SetTargetRotToRotate( qtRot, true );

			CreateFire( pOwner, pTarget );
		}
		break;
	case SST_A_FIRE:
		rkPacket >> qtRot;
		rkPacket >> vPos;

		pOwner->SetTargetRotToRotate( qtRot, true );
		pOwner->SetWorldPosition( vPos );

		Change_A_EndState( pOwner );
		break;
	case SST_END_FIRE:
		rkPacket >> qtRot;
		rkPacket >> vPos;
		rkPacket >> nIndex;

		pOwner->SetTargetRotToRotate( qtRot, true );
		pOwner->SetWorldPosition( vPos );

		ChangeEndState( pOwner, nIndex );
		break;
	default: break;
	}
}

void ioDarkSeasItem::SetExtraInfoToWeapon( ioWeapon *pWeapon, bool bJumpAttack )
{
	ioAttackableItem::SetExtraInfoToWeapon( pWeapon, bJumpAttack );
}

bool ioDarkSeasItem::IsChargeAttackState( ioBaseChar *pOwner )
{
	if( pOwner->GetState() == CS_ATTACK )
	{
		switch( m_ChargeState )
		{
		case CS_CHARGING:
			return true;
		}
	}

	return false;
}

void ioDarkSeasItem::OnEndExtendAttack( ioBaseChar *pOwner, bool bReleaseItem )
{
	m_ChargeState = CS_NONE;

	pOwner->SetAttackMoveEnable( false );
	pOwner->SetChargingState( false );

	pOwner->ClearDirDoubleClick();

	m_fCurChargeRate = 0.0f;
}


void ioDarkSeasItem::SetDarkSeasState( ioBaseChar *pOwner, bool bSendNet )
{
	if( !pOwner )
		return;

	ioEntityGroup *pGrp = pOwner->GetGroup();
	if( !pGrp )
		return;

	if( pOwner->IsNeedProcess() )
	{
		D3DXQUATERNION qtRot = pOwner->GetTargetRot();
		pOwner->SetTargetRotToRotate( qtRot, true );
	}

	pOwner->SetState( CS_DARKSEAS_STATE );

	ChangeStartState( pOwner, bSendNet );
	//////////////////////////////////////////////////////////////////////////
	pOwner->SetAttackMoveEnable( true );
	pOwner->ChangeDirectionExplicitIfDirChanged( true );
	//////////////////////////////////////////////////////////////////////////

	if( bSendNet && pOwner->IsNeedSendNetwork() )
	{
		SP2Packet kPacket( CUPK_ITEM_GAUGE_CANCEL );
		FillItemGaugePacketHeader( pOwner, kPacket );
		kPacket << SST_CHANGE_STATE;
		kPacket << m_DarkSeasState;
		kPacket << pOwner->GetTargetRot();
		kPacket << pOwner->GetWorldPosition();
		kPacket << m_fCurGauge;
		P2PNetwork::SendToAllPlayingUser( kPacket );
	}
}

void ioDarkSeasItem::CheckKeyInput( ioBaseChar *pOwner )
{
	bool bChangeDir = false;
	bool bChangeUpDown = false;

	// Check Dir
	ioUserKeyInput::DirKeyInput eNewDirKey = pOwner->CheckCurDirKey();
	if( eNewDirKey != m_CurDirKey )
	{
		//if( eNewDirKey != ioUserKeyInput::DKI_NONE )
		bChangeDir = true;
		m_CurDirKey = eNewDirKey;
	}

	if( bChangeDir )
		ChangeMoveAni( pOwner );
}



void ioDarkSeasItem::ChangeMoveAni( ioBaseChar *pOwner )
{
	if( !pOwner )
		return;

	ioEntityGroup *pGrp = pOwner->GetGroup();
	if( !pGrp )
		return;

	//None은 에외 처리
	if( m_CurDirKey == ioUserKeyInput::DKI_NONE )
	{
		pGrp->ClearAllLoopAni( FLOAT100, true );
		pGrp->ClearAllActionAni( FLOAT100, true );

		int iAniID	     = pGrp->GetAnimationIdx( m_AttackInfo.m_DelayAni );
		float fTimeRate  = m_AttackInfo.m_fDelayAniRate;

		pGrp->SetLoopAni( iAniID, FLOAT100, FLOAT1, FLOAT1/fTimeRate );
		return;
	}


	ChangeMoveAni( pOwner, m_AttackInfo.m_MoveAniFr, m_AttackInfo.m_fMoveAniFrRate );
}

void ioDarkSeasItem::ProcessDarkSeasState( ioBaseChar *pOwner )
{
	if( !pOwner )
		return;

	pOwner->SetCurJumpPowerAmt( 0.0f );
	pOwner->SetGravityAmt( 0.0f );
	pOwner->SetForcePowerAmt( 0.0f );

	ioPlayStage* pStage = pOwner->GetCreator();

	if( m_dwGatherAuraEffectID && pStage )
	{
		ioMapEffect* pMapEffect = pStage->FindMapEffect( m_dwGatherAuraEffectID );
		if( pMapEffect )
		{
			D3DXVECTOR3 vPos( pOwner->GetWorldPosition().x, pOwner->GetMidHeightByRate(), pOwner->GetWorldPosition().z );
			pMapEffect->SetWorldPosition( vPos );

			ioEffect* pEffect = pMapEffect->GetEffect();
			if( pEffect )
				pEffect->GetParentNode()->SetScale( m_fAuraEffectScaleRate, m_fAuraEffectScaleRate, m_fAuraEffectScaleRate );
		}
	}

	DWORD dwCurTime = FRAMEGETTIME();

	switch( m_DarkSeasState )
	{
	case CFS_START:
		if( m_dwMotionEndTime < dwCurTime )
			ChangeDelayState(pOwner, false);

		break;
	case CFS_DELAY:
		if( pOwner->IsNeedProcess() && m_fCurGauge <= 0.0f )
		{
			ChangeEndState( pOwner );
			return;
		}

		if( pOwner->IsNeedProcess() )
		{
			//디꾹때기
			if( !CheckChargingMode( pOwner ) )
			{
				m_fCurGauge = max( 0.0f, m_fCurGauge - m_fDecD_Attack );
				return;
			}

			if( pOwner->IsJumpKeyDown() )
			{
				m_fCurGauge = max( 0.0f, m_fCurGauge - m_fDecA_Attack );
				Change_A_EndState( pOwner );
				return;
			}
		}
			

		CheckKeyInput( pOwner );

		if( pOwner->IsNeedProcess() && m_dwSpecialAttackNextTime < FRAMEGETTIME() )
		{
			//적을 찾지 못했다면 다음 프레임에 다시 찾는다
			if( CheckRangeAttack( pOwner ) )
				m_dwSpecialAttackNextTime = FRAMEGETTIME() + m_dwSpecialAttakTicTime;
			else
				m_dwSpecialAttackNextTime = FRAMEGETTIME();
		}

		break;
	case CFS_END:
		if( m_dwMotionEndTime < dwCurTime )
		{
			pOwner->SetState( CS_DELAY, false );
			ClearDarkSeasState(pOwner);
			return;
		}
		break;
	case CFS_A_END:
		if( m_dwMotionEndTime < dwCurTime )
		{
			ClearDarkSeasState(pOwner);

			pOwner->SetCurMoveSpeed( 0.0f );
			pOwner->SetSKillEndJumpState( m_fSpecial_a_attack_end_jump_power, false, false, false, true );
			return;
		}
		break;
	}
}


bool ioDarkSeasItem::CheckRangeAttack( ioBaseChar *pOwner )
{
	ioPlayStage* pStage = pOwner->GetCreator();
	if( !pStage )
		return false;

	const BaseCharList &rkCharList = pStage->GetBaseCharList();
	BaseCharList::const_iterator iter;

	ioHashString szTarget;
	float fCurGap = -FLOAT1;

	float fTargetRangeSq = m_fSpecialAttackMaxRange;// * m_fSpecialAttackMaxRange;

	D3DXVECTOR3 vCurPos = pOwner->GetMidPositionByRate();
	D3DXVECTOR3 vDiff= ioMath::VEC3_ZERO;

	for( iter=rkCharList.begin() ; iter!=rkCharList.end() ; ++iter )
	{
		ioBaseChar *pTarget = *iter;
		if( !pTarget ) continue;

		if( pTarget->GetTeam() == pOwner->GetTeam() ) 
			continue;
		if( pTarget->GetCharName() == GetOwnerName() ) 
			continue;
		if( pTarget->GetExperienceMode() != EMS_NONE ) 
			continue;
		if( pTarget->HasBuff( m_SpecialCheckBuff ) )
			continue;

		if( !IsEnableTargetState( pTarget ) )
			continue;

		D3DXVECTOR3 vTargetPos = pTarget->GetMidPositionByRate();
		vDiff = vTargetPos - vCurPos;
		//float fDiffRangeSq = D3DXVec3LengthSq( &vDiff );
		float fDiffRangeSq = D3DXVec3Length( &vDiff );

		// 거리 체크
		if( fDiffRangeSq < fTargetRangeSq )
		{
			if( fCurGap == -FLOAT1 )
			{
				fCurGap = fDiffRangeSq;
				szTarget = pTarget->GetCharName();
			}
			else if( fCurGap >= 0.0f && fDiffRangeSq < fCurGap )
			{
				fCurGap = fDiffRangeSq;
				szTarget = pTarget->GetCharName();
			}
		}
	}

	if( !szTarget.IsEmpty() )
	{
		ioBaseChar *pTarget = pStage->GetBaseChar( szTarget );
		if( pTarget )
		{
			CreateFire( pOwner, pTarget );
			return true;
		}
	}

	return false;
}


void ioDarkSeasItem::CreateFire( ioBaseChar* pOwner, ioBaseChar* pTarget )
{
	if( pTarget == NULL )
		return;

	DWORD dwCurTime = FRAMEGETTIME();
	pOwner->InitExtendAttackTagInfo();

	FireTime kFireTime;
	kFireTime.dwStart = dwCurTime;

	kFireTime.iAttributeIdx = m_nSpecialAttackIdx;
	kFireTime.iResistanceIdx = m_nSpecialAttackResist;
	kFireTime.szWoundedAni  = "";
	kFireTime.dwWoundedAniDuration = 0;
	kFireTime.bLoopWoundedAni = false;

	pOwner->IncreaseWeaponIndexBase();
	kFireTime.dwWeaponIdx = pOwner->GetWeaponIndexBase();
	kFireTime.eFireTimeType = FTT_NORMAL_ATTACK;

	D3DXVECTOR3 vPos = pOwner->GetMidPositionByRate();
	D3DXVECTOR3 vDir = pTarget->GetWorldPosition() - pOwner->GetWorldPosition();
	D3DXVec3Normalize( &vDir, &vDir );
	pOwner->ExtendFireExplicit( kFireTime, vPos, vDir, GetName() );

	if( pOwner->IsNeedProcess() )
	{
		SP2Packet kPacket( CUPK_ITEM_GAUGE_CANCEL );
		FillItemGaugePacketHeader( pOwner, kPacket );
		kPacket << SST_AUTO_FIRE;
		kPacket << pOwner->GetTargetRot();
		kPacket << pOwner->GetWorldPosition();
		kPacket << pTarget->GetCharName();
		P2PNetwork::SendToAllPlayingUser( kPacket );
	}
}


bool ioDarkSeasItem::IsEnableTargetState( ioBaseChar *pTarget )
{
	if( !pTarget )
		return false;

	if( pTarget->IsSystemState() )
		return false;

	if( !pTarget->IsEnableTarget() )
		return false;

	// 쓰러짐 관련 체크
	if( !pTarget->CheckTargetWoundType( TWT_DOWN_WOUND_EXCEPT ) )
		return false;

	return true;
}

void ioDarkSeasItem::CheckDarkSeasState( ioBaseChar *pOwner, SP2Packet &rkPacket )
{
	if( !pOwner )
		return;

	int iState;
	rkPacket >> iState;

	D3DXQUATERNION qtRot;
	rkPacket >> qtRot;

	switch( iState )
	{
	case CFS_START:
		{
			D3DXVECTOR3 vPos;
			rkPacket >> vPos;
			rkPacket >> m_fCurGauge;
			pOwner->SetWorldPosition( vPos );
			pOwner->SetTargetRotToRotate( qtRot, true );
			SetDarkSeasState( pOwner, false );
		}
		break;
	case CFS_DELAY:
		ChangeDelayState(pOwner, false);
		break;
	case CFS_END:
		ChangeEndState(pOwner);
		break;
	default: break;
	}
}

int ioDarkSeasItem::GetMaxBullet()
{
	return (int)m_fMaxGauge;
}

int ioDarkSeasItem::GetNeedBullet()
{
	return (int)m_fNeedGauge;
}

int ioDarkSeasItem::GetCurBullet()
{
	float fCurCoolTime = m_fCurGauge;
	fCurCoolTime = max( 0, min( fCurCoolTime, m_fMaxGauge ) );

	return (int)fCurCoolTime;
}

void ioDarkSeasItem::SetCurBullet( int iCurBullet )
{
	if( iCurBullet < 0 )
		return;

	float fCurBullet = iCurBullet;
	fCurBullet = max( 0, min( fCurBullet, m_fMaxGauge ) );

	m_fCurGauge = fCurBullet;
}

void ioDarkSeasItem::InitGauge()
{
	m_fCurGauge = 0.0f;
}

void ioDarkSeasItem::MaxGauge()
{
	m_fCurGauge = m_fMaxGauge;
}

float ioDarkSeasItem::GetCurGauge()
{
	return m_fCurGauge;
}

void ioDarkSeasItem::SetCurGauge( float fGauge )
{
	m_fCurGauge = fGauge;
	m_fCurGauge = (float)max( 0, min( m_fCurGauge, m_fMaxGauge ) );
}

void ioDarkSeasItem::CheckEnableGauge( ioBaseChar *pChar )
{
	/*if( m_fCurCyvogueGauge >= m_fNeedCyvogueGauge )
		SetEnableCyvogueEffect( pChar );*/
}

bool ioDarkSeasItem::IsEnableGauge()
{
	if( m_fCurGauge >= m_fNeedGauge )
		return true;

	return false;
}

bool ioDarkSeasItem::IsDarkSeasState()
{
	switch( m_DarkSeasState )
	{
	case CFS_DELAY:
		return true;
	}

	return false;
}

void ioDarkSeasItem::UpdateExtraData( ioBaseChar *pOwner )
{
	if( !pOwner )
		return;

	// check owner state
	float fTimePerSec = g_FrameTimer.GetSecPerFrame();

	if( pOwner->IsSystemState() )
		return;

	switch( pOwner->GetState() )
	{
	case CS_DARKSEAS_STATE:
		{
			if(m_DarkSeasState == CFS_START || m_DarkSeasState == CFS_END)
				return;

			float fGauge = m_fDecreaseGauge * fTimePerSec;

			ioPlayStage *pStage = pOwner->GetCreator();
			if( pStage )
			{
				D3DXVECTOR3 vCurPos = pOwner->GetWorldPosition();
				float fMapHeight = pStage->GetMapHeight( vCurPos.x, vCurPos.z, pOwner, false, pOwner->GetMidHeightByRate(1.0f) );
				float fGapHeight = fabs( vCurPos.y - fMapHeight );

				if( m_fHighDecreaseHeight < fGapHeight )
					fGauge = m_fHighDecreaseGauge * fTimePerSec;
			}

			m_fCurGauge -= fGauge;
			m_fCurGauge = max( 0.0f, m_fCurGauge );
		}
		break;
	default:
		if( m_fCurGauge < m_fMaxGauge )
		{
			float fGauge = m_fIncreaseGauge * fTimePerSec;

			m_fCurGauge += fGauge;
			m_fCurGauge = min( m_fCurGauge, m_fMaxGauge );
		}
		break;
	}
}

void ioDarkSeasItem::ClearDarkSeasState( ioBaseChar *pOwner )
{
	m_DarkSeasState = CFS_NONE;

	// 순서중요
	/*if( pOwner && !m_CurCyvogueInfo.m_CyvogueFlyEffect.IsEmpty() )
	{
		pOwner->EndEffect( m_CurCyvogueInfo.m_CyvogueFlyEffect, false );
	}*/

	ReleaseEffect( pOwner );

	m_dwGatherAuraEffectID = 0;

	if( pOwner )
	{
		pOwner->SetMoveDirByRotate(pOwner->GetTargetRot());
	}
}

void ioDarkSeasItem::ChangeMoveAni( ioBaseChar *pOwner, const ioHashString &szAni, float fAniRate )
{
	if( !pOwner )
		return;

	ioEntityGroup *pGrp = pOwner->GetGroup();
	if( !pGrp )
		return;

	DWORD dwCurTime = FRAMEGETTIME();

	pOwner->ClearReservedSliding();
	pOwner->SetForcePowerAmt( 0.0f );

	m_dwKeyReserveTime = 0;
	float fTimeRate = fAniRate;
	if( fTimeRate <= 0.0f )
		fTimeRate = FLOAT1;

	int iAniID = pGrp->GetAnimationIdx( szAni );
	pGrp->ClearAllActionAni( 0.0f, true );
	pGrp->ClearAllLoopAni( 0.0f, true );
	pGrp->SetLoopAni( iAniID, FLOAT100, FLOAT1, FLOAT1/fAniRate );
	//pGrp->SetActionAni( iAniID, FLOAT100, FLOAT100, FLOAT1, FLOAT1/fTimeRate );
}

void ioDarkSeasItem::OnEquiped( ioBaseChar *pOwner )
{
	ioWeaponItem::OnEquiped( pOwner );
}

void ioDarkSeasItem::OnReleased( ioBaseChar *pOwner )
{
//	ReleaseEnableCyvogueEffect( pOwner );
//	ReleaseCyvogueFlyMoveEffect( pOwner );

	ClearDarkSeasState( pOwner );
	m_fCurGauge = 0.0f;

	pOwner->SetTargetMarker( ioTargetMarker::MT_NONE, "", "", "", "", "" );

	ioAttackableItem::OnReleased( pOwner );
}

void ioDarkSeasItem::ChangeStartState( ioBaseChar *pOwner, bool bSendNet )
{
	if( !pOwner )
		return;

	ioEntityGroup *pGrp = pOwner->GetGroup();
	if( !pGrp )
		return;

	DWORD dwCurTime = FRAMEGETTIME();

	pOwner->InitExtendAttackTagInfo();

	pOwner->ClearReservedSliding();
	pOwner->SetForcePowerAmt( 0.0f );

	pGrp->ClearAllLoopAni( FLOAT100, true );
	pGrp->ClearAllActionAni( FLOAT100, true );

	int iAniID	     = pGrp->GetAnimationIdx( m_AttackInfo.m_StartAni );
	float fTimeRate  = m_AttackInfo.m_fStartAniRate;

	pGrp->SetActionAni( iAniID, FLOAT100, FLOAT100, FLOAT1, FLOAT1/fTimeRate, 0.0f, 0.0f, true );

	m_DarkSeasState = CFS_START;

	m_dwModeChargeStartTime = 0;
	m_CurDirKey = ioUserKeyInput::DKI_NONE;
	m_dwMotionEndTime = 0;

	m_dwMotionEndTime = dwCurTime + pGrp->GetAnimationFullTime( iAniID ) * fTimeRate;

	CreateEffect(pOwner);

	m_dwSpecialAttackNextTime = FRAMEGETTIME();

	m_dwChargeStartTime = FRAMEGETTIME();
}

void ioDarkSeasItem::ChangeDelayState( ioBaseChar *pOwner, bool bSendNet  )
{
	if( !pOwner )
		return;

	ioEntityGroup *pGrp = pOwner->GetGroup();
	if( !pGrp )
		return;

	if( pOwner->IsNeedProcess() )
	{
		pOwner->ChangeDirectionByInputDir( true );
	}

	pOwner->InitExtendAttackTagInfo();

	pOwner->ClearReservedSliding();
	pOwner->SetForcePowerAmt( 0.0f );

	pGrp->ClearAllLoopAni( FLOAT100, true );
	pGrp->ClearAllActionAni( FLOAT100, true );

	int iAniID	     = pGrp->GetAnimationIdx( m_AttackInfo.m_DelayAni );
	float fTimeRate  = m_AttackInfo.m_fDelayAniRate;

	pGrp->SetLoopAni( iAniID, FLOAT100, FLOAT1, FLOAT1/fTimeRate );

	m_DarkSeasState = CFS_DELAY;
	//////////////////////////////////////////////////////////////////////////
	m_dwModeChargeStartTime = 0;
	m_CurDirKey = ioUserKeyInput::DKI_NONE;
	m_dwMotionEndTime = 0;

	if( bSendNet && pOwner->IsNeedSendNetwork() )
	{
		SP2Packet kPacket( CUPK_ITEM_GAUGE_CANCEL );
		FillItemGaugePacketHeader( pOwner, kPacket );
		kPacket << SST_CHANGE_STATE;
		kPacket << m_DarkSeasState;
		kPacket << pOwner->GetTargetRot();
		kPacket << m_fCurGauge;
		P2PNetwork::SendToAllPlayingUser( kPacket );
	}
}

void ioDarkSeasItem::ChangeEndState( ioBaseChar *pOwner, int nChargeIndex )
{
	if( !pOwner )
		return;

	ioPlayStage* pStage = pOwner->GetCreator();
	if( !pStage )
		return;

	ioEntityGroup *pGrp = pOwner->GetGroup();
	if( !pGrp )
		return;

	DWORD dwCurTime = FRAMEGETTIME();

	pOwner->InitExtendAttackTagInfo();

	pOwner->ClearReservedSliding();
	pOwner->SetForcePowerAmt( 0.0f );

	pGrp->ClearAllLoopAni( FLOAT100, true );
	pGrp->ClearAllActionAni( FLOAT100, true );

	int iAniID	     = pGrp->GetAnimationIdx( m_AttackInfo.m_EndAni );
	float fTimeRate  = m_AttackInfo.m_fEndAniRate;

	pGrp->SetActionAni( iAniID, FLOAT100, FLOAT100, FLOAT1, FLOAT1/fTimeRate, 0.0f, 0.0f, true );

	m_DarkSeasState = CFS_END;

	m_dwModeChargeStartTime = 0;
	m_CurDirKey = ioUserKeyInput::DKI_NONE;
	m_dwMotionEndTime = 0;

	m_dwMotionEndTime = dwCurTime + pGrp->GetAnimationFullTime( iAniID ) * fTimeRate;

	int nIndex = m_vChargeTimeInfo.size() - 1;
	DWORD dwChargeTime = FRAMEGETTIME() - m_dwChargeStartTime;

	if( pOwner->IsNeedProcess() )
	{
		for( int i = 0; i < (int)m_vChargeTimeInfo.size(); i++ )
		{
			if( m_vChargeTimeInfo[i].m_dwTime < dwChargeTime )
			{
				nIndex = i;
				break;
			}
		}
	}

	//주변을 검색해서 공격
	const BaseCharList &rkCharList = pStage->GetBaseCharList();
	BaseCharList::const_iterator iter;

	for( iter=rkCharList.begin() ; iter!=rkCharList.end() ; ++iter )
	{
		ioBaseChar *pTarget = *iter;
		if( !pTarget ) 
			continue;

		if( pTarget->HasBuff( pOwner->GetCharName(), m_SpecialCheckBuff ) )
		{
			ioFlyFollowStateBuff* pFlyFollowBuff = ToFlyFollowBuff( pTarget->GetBuff(m_SpecialCheckBuff, pOwner->GetCharName() ) );
			if( pFlyFollowBuff )
				pFlyFollowBuff->SetEndJump( false );

			if( pOwner->IsNeedProcess() )
				pTarget->AddNewBuff( m_vChargeTimeInfo[nIndex].m_stAttackBuff, pOwner->GetCharName(), "", NULL );
			else
				pTarget->AddNewBuff( m_vChargeTimeInfo[nChargeIndex].m_stAttackBuff, pOwner->GetCharName(), "", NULL );
		}
	}

	if( pOwner->IsNeedSendNetwork() )
	{
		SP2Packet kPacket( CUPK_ITEM_GAUGE_CANCEL );
		FillItemGaugePacketHeader( pOwner, kPacket );
		kPacket << SST_END_FIRE;
		kPacket << pOwner->GetTargetRot();
		kPacket << pOwner->GetWorldPosition();
		kPacket << nIndex;
		P2PNetwork::SendToAllPlayingUser( kPacket );
	}

	//pOwner->SetState( CS_DELAY );
	//ClearCyvogueState(pOwner);
}


void ioDarkSeasItem::Change_A_EndState( ioBaseChar *pOwner )
{
	if( !pOwner )
		return;

	ioPlayStage* pStage = pOwner->GetCreator();
	if( !pStage )
		return;

	ioEntityGroup *pGrp = pOwner->GetGroup();
	if( !pGrp )
		return;

	DWORD dwCurTime = FRAMEGETTIME();

	pOwner->InitExtendAttackTagInfo();

	pOwner->ClearReservedSliding();
	pOwner->SetForcePowerAmt( 0.0f );

	pGrp->ClearAllLoopAni( FLOAT100, true );
	pGrp->ClearAllActionAni( FLOAT100, true );

	int iAniID	     = pGrp->GetAnimationIdx( m_stSpecial_a_attack_motion );
	float fTimeRate  = m_fSpecial_a_attack_motion_rate;

	pGrp->SetActionAni( iAniID, FLOAT100, FLOAT100, FLOAT1, FLOAT1/fTimeRate, 0.0f, 0.0f, true );

	m_DarkSeasState = CFS_A_END;

	m_dwModeChargeStartTime = 0;
	m_CurDirKey = ioUserKeyInput::DKI_NONE;
	m_dwMotionEndTime = 0;

	m_dwMotionEndTime = dwCurTime + pGrp->GetAnimationFullTime( iAniID ) * fTimeRate;

	//주변을 검색해서 공격
	const BaseCharList &rkCharList = pStage->GetBaseCharList();
	BaseCharList::const_iterator iter;

	for( iter=rkCharList.begin() ; iter!=rkCharList.end() ; ++iter )
	{
		ioBaseChar *pTarget = *iter;
		if( !pTarget ) 
			continue;

		if( pTarget->HasBuff( pOwner->GetCharName(), m_SpecialCheckBuff ) )
		{
			ioFlyFollowStateBuff* pFlyFollowBuff = ToFlyFollowBuff( pTarget->GetBuff(m_SpecialCheckBuff, pOwner->GetCharName() ) );
			if( pFlyFollowBuff )
				pFlyFollowBuff->SetEndJump( false );

			pTarget->AddNewBuff( m_stSpecial_a_attack_buff, pOwner->GetCharName(), "", NULL );
		}
	}


	if( pOwner->IsNeedSendNetwork() )
	{
		SP2Packet kPacket( CUPK_ITEM_GAUGE_CANCEL );
		FillItemGaugePacketHeader( pOwner, kPacket );
		kPacket << SST_A_FIRE;
		kPacket << pOwner->GetTargetRot();
		kPacket << pOwner->GetWorldPosition();
		P2PNetwork::SendToAllPlayingUser( kPacket );
	}
}


bool ioDarkSeasItem::IsBallTarget()
{
	return m_bAimedBall;
}

void ioDarkSeasItem::ReleaseBipe( ioBaseChar *pOwner )
{
	ioEntityGroup *pGrp = pOwner->GetGroup();
	ioBiped *pBiped = pGrp->GetBiped("Bip01 Spine1");
	if(!pBiped)	
		return;

	pBiped->ReleaseExtraRotation();
}


bool ioDarkSeasItem::CheckChargingMode( ioBaseChar *pOwner )
{
	DWORD dwCurTime = FRAMEGETTIME();

	if( pOwner->IsNeedProcess() && !pOwner->IsAttackKeyDown() )
	{
		ChangeEndState(pOwner);
		return false;
	}

	return true;
}

bool ioDarkSeasItem::IsEnableMoveState()
{
	if( m_DarkSeasState == CFS_DELAY )
		return true;


	return false;
}

float ioDarkSeasItem::GetAttackSpeedRate() const
{
	if( m_DarkSeasState != CFS_NONE )
		return m_fAttackSpeedRate;

	return FLOAT1;
}

float ioDarkSeasItem::GetGravity()
{
	return m_fGravity;
}

void ioDarkSeasItem::ReleaseEffect( ioBaseChar *pOwner )
{
	if( m_dwGatherAuraEffectID )
	{
		pOwner->EndMapEffect( m_dwGatherAuraEffectID, false );
	}
}

void ioDarkSeasItem::CreateEffect( ioBaseChar *pOwner )
{
	m_dwGatherAuraEffectID = 0;
	ioPlayStage* pStage = pOwner->GetCreator();
	if( pStage )
	{
		D3DXVECTOR3 vPos( pOwner->GetWorldPosition().x, pOwner->GetMidHeightByRate(), pOwner->GetWorldPosition().z );
		ioMapEffect* pMapEffect = pStage->CreateMapEffect( m_GatherAuraEffect, vPos, D3DXVECTOR3( m_fAuraEffectScaleRate, m_fAuraEffectScaleRate, m_fAuraEffectScaleRate ) );
		if( pMapEffect )
		{
			m_dwGatherAuraEffectID = pMapEffect->GetUniqueID();
		}
	}
}

//-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

ioDarkSeasSpecialState::ioDarkSeasSpecialState()
{
}

ioDarkSeasSpecialState::~ioDarkSeasSpecialState()
{
}

void ioDarkSeasSpecialState::Initialize( ioBaseChar* pOwner, CharState eCurrState )
{
	ioSpecialStateBase::Initialize( pOwner, eCurrState );
}

void ioDarkSeasSpecialState::Finalize( ioBaseChar* pOwner, CharState eCurrState, CharState eNewState )
{
	ioSpecialStateBase::Finalize( pOwner, eCurrState, eNewState );

	ioDarkSeasItem *pDarkSeasItem = ToDarkSeasItem( pOwner->GetPriorityItem( SPT_ATTACK ) );
	if( pDarkSeasItem )
		pDarkSeasItem->ClearDarkSeasState( pOwner );
}

void ioDarkSeasSpecialState::CheckSpecialState( ioBaseChar* pOwner )
{
	ioDarkSeasItem *pDarkSeasItem = ToDarkSeasItem( pOwner->GetPriorityItem( SPT_ATTACK ) );
	if( pDarkSeasItem )
		pDarkSeasItem->ProcessDarkSeasState( pOwner );
	else
	{
		pOwner->SetState( CS_DELAY );
		return;
	}
}

void ioDarkSeasSpecialState::CheckFallStateChange( ioBaseChar* pOwner, float fMinFallHeightRate, float fBottomHeight, bool bBottomMaxCheck, float fMaxBottomHeight )
{
	return;
}

void ioDarkSeasSpecialState::ProcessKeyInput( ioBaseChar* pOwner )
{
	pOwner->ChangeDirectionByInputDir( false );
}

void ioDarkSeasSpecialState::GetGravity( ioBaseChar* pOwner, float &fCurGravity )
{
	ioDarkSeasItem *pDarkSeasItem = ToDarkSeasItem( pOwner->GetPriorityItem( SPT_ATTACK ) );
	if( pDarkSeasItem )
		fCurGravity = pDarkSeasItem->GetGravity();
}