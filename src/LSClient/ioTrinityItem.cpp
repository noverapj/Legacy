
#include "stdafx.h"

#include "ioPlayStage.h"
#include "ioBaseChar.h"

#include "ioTrinityItem.h"

#include "WeaponDefine.h"

ioTrinityItem::ioTrinityItem()
{
	m_ChargeState = CS_NONE;

	m_dwAttackStartTime = 0;
	m_iCurCombo = 0;

	m_fCurChargeRate = 0.0f;
	m_fCurChargeRateForSkill = 0.0f;

	m_fCurGauge = 0.0f;
	m_nWeaponIndex = 0;

	m_State = TS_NONE;
}

ioTrinityItem::ioTrinityItem( const ioTrinityItem &rhs )
	: ioWeaponItem( rhs ),
	m_AttackReadyAniList( rhs.m_AttackReadyAniList ),
	m_AttackInfo( rhs.m_AttackInfo ),
	m_fMaxGauge( rhs.m_fMaxGauge ),
	m_GatherAnimation( rhs.m_GatherAnimation ),
	m_fGatherAniRate( rhs.m_fGatherAniRate ),
	m_fIncreaseGauge( rhs.m_fIncreaseGauge ),
	m_nMaxWeaponCnt( rhs.m_nMaxWeaponCnt )
{
	m_ChargeState = CS_NONE;

	m_dwAttackStartTime = 0;
	m_iCurCombo = 0;

	m_fCurChargeRate = 0.0f;
	m_fCurChargeRateForSkill = 0.0f;

	m_fCurGauge = m_fMaxGauge;
	m_nWeaponIndex = 0;
	m_dwGatheringStartTime = 0;
	m_dwEffect = 0;

	m_State = TS_NONE;

	m_bVisibleMesh = false;
}

ioTrinityItem::~ioTrinityItem()
{
}

void ioTrinityItem::LoadProperty( ioINILoader &rkLoader )
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

	LoadTrinityInfo( rkLoader );

	m_fMaxGauge			= rkLoader.LoadFloat_e( "max_gauge", FLOAT100 );
	m_fIncreaseGauge    = rkLoader.LoadFloat_e( "increase_gauge", FLOAT1 );
	m_fAttackSpeedRate	= rkLoader.LoadFloat_e( "attack_max_speed_rate", FLOAT1 );
}

void ioTrinityItem::LoadTrinityInfo( ioINILoader &rkLoader )
{
	char szBuf[MAX_PATH];
	char szKey[MAX_PATH];
	m_AttackInfo.Init();

	rkLoader.LoadString_e( "gathering_animation", "", szBuf, MAX_PATH );
	m_GatherAnimation = szBuf;
	m_fGatherAniRate = rkLoader.LoadFloat_e( "gathering_ani_rate", FLOAT1 );


	m_nMaxWeaponCnt = rkLoader.LoadInt_e( "max_weapon_cnt", 0 );
	m_nMaxWeaponCnt = max( 1, m_nMaxWeaponCnt );
	for( int i = 0; i < m_nMaxWeaponCnt; i++ )
	{
		//D~
		wsprintf_e( szBuf, "attack_atribute_full_%d", i+1 );
		LoadAttackAttribute( szBuf, m_AttackInfo.m_FullAttack[i], rkLoader );

		//~D
		wsprintf_e( szBuf, "attack_atribute_none_%d", i+1 );
		LoadAttackAttribute( szBuf, m_AttackInfo.m_NoneFullAttack[i], rkLoader );

		//메쉬
		//일반
		wsprintf_e( szKey, "add_normal_mesh_cnt_%d", i+1 );
		int nAddNormalMeshCnt = rkLoader.LoadInt( szKey, 0 );
		for( int i2 = 0; i2 < nAddNormalMeshCnt; i2++ )
		{
			wsprintf_e( szKey, "add_normal_mesh_%d_%d", i+1, i2+1 );
			rkLoader.LoadString( szKey, "", szBuf, MAX_PATH );
			m_AttackInfo.m_AddNormalMesh[i].push_back( szBuf );
		}

		//D~
		wsprintf_e( szKey, "add_full_mesh_cnt_%d", i+1 );
		int nAddFullMeshCnt = rkLoader.LoadInt( szKey, 0 );
		for( int i2 = 0; i2 < nAddFullMeshCnt; i2++ )
		{
			wsprintf_e( szKey, "add_full_mesh_%d_%d", i+1, i2+1 );
			rkLoader.LoadString( szKey, "", szBuf, MAX_PATH );
			m_AttackInfo.m_AddFullMesh[i].push_back( szBuf );
		}

		//~D
		wsprintf_e( szKey, "add_none_mesh_cnt_%d", i+1 );
		int nAddNoneMeshCnt = rkLoader.LoadInt( szKey, 0 );
		for( int i2 = 0; i2 < nAddNoneMeshCnt; i2++ )
		{
			wsprintf_e( szKey, "add_none_mesh_%d_%d", i+1, i2+1 );
			rkLoader.LoadString( szKey, "", szBuf, MAX_PATH );
			m_AttackInfo.m_AddNoneMesh[i].push_back( szBuf );
		}

		//~D(점프)
		wsprintf_e( szKey, "jump_add_none_mesh_cnt_%d", i+1 );
		int nAddNoneMeshCntForJump = rkLoader.LoadInt( szKey, 0 );
		for( int i2 = 0; i2 < nAddNoneMeshCntForJump; i2++ )
		{
			wsprintf_e( szKey, "jump_add_none_mesh_%d_%d", i+1, i2+1 );
			rkLoader.LoadString( szKey, "", szBuf, MAX_PATH );
			m_AttackInfo.m_AddNoneMeshForJump[i].push_back( szBuf );
		}

		//이펙트
		//일반
		wsprintf_e( szKey, "add_normal_effect_%d", i+1 );
		rkLoader.LoadString( szKey, "", szBuf, MAX_PATH );
		m_AttackInfo.m_AddNormalEffect[i]= szBuf;
		

		//D~
		wsprintf_e( szKey, "add_full_effect_%d", i+1 );
		rkLoader.LoadString( szKey, "", szBuf, MAX_PATH );
		m_AttackInfo.m_AddFullEffect[i] = szBuf;
		

		//~D
		wsprintf_e( szKey, "add_none_effect_%d_%d", i+1 );
		rkLoader.LoadString( szKey, "", szBuf, MAX_PATH );
		m_AttackInfo.m_AddNoneEffect[i] = szBuf;

		//~D(점프)
		wsprintf_e( szKey, "jump_add_none_effect_%d_%d", i+1 );
		rkLoader.LoadString( szKey, "", szBuf, MAX_PATH );
		m_AttackInfo.m_AddNoneEffectForJump[i] = szBuf;
	}

	rkLoader.LoadString_e( "reload_animation", "", szBuf, MAX_PATH );
	m_AttackInfo.m_ReloadAni = szBuf;
	m_AttackInfo.m_fReloadAniRate = rkLoader.LoadFloat_e( "reload_time_rate", FLOAT1 );
}

