

#include "stdafx.h"

#include "ioCuchulainItem.h"
#include "ioPlayStage.h"

#include "ioBaseChar.h"
#include "WeaponDefine.h"
#include "ItemDefine.h"

ioCuchulainItem::ioCuchulainItem()
{
	m_ChargeState = CS_NONE;
	m_dwAttackStartTime = 0;
	m_iCurCombo = 0;

	m_iCurAirJump = 0;
	m_fAirJumpPowerRate = 0.0f;

	m_bCharged = false;
}

ioCuchulainItem::ioCuchulainItem( const ioCuchulainItem &rhs )
	: ioWeaponItem( rhs ),
	m_AttackReadyAniList( rhs.m_AttackReadyAniList ),
	m_vExtendAttributeList( rhs.m_vExtendAttributeList ),
	m_dwMiddleChargeTime( rhs.m_dwMiddleChargeTime),
	m_dwFullChargeTime( rhs.m_dwFullChargeTime ),
	m_SearchInfo( rhs.m_SearchInfo ),
	m_stAimingMaker( rhs.m_stAimingMaker ),
	m_stAimedMaker( rhs.m_stAimedMaker ),
	m_MiddleAttack( rhs.m_MiddleAttack ),
	m_FlyInfo( rhs.m_FlyInfo ),
	m_fChargeBuffSearchRange( rhs.m_fChargeBuffSearchRange ),
	m_ChargeSearchBuff1( rhs.m_ChargeSearchBuff1 ),
	m_ChargeSearchAddBuff1( rhs.m_ChargeSearchAddBuff1 ),
	m_ChargeSearchBuff2( rhs.m_ChargeSearchBuff2 ),
	m_ChargeSearchAddBuff2( rhs.m_ChargeSearchAddBuff2 ),
	m_ChargeSearchBuff3( rhs.m_ChargeSearchBuff3 ),
	m_ChargeSearchAddBuff3( rhs.m_ChargeSearchAddBuff3 ),
	m_dwFlyKeyReservTime( rhs.m_dwFlyKeyReservTime ),
	m_dwSpecialChargeTime( rhs.m_dwSpecialChargeTime )
{
	m_ChargeState = CS_NONE;
	m_dwAttackStartTime = 0;
	m_iCurCombo = 0;

	m_iCurAirJump = 0;
	m_fAirJumpPowerRate = 0.0f;

	m_bCharged = false;
}

ioCuchulainItem::~ioCuchulainItem()
{
}

void ioCuchulainItem::LoadProperty( ioINILoader &rkLoader )
{
	ioWeaponItem::LoadProperty( rkLoader );

	int i;
	char szBuf[MAX_PATH];
	//char szKey[MAX_PATH];

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

	LoadAttackAttribute( "middle_attack", m_MiddleAttack, rkLoader );

	//////////////////////////////////////////////////////////////////////////
	m_SearchInfo.fAngle = rkLoader.LoadFloat( "middle_search_angle", 0.f );
	m_SearchInfo.fRange = rkLoader.LoadFloat( "middle_search_range", 0.f );

	rkLoader.LoadString_e( "aiming_marker", "", szBuf, MAX_PATH );
	m_stAimingMaker = szBuf;
	rkLoader.LoadString_e( "aimed_marker", "", szBuf, MAX_PATH );
	m_stAimedMaker = szBuf;


	m_dwMiddleChargeTime = rkLoader.LoadInt( "middle_charge_time", 10000 );
	m_dwFullChargeTime = rkLoader.LoadInt( "full_charge_time", 50000 );

	rkLoader.LoadString_e( "special_fly_start_ani", "", szBuf, MAX_PATH );
	m_FlyInfo.m_StartAni = szBuf;
	m_FlyInfo.m_fStartAniRate = rkLoader.LoadFloat( "special_fly_start_ani_rate", FLOAT1 );
	m_FlyInfo.m_dwStartWaitTime = (int)rkLoader.LoadInt( "special_fly_start_wait_time", 100000 );
	

	rkLoader.LoadString_e( "special_fly_move_ani", "", szBuf, MAX_PATH );
	m_FlyInfo.m_FlyAni = szBuf;
	m_FlyInfo.m_fFlyAniRate = rkLoader.LoadFloat( "special_fly_move_ani_rate", FLOAT1 );
	m_FlyInfo.m_fFlyAngle = rkLoader.LoadFloat( "special_fly_move_angle", 0.f );
	m_FlyInfo.m_fFlySpeed = rkLoader.LoadFloat( "special_fly_move_speed", 10000.f );
	m_FlyInfo.m_fFlyMaxRange = rkLoader.LoadFloat( "special_fly_move_range", 0.f );
	
	m_FlyInfo.m_fEndJumpPower = rkLoader.LoadFloat( "special_fly_end_jump_power",  0.f );

	rkLoader.LoadString_e( "special_fly_add_buff", "", szBuf, MAX_PATH );
	m_FlyInfo.m_stBuff= szBuf;

	//////////////////////////////////////////////////////////////////////////
	m_fChargeBuffSearchRange = rkLoader.LoadFloat( "charge_buff_search_range", 0.f );

	rkLoader.LoadString_e( "charge_search_buff1", "", szBuf, MAX_PATH );
	m_ChargeSearchBuff1 = szBuf;
	rkLoader.LoadString_e( "charge_search_add_buff1", "", szBuf, MAX_PATH );
	m_ChargeSearchAddBuff1 = szBuf;

	rkLoader.LoadString_e( "charge_search_buff2", "", szBuf, MAX_PATH );
	m_ChargeSearchBuff2 = szBuf;
	rkLoader.LoadString_e( "charge_search_add_buff2", "", szBuf, MAX_PATH );
	m_ChargeSearchAddBuff2 = szBuf;

	rkLoader.LoadString_e( "charge_search_buff3", "", szBuf, MAX_PATH );
	m_ChargeSearchBuff3 = szBuf;
	rkLoader.LoadString_e( "charge_search_add_buff3", "", szBuf, MAX_PATH );
	m_ChargeSearchAddBuff3 = szBuf;

	//////////////////////////////////////////////////////////////////////////
	m_dwFlyKeyReservTime = (DWORD)rkLoader.LoadInt("special_fly_key_reserve_time", 0 );
	m_dwSpecialChargeTime = (DWORD)rkLoader.LoadInt("special_fly_key_charge_time", 1000 );

}

