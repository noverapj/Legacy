

#include "stdafx.h"

#include "ioDashAttack2.h"

#include "ioBaseChar.h"
#include "ioPlayStage.h"


#include "WeaponDefine.h"
#include "ItemDefine.h"

ioDashAttack2::ioDashAttack2()
{
	m_dwActionEndTime = 0;
	m_dwPreReserveEnableTime = 0;
	m_dwRunRotateStartTime = 0;

	m_bSetCombo = false;
	m_iCurComboCnt = 0;
	m_dwChargingStartTime = 0;
	m_DashState = DS_NONE;

	m_dwCancelCheckTime = 0;
	m_dwInputCancelCheckTime = 0;

	m_fCurChargeRate = 0.0f;
	m_GatheringEffectID = -1;

	m_stAniJump.Clear();
	m_fAniJumpRate = FLOAT1;
	m_iCurAniRotate = 0;
	m_dwAniRotateTime = 0;
	m_fAniRotateAngle = 0;
}

ioDashAttack2::ioDashAttack2( const ioDashAttack2 &rhs )
: ioExtendDash( rhs ),
m_dwChargingTime( rhs.m_dwChargingTime ),
m_DashExtendType( rhs.m_DashExtendType ),
m_BuffAddAttribute( rhs.m_BuffAddAttribute ),
m_vBuffAddList( rhs.m_vBuffAddList ),
m_ExtendAttackList( rhs.m_ExtendAttackList ),
m_ChargingAniList( rhs.m_ChargingAniList ),
m_fRunSpeed( rhs.m_fRunSpeed ),
m_dwRunDuration( rhs.m_dwRunDuration ),
m_dwRotateSpeed( rhs.m_dwRotateSpeed ),
m_RunRotateAttack( rhs.m_RunRotateAttack ),
m_RunRotateAttackEnd( rhs.m_RunRotateAttackEnd ),
m_dwRunRotateJumpTime( rhs.m_dwRunRotateJumpTime ),
m_PowerChargeAni( rhs.m_PowerChargeAni ),
m_dwMaxChargeTime( rhs.m_dwMaxChargeTime ),
m_fMaxDamageRate( rhs.m_fMaxDamageRate ),
m_fMaxBlowRate( rhs.m_fMaxBlowRate ),
m_fMaxForceRate( rhs.m_fMaxForceRate ),
m_fMaxRangeRate( rhs.m_fMaxRangeRate ),
m_fMaxSpeedRate( rhs.m_fMaxSpeedRate ),
m_fMaxChargeAniRate( rhs.m_fMaxChargeAniRate ),
m_fMaxForceSlidingRate( rhs.m_fMaxForceSlidingRate ),
m_fMaxTargetRangeRate( rhs.m_fMaxTargetRangeRate ),
m_fMaxTargetAngleRate( rhs.m_fMaxTargetAngleRate ),
m_fMaxFloatRate( rhs.m_fMaxFloatRate ),
m_fMaxScaleRate( rhs.m_fMaxScaleRate ),
m_fMaxExplosionRate( rhs.m_fMaxExplosionRate ),
m_fMaxWoundedTimeRate( rhs.m_fMaxWoundedTimeRate ),
m_fMaxLiveTimeRate( rhs.m_fMaxLiveTimeRate ),
m_PowerChargeAttack( rhs.m_PowerChargeAttack ),
m_bPowerChargeUseDefense( rhs.m_bPowerChargeUseDefense ),
m_DashGatheringEffect( rhs.m_DashGatheringEffect ),
m_bPlayDashExtendWoundAni( rhs.m_bPlayDashExtendWoundAni )
{
	m_dwActionEndTime = 0;
	m_dwPreReserveEnableTime = 0;
	m_dwRunRotateStartTime = 0;

	m_bSetCombo = false;
	m_iCurComboCnt = 0;
	m_dwChargingStartTime = 0;
	m_DashState = DS_NONE;

	m_dwCancelCheckTime = 0;
	m_dwInputCancelCheckTime = 0;

	m_fCurChargeRate = 0.0f;
	m_GatheringEffectID = -1;

	m_stAniJump.Clear();
	m_fAniJumpRate = FLOAT1;
	m_iCurAniRotate = 0;
	m_dwAniRotateTime = 0;
	m_fAniRotateAngle = 0;
}

ioExtendDash* ioDashAttack2::Clone()
{
	return new ioDashAttack2( *this );
}

ioDashAttack2::~ioDashAttack2()
{
}

DashType ioDashAttack2::GetType() const
{
	return DT_DASH_ATTACK2;
}

void ioDashAttack2::LoadProperty( ioINILoader &rkLoader )
{
	ioExtendDash::LoadProperty( rkLoader );

	char szKey[MAX_PATH];
	char szBuf[MAX_PATH];

	m_dwChargingTime = rkLoader.LoadInt_e( "dash_charge_time", 0 );

	int iMaxChargeAni = rkLoader.LoadInt_e( "charging_dash_ani_cnt", 0 );
	for( int i=0; i < iMaxChargeAni; ++i )
	{
		wsprintf_e( szKey, "charging_dash_ani%d", i+1 );
		rkLoader.LoadString( szKey, "", szBuf, MAX_PATH );
		m_ChargingAniList.push_back( szBuf );
	}

	m_DashExtendType = (DashExtendType)rkLoader.LoadInt_e( "dash_extend_type", DET_NONE );

	// buff add
	LoadAttackAttribute_e( "dash_buff_add_attack", m_BuffAddAttribute, rkLoader );

	m_vBuffAddList.clear();
	int iBuffCnt = rkLoader.LoadInt_e( "dash_buff_add_cnt", 0 );
	for( int i=0; i < iBuffCnt; ++i )
	{
		wsprintf_e( szKey, "dash_buff_add%d", i+1 );
		rkLoader.LoadString( szKey, "", szBuf, MAX_PATH );

		ioHashString szBuffName = szBuf;
		m_vBuffAddList.push_back( szBuffName );
	}

	// Extend combo
	int iMax = rkLoader.LoadInt_e( "extend_dash_max_cnt", 0 );
	for( int i=0; i < iMax; ++i )
	{
		AttackAttribute eAttribute;

		wsprintf_e( szKey, "extend_dash_attack%d", i+1);
		LoadAttackAttribute( szKey, eAttribute, rkLoader );

		m_ExtendAttackList.push_back( eAttribute );
	}

	// run rotate
	m_fRunSpeed = rkLoader.LoadFloat_e( "extend_run_speed", 0.0f );
	m_dwRunDuration = (DWORD)rkLoader.LoadInt_e( "extend_run_duration", 0 );
	m_dwRotateSpeed = (DWORD)rkLoader.LoadInt_e( "extend_rotate_speed", 0 );

	m_dwRunRotateJumpTime = (DWORD)rkLoader.LoadInt_e( "extend_run_jump_time", 0 );

	LoadAttackAttribute_e( "extend_run_attack", m_RunRotateAttack, rkLoader );
	LoadAttackAttribute_e( "extend_run_attack_end", m_RunRotateAttackEnd, rkLoader );

	//
	m_dwMaxChargeTime = rkLoader.LoadInt_e( "dash_gathering_max_time", 0 );
	m_fMaxDamageRate = rkLoader.LoadFloat_e( "dash_gathering_max_damage_rate", FLOAT1 );
	m_fMaxForceRate = rkLoader.LoadFloat_e( "dash_gathering_max_force_rate", FLOAT1 );
	m_fMaxBlowRate = rkLoader.LoadFloat_e( "dash_gathering_max_blow_rate", FLOAT1 );

	m_fMaxRangeRate = rkLoader.LoadFloat_e( "dash_gathering_max_range_rate", FLOAT1 );
	m_fMaxSpeedRate = rkLoader.LoadFloat_e( "dash_gathering_max_speed_rate", FLOAT1 );
	m_fMaxChargeAniRate = rkLoader.LoadFloat_e( "dash_gathering_max_ani_rate", 0.0f );
	m_fMaxForceSlidingRate = rkLoader.LoadFloat_e( "dash_gathering_max_force_sliding_rate", 0.0f );

	m_fMaxTargetRangeRate = rkLoader.LoadFloat_e( "dash_gathering_max_target_range_rate", 0.0f );
	m_fMaxTargetAngleRate = rkLoader.LoadFloat_e( "dash_gathering_max_target_angle_rate", 0.0f );

	m_fMaxFloatRate = rkLoader.LoadFloat_e( "dash_gathering_max_float_rate", 0.0f );

	m_fMaxScaleRate = rkLoader.LoadFloat_e( "dash_gathering_max_scale_rate", 0.0f );
	m_fMaxExplosionRate = rkLoader.LoadFloat_e( "dash_gathering_max_explosion_rate", 0.0f );
	m_fMaxWoundedTimeRate = rkLoader.LoadFloat_e( "dash_gathering_max_wounded_time_rate", FLOAT1 );
	m_fMaxLiveTimeRate = rkLoader.LoadFloat_e( "dash_gathering_max_livetime_rate", FLOAT1 );

	rkLoader.LoadString_e( "dash_power_charge_ani", "", szBuf, MAX_PATH );
	m_PowerChargeAni = szBuf;

	LoadAttackAttribute_e( "dash_power_charge_attack", m_PowerChargeAttack, rkLoader );

	rkLoader.LoadString_e( "dash_power_charge_effect", "", szBuf, MAX_PATH );
	m_DashGatheringEffect = szBuf;

	m_bPlayDashExtendWoundAni = rkLoader.LoadBool_e( "dash_damage_play_ani_wounded", false );
	m_bPowerChargeUseDefense = rkLoader.LoadBool_e( "dash_power_charge_use_defense", false );
}	


