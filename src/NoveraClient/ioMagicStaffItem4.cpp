

#include "stdafx.h"

#include "ioMagicStaffItem4.h"

#include "ioBaseChar.h"
#include "ioPlayStage.h"

#include "WeaponDefine.h"
#include "ItemDefine.h"

#include "ioGrowthUpInfo.h"

ioMagicStaffItem4::ioMagicStaffItem4()
{
	m_dwTimeGap = 0;
	m_dwChargeStartTime = 0;
	m_iCurCombo = 0;
	m_FireState = FS_NONE;

	m_iCurAirJump = 0;
	m_fAirJumpPowerRate = 0.0f;
	m_vAirJumpTimeList.clear();
}

ioMagicStaffItem4::ioMagicStaffItem4( const ioMagicStaffItem4 &rhs )
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
	m_fMoveSpeedRateExtend( rhs.m_fMoveSpeedRateExtend ),
	m_ExtendMoveFront( rhs.m_ExtendMoveFront ),
	m_fExtendMoveAniRate( rhs.m_fExtendMoveAniRate ),
	m_fMagicCircleOffset( rhs.m_fMagicCircleOffset ),
	m_FinishAttribute( rhs.m_FinishAttribute ),
	m_vExtendBuffList( rhs.m_vExtendBuffList )
{
	m_dwTimeGap = 0;
	m_dwChargeStartTime = 0;
	m_iCurCombo = 0;
	m_FireState = FS_NONE;

	m_iCurAirJump = 0;
	m_fAirJumpPowerRate = 0.0f;
	m_vAirJumpTimeList.clear();

	m_bFirstExtendMoveMotion = true;
	m_bExtendMotionSetted = false;
	m_ExtendStartDirKey = ioUserKeyInput::DKI_NONE;

	m_bChangeDir = false;

	m_dwFinishAttackEndTime = 0;

	m_vExtendAttackPos = ioMath::VEC3_ZERO;

	m_bChargeAttacked = false;
}

ioMagicStaffItem4::~ioMagicStaffItem4()
{
	m_vAirJumpTimeList.clear();
}

void ioMagicStaffItem4::LoadProperty( ioINILoader &rkLoader )
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
	m_fMoveSpeedRateExtend = rkLoader.LoadFloat_e( "extend_move_speed_rate", 0.f );

	m_fExtendMoveAniRate = rkLoader.LoadFloat_e( "extend_move_ani_rate", FLOAT1 );

	rkLoader.LoadString_e( "extend_move_front", "", szBuf, MAX_PATH );
	m_ExtendMoveFront = szBuf;

	m_fMagicCircleOffset = rkLoader.LoadFloat_e( "magic_circle_offset", 0.f );

	wsprintf_e( szBuf, "extend_finish_attack" );
	LoadAttackAttribute( szBuf, m_FinishAttribute, rkLoader );

	//Charge Buff
	int nBuffCnt = rkLoader.LoadInt_e( "max_extend_buff_cnt", 0 );
	for( int i = 0; i < nBuffCnt; i++ )
	{
		wsprintf_e( szKey, "extend_buff%d", i+1 );
		rkLoader.LoadString( szKey, "", szBuf, MAX_PATH );

		ioHashString szBuff = szBuf;
		m_vExtendBuffList.push_back( szBuff );
	}
}

ioItem* ioMagicStaffItem4::Clone()
{
	return new ioMagicStaffItem4( *this );
}

void ioMagicStaffItem4::SetNormalAttack( ioBaseChar *pOwner, int iCurCombo )
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

		m_bChangeDir = false;
	}

	m_dwFinishAttackEndTime = 0;
	pOwner->SetAttackMoveEnable( false );
	m_bChargeAttacked = false;
}

void ioMagicStaffItem4::ChangeToGathering( ioBaseChar *pOwner )
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
	pOwner->SetAttackMoveEnable( true );
	pOwner->ChangeDirectionExplicitIfDirChanged( true );

	m_ExtendStartDirKey = pOwner->GetCurDirKey();

	m_bFirstExtendMoveMotion = true;
	m_bExtendMotionSetted = true;

	// add buff
	for( int j=0; j < (int)m_vExtendBuffList.size(); ++j )
	{
		ioHashString szBuffName = m_vExtendBuffList[j];
		pOwner->AddNewBuff( szBuffName, pOwner->GetCharName(), GetName(), NULL );
	}

	if( pOwner->IsNeedSendNetwork() )
	{
		SP2Packet kPacket( CUPK_ITEM_GAUGE_CANCEL );
		FillItemGaugePacketHeader( pOwner, kPacket );
		kPacket << (int)m_FireState;
		kPacket << m_iCurCombo;
		P2PNetwork::SendToAllPlayingUser( kPacket );
	}
}

