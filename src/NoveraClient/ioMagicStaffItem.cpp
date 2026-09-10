

#include "stdafx.h"

#include "ioBaseChar.h"
#include "ioPlayStage.h"

#include "ioMagicStaffItem.h"

#include "ioGrowthUpInfo.h"

#include "WeaponDefine.h"

ioMagicStaffItem::ioMagicStaffItem()
{
	m_dwTimeGap = 0;
	m_dwChargeStartTime = 0;
	m_iCurCombo = 0;
	m_FireState = FS_NONE;

	m_iCurAirJump = 0;
	m_fAirJumpPowerRate = 0.0f;
	m_vAirJumpTimeList.clear();

	m_dwSpecialStartTime = 0;
	m_dwSpecialEndTime = 0;
	m_dwSpecialFireTime = 0;
	m_dwInputAttackTime = 0;
}

ioMagicStaffItem::ioMagicStaffItem( const ioMagicStaffItem &rhs )
: ioWeaponItem( rhs ),
 m_PreGatherAnimation( rhs.m_PreGatherAnimation ),
 m_GatherAnimation( rhs.m_GatherAnimation ),
 m_GatherEffect( rhs.m_GatherEffect ),
 m_EndGatherAnimation( rhs.m_EndGatherAnimation ),
 m_fMoveSpeed( rhs.m_fMoveSpeed ),
 m_fRange( rhs.m_fRange ),
 m_fHeightGap( rhs.m_fHeightGap ),
 m_fCircleOffSet( rhs.m_fCircleOffSet ),
 m_fGaugePerAllDefense( rhs.m_fGaugePerAllDefense ),
 m_dwDefenseTicTime( rhs.m_dwDefenseTicTime ),
 m_ReadyCircle( rhs.m_ReadyCircle ),
 m_EnableCircle( rhs.m_EnableCircle ),
 m_fMagicCircleRadius( rhs.m_fMagicCircleRadius ),
 m_fSkillGaugeRate( rhs.m_fSkillGaugeRate ),
 m_fMaxGauge( rhs.m_fMaxGauge ),
 m_fNeedGauge( rhs.m_fNeedGauge ),
 m_fIncreaseGauge( rhs.m_fIncreaseGauge ),
 m_AreaWeaponName( rhs.m_AreaWeaponName ),
 m_ChargeAttackRedCircle( rhs.m_ChargeAttackRedCircle ),
 m_ChargeAttackBlueCircle( rhs.m_ChargeAttackBlueCircle ),
 m_ChargeAttackDelay( rhs.m_ChargeAttackDelay ),
 m_ChargeAttackAni( rhs.m_ChargeAttackAni ),
 m_dwPreDelayTime( rhs.m_dwPreDelayTime ),
 m_fChargeAttackAniRate( rhs.m_fChargeAttackAniRate ),
 m_SpecialAttackDelay( rhs.m_SpecialAttackDelay ),
 m_SpecialAttack( rhs.m_SpecialAttack ),
 m_fSpecialAttackAniRate( rhs.m_fSpecialAttackAniRate ),
 m_vSpecialWeaponOffset( rhs.m_vSpecialWeaponOffset ),
 m_dwSpecialAttackAttribute( rhs.m_dwSpecialAttackAttribute ),
 m_iMaxSpecialWeaponCnt( rhs.m_iMaxSpecialWeaponCnt ),
 m_cInputAttributeA( rhs.m_cInputAttributeA ),
 m_cInputAttributeS( rhs.m_cInputAttributeS ),
 m_cJumpInputAttributeA( rhs.m_cJumpInputAttributeA ),
 m_cJumpInputAttributeS( rhs.m_cJumpInputAttributeS ),
 m_cDashInputAttributeA( rhs.m_cDashInputAttributeA ),
 m_cDashInputAttributeS( rhs.m_cDashInputAttributeS )
{
	m_dwTimeGap = 0;
	m_dwChargeStartTime = 0;
	m_iCurCombo = 0;
	m_FireState = FS_NONE;

	m_iCurAirJump = 0;
	m_fAirJumpPowerRate = 0.0f;
	m_vAirJumpTimeList.clear();

	m_fCurGauge = 0.0f;
	m_dwCurMapEffect = -1;
	m_dwChargeAttackPreDelayStartTime = 0;
	m_dwChargeAttackEndTime = 0;
	m_dwChargeAttackFireTime = 0;

	m_dwSpecialStartTime = 0;
	m_dwSpecialEndTime = 0;
	m_dwSpecialFireTime = 0;
}

ioMagicStaffItem::~ioMagicStaffItem()
{
	m_vAirJumpTimeList.clear();
}

