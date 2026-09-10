#include "stdafx.h"

#include "ioBaseChar.h"
#include "ioPlayStage.h"

#include "ioAnubisItem.h"

#include "ioGrowthUpInfo.h"

#include "WeaponDefine.h"

ioAnubisItem::ioAnubisItem()
{
	m_dwTimeGap = 0;
	m_dwChargeStartTime = 0;
	m_iCurCombo = 0;
	m_FireState = FS_NONE;

	m_iCurAirJump = 0;
	m_fAirJumpPowerRate = 0.0f;
	m_vAirJumpTimeList.clear();
}

ioAnubisItem::ioAnubisItem( const ioAnubisItem &rhs )
	: ioWeaponItem( rhs ),
	m_PreGatherAnimation( rhs.m_PreGatherAnimation ),
	m_GatherAnimation( rhs.m_GatherAnimation ),
	m_GatherEffect( rhs.m_GatherEffect ),
	m_EndGatherAnimation( rhs.m_EndGatherAnimation ),
	m_fMoveSpeed( rhs.m_fMoveSpeed ),
	m_fRange( rhs.m_fRange ),
	m_fHeightGap( rhs.m_fHeightGap ),
	m_fGaugePerAllDefense( rhs.m_fGaugePerAllDefense ),
	m_dwDefenseTicTime( rhs.m_dwDefenseTicTime ),
	m_ReadyCircle( rhs.m_ReadyCircle ),
	m_EnableCircle( rhs.m_EnableCircle ),
	m_fMagicCircleRadius( rhs.m_fMagicCircleRadius ),
	m_fSkillGaugeRate( rhs.m_fSkillGaugeRate ),
	m_dwMiddleChargeTime( rhs.m_dwMiddleChargeTime ),
	m_AreaWeaponName( rhs.m_AreaWeaponName ),
	m_CreateAreaWeaponAnimation( rhs.m_CreateAreaWeaponAnimation ),
	m_fCreateAreaWeaponRate( rhs.m_fCreateAreaWeaponRate ),
	m_ExtendAttack( rhs.m_ExtendAttack ),
	m_stCheckBuff( rhs.m_stCheckBuff ),
	m_nExtendAttackCnt( rhs.m_nExtendAttackCnt )
{
	m_dwTimeGap = 0;
	m_dwChargeStartTime = 0;
	m_iCurCombo = 0;
	m_FireState = FS_NONE;

	m_iCurAirJump = 0;
	m_fAirJumpPowerRate = 0.0f;
	m_vAirJumpTimeList.clear();

	m_dwCreatedAreaWeaponIndex = 0;
	m_dwMotionEndTime = 0;
}

ioAnubisItem::~ioAnubisItem()
{
	ioPlayStage *pStage = g_ItemMaker.GetPlayStage();
	if( pStage )
	{
		ioHashString stOwnerName = GetOwnerName();
		ioBaseChar *pOwner = pStage->GetBaseChar( stOwnerName );
		if( pOwner )
			DestroyAreaWeapon( pOwner );
	}

	m_vAirJumpTimeList.clear();
}

