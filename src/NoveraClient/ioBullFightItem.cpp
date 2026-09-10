
#include "stdafx.h"

#include "ioBullFightItem.h"

#include "ioPlayStage.h"
#include "ioBaseChar.h"

#include "WeaponDefine.h"

ioBullFightItem::ioBullFightItem()
{
	Init();
}

ioBullFightItem::ioBullFightItem( const ioBullFightItem &rhs )
	: ioWeaponItem( rhs ),
	  m_fNeedGauge( rhs.m_fNeedGauge ),  
	  m_szPreGatherAniList( rhs.m_szPreGatherAniList ),
	  m_ChargeAttackAttribute( rhs.m_ChargeAttackAttribute ),
	  m_szExtendDelayAni( rhs.m_szExtendDelayAni ),
	  m_fExtendDelayAniRate( rhs.m_fExtendDelayAniRate ),
	  m_szExtendMoveAni( rhs.m_szExtendMoveAni ),
	  m_fExtendMoveAniRate( rhs.m_fExtendMoveAniRate ),
	  m_fExtendMoveSpeedRate( rhs.m_fExtendMoveSpeedRate ),
	  m_dwExtendMoveLoopTime( rhs.m_dwExtendMoveLoopTime ),
	  m_ExtendAttackAttribute( rhs.m_ExtendAttackAttribute ),
	  m_szExtendBuffList( rhs.m_szExtendBuffList )
{
	Init();
}

