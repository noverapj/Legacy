
#include "stdafx.h"

#include "ioPlayStage.h"
#include "ioBaseChar.h"
#include "ioEvyItem.h"

#include "WeaponDefine.h"
#include "ItemDefine.h"
#include "BuffDefine.h"

ioEvyItem::ioEvyItem()
{
	m_ChargeState = ES_NONE;

	m_dwAttackStartTime = 0;
	m_iCurCombo = 0;
	m_nAttackCnt = 0;

	m_fCurChargeRate = 0.0f;
	m_fCurChargeRateForSkill = 0.0f;

	m_CurDirKey = ioUserKeyInput::DKI_NONE;

	m_CurBlinkInfo.Init();

	m_vFlyMoveDir = ioMath::VEC3_ZERO;
	m_fFlyMoveSpeed = 0.0f;

	m_fCurGauge = 0.0f;
	m_BlankState = BS_NONE;
}

ioEvyItem::ioEvyItem( const ioEvyItem &rhs )
: ioWeaponItem( rhs ),
m_AttackReadyAniList( rhs.m_AttackReadyAniList ),
m_fMaxGauge( rhs.m_fMaxGauge ),
m_fNeedGauge( rhs.m_fNeedGauge ),
m_fIncreaseGauge( rhs.m_fIncreaseGauge ),
m_fDecreaseGauge( rhs.m_fDecreaseGauge ),

m_vfGatherAniRate( rhs.m_vfGatherAniRate ),
m_vGatherAnimation( rhs.m_vGatherAnimation ),

m_NormalBlinkInfo( rhs.m_NormalBlinkInfo ),

m_ChargeAttackList( rhs.m_ChargeAttackList ),

m_stSD_Blow_Motion( rhs.m_stSD_Blow_Motion ),
m_fSD_BlowMotionRate( rhs.m_fSD_BlowMotionRate )
{
	m_ChargeState = ES_NONE;

	m_dwAttackStartTime = 0;
	m_iCurCombo = 0;
	m_nAttackCnt =0;

	m_fCurChargeRate = 0.0f;
	m_fCurChargeRateForSkill = 0.0f;

	m_CurDirKey = ioUserKeyInput::DKI_NONE;

	m_BlankState = BS_NONE;
	m_CurBlinkInfo.Init();

	m_vFlyMoveDir = ioMath::VEC3_ZERO;
	m_fFlyMoveSpeed = 0.0f;

	m_fCurGauge = 0.0f;

	m_bThirdChargeAttack = false;

	m_bSDSkillUsed = false;
}

ioEvyItem::~ioEvyItem()
{
}

void ioEvyItem::LoadProperty( ioINILoader &rkLoader )
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

	LoadEvyInfo( rkLoader );
	

	m_fMaxGauge = rkLoader.LoadFloat_e( "max_gauge", FLOAT1 );
	m_fNeedGauge = rkLoader.LoadFloat_e( "need_gauge", FLOAT100 );

	m_fIncreaseGauge = rkLoader.LoadFloat_e( "increase_gauge", .0f );
	m_fDecreaseGauge = rkLoader.LoadFloat_e( "decrease_gauge", 2.0f );
}

void ioEvyItem::LoadEvyInfo( ioINILoader &rkLoader )
{
	char szBuf[MAX_PATH] = "";
	char szKey[MAX_PATH] = "";
	
	//m_AttackGoomihoFly.Init();

	rkLoader.LoadString_e( "special_dash_animation", "", szBuf, MAX_PATH );
	m_NormalBlinkInfo.m_stDashAni = szBuf;
	m_NormalBlinkInfo.m_fDashAniRate = rkLoader.LoadFloat_e( "special_dash_ani_rate", FLOAT1 );
	m_NormalBlinkInfo.m_fDashSpeed = rkLoader.LoadFloat_e( "special_dash_speed", 0.f );
	m_NormalBlinkInfo.m_fDashLength = rkLoader.LoadFloat_e( "special_dash_range", 0.f );

	rkLoader.LoadString_e( "special_dash_end_ani", "", szBuf, MAX_PATH );
	m_NormalBlinkInfo.m_stDashEndAni = szBuf;
	m_NormalBlinkInfo.m_fDashEndAniRate = rkLoader.LoadFloat_e( "special_dash_end_ani_rate", FLOAT1 );

	m_NormalBlinkInfo.m_fMaxBlinkLength = rkLoader.LoadFloat_e( "max_blink_length", 0.f );
	m_NormalBlinkInfo.m_fMapHeightGap = rkLoader.LoadFloat_e( "max_height_gap", 0.f );

	rkLoader.LoadString_e( "blink_start_ani", "", szBuf, MAX_PATH );
	m_NormalBlinkInfo.m_StartBlinkMotion = szBuf;
	m_NormalBlinkInfo.m_fStartBlinkMotionRate = rkLoader.LoadFloat_e( "blink_start_ani_rate", FLOAT1 );

	rkLoader.LoadString_e( "blink_end_ani", "", szBuf, MAX_PATH );
	m_NormalBlinkInfo.m_EndBlinkMotion = szBuf;
	m_NormalBlinkInfo.m_fEndBlinkMotionRate = rkLoader.LoadFloat_e( "blink_end_ani_rate", FLOAT1 );

	m_NormalBlinkInfo.m_fJumpAmt = rkLoader.LoadFloat_e( "blink_end_jump_power", 0.f );

	//////////////////////////////////////////////////////////////////////////
	int iExtendCnt = rkLoader.LoadInt_e( "max_extend_attack_cnt", 0 );
	m_NormalBlinkInfo.m_SpecialAttributeList.clear();
	m_NormalBlinkInfo.m_SpecialAttributeList.reserve( iExtendCnt );
	for( int i=0 ; i<iExtendCnt ; i++ )
	{
		AttackAttribute kAttribute;
		wsprintf( szBuf, "extend_attack0%d", i+1 );
		LoadAttackAttribute( szBuf, kAttribute, rkLoader );
		m_NormalBlinkInfo.m_SpecialAttributeList.push_back( kAttribute );

		wsprintf( szKey, "gathering_animation%d", i+1 );

		rkLoader.LoadString( szKey, "", szBuf, MAX_PATH );
		m_vGatherAnimation.push_back(szBuf);

		wsprintf( szKey, "gathering_ani_rate%d", i+1 );
		float fRate = rkLoader.LoadFloat( szKey, FLOAT1 );
		m_vfGatherAniRate.push_back(fRate);
	}

	m_NormalBlinkInfo.m_fSpecilAfterJumpPower = rkLoader.LoadFloat_e( "afeter_special_jump_amt", 0.f );

	rkLoader.LoadString_e( "special_blow_ani", "", szBuf, MAX_PATH );
	m_stSD_Blow_Motion = szBuf;
	m_fSD_BlowMotionRate = rkLoader.LoadFloat_e( "special_blow_ani_rate", FLOAT1 );
}

