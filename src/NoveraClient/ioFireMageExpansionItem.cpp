

#include "stdafx.h"

#include "ioBaseChar.h"
#include "ioPlayStage.h"

#include "ioFireMageExpansionItem.h"

#include "ioGrowthUpInfo.h"

#include "WeaponDefine.h"

ioFireMageExpansionItem::ioFireMageExpansionItem()
{
	m_dwTimeGap = 0;
	m_dwChargeStartTime = 0;
	m_iCurCombo = 0;
	m_FireState = FS_NONE;

	m_iCurAirJump = 0;
	m_fAirJumpPowerRate = 0.0f;
	m_vAirJumpTimeList.clear();
}

ioFireMageExpansionItem::ioFireMageExpansionItem( const ioFireMageExpansionItem &rhs )
	: ioWeaponItem( rhs ),
	m_PreGatherAnimation( rhs.m_PreGatherAnimation ),
	m_GatherAnimation( rhs.m_GatherAnimation ),
	m_GatherEffect( rhs.m_GatherEffect ),
	m_EndGatherAnimation( rhs.m_EndGatherAnimation ),
	m_fMoveSpeed( rhs.m_fMoveSpeed ),
	m_fRange( rhs.m_fRange ),
	m_fHeightGap( rhs.m_fHeightGap ),
	m_fGaugePerAllDefense( rhs.m_fGaugePerAllDefense ),
	m_dwDefenseTicTime( rhs.m_dwDefenseTicTime ),
	m_ReadyCircle( rhs.m_ReadyCircle ),
	m_EnableCircle( rhs.m_EnableCircle ),
	m_fMagicCircleRadius( rhs.m_fMagicCircleRadius ),
	m_fSkillGaugeRate( rhs.m_fSkillGaugeRate ),
	m_dwExpansionAttackTic( rhs.m_dwExpansionAttackTic ),
	m_dwExpansionAttackTicFirst( rhs.m_dwExpansionAttackTicFirst ),
	m_stExpansionAttackMotion( rhs.m_stExpansionAttackMotion ),
	m_fExpansionAttackMotionRate( rhs.m_fExpansionAttackMotionRate ),
	m_stExpansionAttackEffect_red( rhs.m_stExpansionAttackEffect_red ),
	m_stExpansionAttackEffect_blue( rhs.m_stExpansionAttackEffect_blue ),
	m_ExtraAttackInfo( rhs.m_ExtraAttackInfo ),
	m_fExpansionAttackHeightOffset( rhs.m_fExpansionAttackHeightOffset )
{
	m_dwTimeGap = 0;
	m_dwChargeStartTime = 0;
	m_iCurCombo = 0;
	m_FireState = FS_NONE;

	m_iCurAirJump = 0;
	m_fAirJumpPowerRate = 0.0f;
	m_vAirJumpTimeList.clear();
}

ioFireMageExpansionItem::~ioFireMageExpansionItem()
{
	m_vAirJumpTimeList.clear();
}