ioItem* ioTrinityItem::Clone()
{
	return new ioTrinityItem( *this );
}

ioWeaponItem::WeaponSubType ioTrinityItem::GetSubType() const
{
	return WST_TRINITY;
}

void ioTrinityItem::SetNormalAttack( ioBaseChar *pOwner, int iCurCombo )
{
	if( !pOwner ) 
		return;

	m_fCurChargeRate = 0.0f;
	m_fCurChargeRateForSkill = 0.0f;

	m_dwKeyReserveTime = 0;

	m_iSkillInput = -1;
	m_bEnableDefenseState = false;
	m_bEnableAttackState = false;
	m_bEnableJumpState = false;

	//ClearState( pOwner );

	int iComboCnt = GetMaxCombo();
	if( !COMPARE( iCurCombo, 0, iComboCnt ) )
	{
		ioWeaponItem::SetNormalAttack( pOwner, iCurCombo );

		m_ChargeState = CS_NONE;
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

		m_ChargeState = CS_CHARGING;
		m_dwAttackStartTime = FRAMEGETTIME();
		m_iCurCombo = iCurCombo;
		pOwner->SetState( CS_ATTACK );
		pOwner->SetAttackMoveEnable( false );
		pOwner->SetChargingState( true );

		/*if( m_bChargeAutoTarget )
		{
			DWORD dwTrackingTime = FRAMEGETTIME();
			dwTrackingTime += pGrp->GetAnimationEventTime_e( iAniID, "tracking_e" ) * fTimeRate;
			pOwner->SetAutoTargetTrackingEndTime( dwTrackingTime );
		}
		else*/
		{
			pOwner->StopAutoTargetTracking();
		}
	}
}

void ioTrinityItem::CheckNormalAttack( ioBaseChar *pOwner )
{
	switch( m_ChargeState )
	{
	case CS_CHARGING:
		OnCharging( pOwner );
		break;
	case CS_NORMAL_ATTACK:
		ioWeaponItem::CheckNormalAttack( pOwner );
		break;
	}
}

void ioTrinityItem::OnCharging( ioBaseChar *pOwner )
{
	if( !pOwner ) return;
	if( !pOwner->IsNeedProcess() )	return;

	if( pOwner->IsAttackKeyDown() )//&& !pOwner->IsHasCrown() )
	{
		if( m_dwAttackStartTime + m_dwChargeTime <= FRAMEGETTIME() )
		{
			SetTrinityState( pOwner, m_AttackInfo, true );
			return;
		}
	}
	else
	{
		ChangeToNormalAttack( pOwner );
	}
}

