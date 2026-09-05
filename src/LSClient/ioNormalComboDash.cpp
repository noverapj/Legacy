

#include "stdafx.h"

#include "ioNormalComboDash.h"
#include "ItemDefine.h"
#include "WeaponDefine.h"

ioNormalComboDash::ioNormalComboDash()
{
	m_dwActionEndTime = 0;
	m_dwPreReserveEnableTime = 0;

	m_dwReserveEnableTime = 0;

	m_iSkillInput = -1;
	m_bEnableDefenseState = false;
	m_bEnableAttackState = false;
	m_bEnableJumpState = false;

	m_iCurComboCnt = 0;
	m_dwChargingStartTime = 0;
	m_DashState = DS_NONE;

	m_dwCancelCheckTime = 0;
	m_dwInputCancelCheckTime = 0;

	m_iCurTimeChargeCnt = 0;

	m_szCurTimeChargeEffect.Clear();

	m_stAniJump.Clear();
	m_dwEnableReaperExpansionTeleportTime = 0;
}

ioNormalComboDash::ioNormalComboDash( const ioNormalComboDash &rhs )
: ioExtendDash( rhs ),
m_DashExtendType( rhs.m_DashExtendType ),
m_iMaxComboCnt( rhs.m_iMaxComboCnt ),
m_dwChargingTime( rhs.m_dwChargingTime ),
m_ChargingAniList( rhs.m_ChargingAniList ),
m_NormalAttackList( rhs.m_NormalAttackList ),
m_ExtendAttackList( rhs.m_ExtendAttackList ),
m_TimeChargeAttackList( rhs.m_TimeChargeAttackList ),
m_bUseTimeChargeAttack( rhs.m_bUseTimeChargeAttack ),
m_vTimeChargeAttackTime( rhs.m_vTimeChargeAttackTime ),
m_vTimeChargeEffect( rhs.m_vTimeChargeEffect ),
m_dwReaperExpansionTeleportTicTime( rhs.m_dwReaperExpansionTeleportTicTime )
{
	m_dwActionEndTime = 0;
	m_dwPreReserveEnableTime = 0;

	m_dwReserveEnableTime = 0;

	m_iSkillInput = -1;
	m_bEnableDefenseState = false;
	m_bEnableAttackState = false;
	m_bEnableJumpState = false;
	
	m_iCurComboCnt = 0;
	m_dwChargingStartTime = 0;
	m_DashState = DS_NONE;

	m_dwCancelCheckTime = 0;
	m_dwInputCancelCheckTime = 0;

	m_iCurTimeChargeCnt = 0;

	m_szCurTimeChargeEffect.Clear();

	m_stAniJump.Clear();
	m_dwEnableReaperExpansionTeleportTime = 0;
}

ioExtendDash* ioNormalComboDash::Clone()
{
	return new ioNormalComboDash( *this );
}

ioNormalComboDash::~ioNormalComboDash()
{
}

DashType ioNormalComboDash::GetType() const
{
	return DT_NORMAL_COMBO;
}