ioItem* ioEvyItem::Clone()
{
	return new ioEvyItem( *this );
}

ioWeaponItem::WeaponSubType ioEvyItem::GetSubType() const
{
	return WST_EVY_ITEM;
}

void ioEvyItem::SetNormalAttack( ioBaseChar *pOwner, int iCurCombo )
{
	if( !pOwner ) return;

	m_fCurChargeRate = 0.0f;
	m_fCurChargeRateForSkill = 0.0f;

	m_dwKeyReserveTime = 0;
	m_bEndAttackJump = false;

	m_iSkillInput = -1;
	m_bEnableDefenseState = false;
	m_bEnableAttackState = false;
	m_bEnableJumpState = false;

	m_CurDirKey = ioUserKeyInput::DKI_NONE;
	m_BlankState = BS_NONE;

	ClearSpecialState( pOwner );

	int iComboCnt = GetMaxCombo();
	if( !COMPARE( iCurCombo, 0, iComboCnt ) )
	{
		ioWeaponItem::SetNormalAttack( pOwner, iCurCombo );

		m_ChargeState = ES_NONE;
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

		m_ChargeState = ES_CHARGING;
		m_dwAttackStartTime = FRAMEGETTIME();
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

void ioEvyItem::CheckNormalAttack( ioBaseChar *pOwner )
{
	DWORD dwCurTime = FRAMEGETTIME();

	switch( m_ChargeState )
	{
	case ES_CHARGING:
		OnCharging( pOwner );
		break;
	case ES_GATHERING:
		OnGathering( pOwner );
		break;
	case ES_SPECIAL_ATTACK:
		if( m_dwMotionEndTime < dwCurTime )
		{
			if( m_bEndAttackJump && m_NormalBlinkInfo.m_fSpecilAfterJumpPower > 0.f )
			{
				if( pOwner->IsNeedProcess() )
				{
					pOwner->SetSKillEndJumpState( m_NormalBlinkInfo.m_fSpecilAfterJumpPower, false, false, false, true );
					//위에 함수에서 패킷을 보내면 받는 쪽에서 점프가 안되서 이렇게 처리
					if( pOwner->IsNeedSendNetwork() )
					{
						SP2Packet kPacket( CUPK_ITEM_GAUGE_OK );
						FillItemGaugePacketHeader( pOwner, kPacket );
						kPacket << ES_SEECIAL_ATTCK_JUMP;
						kPacket << pOwner->GetTargetRot();
						kPacket << pOwner->GetWorldPosition();
						P2PNetwork::SendToAllPlayingUser( kPacket );
					}
				}
			}
			else
				pOwner->SetState( CS_DELAY, false );

			return;
		}
		break;
	
	case ES_NORMAL_ATTACK:
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

void ioEvyItem::OnCharging( ioBaseChar *pOwner )
{
	m_bKeyReserved = false;
	m_bAttackKeyReserved = false;
	m_bDefenseKeyReserved = false;
	m_bJumpKeyReserved = false;

	if( !pOwner ) return;
	if( !pOwner->IsNeedProcess() )	return;

	if( (pOwner->IsAttackKeyDown() && !pOwner->IsHasCrown()) || (pOwner->IsAttackKeyDown() && pOwner->HasBuff( BT_CHECK_CHAIN )) )
	{
		if( m_dwAttackStartTime + m_dwChargeTime <= FRAMEGETTIME() )
		{
			ChangeToGathering( pOwner );
			return;
		}
	}
	else
	{
		ChangeToNormalAttack( pOwner );
	}
}

void ioEvyItem::OnGathering( ioBaseChar *pOwner )
{
	if( !pOwner ) return;
	if( !pOwner->IsNeedProcess() )	return;

	if( (pOwner->IsAttackKeyDown() && !pOwner->IsHasCrown()) || (pOwner->IsAttackKeyDown() && pOwner->HasBuff( BT_CHECK_CHAIN )) )
	{
		DWORD dwCurTime = FRAMEGETTIME();

		if( m_dwMotionEndTime < dwCurTime )
		{
			if( pOwner->HasBuff( BT_CHECK_CHAIN ) ) 
			{
				m_bSDSkillUsed = false;
				ChangeToSpecialBlow( pOwner );
			}
			else
			{
				//ChangeToDashState( pOwner );
				SetEvyBlinkState( pOwner, m_NormalBlinkInfo, true );
			}

			return;
		}
	}
	else
	{
		//디꾹때기
		ChangeToChargeAttack( pOwner, m_iCurCombo );
	}
}

void ioEvyItem::OnDash( ioBaseChar *pOwner )
{
	if( !pOwner ) return;

	ioPlayStage *pStage = pOwner->GetCreator();
	if( !pStage ) return;

	/*if( pOwner->IsNeedProcess() && pOwner->IsJumpKeyDown() )
	{
		pOwner->SetExtendAttackEndJump( m_fDashJumpAmt, m_fExtendLandingRate );
		return;
	}*/

	float fTimePerSec = g_FrameTimer.GetSecPerFrame();

	bool bCurMove = false;
	D3DXVECTOR3 vTotalMove( 0.0f, 0.0f, 0.0f );

	float fCurTimeGap = fTimePerSec * FLOAT1000;
	while( fCurTimeGap > 0.0f )
	{
		float fDivideTimePerSec = min( fCurTimeGap, MIN_FRAME_TIME ) * 0.001f;
		fCurTimeGap -= MIN_FRAME_TIME;

		D3DXVECTOR3 vMoveAmt;

		if( m_CurBlinkInfo.m_fDashSpeed > 0.0f && m_vDashMoveDir != ioMath::VEC3_ZERO )
		{
			float fFinalMoveAmt = m_CurBlinkInfo.m_fDashSpeed * fDivideTimePerSec;
			if( fFinalMoveAmt > 0.0f )
			{
				vMoveAmt = m_vDashMoveDir * fFinalMoveAmt;
				vTotalMove += vMoveAmt;
				bCurMove = true;

				m_fDashMovedLenth += fFinalMoveAmt;
			}
		}

		if( pOwner->ProcessForceMove( fDivideTimePerSec, vMoveAmt ) )
		{
			vTotalMove += vMoveAmt;
			bCurMove = true;
		}
	}

	if( bCurMove )
	{
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

					float fTopHeight = pStage->GetMapHeight( vCurPos.x,
															 vCurPos.z,
															 pOwner,
															 false,
															 pOwner->GetMidHeightByRate( FLOAT1 ) );

					if( fTopHeight > FLOAT1 )
					{
						vCurPos.y = max( fTopHeight, fPreHeight );
						pOwner->SetWorldPosition( vCurPos );
					}
				}
			}
		}
	}

	D3DXVECTOR3 vPos = pOwner->GetWorldPosition();
	float fPreHeight = pOwner->GetBottomHeight();
	
	if( vPos.y < fPreHeight )
		vPos.y = fPreHeight;

	if( pStage && pOwner->IsAxisLimit() )
	{
		pStage->CheckAxisLimit( &vPos );
		pOwner->SetWorldPosition( vPos );
	}

	if( pOwner->IsNeedProcess() )
	{
		//DWORD dwCurTime = FRAMEGETTIME();

		//if( dwCurTime > m_dwKeyReserveTime )
			KeyReserve(pOwner);
	}
}

void ioEvyItem::OnDashEnding( ioBaseChar *pOwner )
{
	if( !pOwner->IsNeedProcess() )
		return;

	DWORD dwCurTime = FRAMEGETTIME();

	//if(m_dwKeyReserveTime > 0 && dwCurTime > m_dwKeyReserveTime )
		KeyReserve(pOwner);
}

void ioEvyItem::OnBlinkEnding( ioBaseChar *pOwner )
{
	if( !pOwner->IsNeedProcess() )
		return;

	DWORD dwCurTime = FRAMEGETTIME();

	if(m_dwKeyReserveTime > 0 && dwCurTime > m_dwKeyReserveTime )
		KeyReserve(pOwner);
}

void ioEvyItem::ChangeToNormalAttack( ioBaseChar *pOwner )
{
	DWORD dwCurTime = FRAMEGETTIME();

	//
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
		{
			pOwner->SetAutoTarget( ATT_NORMAL );
		}

		pOwner->SetNormalAttackByAttribute( *pAttribute );

		//
		SetCurAttackAniForInputJump( pAttribute->m_AttackAnimation, pAttribute->m_fAttackAniRate );
	}

	pOwner->ApplyNormalAttackResult( m_iCurCombo );

	m_ChargeState = ES_NORMAL_ATTACK;

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

void ioEvyItem::ChangeToChargeAttack( ioBaseChar *pOwner, int iCurCombo )
{
	if( !pOwner ) return;

	DWORD dwCurTime = FRAMEGETTIME();

	ioEntityGroup *pGrp = pOwner->GetGroup();
	if( !pGrp )
		return;

	if( iCurCombo >= (int)m_NormalBlinkInfo.m_SpecialAttributeList.size())
		return;

	pOwner->SetAttackMoveEnable( false );

	pGrp->ClearAllLoopAni( FLOAT100, true );
	pGrp->ClearAllActionAni( FLOAT100, true );

	m_ChargeState = ES_SPECIAL_ATTACK;

	int iAniID	     = pGrp->GetAnimationIdx( m_NormalBlinkInfo.m_SpecialAttributeList[iCurCombo].m_AttackAnimation );
	float fTimeRate  = m_NormalBlinkInfo.m_SpecialAttributeList[iCurCombo].m_fAttackAniRate;
	if( fTimeRate <= 0.0f )
		fTimeRate = FLOAT1;

	DWORD dwPreDelay = m_NormalBlinkInfo.m_SpecialAttributeList[iCurCombo].m_dwPreDelay;
	pGrp->SetActionAni( iAniID, FLOAT100, FLOAT100, FLOAT1, FLOAT1/fTimeRate, 0.0f, 0.0f, true );

	DWORD dwCurBaseIndex = pOwner->IncreaseWeaponIndexBase();
	pOwner->RefreshFireTimeListWithBase( dwCurBaseIndex,
		iAniID,
		m_NormalBlinkInfo.m_SpecialAttributeList[iCurCombo].m_vWeaponInfoList,
		FTT_NORMAL_ATTACK,
		fTimeRate,
		dwPreDelay );

	pOwner->SetReservedSliding( m_NormalBlinkInfo.m_SpecialAttributeList[iCurCombo].m_vForceInfoList, iAniID, fTimeRate, dwPreDelay );

	m_dwMotionEndTime = dwCurTime + pGrp->GetAnimationFullTime( iAniID ) * fTimeRate;

	if( iCurCombo == 0 )
		m_bEndAttackJump = true;

	//Send
	if( pOwner->IsNeedSendNetwork() )
	{
		SP2Packet kPacket( CUPK_ITEM_GAUGE_OK );
		FillItemGaugePacketHeader( pOwner, kPacket );
		kPacket << m_ChargeState;
		kPacket << pOwner->GetTargetRot();
		kPacket << pOwner->GetWorldPosition();
		kPacket << iCurCombo;
		P2PNetwork::SendToAllPlayingUser( kPacket );
	}
}

void ioEvyItem::ChangeToGathering( ioBaseChar *pOwner )
{
	if( !pOwner )
		return;

	DWORD dwCurTime = FRAMEGETTIME();

	ioEntityGroup *pGrp = pOwner->GetGroup();
	if( !pGrp )
		return;

	pOwner->ClearReservedSliding();
	pOwner->SetForcePowerAmt( 0.0f );

	pGrp->ClearAllLoopAni( FLOAT100, true );
	pGrp->ClearAllActionAni( FLOAT100, true );

	int iAniID	     = pGrp->GetAnimationIdx( m_vGatherAnimation[ m_iCurCombo] );
	float fTimeRate  = m_vfGatherAniRate[ m_iCurCombo ];

	pGrp->SetActionAni( iAniID, FLOAT100, FLOAT100, FLOAT1, FLOAT1/fTimeRate, 0.0f, 0.0f, true );

	m_ChargeState = ES_GATHERING;

	m_dwMotionEndTime = dwCurTime + pGrp->GetAnimationFullTime( iAniID ) * fTimeRate;

	if( pOwner->IsNeedSendNetwork() )
	{
		SP2Packet kPacket( CUPK_ITEM_GAUGE_OK );
		FillItemGaugePacketHeader( pOwner, kPacket );
		kPacket << m_ChargeState;
		kPacket << pOwner->GetTargetRot();
		kPacket << pOwner->GetWorldPosition();
		P2PNetwork::SendToAllPlayingUser( kPacket );
	}
}

void ioEvyItem::ChangeToSpecialBlow( ioBaseChar *pOwner )
{
	if( !pOwner )
		return;

	DWORD dwCurTime = FRAMEGETTIME();

	ioEntityGroup *pGrp = pOwner->GetGroup();
	if( !pGrp )
		return;

	pOwner->ClearReservedSliding();
	pOwner->SetForcePowerAmt( 0.0f );

	pGrp->ClearAllLoopAni( FLOAT100, true );
	pGrp->ClearAllActionAni( FLOAT100, true );

	int iAniID	     = pGrp->GetAnimationIdx( m_stSD_Blow_Motion );
	float fTimeRate  = m_fSD_BlowMotionRate;

	pGrp->SetActionAni( iAniID, FLOAT100, FLOAT100, FLOAT1, FLOAT1/fTimeRate, 0.0f, 0.0f, true );

	m_BlankState = BS_BOMB_MOTION;

	m_dwMotionEndTime = dwCurTime + pGrp->GetAnimationFullTime( iAniID ) * fTimeRate;

	pOwner->SetState( CS_EVY_BOMB_STATE );

	if( pOwner->IsNeedProcess() )
	{
		m_dwKeyReserveTime = pGrp->GetAnimationEventTime_e( iAniID, "key_reserve" ) * fTimeRate;

		if( m_dwKeyReserveTime )
			m_dwKeyReserveTime += dwCurTime;
	}

	//SendPacket
	if( pOwner->IsNeedSendNetwork() )
	{
		SP2Packet kPacket( CUPK_ITEM_GAUGE_CANCEL );
		FillItemGaugePacketHeader( pOwner, kPacket );
		kPacket << m_BlankState;
		kPacket << pOwner->GetTargetRot();
		kPacket << pOwner->GetWorldPosition();
		kPacket << m_fCurGauge;
		P2PNetwork::SendToAllPlayingUser( kPacket );
	}

}

void ioEvyItem::ChangeToDashState( ioBaseChar *pOwner )
{
	if( !pOwner ) return;

	ioEntityGroup *pGrp = pOwner->GetGroup();
	if( !pGrp )
		return;

	DWORD dwCurTime = FRAMEGETTIME();

	pOwner->ClearReservedSliding();
	pOwner->SetForcePowerAmt( 0.0f );

	pGrp->ClearAllLoopAni( FLOAT100, true );
	pGrp->ClearAllActionAni( FLOAT100, true );

	int iAniID	     = pGrp->GetAnimationIdx( m_CurBlinkInfo.m_stDashAni );
	float fTimeRate  = m_CurBlinkInfo.m_fDashAniRate;
	if( fTimeRate <= 0.0f )
		fTimeRate = FLOAT1;

	pGrp->SetActionAni( iAniID, FLOAT100, FLOAT100, FLOAT1, FLOAT1/fTimeRate, 0.0f, 0.0f, true );

	pOwner->CheckCharColSkipTime( iAniID, fTimeRate );

	m_BlankState = BS_DASH;

	m_fDashMovedLenth = 0.f;

	if( pOwner->IsNeedProcess() )
	{
		CheckDashMoveDir( pOwner );

		m_dwKeyReserveTime = pGrp->GetAnimationEventTime_e( iAniID, "key_reserve" ) * fTimeRate;

		if( m_dwKeyReserveTime )
			m_dwKeyReserveTime += dwCurTime;

		/*D3DXQUATERNION qtTargetRot;
		ioMath::CalcDirectionQuaternion( qtTargetRot, -m_vDashMoveDir );
		pOwner->SetTargetRotAndMoveDirChange( qtTargetRot );*/
	}
}

void ioEvyItem::ChangeToDashEnding( ioBaseChar *pOwner )
{
	if( !pOwner ) return;

	DWORD dwCurTime = FRAMEGETTIME();

	ioEntityGroup *pGrp = pOwner->GetGroup();
	if( !pGrp )
		return;

	pOwner->ClearReservedSliding();
	pOwner->SetForcePowerAmt( 0.0f );
	pOwner->ClearDirDoubleClick();

	pGrp->ClearAllLoopAni( FLOAT100, true );
	pGrp->ClearAllActionAni( FLOAT100, true );

	int iAniID	     = pGrp->GetAnimationIdx( m_CurBlinkInfo.m_stDashEndAni );
	float fTimeRate  = m_CurBlinkInfo.m_fDashEndAniRate;
	if( fTimeRate <= 0.0f )
		fTimeRate = FLOAT1;

	DWORD dwPreDelay = 0;//m_DashAttackMove.m_dwPreDelay;

	pGrp->SetActionAni( iAniID, FLOAT100, FLOAT100, FLOAT1, FLOAT1/fTimeRate, 0.0f, 0.0f, true );

	m_dwMotionEndTime = dwCurTime + pGrp->GetAnimationFullTime( iAniID ) * fTimeRate;

	m_BlankState = BS_DASH_ENDING;

	m_dwKeyReserveTime = pGrp->GetAnimationEventTime_e( iAniID, "key_reserve" ) * fTimeRate + dwPreDelay;
	if( m_dwKeyReserveTime )
		m_dwKeyReserveTime += dwCurTime;
	//send

	/*if( pOwner->IsNeedSendNetwork() )
	{
		SP2Packet kPacket( CUPK_ITEM_GAUGE_OK );
		FillItemGaugePacketHeader( pOwner, kPacket );
		kPacket << m_ChargeState;
		kPacket << pOwner->GetTargetRot();
		kPacket << pOwner->GetWorldPosition();
		P2PNetwork::SendToAllPlayingUser( kPacket );
	}*/
}

void ioEvyItem::ChangeToEnd( ioBaseChar *pOwner )
{
	m_BlankState = BS_END;

	if( pOwner->IsNeedSendNetwork() )
	{
		SP2Packet kPacket( CUPK_ITEM_GAUGE_OK );
		FillItemGaugePacketHeader( pOwner, kPacket );
		kPacket << m_BlankState;
		kPacket << pOwner->GetTargetRot();
		kPacket << pOwner->GetWorldPosition();
		P2PNetwork::SendToAllPlayingUser( kPacket );
	}

	pOwner->SetState( CS_DELAY, false );
}

void ioEvyItem::CheckDashMoveDir( ioBaseChar *pOwner )
{
	m_vDashMoveDir = pOwner->GetTargetRot() * D3DXVECTOR3( 0.0f, 0.0f, -FLOAT1 );
	D3DXVec3Normalize( &m_vDashMoveDir, &m_vDashMoveDir );
}

void ioEvyItem::OnItemGaugeOK( ioBaseChar *pOwner, SP2Packet &rkPacket )
{
	int iState;
	rkPacket >> iState;
	
	D3DXQUATERNION qtRot;
	D3DXVECTOR3 vPos;
	int iComboCnt;

	switch( iState )
	{
	case ES_NONE:
		rkPacket >> m_iCurCombo;

		pOwner->SetAttackMoveEnable( false );
		pOwner->SetChargingState( false );

		m_ChargeState = ES_NONE;
		break;

	case ES_NORMAL_ATTACK:
		rkPacket >> m_iCurCombo;

		if( !m_bChargeAutoTarget )
			pOwner->ApplyAutoTargetInfo( rkPacket );
		ChangeToNormalAttack( pOwner );
		break;

	case ES_GATHERING:
		rkPacket >> qtRot;
		rkPacket >> vPos;

		pOwner->SetTargetRotToRotate( qtRot, true );
		pOwner->SetWorldPosition( vPos );

		ChangeToGathering(pOwner);
		break;

	case ES_SPECIAL_ATTACK:
		rkPacket >> qtRot;
		rkPacket >> vPos;
		rkPacket >> iComboCnt;

		pOwner->SetTargetRotToRotate( qtRot, true );
		pOwner->SetWorldPosition( vPos );

		ChangeToChargeAttack(pOwner, iComboCnt);
		break;

	case ES_SEECIAL_ATTCK_JUMP:
		rkPacket >> qtRot;
		rkPacket >> vPos;

		pOwner->SetTargetRotToRotate( qtRot, true );
		pOwner->SetWorldPosition( vPos );

		pOwner->SetSKillEndJumpState( m_NormalBlinkInfo.m_fSpecilAfterJumpPower, false, false, false, true );
		break;
	}
}

void ioEvyItem::OnItemGaugeCancel( ioBaseChar *pOwner, SP2Packet &rkPacket )
{
	int iSyncState;
	rkPacket >> iSyncState;

	D3DXQUATERNION qtRot;
	D3DXVECTOR3 vPos;
	float fGauge;

	rkPacket >> qtRot;
	rkPacket >> vPos;
	rkPacket >> fGauge;

	pOwner->SetTargetRotToRotate( qtRot, true );
	pOwner->SetWorldPosition( vPos );

	m_fCurGauge = fGauge;

	float fJumpPower;

	switch( iSyncState )
	{
	case BS_NONE:
		SetEvyBlinkState( pOwner, m_NormalBlinkInfo, false );
		break;
	case BS_BOMB_MOTION:
		ChangeToSpecialBlow( pOwner );
		break;
	case BS_DASH:
		break;
	case BS_DASH_ENDING:
		break;
	case BS_DASH_END_JUMP:
		ChangeDashEndJump( pOwner );
		break;
	case BS_STARTMOTION:
		ChangeToBlinkStartMotion( pOwner );
		break;
	case BS_ENDMOTION:
		ChangeToBlinkEndMotion( pOwner );
		break;
	case BS_END_JUMP:
		rkPacket >> fJumpPower;
		pOwner->SetSKillEndJumpState( fJumpPower, false, false, false, true );
		break;
	case BS_END:
		ChangeToEnd( pOwner );
		break;
	}
}

void ioEvyItem::SetExtraInfoToWeapon( ioWeapon *pWeapon, bool bJumpAttack )
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
			pZone->SetZoneValue( true, false );
		}
	}
}