void ioTrinityItem::SetTrinityState( ioBaseChar *pOwner, const TrinityInfo &rkInfo, bool bSendNet )
{
	if( !pOwner )
		return;

	ioEntityGroup *pGrp = pOwner->GetGroup();
	if( !pGrp )
		return;

	if( pOwner->IsNeedProcess() )
	{
		D3DXQUATERNION qtRot = pOwner->GetTargetRot();
		pOwner->SetTargetRotToRotate( qtRot, true );
	}

	pOwner->SetState( CS_TRINITY );

	//m_CurInfo = rkInfo;

	ChangeGatheringState( pOwner );

	if( bSendNet && pOwner->IsNeedSendNetwork() )
	{
		SP2Packet kPacket( CUPK_ITEM_GAUGE_CANCEL );
		FillItemGaugePacketHeader( pOwner, kPacket );
		kPacket << SST_CHANGE_STATE;
		kPacket << m_State;
		kPacket << pOwner->GetTargetRot();
		kPacket << m_fCurGauge;
		kPacket << m_nWeaponIndex;
		P2PNetwork::SendToAllPlayingUser( kPacket );
	}
}

void ioTrinityItem::ChangeGatheringState(  ioBaseChar *pOwner )
{
	if( !pOwner )
		return;

	ioEntityGroup *pGrp = pOwner->GetGroup();
	if( !pGrp )
		return;

	DWORD dwCurTime = FRAMEGETTIME();

	pOwner->InitExtendAttackTagInfo();

	pOwner->ClearReservedSliding();
	pOwner->SetForcePowerAmt( 0.0f );

	pGrp->ClearAllLoopAni( FLOAT100, true );
	pGrp->ClearAllActionAni( FLOAT100, true );

	int iAniID	     = pGrp->GetAnimationIdx( m_GatherAnimation );
	float fTimeRate  = m_fGatherAniRate;

	pGrp->SetActionAni( iAniID, FLOAT100, FLOAT100, FLOAT1, FLOAT1/fTimeRate, 0.0f, 0.0f, true );

	m_State = TS_GATHERING;
	m_dwMotionEndTime = dwCurTime + pGrp->GetAnimationFullTime( iAniID ) * fTimeRate;

	if( pOwner->IsNeedProcess() )
	{
		SP2Packet kPacket( CUPK_ITEM_GAUGE_CANCEL );
		FillItemGaugePacketHeader( pOwner, kPacket );
		kPacket << SST_GATHERING;
		kPacket << m_State;
		kPacket << pOwner->GetTargetRot();
		kPacket << m_fCurGauge;
		kPacket << m_nWeaponIndex;
		P2PNetwork::SendToAllPlayingUser( kPacket );
	}
}

void ioTrinityItem::ChangeToNormalAttack( ioBaseChar *pOwner )
{
	DWORD dwCurTime = FRAMEGETTIME();

	//
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
	}

	pOwner->ApplyNormalAttackResult( m_iCurCombo );

	m_ChargeState = CS_NORMAL_ATTACK;

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

void ioTrinityItem::ChangeFullChargeAttack( ioBaseChar *pOwner )
{
	if( !pOwner ) return;

	DWORD dwCurTime = FRAMEGETTIME();

	m_State = TS_SPECIAL_FULL_CHARGE;

	//pOwner->SetAutoTarget( ATT_NORMAL );

	ioEntityGroup *pGrp = pOwner->GetGroup();
	if( !pGrp )
		return;

	/*pOwner->InitExtendAttackTagInfo();

	pGrp->ClearAllLoopAni( FLOAT10, true );
	pGrp->ClearAllActionAni( FLOAT10, true );*/

	int iAniID	     = pGrp->GetAnimationIdx(m_AttackInfo.m_FullAttack[m_nWeaponIndex].m_AttackAnimation);
	float fTimeRate  = m_AttackInfo.m_FullAttack[m_nWeaponIndex].m_fAttackAniRate;
	
	DWORD dwPreDelay = m_AttackInfo.m_FullAttack[m_nWeaponIndex].m_dwPreDelay;
	DWORD dwEndDelay = m_AttackInfo.m_FullAttack[m_nWeaponIndex].m_dwEndDelay;

	/*pOwner->RefreshFireTimeList( iAniID, m_CurInfo.m_FullAttack[m_nWeaponIndex].m_vWeaponInfoList, FTT_NORMAL_ATTACK, fTimeRate, dwPreDelay );
	pOwner->IncreaseWeaponIndexBase();

	pGrp->SetActionAni( iAniID, FLOAT100, FLOAT100, FLOAT1, FLOAT1/fTimeRate, 0.0f, 0.0f, true );*/

	pOwner->SetAutoTarget( ATT_NORMAL );

	pOwner->SetNormalAttackByAttribute( m_AttackInfo.m_FullAttack[m_nWeaponIndex] );

	EraseEntityAndEffect(pOwner);
	AddEntityAndEffect( pOwner );


	m_dwMotionEndTime = dwCurTime + pGrp->GetAnimationFullTime( iAniID ) * fTimeRate  + dwPreDelay;


	if( pOwner->IsNeedProcess() )
	{
		m_fCurGauge = 0;

		SP2Packet kPacket( CUPK_ITEM_GAUGE_CANCEL );
		FillItemGaugePacketHeader( pOwner, kPacket );
		kPacket << SST_CHARGE_FIRE;
		kPacket << m_State;
		kPacket << pOwner->GetTargetRot();
		kPacket << m_fCurGauge;
		kPacket << m_nWeaponIndex;
		P2PNetwork::SendToAllPlayingUser( kPacket );
	}
}

