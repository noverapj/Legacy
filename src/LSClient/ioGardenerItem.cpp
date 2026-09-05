#include "StdAfx.h"

#include "ioGardenerItem.h"
#include "WeaponDefine.h"
#include "ioDummyStruct.h"

ioGardenerItem::ioGardenerItem(void)
{
	ClearData();
}

ioGardenerItem::ioGardenerItem( const ioGardenerItem &rhs ):
ioWeaponItem( rhs ),
	m_AttackReadyAniList( rhs.m_AttackReadyAniList ),
	m_szGatheringAni( rhs.m_szGatheringAni ),
	m_fGatheringAniRate( rhs.m_fGatheringAniRate ),
	m_dwGatheringTime( rhs.m_dwGatheringTime ),
	m_szCreateAreaWeaponLoopAni( rhs.m_szCreateAreaWeaponLoopAni ),
	m_szCreateAreaWeaponEndAni( rhs.m_szCreateAreaWeaponEndAni ),
	m_szHealAreaWeaponAni( rhs.m_szHealAreaWeaponAni ),
	m_fCreateAreaWeaponAniRate( rhs.m_fCreateAreaWeaponAniRate ),
	m_fHealAreaWeaponAniRate( rhs.m_fHealAreaWeaponAniRate ),
	m_dwCreateAreaWeaponTime( rhs.m_dwCreateAreaWeaponTime ),
	m_iAreaWeaponCount( rhs.m_iAreaWeaponCount ),
	m_AreaWeaponNameVec( rhs.m_AreaWeaponNameVec ),
	m_szHealAreaWeaponName( rhs.m_szHealAreaWeaponName ),
	m_fAreaWeaponOffset( rhs.m_fAreaWeaponOffset ),
	m_fHealAreaWeaponOffset( rhs.m_fHealAreaWeaponOffset ),
	m_iWallDummyMaxCount( rhs.m_iWallDummyMaxCount ),
	m_fWallDummyOffset( rhs.m_fWallDummyOffset ),
	m_szWallDummyName( rhs.m_szWallDummyName ),
	m_szWallDummyAni( rhs.m_szWallDummyAni ),
	m_fWallDummyAniRate( rhs.m_fWallDummyAniRate ),
	m_fTreeDummyOffset( rhs.m_fTreeDummyOffset ),
	m_szTreeDummyName( rhs.m_szTreeDummyName ),
	m_szTreeDummyAni( rhs.m_szTreeDummyAni ),
	m_fTreeDummyAniRate( rhs.m_fTreeDummyAniRate ),
	m_AniDefenseFrontMove( rhs.m_AniDefenseFrontMove ),
	m_AniDefenseBackMove( rhs.m_AniDefenseBackMove ),
	m_AniDefenseRightMove( rhs.m_AniDefenseRightMove ),
	m_AniDefenseLeftMove( rhs.m_AniDefenseLeftMove ),
	m_szRetreatFr( rhs.m_szRetreatFr ),
	m_szRetreatBack( rhs.m_szRetreatBack ),
	m_szRetreatLt( rhs.m_szRetreatLt ),
	m_szRetreatRt( rhs.m_szRetreatRt ),
	m_fRetreatFrRate( rhs.m_fRetreatFrRate ),
	m_fRetreatBackRate( rhs.m_fRetreatBackRate ),
	m_fRetreatLtRate( rhs.m_fRetreatLtRate ),
	m_fRetreatRtRate( rhs.m_fRetreatRtRate ),
	m_fRetreatForceAmt( rhs.m_fRetreatForceAmt ),
	m_fRetreatForceFric( rhs.m_fRetreatForceFric ),
	m_szAreaWeaponEffectName( rhs.m_szAreaWeaponEffectName ),
	m_szAreaWeaponMeshName( rhs.m_szAreaWeaponMeshName ),
	m_fMaxGauge( rhs.m_fMaxGauge ),
	m_fTreeNeedGauge( rhs.m_fTreeNeedGauge ),
	m_fWallNeedGauge( rhs.m_fWallNeedGauge ),
	m_fAreaNeedGauge( rhs.m_fAreaNeedGauge ),
	m_fHealNeedGauge( rhs.m_fHealNeedGauge ),
	m_fAreaDecreaseGauge( rhs.m_fAreaDecreaseGauge ),
	m_fIncreaseGauge( rhs.m_fIncreaseGauge )
{
	ClearData();

	m_fCurGauge = 0.0f;
}

ioGardenerItem::~ioGardenerItem(void)
{
	ClearData();
}

void ioGardenerItem::ClearData()
{
	m_ChargeState = CS_NONE;
	
	m_iCurCombo = 0;
	m_dwMotionStartTime = 0;
	m_dwMotionEndTime = 0;
	m_dwAreaWeaponEffectIndex = 0;

	m_bDefenseMotionSetted = false;
	m_bDefenseFirstMoveMotion = true;
}

void ioGardenerItem::ClearState()
{
	m_ChargeState = CS_NONE;

	m_iSkillInput = -1;
	m_bEnableDefenseState = false;
	m_bEnableAttackState = false;
	m_bEnableJumpState = false;
}

ioItem* ioGardenerItem::Clone()
{
	return new ioGardenerItem( *this );
}

ioWeaponItem::WeaponSubType ioGardenerItem::GetSubType() const
{
	return WST_GARDENER_ITEM;
}

void ioGardenerItem::SetNoneState( ioBaseChar* pOwner, int eNewState /* = 0 */ )
{
	DestroyAreaWeaponEffect( pOwner );

	ClearData();
}

void ioGardenerItem::LoadProperty( ioINILoader &rkLoader )
{
	ioWeaponItem::LoadProperty( rkLoader );

	char szBuf[MAX_PATH];
	char szKey[MAX_PATH];

	int iMaxCombo = GetMaxCombo();
	m_AttackReadyAniList.clear();
	for( int i=0; i < iMaxCombo; ++i )
	{
		wsprintf_e( szBuf, "attack_ready_animation%d", i+1 );
		rkLoader.LoadString( szBuf, "", szBuf, MAX_PATH );
		m_AttackReadyAniList.push_back( ioHashString(szBuf) );
	}	
	
	rkLoader.LoadString_e( "gathering_animation", "", szBuf, MAX_PATH );
	m_szGatheringAni = szBuf;
	m_fGatheringAniRate = rkLoader.LoadFloat_e( "gathering_ani_rate", FLOAT1 );
	m_dwGatheringTime = (DWORD)rkLoader.LoadInt_e( "gathering_time", 0 );

	rkLoader.LoadString_e( "create_areaweapon_loop_ani", "", szBuf, MAX_PATH );
	m_szCreateAreaWeaponLoopAni = szBuf;
	rkLoader.LoadString_e( "create_areaweapon_end_ani", "", szBuf, MAX_PATH );
	m_szCreateAreaWeaponEndAni = szBuf;
	rkLoader.LoadString_e( "heal_areaweapon_ani", "", szBuf, MAX_PATH );
	m_szHealAreaWeaponAni = szBuf;
	m_fCreateAreaWeaponAniRate = rkLoader.LoadFloat_e( "create_areaweapon_ani_rate", FLOAT1 );
	m_fHealAreaWeaponAniRate = rkLoader.LoadFloat_e( "heal_areaweapon_ani_rate", FLOAT1 );
	m_dwCreateAreaWeaponTime = (DWORD)rkLoader.LoadInt_e( "create_areaweapon_time", 0 );
	
	m_iAreaWeaponCount = rkLoader.LoadInt_e( "areaweapon_cnt", 0 );
	for( int i = 0; i < m_iAreaWeaponCount; ++i )
	{
		AreaWeaponInfo cInfo;
		wsprintf_e( szKey, "areaweapon%d_name1", i+1 );
		rkLoader.LoadString( szKey, "", szBuf, MAX_PATH );
		cInfo.m_szName[0] = szBuf;

		wsprintf_e( szKey, "areaweapon%d_name2", i+1 );
		rkLoader.LoadString( szKey, "", szBuf, MAX_PATH );
		cInfo.m_szName[1] = szBuf;

		m_AreaWeaponNameVec.push_back( cInfo );
	}
	m_fAreaWeaponOffset = rkLoader.LoadFloat_e( "areaweapon_offset", FLOAT1 );

	rkLoader.LoadString_e( "areaweapon_effect_name", "", szBuf, MAX_PATH );
	m_szAreaWeaponEffectName = szBuf;
	rkLoader.LoadString_e( "areaweapon_mesh_name", "", szBuf, MAX_PATH );
	m_szAreaWeaponMeshName = szBuf;

	rkLoader.LoadString_e( "heal_areaweapon_name", "", szBuf, MAX_PATH );
	m_szHealAreaWeaponName = szBuf;
	m_fHealAreaWeaponOffset = rkLoader.LoadFloat_e( "heal_areaweapon_offset", FLOAT1 );

	m_iWallDummyMaxCount = rkLoader.LoadInt_e( "wall_dummy_max_count", 0 );
	m_fWallDummyOffset = rkLoader.LoadFloat_e( "wall_dummy_offset", FLOAT1 );
	m_fWallDummyAniRate = rkLoader.LoadFloat_e( "wall_dummy_ani_rate", FLOAT1 );
	rkLoader.LoadString_e( "wall_dummy_name", "", szBuf, MAX_PATH );
	m_szWallDummyName = szBuf;
	rkLoader.LoadString_e( "wall_dummy_ani", "", szBuf, MAX_PATH );
	m_szWallDummyAni = szBuf;

	m_fTreeDummyOffset = rkLoader.LoadFloat_e( "tree_dummy_offset", FLOAT1 );
	m_fTreeDummyAniRate = rkLoader.LoadFloat_e( "tree_dummy_ani_rate", FLOAT1 );
	rkLoader.LoadString_e( "tree_dummy_name", "", szBuf, MAX_PATH );
	m_szTreeDummyName = szBuf;
	rkLoader.LoadString_e( "tree_dummy_ani", "", szBuf, MAX_PATH );
	m_szTreeDummyAni = szBuf;

	rkLoader.LoadString_e( "ani_defense_front_move", "victory_defense_move_fr.ani", szBuf, MAX_PATH );
	m_AniDefenseFrontMove = szBuf;
	rkLoader.LoadString_e( "ani_defense_back_move", "victory_defense_move_bk.ani", szBuf, MAX_PATH );
	m_AniDefenseBackMove = szBuf;
	rkLoader.LoadString_e( "ani_defense_right_move", "victory_defense_move_rt.ani", szBuf, MAX_PATH );
	m_AniDefenseRightMove = szBuf;
	rkLoader.LoadString_e( "ani_defense_left_move", "victory_defense_move_lt.ani", szBuf, MAX_PATH );
	m_AniDefenseLeftMove = szBuf;

	rkLoader.LoadString_e( "retreat_animation_front", "", szBuf, MAX_PATH );
	m_szRetreatFr = szBuf;
	m_fRetreatFrRate = rkLoader.LoadFloat_e( "retreat_animation_front_rate", FLOAT1 );
	rkLoader.LoadString_e( "retreat_animation_back", "", szBuf, MAX_PATH );
	m_szRetreatBack = szBuf;
	m_fRetreatBackRate = rkLoader.LoadFloat_e( "retreat_animation_back_rate", FLOAT1 );
	rkLoader.LoadString_e( "retreat_animation_right", "", szBuf, MAX_PATH );
	m_szRetreatRt = szBuf;
	m_fRetreatRtRate = rkLoader.LoadFloat_e( "retreat_animation_right_rate", FLOAT1 );
	rkLoader.LoadString_e( "retreat_animation_left", "", szBuf, MAX_PATH );
	m_szRetreatLt = szBuf;
	m_fRetreatLtRate = rkLoader.LoadFloat_e( "retreat_animation_left_rate", FLOAT1 );

	m_fRetreatForceAmt = rkLoader.LoadFloat_e( "retreat_force_amt", 0.0f );
	m_fRetreatForceFric = rkLoader.LoadFloat_e( "retreat_force_friction", FLOAT1 );

	m_fMaxGauge = rkLoader.LoadFloat_e( "max_gauge", FLOAT100 );
	m_fTreeNeedGauge = rkLoader.LoadFloat_e( "tree_need_gauge", FLOAT1 );
	m_fWallNeedGauge = rkLoader.LoadFloat_e( "wall_need_gauge", FLOAT1 );
	m_fAreaNeedGauge = rkLoader.LoadFloat_e( "area_need_gauge", FLOAT1 );
	m_fHealNeedGauge = rkLoader.LoadFloat_e( "heal_need_gauge", FLOAT1 );
	m_fAreaDecreaseGauge = rkLoader.LoadFloat_e( "area_decrease_gauge", FLOAT1 );
	m_fIncreaseGauge = rkLoader.LoadFloat_e( "increase_gauge", FLOAT1 );
}

