
#include "stdafx.h"

#include "ioNephilimItem.h"

#include "ioPlayStage.h"
#include "ioBaseChar.h"

#include "WeaponDefine.h"

ioNephilimItem::ioNephilimItem()
{
	Init();
}

ioNephilimItem::ioNephilimItem( const ioNephilimItem &rhs )
	: ioWeaponItem( rhs ), 
	  m_szPreGatherAniList( rhs.m_szPreGatherAniList ),
	  m_ChargeAttackAttribute( rhs.m_ChargeAttackAttribute ),
	  m_PushAttackAttribute( rhs.m_PushAttackAttribute ),
	  m_BombAttackAttribute( rhs.m_BombAttackAttribute ),
	  m_AreaWeaponInfoList( rhs.m_AreaWeaponInfoList ),
	  m_fTeleportHeightOffset( rhs.m_fTeleportHeightOffset ),
	  m_fTeleportEndJumpPower( rhs.m_fTeleportEndJumpPower ),
	  m_fBombStartJumpPower( rhs.m_fBombStartJumpPower ),
	  m_fBombEndJumpPower( rhs.m_fBombEndJumpPower ),
	  m_szSpecialCheckBuff( rhs.m_szSpecialCheckBuff ),
	  m_szTargetCheckBuff( rhs.m_szTargetCheckBuff ),
	  m_nMaxCreateAreaIdx( rhs.m_nMaxCreateAreaIdx )
{
	Init();
}