bool ioDashAttack2::StartDash( ioBaseChar *pOwner )
{
	if( !pOwner )
		return false;

	ioExtendDash::StartDash( pOwner );

	m_bSetCombo = false;
	m_iCurComboCnt = 0;
	m_dwChargingStartTime = 0;

	m_dwActionEndTime = 0;
	m_dwPreReserveEnableTime = 0;
	m_dwRunRotateStartTime = 0;

	m_DashState = DS_NONE;

	m_dwInputCancelCheckTime = 0;
	m_dwCancelCheckTime = 0;

	m_fCurChargeRate = 0.0f;

	pOwner->InitExtendAttackTagInfo();
	pOwner->SetNoWoundState( false );
	pOwner->SetState( CS_EXTEND_DASH );

	//
	if( CheckRunRotateState( pOwner ) )
		SetRunRotateState( pOwner, false );
	else if( CheckPowerChargeState( pOwner ) )
		SetPowerChargeState( pOwner, false );
	else
		ChangeToCharging( pOwner );

	if( pOwner->IsNeedSendNetwork() )
	{
		SP2Packet kPacket( CUPK_EXTEND_DASH );
		kPacket << pOwner->GetCharName();
		kPacket << (int)GetType();
		kPacket << pOwner->GetHP().m_fCurValue;
		kPacket << DST_STATE;
		kPacket << DS_NONE;
		kPacket << m_iCurComboCnt;
		kPacket << pOwner->GetTargetRot();
		P2PNetwork::SendToAllPlayingUser( kPacket );
	}

	return true;
}

void ioDashAttack2::SetExtraInfoToWeapon( ioBaseChar *pOwner, ioPlayStage *pStage, ioWeapon *pWeapon, const ioHashString &szName )
{
	if( !pWeapon ) return;

	if( m_DashState == DS_RUN_ROTATE )
		pWeapon->SetLiveTime( m_dwRunDuration );
	else if( m_DashState == DS_POWER_CHARGE_ATTACK )
		CheckChargeInfo( pWeapon );

	ioZoneEffectWeapon *pZone = ToZoneEffectWeapon( pWeapon );
	ioMissileWeapon *pMissleWeapon = ToMissileWeapon( pWeapon );
	if( pZone )
	{
		ioBaseChar *pChar = pZone->GetOwner();
		if( pChar )
		{
			D3DXVECTOR3 vDir = pChar->GetTargetRot() * D3DXVECTOR3( 0.0f, 0.0f, -FLOAT1 );
			pZone->SetCollisionDir( vDir );

			if( m_DashState == DS_RUN_ROTATE )
				pZone->SetZoneValue( true, false );
			else
				pZone->SetZoneValue( false, false );
		}
	}

	if( pMissleWeapon )
	{
		ioBaseChar *pChar = pMissleWeapon->GetOwner();
		if( pChar )
		{
			ioItem *pItem = pOwner->GetEquipedItem( ES_WEAPON );
			ioChargeComboBowItem *pChargeComboBowItem = ToChargeComboBowItem( pItem );
			if( pChargeComboBowItem )
				pChargeComboBowItem->WasteBullet();
		}
	}
}

void ioDashAttack2::CheckKeyReserve( ioBaseChar *pOwner )
{
	if( !pOwner || !pOwner->IsNeedProcess() )
		return;

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
	else if( pOwner->IsJumpKey() && !m_bEnableJumpState && !bReserveSkill )
	{
		m_bEnableJumpState = true;
		m_bEnableAttackState = false;
		m_bEnableDefenseState = false;
		m_iSkillInput = -1;
	}
	//노말타때만 doublekey처리
	else if(pOwner->IsDirKeyDoubleClick() && !m_bEnableJumpState && !bReserveSkill && m_DashState == DS_NORMAL_ATTACK )
	{
		ioUserKeyInput::DirKeyInput eCurKey = pOwner->CheckCurDirKey();

		if( pOwner->IsSettedDirection() )
		{
			float fYawD = 180.0f + 45.0f * ( eCurKey - ioUserKeyInput::DKI_UP );
			fYawD = ioMath::ArrangeHead( fYawD );

			float fYawR = DEGtoRAD( fYawD );
			D3DXQUATERNION qtRotate;
			D3DXQuaternionRotationYawPitchRoll( &qtRotate, fYawR, 0.0f, 0.0f );

			pOwner->SetTargetRotToRotate( qtRotate, true, false );
		}

		if( m_DashExtendType == DET_EXTEND_COMBO )
		{
			if( CheckEnableExtendComboState( pOwner ) )
			{
				m_iCurComboCnt = 0;
				SetExtendComboState( pOwner );
				return;
			}
		}
	}
	else if( pOwner->IsAttackKey() && !m_bEnableAttackState && !bReserveSkill )
	{
		ioUserKeyInput::DirKeyInput eCurKey = pOwner->CheckCurDirKey();
		
		if( pOwner->IsSettedDirection() )
		{
			float fYawD = 180.0f + 45.0f * ( eCurKey - ioUserKeyInput::DKI_UP );
			fYawD = ioMath::ArrangeHead( fYawD );

			float fYawR = DEGtoRAD( fYawD );
			D3DXQUATERNION qtRotate;
			D3DXQuaternionRotationYawPitchRoll( &qtRotate, fYawR, 0.0f, 0.0f );

			pOwner->SetTargetRotToRotate( qtRotate, true, false );
		}

		ChangeToCharging( pOwner );

		if( pOwner->IsNeedSendNetwork() )
		{
			SP2Packet kPacket( CUPK_EXTEND_DASH );
			kPacket << pOwner->GetCharName();
			kPacket << (int)GetType();
			kPacket << pOwner->GetHP().m_fCurValue;
			kPacket << DST_STATE;
			kPacket << DS_CHARGE;
			kPacket << m_iCurComboCnt;
			kPacket << pOwner->GetTargetRot();
			P2PNetwork::SendToAllPlayingUser( kPacket );
		}
		return;
	}
}

void ioDashAttack2::CheckExtraReserve( ioBaseChar *pOwner )
{
	if( !pOwner || !pOwner->IsNeedProcess() )
		return;

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
	else if( pOwner->IsJumpKey() && !m_bEnableJumpState && !bReserveSkill )
	{
		m_bEnableJumpState = true;
		m_bEnableAttackState = false;
		m_bEnableDefenseState = false;
		m_iSkillInput = -1;
	}
	else if( pOwner->IsAttackKey() && !m_bEnableAttackState && !bReserveSkill )
	{
		m_bEnableAttackState = true;
		m_bEnableDefenseState = false;
		m_bEnableJumpState = false;

		m_iSkillInput = -1;
	}
}

void ioDashAttack2::ProcessDash( ioBaseChar *pOwner )
{
	DWORD dwCurTime = FRAMEGETTIME();

	switch( m_DashState )
	{
	case DS_CHARGE:
		ProcessCharging( pOwner );
		break;
	case DS_NORMAL_ATTACK:
	case DS_COMBO_ATTACK:
		ProcessNormalComboDash( pOwner );
		break;
	case DS_EXTEND_COMBO:
		ProcessExtendComboDash( pOwner );
		break;
	case DS_RUN_ROTATE:
		ProcessRunRotate( pOwner );
		break;
	case DS_RUN_ROTATE_END:
		ProcessRunRotateEnd( pOwner );
		break;
	case DS_POWER_CHARGE:
		ProcessPowerCharge( pOwner );
		break;
	case DS_POWER_CHARGE_ATTACK:
		ProcessPowerChargeAttack( pOwner );
		break;
	}
}

void ioDashAttack2::SetComboDash( ioBaseChar *pOwner )
{
	if( !pOwner ) return;

	int iMaxCnt = pOwner->GetExtraDashAttackMax();

	if( !COMPARE( m_iCurComboCnt, 0, iMaxCnt ) )
		return;

	ioEntityGroup *pGrp = pOwner->GetGroup();

	const AttackAttribute *pAttr = pOwner->GetAttackAttribute( AT_EXTRA_DASH_ATTACK, m_iCurComboCnt );
	if( !pAttr )
		return;

	int iAniID = pGrp->GetAnimationIdx( pAttr->m_AttackAnimation );

	if( iAniID == -1 )
	{
		LOG.PrintTimeAndLog( 0, "ioDashAttack2::SetComboDash - %s Animation is Not Exist",
								pAttr->m_AttackAnimation.c_str() );
		return;
	}

	float fTimeRate = pAttr->m_fAttackAniRate;
	DWORD dwPreDelay = pAttr->m_dwPreDelay;

	m_dwActionEndTime = 0;
	m_dwPreReserveEnableTime = 0;
	m_dwReserveEnableTime = 0;

	m_iSkillInput = -1;
	m_bEnableDefenseState = false;
	m_bEnableAttackState = false;
	m_bEnableJumpState = false;

	pGrp->ClearAllLoopAni( FLOAT100, true );
	pGrp->ClearAllActionAni( FLOAT100, true );

	pOwner->RefreshFireTimeList( iAniID,
								 pAttr->m_vWeaponInfoList,
								 FTT_EXTEND_DASH_ATTACK,
								 fTimeRate,
								 dwPreDelay );

	pOwner->IncreaseWeaponIndexBase();
	pOwner->SetNoWoundState( false );

	pOwner->SetState( CS_EXTEND_DASH );
	pOwner->SetAutoTarget( ATT_DASH );
	pGrp->SetActionAni( iAniID,	FLOAT100, FLOAT100, FLOAT1, FLOAT1/fTimeRate, 0.0f, (float)dwPreDelay, true );

	DWORD dwCurTime = FRAMEGETTIME();
	DWORD dwDuration = dwPreDelay + pGrp->GetAnimationFullTime( iAniID ) * fTimeRate + pAttr->m_dwEndDelay;

	m_dwActionEndTime = dwCurTime + dwDuration;

	CheckKeyReserveTime( pOwner, iAniID, fTimeRate );

	pOwner->SetReservedSliding( pAttr->m_vForceInfoList, iAniID, fTimeRate, dwPreDelay );

	pOwner->CheckCharColSkipTime( iAniID, fTimeRate, dwPreDelay );

	DWORD dwTrackingTime = FRAMEGETTIME() + dwPreDelay;
	dwTrackingTime += pGrp->GetAnimationEventTime_e( iAniID, "tracking_e" ) * fTimeRate;
	pOwner->SetAutoTargetTrackingEndTime( dwTrackingTime );

	pOwner->InitExtendAttackTagInfo();
	pOwner->SetBaseAttackExtraAniJump( iAniID, fTimeRate, dwPreDelay );
	pOwner->SetExtendAttackEndJumpInfo( iAniID, fTimeRate, dwPreDelay );

	m_bSetCombo = true;
	m_DashState = DS_COMBO_ATTACK;

	if( pOwner->IsNeedSendNetwork() )
	{
		SP2Packet kPacket( CUPK_EXTEND_DASH );
		kPacket << pOwner->GetCharName();
		kPacket << (int)GetType();
		kPacket << pOwner->GetHP().m_fCurValue;
		kPacket << DST_STATE;
		kPacket << DS_COMBO_ATTACK;
		kPacket << m_iCurComboCnt;
		kPacket << pOwner->GetTargetRot();
		P2PNetwork::SendToAllPlayingUser( kPacket );
	}

	// send 이후
	m_iCurComboCnt++;
}