void ioNormalComboDash::LoadProperty( ioINILoader &rkLoader )
{
	ioExtendDash::LoadProperty( rkLoader );

	char szKey[MAX_PATH];
	char szBuf[MAX_PATH];

	m_DashExtendType = (DashExtendType)rkLoader.LoadInt_e( "dash_extend_type", DET_NONE );

	m_dwChargingTime = rkLoader.LoadInt_e( "dash_charge_time", 0 );

	m_iMaxComboCnt = rkLoader.LoadInt_e( "dash_max_combo_cnt", 0 );

	for( int i=0; i < m_iMaxComboCnt; ++i )
	{
		wsprintf_e( szKey, "charging_dash_ani%d", i+1 );
		rkLoader.LoadString( szKey, "", szBuf, MAX_PATH );
		m_ChargingAniList.push_back( szBuf );
	}

	// normal combo
	for( int i=0; i < m_iMaxComboCnt; ++i )
	{
		AttackAttribute eAttribute;

		wsprintf_e( szKey, "normal_dash_attack%d", i+1);
		LoadAttackAttribute( szKey, eAttribute, rkLoader );

		m_NormalAttackList.push_back( eAttribute );
	}

	// Extend combo
	for( int i=0; i < m_iMaxComboCnt; ++i )
	{
		AttackAttribute eAttribute;

		wsprintf_e( szKey, "extend_dash_attack%d", i+1);
		LoadAttackAttribute( szKey, eAttribute, rkLoader );

		m_ExtendAttackList.push_back( eAttribute );
	}

	m_bUseTimeChargeAttack = rkLoader.LoadBool_e( "use_time_charge_attack", false );

	// time charge dash attack
	int iCnt = rkLoader.LoadInt_e( "time_charge_dash_attack_cnt", 0 );
	for( int i=0; i < iCnt; ++i )
	{
		wsprintf_e( szKey, "time_charge_dash_attack%d_enable_time", i+1);
		DWORD dwTime = rkLoader.LoadInt( szKey, 0 );
		m_vTimeChargeAttackTime.push_back( dwTime );

		wsprintf_e( szKey, "time_charge_dash_attack%d", i+1);
		AttackAttribute eAttribute;
		LoadAttackAttribute( szKey, eAttribute, rkLoader );

		m_TimeChargeAttackList.push_back( eAttribute );

		wsprintf_e( szKey, "time_charge_dash_effect%d", i+1);
		rkLoader.LoadString( szKey, "", szBuf, MAX_PATH );
		m_vTimeChargeEffect.push_back( ioHashString(szBuf) );
	}

	m_dwReaperExpansionTeleportTicTime = (DWORD)rkLoader.LoadInt_e( "reaper_dash_teleport_tic_time", 0 );
}	

bool ioNormalComboDash::StartDash( ioBaseChar *pOwner )
{
	if( !pOwner )
		return false;

	ioExtendDash::StartDash( pOwner );

	m_iCurComboCnt = 0;
	m_dwChargingStartTime = 0;

	m_dwActionEndTime = 0;
	m_dwPreReserveEnableTime = 0;

	m_DashState = DS_NONE;

	m_dwInputCancelCheckTime = 0;
	m_dwCancelCheckTime = 0;

	pOwner->InitExtendAttackTagInfo();
	pOwner->SetNoWoundState( false );
	pOwner->SetState( CS_EXTEND_DASH );

	m_dwEnableReaperExpansionTeleportTime = 0;

	ChangeToCharging( pOwner );

	if( pOwner->IsNeedSendNetwork() )
	{
		SP2Packet kPacket( CUPK_EXTEND_DASH );
		kPacket << pOwner->GetCharName();
		kPacket << (int)GetType();
		kPacket << pOwner->GetHP().m_fCurValue;
		kPacket << (int)DS_NONE;
		kPacket << m_iCurComboCnt;
		kPacket << pOwner->GetTargetRot();
		P2PNetwork::SendToAllPlayingUser( kPacket );
	}

	return true;
}

