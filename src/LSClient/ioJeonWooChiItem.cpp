
#include "stdafx.h"

#include "ioBaseChar.h"
#include "ioPlayStage.h"

#include "WeaponDefine.h"
#include "ItemDefine.h"
#include "ioActiveCountBuff3.h"

ioJeonWooChiItem::ioJeonWooChiItem()
{
	m_ChargeState = CS_NONE;
	m_dwAttackStartTime = 0;
	m_iCurCombo = 0;

	//m_dwDodgeDefenseEnd = 0;

	m_iCurAirJump = 0;
	m_fAirJumpPowerRate = 0.0f;

	m_bCharged = false;
	m_bUsedExtraGauge = false;
}

ioJeonWooChiItem::ioJeonWooChiItem( const ioJeonWooChiItem &rhs )
	: ioWeaponItem( rhs ),
	m_AttackReadyAniList( rhs.m_AttackReadyAniList ),
	m_vExtendAttributeList( rhs.m_vExtendAttributeList ),
	/*m_vDodgeAniList( rhs.m_vDodgeAniList ),
	m_fDodgeAniRate( rhs.m_fDodgeAniRate ),
	m_fDodgeForce( rhs.m_fDodgeForce ),
	m_fDodgeFrictionRate( rhs.m_fDodgeFrictionRate ),*/
	m_bEnableExtraGauge( rhs.m_bEnableExtraGauge ),
	m_ExtraGaugeBuffList( rhs.m_ExtraGaugeBuffList ),
	m_fNeedGauge( rhs.m_fNeedGauge ),
	m_stChargeAttackCheckBuff( rhs.m_stChargeAttackCheckBuff ),
	m_ChargeFireAttack( rhs.m_ChargeFireAttack ),
	m_stChargeFireAreaWeapon( rhs.m_stChargeFireAreaWeapon ),
	m_fChargeFireFindAngle( rhs.m_fChargeFireFindAngle ),
	m_fChargeFireRange( rhs.m_fChargeFireRange ),
	m_fAreaWeaponFrontOffSet( rhs.m_fAreaWeaponFrontOffSet ),
	m_fAreaWeaponSideOffSet( rhs.m_fAreaWeaponSideOffSet ),
	m_fAreaWeaponHeightOffSet( rhs.m_fAreaWeaponHeightOffSet ),
	m_fChargeBuffCheckRange( rhs.m_fChargeBuffCheckRange ),
	m_stBuffAttackMotion( rhs.m_stBuffAttackMotion ),
	m_fBuffAttackMotionRate( rhs.m_fBuffAttackMotionRate ),
	m_stChargeAttackBuff( rhs.m_stChargeAttackBuff ),
	m_stSMoveOwnerBuff( rhs.m_stSMoveOwnerBuff ),
	m_fSMoveSearchRange( rhs.m_fSMoveSearchRange ),
	m_stSMoveStep1Motion( rhs.m_stSMoveStep1Motion ),
	m_fSMoveStep1MotionRate( rhs.m_fSMoveStep1MotionRate ),
	m_stSMoveStep2Motion( rhs.m_stSMoveStep2Motion ),
	m_fSMoveStep2MotionRate( rhs.m_fSMoveStep2MotionRate ),
	m_stSMoveDummyName( rhs.m_stSMoveDummyName ),
	m_fSMoveStep2TeleportOffSet( rhs.m_fSMoveStep2TeleportOffSet ),
	m_fSMoveEndJumpPower( rhs.m_fSMoveEndJumpPower )
{
	m_ChargeState = CS_NONE;
	m_dwAttackStartTime = 0;
	m_iCurCombo = 0;

	//m_dwDodgeDefenseEnd = 0;

	m_iCurAirJump = 0;
	m_fAirJumpPowerRate = 0.0f;

	m_bCharged = false;
	m_bUsedExtraGauge = false;
}

ioJeonWooChiItem::~ioJeonWooChiItem()
{
}

void ioJeonWooChiItem::LoadProperty( ioINILoader &rkLoader )
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

	m_bEnableExtraGauge = rkLoader.LoadBool_e( "extra_gauge_enable", false );

	m_ExtraGaugeBuffList.clear();
	int iBuffCnt = rkLoader.LoadInt_e( "extra_gauge_buff_cnt", 0 );
	for( int i=0; i < iBuffCnt; ++i )
	{
		wsprintf_e( szKey, "extra_gauge_buff%d", i+1 );
		rkLoader.LoadString( szKey, "", szBuf, MAX_PATH );

		m_ExtraGaugeBuffList.push_back( szBuf );
	}

	//////////////////////////////////////////////////////////////////////////
	m_fNeedGauge = rkLoader.LoadFloat_e( "extra_need_gauge", FLOAT10 );

	//////////////////////////////////////////////////////////////////////////
	//D~ 주변검색용 버프
	rkLoader.LoadString_e( "charge_attack_check_buff", "", szBuf, MAX_PATH );
	m_stChargeAttackCheckBuff = szBuf;
	m_fChargeBuffCheckRange		= rkLoader.LoadFloat_e( "charge_attack_check_range",0.f );

	//주변에서 checkbuff유저에게 공격하기 위해 하는 모션
	rkLoader.LoadString_e( "charge_buff_find_attack", "", szBuf, MAX_PATH );
	m_stBuffAttackMotion = szBuf;
	m_fBuffAttackMotionRate = rkLoader.LoadFloat_e( "charge_buff_find_attack_rate", FLOAT1 );
	//모션후 공격 버프
	rkLoader.LoadString_e( "charge_attack_buff", "", szBuf, MAX_PATH );
	m_stChargeAttackBuff = szBuf;

	LoadAttackAttribute( "charge_extend_attack", m_ChargeFireAttack, rkLoader );
	rkLoader.LoadString_e( "charge_exrend_attack_area_weapon", "", szBuf, MAX_PATH );
	m_stChargeFireAreaWeapon = szBuf;
	m_fChargeFireFindAngle = rkLoader.LoadFloat_e( "charge_target_angle", 0.f );
	m_fChargeFireRange = rkLoader.LoadFloat_e( "charge_target_range", 0.f );

	m_fAreaWeaponFrontOffSet	= rkLoader.LoadFloat_e( "charge_area_weapon_offset_front", 0.f );
	m_fAreaWeaponSideOffSet		= rkLoader.LoadFloat_e( "charge_area_weapon_offset_side", 0.f );
	m_fAreaWeaponHeightOffSet	= rkLoader.LoadFloat_e( "charge_area_weapon_offset_height", 0.f );
	//////////////////////////////////////////////////////////////////////////
	rkLoader.LoadString_e( "s_move_owner_buff", "", szBuf, MAX_PATH );
	m_stSMoveOwnerBuff		= szBuf;
	m_fSMoveSearchRange		= rkLoader.LoadFloat_e( "s_move_search_range", 0.f );

	rkLoader.LoadString_e( "s_move_step1_motion", "", szBuf, MAX_PATH );
	m_stSMoveStep1Motion			= szBuf;
	m_fSMoveStep1MotionRate = rkLoader.LoadFloat_e( "s_move_step1_motion_rate", FLOAT1 );

	rkLoader.LoadString_e( "s_move_step2_motion", "", szBuf, MAX_PATH );
	m_stSMoveStep2Motion			= szBuf;
	m_fSMoveStep2MotionRate = rkLoader.LoadFloat_e( "s_move_step2_motion_rate", FLOAT1 );

	m_fSMoveStep2TeleportOffSet = rkLoader.LoadFloat_e( "s_move_step2_teleport_offset", -FLOAT100 );

	rkLoader.LoadString_e( "s_move_owner_dummy", "", szBuf, MAX_PATH );
	m_stSMoveDummyName = szBuf;

	m_fSMoveEndJumpPower = rkLoader.LoadFloat_e( "s_move_end_jump_power", 0.f );
}

ioItem* ioJeonWooChiItem::Clone()
{
	return new ioJeonWooChiItem( *this );
}

ioWeaponItem::WeaponSubType ioJeonWooChiItem::GetSubType() const
{
	return WST_JEONWOOCHI_ITEM;
}

void ioJeonWooChiItem::OnReleased( ioBaseChar *pOwner )
{
	RemoveExtraGaugeBuff( pOwner );
	m_fCurExtraGauge = 0.0f;

	ioAttackableItem::OnReleased( pOwner );
}


bool ioJeonWooChiItem::CheckExtraStep( ioBaseChar *pOwner )
{
	if( pOwner->IsNeedProcess() )
	{
		if( pOwner->IsDefenseKey() && Find_S_MoveTarget( pOwner ) )
		{
			Set_S_Move_Step1( pOwner, true );
			return true;
		}
	}

	return false;
}