void ioMagicStaffItem::LoadProperty( ioINILoader &rkLoader )
{
	ioWeaponItem::LoadProperty( rkLoader );

	char szBuf[MAX_PATH];

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
	m_fCircleOffSet = rkLoader.LoadFloat_e( "magic_circle_offset", 0.0f );

	m_fGaugePerAllDefense = rkLoader.LoadFloat_e( "gauge_per_defense", 5.0f );
	m_dwDefenseTicTime = rkLoader.LoadInt_e( "defense_tic_time", 500 );
	m_fSkillGaugeRate = rkLoader.LoadFloat_e( "skill_gauge_rate", FLOAT1 );

	m_fMaxGauge = rkLoader.LoadFloat_e( "max_gauge", 0.f );
	m_fNeedGauge = rkLoader.LoadFloat_e( "need_gauge", 0.f );
	m_fIncreaseGauge = rkLoader.LoadFloat_e( "increase_gauge", FLOAT1 );

	rkLoader.LoadString_e( "charge_attack_red_team_circle", "", szBuf, MAX_PATH );
	m_ChargeAttackRedCircle = szBuf;
	rkLoader.LoadString_e( "charge_attack_blue_team_circle", "", szBuf, MAX_PATH );
	m_ChargeAttackBlueCircle = szBuf;
	rkLoader.LoadString_e( "charge_attack_delay_ani", "", szBuf, MAX_PATH );
	m_ChargeAttackDelay = szBuf;
	rkLoader.LoadString_e( "charge_attack_ani", "", szBuf, MAX_PATH );
	m_ChargeAttackAni = szBuf;
	rkLoader.LoadString_e( "area_weapon_name", "", szBuf, MAX_PATH );
	m_AreaWeaponName = szBuf;

	m_dwPreDelayTime = rkLoader.LoadInt_e( "charge_attack_predelay_time", 800 );
	m_fChargeAttackAniRate = rkLoader.LoadFloat_e( "charge_attack_ani_rate", FLOAT1 );

	rkLoader.LoadString_e( "special_attack_delay_ani", "", szBuf, MAX_PATH );
	m_SpecialAttackDelay = szBuf;
	rkLoader.LoadString_e( "special_attack_ani", "", szBuf, MAX_PATH );
	m_SpecialAttack = szBuf;

	m_fSpecialAttackAniRate = rkLoader.LoadFloat_e( "special_attack_ani_rate", FLOAT1 );

	m_dwSpecialAttackAttribute = rkLoader.LoadInt_e( "special_weapon_attribute", 0 );
	m_iMaxSpecialWeaponCnt = rkLoader.LoadInt_e( "max_special_weapon_cnt", 0 );
	m_vSpecialWeaponOffset.clear();
	m_vSpecialWeaponOffset.reserve( max(m_iMaxSpecialWeaponCnt, 1) );
	char szKey[MAX_PATH];
	for( int i = 1; i <=m_iMaxSpecialWeaponCnt; i++  )
	{
		wsprintf_e( szKey, "special_weaponOffset%d", i );
		int nValue = rkLoader.LoadInt( szKey, 0 );;
		m_vSpecialWeaponOffset.push_back( nValue );
	}
	LoadAttackAttribute_e( "input_attack_a", m_cInputAttributeA, rkLoader );
	LoadAttackAttribute_e( "input_attack_s", m_cInputAttributeS, rkLoader );
	LoadAttackAttribute_e( "jump_input_attack_a", m_cJumpInputAttributeA, rkLoader );
	LoadAttackAttribute_e( "jump_input_attack_s", m_cJumpInputAttributeS, rkLoader );
	LoadAttackAttribute_e( "dash_input_attack_a", m_cDashInputAttributeA, rkLoader );
	LoadAttackAttribute_e( "dash_input_attack_s", m_cDashInputAttributeS, rkLoader );
}

ioItem* ioMagicStaffItem::Clone()
{
	return new ioMagicStaffItem( *this );
}

void ioMagicStaffItem::SetNormalAttack( ioBaseChar *pOwner, int iCurCombo )
{
	if( !pOwner ) return;

	ClearCancelInfo();
	m_dwInputAttackTime = 0;

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
	}
}

void ioMagicStaffItem::ChangeToGathering( ioBaseChar *pOwner )
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
	m_dwInputAttackTime = 0;

	if( pOwner->IsNeedSendNetwork() )
	{
		SP2Packet kPacket( CUPK_ITEM_GAUGE_CANCEL );
		FillItemGaugePacketHeader( pOwner, kPacket );
		kPacket << (int)m_FireState;
		kPacket << m_iCurCombo;
		P2PNetwork::SendToAllPlayingUser( kPacket );
	}
}

