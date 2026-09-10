

#include "stdafx.h"

#include "ioJumperItem.h"

#include "ioPlayStage.h"
#include "ioBaseChar.h"

#include "ioChargeComboDash.h"
#include "ioChargeComboJump3.h"

#include "WeaponDefine.h"

ioJumperItem::ioJumperItem()
{
	Init();	
}

ioJumperItem::ioJumperItem( const ioJumperItem &rhs )
	: ioWeaponItem( rhs ),
	m_fNeedGauge( rhs.m_fNeedGauge ),
	m_fNeedCancelGauge( rhs.m_fNeedCancelGauge ),
	m_PreAnimationList( rhs.m_PreAnimationList ),
	m_ExtendAttackAttribute( rhs.m_ExtendAttackAttribute ),
	m_ExtendJumpAttackAttribute( rhs.m_ExtendJumpAttackAttribute ),
	m_fExtendAttackNextJumpAmt( rhs.m_fExtendAttackNextJumpAmt ),
	m_GrapperPreAnimation( rhs.m_GrapperPreAnimation ),
	m_GrapperPreAnimationRate( rhs.m_GrapperPreAnimationRate ),
	m_GrapplingWounded( rhs.m_GrapplingWounded ),
	m_fGrapplingOffsetGapRange( rhs.m_fGrapplingOffsetGapRange ),
	m_fGrapplingOffsetHeightRate( rhs. m_fGrapplingOffsetHeightRate ),
	m_GrapperMoveAnimation( rhs.m_GrapperMoveAnimation ),
	m_GrapperMoveAnimationRate( rhs.m_GrapperMoveAnimationRate ),
	m_fGrapAfterMoveSpeed( rhs.m_fGrapAfterMoveSpeed ),
	m_fCancelDashTeleport( rhs.m_fCancelDashTeleport ),
	m_fInPortalOffset( rhs.m_fInPortalOffset ),
	m_fOutPortalOffset( rhs.m_fOutPortalOffset ),
	m_fInPortalHeightOffset( rhs.m_fInPortalHeightOffset ),
	m_fOutPortalHeightOffset( rhs.m_fOutPortalHeightOffset ),
	m_szInPortalMapEffect( rhs. m_szInPortalMapEffect ),
	m_szOutPortalMapEffect( rhs.m_szOutPortalMapEffect ),
	m_CancelDashTeleportStartAnimation( rhs.m_CancelDashTeleportStartAnimation ),
	m_CancelDashTeleportStartAnimationRate( rhs.m_CancelDashTeleportStartAnimationRate ),
	m_CancelDashTeleportEndAnimation( rhs.m_CancelDashTeleportEndAnimation ),
	m_CancelDashTeleportEndAnimationRate( rhs.m_CancelDashTeleportEndAnimationRate ),
	m_TeleportEndAttribute( rhs.m_TeleportEndAttribute ),
	m_fCancelDashEndJumpJudgment( rhs.m_fCancelDashEndJumpJudgment ),
	m_fCancelDashEndNextJumpAmt( rhs.m_fCancelDashEndNextJumpAmt ),
	m_fTeleportAttackNextJumpAmt( rhs.m_fTeleportAttackNextJumpAmt )
{
	Init();
}

ioJumperItem::~ioJumperItem()
{
}

void ioJumperItem::Init()
{
	m_AttackState						= AS_NONE;
	m_eCurrCallerType					= CCSD_NONE;

	m_dwAttackStartTime					= 0;
	m_iCurCombo							= 0;	
	m_iCurAirJump						= 0;

	m_fCurrGrapAfterMove				= 0.0f;
	m_fAirJumpPowerRate					= 0.0f;	
	
	m_InPortalID						= -1;
	m_OutPortalID						= -1;
	m_vTeleportPos						= ioMath::VEC3_ZERO;

	m_dwKeyReserveTime					= 0;

	m_dwExtendAttackEndTime				= 0;
	m_dwGrapperPreEndTime				= 0;
	m_dwCancelDashTeleportStartEndTime	= 0;
	m_dwTeleportEndTime					= 0;
}

void ioJumperItem::LoadProperty( ioINILoader &rkLoader )
{
	ioWeaponItem::LoadProperty( rkLoader );

	char szBuf[MAX_PATH];	
	m_fNeedGauge		= rkLoader.LoadFloat_e( "need_extra_gauge", 0.0f );	
	m_fNeedCancelGauge	= rkLoader.LoadFloat_e( "need_cancel_gauge", 0.0f );	

	int iPreCount = rkLoader.LoadInt_e( "pre_animation_cnt", 0 );
	for( int i = 0; i < iPreCount; ++i )
	{
		wsprintf_e( szBuf, "pre_animation%d", i+1 );
		rkLoader.LoadString( szBuf, "", szBuf, MAX_PATH );
		m_PreAnimationList.push_back( ioHashString( szBuf ) );
	}

	//Grap Attack
	LoadAttackAttribute( "extend_attack", m_ExtendAttackAttribute, rkLoader );
	LoadAttackAttribute( "extend_jump_attack", m_ExtendJumpAttackAttribute, rkLoader );
	m_fExtendAttackNextJumpAmt = rkLoader.LoadFloat_e( "extend_attack_end_next_jump_amt", FLOAT1000 );


	//Grapper
	rkLoader.LoadString( "grappling_pre_animation", "", szBuf, MAX_PATH );
	m_GrapperPreAnimation = szBuf;
	m_GrapperPreAnimationRate = rkLoader.LoadFloat_e( "grappling_pre_animation_rate", FLOAT1 );

	//Grapping Target
	LoadGrapplingTargetProperty( rkLoader, m_GrapplingWounded );

	//Move
	rkLoader.LoadString( "grappling_move_animation", "", szBuf, MAX_PATH );
	m_GrapperMoveAnimation = szBuf;

	m_GrapperMoveAnimationRate	= rkLoader.LoadFloat_e( "grappling_move_animation_rate", FLOAT1 );
	m_fGrapAfterMoveSpeed		= rkLoader.LoadFloat_e( "grappling_move_speed", FLOAT500 );

	//portal
	m_fInPortalOffset			= rkLoader.LoadFloat_e( "portal_in_offset", FLOAT100 );
	m_fOutPortalOffset			= rkLoader.LoadFloat_e( "portal_out_offset", FLOAT500 );
	m_fInPortalHeightOffset		= rkLoader.LoadFloat_e( "portal_in_height_offset", 20.0f );
	m_fOutPortalHeightOffset	= rkLoader.LoadFloat_e( "portal_out_height_offset", 20.0f );

	rkLoader.LoadString( "portal_in_effect", "", szBuf, MAX_PATH );
	m_szInPortalMapEffect = szBuf;
	rkLoader.LoadString( "portal_out_effect", "", szBuf, MAX_PATH );
	m_szOutPortalMapEffect = szBuf;

	//cancel dash teleport
	rkLoader.LoadString( "cancel_dash_teleport_start_animation", "", szBuf, MAX_PATH );
	m_CancelDashTeleportStartAnimation = szBuf;
	m_CancelDashTeleportStartAnimationRate = rkLoader.LoadFloat_e( "cancel_dash_teleport_start_animation_rate", FLOAT1 );	

	rkLoader.LoadString( "cancel_dash_teleport_end_animation", "", szBuf, MAX_PATH );
	m_CancelDashTeleportEndAnimation = szBuf;
	m_CancelDashTeleportEndAnimationRate = rkLoader.LoadFloat_e( "cancel_dash_teleport_end_animation_rate", FLOAT1 );	

	//cancel dash
	m_fCancelDashTeleport			= rkLoader.LoadFloat_e( "cancel_dash_teleport_range", 200.0f );
	m_fCancelDashEndJumpJudgment	= rkLoader.LoadFloat_e( "cancel_dash_end_jump_judgment", 5.0f );
	m_fCancelDashEndNextJumpAmt		= rkLoader.LoadFloat_e( "cancel_dash_end_next_jump_amt", FLOAT1000 );

	//teleport end
	LoadAttackAttribute( "teleport_end", m_TeleportEndAttribute, rkLoader );
	m_fTeleportAttackNextJumpAmt	= rkLoader.LoadFloat_e( "teleport_end_next_jump_amt", FLOAT1000 );
}