void ioJeonWooChiItem::SetNormalAttack( ioBaseChar *pOwner, int iCurCombo )
{
	ClearCancelInfo();

	m_bCharged = false;

	int iExtendCnt = m_vExtendAttributeList.size();
	if( !COMPARE( iCurCombo, 0, iExtendCnt ) )
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
		m_dwAttackStartTime = FRAMEGETTIME();
		m_iCurCombo = iCurCombo;
		pOwner->SetState( CS_ATTACK );

		DWORD dwTrackingTime = FRAMEGETTIME();
		dwTrackingTime += pGrp->GetAnimationEventTime_e( iAniID, "tracking_e" ) * fTimeRate;
		pOwner->SetAutoTargetTrackingEndTime( dwTrackingTime );
	}
}

void ioJeonWooChiItem::CheckNormalAttack( ioBaseChar *pOwner )
{
	DWORD dwCurTime = FRAMEGETTIME();

	switch( m_ChargeState )
	{
	case CS_CHARGING:
		OnCharging( pOwner );
		break;
	case CS_ATTACK_FIRE:
		{
			if( m_bCharged )
				pOwner->CheckExtraAniJump();

			if( pOwner->IsNeedProcess() )
			{
				if( pOwner->IsDefenseKey() && Find_S_MoveTarget( pOwner ) )
				{
					Set_S_Move_Step1( pOwner, true );
					return;
				}
				
				if( m_bCharged )
				{
					if( pOwner->CheckExtendAttackEndJump() )
						return;
				}

				CheckCancelReserve( pOwner );

				if( ProcessCancel(pOwner) )
					return;
			}

			ioWeaponItem::CheckNormalAttack( pOwner );
		}
		break;
	case CS_CHARGE_ATTACK_FIRE:
		if( m_dwFireCreateTime && m_dwFireCreateTime < dwCurTime )
		{
			CreateAreaWeapon( pOwner );
			m_dwFireCreateTime = 0;
		}

		if( pOwner->IsNeedProcess() )
		{
			if( m_dwKeyReserveTime && m_dwKeyReserveTime < dwCurTime )
				CheckReserve( pOwner );

			if( dwCurTime > m_dwAttackEndTime )
				ProcessReserveKeyInput( pOwner );
			/*if( dwCurTime > m_dwAttackEndTime )
				pOwner->SetState( CS_DELAY, false );*/
		}
		//if( m_dwKeyReserveTime < dwCurTime )
			//CheckReserve( pOwner );

		break;
	case CS_CHARGE_BUFF_ATTACK_FIRE:
		if( pOwner->IsNeedProcess() )
		{
			if( m_dwKeyReserveTime && m_dwKeyReserveTime < dwCurTime )
				CheckReserve( pOwner );

			if( dwCurTime > m_dwAttackEndTime )
				ProcessReserveKeyInput( pOwner );
				//pOwner->SetState( CS_DELAY, false );
		}

		break;
		/*case CS_EXTEND_S_ATTACK_STEP1:
		if( pOwner->IsNeedProcess() )
		{
		if( dwCurTime > m_dwAttackEndTime )
		Set_S_Attack_Step2( pOwner );
		}
		break;
		case CS_EXTEND_S_ATTACK_STEP2:
		if( pOwner->IsNeedProcess() )
		{
		if( dwCurTime > m_dwAttackEndTime )
		pOwner->SetState( CS_DELAY );
		}
		break;*/
	}
}



void ioJeonWooChiItem::ProcessReserveKeyInput( ioBaseChar *pOwner )
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
				pOwner->SetNormalAttack( 0 );
			}
		}
		return;
	}

	// 대기
	pOwner->SetState( CS_DELAY );
}


void ioJeonWooChiItem::ProcessWooChiDashState( ioBaseChar *pOwner, float fHeightGap )
{

}


void ioJeonWooChiItem::ProcessWooChiMoveState( ioBaseChar *pOwner, float fHeightGap )
{
	if( !pOwner )
		return;

	ioPlayStage *pStage = pOwner->GetCreator();
	if( !pStage )
		return;

	DWORD dwCurTime = FRAMEGETTIME();

	switch( m_WooChiState )
	{
	case WMS_STEP1:
		if( pOwner->IsNeedProcess() && m_dwAttackEndTime < dwCurTime )
			Set_S_Move_Step2( pOwner );
		break;
	
	case WMS_STEP2:
		if( pOwner->IsNeedProcess() && m_dwAttackEndTime < dwCurTime )
		{
			D3DXVECTOR3 vPos = pOwner->GetWorldPosition();
			float fMapHeight = pStage->GetMapHeightOnlyTerrain( vPos.x, vPos.z );
			if( (fMapHeight +FLOAT10) < vPos.y )
				Set_S_Move_End( pOwner, true );
			else
				Set_S_Move_End( pOwner, false );
		}
			
		break;
	}
}


void ioJeonWooChiItem::Set_S_Move_End( ioBaseChar *pOwner, bool bJump )
{
	pOwner->SetCurMoveSpeed( 0.0f );
	pOwner->SetCurJumpPowerAmt( 0.0f );
	pOwner->SetGravityAmt( 0.0f );
	pOwner->SetForcePowerAmt( 0.0f );

	if(bJump)
	{
		pOwner->SetSKillEndJumpState( m_fSMoveEndJumpPower, false, false, false, true );
		pOwner->ClearDirDoubleClick();
	}
	else
	{
		pOwner->SetState( CS_DELAY, false );
	}

	m_ChargeState = CS_EXTEND_STATE;
	m_WooChiState = WMS_JUMP;
	//SendPacket
	if( pOwner->IsNeedProcess() )
	{
		SP2Packet kPacket( CUPK_ITEM_GAUGE_CANCEL );
		FillItemGaugePacketHeader( pOwner, kPacket );
		kPacket << (int)m_ChargeState;
		kPacket << (int)m_WooChiState;
		kPacket << pOwner->GetWorldPosition();
		kPacket << pOwner->GetTargetRot();
		kPacket << bJump;
		P2PNetwork::SendToAllPlayingUser( kPacket );
	}
}


void ioJeonWooChiItem::Set_S_Move_Step1( ioBaseChar *pOwner, bool bSendNet )
{
	if( !pOwner )
		return;

	ioEntityGroup *pGrp = pOwner->GetGroup();
	if( !pGrp )
		return;

	ioBaseChar *pTarget = pOwner->GetBaseChar( m_stSMoveTargetName );
	if( pTarget )
	{
		/*ioBuff *pTargetBuff = pTarget->GetBuff(m_stChargeAttackCheckBuff, pOwner->GetCharName() );
		if( pTargetBuff )
			pTargetBuff->SetReserveEndBuff();*/
		ioBuff *pTargetBuff = pTarget->GetBuff(m_stChargeAttackCheckBuff, pOwner->GetCharName() );
		ioActiveCountBuff3 *pActiveBuff3 = ToActiveCountBuff3( pTargetBuff );
		if( pActiveBuff3 )
			pActiveBuff3->CheckActiveCount();
	}

	DWORD dwCurTime = FRAMEGETTIME();

	pOwner->SetState( CS_JEONWOOCHI_EXTEND_MOVE );

	m_WooChiState = WMS_STEP1;
	m_ChargeState = CS_EXTEND_STATE;

	pGrp->ClearAllLoopAni( FLOAT100, true );
	pGrp->ClearAllActionAni( FLOAT100, true );

	int iAniID = pGrp->GetAnimationIdx( m_stSMoveStep1Motion );

	pGrp->SetActionAni( iAniID, FLOAT100, FLOAT100, FLOAT1, FLOAT1/m_fSMoveStep1MotionRate, 0.0f, 0.0f, true );
	pOwner->AddNewBuff( m_stSMoveOwnerBuff, pOwner->GetCharName(), "", NULL );

	m_dwAttackEndTime = dwCurTime;
	m_dwAttackEndTime  += ( pGrp->GetAnimationFullTime( iAniID ) * m_fSMoveStep1MotionRate );

	//SendPacket
	if( pOwner->IsNeedProcess() && bSendNet )
	{
		SP2Packet kPacket( CUPK_ITEM_GAUGE_CANCEL );
		FillItemGaugePacketHeader( pOwner, kPacket );
		kPacket << (int)m_ChargeState;
		kPacket << (int)m_WooChiState;
		kPacket << pOwner->GetWorldPosition();
		kPacket << pOwner->GetTargetRot();
		kPacket << m_stSMoveTargetName;
		P2PNetwork::SendToAllPlayingUser( kPacket );
	}

}