//Normal Attack
void ioGardenerItem::SetNormalAttack( ioBaseChar *pOwner, int iCurCombo )
{
	if( !pOwner ) return;

	m_fCurChargeRate = 0.0f;
	m_fCurChargeRateForSkill = 0.0f;

	m_dwKeyReserveTime = 0;
	m_dwMotionStartTime = 0;
	m_dwMotionEndTime = 0;

	m_iSkillInput = -1;
	m_bEnableDefenseState = false;
	m_bEnableAttackState = false;
	m_bEnableJumpState = false;

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
		m_dwMotionStartTime = FRAMEGETTIME();
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

bool ioGardenerItem::IsChargeAutoTarget( ioBaseChar *pOwner )
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

void ioGardenerItem::GetAutoTargetValue( ioBaseChar *pOwner, float &fRange, float &fMinAngle, float &fMaxAngle, AutoTargetType eType )
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

bool ioGardenerItem::IsEnableAttackDashCheck( ioBaseChar *pOwner, bool bCheck )
{
	switch( m_ChargeState )
	{
	case CS_CREATE_AREA_WEAPON:
		return true;
	}
	return false;
}

bool ioGardenerItem::IsEnableAttackRotateCheck( ioBaseChar *pOwner )
{
	switch( m_ChargeState )
	{
	case CS_CREATE_AREA_WEAPON:
		return true;
	}
	return false;
}

void ioGardenerItem::CheckNormalAttack( ioBaseChar *pOwner )
{
	DWORD dwCurTime = FRAMEGETTIME();
	switch( m_ChargeState )
	{
	case CS_CHARGING:
		OnCharging( pOwner );
		break;
	case CS_GATHERING:
		OnGathering( pOwner );
		break;
	case CS_NORMAL_ATTACK:
		if( pOwner->IsNeedProcess() )
		{
			CheckCancelReserve( pOwner );

			if( ProcessCancel(pOwner) )
				return;
		}
		ioWeaponItem::CheckNormalAttack( pOwner );
		break;
	case CS_ACTION_TREE_DUMMY_PRE:
		ChangeToTreeDummyAction( pOwner );
		break;
	case CS_ACTION_WALL_DUMMY:
		OnDummyWallAction( pOwner );
		break;
	case CS_ACTION_TREE_DUMMY:
		OnDummyTreeAction( pOwner );
		break;
	case CS_MAKE_WALL_DUMMY:
	case CS_MAKE_TREE_DUMMY:
		OnDummyMake( pOwner );
		break;
	case CS_CREATE_AREA_WEAPON:
		OnCreateAreaWeapon( pOwner );
		break;
	case CS_CREATE_AREA_WEAPON_END:
	case CS_HEAL_AREA_WEAPON:
		if( m_dwMotionEndTime < FRAMEGETTIME() )
		{
			m_ChargeState = CS_NONE;
			pOwner->SetState( CS_DELAY );
		}
		break;
	}
}

void ioGardenerItem::OnCharging( ioBaseChar *pOwner )
{
	if( !pOwner || !pOwner->IsNeedProcess() )	return;

	if( pOwner->IsAttackKeyDown() && !pOwner->IsHasCrown() )
	{
		if( m_dwMotionStartTime + m_dwChargeTime <= FRAMEGETTIME() )
		{
			ChangeGatheringState( pOwner );
		}
	}
	else
	{
		ChangeToNormalAttack( pOwner );
	}
}

void ioGardenerItem::OnGathering( ioBaseChar *pOwner )
{
	if( !pOwner )
		return;

	if( !pOwner->IsNeedProcess() )	
		return;

	if( pOwner->IsAttackKeyDown() && !pOwner->IsHasCrown() && IsEnableGauge( m_fAreaNeedGauge ) )
	{
		if( m_dwMotionEndTime <= FRAMEGETTIME() )
			ChangeToCraeteAreaWeaponState( pOwner );
	}
	else if( IsEnableGauge( m_fWallNeedGauge ) )
		ChangeToWallDummyAction( pOwner );
	else
		ChangeToNormalAttack( pOwner );
}

void ioGardenerItem::OnDummyMake( ioBaseChar *pOwner )
{
	if( m_dwDummyEndTime < FRAMEGETTIME() )
	{
		m_ChargeState = CS_NONE;
		pOwner->SetState( CS_DELAY );
	}
}

void ioGardenerItem::OnDummyWallAction( ioBaseChar *pOwner )
{
	if( m_dwDummyCreateTime < FRAMEGETTIME() )
	{
		ChangeToWallDummyMake( pOwner->GetCreator(), pOwner );
	}
}

void ioGardenerItem::OnDummyTreeAction( ioBaseChar *pOwner )
{
	if( m_dwDummyCreateTime < FRAMEGETTIME() )
	{
		ChangeToTreeDummyMake( pOwner->GetCreator(), pOwner );
	}
}

void ioGardenerItem::OnCreateAreaWeapon( ioBaseChar *pOwner )
{
	if( !pOwner || !pOwner->IsNeedProcess() )
		return;

	DWORD dwCurTime = FRAMEGETTIME();
	if( pOwner->IsAttackKeyDown() && IsEnableGauge( 0.1f ) )
	{
		if( pOwner->IsDefenseKeyDown() && IsEnableGauge( m_fHealNeedGauge ) )
		{
			ChangeToHealAreaWeaponState( pOwner );
			return;
		}
		if( m_dwMotionStartTime + m_dwCreateAreaWeaponTime < dwCurTime )
		{
			CreateAreaWeapon( pOwner, true );
			m_dwMotionStartTime = dwCurTime;
		}
	}
	else
		ChangeToCraeteAreaWeaponEndState( pOwner );
}

void ioGardenerItem::ChangeToCraeteAreaWeaponState( ioBaseChar *pOwner )
{
	if( !pOwner )
		return;

	ioEntityGroup *pGrp = pOwner->GetGroup();
	if( !pGrp )
		return;

	pOwner->SetAttackMoveEnable( false );
	pOwner->SetChargingState( false );
	pGrp->ClearAllLoopAni( FLOAT100, true );
	pGrp->ClearAllActionAni( FLOAT100, true );
	pGrp->SetLoopAni( m_szCreateAreaWeaponLoopAni, FLOAT100 );

	m_ChargeState = CS_CREATE_AREA_WEAPON;
	m_dwMotionStartTime = FRAMEGETTIME();
	CheckDecreaseGauge( m_fAreaNeedGauge );

	CreateAreaWeapon( pOwner, false );
	CreateAreaWeaponEffect( pOwner );

	if( pOwner->IsNeedSendNetwork() )
	{
		SP2Packet kPacket( CUPK_ITEM_GAUGE_CANCEL );
		FillItemGaugePacketHeader( pOwner, kPacket );
		kPacket << (int)m_ChargeState;
		kPacket << m_iCurCombo;
		kPacket << pOwner->GetTargetRot();
		kPacket << pOwner->GetWorldPosition();
		P2PNetwork::SendToAllPlayingUser( kPacket );
	}
}

void ioGardenerItem::CreateAreaWeapon( ioBaseChar *pOwner, bool bSendNetwork )
{
	if( !pOwner ) return;

	D3DXVECTOR3 vDir = pOwner->GetTargetRot() * D3DXVECTOR3( 0.0f, 0.0f, -FLOAT1 );
	D3DXVECTOR3 vPos = pOwner->GetWorldPosition() + vDir * m_fAreaWeaponOffset;
	D3DXVECTOR3 vNewPos = vPos;

	int iCount = m_AreaWeaponNameVec.size();
	int iIndex = iCount - 1;
	ioAreaWeapon *pAreaWeapon = NULL;
	for( ; iIndex >= 0; iIndex-- )
	{
		bool bBreak = false;
		pAreaWeapon = g_AreaWeaponMgr.FindAreaWeaponInRangeByName( m_AreaWeaponNameVec[iIndex].m_szName[0], pOwner->GetCharName(), vPos );
		if( pAreaWeapon )
		{
			vNewPos = pAreaWeapon->GetStartPos();
			bBreak = true;

			g_AreaWeaponMgr.DestroyAreaWeapon( pAreaWeapon->GetAreaWeaponIndex() );
		}
		pAreaWeapon = g_AreaWeaponMgr.FindAreaWeaponInRangeByName( m_AreaWeaponNameVec[iIndex].m_szName[1], pOwner->GetCharName(), vPos );
		if( pAreaWeapon )
		{
			vNewPos = pAreaWeapon->GetStartPos();
			bBreak = true;

			g_AreaWeaponMgr.DestroyAreaWeapon( pAreaWeapon->GetAreaWeaponIndex() );
		}
		if( bBreak )
			break;
	}
	if( iIndex != iCount - 1 )
		iIndex++;

	g_AreaWeaponMgr.CreateAreaWeapon( pOwner->GetCharName(), "", GetName(), m_AreaWeaponNameVec[iIndex].m_szName[0], 
									  vNewPos, pOwner->GetTargetRot(), ioAreaWeapon::CT_NORMAL );
	g_AreaWeaponMgr.CreateAreaWeapon( pOwner->GetCharName(), "", GetName(), m_AreaWeaponNameVec[iIndex].m_szName[1], 
									  vNewPos, pOwner->GetTargetRot(), ioAreaWeapon::CT_NORMAL );

	if( bSendNetwork && pOwner->IsNeedSendNetwork() )
	{
		SP2Packet kPacket( CUPK_ITEM_GAUGE_CANCEL );
		FillItemGaugePacketHeader( pOwner, kPacket );
		kPacket << (int)CS_CREATE_AREA_WEAPON_SYNC;
		kPacket << m_iCurCombo;
		kPacket << pOwner->GetTargetRot();
		kPacket << pOwner->GetWorldPosition();
		P2PNetwork::SendToAllPlayingUser( kPacket );
	}
}

void ioGardenerItem::ChangeGatheringState( ioBaseChar *pOwner )
{
	if( !pOwner ) return;

	ioEntityGroup *pGrp = pOwner->GetGroup();
	if( !pGrp )
		return;

	DWORD dwCurTime = FRAMEGETTIME();

	pGrp->ClearAllLoopAni( FLOAT100, true );
	pGrp->ClearAllActionAni( FLOAT100, true );

	int iAniID	     = pGrp->GetAnimationIdx( m_szGatheringAni );
	float fTimeRate  = m_fGatheringAniRate;

	pGrp->SetActionAni( iAniID, FLOAT100, FLOAT100, FLOAT1, FLOAT1/fTimeRate, 0.0f, 0.0f, true );

	m_ChargeState = CS_GATHERING;
	if( m_dwGatheringTime == 0 )
		m_dwMotionEndTime = dwCurTime + ( pGrp->GetAnimationFullTime( iAniID ) * fTimeRate );
	else
		m_dwMotionEndTime = dwCurTime + m_dwGatheringTime;

	if( pOwner->IsNeedSendNetwork() )
	{
		SP2Packet kPacket( CUPK_ITEM_GAUGE_CANCEL );
		FillItemGaugePacketHeader( pOwner, kPacket );
		kPacket << (int)m_ChargeState;
		kPacket << m_iCurCombo;
		kPacket << pOwner->GetTargetRot();
		kPacket << pOwner->GetWorldPosition();
		P2PNetwork::SendToAllPlayingUser( kPacket );
	}
}

void ioGardenerItem::ChangeToNormalAttack( ioBaseChar *pOwner )
{
	DWORD dwCurTime = FRAMEGETTIME();

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
			pOwner->SetAutoTarget( ATT_NORMAL );

		pOwner->SetNormalAttackByAttribute( *pAttribute );

		SetCurAttackAniForInputJump( pAttribute->m_AttackAnimation, pAttribute->m_fAttackAniRate );
	}

	pOwner->ApplyNormalAttackResult( m_iCurCombo );

	m_ChargeState = CS_NORMAL_ATTACK;

	if( pOwner->IsNeedSendNetwork() )
	{
		SP2Packet kPacket( CUPK_ITEM_GAUGE_CANCEL );
		FillItemGaugePacketHeader( pOwner, kPacket );
		kPacket << (int)m_ChargeState;
		kPacket << m_iCurCombo;
		kPacket << pOwner->GetTargetRot();
		kPacket << pOwner->GetWorldPosition();
		if( !m_bChargeAutoTarget )
			pOwner->FillAutoTargetInfo( kPacket );
		P2PNetwork::SendToAllPlayingUser( kPacket );
	}
}