void ioFireMageExpansionItem::LoadProperty( ioINILoader &rkLoader )
{
	ioWeaponItem::LoadProperty( rkLoader );

	char szBuf[MAX_PATH];
	char szKey[MAX_PATH];

	rkLoader.LoadString_e( "pre_gather_animation", "", szBuf, MAX_PATH );
	m_PreGatherAnimation = szBuf;
	rkLoader.LoadString_e( "gather_animation", "", szBuf, MAX_PATH );
	m_GatherAnimation = szBuf;
	rkLoader.LoadString_e( "gather_effect", "", szBuf, MAX_PATH );
	m_GatherEffect = szBuf;
	rkLoader.LoadString_e( "end_gather_animation", "", szBuf, MAX_PATH );
	m_EndGatherAnimation = szBuf;

	rkLoader.LoadString_e( "ready_circle", "", szBuf, MAX_PATH );
	m_ReadyCircle = szBuf;
	rkLoader.LoadString_e( "enable_circle", "", szBuf, MAX_PATH );
	m_EnableCircle = szBuf;

	m_fMagicCircleRadius = rkLoader.LoadFloat_e( "magic_circle_radius", FLOAT100 );
	m_fRange = rkLoader.LoadFloat_e( "magic_circle_range", FLOAT1000 );
	m_fMoveSpeed = rkLoader.LoadFloat_e( "magic_circle_move_speed", 350.0f );
	m_fHeightGap = rkLoader.LoadFloat_e( "magic_circle_height_gap", 20.0f );

	m_fGaugePerAllDefense = rkLoader.LoadFloat_e( "gauge_per_defense", 5.0f );
	m_dwDefenseTicTime = rkLoader.LoadInt_e( "defense_tic_time", 500 );
	m_fSkillGaugeRate = rkLoader.LoadFloat_e( "skill_gauge_rate", FLOAT1 );

	//////////////////////////////////////////////////////////////////////////
	m_dwExpansionAttackTic = (DWORD)rkLoader.LoadInt_e( "charge_expansion_attack_tic", 10000 );
	m_dwExpansionAttackTicFirst = (DWORD)rkLoader.LoadInt_e( "charge_expansion_attack_tic_first", 10000 );

	rkLoader.LoadString_e( "charge_expansion_attack_ani", "", szBuf, MAX_PATH );
	m_stExpansionAttackMotion = szBuf;
	m_fExpansionAttackMotionRate = rkLoader.LoadFloat_e( "charge_expansion_attack_ani_rate", 350.0f );

	rkLoader.LoadString_e( "charge_expansion_attack_ani_land_red_effect", "", szBuf, MAX_PATH );
	m_stExpansionAttackEffect_red = szBuf;

	rkLoader.LoadString_e( "charge_expansion_attack_ani_land_blue_effect", "", szBuf, MAX_PATH );
	m_stExpansionAttackEffect_blue = szBuf;


	wsprintf_e( szKey, "charge_expansion_attack_type" );
	m_ExtraAttackInfo.m_iWeaponIdx = rkLoader.LoadInt( szKey, NO_WEAPON_ATTRIBUTE );

	wsprintf_e( szKey, "charge_expansion_attack_type_resist" );
	m_ExtraAttackInfo.m_iResistanceIdx = rkLoader.LoadInt( szKey, NO_WEAPON_ATTRIBUTE );

	wsprintf_e( szKey, "charge_expansion_attack_type_wounded" );
	rkLoader.LoadString( szKey, "", szBuf, MAX_PATH );
	m_ExtraAttackInfo.m_WoundedAnimation = szBuf;

	wsprintf_e( szKey, "charge_expansion_attack_type_wounded_time" );
	m_ExtraAttackInfo.m_dwWoundedDuration = rkLoader.LoadInt( szKey, 0 );

	wsprintf_e( szKey, "charge_expansion_attack_type_wounded_loop_ani" );
	m_ExtraAttackInfo.m_bWoundedLoopAni = rkLoader.LoadBool( szKey, false );

	m_fExpansionAttackHeightOffset = rkLoader.LoadFloat_e( "charge_expansion_attack_height_offset", 10000.f );
}

ioItem* ioFireMageExpansionItem::Clone()
{
	return new ioFireMageExpansionItem( *this );
}

void ioFireMageExpansionItem::SetNormalAttack( ioBaseChar *pOwner, int iCurCombo )
{
	if( !pOwner ) return;

	ClearCancelInfo();

	int iMaxCombo = GetMaxCombo();
	if( !COMPARE( iCurCombo, 0, iMaxCombo ) )
	{
		ioAttackableItem::SetNormalAttack( pOwner, iCurCombo );
		m_FireState = FS_NONE;
	}
	else
	{
		m_FireState = FS_CHARGING;
		m_dwChargeStartTime = FRAMEGETTIME();
		m_iCurCombo = iCurCombo;

		pOwner->SetState( CS_ATTACK );
		pOwner->SetChargingState( true );

		ioEntityGroup *pGrp = pOwner->GetGroup();
		pGrp->ClearAllActionAni( FLOAT100, true );
		pGrp->ClearAllLoopAni( FLOAT100, true );

		int iAniID = pGrp->GetAnimationIdx( m_PreGatherAnimation );
		float fTimeRate = (float)m_dwChargeTime / pGrp->GetAnimationFullTime( iAniID );
		pGrp->SetActionAni( iAniID, FLOAT100, FLOAT100, FLOAT1, FLOAT1/fTimeRate, 0.0f, 0.0f, true );

		m_dwNextExpansionFireTime = 0;
	}
}

