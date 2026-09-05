

#include "stdafx.h"

#include "ioPlayStage.h"
#include "ioBaseChar.h"

#include "ioBladeChargeObjItem.h"

#include "WeaponDefine.h"
#include "ItemDefine.h"

ioBladeChargeObjItem::ioBladeChargeObjItem()
{
	m_ChargeState = CS_NONE;
	m_dwAttackStartTime = 0;
	m_iCurCombo = 0;

	m_iSkillInput = -1;
	m_bEnableDefenseState = false;
	m_bEnableAttackState = false;
	m_bEnableJumpState = false;

	m_iCurAirJump = 0;
	m_fAirJumpPowerRate = 0.0f;
	m_nCurGatherAttackCnt = 0;
	m_dwEquipTime = 0;
}

ioBladeChargeObjItem::ioBladeChargeObjItem( const ioBladeChargeObjItem &rhs )
	: ioObjectItem( rhs ),
	m_ChargeBranchAni( rhs.m_ChargeBranchAni ),
	//m_vExtendAttributeList( rhs.m_vExtendAttributeList ),
	m_GatherAnimation( rhs.m_GatherAnimation ),
	m_fGatherAniRate( rhs.m_fGatherAniRate ),
	m_bEnableMaxCharge( rhs.m_bEnableMaxCharge ),
	m_bUseTimeOver( rhs.m_bUseTimeOver ),
	m_dwMaxUseTime( rhs.m_dwMaxUseTime ),
	m_dwGatherAttackTic( rhs.m_dwGatherAttackTic ),
	m_dwGatherAttackID( rhs. m_dwGatherAttackID ),
	m_stEnemyFindBuff( rhs.m_stEnemyFindBuff ),
	m_AreaWeaponName( rhs.m_AreaWeaponName ),
	m_fAreaWeponOffSet( rhs.m_fAreaWeponOffSet ),
	m_iReduceGaugeSlot( rhs.m_iReduceGaugeSlot )
{
	m_ChargeState = CS_NONE;
	m_dwAttackStartTime = 0;
	m_iCurCombo = 0;

	m_iCurAirJump = 0;
	m_fAirJumpPowerRate = 0.0f;
	m_nCurGatherAttackCnt = 0;
	m_bWounded = false;
	m_dwEquipTime = 0;
	m_dwCreateAreaWeaponTime = 0;
}

ioBladeChargeObjItem::~ioBladeChargeObjItem()
{
}

void ioBladeChargeObjItem::LoadProperty( ioINILoader &rkLoader )
{
	ioObjectItem::LoadProperty( rkLoader );

	//int i;
	char szBuf[MAX_PATH];

	/*int iExtendCnt = rkLoader.LoadInt_e( "extend_attack_cnt", 0 );
	m_vExtendAttributeList.clear();
	m_vExtendAttributeList.reserve( iExtendCnt );
	for( i=0 ; i<iExtendCnt ; i++ )
	{
		AttackAttribute kAttribute;
		wsprintf_e( szBuf, "extend_attack0%d", i+1 );
		LoadAttackAttribute( szBuf, kAttribute, rkLoader );
		m_vExtendAttributeList.push_back( kAttribute );
	}*/

	//for( i=0 ; i<iExtendCnt ; i++ )
	{
		rkLoader.LoadString( "charge_branch", "", szBuf, MAX_PATH );

		m_ChargeBranchAni = szBuf;
		//m_AttackReadyAniList.push_back( ioHashString(szBuf) );
	}

	m_bEnableMaxCharge = rkLoader.LoadBool_e( "enable_max_charge", false );

	rkLoader.LoadString_e( "gathering_animation", "", szBuf, MAX_PATH );
	m_GatherAnimation = szBuf;

	m_fGatherAniRate = rkLoader.LoadFloat_e( "gathering_ani_rate", FLOAT1 );

	m_bUseTimeOver = rkLoader.LoadBool_e( "use_time_over", false );
	m_dwMaxUseTime = rkLoader.LoadInt_e( "use_time_over_max_time", 0 );

	m_dwGatherAttackTic = rkLoader.LoadInt_e( "gather_attack_tic", 10000 );

	m_dwGatherAttackID = rkLoader.LoadInt_e( "gather_attack", 0 );

	rkLoader.LoadString_e( "gather_attack_find_buff", "", szBuf, MAX_PATH );
	m_stEnemyFindBuff = szBuf;

	rkLoader.LoadString_e( "area_weapon_name", "", szBuf, MAX_PATH );
	m_AreaWeaponName = szBuf;

	m_fAreaWeponOffSet = rkLoader.LoadFloat_e( "area_weapon_offset", 0.f );

	m_iReduceGaugeSlot = rkLoader.LoadInt_e( "reduce_gauge_slot", 0 );
}