void ioGardenerItem::OnReleased( ioBaseChar *pOwner )
{
	m_fCurGauge = 0.0f;
	DestroyAreaWeaponEffect( pOwner );

	ioWeaponItem::OnReleased( pOwner );
}

void ioGardenerItem::OnReleasedByEndGame( ioBaseChar *pOwner )
{
	m_fCurGauge = 0.0f;
	DestroyAreaWeaponEffect( pOwner );
}

void ioGardenerItem::SetJumpping( ioBaseChar *pOwner, bool bFullTime )
{
	if( bFullTime )
	{
		if( IsEnableMakeTreeDummy( pOwner ) )
		{
			ChangeToTreeDummyActionPre( pOwner );
			return;
		}
	}

	float fJumpPower = pOwner->GetJumpPower() * m_fJumpPowerRate;
	pOwner->SetJumpPower( fJumpPower );

	if( m_pExtendJump )
		m_pExtendJump->SetJumpping( pOwner, bFullTime );
}

void ioGardenerItem::OnItemGaugeCancel( ioBaseChar *pOwner, SP2Packet &rkPacket )
{
	int iState;
	rkPacket >> iState;
	rkPacket >> m_iCurCombo;

	D3DXQUATERNION qtRot;
	rkPacket >> qtRot;
	pOwner->SetTargetRotToRotate( qtRot, true );

	D3DXVECTOR3 vPos;
	rkPacket >> vPos;
	pOwner->SetWorldPosition( vPos );

	switch( iState )
	{
	case CS_NORMAL_ATTACK:
		{
			if( !m_bChargeAutoTarget )
				pOwner->ApplyAutoTargetInfo( rkPacket );

			ChangeToNormalAttack( pOwner );
		}
		break;
	case CS_GATHERING:
		ChangeGatheringState( pOwner );
		break;
	case CS_CREATE_AREA_WEAPON:
		ChangeToCraeteAreaWeaponState( pOwner );
		break;
	case CS_CREATE_AREA_WEAPON_SYNC:
		CreateAreaWeapon( pOwner, false );
		break;
	case CS_CREATE_AREA_WEAPON_END:
		ChangeToCraeteAreaWeaponEndState( pOwner );
		break;
	case CS_HEAL_AREA_WEAPON:
		ChangeToHealAreaWeaponState( pOwner );
		break;
	case CS_ACTION_TREE_DUMMY_PRE:
		ChangeToTreeDummyActionPre( pOwner );
		break;
	case CS_MAKE_WALL_DUMMY:
		{
			D3DXVECTOR3 vPos = ioMath::VEC3_ZERO;
			rkPacket >> vPos;

			int iNewDummyIndex = 0;
			int iDieDummyIndex = 0;
			rkPacket >> iNewDummyIndex;
			rkPacket >> iDieDummyIndex;

			ioDummyChar* pNewDummy = pOwner->GetCreator()->CreateDummyChar( m_szWallDummyName, iNewDummyIndex, pOwner->GetCharName(), vPos, 0.0f, 0, true );
			if( !pNewDummy )
			{
				LOG.PrintTimeAndLog(0, "%s Create Dummy Failed : %s", __FUNCTION__, m_szWallDummyName.c_str() );
				return;
			}

			ioDummyChar *pDieDummy = g_DummyCharMgr.FindDummyChar( pOwner, iDieDummyIndex );
			if( pDieDummy )
				pDieDummy->SetDieState( true );

			ChangeToWallDummyMake( pOwner->GetCreator(), pOwner );
		}
		break;
	case CS_MAKE_TREE_DUMMY:
		{
			D3DXVECTOR3 vPos = ioMath::VEC3_ZERO;
			rkPacket >> vPos;

			int iNewDummyIndex = 0;
			rkPacket >> iNewDummyIndex;
			ioDummyChar* pNewDummy = pOwner->GetCreator()->CreateDummyChar( m_szTreeDummyName, iNewDummyIndex, pOwner->GetCharName(), vPos, 0.0f, 0, true );
			if( !pNewDummy )
			{
				LOG.PrintTimeAndLog(0, "%s Create Dummy Failed : %s", __FUNCTION__, m_szTreeDummyName.c_str() );
				return;
			}
			ChangeToTreeDummyMake( pOwner->GetCreator(), pOwner );
		}
		break;
	case CS_RETREAT:
		{
			float fAniRate;
			D3DXVECTOR3 vDir;
			ioHashString szAni;

			rkPacket >> szAni >> fAniRate >> vDir;

			EndDefense( pOwner );
			ChangeToRetreatState( pOwner, szAni, fAniRate, vDir );
		}
		break;
	}
}

void ioGardenerItem::ChangeToWallDummyAction( ioBaseChar *pOwner )
{
	ioDummyGroup* pGroup = GetDummyGroup( pOwner );
	if( !pGroup )
		return;
	
	m_ChargeState = CS_ACTION_WALL_DUMMY;
	CheckDecreaseGauge( m_fWallNeedGauge );

	SetMakeDummyActionAni( pOwner, m_szWallDummyAni, m_fWallDummyAniRate );
}

void ioGardenerItem::ChangeToWallDummyMake( ioPlayStage* pStage, ioBaseChar *pOwner )
{
	ioDummyGroup* pGroup = GetDummyGroup( pOwner );
	if( !pGroup )
		return;
	
	m_ChargeState = CS_MAKE_WALL_DUMMY;
	if( pOwner->IsNeedProcess() )
	{
		//지정된 영역안의 더미 찾기
		if( m_szWallDummyName.IsEmpty() )
			return;

		//더미 생성
		D3DXVECTOR3 vDir = pOwner->GetTargetRot() * D3DXVECTOR3( 0.0f, 0.0f, -FLOAT1 );
		D3DXVECTOR3 vPos = pOwner->GetWorldPosition() + vDir * m_fWallDummyOffset;
		float fHeight = pStage->GetMapHeight( vPos.x, vPos.z );
		if( 0.0f < fHeight )
			vPos.y = fHeight;

		ioDummyChar* pNewDummy = pStage->CreateDummyChar( m_szWallDummyName, g_DummyCharMgr.GetCurLocalIndex(), pOwner->GetCharName(), vPos, 0.0f, 0, true );
		if( !pNewDummy )
			return;

		pGroup->AddDummy( pNewDummy->GetDummyCharIdx() );

		ioDummyChar* pDieDummy = NULL;
		int iCount = pGroup->GetDummyCount( m_szWallDummyName );
		if( iCount > m_iWallDummyMaxCount )
		{
			pDieDummy = pGroup->FirstDummyDie( pNewDummy );
		}

		if( pOwner->IsNeedSendNetwork() )
		{
			SP2Packet kPacket( CUPK_ITEM_GAUGE_CANCEL );
			FillItemGaugePacketHeader( pOwner, kPacket );
			kPacket << (int)m_ChargeState;
			kPacket << m_iCurCombo;
			kPacket << pOwner->GetTargetRot();
			kPacket << pOwner->GetWorldPosition();
			kPacket << vPos;
			kPacket << pNewDummy->GetDummyCharIdx();

			if( pDieDummy )
				kPacket << pDieDummy->GetDummyCharIdx();
			else
				kPacket << 0;

			P2PNetwork::SendToAllPlayingUser( kPacket );
		}
	}
}

void ioGardenerItem::ChangeToTreeDummyActionPre( ioBaseChar *pOwner )
{
	pOwner->SetState( CS_ATTACK );
	pOwner->SetCurNormalAttackItem( this );

	m_ChargeState = CS_ACTION_TREE_DUMMY_PRE;
	CheckDecreaseGauge( m_fTreeNeedGauge );

	if( pOwner->IsNeedSendNetwork() )
	{
		SP2Packet kPacket( CUPK_ITEM_GAUGE_CANCEL );
		FillItemGaugePacketHeader( pOwner, kPacket );
		kPacket << (int)m_ChargeState;
		kPacket << m_iCurCombo;
		kPacket << pOwner->GetTargetRot();
		kPacket << pOwner->GetWorldPosition();

		P2PNetwork::SendToAllPlayingUser( kPacket );
	}
}

void ioGardenerItem::ChangeToTreeDummyAction( ioBaseChar *pOwner )
{
	ioDummyGroup* pGroup = GetDummyGroup( pOwner );
	if( !pGroup )
		return;
	
	m_ChargeState = CS_ACTION_TREE_DUMMY;

	SetMakeDummyActionAni( pOwner, m_szTreeDummyAni, m_fTreeDummyAniRate );
}