ioItem* ioCuchulainItem::Clone()
{
	return new ioCuchulainItem( *this );
}

ioWeaponItem::WeaponSubType ioCuchulainItem::GetSubType() const
{
	return WST_CUCHULAIN_ITEM;
}

void ioCuchulainItem::OnReleased( ioBaseChar *pOwner )
{
	m_fCurExtraGauge = 0.0f;

	pOwner->SetTargetMarker( ioTargetMarker::MT_NONE, "", "", "", "", "" );

	ioAttackableItem::OnReleased( pOwner );
}

void ioCuchulainItem::SetNormalAttack( ioBaseChar *pOwner, int iCurCombo )
{
	ClearCancelInfo();

	m_bCharged = false;
	m_bMiddleCharged = false;

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


void ioCuchulainItem::ChargeBuffSearch( ioBaseChar* pOwner, bool bSend )
{
	if( !pOwner )
		return;

	ioPlayStage *pStage = pOwner->GetCreator();
	if( !pStage )
		return;

	if( pOwner->GetExperienceMode() != EMS_NONE )
		return;

	D3DXVECTOR3 vCurPos = pOwner->GetMidPositionByRate();
	D3DXVECTOR3 vDiff= ioMath::VEC3_ZERO;
	float fTargetRangeSq = m_fChargeBuffSearchRange * m_fChargeBuffSearchRange;

	const BaseCharList &rkCharList = pStage->GetBaseCharList();
	BaseCharList::const_iterator iter;

	/*ioHashStringVec vBuff1List;
	ioHashStringVec vBuff2List;
	ioHashStringVec vBuff3List;*/

	for( iter=rkCharList.begin() ; iter!=rkCharList.end() ; ++iter )
	{
		ioBaseChar *pTarget = *iter;
		if( !pTarget ) 
			continue;

		if( pTarget->GetTeam() == pOwner->GetTeam() ) 
			continue;
		if( pTarget->GetCharName() == GetOwnerName() ) 
			continue;
		if( pTarget->GetExperienceMode() != EMS_NONE ) 
			continue;

		D3DXVECTOR3 vTargetPos = pTarget->GetMidPositionByRate();
		vDiff = vTargetPos - vCurPos;
		float fDiffRangeSq = D3DXVec3LengthSq( &vDiff );

		if( fDiffRangeSq < fTargetRangeSq )
		{
			if( pTarget->HasBuff( pOwner->GetCharName(), m_ChargeSearchBuff1 ) )
			{
				//vBuff1List.push_back( pTarget->GetCharName() );
				//if( pOwner->IsNeedProcess() )
					pTarget->AddNewBuff( m_ChargeSearchAddBuff1, pOwner->GetCharName(), "", NULL );
			}
			else if( pTarget->HasBuff( pOwner->GetCharName(), m_ChargeSearchBuff2 ) )
			{
				//vBuff2List.push_back( pTarget->GetCharName() );
				//if( pOwner->IsNeedProcess() )
					pTarget->AddNewBuff( m_ChargeSearchAddBuff2, pOwner->GetCharName(), "", NULL );
			}
			else if( pTarget->HasBuff( pOwner->GetCharName(), m_ChargeSearchBuff3 ) )
			{
				//vBuff3List.push_back( pTarget->GetCharName() );
				//if( pOwner->IsNeedProcess() )
					pTarget->AddNewBuff( m_ChargeSearchAddBuff3, pOwner->GetCharName(), "", NULL );
			}
		}
	}

	//SendPacket
	if( pOwner->IsNeedProcess() && bSend )
	{
		//SendPacket
		SP2Packet kPacket( CUPK_ITEM_GAUGE_CANCEL );
		FillItemGaugePacketHeader( pOwner, kPacket );
		kPacket << NST_FULL_BUFF_SEARCH_ATTACK;
		kPacket << pOwner->GetWorldPosition();
		kPacket << pOwner->GetTargetRot();
		P2PNetwork::SendToAllPlayingUser( kPacket );
	}
}


void ioCuchulainItem::CheckMiddleSearch( ioBaseChar *pOwner, bool bTargetEffect )
{
	if( !pOwner ) 
		return;
	if( !pOwner->IsNeedProcess() ) 
		return;

	if( m_AimedTarget.IsEmpty() )
	{
		FindMiddleSearchTarget( pOwner, m_SearchInfo );

		if( !m_AimedTarget.IsEmpty() && bTargetEffect )
		{
			pOwner->SetTargetMarkerTarget( m_AimedTarget, false );
			pOwner->SetTargetMarkerState( ioTargetMarker::MS_AIMED );

			//SendPacket
			SP2Packet kPacket( CUPK_ITEM_GAUGE_CANCEL );
			FillItemGaugePacketHeader( pOwner, kPacket );
			kPacket << NST_TARGETING;
			kPacket << m_AimedTarget;
			P2PNetwork::SendToAllPlayingUser( kPacket );
		}
	}
	else if( !CheckTargetValidate(pOwner, m_SearchInfo ) )
	{
		FindMiddleSearchTarget( pOwner, m_SearchInfo  );
		if( !m_AimedTarget.IsEmpty() )
		{
			if( bTargetEffect )
			{
				pOwner->SetTargetMarkerTarget( m_AimedTarget, false );
				pOwner->SetTargetMarkerState( ioTargetMarker::MS_AIMED );
			}
		}
		else
		{
			if( bTargetEffect )
			{
				pOwner->SetTargetMarkerTarget( m_AimedTarget, false );
				pOwner->SetTargetMarkerState( ioTargetMarker::MS_READY );
			}
		}

		//SendPacket
		SP2Packet kPacket( CUPK_ITEM_GAUGE_CANCEL );
		FillItemGaugePacketHeader( pOwner, kPacket );
		kPacket << NST_RE_TARGETING;
		kPacket << m_AimedTarget;
		P2PNetwork::SendToAllPlayingUser( kPacket );
	}
}


bool ioCuchulainItem::CheckTargetValidate( ioBaseChar *pOwner, const CuchulainSearchInfo &info )
{
	if( !pOwner )
		return false;

	D3DXVECTOR3 vCurPos = pOwner->GetMidPositionByRate();
	D3DXVECTOR3 vDiff= ioMath::VEC3_ZERO;

	float fCurAngle = ioMath::ArrangeHead( info.fAngle );
	float fHalfValue = cosf( DEGtoRAD(fCurAngle*0.5f) );
	float fTargetRangeSq = info.fRange * info.fRange;

	ioBaseChar *pTarget = pOwner->GetBaseChar( m_AimedTarget );
	if( !pTarget )
		return false;

	if( !IsEnableTargetState( pTarget ) )
		return false;

	if( pTarget->GetExperienceMode() != EMS_NONE )
		return false;

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
			return false;
	}

	// 거리 체크
	if( fDiffRangeSq >= fTargetRangeSq )
		return false;

	return true;
}