bool ioEvyItem::IsChargeAttackState( ioBaseChar *pOwner )
{
	if( pOwner->GetState() == CS_ATTACK )
	{
		switch( m_ChargeState )
		{
		case ES_CHARGING:
			return true;
		}
	}

	return false;
}

void ioEvyItem::OnEndExtendAttack( ioBaseChar *pOwner, bool bReleaseItem )
{
	m_ChargeState = ES_NONE;

	pOwner->SetAttackMoveEnable( false );
	pOwner->SetChargingState( false );

	pOwner->ClearDirDoubleClick();

	m_fCurChargeRate = 0.0f;
}

bool ioEvyItem::IsChargeAutoTarget( ioBaseChar *pOwner )
{
	if( !pOwner )
		return false;

	if( pOwner->GetState() != CS_ATTACK )
		return false;

	if( m_fCurChargeRate == 0.0f )
		return false;

	if( m_fMaxTargetRangeRate == 0.0f || m_fMaxTargetAngleRate == 0.0f )
		return false;

	return true;
}

void ioEvyItem::CalcBlankPos( ioBaseChar *pOwner )
{
	if( !pOwner )
		return;

	ioPlayStage *pStage = pOwner->GetCreator();
	if( !pStage) 
		return;

	D3DXVECTOR3 vLook = pOwner->GetTargetRot() * D3DXVECTOR3( 0.f, 0.f, -FLOAT1 );
	D3DXVec3Normalize( &vLook, &vLook );

	D3DXVECTOR3 vAddVec = vLook * m_CurBlinkInfo.m_fMaxBlinkLength;

	D3DXVECTOR3 vWorldPosSrc = pOwner->GetWorldPosition();

	D3DXVECTOR3 vWorldPosDest = vWorldPosSrc + vAddVec;

	D3DXVECTOR3 vBackLook = vLook * -FLOAT1;

	D3DXVec3Normalize( &vBackLook, &vBackLook );

	D3DXVECTOR3 vDist = vWorldPosDest - vWorldPosSrc;

	//float fMaxLength = D3DXVec3Length(&vDist);
	float fMovedLength = 0.f;

	float fCalcGap = FLOAT10;//<--이건 나중에 좀 가변적으로 해볼까..나....해킹이 걱정이면 하드 코딩으로

	while( m_CurBlinkInfo.m_fMaxBlinkLength > fMovedLength + fCalcGap )
	{
		float fMapHeight = pStage->GetMapHeight( vWorldPosDest.x, vWorldPosDest.z );
		if( fMapHeight > vWorldPosSrc.y + m_CurBlinkInfo.m_fMapHeightGap )
		{
			//뒤로 후퇴
			fMovedLength += fCalcGap;

			vWorldPosDest = vWorldPosDest + (vBackLook*fCalcGap);

			continue;
		}

		if( pStage->GetMapHeight( vWorldPosDest.x, vWorldPosDest.z ) > FLOAT1 )
			vWorldPosDest.y = fMapHeight;

		if( vWorldPosDest.y < vWorldPosSrc.y )
			vWorldPosDest.y = vWorldPosSrc.y;

		pOwner->SetWorldPosition( vWorldPosDest );
		break;

	}
	//float 

	/*if( !pOwner )
		return;

	ioPlayStage *pStage = pOwner->GetCreator();
	if( !pStage) 
		return;

	D3DXVECTOR3 vLook = pOwner->GetTargetRot() * D3DXVECTOR3( 0.f, 0.f, -FLOAT1 );
	D3DXVec3Normalize( &vLook, &vLook );

	float fCalcGap = FLOAT10;

	bool bCol = false;
	float fTotalMoveLenth = 0.f;
	while( fTotalMoveLenth < m_CurBlankInfo.m_fMaxLength )
	{
		D3DXVECTOR3 vMoveDir;
		fTotalMoveLenth += fCalcGap;

		vMoveDir = vLook * fCalcGap;

		if( pStage->ReCalculateMoveVectorByTerrain( pOwner, &vMoveDir, bCol ) )
		{
			float fPreHeight = pOwner->GetBottomHeight();
			pOwner->Translate( vMoveDir );

			if( pOwner->GetForceAmt() != 0.0f )	// 빠른 ForceMove로 경사면을 통과하는 경우에 대한 보강
			{
				float fMoveHeight = pOwner->GetBottomHeight();

				if( fPreHeight > 0.0f && fMoveHeight <= FLOAT1 )
				{
					D3DXVECTOR3 vCurPos = pOwner->GetWorldPosition();

					float fTopHeight = pStage->GetMapHeight( vCurPos.x,
						vCurPos.z,
						pOwner,
						false,
						pOwner->GetMidHeightByRate( FLOAT1 ) );

					if( fTopHeight > FLOAT1 )
					{
						vCurPos.y = max( fTopHeight, fPreHeight );
						pOwner->SetWorldPosition( vCurPos );
					}
				}
			}

			D3DXVECTOR3 vPos = pOwner->GetWorldPosition();
			fPreHeight = pOwner->GetBottomHeight();

			if( vPos.y < fPreHeight )
				vPos.y = fPreHeight;

			if( pStage && pOwner->IsAxisLimit() )
			{
				pStage->CheckAxisLimit( &vPos );
				pOwner->SetWorldPosition( vPos );
			}
		}
		else
			break;
	}*/
}