void ioGardenerItem::ChangeToTreeDummyMake( ioPlayStage* pStage, ioBaseChar *pOwner )
{
	ioDummyGroup* pGroup = GetDummyGroup( pOwner );
	if( !pGroup )
		return;
	
	m_ChargeState = CS_MAKE_TREE_DUMMY;
	if( pOwner->IsNeedProcess() )
	{
		//지정된 영역안의 더미 찾기
		if( m_szTreeDummyName.IsEmpty() )
			return;

		//더미 생성
		D3DXVECTOR3 vDir = pOwner->GetTargetRot() * D3DXVECTOR3( 0.0f, 0.0f, -FLOAT1 );
		D3DXVECTOR3 vPos = pOwner->GetWorldPosition() + vDir * m_fTreeDummyOffset;
		float fHeight = pStage->GetMapHeight( vPos.x, vPos.z );
		if( 0.0f < fHeight )
			vPos.y = fHeight;

		ioDummyStruct* pNewDummy = dynamic_cast< ioDummyStruct* >( pStage->CreateDummyChar( m_szTreeDummyName, g_DummyCharMgr.GetCurLocalIndex(), pOwner->GetCharName(), vPos, 0.0f, 0, true ) );
		if( !pNewDummy )
			return;

		pGroup->AddDummy( pNewDummy->GetDummyCharIdx() );
		pNewDummy->SetGardenerTarget( pOwner->GetCharName() );

		D3DXVECTOR3 vNewPos = vPos + pNewDummy->GetGardenerOffset();
		pOwner->SetWorldPosition( vNewPos );
		pOwner->SetTargetRotToTargetPos( vNewPos + D3DXVECTOR3( 0.0f, 0.0f, -FLOAT1 ), true, true );
	
		if( pOwner->IsNeedSendNetwork() )
		{
			SP2Packet kPacket( CUPK_ITEM_GAUGE_CANCEL );
			FillItemGaugePacketHeader( pOwner, kPacket );
			kPacket << (int)m_ChargeState;
			kPacket << m_iCurCombo;
			kPacket << pOwner->GetTargetRot();
			kPacket << pOwner->GetWorldPosition();
			kPacket << vPos;
			kPacket << pNewDummy->GetDummyCharIdx();

			P2PNetwork::SendToAllPlayingUser( kPacket );
		}
	}
	pOwner->SetState( CS_GARDENER_DELAY_SPECIAL, false );
}

bool ioGardenerItem::IsEnableMakeTreeDummy( ioBaseChar *pOwner )
{
	if( !pOwner )
		return false;
	if( !IsEnableGauge( m_fTreeNeedGauge ) )
		return false;

	ioDummyGroup* pGroup = GetDummyGroup( pOwner );
	if( !pGroup )
		return false;

	int iCount = pGroup->GetDummyCount( m_szTreeDummyName );
	if( iCount != 0 )
		return false;

	return true;
}

void ioGardenerItem::ChangeToHealAreaWeaponState( ioBaseChar *pOwner )
{
	if( !pOwner ) return;

	ioEntityGroup *pGrp = pOwner->GetGroup();
	if( !pGrp )
		return;

	m_ChargeState = CS_HEAL_AREA_WEAPON;

	DWORD dwCurTime = FRAMEGETTIME();
	int iAniID = pGrp->GetAnimationIdx( m_szHealAreaWeaponAni );
	if( iAniID == -1 )
		return;

	pGrp->ClearAllLoopAni( FLOAT100, true );
	pGrp->ClearAllActionAni( FLOAT100, true );
	pGrp->SetActionAni( iAniID, FLOAT100, FLOAT100, FLOAT1, FLOAT1/m_fHealAreaWeaponAniRate );
	
	m_dwMotionEndTime = dwCurTime;
	m_dwMotionEndTime += pGrp->GetAnimationFullTime( iAniID ) * m_fHealAreaWeaponAniRate;

	D3DXVECTOR3 vDir = pOwner->GetTargetRot() * D3DXVECTOR3( 0.0f, 0.0f, -FLOAT1 );
	D3DXVECTOR3 vPos = pOwner->GetWorldPosition() + vDir * m_fHealAreaWeaponOffset;

	CheckDecreaseGauge( m_fHealNeedGauge );
	DestroyAreaWeaponEffect( pOwner );
	g_AreaWeaponMgr.CreateAreaWeapon( pOwner->GetCharName(), "", GetName(), m_szHealAreaWeaponName, 
									  vPos, pOwner->GetTargetRot(), ioAreaWeapon::CT_NORMAL );

	if( pOwner->IsNeedSendNetwork() )
	{
		SP2Packet kPacket( CUPK_ITEM_GAUGE_CANCEL );
		FillItemGaugePacketHeader( pOwner, kPacket );
		kPacket << (int)m_ChargeState;
		kPacket << m_iCurCombo;
		kPacket << pOwner->GetTargetRot();
		kPacket << pOwner->GetWorldPosition();
		P2PNetwork::SendToAllPlayingUser( kPacket );
	}
}

void ioGardenerItem::ChangeToCraeteAreaWeaponEndState( ioBaseChar *pOwner )
{
	if( !pOwner ) return;

	ioEntityGroup *pGrp = pOwner->GetGroup();
	if( !pGrp )
		return;

	m_ChargeState = CS_CREATE_AREA_WEAPON_END;

	DWORD dwCurTime = FRAMEGETTIME();
	int iAniID = pGrp->GetAnimationIdx( m_szCreateAreaWeaponEndAni );
	if( iAniID == -1 )
		return;

	pGrp->ClearAllLoopAni( FLOAT100, true );
	pGrp->ClearAllActionAni( FLOAT100, true );
	pGrp->SetActionAni( iAniID, FLOAT100, FLOAT100, FLOAT1, FLOAT1/m_fCreateAreaWeaponAniRate );
	
	m_dwMotionEndTime = dwCurTime;
	m_dwMotionEndTime += pGrp->GetAnimationFullTime( iAniID ) * m_fCreateAreaWeaponAniRate;

	DestroyAreaWeaponEffect( pOwner );

	if( pOwner->IsNeedSendNetwork() )
	{
		SP2Packet kPacket( CUPK_ITEM_GAUGE_CANCEL );
		FillItemGaugePacketHeader( pOwner, kPacket );
		kPacket << (int)m_ChargeState;
		kPacket << m_iCurCombo;
		kPacket << pOwner->GetTargetRot();
		kPacket << pOwner->GetWorldPosition();
		P2PNetwork::SendToAllPlayingUser( kPacket );
	}
}

ioDummyGroup* ioGardenerItem::GetDummyGroup( ioBaseChar* pOwner )
{
	ioDummyGroup* pDummyGroup = g_DummyCharMgr.GetDummyGroup( pOwner->GetCharName(), GetName() );
	if( !pDummyGroup )
		pDummyGroup = g_DummyCharMgr.AddDummyGroup( pOwner->GetCharName(), GetName() );

	return pDummyGroup;
}

int ioGardenerItem::SetMakeDummyActionAni( IN ioBaseChar* pOwner, IN const ioHashString& szAnimation, IN float fTimeRate )
{
	ioEntityGroup *pGrp = pOwner->GetGroup();
	if( pGrp )
	{
		pGrp->ClearAllActionAni( FLOAT100 );
		pGrp->ClearAllLoopAni( FLOAT100 );

		int iAniID = pGrp->GetAnimationIdx( szAnimation );
		pGrp->SetActionAni( iAniID, FLOAT100, FLOAT100, FLOAT1, FLOAT1 / fTimeRate, 0.0f, 0, true );
		m_dwDummyEndTime = FRAMEGETTIME() + pGrp->GetAnimationFullTime(iAniID) * fTimeRate;
		m_dwDummyCreateTime = FRAMEGETTIME() + pGrp->GetAnimationEventTime_e( iAniID, "fire_s" ) * fTimeRate;

		return iAniID;
	}

	return -1;
}

bool ioGardenerItem::SetExtendDefenseState( ioBaseChar *pOwner )
{
	if( pOwner->IsPrisonerMode() ) return false;
	if( pOwner->IsCatchMode() ) return false;
	if( pOwner->IsHasCrown() ) return false;

	pOwner->SetState( CS_EXTEND_DEFENSE );
	pOwner->SetChargingState( true );
	pOwner->SetDefenseMoveEnable( true, false );

	m_bDefenseFirstMoveMotion = true;
	m_bDefenseMotionSetted = true;

	m_bDefenseEnableSet = false;
	m_dwDefenseStartTime = FRAMEGETTIME();
	m_dwDefenseEndTime = 0;

	m_iSkillInput = -1;
	m_bEnableDefenseState = false;
	m_bEnableAttackState = false;
	m_bEnableJumpState = false;

	m_iCurCombo = 0;

	return true;
}

void ioGardenerItem::CheckExtendDefenseState( ioBaseChar *pOwner )
{
	DWORD dwCurTime = FRAMEGETTIME();

	if( pOwner->IsNeedProcess() && pOwner->IsDirKeyDoubleClick() )
	{
		if( CheckRetreat( pOwner ) )
		{
			if( pOwner->IsNeedSendNetwork() )
			{
				SP2Packet kPacket( CUPK_ITEM_GAUGE_CANCEL );
				FillItemGaugePacketHeader( pOwner, kPacket );
				kPacket << (int)m_ChargeState;
				kPacket << m_iCurCombo;
				kPacket << pOwner->GetTargetRot();
				kPacket << pOwner->GetWorldPosition();
				kPacket << m_szCurRetreatAni;
				kPacket << m_fCurRetreatAniRate;
				kPacket << m_vForceDir;
				P2PNetwork::SendToAllPlayingUser( kPacket );
			}
		}
		return;
	}

	if( !m_bDefenseEnableSet && m_dwDefenseStartTime+m_dwDefensePreDuration <= dwCurTime )
	{
		m_bDefenseEnableSet = true;
		pOwner->SetDefenseEnable( true );
	}

	if( m_dwDefenseEndTime > 0 )
	{
		if( m_dwDefenseEndTime + m_dwDefenseEndDuration < dwCurTime )
		{
			if( pOwner->IsNeedProcess() )
			{
				if( m_iSkillInput >= 0 && !pOwner->IsBuffLimitSkill() && pOwner->SetUseSkill( m_iSkillInput, SUT_NORMAL ) )
				{
					m_iSkillInput = -1;
				}
				else if( m_bEnableDefenseState && pOwner->IsEnableDefenseByObject() && !pOwner->IsBuffLimitDefense() )
				{
					pOwner->SetDefenseState();
				}
				else if( m_bEnableJumpState && pOwner->IsCanJumpState() && !pOwner->IsBuffLimitJump() )
				{
					pOwner->SetJumpState();
				}
				else if( m_bEnableAttackState )
				{
					if( pOwner->IsCanNormalAttack() && !pOwner->IsBuffLimitAttack() )
					{
						pOwner->SetNormalAttack( m_iCurCombo );
					}
				}
				else
				{
					pOwner->SetState( CS_DELAY );
				}
			}
			else
			{
				pOwner->SetState( CS_DELAY );
			}
		}
		else
		{
			if( pOwner->IsNeedProcess() )
			{
				bool bReserveSkill = false;
				int iSkillNum = pOwner->GetSkillKeyInput();
				if( iSkillNum >= 0 && !pOwner->IsChatModeState( true ) )
				{
					m_iSkillInput = iSkillNum;
					bReserveSkill = true;
				}

				if( pOwner->IsDefenseKeyDown() && !bReserveSkill && !m_bEnableDefenseState )
				{
					m_bEnableDefenseState = true;
					m_bEnableAttackState = false;
					m_bEnableJumpState = false;
				}
				else if( pOwner->IsAttackKey() && !bReserveSkill && !m_bEnableAttackState )
				{
					m_bEnableAttackState = true;
					m_bEnableDefenseState = false;
					m_bEnableJumpState = false;
				}
				else if( pOwner->IsJumpKey() && !bReserveSkill && !m_bEnableJumpState )
				{
					m_bEnableJumpState = true;
					m_bEnableAttackState = false;
					m_bEnableDefenseState = false;
				}
			}
		}

		return;
	}

	if( !pOwner->IsNeedProcess() ) return;

	if( !m_bDefenseEnableSet )
		return;

	if( m_dwDefenseStartTime+m_dwDefenseDuration > dwCurTime )
		return;

	if( m_dwExtendDefenseWoundedTime > 0 )
	{
		if( m_dwExtendDefenseWoundedTime > dwCurTime )
			return;
		else
			m_dwExtendDefenseWoundedTime = 0;
	}

	if( !pOwner->IsDefenseKeyDown() )
	{
		EndDefense( pOwner );

		if( pOwner->IsNeedSendNetwork() )
		{
			SP2Packet kPacket( CUPK_ITEM_GAUGE_CANCEL );
			FillItemGaugePacketHeader( pOwner, kPacket );
			kPacket << (int)m_ChargeState;
			kPacket << m_iCurCombo;
			kPacket << pOwner->GetTargetRot();
			kPacket << pOwner->GetWorldPosition();
			P2PNetwork::SendToAllPlayingUser( kPacket );
		}
	}
	else if( pOwner->IsAttackKeyDown() && pOwner->GetUseEnableSkill( 0, SUT_NORMAL ) )
	{
		int iSkillNum = pOwner->GetSkillKeyInput();
		if( iSkillNum == 0 && !pOwner->IsBuffLimitSkill() )
		{
			EndDefense( pOwner );

			if( pOwner->IsNeedSendNetwork() )
			{
				SP2Packet kPacket( CUPK_ITEM_GAUGE_CANCEL );
				FillItemGaugePacketHeader( pOwner, kPacket );
				kPacket << (int)m_ChargeState;
				kPacket << m_iCurCombo;
				kPacket << pOwner->GetTargetRot();
				kPacket << pOwner->GetWorldPosition();
				P2PNetwork::SendToAllPlayingUser( kPacket );
			}

			if( pOwner->SetUseSkill(0, SUT_NORMAL) )
			{
				return;
			}
		}
	}
}