void ioCuchulainItem::SetSpecialMoveState( ioBaseChar *pOwner, const CuchulainFlyInfo& info, bool SendNet )
{
	if( !pOwner )
		return;

	m_SpecialMoveState = CFS_NONE;

	m_CurFlyInfo = info;
	pOwner->SetState( CS_CUCHULAIN_SPECIAL_STATE );

	/*pOwner->SetCurMoveSpeed( 0.0f );
	pOwner->SetCurJumpPowerAmt( 0.0f );
	pOwner->SetGravityAmt( 0.0f );
	pOwner->SetForcePowerAmt( 0.0f );*/

	ChangeSpecialMove_Start( pOwner );

	m_bSpecialChagerKey = false;
	m_dwSpecialChargeStartTime = 0;

	DecreseBullet();

	//SendPacket
	if( pOwner->IsNeedProcess() && SendNet )
	{
		SP2Packet kPacket( CUPK_ITEM_GAUGE_CANCEL );
		FillItemGaugePacketHeader( pOwner, kPacket );
		kPacket << NST_SPECIAL_S_MOVE;
		kPacket << pOwner->GetWorldPosition();
		kPacket << pOwner->GetTargetRot();
		P2PNetwork::SendToAllPlayingUser( kPacket );
	}
}


void ioCuchulainItem::ChangeSpecialMove_Start( ioBaseChar *pOwner )
{
	DWORD dwCurTime = FRAMEGETTIME();
	m_dwFlyStartTime = dwCurTime;

	if( !pOwner )
		return;

	ioEntityGroup *pGrp = pOwner->GetGroup();
	if( !pGrp )
		return;

	pOwner->InitExtendAttackTagInfo();

	pGrp->ClearAllLoopAni( FLOAT10, true );
	pGrp->ClearAllActionAni( FLOAT10, true );

	int iAniID	     = pGrp->GetAnimationIdx( m_CurFlyInfo.m_StartAni );
	float fTimeRate  = m_CurFlyInfo.m_fStartAniRate;
	DWORD dwPreDelay = 0;
	DWORD dwEndDelay = 0;

	pGrp->SetActionAni( iAniID, FLOAT10, FLOAT10, FLOAT1, FLOAT1/fTimeRate, 0.0f, dwPreDelay, true );

	m_SpecialMoveState = CFS_START;

	pOwner->AddNewBuff( m_CurFlyInfo.m_stBuff, "", "", NULL );
}