void ioJeonWooChiItem::Set_S_Move_Step2( ioBaseChar *pOwner )
{
	if( !pOwner )
		return;

	ioPlayStage *pStage = pOwner->GetCreator();
	if( !pStage )
		return;

	ioEntityGroup *pGrp = pOwner->GetGroup();
	if( !pGrp )
		return;

	DWORD dwCurTime = FRAMEGETTIME();
	D3DXVECTOR3 vRegPos = pOwner->GetWorldPosition();

	m_WooChiState = WMS_STEP2;
	m_ChargeState = CS_EXTEND_STATE;

	D3DXVECTOR3 vPos = pOwner->GetWorldPosition();
	int iCurIndex = g_DummyCharMgr.GetCurLocalIndex();
	float fMapHeight = pStage->GetMapHeight( vPos.x, vPos.z );
	if( fMapHeight > FLOAT1 )
	{
		vPos.y = fMapHeight;
		ioDummyChar *pDummy = pStage->CreateDummyChar( m_stSMoveDummyName, iCurIndex, pOwner->GetCharName(), vPos, 0.f, 0 );
		if( pDummy )
		{
			pDummy->SetWorldOrientation( pOwner->GetWorldOrientation() );
		}
	}

	if( pOwner->IsNeedProcess() )
	{
		pOwner->SetWorldPosition( m_vSMovePos );
		pOwner->SetTargetRotToTargetPos( m_vSMoveLookPos, true );
	}

	pGrp->ClearAllLoopAni( FLOAT100, true );
	pGrp->ClearAllActionAni( FLOAT100, true );

	int iAniID = pGrp->GetAnimationIdx( m_stSMoveStep2Motion );

	pGrp->SetActionAni( iAniID, FLOAT100, FLOAT100, FLOAT1, FLOAT1/m_fSMoveStep2MotionRate, 0.0f, 0.0f, true );

	m_dwAttackEndTime = dwCurTime;
	m_dwAttackEndTime  += ( pGrp->GetAnimationFullTime( iAniID ) * m_fSMoveStep2MotionRate );

	//SendPacket
	if( pOwner->IsNeedProcess() )
	{
		SP2Packet kPacket( CUPK_ITEM_GAUGE_CANCEL );
		FillItemGaugePacketHeader( pOwner, kPacket );
		kPacket << (int)m_ChargeState;
		kPacket << (int)m_WooChiState;
		kPacket << vPos;
		kPacket << pOwner->GetWorldPosition();
		kPacket << pOwner->GetTargetRot();
		P2PNetwork::SendToAllPlayingUser( kPacket );
	}
}


/*bool ioJeonWooChiItem::IsNoDropState( ioBaseChar *pOwner )
{
	if( m_ChargeState == CS_EXTEND_S_ATTACK_STEP1 || m_ChargeState == CS_EXTEND_S_ATTACK_STEP2 )
		return true;

	return false;
}*/


bool ioJeonWooChiItem::Find_S_MoveTarget( ioBaseChar *pOwner )
{
	if( !pOwner || !pOwner->IsNeedProcess() ) 
		return false;

	ioPlayStage *pStage = pOwner->GetCreator();
	if ( !pStage )
		return false;

	//타겟 리스트
	const BaseCharList &rkCharList = pStage->GetBaseCharList();
	BaseCharList::const_iterator iter;
	D3DXVECTOR3 vCurPos = pOwner->GetWorldPosition();
	D3DXVECTOR3 vDiff;
	ioBaseChar *pFindTarget = NULL;
	float fMinLength = 999999.f;

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
		if( !pTarget->HasBuff( pOwner->GetCharName(), m_stChargeAttackCheckBuff) )
			continue;

		D3DXVECTOR3 vTargetPos = pTarget->GetWorldPosition();
		vDiff = vTargetPos - vCurPos;
		float fDiffRange = D3DXVec3Length( &vDiff );

		// 거리 체크
		if( fDiffRange < m_fSMoveSearchRange && fDiffRange < fMinLength )
		{
			fMinLength = fDiffRange;
			pFindTarget = pTarget;
		}
	}

	if( pFindTarget )
	{
		m_stSMoveTargetName = pFindTarget->GetCharName();

		m_vSMoveLookPos = pFindTarget->GetWorldPosition();
		m_vSMovePos = pFindTarget->GetWorldPosition() + ( (pFindTarget->GetWorldOrientation() * D3DXVECTOR3( 0.f, 0.f, -FLOAT1 )) * m_fSMoveStep2TeleportOffSet );
		float fMapHeight = pStage->GetMapHeight( m_vSMovePos.x, m_vSMovePos.z );
		if ( fMapHeight > 0 && m_vSMovePos.y < fMapHeight )
			m_vSMovePos.y = fMapHeight;

		return true;
	}

	return false;
}

void ioJeonWooChiItem::CreateAreaWeapon( ioBaseChar *pOwner )
{
	if( m_stChargeFireAreaWeapon.IsEmpty() ) 
		return;

	if( !pOwner )
		return;

	D3DXQUATERNION qtRot = Help::ConvertDirToQuaternion( m_vChargeFireDir );
	D3DXVECTOR3 vPos = pOwner->GetMidPositionByRate();
	D3DXVECTOR3 vScale = pOwner->GetWorldScale();
	D3DXVECTOR3 vTargetDir = pOwner->GetTargetRot() * D3DXVECTOR3( 0.0f, 0.0f, -FLOAT1 );
	D3DXVec3Normalize( &vTargetDir, &vTargetDir );

	D3DXVECTOR3 vSide;
	D3DXVec3Cross( &vSide, &vTargetDir, &ioMath::UNIT_Y );

	vPos += vTargetDir * m_fAreaWeaponFrontOffSet;
	vPos += vSide * m_fAreaWeaponSideOffSet;
	vPos.y += m_fAreaWeaponHeightOffSet;

	ioAreaWeapon *pArea = g_AreaWeaponMgr.CreateAreaWeapon( pOwner->GetCharName(), "", GetName(), m_stChargeFireAreaWeapon, vPos, qtRot, ioAreaWeapon::CT_NORMAL );

}

void ioJeonWooChiItem::OnCharging( ioBaseChar *pOwner )
{
	if( !pOwner->IsNeedProcess() )	
		return;

	if( pOwner->IsAttackKeyDown()/* && IsEnableGauge()*/ )
	{
		if( m_dwAttackStartTime + m_dwChargeTime <= FRAMEGETTIME() )
		{
			/*DecreaseBullet();
			ChangeToAttackFire( pOwner, true );
			g_TutorialMgr.ActionChargeAttack();*/
			CheckChargeFire( pOwner );
		}
	}
	else
	{
		//ChangeToAttackFire( pOwner, false );
		ChangeToNormalFire( pOwner );
	}
}


void ioJeonWooChiItem::ChangeToNormalFire( ioBaseChar *pOwner )
{
	if( !pOwner ) return;

	DWORD dwCurTime = FRAMEGETTIME();
	//m_bCharged = bCharged;

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

	m_ChargeState = CS_ATTACK_FIRE;

	if( pOwner->IsNeedSendNetwork() )
	{
		SP2Packet kPacket( CUPK_ITEM_GAUGE_OK );
		FillItemGaugePacketHeader( pOwner, kPacket );
		kPacket << m_iCurCombo;
		//kPacket << bCharged;
		kPacket << pOwner->GetTargetRot();
		P2PNetwork::SendToAllPlayingUser( kPacket );
	}
}


void ioJeonWooChiItem::CheckChargeFire( ioBaseChar *pOwner )
{
	ioPlayStage* pStage = pOwner->GetCreator();
	if( !pStage )
		return;

	D3DXVECTOR3 vCurPos = pOwner->GetMidPositionByRate();
	D3DXVECTOR3 vDiff= ioMath::VEC3_ZERO;

	const BaseCharList &rkCharList = pStage->GetBaseCharList();
	BaseCharList::const_iterator iter;

	ioHashString szTarget;
	float fCurGap = -FLOAT1;

	m_szAttackTargetVec.clear();

	for( iter=rkCharList.begin() ; iter!=rkCharList.end() ; ++iter )
	{
		ioBaseChar *pTarget = *iter;
		if( !pTarget ) continue;

		if( pTarget->GetTeam() == pOwner->GetTeam() ) continue;
		if( pTarget->GetCharName() == GetOwnerName() ) continue;
		if( pTarget->GetExperienceMode() != EMS_NONE ) continue;

		D3DXVECTOR3 vTargetPos = pTarget->GetMidPositionByRate();
		vDiff = vTargetPos - vCurPos;
		float fDiffRange = D3DXVec3Length( &vDiff );

		// 거리 체크
		if( fDiffRange < m_fChargeBuffCheckRange && pTarget->HasBuff( pOwner->GetCharName(), m_stChargeAttackCheckBuff ) )
		{
			szTarget = pTarget->GetCharName();
			m_szAttackTargetVec.push_back( szTarget );
		}
	}

	//타겟이 있으면
	if( m_szAttackTargetVec.size() )
	{
		SetChargeBuffAttack( pOwner );
	}
	else
	{
		if( IsEnableGauge() )
		{
			DecreaseBullet();
			ChangeToChargeFire( pOwner );
		}
		else
		{
			ChangeToNormalFire( pOwner );
		}
	}
}


