
#include "stdafx.h"

#include "ioJackItem.h"

#include "ioPlayStage.h"
#include "ioBaseChar.h"

#include "WeaponDefine.h"

ioJackItem::ioJackItem()
{
	Init();
}

ioJackItem::ioJackItem( const ioJackItem &rhs )
	: ioWeaponItem( rhs ),
	  m_fNeedGauge( rhs.m_fNeedGauge ),  
	  m_szPreGatherAniList( rhs.m_szPreGatherAniList ),
	  m_ChargeAttackAttribute( rhs.m_ChargeAttackAttribute )
{
	Init();
}

ioJackItem::~ioJackItem()
{
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void ioJackItem::LoadProperty( ioINILoader &rkLoader )
{
	ioWeaponItem::LoadProperty( rkLoader );

	char szBuf[MAX_PATH];

	m_fNeedGauge = rkLoader.LoadFloat_e( "need_extra_gauge", FLOAT100 );

	int nPreCount = rkLoader.LoadInt_e( "max_combo", 0 );
	for( int i=0; i<nPreCount; ++i )
	{
		wsprintf_e( szBuf, "attack_ready_animation%d", i+1 );
		rkLoader.LoadString( szBuf, "", szBuf, MAX_PATH );
		m_szPreGatherAniList.push_back( szBuf );
	}
	
	LoadAttackAttribute( "charge_attack", m_ChargeAttackAttribute, rkLoader );	
}

ioItem* ioJackItem::Clone()
{
	return new ioJackItem( *this );
}

ioWeaponItem::WeaponSubType ioJackItem::GetSubType() const
{
	return WST_JACK_ITEM;
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

const char* ioJackItem::GetAttackStateText() const
{
	switch( m_ChargeState )
	{
	case CS_NONE:
		return "CS_NONE";
	case CS_CHARGING:
		return "CS_CHARGING";
	case CS_ATTACK_FIRE:
		return "CS_ATTACK_FIRE";
	case CS_CHARGE_ATTACK:
		return "CS_CHARGE_ATTACK";
	}

	return "";
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void ioJackItem::SetNormalAttack( ioBaseChar *pOwner, int iCurCombo )
{
	ClearCancelInfo();	
	
	m_ChargeState		= CS_CHARGING;
	m_dwAttackStartTime	= FRAMEGETTIME();
	m_nCurCombo			= iCurCombo;

	pOwner->SetState( CS_ATTACK );
	pOwner->SetChargingState( true );

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

void ioJackItem::CheckNormalAttack( ioBaseChar *pOwner )
{
	DWORD dwCurTime = FRAMEGETTIME();

	switch( m_ChargeState )
	{
	case CS_CHARGING:
		OnCharging( pOwner );
		break;
	case CS_ATTACK_FIRE:
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
				pOwner->SetState( CS_DELAY, false );
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

bool ioJackItem::SetJumpAttack( ioBaseChar *pOwner, int iAniID, float fTimeRate, AttackType eType )
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

void ioJackItem::ProcessJumpAttack( ioBaseChar *pOwner, float fHeightGap )
{
	if( m_pExtendJump )
		m_pExtendJump->ProcessJumpAttack( pOwner, fHeightGap );

	CheckAirJump( pOwner );
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void ioJackItem::SetExtraInfoToWeapon( ioWeapon *pWeapon, bool bJumpAttack )
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

void ioJackItem::SetUseExtraGauge( float fAmt )
{
	m_fCurExtraGauge -= fAmt;
	m_fCurExtraGauge = max( 0.0f, m_fCurExtraGauge );
}

void ioJackItem::UpdateExtraData( ioBaseChar *pOwner )
{
	if( !pOwner ) 
		return;

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

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

bool ioJackItem::IsChargeAttackState( ioBaseChar *pOwner )
{
	if( pOwner->GetState() == CS_ATTACK )
	{
		if( m_ChargeState == CS_CHARGING )
			return true;
	}

	return false;
}

bool ioJackItem::ProcessCancel( ioBaseChar *pOwner )
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

void ioJackItem::OnItemGaugeOK( ioBaseChar *pOwner, SP2Packet &rkPacket )
{
	if( !pOwner ) 
		return;

	byte eState;
	rkPacket >> eState;

	switch( eState )
	{
	case ST_ATTACK:
		{
			D3DXQUATERNION qtRot;
			rkPacket >> m_nCurCombo;
			rkPacket >> qtRot;

			pOwner->SetTargetRot( qtRot );
			ChangeToAttackFire( pOwner );
		}
		break;
	case ST_CHARGE_ATTACK:
		{
			D3DXQUATERNION qtRot;
			rkPacket >> qtRot;

			pOwner->SetTargetRot( qtRot );
			ChangeToChargeAttack( pOwner );
		}
		break;
	}	
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

bool ioJackItem::IsConditionalSpecialDashReserve( ioBaseChar *pOwner )
{
	return true;
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

int ioJackItem::GetMaxBullet()
{
	return (int)m_fMaxExtraGauge;
}

int ioJackItem::GetNeedBullet()
{
	return (int)m_fNeedGauge;
}

int ioJackItem::GetCurBullet()
{
	float fCurCoolTime = m_fCurExtraGauge;
	fCurCoolTime = max( 0, min( fCurCoolTime, m_fMaxExtraGauge ) );

	return (int)fCurCoolTime;
}

void ioJackItem::SetCurBullet( int iCurBullet )
{
	if( iCurBullet < 0 )
		return;

	float fCurBullet = iCurBullet;
	fCurBullet = max( 0, min( fCurBullet, m_fMaxExtraGauge ) );

	m_fCurExtraGauge = fCurBullet;
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void ioJackItem::Init()
{
	m_ChargeState				= CS_NONE;

	m_dwAttackStartTime			= 0;
	m_nCurCombo					= 0;
	m_iCurAirJump				= 0;

	m_fAirJumpPowerRate			= 0.0f;
	m_fCurExtraGauge = 0.0f;
	m_dwMotionEndTime = 0;
}

void ioJackItem::OnCharging( ioBaseChar *pOwner )
{
	if( !pOwner || !pOwner->IsNeedProcess() )	
		return;

	if( m_fCurExtraGauge >= m_fNeedGauge && pOwner->IsAttackKeyDown() )
	{
		if( m_dwAttackStartTime + m_dwChargeTime <= FRAMEGETTIME() )
			ChangeToChargeAttack( pOwner );
	}
	else
		ChangeToAttackFire( pOwner );
}

void ioJackItem::ChangeToAttackFire( ioBaseChar *pOwner )
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
	m_ChargeState = CS_ATTACK_FIRE;

	if( pOwner->IsNeedSendNetwork() )
	{
		SP2Packet kPacket( CUPK_ITEM_GAUGE_OK );
		FillItemGaugePacketHeader( pOwner, kPacket );
		kPacket << static_cast<byte>( ST_ATTACK );
		kPacket << m_nCurCombo;
		kPacket << pOwner->GetTargetRot();
		P2PNetwork::SendToAllPlayingUser( kPacket );
	}
}

void ioJackItem::ChangeToChargeAttack( ioBaseChar *pOwner )
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

	if( pOwner->IsNeedSendNetwork() )
	{
		SP2Packet kPacket( CUPK_ITEM_GAUGE_OK );
		FillItemGaugePacketHeader( pOwner, kPacket );
		kPacket << static_cast<byte>( ST_CHARGE_ATTACK );
		kPacket << pOwner->GetTargetRot();
		P2PNetwork::SendToAllPlayingUser( kPacket );
	}
}