void ioCuchulainItem::CheckNormalAttack( ioBaseChar *pOwner )
{
	DWORD dwCurTime = FRAMEGETTIME();

	switch( m_ChargeState )
	{
	case CS_CHARGING:
		if( m_bMiddleCharged )
			CheckMiddleSearch( pOwner );

		OnCharging( pOwner );
		break;
	case CS_ATTACK_FIRE:
		{
			if( m_bCharged )
				pOwner->CheckExtraAniJump();

			if( pOwner->IsNeedProcess() )
			{
				if( m_bCharged )
				{
					if( pOwner->CheckExtendAttackEndJump() )
						return;
				}
				else
				{
					if( pOwner->IsDefenseKey() && m_fCurExtraGauge >= FLOAT1 )
					{
						SetSpecialMoveState( pOwner, m_FlyInfo, true );
						return;
					}
				}

				CheckCancelReserve( pOwner );

				if( ProcessCancel(pOwner) )
					return;
			}

			ioWeaponItem::CheckNormalAttack( pOwner );
		}
		break;
	case CS_MIDDLE_ATTACK_FIRE:
		if( pOwner->IsNeedProcess() )
		{
			if( pOwner->IsDefenseKey() && m_fCurExtraGauge >= FLOAT1 )
			{
				SetSpecialMoveState( pOwner, m_FlyInfo, true );
				return;
			}

			if( m_dwMiddleAttackEndTime < dwCurTime )
				ProcessReserveKeyInput( pOwner );
			else
				CheckReserve( pOwner );
		}
		break;
	}
}


void ioCuchulainItem::SetMiddleAttack( ioBaseChar *pOwner,const AttackAttribute& att )
{
	if( !pOwner ) return;

	ioEntityGroup *pGrp = pOwner->GetGroup();
	int iAniID = pGrp->GetAnimationIdx( att.m_AttackAnimation );
	if ( iAniID == -1)
		return;

	float fTimeRate  = att.m_fAttackAniRate;
	DWORD dwPreDelay = att.m_dwPreDelay;
	DWORD dwEndDelay = att.m_dwEndDelay;

	DWORD dwCurTime = FRAMEGETTIME();

	pOwner->InitExtendAttackTagInfo();

	pGrp->ClearAllLoopAni( FLOAT100, true );
	pGrp->ClearAllActionAni( FLOAT100, true );


	pGrp->SetActionAni( iAniID, FLOAT100, 0.0f, FLOAT1, FLOAT1/fTimeRate, 0.0f, dwPreDelay );

	m_dwMiddleAttackEndTime = dwCurTime;
	m_dwMiddleAttackEndTime += pGrp->GetAnimationFullTime( iAniID ) * fTimeRate + dwPreDelay + att.m_dwEndDelay;

	pOwner->ClearReservedSliding();
	pOwner->SetReservedSliding( att.m_vForceInfoList, iAniID, fTimeRate, dwPreDelay );

	pOwner->RefreshFireTimeList( iAniID, att.m_vWeaponInfoList, FTT_NORMAL_ATTACK, fTimeRate, dwPreDelay );

	m_ChargeState = CS_MIDDLE_ATTACK_FIRE;

	pOwner->SetTargetMarkerState( ioTargetMarker::MS_NONE );

	//SendPacket
	if( pOwner->IsNeedProcess() )
	{
		SP2Packet kPacket( CUPK_ITEM_GAUGE_CANCEL );
		FillItemGaugePacketHeader( pOwner, kPacket );
		kPacket << NST_MIDDLE_ATTACK;
		kPacket << pOwner->GetWorldPosition();
		kPacket << pOwner->GetTargetRot();
		kPacket << m_AimedTarget;
		P2PNetwork::SendToAllPlayingUser( kPacket );
	}
}


void ioCuchulainItem::ProcessReserveKeyInput( ioBaseChar *pOwner )
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


void ioCuchulainItem::OnCharging( ioBaseChar *pOwner )
{
	if( !pOwner->IsNeedProcess() )	
		return;

	if( pOwner->IsAttackKeyDown() )
	{
		if( m_dwAttackStartTime + m_dwFullChargeTime <= FRAMEGETTIME() )
		{
			ChargeBuffSearch( pOwner, true );

			ChangeToAttackFire( pOwner, true );
			g_TutorialMgr.ActionChargeAttack();
		}
		else if( m_dwAttackStartTime + m_dwMiddleChargeTime <= FRAMEGETTIME() )
		{
			if( !m_bMiddleCharged )
			{
				m_bMiddleCharged = true;

				//m_AimedTarget.Clear();
				////찾기 
				//pOwner->SetTargetMarker( ioTargetMarker::MT_OTHER_MARKER,
				//	m_stAimingMaker, m_stAimingMaker, m_stAimedMaker, m_stAimedMaker ,m_stAimedMaker );

				//pOwner->SetTargetMarkerState( ioTargetMarker::MS_READY );
				//pOwner->SetTargetMarkerRange( m_SearchInfo.fRange );

				AimTargetInit( pOwner, true );
				
				//FindMiddleSearchTarget( pOwner, m_SearchInfo );

				/*if( !m_AimedTarget.IsEmpty() )
				{
					pOwner->SetTargetMarkerTarget( m_AimedTarget, false );
					pOwner->SetTargetMarkerState( ioTargetMarker::MS_AIMED );

					//SendPacket
				}*/
			}
		}
	}
	else
	{
		if( m_bMiddleCharged )
			SetMiddleAttack( pOwner, m_MiddleAttack );
		else
			ChangeToAttackFire( pOwner, false );
	}
}