void ioJumperItem::LoadGrapplingTargetProperty( IN ioINILoader& rkLoader, OUT GrapplingSkillWoundedInfo& rkInfo )
{
	char szBuf[MAX_PATH];

	rkInfo.Init();
	rkInfo.m_bReverseTargetRot	= rkLoader.LoadBool_e( "grappling_target_reverse_rotate", false );
	rkInfo.m_GrapplingColType	= (GrapplingColType)rkLoader.LoadInt_e( "grappling_target_enable_col", GCT_ALL );

	rkLoader.LoadString_e( "grappling_target_loop_motion", "", szBuf, MAX_PATH );
	rkInfo.m_GrapplingLoopMotion		= szBuf;
	rkInfo.m_GrapplingLoopMotion2		= szBuf;
	rkInfo.m_fGrapplingLoopMotionRate	= rkLoader.LoadFloat_e( "grappling_target_loop_motion_rate", FLOAT1 );

	m_fGrapplingOffsetGapRange			= rkLoader.LoadFloat_e( "grappling_target_gap_range", 0.0f );
	m_fGrapplingOffsetHeightRate		= rkLoader.LoadFloat_e( "grappling_target_height_rate", FLOAT05 );
}

ioItem* ioJumperItem::Clone()
{
	return new ioJumperItem( *this );
}

ioWeaponItem::WeaponSubType ioJumperItem::GetSubType() const
{
	return WST_JUMPER_ITEM;
}

void ioJumperItem::ClearSpecial( ioBaseChar* pOwner, CharState eNewState )
{
	DestroyMapEffect( pOwner->GetCreator(), pOwner );

	Init();

	ClearKeyReserve();
	ClearCancelInfo();

	pOwner->ClearSkillReserveKey();
	pOwner->ClearReserveKeyInput();
}

void ioJumperItem::DestroyMapEffect( ioPlayStage* pStage, ioBaseChar* pOwner )
{
	if( !pOwner )
		return;

	if( !pStage )
		return;

	if( m_InPortalID != -1 )
	{
		ioMapEffect* pMapEffect = pStage->FindMapEffect( m_InPortalID );
		if( pMapEffect )
			pMapEffect->EndEffectGrace();

		m_InPortalID = -1;
	}

	if( m_OutPortalID != -1 )
	{
		ioMapEffect* pMapEffect = pStage->FindMapEffect( m_OutPortalID );
		if( pMapEffect )
			pMapEffect->EndEffectGrace();

		m_OutPortalID = - 1;
	}

}

// 노말 공격 ----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

void ioJumperItem::SetNormalAttack( ioBaseChar *pOwner, int iCurCombo )
{
	if( !pOwner )
		return;

	ClearCancelInfo();

	int iPreGatherCnt = m_PreAnimationList.size();

	m_AttackState		= AS_CHARGING;
	m_dwAttackStartTime	= FRAMEGETTIME();
	m_iCurCombo			= iCurCombo;

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

void ioJumperItem::CheckNormalAttack( ioBaseChar *pOwner )
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
	}
}

void ioJumperItem::OnNormalAttack( ioBaseChar *pOwner )
{
	if( pOwner->IsNeedProcess() )
	{
		CheckCancelReserveByConditionalSpecialDash( pOwner );
		if( ProcessCancelBySpeicalDash( pOwner ) )
			return;

		CheckCancelReserve( pOwner );
		if( ProcessCancel(pOwner) )
			return;
	}

	ioWeaponItem::CheckNormalAttack( pOwner );
}

// 공용 함수 ----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

int ioJumperItem::SetActionAni( IN ioBaseChar* pOwner, IN const ioHashString& szAnimation, IN float fTimeRate, IN DWORD dwPreDelay, OUT DWORD& dwEndTime, OUT DWORD& dwKeyReserve )
{
	ioEntityGroup *pGrp = pOwner->GetGroup();
	if( pGrp )
	{
		pGrp->ClearAllActionAni( FLOAT100 );
		pGrp->ClearAllLoopAni( FLOAT100 );

		int iAniID = pGrp->GetAnimationIdx( szAnimation );
		pGrp->SetActionAni( iAniID, FLOAT100, FLOAT100, FLOAT1, FLOAT1 / fTimeRate, 0.0f, dwPreDelay, true );
		dwEndTime = FRAMEGETTIME() + pGrp->GetAnimationFullTime(iAniID) * fTimeRate;
		DWORD dwReserve = pGrp->GetAnimationEventTime_e( iAniID, "key_reserve" ) * fTimeRate + dwPreDelay;
		if( 0 < dwReserve )
			dwKeyReserve = FRAMEGETTIME() + dwReserve;

		return iAniID;
	}

	return -1;
}