void ioEvyItem::SetEvyBlinkState( ioBaseChar *pOwner, const EvyBlinkInfo &rkInfo, bool bSendNet )
{
	if( !pOwner )
		return;

	ioEntityGroup *pGrp = pOwner->GetGroup();
	if( !pGrp )
		return;

	pOwner->SetState( CS_EVY_BLINK );

	m_CurBlinkInfo = rkInfo;
	m_BlankState = BS_NONE;

	CheckDashMoveDir( pOwner );

	ClearReserveKey();

	if( bSendNet && pOwner->IsNeedSendNetwork() )
	{
		SP2Packet kPacket( CUPK_ITEM_GAUGE_CANCEL );
		FillItemGaugePacketHeader( pOwner, kPacket );
		kPacket << m_BlankState;
		kPacket << pOwner->GetTargetRot();
		kPacket << pOwner->GetWorldPosition();
		kPacket << m_fCurGauge;
		P2PNetwork::SendToAllPlayingUser( kPacket );
	}
}

void ioEvyItem::ProcessBlinkState( ioBaseChar *pOwner )
{
	DWORD dwCurTime = FRAMEGETTIME();

	switch( m_BlankState )
	{
		// 위치를 이동시키고
	case BS_BOMB_MOTION:
		{
			if( m_dwMotionEndTime < dwCurTime )
			{
				pOwner->SetState( CS_DELAY, false );
				return;
			}
				//ChangeToEnd(pOwner);
		}
		break;
	case BS_NONE:
		if( m_CurBlinkInfo.m_fDashLength > 0)
		{
			ChangeToDashState( pOwner );
		}
		else
		{
			if( pOwner->IsNeedProcess() )
				SetCurBullet( max( 0, GetCurBullet() - GetNeedBullet() ) );

			ChangeToBlinkStartMotion( pOwner );
		}
		
		break;
	case BS_DASH:
		if( m_fDashMovedLenth > m_CurBlinkInfo.m_fDashLength )
		{
			ChangeToDashEnding(pOwner);
			//ChangeToEnd( pOwner );
			return;
		}

		OnDash( pOwner );
		if( pOwner->IsNeedProcess() )//&& dwCurTime >= m_dwKeyReserveTime )
		{
			if( m_bAttackKeyReserved && GetCurBullet() >= GetNeedBullet() )
			{
				SetCurBullet( max( 0, GetCurBullet() - GetNeedBullet() ) );
				ChangeToBlinkStartMotion( pOwner );
			}
		}
		break;
	case BS_DASH_ENDING:
		if( pOwner->IsNeedProcess() )
		{
			if( m_bAttackKeyReserved  && GetCurBullet() >= GetNeedBullet() )
			{
				SetCurBullet( max( 0, GetCurBullet() - GetNeedBullet() ) );
				ChangeToBlinkStartMotion( pOwner );
				return;
			}

			if( m_dwMotionEndTime < dwCurTime )
			{
				if( m_bDefenseKeyReserved && pOwner->IsDefenseKeyDown() )
					ChangeDefenseState(pOwner);
				else if( m_bJumpKeyReserved )
					ChangeDashEndJump( pOwner );
				else
					ChangeToEnd( pOwner );
				return;
			}
		}

		OnDashEnding( pOwner );
		break;
	case BS_STARTMOTION:
		if( pOwner->IsNeedProcess() && m_dwMotionEndTime < dwCurTime )
		{
			CalcBlankPos( pOwner );
			ChangeToBlinkEndMotion( pOwner );
		}
		break;
	case BS_ENDMOTION:
		if( pOwner->IsNeedProcess() )
		{
			if( dwCurTime > m_dwMotionEndTime )
			{
				if( m_CurBlinkInfo.m_fJumpAmt )
				{
					{
						pOwner->SetSKillEndJumpState( m_CurBlinkInfo.m_fJumpAmt, false, false, false, true );

						SP2Packet kPacket( CUPK_ITEM_GAUGE_CANCEL );
						FillItemGaugePacketHeader( pOwner, kPacket );
						kPacket << BS_END_JUMP;
						kPacket << pOwner->GetTargetRot();
						kPacket << pOwner->GetWorldPosition();
						kPacket << m_fCurGauge;
						kPacket << m_CurBlinkInfo.m_fJumpAmt;
						P2PNetwork::SendToAllPlayingUser( kPacket );

						//이비 예외 사황 처리( 점프후 스페셜 사용후 위에 라인에서 점프를 뛰는데 이후 다시 스페셜을 사용 못하게 하기 위해서 하드 코딩 )
						/*ioAttackableItem *pItem = pOwner->GetPriorityItem( SPT_JUMP );
						if( pItem )
						{
							ioChargeComboJump2 *pChargeComboJump2 = ToChargeComboJump2( pItem->GetCurExtendJump() );
							if( pChargeComboJump2 )
								pChargeComboJump2->SetEvySpecial( false );
						}*/

						return;
					}
				}
				else
				{	
					if( m_bAttackKeyReserved   )
						pOwner->SetNormalAttack(0);
					else if( m_bDefenseKeyReserved && pOwner->IsDefenseKeyDown() )
						ChangeDefenseState(pOwner);
					else if( m_bJumpKeyReserved )
						ChangeDashEndJump( pOwner );
					else
						ChangeToEnd( pOwner );
					return;
				}
			}
			OnBlinkEnding( pOwner );
		}
		break;
	case BS_END:
		//pOwner->SetState( CS_DELAY );
		break;
	}
}