void ioGardenerItem::CheckDefenseMoveAniState( ioBaseChar *pOwner )
{
	if( !pOwner->IsCanDefenseMove( false ) )	return;

	DWORD dwCurTime = FRAMEGETTIME();
	ioEntityGroup *pGrp = pOwner->GetGroup();
	int iAniID = 0;

	if( pOwner->IsSettedDirection() )
	{
		if( pOwner->IsChangeDirection() || m_bDefenseFirstMoveMotion )
		{
			m_bDefenseMotionSetted = false;
			m_bDefenseFirstMoveMotion = false;

			pGrp->ClearAllActionAni( FLOAT100, true );
			pGrp->ClearAllLoopAni( FLOAT100, true );

			CheckDefenseMoveAniDir( pOwner );

			g_TutorialMgr.ActionDefenseMove();
		}
	}
	else
	{
		if( !m_bDefenseMotionSetted )
		{
			pGrp->ClearAllLoopAni( FLOAT100, true );
			
			int iAniID = pOwner->GetDefenseAnimationIdx();
			pGrp->SetLoopAni( iAniID, 50 );

			m_bDefenseMotionSetted = true;
		}
	}
}

void ioGardenerItem::EndDefense( ioBaseChar *pOwner )
{
	pOwner->SetDefenseMoveEnable( false, false );
	pOwner->SetDefenseEnable( false );

	m_ChargeState = CS_NONE;
	m_dwDefenseEndTime = FRAMEGETTIME();

	ioEntityGroup *pGrp = pOwner->GetGroup();
	if( pGrp )
	{
		int iAniID = pOwner->GetDelayAnimationIdx();

		pGrp->ClearAllLoopAniExceptOne( iAniID, FLOAT100 );
		pGrp->ClearAllActionAni( FLOAT100, true );
		pGrp->SetLoopAni( iAniID, (float)m_dwDefenseEndDuration );
	}
}

void ioGardenerItem::CheckDefenseMoveAniDir( ioBaseChar *pOwner )
{
	if( !pOwner ) return;

	ioEntityGroup *pGrp = pOwner->GetGroup();
	if( !pGrp ) return;

	// KeyDir
	D3DXVECTOR3 vDefenseDir = pOwner->GetDefenseDir();
	D3DXVec3Normalize( &vDefenseDir, &vDefenseDir );

	ioUserKeyInput::DirKeyInput eDir = pOwner->CheckCurDirKey();
	float fYawD = 180.0f + 45.0f * ( eDir - ioUserKeyInput::DKI_UP );
	fYawD = ioMath::ArrangeHead( fYawD );

	D3DXQUATERNION qtRotate;
	D3DXQuaternionRotationYawPitchRoll( &qtRotate, DEGtoRAD( fYawD ), 0.0f, 0.0f );
	D3DXVECTOR3 vMoveDir = qtRotate * D3DXVECTOR3( 0.0f, 0.0f, -FLOAT1 );
	vMoveDir.y = 0.0f;
	D3DXVec3Normalize( &vMoveDir, &vMoveDir );

	D3DXQUATERNION qtDefenseRot;
	ioMath::CalcDirectionQuaternion( qtDefenseRot, -vDefenseDir );

	D3DXQUATERNION qtTargetRot;
	ioMath::CalcDirectionQuaternion( qtTargetRot, -vMoveDir );

	float fCurYaw, fTargetYaw;
	fCurYaw = RADtoDEG( ioMath::QuaterToYaw( qtDefenseRot ) );
	fCurYaw = ioMath::ArrangeHead( fCurYaw );

	fTargetYaw = RADtoDEG( ioMath::QuaterToYaw( qtTargetRot ) );
	fTargetYaw = ioMath::ArrangeHead( fTargetYaw );

	float fAngleGap = ioMath::AngleGap( fCurYaw, fTargetYaw );
	fAngleGap += 360.0f;
	int iSide = (int)fAngleGap % 360;

	int iCnt = iSide / 45;
	int iRest = iSide % 45;

	ioHashString szCurMoveAni;

	switch( iCnt )
	{
	case 0:
		if( !m_AniDefenseFrontMove.IsEmpty() )
			szCurMoveAni = m_AniDefenseFrontMove;
		break;
	case 7:
		if( !m_AniDefenseFrontMove.IsEmpty() )
			szCurMoveAni = m_AniDefenseFrontMove;
		break;
	case 1:
		if( iRest > 15 )
		{
			if( !m_AniDefenseRightMove.IsEmpty() )
				szCurMoveAni = m_AniDefenseRightMove;
		}
		else
		{
			if( !m_AniDefenseFrontMove.IsEmpty() )
				szCurMoveAni = m_AniDefenseFrontMove;
		}
		break;
	case 2:
		if( iRest <= 40 )
		{
			if( !m_AniDefenseRightMove.IsEmpty() )
				szCurMoveAni = m_AniDefenseRightMove;
		}
		else
		{
			if( !m_AniDefenseBackMove.IsEmpty() )
				szCurMoveAni = m_AniDefenseBackMove;
		}
		break;
	case 3:
	case 4:
		if( !m_AniDefenseBackMove.IsEmpty() )
			szCurMoveAni = m_AniDefenseBackMove;
		break;
	case 5:
		if( iRest > 5 )
		{
			if( !m_AniDefenseLeftMove.IsEmpty() )
				szCurMoveAni = m_AniDefenseLeftMove;
		}
		else
		{
			if( !m_AniDefenseBackMove.IsEmpty() )
				szCurMoveAni = m_AniDefenseBackMove;
		}
		break;
	case 6:
		if( iRest <= 30 )
		{
			if( !m_AniDefenseLeftMove.IsEmpty() )
				szCurMoveAni = m_AniDefenseLeftMove;
		}
		else
		{
			if( !m_AniDefenseFrontMove.IsEmpty() )
				szCurMoveAni = m_AniDefenseFrontMove;
		}
		break;
	}

	if( !szCurMoveAni.IsEmpty() )
	{
		pGrp->SetLoopAni( szCurMoveAni, FLOAT100, FLOAT1 );
	}
	else
	{
		LOG.PrintTimeAndLog( 0, "ioPowerChargeShieldItem::CheckDefenseMoveAniDir MoveAni Fail!" );
	}
}

bool ioGardenerItem::CheckRetreat( ioBaseChar *pOwner )
{
	if( !pOwner ) return false;

	DWORD dwCurTime = FRAMEGETTIME();

	ioEntityGroup *pGrp = pOwner->GetGroup();
	int iAniID = 0;

	// KeyDir
	ioUserKeyInput::DirKeyInput eDir = pOwner->CheckCurDirKey();
	float fYawD = 180.0f + 45.0f * ( eDir - ioUserKeyInput::DKI_UP );
	fYawD = ioMath::ArrangeHead( fYawD );

	D3DXQUATERNION qtRotate;
	D3DXQuaternionRotationYawPitchRoll( &qtRotate, DEGtoRAD( fYawD ), 0.0f, 0.0f );
	D3DXVECTOR3 vSightDir = qtRotate * D3DXVECTOR3( 0.0f, 0.0f, -FLOAT1 );
	D3DXVECTOR3 vMoveDir( vSightDir.x, 0.0f, vSightDir.z );
	D3DXVec3Normalize( &vMoveDir, &vMoveDir );

	D3DXQUATERNION qtTargetRot;
	ioMath::CalcDirectionQuaternion( qtTargetRot, -vMoveDir );

	float fCurYaw, fTargetYaw;
	fTargetYaw = RADtoDEG( ioMath::QuaterToYaw( qtTargetRot ) );
	fTargetYaw = ioMath::ArrangeHead( fTargetYaw );
	fCurYaw = RADtoDEG( ioMath::QuaterToYaw( pOwner->GetTargetRot() ) );
	fCurYaw = ioMath::ArrangeHead( fCurYaw );

	float fAngleGap = ioMath::AngleGap( fCurYaw, fTargetYaw );
	fAngleGap += 360.0f;
	int iSide = (int)fAngleGap % 360;

	int iCnt = iSide / 45;
	int iRest = iSide % 45;

	switch( iCnt )
	{
	case 0:
	case 7:
		break;
	case 1:
		if( iRest > 15 )
		{
			if( !m_szRetreatRt.IsEmpty() )
			{
				EndDefense( pOwner );
				ChangeToRetreatState( pOwner, m_szRetreatRt, m_fRetreatRtRate, vMoveDir );
				return true;
			}
		}
		break;
	case 2:
		if( iRest <= 40 )
		{
			if( !m_szRetreatRt.IsEmpty() )
			{
				EndDefense( pOwner );
				ChangeToRetreatState( pOwner, m_szRetreatRt, m_fRetreatRtRate, vMoveDir );
				return true;
			}
		}
		else
		{
			if( !m_szRetreatBack.IsEmpty() )
			{
				EndDefense( pOwner );
				ChangeToRetreatState( pOwner, m_szRetreatBack, m_fRetreatBackRate, vMoveDir );
				return true;
			}
		}
		break;
	case 3:
	case 4:
		if( !m_szRetreatBack.IsEmpty() )
		{
			EndDefense( pOwner );
			ChangeToRetreatState( pOwner, m_szRetreatBack, m_fRetreatBackRate, vMoveDir );
			return true;
		}
		break;
	case 5:
		if( iRest > 5 )
		{
			if( !m_szRetreatLt.IsEmpty() )
			{
				EndDefense( pOwner );
				ChangeToRetreatState( pOwner, m_szRetreatLt, m_fRetreatLtRate, vMoveDir );
				return true;
			}
		}
		else
		{
			if( !m_szRetreatBack.IsEmpty() )
			{
				EndDefense( pOwner );
				ChangeToRetreatState( pOwner, m_szRetreatBack, m_fRetreatBackRate, vMoveDir );
				return true;
			}
		}
		break;
	case 6:
		if( iRest <= 30 )
		{
			if( !m_szRetreatLt.IsEmpty() )
			{
				EndDefense( pOwner );
				ChangeToRetreatState( pOwner, m_szRetreatLt, m_fRetreatLtRate, vMoveDir );
				return true;
			}
		}
		break;
	}

	return false;
}