void ioJumperItem::SetAttack( IN ioBaseChar* pOwner, IN const AttackAttribute* pAttribute, OUT DWORD& dwAttackEnd, OUT DWORD& dwReserve )
{
	if( !pAttribute )
		return;

	ioEntityGroup *pGrp = pOwner->GetGroup();
	if( !pGrp )
		return;

	DWORD dwCurTime = FRAMEGETTIME();
	pGrp->ClearAllLoopAni( FLOAT10, true );
	pGrp->ClearAllActionAni( FLOAT10, true );

	int iAniID	     = pGrp->GetAnimationIdx( pAttribute->m_AttackAnimation );
	float fTimeRate  = pAttribute->m_fAttackAniRate;
	DWORD dwPreDelay = pAttribute->m_dwPreDelay;
	DWORD dwEndDelay = pAttribute->m_dwEndDelay;

	pGrp->SetActionAni( iAniID, FLOAT10, FLOAT10, FLOAT1, FLOAT1/fTimeRate, 0.0f, dwPreDelay, true );
	pOwner->RefreshFireTimeList( iAniID, pAttribute->m_vWeaponInfoList, FTT_NORMAL_ATTACK, fTimeRate, dwPreDelay );
	pOwner->IncreaseWeaponIndexBase();

	pOwner->CheckCharColSkipTime( iAniID, fTimeRate, dwPreDelay );
	pOwner->SetReservedSliding( pAttribute->m_vForceInfoList, iAniID, fTimeRate, dwPreDelay );

	dwAttackEnd = dwCurTime;
	dwAttackEnd += pGrp->GetAnimationFullTime( iAniID ) * fTimeRate + dwPreDelay;
	DWORD dwTrackingTime = FRAMEGETTIME() + pGrp->GetAnimationEventTime_e( iAniID, "tracking_e" ) * fTimeRate;			

	dwReserve = pGrp->GetAnimationEventTime_e( iAniID, "key_reserve" ) * fTimeRate + dwPreDelay;
	if( dwReserve > 0 )
		dwReserve += dwCurTime;
}

// 노말 공격 -----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

void ioJumperItem::ChangeToAttackFire( ioBaseChar *pOwner )
{
	if( !pOwner )
		return;

	DWORD dwCurTime = FRAMEGETTIME();
	pOwner->InitExtendAttackTagInfo();
	ClearCancelInfo();

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
		SP2Packet kPacket( CUPK_ITEM_GAUGE_OK );
		FillItemGaugePacketHeader( pOwner, kPacket );
		kPacket << static_cast<byte>( AS_ATTACK_FIRE );
		kPacket << pOwner->GetTargetRot();
		kPacket << m_iCurCombo;
		P2PNetwork::SendToAllPlayingUser( kPacket );
	}
}

// D꾹 ---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

void ioJumperItem::OnCharging( ioBaseChar *pOwner )
{
	if( !pOwner->IsNeedProcess() )
		return;

	if( pOwner->IsAttackKeyDown() && GetNeedBullet() <= GetCurBullet() )
	{
		if( m_dwAttackStartTime + m_dwChargeTime <= FRAMEGETTIME() )
		{	
			ChangeToExtendAttackFire( pOwner, FTT_NORMAL_ATTACK );			
		}		
	}
	else
	{
		ChangeToAttackFire( pOwner );
	}
}

void ioJumperItem::CheckSpecialState( ioBaseChar *pOwner )
{
	switch( m_AttackState )
	{
	case AS_EXTEND_ATTACK:
		OnExtendAttackFire( pOwner );
		break;
	case AS_GRAPPING_PRE:
		OnGrapplingPre( pOwner );
		break;
	case AS_GRAPPING_MOVE:
		OnGrapplingMove( pOwner );
		break;
	case AS_CANCEL_TELEPORT_PRE:
		OnCancelTeleportPre( pOwner );
		break;
	case AS_CANCEL_TELEPORT:
		OnCancelTeleport( pOwner );
		break;
	case AS_TELEPORT:
		OnTeleport( pOwner->GetCreator(), pOwner );
		break;
	}
}

// 특별 공격 --------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

void ioJumperItem::ChangeToExtendAttackFire( ioBaseChar *pOwner, FireTimeType eType )
{
	if( !pOwner )
		return;

	DWORD dwCurTime = FRAMEGETTIME();
	pOwner->InitExtendAttackTagInfo();	
	pOwner->ClearAttackFireTimeAndSkill();
	pOwner->SetSkillNextJumpAmt( 0.0f );
	ClearCancelInfo();


	FireTimeType eFireType = FTT_NORMAL_ATTACK;
	AttackAttribute* pAttackAttribute = NULL;
	switch( eType )
	{
	case FTT_DASH_ATTACK:
		{
			pOwner->SetAutoTarget( ATT_DASH );
			pAttackAttribute = &m_ExtendAttackAttribute;			
		}
		break;
	case FTT_JUMP_ATTACK:
		{
			pOwner->SetAutoTarget( ATT_JUMP );
			pAttackAttribute = &m_ExtendJumpAttackAttribute;
		}
		break;
	default:
		{
			pOwner->SetAutoTarget( ATT_NORMAL );
			pAttackAttribute = &m_ExtendAttackAttribute;
		}
		break;
	}

	pOwner->SetState( CS_JUMPER_SPECIAL );

	if( pOwner->IsNeedProcess() && pOwner->CheckCurDirKey() != ioUserKeyInput::DKI_NONE )
		pOwner->SetTargetRotToDirKey( pOwner->CheckCurDirKey() );

	m_AttackState			= AS_EXTEND_ATTACK;
	m_dwExtendAttackEndTime = FRAMEGETTIME();
	m_dwKeyReserveTime		= 0;

	SetUseExtraGauge( GetNeedBullet() );

	if( pAttackAttribute )
	{
		ioEntityGroup *pGrp = pOwner->GetGroup();
		if( pGrp )
		{
			pGrp->ClearAllLoopAni( FLOAT10, true );
			pGrp->ClearAllActionAni( FLOAT10, true );

			int iAniID	     = pGrp->GetAnimationIdx( pAttackAttribute->m_AttackAnimation );
			float fTimeRate  = pAttackAttribute->m_fAttackAniRate;
			DWORD dwPreDelay = pAttackAttribute->m_dwPreDelay;
			DWORD dwEndDelay = pAttackAttribute->m_dwEndDelay;

			pGrp->SetActionAni( iAniID, FLOAT10, FLOAT10, FLOAT1, FLOAT1/fTimeRate, 0.0f, dwPreDelay, true );
			pOwner->RefreshFireTimeList( iAniID, pAttackAttribute->m_vWeaponInfoList, eType, fTimeRate, dwPreDelay );
			pOwner->IncreaseWeaponIndexBase();

			pOwner->CheckCharColSkipTime( iAniID, fTimeRate, dwPreDelay );
			pOwner->SetReservedSliding( pAttackAttribute->m_vForceInfoList, iAniID, fTimeRate, dwPreDelay, FLOAT1, false );

			m_dwExtendAttackEndTime += pGrp->GetAnimationFullTime( iAniID ) * fTimeRate + dwPreDelay;
			DWORD dwTrackingTime = FRAMEGETTIME() + pGrp->GetAnimationEventTime_e( iAniID, "tracking_e" ) * fTimeRate;
			pOwner->SetAutoTargetTrackingEndTime( dwTrackingTime );
		}
	}

	if( pOwner->IsNeedSendNetwork() )
	{
		SP2Packet kPacket( CUPK_ITEM_GAUGE_OK );
		FillItemGaugePacketHeader( pOwner, kPacket );
		kPacket << static_cast<byte>( AS_EXTEND_ATTACK );
		kPacket << pOwner->GetTargetRot();
		kPacket << (byte)eType;
		P2PNetwork::SendToAllPlayingUser( kPacket );
	}
}