void ioAnubisItem::LoadProperty( ioINILoader &rkLoader )
{
	ioWeaponItem::LoadProperty( rkLoader );

	char szBuf[MAX_PATH];

	rkLoader.LoadString_e( "pre_gather_animation", "", szBuf, MAX_PATH );
	m_PreGatherAnimation = szBuf;
	rkLoader.LoadString_e( "gather_animation", "", szBuf, MAX_PATH );
	m_GatherAnimation = szBuf;
	rkLoader.LoadString_e( "gather_effect", "", szBuf, MAX_PATH );
	m_GatherEffect = szBuf;
	rkLoader.LoadString_e( "end_gather_animation", "", szBuf, MAX_PATH );
	m_EndGatherAnimation = szBuf;

	rkLoader.LoadString_e( "ready_circle", "", szBuf, MAX_PATH );
	m_ReadyCircle = szBuf;
	rkLoader.LoadString_e( "enable_circle", "", szBuf, MAX_PATH );
	m_EnableCircle = szBuf;

	m_fMagicCircleRadius = rkLoader.LoadFloat_e( "magic_circle_radius", FLOAT100 );
	m_fRange = rkLoader.LoadFloat_e( "magic_circle_range", FLOAT1000 );
	m_fMoveSpeed = rkLoader.LoadFloat_e( "magic_circle_move_speed", 350.0f );
	m_fHeightGap = rkLoader.LoadFloat_e( "magic_circle_height_gap", 20.0f );

	m_fGaugePerAllDefense = rkLoader.LoadFloat_e( "gauge_per_defense", 5.0f );
	m_dwDefenseTicTime = rkLoader.LoadInt_e( "defense_tic_time", 500 );
	m_fSkillGaugeRate = rkLoader.LoadFloat_e( "skill_gauge_rate", FLOAT1 );

	//////////////////////////////////////////////////////////////////////////

	m_dwMiddleChargeTime = (DWORD)rkLoader.LoadInt_e( "middle_charge_time", 0.f );

	rkLoader.LoadString_e( "area_weapon_name", "", szBuf, MAX_PATH );
	m_AreaWeaponName = szBuf;

	rkLoader.LoadString_e( "check_buff", "", szBuf, MAX_PATH );
	m_stCheckBuff = szBuf;

	rkLoader.LoadString_e( "area_create_animation", "", szBuf, MAX_PATH );
	m_CreateAreaWeaponAnimation = szBuf;

	m_fCreateAreaWeaponRate = rkLoader.LoadFloat_e( "area_create_animation_rate", FLOAT1 );

	m_nExtendAttackCnt = rkLoader.LoadInt_e( "extend_attack_max_user_cnt", 1 );

	wsprintf_e( szBuf, "extend_attack" );
	LoadAttackAttribute( szBuf, m_ExtendAttack, rkLoader );
}

ioItem* ioAnubisItem::Clone()
{
	return new ioAnubisItem( *this );
}

void ioAnubisItem::SetNormalAttack( ioBaseChar *pOwner, int iCurCombo )
{
	if( !pOwner ) return;

	ClearCancelInfo();

	int iMaxCombo = GetMaxCombo();
	if( !COMPARE( iCurCombo, 0, iMaxCombo ) )
	{
		ioAttackableItem::SetNormalAttack( pOwner, iCurCombo );
		m_FireState = FS_NONE;
	}
	else
	{
		m_FireState = FS_CHARGING;
		m_dwChargeStartTime = FRAMEGETTIME();
		m_iCurCombo = iCurCombo;

		pOwner->SetState( CS_ATTACK );
		pOwner->SetChargingState( true );

		ioEntityGroup *pGrp = pOwner->GetGroup();
		pGrp->ClearAllActionAni( FLOAT100, true );
		pGrp->ClearAllLoopAni( FLOAT100, true );

		int iAniID = pGrp->GetAnimationIdx( m_PreGatherAnimation );
		float fTimeRate = (float)m_dwChargeTime / pGrp->GetAnimationFullTime( iAniID );
		pGrp->SetActionAni( iAniID, FLOAT100, FLOAT100, FLOAT1, FLOAT1/fTimeRate, 0.0f, 0.0f, true );
	}
}

void ioAnubisItem::ChangeToGathering( ioBaseChar *pOwner )
{
	pOwner->StopAutoTargetTracking();

	ioEntityGroup *pGrp = pOwner->GetGroup();
	pGrp->ClearAllActionAni( FLOAT100, true );
	pGrp->ClearAllLoopAni( FLOAT100, true );
	pGrp->SetLoopAni( m_GatherAnimation, FLOAT100 );
	pOwner->AttachEffect( m_GatherEffect );

	ioHashString szDummyCircle;
	pOwner->CreateMagicCircle( m_ReadyCircle, m_EnableCircle, szDummyCircle, m_fMagicCircleOffSet );

	m_FireState = FS_GATHERING;

	if( pOwner->IsNeedSendNetwork() )
	{
		SP2Packet kPacket( CUPK_ITEM_GAUGE_CANCEL );
		FillItemGaugePacketHeader( pOwner, kPacket );
		kPacket << (int)m_FireState;
		kPacket << m_iCurCombo;
		P2PNetwork::SendToAllPlayingUser( kPacket );
	}
}

