#include "stdafx.h"

#include "ioGanesa.h"

#include "ioPlayStage.h"
#include "ioBaseChar.h"

#include "WeaponDefine.h"

ioGanesa::ioGanesa()
{
	Init();
}

ioGanesa::ioGanesa( const ioGanesa &rhs )
	: ioChargeCommandItem( rhs ),
	m_vExtendAttributeList( rhs.m_vExtendAttributeList )
{
	Init();
}

ioGanesa::~ioGanesa()
{
}

void ioGanesa::Init()
{	
	m_AttackState			= AS_NONE;

	m_iCurCombo				= 0;
	m_iCurAirJump			= 0;
	
	m_dwAttackStartTime		= 0;
	m_dwKeyReserveTime		= 0;
	
	m_fAirJumpPowerRate		= 0.0f;

	m_iCurExtendCombo		= 0;
}

void ioGanesa::LoadProperty( ioINILoader &rkLoader )
{
	ioChargeCommandItem::LoadProperty( rkLoader );

	char szBuf[MAX_PATH];
	char szKey[MAX_PATH];

	//PRE
	int iPreCount = rkLoader.LoadInt_e( "pre_animation_cnt", 0 );
	for( int i = 0; i < iPreCount; ++i )
	{
		wsprintf_e( szKey, "pre_animation%d", i+1 );
		rkLoader.LoadString( szKey, "", szBuf, MAX_PATH );
		m_PreAnimationList.push_back( ioHashString( szBuf ) );
	}

	//EXTEND
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
}

ioItem* ioGanesa::Clone()
{
	return new ioGanesa( *this );
}

ioWeaponItem::WeaponSubType ioGanesa::GetSubType() const
{
	return WST_GANESA;
}

void ioGanesa::OnEquiped( ioBaseChar *pOwner )
{
	ioAttackableItem::OnEquiped( pOwner );	
}

void ioGanesa::OnReleased( ioBaseChar *pOwner )
{
	ioAttackableItem::OnReleased( pOwner );
}

// 노말 공격 ----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

void ioGanesa::SetNormalAttack( ioBaseChar *pOwner, int iCurCombo )
{
	if( !pOwner )
		return;

	ClearCancelInfo();

	int iPreGatherCnt = m_PreAnimationList.size();

	//특별 공격후 노말타는 무조건 첫번째 부터 나가도록함
	if( pOwner->GetState() == CS_ATTACK && m_AttackState == AS_EXTEND_FIRE )
		m_iCurCombo = 0;
	else
		m_iCurCombo = iCurCombo;
		
	m_AttackState		= AS_CHARGING;
	m_dwAttackStartTime	= FRAMEGETTIME();

	pOwner->SetState( CS_ATTACK );
	pOwner->SetChargingState( false );
	pOwner->SetAttackMoveEnable( false );

	ioEntityGroup *pGrp = pOwner->GetGroup();
	pGrp->ClearAllActionAni( FLOAT100, true );
	pGrp->ClearAllLoopAni( FLOAT100, true );

	if( 0 < iPreGatherCnt && COMPARE( iCurCombo, 0, iPreGatherCnt ) )
	{
		int iAniID = pGrp->GetAnimationIdx( m_PreAnimationList[iCurCombo] );
		float fTimeRate = (float)m_dwChargeTime / pGrp->GetAnimationFullTime( iAniID );
		pGrp->SetActionAni( iAniID, FLOAT100, FLOAT100, FLOAT1, FLOAT1/fTimeRate, 0.0f, 0.0f, true );
	}
}

void ioGanesa::CheckNormalAttack( ioBaseChar *pOwner )
{
	DWORD dwCurTime = FRAMEGETTIME();

	switch( m_AttackState )
	{
	case AS_CHARGING:
		OnCharging( pOwner );
		break;
	case AS_ATTACK_FIRE:
		OnNormalAttack( pOwner );
		break;
	case AS_EXTEND_FIRE:	
		OnExtendAttack( pOwner );
		break;

	}
}

// D꾹 ---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

void ioGanesa::OnCharging( ioBaseChar *pOwner )
{
	if( !pOwner || !pOwner->IsNeedProcess() )
		return;

	if( pOwner->IsAttackKeyDown() )
	{
		if( m_dwAttackStartTime + m_dwChargeTime <= FRAMEGETTIME() )
		{
			if( COMPARE( m_iCurExtendCombo, 0, (int)m_vExtendAttributeList.size() ) )
				ChangeToExtendAttackFire( pOwner );
			else
				ChangeToAttackFire( pOwner );
		}
	}
	else
	{
		ChangeToAttackFire( pOwner );
	}
}