void ioDashAttack2::ApplyDash( ioBaseChar *pOwner, SP2Packet &rkPacket )
{
	if( !pOwner ) return;

	int iSyncType;
	rkPacket >> iSyncType;

	switch( iSyncType )
	{
	case DST_STATE:
		ApplyDashState( pOwner, rkPacket );
		break;
	case DST_KEY:
		ApplyDashDirKey( pOwner, rkPacket );
		break;
	}
}

bool ioDashAttack2::CheckEnableBuffAddState( ioBaseChar *pOwner )
{
	if( !pOwner )
		return false;

	if( m_DashExtendType != DET_BUFF_ADD )
		return false;

	ioAttackableItem *pItem = pOwner->GetPriorityItem( SPT_ATTACK );
	ioBuffAddItem *pBuffAddItem = ToBuffAddItem( pItem );
	if( !pBuffAddItem )
		return false;

	if( pBuffAddItem->GetCurBullet() >= pBuffAddItem->GetMaxBullet() )
		return true;

	return false;
}

void ioDashAttack2::SetBuffAddState( ioBaseChar *pOwner )
{
	if( !pOwner )
		return;

	ioAttackableItem *pItem = pOwner->GetPriorityItem( SPT_ATTACK );
	ioBuffAddItem *pBuffAddItem = ToBuffAddItem( pItem );
	if( !pBuffAddItem )
		return;

	if( pOwner->IsNeedProcess() )
	{
		pOwner->SetTargetRotToRotate( pOwner->GetTargetRot(), false, false );

		if( pOwner->IsCanRotateJumpAttack() )
		{
			ioUserKeyInput::DirKeyInput eDir = pOwner->CheckCurDirKey();
			if( eDir != ioUserKeyInput::DKI_NONE )
			{
				float fYawD = 180.0f + 45.0f * ( eDir - ioUserKeyInput::DKI_UP );
				fYawD = ioMath::ArrangeHead( fYawD );

				float fYawR = DEGtoRAD( fYawD );
				D3DXQUATERNION qtRotate;
				D3DXQuaternionRotationYawPitchRoll( &qtRotate, fYawR, 0.0f, 0.0f );

				pOwner->SetTargetRotToRotate( qtRotate, false, false );
				pOwner->SetInputDirection( pOwner->GetMoveDir() );
			}
		}
	}

	// change state & motion
	pOwner->SetWereWolfState( m_BuffAddAttribute.m_AttackAnimation, m_BuffAddAttribute.m_fAttackAniRate );

	pBuffAddItem->SetCurBullet( 0 );
	pOwner->SetCurJumpPowerAmt( 0.0f );
	pOwner->SetForcePowerAmt( 0.0f );
	pOwner->SetGravityAmt( 0.0f );

	ioEntityGroup *pGrp = pOwner->GetGroup();
	int iAniID = pGrp->GetAnimationIdx( m_BuffAddAttribute.m_AttackAnimation );
	float fTimeRate = m_BuffAddAttribute.m_fAttackAniRate;
	if( fTimeRate <= 0.0f )
		fTimeRate = FLOAT1;

	pOwner->RefreshFireTimeList( iAniID, m_BuffAddAttribute.m_vWeaponInfoList, FTT_NORMAL_ATTACK, fTimeRate, 0 );
	pOwner->IncreaseWeaponIndexBase();

	pOwner->CheckCharColSkipTime( iAniID, fTimeRate, m_BuffAddAttribute.m_dwPreDelay );
	pOwner->SetReservedSliding( m_BuffAddAttribute.m_vForceInfoList, iAniID, fTimeRate, 0 );

	// add buff
	int iBuffCnt = m_vBuffAddList.size();
	for( int j=0; j < iBuffCnt; ++j )
	{
		ioHashString szBuffName = m_vBuffAddList[j];
		pOwner->AddNewBuff( szBuffName, pOwner->GetCharName(), pBuffAddItem->GetName(), NULL );
	}

	m_DashState = DS_NONE;

	if( pOwner->IsNeedSendNetwork() )
	{
		SP2Packet kPacket( CUPK_EXTEND_DASH );
		kPacket << pOwner->GetCharName();
		kPacket << (int)GetType();
		kPacket << pOwner->GetHP().m_fCurValue;
		kPacket << DST_STATE;
		kPacket << DS_BUFF_ADD;
		kPacket << m_iCurComboCnt;
		kPacket << pOwner->GetTargetRot();
		P2PNetwork::SendToAllPlayingUser( kPacket );
	}
}

void ioDashAttack2::ExtendDashEnd( ioBaseChar *pOwner )
{
	m_DashState = DS_NONE;

	m_bSetCombo = false;
	m_iCurComboCnt = 0;
	m_dwChargingStartTime = 0;

	m_dwActionEndTime = 0;
	m_dwReserveEnableTime = 0;
	m_dwPreReserveEnableTime = 0;

	m_iSkillInput = -1;
	m_bEnableDefenseState = false;
	m_bEnableAttackState = false;
	m_bEnableJumpState = false;

	if( pOwner )
		pOwner->InitExtendAttackTagInfo();

	if( !m_DashGatheringEffect.IsEmpty() )
	{
		if( m_GatheringEffectID != -1 )
		{
			pOwner->EndEffect( m_GatheringEffectID, false );
			m_GatheringEffectID = -1;
		}
	}

	RemoveRundRotateBuff( pOwner );
}

void ioDashAttack2::ChangeToCharging( ioBaseChar *pOwner )
{
	if( !pOwner )
		return;

	ioEntityGroup *pGrp = pOwner->GetGroup();
	if( !pGrp )
		return;

	if( m_ChargingAniList.empty() )
		return;

	ioHashString szCurChargingAni;

	int iChargingCnt = m_ChargingAniList.size();
	int iCurIndex = m_iCurComboCnt;

	if( m_bSetCombo )
		iCurIndex = m_iCurComboCnt+1;

	if( COMPARE( iCurIndex, 0, iChargingCnt ) )
		szCurChargingAni = m_ChargingAniList[iCurIndex];
	else
		szCurChargingAni = m_ChargingAniList[0];

	int iAniID = pGrp->GetAnimationIdx( szCurChargingAni );

	pGrp->ClearAllLoopAni( FLOAT100, true );
	pGrp->ClearAllActionAni( FLOAT100, true );

	pGrp->SetLoopAni( szCurChargingAni, FLOAT100 );

	m_dwChargingStartTime = FRAMEGETTIME();

	m_dwActionEndTime = 0;
	m_dwReserveEnableTime = 0;
	m_dwPreReserveEnableTime = 0;

	m_iSkillInput = -1;
	m_bEnableDefenseState = false;
	m_bEnableAttackState = false;
	m_bEnableJumpState = false;

	m_DashState = DS_CHARGE;
}

void ioDashAttack2::ChangeToNormalAttack( ioBaseChar *pOwner )
{
	const AttackAttribute *pAttr = pOwner->GetAttackAttribute( AT_DASH_ATTACK );
	if( !pAttr )
		return;

	ioEntityGroup *pGrp = pOwner->GetGroup();
	int iAniID = pGrp->GetAnimationIdx( pAttr->m_AttackAnimation );
	if( iAniID == -1 )
	{
		LOG.PrintTimeAndLog( 0, "ioDashAttack2::ChangeToNormalAttack - %s Animation is Not Exist",
								pAttr->m_AttackAnimation.c_str() );
		return;
	}

	float fTimeRate = pAttr->m_fAttackAniRate;

	m_dwActionEndTime = 0;
	m_dwReserveEnableTime = 0;
	m_dwPreReserveEnableTime = 0;

	m_iSkillInput = -1;
	m_bEnableDefenseState = false;
	m_bEnableAttackState = false;
	m_bEnableJumpState = false;

	pGrp->ClearAllLoopAni( FLOAT100, true );
	pGrp->ClearAllActionAni( FLOAT100, true );

	pOwner->RefreshFireTimeList( iAniID,
								 pAttr->m_vWeaponInfoList,
								 FTT_EXTEND_DASH_ATTACK,
								 fTimeRate,
								 pAttr->m_dwPreDelay );

	pOwner->IncreaseWeaponIndexBase();
	pOwner->SetNoWoundState( false );

	pOwner->SetState( CS_EXTEND_DASH );
	pOwner->SetAutoTarget( ATT_DASH );
	pGrp->SetActionAni( iAniID,	FLOAT100, FLOAT100, FLOAT1, FLOAT1/fTimeRate, 0.0f, (float)pAttr->m_dwPreDelay, true );

	DWORD dwCurTime = FRAMEGETTIME();
	DWORD dwDuration = pAttr->m_dwPreDelay + pGrp->GetAnimationFullTime( iAniID ) * fTimeRate + pAttr->m_dwEndDelay;

	m_dwActionEndTime = dwCurTime + dwDuration;

	CheckKeyReserveTime( pOwner, iAniID, fTimeRate );

	pOwner->SetReservedSliding( pAttr->m_vForceInfoList, iAniID, fTimeRate, pAttr->m_dwPreDelay );

	pOwner->CheckCharColSkipTime( iAniID, fTimeRate, pAttr->m_dwPreDelay );

	DWORD dwTrackingTime = FRAMEGETTIME() + pAttr->m_dwPreDelay;
	dwTrackingTime += pGrp->GetAnimationEventTime_e( iAniID, "tracking_e" ) * fTimeRate;
	pOwner->SetAutoTargetTrackingEndTime( dwTrackingTime );

	m_bSetCombo = true;
	m_DashState = DS_NORMAL_ATTACK;

	if( pOwner->IsNeedSendNetwork() )
	{
		SP2Packet kPacket( CUPK_EXTEND_DASH );
		kPacket << pOwner->GetCharName();
		kPacket << (int)GetType();
		kPacket << pOwner->GetHP().m_fCurValue;
		kPacket << DST_STATE;
		kPacket << DS_NORMAL_ATTACK;
		kPacket << m_iCurComboCnt;
		kPacket << pOwner->GetTargetRot();
		P2PNetwork::SendToAllPlayingUser( kPacket );
	}
}