void ioNormalComboDash::SetExtraInfoToWeapon( ioBaseChar *pOwner, ioPlayStage *pStage, ioWeapon *pWeapon, const ioHashString &szName )
{
	if( !pWeapon ) return;

	switch( pWeapon->GetType() )
	{
	case ioWeapon::WT_ZONE_EFFECT:
		{
			ioZoneEffectWeapon *pZone = ToZoneEffectWeapon( pWeapon );
			ioMissileWeapon *pMissleWeapon = ToMissileWeapon( pWeapon );
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
		break;
	case ioWeapon::WT_MISSILE:
		{
			ioMissileWeapon *pMissleWeapon = ToMissileWeapon( pWeapon );
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
		break;
	case ioWeapon::WT_BOMB_MISSILE3:
		{
			ioBombMissileWeapon3* pBombMissileWeapon3 = ToBombMissileWeapon3( pWeapon );
			if( pBombMissileWeapon3 )
				pBombMissileWeapon3->SetDsiableChangeDir( false );
		}
		break;
	case ioWeapon::WT_ZONE_MOVE_WEAPON:
		{
			ioZoneMoveWeapon *pZoneMoveWeapon = ToZoneMoveWeapon( pWeapon );
			if( pZoneMoveWeapon )
			{
				ioBaseChar *pChar = pZoneMoveWeapon->GetOwner();
				if( pChar )
				{
					D3DXVECTOR3	vDir = pChar->GetTargetRot() * D3DXVECTOR3( 0.0f, 0.0f, -FLOAT1 );
					pWeapon->SetMoveDir( vDir );
				}
			}
		}
		break;
	}
}

void ioNormalComboDash::CheckProcessDash( ioBaseChar *pOwner )
{
	if( !pOwner )
		return;

	DWORD dwGap = FRAMEGETTIME() - pOwner->GetDashStartTime();
	int iAttackCnt = m_vTimeChargeAttackTime.size();
	for( int i=0; i<iAttackCnt; ++i )
	{
		if( dwGap < m_vTimeChargeAttackTime[i] )
		{
			m_iCurTimeChargeCnt = i;
			break;
		}
	}

	if( !COMPARE( m_iCurTimeChargeCnt, 0, (int)m_vTimeChargeEffect.size() ) )
		return;

	if( m_szCurTimeChargeEffect != m_vTimeChargeEffect[m_iCurTimeChargeCnt] )
	{
		if( !m_szCurTimeChargeEffect.IsEmpty() )
			pOwner->EndEffect( m_szCurTimeChargeEffect, false );

		m_szCurTimeChargeEffect = m_vTimeChargeEffect[m_iCurTimeChargeCnt];
		pOwner->AttachEffect( m_szCurTimeChargeEffect );
	}
}

void ioNormalComboDash::CheckEndDash( ioBaseChar *pOwner )
{
	if( !pOwner )
		return;

	if( !m_szCurTimeChargeEffect.IsEmpty() )
		pOwner->EndEffect( m_szCurTimeChargeEffect );
}

void ioNormalComboDash::CheckKeyReserve( ioBaseChar *pOwner )
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
		if( m_bUseTimeChargeAttack )
		{
			return;
		}

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
			kPacket << (int)DS_CHARGE;
			kPacket << m_iCurComboCnt;
			kPacket << pOwner->GetTargetRot();
			P2PNetwork::SendToAllPlayingUser( kPacket );
		}
		return;
	}
}

void ioNormalComboDash::CheckExtraReserve( ioBaseChar *pOwner )
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

bool ioNormalComboDash::CheckAniJump_Input_Jump_S( ioBaseChar *pOwner )
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

void ioNormalComboDash::ProcessDash( ioBaseChar *pOwner )
{
	DWORD dwCurTime = FRAMEGETTIME();

	switch( m_DashState )
	{
	case DS_CHARGE:
		{
			ProcessCharging( pOwner );
		}
		break;
	case DS_ATTACK_FIRE:
	case DS_TIME_CHARGE_ATTACK_FIRE:
		{
			ProcessAttackFireDash( pOwner );
			CheckAniJump_Input_Jump_S( pOwner );
		}
		break;
	}
}

void ioNormalComboDash::ApplyDash( ioBaseChar *pOwner, SP2Packet &rkPacket )
{
	if( !pOwner ) return;

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
	case DS_ATTACK_FIRE:
		{
			bool bExtend;
			rkPacket >> bExtend;

			ChangeToAttackFire( pOwner, bExtend );
		}
		break;
	case DS_TIME_CHARGE_ATTACK_FIRE:
		{
			rkPacket >> m_iCurTimeChargeCnt;
			ChangeToTimeChargeAttackFire( pOwner );
		}
	}
}