// 노말 공격 -----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

void ioGanesa::ChangeToAttackFire( ioBaseChar *pOwner )
{
	if( !pOwner )
		return;

	DWORD dwCurTime = FRAMEGETTIME();
	pOwner->InitExtendAttackTagInfo();
	ClearCancelInfo();

	m_iCurExtendCombo = 0;

	const AttackAttribute *pAttribute = m_FormalAttack.GetAttribute( AT_NORMAL, m_iCurCombo );
	if( pAttribute )
	{
		pOwner->SetNormalAttackByAttribute( *pAttribute );

		m_szCurAni					= pAttribute->m_AttackAnimation;
		m_fCurAniRate				= pAttribute->m_fAttackAniRate;
		m_dwInputCancelCheckTime	= dwCurTime;
		m_dwCancelCheckTime			= dwCurTime;
	}

	pOwner->ApplyNormalAttackResult( m_iCurCombo );
	m_AttackState = AS_ATTACK_FIRE;

	if( pOwner->IsNeedSendNetwork() )
	{
		SP2Packet kPacket( CUPK_ITEM_GAUGE_CANCEL );
		FillItemGaugePacketHeader( pOwner, kPacket );
		kPacket << static_cast<byte>( AS_ATTACK_FIRE );
		kPacket << pOwner->GetTargetRot();
		kPacket << m_iCurCombo;
		P2PNetwork::SendToAllPlayingUser( kPacket );
	}
}

void ioGanesa::OnNormalAttack( ioBaseChar *pOwner )
{
	if( pOwner->IsNeedProcess() )
	{
		CheckCancelReserve( pOwner );
		if( ProcessCancel(pOwner) )
			return;

		if( CheckCmdInputState( pOwner ) )
			return;

		if( CheckSkillInputState( pOwner ) )
			return;
	}

	ioWeaponItem::CheckNormalAttack( pOwner );
}

// 특별 ---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

void ioGanesa::ChangeToExtendAttackFire( ioBaseChar *pOwner )
{
	if( !pOwner )
		return;

	ioEntityGroup *pGrp = pOwner->GetGroup();
	if( !pGrp )
		return;
	
	DWORD dwCurTime = FRAMEGETTIME();
	pOwner->InitExtendAttackTagInfo();
	ClearCancelInfo();
	m_AttackState = AS_EXTEND_FIRE;

	pOwner->SetNormalAttackByAttribute( m_vExtendAttributeList[m_iCurExtendCombo] );
	m_szCurAni					= m_vExtendAttributeList[m_iCurExtendCombo].m_AttackAnimation;
	m_fCurAniRate				= m_vExtendAttributeList[m_iCurExtendCombo].m_fAttackAniRate;
	m_dwInputCancelCheckTime	= dwCurTime;
	m_dwCancelCheckTime			= dwCurTime;
	pOwner->ApplyNormalAttackResult( m_iCurExtendCombo );

	float fTimeRate		= m_vExtendAttributeList[m_iCurExtendCombo].m_fAttackAniRate;
	DWORD dwPreDelay	= m_vExtendAttributeList[m_iCurExtendCombo].m_dwPreDelay;
	int iAniID			= pGrp->GetAnimationIdx( m_vExtendAttributeList[m_iCurExtendCombo].m_AttackAnimation );

	pOwner->SetBaseAttackExtraAniJump( iAniID, fTimeRate, dwPreDelay );
	pOwner->SetExtendAttackEndJumpInfo( iAniID, fTimeRate, dwPreDelay );

	if( pOwner->IsNeedSendNetwork() )
	{
		SP2Packet kPacket( CUPK_ITEM_GAUGE_CANCEL );
		FillItemGaugePacketHeader( pOwner, kPacket );
		kPacket << static_cast<byte>( AS_EXTEND_FIRE );
		kPacket << pOwner->GetTargetRot();
		kPacket << m_iCurExtendCombo;
		P2PNetwork::SendToAllPlayingUser( kPacket );
	}
	m_iCurExtendCombo++;
}

void ioGanesa::OnExtendAttack( ioBaseChar *pOwner )
{	
	pOwner->CheckExtraAniJump();
	if( pOwner->IsNeedProcess() )
	{	
		if( pOwner->CheckExtendAttackEndJump() )
			return;		

		CheckCancelReserve( pOwner );
		if( ProcessCancel(pOwner) )
			return;

		if( CheckCmdInputState( pOwner ) )
			return;

		if( CheckSkillInputState( pOwner ) )
			return;
	}

	ioWeaponItem::CheckNormalAttack( pOwner );
}