void ioDashAttack2::ProcessCharging( ioBaseChar *pOwner )
{
	if( !pOwner ) return;
	if( !pOwner->IsNeedProcess() )
		return;

	ioEntityGroup *pGrp = pOwner->GetGroup();
	if( !pGrp ) return;

	DWORD dwCurTime = FRAMEGETTIME();
	if( pOwner->IsAttackKeyDown() )
	{
		//if( m_dwChargingStartTime+m_dwChargingTime < dwCurTime )
		{
			/*switch( m_DashExtendType )
			{
			case DET_BUFF_ADD:
				if( CheckEnableBuffAddState( pOwner ) )
				{
					SetBuffAddState( pOwner );
					return;
				}
				break;
			case DET_EXTEND_COMBO:
				if( CheckEnableExtendComboState( pOwner ) )
				{
					m_iCurComboCnt = 0;
					SetExtendComboState( pOwner );
					return;
				}
				break;
			case DET_RUN_ROTATE:
				if( CheckRunRotateState( pOwner ) )
				{
					SetRunRotateState( pOwner, true );
					return;
				}
				break;
			case DET_RUN_ROTATE2:
				if( CheckRunRotateState2( pOwner ) )
				{
					SetRunRotateState( pOwner, true );
					return;
				}
				break;
			}*/

			if( m_bSetCombo && !CheckCanComboDash( pOwner ) )
				return;

			if( m_bSetCombo )
				SetComboDash( pOwner );
			else
				ChangeToNormalAttack( pOwner );
			return;
		}
	}
	else
	{
		if( m_bSetCombo && !CheckCanComboDash( pOwner ) )
			return;

		if( m_bSetCombo )
		{
			SetComboDash( pOwner );
		}
		else
		{
			ChangeToNormalAttack( pOwner );
		}
		return;
	}
}


bool ioDashAttack2::CheckCanComboDash( ioBaseChar *pOwner )
{
	if( m_bSetCombo && m_DashExtendType == DET_EXTEND_COMBO )
	{
		ioItem *pItem = pOwner->GetEquipedItem( ES_WEAPON );
		ioChargeComboBowItem *pChargeComboBowItem = ToChargeComboBowItem( pItem );
		if( pChargeComboBowItem )
		{
			if( pChargeComboBowItem->GetCurBullet() == 0 )
			{
				//ExtendDashEnd( pOwner );
				m_bSetCombo = false;
				pOwner->ClearAttackFireTimeAndSkill();
				pOwner->ClearReservedSliding();

				pOwner->SetState( CS_DELAY );
				return false;
			}
		}
	}

	return true;
}

void ioDashAttack2::ProcessNormalComboDash( ioBaseChar *pOwner )
{
	DWORD dwCurTime = FRAMEGETTIME();
	DWORD dwEndTime = m_dwActionEndTime + pOwner->GetActionStopTotalDelay();

	if( dwEndTime > dwCurTime )
	{
		pOwner->CheckExtraAniJump();

		if( pOwner->IsNeedProcess() )
		{
			if( pOwner->CheckExtendAttackEndJump() )
				return;
		}

		if( m_dwReserveEnableTime > 0 && m_dwReserveEnableTime + pOwner->GetActionStopTotalDelay() < dwCurTime )
		{
			// 예약체크
			int iMaxCnt = pOwner->GetExtraDashAttackMax();
			if( m_bSetCombo && m_iCurComboCnt >= iMaxCnt )
				CheckExtraReserve( pOwner );
			else
				CheckKeyReserve( pOwner );

			return;
		}
	}
	else
	{
		ProcessKeyReserve( pOwner );
		return;
	}
}

void ioDashAttack2::ProcessKeyReserve( ioBaseChar *pOwner )
{
	// 공격키가 눌러져 있으면 공격 상태 처리
	if( pOwner->IsAttackKeyDown() && 
		!pOwner->IsJumpKeyDown() &&
		!pOwner->IsDefenseKeyDown() &&
		!pOwner->IsChatModeState(true) )
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
				pOwner->SetNormalAttack( 0 );
			}
		}

		return;
	}

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
				pOwner->SetNormalAttack( 0 );
			}
		}
		return;
	}

	// 대기
	pOwner->ClearAttackFireTimeAndSkill();
	pOwner->ClearReservedSliding();

	pOwner->SetState( CS_DELAY );
}



bool ioDashAttack2::CheckEnableExtendComboState( ioBaseChar *pOwner )
{
	if( !pOwner )
		return false;

	if( m_DashExtendType != DET_EXTEND_COMBO )
		return false;

	if( m_ExtendAttackList.empty() )
		return false;

	ioItem *pItem = pOwner->GetEquipedItem( ES_WEAPON );
	ioChargeComboBowItem *pChargeComboBowItem = ToChargeComboBowItem( pItem );
	if( pChargeComboBowItem )
	{
		if( pChargeComboBowItem->GetCurBullet() == 0 )
			return false;
	}

	return true;
}

void ioDashAttack2::SetExtendComboState( ioBaseChar *pOwner )
{
	if( !pOwner ) return;

	int iMaxCnt = m_ExtendAttackList.size();
	if( !COMPARE( m_iCurComboCnt, 0, iMaxCnt ) )
		return;

	ioEntityGroup *pGrp = pOwner->GetGroup();
	if( !pGrp ) return;

	const AttackAttribute &rkAttribute = m_ExtendAttackList[m_iCurComboCnt];

	int iAniID = pGrp->GetAnimationIdx( rkAttribute.m_AttackAnimation );
	m_stAniJump = rkAttribute.m_AttackAnimation;
	if( iAniID == -1 )
	{
		LOG.PrintTimeAndLog( 0, "ioDashAttack2::SetExtendComboState - %s Animation is Not Exist",
								rkAttribute.m_AttackAnimation.c_str() );
		return;
	}

	float fTimeRate = rkAttribute.m_fAttackAniRate;
	m_fAniJumpRate = fTimeRate;
	DWORD dwPreDelay = rkAttribute.m_dwPreDelay;

	m_dwActionEndTime = 0;
	m_dwReserveEnableTime = 0;
	m_dwPreReserveEnableTime = 0;

	m_iSkillInput = -1;
	m_bEnableDefenseState = false;
	m_bEnableAttackState = false;
	m_bEnableJumpState = false;

	pGrp->ClearAllLoopAni( FLOAT100, true );
	pGrp->ClearAllActionAni( FLOAT100, true );

	pOwner->InitExtendAttackTagInfo();
	pOwner->InitActionStopDelay( true );

	pOwner->RefreshFireTimeList( iAniID, rkAttribute.m_vWeaponInfoList, FTT_EXTEND_DASH_ATTACK, fTimeRate, dwPreDelay );

	pOwner->IncreaseWeaponIndexBase();
	pOwner->SetNoWoundState( false );

	pOwner->SetAutoTarget( ATT_DASH );
	pGrp->SetActionAni( iAniID,	FLOAT100, FLOAT100, FLOAT1, FLOAT1/fTimeRate, 0.0f, (float)dwPreDelay, true );

	DWORD dwCurTime = FRAMEGETTIME();
	DWORD dwDuration = dwPreDelay + pGrp->GetAnimationFullTime( iAniID ) * fTimeRate + rkAttribute.m_dwEndDelay;

	DWORD dwEndTime = pGrp->GetAnimationEventTime_e( iAniID, "landing" );
	if( dwEndTime == 0 )
	{
		dwEndTime = pGrp->GetAnimationFullTime( iAniID );
	}

	int iEndAniID = pGrp->GetAnimationIdx( rkAttribute.m_AttackEndAnimation );
	if( iEndAniID == -1 )
	{
		pGrp->SetActionAni( iAniID, FLOAT100, FLOAT100, FLOAT1, FLOAT1/fTimeRate, 0.0f, dwPreDelay, true );

		CheckKeyReserveTime( pOwner, iAniID, fTimeRate );
	}
	else
	{
		pGrp->SetActionAni( iAniID, FLOAT100, 0.0f, FLOAT1, FLOAT1/fTimeRate, 0.0f, dwPreDelay );

		float fReserveTime = pGrp->GetAnimationEventTime_e( iAniID, "key_reserve" ) * fTimeRate;
		if( fReserveTime > 0.0f )
		{
			m_dwPreReserveEnableTime = dwCurTime + fReserveTime;
		}

		float fWaitTime = dwEndTime * fTimeRate + dwPreDelay;
		float fEndTimeRate = rkAttribute.m_fAttackEndAniRate;

		pGrp->SetActionAni( iEndAniID, 0.0f, FLOAT100, FLOAT1, FLOAT1/fEndTimeRate, fWaitTime, 0.0f, true );

		CheckKeyReserveTime( pOwner, iEndAniID, fEndTimeRate );
		if( m_dwReserveEnableTime > 0 )
		{
			m_dwReserveEnableTime = dwDuration;
		}

		dwDuration += pGrp->GetAnimationFullTime( iEndAniID ) * fEndTimeRate + rkAttribute.m_dwEndDelay;
	}

	m_dwActionStartTime = dwCurTime;
	m_dwActionEndTime = dwCurTime + dwDuration;

	pOwner->SetReservedSliding( rkAttribute.m_vForceInfoList, iAniID, fTimeRate, dwPreDelay );
	pOwner->CheckCharColSkipTime( iAniID, fTimeRate, dwPreDelay );

	DWORD dwTrackingTime = dwCurTime + dwPreDelay;
	dwTrackingTime += pGrp->GetAnimationEventTime_e( iAniID, "tracking_e" ) * fTimeRate;
	pOwner->SetAutoTargetTrackingEndTime( dwTrackingTime );

	pOwner->SetBaseAttackExtraAniJump( iAniID, fTimeRate, dwPreDelay );
	pOwner->SetExtendAttackEndJumpInfo( iAniID, fTimeRate, dwPreDelay );

	m_bSetCombo = true;
	m_DashState = DS_EXTEND_COMBO;

	//////////////////////////////////////////////////////////////////////////
	m_iCurAniRotate = 0;
	m_dwAniRotateTime = 0;
	m_fAniRotateAngle = 0;
	m_vAniRotateInfoList.clear();

	AniEventConstPtrList vAniRotateList;
	pGrp->GetAnimationEventConstPtrList( iAniID, "ani_rotate", vAniRotateList );

	AniEventConstPtrList::iterator iter_r = vAniRotateList.begin();
	while( iter_r != vAniRotateList.end() )
	{
		AniRotateInfo kInfo;

		kInfo.m_dwTime = (*iter_r)->fEventTime * fTimeRate;

		if( kInfo.m_dwTime > 0 )
			kInfo.m_dwTime += dwCurTime;
		else
			kInfo.m_dwTime = 0;

		if( !(*iter_r)->szSubInfo.IsEmpty() )
		{
			const StringVector &vParam = ioStringConverter::Split( (*iter_r)->szSubInfo.c_str(), " " );
			if( vParam.size() == 1 )
			{
				kInfo.m_fAngle = ioStringConverter::ParseFloat( vParam[0].c_str() );
			}
		}

		m_vAniRotateInfoList.push_back(kInfo);

		++iter_r;
	}

	if( !m_vAniRotateInfoList.empty() )
	{
		m_iCurAniRotate = 0;
		m_dwAniRotateTime = m_vAniRotateInfoList[m_iCurAniRotate].m_dwTime;
		m_fAniRotateAngle = m_vAniRotateInfoList[m_iCurAniRotate].m_fAngle;
	}
	//////////////////////////////////////////////////////////////////////////


	if( pOwner->IsNeedSendNetwork() )
	{
		SP2Packet kPacket( CUPK_EXTEND_DASH );
		kPacket << pOwner->GetCharName();
		kPacket << (int)GetType();
		kPacket << pOwner->GetHP().m_fCurValue;
		kPacket << DST_STATE;
		kPacket << DS_EXTEND_COMBO;
		kPacket << m_iCurComboCnt;
		kPacket << pOwner->GetTargetRot();
		P2PNetwork::SendToAllPlayingUser( kPacket );
	}

	// send 이후
	m_iCurComboCnt++;
}