void ioNormalComboDash::ExtendDashEnd( ioBaseChar *pOwner )
{
	m_DashState = DS_NONE;

	m_iCurComboCnt = 0;
	m_dwChargingStartTime = 0;

	m_dwActionEndTime = 0;
	m_dwPreReserveEnableTime = 0;

	m_dwReserveEnableTime = 0;

	m_iSkillInput = -1;
	m_bEnableDefenseState = false;
	m_bEnableAttackState = false;
	m_bEnableJumpState = false;

	if( pOwner )
	{
		pOwner->InitExtendAttackTagInfo();
	}
}

void ioNormalComboDash::ChangeToCharging( ioBaseChar *pOwner )
{
	if( !pOwner )
		return;

	ioEntityGroup *pGrp = pOwner->GetGroup();
	if( !pGrp )
		return;

	if( m_bUseTimeChargeAttack )
	{
		pGrp->ClearAllLoopAni( FLOAT100, true );
		pGrp->ClearAllActionAni( FLOAT100, true );

		m_dwChargingStartTime = FRAMEGETTIME();

		m_dwActionEndTime = 0;
		m_dwPreReserveEnableTime = 0;

		m_dwReserveEnableTime = 0;

		m_iSkillInput = -1;
		m_bEnableDefenseState = false;
		m_bEnableAttackState = false;
		m_bEnableJumpState = false;

		m_DashState = DS_CHARGE;
	}
	else if( !m_ChargingAniList.empty() )
	{
		ioHashString szCurChargingAni;
		if( COMPARE( m_iCurComboCnt, 0, m_iMaxComboCnt ) )
			szCurChargingAni = m_ChargingAniList[m_iCurComboCnt];
		else
			szCurChargingAni = m_ChargingAniList[0];

		pGrp->ClearAllLoopAni( FLOAT100, true );
		pGrp->ClearAllActionAni( FLOAT100, true );

		pGrp->SetLoopAni( szCurChargingAni, FLOAT100 );

		m_dwChargingStartTime = FRAMEGETTIME();

		m_dwActionEndTime = 0;
		m_dwPreReserveEnableTime = 0;

		m_dwReserveEnableTime = 0;

		m_iSkillInput = -1;
		m_bEnableDefenseState = false;
		m_bEnableAttackState = false;
		m_bEnableJumpState = false;

		m_DashState = DS_CHARGE;
	}
}