void ioJeonWooChiItem::SetChargeBuffAttack( ioBaseChar *pOwner )
{
	if( !pOwner )
		return;

	DWORD dwCurTime = FRAMEGETTIME();

	ioEntityGroup *pGrp = pOwner->GetGroup();
	pGrp->ClearAllLoopAni( FLOAT100, true );
	pGrp->ClearAllActionAni( FLOAT100, true );

	int iAniID = pGrp->GetAnimationIdx( m_stBuffAttackMotion );
	if( iAniID <= 0 )
		return;

	m_ChargeState = CS_CHARGE_BUFF_ATTACK_FIRE;
	m_dwKeyReserveTime = 0;

	pGrp->SetActionAni( iAniID, FLOAT100, FLOAT100, FLOAT1, FLOAT1/m_fBuffAttackMotionRate, 0.0f, 0.0f, true );

	m_dwAttackEndTime = dwCurTime;
	m_dwAttackEndTime += ( pGrp->GetAnimationFullTime( iAniID ) * m_fBuffAttackMotionRate );

	//이제 찾은 유저에게 버프를 걸자
	ioBaseChar* pTarget;
	for( int i = 0; i < (int)m_szAttackTargetVec.size(); i++ )
	{
		ioHashString stName = m_szAttackTargetVec[i];
		pTarget = pOwner->GetBaseChar( m_szAttackTargetVec[i] );
		if( pTarget && pTarget->GetCharName() == stName )
			pTarget->AddNewBuff( m_stChargeAttackBuff, pOwner->GetCharName(), "", NULL );
	}

	m_dwKeyReserveTime = dwCurTime + (pGrp->GetAnimationEventTime_e( iAniID, "key_reserve" ) * m_fBuffAttackMotionRate);

	//SendPacket
	if( pOwner->IsNeedSendNetwork() )
	{
		SP2Packet kPacket( CUPK_ITEM_GAUGE_CANCEL );
		FillItemGaugePacketHeader( pOwner, kPacket );
		kPacket << (int)m_ChargeState;
		kPacket << pOwner->GetWorldPosition();
		kPacket << pOwner->GetTargetRot();
		kPacket << (int)m_szAttackTargetVec.size();
		for( int i = 0; i < (int)m_szAttackTargetVec.size(); i++ )
		{
			kPacket << m_szAttackTargetVec[i];
		}

		P2PNetwork::SendToAllPlayingUser( kPacket );
	}
}


void ioJeonWooChiItem::ChangeToChargeFire( ioBaseChar *pOwner )
{
	if( !pOwner )
		return;

	ioEntityGroup *pGrp = pOwner->GetGroup();
	if( !pGrp )
		return;

	if( pOwner->IsNeedProcess() )
		FindChargeFireTarget( pOwner );

	m_ChargeState = CS_CHARGE_ATTACK_FIRE;
	m_dwKeyReserveTime = 0;

	pOwner->InitExtendAttackTagInfo();

	pGrp->ClearAllLoopAni( FLOAT10, true );
	pGrp->ClearAllActionAni( FLOAT10, true );

	int iAniID	     = pGrp->GetAnimationIdx( m_ChargeFireAttack.m_AttackAnimation );

	if( iAniID <= 0 )
		return;

	pGrp->SetActionAni( iAniID, FLOAT100, FLOAT100, FLOAT1, FLOAT1/m_ChargeFireAttack.m_fAttackAniRate );

	//DWORD dwCurBaseIndex = pOwner->IncreaseWeaponIndexBase();
	/*pOwner->RefreshFireTimeListWithBase( dwCurBaseIndex,
		iAniID, m_ChargeFireAttack.m_vWeaponInfoList, FTT_NORMAL_ATTACK, m_ChargeFireAttack.m_fAttackAniRate, m_ChargeFireAttack.m_dwPreDelay );*/



	DWORD dwCurTime = FRAMEGETTIME();
	m_dwAttackEndTime = m_dwFireCreateTime = dwCurTime;
	m_dwAttackEndTime += ( pGrp->GetAnimationFullTime( iAniID ) * m_ChargeFireAttack.m_fAttackAniRate );

	m_dwFireCreateTime += pGrp->GetAnimationEventTime_e( iAniID, "fire_s" ) * m_ChargeFireAttack.m_fAttackAniRate;
	m_dwKeyReserveTime = dwCurTime + (pGrp->GetAnimationEventTime_e( iAniID, "key_reserve" ) * m_ChargeFireAttack.m_fAttackAniRate + m_ChargeFireAttack.m_dwPreDelay);

	//SendPacket
	if( pOwner->IsNeedSendNetwork() )
	{
		SP2Packet kPacket( CUPK_ITEM_GAUGE_CANCEL );
		FillItemGaugePacketHeader( pOwner, kPacket );
		kPacket << (int)m_ChargeState;
		kPacket << pOwner->GetWorldPosition();
		kPacket << pOwner->GetTargetRot();
		kPacket << m_vChargeFireDir;
		P2PNetwork::SendToAllPlayingUser( kPacket );
	}
}


void ioJeonWooChiItem::FindChargeFireTarget( ioBaseChar *pOwner )
{
	m_Target.Clear();

	if( !pOwner )
		return;

	ioPlayStage *pStage = pOwner->GetCreator();
	if( !pStage )
		return;

	D3DXVECTOR3 vCurPos = pOwner->GetMidPositionByRate();
	D3DXVECTOR3 vDiff= ioMath::VEC3_ZERO;

	float fCurAngle = ioMath::ArrangeHead( m_fChargeFireFindAngle );
	float fHalfValue = cosf( DEGtoRAD(fCurAngle*0.5f) );
	float fTargetRangeSq = m_fChargeFireRange * m_fChargeFireRange;

	const BaseCharList &rkCharList = pStage->GetBaseCharList();
	BaseCharList::const_iterator iter;

	ioHashString szTarget;
	float fCurGap = -FLOAT1;
	ioBaseChar* pFindTarget = NULL;

	for( iter=rkCharList.begin() ; iter!=rkCharList.end() ; ++iter )
	{
		ioBaseChar *pTarget = *iter;
		if( !pTarget ) continue;

		if( pTarget->GetTeam() == pOwner->GetTeam() ) continue;
		if( pTarget->GetCharName() == GetOwnerName() ) continue;
		if( pTarget->GetExperienceMode() != EMS_NONE ) continue;

		/*if( !IsEnableTargetState( pTarget ) )
			continue;*/

		D3DXVECTOR3 vTargetPos = pTarget->GetMidPositionByRate();
		vDiff = vTargetPos - vCurPos;
		float fDiffRangeSq = D3DXVec3LengthSq( &vDiff );

		// 각도 체크
		if( fCurAngle != 0.0f )
		{
			D3DXQUATERNION qtRot = pOwner->GetTargetRot();
			D3DXVECTOR3 vDir = qtRot * D3DXVECTOR3( 0.0f, 0.0f, -FLOAT1 );
			D3DXVec3Normalize( &vDir, &vDir );
			D3DXVECTOR3 vDiff2 = vDiff;
			vDiff2.y = 0.0f;
			D3DXVec3Normalize( &vDiff2, &vDiff2 );

			float fCurValue = D3DXVec3Dot( &vDir, &vDiff2 );
			if( fCurValue < fHalfValue )
				continue;
		}

		// 거리 체크
		if( fDiffRangeSq < fTargetRangeSq )
		{
			if( fCurGap == -FLOAT1 )
			{
				fCurGap = fDiffRangeSq;
				m_Target = pTarget->GetCharName();
				pFindTarget = pTarget;
			}
			else if( fCurGap >= 0.0f && fDiffRangeSq < fCurGap )
			{
				fCurGap = fDiffRangeSq;
				m_Target = pTarget->GetCharName();
				pFindTarget = pTarget;
			}
		}
	}

	if( pFindTarget )
	{
		m_vChargeFireDir = pFindTarget->GetMidPositionByRate() - pOwner->GetMidPositionByRate();
		D3DXVec3Normalize( &m_vChargeFireDir, &m_vChargeFireDir );	
		D3DXVECTOR3 vRotDir = m_vChargeFireDir;
		vRotDir.y = 0.0f;
		D3DXVec3Normalize( &vRotDir, &vRotDir );
		D3DXQUATERNION qtNewRot;
		ioMath::CalcDirectionQuaternion( qtNewRot, -vRotDir );
		pOwner->SetTargetRotToRotate( qtNewRot, true );
	}
	else
	{
		m_vChargeFireDir = pOwner->GetTargetRot() * D3DXVECTOR3( 0.0f, 0.0f, -FLOAT1 );
		D3DXVec3Normalize( &m_vChargeFireDir, &m_vChargeFireDir );
	}
}