void ioEvyItem::ChangeToBlinkStartMotion( ioBaseChar *pOwner )
{
	if( !pOwner ) return;

	DWORD dwCurTime = FRAMEGETTIME();

	ioEntityGroup *pGrp = pOwner->GetGroup();
	if( !pGrp )
		return;

	pOwner->ClearReservedSliding();
	pOwner->SetForcePowerAmt( 0.0f );
	pOwner->ClearDirDoubleClick();

	pGrp->ClearAllLoopAni( FLOAT100, true );
	pGrp->ClearAllActionAni( FLOAT100, true );

	int iAniID	     = pGrp->GetAnimationIdx( m_CurBlinkInfo.m_StartBlinkMotion );
	float fTimeRate  = m_CurBlinkInfo.m_fStartBlinkMotionRate;
	if( fTimeRate <= 0.0f )
		fTimeRate = FLOAT1;

	DWORD dwPreDelay = 0;

	pGrp->SetActionAni( iAniID, FLOAT100, FLOAT100, FLOAT1, FLOAT1/fTimeRate, 0.0f, 0.0f, true );

	m_dwMotionEndTime = dwCurTime + pGrp->GetAnimationFullTime( iAniID ) * fTimeRate;

	m_BlankState = BS_STARTMOTION;

	ClearReserveKey();

	if( pOwner->IsNeedProcess() )
	{
		ioUserKeyInput::DirKeyInput curKey = ioUserKeyInput::DKI_NONE;
		curKey = pOwner->CheckCurDirKey();

		D3DXQUATERNION qtRot;
		if( curKey != ioUserKeyInput::DKI_NONE )
		{
			float fYawD = 180.0f + 45.0f * ( curKey - ioUserKeyInput::DKI_UP );
			fYawD = ioMath::ArrangeHead( fYawD );

			D3DXQUATERNION qtRotate;
			D3DXQuaternionRotationYawPitchRoll( &qtRotate, DEGtoRAD( fYawD ), 0.0f, 0.0f );

			pOwner->SetTargetRotToRotate( qtRotate, true );	
		}
	}

	m_dwKeyReserveTime = pGrp->GetAnimationEventTime_e( iAniID, "key_reserve" ) * fTimeRate + dwPreDelay;
	if( m_dwKeyReserveTime )
		m_dwKeyReserveTime += dwCurTime;

	if( pOwner->IsNeedSendNetwork() )
	{
		SP2Packet kPacket( CUPK_ITEM_GAUGE_CANCEL );
		FillItemGaugePacketHeader( pOwner, kPacket );
		kPacket << m_BlankState;
		kPacket << pOwner->GetTargetRot();
		kPacket << pOwner->GetWorldPosition();
		kPacket << m_fCurGauge;
		P2PNetwork::SendToAllPlayingUser( kPacket );
	}
}