void ioMagicStaffItem4::CheckNormalAttack( ioBaseChar *pOwner )
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
	case FS_EXTEND_ATTACK:
		OnExtendAttack( pOwner );
		break;
	}

	CheckExtendMoveAniState( pOwner );
}

void ioMagicStaffItem4::OnCharging( ioBaseChar *pOwner )
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

void ioMagicStaffItem4::OnGathering( ioBaseChar *pOwner )
{
	if( !pOwner || !pOwner->IsNeedProcess() )
		return;

	if( !pOwner->IsAttackKeyDown() )	// 버튼 뗌
	{
		ChangeExtendAttack( pOwner );
		return;
	}

	ioPlayStage* pStage = pOwner->GetCreator();
	if( !pStage )
		return;

	float fCurRange = m_fRange;
	float fValue = 0.0f;
	ioGrowthNormalUpInfo *pUpInfo = ToNormalUpInfo( GetGrowthUpInfoByType(GT_MAGIC_CIRCLE_RANGE, pOwner->GetCharName()) );
	if( pUpInfo )
	{
		ioPlayStage *pStage = m_pCreator->GetPlayStage();
		fValue = pUpInfo->GetValue(pStage);
	}

	fCurRange += fValue;


	D3DXVECTOR3 vLook = pOwner->GetTargetRot() * D3DXVECTOR3( 0.0f, 0.0f, -FLOAT1 );
	D3DXVec3Normalize( &vLook, &vLook );

	D3DXVECTOR3 vPos = pOwner->GetWorldPosition()  + (vLook * m_fMagicCircleOffset);

	if( pOwner->IsDefenseKeyDown() )
	{
		vPos.y = 0;
		vPos.y = pStage->GetMapHeight( vPos.x, vPos.z );
		if( vPos.y <= 0.0f )
			vPos.y = pOwner->GetBottomHeight();

		if( pOwner->GetUseEnableSkill( 0, SUT_NORMAL ) )
		{
			pOwner->SetMagicCirclePos( false, vPos );
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
		vPos.y = 0;
		vPos.y = pStage->GetMapHeight( vPos.x, vPos.z );
		if( vPos.y <= 0.0f )
			vPos.y = pOwner->GetBottomHeight();

		pOwner->SetMagicCirclePos( false, vPos );
		if( pOwner->GetUseEnableSkill( 0, SUT_NORMAL ) )
			pOwner->UpdateMagicCirclePos( m_fMoveSpeed, fCurRange, m_fHeightGap, true );
		else
			pOwner->UpdateMagicCirclePos( m_fMoveSpeed, fCurRange, m_fHeightGap, false );
	}
}


void ioMagicStaffItem4::OnExtendAttack( ioBaseChar *pOwner )
{
	DWORD dwCurTime = FRAMEGETTIME();

	if( m_dwFinishAttackEndTime > 0 && m_dwFinishAttackEndTime < dwCurTime )
	{
		m_dwFinishAttackEndTime = 0;

		// 예약처리
		pOwner->NormalAttackOver();

		// 공격키가 눌러져 있으면 공격 상태 처리
		/*if( pOwner->IsAttackKeyDown() && 
			!pOwner->IsJumpKeyDown() &&
			!pOwner->IsDefenseKeyDown() &&
			!pOwner->IsChatModeState(true) )
		{
			//ClearState();

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
		}*/

		// 필살기 예약되었으면 필살기 발동
		if( m_iSkillInput >= 0 && !pOwner->IsBuffLimitSkill() && pOwner->SetUseSkill( m_iSkillInput, SUT_NORMAL ) )
		{
			//ClearState();
			m_iSkillInput = -1;
			return;
		}

		// 방어
		if( m_bEnableDefenseState && pOwner->IsEnableDefenseByObject() && !pOwner->IsBuffLimitDefense() )
		{
			//ClearState();
			pOwner->SetDefenseState();
			return;
		}

		// 점프
		if( m_bEnableJumpState && pOwner->IsCanJumpState() && !pOwner->IsBuffLimitJump() )
		{
			//ClearState();
			pOwner->SetJumpState();
			return;
		}

		// 공격
		if( m_bEnableAttackState )
		{
			//ClearState();
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

		//ClearState();
		pOwner->SetState( CS_DELAY, false );
		return;
	}
	else if( m_dwKeyReserveTime > 0 && m_dwKeyReserveTime < dwCurTime )
	{
		// 예약체크
		CheckReserve( pOwner );
	}
}


void ioMagicStaffItem4::ChangeExtendAttack( ioBaseChar *pOwner )
{
	if( !pOwner ) return;

	pOwner->SetAttackMoveEnable( false );

	m_FireState = FS_EXTEND_ATTACK;

	if( pOwner->IsNeedProcess() )
		m_vExtendAttackPos = pOwner->GetMagicCirclePos( 0.f, 0.f );
	
	pOwner->DestroyMagicCircle();
	pOwner->EndEffect( m_GatherEffect );


	DWORD dwCurTime = FRAMEGETTIME();
	m_dwFinishAttackEndTime = dwCurTime;

	m_fCurChargeRateForSkill = 0.0f;

	pOwner->SetChargingState( false );

	m_dwKeyReserveTime = 0;
	ioEntityGroup *pGrp = pOwner->GetGroup();
	if( pGrp )
	{
		AttackAttribute stFinishAttribute;
		stFinishAttribute.Init();
		stFinishAttribute = m_FinishAttribute;

		pOwner->InitActionStopDelay( true );

		pOwner->ClearAttackFireTimeAndSkill();

		int iAniID = pGrp->GetAnimationIdx( stFinishAttribute.m_AttackAnimation );

		float fCurRate = stFinishAttribute.m_fAttackAniRate;
		if( fCurRate <= 0.0f )
			fCurRate = FLOAT1;

		DWORD dwPreDelay = stFinishAttribute.m_dwPreDelay;
		DWORD dwEndDelay = stFinishAttribute.m_dwEndDelay;

		pGrp->ClearAllActionAni( FLOAT100 );
		pGrp->ClearAllLoopAni( FLOAT100 );
		pGrp->SetActionAni( iAniID, FLOAT100, FLOAT100, FLOAT1, FLOAT1 / fCurRate, 0.0f, dwPreDelay, true );

		pOwner->RefreshFireTimeList( iAniID, stFinishAttribute.m_vWeaponInfoList, FTT_NORMAL_ATTACK, fCurRate, dwPreDelay );
		pOwner->IncreaseWeaponIndexBase();
		pOwner->SetReservedSliding( stFinishAttribute.m_vForceInfoList, iAniID, fCurRate, 0 );

		m_bChargeAttacked = true;

		m_dwFinishAttackEndTime += pGrp->GetAnimationFullTime(iAniID) * fCurRate;
		m_dwFinishAttackEndTime += dwEndDelay;

		DWORD dwKeyReserve = pGrp->GetAnimationEventTime_e( iAniID, "key_reserve" ) * fCurRate;
		if( dwKeyReserve > 0 )
			m_dwKeyReserveTime = dwCurTime + dwKeyReserve + dwPreDelay;
	}

	if( pOwner->IsNeedSendNetwork() )
	{
		SP2Packet kPacket( CUPK_ITEM_GAUGE_CANCEL );
		FillItemGaugePacketHeader( pOwner, kPacket );
		kPacket << (int)m_FireState;
		kPacket << m_iCurCombo;
		kPacket << m_vExtendAttackPos;
		P2PNetwork::SendToAllPlayingUser( kPacket );
	}
}


void ioMagicStaffItem4::ChangeToNormalAttack( ioBaseChar *pOwner )
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

void ioMagicStaffItem4::OnItemGaugeCancel( ioBaseChar *pOwner, SP2Packet &rkPacket )
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
	case FS_EXTEND_ATTACK:
		rkPacket >> m_vExtendAttackPos;
		ChangeExtendAttack( pOwner );
		break;
	}
}