void ioTrinityItem::ChangeNoneChargeAttack( ioBaseChar *pOwner )
{
	if( !pOwner ) return;

	DWORD dwCurTime = FRAMEGETTIME();

	m_State = TS_SPECIAL_NONE_CHARGE;

	//pOwner->SetAutoTarget( ATT_NORMAL );

	ioEntityGroup *pGrp = pOwner->GetGroup();
	if( !pGrp )
		return;

	/*pOwner->InitExtendAttackTagInfo();

	pGrp->ClearAllLoopAni( FLOAT10, true );
	pGrp->ClearAllActionAni( FLOAT10, true );*/

	int iAniID	     = pGrp->GetAnimationIdx(m_AttackInfo.m_NoneFullAttack[m_nWeaponIndex].m_AttackAnimation);
	float fTimeRate  = m_AttackInfo.m_NoneFullAttack[m_nWeaponIndex].m_fAttackAniRate;
	DWORD dwPreDelay = m_AttackInfo.m_NoneFullAttack[m_nWeaponIndex].m_dwPreDelay;
	DWORD dwEndDelay = m_AttackInfo.m_NoneFullAttack[m_nWeaponIndex].m_dwEndDelay;

	/*pOwner->RefreshFireTimeList( iAniID, m_CurInfo.m_NoneFullAttack[m_nWeaponIndex].m_vWeaponInfoList, FTT_NORMAL_ATTACK, fTimeRate, dwPreDelay );
	pOwner->IncreaseWeaponIndexBase();*

	pGrp->SetActionAni( iAniID, FLOAT100, FLOAT100, FLOAT1, FLOAT1/fTimeRate, 0.0f, 0.0f, true );*/

	pOwner->SetAutoTarget( ATT_NORMAL );

	pOwner->SetNormalAttackByAttribute( m_AttackInfo.m_NoneFullAttack[m_nWeaponIndex] );

	m_dwMotionEndTime = dwCurTime + pGrp->GetAnimationFullTime( iAniID ) * fTimeRate  + dwPreDelay;

	EraseEntityAndEffect(pOwner);
	AddEntityAndEffect( pOwner );


	if( pOwner->IsNeedProcess() )
	{
		DecreaseGauge();

		SP2Packet kPacket( CUPK_ITEM_GAUGE_CANCEL );
		FillItemGaugePacketHeader( pOwner, kPacket );
		kPacket << SST_NONE_FIRE;
		kPacket << m_State;
		kPacket << pOwner->GetTargetRot();
		kPacket << m_fCurGauge;
		kPacket << m_nWeaponIndex;
		P2PNetwork::SendToAllPlayingUser( kPacket );
	}
}

void ioTrinityItem::ChangeReloadState( ioBaseChar *pOwner )
{
	if( !pOwner ) return;

	DWORD dwCurTime = FRAMEGETTIME();

	m_State = TS_RELOAD;

	ioEntityGroup *pGrp = pOwner->GetGroup();
	if( !pGrp )
		return;

	pOwner->SetAttackMoveEnable( true );
	pOwner->SetChargingState( true );

	pOwner->InitExtendAttackTagInfo();

	pGrp->ClearAllLoopAni( FLOAT10, true );
	pGrp->ClearAllActionAni( FLOAT10, true );

	int iAniID	     = pGrp->GetAnimationIdx(m_AttackInfo.m_ReloadAni );
	float fTimeRate  = m_AttackInfo.m_fReloadAniRate;

	pGrp->SetActionAni( iAniID, FLOAT100, FLOAT100, FLOAT1, FLOAT1/fTimeRate, 0.0f, 0.0f, true );

	m_dwMotionEndTime = dwCurTime + pGrp->GetAnimationFullTime( iAniID ) * fTimeRate;

	if( pOwner->IsNeedProcess() )
	{
		SP2Packet kPacket( CUPK_ITEM_GAUGE_CANCEL );
		FillItemGaugePacketHeader( pOwner, kPacket );
		kPacket << SST_RELOAD;
		kPacket << m_State;
		kPacket << pOwner->GetTargetRot();
		kPacket << m_fCurGauge;
		kPacket << m_nWeaponIndex;
		P2PNetwork::SendToAllPlayingUser( kPacket );
	}
}

void ioTrinityItem::ChangeEndState( ioBaseChar *pOwner )
{
	ClearState(pOwner);

	if( pOwner->IsNeedProcess() )
	{
		SP2Packet kPacket( CUPK_ITEM_GAUGE_CANCEL );
		FillItemGaugePacketHeader( pOwner, kPacket );
		kPacket << SST_END;
		kPacket << m_State;
		kPacket << pOwner->GetTargetRot();
		kPacket << m_fCurGauge;
		kPacket << m_nWeaponIndex;
		P2PNetwork::SendToAllPlayingUser( kPacket );
	}

	pOwner->SetState( CS_DELAY );
}