void ioJumperItem::OnExtendAttackFire( ioBaseChar *pOwner )
{
	DWORD dwCurTime = FRAMEGETTIME();
	if( m_dwExtendAttackEndTime < dwCurTime )
	{
		float fBottomHeight = pOwner->GetBottomHeight();
		if( pOwner->GetWorldPosition().y - fBottomHeight < m_fCancelDashEndJumpJudgment )
		{
			m_AttackState = AS_NONE;
			pOwner->SetState( CS_DELAY );
		}
		else
		{
			pOwner->SetSkillNextJumpAmt( m_fExtendAttackNextJumpAmt );
		}
	}
}

// 그랩 플링(그랩 공격 후에 ApplyWounded를 통해 들어옴)------------------------------------------------------------------------------------------------------------------------------------------------------

bool ioJumperItem::SetItemGrapplingState( ioBaseChar *pOwner, const ioHashString &szTargetName )
{
	if( !pOwner )
		return false;

	if( !pOwner->GetCreator() )
		return false;

	ioBaseChar *pTarget = pOwner->GetBaseChar( szTargetName );
	if( !pTarget )
		return false;

	if( pOwner->IsNeedProcess() && pOwner->CheckCurDirKey() != ioUserKeyInput::DKI_NONE )
		pOwner->SetTargetRotToDirKey( pOwner->CheckCurDirKey() );

	pOwner->SetState( CS_JUMPER_SPECIAL );

	m_AttackState			= AS_GRAPPING_PRE;

	m_GrapplingTargetName	= szTargetName;
	m_dwGrapperPreEndTime	= 0;
	m_dwKeyReserveTime		= 0;

	SetActionAni( pOwner, m_GrapperPreAnimation, m_GrapperPreAnimationRate, 0, m_dwGrapperPreEndTime, m_dwKeyReserveTime );

	if( pTarget->SetItemGrapplingState( pOwner->GetCharName(), m_GrapplingWounded ) )
	{
		pTarget->SetTargetRotToRotate( pOwner->GetTargetRot(), true, false );
	}

	if( pOwner->IsNeedSendNetwork() )
	{
		SP2Packet kPacket( CUPK_ITEM_GAUGE_OK );
		FillItemGaugePacketHeader( pOwner, kPacket );
		kPacket << static_cast<byte>( AS_GRAPPING_PRE );
		kPacket << pOwner->GetTargetRot();
		kPacket << pOwner->GetWorldPosition();
		kPacket << m_GrapplingTargetName;
		P2PNetwork::SendToAllPlayingUser( kPacket );
	}

	return true;
}

void ioJumperItem::MakePortal( ioPlayStage* pStage, ioBaseChar *pOwner )
{
	if( !pStage )
		return;
		
	if( !pOwner )
		return;

	float fHeight = 0.0f;
	if( !pOwner->IsCharDropZonePos() )
		 fHeight = pOwner->GetWorldPosition().y - pStage->GetMapHeightNoneEntity( pOwner->GetWorldPosition().x, pOwner->GetWorldPosition().z );

	D3DXVECTOR3 vInPortal = CalcPortalPos( pStage, pOwner, fHeight, m_fInPortalOffset, m_fInPortalHeightOffset );
	MakeMapEffect( pStage, pOwner, m_szInPortalMapEffect, vInPortal, m_InPortalID );
}

D3DXVECTOR3 ioJumperItem::CalcPortalPos( ioPlayStage* pStage, ioBaseChar *pOwner, float fHeight, float fOffset, float fHeightOffset )
{
	D3DXVECTOR3 vPos = pOwner->GetWorldPosition();
	D3DXVECTOR3 vDir = pOwner->GetTargetRot() * D3DXVECTOR3( 0.0f, 0.0f, -FLOAT1 );
	D3DXVec3Normalize( &vDir, &vDir );

	//지형 체크, 단 드랍존 일경우 현재 높이 유지
	D3DXVECTOR3 vPortal = vPos + vDir * fOffset;
	float fPortal = pStage->GetMapHeight( vPortal.x, vPortal.z );
	if( 0.0f < fPortal )
		vPortal.y = fPortal + fHeight + fHeightOffset;

	return vPortal;
}

void ioJumperItem::MakeMapEffect( IN ioPlayStage* pStage, IN ioBaseChar *pOwner, IN const ioHashString& szEffectName, IN D3DXVECTOR3 vPos, OUT UniqueObjID& ObjID )
{
	ioMapEffect* pMapEffect = pStage->CreateMapEffect( szEffectName, vPos, D3DXVECTOR3( FLOAT1, FLOAT1, FLOAT1 ) );
	if( pMapEffect )
	{
		ObjID = pMapEffect->GetUniqueID();
	}
}

D3DXVECTOR3 ioJumperItem::GetGrapplingTargetPos( ioBaseChar *pOwner )
{
	if( m_fGrapplingOffsetGapRange > 0.0f )
	{
		D3DXVECTOR3 vPos = ioMath::VEC3_ZERO;

		vPos = pOwner->GetMidPositionByRate( m_fGrapplingOffsetHeightRate );		
		D3DXVECTOR3 vDir = pOwner->GetTargetRot() * D3DXVECTOR3( 0.0f, 0.0f, -FLOAT1 );
		D3DXVec3Normalize( &vDir, &vDir );

		if( m_fGrapplingOffsetGapRange > 0.0f )
			vPos += m_fGrapplingOffsetGapRange * vDir;

		return vPos;
	}

	return ioMath::VEC3_ZERO;
}

void ioJumperItem::OnGrapplingPre( ioBaseChar *pOwner )
{
	if( m_dwGrapperPreEndTime < FRAMEGETTIME() )
	{
		MakePortal( pOwner->GetCreator(), pOwner );
		ChangeToGrapplingMove( pOwner );
	}
}

void ioJumperItem::ChangeToGrapplingMove( ioBaseChar *pOwner )
{
	m_AttackState = AS_GRAPPING_MOVE;

	DWORD dwEndTime		= 0;
	DWORD dwKeyReserve	= 0;
	SetActionAni( pOwner, m_GrapperMoveAnimation, m_GrapperMoveAnimationRate, 0, dwEndTime, dwKeyReserve );
}