void ioAnubisItem::CheckNormalAttack( ioBaseChar *pOwner )
{
	DWORD dwCurTime = FRAMEGETTIME();

	switch( m_FireState )
	{
	case FS_CHARGING:
		OnCharging( pOwner );
		break;
	case FS_NORMAL_ATTACK:
		{
			if( pOwner->IsNeedProcess() )
			{
				CheckCancelReserve( pOwner );

				if( ProcessCancel(pOwner) )
					return;
			}

			ioAttackableItem::CheckNormalAttack( pOwner );
		}
		break;
	case FS_GATHERING:
		OnGathering( pOwner );
		break;
	case FS_AREA_CREATING:
		if( pOwner->IsNeedProcess() && m_dwFireTime < dwCurTime )
		{
			CreateAreaWeapon( pOwner, m_vAreaPos );
			return;	
		}
		break;
	case FS_AREA_CREATED:
		if( m_dwMotionEndTime < dwCurTime )
		{
			m_FireState = FS_NONE;
			pOwner->SetState( CS_DELAY );
			return;
		}
		break;
	case FS_EXTEND_ATTACKING:
		if( pOwner->IsNeedProcess() && m_dwFireTime < dwCurTime )
		{
			CreateExtendAttack(pOwner);
			DestroyAreaWeapon(pOwner);
			return;
		}
		break;
	case FS_EXTEND_ATTACKED:
		if( m_dwMotionEndTime < dwCurTime )
		{
			m_FireState = FS_NONE;
			pOwner->SetState( CS_DELAY );
			return;
		}
		break;
	}
}

void ioAnubisItem::OnCharging( ioBaseChar *pOwner )
{
	if( !pOwner->IsNeedProcess() )
		return;

	if( pOwner->IsAttackKeyDown() )
	{
		if( m_dwChargeStartTime + m_dwChargeTime <= FRAMEGETTIME() )
		{
			pOwner->EndEffect( m_GatherEffect );
			ChangeToGathering( pOwner );
		}
	}
	else
	{
		pOwner->SetChargingState( false );

		if( m_dwChargeStartTime + m_dwMiddleChargeTime <= FRAMEGETTIME() )
		{
			ioAreaWeapon *pAreaWeapon = FindAreaWeapon(pOwner);
			if( pAreaWeapon )
				ChangeToExtendAttack( pOwner );
			else
				ChangeToNormalAttack( pOwner );
		}
		else
			ChangeToNormalAttack( pOwner );
	}
}

void ioAnubisItem::OnGathering( ioBaseChar *pOwner )
{
	if( !pOwner || !pOwner->IsNeedProcess() )
		return;

	if( !pOwner->IsAttackKeyDown() )	// ¹öÆ° ¶À
	{
		D3DXVECTOR3 vCirclePso = ioMath::VEC3_ZERO;
		if( pOwner->CheckMagicCircle() )
		{
			vCirclePso = pOwner->GetMagicCirclePos();
		}
		else
		{
			m_FireState = FS_NONE;
			return;
		}

		pOwner->DestroyMagicCircle();

		ChangeToCreateArea(pOwner, vCirclePso);
		return;
	}

	float fCurRange = m_fRange;
	float fValue = 0.0f;
	ioGrowthNormalUpInfo *pUpInfo = ToNormalUpInfo( GetGrowthUpInfoByType(GT_MAGIC_CIRCLE_RANGE, pOwner->GetCharName()) );
	if( pUpInfo )
	{
		ioPlayStage *pStage = m_pCreator->GetPlayStage();
		fValue = pUpInfo->GetValue(pStage);
	}

	fCurRange += fValue;

	//Gather Gauge
	if( pOwner->CheckMagicCircle() )
		pOwner->UpdateMagicCirclePos( m_fMoveSpeed, fCurRange, m_fHeightGap, false );
}