void ioDashAttack2::ProcessExtendComboDash( ioBaseChar *pOwner )
{
	DWORD dwCurTime = FRAMEGETTIME();
	DWORD dwEndTime = m_dwActionEndTime + pOwner->GetActionStopTotalDelay();

	if( dwEndTime > dwCurTime )
	{
		pOwner->CheckExtraAniJump();
		CheckAniRotate( pOwner );

		if( pOwner->IsNeedProcess() )
		{
			if( pOwner->CheckExtendAttackEndJump() )
				return;
		}

		if( m_dwPreReserveEnableTime > 0 && m_dwPreReserveEnableTime + pOwner->GetActionStopTotalDelay() < dwCurTime )
		{
			if( CheckPreReserve( pOwner ) && !pOwner->IsBuffLimitJump() )
			{
				pOwner->SetJumpState();
				return;
			}
		}

		if( m_dwReserveEnableTime > 0 && m_dwReserveEnableTime + pOwner->GetActionStopTotalDelay() < dwCurTime )
		{
			// 예약체크
			int iMaxCnt = m_ExtendAttackList.size();
			if( m_bSetCombo && m_iCurComboCnt >= iMaxCnt )
				CheckExtraReserve( pOwner );
			else
				CheckExtendReserve( pOwner );

			//CheckAniJump_Input_Jump_S( pOwner );

			return;
		}
	}
	else
	{
		// 공격키가 눌러져 있으면 공격 상태 처리
		if( pOwner->IsAttackKeyDown() && 
			!pOwner->IsJumpKeyDown() &&
			!pOwner->IsDefenseKeyDown() &&
			!pOwner->IsChatModeState(true) )
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
					pOwner->SetNormalAttack( 0 );
				}
			}

			return;
		}

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
					pOwner->SetNormalAttack( 0 );
				}
			}
			return;
		}

		// 대기
		pOwner->ClearAttackFireTimeAndSkill();
		pOwner->ClearReservedSliding();

		pOwner->SetState( CS_DELAY );
	}
}

bool ioDashAttack2::CheckPreReserve( ioBaseChar *pOwner )
{
	if( !pOwner || !pOwner->IsNeedProcess() )
		return false;

	if( pOwner->IsJumpKey() )
	{
		return true;
	}

	return false;
}

void ioDashAttack2::CheckExtendReserve( ioBaseChar *pOwner )
{
	if( !pOwner || !pOwner->IsNeedProcess() )
		return;

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
	else if( pOwner->IsJumpKey() && !m_bEnableJumpState && !bReserveSkill )
	{
		m_bEnableJumpState = true;
		m_bEnableAttackState = false;
		m_bEnableDefenseState = false;
		m_iSkillInput = -1;
	}
	else if( pOwner->IsAttackKey() && !m_bEnableAttackState && !bReserveSkill )
	{
		ioUserKeyInput::DirKeyInput eCurKey = pOwner->CheckCurDirKey();

		if( pOwner->IsSettedDirection() )
		{
			float fYawD = 180.0f + 45.0f * ( eCurKey - ioUserKeyInput::DKI_UP );
			fYawD = ioMath::ArrangeHead( fYawD );

			float fYawR = DEGtoRAD( fYawD );
			D3DXQUATERNION qtRotate;
			D3DXQuaternionRotationYawPitchRoll( &qtRotate, fYawR, 0.0f, 0.0f );

			pOwner->SetTargetRotToRotate( qtRotate, true, false );
		}

		SetExtendComboState( pOwner );
		return;
	}
}

void ioDashAttack2::ProcessRunRotate( ioBaseChar *pOwner )
{
	if( !pOwner )
		return;

	DWORD dwCurTime = FRAMEGETTIME();
	DWORD dwEndTime = m_dwActionEndTime + pOwner->GetActionStopTotalDelay();

	if( dwEndTime > dwCurTime )
	{
		pOwner->CheckExtraAniJump();

		if( pOwner->IsNeedProcess() )
		{
			// 점프키 누름. 점프상태 전환
			bool bEnableJumpTime = false;
			if( m_dwRunRotateStartTime+m_dwRunRotateJumpTime < dwCurTime )
				bEnableJumpTime = true;
			
			if( bEnableJumpTime && pOwner->IsJumpKeyDown() && pOwner->IsCanJumpState() && !pOwner->IsBuffLimitJump() )
			{
				pOwner->SetJumpState();
				return;
			}

			if( pOwner->CheckExtendAttackEndJump() )
				return;
		}

		CheckKeyInput( pOwner );
		ProcessRotate( pOwner );

		pOwner->SetCurMoveSpeed( m_fRunSpeed );
	}
	else
	{
		SetRunRotateStateEnd( pOwner );
	}
}

void ioDashAttack2::ProcessRunRotateEnd( ioBaseChar *pOwner )
{
	if( !pOwner )
		return;

	DWORD dwCurTime = FRAMEGETTIME();
	DWORD dwEndTime = m_dwActionEndTime + pOwner->GetActionStopTotalDelay();

	if( dwEndTime > dwCurTime )
	{
		pOwner->CheckExtraAniJump();

		if( pOwner->IsNeedProcess() )
		{
			if( pOwner->CheckExtendAttackEndJump() )
				return;
		}

		if( m_dwReserveEnableTime > 0 && m_dwReserveEnableTime + pOwner->GetActionStopTotalDelay() < dwCurTime )
		{
			CheckExtraReserve( pOwner );
			return;
		}
	}
	else
	{
		ProcessKeyReserve( pOwner );
	}
}

bool ioDashAttack2::CheckRunRotateState( ioBaseChar *pOwner )
{
	if( !pOwner )
		return false;

	if( m_DashExtendType != DET_RUN_ROTATE )
		return false;

	return true;
}

bool ioDashAttack2::CheckRunRotateState2( ioBaseChar *pOwner )
{
	if( !pOwner )
		return false;

	if( m_DashExtendType != DET_RUN_ROTATE2 )
		return false;

	return true;
}


void ioDashAttack2::CheckAniRotate( ioBaseChar *pChar )
{
	if( !pChar ) return;
	if( m_vAniRotateInfoList.empty() ) return;

	DWORD dwCurTime = FRAMEGETTIME();
	if( m_dwAniRotateTime > 0 && m_dwAniRotateTime < dwCurTime )
	{
		if( m_fAniRotateAngle > 0.0f )
		{
			D3DXQUATERNION qtRot;
			D3DXQuaternionRotationAxis( &qtRot, &ioMath::UNIT_Y, DEGtoRAD(m_fAniRotateAngle) );
			D3DXVECTOR3 vMoveDir = qtRot * pChar->GetMoveDir();

			D3DXQUATERNION qtTargetRot;
			ioMath::CalcDirectionQuaternion( qtTargetRot, -vMoveDir );

			pChar->SetTargetRotAndMoveDirChange( qtTargetRot );
		}

		m_iCurAniRotate++;

		int iSize = m_vAniRotateInfoList.size();
		if( COMPARE(m_iCurAniRotate, 0, iSize) )
		{
			m_dwAniRotateTime = m_vAniRotateInfoList[m_iCurAniRotate].m_dwTime;
			m_fAniRotateAngle = m_vAniRotateInfoList[m_iCurAniRotate].m_fAngle;
		}
		else
		{
			m_dwAniRotateTime = 0;
			m_fAniRotateAngle = 0.0f;
		}
	}
}