void ioTrinityItem::ChangeReloadedState( ioBaseChar *pOwner )
{
	ClearState(pOwner);
	
	EraseEntityAndEffect( pOwner );

	if( pOwner->IsNeedProcess() )
		NextWeapon();

	AddNormalEntityAndEffect(pOwner);

	if( pOwner->IsNeedProcess() )
	{
		SP2Packet kPacket( CUPK_ITEM_GAUGE_CANCEL );
		FillItemGaugePacketHeader( pOwner, kPacket );
		kPacket << SST_RELOADED;
		kPacket << m_State;
		kPacket << pOwner->GetTargetRot();
		kPacket << m_fCurGauge;
		kPacket << m_nWeaponIndex;
		P2PNetwork::SendToAllPlayingUser( kPacket );
	}

	pOwner->SetState( CS_DELAY );
}

void ioTrinityItem::OnItemGaugeOK( ioBaseChar *pOwner, SP2Packet &rkPacket )
{
	int iState;
	rkPacket >> iState;
	rkPacket >> m_iCurCombo;

	switch( iState )
	{
	case CS_NONE:
		pOwner->SetAttackMoveEnable( false );
		pOwner->SetChargingState( false );

		m_ChargeState = CS_NONE;
		break;
	case CS_NORMAL_ATTACK:
		if( !m_bChargeAutoTarget )
			pOwner->ApplyAutoTargetInfo( rkPacket );
		ChangeToNormalAttack( pOwner );
		break;
	}
}

void ioTrinityItem::OnItemGaugeCancel( ioBaseChar *pOwner, SP2Packet &rkPacket )
{
	int iSyncType;
	rkPacket >> iSyncType;

	switch( iSyncType )
	{
	case SST_CHANGE_STATE:
		{
			CheckState( pOwner, rkPacket );
		}
		break;
	case SST_GATHERING:
		CheckNetWork( pOwner, rkPacket );
		break;
	case SST_NONE_FIRE:
		CheckNetWork( pOwner, rkPacket );
		break;
	case SST_CHARGE_FIRE:
		CheckNetWork( pOwner, rkPacket );
		break;
	case SST_RELOAD:
		CheckNetWork( pOwner, rkPacket );
		break;
	case SST_RELOADED:
		{

			int nState;
			rkPacket >> nState;
			m_State = (TrinityState)nState;
			D3DXQUATERNION qtRot;
			rkPacket >> qtRot;
			pOwner->SetTargetRot( qtRot );
			rkPacket >> m_fCurGauge;
			rkPacket >> m_nWeaponIndex;

			ChangeReloadedState( pOwner );
		}
		break;
	case SST_END:
		ChangeEndState(pOwner);
		break;
	default: break;
	}
}

void ioTrinityItem::CheckNetWork( ioBaseChar *pOwner, SP2Packet &rkPacket )
{
	int nState;
	rkPacket >> nState;
	m_State = (TrinityState)nState;
	D3DXQUATERNION qtRot;
	rkPacket >> qtRot;
	pOwner->SetTargetRot( qtRot );
	rkPacket >> m_fCurGauge;
	rkPacket >> m_nWeaponIndex;

	if( m_State == TS_GATHERING )
		ChangeGatheringState(pOwner);
	else if( m_State == TS_SPECIAL_NONE_CHARGE )
		ChangeNoneChargeAttack(pOwner);
	else if( m_State == TS_SPECIAL_FULL_CHARGE )
		ChangeFullChargeAttack(pOwner);
	else if( m_State == TS_RELOAD )
		ChangeReloadState(pOwner);
}

void ioTrinityItem::SetExtraInfoToWeapon( ioWeapon *pWeapon, bool bJumpAttack )
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

bool ioTrinityItem::IsChargeAttackState( ioBaseChar *pOwner )
{
	if( pOwner->GetState() == CS_ATTACK )
	{
		switch( m_ChargeState )
		{
		case CS_CHARGING:
			return true;
		}
	}

	return false;
}

void ioTrinityItem::OnEndExtendAttack( ioBaseChar *pOwner, bool bReleaseItem )
{
	m_ChargeState = CS_NONE;

	pOwner->SetAttackMoveEnable( false );
	pOwner->SetChargingState( false );

	pOwner->ClearDirDoubleClick();

	m_fCurChargeRate = 0.0f;
}

bool ioTrinityItem::IsChargeAutoTarget( ioBaseChar *pOwner )
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