void ioFireMageExpansionItem::ChangeToGathering( ioBaseChar *pOwner )
{
	pOwner->StopAutoTargetTracking();

	ioEntityGroup *pGrp = pOwner->GetGroup();
	pGrp->ClearAllActionAni( FLOAT100, true );
	pGrp->ClearAllLoopAni( FLOAT100, true );
	pGrp->SetLoopAni( m_GatherAnimation, FLOAT100 );
	pOwner->AttachEffect( m_GatherEffect );

	ioHashString szDummyCircle;
	pOwner->CreateMagicCircle( m_ReadyCircle, m_EnableCircle, szDummyCircle, m_fMagicCircleOffSet );

	m_FireState = FS_GATHERING;

	DWORD dwCurTime = FRAMEGETTIME();
	m_dwNextExpansionFireTime = dwCurTime + m_dwExpansionAttackTicFirst;
	m_dwExpansionFireMotionEndTime = 0;

	if( pOwner->IsNeedSendNetwork() )
	{
		SP2Packet kPacket( CUPK_ITEM_GAUGE_CANCEL );
		FillItemGaugePacketHeader( pOwner, kPacket );
		kPacket << (int)m_FireState;
		kPacket << m_iCurCombo;
		P2PNetwork::SendToAllPlayingUser( kPacket );
	}
}

void ioFireMageExpansionItem::CheckNormalAttack( ioBaseChar *pOwner )
{
	switch( m_FireState )
	{
	case FS_CHARGING:
		OnCharging( pOwner );
		break;
	case FS_NORMAL_ATTACK:
		{
			if( pOwner->IsNeedProcess() )
			{
				CheckCancelReserve( pOwner );

				if( ProcessCancel(pOwner) )
					return;
			}

			ioAttackableItem::CheckNormalAttack( pOwner );
		}
		break;
	case FS_GATHERING:
		OnGathering( pOwner );
		break;
	}
}

void ioFireMageExpansionItem::OnCharging( ioBaseChar *pOwner )
{
	if( !pOwner->IsNeedProcess() )
		return;

	if( pOwner->IsAttackKeyDown() )
	{
		if( m_dwChargeStartTime + m_dwChargeTime <= FRAMEGETTIME() )
		{
			pOwner->EndEffect( m_GatherEffect );
			ChangeToGathering( pOwner );
		}
	}
	else
	{
		pOwner->SetChargingState( false );

		ChangeToNormalAttack( pOwner );
	}
}