void ioAnubisItem::ChangeToNormalAttack( ioBaseChar *pOwner )
{
	DWORD dwCurTime = FRAMEGETTIME();
	ClearCancelInfo();

	const AttackAttribute *pAttribute = m_FormalAttack.GetAttribute( AT_NORMAL, m_iCurCombo );
	if( pAttribute )
	{
		pOwner->SetNormalAttackByAttribute( *pAttribute );

		m_dwCancelCheckTime = dwCurTime;
		m_dwInputCancelCheckTime = dwCurTime;

		m_szCurAni = pAttribute->m_AttackAnimation;
		m_fCurAniRate = pAttribute->m_fAttackAniRate;
	}

	pOwner->ApplyNormalAttackResult( m_iCurCombo );
	m_FireState = FS_NORMAL_ATTACK;

	if( pOwner->IsNeedSendNetwork() )
	{
		SP2Packet kPacket( CUPK_ITEM_GAUGE_CANCEL );
		FillItemGaugePacketHeader( pOwner, kPacket );
		kPacket << (int)m_FireState;
		kPacket << m_iCurCombo;
		P2PNetwork::SendToAllPlayingUser( kPacket );
	}
}


void ioAnubisItem::ChangeToCreateArea( ioBaseChar *pOwner, const D3DXVECTOR3& vPos )
{
	ioEntityGroup *pGrp = pOwner->GetGroup();
	if( !pGrp )
		return;

	m_FireState = FS_AREA_CREATING;

	pGrp->ClearAllLoopAni( FLOAT100, true );
	pGrp->ClearAllActionAni( FLOAT100, true );

	int iAniID	     = pGrp->GetAnimationIdx( m_CreateAreaWeaponAnimation );
	float fTimeRate  = m_fCreateAreaWeaponRate;

	pGrp->SetActionAni( iAniID, FLOAT100, FLOAT100, FLOAT1, FLOAT1/fTimeRate, 0.0f, 0.0f, true );

	DWORD dwCurTime = FRAMEGETTIME();
	m_dwMotionEndTime = dwCurTime + pGrp->GetAnimationFullTime( iAniID ) * fTimeRate;
	m_dwFireTime = dwCurTime + pGrp->GetAnimationEventTime_e( iAniID, "fire_s" ) * fTimeRate;

	DestroyAreaWeapon( pOwner );

	m_vAreaPos = vPos;

	//SendPacke
	if( pOwner->IsNeedSendNetwork() )
	{
		SP2Packet kPacket( CUPK_ITEM_GAUGE_CANCEL );
		FillItemGaugePacketHeader( pOwner, kPacket );
		kPacket << (int)m_FireState;
		kPacket << m_iCurCombo;
		kPacket << vPos;
		P2PNetwork::SendToAllPlayingUser( kPacket );
	}
}


void ioAnubisItem::ChangeToExtendAttack( ioBaseChar *pOwner )
{
	if( pOwner->IsNeedProcess() && m_dwCreatedAreaWeaponIndex == 0 )
		return;

	ioEntityGroup *pGrp = pOwner->GetGroup();
	if( !pGrp )
		return;

	m_FireState = FS_EXTEND_ATTACKING;

	pGrp->ClearAllLoopAni( FLOAT100, true );
	pGrp->ClearAllActionAni( FLOAT100, true );

	int iAniID	     = pGrp->GetAnimationIdx( m_ExtendAttack.m_AttackAnimation );
	float fTimeRate  = m_ExtendAttack.m_fAttackAniRate;

	pGrp->SetActionAni( iAniID, FLOAT100, FLOAT100, FLOAT1, FLOAT1/fTimeRate, 0.0f, 0.0f, true );

	DWORD dwCurTime = FRAMEGETTIME();
	m_dwMotionEndTime = dwCurTime + pGrp->GetAnimationFullTime( iAniID ) * fTimeRate;
	m_dwFireTime = dwCurTime + pGrp->GetAnimationEventTime_e( iAniID, "fire_s" ) * fTimeRate;

	//Send Packet
	if( pOwner->IsNeedSendNetwork() )
	{
		SP2Packet kPacket( CUPK_ITEM_GAUGE_CANCEL );
		FillItemGaugePacketHeader( pOwner, kPacket );
		kPacket << (int)m_FireState;
		kPacket << m_iCurCombo;
		P2PNetwork::SendToAllPlayingUser( kPacket );
	}
}

