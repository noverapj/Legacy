
#include "stdafx.h"

#include "ioChainMagicianItem.h"

#include "ioBaseChar.h"
#include "ioPlayStage.h"

#include "WeaponDefine.h"
#include "ItemDefine.h"

ioChainMagicianItem::ioChainMagicianItem()
{
	m_fCurExtraGauge = 0.0f;
	Init();
}

ioChainMagicianItem::ioChainMagicianItem( const ioChainMagicianItem &rhs )
	: ioWeaponItem( rhs ),
	  m_AttackReadyAniList( rhs.m_AttackReadyAniList ),
	  m_fNeedExtraGauge( rhs.m_fNeedExtraGauge ),
	  m_fSkillGaugeRate( rhs.m_fSkillGaugeRate ),
	  m_szGatherAni( rhs.m_szGatherAni ),
	  m_fGatherAniRate( rhs.m_fGatherAniRate ),
	  m_szSpecialLoopAni( rhs.m_szSpecialLoopAni ),
	  m_fSpecialLoopAniRate( rhs.m_fSpecialLoopAniRate ),
	  m_ReadyCircle( rhs.m_ReadyCircle ),
	  m_fCircleRange( rhs.m_fCircleRange ),
	  m_fCircleMoveSpeed( rhs.m_fCircleMoveSpeed ),
	  m_fCircleOffset( rhs.m_fCircleOffset ),
	  m_szChainDummy( rhs.m_szChainDummy )
{
	m_fCurExtraGauge = 0.0f;
	Init();
}