void ioEvyItem::ChangeToBlinkEndMotion( ioBaseChar *pOwner )
{
	if( !pOwner ) return;

	ioPlayStage *pStage = pOwner->GetCreator();
	if( !pStage ) 
		return;

	DWORD dwCurTime = FRAMEGETTIME();

	D3DXVECTOR3 vCurPos = pOwner->GetWorldPosition();
	float fTopHeight = pStage->GetMapHeight( vCurPos.x, vCurPos.z, pOwner, false, pOwner->GetMidHeightByRate( FLOAT1 ) );

	if( fTopHeight <= FLOAT1 )
	{
		pOwner->SetSKillEndJumpState( m_CurBlinkInfo.m_fJumpAmt, false, false, false, true );

		SP2Packet kPacket( CUPK_ITEM_GAUGE_CANCEL );
		FillItemGaugePacketHeader( pOwner, kPacket );
		kPacket << BS_END_JUMP;
		kPacket << pOwner->GetTargetRot();
		kPacket << pOwner->GetWorldPosition();
		kPacket << m_fCurGauge;
		kPacket << m_CurBlinkInfo.m_fJumpAmt;
		P2PNetwork::SendToAllPlayingUser( kPacket );
		return;
	}


	ioEntityGroup *pGrp = pOwner->GetGroup();
	if( !pGrp )
		return;

	pOwner->ClearReservedSliding();
	pOwner->SetForcePowerAmt( 0.0f );
	pOwner->ClearDirDoubleClick();

	pGrp->ClearAllLoopAni( FLOAT100, true );
	pGrp->ClearAllActionAni( FLOAT100, true );

	int iAniID	     = pGrp->GetAnimationIdx( m_CurBlinkInfo.m_EndBlinkMotion );
	float fTimeRate  = m_CurBlinkInfo.m_fEndBlinkMotionRate;
	if( fTimeRate <= 0.0f )
		fTimeRate = FLOAT1;

	DWORD dwPreDelay = 0;//m_DashAttackMove.m_dwPreDelay;

	pGrp->SetActionAni( iAniID, FLOAT100, FLOAT100, FLOAT1, FLOAT1/fTimeRate, 0.0f, 0.0f, true );

	m_dwMotionEndTime = dwCurTime + pGrp->GetAnimationFullTime( iAniID ) * fTimeRate;

	m_BlankState = BS_ENDMOTION;

	m_dwKeyReserveTime = pGrp->GetAnimationEventTime_e( iAniID, "key_reserve" ) * fTimeRate + dwPreDelay;
	if( m_dwKeyReserveTime )
		m_dwKeyReserveTime += dwCurTime;

	if( pOwner->IsNeedSendNetwork() )
	{
		SP2Packet kPacket( CUPK_ITEM_GAUGE_CANCEL );
		FillItemGaugePacketHeader( pOwner, kPacket );
		kPacket << m_BlankState;
		kPacket << pOwner->GetTargetRot();
		kPacket << pOwner->GetWorldPosition();
		kPacket << m_fCurGauge;
		P2PNetwork::SendToAllPlayingUser( kPacket );
	}
}