void ioCuchulainItem::OnEquiped( ioBaseChar *pOwner )
{
	ioWeaponItem::OnEquiped( pOwner );

	if( !pOwner )
		return;

	pOwner->SetTargetMarker( ioTargetMarker::MT_OTHER_MARKER,
		m_szReadyMarker, m_szAimingMarker, m_szAimedMarker, m_szZeroAimedMarker, "" );

	pOwner->SetTargetMarkerRange( m_SearchInfo.fRange );
}


void ioCuchulainItem::AimTargetInit( ioBaseChar *pOwner, bool bSend )
{
	m_AimedTarget.Clear();
	//찾기 
	pOwner->SetTargetMarker( ioTargetMarker::MT_OTHER_MARKER,
		m_stAimingMaker, m_stAimingMaker, m_stAimedMaker, m_stAimedMaker ,m_stAimedMaker );

	pOwner->SetTargetMarkerState( ioTargetMarker::MS_READY );
	pOwner->SetTargetMarkerRange( m_SearchInfo.fRange );

	if( pOwner->IsNeedProcess() && bSend )
	{
		//SendPacket
		SP2Packet kPacket( CUPK_ITEM_GAUGE_CANCEL );
		FillItemGaugePacketHeader( pOwner, kPacket );
		kPacket << NST_TARGET_INIT;
		P2PNetwork::SendToAllPlayingUser( kPacket );
	}
}