void ioMagicStaffItem::CheckNormalAttack( ioBaseChar *pOwner )
{
	DWORD dwCurTime = FRAMEGETTIME();
	if( pOwner->IsNeedProcess() && CheckInputAttack() )
	{
		if( pOwner->IsJumpKey() )
		{
			SetInputAttackState( pOwner, true, IAS_BASE_A );
			return;
		}
		else if( pOwner->IsDefenseKey() )
		{
			SetInputAttackState( pOwner, true, IAS_BASE_S );
			return;
		}
	}

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
	case FS_CHARGE_ATTACK_DELAY:
		if( m_dwChargeAttackPreDelayStartTime > 0 && m_dwChargeAttackPreDelayStartTime + m_dwPreDelayTime < dwCurTime )
		{		
			m_dwChargeAttackPreDelayStartTime = 0;
			SetChargeAttackState( pOwner );
		}
		break;
	case FS_CHARGE_ATTACK:
		if( m_dwChargeAttackFireTime != 0 && m_dwChargeAttackFireTime < dwCurTime )
		{
			CreateArea( pOwner );
			CheckDecreaseGauge();

			if( pOwner->IsNeedSendNetwork() )
			{
				SP2Packet kPacket( CUPK_ITEM_GAUGE_CANCEL );
				FillItemGaugePacketHeader( pOwner, kPacket );
				kPacket << (int)m_FireState;
				kPacket << m_iCurCombo;
				P2PNetwork::SendToAllPlayingUser( kPacket );
			}
			m_dwChargeAttackFireTime = 0;
		}
		else if( m_dwChargeAttackEndTime != 0 && m_dwChargeAttackEndTime < dwCurTime )
		{
			pOwner->EndMapEffect( m_dwCurMapEffect );
			pOwner->DestroyMagicCircle();

			m_FireState = FS_NONE;
			pOwner->SetState( CS_DELAY );

			if( pOwner->IsNeedSendNetwork() )
			{
				SP2Packet kPacket( CUPK_ITEM_GAUGE_CANCEL );
				FillItemGaugePacketHeader( pOwner, kPacket );
				kPacket << (int)m_FireState;
				kPacket << m_iCurCombo;
				P2PNetwork::SendToAllPlayingUser( kPacket );
			}
			m_dwChargeAttackEndTime = 0;
		}
		break;
	}
}

void ioMagicStaffItem::OnCharging( ioBaseChar *pOwner )
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