/*void ioJeonWooChiItem::ChangeToAttackFire( ioBaseChar *pOwner, bool bCharged )
{
	if( !pOwner ) return;

	DWORD dwCurTime = FRAMEGETTIME();
	m_bCharged = bCharged;

	pOwner->InitExtendAttackTagInfo();

	ClearCancelInfo();

	if( m_bCharged )
	{
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
		{
			pOwner->ApplyNormalAttackResult( m_iCurCombo, true );
		}
		else
		{
			pOwner->ApplyNormalAttackResult( m_iCurCombo );
		}
	}
	else
	{
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
	}

	m_ChargeState = CS_ATTACK_FIRE;

	if( pOwner->IsNeedSendNetwork() )
	{
		SP2Packet kPacket( CUPK_ITEM_GAUGE_OK );
		FillItemGaugePacketHeader( pOwner, kPacket );
		kPacket << m_iCurCombo;
		kPacket << bCharged;
		kPacket << pOwner->GetTargetRot();
		P2PNetwork::SendToAllPlayingUser( kPacket );
	}
}*/

void ioJeonWooChiItem::OnItemGaugeOK( ioBaseChar *pOwner, SP2Packet &rkPacket )
{
	if( !pOwner ) return;

	rkPacket >> m_iCurCombo;

	D3DXQUATERNION qtRot;
	rkPacket >> qtRot;
	pOwner->SetTargetRot( qtRot );

	ChangeToNormalFire( pOwner );
}


void ioJeonWooChiItem::OnItemGaugeCancel( ioBaseChar *pOwner, SP2Packet &rkPacket )
{
	if( !pOwner )
		return;

	int nState;
	D3DXQUATERNION qtRot;
	D3DXVECTOR3 vPos;

	rkPacket >> nState;
	

	m_ChargeState = (ChargeState)nState;

	switch( m_ChargeState )
	{
	case CS_CHARGE_ATTACK_FIRE:
		rkPacket >> vPos;
		rkPacket >> qtRot;
		rkPacket >> m_vChargeFireDir;
		
		pOwner->SetWorldPosition( vPos );
		pOwner->SetTargetRot( qtRot );

		ChangeToChargeFire( pOwner );
		break;
	case CS_CHARGE_BUFF_ATTACK_FIRE:
		{
			rkPacket >> vPos;
			rkPacket >> qtRot;
			pOwner->SetWorldPosition( vPos );
			pOwner->SetTargetRot( qtRot );
			
			int nSize;
			ioHashString stName;

			rkPacket >> nSize;
			m_szAttackTargetVec.clear();
			
			for( int i = 0; i < nSize; i++ )
			{
				rkPacket >> stName;
				m_szAttackTargetVec.push_back( stName );
			}

			SetChargeBuffAttack( pOwner );
		}
		break;
	case CS_EXTEND_STATE:
		{
			int nWooChiState;
			rkPacket >> nWooChiState;
			m_WooChiState = (WooChiMoveState)nWooChiState;
			if( m_WooChiState == WMS_STEP1 )
			{
				rkPacket >> vPos;
				rkPacket >> qtRot;
				rkPacket >> m_stSMoveTargetName;

				pOwner->SetWorldPosition( vPos );
				pOwner->SetTargetRot( qtRot );

				Set_S_Move_Step1( pOwner, false );
			}
			else if( m_WooChiState == WMS_STEP2 )
			{
				D3DXVECTOR3 vRegPos;
				rkPacket >> vRegPos;
				rkPacket >> vPos;
				rkPacket >> qtRot;
				pOwner->SetWorldPosition( vRegPos );

				Set_S_Move_Step2( pOwner );

				pOwner->SetWorldPosition( vPos );
				pOwner->SetTargetRot( qtRot );
			}
			else if( m_WooChiState == WMS_JUMP )
			{
				bool bJump;
				rkPacket >> vPos;
				rkPacket >> qtRot;
				rkPacket >> bJump;
				pOwner->SetWorldPosition( vPos );
				pOwner->SetTargetRot( qtRot );

				Set_S_Move_End( pOwner, bJump );
			}
			/*
			kPacket << (int)m_ChargeState;
			kPacket << pOwner->GetWorldPosition();
			kPacket << pOwner->GetTargetRot();
			kPacket << m_stSMoveTargetName;
			kPacket << (int)m_WooChiState;
			*/
		}
		break;
	}
}