void ioTrinityItem::GetAutoTargetValue( ioBaseChar *pOwner,
	float &fRange, float &fMinAngle, float &fMaxAngle,
	AutoTargetType eType )
{
	if( IsChargeAutoTarget(pOwner) )
	{
		float fCurTargetRangeRate, fCurTargetAngleRate;
		fCurTargetRangeRate = fCurTargetAngleRate = FLOAT1;

		float fCurMaxTargetRange = max( 0.0f, (m_fMaxTargetRangeRate - FLOAT1) );
		float fCurMaxTargetAngle = max( 0.0f, (m_fMaxTargetAngleRate - FLOAT1) );

		fCurTargetRangeRate += fCurMaxTargetRange * m_fCurChargeRate;
		fCurTargetAngleRate += fCurMaxTargetAngle * m_fCurChargeRate;

		fRange = m_fAutoTargetRange * fCurTargetRangeRate;
		fMinAngle = m_fAutoTargetMinAngle / fCurTargetAngleRate;
		fMaxAngle = m_fAutoTargetMaxAngle / fCurTargetAngleRate;
	}
	else
	{
		switch( eType )
		{
		case ATT_NORMAL:
			fRange = m_fAutoTargetRange;
			fMinAngle = m_fAutoTargetMinAngle;
			fMaxAngle = m_fAutoTargetMaxAngle;
			return;
		case ATT_DASH:
			if( m_fDashAutoTargetRange == 0.0f )
				break;

			fRange = m_fDashAutoTargetRange;
			fMinAngle = m_fDashAutoTargetMinAngle;
			fMaxAngle = m_fDashAutoTargetMaxAngle;
			return;
		case ATT_JUMP:
			if( m_fJumpAutoTargetRange == 0.0f )
				break;

			fRange = m_fJumpAutoTargetRange;
			fMinAngle = m_fJumpAutoTargetMinAngle;
			fMaxAngle = m_fJumpAutoTargetMaxAngle;
			return;
		case ATT_COUNTER:
			if( m_fCountAutoTargetRange == 0.0f )
				break;

			fRange = m_fCountAutoTargetRange;
			fMinAngle = m_fCountAutoTargetMinAngle;
			fMaxAngle = m_fCountAutoTargetMaxAngle;
			return;
		}


		fRange = m_fAutoTargetRange;
		fMinAngle = m_fAutoTargetMinAngle;
		fMaxAngle = m_fAutoTargetMaxAngle;

		return;
	}
}

void ioTrinityItem::ProcessTrinityState( ioBaseChar *pOwner )
{
	if( !pOwner )
		return;

	DWORD dwCurTime = FRAMEGETTIME();

	switch( m_State )
	{
	case TS_GATHERING:
		if( pOwner->IsNeedProcess() )
		{
			if( m_fCurGauge < FLOAT1 )
			{
				ChangeReloadState(pOwner);
				return;
			}

			if( pOwner->IsAttackKeyDown() )
			{
				if(	dwCurTime > m_dwMotionEndTime )
				{
					ChangeFullChargeAttack( pOwner );
				}
			}
			else
			{
				ChangeNoneChargeAttack( pOwner );
			}
		}

		break;
	case TS_SPECIAL_FULL_CHARGE:
		if( pOwner->IsNeedProcess() && dwCurTime > m_dwMotionEndTime )
		{
			ChangeEndState( pOwner );
			//AddNormalEntityAndEffect(pOwner);
		}
		break;
	case TS_SPECIAL_NONE_CHARGE:
		if( pOwner->IsNeedProcess() && dwCurTime > m_dwMotionEndTime )
		{
			ChangeEndState( pOwner );
			//AddNormalEntityAndEffect(pOwner);
		}
		break;
	case TS_RELOAD:
		if( pOwner->IsNeedProcess() && dwCurTime > m_dwMotionEndTime )
		{
			ChangeReloadedState( pOwner );
		}
		break;
	}
}

void ioTrinityItem::CheckState( ioBaseChar *pOwner, SP2Packet &rkPacket )
{
	if( !pOwner )
		return;

	int nState;
	rkPacket >> nState;

	D3DXQUATERNION qtRot;
	rkPacket >> qtRot;
	rkPacket >> m_fCurGauge;
	rkPacket >> m_nWeaponIndex;

	switch( nState )
	{
	case TS_GATHERING:
		SetTrinityState( pOwner, m_AttackInfo, false );
		break;
	default: break;
	}
}

int ioTrinityItem::GetMaxBullet()
{
	return (int)m_fMaxGauge;
}

int ioTrinityItem::GetCurBullet()
{
	float fCurCoolTime = m_fCurGauge;
	fCurCoolTime = max( 0, min( fCurCoolTime, m_fMaxGauge ) );

	return (int)fCurCoolTime;
}

void ioTrinityItem::SetCurBullet( int iCurBullet )
{
	if( iCurBullet < 0 )
		return;

	float fCurBullet = iCurBullet;
	fCurBullet = max( 0, min( fCurBullet, m_fMaxGauge ) );

	m_fCurGauge = fCurBullet;
}

void ioTrinityItem::InitGauge()
{
	m_fCurGauge = 0.0f;
}