void ioFireMageExpansionItem::OnGathering( ioBaseChar *pOwner )
{
	if( !pOwner || !pOwner->IsNeedProcess() )
		return;

	if( !pOwner->IsAttackKeyDown() )	// ¹öÆ° ¶À
	{
		m_FireState = FS_NONE;

		pOwner->DestroyMagicCircle();
		pOwner->EndEffect( m_GatherEffect );
		pOwner->SetState( CS_DELAY );

		if( pOwner->IsNeedSendNetwork() )
		{
			SP2Packet kPacket( CUPK_ITEM_GAUGE_CANCEL );
			FillItemGaugePacketHeader( pOwner, kPacket );
			kPacket << (int)m_FireState;
			kPacket << m_iCurCombo;
			P2PNetwork::SendToAllPlayingUser( kPacket );
		}
		return;
	}

	float fCurRange = m_fRange;
	float fValue = 0.0f;
	ioGrowthNormalUpInfo *pUpInfo = ToNormalUpInfo( GetGrowthUpInfoByType(GT_MAGIC_CIRCLE_RANGE, pOwner->GetCharName()) );
	if( pUpInfo )
	{
		ioPlayStage *pStage = m_pCreator->GetPlayStage();
		fValue = pUpInfo->GetValue(pStage);
	}

	fCurRange += fValue;

	if( pOwner->IsDefenseKeyDown() )
	{
		if( pOwner->GetUseEnableSkill( 0, SUT_NORMAL ) )
		{
			pOwner->UpdateMagicCirclePos( m_fMoveSpeed, fCurRange, m_fHeightGap, true, true );
		}

		int iSkillNum = pOwner->GetSkillKeyInput();
		if( iSkillNum == 0 && !pOwner->IsBuffLimitSkill() && pOwner->SetUseSkill(0, SUT_NORMAL) )
		{
			m_FireState = FS_NONE;
			pOwner->EndEffect( m_GatherEffect );
			return;
		}
	}

	//Gather Gauge
	if( pOwner->CheckMagicCircle() )
	{
		if( pOwner->GetUseEnableSkill( 0, SUT_NORMAL ) )
			pOwner->UpdateMagicCirclePos( m_fMoveSpeed, fCurRange, m_fHeightGap, true );
		else
			pOwner->UpdateMagicCirclePos( m_fMoveSpeed, fCurRange, m_fHeightGap, false );
	}


	DWORD dwCurTime = FRAMEGETTIME();
	if( m_dwExpansionFireMotionEndTime && m_dwExpansionFireMotionEndTime < dwCurTime )
	{
		ioEntityGroup *pGrp = pOwner->GetGroup();
		if( pGrp )
		{
			pGrp->ClearAllActionAni( FLOAT100, true );
			pGrp->ClearAllLoopAni( FLOAT100, true );
			pGrp->SetLoopAni( m_GatherAnimation, FLOAT100 );
		}

		m_dwExpansionFireMotionEndTime = 0;
	}

	if( pOwner->IsNeedProcess() && dwCurTime > m_dwNextExpansionFireTime )
		FireExpansionAttack( pOwner, D3DXVECTOR3( 0.f, 0.f, 0.f) );
}


void ioFireMageExpansionItem::FireExpansionAttack( ioBaseChar *pOwner, D3DXVECTOR3 vPos )
{
	if( !pOwner )
		return;

	ioEntityGroup *pGrp = pOwner->GetGroup();
	if( !pGrp )
		return;

	int iAniID = pGrp->GetAnimationIdx( m_stExpansionAttackMotion );

	pGrp->ClearAllActionAni( FLOAT100, true );
	pGrp->ClearAllLoopAni( FLOAT100, true );

	if( iAniID <= 0 )
		return;

	pGrp->SetActionAni( iAniID, FLOAT100, FLOAT100, FLOAT1, FLOAT1/m_fExpansionAttackMotionRate );

	m_dwExpansionFireMotionEndTime = FRAMEGETTIME();
	m_dwExpansionFireMotionEndTime  += pGrp->GetAnimationFullTime( iAniID ) * m_fExpansionAttackMotionRate;

	if( pOwner->IsNeedProcess() )
		vPos = pOwner->GetMagicCirclePos();

	ioMapEffect *pMapEffect = NULL;
	if( pOwner->GetTeam() == TEAM_RED )
	{
		pMapEffect = pOwner->CreateMapEffectBySkill( m_stExpansionAttackEffect_red, vPos );
	}
	else if( pOwner->GetTeam() == TEAM_BLUE )
	{
		pMapEffect = pOwner->CreateMapEffectBySkill( m_stExpansionAttackEffect_blue, vPos );
	}
	else
		pMapEffect = pOwner->CreateMapEffectBySkill( m_stExpansionAttackEffect_red, vPos );

	vPos.y += m_fExpansionAttackHeightOffset;

	DWORD dwBaseWeaponIndex = pOwner->IncreaseWeaponIndexBase();
	ioWeapon* pWeapon = pOwner->DummyExplicit( m_ExtraAttackInfo, vPos, dwBaseWeaponIndex, false, pOwner->GetCharName(), "", true );
	if( pWeapon )
	{
		pWeapon->SetPosition( vPos );
		pWeapon->SetMoveDir( D3DXVECTOR3(0.f, -FLOAT1, 0.f) );
	}


	DWORD dwCurTime = FRAMEGETTIME();
	m_dwNextExpansionFireTime = dwCurTime + m_dwExpansionAttackTic;

	//SendPacket
	if( pOwner->IsNeedProcess() )
	{
		vPos = pOwner->GetMagicCirclePos();
		SP2Packet kPacket( CUPK_ITEM_GAUGE_OK );
		FillItemGaugePacketHeader( pOwner, kPacket );
		kPacket << vPos;
		P2PNetwork::SendToAllPlayingUser( kPacket );
	}
}