void ioJumperItem::OnGrapplingMove( ioBaseChar *pOwner )
{
	D3DXVECTOR3 vMoveDir = pOwner->GetTargetRot() * D3DXVECTOR3( 0.0f, 0.0f, -FLOAT1 );
	if( ProcessMove( pOwner, vMoveDir, m_fGrapAfterMoveSpeed, m_fInPortalOffset, false, m_fCurrGrapAfterMove ) == MS_END )
	{
		if( pOwner->IsNeedProcess() )
			ChangeToTeleport( pOwner );
	}
}

ioJumperItem::MoveState ioJumperItem::ProcessMove( IN ioBaseChar *pOwner, IN const D3DXVECTOR3& vMoveDir, IN float fMoveSpeed, IN float fMoveMaxRange, IN bool bCheckLanding, OUT float& fCurrRange )
{
	if( !pOwner )
		return MS_END;

	DWORD dwCurTime		= FRAMEGETTIME();
	float fTimePerSec	= g_FrameTimer.GetSecPerFrame();

	float fDistance = fMoveSpeed * fTimePerSec;

	MoveState eRangeLimit = MS_END;
	D3DXVECTOR3 vMoveAmt( 0.0f, 0.0f, 0.0f );
	if( fCurrRange + fDistance < fMoveMaxRange )
	{
		eRangeLimit = MS_MOVE;
		vMoveAmt = vMoveDir * fDistance;
		fCurrRange += fDistance;
	}
	else
	{
		eRangeLimit = MS_END;

		//맥스 범위를 넘어갔다면 종료 처리
		float fGapDistance = fMoveMaxRange - fCurrRange;
		if( fGapDistance <= 0.0f )
			return MS_END;

		vMoveAmt = vMoveDir * fGapDistance;
	}

	bool bCol = false;
	ioPlayStage *pStage = pOwner->GetCreator();
	if( pStage && pStage->ReCalculateMoveVectorByTerrain( pOwner, &vMoveAmt, bCol ) )
	{		
		pOwner->Translate( vMoveAmt );
	}

	D3DXVECTOR3 vPos = pOwner->GetWorldPosition();
	if( pOwner->CheckAxisLimit( &vPos ) )
	{
		pOwner->SetWorldPosition( vPos );
	}

	float fHeightGap = pOwner->GetWorldPosition().y - pOwner->GetBottomHeight();
	if( bCheckLanding && fHeightGap <= FLOAT10 )
	{
		return MS_LANDING;
	}

	return eRangeLimit;
}


void ioJumperItem::ChangeToTeleport( ioBaseChar *pOwner )
{
	m_fCurrGrapAfterMove	= 0.0f;
	m_AttackState			= AS_TELEPORT;

	D3DXVECTOR3 vOutPortal = pOwner->GetWorldPosition();
	if( pOwner->IsNeedProcess() )
	{
		if( pOwner->CheckCurDirKey() != ioUserKeyInput::DKI_NONE )
			pOwner->SetTargetRotToDirKey( pOwner->CheckCurDirKey() );

		float fHeight = 0.0f;
		if( !pOwner->IsCharDropZonePos() )
			fHeight = pOwner->GetWorldPosition().y - pOwner->GetCreator()->GetMapHeightNoneEntity( pOwner->GetWorldPosition().x, pOwner->GetWorldPosition().z );

		vOutPortal = CalcPortalPos( pOwner->GetCreator(), pOwner, fHeight, m_fOutPortalOffset, m_fOutPortalHeightOffset );
	}

	MakeMapEffect( pOwner->GetCreator(), pOwner, m_szOutPortalMapEffect, vOutPortal, m_OutPortalID );
	pOwner->SetWorldPosition( vOutPortal );
		
	DWORD dwKeyReserve = 0;	
	DWORD dwCurTime = FRAMEGETTIME();
	pOwner->InitExtendAttackTagInfo();
	ClearCancelInfo();

	m_dwTeleportEndTime = FRAMEGETTIME();
	SetAttack( pOwner, &m_TeleportEndAttribute, m_dwTeleportEndTime, m_dwKeyReserveTime );

	if( pOwner->IsNeedSendNetwork() )
	{
		SP2Packet kPacket( CUPK_ITEM_GAUGE_OK );
		FillItemGaugePacketHeader( pOwner, kPacket );
		kPacket << static_cast<byte>( AS_TELEPORT );
		kPacket << pOwner->GetTargetRot();
		kPacket << pOwner->GetWorldPosition();		
		P2PNetwork::SendToAllPlayingUser( kPacket );
	}
}

void ioJumperItem::OnTeleport( ioPlayStage* pStage, ioBaseChar *pOwner )
{
	if( m_dwTeleportEndTime < FRAMEGETTIME() )
	{
		ChangeToTeleportEnd( pOwner );
	}
}

void ioJumperItem::ChangeToTeleportEnd( ioBaseChar *pOwner )
{
	m_AttackState = AS_TELEPORT_END;

	DestroyMapEffect( pOwner->GetCreator(), pOwner );

	float fBottomHeight = pOwner->GetBottomHeight();
	if( pOwner->GetWorldPosition().y - fBottomHeight < m_fCancelDashEndJumpJudgment )
	{
		Init();
		pOwner->SetState( CS_DELAY );
	}
	else
	{
		Init();
		pOwner->SetSkillNextJumpAmt( m_fTeleportAttackNextJumpAmt );
	}
}


// 캔슬 대쉬 --------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

void ioJumperItem::CheckExtendDashState( ioBaseChar *pOwner )
{
	if( m_pExtendDash )
	{
		ioChargeComboDash* pChargeDash = ToChargeComboDash( m_pExtendDash );
		if( pChargeDash && IsConditionalSpecialDashReserve( pOwner ) )
		{
			pChargeDash->CheckCancelReserveByConditionalSpecialDash( pOwner );
		}
		m_pExtendDash->ProcessDash( pOwner );
	}
}