ioItem* ioBladeChargeObjItem::Clone()
{
	return new ioBladeChargeObjItem( *this );
}

ioObjectItem::ObjectSubType ioBladeChargeObjItem::GetObjectSubType() const
{
	return OST_BLADE_CHARGE;
}

void ioBladeChargeObjItem::SetNormalAttack( ioBaseChar *pOwner, int iCurCombo )
{
	ClearCancelInfo();

	/*int iExtendCnt = m_vExtendAttributeList.size();
	if( iExtendCnt == 0 || !COMPARE( iCurCombo, 0, iExtendCnt ) )
	{
		ioWeaponItem::SetNormalAttack( pOwner, iCurCombo );

		m_ChargeState = CS_NONE;
		m_iCurCombo = iCurCombo;
	}
	else*/
	{
		ioEntityGroup *pGrp = pOwner->GetGroup();
		pGrp->ClearAllLoopAni( FLOAT100, true );
		pGrp->ClearAllActionAni( FLOAT100, true );

		int iAniID = pGrp->GetAnimationIdx( m_ChargeBranchAni );
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

void ioBladeChargeObjItem::CheckNormalAttack( ioBaseChar *pOwner )
{
	switch( m_ChargeState )
	{
	case CS_CHARGING:
		OnCharging( pOwner );
		break;
	case CS_GATHERING:
		OnGathering( pOwner );
		break;
	case CS_ATTACK_FIRE:
		{
			if( pOwner->IsNeedProcess() )
			{
				CheckCancelReserve( pOwner );

				if( ProcessCancel(pOwner) )
					return;
			}

			DWORD dwCurTime = FRAMEGETTIME();
			if( m_dwCreateAreaWeaponTime && m_dwCreateAreaWeaponTime < dwCurTime )
			{
				m_dwCreateAreaWeaponTime = 0;

				D3DXVECTOR3 vPos =  pOwner->GetWorldPosition();
				D3DXVECTOR3 vLook = pOwner->GetTargetRot() * D3DXVECTOR3( 0.f, 0.f, -FLOAT1 ); 
				vPos = vPos + (vLook * m_fAreaWeponOffSet);
				
				ioAreaWeapon *pArea = g_AreaWeaponMgr.CreateAreaWeapon( pOwner->GetCharName(),
					"",
					"",
					m_AreaWeaponName,
					vPos,
					pOwner->GetTargetRot(),
					ioAreaWeapon::CT_NORMAL );
			}

			ioWeaponItem::CheckNormalAttack( pOwner );
		}
		break;
	/*case CS_GATHER_ATTACK:
		{
			if( pOwner->IsNeedProcess() )
			{

			}
		}
		break;*/
	case CS_END:
		break;
	}
}

void ioBladeChargeObjItem::OnCharging( ioBaseChar *pOwner )
{
	if( !pOwner->IsNeedProcess() )	return;

	if( pOwner->IsAttackKeyDown() )
	{
		if( m_dwAttackStartTime + m_dwChargeTime <= FRAMEGETTIME() )
		{
			ChangeToGathering( pOwner );
		}
	}
	else
	{
		ChangeToAttackFire( pOwner );
	}
}


void ioBladeChargeObjItem::ChangeToGatherAttackFire( ioBaseChar *pOwner )
{
	int iEquipBuffCnt = m_EquipBuffList.size();
	for( int i=0; i < iEquipBuffCnt; ++i )
	{
		ioHashString szBuffName = m_EquipBuffList[i];
		pOwner->RemoveBuff( szBuffName );
	}

	if( !pOwner )
		return;

	ioPlayStage *pStage = pOwner->GetCreator();
	if( !pStage )
		return;

	DWORD dwWeaponIndex = pOwner->IncreaseWeaponIndexBase();

	const BaseCharList &rkCharList = pStage->GetBaseCharList();
	BaseCharList::const_iterator iter;
	bool bFind = false;
	for( iter=rkCharList.begin() ; iter!=rkCharList.end() ; ++iter )
	{
		ioBaseChar *pTarget = *iter;
		if( !pTarget ) continue;

		if( pTarget->GetTeam() == pOwner->GetTeam() ) continue;
		if( pTarget->GetCharName() == GetOwnerName() ) continue;
		if( pTarget->GetExperienceMode() != EMS_NONE ) continue;

		if( !pTarget->HasBuff( pOwner->GetCharName(), m_stEnemyFindBuff ) )
			continue;

		bFind = true;
		FireTime kFireTime;
		kFireTime.dwStart = FRAMEGETTIME();

		kFireTime.iAttributeIdx = m_dwGatherAttackID;
		kFireTime.iResistanceIdx = 0;
		kFireTime.szWoundedAni  = "";
		kFireTime.dwWoundedAniDuration = 0;
		kFireTime.bLoopWoundedAni = false;
		kFireTime.dwWeaponIdx = dwWeaponIndex++;
		kFireTime.eFireTimeType = FTT_NORMAL_ATTACK;

		D3DXVECTOR3 vPos = pOwner->GetMidPositionByRate();
		D3DXVECTOR3 vDir;// = pOwner->GetTargetRot() * D3DXVECTOR3( 0, 0, -1 );
		vDir = pTarget->GetWorldPosition() - pOwner->GetWorldPosition();
		D3DXVec3Normalize( &vDir, &vDir );

		ioWeapon *pWeapon = pOwner->ExtendFireExplicit( kFireTime, vPos, vDir, "" );	
	}

	if( !bFind )
	{
		FireTime kFireTime;
		kFireTime.dwStart = FRAMEGETTIME();

		kFireTime.iAttributeIdx = m_dwGatherAttackID;
		kFireTime.iResistanceIdx = 0;
		kFireTime.szWoundedAni  = "";
		kFireTime.dwWoundedAniDuration = 0;
		kFireTime.bLoopWoundedAni = false;
		kFireTime.dwWeaponIdx = dwWeaponIndex++;
		kFireTime.eFireTimeType = FTT_NORMAL_ATTACK;

		D3DXVECTOR3 vPos = pOwner->GetMidPositionByRate();
		D3DXVECTOR3 vDir = pOwner->GetTargetRot() * D3DXVECTOR3( 0, 0, -1 );

		ioWeapon *pWeapon = pOwner->ExtendFireExplicit( kFireTime, vPos, vDir, GetName() );	
	}

	if( pOwner->IsNeedSendNetwork() )
	{
		SP2Packet kPacket( CUPK_ITEM_GAUGE_OK );
		FillItemGaugePacketHeader( pOwner, kPacket );
		kPacket << m_ChargeState;
		kPacket << m_iCurCombo;
		kPacket << true;
		P2PNetwork::SendToAllPlayingUser( kPacket );
	}
}

void ioBladeChargeObjItem::ChangeToAttackFire( ioBaseChar *pOwner )
{
	if( !pOwner ) return;

	DWORD dwCurTime = FRAMEGETTIME();
	ClearCancelInfo();

	pOwner->InitExtendAttackTagInfo();

	bool bMaxAttack = false;
	bool bMaxBuffRemove = false;
	m_fCurChargeRate = 0.0f;
	m_fCurChargeRateForSkill = 0.0f;

	const AttackAttribute *pAttribute = m_FormalAttack.GetAttribute( AT_NORMAL, m_iCurCombo );
	if( pAttribute )
	{
		pOwner->SetNormalAttackByAttribute( *pAttribute, false );

		ioEntityGroup *pGrp = pOwner->GetGroup();
		int iAniID = pGrp->GetAnimationIdx( pAttribute->m_AttackAnimation );
		if( iAniID != -1 )
		{
			DWORD dwPreDelay = pAttribute->m_dwPreDelay;
			DWORD dwCurTime = FRAMEGETTIME();
			m_dwCreateAreaWeaponTime = dwCurTime + dwPreDelay;
			m_dwCreateAreaWeaponTime += pGrp->GetAnimationEventTime( iAniID, "fire_s" ) * pAttribute->m_fAttackAniRate;
		}
	}


	//pOwner->ApplyNormalAttackResult( m_iCurCombo );

	m_ChargeState = CS_ATTACK_FIRE;

	if( pOwner->IsNeedSendNetwork() )
	{
		SP2Packet kPacket( CUPK_ITEM_GAUGE_OK );
		FillItemGaugePacketHeader( pOwner, kPacket );
		kPacket << m_ChargeState;
		kPacket << m_iCurCombo;
		kPacket << pOwner->GetTargetRot();
		P2PNetwork::SendToAllPlayingUser( kPacket );
	}
}

void ioBladeChargeObjItem::OnItemGaugeOK( ioBaseChar *pOwner, SP2Packet &rkPacket )
{
	if( !pOwner ) return;

	int iState;
	rkPacket >> iState;
	rkPacket >> m_iCurCombo;

	switch( iState )
	{
	case CS_GATHERING:
		{
			bool bRelease;
			rkPacket >> bRelease;

			if( !bRelease )
				ChangeToGathering( pOwner );
			else
				ChangeToGatherAttackFire( pOwner );
		}
		break;
	case CS_ATTACK_FIRE:
		{
			D3DXQUATERNION qtRot;
			rkPacket >> qtRot;
			pOwner->SetTargetRot( qtRot );

			ChangeToAttackFire( pOwner );
		}
		break;
	}
}

void ioBladeChargeObjItem::SetExtraInfoToWeapon( ioWeapon *pWeapon, bool bJumpAttack )
{
	ioObjectItem::SetExtraInfoToWeapon( pWeapon, bJumpAttack );

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

bool ioBladeChargeObjItem::IsChargeAttackState( ioBaseChar *pOwner )
{
	if( pOwner->GetState() == CS_ATTACK )
	{
		if( m_ChargeState == CS_CHARGING )
			return true;
	}

	return false;
}

bool ioBladeChargeObjItem::IsEnableAttackDashCheck( ioBaseChar *pOwner, bool bCheck )
{
	if( !IsChargeDash() ) return false;

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

//void ioBladeChargeObjItem::CheckReserve( ioBaseChar *pOwner )
//{
//	bool bReserveSkill = false;
//	int iSkillNum = pOwner->GetSkillKeyInput();
//	if( iSkillNum >= 0 )
//	{
//		m_iSkillInput = iSkillNum;
//		bReserveSkill = true;
//	}
//
//	if( pOwner->IsDefenseKeyDown() && !m_bEnableDefenseState && !bReserveSkill )
//	{
//		m_bEnableDefenseState = true;
//		m_bEnableAttackState = false;
//		m_bEnableJumpState = false;
//
//		m_iSkillInput = -1;
//	}
//	else if( pOwner->IsAttackKey() && !m_bEnableAttackState && !bReserveSkill )
//	{
//		m_bEnableAttackState = true;
//		m_bEnableDefenseState = false;
//		m_bEnableJumpState = false;
//
//		m_iSkillInput = -1;
//	}
//	else if( pOwner->IsJumpKey() && !m_bEnableJumpState && !bReserveSkill )
//	{
//		m_bEnableJumpState = true;
//		m_bEnableAttackState = false;
//		m_bEnableDefenseState = false;
//		m_iSkillInput = -1;
//	}
//}

//bool ioBladeChargeObjItem::ProcessCancel( ioBaseChar *pOwner )
//{
//	if( m_szCurAni.IsEmpty() )
//		return false;
//
//	if( !pOwner ) return false;
//
//	ioEntityGroup *pGrp = pOwner->GetGroup();
//	if( !pGrp ) return false;
//
//	int iAniID = pGrp->GetAnimationIdx( m_szCurAni );
//
//	std::list<float> vTimeList;
//	pGrp->GetAnimationEventTimeList_e( iAniID, "input_jump_s", vTimeList );
//
//	if( vTimeList.empty() )
//		return false;
//
//	DWORD dwCurTime = FRAMEGETTIME();
//	std::list< float >::iterator iter = vTimeList.begin();
//	while( iter != vTimeList.end() )
//	{
//		DWORD dwTime = (DWORD)*iter;
//		dwTime *= m_fCurAniRate;
//
//		dwTime += m_dwInputCancelCheckTime;
//
//		if( m_dwCancelCheckTime > 0 && COMPARE( dwTime, m_dwCancelCheckTime, dwCurTime ) )
//		{
//			m_dwCancelCheckTime = dwTime+1;
//
//			if( pOwner->IsDirKeyDoubleClick() && !pOwner->HasBuff(BT_DISABLE_DASH) )
//			{
//				pOwner->ChangeDirectionByInputDir( false );
//				pOwner->SetDashState( false );
//				pOwner->SendDashState( false );
//
//				return true;
//			}
//			else if( m_bEnableJumpState && !pOwner->IsBuffLimitJump() )
//			{
//				pOwner->SetJumpState();
//				return true;
//			}
//
//			return false;
//		}
//
//		if( dwTime > dwCurTime )
//			return false;
//
//		iter++;
//	}
//
//	return false;
//}

bool ioBladeChargeObjItem::SetJumpAttack( ioBaseChar *pOwner, int iAniID, float fTimeRate, AttackType eType )
{
	if( !pOwner ) 
		return false;

	ioEntityGroup *pGrp = pOwner->GetGroup();
	if( !pGrp )
		return false;

	DWORD dwCurTime = FRAMEGETTIME();
	ClearAirJump();

	AttackType eExtendType = AT_JUMP_ATTACK;

	const AttackAttribute *pAttr = GetAttackAttribute( eExtendType );
	if( !pAttr )
	{
		LOG.PrintTimeAndLog( 0, "ioBladeChargeObjItem::SetJumpAttack - AttackAttribute Not Exist" );
		return false;
	}

	iAniID = pGrp->GetAnimationIdx( pAttr->m_AttackAnimation );
	if( iAniID == -1 )
	{
		LOG.PrintTimeAndLog( 0, "ioBladeChargeObjItem::SetJumpAttack - JumpAttack Animation Not Exist" );
		return false;
	}

	fTimeRate   = pAttr->m_fAttackAniRate;
	DWORD dwPreDelay  = pAttr->m_dwPreDelay;

	SetAirJump( pOwner, iAniID, fTimeRate );

	pGrp->SetActionAni( iAniID, FLOAT100, FLOAT100, FLOAT1, FLOAT1/fTimeRate, 0.0f, dwPreDelay, true );

	m_dwCreateAreaWeaponTime = dwCurTime + dwPreDelay;
	m_dwCreateAreaWeaponTime += pGrp->GetAnimationEventTime( iAniID, "fire_s" ) * fTimeRate;

	return true;

	//return false;
	//return ioObjectItem::SetJumpAttack( pOwner, iAniID, fTimeRate, eType );
}

void ioBladeChargeObjItem::ProcessJumpAttack( ioBaseChar *pOwner, float fHeightGap )
{
	if( m_pExtendJump )
	{
		m_pExtendJump->ProcessJumpAttack( pOwner, fHeightGap );
	}

	CheckAirJump( pOwner );

	DWORD dwCurTime = FRAMEGETTIME();
	if( m_dwCreateAreaWeaponTime && m_dwCreateAreaWeaponTime < dwCurTime )
	{
		m_dwCreateAreaWeaponTime = 0;

		D3DXVECTOR3 vPos =  pOwner->GetWorldPosition();
		D3DXVECTOR3 vLook = pOwner->GetTargetRot() * D3DXVECTOR3( 0.f, 0.f, -FLOAT1 ); 
		vPos = vPos + (vLook * m_fAreaWeponOffSet);
		ioAreaWeapon *pArea = g_AreaWeaponMgr.CreateAreaWeapon( pOwner->GetCharName(),
			"",
			"",
			m_AreaWeaponName,
			vPos,
			pOwner->GetTargetRot(),
			ioAreaWeapon::CT_NORMAL );
	}
}

bool ioBladeChargeObjItem::IsOneTimeWeaponItem() const
{
	return false;
}

bool ioBladeChargeObjItem::IsEnableDash() const
{
	return true;
}

bool ioBladeChargeObjItem::IsEnableDefense() const
{
	return true;
}

void ioBladeChargeObjItem::OnEndExtendAttack( ioBaseChar *pOwner, bool bReleaseItem )
{
	pOwner->SetAttackMoveEnable( false );
	pOwner->SetChargingState( false );

	m_fCurChargeRate = 0.0f;
}

void ioBladeChargeObjItem::ChangeToGathering( ioBaseChar *pOwner )
{
	ioEntityGroup *pGrp = pOwner->GetGroup();
	pGrp->ClearAllActionAni( FLOAT100, true );
	pGrp->ClearAllLoopAni( FLOAT100, true );

	float fCurRate = FLOAT1;
	if( m_fGatherAniRate > 0.0f )
		fCurRate = m_fGatherAniRate;

	pGrp->SetLoopAni( m_GatherAnimation, FLOAT100, FLOAT1, FLOAT1/fCurRate );

	m_ChargeState = CS_GATHERING;
	m_dwGatheringStartTime = FRAMEGETTIME();
	m_dwGatherAttackTime = m_dwGatheringStartTime;

	pOwner->SetAttackMoveEnable( false );

	if( pOwner->IsNeedSendNetwork() )
	{
		SP2Packet kPacket( CUPK_ITEM_GAUGE_OK );
		FillItemGaugePacketHeader( pOwner, kPacket );
		kPacket << m_ChargeState;
		kPacket << m_iCurCombo;
		kPacket << false;		// release
		P2PNetwork::SendToAllPlayingUser( kPacket );
	}
}

void ioBladeChargeObjItem::OnGathering( ioBaseChar *pOwner )
{
	if( !pOwner || !pOwner->IsNeedProcess() )	return;

	DWORD dwCurTime = FRAMEGETTIME();

	if( pOwner->IsAttackKeyDown() )
	{
		if( m_dwGatheringStartTime + m_dwMaxChargeTime < dwCurTime )
		{
			m_ChargeState = CS_END;
			return;
		}
		
		if( m_dwGatherAttackTime < dwCurTime )
		{
			ChangeToGatherAttackFire( pOwner );
			m_dwGatherAttackTime += m_dwGatherAttackTic;
			return;
		}
	}
	else
	{
		m_ChargeState = CS_END;
	}
}

bool ioBladeChargeObjItem::IsTimeOver()
{
	if( !m_bUseTimeOver )
		return false;

	DWORD dwCurTime = FRAMEGETTIME();
	if( dwCurTime > (m_dwEquipTime + m_dwMaxUseTime) && m_ChargeState != CS_GATHERING )
		return true;

	return false;
}


bool ioBladeChargeObjItem::EnableReleaseItem( ioBaseChar *pOwner )
{
	if( m_ChargeState == CS_END )
		return true;

	return false;
}


void ioBladeChargeObjItem::OnEquiped( ioBaseChar *pOwner )
{
	__super::OnEquiped( pOwner );

	m_dwEquipTime = FRAMEGETTIME();
}


void ioBladeChargeObjItem::OnReleased( ioBaseChar *pOwner )
{
	ioPlayStage *pStage = pOwner->GetCreator();
	if( !pStage )
		return;

	ioObjectItem::OnReleased( pOwner );

	int iEquipBuffCnt = m_EquipBuffList.size();
	for( int i=0; i < iEquipBuffCnt; ++i )
	{
		ioHashString szBuffName = m_EquipBuffList[i];
		pOwner->RemoveBuff( szBuffName );
	}

	//////////////////////////////////////////////////////////////////////////
	const BaseCharList &rkCharList = pStage->GetBaseCharList();
	BaseCharList::const_iterator iter;
	bool bFind = false;
	for( iter=rkCharList.begin() ; iter!=rkCharList.end() ; ++iter )
	{
		ioBaseChar *pTarget = *iter;
		if( !pTarget ) continue;
		if( pTarget->GetTeam() == pOwner->GetTeam() ) continue;
		if( pTarget->GetCharName() == GetOwnerName() ) continue;

		ioBuff* pBuff = pTarget->GetBuff( m_stEnemyFindBuff, pOwner->GetCharName() );
		if( pBuff )
			pBuff->SetReserveEndBuff();
	}
}

void ioBladeChargeObjItem::OnProcessState( ioPlayStage *pStage, ioBaseChar *pOwner )
{
	if( !pOwner )
		return;

	if( !COMPARE( m_iReduceGaugeSlot, ES_WEAPON, ES_CLOAK+1 ) )
		return;

	ioItem *pItem = pOwner->GetEquipedItem( m_iReduceGaugeSlot );
	if( pItem && pItem->HasSkillGauge() )
	{
		pItem->ReduceNeedGauge( pItem->GetMaxSkillGauge( pOwner->GetCharName() ), pOwner );
	}
}

bool ioBladeChargeObjItem::CheckReleaseProcess( ioPlayStage *pStage, ioBaseChar *pOwner )
{
	bool bRet = ioObjectItem::CheckReleaseProcess( pStage, pOwner );
	if( bRet )
		return true;

	if( EnableReleaseItem( pOwner ) )
	{
		return true;
	}
	else if( IsWounded() )
	{
		return true;
	}
	else if( IsTimeOver() )
	{
		return true;
	}

	return false;
}