//--------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

void ioGanesa::SetCmdAttack( ioBaseChar *pOwner, const AttackAttribute &rkAttr, float fExtraAniRate /* = FLOAT1 */, float fExtraForceRate /* = FLOAT1 */ )
{
	ioChargeCommandItem::SetCmdAttack( pOwner, rkAttr, fExtraAniRate, fExtraForceRate );	
	SetCurAttackAniForInputJump( rkAttr.m_AttackAnimation, rkAttr.m_fAttackAniRate );
}

void ioGanesa::CheckCmdState( ioBaseChar *pOwner )
{
	ioChargeCommandItem::CheckCmdState( pOwner );

	if( m_dwCmdAttackEndTime > FRAMEGETTIME() )
	{
		if( pOwner->IsNeedProcess() )
		{
			CheckCancelReserve( pOwner );
			if( ProcessCancel(pOwner) )
				return;
		}
	}
}

//--------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

bool ioGanesa::IsChargeAttackState( ioBaseChar *pOwner )
{
	if( pOwner->GetState() == CS_ATTACK )
	{
		return true; 
	}

	return false;
}

bool ioGanesa::IsEnableAttackDashCheck( ioBaseChar *pOwner, bool bCheck )
{
	if( pOwner->GetState() == CS_ATTACK )
	{
		switch( m_AttackState )
		{
		case AS_ATTACK_FIRE:
			if( !bCheck )
				return false;

			return true;
		}
	}
	return false;
}
//--------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

void ioGanesa::SetNoneState( ioBaseChar* pOwner, int eNewState )
{
	Init();
}

bool ioGanesa::CheckLanding( ioBaseChar *pChar )
{					
	if( CheckCmdInputState( pChar ) )
		return true;

	if( CheckSkillInputState( pChar ) )
		return true;

	return true;
}

//--------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

void ioGanesa::OnItemGaugeCancel( ioBaseChar *pOwner, SP2Packet &rkPacket )
{
	if( !pOwner )
		return;

	byte eState;
	rkPacket >> eState;

	D3DXQUATERNION qtRot;
	rkPacket >> qtRot;
	pOwner->SetTargetRot( qtRot );

	switch( eState )
	{
	case AS_ATTACK_FIRE:
		{	
			rkPacket >> m_iCurCombo;
			ChangeToAttackFire( pOwner );
		}
		break;
	case AS_EXTEND_FIRE:
		{
			rkPacket >> m_iCurExtendCombo;
			ChangeToExtendAttackFire( pOwner );
		}
		break;
	}	
}

void ioGanesa::UpdateExtraData( ioBaseChar *pOwner )
{
	if( !pOwner ) return;
		
	float fTimePerSec = g_FrameTimer.GetSecPerFrame();
	if( pOwner->IsSystemState() )
		return;

	switch( pOwner->GetState() )
	{
	case CS_DELAY:
	case CS_RUN:
		if( m_fCurExtraGauge < m_fMaxExtraGauge )
		{
			float fGauge = m_fIncreaseExtraGauge * fTimePerSec;

			m_fCurExtraGauge += fGauge;
			m_fCurExtraGauge = min( m_fCurExtraGauge, m_fMaxExtraGauge );
		}
		return;
	}
}

int ioGanesa::GetMaxBullet()
{
	return (int)m_fMaxExtraGauge;
}

int ioGanesa::GetCurBullet()
{
	float fCurCoolTime = m_fCurExtraGauge;
	fCurCoolTime = max( 0, min( fCurCoolTime, m_fMaxExtraGauge ) );

	return (int)fCurCoolTime;
}

void ioGanesa::SetCurBullet( int iCurBullet )
{
	if( iCurBullet < 0 )
		return;

	float fCurBullet = iCurBullet;
	fCurBullet = max( 0, min( fCurBullet, m_fMaxExtraGauge ) );

	m_fCurExtraGauge = fCurBullet;
}

void ioGanesa::SetUseExtraGauge( float fAmt )
{
	m_fCurExtraGauge -= fAmt;
	m_fCurExtraGauge = max( 0.0f, m_fCurExtraGauge );
}

void ioGanesa::SetExtraInfoToWeapon( ioWeapon *pWeapon, bool bJumpAttack )
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