void ioCuchulainItem::ChangeToAttackFire( ioBaseChar *pOwner, bool bCharged )
{
	if( !pOwner ) return;

	DWORD dwCurTime = FRAMEGETTIME();
	m_bCharged = bCharged;

	pOwner->InitExtendAttackTagInfo();

	ClearCancelInfo();
	
	//에임 이펙트 초기화
	ClearAimInfo( pOwner );

	if( m_bCharged )
	{
		int iExtendCnt = m_vExtendAttributeList.size();
		if( COMPARE( m_iCurCombo, 0, iExtendCnt ) )
		{
			m_szCurAni = m_vExtendAttributeList[m_iCurCombo].m_AttackAnimation;
			m_fCurAniRate = m_vExtendAttributeList[m_iCurCombo].m_fAttackAniRate;

			m_dwInputCancelCheckTime = dwCurTime;
			m_dwCancelCheckTime = dwCurTime;

			pOwner->SetNormalAttackByAttribute( m_vExtendAttributeList[m_iCurCombo], false );

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
}


void ioCuchulainItem::DecreseBullet()
{
	SetCurBullet( GetCurBullet() - FLOAT1 );
}

void ioCuchulainItem::OnItemGaugeCancel( ioBaseChar *pOwner, SP2Packet &rkPacket )
{
	if( !pOwner ) 
		return;

	int iState;
	rkPacket >> iState;

	D3DXVECTOR3 vWorldPos;
	D3DXQUATERNION qt;

	switch( iState )
	{
	case NST_TARGET_INIT:
		AimTargetInit( pOwner, false );
		break;
	case NST_TARGETING:
		rkPacket >> m_AimedTarget;

		pOwner->SetTargetMarkerTarget( m_AimedTarget, false );
		pOwner->SetTargetMarkerState( ioTargetMarker::MS_AIMED );

		break;
	case NST_RE_TARGETING:
		rkPacket >> m_AimedTarget;

		if( !m_AimedTarget.IsEmpty() )
		{
			pOwner->SetTargetMarkerTarget( m_AimedTarget, false );
			pOwner->SetTargetMarkerState( ioTargetMarker::MS_AIMED );
		}
		else
		{
			pOwner->SetTargetMarkerTarget( m_AimedTarget, false );
			pOwner->SetTargetMarkerState( ioTargetMarker::MS_READY );
		}
		break;
	case NST_MIDDLE_ATTACK:
		rkPacket >> vWorldPos;
		rkPacket >> qt;
		rkPacket >> m_AimedTarget;

		pOwner->SetWorldPosition( vWorldPos );
		pOwner->SetTargetRot( qt );

		SetMiddleAttack( pOwner, m_MiddleAttack );

		break;

	case NST_FULL_BUFF_SEARCH_ATTACK:
		rkPacket >> vWorldPos;
		rkPacket >> qt;

		pOwner->SetWorldPosition( vWorldPos );
		pOwner->SetTargetRot( qt );

		ChargeBuffSearch( pOwner, false );
		break;

	case NST_SPECIAL_S_MOVE:
		rkPacket >> vWorldPos;
		rkPacket >> qt;

		pOwner->SetWorldPosition( vWorldPos );
		pOwner->SetTargetRot( qt );
		
		SetSpecialMoveState( pOwner, m_FlyInfo, false );
		break;

	case NST_SPECIAL_S_END_JUMP:
		rkPacket >> vWorldPos;
		rkPacket >> qt;

		pOwner->SetWorldPosition( vWorldPos );
		pOwner->SetTargetRot( qt );

		ChangeSpecialMove_End( pOwner );
		break;
	}
}

void ioCuchulainItem::OnItemGaugeOK( ioBaseChar *pOwner, SP2Packet &rkPacket )
{
	if( !pOwner ) 
		return;

	bool bCharged;
	rkPacket >> m_iCurCombo;
	rkPacket >> bCharged;

	D3DXQUATERNION qtRot;
	rkPacket >> qtRot;
	pOwner->SetTargetRot( qtRot );

	ChangeToAttackFire( pOwner, bCharged );
}

void ioCuchulainItem::SetExtraInfoToWeapon( ioWeapon *pWeapon, bool bJumpAttack )
{
	ioAttackableItem::SetExtraInfoToWeapon( pWeapon, bJumpAttack );

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

	ioTraceWeapon *pTrace = ToTraceWeapon( pWeapon );
	if( pTrace && !m_AimedTarget.IsEmpty() )
	{
		pTrace->SetTraceTarget( m_AimedTarget );
	}
}

bool ioCuchulainItem::IsChargeAttackState( ioBaseChar *pOwner )
{
	if( pOwner->GetState() == CS_ATTACK )
	{
		if( m_ChargeState == CS_CHARGING )
			return true;
	}

	return false;
}

bool ioCuchulainItem::IsEnableAttackDashCheck( ioBaseChar *pOwner, bool bCheck )
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


void ioCuchulainItem::CheckReserve( ioBaseChar *pOwner )
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

bool ioCuchulainItem::ProcessCancel( ioBaseChar *pOwner )
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

bool ioCuchulainItem::SetJumpAttack( ioBaseChar *pOwner, int iAniID, float fTimeRate, AttackType eType )
{
	if( !pOwner ) return false;

	DWORD dwCurTime = FRAMEGETTIME();
	ClearAirJump();

	AttackType eExtendType = AT_JUMP_ATTACK;

	const AttackAttribute *pAttr = GetAttackAttribute( eExtendType );
	if( !pAttr )
	{
		LOG.PrintTimeAndLog( 0, "ioCuchulainItem::SetJumpAttack - AttackAttribute Not Exist" );
		return false;
	}

	ioEntityGroup *pGrp = pOwner->GetGroup();
	iAniID = pGrp->GetAnimationIdx( pAttr->m_AttackAnimation );
	if( iAniID == -1 )
	{
		LOG.PrintTimeAndLog( 0, "ioCuchulainItem::SetJumpAttack - JumpAttack Animation Not Exist" );
		return false;
	}

	fTimeRate   = pAttr->m_fAttackAniRate;
	DWORD dwPreDelay  = pAttr->m_dwPreDelay;

	// For AirJumpPower
	SetAirJump( pOwner, iAniID, fTimeRate );

	return false;
}

void ioCuchulainItem::ProcessJumpAttack( ioBaseChar *pOwner, float fHeightGap )
{
	if( m_pExtendJump )
	{
		m_pExtendJump->ProcessJumpAttack( pOwner, fHeightGap );
	}

	CheckAirJump( pOwner );
}


int ioCuchulainItem::GetMaxBullet()
{
	return (int)m_fMaxExtraGauge;
}

int ioCuchulainItem::GetCurBullet()
{
	float fCurCoolTime = m_fCurExtraGauge;
	fCurCoolTime = max( 0, min( fCurCoolTime, m_fMaxExtraGauge ) );

	return (int)fCurCoolTime;
}

void ioCuchulainItem::SetCurBullet( int iCurBullet )
{
	if( iCurBullet < 0 )
		return;

	float fCurBullet = iCurBullet;
	fCurBullet = max( 0, min( fCurBullet, m_fMaxExtraGauge ) );

	m_fCurExtraGauge = fCurBullet;
}

void ioCuchulainItem::UpdateExtraData( ioBaseChar *pOwner )
{
	if( !pOwner ) return;

	// check owner state
	float fTimePerSec = g_FrameTimer.GetSecPerFrame();

	if( pOwner->IsSystemState() )
		return;

	switch( pOwner->GetState() )
	{
	case CS_DELAY:
	case CS_DASH:
	case CS_RUN:
		if( m_fCurExtraGauge < m_fMaxExtraGauge )
		{
			float fGauge = m_fExtraGaugeBaseInc * fTimePerSec;

			m_fCurExtraGauge += fGauge;
			m_fCurExtraGauge = min( m_fCurExtraGauge, m_fMaxExtraGauge );
		}
		return;
	}

}

int ioCuchulainItem::GetNeedBullet()
{
	return 1;
}

void ioCuchulainItem::FindMiddleSearchTarget( ioBaseChar *pOwner, const CuchulainSearchInfo &info )
{
	m_AimedTarget.Clear();

	if( !pOwner )
		return;

	ioPlayStage *pStage = pOwner->GetCreator();
	if( !pStage )
		return;

	D3DXVECTOR3 vCurPos = pOwner->GetMidPositionByRate();
	D3DXVECTOR3 vDiff= ioMath::VEC3_ZERO;

	float fCurAngle = ioMath::ArrangeHead( info.fAngle );
	float fHalfValue = cosf( DEGtoRAD(fCurAngle*0.5f) );
	float fTargetRangeSq = info.fRange * info.fRange;

	const BaseCharList &rkCharList = pStage->GetBaseCharList();
	BaseCharList::const_iterator iter;

	ioHashString szTarget;
	float fCurGap = -FLOAT1;
	for( iter=rkCharList.begin() ; iter!=rkCharList.end() ; ++iter )
	{
		ioBaseChar *pTarget = *iter;
		if( !pTarget ) continue;

		if( pTarget->GetTeam() == pOwner->GetTeam() ) continue;
		if( pTarget->GetCharName() == GetOwnerName() ) continue;
		if( pTarget->GetExperienceMode() != EMS_NONE ) continue;

		if( !IsEnableTargetState( pTarget ) )
			continue;

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
			m_AimedTarget = szTarget;
		}
	}
}


bool ioCuchulainItem::IsEnableTargetState( ioBaseChar *pTarget )
{
	if( !pTarget )
		return false;

	if( pTarget->IsSystemState() )
		return false;

	if( !pTarget->IsEnableTarget( false ) )
		return false;

	if( pTarget->IsApplyDownState(false) )
		return false;

	return true;
}


void ioCuchulainItem::ClearAimInfo( ioBaseChar *pOwner )
{
	m_dwMiddleAttackEndTime = 0;
	pOwner->SetTargetMarkerState( ioTargetMarker::MS_NONE );
	m_AimedTarget.Clear();
}

void ioCuchulainItem::OnEndNormalAttack( ioBaseChar *pOwner )
{
	ClearAimInfo( pOwner );
}


void ioCuchulainItem::ProcessSpecialMove(ioBaseChar *pOwner )
{
	if( !pOwner )
		return;

	DWORD dwCurTime = FRAMEGETTIME();

	switch( m_SpecialMoveState )
	{
	case CFS_START:
		pOwner->SetCurMoveSpeed( 0 );
		
		if( dwCurTime > (m_dwFlyStartTime + m_CurFlyInfo.m_dwStartWaitTime) )
		{
			ChangeSpecialMove_Move( pOwner );
			return;
		}
		break;
	case CFS_MOVE:

		if( pOwner->IsNeedProcess() && m_dwFlyKeyReservTime && (m_dwFlyKeyReservTime+m_dwFlyStartTime < dwCurTime ) )
		{
			if( m_bSpecialChagerKey )
			{
				if( !pOwner->IsAttackKeyDown() )
				{
					ChangeSpecialMove_End( pOwner );

					ioChargeComboJump3 *pChargeCombo3 = ToChargeComboJump3( m_pExtendJump );
					if( pChargeCombo3 )
						pChargeCombo3->SetComboJumpAttack( pOwner );

					return;
				}
				else
				if( (m_dwSpecialChargeStartTime + m_dwSpecialChargeTime) < dwCurTime )
				{
					ChangeSpecialMove_End( pOwner );
					ioChargeComboJump3 *pChargeCombo3 = ToChargeComboJump3( m_pExtendJump );
					if( pChargeCombo3 )
						pChargeCombo3->ChangeToPowerChargeJumpAttack( pOwner );

					return;
				}
			}
			else
			{
				if( pOwner->IsAttackKeyDown() )
				{
					m_bSpecialChagerKey = true;
					m_dwSpecialChargeStartTime = dwCurTime;
					return;
				}
				else if( pOwner->IsDefenseKey() && m_fCurExtraGauge >= FLOAT1 )
				{
					ChangeSpecialMove_End( pOwner );
					ioChargeComboJump3 *pChargeCombo3 = ToChargeComboJump3( m_pExtendJump );
					if( pChargeCombo3 )
					{
						pChargeCombo3->SetCuchulainJumpAttack( pOwner );
						return;
					}
				}
			}
		}
		
		if( !ProcessSpecialMove_Move( pOwner ) )
		{
			m_SpecialMoveState = CFS_END;

			if( pOwner->IsNeedProcess() )
				ChangeSpecialMove_End( pOwner );
		}
		break;
	case CFS_END:
		break;
	}
}


void ioCuchulainItem::ChangeSpecialMove_Move( ioBaseChar *pOwner )
{
	if( !pOwner )
		return;

	ioEntityGroup *pGrp = pOwner->GetGroup();
	if( !pGrp )
		return;

	m_fFlyMoveCurRange = 0.f;
	m_SpecialMoveState = CFS_MOVE;

	pGrp->ClearAllLoopAni( FLOAT100, true );
	pGrp->ClearAllActionAni( FLOAT100, true );

	int iAniID	     = pGrp->GetAnimationIdx( m_CurFlyInfo.m_FlyAni );
	float fTimeRate  = m_CurFlyInfo.m_fFlyAniRate;

	pGrp->SetLoopAni( iAniID, FLOAT100, FLOAT1, FLOAT1/fTimeRate );
}


void ioCuchulainItem::ChangeSpecialMove_End( ioBaseChar *pOwner )
{
	m_SpecialMoveState = CFS_END;
	pOwner->SetCurMoveSpeed( 0.0f );
	/*pOwner->SetCurJumpPowerAmt( 0.0f );
	pOwner->SetGravityAmt( 0.0f );
	pOwner->SetForcePowerAmt( 0.0f );*/

	pOwner->SetSKillEndJumpState( m_CurFlyInfo.m_fEndJumpPower, false, false, false, true );
	pOwner->ClearDirDoubleClick();

	if( pOwner->IsNeedProcess() )
	{
		//SendPacket
		SP2Packet kPacket( CUPK_ITEM_GAUGE_CANCEL );
		FillItemGaugePacketHeader( pOwner, kPacket );
		kPacket << NST_SPECIAL_S_END_JUMP;
		kPacket << pOwner->GetWorldPosition();
		kPacket << pOwner->GetTargetRot();
		P2PNetwork::SendToAllPlayingUser( kPacket );
	}
}


bool ioCuchulainItem::ProcessSpecialMove_Move( ioBaseChar *pOwner )
{
	if( !pOwner )
		return false;

	DWORD dwCurTime = FRAMEGETTIME();
	float fTimePerSec = g_FrameTimer.GetSecPerFrame();

	D3DXVECTOR3 vTargetDir = pOwner->GetTargetRot() * D3DXVECTOR3( 0.0f, 0.0f, -FLOAT1 );
	D3DXVec3Normalize( &vTargetDir, &vTargetDir );

	D3DXVECTOR3 vAxis;
	D3DXVec3Cross( &vAxis, &vTargetDir, &ioMath::UNIT_Y );
	D3DXVec3Normalize( &vAxis, &vAxis );

	D3DXQUATERNION qtNewRot;
	float fCurAngle = ioMath::ArrangeHead( m_CurFlyInfo.m_fFlyAngle );
	D3DXQuaternionRotationAxis( &qtNewRot, &vAxis, DEGtoRAD(fCurAngle) );

	D3DXVECTOR3 vMoveDir = qtNewRot * vTargetDir;
	D3DXVec3Normalize( &vMoveDir, &vMoveDir );

	if( vMoveDir == ioMath::VEC3_ZERO )
		return false;
	
	float fCurSpeed = m_CurFlyInfo.m_fFlySpeed;
	float fDistance = fCurSpeed * fTimePerSec;

	bool bRangeLimit = false;
	D3DXVECTOR3 vMoveAmt( 0.0f, 0.0f, 0.0f );
	if( m_fFlyMoveCurRange+fDistance < m_CurFlyInfo.m_fFlyMaxRange )
	{
		bRangeLimit = true;
		vMoveAmt = vMoveDir * fDistance;

		m_fFlyMoveCurRange += fDistance;
	}
	else
	{
		bRangeLimit = false;

		float fGapDistance = m_CurFlyInfo.m_fFlyMaxRange - m_fFlyMoveCurRange;
		vMoveAmt = vMoveDir * fGapDistance;
	}

	bool bCol = false;
	ioPlayStage *pStage = pOwner->GetCreator();
	if( pStage && pStage->ReCalculateMoveVectorByTerrain( pOwner, &vMoveAmt, bCol ) )
	{
		pOwner->Translate( vMoveAmt );
	}

	D3DXVECTOR3 vPos = pOwner->GetWorldPosition();
	if( pOwner->CheckAxisLimit( &vPos ) )
		pOwner->SetWorldPosition( vPos );

	return bRangeLimit;
}


const ioHashString& ioCuchulainItem::GetAimedTarget()
{
	return m_AimedTarget;
}


void ioCuchulainItem::SetAimedTarget( const ioHashString& stTarget)
{
	m_AimedTarget = stTarget;
}


//////////////////////////////////////////////////////////////////////////

ioCuchulainItemSpecialState::ioCuchulainItemSpecialState()
{
}

ioCuchulainItemSpecialState::~ioCuchulainItemSpecialState()
{
}

void ioCuchulainItemSpecialState::Initialize( ioBaseChar* pOwner, CharState eCurrState )
{

}

void ioCuchulainItemSpecialState::Finalize( ioBaseChar* pOwner, CharState eCurrState, CharState eNewState )
{
	ioSpecialStateBase::Finalize( pOwner, eCurrState, eNewState );
}


bool ioCuchulainItemSpecialState::IsCharCollisionSkipState( const ioBaseChar *const pOwner, DWORD dwStartTime, DWORD dwEndTime, DWORD dwSkipType, bool bTeamOnly, bool bDefense, TeamType eOwnerTeam, TeamType eOtherTeam ) const
{
	return true;
}


void ioCuchulainItemSpecialState::CheckSpecialState( ioBaseChar* pOwner )
{
	ioCuchulainItem *pCuchulainItem = ToCuchulainWeaponItem( pOwner->GetPriorityItem( SPT_ATTACK ) );
	if( pCuchulainItem )
	{
		pCuchulainItem->ProcessSpecialMove( pOwner );
	}
	else
	{
		pOwner->SetState( CS_DELAY );
		pOwner->SetFallState( true );
		return;
	}
}


void ioCuchulainItemSpecialState::CheckFallStateChange( ioBaseChar* pOwner, float fMinFallHeightRate, float fBottomHeight, bool bBottomMaxCheck, float fMaxBottomHeight )
{
	return;
}


bool ioCuchulainItemSpecialState::ProcessMove( ioBaseChar* pOwner, float fTimePerSec, DWORD dwPreTime, DWORD dwCurTime )
{
	return true;
}