void ioTrinityItem::MaxGauge()
{
	m_fCurGauge = m_fMaxGauge;
}

float ioTrinityItem::GetCurGauge()
{
	return m_fCurGauge;
}

void ioTrinityItem::SetCurGauge( float fGauge )
{
	m_fCurGauge = fGauge;
	m_fCurGauge = (float)max( 0, min( m_fCurGauge, m_fMaxGauge ) );
}

void ioTrinityItem::DecreaseGauge()
{
	m_fCurGauge = max( 0.f, m_fCurGauge - FLOAT1 );
}

bool ioTrinityItem::IsTrinityState()
{
	/*switch( m_State )
	{
	case CFS_DELAY:
		return true;
	}*/

	return false;
}

void ioTrinityItem::UpdateExtraData( ioBaseChar *pOwner )
{
	if( !pOwner )
		return;

	// check owner state
	float fTimePerSec = g_FrameTimer.GetSecPerFrame();

	if( pOwner->IsSystemState() )
		return;

	switch( pOwner->GetState() )
	{
	case CS_TRINITY:
		return;
	default:
		if( m_fCurGauge < m_fMaxGauge )
		{
			float fGauge = m_fIncreaseGauge * fTimePerSec;

			m_fCurGauge += fGauge;
			m_fCurGauge = min( m_fCurGauge, m_fMaxGauge );
		}
		return;
	}
}

void ioTrinityItem::ClearState( ioBaseChar *pOwner )
{
	m_State = TS_NONE;
	EraseEntityAndEffect(pOwner);
	AddNormalEntityAndEffect(pOwner);
}

void ioTrinityItem::OnEquiped( ioBaseChar *pOwner )
{
	ioWeaponItem::OnEquiped( pOwner );

	AddNormalEntityAndEffect(pOwner);
}

void ioTrinityItem::OnReleased( ioBaseChar *pOwner )
{
	//EraseEntityAndEffect( pOwner );

	ClearState( pOwner );

	EraseEntityAndEffect( pOwner );

	ioAttackableItem::OnReleased( pOwner );
}

float ioTrinityItem::GetAttackSpeedRate() const
{
	if( m_State != TS_NONE )
		return m_fAttackSpeedRate;

	return FLOAT1;
}

void ioTrinityItem::AddEntityAndEffect( ioBaseChar *pOwner, bool bJumpAttack )
{
	if( bJumpAttack == false )
	{
		// add mesh
		if( m_State == TS_SPECIAL_FULL_CHARGE )
		{
			int iMeshCnt = m_AttackInfo.m_AddFullMesh[m_nWeaponIndex].size();
			for( int i=0; i < iMeshCnt; ++i )
			{
				ioHashString szMeshName = (m_AttackInfo.m_AddFullMesh[m_nWeaponIndex])[i];
				pOwner->AddEntity( szMeshName );

				m_bVisibleMesh = true;
			}

			ioHashString szEffectName = m_AttackInfo.m_AddFullEffect[m_nWeaponIndex];
			if( !szEffectName.IsEmpty() )
			{
				ioEffect *pEffect = pOwner->AttachEffect(szEffectName);
				if( pEffect )
				{
					m_dwEffect = pEffect->GetUniqueID();
				}
			}
		}

		if( m_State == TS_SPECIAL_NONE_CHARGE )
		{
			int iMeshCnt = m_AttackInfo.m_AddNoneMesh[m_nWeaponIndex].size();
			for( int i=0; i < iMeshCnt; ++i )
			{
				ioHashString szMeshName = (m_AttackInfo.m_AddNoneMesh[m_nWeaponIndex])[i];
				pOwner->AddEntity( szMeshName );

				m_bVisibleMesh = true;
			}

			ioHashString szEffectName = m_AttackInfo.m_AddNoneEffect[m_nWeaponIndex];
			if( !szEffectName.IsEmpty() )
			{
				ioEffect *pEffect = pOwner->AttachEffect(szEffectName);
				if( pEffect )
				{
					m_dwEffect = pEffect->GetUniqueID();
				}
			}
		}
	}
	else
	{
		int iMeshCnt = m_AttackInfo.m_AddNoneMeshForJump[m_nWeaponIndex].size();
		for( int i=0; i < iMeshCnt; ++i )
		{
			ioHashString szMeshName = (m_AttackInfo.m_AddNoneMeshForJump[m_nWeaponIndex])[i];
			pOwner->AddEntity( szMeshName );

			m_bVisibleMesh = true;
		}

		ioHashString szEffectName = m_AttackInfo.m_AddNoneEffectForJump[m_nWeaponIndex];
		if( !szEffectName.IsEmpty() )
		{
			ioEffect *pEffect = pOwner->AttachEffect(szEffectName);
			if( pEffect )
			{
				m_dwEffect = pEffect->GetUniqueID();
			}
		}
	}
}