ioWeaponItem::WeaponSubType ioMagicStaffItem4::GetSubType() const
{
	return WST_MAGIC_STAFF4;
}

void ioMagicStaffItem4::OnEndExtendAttack( ioBaseChar *pOwner, bool bReleaseItem )
{
	pOwner->EndEffect( m_GatherEffect );

	int iBuffCnt = m_vExtendBuffList.size();
	for( int j=0; j < iBuffCnt; ++j )
	{
		ioHashString szBuffName = m_vExtendBuffList[j];
		pOwner->RemoveBuff( szBuffName );
	}
}

float ioMagicStaffItem4::GetCircleRadius()
{
	return m_fMagicCircleRadius;
}

bool ioMagicStaffItem4::IsChargeAttackState( ioBaseChar *pOwner )
{
	if( pOwner->GetState() == CS_ATTACK )
	{
		if( m_FireState == FS_GATHERING || m_FireState == FS_CHARGING )
			return true;
	}

	return false;
}

void ioMagicStaffItem4::UpdateSkillGauge( ioBaseChar *pOwner, float fRate, bool bUpdate )
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

bool ioMagicStaffItem4::IsEnableExtraGauge( ioBaseChar *pChar )
{
	if( m_bGaugeUpdateChargeAttack && IsChargeAttackState( pChar ) )
		return false;

	return true;
}