ioNephilimItem::~ioNephilimItem()
{
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void ioNephilimItem::LoadProperty( ioINILoader &rkLoader )
{
	ioWeaponItem::LoadProperty( rkLoader );

	char szBuf[MAX_PATH], szKey[MAX_PATH];
	
	int nPreCount = rkLoader.LoadInt_e( "max_combo", 0 );
	for( int i=0; i<nPreCount; ++i )
	{
		wsprintf_e( szBuf, "attack_ready_animation%d", i+1 );
		rkLoader.LoadString( szBuf, "", szBuf, MAX_PATH );
		m_szPreGatherAniList.push_back( szBuf );
	}
	
	LoadAttackAttribute( "charge_attack", m_ChargeAttackAttribute, rkLoader );	
	LoadAttackAttribute( "push_attack", m_PushAttackAttribute, rkLoader );
	LoadAttackAttribute( "bomb_attack", m_BombAttackAttribute, rkLoader );

	int nSize = rkLoader.LoadInt_e( "area_weapon_cnt", 0 );
	for( int i=0; i<nSize; ++i )
	{
		AreaWeaponInfo sAreaWeaponInfo;

		wsprintf_e( szKey, "area_weapon%d", i+1 );
		rkLoader.LoadString( szKey, "", szBuf, MAX_PATH );
		sAreaWeaponInfo.m_szAreaWeapon = szBuf;

		wsprintf_e( szKey, "area_weapon%d_teleport_weapon_num", i+1 );
		sAreaWeaponInfo.m_nTeleportWeaponNum = rkLoader.LoadInt( szKey, NO_WEAPON_ATTRIBUTE );
		wsprintf_e( szKey, "area_weapon%d_bomb_weapon_num", i+1 );
		sAreaWeaponInfo.m_nBombWeaponNum = rkLoader.LoadInt( szKey, NO_WEAPON_ATTRIBUTE );

		m_AreaWeaponInfoList.push_back( sAreaWeaponInfo );
	}

	m_fTeleportHeightOffset = rkLoader.LoadFloat_e( "teleport_height_offset", 0.0f );
	m_fTeleportEndJumpPower = rkLoader.LoadFloat_e( "teleport_end_jump_power", 0.0f );
	m_fBombStartJumpPower = rkLoader.LoadFloat_e( "bomb_start_jump_power", 0.0f );
	m_fBombEndJumpPower = rkLoader.LoadFloat_e( "bomb_end_jump_power", 0.0f );

	rkLoader.LoadString( "special_check_buff", "", szBuf, MAX_PATH );
	m_szSpecialCheckBuff = szBuf;

	rkLoader.LoadString( "push_target_check_buff", "", szBuf, MAX_PATH );
	m_szTargetCheckBuff = szBuf;

	m_nMaxCreateAreaIdx = rkLoader.LoadInt_e( "max_create_area_idx", 0 );
}

ioItem* ioNephilimItem::Clone()
{
	return new ioNephilimItem( *this );
}

ioWeaponItem::WeaponSubType ioNephilimItem::GetSubType() const
{
	return WST_NEPHILIM_ITEM;
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

const char* ioNephilimItem::GetAttackStateText() const
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
	case CS_PUSH_ATTACK:
		return "CS_PUSH_ATTACK";
	case CS_BOMB_ATTACK:
		return "CS_BOMB_ATTACK";
	}

	return "";
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void ioNephilimItem::SetNormalAttack( ioBaseChar *pOwner, int iCurCombo )
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

void ioNephilimItem::CheckNormalAttack( ioBaseChar *pOwner )
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

bool ioNephilimItem::SetJumpAttack( ioBaseChar *pOwner, int iAniID, float fTimeRate, AttackType eType )
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

void ioNephilimItem::ProcessJumpAttack( ioBaseChar *pOwner, float fHeightGap )
{
	if( m_pExtendJump )
		m_pExtendJump->ProcessJumpAttack( pOwner, fHeightGap );

	CheckAirJump( pOwner );
}

void ioNephilimItem::SetJumpping( ioBaseChar *pOwner, bool bFullTime )
{
	if ( bFullTime )
	{
		bool bCheckBuff = pOwner->HasBuff( pOwner->GetCharName(), m_szSpecialCheckBuff );
		if ( bCheckBuff )
			ChangeToSpecialAttack( pOwner, CS_NEPHILIM_BOMB_STATE, CS_BOMB_ATTACK, false );
		else
		{
			if ( !ChangeToChargeJump( pOwner ) )
				ioAttackableItem::SetJumpping( pOwner, false );
			return;
		}
	}

	ioAttackableItem::SetJumpping( pOwner, bFullTime );
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void ioNephilimItem::SetExtraInfoToWeapon( ioWeapon *pWeapon, bool bJumpAttack )
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

bool ioNephilimItem::IsChargeAttackState( ioBaseChar *pOwner )
{
	if( pOwner->GetState() == CS_ATTACK )
	{
		if( m_ChargeState == CS_CHARGING )
			return true;
	}

	return false;
}

bool ioNephilimItem::ProcessCancel( ioBaseChar *pOwner )
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

void ioNephilimItem::OnItemGaugeOK( ioBaseChar *pOwner, SP2Packet &rkPacket )
{
	if( !pOwner ) 
		return;

	int nState;
	rkPacket >> nState;

	switch( nState )
	{
	case CS_ATTACK_FIRE:
		{
			rkPacket >> m_nCurCombo;

			D3DXVECTOR3 vPos;
			rkPacket >> vPos;
			pOwner->SetWorldPosition( vPos );

			D3DXQUATERNION qtRot;
			rkPacket >> qtRot;
			pOwner->SetTargetRotToRotate( qtRot, true );

			ChangeToAttackFire( pOwner );
		}
		break;
	case CS_CHARGE_ATTACK:
		{
			D3DXVECTOR3 vPos;
			rkPacket >> vPos;
			pOwner->SetWorldPosition( vPos );

			D3DXQUATERNION qtRot;
			rkPacket >> qtRot;
			pOwner->SetTargetRotToRotate( qtRot, true );

			ChangeToChargeAttack( pOwner );
		}
		break;
	case CS_PUSH_ATTACK:
		{
			D3DXVECTOR3 vPos;
			rkPacket >> vPos;
			pOwner->SetWorldPosition( vPos );

			D3DXQUATERNION qtRot;
			rkPacket >> qtRot;
			pOwner->SetTargetRotToRotate( qtRot, true );

			ChangeToSpecialAttack( pOwner, CS_NEPHILIM_PUSH_STATE, CS_PUSH_ATTACK, false );
		}
		break;
	case CS_SPECIAL:
		OnSpecialRecv( pOwner, rkPacket );
		break;
	case CS_CREATE_AREA:
		{
			D3DXVECTOR3 vPos;
			rkPacket >> vPos;

			D3DXQUATERNION qtRot;
			rkPacket >> qtRot;

			if ( m_AreaWeaponInfoList.empty() )
				return;

			for ( int i=0; i<(int)m_AreaWeaponInfoList.size(); i++ )
			{
				ioAreaWeapon *pAreaWeapon = g_AreaWeaponMgr.FindAreaWeaponByName( m_AreaWeaponInfoList[i].m_szAreaWeapon, pOwner->GetCharName() );
				if ( !pAreaWeapon )
					continue;

				g_AreaWeaponMgr.DestroyAreaWeapon( pAreaWeapon->GetAreaWeaponIndex() );
			}

			for ( int i=0; i<m_nMaxCreateAreaIdx; i++ )
			{
				if ( COMPARE( i, 0, (int)m_AreaWeaponInfoList.size() ) )
					g_AreaWeaponMgr.CreateAreaWeapon( pOwner->GetCharName(), "", "", m_AreaWeaponInfoList[i].m_szAreaWeapon, vPos, qtRot, ioAreaWeapon::CT_NORMAL );
			}
		}
		break;
	}	
}

void ioNephilimItem::OnSpecialRecv( ioBaseChar *pOwner, SP2Packet &rkPacket )
{
	if( !pOwner ) 
		return;

	int nState;
	rkPacket >> nState;

	switch( nState )
	{
	case SS_PUSH:
		{
			if ( m_PushAttackAttribute.m_vWeaponInfoList.empty() )
				return;

			DWORD dwWeaponBaseIdx;
			rkPacket >> dwWeaponBaseIdx;

			pOwner->SetWeaponIndexBase( dwWeaponBaseIdx );
			pOwner->IncreaseWeaponIndexBase();

			const WeaponInfo &sPushWeaponInfo = m_PushAttackAttribute.m_vWeaponInfoList[0];

			FireTime kFireTime;
			kFireTime.dwStart = FRAMEGETTIME();
			kFireTime.iAttributeIdx = sPushWeaponInfo.m_iWeaponIdx;
			kFireTime.iResistanceIdx = sPushWeaponInfo.m_iResistanceIdx;
			kFireTime.szWoundedAni  = sPushWeaponInfo.m_WoundedAnimation;
			kFireTime.dwWoundedAniDuration = sPushWeaponInfo.m_dwWoundedDuration;
			kFireTime.bLoopWoundedAni = sPushWeaponInfo.m_bWoundedLoopAni;
			kFireTime.eFireTimeType = FTT_NORMAL_ATTACK;

			int nFireCnt;
			rkPacket >> nFireCnt;

			for ( int i=0; i<nFireCnt; i++ )
			{
				D3DXVECTOR3 vPos;
				rkPacket >> vPos;

				kFireTime.dwWeaponIdx = dwWeaponBaseIdx++;
				pOwner->ExtendFireExplicit( kFireTime, vPos, ioMath::VEC3_ZERO, "" );
			}
		}
		break;
	}
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void ioNephilimItem::Init()
{
	m_ChargeState				= CS_NONE;
	m_SpecialState				= SS_NONE;

	m_dwAttackStartTime			= 0;
	m_nCurCombo					= 0;
	m_iCurAirJump				= 0;

	m_fAirJumpPowerRate			= 0.0f;
	m_dwMotionEndTime = 0;
	m_dwFireTime = 0;
}

void ioNephilimItem::OnCharging( ioBaseChar *pOwner )
{
	if( !pOwner || !pOwner->IsNeedProcess() )	
		return;

	bool bCheckBuff = pOwner->HasBuff( pOwner->GetCharName(), m_szSpecialCheckBuff );
	if( pOwner->IsAttackKeyDown() && !pOwner->IsHasCrown() )
	{
		if( m_dwAttackStartTime + m_dwChargeTime <= FRAMEGETTIME() )
		{
			if ( bCheckBuff )
				ChangeToSpecialAttack( pOwner, CS_NEPHILIM_PUSH_STATE, CS_PUSH_ATTACK, true );
			else
				ChangeToChargeAttack( pOwner );
		}
	}
	else
		ChangeToAttackFire( pOwner );
}

void ioNephilimItem::ChangeToAttackFire( ioBaseChar *pOwner )
{
	if( !pOwner ) 
		return;

	ClearCancelInfo();

	DWORD dwCurTime = FRAMEGETTIME();

	pOwner->InitExtendAttackTagInfo();
	pOwner->InitActionStopDelay( true );

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
		kPacket << CS_ATTACK_FIRE;
		kPacket << m_nCurCombo;
		kPacket << pOwner->GetWorldPosition();
		kPacket << pOwner->GetTargetRot();
		P2PNetwork::SendToAllPlayingUser( kPacket );
	}
}

void ioNephilimItem::ChangeToChargeAttack( ioBaseChar *pOwner )
{
	if( !pOwner ) 
		return;
	
	ChangeAniState( pOwner, m_ChargeAttackAttribute, true );

	m_ChargeState = CS_CHARGE_ATTACK;
	
	if( pOwner->IsNeedSendNetwork() )
	{
		SP2Packet kPacket( CUPK_ITEM_GAUGE_OK );
		FillItemGaugePacketHeader( pOwner, kPacket );
		kPacket << CS_CHARGE_ATTACK;
		kPacket << pOwner->GetWorldPosition();
		kPacket << pOwner->GetTargetRot();
		P2PNetwork::SendToAllPlayingUser( kPacket );
	}
}

bool ioNephilimItem::ChangeToChargeJump( ioBaseChar *pOwner )
{
	if( !pOwner )
		return false;
	
	ioPlayStage *pStage = pOwner->GetCreator();
	if ( !pStage )
		return false;

	for ( int i=0; i<(int)m_AreaWeaponInfoList.size(); i++ )
	{
		ioAreaWeapon *pAreaWeapon = g_AreaWeaponMgr.FindAreaWeaponByName( m_AreaWeaponInfoList[i].m_szAreaWeapon, pOwner->GetCharName() );
		if ( !pAreaWeapon )
			continue;

		if ( m_AreaWeaponInfoList[i].m_nTeleportWeaponNum == NO_WEAPON_ATTRIBUTE )
			continue;

		D3DXVECTOR3 vAreaPos = pAreaWeapon->GetStartPos();
		float fMapHeight = pStage->GetMapHeightNoneEntity( vAreaPos.x, vAreaPos.z );
		if ( vAreaPos.y < fMapHeight )
			vAreaPos.y = fMapHeight;
		vAreaPos.y += m_fTeleportHeightOffset;

		pOwner->SetWorldPosition( vAreaPos );

		CreateAreaAttachWeapon( pOwner, pAreaWeapon, m_AreaWeaponInfoList[i].m_nTeleportWeaponNum );

		ChangeToSpecialAttack( pOwner, CS_NEPHILIM_TELEPORT_STATE, CS_TELEPORT, false );
		m_SpecialState = SS_TELEPORT;
		return true;
	}
	
	return false;
}

void ioNephilimItem::ChangeToSpecialAttack( ioBaseChar *pOwner, int nCharState, ChargeState eChargeState, bool bSendNetwork )
{
	if ( !pOwner )
		return;

	Init();
	
	pOwner->SetState( nCharState );
	m_ChargeState = eChargeState;

	if( bSendNetwork && pOwner->IsNeedSendNetwork() )
	{
		SP2Packet kPacket( CUPK_ITEM_GAUGE_OK );
		FillItemGaugePacketHeader( pOwner, kPacket );
		kPacket << m_ChargeState;
		kPacket << pOwner->GetWorldPosition();
		kPacket << pOwner->GetTargetRot();
		P2PNetwork::SendToAllPlayingUser( kPacket );
	}
}

void ioNephilimItem::CreateAreaAttachWeapon( ioBaseChar *pOwner, ioAreaWeapon *pAreaWeapon, int nWeaponNum )
{
	if( !pOwner )
		return;

	if ( !pAreaWeapon || nWeaponNum == NO_WEAPON_ATTRIBUTE )
		return;

	FireTime kFireTime;
	kFireTime.dwStart = FRAMEGETTIME();
	kFireTime.iAttributeIdx = nWeaponNum;
	kFireTime.dwWeaponIdx = pOwner->IncreaseWeaponIndexBase();
	kFireTime.eFireTimeType = FTT_NORMAL_ATTACK;

	D3DXVECTOR3 vDir = pAreaWeapon->GetRotation() * D3DXVECTOR3( 0, 0, -1 );
	pOwner->ExtendFireExplicit( kFireTime, pAreaWeapon->GetStartPos(), vDir, "" );
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void ioNephilimItem::ChangeAniState( ioBaseChar *pOwner, const AttackAttribute &AttackInfo, bool bRefreshFire )
{
	if( !pOwner )
		return;

	ioEntityGroup *pGrp = pOwner->GetGroup();
	int iAniID = pGrp->GetAnimationIdx( AttackInfo.m_AttackAnimation );
	if ( iAniID == -1 )
		return;

	float fTimeRate  = AttackInfo.m_fAttackAniRate;
	DWORD dwPreDelay = AttackInfo.m_dwPreDelay;

	pGrp->ClearAllLoopAni( FLOAT100, true );
	pGrp->ClearAllActionAni( FLOAT100, true );
	pGrp->SetActionAni( iAniID, FLOAT100, FLOAT100, FLOAT1, FLOAT1/fTimeRate, 0.0f, dwPreDelay );	

	pOwner->ClearReservedSliding();
	pOwner->SetReservedSliding( AttackInfo.m_vForceInfoList, iAniID, fTimeRate, dwPreDelay );

	if ( bRefreshFire )
		pOwner->RefreshFireTimeList( iAniID, AttackInfo.m_vWeaponInfoList, FTT_NORMAL_ATTACK, fTimeRate, dwPreDelay );

	DWORD dwCurTime = FRAMEGETTIME();
	m_dwMotionEndTime = dwCurTime + pGrp->GetAnimationFullTime( iAniID ) * fTimeRate;
	m_dwFireTime = dwCurTime + pGrp->GetAnimationEventTime_e( iAniID, "fire_s" ) * fTimeRate;

	pOwner->SetCurJumpPowerAmt( 0.0f );
	pOwner->SetForcePowerAmt( 0.0f );
	pOwner->SetGravityAmt( 0.0f );	
}

void ioNephilimItem::ProcessSpecialState( ioBaseChar *pOwner )
{
	if ( !pOwner )
		return;
	
	DWORD dwCurTime = FRAMEGETTIME();

	switch( m_SpecialState )
	{
	case SS_NONE:
		{
			if ( m_ChargeState == CS_PUSH_ATTACK )
			{
				ChangeAniState( pOwner, m_PushAttackAttribute, false );
				m_SpecialState = SS_PUSH;
			}
			else if ( m_ChargeState == CS_BOMB_ATTACK )
			{
				ChangeAniState( pOwner, m_BombAttackAttribute, false );
				m_SpecialState = SS_BOMB;
				pOwner->SetJumpPower( m_fBombStartJumpPower );
			}
		}
		break;
	case SS_PUSH:
		{
			if ( m_dwMotionEndTime < FRAMEGETTIME() )
				pOwner->SetState( CS_DELAY );

			PushCreateWeapon( pOwner );
		}
		break;
	case SS_BOMB:
		{
			if ( m_dwMotionEndTime < FRAMEGETTIME() )
				pOwner->SetSKillEndJumpState( m_fBombEndJumpPower, false, false, true, true );

			if ( m_dwFireTime > 0 && m_dwFireTime < dwCurTime )
			{
				for ( int i=0; i<(int)m_AreaWeaponInfoList.size(); i++ )
				{
					ioAreaWeapon *pAreaWeapon = g_AreaWeaponMgr.FindAreaWeaponByName( m_AreaWeaponInfoList[i].m_szAreaWeapon, pOwner->GetCharName() );
					if ( !pAreaWeapon )
						continue;
					
					if ( m_dwFireTime > 0 && m_AreaWeaponInfoList[i].m_nBombWeaponNum != NO_WEAPON_ATTRIBUTE )
					{
						CreateAreaAttachWeapon( pOwner, pAreaWeapon, m_AreaWeaponInfoList[i].m_nBombWeaponNum );
						m_dwFireTime = 0;
					}

					g_AreaWeaponMgr.DestroyAreaWeapon( pAreaWeapon->GetAreaWeaponIndex() );
				}
			}
		}
		break;
	case SS_TELEPORT:
		{
			pOwner->SetCurJumpPowerAmt( 0.0f );
			pOwner->SetForcePowerAmt( 0.0f );
			pOwner->SetGravityAmt( 0.0f );

			pOwner->SetSKillEndJumpState( m_fTeleportEndJumpPower, false, false, true, true );
		}
		break;
	}
}

void ioNephilimItem::PushCreateWeapon( ioBaseChar *pOwner )
{
	if ( !pOwner || !pOwner->IsNeedProcess() )
		return;

	ioPlayStage *pPlayStage = pOwner->GetCreator();
	if ( !pPlayStage )
		return;
	
	DWORD dwCurTime = FRAMEGETTIME();
	if ( m_dwFireTime == 0 || m_dwFireTime > dwCurTime )
		return;

	if ( m_PushAttackAttribute.m_vWeaponInfoList.empty() )
		return;

	const WeaponInfo &sPushWeaponInfo = m_PushAttackAttribute.m_vWeaponInfoList[0];

	FireTime kFireTime;
	kFireTime.dwStart = dwCurTime;
	kFireTime.iAttributeIdx = sPushWeaponInfo.m_iWeaponIdx;
	kFireTime.iResistanceIdx = sPushWeaponInfo.m_iResistanceIdx;
	kFireTime.szWoundedAni  = sPushWeaponInfo.m_WoundedAnimation;
	kFireTime.dwWoundedAniDuration = sPushWeaponInfo.m_dwWoundedDuration;
	kFireTime.bLoopWoundedAni = sPushWeaponInfo.m_bWoundedLoopAni;
	kFireTime.eFireTimeType = FTT_NORMAL_ATTACK;

	DWORD dwWeaponBaseIdx = pOwner->IncreaseWeaponIndexBase();
	DWORD dwCreateWeaponIdx = dwWeaponBaseIdx;

	Vector3Vec vFirePos;
	const BaseCharList &rkCharList = pPlayStage->GetBaseCharList();
	BaseCharList::const_iterator iter;
	for( iter=rkCharList.begin() ; iter!=rkCharList.end() ; ++iter )
	{
		ioBaseChar *pTarget = *iter;
		if( !pTarget )	
			continue;

		if( pTarget == pOwner )
			continue;

		if ( !pTarget->HasBuff( pOwner->GetCharName(), m_szTargetCheckBuff ) )
			continue;

		kFireTime.dwWeaponIdx = dwCreateWeaponIdx++;

		D3DXVECTOR3 vPos = pTarget->GetMidPositionByRate();
		D3DXVECTOR3 vDir = ioMath::VEC3_ZERO;

		pOwner->ExtendFireExplicit( kFireTime, vPos, vDir, "" );
		vFirePos.push_back( vPos );
	}

	m_dwFireTime = 0;

	if( !vFirePos.empty() && pOwner->IsNeedSendNetwork() )
	{
		SP2Packet kPacket( CUPK_ITEM_GAUGE_OK );
		FillItemGaugePacketHeader( pOwner, kPacket );
		kPacket << CS_SPECIAL;
		kPacket << SS_PUSH;
		kPacket << dwWeaponBaseIdx;
		int nFireCnt = (int)vFirePos.size();
		kPacket << nFireCnt;
		for ( int i=0; i<nFireCnt; i++ )
			kPacket << vFirePos[i];
		P2PNetwork::SendToAllPlayingUser( kPacket );
	}
}

void ioNephilimItem::CreateAreaWeapon( ioBaseChar *pOwner, D3DXVECTOR3 vPos, D3DXQUATERNION qtRot )
{
	if ( !pOwner || !pOwner->IsNeedProcess() )
		return;
	
	ioPlayStage *pStage = pOwner->GetCreator();
	if ( !pStage )
		return;

	if ( m_AreaWeaponInfoList.empty() )
		return;

	for ( int i=0; i<(int)m_AreaWeaponInfoList.size(); i++ )
	{
		ioAreaWeapon *pAreaWeapon = g_AreaWeaponMgr.FindAreaWeaponByName( m_AreaWeaponInfoList[i].m_szAreaWeapon, pOwner->GetCharName() );
		if ( !pAreaWeapon )
			continue;

		g_AreaWeaponMgr.DestroyAreaWeapon( pAreaWeapon->GetAreaWeaponIndex() );
	}

	float fHeight = pStage->GetMapHeight( vPos.x, vPos.z );
	if ( fHeight > 0.0f )
	{
		vPos.y = fHeight;

		for ( int i=0; i<m_nMaxCreateAreaIdx; i++ )
		{
			if ( COMPARE( i, 0, (int)m_AreaWeaponInfoList.size() ) )
				g_AreaWeaponMgr.CreateAreaWeapon( pOwner->GetCharName(), "", "", m_AreaWeaponInfoList[i].m_szAreaWeapon, vPos, qtRot, ioAreaWeapon::CT_NORMAL );
		}

		if( pOwner->IsNeedSendNetwork() )
		{
			SP2Packet kPacket( CUPK_ITEM_GAUGE_OK );
			FillItemGaugePacketHeader( pOwner, kPacket );
			kPacket << CS_CREATE_AREA;
			kPacket << vPos;
			kPacket << qtRot;
			P2PNetwork::SendToAllPlayingUser( kPacket );
		}
	}
}

//-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

ioNephilimPushState::ioNephilimPushState()
{
}

ioNephilimPushState::~ioNephilimPushState()
{
}

void ioNephilimPushState::Initialize( ioBaseChar* pOwner, CharState eCurrState )
{
	ioSpecialStateBase::Initialize( pOwner, eCurrState );
}

void ioNephilimPushState::Finalize( ioBaseChar* pOwner, CharState eCurrState, CharState eNewState )
{
	ioSpecialStateBase::Finalize( pOwner, eCurrState, eNewState );
}

void ioNephilimPushState::CheckSpecialState( ioBaseChar* pOwner )
{
	ioNephilimItem *pNephilimItem = ToNephilimItem( pOwner->GetWeapon() );
	if( pNephilimItem )
		pNephilimItem->ProcessSpecialState( pOwner );
	else
	{
		pOwner->SetState( CS_DELAY );
		pOwner->SetFallState( true );
		return;
	}
}

//-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

ioNephilimBombState::ioNephilimBombState()
{
}

ioNephilimBombState::~ioNephilimBombState()
{
}

void ioNephilimBombState::Initialize( ioBaseChar* pOwner, CharState eCurrState )
{
	ioSpecialStateBase::Initialize( pOwner, eCurrState );
}

void ioNephilimBombState::Finalize( ioBaseChar* pOwner, CharState eCurrState, CharState eNewState )
{
	ioSpecialStateBase::Finalize( pOwner, eCurrState, eNewState );
}

void ioNephilimBombState::CheckSpecialState( ioBaseChar* pOwner )
{
	ioNephilimItem *pNephilimItem = ToNephilimItem( pOwner->GetWeapon() );
	if( pNephilimItem )
		pNephilimItem->ProcessSpecialState( pOwner );
	else
	{
		pOwner->SetState( CS_DELAY );
		pOwner->SetFallState( true );
		return;
	}
}

void ioNephilimBombState::CheckFallStateChange( ioBaseChar* pOwner, float fMinFallHeightRate, float fBottomHeight, bool bBottomMaxCheck, float fMaxBottomHeight )
{
	return;
}

//-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

ioNephilimTeleportState::ioNephilimTeleportState()
{
}

ioNephilimTeleportState::~ioNephilimTeleportState()
{
}

void ioNephilimTeleportState::Initialize( ioBaseChar* pOwner, CharState eCurrState )
{
	ioSpecialStateBase::Initialize( pOwner, eCurrState );
}

void ioNephilimTeleportState::Finalize( ioBaseChar* pOwner, CharState eCurrState, CharState eNewState )
{
	ioSpecialStateBase::Finalize( pOwner, eCurrState, eNewState );
}

void ioNephilimTeleportState::CheckSpecialState( ioBaseChar* pOwner )
{
	ioNephilimItem *pNephilimItem = ToNephilimItem( pOwner->GetWeapon() );
	if( pNephilimItem )
		pNephilimItem->ProcessSpecialState( pOwner );
	else
	{
		pOwner->SetState( CS_DELAY );
		pOwner->SetFallState( true );
		return;
	}
}

void ioNephilimTeleportState::CheckFallStateChange( ioBaseChar* pOwner, float fMinFallHeightRate, float fBottomHeight, bool bBottomMaxCheck, float fMaxBottomHeight )
{
	return;
}

bool ioNephilimTeleportState::ProcessMove( ioBaseChar* pOwner, float fTimePerSec, DWORD dwPreTime, DWORD dwCurTime )
{
	return true;
}