void ioJeonWooChiItem::SetExtraInfoToWeapon( ioWeapon *pWeapon, bool bJumpAttack )
{
	ioAttackableItem::SetExtraInfoToWeapon( pWeapon, bJumpAttack );


	ioUroborusWeapon4 *pUroborus4 = ToUroborusWeapon4( pWeapon );
	if( pUroborus4 )
	{
		ioBaseChar *pChar = pWeapon->GetOwner();
		if( pChar )
		{
			D3DXVECTOR3 vDir = pChar->GetTargetRot() * D3DXVECTOR3( 0, 0, -1 );
			D3DXVec3Normalize( &vDir, &vDir );

			if( !m_Target.IsEmpty() )
			{
				ioBaseChar *pTargetChar = pChar->GetBaseChar( m_Target );
				if( pTargetChar )
				{
					vDir = pTargetChar->GetWorldPosition() - pChar->GetWorldPosition();
					D3DXVec3Normalize( &vDir, &vDir );
				}
			}	

			pUroborus4->SetMoveDir( vDir );
		}
	}
	//
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

bool ioJeonWooChiItem::IsChargeAttackState( ioBaseChar *pOwner )
{
	if( pOwner->GetState() == CS_ATTACK )
	{
		if( m_ChargeState == CS_CHARGING )
			return true;
	}

	return false;
}

bool ioJeonWooChiItem::IsEnableAttackDashCheck( ioBaseChar *pOwner, bool bCheck )
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

/*bool ioJeonWooChiItem::SetDodgeDefenseState( ioBaseChar *pOwner )
{
	if( !pOwner ) return false;
	if( !pOwner->IsNeedProcess() ) return false;

	ioEntityGroup *pGrp = pOwner->GetGroup();
	if( !pGrp ) return false;

	m_dwKeyReserveTime = 0;

	m_iSkillInput = -1;
	m_bEnableDefenseState = false;
	m_bEnableAttackState = false;
	m_bEnableJumpState = false;

	int iAniID = -1;
	D3DXVECTOR3 vMoveDir = pOwner->GetTargetRot() * D3DXVECTOR3( 0.0f, 0.0f, -FLOAT1 );
	vMoveDir = -vMoveDir;

	if( pOwner->IsSettedDirection() )
	{
		ioUserKeyInput::DirKeyInput eDir = pOwner->CheckCurDirKey();
		if( eDir != ioUserKeyInput::DKI_NONE )
		{
			float fYawD = 180.0f + 45.0f * ( eDir - ioUserKeyInput::DKI_UP );
			fYawD = ioMath::ArrangeHead( fYawD );

			float fYawR = DEGtoRAD( fYawD );
			D3DXQUATERNION qtRotate;
			D3DXQuaternionRotationYawPitchRoll( &qtRotate, fYawR, 0.0f, 0.0f );

			vMoveDir = qtRotate * D3DXVECTOR3( 0.0f, 0.0f, -FLOAT1 );
		}
	}

	D3DXVECTOR3 vXZDir( vMoveDir.x, 0.0f, vMoveDir.z );
	D3DXVec3Normalize( &vXZDir, &vXZDir );

	D3DXQUATERNION qtTargetRot;
	ioMath::CalcDirectionQuaternion( qtTargetRot, -vXZDir );

	float fCurYaw, fTargetYaw;
	fTargetYaw = RADtoDEG( ioMath::QuaterToYaw( qtTargetRot ) );
	fTargetYaw = ioMath::ArrangeHead( fTargetYaw );
	fCurYaw = RADtoDEG( ioMath::QuaterToYaw( pOwner->GetTargetRot() ) );
	fCurYaw = ioMath::ArrangeHead( fCurYaw );

	float fAngleGap = ioMath::AngleGap( fCurYaw, fTargetYaw );
	fAngleGap += 360.0f;
	int iTest = (int)fAngleGap % 360;

	float fRate = 0.0f;
	int iIndex = -1;
	int iIndex2 = -1;
	iIndex = iTest / 90;
	iTest = iTest % 90;

	int iCnt = m_vDodgeAniList.size();
	if( COMPARE( iIndex, 0, iCnt ) )
	{
		fRate = (float)iTest /FLOAT90;
		iIndex2 = (iIndex+1) % iCnt;

		if( !m_vDodgeAniList[iIndex].IsEmpty() && !m_vDodgeAniList[iIndex2].IsEmpty() )
		{
			pGrp->ClearAllLoopAni( FLOAT100, true );
			pGrp->ClearAllActionAni( FLOAT100, true );

			if( fRate <= FLOAT05 )
			{
				iAniID = pGrp->GetAnimationIdx( m_vDodgeAniList[iIndex] );
				pGrp->SetActionAni( iAniID, FLOAT100, FLOAT100, FLOAT1, FLOAT1/m_fDodgeAniRate, 0.0f, 0, true );
			}
			else
			{
				iAniID = pGrp->GetAnimationIdx( m_vDodgeAniList[iIndex2] );
				pGrp->SetActionAni( iAniID, FLOAT100, FLOAT100, FLOAT1, FLOAT1/m_fDodgeAniRate, 0.0f, 0, true );
			}
		}
		else if( !m_vDodgeAniList[iIndex].IsEmpty() && m_vDodgeAniList[iIndex2].IsEmpty() )
		{
			pGrp->ClearAllLoopAni( FLOAT100, true );
			pGrp->ClearAllActionAni( FLOAT100, true );

			iAniID = pGrp->GetAnimationIdx( m_vDodgeAniList[iIndex] );
			pGrp->SetActionAni( iAniID, FLOAT100, FLOAT100, FLOAT1, FLOAT1/m_fDodgeAniRate, 0.0f, 0, true );
		}
		else if( !m_vDodgeAniList[iIndex2].IsEmpty() && m_vDodgeAniList[iIndex].IsEmpty() )
		{
			pGrp->ClearAllLoopAni( FLOAT100, true );
			pGrp->ClearAllActionAni( FLOAT100, true );

			iAniID = pGrp->GetAnimationIdx( m_vDodgeAniList[iIndex2] );
			pGrp->SetActionAni( iAniID, FLOAT100, FLOAT100, FLOAT1, FLOAT1/m_fDodgeAniRate, 0.0f, 0, true );
		}
		else
		{
			return false;
		}
	}
	else
	{
		return false;
	}
	//

	pOwner->SetState( CS_DODGE_DEFENSE );

	pOwner->SetForcePowerAmt( 0.0f );
	if( m_fDodgeForce > 0.0f )
		pOwner->SetForcePower( vMoveDir, fabs(m_fDodgeForce), m_fDodgeFrictionRate );
	else
		pOwner->SetForcePower( -vMoveDir, fabs(m_fDodgeForce), m_fDodgeFrictionRate );

	DWORD dwCurTime = FRAMEGETTIME();
	m_dwDodgeDefenseEnd = dwCurTime + pGrp->GetAnimationFullTime( iAniID ) * m_fDodgeAniRate;

	m_dwKeyReserveTime = pGrp->GetAnimationEventTime_e( iAniID, "key_reserve" ) * m_fDodgeAniRate;
	if( m_dwKeyReserveTime > 0 )
	{
		m_dwKeyReserveTime += dwCurTime;
	}

	pOwner->SetEnableHide( false );
	pOwner->SetEnableChangeTeam( false );

	if( pOwner->IsNeedSendNetwork() )
	{
		SP2Packet kPacket( CUPK_DODGE_DEFENSE );
		kPacket << pOwner->GetCharName();
		kPacket << pOwner->GetTargetRot();
		kPacket << vMoveDir;
		P2PNetwork::SendToAllPlayingUser( kPacket );
	}

	return true;
}

void ioJeonWooChiItem::ApplyDodgeDefenseState( ioBaseChar *pOwner, SP2Packet &rkPacket )
{
	if( !pOwner ) return;

	ioEntityGroup *pGrp = pOwner->GetGroup();
	if( !pGrp ) return;

	D3DXVECTOR3 vMoveDir;
	rkPacket >> vMoveDir;

	m_dwKeyReserveTime = 0;

	m_iSkillInput = -1;
	m_bEnableDefenseState = false;
	m_bEnableAttackState = false;
	m_bEnableJumpState = false;

	int iAniID = -1;
	D3DXVECTOR3 vXZDir( vMoveDir.x, 0.0f, vMoveDir.z );
	D3DXVec3Normalize( &vXZDir, &vXZDir );

	D3DXQUATERNION qtTargetRot;
	ioMath::CalcDirectionQuaternion( qtTargetRot, -vXZDir );

	float fCurYaw, fTargetYaw;
	fTargetYaw = RADtoDEG( ioMath::QuaterToYaw( qtTargetRot ) );
	fTargetYaw = ioMath::ArrangeHead( fTargetYaw );
	fCurYaw = RADtoDEG( ioMath::QuaterToYaw( pOwner->GetTargetRot() ) );
	fCurYaw = ioMath::ArrangeHead( fCurYaw );

	float fAngleGap = ioMath::AngleGap( fCurYaw, fTargetYaw );
	fAngleGap += 360.0f;
	int iTest = (int)fAngleGap % 360;

	float fRate = 0.0f;
	int iIndex = -1;
	int iIndex2 = -1;
	iIndex = iTest / 90;
	iTest = iTest % 90;

	int iCnt = m_vDodgeAniList.size();
	if( COMPARE( iIndex, 0, iCnt ) )
	{
		fRate = (float)iTest /FLOAT90;
		iIndex2 = (iIndex+1) % iCnt;

		if( !m_vDodgeAniList[iIndex].IsEmpty() && !m_vDodgeAniList[iIndex2].IsEmpty() )
		{
			pGrp->ClearAllActionAni( FLOAT100, true );

			if( fRate <= FLOAT05 )
			{
				iAniID = pGrp->GetAnimationIdx( m_vDodgeAniList[iIndex] );
				pGrp->SetActionAni( iAniID, FLOAT100, FLOAT100, FLOAT1, FLOAT1/m_fDodgeAniRate, 0.0f, 0, true );
			}
			else
			{
				iAniID = pGrp->GetAnimationIdx( m_vDodgeAniList[iIndex2] );
				pGrp->SetActionAni( iAniID, FLOAT100, FLOAT100, FLOAT1, FLOAT1/m_fDodgeAniRate, 0.0f, 0, true );
			}
		}
		else if( !m_vDodgeAniList[iIndex].IsEmpty() && m_vDodgeAniList[iIndex2].IsEmpty() )
		{
			pGrp->ClearAllActionAni( FLOAT100, true );

			iAniID = pGrp->GetAnimationIdx( m_vDodgeAniList[iIndex] );
			pGrp->SetActionAni( iAniID, FLOAT100, FLOAT100, FLOAT1, FLOAT1/m_fDodgeAniRate, 0.0f, 0, true );
		}
		else if( !m_vDodgeAniList[iIndex2].IsEmpty() && m_vDodgeAniList[iIndex].IsEmpty() )
		{
			pGrp->ClearAllActionAni( FLOAT100, true );

			iAniID = pGrp->GetAnimationIdx( m_vDodgeAniList[iIndex2] );
			pGrp->SetActionAni( iAniID, FLOAT100, FLOAT100, FLOAT1, FLOAT1/m_fDodgeAniRate, 0.0f, 0, true );
		}
		else
		{
			return;
		}
	}
	else
	{
		return;
	}
	//

	pOwner->SetState( CS_DODGE_DEFENSE );

	pOwner->SetForcePowerAmt( 0.0f );
	if( m_fDodgeForce > 0.0f )
		pOwner->SetForcePower( vMoveDir, fabs(m_fDodgeForce), m_fDodgeFrictionRate );
	else
		pOwner->SetForcePower( -vMoveDir, fabs(m_fDodgeForce), m_fDodgeFrictionRate );

	DWORD dwCurTime = FRAMEGETTIME();
	m_dwDodgeDefenseEnd = dwCurTime + pGrp->GetAnimationFullTime( iAniID ) * m_fDodgeAniRate;

	m_dwKeyReserveTime = pGrp->GetAnimationEventTime_e( iAniID, "key_reserve" ) * m_fDodgeAniRate;
	if( m_dwKeyReserveTime > 0 )
	{
		m_dwKeyReserveTime += dwCurTime;
	}

	pOwner->SetEnableHide( false );
	pOwner->SetEnableChangeTeam( false );
}

void ioJeonWooChiItem::CheckDodgeDefenseState( ioBaseChar *pOwner )
{
	if( !pOwner ) return;

	DWORD dwCurTime = FRAMEGETTIME();
	if( m_dwDodgeDefenseEnd > dwCurTime )
	{
		if( m_dwKeyReserveTime > 0 && m_dwKeyReserveTime < dwCurTime )
		{
			// 예약체크
			CheckReserve( pOwner );
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
		pOwner->SetState( CS_DELAY );
	}
}*/

void ioJeonWooChiItem::CheckReserve( ioBaseChar *pOwner )
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

bool ioJeonWooChiItem::ProcessCancel( ioBaseChar *pOwner )
{
	if( m_szCurAni.IsEmpty() )
		return false;

	if( !pOwner ) return false;

	ioEntityGroup *pGrp = pOwner->GetGroup();
	if( !pGrp ) return false;

	int iAniID = pGrp->GetAnimationIdx( m_szCurAni );

	std::list<float> vTimeList;
	pGrp->GetAnimationEventTimeList_e( iAniID, "input_jump_s", vTimeList );

	if( vTimeList.empty() )
		return false;

	DWORD dwCurTime = FRAMEGETTIME();
	std::list< float >::iterator iter = vTimeList.begin();
	while( iter != vTimeList.end() )
	{
		DWORD dwTime = (DWORD)*iter;
		dwTime *= m_fCurAniRate;

		dwTime += m_dwInputCancelCheckTime;

		if( m_dwCancelCheckTime > 0 && COMPARE( dwTime, m_dwCancelCheckTime, dwCurTime ) )
		{
			m_dwCancelCheckTime = dwTime+1;

			if( m_bCharged && pOwner->IsDirKeyDoubleClick() && !pOwner->HasBuff(BT_DISABLE_DASH) )
			{
				pOwner->ChangeDirectionByInputDir( false );
				pOwner->SetDashState( false );
				pOwner->SendDashState( false );

				return true;
			}
			else if( m_bEnableJumpState && !pOwner->IsBuffLimitJump() )
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

bool ioJeonWooChiItem::SetJumpAttack( ioBaseChar *pOwner, int iAniID, float fTimeRate, AttackType eType )
{
	if( !pOwner ) return false;

	DWORD dwCurTime = FRAMEGETTIME();
	ClearAirJump();

	AttackType eExtendType = AT_JUMP_ATTACK;

	const AttackAttribute *pAttr = GetAttackAttribute( eExtendType );
	if( !pAttr )
	{
		LOG.PrintTimeAndLog( 0, "ioJeonWooChiItem::SetJumpAttack - AttackAttribute Not Exist" );
		return false;
	}

	ioEntityGroup *pGrp = pOwner->GetGroup();
	iAniID = pGrp->GetAnimationIdx( pAttr->m_AttackAnimation );
	if( iAniID == -1 )
	{
		LOG.PrintTimeAndLog( 0, "ioJeonWooChiItem::SetJumpAttack - JumpAttack Animation Not Exist" );
		return false;
	}

	fTimeRate   = pAttr->m_fAttackAniRate;
	DWORD dwPreDelay  = pAttr->m_dwPreDelay;

	// For AirJumpPower
	SetAirJump( pOwner, iAniID, fTimeRate );

	return false;
}

void ioJeonWooChiItem::ProcessJumpAttack( ioBaseChar *pOwner, float fHeightGap )
{
	if( m_pExtendJump )
	{
		m_pExtendJump->ProcessJumpAttack( pOwner, fHeightGap );
	}

	CheckAirJump( pOwner );
}

void ioJeonWooChiItem::SetExtraGaugeTypeByWeapon( ioBaseChar *pOwner, int iType, int iWGT, float fDamage )
{
	if( !pOwner )
		return;

	if( !m_bEnableExtraGauge )
		return;

	float fTypeRate = 0.0f;
	switch( iType )
	{
	case EGT_ATTACK:
		if( iWGT == WGT_CHARGE_CONTACT )
		{
			if( m_bUsedExtraGauge )
				fTypeRate = m_fExtraGaugeAttackDec;
			else
				fTypeRate = m_fExtraGaugeAttack;
		}
		break;
	case EGT_ATTACK_DEF:
		if( iWGT == WGT_CHARGE_CONTACT )
		{
			if( m_bUsedExtraGauge )
				fTypeRate = m_fExtraGaugeAttackDefDec;
			else
				fTypeRate = m_fExtraGaugeAttackDef;
		}
		break;
	case EGT_WOUNDED:
		if( m_bUsedExtraGauge )
			fTypeRate = m_fExtraGaugeWoundDec;
		else
			fTypeRate = m_fExtraGaugeWound;
		break;
	case EGT_WOUNDED_DEF:
		if( m_bUsedExtraGauge )
			fTypeRate = m_fExtraGaugeWoundDefDec;
		else
			fTypeRate = m_fExtraGaugeWoundDef;
		break;
	}

	float fCurGauge = fTypeRate * fDamage;
	if( fTypeRate <= 0.0f )
		return;

	if( m_bUsedExtraGauge )
	{
		m_fCurExtraGauge -= fCurGauge;
		m_fCurExtraGauge = max( 0.0f, m_fCurExtraGauge );

		if( m_fCurExtraGauge <= 0.0f )
			RemoveExtraGaugeBuff( pOwner );
	}
	else
	{
		m_fCurExtraGauge += fCurGauge;
		m_fCurExtraGauge = min( m_fCurExtraGauge, m_fMaxExtraGauge );

		if( m_fCurExtraGauge >= m_fMaxExtraGauge )
			SetExtraGaugeBuff( pOwner );
	}
}

void ioJeonWooChiItem::SetUseExtraGauge( float fAmt )
{
	if( !m_bUsedExtraGauge )
		return;

	m_fCurExtraGauge -= fAmt;
	m_fCurExtraGauge = max( 0.0f, m_fCurExtraGauge );
}

int ioJeonWooChiItem::GetMaxBullet()
{
	return (int)m_fMaxExtraGauge;
}

int ioJeonWooChiItem::GetNeedBullet()
{
	return m_fNeedGauge;
}

int ioJeonWooChiItem::GetCurBullet()
{
	float fCurCoolTime = m_fCurExtraGauge;
	fCurCoolTime = max( 0, min( fCurCoolTime, m_fMaxExtraGauge ) );

	return (int)fCurCoolTime;
}


bool ioJeonWooChiItem::IsEnableGauge()
{
	if( m_fCurExtraGauge >= m_fNeedGauge )
		return true;

	return false;
}

void ioJeonWooChiItem::SetCurBullet( int iCurBullet )
{
	if( iCurBullet < 0 )
		return;

	float fCurBullet = iCurBullet;
	fCurBullet = max( 0, min( fCurBullet, m_fMaxExtraGauge ) );

	m_fCurExtraGauge = fCurBullet;
}

void ioJeonWooChiItem::DecreaseBullet()
{
	m_fCurExtraGauge = max( 0.f, m_fCurExtraGauge - m_fNeedGauge );
}

void ioJeonWooChiItem::UpdateExtraData( ioBaseChar *pOwner )
{
	if( !pOwner ) return;

	// check bullet
	if( m_fMaxExtraGauge <= m_fCurExtraGauge )
		return;

	// check owner state
	switch( pOwner->GetState() )
	{
	case CS_DELAY:
	case CS_RUN:
		break;
	default:
		return;
	}

	float fTimePerSec = g_FrameTimer.GetSecPerFrame();
	float fGauge = m_fExtraGaugeBaseInc * fTimePerSec;

	m_fCurExtraGauge += fGauge;
	m_fCurExtraGauge = (float)min( m_fCurExtraGauge, (float)m_fCurExtraGauge );
}

void ioJeonWooChiItem::SetExtraGaugeBuff( ioBaseChar *pOwner )
{
	if( !pOwner )
		return;

	if( m_ExtraGaugeBuffList.empty() )
		return;

	int iBuffCnt = m_ExtraGaugeBuffList.size();
	for( int i=0; i < iBuffCnt; ++i )
	{
		pOwner->AddNewBuff( m_ExtraGaugeBuffList[i], pOwner->GetCharName(), "", NULL );
	}

	m_bUsedExtraGauge = true;
}

void ioJeonWooChiItem::RemoveExtraGaugeBuff( ioBaseChar *pOwner )
{
	if( !pOwner )
		return;

	if( m_ExtraGaugeBuffList.empty() )
		return;

	int iBuffCnt = m_ExtraGaugeBuffList.size();
	for( int i=0; i < iBuffCnt; ++i )
	{
		pOwner->RemoveBuff( m_ExtraGaugeBuffList[i] );
	}

	m_bUsedExtraGauge = false;
}


//-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

ioJeonWooChiExtendMoveSpecialState::ioJeonWooChiExtendMoveSpecialState()
{
}

ioJeonWooChiExtendMoveSpecialState::~ioJeonWooChiExtendMoveSpecialState()
{
}

void ioJeonWooChiExtendMoveSpecialState::Initialize( ioBaseChar* pOwner, CharState eCurrState )
{
	ioSpecialStateBase::Initialize( pOwner, eCurrState );
}

void ioJeonWooChiExtendMoveSpecialState::Finalize( ioBaseChar* pOwner, CharState eCurrState, CharState eNewState )
{
	ioSpecialStateBase::Finalize( pOwner, eCurrState, eNewState );
}

void ioJeonWooChiExtendMoveSpecialState::CheckSpecialState( ioBaseChar* pOwner )
{
	ioJeonWooChiItem *pJeonWooChiItem = ToJeonWooChiWeaponItem( pOwner->GetPriorityItem( SPT_ATTACK ) );
	if( pJeonWooChiItem && pOwner->GetState() == CS_JEONWOOCHI_EXTEND_MOVE )
	{
		float fHeightGap = pOwner->GetWorldPosition().y - pOwner->GetBottomHeight();
		pJeonWooChiItem->ProcessWooChiMoveState( pOwner, fHeightGap );
	}
}

void ioJeonWooChiExtendMoveSpecialState::CheckFallStateChange( ioBaseChar* pOwner, float fMinFallHeightRate, float fBottomHeight, bool bBottomMaxCheck, float fMaxBottomHeight )
{
	return;
}

bool ioJeonWooChiExtendMoveSpecialState::ProcessMove( ioBaseChar* pOwner, float fTimePerSec, DWORD dwPreTime, DWORD dwCurTime )
{
	return true;

	/*ioPlayStage *pStage = pOwner->GetCreator();
	if ( !pStage )
		return true;

	if( m_fJeonWooChiExtendMoveCurRange >= m_fJeonWooChiExtendMoveMaxRange )
		return true;

	bool bCurMove = false;
	D3DXVECTOR3 vTotalMove( 0.0f, 0.0f, 0.0f );

	float fCurTimeGap = fTimePerSec * FLOAT1000;
	while( fCurTimeGap > 0.0f )
	{
		float fDivideTimePerSec = min( fCurTimeGap, MIN_FRAME_TIME ) * 0.001f;
		fCurTimeGap -= MIN_FRAME_TIME;

		D3DXVECTOR3 vMoveAmt;
		float fFinalMoveAmt = m_fJeonWooChiExtendMoveSpeed * fDivideTimePerSec;
		if( fFinalMoveAmt > 0.0f )	// 감속이 심해서 음수가 나올 경우는 제외시킴
		{
			vMoveAmt = m_JeonWooChiExtendMoveDir * fFinalMoveAmt;
			vTotalMove += vMoveAmt;
			bCurMove = true;
		}
	}

	if( bCurMove )
	{
		float fCurLength = D3DXVec3Length( &vTotalMove );
		m_fJeonWooChiExtendMoveCurRange += fCurLength;

		bool bCol = false;
		if( pStage->ReCalculateMoveVectorByTerrain( pOwner, &vTotalMove, bCol ) )
		{
			float fPreHeight = pOwner->GetBottomHeight();

			pOwner->Translate( vTotalMove );

			if( pOwner->GetForceAmt() != 0.0f )	// 빠른 ForceMove로 경사면을 통과하는 경우에 대한 보강
			{
				float fMoveHeight = pOwner->GetBottomHeight();

				if( fPreHeight > 0.0f && fMoveHeight <= FLOAT1 )
				{
					D3DXVECTOR3 vCurPos = pOwner->GetWorldPosition();

					float fTopHeight = pStage->GetMapHeight( vCurPos.x, vCurPos.z, pOwner, false, pOwner->GetMidHeightByRate( FLOAT1 ) );
					if( fTopHeight > FLOAT1 )
					{
						vCurPos.y = max( fTopHeight, fPreHeight );
						pOwner->SetWorldPosition( vCurPos );
					}
				}
			}
		}

		if( bCol )
		{
			pOwner->SetSKillEndJumpState( 0.1f, false, true, false );
			return true;
		}
	}

	if ( pOwner->IsAxisLimit() )
	{
		D3DXVECTOR3 vPos = pOwner->GetWorldPosition();
		float fBottomHeight = pOwner->GetBottomHeight();
		vPos.y = max( fBottomHeight, vPos.y );
		pStage->CheckAxisLimit( &vPos );
		pOwner->SetWorldPosition( vPos );
	}

	return true;*/
}


//-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

ioJeonWooChiExtendDashSpecialState::ioJeonWooChiExtendDashSpecialState()
{
}

ioJeonWooChiExtendDashSpecialState::~ioJeonWooChiExtendDashSpecialState()
{
}

void ioJeonWooChiExtendDashSpecialState::Initialize( ioBaseChar* pOwner, CharState eCurrState )
{
	ioSpecialStateBase::Initialize( pOwner, eCurrState );
}

void ioJeonWooChiExtendDashSpecialState::Finalize( ioBaseChar* pOwner, CharState eCurrState, CharState eNewState )
{
	ioSpecialStateBase::Finalize( pOwner, eCurrState, eNewState );
}

void ioJeonWooChiExtendDashSpecialState::CheckSpecialState( ioBaseChar* pOwner )
{
	ioJeonWooChiItem *pJeonWooChiItem = ToJeonWooChiWeaponItem( pOwner->GetPriorityItem( SPT_ATTACK ) );
	if( pJeonWooChiItem && pOwner->GetState() == CS_JEONWOOCHI_EXTEND_DASH )
	{
		float fHeightGap = pOwner->GetWorldPosition().y - pOwner->GetBottomHeight();
		pJeonWooChiItem->ProcessWooChiDashState( pOwner, fHeightGap );
	}
}

void ioJeonWooChiExtendDashSpecialState::CheckFallStateChange( ioBaseChar* pOwner, float fMinFallHeightRate, float fBottomHeight, bool bBottomMaxCheck, float fMaxBottomHeight )
{
	return;
}

bool ioJeonWooChiExtendDashSpecialState::ProcessMove( ioBaseChar* pOwner, float fTimePerSec, DWORD dwPreTime, DWORD dwCurTime )
{
	return true;
	/*ioPlayStage *pStage = pOwner->GetCreator();
	if ( !pStage )
		return true;

	if( m_fJeonWooChiExtendMoveCurRange >= m_fJeonWooChiExtendMoveMaxRange )
		return true;

	bool bCurMove = false;
	D3DXVECTOR3 vTotalMove( 0.0f, 0.0f, 0.0f );

	float fCurTimeGap = fTimePerSec * FLOAT1000;
	while( fCurTimeGap > 0.0f )
	{
		float fDivideTimePerSec = min( fCurTimeGap, MIN_FRAME_TIME ) * 0.001f;
		fCurTimeGap -= MIN_FRAME_TIME;

		D3DXVECTOR3 vMoveAmt;
		float fFinalMoveAmt = m_fJeonWooChiExtendMoveSpeed * fDivideTimePerSec;
		if( fFinalMoveAmt > 0.0f )	// 감속이 심해서 음수가 나올 경우는 제외시킴
		{
			vMoveAmt = m_JeonWooChiExtendMoveDir * fFinalMoveAmt;
			vTotalMove += vMoveAmt;
			bCurMove = true;
		}
	}

	if( bCurMove )
	{
		float fCurLength = D3DXVec3Length( &vTotalMove );
		m_fJeonWooChiExtendMoveCurRange += fCurLength;

		bool bCol = false;
		if( pStage->ReCalculateMoveVectorByTerrain( pOwner, &vTotalMove, bCol ) )
		{
			float fPreHeight = pOwner->GetBottomHeight();

			pOwner->Translate( vTotalMove );

			if( pOwner->GetForceAmt() != 0.0f )	// 빠른 ForceMove로 경사면을 통과하는 경우에 대한 보강
			{
				float fMoveHeight = pOwner->GetBottomHeight();

				if( fPreHeight > 0.0f && fMoveHeight <= FLOAT1 )
				{
					D3DXVECTOR3 vCurPos = pOwner->GetWorldPosition();

					float fTopHeight = pStage->GetMapHeight( vCurPos.x, vCurPos.z, pOwner, false, pOwner->GetMidHeightByRate( FLOAT1 ) );
					if( fTopHeight > FLOAT1 )
					{
						vCurPos.y = max( fTopHeight, fPreHeight );
						pOwner->SetWorldPosition( vCurPos );
					}
				}
			}
		}

		if( bCol )
		{
			pOwner->SetSKillEndJumpState( 0.1f, false, true, false );
			return true;
		}
	}

	if ( pOwner->IsAxisLimit() )
	{
		D3DXVECTOR3 vPos = pOwner->GetWorldPosition();
		float fBottomHeight = pOwner->GetBottomHeight();
		vPos.y = max( fBottomHeight, vPos.y );
		pStage->CheckAxisLimit( &vPos );
		pOwner->SetWorldPosition( vPos );
	}

	return true;*/
}



void ioJeonWooChiExtendDashSpecialState::SetExtendDashInfo( D3DXVECTOR3 vMoveDir, D3DXVECTOR3 vTargetPos, float fMoveSpeed, float fMaxRange )
{
	m_vMoveDir = vMoveDir;
	m_vTargetPos = vTargetPos;
	m_fMoveSpeed = fMoveSpeed;
	m_fMaxRange = fMaxRange;
	m_fMoveCurRange = 0.0f;
}