void ioJumperItem::SetChangeConditionalSpecialDash( ioBaseChar *pOwner, ioUserKeyInput::DirKeyInput eCurrDir, int eCallerType )
{
	if( !pOwner )
		return;

	ClearKeyReserve();
	ClearCancelInfo();

	pOwner->ClearSkillReserveKey();
	pOwner->ClearReserveKeyInput();
	pOwner->ClearReservedSliding();
	pOwner->SetSkillNextJumpAmt( 0.0f );
	pOwner->ClearDirDoubleClick();
	pOwner->InitActionStopDelay( true );

	if( pOwner->IsNeedProcess() )
	{
		ReserveAttackComoboCount( pOwner, eCallerType );
	}

	pOwner->SetState( CS_JUMPER_SPECIAL );

	m_AttackState						= AS_CANCEL_TELEPORT_PRE;
	m_dwCancelDashTeleportStartEndTime	= 0;
	m_dwKeyReserveTime					= 0;
	m_eCurrCallerType					= eCallerType;

	SetActionAni( pOwner, m_CancelDashTeleportStartAnimation, m_CancelDashTeleportStartAnimationRate, 0, m_dwCancelDashTeleportStartEndTime, m_dwKeyReserveTime );

	float fYawD = 180.0f + 45.0f * ( eCurrDir - ioUserKeyInput::DKI_UP );
	fYawD = ioMath::ArrangeHead( fYawD );

	D3DXQUATERNION qtRotate;
	D3DXQuaternionRotationYawPitchRoll( &qtRotate, DEGtoRAD( fYawD ), 0.0f, 0.0f );
	D3DXVECTOR3 vSightDir = qtRotate * D3DXVECTOR3( 0.0f, 0.0f, -FLOAT1 );
	D3DXVECTOR3 vMoveDir( vSightDir.x, 0.0f, vSightDir.z );
	D3DXVec3Normalize( &vMoveDir, &vMoveDir );

	D3DXQUATERNION qtTargetRot;
	ioMath::CalcDirectionQuaternion( qtTargetRot, -vMoveDir );
	pOwner->SetTargetRotToDirKey( eCurrDir );

	D3DXVECTOR3 vPos = pOwner->GetWorldPosition();
	m_vTeleportPos = vPos + vMoveDir * m_fCancelDashTeleport;

	float fHeight = pOwner->GetCreator()->GetMapHeight( m_vTeleportPos.x, m_vTeleportPos.z );
	if( 0.0f < fHeight && m_vTeleportPos.y < fHeight )
		m_vTeleportPos.y = fHeight;

	if( pOwner->IsNeedSendNetwork() )
	{
		SP2Packet kPacket( CUPK_ITEM_GAUGE_OK );
		FillItemGaugePacketHeader( pOwner, kPacket );
		kPacket << static_cast<byte>( AS_CANCEL_TELEPORT_PRE );
		kPacket << pOwner->GetTargetRot();
		kPacket << pOwner->GetWorldPosition();
		kPacket << (byte)eCurrDir;
		kPacket << (byte)eCallerType;
		kPacket << m_iCurReserveCombo;
		kPacket << m_iCurCombo;
		P2PNetwork::SendToAllPlayingUser( kPacket );
	}
}

void ioJumperItem::ReserveAttackComoboCount( ioBaseChar *pOwner, int eCallerType )
{
	if( !pOwner )
		return;

	if( eCallerType == ioAttackableItem::CCSD_JUMP )
	{
		ioChargeComboJump3* pJump = ToChargeComboJump3( m_pExtendJump );
		if( pJump )
		{
			m_iCurReserveCombo = pJump->GetCurrComboCnt();
		}
	}
	else if( eCallerType == ioAttackableItem::CCSD_DASH_ATTACK )
	{
		ioChargeComboDash* pDash = ToChargeComboDash( m_pExtendDash );
		if( pDash )
		{
			m_iCurReserveCombo = pDash->GetCurrComboCnt();
		}
	}
}

void ioJumperItem::OnCancelTeleportPre( ioBaseChar *pOwner )
{
	if( m_dwCancelDashTeleportStartEndTime < FRAMEGETTIME() )
	{
		ChangeToCancelTeleport( pOwner );
	}
}

void ioJumperItem::ChangeToCancelTeleport( ioBaseChar *pOwner )
{	
	pOwner->SetWorldPosition( m_vTeleportPos );

	m_AttackState						= AS_CANCEL_TELEPORT;
	m_dwKeyReserveTime					= 0;
	m_dwCancelDashTeleportEndEndTime	= 0;

	SetUseExtraGauge( GetNeedCancelBullet() );
	SetActionAni( pOwner, m_CancelDashTeleportEndAnimation, m_CancelDashTeleportEndAnimationRate, 0, m_dwCancelDashTeleportEndEndTime, m_dwKeyReserveTime );
	SetCurAttackAniForInputDashS( m_CancelDashTeleportEndAnimation, m_CancelDashTeleportEndAnimationRate );
}

void ioJumperItem::OnCancelTeleport( ioBaseChar *pOwner )
{
	if( m_dwCancelDashTeleportEndEndTime < FRAMEGETTIME() )
	{
		float fBottomHeight = pOwner->GetBottomHeight();
		if( pOwner->GetWorldPosition().y - fBottomHeight < m_fCancelDashEndJumpJudgment )
		{
			m_AttackState			= AS_NONE;
			m_bEnableAttackState	= false;

			ProcessReserveKeyInput( pOwner );
		}
		else
		{
			pOwner->SetSKillEndJumpState( m_fCancelDashEndNextJumpAmt );
		}
	}
	else
	{
		pOwner->CheckExtraAniJump();

		CheckReserve( pOwner );
		if( ProcessCancel( pOwner ) )
		{
			Init();
			return;
		}

		if( pOwner->IsAttackKey() && pOwner->IsCanNormalAttack() && !pOwner->IsBuffLimitAttack() && GetNeedCancelBullet() <= GetCurBullet() )
		{
			ChangeToCancelTeleportAfter( pOwner );
		}
	}
}

void ioJumperItem::ChangeToCancelTeleportAfter( ioBaseChar *pOwner )
{
	ioUserKeyInput::DirKeyInput eCurrKey = pOwner->CheckCurDirKey();

	switch( m_eCurrCallerType )
	{
	case ioAttackableItem::CCSD_DASH_ATTACK:
		{
			ChangeToCancelTeleportAfterDashAttack( pOwner, eCurrKey );
		}
		break;
	case ioAttackableItem::CCSD_JUMP:
		{
			ChangeToCancelTeleportAfterJumpAttack( pOwner, eCurrKey );
		}
		break;
	default:
		{
			m_iCurCombo = m_iCurCombo + 1;
			ChangeToCancelTeleportAfterNormalAttack( pOwner, eCurrKey );
		}
		break;
	}
}

void ioJumperItem::ChangeToCancelTeleportAfterNormalAttack( ioBaseChar *pOwner, ioUserKeyInput::DirKeyInput eCurrKey )
{
	if( eCurrKey != ioUserKeyInput::DKI_NONE )
		pOwner->SetTargetRotToDirKey( eCurrKey );

	pOwner->SetState( CS_ATTACK );
	pOwner->SetChargingState( false );
	pOwner->SetAttackMoveEnable( false );

	ChangeToAttackFire( pOwner );

	if( pOwner->IsNeedSendNetwork() )
	{
		SP2Packet kPacket( CUPK_ITEM_GAUGE_OK );
		FillItemGaugePacketHeader( pOwner, kPacket );
		kPacket << static_cast<byte>( AS_CANCEL_TELEPORT );
		kPacket << pOwner->GetTargetRot();
		kPacket << pOwner->GetWorldPosition();
		kPacket << m_eCurrCallerType;
		kPacket << (byte)eCurrKey;
		kPacket << m_iCurCombo;
		P2PNetwork::SendToAllPlayingUser( kPacket );
	}
}