void ioNormalComboDash::ChangeToAttackFire( ioBaseChar *pOwner, bool bExtend )
{
	if( !pOwner || m_iMaxComboCnt <= 0 )
		return;

	int iCurIndex = m_iCurComboCnt;
	if( !COMPARE( iCurIndex, 0, m_iMaxComboCnt ) )
		iCurIndex = 0;

	AttackAttribute kAttackInfo = m_NormalAttackList[iCurIndex];
	if( bExtend )
		kAttackInfo = m_ExtendAttackList[iCurIndex];

	m_stAniJump = kAttackInfo.m_AttackAnimation;
	m_fAniJumpRate = kAttackInfo.m_fAttackAniRate;
	m_dwActionStartTime = FRAMEGETTIME();

	ioEntityGroup *pGrp = pOwner->GetGroup();
	int iAniID = pGrp->GetAnimationIdx( kAttackInfo.m_AttackAnimation );
	if( iAniID == -1 )
	{
		LOG.PrintTimeAndLog( 0, "ioNormalComboDash::ChangeToAttackFire - %s Animation is Not Exist",
								kAttackInfo.m_AttackAnimation.c_str() );
		return;
	}

	float fTimeRate = kAttackInfo.m_fAttackAniRate;
	if( fTimeRate <= 0.0f )
		fTimeRate = FLOAT1;

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
								 kAttackInfo.m_vWeaponInfoList,
								 FTT_EXTEND_DASH_ATTACK,
								 fTimeRate,
								 kAttackInfo.m_dwPreDelay );

	pOwner->IncreaseWeaponIndexBase();
	pOwner->SetNoWoundState( false );

	pOwner->SetState( CS_EXTEND_DASH );
	pOwner->SetAutoTarget( ATT_DASH );
	pGrp->SetActionAni( iAniID,	FLOAT100, FLOAT100, FLOAT1, FLOAT1/fTimeRate, 0.0f, (float)kAttackInfo.m_dwPreDelay, true );

	DWORD dwCurTime = FRAMEGETTIME();
	DWORD dwDuration = kAttackInfo.m_dwPreDelay + pGrp->GetAnimationFullTime( iAniID ) * fTimeRate + kAttackInfo.m_dwEndDelay;

	m_dwActionEndTime = dwCurTime + dwDuration;

	CheckKeyReserveTime( pOwner, iAniID, fTimeRate );

	pOwner->SetReservedSliding( kAttackInfo.m_vForceInfoList, iAniID, fTimeRate, kAttackInfo.m_dwPreDelay );
	pOwner->CheckCharColSkipTime( iAniID, fTimeRate, kAttackInfo.m_dwPreDelay );

	DWORD dwTrackingTime = FRAMEGETTIME() + kAttackInfo.m_dwPreDelay;
	dwTrackingTime += pGrp->GetAnimationEventTime_e( iAniID, "tracking_e" ) * fTimeRate;
	pOwner->SetAutoTargetTrackingEndTime( dwTrackingTime );

	m_DashState = DS_ATTACK_FIRE;

	if ( m_DashExtendType == DET_REAPER_EXPANSION )
	{
		if ( m_dwReaperExpansionTeleportTicTime > 0 )
			m_dwEnableReaperExpansionTeleportTime = dwCurTime + kAttackInfo.m_dwPreDelay + m_dwReaperExpansionTeleportTicTime;
	}

	if( pOwner->IsNeedSendNetwork() )
	{
		SP2Packet kPacket( CUPK_EXTEND_DASH );
		kPacket << pOwner->GetCharName();
		kPacket << (int)GetType();
		kPacket << pOwner->GetHP().m_fCurValue;
		kPacket << (int)DS_ATTACK_FIRE;
		kPacket << m_iCurComboCnt;
		kPacket << pOwner->GetTargetRot();
		kPacket << bExtend;
		P2PNetwork::SendToAllPlayingUser( kPacket );
	}

	// after send
	m_iCurComboCnt++;
}