void ioGardenerItem::ChangeToRetreatState( ioBaseChar *pOwner, const ioHashString &szAni, float fAniRate, D3DXVECTOR3 vDir )
{
	if( !pOwner ) return;
	ioEntityGroup *pGrp = pOwner->GetGroup();
	if( !pGrp ) return;

	DWORD dwCurTime = FRAMEGETTIME();

	pOwner->ClearDirDoubleClick();
	pOwner->ClearReservedSliding();

	pOwner->SetForcePowerAmt( 0.0f );

	m_dwRetreatEndTime = 0;
	m_dwRetreatForceTime = 0;

	float fCurRate = fAniRate;
	if( fCurRate <= 0.0f )
		fCurRate = FLOAT1;

	m_ChargeState = CS_RETREAT;

	D3DXQUATERNION qtTargetRot;
	ioMath::CalcDirectionQuaternion( qtTargetRot, -vDir );

	pOwner->SetMoveDirByRotate( qtTargetRot );

	int iAniID = pGrp->GetAnimationIdx( szAni );
	pGrp->ClearAllActionAni( 0.0f );
	pGrp->ClearAllLoopAni( 0.0f );
	pGrp->SetActionAni( iAniID, FLOAT100, FLOAT100, FLOAT1, FLOAT1/fCurRate );

	m_dwRetreatEndTime = dwCurTime + pGrp->GetAnimationFullTime(iAniID) * fCurRate;

	DWORD dwTime = (DWORD)pGrp->GetAnimationEventTime_e( iAniID, "fire_sliding" );
	m_dwRetreatForceTime = dwCurTime + dwTime;

	m_szCurRetreatAni = szAni;
	m_fCurRetreatAniRate = fCurRate;
	m_vForceDir = vDir;

	m_dwKeyReserveTime = 0;

	m_iSkillInput = -1;
	m_bEnableDefenseState = false;
	m_bEnableAttackState = false;
	m_bEnableJumpState = false;

	DWORD dwKeyReserve = pGrp->GetAnimationEventTime_e( iAniID, "key_reserve" ) * fCurRate;
	if( dwKeyReserve > 0 )
		m_dwKeyReserveTime = dwCurTime + dwKeyReserve;

	pOwner->CheckCharColSkipTime( iAniID, fCurRate, 0 );
	pOwner->SetState( CS_RETREAT_STATE );
}

void ioGardenerItem::CheckRetreatState( ioBaseChar *pOwner )
{
	DWORD dwCurTime = FRAMEGETTIME();

	CheckAniRotate( pOwner );

	if( m_dwRetreatEndTime > 0 && m_dwRetreatEndTime < dwCurTime )
	{
		// 공격키가 눌러져 있으면 공격 상태 처리
		if( pOwner->IsAttackKeyDown() && 
			!pOwner->IsJumpKeyDown() &&
			!pOwner->IsDefenseKeyDown() &&
			!pOwner->IsChatModeState(true) )
		{
			ClearState();

			if( pOwner->IsCanNormalAttack() && !pOwner->IsBuffLimitAttack() )
			{
				pOwner->SetNormalAttack( 0 );
			}
			return;
		}

		// 필살기 예약되었으면 필살기 발동
		if( m_iSkillInput >= 0 && !pOwner->IsBuffLimitSkill() && pOwner->SetUseSkill( m_iSkillInput, SUT_NORMAL ) )
		{
			ClearState();
			return;
		}

		// 방어
		if( m_bEnableDefenseState && pOwner->IsEnableDefenseByObject() && !pOwner->IsBuffLimitDefense() )
		{
			ClearState();
			pOwner->SetDefenseState();
			return;
		}

		// 점프
		if( m_bEnableJumpState && pOwner->IsCanJumpState() && !pOwner->IsBuffLimitJump() )
		{
			ClearState();
			pOwner->SetJumpState();
			return;
		}

		// 공격
		if( m_bEnableAttackState )
		{
			ClearState();
			if( pOwner->IsCanNormalAttack() && !pOwner->IsBuffLimitAttack() )
			{
				pOwner->SetNormalAttack( 0 );
			}
			return;
		}

		ClearState();
		pOwner->SetState( CS_DELAY );
		return;
	}
	else
	{
		if( m_dwKeyReserveTime > 0 && m_dwKeyReserveTime < dwCurTime )
		{
			CheckReserve( pOwner );
		}

		if( m_dwRetreatForceTime > 0 && m_dwRetreatForceTime < dwCurTime )
		{
			m_dwRetreatForceTime = 0;
			pOwner->SetForcePower( m_vForceDir, m_fRetreatForceAmt, m_fRetreatForceFric );
		}
	}
}

void ioGardenerItem::CreateAreaWeaponEffect( ioBaseChar *pChar )
{
	if( !pChar )
		return;

	if( !m_szAreaWeaponMeshName.IsEmpty() )
		pChar->AddEntity( m_szAreaWeaponMeshName );
	if( m_dwAreaWeaponEffectIndex == 0 )
	{
		ioEntityGroup *pGrp = pChar->GetGroup();

		if( pGrp )
		{
			ioEffect *pEffect = pGrp->AttachEffect( m_szAreaWeaponEffectName, NULL, ioMath::QUAT_IDENTITY );
			if( !pEffect )	
				return;

			m_dwAreaWeaponEffectIndex = pEffect->GetUniqueID();
		}
	}
}

void ioGardenerItem::DestroyAreaWeaponEffect( ioBaseChar *pChar )
{
	if( !pChar )
		return;

	if( !m_szAreaWeaponMeshName.IsEmpty() )
		pChar->DestroyEntity( m_szAreaWeaponMeshName );
	if( m_dwAreaWeaponEffectIndex != 0 )
	{
		ioEffect *pEffect = g_EffectFactory.GetEffect( m_dwAreaWeaponEffectIndex );
		if( pEffect )
			pEffect->EndEffectForce();
	}
	m_dwAreaWeaponEffectIndex = 0;
}

void ioGardenerItem::CheckDecreaseGauge( float fNeedGauge )
{
	m_fCurGauge -= fNeedGauge;
	m_fCurGauge = (float)max( 0, min( m_fCurGauge, m_fMaxGauge ) );
}

int ioGardenerItem::GetMaxBullet()
{
	return (int)m_fMaxGauge;
}

int ioGardenerItem::GetNeedBullet()
{
	return (int)m_fAreaNeedGauge;
}

int ioGardenerItem::GetCurBullet()
{
	float fCurCoolTime = m_fCurGauge;
	fCurCoolTime = max( 0, min( fCurCoolTime, m_fMaxGauge ) );

	return (int)fCurCoolTime;
}

void ioGardenerItem::SetCurBullet( int iCurBullet )
{
	if( iCurBullet < 0 )
		return;

	float fCurBullet = iCurBullet;
	fCurBullet = max( 0, min( fCurBullet, m_fMaxGauge ) );

	m_fCurGauge = fCurBullet;
}

bool ioGardenerItem::IsEnableGauge( float fNeedGauge )
{
	if( m_fCurGauge >= fNeedGauge )
		return true;

	return false;
}

void ioGardenerItem::UpdateExtraData( ioBaseChar *pOwner )
{
	if( !pOwner )
		return;

	float fTimePerSec = g_FrameTimer.GetSecPerFrame();
	switch( pOwner->GetState() )
	{
	case CS_DIE:
	case CS_DROP_ZONE_DOWN:
	case CS_READY:
	case CS_VIEW:
	case CS_OBSERVER:
	case CS_LOADING:
	case CS_FISHING:
	case CS_STARTMOTION:
	case CS_ENDMOTION:
	case CS_EXCAVATING:
	case CS_CREATEMOTION:
	case CS_ROULETTE:
		return;
	case CS_ATTACK:
		if( CS_CREATE_AREA_WEAPON == m_ChargeState && m_fCurGauge > 0.f )
		{
			float fGauge = m_fAreaDecreaseGauge * fTimePerSec;

			m_fCurGauge -= fGauge;
			m_fCurGauge = max( m_fCurGauge, 0.f );
		}
		return;
	case CS_DELAY:
	case CS_RUN:
	case CS_DASH:
		if( m_fCurGauge < m_fMaxGauge )
		{
			float fGauge = m_fIncreaseGauge * fTimePerSec;

			m_fCurGauge += fGauge;
			m_fCurGauge = min( m_fCurGauge, m_fMaxGauge );
		}
		return;
	}
}
//////////////////////////////////////////////////////////////////////////////////
ioGardenerDelaySpecialState::ioGardenerDelaySpecialState()
{
}

ioGardenerDelaySpecialState::~ioGardenerDelaySpecialState()
{
}

void ioGardenerDelaySpecialState::Initialize( ioBaseChar* pOwner, CharState eCurrState )
{
	ioSpecialStateBase::Initialize( pOwner, eCurrState );

	if ( !pOwner )
		return;

	int nBuffCnt = (int)m_OwnerBuffList.size();	
	for ( int i=0; i<nBuffCnt; ++i )
		pOwner->AddNewBuff( m_OwnerBuffList[i], pOwner->GetCharName(), "", NULL );

	ioEntityGroup *pGrp = pOwner->GetGroup();
	if( !pGrp )
		return;

	pOwner->ClearReservedSliding();
	pOwner->SetForcePowerAmt( 0.0f );
	pOwner->ClearDirDoubleClick();

	int iAniID = pGrp->GetAnimationIdx( m_szStartAni );
	float fTimeRate = m_fStartAniRate;

	pOwner->InitExtendAttackTagInfo();
	pOwner->ClearReserveKeyInput();
	pGrp->ClearAllLoopAni( FLOAT10, true );
	pGrp->ClearAllActionAni( FLOAT10, true );

	DWORD dwPreDelay = 0;
	pGrp->SetActionAni( iAniID, FLOAT10, FLOAT10, FLOAT1, FLOAT1/fTimeRate, 0.0f, dwPreDelay, true );

	m_dwMotionEndTime = FRAMEGETTIME();
	m_dwMotionEndTime += ( pGrp->GetAnimationFullTime( iAniID ) * fTimeRate);
	m_State = GS_START;
}

void ioGardenerDelaySpecialState::Finalize( ioBaseChar* pOwner, CharState eCurrState, CharState eNewState )
{
	ioSpecialStateBase::Finalize( pOwner, eCurrState, eNewState );

	if ( !pOwner )
		return;

	int nBuffCnt = (int)m_OwnerBuffList.size();	
	for ( int i=0; i<nBuffCnt; ++i )
		pOwner->RemoveBuff( m_OwnerBuffList[i], true );
}