void ioJumperItem::ChangeToCancelTeleportAfterDashAttack( ioBaseChar *pOwner, ioUserKeyInput::DirKeyInput eCurrKey )
{
	if( eCurrKey != ioUserKeyInput::DKI_NONE )
		pOwner->SetTargetRotToDirKey( eCurrKey );

	pOwner->SetState( CS_ATTACK );
	pOwner->SetChargingState( false );
	pOwner->SetAttackMoveEnable( false );

	ioChargeComboDash* pDash = ToChargeComboDash( m_pExtendDash );
	if( pDash )
	{
		pDash->StartDash( pOwner );

		if( m_iCurReserveCombo < pOwner->GetExtraDashAttackMax() )
		{
			pDash->SetCurrComboCnt( m_iCurReserveCombo );
			pDash->SetCombo( true );
		}
		else
		{
			pDash->SetCurrComboCnt( 0 );
			pDash->SetCombo( false );
		}
		m_iCurReserveCombo = 0;
	}

	if( pOwner->IsNeedSendNetwork() )
	{
		SP2Packet kPacket( CUPK_ITEM_GAUGE_OK );
		FillItemGaugePacketHeader( pOwner, kPacket );
		kPacket << static_cast<byte>( AS_CANCEL_TELEPORT );
		kPacket << pOwner->GetTargetRot();
		kPacket << pOwner->GetWorldPosition();
		kPacket << m_eCurrCallerType;
		kPacket << (byte)eCurrKey;
		kPacket << m_iCurReserveCombo;
		P2PNetwork::SendToAllPlayingUser( kPacket );
	}
}

void ioJumperItem::ChangeToCancelTeleportAfterJumpAttack( ioBaseChar *pOwner, ioUserKeyInput::DirKeyInput eCurrKey )
{
	if( !pOwner || !pOwner->IsNeedProcess() )
		return;

	if( eCurrKey != ioUserKeyInput::DKI_NONE )
		pOwner->SetTargetRotToDirKey( eCurrKey );

	//아래 SetSKillEndJumpState() 호출하면서 상태값이 바뀜	
	pOwner->SetSKillEndJumpState( m_fCancelDashEndNextJumpAmt );

	ioChargeComboJump3* pJump = ToChargeComboJump3( m_pExtendJump );
	if( pJump )
	{
		pJump->SetCurrComboCnt( m_iCurReserveCombo );
		pJump->SetReserveJumpAttack( pOwner );
		m_iCurReserveCombo = 0;
	}

	if( pOwner->IsNeedSendNetwork() )
	{
		SP2Packet kPacket( CUPK_ITEM_GAUGE_OK );
		FillItemGaugePacketHeader( pOwner, kPacket );
		kPacket << static_cast<byte>( AS_CANCEL_TELEPORT );
		kPacket << pOwner->GetTargetRot();
		kPacket << pOwner->GetWorldPosition();
		kPacket << m_eCurrCallerType;
		kPacket << (byte)eCurrKey;
		kPacket << m_iCurReserveCombo;
		P2PNetwork::SendToAllPlayingUser( kPacket );
	}
}

//--------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

bool ioJumperItem::IsConditionalSpecialDashReserve( ioBaseChar *pOwner )
{
	if( pOwner->GetState() == CS_JUMPER_SPECIAL && m_AttackState != AS_CANCEL_TELEPORT )
	{
		return false;
	}

	if( GetCurBullet() < GetNeedCancelBullet() )
	{
		return false;
	}

	return true;
}

bool ioJumperItem::IsChargeAttackState( ioBaseChar *pOwner )
{
	if( pOwner->GetState() == CS_ATTACK )
	{
		return true; 
	}

	return false;
}

bool ioJumperItem::IsEnableAttackDashCheck( ioBaseChar *pOwner, bool bCheck )
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

void ioJumperItem::OnItemGaugeOK( ioBaseChar *pOwner, SP2Packet &rkPacket )
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
	case AS_EXTEND_ATTACK:
		{
			byte eFireTimeType = 0;
			rkPacket >> eFireTimeType;
			ChangeToExtendAttackFire( pOwner,(FireTimeType)eFireTimeType );
		}
		break;
	case AS_GRAPPING_PRE:
		{

			D3DXVECTOR3 vPos;
			rkPacket >> vPos;
			pOwner->SetWorldPosition( vPos );

			ioHashString szTargetName;
			rkPacket >> szTargetName;			

			SetItemGrapplingState( pOwner, szTargetName );
		}
		break;
	case AS_TELEPORT:
		{
			D3DXVECTOR3 vPos;
			rkPacket >> vPos;
			pOwner->SetWorldPosition( vPos );

			ChangeToTeleport( pOwner );
		}
		break;
	case AS_CANCEL_TELEPORT_PRE:
		{
			D3DXVECTOR3 vPos;
			rkPacket >> vPos;
			pOwner->SetWorldPosition( vPos );

			byte eDirKey;
			byte eCallerType;
			rkPacket >> eDirKey;
			rkPacket >> eCallerType;
			rkPacket >> m_iCurReserveCombo;
			rkPacket >> m_iCurCombo;
			SetChangeConditionalSpecialDash( pOwner, (ioUserKeyInput::DirKeyInput)eDirKey, eCallerType );

		}
		break;
	case AS_CANCEL_TELEPORT:
		{
			D3DXVECTOR3 vPos;
			rkPacket >> vPos;
			pOwner->SetWorldPosition( vPos );
			
			rkPacket >> m_eCurrCallerType;

			byte eCurrKey;
			rkPacket >> eCurrKey;

			switch( m_eCurrCallerType )
			{
			case ioAttackableItem::CCSD_DASH_ATTACK:
				{
					rkPacket >> m_iCurReserveCombo;
					ChangeToCancelTeleportAfterDashAttack( pOwner, (ioUserKeyInput::DirKeyInput)eCurrKey );
				}
				break;
			case ioAttackableItem::CCSD_JUMP:
				{
					rkPacket >> m_iCurReserveCombo;
					ChangeToCancelTeleportAfterJumpAttack( pOwner, (ioUserKeyInput::DirKeyInput)eCurrKey );
				}
				break;
			default:
				{
					rkPacket >> m_iCurCombo;
					ChangeToCancelTeleportAfterNormalAttack( pOwner, (ioUserKeyInput::DirKeyInput)eCurrKey );
				}
				break;
			}
		}
		break;
	}	
}