void ioNormalComboDash::ChangeToTimeChargeAttackFire( ioBaseChar *pOwner )
{
	if( !pOwner )
		return;

	int iAttackCnt = m_vTimeChargeAttackTime.size();
	if( !COMPARE( m_iCurTimeChargeCnt, 0, iAttackCnt ) )
		return;

	if( !m_szCurTimeChargeEffect.IsEmpty() )
		pOwner->EndEffect( m_szCurTimeChargeEffect, false );

	AttackAttribute kAttackInfo = m_TimeChargeAttackList[m_iCurTimeChargeCnt];

	ioEntityGroup *pGrp = pOwner->GetGroup();
	if( !pGrp )
		return;

	int iAniID = pGrp->GetAnimationIdx( kAttackInfo.m_AttackAnimation );
	if( iAniID == -1 )
	{
		LOG.PrintTimeAndLog( 0, "ioNormalComboDash::ChangeToTimeChargeAttackFire - %s Animation is Not Exist",
			kAttackInfo.m_AttackAnimation.c_str() );
		return;
	}

	m_stAniJump = kAttackInfo.m_AttackAnimation;
	m_fAniJumpRate = kAttackInfo.m_fAttackAniRate;
	m_dwActionStartTime = FRAMEGETTIME();

	float fTimeRate = kAttackInfo.m_fAttackAniRate;
	if( fTimeRate <= 0.0f )
		fTimeRate = FLOAT1;

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
		kAttackInfo.m_vWeaponInfoList,
		FTT_EXTEND_DASH_ATTACK,
		fTimeRate,
		kAttackInfo.m_dwPreDelay );

	pOwner->IncreaseWeaponIndexBase();
	pOwner->SetNoWoundState( false );

	pOwner->SetState( CS_EXTEND_DASH );
	pOwner->SetAutoTarget( ATT_DASH );
	pGrp->SetActionAni( iAniID,	FLOAT100, FLOAT100, FLOAT1, FLOAT1/fTimeRate, 0.0f, (float)kAttackInfo.m_dwPreDelay, true );

	DWORD dwCurTime = FRAMEGETTIME();
	DWORD dwDuration = kAttackInfo.m_dwPreDelay + pGrp->GetAnimationFullTime( iAniID ) * fTimeRate + kAttackInfo.m_dwEndDelay;

	m_dwActionEndTime = dwCurTime + dwDuration;

	CheckKeyReserveTime( pOwner, iAniID, fTimeRate );

	pOwner->SetReservedSliding( kAttackInfo.m_vForceInfoList, iAniID, fTimeRate, kAttackInfo.m_dwPreDelay );
	pOwner->CheckCharColSkipTime( iAniID, fTimeRate, kAttackInfo.m_dwPreDelay );

	DWORD dwTrackingTime = FRAMEGETTIME() + kAttackInfo.m_dwPreDelay;
	dwTrackingTime += pGrp->GetAnimationEventTime_e( iAniID, "tracking_e" ) * fTimeRate;
	pOwner->SetAutoTargetTrackingEndTime( dwTrackingTime );

	m_DashState = DS_ATTACK_FIRE;

	if( pOwner->IsNeedSendNetwork() )
	{
		SP2Packet kPacket( CUPK_EXTEND_DASH );
		kPacket << pOwner->GetCharName();
		kPacket << (int)GetType();
		kPacket << pOwner->GetHP().m_fCurValue;
		kPacket << (int)DS_TIME_CHARGE_ATTACK_FIRE;
		kPacket << m_iCurComboCnt;
		kPacket << pOwner->GetTargetRot();
		kPacket << m_iCurTimeChargeCnt;
		P2PNetwork::SendToAllPlayingUser( kPacket );
	}

	m_iCurTimeChargeCnt = 0;
	m_szCurTimeChargeEffect.Clear();
}

bool ioNormalComboDash::CheckEnableSurferState( ioBaseChar *pOwner )
{
	if( !pOwner || pOwner->IsHasCrown() )
		return false;

	if( m_DashExtendType != DET_SURFING )
		return false;

	ioSurferItem* pSurferItem = ToSurferItem( pOwner->GetPriorityItem( SPT_JUMP ) );
	if( !pSurferItem || !pSurferItem->IsEnableGauge() )
		return false;

	return true;
}

void ioNormalComboDash::ChangeToSurfingState( ioBaseChar *pOwner )
{
	if( !pOwner )
		return;

	if( !m_szCurTimeChargeEffect.IsEmpty() )
		pOwner->EndEffect( m_szCurTimeChargeEffect, false );

	ioSurferItem* pSurferItem = ToSurferItem( pOwner->GetPriorityItem( SPT_DASH ) );
	if( pSurferItem )
		pSurferItem->SetSurferState( pOwner );
}

void ioNormalComboDash::ProcessCharging( ioBaseChar *pOwner )
{
	if( !pOwner ) return;
	if( !pOwner->IsNeedProcess() )
		return;

	ioEntityGroup *pGrp = pOwner->GetGroup();
	if( !pGrp ) return;

	DWORD dwCurTime = FRAMEGETTIME();
	if( pOwner->IsAttackKeyDown() )
	{
		if( m_dwChargingStartTime+m_dwChargingTime < dwCurTime )
		{
			if( CheckEnableSurferState( pOwner ) )
			{
				ChangeToSurfingState( pOwner );
			}
			else if ( CheckExpansionDashChargeAttackState( pOwner ) )
				ChangeToExpansionDashChargeAttackState( pOwner );
			else
			{
				ChangeToAttackFire( pOwner, true );
			}
			return;
		}
	}
	else
	{
		if( m_bUseTimeChargeAttack )
		{
			ChangeToTimeChargeAttackFire( pOwner );
		}
		else
		{
			ChangeToAttackFire( pOwner, false );
		}
		return;
	}
}

