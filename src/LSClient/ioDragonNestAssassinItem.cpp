
#include "stdafx.h"

#include "ioDragonNestAssassinItem.h"

#include "ioBaseChar.h"
#include "ioPlayStage.h"

#include "WeaponDefine.h"
#include "ItemDefine.h"

ioDragonNestAssassinItem::ioDragonNestAssassinItem()
{	
	Init();
}

ioDragonNestAssassinItem::ioDragonNestAssassinItem( const ioDragonNestAssassinItem &rhs )
	: ioWeaponItem( rhs ),
	  m_nMaxCombo( rhs.m_nMaxCombo ),
	  m_nMaxSpecialCombo( rhs.m_nMaxSpecialCombo ),
	  m_nStartSpecialCombo( rhs.m_nStartSpecialCombo ),
	  m_AttackReadyAniList( rhs.m_AttackReadyAniList ),
	  m_PreAttackAttibute( rhs.m_PreAttackAttibute ),
	  m_vChargeComboAttList( rhs.m_vChargeComboAttList ),
	  m_ChargeSpecialComboDelayAttibute( rhs.m_ChargeSpecialComboDelayAttibute ),
	  m_vChargeSpecialComboAttList( rhs.m_vChargeSpecialComboAttList ),
	  m_ChargeSpecialComboEndAttibute( rhs.m_ChargeSpecialComboEndAttibute )
{
	Init();
}

ioDragonNestAssassinItem::~ioDragonNestAssassinItem()
{
}

void ioDragonNestAssassinItem::LoadProperty( ioINILoader &rkLoader )
{
	ioWeaponItem::LoadProperty( rkLoader );

	LoadAttackProperty( rkLoader );		
}

ioItem* ioDragonNestAssassinItem::Clone()
{
	return new ioDragonNestAssassinItem( *this );
}