void ioTrinityItem::AddNormalEntityAndEffect( ioBaseChar *pOwner )
{
	int iMeshCnt = m_AttackInfo.m_AddNormalMesh[m_nWeaponIndex].size();
	for( int i = 0; i < iMeshCnt; i++ )
	{
		ioHashString szMeshName = (m_AttackInfo.m_AddNormalMesh[m_nWeaponIndex])[i];
		pOwner->AddEntity( szMeshName );

		m_bVisibleMesh = true;
	}

	ioHashString szEffectName = m_AttackInfo.m_AddNormalEffect[m_nWeaponIndex];
	if( !szEffectName.IsEmpty() )
	{
		ioEffect *pEffect = pOwner->AttachEffect(szEffectName);
		if( pEffect )
		{
			m_dwEffect = pEffect->GetUniqueID();
		}
	}
}

void ioTrinityItem::EraseEntityAndEffect( ioBaseChar *pOwner )
{
	// remove mesh
	//Full
	for( int nWeaponIndex = 0; nWeaponIndex < MAX_TRINITY_ATTACK_CNT; nWeaponIndex++ )
	{
		int iMeshCnt = m_AttackInfo.m_AddFullMesh[nWeaponIndex].size();
		for( int i=0; i < iMeshCnt; ++i )
		{
			ioHashString szMeshName = (m_AttackInfo.m_AddFullMesh[nWeaponIndex])[i];
			pOwner->DestroyEntity( szMeshName );
		}

		//None
		iMeshCnt = m_AttackInfo.m_AddNoneMesh[nWeaponIndex].size();
		for( int i=0; i < iMeshCnt; ++i )
		{
			ioHashString szMeshName = (m_AttackInfo.m_AddNoneMesh[nWeaponIndex])[i];
			pOwner->DestroyEntity( szMeshName );
		}

		//None _ Jump
		iMeshCnt = m_AttackInfo.m_AddNoneMeshForJump[nWeaponIndex].size();
		for( int i=0; i < iMeshCnt; ++i )
		{
			ioHashString szMeshName = (m_AttackInfo.m_AddNoneMeshForJump[nWeaponIndex])[i];
			pOwner->DestroyEntity( szMeshName );
		}

		//Normal
		iMeshCnt = m_AttackInfo.m_AddNormalMesh[nWeaponIndex].size();
		for( int i = 0; i < iMeshCnt; i++ )
		{
			ioHashString szMeshName = (m_AttackInfo.m_AddNormalMesh[nWeaponIndex])[i];
			pOwner->DestroyEntity( szMeshName );
		}
	}

	//이건 이펙트
	if( m_dwEffect != 0 )
	{
		pOwner->EndEffect(m_dwEffect, false );
		m_dwEffect = 0;
	}
}

int ioTrinityItem::GetCurWeapon()
{
	return m_nWeaponIndex;
}

void ioTrinityItem::SetCurWeapon( int nCurWeapon )
{
	m_nWeaponIndex = nCurWeapon;
}

void ioTrinityItem::NextWeapon()
{
	m_fCurGauge = m_fMaxGauge;
	m_nWeaponIndex++;

	if( m_nWeaponIndex >= m_nMaxWeaponCnt )
		m_nWeaponIndex = 0;
}

void ioTrinityItem::SetVisible( ioBaseChar *pOwner, bool bVisible )
{
	__super::SetVisible(pOwner, bVisible);

	if( bVisible == false )
		m_bVisibleMesh = false;

	if( m_bVisibleMesh && bVisible )
		return;

	if( bVisible )
		AddNormalEntityAndEffect( pOwner );
	else
		EraseEntityAndEffect(pOwner);
}

void ioTrinityItem::ShowWeapon( ioBaseChar *pOwner )
{
	EraseEntityAndEffect( pOwner );
	AddNormalEntityAndEffect( pOwner );
}

void ioTrinityItem::HideWeapon( ioBaseChar *pOwner )
{
	EraseEntityAndEffect(pOwner);
}

//-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

ioTrinitySpecialState::ioTrinitySpecialState()
{
}

ioTrinitySpecialState::~ioTrinitySpecialState()
{
}

void ioTrinitySpecialState::Initialize( ioBaseChar* pOwner, CharState eCurrState )
{
	ioSpecialStateBase::Initialize( pOwner, eCurrState );
}

void ioTrinitySpecialState::Finalize( ioBaseChar* pOwner, CharState eCurrState, CharState eNewState )
{
	ioSpecialStateBase::Finalize( pOwner, eCurrState, eNewState );

	ioTrinityItem *pTrinityItem = ToTrinityItem( pOwner->GetPriorityItem( SPT_ATTACK ) );
	if( pTrinityItem )
		pTrinityItem->ClearState( pOwner );
}

void ioTrinitySpecialState::CheckSpecialState( ioBaseChar* pOwner )
{
	ioTrinityItem *pTrinity = ToTrinityItem( pOwner->GetPriorityItem( SPT_ATTACK ) );
	if( pTrinity )
		pTrinity->ProcessTrinityState( pOwner );
	else
	{
		pOwner->SetState( CS_DELAY );
		return;
	}
}