ioChainMagicianItem::~ioChainMagicianItem()
{
	m_vAirJumpTimeList.clear();
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void ioChainMagicianItem::LoadProperty( ioINILoader &rkLoader )
{
	ioWeaponItem::LoadProperty( rkLoader );

	char szBuf[MAX_PATH], szKey[MAX_PATH];

	int nAttackCnt = rkLoader.LoadInt_e( "attack_ready_cnt", 0 );	
	m_AttackReadyAniList.clear();

	for( int i=0; i<nAttackCnt; i++ )
	{
		wsprintf_e( szKey, "attack_ready_animation%d", i+1 );
		rkLoader.LoadString( szKey, "", szBuf, MAX_PATH );
		m_AttackReadyAniList.push_back( szBuf );
	}

	m_fNeedExtraGauge = rkLoader.LoadFloat_e( "need_extra_gauge", FLOAT100 );
	m_fSkillGaugeRate = rkLoader.LoadFloat_e( "skill_gauge_rate", FLOAT1 );

	rkLoader.LoadString_e( "gather_ani", "", szBuf, MAX_PATH );
	m_szGatherAni = szBuf;
	m_fGatherAniRate = rkLoader.LoadFloat_e( "gather_ani_rate", FLOAT1 );

	rkLoader.LoadString_e( "special_loop_ani", "", szBuf, MAX_PATH );
	m_szSpecialLoopAni = szBuf;
	m_fSpecialLoopAniRate = rkLoader.LoadFloat_e( "special_loop_ani_rate", FLOAT1 );

	//마법진 설정
	rkLoader.LoadString_e( "ready_circle", "", szBuf, MAX_PATH );
	m_ReadyCircle = szBuf;
	m_fCircleRange = rkLoader.LoadFloat_e( "magic_circle_range", 0.0f );
	m_fCircleMoveSpeed = rkLoader.LoadFloat_e( "magic_circle_move_speed", 0.0f );
	m_fCircleHeightGap = rkLoader.LoadFloat_e( "magic_circle_height_gap", 0.0f );
	m_fCircleOffset = rkLoader.LoadFloat_e( "magic_circle_offset", 0.0f );

	rkLoader.LoadString_e( "chain_dummy", "", szBuf, MAX_PATH );
	m_szChainDummy = szBuf;
}

ioItem* ioChainMagicianItem::Clone()
{
	return new ioChainMagicianItem( *this );
}

ioWeaponItem::WeaponSubType ioChainMagicianItem::GetSubType() const
{
	return WST_CHAIN_MAGICIAN_ITEM;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void ioChainMagicianItem::SetNormalAttack( ioBaseChar *pOwner, int iCurCombo )
{
	Init();

	DWORD dwCurTime = FRAMEGETTIME();
	int iComboCnt = GetMaxCombo();
	if( !COMPARE( iCurCombo, 0, iComboCnt ) )
	{
		ioWeaponItem::SetNormalAttack( pOwner, iCurCombo );

		m_nCurCombo = 0;
		pOwner->SetAttackMoveEnable( false );
		pOwner->SetChargingState( false );

		m_WeaponItemState = WS_NONE;
	}
	else
	{
		ioEntityGroup *pGrp = pOwner->GetGroup();

		pGrp->ClearAllActionAni( FLOAT100, true );
		pGrp->ClearAllLoopAni( FLOAT100, true );

		int iAniID = pGrp->GetAnimationIdx( m_AttackReadyAniList[iCurCombo] );
		float fTimeRate = (float)m_dwChargeTime / pGrp->GetAnimationFullTime( iAniID );
		pGrp->SetActionAni( iAniID, FLOAT100, FLOAT100, FLOAT1, FLOAT1/fTimeRate, 0.0f, 0.0f, true );

		m_dwAttackStartTime = dwCurTime;
		m_nCurCombo = iCurCombo;
		pOwner->SetState( CS_ATTACK );
		pOwner->SetAttackMoveEnable( false );
		pOwner->SetChargingState( true );

		DWORD dwTrackingTime = dwCurTime;
		dwTrackingTime += pGrp->GetAnimationEventTime( iAniID, "tracking_e" ) * fTimeRate;
		pOwner->SetAutoTargetTrackingEndTime( dwTrackingTime );

		m_WeaponItemState = WS_CHECK_CHARGING;
	}
}

void ioChainMagicianItem::CheckNormalAttack( ioBaseChar *pOwner )
{
	switch( m_WeaponItemState )
	{
	case WS_CHECK_CHARGING:
		CheckCharging( pOwner );
		break;
	case WS_CHARGE_ATTACK:
		OnGathering( pOwner );
		break;
	case WS_NORMAL_ATTACK:
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
	case WS_SPECIAL_ATTACK:
		OnSpecial( pOwner );
		break;
	case WS_END:
		pOwner->SetState( CS_DELAY );
		break;
	}
}

bool ioChainMagicianItem::SetJumpAttack( ioBaseChar *pOwner, int iAniID, float fTimeRate, AttackType eType )
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

void ioChainMagicianItem::ProcessJumpAttack( ioBaseChar *pOwner, float fHeightGap )
{
	if( m_pExtendJump )
		m_pExtendJump->ProcessJumpAttack( pOwner, fHeightGap );

	CheckAirJump( pOwner );
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void ioChainMagicianItem::SetExtraInfoToWeapon( ioWeapon *pWeapon, bool bJumpAttack )
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

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void ioChainMagicianItem::SetUseExtraGauge( float fAmt )
{
	m_fCurExtraGauge -= fAmt;
	m_fCurExtraGauge = max( 0.0f, m_fCurExtraGauge );
}

void ioChainMagicianItem::UpdateExtraData( ioBaseChar *pOwner )
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

void ioChainMagicianItem::InitExtraGauge()
{
	m_fCurExtraGauge = 0.0f;
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

bool ioChainMagicianItem::IsChargeAttackState( ioBaseChar *pOwner )
{
	switch( m_WeaponItemState )
	{
	case WS_CHARGE_ATTACK:
		return true;
	}

	return false;
}

bool ioChainMagicianItem::ProcessCancel( ioBaseChar *pOwner )
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

void ioChainMagicianItem::OnEndExtendAttack( ioBaseChar *pOwner, bool bReleaseItem )
{
	ioDummyChar *pDummyChar = g_DummyCharMgr.FindDummyCharToName( pOwner, m_szChainDummy );
	if ( pDummyChar && pDummyChar->GetState() != DCS_DIE )
		pDummyChar->SetDieState( true );
}

void ioChainMagicianItem::OnItemGaugeOK( ioBaseChar *pOwner, SP2Packet &rkPacket )
{
	if( !pOwner ) 
		return;

	D3DXQUATERNION qtRot;
	rkPacket >> qtRot;
	pOwner->SetTargetRot( qtRot );

	D3DXVECTOR3 vPos;
	rkPacket >> vPos;
	pOwner->SetWorldPosition( vPos );

	int nState;
	rkPacket >> nState;

	switch( nState )
	{
	case WS_CHARGE_ATTACK:
		ChangeToGathering( pOwner );
		break;
	case WS_NORMAL_ATTACK:
		{
			rkPacket >> m_nCurCombo;
			ChangeToNormalAttack( pOwner );
		}
		break;
	case WS_SPECIAL_ATTACK:
		{
			int nIndex;
			rkPacket >> nIndex;

			ChangeToSpecialAttack( pOwner, nIndex );
		}
		break;
	case WS_END:
		m_WeaponItemState = WS_END;
		break;
	}
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

bool ioChainMagicianItem::IsEnableExtraGauge( ioBaseChar *pChar )
{
	if( m_bGaugeUpdateChargeAttack && IsChargeAttackState( pChar ) )
		return false;

	return true;
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

int ioChainMagicianItem::GetMaxBullet()
{
	return (int)m_fMaxExtraGauge;
}

int ioChainMagicianItem::GetNeedBullet()
{
	return (int)m_fNeedExtraGauge;
}

int ioChainMagicianItem::GetCurBullet()
{
	float fCurCoolTime = m_fCurExtraGauge;
	fCurCoolTime = max( 0, min( fCurCoolTime, m_fMaxExtraGauge ) );

	return (int)fCurCoolTime;
}

void ioChainMagicianItem::SetCurBullet( int iCurBullet )
{
	if( iCurBullet < 0 )
		return;

	float fCurBullet = iCurBullet;
	fCurBullet = max( 0, min( fCurBullet, m_fMaxExtraGauge ) );

	m_fCurExtraGauge = fCurBullet;
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void ioChainMagicianItem::Init()
{
	m_WeaponItemState = WS_NONE;

	m_dwAttackStartTime	= 0;
	m_nCurCombo	= 0;
	m_iCurAirJump = 0;

	m_fAirJumpPowerRate	= 0.0f;
	m_dwMotionEndTime = 0;
	m_dwKeyReserveTime = 0;

	m_vCirclePos = ioMath::VEC3_ZERO;
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void ioChainMagicianItem::CheckCharging( ioBaseChar *pOwner )
{
	if( !pOwner->IsNeedProcess() )
		return;

	if( pOwner->IsAttackKeyDown() && !pOwner->IsHasCrown() )
	{
		if( m_dwAttackStartTime + m_dwChargeTime <= FRAMEGETTIME() )
			ChangeToGathering( pOwner );
	}
	else
		ChangeToNormalAttack( pOwner );
}

void ioChainMagicianItem::OnGathering( ioBaseChar *pOwner )
{
	if( !pOwner )
		return;

	if ( CheckGatheringInputKey( pOwner ) )
		return;

	UpdateMagicCircle( pOwner );
}

void ioChainMagicianItem::OnSpecial( ioBaseChar *pOwner )
{
	if( !pOwner )
		return;

	ioDummyChar *pDummyChar = g_DummyCharMgr.FindDummyCharToName( pOwner, m_szChainDummy );
	if ( !pDummyChar )
		m_WeaponItemState = WS_END;
	else
	{
		D3DXVECTOR3 vDummyDir = pDummyChar->GetWorldPosition() - pOwner->GetWorldPosition();
		vDummyDir.y = 0.0f;
		D3DXVec3Normalize( &vDummyDir, &vDummyDir );

		pOwner->SetTargetRotToDir( vDummyDir, true );
	}
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void ioChainMagicianItem::ChangeToNormalAttack( ioBaseChar *pOwner )
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
	m_WeaponItemState = WS_NORMAL_ATTACK;

	pOwner->SetAttackMoveEnable( false );
	pOwner->SetChargingState( false );

	if( pOwner->IsNeedSendNetwork() )
	{
		SP2Packet kPacket( CUPK_ITEM_GAUGE_OK );
		FillItemGaugePacketHeader( pOwner, kPacket );
		kPacket << pOwner->GetTargetRot();
		kPacket << pOwner->GetWorldPosition();
		kPacket << WS_NORMAL_ATTACK;
		kPacket << m_nCurCombo;
		P2PNetwork::SendToAllPlayingUser( kPacket );
	}
}

void ioChainMagicianItem::ChangeToGathering( ioBaseChar *pOwner )
{
	if ( !pOwner )
		return;

	pOwner->StopAutoTargetTracking();

	ioEntityGroup *pGrp = pOwner->GetGroup();
	pGrp->ClearAllActionAni( FLOAT100, true );
	pGrp->ClearAllLoopAni( FLOAT100, true );

	pGrp->SetLoopAni( m_szGatherAni, FLOAT100, FLOAT1, FLOAT1/m_fGatherAniRate );

	pOwner->CreateMagicCircle( m_ReadyCircle, m_ReadyCircle, "", m_fCircleOffset );

	m_WeaponItemState = WS_CHARGE_ATTACK;

	if( pOwner->IsNeedSendNetwork() )
	{
		SP2Packet kPacket( CUPK_ITEM_GAUGE_OK );
		FillItemGaugePacketHeader( pOwner, kPacket );
		kPacket << pOwner->GetTargetRot();
		kPacket << pOwner->GetWorldPosition();
		kPacket << WS_CHARGE_ATTACK;
		P2PNetwork::SendToAllPlayingUser( kPacket );
	}
}

void ioChainMagicianItem::ChangeToSpecialAttack( ioBaseChar *pOwner, int nIndex )
{
	if ( !pOwner )
		return;

	ioEntityGroup *pGrp = pOwner->GetGroup();
	pGrp->ClearAllActionAni( FLOAT100, true );
	pGrp->ClearAllLoopAni( FLOAT100, true );

	pGrp->SetLoopAni( m_szSpecialLoopAni, FLOAT100, FLOAT1, FLOAT1/m_fSpecialLoopAniRate );
	
	m_WeaponItemState = WS_SPECIAL_ATTACK;
	SetUseExtraGauge( m_fNeedExtraGauge );

	pOwner->DestroyMagicCircle();
	CreateChainDummy( pOwner, nIndex );

	if( pOwner->IsNeedSendNetwork() )
	{
		SP2Packet kPacket( CUPK_ITEM_GAUGE_OK );
		FillItemGaugePacketHeader( pOwner, kPacket );
		kPacket << pOwner->GetTargetRot();
		kPacket << pOwner->GetWorldPosition();
		kPacket << WS_SPECIAL_ATTACK;
		kPacket << nIndex;
		P2PNetwork::SendToAllPlayingUser( kPacket );
	}
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void ioChainMagicianItem::UpdateMagicCircle( ioBaseChar *pOwner )
{
	if ( !pOwner || !pOwner->CheckMagicCircle() )
		return;

	pOwner->CheckChangeDirectionForMagicCircle();

	bool bEnableSkill = false;
	if( pOwner->GetUseEnableSkill( 0, SUT_NORMAL ) )
		bEnableSkill = true;

	pOwner->UpdateMagicCirclePosAll( m_fCircleMoveSpeed, m_fCircleRange, m_fCircleHeightGap, bEnableSkill );

	m_vCirclePos = pOwner->GetMagicCirclePos( m_fCircleOffset, false );

	D3DXVECTOR3 vCircleDir = m_vCirclePos - pOwner->GetWorldPosition();
	vCircleDir.y = 0.0f;
	D3DXVec3Normalize( &vCircleDir, &vCircleDir );

	pOwner->SetTargetRotToDir( vCircleDir, true );
}

bool ioChainMagicianItem::CheckGatheringInputKey( ioBaseChar *pOwner )
{
	if( !pOwner || !pOwner->IsNeedProcess() )
		return false;

	if( !pOwner->IsAttackKeyDown() )
	{		
		if ( GetCurBullet() >= GetNeedBullet() )
			ChangeToSpecialAttack( pOwner, g_DummyCharMgr.GetCurLocalIndex() );
		else
		{
			m_WeaponItemState = WS_END;

			if( pOwner->IsNeedSendNetwork() )
			{
				SP2Packet kPacket( CUPK_ITEM_GAUGE_OK );
				FillItemGaugePacketHeader( pOwner, kPacket );
				kPacket << pOwner->GetTargetRot();
				kPacket << pOwner->GetWorldPosition();
				kPacket << WS_END;
				P2PNetwork::SendToAllPlayingUser( kPacket );
			}
		}
		return true;
	}

	if( pOwner->IsDefenseKeyDown() )
	{
		if( pOwner->GetUseEnableSkill( 0, SUT_NORMAL ) )
			pOwner->UpdateMagicCirclePos( m_fCircleMoveSpeed, m_fCircleRange, m_fCircleHeightGap, true, true );

		int iSkillNum = pOwner->GetSkillKeyInput();
		if( iSkillNum == 0 && !pOwner->IsBuffLimitSkill() && pOwner->SetUseSkill(0, SUT_NORMAL) )
		{
			m_WeaponItemState = WS_NONE;
			return true;
		}
	}

	return false;
}

void ioChainMagicianItem::CreateChainDummy( ioBaseChar *pOwner, int nIndex )
{
	if ( !pOwner )
		return;

	ioPlayStage *pStage = pOwner->GetCreator();
	if ( !pStage )
		return;

	g_DummyCharMgr.DestroyDummyCharByName( pOwner, m_szChainDummy );	
	pStage->CreateDummyChar( m_szChainDummy, nIndex, pOwner->GetCharName(), m_vCirclePos, 0.0f, 0, true );
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

bool ioChainMagicianItem::IsDummyCharControl( ioBaseChar *pOwner )
{
	if ( m_WeaponItemState != WS_SPECIAL_ATTACK )
		return false;

	ioDummyChar *pDummyChar = g_DummyCharMgr.FindDummyCharToName( pOwner, m_szChainDummy );
	if ( pDummyChar && pDummyChar->GetState() != DCS_DIE )
		return true;

	return false;
}

D3DXVECTOR3 ioChainMagicianItem::GetDummyCharControlPos( ioBaseChar *pOwner )
{
	if( !pOwner )
		return m_vCirclePos;

	if( m_WeaponItemState != WS_SPECIAL_ATTACK )
		return pOwner->GetWorldPosition();

	ioDummyChar *pDummyChar = g_DummyCharMgr.FindDummyCharToName( pOwner, m_szChainDummy );
	if( pDummyChar )
		return pDummyChar->GetCameraPosition();

	return pOwner->GetWorldPosition();
}