void ioAnubisItem::OnItemGaugeCancel( ioBaseChar *pOwner, SP2Packet &rkPacket )
{
	int iFireState;
	rkPacket >> iFireState;
	rkPacket >> m_iCurCombo;

	switch( iFireState )
	{
	case FS_NORMAL_ATTACK:
		ioAttackableItem::SetNormalAttack( pOwner, m_iCurCombo );
		m_FireState = FS_NORMAL_ATTACK;
		break;
	case FS_GATHERING:
		pOwner->EndEffect( m_GatherEffect );
		ChangeToGathering( pOwner );
		break;
	case FS_AREA_CREATING:
		{
			D3DXVECTOR3 vPos;
			rkPacket >> vPos;
			ChangeToCreateArea( pOwner, vPos );
		}
		break;
	case FS_AREA_CREATED:
		{
			D3DXVECTOR3 vPos;
			rkPacket >> vPos;
			CreateAreaWeapon( pOwner, vPos );
		}
		break;
	case FS_EXTEND_ATTACKING:
		ChangeToExtendAttack(pOwner);
		break;
	case FS_EXTEND_ATTACKED:
		CreateExtendAttack(pOwner);
		DestroyAreaWeapon(pOwner);
		break;
	case FS_NONE:
		m_FireState = FS_NONE;

		pOwner->EndEffect( m_GatherEffect );
		pOwner->SetState( CS_DELAY );
		break;
	}
}

ioWeaponItem::WeaponSubType ioAnubisItem::GetSubType() const
{
	return WST_MAGIC_STAFF;
}

void ioAnubisItem::OnEndExtendAttack( ioBaseChar *pOwner, bool bReleaseItem )
{
	pOwner->EndEffect( m_GatherEffect );
}

float ioAnubisItem::GetCircleRadius()
{
	return m_fMagicCircleRadius;
}

bool ioAnubisItem::IsChargeAttackState( ioBaseChar *pOwner )
{
	if( pOwner->GetState() == CS_ATTACK )
	{
		if( m_FireState == FS_GATHERING || m_FireState == FS_CHARGING )
			return true;
	}

	return false;
}


bool ioAnubisItem::IsEnableExtraGauge( ioBaseChar *pChar )
{
	if( m_bGaugeUpdateChargeAttack && IsChargeAttackState( pChar ) )
		return false;

	return true;
}


bool ioAnubisItem::SetJumpAttack( ioBaseChar *pOwner, int iAniID, float fTimeRate, AttackType eType )
{
	if( !pOwner ) return false;

	DWORD dwCurTime = FRAMEGETTIME();
	ClearAirJump();
	AttackType eExtendType = AT_JUMP_ATTACK;

	const AttackAttribute *pAttr = GetAttackAttribute( eExtendType );
	if( !pAttr )
	{
		LOG.PrintTimeAndLog( 0, "ioAnubisItem::SetJumpAttack - AttackAttribute Not Exist" );
		return false;
	}

	ioEntityGroup *pGrp = pOwner->GetGroup();
	iAniID = pGrp->GetAnimationIdx( pAttr->m_AttackAnimation );
	if( iAniID == -1 )
	{
		LOG.PrintTimeAndLog( 0, "ioAnubisItem::SetJumpAttack - JumpAttack Animation Not Exist" );
		return false;
	}

	fTimeRate   = pAttr->m_fAttackAniRate;
	DWORD dwPreDelay  = pAttr->m_dwPreDelay;

	// For AirJumpPower
	SetAirJump( pOwner, iAniID, fTimeRate );

	return false;
}

void ioAnubisItem::ProcessJumpAttack( ioBaseChar *pOwner, float fHeightGap )
{
	if( m_pExtendJump )
	{
		m_pExtendJump->ProcessJumpAttack( pOwner, fHeightGap );
	}

	CheckAirJump( pOwner );
}