void ioGardenerDelaySpecialState::CheckSpecialState( ioBaseChar* pOwner )
{
	DWORD dwCurTime = FRAMEGETTIME();
	switch( m_State )
	{
	case GS_START:
		pOwner->SetCurMoveSpeed( 0 );
		if( dwCurTime > m_dwMotionEndTime )
		{
			ChangeDelayState( pOwner );
			return;
		}
		break;
	case GS_DELAY:
		if( pOwner->IsNeedProcess() )
		{
			if( pOwner->IsJumpKey() )
			{
				pOwner->ChangeDirectionByInputDir( false );
				pOwner->SetSKillEndJumpState( m_fEndJumpPower, false, false, true, true );
				return;
			}
			else if( pOwner->IsDefenseKey() )
			{
				pOwner->ChangeDirectionByInputDir( false );
				pOwner->SetSKillEndJumpState( m_fDefenseEndJumpPower, false, false, true, true );
				return;
			}
			else if( pOwner->IsAttackKey() )
			{
				ChangeGardenerAttackSpecial( pOwner );
				return;
			}
		}
		break;
	case GS_END:
		pOwner->SetCurMoveSpeed( 0 );
		if( dwCurTime > m_dwMotionEndTime )
		{
			pOwner->SetState( CS_DELAY );
			return;
		}
		break;
	}
	pOwner->SetCurJumpPowerAmt( 0.0f );
	pOwner->SetForcePowerAmt( 0.0f );
	pOwner->SetGravityAmt( 0.0f );
}

void ioGardenerDelaySpecialState::CheckFallStateChange( ioBaseChar* pOwner, float fMinFallHeightRate, float fBottomHeight, bool bBottomMaxCheck, float fMaxBottomHeight )
{
	return;
}

bool ioGardenerDelaySpecialState::IsNoDropState( ioBaseChar* pOwner )
{
	return true;

}
void ioGardenerDelaySpecialState::LoadProperty( ioINILoader &rkLoader )
{
	ioSpecialStateBase::LoadProperty( rkLoader );

	m_OwnerBuffList.clear();

	int iCnt = rkLoader.LoadInt_e( "owner_buff_cnt", 0 );
	char szTitle[MAX_PATH], szBuf[MAX_PATH];
	for( int i=0; i < iCnt; ++i )
	{
		wsprintf_e( szTitle, "owner_buff%d", i+1 );
		rkLoader.LoadString( szTitle, "", szBuf, MAX_PATH );

		m_OwnerBuffList.push_back( szBuf );
	}

	rkLoader.LoadString_e( "start_ani", "", szBuf, MAX_PATH );
	m_szStartAni = szBuf;
	m_fStartAniRate = rkLoader.LoadFloat_e( "start_ani_rate", FLOAT1 );

	rkLoader.LoadString_e( "delay_ani", "", szBuf, MAX_PATH );
	m_szDelayAni = szBuf;
	m_fDelayAniRate = rkLoader.LoadFloat_e( "delay_ani_rate", FLOAT1 );

	m_fEndJumpPower = rkLoader.LoadFloat_e( "end_jump_power", FLOAT1 );
	m_fDefenseEndJumpPower = rkLoader.LoadFloat_e( "defense_end_jump_power", FLOAT1 );
}

void ioGardenerDelaySpecialState::ChangeDelayState( ioBaseChar *pOwner )
{
	if( !pOwner )
		return;

	ioEntityGroup *pGrp = pOwner->GetGroup();
	if( !pGrp )
		return;

	pOwner->InitExtendAttackTagInfo();
	pOwner->ClearReservedSliding();
	pOwner->SetForcePowerAmt( 0.0f );

	pGrp->ClearAllLoopAni( FLOAT100, true );
	pGrp->ClearAllActionAni( FLOAT100, true );

	int iAniID	     = pGrp->GetAnimationIdx( m_szDelayAni );
	float fTimeRate  = m_fDelayAniRate;

	pGrp->SetLoopAni( iAniID, FLOAT100, FLOAT1, FLOAT1/fTimeRate );

	m_State = GS_DELAY;

	if( pOwner->IsNeedSendNetwork() )
	{
		SP2Packet kPacket( CUPK_SPECIAL_STATE_SYNC );
		kPacket << m_pOwner->GetCharName();
		kPacket << pOwner->GetTargetRot();
		kPacket	<< pOwner->GetWorldPosition();
		kPacket << m_State;
		P2PNetwork::SendToAllPlayingUser( kPacket );
	}
}

void ioGardenerDelaySpecialState::ChangeGardenerAttackSpecial( ioBaseChar *pOwner )
{
	if( !pOwner )
		return;

	pOwner->ChangeDirectionByInputDir( false );
	m_State = GS_SPECIAL;

	if( pOwner->IsNeedSendNetwork() )
	{
		SP2Packet kPacket( CUPK_SPECIAL_STATE_SYNC );
		kPacket << m_pOwner->GetCharName();
		kPacket << pOwner->GetTargetRot();
		kPacket	<< pOwner->GetWorldPosition();
		kPacket << m_State;
		P2PNetwork::SendToAllPlayingUser( kPacket );
	}
	pOwner->SetState( CS_GARDENER_ATTACK_SPECIAL, false );
}

void ioGardenerDelaySpecialState::ApplyStateSync( SP2Packet &rkPacket, ioBaseChar* pOwner )
{
	ioSpecialStateBase::ApplyStateSync( rkPacket, pOwner );

	D3DXQUATERNION qtRot;
	rkPacket >> qtRot;
	pOwner->SetTargetRotToRotate( qtRot, true );

	D3DXVECTOR3 vPos;
	rkPacket >> vPos;
	pOwner->SetWorldPosition( vPos );

	int eState;
	rkPacket >> eState;

	switch( eState )
	{
	case GS_DELAY:
		ChangeDelayState( pOwner );
		break;
	case GS_SPECIAL:
		ChangeGardenerAttackSpecial( pOwner );
		break;
	}
}
//////////////////////////////////////////////////////////////////////////////////
ioGardenerAttackSpecialState::ioGardenerAttackSpecialState()
{
}

ioGardenerAttackSpecialState::~ioGardenerAttackSpecialState()
{
}

void ioGardenerAttackSpecialState::Initialize( ioBaseChar* pOwner, CharState eCurrState )
{
	ioSpecialStateBase::Initialize( pOwner, eCurrState );

	ioEntityGroup *pGrp = pOwner->GetGroup();
	if( !pGrp )
		return;

	int iAniID = pGrp->GetAnimationIdx( m_szStartAni );
	float fTimeRate = m_fStartAniRate;

	pOwner->InitExtendAttackTagInfo();
	pOwner->ClearReserveKeyInput();
	pGrp->ClearAllLoopAni( FLOAT10, true );
	pGrp->ClearAllActionAni( FLOAT10, true );

	DWORD dwPreDelay = 0;
	pGrp->SetActionAni( iAniID, FLOAT10, FLOAT10, FLOAT1, FLOAT1/fTimeRate, 0.0f, dwPreDelay, true );

	m_State = GAS_DELAY;

	float fCurGravity = pOwner->GetCharGravity();
	m_fFlightDuration = GetFlightDuration( fCurGravity );

	if( m_fFlightDuration > 0.0f )
	{
		m_fCharMoveSpeed = m_fCircleOffSet / (m_fFlightDuration / FLOAT1000);
	}

	if( !pOwner->CheckMagicCircle() )
	{
		ioHashString szDummyCircle;
		pOwner->CreateMagicCircle( m_ReadyCircle, m_ReadyCircle, szDummyCircle, m_fCircleOffSet );	
	}

	pOwner->UpdateMagicCirclePos( 0.0f, m_fCircleOffSet, m_fCircleHeightGap, true, false );
	m_vCirclePos = pOwner->GetMagicCirclePos( m_fCircleOffSet, false );

	// CharMove
	m_dwActionStartTime = FRAMEGETTIME();
	m_vCharMoveDir = ioMath::VEC3_ZERO;

	D3DXVECTOR3 vDir = m_vCirclePos - pOwner->GetWorldPosition();
	vDir.y = 0.0f;

	float fRate = FLOAT1;
	float fRange = D3DXVec3Length( &vDir );
	if( fRange > 5.0f )
	{
		D3DXVec3Normalize( &m_vCharMoveDir, &vDir );

		D3DXQUATERNION qtRot;
		ioMath::CalcDirectionQuaternion( qtRot, -m_vCharMoveDir );
		pOwner->SetMoveDirByRotate( qtRot );
	}

	pOwner->SetJumpPower( m_fCharJumpPower );
	m_vCircleMoveDir = m_vCharMoveDir;

	if( pOwner->IsNeedSendNetwork() )
	{
		SP2Packet kPacket( CUPK_SPECIAL_STATE_SYNC );
		kPacket << m_pOwner->GetCharName();
		kPacket << pOwner->GetTargetRot();
		kPacket	<< pOwner->GetWorldPosition();
		kPacket << m_State;
		kPacket << m_vCirclePos;
		kPacket << m_vCircleMoveDir;
		P2PNetwork::SendToAllPlayingUser( kPacket );
	}
}

void ioGardenerAttackSpecialState::Finalize( ioBaseChar* pOwner, CharState eCurrState, CharState eNewState )
{
	ioSpecialStateBase::Finalize( pOwner, eCurrState, eNewState );

	if( pOwner )
		pOwner->DestroyMagicCircle();
}

void ioGardenerAttackSpecialState::CheckSpecialState( ioBaseChar* pOwner )
{
	DWORD dwCurTime = FRAMEGETTIME();
	switch( m_State )
	{
	case GAS_DELAY:
	case GAS_MOVE:
		if( CheckLanding( pOwner ) )
		{
			SetEndState( pOwner );
		}
		else
		{
			ioPlayStage *pStage = pOwner->GetCreator();
			if( !pStage )
				return;

			CheckControl( pOwner );
			ProcessControl( pOwner, pStage );

			CheckMove( pOwner, pStage );
			ProcessMove( pOwner, pStage );
		}
		break;
	case GAS_END:
		ProcessEnd( pOwner );
		break;
	}
}

void ioGardenerAttackSpecialState::CheckFallStateChange( ioBaseChar* pOwner, float fMinFallHeightRate, float fBottomHeight, bool bBottomMaxCheck, float fMaxBottomHeight )
{
	return;
}