ioWeaponItem::WeaponSubType ioDragonNestAssassinItem::GetSubType() const
{
	return WST_DRAGONNEST_ASSASASSIN_ITEM;
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void ioDragonNestAssassinItem::SetNormalAttack( ioBaseChar *pOwner, int iCurCombo )
{
	ClearCancelInfo();

	Init();

	DWORD dwCurTime = FRAMEGETTIME();
	int iComboCnt = GetMaxCombo();
	if( !COMPARE( iCurCombo, 0, iComboCnt ) )
	{
		ioWeaponItem::SetNormalAttack( pOwner, iCurCombo );

		m_nNormalCombo = 0;
		pOwner->SetAttackMoveEnable( false );
		pOwner->SetChargingState( false );

		m_WeaponItemState = WS_NONE;
	}
	else
	{
		//공격 준비 애니메이션 설정
		ioEntityGroup *pGrp = pOwner->GetGroup();

		pGrp->ClearAllActionAni( FLOAT100, true );
		pGrp->ClearAllLoopAni( FLOAT100, true );

		int iAniID = pGrp->GetAnimationIdx( m_AttackReadyAniList[iCurCombo] );
		float fTimeRate = (float)m_dwChargeTime / pGrp->GetAnimationFullTime( iAniID );
		pGrp->SetActionAni( iAniID, FLOAT100, FLOAT100, FLOAT1, FLOAT1/fTimeRate, 0.0f, 0.0f, true );

		m_dwAttackStartTime = dwCurTime;
		m_nNormalCombo = iCurCombo;
		pOwner->SetState( CS_ATTACK );
		pOwner->SetAttackMoveEnable( false );
		pOwner->SetChargingState( true );

		DWORD dwTrackingTime = dwCurTime;
		dwTrackingTime += pGrp->GetAnimationEventTime( iAniID, "tracking_e" ) * fTimeRate;
		pOwner->SetAutoTargetTrackingEndTime( dwTrackingTime );

		m_WeaponItemState = WS_CHECK_CHARGING;
	}
}

void ioDragonNestAssassinItem::CheckNormalAttack( ioBaseChar *pOwner )
{
	switch( m_WeaponItemState )
	{
	case WS_CHECK_CHARGING:
		{
			CheckCharging( pOwner );
		}
		break;
	case WS_NORMAL_ATTACK:
		{
			//input_jump_s 이벤트
			if( pOwner->IsNeedProcess() )
			{
				CheckCancelReserve( pOwner );

				if( ProcessCancel(pOwner) )
					return;
			}

			ioWeaponItem::CheckNormalAttack( pOwner );
		}
		break;
	}
}

void ioDragonNestAssassinItem::SetExtraInfoToWeapon( ioWeapon *pWeapon, bool bJumpAttack )
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

void ioDragonNestAssassinItem::OnItemGaugeOK( ioBaseChar *pOwner, SP2Packet &rkPacket )
{
	if( !pOwner ) 
		return;

	int iState;
	rkPacket >> iState;

	switch( iState )
	{
	case WS_NORMAL_ATTACK:
		{
			rkPacket >> m_nNormalCombo;

			D3DXQUATERNION qtRot;
			rkPacket >> qtRot;

			pOwner->SetTargetRotToRotate( qtRot, true );
			SetNormalAttackState( pOwner );
		}
		break;
	case WS_SPECIAL_STATE_INIT:
		{
			pOwner->SetState( CS_DRAGONNEST_ASS_SPECIAL );
			SetChargePreAttackState( pOwner );
		}
		break;
	case WS_SPECIAL_STATE_PROCESS:
		{
			RecvSpecialState( pOwner, rkPacket );
		}
		break;
	}
}

bool ioDragonNestAssassinItem::IsChargeAttackState( ioBaseChar *pOwner )
{
	return false;
}

void ioDragonNestAssassinItem::SetNextItemState( ioBaseChar *pWoundChar, ioBaseChar *pAttacker, ioWeapon *pWeapon, bool bDefense, float fDamage )
{
	pAttacker->ClearAttackFireTimeAndSkill();

	if( pWoundChar->IsNeedProcess() && fDamage > 0.0f )
	{
		if ( pWoundChar && pAttacker )
		{		
			ioChargeComboDash *pChargeComboDash = ToChargeComboDash( m_pExtendDash );
			if ( pChargeComboDash )
				pChargeComboDash->SetDragonNestEnableTeleport( pAttacker );

			if( pWoundChar->IsNeedSendNetwork() )
			{
				SP2Packet kPacket( CUPK_ITEM_GAUGE_OK );
				FillItemGaugePacketHeader( pAttacker, kPacket );
				kPacket << WS_SPECIAL_STATE_PROCESS;
				kPacket << DNASS_Dash_Teleport;
				kPacket << pAttacker->GetWorldPosition();
				kPacket << pAttacker->GetWorldOrientation();
				P2PNetwork::SendToAllPlayingUser( kPacket );
			}
		}
	}
	
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void ioDragonNestAssassinItem::Init()
{	
	m_WeaponItemState = WS_NONE;
	m_DragonNestAssassinSpecialState = DNASS_None;

	m_nNormalCombo = 0;

	m_bAttackKeyReserved = false;
	m_bDefenseKeyReserved = false;
	m_bJumpKeyReserved = false;

	m_dwPreMotionEndTime = 0;
	m_dwComboEndMotionEndTime = 0;
	m_dwComboReserveTime = 0;

	m_nCurCombo = 0;
	m_nCurSpecialCombo = 0;
	m_CurDirKey = ioUserKeyInput::DKI_NONE;

	ClearCancelInfo();
}

void ioDragonNestAssassinItem::LoadAttackProperty( ioINILoader &rkLoader )
{
	char szBuf[MAX_PATH], szKey[MAX_PATH];

	//준비 동작
	int iExtendCnt = rkLoader.LoadInt_e( "extend_attack_cnt", 0 );	
	m_AttackReadyAniList.clear();

	for( int i=0; i<iExtendCnt; i++ )
	{
		wsprintf_e( szKey, "attack_ready_animation%d", i+1 );
		rkLoader.LoadString( szKey, "", szBuf, MAX_PATH );
		m_AttackReadyAniList.push_back( ioHashString(szBuf) );
	}

	LoadAttackAttribute( "charge_pre_attack", m_PreAttackAttibute, rkLoader );

	m_nMaxCombo = rkLoader.LoadInt_e( "charge_combo_max_cnt", 0 );
	m_vChargeComboAttList.clear();
	for( int i=0; i < m_nMaxCombo; ++i )
	{
		AttackAttribute kAttribute;
		wsprintf( szKey, "charge_combo_attack%d", i+1 );
		LoadAttackAttribute( szKey, kAttribute, rkLoader );
		m_vChargeComboAttList.push_back( kAttribute );
	}
	
	LoadAttackAttribute( "charge_special_combo_delay", m_ChargeSpecialComboDelayAttibute, rkLoader );

	m_nMaxSpecialCombo = rkLoader.LoadInt_e( "charge_special_combo_max_cnt", 0 );
	m_vChargeSpecialComboAttList.clear();
	for( int i=0; i < m_nMaxSpecialCombo; ++i )
	{
		AttackAttribute kAttribute;
		wsprintf( szKey, "charge_special_combo_attack%d", i+1 );
		LoadAttackAttribute( szKey, kAttribute, rkLoader );
		m_vChargeSpecialComboAttList.push_back( kAttribute );
	}

	LoadAttackAttribute( "charge_special_combo_end", m_ChargeSpecialComboEndAttibute, rkLoader );

	m_nStartSpecialCombo = rkLoader.LoadInt_e( "charge_special_combo_start", 0 );
}

void ioDragonNestAssassinItem::RecvSpecialState( ioBaseChar *pOwner, SP2Packet &rkPacket )
{
	if( !pOwner ) 
		return;

	int nSpecialState;
	rkPacket >> nSpecialState;

	switch( nSpecialState )
	{
	case DNASS_Combo_Attack:
		{
			rkPacket >> m_nCurCombo;

			int nDirKey;
			rkPacket >> nDirKey;
			m_CurDirKey = (ioUserKeyInput::DirKeyInput)nDirKey;
			pOwner->SetTargetRotToDirKey( m_CurDirKey );

			SetComboNormalAttackState( pOwner );
		}
		break;
	case DNASS_Combo_Special_Delay:
		{
			SetComboSpecialDelayState( pOwner );
		}
		break;
	case DNASS_Combo_Special_Attack:
		{
			rkPacket >> m_nCurSpecialCombo;

			SetComboSpecialAttackState( pOwner );
		}
		break;
	case DNASS_Dash_Teleport:
		{
			D3DXVECTOR3 vPos;
			rkPacket >> vPos;
			
			D3DXQUATERNION qtRot;
			rkPacket >> qtRot;

			ioChargeComboDash *pChargeComboDash = ToChargeComboDash( m_pExtendDash );
			if ( pChargeComboDash )
				pChargeComboDash->SetDragonNestEnableTeleport( pOwner );

			pOwner->SetWorldPosition( vPos );
			pOwner->SetTargetRotToRotate( qtRot, true );
		}
		break;
	case DNASS_End:
		{
			m_DragonNestAssassinSpecialState = DNASS_End;			
		}
		break;
	}
}

void ioDragonNestAssassinItem::CheckCharging( ioBaseChar *pOwner )
{
	if( !pOwner->IsNeedProcess() )	
		return;

	//게이지 확인
	if( pOwner->IsAttackKeyDown() && !pOwner->IsHasCrown() )
	{
		if( m_dwAttackStartTime + m_dwChargeTime <= FRAMEGETTIME() )
			SetDragonNestAssSpecialState( pOwner );
	}
	else
	{
		//일반 공격
		SetNormalAttackState( pOwner );
	}
}

void ioDragonNestAssassinItem::SetNormalAttackState( ioBaseChar *pOwner )
{
	if( !pOwner ) 
		return;

	ClearCancelInfo();

	DWORD dwCurTime = FRAMEGETTIME();

	pOwner->InitExtendAttackTagInfo();
	pOwner->InitActionStopDelay( true );

	const AttackAttribute *pAttribute = m_FormalAttack.GetAttribute( AT_NORMAL, m_nNormalCombo );
	if( pAttribute )
	{
		pOwner->SetNormalAttackByAttribute( *pAttribute );

		m_szCurAni = pAttribute->m_AttackAnimation;
		m_fCurAniRate = pAttribute->m_fAttackAniRate;

		m_dwInputCancelCheckTime = dwCurTime;
		m_dwCancelCheckTime = dwCurTime;
	}

	pOwner->ApplyNormalAttackResult( m_nNormalCombo );
	m_WeaponItemState = WS_NORMAL_ATTACK;

	if( pOwner->IsNeedSendNetwork() )
	{
		SP2Packet kPacket( CUPK_ITEM_GAUGE_OK );
		FillItemGaugePacketHeader( pOwner, kPacket );
		kPacket << m_WeaponItemState;
		kPacket << m_nNormalCombo;
		kPacket << pOwner->GetTargetRot();
		P2PNetwork::SendToAllPlayingUser( kPacket );
	}
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void ioDragonNestAssassinItem::ClearState( ioBaseChar *pOwner )
{
	Init();
}

void ioDragonNestAssassinItem::SetDragonNestAssSpecialState( ioBaseChar *pOwner )
{
	if( !pOwner || !pOwner->IsNeedProcess() )
		return;
	
	pOwner->SetState( CS_DRAGONNEST_ASS_SPECIAL );
	SetChargePreAttackState( pOwner );

	if( pOwner->IsNeedSendNetwork() )
	{
		SP2Packet kPacket( CUPK_ITEM_GAUGE_OK );
		FillItemGaugePacketHeader( pOwner, kPacket );
		kPacket << WS_SPECIAL_STATE_INIT;
		P2PNetwork::SendToAllPlayingUser( kPacket );
	}
}

void ioDragonNestAssassinItem::ProcessDragonNestAssSpecialState( ioBaseChar *pOwner )
{
	if( !pOwner )
		return;

	if ( !pOwner->IsNeedProcess() )
	{
		switch ( m_DragonNestAssassinSpecialState )
		{
		case DNASS_Pre_Attack:
		case DNASS_Combo_Attack:
		case DNASS_Combo_Check:
		case DNASS_Combo_Special_Delay:
			return;
		}
	}

	DWORD dwCurTime = FRAMEGETTIME();
	
	switch( m_DragonNestAssassinSpecialState )
	{	
	case DNASS_Pre_Attack:
		{
			CheckCancelReserve( pOwner );

			if( ProcessCancel(pOwner) )
				return;

			if ( pOwner->IsAttackKey() && m_dwComboReserveTime < dwCurTime )
			{
				m_DragonNestAssassinSpecialState = DNASS_Combo_Attack;
				return;
			}

			if ( m_dwPreMotionEndTime > 0 && m_dwPreMotionEndTime < dwCurTime )
				CheckReserveState( pOwner );	
		}
		break;
	case DNASS_Combo_Attack:
		{
			/*
			CheckCancelReserve( pOwner );

			if( ProcessCancel(pOwner) )
				return;
			*/

			if ( !CheckComboInput( pOwner ) )
			{					
				if ( m_dwComboEndMotionEndTime > 0 && m_dwComboEndMotionEndTime < dwCurTime )
					CheckReserveState( pOwner );	
			}		
		}
		break;
	case DNASS_Combo_Check:
		{
			/*
			CheckCancelReserve( pOwner );

			if( ProcessCancel(pOwner) )
				return;
			*/

			if ( pOwner->IsAttackKey() && m_nCurCombo < m_nMaxCombo && m_dwComboReserveTime < dwCurTime )
			{
				m_DragonNestAssassinSpecialState = DNASS_Combo_Attack;
				return;
			}
			else if ( m_dwComboEndMotionEndTime > 0 && m_dwComboEndMotionEndTime < dwCurTime )
				CheckReserveState( pOwner );
		}
		break;
	case DNASS_Combo_Special_Delay:
		{
			if ( m_dwComboSpecialMotionEndTime < dwCurTime )
			{
				SetComboSpecialAttackState( pOwner );
				return;
			}

			CheckComboSpecialInput( pOwner );
		}
		break;
	case DNASS_Combo_Special_Attack:
		{
			if ( m_dwComboSpecialMotionEndTime < dwCurTime )
				SetComboSpecialEndState( pOwner );
		}
		break;
	case DNASS_End:
		{
			pOwner->SetState( CS_DELAY );
		}
		break;
	}
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void ioDragonNestAssassinItem::SetChargePreAttackState( ioBaseChar *pOwner )
{
	if ( !pOwner )
		return;

	ioEntityGroup *pGrp = pOwner->GetGroup();

	int iAniID = pGrp->GetAnimationIdx( m_PreAttackAttibute.m_AttackAnimation );
	if ( iAniID == -1)
		return;
	
	float fTimeRate  = m_PreAttackAttibute.m_fAttackAniRate;
	DWORD dwPreDelay = m_PreAttackAttibute.m_dwPreDelay;
	
	pGrp->ClearAllLoopAni( FLOAT100, true );
	pGrp->ClearAllActionAni( FLOAT100, true );
	pGrp->SetActionAni( iAniID, FLOAT100, FLOAT100, FLOAT1, FLOAT1/fTimeRate, 0.0f, dwPreDelay );	

	pOwner->ClearReservedSliding();
	pOwner->SetReservedSliding( m_PreAttackAttibute.m_vForceInfoList, iAniID, fTimeRate, dwPreDelay );

	pOwner->RefreshFireTimeList( iAniID, m_PreAttackAttibute.m_vWeaponInfoList, FTT_NORMAL_ATTACK, fTimeRate, dwPreDelay );

	DWORD dwCurTime = FRAMEGETTIME();
	m_dwPreMotionEndTime = dwCurTime;
	m_dwPreMotionEndTime += pGrp->GetAnimationFullTime( iAniID ) * fTimeRate;
	m_dwPreMotionEndTime += m_PreAttackAttibute.m_dwPreDelay;

	m_dwComboReserveTime = dwCurTime;
	m_dwComboReserveTime += pGrp->GetAnimationEventTime_e( iAniID, "key_reserve" ) * fTimeRate;

	m_DragonNestAssassinSpecialState = DNASS_Pre_Attack;
}

void ioDragonNestAssassinItem::SetComboAttackState( ioBaseChar *pOwner )
{
	if ( !pOwner || !pOwner->IsNeedProcess() || m_nMaxCombo <= 0 )
		return;

	if ( m_nCurCombo == m_nStartSpecialCombo )
		SetComboSpecialDelayState( pOwner );
	else
		SetComboNormalAttackState( pOwner );

	m_nCurCombo++;
}

void ioDragonNestAssassinItem::SetComboNormalAttackState( ioBaseChar *pOwner )
{
	if ( !pOwner || m_vChargeComboAttList.empty() )
		return;

	//연타시 4타 이상일때 타격 가능하게 만듬
	pOwner->InitActionStopDelay( true );
	
	const AttackAttribute &pAttribute = m_vChargeComboAttList[m_nCurCombo];

	ioEntityGroup *pGrp = pOwner->GetGroup();
	int iAniID = pGrp->GetAnimationIdx( pAttribute.m_AttackAnimation );
	if ( iAniID == -1)
		return;
	
	float fTimeRate  = pAttribute.m_fAttackAniRate;
	DWORD dwPreDelay = pAttribute.m_dwPreDelay;

	pGrp->ClearAllLoopAni( FLOAT100, true );
	pGrp->ClearAllActionAni( FLOAT100, true );

	pGrp->SetActionAni( iAniID, FLOAT100, FLOAT100, FLOAT1, FLOAT1/fTimeRate, 0.0f, dwPreDelay );	

	pOwner->ClearReservedSliding();
	pOwner->SetReservedSliding( pAttribute.m_vForceInfoList, iAniID, fTimeRate, dwPreDelay );

	pOwner->RefreshFireTimeList( iAniID, pAttribute.m_vWeaponInfoList, FTT_NORMAL_ATTACK, fTimeRate, dwPreDelay );

	DWORD dwCurTime = FRAMEGETTIME();
	m_dwComboReserveTime = dwCurTime;
	m_dwComboReserveTime += pGrp->GetAnimationEventTime_e( iAniID, "key_reserve" ) * fTimeRate;

	m_dwComboEndMotionEndTime = dwCurTime;
	m_dwComboEndMotionEndTime += dwPreDelay;
	m_dwComboEndMotionEndTime += pGrp->GetAnimationFullTime( iAniID ) * fTimeRate;

	m_DragonNestAssassinSpecialState = DNASS_Combo_Check;
	
	SetCurAttackAniForInputJump( pAttribute.m_AttackAnimation, fTimeRate );

	SetCharRotate( pOwner );

	if( pOwner->IsNeedSendNetwork() )
	{
		SP2Packet kPacket( CUPK_ITEM_GAUGE_OK );
		FillItemGaugePacketHeader( pOwner, kPacket );
		kPacket << WS_SPECIAL_STATE_PROCESS;
		kPacket << DNASS_Combo_Attack;
		kPacket << m_nCurCombo;
		kPacket << m_CurDirKey;
		P2PNetwork::SendToAllPlayingUser( kPacket );
	}	
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void ioDragonNestAssassinItem::SetComboSpecialDelayState( ioBaseChar *pOwner )
{
	if ( !pOwner )
		return;

	ioEntityGroup *pGrp = pOwner->GetGroup();
	int iAniID = pGrp->GetAnimationIdx( m_ChargeSpecialComboDelayAttibute.m_AttackAnimation );
	if ( iAniID == -1)
		return;

	float fTimeRate  = m_ChargeSpecialComboDelayAttibute.m_fAttackAniRate;
	DWORD dwPreDelay = m_ChargeSpecialComboDelayAttibute.m_dwPreDelay;

	pGrp->ClearAllLoopAni( FLOAT100, true );
	pGrp->ClearAllActionAni( FLOAT100, true );

	pGrp->SetActionAni( iAniID, FLOAT100, FLOAT100, FLOAT1, FLOAT1/fTimeRate, 0.0f, dwPreDelay );	

	pOwner->ClearReservedSliding();
	pOwner->SetReservedSliding( m_ChargeSpecialComboDelayAttibute.m_vForceInfoList, iAniID, fTimeRate, dwPreDelay );

	pOwner->RefreshFireTimeList( iAniID, m_ChargeSpecialComboDelayAttibute.m_vWeaponInfoList, FTT_NORMAL_ATTACK, fTimeRate, dwPreDelay );

	DWORD dwCurTime = FRAMEGETTIME();
	m_dwComboSpecialMotionEndTime = dwCurTime;
	m_dwComboSpecialMotionEndTime += pGrp->GetAnimationFullTime( iAniID ) * fTimeRate;
	m_dwComboSpecialMotionEndTime += dwPreDelay;

	m_DragonNestAssassinSpecialState = DNASS_Combo_Special_Delay;
	m_nCurSpecialCombo = 0;

	if( pOwner->IsNeedSendNetwork() )
	{
		SP2Packet kPacket( CUPK_ITEM_GAUGE_OK );
		FillItemGaugePacketHeader( pOwner, kPacket );
		kPacket << WS_SPECIAL_STATE_PROCESS;
		kPacket << DNASS_Combo_Special_Delay;
		P2PNetwork::SendToAllPlayingUser( kPacket );
	}	
}

void ioDragonNestAssassinItem::SetComboSpecialAttackState( ioBaseChar *pOwner )
{
	if ( !pOwner || m_vChargeSpecialComboAttList.empty() )
		return;

	//연타시 4타 이상일때 타격 가능하게 만듬
	pOwner->InitActionStopDelay( true );

	const AttackAttribute &pAttribute = m_vChargeSpecialComboAttList[m_nCurSpecialCombo];

	ioEntityGroup *pGrp = pOwner->GetGroup();
	int iAniID = pGrp->GetAnimationIdx( pAttribute.m_AttackAnimation );
	if ( iAniID == -1)
		return;
	
	float fTimeRate  = pAttribute.m_fAttackAniRate;
	DWORD dwPreDelay = pAttribute.m_dwPreDelay;

	pGrp->ClearAllLoopAni( FLOAT100, true );
	pGrp->ClearAllActionAni( FLOAT100, true );

	pGrp->SetActionAni( iAniID, FLOAT100, FLOAT100, FLOAT1, FLOAT1/fTimeRate, 0.0f, dwPreDelay );	

	pOwner->ClearReservedSliding();
	pOwner->SetReservedSliding( pAttribute.m_vForceInfoList, iAniID, fTimeRate, dwPreDelay );

	pOwner->RefreshFireTimeList( iAniID, pAttribute.m_vWeaponInfoList, FTT_NORMAL_ATTACK, fTimeRate, dwPreDelay, true );

	DWORD dwCurTime = FRAMEGETTIME();
	m_dwComboSpecialMotionEndTime = dwCurTime;
	m_dwComboSpecialMotionEndTime += dwPreDelay;
	m_dwComboSpecialMotionEndTime += pGrp->GetAnimationFullTime( iAniID ) * fTimeRate;

	m_DragonNestAssassinSpecialState = DNASS_Combo_Special_Attack;
	
	if( pOwner->IsNeedSendNetwork() )
	{
		SP2Packet kPacket( CUPK_ITEM_GAUGE_OK );
		FillItemGaugePacketHeader( pOwner, kPacket );
		kPacket << WS_SPECIAL_STATE_PROCESS;
		kPacket << DNASS_Combo_Special_Attack;
		kPacket << m_nCurSpecialCombo;
		P2PNetwork::SendToAllPlayingUser( kPacket );
	}	
}

void ioDragonNestAssassinItem::SetComboSpecialEndState( ioBaseChar *pOwner )
{
	if ( !pOwner )
		return;

	ioEntityGroup *pGrp = pOwner->GetGroup();

	int iAniID = pGrp->GetAnimationIdx( m_ChargeSpecialComboEndAttibute.m_AttackAnimation );
	if ( iAniID == -1)
		return;

	float fTimeRate  = m_ChargeSpecialComboEndAttibute.m_fAttackAniRate;
	DWORD dwPreDelay = m_ChargeSpecialComboEndAttibute.m_dwPreDelay;

	pGrp->ClearAllLoopAni( FLOAT100, true );
	pGrp->ClearAllActionAni( FLOAT100, true );
	pGrp->SetActionAni( iAniID, FLOAT100, FLOAT100, FLOAT1, FLOAT1/fTimeRate, 0.0f, dwPreDelay );	

	pOwner->ClearReservedSliding();
	pOwner->SetReservedSliding( m_ChargeSpecialComboEndAttibute.m_vForceInfoList, iAniID, fTimeRate, dwPreDelay );

	pOwner->RefreshFireTimeList( iAniID, m_ChargeSpecialComboEndAttibute.m_vWeaponInfoList, FTT_NORMAL_ATTACK, fTimeRate, dwPreDelay );

	DWORD dwCurTime = FRAMEGETTIME();
	m_dwComboReserveTime = dwCurTime;
	m_dwComboReserveTime += pGrp->GetAnimationEventTime_e( iAniID, "key_reserve" ) * fTimeRate;

	m_dwComboEndMotionEndTime = dwCurTime;
	m_dwComboEndMotionEndTime += dwPreDelay;
	m_dwComboEndMotionEndTime += pGrp->GetAnimationFullTime( iAniID ) * fTimeRate;

	m_DragonNestAssassinSpecialState = DNASS_Combo_Check;
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////

bool ioDragonNestAssassinItem::CheckComboInput( ioBaseChar *pOwner )
{
	if ( !pOwner || !pOwner->IsNeedProcess() )
		return false;

	DWORD dwCurTime = FRAMEGETTIME();
	if ( m_nCurCombo < m_nMaxCombo && m_dwComboReserveTime < dwCurTime )
	{
		SetComboAttackState( pOwner );
		return true;
	}

	return false;
}

void ioDragonNestAssassinItem::CheckComboSpecialInput( ioBaseChar *pOwner )
{
	if ( !pOwner || !pOwner->IsNeedProcess() )
		return;

	if ( pOwner->IsAttackKey() && m_nCurSpecialCombo < m_nMaxSpecialCombo - 1 )
		m_nCurSpecialCombo++;
}

void ioDragonNestAssassinItem::SetCharRotate( ioBaseChar *pOwner )
{
	if( pOwner->IsNeedProcess() && pOwner->IsSettedDirection() )
	{
		ioUserKeyInput::DirKeyInput eNewDirKey = pOwner->CheckCurDirKey();
		if( m_CurDirKey == eNewDirKey )
		{
			m_CurDirKey = ioUserKeyInput::DKI_NONE;
			return;
		}

		m_CurDirKey = eNewDirKey;
		pOwner->SetTargetRotToDirKey( m_CurDirKey );
	}
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void ioDragonNestAssassinItem::ReserveInputKey(ioBaseChar *pOwner )
{
	if ( !pOwner || !pOwner->IsNeedProcess() )
		return;

	if( pOwner->IsAttackKey() )
		m_bAttackKeyReserved = true;

	if( pOwner->IsDefenseKey() )
		m_bDefenseKeyReserved = true;
}

void ioDragonNestAssassinItem::CheckReserveState( ioBaseChar *pOwner )
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

	m_DragonNestAssassinSpecialState = DNASS_End;

	if( pOwner->IsNeedSendNetwork() )
	{
		SP2Packet kPacket( CUPK_ITEM_GAUGE_OK );
		FillItemGaugePacketHeader( pOwner, kPacket );
		kPacket << WS_SPECIAL_STATE_PROCESS;
		kPacket << DNASS_End;
		P2PNetwork::SendToAllPlayingUser( kPacket );
	}	
}

void ioDragonNestAssassinItem::SetReserveAttack( ioBaseChar *pOwner )
{
	if ( !pOwner || !pOwner->IsNeedProcess() )
		return;

	pOwner->SetNormalAttack(0);

	m_bAttackKeyReserved = false;
}

void ioDragonNestAssassinItem::SetReserveDefence( ioBaseChar *pOwner )
{
	if ( !pOwner || !pOwner->IsNeedProcess() )
		return;

	pOwner->SetDefenseState();
	m_bDefenseKeyReserved = false;
}

void ioDragonNestAssassinItem::SetReserveJump( ioBaseChar *pOwner )
{
	if ( !pOwner || !pOwner->IsNeedProcess() )
		return;

	pOwner->SetJumpState();
	m_bJumpKeyReserved = false;	
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*
void ioDragonNestAssassinItem::SetDashTeleportState( ioBaseChar *pTarget, ioBaseChar *pAttacker )
{
	if ( pTarget && pAttacker )
	{		
		ioChargeComboDash *pChargeComboDash = ToChargeComboDash( m_pExtendDash );
		if ( pChargeComboDash )
			pChargeComboDash->SetDragonNestEnableTeleport( pAttacker );

		if( pTarget->IsNeedProcess() && pTarget->IsNeedSendNetwork() )
		{
			SP2Packet kPacket( CUPK_ITEM_GAUGE_OK );
			FillItemGaugePacketHeader( pAttacker, kPacket );
			kPacket << WS_SPECIAL_STATE_PROCESS;
			kPacket << DNASS_Dash_Teleport;
			kPacket << pAttacker->GetWorldPosition();
			kPacket << pAttacker->GetWorldOrientation();
			P2PNetwork::SendToAllPlayingUser( kPacket );
		}
	}
}
*/
//-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

ioDragonNestAssSpecialState::ioDragonNestAssSpecialState()
{
}

ioDragonNestAssSpecialState::~ioDragonNestAssSpecialState()
{
}

void ioDragonNestAssSpecialState::Initialize( ioBaseChar* pOwner, CharState eCurrState )
{
	ioSpecialStateBase::Initialize( pOwner, eCurrState );
}

void ioDragonNestAssSpecialState::Finalize( ioBaseChar* pOwner, CharState eCurrState, CharState eNewState )
{
	ioSpecialStateBase::Finalize( pOwner, eCurrState, eNewState );

	ioDragonNestAssassinItem *pDragonNestAssassinItem = ToDragonNestAssassinItem( pOwner->GetPriorityItem( SPT_ATTACK ) );
	if( pDragonNestAssassinItem )
		pDragonNestAssassinItem->ClearState( pOwner );
}

void ioDragonNestAssSpecialState::CheckSpecialState( ioBaseChar* pOwner )
{
	ioDragonNestAssassinItem *pDragonNestAssassinItem = ToDragonNestAssassinItem( pOwner->GetPriorityItem( SPT_ATTACK ) );
	if( pDragonNestAssassinItem )
		pDragonNestAssassinItem->ProcessDragonNestAssSpecialState( pOwner );
	else
	{
		pOwner->SetState( CS_DELAY );
		pOwner->SetFallState( true );
		return;
	}
}

void ioDragonNestAssSpecialState::CheckFallStateChange( ioBaseChar* pOwner, float fMinFallHeightRate, float fBottomHeight, bool bBottomMaxCheck, float fMaxBottomHeight )
{
	return;
}