int ioEvyItem::GetMaxBullet()
{
	return (int)m_fMaxGauge;
}

int ioEvyItem::GetNeedBullet()
{
	return (int)m_fNeedGauge;
}

int ioEvyItem::GetCurBullet()
{
	float fCurCoolTime = m_fCurGauge;
	fCurCoolTime = max( 0, min( fCurCoolTime, m_fMaxGauge ) );

	return (int)fCurCoolTime;
}

void ioEvyItem::SetCurBullet( int iCurBullet )
{
	if( iCurBullet < 0 )
		return;

	float fCurBullet = iCurBullet;
	fCurBullet = max( 0, min( fCurBullet, m_fMaxGauge ) );

	m_fCurGauge = fCurBullet;
}

void ioEvyItem::InitExtraGauge()
{
	m_fCurGauge = 0.0f;
}

void ioEvyItem::MaxExtraGauge()
{
	m_fCurGauge = m_fMaxGauge;
}

float ioEvyItem::GetMaxExtraGauge()
{
	return m_fMaxGauge;
}

float ioEvyItem::GetCurExtraGauge()
{
	return m_fCurGauge;
}

void ioEvyItem::SetCurExtraGauge( float fGauge )
{
	m_fCurGauge = fGauge;
	m_fCurGauge = (float)max( 0, min( m_fCurGauge, m_fMaxGauge ) );
}