void ioMagicStaffItem::OnGathering( ioBaseChar *pOwner )
{
	if( !pOwner || !pOwner->IsNeedProcess() )
		return;

	if( !pOwner->IsAttackKeyDown() )	// 버튼 뗌
	{
		pOwner->EndEffect( m_GatherEffect );

		if( IsEnableGauge() && !m_ChargeAttackDelay.IsEmpty() )
		{
			m_FireState = FS_CHARGE_ATTACK_DELAY;

			if( pOwner->CheckMagicCircle() )
			{
				pOwner->CheckMagicCircleVisible( false );
				pOwner->UpdateMagicCirclePos( 0.0f, 0.0f, 0.0f, true, true );
			}

			float fCurOffSet = GetMagicCircleOffSet();
			m_vCreatePos = pOwner->GetMagicCirclePos( fCurOffSet );

			ChangeToChargeAttack( pOwner );
		}
		else if( !m_SpecialAttackDelay.IsEmpty() )
		{
			ChangeToSpecialDelayState( pOwner );
		}
		else
		{
			m_FireState = FS_NONE;

			pOwner->DestroyMagicCircle();
			pOwner->SetState( CS_DELAY );

			if( pOwner->IsNeedSendNetwork() )
			{
				SP2Packet kPacket( CUPK_ITEM_GAUGE_CANCEL );
				FillItemGaugePacketHeader( pOwner, kPacket );
				kPacket << (int)m_FireState;
				kPacket << m_iCurCombo;
				P2PNetwork::SendToAllPlayingUser( kPacket );
			}
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
}

void ioMagicStaffItem::ChangeToNormalAttack( ioBaseChar *pOwner )
{
	ioEntityGroup *pGrp = pOwner->GetGroup();
	if( !pGrp )
		return;

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

		int iAniID = pGrp->GetAnimationIdx( pAttribute->m_AttackAnimation );
		SetInputAttackInfo( pOwner, iAniID, pAttribute->m_fAttackAniRate );
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

void ioMagicStaffItem::ChangeToChargeAttack( ioBaseChar *pOwner )
{
	ioMapEffect *pMapEffect = NULL;
	if( pOwner->GetSingleConvertTeam() == TEAM_BLUE )
		pMapEffect = pOwner->CreateMapEffectBySkill( m_ChargeAttackBlueCircle, m_vCreatePos );
	else
		pMapEffect = pOwner->CreateMapEffectBySkill( m_ChargeAttackRedCircle, m_vCreatePos );

	if( pMapEffect )
		m_dwCurMapEffect = pMapEffect->GetUniqueID();

	ioEntityGroup *pGrp = pOwner->GetGroup();
	pGrp->ClearAllLoopAni( FLOAT100, true );
	pGrp->ClearAllActionAni( FLOAT100, true );
	pGrp->SetLoopAni( m_ChargeAttackDelay, FLOAT100 );

	m_dwChargeAttackPreDelayStartTime = FRAMEGETTIME();

	if( pOwner->IsNeedSendNetwork() )
	{
		SP2Packet kPacket( CUPK_ITEM_GAUGE_CANCEL );
		FillItemGaugePacketHeader( pOwner, kPacket );
		kPacket << (int)m_FireState;
		kPacket << m_iCurCombo;
		kPacket << m_vCreatePos;
		P2PNetwork::SendToAllPlayingUser( kPacket );
	}
}

void ioMagicStaffItem::OnItemGaugeCancel( ioBaseChar *pOwner, SP2Packet &rkPacket )
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
	case FS_CHARGE_ATTACK_DELAY:
		rkPacket >> m_vCreatePos;
		m_FireState = FS_CHARGE_ATTACK_DELAY;
		ChangeToChargeAttack( pOwner );
		break;
	case FS_CHARGE_ATTACK:
		CreateArea( pOwner );
		break;
	}
}

void ioMagicStaffItem::OnItemGaugeOK( ioBaseChar *pOwner, SP2Packet &rkPacket )
{
	if( !pOwner ) return;

	int iState;
	rkPacket >> iState;

	D3DXVECTOR3 vPos;
	rkPacket >> vPos;
	pOwner->SetWorldPosition( vPos );

	D3DXQUATERNION qtRot;
	rkPacket >> qtRot;
	pOwner->SetTargetRotAndMoveDirChange( qtRot );

	switch( iState )
	{
	case FS_SPECIAL_ATTACK_DELAY:
		{
			D3DXVECTOR3 vCirclePos;
			rkPacket >> vCirclePos;
			pOwner->SetMagicCirclePos( false, vCirclePos );

			ChangeToSpecialDelayState( pOwner );
		}
		break;
	case FS_SPECIAL_ATTACK:
		{
			m_vSpecialWeaponPosList.clear();
			int nCnt = 0;
			rkPacket >> nCnt;
			m_iCurSpecialWeaponCnt = nCnt;
			for( int i = 0; i < nCnt; i++ )
			{
				D3DXVECTOR3 vPos;
				rkPacket >> vPos;
				m_vSpecialWeaponPosList.push_back( vPos );
			}
			ChangeToSpecialAttackState( pOwner );
		}
		break;
	case FS_SPECIAL_END:
		ChangeToSpecialEndState( pOwner );
		break;
	case FS_INPUT_ATTACK:
		{
			int iState;
			rkPacket >> iState;
			SetInputAttackState( pOwner, false, iState );
		}
		break;
	}
}

ioWeaponItem::WeaponSubType ioMagicStaffItem::GetSubType() const
{
	return WST_MAGIC_STAFF;
}

void ioMagicStaffItem::OnEndExtendAttack( ioBaseChar *pOwner, bool bReleaseItem )
{
	pOwner->EndEffect( m_GatherEffect );
}

float ioMagicStaffItem::GetCircleRadius()
{
	return m_fMagicCircleRadius;
}

bool ioMagicStaffItem::IsChargeAttackState( ioBaseChar *pOwner )
{
	if( pOwner->GetState() == CS_ATTACK )
	{
		if( m_FireState == FS_GATHERING || m_FireState == FS_CHARGING )
			return true;
	}

	return false;
}

void ioMagicStaffItem::UpdateSkillGauge( ioBaseChar *pOwner, float fRate, bool bUpdate )
{
	if( m_dwTicCheckTime == 0 )
		return;

	float fRecoveryGauge = 0.0f;

	if( pOwner->GetState() == CS_RUN ||
		pOwner->GetState() == CS_DELAY ||
		pOwner->GetState() == CS_SPECIAL_MOTION ||
		pOwner->GetState() == CS_ETCITEM_MOTION ||
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
	
	if( fRate > 0.0f )
		m_bUpdateEnableSkillGauge = bUpdate;
	else
		m_bUpdateEnableSkillGauge = false;
}

bool ioMagicStaffItem::IsEnableExtraGauge( ioBaseChar *pChar )
{
	if( m_bGaugeUpdateChargeAttack && IsChargeAttackState( pChar ) )
		return false;

	return true;
}

bool ioMagicStaffItem::SetJumpAttack( ioBaseChar *pOwner, int iAniID, float fTimeRate, AttackType eType )
{
	if( !pOwner ) return false;

	DWORD dwCurTime = FRAMEGETTIME();
	ClearAirJump();
	AttackType eExtendType = AT_JUMP_ATTACK;

	const AttackAttribute *pAttr = GetAttackAttribute( eExtendType );
	if( !pAttr )
	{
		LOG.PrintTimeAndLog( 0, "ioMagicStaffItem::SetJumpAttack - AttackAttribute Not Exist" );
		return false;
	}

	ioEntityGroup *pGrp = pOwner->GetGroup();
	iAniID = pGrp->GetAnimationIdx( pAttr->m_AttackAnimation );
	if( iAniID == -1 )
	{
		LOG.PrintTimeAndLog( 0, "ioMagicStaffItem::SetJumpAttack - JumpAttack Animation Not Exist" );
		return false;
	}

	fTimeRate   = pAttr->m_fAttackAniRate;
	DWORD dwPreDelay  = pAttr->m_dwPreDelay;

	// For AirJumpPower
	SetAirJump( pOwner, iAniID, fTimeRate );

	return false;
}

void ioMagicStaffItem::ProcessJumpAttack( ioBaseChar *pOwner, float fHeightGap )
{
	if( m_pExtendJump )
	{
		m_pExtendJump->ProcessJumpAttack( pOwner, fHeightGap );
	}

	CheckAirJump( pOwner );
}

void ioMagicStaffItem::SetExtraInfoToWeapon( ioWeapon *pWeapon, bool bJumpAttack )
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

void ioMagicStaffItem::CheckDecreaseGauge()
{
	m_fCurGauge -= m_fNeedGauge;
	m_fCurGauge = (float)max( 0, min( m_fCurGauge, m_fMaxGauge ) );
}

void ioMagicStaffItem::SetCurGauge( float fGauge )
{
	m_fCurGauge = fGauge;
	m_fCurGauge = (float)max( 0, min( m_fCurGauge, m_fMaxGauge ) );
}

int ioMagicStaffItem::GetMaxBullet()
{
	return (int)m_fMaxGauge;
}

int ioMagicStaffItem::GetNeedBullet()
{
	return (int)m_fNeedGauge;
}

int ioMagicStaffItem::GetCurBullet()
{
	float fCurCoolTime = m_fCurGauge;
	fCurCoolTime = max( 0, min( fCurCoolTime, m_fMaxGauge ) );

	return (int)fCurCoolTime;
}

void ioMagicStaffItem::SetCurBullet( int iCurBullet )
{
	if( iCurBullet < 0 )
		return;

	float fCurBullet = iCurBullet;
	fCurBullet = max( 0, min( fCurBullet, m_fMaxGauge ) );

	m_fCurGauge = fCurBullet;
}

bool ioMagicStaffItem::IsEnableGauge()
{
	if( m_fMaxGauge != 0.f && m_fCurGauge >= m_fNeedGauge )
		return true;

	return false;

}
void ioMagicStaffItem::UpdateExtraData( ioBaseChar *pOwner )
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
	case CS_JUMP:
		if( m_fCurGauge < m_fMaxGauge )
		{
			float fGauge = m_fIncreaseGauge * fTimePerSec;

			m_fCurGauge += fGauge;
			m_fCurGauge = min( m_fCurGauge, m_fMaxGauge );
		}
		return;
	}
}