void ioDashAttack2::SetRunRotateState( ioBaseChar *pOwner, bool bSendNet )
{
	if( !pOwner ) return;

	ioEntityGroup *pGrp = pOwner->GetGroup();
	if( !pGrp ) return;

	int iAniID = pGrp->GetAnimationIdx( m_RunRotateAttack.m_AttackAnimation );
	if( iAniID == -1 )
	{
		LOG.PrintTimeAndLog( 0, "ioDashAttack2::SetRunRotateState - %s Animation is Not Exist",
								m_RunRotateAttack.m_AttackAnimation.c_str() );
		return;
	}

	float fTimeRate = m_RunRotateAttack.m_fAttackAniRate;
	DWORD dwPreDelay = m_RunRotateAttack.m_dwPreDelay;

	DWORD dwCurTime = FRAMEGETTIME();

	m_dwActionEndTime = 0;
	m_dwReserveEnableTime = 0;
	m_dwPreReserveEnableTime = 0;
	m_dwRunRotateStartTime = dwCurTime;

	m_iSkillInput = -1;
	m_bEnableDefenseState = false;
	m_bEnableAttackState = false;
	m_bEnableJumpState = false;

	pGrp->ClearAllLoopAni( FLOAT100, true );
	pGrp->ClearAllActionAni( FLOAT100, true );

	pOwner->RefreshFireTimeList( iAniID, m_RunRotateAttack.m_vWeaponInfoList, FTT_EXTEND_DASH_ATTACK, fTimeRate, dwPreDelay );
	pOwner->IncreaseWeaponIndexBase();
	pOwner->SetNoWoundState( false );

	pGrp->SetLoopAni( iAniID, FLOAT100, FLOAT1, FLOAT1/fTimeRate );

	DWORD dwDuration = dwPreDelay + pGrp->GetAnimationFullTime( iAniID ) * fTimeRate + m_RunRotateAttack.m_dwEndDelay;

	m_dwActionEndTime = dwCurTime + m_dwRunDuration;

	CheckKeyReserveTime( pOwner, iAniID, fTimeRate );

	pOwner->SetReservedSliding( m_RunRotateAttack.m_vForceInfoList, iAniID, fTimeRate, dwPreDelay );
	pOwner->CheckCharColSkipTime( iAniID, fTimeRate, dwPreDelay );

	pOwner->InitExtendAttackTagInfo();
	pOwner->SetBaseAttackExtraAniJump( iAniID, fTimeRate, dwPreDelay );
	pOwner->SetExtendAttackEndJumpInfo( iAniID, fTimeRate, dwPreDelay );

	m_DashState = DS_RUN_ROTATE;

	SetRunRotateBuff( pOwner );

	if( bSendNet && pOwner->IsNeedSendNetwork() )
	{
		SP2Packet kPacket( CUPK_EXTEND_DASH );
		kPacket << pOwner->GetCharName();
		kPacket << (int)GetType();
		kPacket << pOwner->GetHP().m_fCurValue;
		kPacket << DST_STATE;
		kPacket << DS_RUN_ROTATE;
		kPacket << m_iCurComboCnt;
		kPacket << pOwner->GetTargetRot();
		P2PNetwork::SendToAllPlayingUser( kPacket );
	}
}

void ioDashAttack2::SetRunRotateStateEnd( ioBaseChar *pOwner )
{
	if( !pOwner ) return;

	ioEntityGroup *pGrp = pOwner->GetGroup();
	if( !pGrp ) return;

	int iAniID = pGrp->GetAnimationIdx( m_RunRotateAttackEnd.m_AttackAnimation );
	if( iAniID == -1 )
	{
		LOG.PrintTimeAndLog( 0, "ioDashAttack2::SetRunRotateStateEnd - %s Animation is Not Exist",
			m_RunRotateAttackEnd.m_AttackAnimation.c_str() );
		return;
	}

	float fTimeRate = m_RunRotateAttackEnd.m_fAttackAniRate;
	DWORD dwPreDelay = m_RunRotateAttackEnd.m_dwPreDelay;

	m_dwActionEndTime = 0;
	m_dwReserveEnableTime = 0;
	m_dwPreReserveEnableTime = 0;

	m_iSkillInput = -1;
	m_bEnableDefenseState = false;
	m_bEnableAttackState = false;
	m_bEnableJumpState = false;

	DWORD dwCurTime = FRAMEGETTIME();

	pGrp->ClearAllLoopAni( FLOAT100, true );
	pGrp->ClearAllActionAni( FLOAT100, true );

	pOwner->ClearAttackFireTimeAndSkill();
	pOwner->ClearReservedSliding();

	pOwner->RefreshFireTimeList( iAniID, m_RunRotateAttackEnd.m_vWeaponInfoList, FTT_EXTEND_DASH_ATTACK, fTimeRate, dwPreDelay );
	pOwner->IncreaseWeaponIndexBase();
	pOwner->SetNoWoundState( false );

	pGrp->SetActionAni( iAniID, FLOAT100, FLOAT100, FLOAT1, FLOAT1/fTimeRate, 0.0f, 0.0f, true );

	DWORD dwDuration = dwPreDelay + pGrp->GetAnimationFullTime( iAniID ) * fTimeRate + m_RunRotateAttackEnd.m_dwEndDelay;
	m_dwActionEndTime = dwCurTime + dwDuration;

	CheckKeyReserveTime( pOwner, iAniID, fTimeRate );

	pOwner->SetReservedSliding( m_RunRotateAttackEnd.m_vForceInfoList, iAniID, fTimeRate, dwPreDelay );
	pOwner->CheckCharColSkipTime( iAniID, fTimeRate, dwPreDelay );

	pOwner->InitExtendAttackTagInfo();
	pOwner->SetBaseAttackExtraAniJump( iAniID, fTimeRate, dwPreDelay );
	pOwner->SetExtendAttackEndJumpInfo( iAniID, fTimeRate, dwPreDelay );

	m_DashState = DS_RUN_ROTATE_END;

	RemoveRundRotateBuff( pOwner );

	m_dwInputCancelCheckTime = dwCurTime;
	m_dwCancelCheckTime = dwCurTime;

	if( pOwner->IsNeedSendNetwork() )
	{
		SP2Packet kPacket( CUPK_EXTEND_DASH );
		kPacket << pOwner->GetCharName();
		kPacket << (int)GetType();
		kPacket << pOwner->GetHP().m_fCurValue;
		kPacket << DST_STATE;
		kPacket << DS_RUN_ROTATE_END;
		kPacket << m_iCurComboCnt;
		kPacket << pOwner->GetTargetRot();
		P2PNetwork::SendToAllPlayingUser( kPacket );
	}
}

void ioDashAttack2::CheckKeyInput( ioBaseChar *pOwner )
{
	if( !pOwner ) return;
	if( !pOwner->IsNeedProcess() ) return;

	ioUserKeyInput::DirKeyInput eNewDirKey = pOwner->CheckCurDirKey();
	if( eNewDirKey != m_CurDirKey )
	{
		m_bTargetRot = false;
		m_CurDirKey = eNewDirKey;

		if( pOwner->IsNeedSendNetwork() )
		{
			SP2Packet kPacket( CUPK_EXTEND_DASH );
			kPacket << pOwner->GetCharName();
			kPacket << (int)GetType();
			kPacket << pOwner->GetHP().m_fCurValue;
			kPacket << DST_KEY;
			kPacket << pOwner->GetTargetRot();
			kPacket << (int)m_CurDirKey;
			P2PNetwork::SendToAllPlayingUser( kPacket );
		}
	}
}

void ioDashAttack2::ProcessRotate( ioBaseChar *pOwner )
{
	if( !pOwner ) return;
	if( m_dwRotateSpeed <= 0 ) return;
	if( m_bTargetRot ) return;

	if( m_CurDirKey == ioUserKeyInput::DKI_NONE )
		return;

	if( m_bTargetRot )
		return;

	float fFrameGap = g_FrameTimer.GetCurFrameGap();
	float fRate = fFrameGap / m_dwRotateSpeed;
	float fRotateAngle = 360.0f * fRate;

	D3DXQUATERNION qtCurRot = pOwner->GetTargetRot();
	D3DXVECTOR3 vCurMoveDir = qtCurRot * D3DXVECTOR3(0.0f, 0.0f, -FLOAT1);
	D3DXVec3Normalize( &vCurMoveDir, &vCurMoveDir );

	// 목표에 도달체크 및 처리
	float fYawD = 180.0f + 45.0f * ( m_CurDirKey - ioUserKeyInput::DKI_UP );
	fYawD = ioMath::ArrangeHead( fYawD );

	float fYawR = DEGtoRAD( fYawD );
	D3DXQUATERNION qtRotate;
	D3DXQuaternionRotationYawPitchRoll( &qtRotate, fYawR, 0.0f, 0.0f );

	D3DXVECTOR3 vTargetDir = qtRotate * D3DXVECTOR3(0.0f, 0.0f, -FLOAT1);
	D3DXVec3Normalize( &vTargetDir, &vTargetDir );

	float fDotValue = D3DXVec3Dot( &vCurMoveDir, &vTargetDir );
	float fGapAngle = RADtoDEG( acosf(fDotValue) );

	if( fDotValue >= 0.98f )
	{
		pOwner->SetTargetRotToRotate( qtRotate, true );
		return;
	}

	D3DXVECTOR3 vAxis;
	if( fDotValue <= -0.98f )
	{
		vAxis = ioMath::UNIT_Y;
	}
	else
	{
		D3DXVec3Cross( &vAxis, &vTargetDir, &vCurMoveDir );
		D3DXVec3Normalize( &vAxis, &-vAxis );
	}

	D3DXQUATERNION qtNewRot;
	if( fGapAngle < fRotateAngle )
		D3DXQuaternionRotationAxis( &qtNewRot, &vAxis, DEGtoRAD(fGapAngle) );
	else
		D3DXQuaternionRotationAxis( &qtNewRot, &vAxis, DEGtoRAD(fRotateAngle) );

	D3DXVECTOR3 vNewDir = qtNewRot * vCurMoveDir;
	D3DXVec3Normalize( &vNewDir, &vNewDir );

	ioMath::CalcDirectionQuaternion( qtNewRot, -vNewDir );
	pOwner->SetTargetRotToRotate( qtNewRot, true );
}