void ioGardenerAttackSpecialState::LoadProperty( ioINILoader &rkLoader )
{
	ioSpecialStateBase::LoadProperty( rkLoader );

	char szKey[MAX_PATH], szBuf[MAX_PATH];

	rkLoader.LoadString_e( "start_ani", "", szBuf, MAX_PATH );
	m_szStartAni = szBuf;
	m_fStartAniRate = rkLoader.LoadFloat_e( "start_ani_rate", FLOAT1 );

	rkLoader.LoadString_e( "end_ani", "", szBuf, MAX_PATH );
	m_szEndAni = szBuf;
	m_fEndAniRate = rkLoader.LoadFloat_e( "end_ani_rate", FLOAT1 );

	m_fCircleMoveSpeed = rkLoader.LoadFloat_e( "magic_circle_move_speed", 0.0f );
	m_fCircleHeightGap = rkLoader.LoadFloat_e( "magic_circle_height_gap", 0.0f );
	m_fCircleOffSet = rkLoader.LoadFloat_e( "magic_circle_offset", 0.0f );

	rkLoader.LoadString_e( "ready_circle", "", szBuf, MAX_PATH );
	m_ReadyCircle = szBuf;

	m_fCharJumpPower = rkLoader.LoadFloat_e( "skill_jump_power", 0.0f );
	m_fSkillGravityRate = rkLoader.LoadFloat_e( "skill_gravity_rate", 0.0f );

	int iAttackTypeCnt = rkLoader.LoadInt_e( "land_attack_type_count", 1 );
	for( int i=0 ; i<iAttackTypeCnt ; i++ )
	{
		WeaponInfo kInfo;

		wsprintf_e( szKey, "land_attack_type%d", i+1 );
		kInfo.m_iWeaponIdx = rkLoader.LoadInt( szKey, NO_WEAPON_ATTRIBUTE );

		wsprintf_e( szKey, "land_attack_type%d_resist", i+1 );
		kInfo.m_iResistanceIdx = rkLoader.LoadInt( szKey, NO_WEAPON_ATTRIBUTE );

		wsprintf_e( szKey, "land_attack_type%d_wounded", i+1 );
		rkLoader.LoadString( szKey, "", szBuf, MAX_PATH );
		kInfo.m_WoundedAnimation = szBuf;

		wsprintf_e( szKey, "land_attack_type%d_wounded_time", i+1 );
		kInfo.m_dwWoundedDuration = rkLoader.LoadInt( szKey, 0 );

		wsprintf_e( szKey, "land_attack_type%d_wounded_loop_ani", i+1 );
		kInfo.m_bWoundedLoopAni = rkLoader.LoadBool( szKey, false );

		m_vWeaponInfoList.push_back( kInfo );
	}
}

float ioGardenerAttackSpecialState::GetFlightDuration( float fCurGravity )
{
	float fTopTime, fTopHeight;
	fTopTime = fTopHeight = 0.0f;

	float fNewGravity = fCurGravity * m_fSkillGravityRate;
	if( fNewGravity > 0.0f )
	{
		fTopTime = m_fCharJumpPower / fNewGravity;
		fTopHeight = (m_fCharJumpPower*fTopTime) - (0.5f*fNewGravity*fTopTime*fTopTime);
	}

	float fResult = 2 * fTopTime * FLOAT1000;

#ifndef SHIPPING 
	if( Setting::ShowExtraInfo() )
	{
		LOG.PrintTimeAndLog( 0, "ioTargetMoveAttackSkill - [CheckValue] TopHeight: %.2f, FlightDuration: %.2f", fTopHeight, fResult );
	}
#endif

	return fResult;
}

void ioGardenerAttackSpecialState::CheckControl( ioBaseChar *pChar )
{
	if( !pChar ) return;
	if( !pChar->IsNeedProcess() ) return;

	bool bUpdate = false;
	if( m_State == GAS_DELAY )
	{
		if( pChar->IsSettedDirection() )
		{
			ioUserKeyInput::DirKeyInput eDir = pChar->CheckCurDirKey();
			if( eDir != ioUserKeyInput::DKI_NONE )
			{
				float fYawD = 180.0f + 45.0f * ( eDir - ioUserKeyInput::DKI_UP );
				fYawD = ioMath::ArrangeHead( fYawD );

				float fYawR = DEGtoRAD( fYawD );
				D3DXQUATERNION qtRotate;
				D3DXQuaternionRotationYawPitchRoll( &qtRotate, fYawR, 0.0f, 0.0f );

				m_vCircleMoveDir = qtRotate * D3DXVECTOR3( 0.0f, 0.0f, -FLOAT1 );
			}

			m_State = GAS_MOVE;
			bUpdate = true;
		}
	}
	else if( m_State == GAS_MOVE )
	{
		if( !pChar->IsSettedDirection() )
		{
			m_State = GAS_DELAY;
			bUpdate = true;
		}
		else
		{
			if( pChar->IsChangeDirection() )
			{
				ioUserKeyInput::DirKeyInput eDir = pChar->CheckCurDirKey();
				if( eDir != ioUserKeyInput::DKI_NONE )
				{
					float fYawD = 180.0f + 45.0f * ( eDir - ioUserKeyInput::DKI_UP );
					fYawD = ioMath::ArrangeHead( fYawD );

					float fYawR = DEGtoRAD( fYawD );
					D3DXQUATERNION qtRotate;
					D3DXQuaternionRotationYawPitchRoll( &qtRotate, fYawR, 0.0f, 0.0f );

					m_vCircleMoveDir = qtRotate * D3DXVECTOR3( 0.0f, 0.0f, -FLOAT1 );
				}

				bUpdate = true;
			}
		}
	}

	if( bUpdate && pChar->IsNeedSendNetwork() )
	{
		SP2Packet kPacket( CUPK_SPECIAL_STATE_SYNC );
		kPacket << m_pOwner->GetCharName();
		kPacket << pChar->GetTargetRot();
		kPacket	<< pChar->GetWorldPosition();
		kPacket << m_State;
		kPacket << m_vCirclePos;
		kPacket << m_vCircleMoveDir;
		P2PNetwork::SendToAllPlayingUser( kPacket );
	}
}

void ioGardenerAttackSpecialState::ProcessControl( ioBaseChar *pChar, ioPlayStage *pStage )
{
	if( m_State == GAS_MOVE && m_fCircleMoveSpeed > 0.0f )
	{
		D3DXVECTOR3 vPrePos, vCurPos, vPos;
		vPos = m_vCircleMoveDir;
		vPos *= m_fCircleMoveSpeed * g_FrameTimer.GetSecPerFrame();

		vPrePos = vCurPos = m_vCirclePos;
		vCurPos += vPos;

		float fMapHeight = pStage->GetMapHeight( vCurPos.x, vCurPos.z, pChar, false );
		if( fMapHeight > 0.0f )
			vCurPos.y = fMapHeight;

		m_vCirclePos = vCurPos;

		if( pChar )
			pChar->SetMagicCirclePos( true, m_vCirclePos );
	}
}

void ioGardenerAttackSpecialState::CheckMove( ioBaseChar *pChar, ioPlayStage *pStage )
{
	DWORD dwCurTime = FRAMEGETTIME();
	D3DXVECTOR3 vDir = m_vCirclePos - pChar->GetWorldPosition();
	vDir.y = 0.0f;

	float fRate = FLOAT1;
	float fRange = D3DXVec3Length( &vDir );
	if( fRange > 5.0f )
	{
		float fGapTime = m_fFlightDuration - (dwCurTime - m_dwActionStartTime);

		float fNewSpeed = m_fCurCharMoveSpeed;
		if( fGapTime > 0 )
		{
			fNewSpeed = fRange / (float)(fGapTime / FLOAT1000);
		}

		m_fCurCharMoveSpeed = fNewSpeed;

		D3DXVec3Normalize( &m_vCharMoveDir, &vDir );

		D3DXQUATERNION qtRot;
		ioMath::CalcDirectionQuaternion( qtRot, -m_vCharMoveDir );
		pChar->SetMoveDirByRotate( qtRot );
	}
}

void ioGardenerAttackSpecialState::ProcessMove( ioBaseChar *pChar, ioPlayStage *pStage )
{
	if( !pChar ) return;
	if( !pStage ) return;
	if( m_fCurCharMoveSpeed == 0.0f ) return;

	float fTimePerSec = g_FrameTimer.GetSecPerFrame();

	bool bCurMove = false;
	D3DXVECTOR3 vTotalMove( 0.0f, 0.0f, 0.0f );

	float fCurTimeGap = fTimePerSec * FLOAT1000;
	while( fCurTimeGap > 0.0f )
	{
		float fDivideTimePerSec = min( fCurTimeGap, MIN_FRAME_TIME ) * 0.001f;
		fCurTimeGap -= MIN_FRAME_TIME;

		D3DXVECTOR3 vMoveAmt = m_vCharMoveDir * m_fCurCharMoveSpeed * fDivideTimePerSec;
		if( vMoveAmt != ioMath::VEC3_ZERO )
		{
			vTotalMove += vMoveAmt;
			bCurMove = true;
		}
	}

	if( bCurMove )
	{
		bool bCol = false;
		if( pStage->ReCalculateMoveVectorByTerrain( pChar, &vTotalMove, bCol ) )
		{
			pChar->Translate( vTotalMove );
		}
	}

	D3DXVECTOR3 vPos = pChar->GetWorldPosition();
	if( pChar->CheckAxisLimit( &vPos ) )
		pChar->SetWorldPosition( vPos );
}

bool ioGardenerAttackSpecialState::CheckLanding( ioBaseChar *pChar )
{
	if( !pChar ) return true;

	DWORD dwCurTime = FRAMEGETTIME();

	float fHeightGap = 0.0f;
	if( pChar->CheckGhostState() )
		fHeightGap = pChar->GetWorldPosition().y - pChar->GetPreHeightForGhostState();
	else
		fHeightGap = pChar->GetWorldPosition().y - pChar->GetBottomHeight();
	
	if( fHeightGap > 0.0f || !pChar->IsDownState() )
	{
		return false;
	}

	return true;
}

void ioGardenerAttackSpecialState::SetEndState( ioBaseChar *pChar )
{
	m_State = GAS_END;
	DWORD dwCurTime = FRAMEGETTIME();

	pChar->DestroyMagicCircle();

	ioEntityGroup *pGrp = pChar->GetGroup();
	int iCurSkillAniID = pGrp->GetAnimationIdx( m_szEndAni );
	if( iCurSkillAniID == -1 )	return;

	float fCurEndRate = m_fEndAniRate;
	if( fCurEndRate <= 0.0f )
		fCurEndRate = FLOAT1;
	
	pGrp->ClearAllLoopAni( FLOAT100, true );
	pGrp->ClearAllActionAni( FLOAT100, true );
	pGrp->SetActionAni( iCurSkillAniID, FLOAT100, FLOAT100, FLOAT1/fCurEndRate );

	m_dwMotionEndTime = dwCurTime + pGrp->GetAnimationFullTime( iCurSkillAniID ) * fCurEndRate;
	m_dwActionStartTime = dwCurTime + pChar->GetFirstFireTime( m_szEndAni ) * fCurEndRate;
}

void ioGardenerAttackSpecialState::ProcessEnd( ioBaseChar *pChar )
{
	if( !pChar ) return;
	if( m_State != GAS_END ) return;

	DWORD dwCurTime = FRAMEGETTIME();
	if( m_dwActionStartTime != 0 && m_dwActionStartTime < dwCurTime && !m_vWeaponInfoList.empty() )
	{
		pChar->NormalAttackFireExplicit( m_vWeaponInfoList.front() );
		m_dwActionStartTime = 0;
	}
	else if( m_dwMotionEndTime < dwCurTime )
		pChar->SetState( CS_DELAY );
}

void ioGardenerAttackSpecialState::ApplyStateSync( SP2Packet &rkPacket, ioBaseChar* pOwner )
{
	ioSpecialStateBase::ApplyStateSync( rkPacket, pOwner );

	D3DXQUATERNION qtRot;
	rkPacket >> qtRot;
	pOwner->SetTargetRotToRotate( qtRot, true );

	D3DXVECTOR3 vPos;
	rkPacket >> vPos;
	pOwner->SetWorldPosition( vPos );

	int eState;
	rkPacket >> eState;

	switch( eState )
	{
	case GAS_DELAY:
	case GAS_MOVE:
		{
			m_State = (GardenerAttackState)eState;
			rkPacket >> m_vCirclePos;
			rkPacket >> m_vCircleMoveDir;
		}
		break;
	}
}