void ioMagicStaffItem::OnReleased( ioBaseChar *pOwner )
{
	m_fCurGauge = 0.0f;

	ioAttackableItem::OnReleased( pOwner );
}

void ioMagicStaffItem::SetChargeAttackState( ioBaseChar *pChar )
{
	m_FireState = FS_CHARGE_ATTACK;

	DWORD dwCurTime = FRAMEGETTIME();
	ioEntityGroup *pGrp = pChar->GetGroup();
	int iCurSkillAniID = pGrp->GetAnimationIdx( m_ChargeAttackAni );
	if( iCurSkillAniID == -1 )
		return;

	pGrp->ClearAllLoopAni( FLOAT100, true );
	pGrp->ClearAllActionAni( FLOAT100, true );
	pGrp->SetActionAni( iCurSkillAniID, FLOAT100, FLOAT100, FLOAT1, FLOAT1/m_fChargeAttackAniRate );
	
	m_dwChargeAttackEndTime = dwCurTime;
	m_dwChargeAttackEndTime += pGrp->GetAnimationFullTime( iCurSkillAniID ) * m_fChargeAttackAniRate;

	m_dwChargeAttackFireTime = dwCurTime;
	m_dwChargeAttackFireTime += pGrp->GetAnimationEventTime_e( iCurSkillAniID, "fire_s" ) * m_fChargeAttackAniRate;
}

void ioMagicStaffItem::CreateArea( ioBaseChar *pOwner )
{
	if( m_AreaWeaponName.IsEmpty() ) return;
	if( !pOwner ) return;

	g_AreaWeaponMgr.CreateAreaWeapon( pOwner->GetCharName(), "", GetName(), m_AreaWeaponName, 
									  m_vCreatePos, pOwner->GetTargetRot(), ioAreaWeapon::CT_NORMAL );
}

void ioMagicStaffItem::ChangeToSpecialDelayState( ioBaseChar *pOwner )
{
	ioEntityGroup *pGrp = pOwner->GetGroup();
	if( !pGrp )
		return;

	int iAniID = pGrp->GetAnimationIdx( m_SpecialAttackDelay );
	pGrp->ClearAllLoopAniExceptOne( iAniID, FLOAT100, true );
	pGrp->ClearAllActionAni( FLOAT10, true );
	pGrp->SetLoopAni( iAniID, FLOAT100 );

	pOwner->SetState( CS_MAGICSTAFF_SPECIAL );
	m_FireState = FS_SPECIAL_ATTACK_DELAY;
	m_dwSpecialStartTime = FRAMEGETTIME();
	m_iCurSpecialWeaponCnt = 0;

	ioPlayStage *pStage = pOwner->GetCreator();
	if ( !pStage )
		return;

	AddSpecialAttackPosition( pOwner, pStage );
	D3DXVECTOR3 vCharPos = pOwner->GetWorldPosition();
	D3DXVECTOR3 vCirclePos = pOwner->GetMagicCirclePos();
	D3DXVECTOR3 vDir = vCirclePos - vCharPos;
	D3DXVec3Normalize( &vDir, &vDir );
					
	D3DXVECTOR3 vTargetPos = vCirclePos + vDir * m_fCircleOffSet;
	pOwner->SetMagicCirclePos(false, vTargetPos);

	if(pOwner->IsNeedProcess() )
	{
		//Send
		SP2Packet kPacket( CUPK_ITEM_GAUGE_OK );
		FillItemGaugePacketHeader( pOwner, kPacket );
		kPacket << (int)m_FireState;
		kPacket << pOwner->GetWorldPosition();
		kPacket << pOwner->GetTargetRot();
		kPacket << vCirclePos;
		P2PNetwork::SendToAllPlayingUser( kPacket );
	}
}