void ioEvyItem::CheckDecreaseGauge()
{
	m_fCurGauge -= m_fNeedGauge;
	m_fCurGauge = (float)max( 0, min( m_fCurGauge, m_fMaxGauge ) );
}

bool ioEvyItem::IsEnableGauge()
{
	if( m_fCurGauge >= m_fNeedGauge )
		return true;

	return false;
}

void ioEvyItem::UpdateExtraData( ioBaseChar *pOwner )
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
		if( m_fCurGauge < m_fMaxGauge )
		{
			float fGauge = m_fIncreaseGauge * fTimePerSec;

			m_fCurGauge += fGauge;
			m_fCurGauge = min( m_fCurGauge, m_fMaxGauge );
		}
		return;
	}
}

void ioEvyItem::ClearSpecialState( ioBaseChar *pOwner )
{
	m_BlankState = BS_NONE;

	// 순서중요
	//m_CurBlankInfo.Init();

	m_vFlyMoveDir = ioMath::VEC3_ZERO;
	m_fFlyMoveSpeed = 0.0f;
}

void ioEvyItem::OnEquiped( ioBaseChar *pOwner )
{
	ioWeaponItem::OnEquiped( pOwner );
}

void ioEvyItem::OnReleased( ioBaseChar *pOwner )
{
	// 순서 주의
	ClearSpecialState( pOwner );

	ioAttackableItem::OnReleased( pOwner );
}

bool ioEvyItem::IsEnableTargetState( ioBaseChar *pTarget )
{
	if( !pTarget ) return false;

	if( pTarget->IsSystemState() )
		return false;

	if( !pTarget->IsEnableTarget() )
		return false;

	return true;
}

bool ioEvyItem::IsBallTarget()
{
	return m_bAimedBall;
}

void ioEvyItem::KeyReserve( ioBaseChar *pOwner )
{
	if( pOwner->IsAttackKey() )
		m_bAttackKeyReserved = true;

	if( pOwner->IsDefenseKey() )
		m_bDefenseKeyReserved = true;

	if( pOwner->IsJumpKey() )
		m_bJumpKeyReserved = true;
}

void ioEvyItem::SetSDSet( bool bUsed )
{
	m_bSDSkillUsed = bUsed;
}

void ioEvyItem::ChangeDefenseState( ioBaseChar *pOwner )
{
	pOwner->SetDefenseState();
}

void ioEvyItem::ChangeDashEndJump( ioBaseChar *pOwner )
{
	pOwner->SetCurMoveSpeed( 0.0f );
	pOwner->SetSKillEndJumpState( m_NormalBlinkInfo.m_fSpecilAfterJumpPower, false, false, false, true );
	pOwner->ClearDirDoubleClick();

	if( pOwner->IsNeedSendNetwork() )
	{
		SP2Packet kPacket( CUPK_ITEM_GAUGE_OK );
		FillItemGaugePacketHeader( pOwner, kPacket );
		kPacket << BS_DASH_END_JUMP;
		kPacket << pOwner->GetTargetRot();
		kPacket << pOwner->GetWorldPosition();
		kPacket << m_fCurGauge;
		P2PNetwork::SendToAllPlayingUser( kPacket );
	}
}

void ioEvyItem::ClearReserveKey()
{
	m_bAttackKeyReserved = false;
	m_bDefenseKeyReserved = false;
	m_bJumpKeyReserved = false;
}

//-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

ioEvyBlinkSpecialState::ioEvyBlinkSpecialState()
{
}

ioEvyBlinkSpecialState::~ioEvyBlinkSpecialState()
{
}

void ioEvyBlinkSpecialState::Initialize( ioBaseChar* pOwner, CharState eCurrState )
{
	ioSpecialStateBase::Initialize( pOwner, eCurrState );
}

void ioEvyBlinkSpecialState::Finalize( ioBaseChar* pOwner, CharState eCurrState, CharState eNewState )
{
	ioSpecialStateBase::Finalize( pOwner, eCurrState, eNewState );

	ioEvyItem *pEvyItem = ToEvyItem( pOwner->GetPriorityItem( SPT_ATTACK ) );
	if( pEvyItem )
		pEvyItem->ClearSpecialState( pOwner );
}

void ioEvyBlinkSpecialState::CheckSpecialState( ioBaseChar* pOwner )
{
	ioEvyItem *pEvyItem = ToEvyItem( pOwner->GetPriorityItem( SPT_ATTACK ) );
	if( pEvyItem )
		pEvyItem->ProcessBlinkState( pOwner );
	else
	{
		pOwner->SetState( CS_DELAY );
		pOwner->SetFallState( true );
		return;
	}
}

void ioEvyBlinkSpecialState::CheckFallStateChange( ioBaseChar* pOwner, float fMinFallHeightRate, float fBottomHeight, bool bBottomMaxCheck, float fMaxBottomHeight )
{
	return;
}

//-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

ioEvyBombSpecialState::ioEvyBombSpecialState()
{
}

ioEvyBombSpecialState::~ioEvyBombSpecialState()
{
}

void ioEvyBombSpecialState::Initialize( ioBaseChar* pOwner, CharState eCurrState )
{
	ioSpecialStateBase::Initialize( pOwner, eCurrState );
}

void ioEvyBombSpecialState::Finalize( ioBaseChar* pOwner, CharState eCurrState, CharState eNewState )
{
	ioSpecialStateBase::Finalize( pOwner, eCurrState, eNewState );

	ioEvyItem *pEvyItem = ToEvyItem( pOwner->GetPriorityItem( SPT_ATTACK ) );
	if( pEvyItem )
		pEvyItem->ClearSpecialState( pOwner );
}

void ioEvyBombSpecialState::CheckSpecialState( ioBaseChar* pOwner )
{
	ioEvyItem *pEvyItem = ToEvyItem( pOwner->GetPriorityItem( SPT_ATTACK ) );
	if( pEvyItem )
		pEvyItem->ProcessBlinkState( pOwner );
	else
	{
		pOwner->SetState( CS_DELAY );
		pOwner->SetFallState( true );
		return;
	}
}