void ioFireMageExpansionItem::ChangeToNormalAttack( ioBaseChar *pOwner )
{
	DWORD dwCurTime = FRAMEGETTIME();
	ClearCancelInfo();

	const AttackAttribute *pAttribute = m_FormalAttack.GetAttribute( AT_NORMAL, m_iCurCombo );
	if( pAttribute )
	{
		pOwner->SetNormalAttackByAttribute( *pAttribute );

		m_dwCancelCheckTime = dwCurTime;
		m_dwInputCancelCheckTime = dwCurTime;

		m_szCurAni = pAttribute->m_AttackAnimation;
		m_fCurAniRate = pAttribute->m_fAttackAniRate;
	}

	pOwner->ApplyNormalAttackResult( m_iCurCombo );
	m_FireState = FS_NORMAL_ATTACK;

	if( pOwner->IsNeedSendNetwork() )
	{
		SP2Packet kPacket( CUPK_ITEM_GAUGE_CANCEL );
		FillItemGaugePacketHeader( pOwner, kPacket );
		kPacket << (int)m_FireState;
		kPacket << m_iCurCombo;
		P2PNetwork::SendToAllPlayingUser( kPacket );
	}
}


void ioFireMageExpansionItem::OnItemGaugeCancel( ioBaseChar *pOwner, SP2Packet &rkPacket )
{
	int iFireState;
	rkPacket >> iFireState;
	rkPacket >> m_iCurCombo;

	switch( iFireState )
	{
	case FS_NORMAL_ATTACK:
		ioAttackableItem::SetNormalAttack( pOwner, m_iCurCombo );
		m_FireState = FS_NORMAL_ATTACK;
		break;
	case FS_GATHERING:
		pOwner->EndEffect( m_GatherEffect );
		ChangeToGathering( pOwner );
		break;
	case FS_NONE:
		m_FireState = FS_NONE;

		pOwner->EndEffect( m_GatherEffect );
		pOwner->SetState( CS_DELAY );
		break;
	}
}


void ioFireMageExpansionItem::OnItemGaugeOK( ioBaseChar *pOwner, SP2Packet &rkPacket )
{
	D3DXVECTOR3 vPos;
	rkPacket >> vPos;

	FireExpansionAttack( pOwner, vPos );
}


ioWeaponItem::WeaponSubType ioFireMageExpansionItem::GetSubType() const
{
	return WST_FIRE_MAGE_EXPANSION_ITEM;
}

void ioFireMageExpansionItem::OnEndExtendAttack( ioBaseChar *pOwner, bool bReleaseItem )
{
	pOwner->EndEffect( m_GatherEffect );
}

float ioFireMageExpansionItem::GetCircleRadius()
{
	return m_fMagicCircleRadius;
}

bool ioFireMageExpansionItem::IsChargeAttackState( ioBaseChar *pOwner )
{
	if( pOwner->GetState() == CS_ATTACK )
	{
		if( m_FireState == FS_GATHERING || m_FireState == FS_CHARGING )
			return true;
	}

	return false;
}