void ioMagicStaffItem::ClearAttackState( ioBaseChar *pOwner )
{
	m_dwSpecialStartTime = 0;
	m_dwSpecialEndTime = 0;
	m_dwSpecialFireTime = 0;
	m_vSpecialWeaponPosList.clear();
	m_iCurSpecialWeaponCnt = 0;

	pOwner->DestroyMagicCircle();

	if( !m_dwEnableCircleIDList.empty() )
	{
		ioPlayStage *pStage = pOwner->GetCreator();
		if( pStage )
		{
			int iCnt = m_dwEnableCircleIDList.size();
			for( int i=0; i<iCnt; ++i)
			{
				pStage->DestroyMapEffect( m_dwEnableCircleIDList[i] );
			}
			m_dwEnableCircleIDList.clear();
		}
	}
}

void ioMagicStaffItem::ProcessSpecialState( ioBaseChar *pOwner )
{
	if( !pOwner )
		return;

	ioPlayStage *pStage = pOwner->GetCreator();
	if( !pStage )
		return;

	DWORD dwCurTime = FRAMEGETTIME();
	switch( m_FireState )
	{
	case FS_SPECIAL_ATTACK_DELAY:
		{
			int nSize = m_vSpecialWeaponPosList.size();
			float fCurRange = 0.f;
			if( m_vSpecialWeaponPosList.size() < m_vSpecialWeaponOffset.size() )
				fCurRange = m_vSpecialWeaponOffset[ nSize ];

			int iSkillNum = pOwner->GetSkillKeyInput();
			if( iSkillNum == 0 )
			{
				if( m_vSpecialWeaponPosList.size() > 1 )
					ChangeToSpecialAttackState( pOwner );
				else
					ChangeToSpecialEndState( pOwner );
			}

			// 뱡향 변화 체크
			pOwner->CheckChangeDirectionForMagicCircle();

			// MagicCircle 위치 갱신
			D3DXVECTOR3 vCharPos = pOwner->GetWorldPosition();
			if( m_vSpecialWeaponPosList.size() != 0 )
			{
				pOwner->UpdateMagicCircleToCirclePosAll( m_fMoveSpeed, fCurRange, m_fCircleOffSet, 0, 0, 0, 
					m_vSpecialWeaponPosList[ m_vSpecialWeaponPosList.size() - 1 ] , false, false );
			}
			else
			{
				pOwner->UpdateMagicCircleToCirclePosAll( m_fMoveSpeed, fCurRange, m_fCircleOffSet, 0, 0, 0, vCharPos, false, false );
			}

			// MagicCircle 위치방향으로 캐릭터 회전
			D3DXVECTOR3 vTargetPos = pOwner->GetMagicCirclePos();
			pOwner->SetTargetRotToTargetPos( vTargetPos, true, false );

			if( pOwner->IsNeedProcess() )
			{
				bool bAddPosition = false;
				if( pOwner->IsAttackKey() )
				{					
					bAddPosition = true;
					AddSpecialAttackPosition( pOwner, pStage );
				}

				if( m_iCurSpecialWeaponCnt >= m_iMaxSpecialWeaponCnt )
				{
					ChangeToSpecialAttackState( pOwner );
				}
				else if( bAddPosition )
				{
					//아직 셋팅할 구역이 있다면 Circle을 offset만큼 이동시켜 준다.
					D3DXVECTOR3 vCharPos = pOwner->GetWorldPosition();
					D3DXVECTOR3 vDir = vTargetPos - vCharPos;
					D3DXVec3Normalize( &vDir, &vDir );
					
					vTargetPos = vTargetPos + vDir * m_fCircleOffSet;
					pOwner->SetMagicCirclePos(false, vTargetPos);
				}
			}
		}
		break;
	case FS_SPECIAL_ATTACK:
		if( m_dwSpecialEndTime < dwCurTime )
		{
			ChangeToSpecialEndState( pOwner );
		}
		else if( m_dwSpecialFireTime > 0 && m_dwSpecialFireTime < FRAMEGETTIME() )
		{
			CreateWeapon( pOwner );

			m_dwSpecialFireTime = 0;
		}
		break;
	case FS_INPUT_ATTACK:
		if( m_iInputAttackState == IAS_JUMP_A || m_iInputAttackState == IAS_JUMP_S )
		{
			pOwner->SetJumpPower( 0 );
			pOwner->SetAirJumpPower( 0 );
			pOwner->SetCurJumpPowerAmt( 0.0f );
			pOwner->SetGravityAmt( 0.0f );
		}
		if( m_dwInputAttackEndTime < dwCurTime )
		{
			ChangeToSpecialEndState( pOwner );
		}
		break;
	}
}