void ioDashAttack2::ApplyDashState( ioBaseChar *pOwner, SP2Packet &rkPacket )
{
	if( !pOwner )
		return;

	int iState;
	rkPacket >> iState;
	rkPacket >> m_iCurComboCnt;

	D3DXQUATERNION qtRot;
	rkPacket >> qtRot;

	pOwner->SetTargetRotToRotate( qtRot, true, false );

	switch( iState )
	{
	case DS_NONE:
		StartDash( pOwner );
		break;
	case DS_CHARGE:
		ChangeToCharging( pOwner );
		break;
	case DS_NORMAL_ATTACK:
		ChangeToNormalAttack( pOwner );
		break;
	case DS_COMBO_ATTACK:
		SetComboDash( pOwner );
		break;
	case DS_BUFF_ADD:
		SetBuffAddState( pOwner );
		break;
	case DS_EXTEND_COMBO:
		SetExtendComboState( pOwner );
		break;
	case DS_RUN_ROTATE:
		SetRunRotateState( pOwner, false );
		break;
	case DS_RUN_ROTATE_END:
		SetRunRotateStateEnd( pOwner );
		break;
	case DS_POWER_CHARGE:
		SetPowerChargeState( pOwner, false );
		break;
	case DS_POWER_CHARGE_ATTACK:
		{
			rkPacket >> m_fCurChargeRate;

			SetPowerChargeAttack( pOwner );
		}
		break;
	}
}

void ioDashAttack2::ApplyDashDirKey( ioBaseChar *pOwner, SP2Packet &rkPacket )
{
	if( !pOwner )
		return;

	D3DXQUATERNION qtRot;
	rkPacket >> qtRot;

	pOwner->SetTargetRotToRotate( qtRot, true, false );

	int iDir;
	rkPacket >> iDir;

	m_bTargetRot = false;
	m_CurDirKey = (ioUserKeyInput::DirKeyInput)iDir;
}

bool ioDashAttack2::CheckJumpRunRoateEnd( ioBaseChar *pOwner )
{
	if( !pOwner )
		return false;

	ioEntityGroup *pGrp = pOwner->GetGroup();
	if( !pGrp ) return false;

	if( m_RunRotateAttackEnd.m_AttackAnimation.IsEmpty() )
		return false;

	int iAniID = pGrp->GetAnimationIdx( m_RunRotateAttackEnd.m_AttackAnimation );

	std::list<float> vTimeList;
	pGrp->GetAnimationEventTimeList_e( iAniID, "input_jump_s", vTimeList );

	if( vTimeList.empty() )
		return false;

	DWORD dwCurTime = FRAMEGETTIME();
	std::list< float >::iterator iter = vTimeList.begin();
	while( iter != vTimeList.end() )
	{
		DWORD dwTime = (DWORD)*iter;
		dwTime *= m_RunRotateAttackEnd.m_fAttackAniRate;

		dwTime += m_dwInputCancelCheckTime;

		if( m_dwCancelCheckTime > 0 && COMPARE( dwTime, m_dwCancelCheckTime, dwCurTime ) )
		{
			m_dwCancelCheckTime = dwTime+1;

			if( m_bEnableJumpState && !pOwner->IsBuffLimitJump() )
			{
				pOwner->SetJumpState();
				return true;
			}

			return false;
		}

		if( dwTime > dwCurTime )
			return false;

		iter++;
	}

	return false;
}


bool ioDashAttack2::CheckAniJump_Input_Jump_S( ioBaseChar *pOwner )
{
	if( !pOwner )
		return false;

	ioEntityGroup *pGrp = pOwner->GetGroup();
	if( !pGrp ) return false;

	if( m_stAniJump.IsEmpty() )
		return false;

	int iAniID = pGrp->GetAnimationIdx( m_stAniJump );

	std::list<float> vTimeList;
	pGrp->GetAnimationEventTimeList_e( iAniID, "input_jump_s", vTimeList );

	if( vTimeList.empty() )
		return false;

	DWORD dwCurTime = FRAMEGETTIME();
	std::list< float >::iterator iter = vTimeList.begin();
	while( iter != vTimeList.end() )
	{
		DWORD dwTime = (DWORD)*iter;
		dwTime *= m_fAniJumpRate;

		dwTime += m_dwActionStartTime;

		if( m_dwReserveEnableTime > 0 && COMPARE( dwTime, m_dwReserveEnableTime, dwCurTime ) )
		{
			if( m_bEnableJumpState && !pOwner->IsBuffLimitJump() )
			{
				pOwner->SetJumpState();
				return true;
			}

			return false;
		}

		if( dwTime > dwCurTime )
			return false;

		iter++;
	}

	return false;
}


bool ioDashAttack2::CheckPowerChargeState( ioBaseChar *pOwner )
{
	if( !pOwner )
		return false;

	if( m_DashExtendType != DET_POWER_CHARGE )
		return false;

	return true;
}

void ioDashAttack2::SetPowerChargeState( ioBaseChar *pOwner, bool bSendNet )
{
	if( !pOwner ) return;

	ioEntityGroup *pGrp = pOwner->GetGroup();
	if( !pGrp ) return;

	DWORD dwCurTime = FRAMEGETTIME();

	pGrp->ClearAllLoopAni( FLOAT100, true );
	pGrp->ClearAllActionAni( FLOAT100, true );

	pGrp->SetLoopAni( m_PowerChargeAni, FLOAT100 );

	m_DashState = DS_POWER_CHARGE;
	m_dwChargeStartTime = dwCurTime;
	m_fCurChargeRate = 0.0f;

	if( !m_DashGatheringEffect.IsEmpty() )
	{
		if( m_GatheringEffectID != -1 )
		{
			pOwner->EndEffect( m_GatheringEffectID, false );
			m_GatheringEffectID = -1;
		}

		ioEffect *pEffect = pOwner->AttachEffect( m_DashGatheringEffect );
		if( pEffect )
			m_GatheringEffectID = pEffect->GetUniqueID();
	}

	if( bSendNet && pOwner->IsNeedSendNetwork() )
	{
		SP2Packet kPacket( CUPK_EXTEND_DASH );
		kPacket << pOwner->GetCharName();
		kPacket << (int)GetType();
		kPacket << pOwner->GetHP().m_fCurValue;
		kPacket << DST_STATE;
		kPacket << DS_POWER_CHARGE;
		kPacket << m_iCurComboCnt;
		kPacket << pOwner->GetTargetRot();
		P2PNetwork::SendToAllPlayingUser( kPacket );
	}
}

void ioDashAttack2::SetPowerChargeAttack( ioBaseChar *pOwner )
{
	if( !pOwner ) return;

	ioEntityGroup *pGrp = pOwner->GetGroup();
	if( !pGrp ) return;

	DWORD dwCurTime = FRAMEGETTIME();

	m_dwActionEndTime = 0;
	m_dwReserveEnableTime = 0;
	m_dwPreReserveEnableTime = 0;

	m_iSkillInput = -1;
	m_bEnableDefenseState = false;
	m_bEnableAttackState = false;
	m_bEnableJumpState = false;

	m_DashState = DS_POWER_CHARGE_ATTACK;

	m_dwInputCancelCheckTime = dwCurTime;
	m_dwCancelCheckTime = dwCurTime;

	pOwner->ClearAttackFireTimeAndSkill();
	pOwner->ClearReservedSliding();
	pOwner->SetNoWoundState( false );

	if( pOwner->IsNeedProcess() )
	{
		DWORD dwGapTime = dwCurTime - m_dwChargeStartTime;
		if( m_dwChargeStartTime > 0 && m_dwMaxChargeTime > 0 )
		{
			if( m_dwMaxChargeTime > dwGapTime )
				m_fCurChargeRate = (float)dwGapTime / m_dwMaxChargeTime;
			else
				m_fCurChargeRate = FLOAT1;

			m_fCurChargeRate = min( m_fCurChargeRate, FLOAT1 );
		}
	}

	float fExtraAniRate, fExtraForceRate;
	fExtraAniRate = fExtraForceRate = FLOAT1;

	if( m_fMaxChargeAniRate > 0.0f )
	{
		float fCurMaxAniRate = max( 0.0f, (m_fMaxChargeAniRate - FLOAT1) );
		fExtraAniRate += fCurMaxAniRate * m_fCurChargeRate;
	}

	if( m_fMaxForceSlidingRate > 0.0f )
	{
		float fCurMaxSlidingRate = max( 0.0f, (m_fMaxForceSlidingRate - FLOAT1) );
		fExtraForceRate += fCurMaxSlidingRate * m_fCurChargeRate;
	}

	pGrp->ClearAllActionAni( FLOAT100, true );
	pGrp->ClearAllLoopAni( FLOAT100, true );

	int iAniID = pGrp->GetAnimationIdx( m_PowerChargeAttack.m_AttackAnimation );
	DWORD dwPreDelay = m_PowerChargeAttack.m_dwPreDelay;
	float fTimeRate = m_PowerChargeAttack.m_fAttackAniRate * fExtraAniRate;

	pOwner->SetAutoTarget( ATT_DASH );

	pOwner->RefreshFireTimeList( iAniID, m_PowerChargeAttack.m_vWeaponInfoList, FTT_EXTEND_DASH_ATTACK, fTimeRate, dwPreDelay );
	pOwner->IncreaseWeaponIndexBase();

	pGrp->SetActionAni( iAniID, FLOAT100, FLOAT100, FLOAT1, FLOAT1/fTimeRate, 0.0f, 0.0f, true );

	DWORD dwDuration = dwPreDelay + pGrp->GetAnimationFullTime( iAniID ) * fTimeRate + m_PowerChargeAttack.m_dwEndDelay;
	m_dwActionEndTime = dwCurTime + dwDuration;

	CheckKeyReserveTime( pOwner, iAniID, fTimeRate );

	pOwner->SetReservedSliding( m_PowerChargeAttack.m_vForceInfoList, iAniID, fTimeRate, dwPreDelay, fExtraForceRate );
	pOwner->CheckCharColSkipTime( iAniID, fTimeRate, dwPreDelay );

	DWORD dwTrackingTime = dwCurTime + dwPreDelay;
	dwTrackingTime += pGrp->GetAnimationEventTime_e( iAniID, "tracking_e" ) * fTimeRate;
	pOwner->SetAutoTargetTrackingEndTime( dwTrackingTime );

	pOwner->InitExtendAttackTagInfo();
	pOwner->SetBaseAttackExtraAniJump( iAniID, fTimeRate, dwPreDelay );
	pOwner->SetExtendAttackEndJumpInfo( iAniID, fTimeRate, dwPreDelay );

	if( !m_DashGatheringEffect.IsEmpty() )
	{
		if( m_GatheringEffectID != -1 )
		{
			pOwner->EndEffect( m_GatheringEffectID, false );
			m_GatheringEffectID = -1;
		}
	}

	if( pOwner->IsNeedSendNetwork() )
	{
		SP2Packet kPacket( CUPK_EXTEND_DASH );
		kPacket << pOwner->GetCharName();
		kPacket << (int)GetType();
		kPacket << pOwner->GetHP().m_fCurValue;
		kPacket << DST_STATE;
		kPacket << DS_POWER_CHARGE_ATTACK;
		kPacket << m_iCurComboCnt;
		kPacket << pOwner->GetTargetRot();
		kPacket << m_fCurChargeRate;
		P2PNetwork::SendToAllPlayingUser( kPacket );
	}
}