void ioJumperItem::UpdateExtraData( ioBaseChar *pOwner )
{
	if( !pOwner )
		return;

	switch( pOwner->GetState() )
	{
	case CS_RUN:
	case CS_DASH:
	case CS_DELAY:
		{
			float fTimePerSec = g_FrameTimer.GetSecPerFrame();
			float fGauge = m_fExtraGaugeBaseInc * fTimePerSec;
			m_fCurExtraGauge += fGauge;
			m_fCurExtraGauge = min( m_fCurExtraGauge, m_fMaxExtraGauge );
		}
		break;
	}
}

int ioJumperItem::GetMaxBullet()
{
	return (int)m_fMaxExtraGauge;
}

int ioJumperItem::GetNeedBullet()
{
	return (int)m_fNeedGauge;
}

int ioJumperItem::GetNeedCancelBullet()
{
	return (int)m_fNeedCancelGauge;
}

int ioJumperItem::GetCurBullet()
{
	float fCurCoolTime = m_fCurExtraGauge;
	fCurCoolTime = max( 0, min( fCurCoolTime, m_fMaxExtraGauge ) );

	return (int)fCurCoolTime;
}

void ioJumperItem::SetCurBullet( int iCurBullet )
{
	if( iCurBullet < 0 )
		return;

	float fCurBullet = iCurBullet;
	fCurBullet = max( 0, min( fCurBullet, m_fMaxExtraGauge ) );

	m_fCurExtraGauge = fCurBullet;
}

void ioJumperItem::SetUseExtraGauge( float fAmt )
{
	m_fCurExtraGauge -= fAmt;
	m_fCurExtraGauge = max( 0.0f, m_fCurExtraGauge );
}

void ioJumperItem::SetExtraInfoToWeapon( ioWeapon *pWeapon, bool bJumpAttack )
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

//--------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

ioJumperSpecialSpState::ioJumperSpecialSpState()
{
	
}

ioJumperSpecialSpState::~ioJumperSpecialSpState()
{
}

void ioJumperSpecialSpState::Initialize( ioBaseChar* pOwner, CharState eCurrState )
{
	ioSpecialStateBase::Initialize( pOwner, eCurrState );
}

void ioJumperSpecialSpState::Finalize( ioBaseChar* pOwner, CharState eCurrState, CharState eNewState )
{
	ioSpecialStateBase::Finalize( pOwner, eCurrState, eNewState );

	ioJumperItem* pJumper = ToJumperItem( pOwner->GetPriorityItem( SPT_ATTACK ) );
	if( pJumper && eNewState != CS_ATTACK )
	{	
		pJumper->ClearSpecial( pOwner, eNewState );
	}
}

void ioJumperSpecialSpState::CheckSpecialState( ioBaseChar* pOwner )
{
	ioJumperItem* pJumper = ToJumperItem( pOwner->GetPriorityItem( SPT_ATTACK ) );
	if( pJumper )
	{
		pJumper->CheckSpecialState( pOwner );
	}
}

void ioJumperSpecialSpState::ProcessKeyInput( ioBaseChar* pOwner )
{
}

bool ioJumperSpecialSpState::ProcessMove( ioBaseChar* pOwner, float fTimePerSec, DWORD dwPreTime, DWORD dwCurTime )
{
	D3DXVECTOR3 vMoveDir = pOwner->GetTargetRot() * D3DXVECTOR3( 0.0f, 0.0f, -FLOAT1 ); 

	return ProcessForceMove( pOwner->GetCreator(), pOwner, vMoveDir, g_FrameTimer.GetSecPerFrame() );
}

bool ioJumperSpecialSpState::ProcessForceMove( ioPlayStage* pStage, ioBaseChar* pOwner, D3DXVECTOR3 vDir, float fTimePerSec )
{
	if( !pOwner )
		return false;

	if( !pStage )
		return false;
	bool bCurMove = false;
	D3DXVECTOR3 vTotalMove( 0.0f, 0.0f, 0.0f );

	float fCurTimeGap = fTimePerSec * FLOAT1000;
	while( fCurTimeGap > 0.0f )
	{
		float fDivideTimePerSec = min( fCurTimeGap, MIN_FRAME_TIME ) * 0.001f;
		fCurTimeGap -= MIN_FRAME_TIME;

		D3DXVECTOR3 vMoveAmt;
		if( pOwner->ProcessForceMove( fDivideTimePerSec, vMoveAmt ) )
		{
			vTotalMove	= vTotalMove + vMoveAmt;
			bCurMove	= true;
		}
	}

	if( bCurMove )
	{
		bool bCol = false;
		if( pStage->ReCalculateMoveVectorByTerrain( pOwner, &vTotalMove, bCol ) )
		{
			float fPreHeight = pOwner->GetBottomHeight();
			pOwner->Translate( vTotalMove );

			// 빠른 ForceMove로 경사면을 통과하는 경우에 대한 보강
			if( pOwner->GetForceAmt() != 0.0f )
			{
				float fMoveHeight = pOwner->GetBottomHeight();
				if( fPreHeight > 0.0f && fMoveHeight <= FLOAT1 )
				{
					D3DXVECTOR3 vCurPos = pOwner->GetWorldPosition();
					float fTopHeight = pStage->GetMapHeight( vCurPos.x, vCurPos.z, pOwner, false, pOwner->GetMidHeightByRate( FLOAT1 ) );
					if( fTopHeight > FLOAT1 )
					{
						vCurPos.y = max( fTopHeight, fPreHeight );
						pOwner->SetWorldPosition( vCurPos );
					}
				}
			}
		}
	}

	D3DXVECTOR3 vPos = pOwner->GetWorldPosition();
	if( pOwner->GetCreator() && pOwner->IsAxisLimit() )
	{
		pOwner->GetCreator()->CheckAxisLimit( &vPos );
		pOwner->SetWorldPosition( vPos );
	}

	return true;
}

void ioJumperSpecialSpState::CheckFallStateChange( ioBaseChar* pOwner, float fMinFallHeightRate, float fBottomHeight, bool bBottomMaxCheck, float fMaxBottomHeight )
{
	ioJumperItem* pJumper = ToJumperItem( pOwner->GetPriorityItem( SPT_ATTACK ) );
	if( pJumper )
	{
		switch( pJumper->GetAttackState() )
		{
		case ioJumperItem::AS_NONE:
		case ioJumperItem::AS_EXTEND_ATTACK:
		case ioJumperItem::AS_CANCEL_TELEPORT:
		case ioJumperItem::AS_TELEPORT_END:
			if( 0.0f < pOwner->GetSkillNextJumpAmt() )
			{
				pOwner->SetSKillEndJumpState( pOwner->GetSkillNextJumpAmt() );
			}
			break;
		}
	}
	else
	{
		ioSpecialStateBase::CheckFallStateChange( pOwner, fMinFallHeightRate, fBottomHeight, bBottomMaxCheck, fMaxBottomHeight );
	}
}