void ioMagicStaffItem::CreateWeapon( ioBaseChar *pChar )
{
	if( !pChar )	return;
	if( m_vSpecialWeaponPosList.empty() )	return;

	ioPlayStage *pStage = pChar->GetCreator();

	int iCnt = m_vSpecialWeaponPosList.size();
	D3DXQUATERNION qtRot;
	int iWeaponIndex = pChar->IncreaseWeaponIndexBase();
	
	for( int i=1; i<iCnt; ++i )
	{
		//서로 점과의 방향을 구해서 돌리자
		D3DXVECTOR3 vMoveDir = m_vSpecialWeaponPosList[i] - m_vSpecialWeaponPosList[i-1];
		ioMath::CalcDirectionQuaternion( qtRot, -vMoveDir );
		float fLength = fabs( D3DXVec3Length( &vMoveDir ) );
		D3DXVec3Normalize( &vMoveDir, &vMoveDir );

		FireTime kFireTime;
		kFireTime.dwStart = FRAMEGETTIME();
		kFireTime.dwDuration = 1;
		kFireTime.iAttributeIdx = m_dwSpecialAttackAttribute;
		kFireTime.dwWeaponIdx = iWeaponIndex++;
		ioWeapon *pWeapon = pChar->ExtendFireExplicit( kFireTime, m_vSpecialWeaponPosList[i-1], vMoveDir, "" );
	
		if( pWeapon == 0 )
		{
			LOG.PrintTimeAndLog(0, "ioMagicStaffItem-->NoWeapon");
			assert( 0 );
		}

		ioFlamewaveWeapon  *pFlamewaveWeapon = ToFlamewaveWeapon(pWeapon);
		if( pFlamewaveWeapon )
			pFlamewaveWeapon->SetCustomRange(true, fLength );
	}
	
	m_vSpecialWeaponPosList.clear();

	if( !m_dwEnableCircleIDList.empty() )
	{
		ioPlayStage *pStage = g_SkillMaker.GetPlayStage();
		if( pStage )
		{
			int iCnt = m_dwEnableCircleIDList.size();
			for( int i=0; i<iCnt; ++i)
			{
				pStage->DestroyMapEffect( m_dwEnableCircleIDList[i] );

			}
			m_dwEnableCircleIDList.clear();
		}
	}
}

void ioMagicStaffItem::ChangeToSpecialEndState( ioBaseChar *pOwner )
{
	if( !pOwner )
		return;

	ClearAttackState( pOwner );
	pOwner->SetSKillEndJumpState( 0.1f, false, false, false );

	if( pOwner->IsNeedSendNetwork() )
	{
		SP2Packet kPacket( CUPK_ITEM_GAUGE_OK );
		FillItemGaugePacketHeader( pOwner, kPacket );
		kPacket << (int)FS_SPECIAL_END;
		kPacket << pOwner->GetWorldPosition();
		kPacket << pOwner->GetTargetRot();
		P2PNetwork::SendToAllPlayingUser( kPacket );
	}
}

void ioMagicStaffItem::ChangeToSpecialAttackState( ioBaseChar *pOwner )
{
	m_FireState = FS_SPECIAL_ATTACK;

	if( pOwner->CheckMagicCircle() )
		pOwner->UpdateMagicCirclePos( m_fMoveSpeed, m_fRange, m_fHeightGap, true, true );
	
	ioEntityGroup *pGrp = pOwner->GetGroup();

	int iCurSkillAniID = pGrp->GetAnimationIdx( m_SpecialAttack );
	if( iCurSkillAniID == -1 )	return;

	DWORD dwCurTime = FRAMEGETTIME();

	pGrp->ClearAllLoopAni( FLOAT100, true );
	pGrp->ClearAllActionAni( FLOAT100, true );
	pGrp->SetActionAni( iCurSkillAniID, FLOAT100, FLOAT100, FLOAT1, FLOAT1/m_fSpecialAttackAniRate );
	
	DWORD dwDuration = pGrp->GetAnimationFullTime( iCurSkillAniID ) * m_fSpecialAttackAniRate;
	m_dwSpecialEndTime = dwCurTime + dwDuration;
	m_dwSpecialFireTime = dwCurTime + pGrp->GetAnimationEventTime_e( iCurSkillAniID, "fire_s" ) * m_fSpecialAttackAniRate;

	if( pOwner->IsNeedSendNetwork() )
	{
		int iSkillPointSize = m_vSpecialWeaponPosList.size();

		SP2Packet kPacket( CUPK_ITEM_GAUGE_OK );
		FillItemGaugePacketHeader( pOwner, kPacket );
		kPacket << (int)m_FireState;
		kPacket << pOwner->GetWorldPosition();
		kPacket << pOwner->GetTargetRot();
		kPacket << iSkillPointSize;
		for( int i = 0; i < iSkillPointSize; i++ )//중간 참여자를 위해서 결과는 다시 처음부터 보낸다.
			kPacket << m_vSpecialWeaponPosList[i];

		P2PNetwork::SendToAllPlayingUser( kPacket );
	}
}

void ioMagicStaffItem::AddSpecialAttackPosition( ioBaseChar *pChar, ioPlayStage *pStage )
{
	if( !pChar )	return;
	if( !pStage )	return;

	D3DXVECTOR3 vCirclePos;
	if( m_iCurSpecialWeaponCnt < m_iMaxSpecialWeaponCnt )
	{
		vCirclePos = pChar->GetMagicCirclePos( m_fCircleOffSet, false );
		m_vSpecialWeaponPosList.push_back( vCirclePos );
	}
	
	m_iCurSpecialWeaponCnt++;

	if( !m_ReadyCircle.IsEmpty() )
	{	
		D3DXVECTOR3 vScale = D3DXVECTOR3( FLOAT1, FLOAT1, FLOAT1 );
		ioMapEffect *pMapEffect = pStage->CreateMapEffect( m_ReadyCircle, vCirclePos, vScale );
		if( pMapEffect )
		{
			DWORD dwID = pMapEffect->GetUniqueID();
			m_dwEnableCircleIDList.push_back(dwID);
		}
	}
}