void ioFireMageExpansionItem::UpdateSkillGauge( ioBaseChar *pOwner, float fRate, bool bUpdate )
{
	if( m_dwTicCheckTime == 0 )
		return;

	float fRecoveryGauge = 0.0f;

	if( pOwner->GetState() == CS_RUN ||
		pOwner->GetState() == CS_DELAY ||
		pOwner->GetState() == CS_SPECIAL_MOTION ||
		pOwner->GetState() == CS_ETCITEM_MOTION ||
		pOwner->GetState() == CS_BLOCK_INTEPLAY ||
		IsCanUpdateGauge(pOwner) )
	{
		fRecoveryGauge = m_fDefaultGauge * pOwner->GetDelayRunGaugeRate();
	}
	else if( pOwner->GetState() == CS_DIE ||
		pOwner->GetState() == CS_VIEW ||
		pOwner->GetState() == CS_LOADING ||
		pOwner->GetState() == CS_OBSERVER )
	{
		bUpdate = false;
	}
	else
	{
		fRecoveryGauge = m_fDefaultGauge * pOwner->GetEtcGaugeRate();
	}


	DWORD dwCurTime = FRAMEGETTIME();
	int iCurRecoveryTic = (int)pOwner->GetDefaultRecoveryGaugeTic();

	float fPreGauge = m_fCurSkillGauge;
	float fNewRecovery = fRecoveryGauge * fRate;

	int iTickGap = (int)( dwCurTime - m_dwTicCheckTime );
	if( iTickGap >= iCurRecoveryTic )
	{
		while( iTickGap >= iCurRecoveryTic )
		{
			if( bUpdate )
			{
				m_fCurSkillGauge = min( m_fCurSkillGauge+fNewRecovery, FLOAT100 );
			}

			iTickGap -= iCurRecoveryTic;
		}

		m_dwTicCheckTime = dwCurTime - iTickGap;
	}

	//#ifndef SHIPPING 
	//	if( Setting::ShowExtraInfo() )
	//	{
	//		float fMaxGauge = GetMaxSkillGauge( pOwner->GetCharName() );
	//
	//		if( fMaxGauge < FLOAT100 )
	//		{
	//			if( fPreGauge == 0.0f && m_fCurSkillGauge > 0.0f )
	//			{
	//				LOG.PrintTimeAndLog( 0, "SkillCheck - %s, MaxGauge: %f", GetSkillName().c_str(), fMaxGauge );
	//				LOG.PrintTimeAndLog( 0, "SkillCheck - %s, RecoveryStart: %d", GetSkillName().c_str(), FRAMEGETTIME() );
	//			}
	//
	//			if( fPreGauge < fMaxGauge && m_fCurSkillGauge >= fMaxGauge )
	//			{
	//				LOG.PrintTimeAndLog( 0, "SkillCheck - %s, RecoveryEnd: %d", GetSkillName().c_str(), FRAMEGETTIME() );
	//			}
	//		}
	//	}
	//#endif

	if( fRate > 0.0f )
		m_bUpdateEnableSkillGauge = bUpdate;
	else
		m_bUpdateEnableSkillGauge = false;
}

bool ioFireMageExpansionItem::IsEnableExtraGauge( ioBaseChar *pChar )
{
	if( m_bGaugeUpdateChargeAttack && IsChargeAttackState( pChar ) )
		return false;

	return true;
}

bool ioFireMageExpansionItem::SetJumpAttack( ioBaseChar *pOwner, int iAniID, float fTimeRate, AttackType eType )
{
	if( !pOwner ) return false;

	DWORD dwCurTime = FRAMEGETTIME();
	ClearAirJump();
	AttackType eExtendType = AT_JUMP_ATTACK;

	const AttackAttribute *pAttr = GetAttackAttribute( eExtendType );
	if( !pAttr )
	{
		LOG.PrintTimeAndLog( 0, "ioFireMageExpansionItem::SetJumpAttack - AttackAttribute Not Exist" );
		return false;
	}

	ioEntityGroup *pGrp = pOwner->GetGroup();
	iAniID = pGrp->GetAnimationIdx( pAttr->m_AttackAnimation );
	if( iAniID == -1 )
	{
		LOG.PrintTimeAndLog( 0, "ioFireMageExpansionItem::SetJumpAttack - JumpAttack Animation Not Exist" );
		return false;
	}

	fTimeRate   = pAttr->m_fAttackAniRate;
	DWORD dwPreDelay  = pAttr->m_dwPreDelay;

	// For AirJumpPower
	SetAirJump( pOwner, iAniID, fTimeRate );

	return false;
}

void ioFireMageExpansionItem::ProcessJumpAttack( ioBaseChar *pOwner, float fHeightGap )
{
	if( m_pExtendJump )
	{
		m_pExtendJump->ProcessJumpAttack( pOwner, fHeightGap );
	}

	CheckAirJump( pOwner );
}

void ioFireMageExpansionItem::SetExtraInfoToWeapon( ioWeapon *pWeapon, bool bJumpAttack )
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