void ioNormalComboDash::ProcessAttackFireDash( ioBaseChar *pOwner )
{
	DWORD dwCurTime = FRAMEGETTIME();
	DWORD dwEndTime = m_dwActionEndTime + pOwner->GetActionStopTotalDelay();

	if( dwEndTime > dwCurTime )
	{
		pOwner->CheckExtraAniJump();

		if ( CheckReaperExpansionTeleport( pOwner ) )
		{
			SetReaperExpansionTeleportState( pOwner );
			return;
		}

		if( m_dwReserveEnableTime > 0 && m_dwReserveEnableTime + pOwner->GetActionStopTotalDelay() < dwCurTime )
		{
			// 예약체크
			if( m_iCurComboCnt >= m_iMaxComboCnt )
			{
				CheckExtraReserve( pOwner );
			}
			else
			{
				CheckKeyReserve( pOwner );
			}

			return;
		}
	}
	else
	{
		ProcessKeyReserve( pOwner );
		return;
	}
}

bool ioNormalComboDash::IsEnableDefense()
{
	return false;
}

bool ioNormalComboDash::IsPowerChargingState()
{
	return false;
}

bool ioNormalComboDash::CheckExpansionDashChargeAttackState( ioBaseChar *pOwner )
{
	if( !pOwner || pOwner->IsHasCrown() )
		return false;

	if( m_DashExtendType != DET_REAPER_EXPANSION )
		return false;

	ioChargeContactWeaponItem *pChargeContactWeaponItem = ToChargeContactWeaponItem( pOwner->GetPriorityItem( SPT_ATTACK ) );
	if ( pChargeContactWeaponItem && pChargeContactWeaponItem->CheckExpansionReaperAttack( pOwner, ioChargeContactWeaponItem::ERCAT_JUMP ) )
		return true;

	return true;
}

void ioNormalComboDash::ChangeToExpansionDashChargeAttackState( ioBaseChar *pOwner )
{
	if( !pOwner || pOwner->IsHasCrown() )
		return;

	ioChargeContactWeaponItem *pChargeContactWeaponItem = ToChargeContactWeaponItem( pOwner->GetPriorityItem( SPT_ATTACK ) );
	if ( !pChargeContactWeaponItem )
		return;

	pChargeContactWeaponItem->ChangeToExpansionReaperAttack( pOwner, ioChargeContactWeaponItem::ERCAT_DASH );
}

bool ioNormalComboDash::CheckReaperExpansionTeleport( ioBaseChar *pOwner )
{
	if ( !pOwner || !pOwner->IsNeedProcess() )
		return false;

	if( m_DashExtendType != DET_REAPER_EXPANSION )
		return false;

	ioChargeContactWeaponItem *pChargeContactWeaponItem = ToChargeContactWeaponItem( pOwner->GetPriorityItem( SPT_ATTACK ) );
	if ( !pChargeContactWeaponItem )
		return false;

	if ( pChargeContactWeaponItem->GetCurBullet() < pChargeContactWeaponItem->GetNeedBullet() )
		return false;

	if ( m_dwEnableReaperExpansionTeleportTime <= 0)
		return false;

	if ( m_dwEnableReaperExpansionTeleportTime > FRAMEGETTIME() )
		return false;

	if ( !pOwner->IsDefenseKey() )
		return false;

	return true;
}

void ioNormalComboDash::SetReaperExpansionTeleportState( ioBaseChar *pOwner )
{
	if( !pOwner || pOwner->IsHasCrown() )
		return;

	ioChargeContactWeaponItem *pChargeContactWeaponItem = ToChargeContactWeaponItem( pOwner->GetPriorityItem( SPT_ATTACK ) );
	if ( !pChargeContactWeaponItem )
		return;

	m_dwEnableReaperExpansionTeleportTime = 0;

	pChargeContactWeaponItem->SetReaperExpansionTeleportState( pOwner );
}