ioBullFightItem::~ioBullFightItem()
{
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void ioBullFightItem::LoadProperty( ioINILoader &rkLoader )
{
	ioWeaponItem::LoadProperty( rkLoader );

	char szBuf[MAX_PATH], szKey[MAX_PATH];

	m_fNeedGauge = rkLoader.LoadFloat_e( "need_extra_gauge", FLOAT100 );

	int nPreCount = rkLoader.LoadInt_e( "max_combo", 0 );
	for( int i=0; i<nPreCount; ++i )
	{
		wsprintf_e( szBuf, "attack_ready_animation%d", i+1 );
		rkLoader.LoadString( szBuf, "", szBuf, MAX_PATH );
		m_szPreGatherAniList.push_back( szBuf );
	}
	
	LoadAttackAttribute( "charge_attack", m_ChargeAttackAttribute, rkLoader );	

	rkLoader.LoadString_e( "extend_delay_ani", "", szBuf, MAX_PATH );
	m_szExtendDelayAni = szBuf;	
	m_fExtendDelayAniRate = rkLoader.LoadFloat_e( "extend_delay_ani_rate", FLOAT1 );

	rkLoader.LoadString_e( "extend_move_ani", "", szBuf, MAX_PATH );
	m_szExtendMoveAni = szBuf;	
	m_fExtendMoveAniRate = rkLoader.LoadFloat_e( "extend_move_ani_rate", FLOAT1 );

	m_fExtendMoveSpeedRate	= rkLoader.LoadFloat_e( "extend_move_speed_rate", 0.0f );

	m_dwExtendMoveLoopTime = rkLoader.LoadInt_e( "extend_move_time", 0 );
	
	LoadAttackAttribute( "extend_attack", m_ExtendAttackAttribute, rkLoader );	

	m_szExtendBuffList.clear();

	int nBuffCnt = rkLoader.LoadInt_e( "extend_buff_cnt", 0 );
	for( int i=0; i<nBuffCnt; i++ )
	{
		wsprintf_e( szKey, "extend_buff%d", i+1 );
		rkLoader.LoadString( szKey, "", szBuf, MAX_PATH );
		m_szExtendBuffList.push_back( szBuf );
	}
}

ioItem* ioBullFightItem::Clone()
{
	return new ioBullFightItem( *this );
}

ioWeaponItem::WeaponSubType ioBullFightItem::GetSubType() const
{
	return WST_BULLFIGHT_ITEM;
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

const char* ioBullFightItem::GetAttackStateText() const
{
	switch( m_ChargeState )
	{
	case CS_NONE:
		return "CS_NONE";
	case CS_CHARGING:
		return "CS_CHARGING";
	case CS_NORMAL_ATTACK:
		return "CS_NORMAL_ATTACK";
	case CS_CHARGE_ATTACK:
		return "CS_CHARGE_ATTACK";
	case CS_EXTEND_MOVE:
		return "CS_EXTEND_MOVE";
	case CS_EXTEND_ATTACK:
		return "CS_EXTEND_ATTACK";
	}

	return "";
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void ioBullFightItem::SetNormalAttack( ioBaseChar *pOwner, int iCurCombo )
{
	ClearCancelInfo();	
	
	m_ChargeState		= CS_CHARGING;
	m_dwAttackStartTime	= FRAMEGETTIME();
	m_nCurCombo			= iCurCombo;
	m_bSetDelay	= false;

	pOwner->SetState( CS_ATTACK );
	pOwner->SetChargingState( true );
	pOwner->SetAttackMoveEnable( false );

	ioEntityGroup *pGrp = pOwner->GetGroup();
	pGrp->ClearAllActionAni( FLOAT100, true );
	pGrp->ClearAllLoopAni( FLOAT100, true );

	int iPreGatherCnt = m_szPreGatherAniList.size();

	ioHashString szPreAni;
	if( 0 < iPreGatherCnt && COMPARE( iCurCombo, 0, iPreGatherCnt ) )
	{
		szPreAni = m_szPreGatherAniList[iCurCombo];
		
		int iAniID = pGrp->GetAnimationIdx( szPreAni );
		float fTimeRate = (float)m_dwChargeTime / pGrp->GetAnimationFullTime( iAniID );
		pGrp->SetActionAni( iAniID, FLOAT100, FLOAT100, FLOAT1, FLOAT1/fTimeRate, 0.0f, 0.0f, true );
	}
}

void ioBullFightItem::CheckNormalAttack( ioBaseChar *pOwner )
{
	DWORD dwCurTime = FRAMEGETTIME();

	switch( m_ChargeState )
	{
	case CS_CHARGING:
		OnCharging( pOwner );
		break;
	case CS_NORMAL_ATTACK:
		{
			if( pOwner->IsNeedProcess() )
			{
				CheckCancelReserve( pOwner );
				if( ProcessCancel(pOwner) )
					return;				
			}

			ioWeaponItem::CheckNormalAttack( pOwner );
		}
		break;
	case CS_CHARGE_ATTACK:
		{
			if( m_dwMotionEndTime < dwCurTime )
			{
				ChangeToExtendMove( pOwner );
				return;
			}

			if( pOwner->IsNeedProcess() )
			{
				CheckCancelReserve( pOwner );
				if( ProcessCancel(pOwner) )
					return;				
			}
		}
		break;
	case CS_EXTEND_MOVE:
		{
			if( m_dwExtendMoveStartTime + m_dwExtendMoveLoopTime < dwCurTime )
			{
				CheckReserveState( pOwner );
				return;
			}

			CheckExtendMoveAniState( pOwner, false );

			if ( pOwner->IsNeedProcess() && pOwner->IsAttackKey() )
				ChangeToExtendAttack( pOwner );
		}
		break;
	case CS_EXTEND_ATTACK:
		{
			if( m_dwMotionEndTime < dwCurTime )
			{
				CheckReserveState( pOwner );
				return;
			}

			if ( m_dwKeyReserveTime < dwCurTime )
			{
				ReserveInputKey( pOwner, true, true, false );
				return;
			}

			if( pOwner->IsNeedProcess() )
			{
				CheckCancelReserve( pOwner );
				if( ProcessCancel(pOwner) )
					return;				
			}
		}
		break;
	}

}

bool ioBullFightItem::SetJumpAttack( ioBaseChar *pOwner, int iAniID, float fTimeRate, AttackType eType )
{
	if( !pOwner ) return false;

	DWORD dwCurTime = FRAMEGETTIME();
	ClearAirJump();

	AttackType eExtendType = AT_JUMP_ATTACK;

	const AttackAttribute *pAttr = GetAttackAttribute( eExtendType );
	if( !pAttr )
	{
		LOG.PrintTimeAndLog( 0, "%s - AttackAttribute Not Exist", __FUNCTION__ );
		return false;
	}

	ioEntityGroup *pGrp = pOwner->GetGroup();
	iAniID = pGrp->GetAnimationIdx( pAttr->m_AttackAnimation );
	if( iAniID == -1 )
	{
		LOG.PrintTimeAndLog( 0, "%s - JumpAttack Animation Not Exist", __FUNCTION__ );
		return false;
	}

	fTimeRate   = pAttr->m_fAttackAniRate;
	DWORD dwPreDelay  = pAttr->m_dwPreDelay;

	// For AirJumpPower
	SetAirJump( pOwner, iAniID, fTimeRate );

	return false;
}

void ioBullFightItem::ProcessJumpAttack( ioBaseChar *pOwner, float fHeightGap )
{
	if( m_pExtendJump )
		m_pExtendJump->ProcessJumpAttack( pOwner, fHeightGap );

	CheckAirJump( pOwner );
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void ioBullFightItem::SetExtraInfoToWeapon( ioWeapon *pWeapon, bool bJumpAttack )
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
			pZone->SetZoneValue( true, false );
		}
	}
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void ioBullFightItem::SetUseExtraGauge( float fAmt )
{
	m_fCurExtraGauge -= fAmt;
	m_fCurExtraGauge = max( 0.0f, m_fCurExtraGauge );
}

void ioBullFightItem::UpdateExtraData( ioBaseChar *pOwner )
{
	if( !pOwner ) 
		return;

	float fTimePerSec = g_FrameTimer.GetSecPerFrame();

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

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

bool ioBullFightItem::IsChargeAttackState( ioBaseChar *pOwner )
{
	if( pOwner->GetState() == CS_ATTACK )
	{
		if( m_ChargeState == CS_CHARGING )
			return true;
	}

	return false;
}

bool ioBullFightItem::ProcessCancel( ioBaseChar *pOwner )
{
	if( m_szCurAni.IsEmpty() )
		return false;

	if( !pOwner ) 
		return false;

	ioEntityGroup *pGrp = pOwner->GetGroup();
	if( !pGrp ) 
		return false;

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
			m_dwCancelCheckTime = dwTime + 1;

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

void ioBullFightItem::OnItemGaugeOK( ioBaseChar *pOwner, SP2Packet &rkPacket )
{
	if( !pOwner ) 
		return;

	byte eState;
	rkPacket >> eState;

	switch( eState )
	{
	case ST_NORMAL_ATTACK:
		{
			D3DXQUATERNION qtRot;
			rkPacket >> m_nCurCombo;
			rkPacket >> qtRot;
			pOwner->SetTargetRot( qtRot );

			D3DXVECTOR3 vPos;
			rkPacket >> vPos;
			pOwner->SetWorldPosition( vPos );

			ChangeToNormalAttack( pOwner );
		}
		break;
	case ST_CHARGE_ATTACK:
		{
			D3DXQUATERNION qtRot;
			rkPacket >> qtRot;
			pOwner->SetTargetRot( qtRot );

			D3DXVECTOR3 vPos;
			rkPacket >> vPos;
			pOwner->SetWorldPosition( vPos );
			
			ChangeToChargeAttack( pOwner );

			pOwner->ApplyAutoTargetInfo( rkPacket );
		}
		break;
	case ST_EXTEND_ATTACK:
		{
			D3DXQUATERNION qtRot;
			rkPacket >> qtRot;
			pOwner->SetTargetRot( qtRot );

			D3DXVECTOR3 vPos;
			rkPacket >> vPos;
			pOwner->SetWorldPosition( vPos );

			ChangeToExtendAttack( pOwner );

			pOwner->ApplyAutoTargetInfo( rkPacket );
		}
		break;
	case ST_END:
		{
			D3DXQUATERNION qtRot;
			rkPacket >> qtRot;
			pOwner->SetTargetRot( qtRot );

			D3DXVECTOR3 vPos;
			rkPacket >> vPos;
			pOwner->SetWorldPosition( vPos );

			pOwner->SetState( CS_DELAY, false );
		}
		break;
	}	
}

void ioBullFightItem::OnEndExtendAttack( ioBaseChar *pOwner, bool bReleaseItem )
{
	if ( !pOwner )
		return;

	ExtendRemoveBuff( pOwner );
}

float ioBullFightItem::GetAttackSpeedRate() const
{
	if( m_ChargeState == CS_EXTEND_MOVE )
		return m_fExtendMoveSpeedRate;

	return FLOAT1;
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

int ioBullFightItem::GetMaxBullet()
{
	return (int)m_fMaxExtraGauge;
}

int ioBullFightItem::GetNeedBullet()
{
	return (int)m_fNeedGauge;
}

int ioBullFightItem::GetCurBullet()
{
	float fCurCoolTime = m_fCurExtraGauge;
	fCurCoolTime = max( 0, min( fCurCoolTime, m_fMaxExtraGauge ) );

	return (int)fCurCoolTime;
}

void ioBullFightItem::SetCurBullet( int iCurBullet )
{
	if( iCurBullet < 0 )
		return;

	float fCurBullet = iCurBullet;
	fCurBullet = max( 0, min( fCurBullet, m_fMaxExtraGauge ) );

	m_fCurExtraGauge = fCurBullet;
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void ioBullFightItem::Init()
{
	m_ChargeState				= CS_NONE;

	m_dwAttackStartTime			= 0;
	m_nCurCombo					= 0;
	m_iCurAirJump				= 0;

	m_fAirJumpPowerRate	= 0.0f;
	m_fCurExtraGauge = 0.0f;
	m_dwMotionEndTime = 0;
	m_dwKeyReserveTime = 0;

	m_bSetDelay	= false;
	m_dwExtendMoveStartTime = 0;

	m_bAttackKeyReserved = false;
	m_bDefenseKeyReserved = false;
	m_bJumpKeyReserved = false;
}

void ioBullFightItem::OnCharging( ioBaseChar *pOwner )
{
	if( !pOwner || !pOwner->IsNeedProcess() )	
		return;

	if( m_fCurExtraGauge >= m_fNeedGauge && pOwner->IsAttackKeyDown() && !pOwner->IsHasCrown() )
	{
		if( m_dwAttackStartTime + m_dwChargeTime <= FRAMEGETTIME() )
			ChangeToChargeAttack( pOwner );
	}
	else
		ChangeToNormalAttack( pOwner );
}

void ioBullFightItem::ChangeToNormalAttack( ioBaseChar *pOwner )
{
	if( !pOwner ) 
		return;

	DWORD dwCurTime = FRAMEGETTIME();

	pOwner->InitExtendAttackTagInfo();

	ClearCancelInfo();
	
	const AttackAttribute *pAttribute = m_FormalAttack.GetAttribute( AT_NORMAL, m_nCurCombo );
	if( pAttribute )
	{
		pOwner->SetNormalAttackByAttribute( *pAttribute );

		m_szCurAni = pAttribute->m_AttackAnimation;
		m_fCurAniRate = pAttribute->m_fAttackAniRate;

		m_dwInputCancelCheckTime = dwCurTime;
		m_dwCancelCheckTime = dwCurTime;
	}

	pOwner->ApplyNormalAttackResult( m_nCurCombo );
	m_ChargeState = CS_NORMAL_ATTACK;
	pOwner->SetAttackMoveEnable( false );

	if( pOwner->IsNeedSendNetwork() )
	{
		SP2Packet kPacket( CUPK_ITEM_GAUGE_OK );
		FillItemGaugePacketHeader( pOwner, kPacket );
		kPacket << static_cast<byte>( ST_NORMAL_ATTACK );
		kPacket << m_nCurCombo;
		kPacket << pOwner->GetTargetRot();
		kPacket << pOwner->GetWorldPosition();
		P2PNetwork::SendToAllPlayingUser( kPacket );
	}
}

void ioBullFightItem::ChangeToChargeAttack( ioBaseChar *pOwner )
{
	if( !pOwner ) 
		return;
	
	ioEntityGroup *pGrp = pOwner->GetGroup();
	int iAniID = pGrp->GetAnimationIdx( m_ChargeAttackAttribute.m_AttackAnimation );
	if ( iAniID == -1 )
		return;

	float fTimeRate  = m_ChargeAttackAttribute.m_fAttackAniRate;
	DWORD dwPreDelay = m_ChargeAttackAttribute.m_dwPreDelay;
	
	pGrp->ClearAllLoopAni( FLOAT100, true );
	pGrp->ClearAllActionAni( FLOAT100, true );
	pGrp->SetActionAni( iAniID, FLOAT100, FLOAT100, FLOAT1, FLOAT1/fTimeRate, 0.0f, dwPreDelay );	

	pOwner->ClearReservedSliding();
	pOwner->SetReservedSliding( m_ChargeAttackAttribute.m_vForceInfoList, iAniID, fTimeRate, dwPreDelay );

	pOwner->RefreshFireTimeList( iAniID, m_ChargeAttackAttribute.m_vWeaponInfoList, FTT_NORMAL_ATTACK, fTimeRate, dwPreDelay );

	DWORD dwCurTime = FRAMEGETTIME();
	m_dwMotionEndTime = dwCurTime + pGrp->GetAnimationFullTime( iAniID ) * fTimeRate;

	m_ChargeState = CS_CHARGE_ATTACK;

	SetUseExtraGauge( m_fNeedGauge );
	pOwner->SetAttackMoveEnable( false );

	pOwner->SetAutoTarget( ATT_NORMAL );

	DWORD dwTrackingTime = dwCurTime + pGrp->GetAnimationEventTime_e( iAniID, "tracking_e" ) * fTimeRate + dwPreDelay;
	pOwner->SetAutoTargetTrackingEndTime( dwTrackingTime );

	if( pOwner->IsNeedSendNetwork() )
	{
		SP2Packet kPacket( CUPK_ITEM_GAUGE_OK );
		FillItemGaugePacketHeader( pOwner, kPacket );
		kPacket << static_cast<byte>( ST_CHARGE_ATTACK );
		kPacket << pOwner->GetTargetRot();
		kPacket << pOwner->GetWorldPosition();
		pOwner->FillAutoTargetInfo( kPacket );
		P2PNetwork::SendToAllPlayingUser( kPacket );
	}
}

void ioBullFightItem::ChangeToExtendMove( ioBaseChar *pOwner )
{
	if( !pOwner ) 
		return;
	
	m_dwExtendMoveStartTime = FRAMEGETTIME();
	pOwner->SetAttackMoveEnable( true );
	pOwner->ChangeDirectionExplicitIfDirChanged( true );

	m_ChargeState = CS_EXTEND_MOVE;
	m_bSetDelay	= false;

	m_bAttackKeyReserved = false;
	m_bDefenseKeyReserved = false;
	m_bJumpKeyReserved = false;
	
	CheckExtendMoveAniState( pOwner, true );
	ExtendAddBuff( pOwner );
}

void ioBullFightItem::CheckExtendMoveAniState( ioBaseChar *pOwner, bool bFirst )
{
	if( m_ChargeState != CS_EXTEND_MOVE )
		return;

	ioEntityGroup *pGrp = pOwner->GetGroup();
	int iAniID = 0;

	if( pOwner->IsSettedDirection() )
	{
		bool bChangeDir = pOwner->IsChangeDirection();
		if( bChangeDir || bFirst )
		{
			pGrp->ClearAllLoopAni( FLOAT100, true );
			pGrp->ClearAllActionAni( FLOAT100, true );			
			pGrp->SetLoopAni( m_szExtendMoveAni, FLOAT100, FLOAT1, FLOAT1/m_fExtendMoveAniRate );

			m_bSetDelay = false;

			g_TutorialMgr.ActionAimingMove();
		}
	}
	else
	{
		if( !m_bSetDelay )
		{
			pGrp->ClearAllLoopAni( FLOAT100, true );
			pGrp->ClearAllActionAni( FLOAT100, true );
			pGrp->SetLoopAni( m_szExtendDelayAni, FLOAT100, FLOAT1, FLOAT1/m_fExtendDelayAniRate );
			m_bSetDelay = true;
		}
	}
}

void ioBullFightItem::ChangeToExtendAttack( ioBaseChar *pOwner )
{
	if( !pOwner ) 
		return;

	ioEntityGroup *pGrp = pOwner->GetGroup();
	int iAniID = pGrp->GetAnimationIdx( m_ExtendAttackAttribute.m_AttackAnimation );
	if ( iAniID == -1 )
		return;

	float fTimeRate  = m_ExtendAttackAttribute.m_fAttackAniRate;
	DWORD dwPreDelay = m_ExtendAttackAttribute.m_dwPreDelay;

	pGrp->ClearAllLoopAni( FLOAT100, true );
	pGrp->ClearAllActionAni( FLOAT100, true );
	pGrp->SetActionAni( iAniID, FLOAT100, FLOAT100, FLOAT1, FLOAT1/fTimeRate, 0.0f, dwPreDelay );	

	pOwner->ClearReservedSliding();
	pOwner->SetReservedSliding( m_ExtendAttackAttribute.m_vForceInfoList, iAniID, fTimeRate, dwPreDelay );

	pOwner->RefreshFireTimeList( iAniID, m_ExtendAttackAttribute.m_vWeaponInfoList, FTT_NORMAL_ATTACK, fTimeRate, dwPreDelay );

	DWORD dwCurTime = FRAMEGETTIME();
	m_dwMotionEndTime = dwCurTime + pGrp->GetAnimationFullTime( iAniID ) * fTimeRate;
	m_dwKeyReserveTime = dwCurTime + pGrp->GetAnimationEventTime_e( iAniID, "key_reserve" ) * fTimeRate;

	m_ChargeState = CS_EXTEND_ATTACK;
	pOwner->SetAttackMoveEnable( false );

	pOwner->SetAutoTarget( ATT_NORMAL );

	DWORD dwTrackingTime = dwCurTime + pGrp->GetAnimationEventTime_e( iAniID, "tracking_e" ) * fTimeRate + dwPreDelay;
	pOwner->SetAutoTargetTrackingEndTime( dwTrackingTime );

	m_bAttackKeyReserved = false;
	m_bDefenseKeyReserved = false;
	m_bJumpKeyReserved = false;

	if( pOwner->IsNeedSendNetwork() )
	{
		SP2Packet kPacket( CUPK_ITEM_GAUGE_OK );
		FillItemGaugePacketHeader( pOwner, kPacket );
		kPacket << static_cast<byte>( ST_EXTEND_ATTACK );
		kPacket << pOwner->GetTargetRot();
		kPacket << pOwner->GetWorldPosition();
		pOwner->FillAutoTargetInfo( kPacket );
		P2PNetwork::SendToAllPlayingUser( kPacket );
	}
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void ioBullFightItem::ReserveInputKey( ioBaseChar *pOwner, bool bAttack, bool bDefence, bool bJump )
{
	if ( !pOwner || !pOwner->IsNeedProcess() )
		return;

	if( bAttack && pOwner->IsAttackKey() )
		m_bAttackKeyReserved = true;

	if( bDefence && pOwner->IsDefenseKey() )
		m_bDefenseKeyReserved = true;

	if( bJump && pOwner->IsJumpKey() )
		m_bJumpKeyReserved = true;
}

void ioBullFightItem::CheckReserveState( ioBaseChar *pOwner )
{
	if ( !pOwner || !pOwner->IsNeedProcess() )
		return;

	if( m_bAttackKeyReserved )
	{
		SetReserveAttack( pOwner );
		return;
	}
	else if( m_bDefenseKeyReserved && pOwner->IsDefenseKeyDown() )
	{
		SetReserveDefence( pOwner );
		return;
	}
	else if( m_bJumpKeyReserved )
	{
		SetReserveJump( pOwner );
		return;
	}

	if ( pOwner->IsNeedSendNetwork() )
	{
		SP2Packet kPacket( CUPK_ITEM_GAUGE_OK );
		FillItemGaugePacketHeader( pOwner, kPacket );
		kPacket << static_cast<byte>( ST_END );
		kPacket << pOwner->GetTargetRot();
		kPacket << pOwner->GetWorldPosition();
		P2PNetwork::SendToAllPlayingUser( kPacket );
	}

	pOwner->SetState( CS_DELAY, false );
}

void ioBullFightItem::SetReserveAttack( ioBaseChar *pOwner )
{
	if ( !pOwner || !pOwner->IsNeedProcess() )
		return;

	pOwner->SetNormalAttack(0);
	m_bAttackKeyReserved = false;
}

void ioBullFightItem::SetReserveDefence( ioBaseChar *pOwner )
{
	if ( !pOwner || !pOwner->IsNeedProcess() )
		return;

	pOwner->SetDefenseState();
	m_bDefenseKeyReserved = false;
}

void ioBullFightItem::SetReserveJump( ioBaseChar *pOwner )
{
	if ( !pOwner || !pOwner->IsNeedProcess() )
		return;

	pOwner->SetJumpState();
	m_bJumpKeyReserved = false;	
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void ioBullFightItem::ExtendAddBuff( ioBaseChar *pOwner )
{
	if ( !pOwner )
		return;

	for ( int i=0; i<(int)m_szExtendBuffList.size(); i++ )
		pOwner->AddNewBuff( m_szExtendBuffList[i], pOwner->GetCharName(), "", NULL );
}

void ioBullFightItem::ExtendRemoveBuff( ioBaseChar *pOwner )
{
	if ( !pOwner )
		return;

	for ( int i=0; i<(int)m_szExtendBuffList.size(); i++ )
	{
		ioBuff *pBuff = pOwner->GetBuff( m_szExtendBuffList[i] );
		if ( pBuff )
			pBuff->SetReserveEndBuff();
	}
}