void ioAnubisItem::SetExtraInfoToWeapon( ioWeapon *pWeapon, bool bJumpAttack )
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

	ioFlameWeapon *pFlame = ToFlameWeapon( pWeapon );
	if( pFlame )
	{
		pFlame->SetStartPosition( m_ExtendAttackPos, NULL );
	}
}


void ioAnubisItem::CreateAreaWeapon( ioBaseChar *pOwner, const D3DXVECTOR3& vPos )
{
	if( m_dwCreatedAreaWeaponIndex )
		DestroyAreaWeapon(pOwner);

	ioAreaWeapon *pArea = g_AreaWeaponMgr.CreateAreaWeapon( pOwner->GetCharName(),
															"",
															GetName(),
															m_AreaWeaponName,
															vPos,
															pOwner->GetTargetRot(),
															ioAreaWeapon::CT_NORMAL );

	m_dwCreatedAreaWeaponIndex = pArea->GetAreaWeaponIndex();

	m_FireState = FS_AREA_CREATED;

	//Send Packet
	if( pOwner->IsNeedSendNetwork() )
	{
		SP2Packet kPacket( CUPK_ITEM_GAUGE_CANCEL );
		FillItemGaugePacketHeader( pOwner, kPacket );
		kPacket << (int)m_FireState;
		kPacket << m_iCurCombo;
		kPacket << vPos;
		P2PNetwork::SendToAllPlayingUser( kPacket );
	}
}


void ioAnubisItem::CreateExtendAttack( ioBaseChar *pOwner)
{
	if( !pOwner )	return;

	ioPlayStage *pStage = pOwner->GetCreator();

	if( !pStage )
		return;

	int nCnt = 0;

	const BaseCharList &rkCharList = pStage->GetBaseCharList();
	BaseCharList::const_iterator iter;

	for( iter=rkCharList.begin() ; iter!=rkCharList.end() ; ++iter )
	{
		ioBaseChar *pChar = (*iter);
		if( !pChar ) continue;

		if( pChar->GetTeam() == pOwner->GetTeam() ) continue;
		if( pChar->GetCharName() == GetOwnerName() ) continue;
		if( pChar->GetExperienceMode() != EMS_NONE ) continue;

		if( pChar->HasBuff( pOwner->GetCharName(), m_stCheckBuff) )
		{
			nCnt++;
			m_ExtendAttackPos = pChar->GetWorldPosition();
			pOwner->NormalAttackFireExplicit( m_ExtendAttack.m_vWeaponInfoList.front() );
		}

		if( nCnt >= m_nExtendAttackCnt )
			break;
	}

	m_FireState = FS_EXTEND_ATTACKED;

	//Send Packet
	if( pOwner->IsNeedSendNetwork() )
	{
		SP2Packet kPacket( CUPK_ITEM_GAUGE_CANCEL );
		FillItemGaugePacketHeader( pOwner, kPacket );
		kPacket << (int)m_FireState;
		kPacket << m_iCurCombo;
		P2PNetwork::SendToAllPlayingUser( kPacket );
	}
}


void ioAnubisItem::DestroyAreaWeapon( ioBaseChar *pOwner )
{
	if( m_dwCreatedAreaWeaponIndex )
		g_AreaWeaponMgr.DestroyAreaWeapon( m_dwCreatedAreaWeaponIndex );

	m_dwCreatedAreaWeaponIndex = 0;
}


void ioAnubisItem::OnReleased( ioBaseChar *pOwner )
{
	DestroyAreaWeapon(pOwner);

	ioAttackableItem::OnReleased( pOwner );
}


ioAreaWeapon* ioAnubisItem::FindAreaWeapon( ioBaseChar *pOwner )
{
	if( m_dwCreatedAreaWeaponIndex == 0 )
		return NULL;

	ioAreaWeapon *pArea = g_AreaWeaponMgr.FindAreaWeaponByIndex( m_dwCreatedAreaWeaponIndex );

	return pArea ;
}