bool ioMagicStaffItem4::SetJumpAttack( ioBaseChar *pOwner, int iAniID, float fTimeRate, AttackType eType )
{
	if( !pOwner ) return false;

	DWORD dwCurTime = FRAMEGETTIME();
	ClearAirJump();
	AttackType eExtendType = AT_JUMP_ATTACK;

	const AttackAttribute *pAttr = GetAttackAttribute( eExtendType );
	if( !pAttr )
	{
		LOG.PrintTimeAndLog( 0, "ioMagicStaffItem4::SetJumpAttack - AttackAttribute Not Exist" );
		return false;
	}

	ioEntityGroup *pGrp = pOwner->GetGroup();
	iAniID = pGrp->GetAnimationIdx( pAttr->m_AttackAnimation );
	if( iAniID == -1 )
	{
		LOG.PrintTimeAndLog( 0, "ioMagicStaffItem4::SetJumpAttack - JumpAttack Animation Not Exist" );
		return false;
	}

	fTimeRate   = pAttr->m_fAttackAniRate;
	DWORD dwPreDelay  = pAttr->m_dwPreDelay;

	// For AirJumpPower
	SetAirJump( pOwner, iAniID, fTimeRate );

	return false;
}

void ioMagicStaffItem4::ProcessJumpAttack( ioBaseChar *pOwner, float fHeightGap )
{
	if( m_pExtendJump )
	{
		m_pExtendJump->ProcessJumpAttack( pOwner, fHeightGap );
	}

	CheckAirJump( pOwner );
}

void ioMagicStaffItem4::SetExtraInfoToWeapon( ioWeapon *pWeapon, bool bJumpAttack )
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

			if( m_bChargeAttacked )
			{
				pZone->SetPosition( m_vExtendAttackPos );
				m_bChargeAttacked = false;
			}
		}
	}
}


float ioMagicStaffItem4::GetAttackSpeedRate() const
{
	if( m_FireState == FS_GATHERING )
		return m_fMoveSpeedRateExtend;


	return FLOAT1;
}


void ioMagicStaffItem4::CheckExtendMoveAniState( ioBaseChar *pOwner )
{
	if( !pOwner->IsCanAttackMove() )	return;

	DWORD dwCurTime = FRAMEGETTIME();
	ioEntityGroup *pGrp = pOwner->GetGroup();
	int iAniID = 0;

	if( pOwner->IsSettedDirection() )
	{
		bool bChangeDir = pOwner->IsChangeDirection();
		if( bChangeDir || m_bFirstExtendMoveMotion )
		{
			int iAniID = 0;
			iAniID = pGrp->GetAnimationIdx(m_ExtendMoveFront);
			pGrp->ClearLoopAni( iAniID, FLOAT100 );
	
			m_bExtendMotionSetted = false;
			m_bFirstExtendMoveMotion = false;

			if( bChangeDir )
				m_bChangeDir = true;

			pGrp->SetLoopAni( m_ExtendMoveFront, FLOAT100, FLOAT1, FLOAT1/m_fExtendMoveAniRate );

			g_TutorialMgr.ActionAimingMove();
		}
		else
		{
			m_bChangeDir = false;
		}
	}
	else
	{
		if( !m_bExtendMotionSetted )
		{
			int iAniID = 0;
			iAniID = pGrp->GetAnimationIdx(m_ExtendMoveFront);
			pGrp->ClearLoopAni( iAniID, FLOAT100, true );

			m_bExtendMotionSetted = true;
			m_bChangeDir = false;
		}
	}
}