void ioDashAttack2::ProcessPowerCharge( ioBaseChar *pOwner )
{
	if( !pOwner )
		return;

	if( pOwner->IsNeedProcess() )
	{
		if( !pOwner->IsAttackKeyDown() )
		{
			SetPowerChargeAttack( pOwner );
			return;
		}
	}

	CheckKeyInput( pOwner );
	ProcessRotate( pOwner );
}

void ioDashAttack2::ProcessPowerChargeAttack( ioBaseChar *pOwner )
{
	if( !pOwner )
		return;

	DWORD dwCurTime = FRAMEGETTIME();
	DWORD dwEndTime = m_dwActionEndTime + pOwner->GetActionStopTotalDelay();

	if( dwEndTime > dwCurTime )
	{
		pOwner->CheckExtraAniJump();

		if( pOwner->IsNeedProcess() )
		{
			if( pOwner->CheckExtendAttackEndJump() )
				return;
		}

		if( m_dwReserveEnableTime > 0 && m_dwReserveEnableTime + pOwner->GetActionStopTotalDelay() < dwCurTime )
		{
			if( pOwner->IsAttackKey() && !m_ExtendAttackList.empty() )
			{
				m_iCurComboCnt = 0;
				SetExtendComboState( pOwner );
				return;
			}
			else
			{
				CheckExtraReserve( pOwner );
				return;
			}
		}
	}
	else
	{
		// 공격키가 눌러져 있으면 공격 상태 처리
		if( pOwner->IsAttackKeyDown() && 
			!pOwner->IsJumpKeyDown() &&
			!pOwner->IsDefenseKeyDown() &&
			!pOwner->IsChatModeState(true) )
		{
			if( !m_ExtendAttackList.empty() )
			{
				m_iCurComboCnt = 0;
				SetExtendComboState( pOwner );
			}
			else if( pOwner->IsCanNormalAttack() && !pOwner->IsBuffLimitAttack() )
			{
				pOwner->SetNormalAttack( 0 );
			}

			return;
		}

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
			if( !m_ExtendAttackList.empty() )
			{
				m_iCurComboCnt = 0;
				SetExtendComboState( pOwner );
			}
			else if( pOwner->IsCanNormalAttack() && !pOwner->IsBuffLimitAttack() )
			{
				pOwner->SetNormalAttack( 0 );
			}

			return;
		}

		// 대기
		pOwner->ClearAttackFireTimeAndSkill();
		pOwner->ClearReservedSliding();

		pOwner->SetState( CS_DELAY );
	}
}

void ioDashAttack2::CheckChargeInfo( ioWeapon *pWeapon )
{
	if( !pWeapon ) return;

	float fCurDamageRate, fCurForceRate, fCurRangeRate, fCurSpeedRate, fCurBlowRate, fCurFloatRate;
	float fCurScaleRate, fCurExplosionRate, fCurWoundedTimeRate, fCurLiveTimeRate;

	fCurDamageRate = fCurForceRate = fCurRangeRate = fCurSpeedRate = fCurBlowRate = fCurFloatRate = FLOAT1;
	fCurScaleRate = fCurExplosionRate = fCurWoundedTimeRate = fCurLiveTimeRate = FLOAT1;

	if( m_fCurChargeRate > 0.0f )
	{
		float fCurMaxDamage = max( 0.0f, (m_fMaxDamageRate - FLOAT1) );
		float fCurMaxForce = max( 0.0f, (m_fMaxForceRate - FLOAT1) );
		float fCurMaxBlow = max( 0.0f, (m_fMaxBlowRate - FLOAT1) );

		float fCurMaxRange = max( 0.0f, (m_fMaxRangeRate - FLOAT1) );
		float fCurMaxSpeed = max( 0.0f, (m_fMaxSpeedRate - FLOAT1) );

		float fCurMaxFloat = max( 0.0f, (m_fMaxFloatRate - FLOAT1) );

		float fCurMaxScale = max( 0.0f, (m_fMaxScaleRate - FLOAT1) );
		float fCurMaxExplosion = max( 0.0f, (m_fMaxExplosionRate - FLOAT1) );

		float fCurMaxWoundedTime = max( 0.0f, (m_fMaxWoundedTimeRate - FLOAT1) );
		float fCurMaxLiveTime = max( 0.0f, (m_fMaxLiveTimeRate - FLOAT1) );

		fCurDamageRate += fCurMaxDamage * m_fCurChargeRate;
		fCurForceRate += fCurMaxForce * m_fCurChargeRate;
		fCurBlowRate += fCurMaxBlow * m_fCurChargeRate;

		fCurRangeRate += fCurMaxRange * m_fCurChargeRate;
		fCurSpeedRate += fCurMaxSpeed * m_fCurChargeRate;

		fCurFloatRate += fCurMaxFloat * m_fCurChargeRate;

		fCurScaleRate += fCurMaxScale * m_fCurChargeRate;
		fCurExplosionRate += fCurMaxExplosion * m_fCurChargeRate;

		fCurWoundedTimeRate += fCurMaxWoundedTime * m_fCurChargeRate;
		fCurLiveTimeRate += fCurMaxLiveTime * m_fCurChargeRate;

		pWeapon->SetOwnerChargeDamageRate( fCurDamageRate );
		pWeapon->SetOwnerChargeForceRate( fCurForceRate );
		pWeapon->SetOwnerChargeBlowRate( fCurBlowRate );

		pWeapon->SetOwnerChargeRangeRate( fCurRangeRate );
		pWeapon->SetOwnerChargeSpeedRate( fCurSpeedRate );

		pWeapon->SetOwnerChargeFloatRate( fCurFloatRate );

		pWeapon->SetOwnerChargeScaleRate( fCurScaleRate );
		pWeapon->SetOwnerChargeExplosionRate( fCurExplosionRate );

		pWeapon->SetOwnerChargeWoundedTimeRate( fCurWoundedTimeRate );
		pWeapon->SetOwnerChargeLiveTimeRate( fCurLiveTimeRate );
	}
}

bool ioDashAttack2::GetAutoTargetValue( IN float &fDashRange, IN float &fDashMinAngle, IN float &fDashMaxAngle,
											OUT float &fRange, OUT float &fMinAngle, OUT float &fMaxAngle )
{
	if( m_fCurChargeRate == 0.0f )
		return false;

	if( m_fMaxTargetRangeRate == 0.0f || m_fMaxTargetAngleRate == 0.0f )
		return false;

	if( m_DashState != DS_POWER_CHARGE_ATTACK )
		return false;

	float fCurTargetRangeRate, fCurTargetAngleRate;
	fCurTargetRangeRate = fCurTargetAngleRate = FLOAT1;

	float fCurMaxTargetRange = max( 0.0f, (m_fMaxTargetRangeRate - FLOAT1) );
	float fCurMaxTargetAngle = max( 0.0f, (m_fMaxTargetAngleRate - FLOAT1) );

	fCurTargetRangeRate += fCurMaxTargetRange * m_fCurChargeRate;
	fCurTargetAngleRate += fCurMaxTargetAngle * m_fCurChargeRate;

	fRange = fDashRange * fCurTargetRangeRate;
	fMinAngle = fDashMinAngle / fCurTargetAngleRate;
	fMaxAngle = fDashMaxAngle / fCurTargetAngleRate;

	return true;
}


bool ioDashAttack2::IsCanPlayWoundedAniFromExtendDash()
{
	if( m_bPlayDashExtendWoundAni )
		return m_DashState==DS_EXTEND_COMBO ? true : false; 

	return false;

}
bool ioDashAttack2::IsEnableDefense()
{
	DWORD dwCurTime = FRAMEGETTIME();

	switch( m_DashState )
	{
	case DS_POWER_CHARGE:
		if( m_bPowerChargeUseDefense )
			return true;
		break;
	}

	return false;
}

bool ioDashAttack2::IsPowerChargingState()
{
	switch( m_DashState )
	{
	case DS_POWER_CHARGE:
		return true;
	}

	return false;
}


void ioDashAttack2::SetRunRotateBuff( ioBaseChar *pOwner )
{
	if( !pOwner )
		return;

	int iBuffCnt = m_vBuffAddList.size();
	for( int i=0; i < iBuffCnt; ++i )
	{
		ioHashString szBuffName = m_vBuffAddList[i];
		pOwner->AddNewBuff( szBuffName, pOwner->GetCharName(), "", NULL );
	}
}

void ioDashAttack2::RemoveRundRotateBuff( ioBaseChar *pOwner )
{
	if( !pOwner )
		return;

	int iBuffCnt = m_vBuffAddList.size();
	for( int i=0; i < iBuffCnt; ++i )
	{
		ioHashString szBuffName = m_vBuffAddList[i];
		pOwner->RemoveBuff( szBuffName );
	}
}