void ioMagicStaffItem::SetInputAttackInfo( ioBaseChar *pOwner, int iAniID, float fTimeRate )
{
	m_dwInputAttackTime = 0;

	ioEntityGroup *pGrp = pOwner->GetGroup();
	if( !pGrp )
		return;

	DWORD dwCurTime = FRAMEGETTIME();
	DWORD dwAniTime = pGrp->GetAnimationEventTime_e( iAniID, "input_attack" ) * fTimeRate;
	if( dwAniTime != 0 )
		m_dwInputAttackTime = dwCurTime + dwAniTime;
}

void ioMagicStaffItem::SetInputAttackState( ioBaseChar *pOwner, bool bSendNet, int iState )
{
	if( !pOwner )
		return;

	ioEntityGroup *pGrp = pOwner->GetGroup();
	if( !pGrp )
		return;

	pOwner->SetAttackMoveEnable( false );
	pOwner->SetChargingState( false );
	pOwner->SetState( CS_MAGICSTAFF_SPECIAL );
	m_FireState = FS_INPUT_ATTACK;
	m_iInputAttackState = iState;

	AttackAttribute cInfo;
	switch( m_iInputAttackState )
	{
	case IAS_BASE_A:	cInfo = m_cInputAttributeA;	break;
	case IAS_BASE_S:	cInfo = m_cInputAttributeS;	break;
	case IAS_DASH_A:	cInfo = m_cDashInputAttributeA;	break;
	case IAS_DASH_S:	cInfo = m_cDashInputAttributeS;	break;
	case IAS_JUMP_A:	cInfo = m_cJumpInputAttributeA;	break;
	case IAS_JUMP_S:	cInfo = m_cJumpInputAttributeS;	break;
	}

	if( pOwner->IsNeedProcess() )
	{
		ioUserKeyInput::DirKeyInput eNewDirKey = pOwner->CheckCurDirKey();
		if( eNewDirKey != ioUserKeyInput::DKI_NONE )
			pOwner->SetTargetRotToDirKey( eNewDirKey );
	}

	DWORD dwCurTime = FRAMEGETTIME();

	m_szCurAni = cInfo.m_AttackAnimation;
	m_dwInputCancelCheckTime = dwCurTime;

	pOwner->SetAutoTarget( ATT_NORMAL );
	pOwner->SetNormalAttackByAttribute( cInfo, true );

	int iAniID = pGrp->GetAnimationIdx( cInfo.m_AttackAnimation );
	float fTimeRate = cInfo.m_fAttackAniRate;
	DWORD dwPreDelay = cInfo.m_dwPreDelay;

	pOwner->SetBaseAttackExtraAniJump( iAniID, fTimeRate, dwPreDelay );

	m_dwInputAttackEndTime = dwCurTime + ( pGrp->GetAnimationFullTime( iAniID ) * cInfo.m_fAttackAniRate );
	CheckDecreaseGauge();

	if( bSendNet && pOwner->IsNeedSendNetwork() )
	{
		SP2Packet kPacket( CUPK_ITEM_GAUGE_OK );
		FillItemGaugePacketHeader( pOwner, kPacket );
		kPacket << m_FireState;
		kPacket << pOwner->GetWorldPosition();
		kPacket << pOwner->GetTargetRot();
		kPacket << m_iInputAttackState;
		P2PNetwork::SendToAllPlayingUser( kPacket );
	}
}

bool ioMagicStaffItem::CheckInputAttack()
{
	DWORD dwCurTime = FRAMEGETTIME();
	if( m_dwInputAttackTime > 0 && m_dwInputAttackTime < dwCurTime && IsEnableGauge() )
		return true;

	return false;
}
//////////////////////////////////////////////////////////////////////////
ioMagicStaffSpecialState::ioMagicStaffSpecialState()
{
}

ioMagicStaffSpecialState::~ioMagicStaffSpecialState()
{
}

void ioMagicStaffSpecialState::Initialize( ioBaseChar* pOwner, CharState eCurrState )
{
	ioSpecialStateBase::Initialize( pOwner, eCurrState );
}

void ioMagicStaffSpecialState::Finalize( ioBaseChar* pOwner, CharState eCurrState, CharState eNewState )
{
	ioSpecialStateBase::Finalize( pOwner, eCurrState, eNewState );

	ioMagicStaffItem *pStaff = ToMagicStaffItem( pOwner->GetPriorityItem( SPT_ATTACK ) );
	if( pStaff )
		pStaff->ClearAttackState( pOwner );
}

void ioMagicStaffSpecialState::CheckSpecialState( ioBaseChar* pOwner )
{
	ioMagicStaffItem *pStaff = ToMagicStaffItem( pOwner->GetPriorityItem( SPT_ATTACK ) );
	if( pStaff )
		pStaff->ProcessSpecialState( pOwner );
	else
	{
		pOwner->SetState( CS_DELAY );
		pOwner->SetFallState( true );
		return;
	}
}

void ioMagicStaffSpecialState::CheckFallStateChange( ioBaseChar* pOwner, float fMinFallHeightRate, float fBottomHeight, bool bBottomMaxCheck, float fMaxBottomHeight )
{
	return;
}