

#include "stdafx.h"

#include "ioDruidItem.h"
#include "ioDruidObjectItem.h"
#include "ioChargeComboJump3.h"

#include "ioBaseChar.h"
#include "ioNpcChar.h"
#include "WeaponAttribute.h"
#include "FindPredicateImpl.h"

#include "ioPlayStage.h"

#include "WeaponDefine.h"

ioDruidObjectItem::ioDruidObjectItem() : m_EquipObjectState( ioDruidItem::EOS_NONE )
{
	Init();
}

ioDruidObjectItem::ioDruidObjectItem( const ioDruidObjectItem &rhs )
	: ioObjectItem( rhs ),
	m_EquipEffect( rhs.m_EquipEffect ),
	m_EquipObjectState( ioDruidItem::EOS_NONE ),
	m_PreAnimationList( rhs.m_PreAnimationList ),
	m_ExtendAttack( rhs.m_ExtendAttack ),
	m_GrappedAttack( rhs.m_GrappedAttack ),
	m_fGrapplingRange( rhs.m_fGrapplingRange ),
	m_fGrapplingMaxAngle( rhs.m_fGrapplingMaxAngle ),
	m_fGrapplingMinAngle( rhs.m_fGrapplingMinAngle ),
	m_dwGrapplingRotateSpeed( rhs.m_dwGrapplingRotateSpeed ),
	m_fGrapplingGapRange( rhs.m_fGrapplingGapRange ),
	m_GrapplingBiteMotion( rhs.m_GrapplingBiteMotion ),
	m_GrapplingBiteMotionRate( rhs.m_GrapplingBiteMotionRate ),
	m_GrapplingLoopMotion( rhs.m_GrapplingLoopMotion ),
	m_GrapplingPushMotion( rhs.m_GrapplingPushMotion ),
	m_fGrapplingPushMotionRate( rhs.m_fGrapplingPushMotionRate ),
	m_GrapplingCancelMotion( rhs.m_GrapplingCancelMotion ),
	m_fGrapplingCancelMotionRate( rhs.m_fGrapplingCancelMotionRate ),	
	m_TargetGrapplingInfo( rhs.m_TargetGrapplingInfo ),
	m_PushedGrapplingInfo( rhs.m_PushedGrapplingInfo ),
	m_CrouchGatherAnimation( rhs.m_CrouchGatherAnimation ),
	m_CrouchGatherMoveFront( rhs.m_CrouchGatherMoveFront ),
	m_CrouchGatherMoveBack( rhs.m_CrouchGatherMoveBack ),
	m_CrouchGatherMoveRight( rhs.m_CrouchGatherMoveRight ),
	m_CrouchGatherMoveLeft( rhs.m_CrouchGatherMoveLeft ),
	m_fCrouchGatherRate( rhs.m_fCrouchGatherRate ),
	m_CrouchGatherEffect( rhs.m_CrouchGatherEffect ),
	m_CrouchAttack( rhs.m_CrouchAttack ),
	m_EquipMaleMeshList( rhs.m_EquipMaleMeshList ), 
	m_EquipFemaleMeshList( rhs.m_EquipFemaleMeshList ),
	m_fChangeJumpPower( rhs.m_fChangeJumpPower )

{
	Init();
}

ioDruidObjectItem::~ioDruidObjectItem()
{
	Init();
}

void ioDruidObjectItem::Init()
{
	m_ChargeState				= CS_NONE;
	m_AttackKeyType				= AKT_NONE;

	m_iCurCombo					= 0;
	m_CurrDirectionKey			= ioUserKeyInput::DKI_NONE;
	m_FireStartDirKey			= ioUserKeyInput::DKI_NONE;

	m_dwAttackStartTime			= 0;
	m_GrapplingBiteEndTime		= 0;
	m_dwGrapplingLoopEndTime	= 0;
	m_dwGrapplingPushEndTime	= 0;
	m_dwGrapplingCancelEndTime	= 0;
	
	m_bTargetRot				= false;
	m_bReserveRelease			= false;
	m_bFirstFireMoveMotion		= true;

	m_GrapplingTargetName.Clear();
	m_GrappedTargetName.Clear();
}

void ioDruidObjectItem::SetNoneState( ioBaseChar* pOwner, int eNewState )
{
	if( eNewState != CS_GRAPPLING )
		Init();

	EndEffect( pOwner );
}

void ioDruidObjectItem::EndEffect( ioBaseChar* pOwner )
{
	if( m_CrouchGatherEffectID != -1 )
	{
		pOwner->EndEffect( m_CrouchGatherEffectID );
		m_CrouchGatherEffectID = -1;
	}
}

void ioDruidObjectItem::LoadProperty( ioINILoader &rkLoader )
{
	ioObjectItem::LoadProperty( rkLoader );

	char szBuf[MAX_PATH];
	char szKey[MAX_PATH];

	int iPreCount = rkLoader.LoadInt_e( "pre_animation_cnt", 0 );
	for( int i = 0; i < iPreCount; ++i )
	{
		wsprintf_e( szKey, "pre_animation%d", i+1 );
		rkLoader.LoadString( szKey, "", szBuf, MAX_PATH );
		m_PreAnimationList.push_back( ioHashString( szBuf ) );
	}
	
	m_fChangeJumpPower = rkLoader.LoadFloat_e( "change_jump_power", 0.0f );

	LoadAttackAttribute( "extend_attack", m_ExtendAttack, rkLoader );
	LoadAttackAttribute( "grapped_attack", m_GrappedAttack, rkLoader );
	LoadAttackAttribute( "crouch_attack", m_CrouchAttack, rkLoader );

	LoadGrappling( rkLoader );

	rkLoader.LoadString_e( "equip_effect", "", szBuf, MAX_PATH );
	m_EquipEffect = szBuf;
}

void ioDruidObjectItem::LoadGrappling( ioINILoader &rkLoader )
{
	char szBuf[MAX_PATH];

	m_fGrapplingRange			= rkLoader.LoadFloat_e( "grappling_target_range", 0.0f );
	m_fGrapplingMaxAngle		= rkLoader.LoadFloat_e( "grappling_target_max_angle", 0.0f );
	m_fGrapplingMinAngle		= rkLoader.LoadFloat_e( "grappling_target_min_angle", 0.0f );
	m_dwGrapplingRotateSpeed	= rkLoader.LoadInt_e( "grappling_rotate_speed", 0 );
	m_fGrapplingGapRange		= rkLoader.LoadFloat_e( "grappling_gap_range", 0.0f );

	//GRAPPLING BITE(PULL)
	rkLoader.LoadString_e( "grappling_bite_motion", "", szBuf, MAX_PATH );
	m_GrapplingBiteMotion		= szBuf;
	m_GrapplingBiteMotionRate	= rkLoader.LoadFloat_e( "grappling_bite_motion_rate", FLOAT1 );

	//GRAPPLING CANCEL
	rkLoader.LoadString_e( "grappling_cancel_motion", "", szBuf, MAX_PATH );
	m_GrapplingCancelMotion			= szBuf;
	m_fGrapplingCancelMotionRate	= rkLoader.LoadFloat_e( "grappling_cancel_motion_rate", FLOAT1 );

	//GRAPPLING LOOP
	rkLoader.LoadString_e( "grappling_loop_motion", "", szBuf, MAX_PATH );
	m_GrapplingLoopMotion = szBuf;

	//GRAPPLING PUSH
	rkLoader.LoadString_e( "grappling_push_motion", "", szBuf, MAX_PATH );
	m_GrapplingPushMotion		= szBuf;
	m_fGrapplingPushMotionRate	= rkLoader.LoadFloat_e( "grappling_push_motion_rate", FLOAT1 );

	LoadGrapplingTarget( rkLoader );
	LoadGrapplingPushed( rkLoader );
	LoadCrouchGathering( rkLoader );
	LoadMesh( rkLoader ); 
}

void ioDruidObjectItem::LoadGrapplingTarget( ioINILoader &rkLoader )
{
	char szBuf[MAX_PATH];
	char szKey[MAX_PATH];
	rkLoader.LoadString_e( "target_grappling_bite_motion", "", szBuf, MAX_PATH );
	m_TargetGrapplingInfo.m_TargetGrapplingPullMotion		= szBuf;
	m_TargetGrapplingInfo.m_fTargetGrapplingPullMotionRate	= rkLoader.LoadFloat_e( "target_grappling_bite_motion_rate", FLOAT1 );

	rkLoader.LoadString_e( "target_grappling_loop_motion", "", szBuf, MAX_PATH );
	m_TargetGrapplingInfo.m_TargetGrapplingMotion = szBuf;

	rkLoader.LoadString_e( "target_grappling_cancel_motion", "", szBuf, MAX_PATH );
	m_TargetGrapplingInfo.m_TargetGrapplingCancelMotion			= szBuf;
	m_TargetGrapplingInfo.m_fTargetGrapplingCancelMotionRate	= rkLoader.LoadFloat_e( "target_grappling_cancel_motion_rate", FLOAT1 );

	m_TargetGrapplingInfo.m_dwGrapplingMinTime		= rkLoader.LoadInt_e( "target_grappling_loop_min_time", 0 );
	m_TargetGrapplingInfo.m_dwGrapplingMaxTime		= rkLoader.LoadInt_e( "target_grappling_loop_max_time", 0 );
	m_TargetGrapplingInfo.m_dwGrapplingFastValue	= rkLoader.LoadInt_e( "target_grappling_loop_min_fast_value", 0 );

	int nCount = rkLoader.LoadInt_e( "target_grappling_bite_buff_count", 0 );
	for( int i = 0; i <nCount; ++i )
	{
		wsprintf_e( szKey, "target_grappling_bite_buff%d", i+1 );
		rkLoader.LoadString( szKey, "", szBuf, MAX_PATH );
		m_TargetGrapplingInfo.m_TargetGrapplingBuff.push_back( szBuf );
	}
}

void ioDruidObjectItem::LoadGrapplingPushed( ioINILoader &rkLoader )
{
	char szBuf[MAX_PATH];
	char szKey[MAX_PATH];

	rkLoader.LoadString_e( "target_grappling_pushed_motion", "", szBuf, MAX_PATH );
	m_PushedGrapplingInfo.m_GrapplingPushedMotion		= szBuf;
	m_PushedGrapplingInfo.m_fGrapplingPushedMotionRate	= rkLoader.LoadFloat_e( "target_grappling_pushed_motion", FLOAT1 );

	rkLoader.LoadString_e( "target_grappling_pushed_loop_motion", "", szBuf, MAX_PATH );
	m_PushedGrapplingInfo.m_GrapplingPushedLoopMotion		= szBuf;
	m_PushedGrapplingInfo.m_fGrapplingPushedLoopMotionRate	= rkLoader.LoadFloat_e( "target_grappling_pushed_loop_motion_rate", FLOAT1 );
	m_PushedGrapplingInfo.m_dwGrapplingPushedDuration		= rkLoader.LoadInt_e( "target_grappling_pushed_duration", 0 );
	m_PushedGrapplingInfo.m_fGrapplingPushedSpeed			= rkLoader.LoadFloat_e( "target_grappling_pushed_speed", 0.0f );
	m_PushedGrapplingInfo.m_dwGrapplingPushedRotSpeed		= rkLoader.LoadInt_e( "target_grappling_pushed_rotate_speed", 0 );
	m_PushedGrapplingInfo.m_dwGrapplingPushedColTime		= rkLoader.LoadInt_e( "target_grappling_pushed_col_time", 0 );

	int iBuffCnt = rkLoader.LoadInt_e( "grappling_pushed_buff_cnt", 0 );
	m_PushedGrapplingInfo.m_GrapplingPushedBuffList.clear();
	m_PushedGrapplingInfo.m_GrapplingPushedBuffList.reserve( iBuffCnt );
	for( int i = 0; i < iBuffCnt; ++i )
	{
		wsprintf_e( szKey, "grappling_pushed_buff%d", i+1 );
		rkLoader.LoadString( szKey, "", szBuf, MAX_PATH );

		m_PushedGrapplingInfo.m_GrapplingPushedBuffList.push_back( szBuf );
	}

	iBuffCnt = rkLoader.LoadInt_e( "grappling_col_buff_cnt", 0 );
	m_PushedGrapplingInfo.m_GrapplingColBuffList.clear();
	m_PushedGrapplingInfo.m_GrapplingColBuffList.reserve( iBuffCnt );
	for( int i = 0; i < iBuffCnt; ++i )
	{
		wsprintf_e( szKey, "grappling_col_buff%d", i+1 );
		rkLoader.LoadString( szKey, "", szBuf, MAX_PATH );
		m_PushedGrapplingInfo.m_GrapplingColBuffList.push_back( szBuf );
	}
}

void ioDruidObjectItem::LoadCrouchGathering( ioINILoader &rkLoader )
{
	char szBuf[MAX_PATH];
	rkLoader.LoadString_e( "crouch_gathering_animation", "", szBuf, MAX_PATH );
	m_CrouchGatherAnimation = szBuf;

	rkLoader.LoadString_e( "crouch_gathering_move_fr", "", szBuf, MAX_PATH );
	m_CrouchGatherMoveFront = szBuf;

	rkLoader.LoadString_e( "crouch_gathering_move_bk", "", szBuf, MAX_PATH );
	m_CrouchGatherMoveBack = szBuf;

	rkLoader.LoadString_e( "crouch_gathering_move_rt", "", szBuf, MAX_PATH );
	m_CrouchGatherMoveRight = szBuf;

	rkLoader.LoadString_e( "crouch_gathering_move_lt", "", szBuf, MAX_PATH );
	m_CrouchGatherMoveLeft = szBuf;

	m_fCrouchGatherRate = rkLoader.LoadFloat_e( "crouch_gathering_rate", FLOAT1 );

	rkLoader.LoadString_e( "crouch_gathering_effect", "", szBuf, MAX_PATH );
	m_CrouchGatherEffect = szBuf;
}

void ioDruidObjectItem::LoadMesh( ioINILoader &rkLoader )
{
	char szBuf[MAX_PATH];
	char szKey[MAX_PATH];

	int iMeshCnt = rkLoader.LoadInt_e( "equip_male_mesh_cnt", 0 );
	for( int i = 0; i<iMeshCnt; ++i )
	{
		wsprintf_e( szKey, "equip_male_mesh%d", i+1 );
		rkLoader.LoadString( szKey, "", szBuf, MAX_PATH );
		m_EquipMaleMeshList.push_back( szBuf );
	}

	iMeshCnt = rkLoader.LoadInt_e( "equip_female_mesh_cnt", 0 );
	for( int i = 0; i<iMeshCnt; ++i )
	{
		wsprintf_e( szKey, "equip_female_mesh%d", i+1 );
		rkLoader.LoadString( szKey, "", szBuf, MAX_PATH );
		m_EquipFemaleMeshList.push_back( szBuf );
	}
}

ioItem* ioDruidObjectItem::Clone()
{
	return new ioDruidObjectItem( *this );
}

ioObjectItem::ObjectSubType ioDruidObjectItem::GetObjectSubType() const
{
	return OST_DRUID;
}

bool ioDruidObjectItem::IsReleaseItem( const ioBaseChar* pOwner ) const
{
	if( pOwner )
	{
		ioWeaponItem *pItem = pOwner->GetWeapon();
		ioDruidItem* pDruid = ToDruidItem( pItem );
		if( pDruid )
		{
			if( m_bReserveRelease )
				return true;

			if( pDruid->GetCurBullet() > 0 )
				return false;
		}
	}

	return true;
}

bool ioDruidObjectItem::IsOneTimeWeaponItem() const
{
	return false;
}

void ioDruidObjectItem::OnEquiped( ioBaseChar *pOwner )
{
	ChangeMesh( pOwner );

	ioObjectItem::OnEquiped( pOwner );

	Init();

	pOwner->AttachEffect( m_EquipEffect );

	switch( m_EquipObjectState )
	{
	case ioDruidItem::EOS_NORMAL:
	case ioDruidItem::EOS_DASH:
		{
			if( pOwner->IsNeedProcess() )
			{
				pOwner->SetNormalAttack( m_iCurCombo );
				m_EquipObjectState = ioDruidItem::EOS_NONE;
			}
		}
		break;
	case ioDruidItem::EOS_JUMP:
		{
			ioChargeComboJump3* pJump = ToChargeComboJump3( m_pExtendJump );
			if( pJump )
			{
				pOwner->SetCurMoveSpeed( 0.0f );
				pOwner->SetJumpPower( m_fChangeJumpPower );
				pOwner->SetGravityAmt( 0.0f );
				pOwner->SetCurGravityGapAmt( 0.0f );
				pOwner->SetJumpState();
				pOwner->SetJumpAttackState();
				pJump->CheckComboJumpAttack( pOwner );
				pJump->SetComboJumpAttack( pOwner );
				m_EquipObjectState = ioDruidItem::EOS_NONE;
			}
		}
		break;
	}
}

void ioDruidObjectItem::OnReleased( ioBaseChar *pOwner )
{
	ReleaseMesh( pOwner );

	ioObjectItem::OnReleased( pOwner );

	EndEffect( pOwner );

	Init();
}

//--------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

void ioDruidObjectItem::ChangeMesh( ioBaseChar* pOwner )
{
	pOwner->SetInvisibleState( true, true, true );

	if( pOwner->IsMale() )
	{
		for( int i = 0; i < (int)m_EquipMaleMeshList.size(); ++i )
			pOwner->AddEntity( m_EquipMaleMeshList[i] );
	}
	else
	{
		for( int i = 0; i < (int)m_EquipFemaleMeshList.size(); ++i )
			pOwner->AddEntity( m_EquipFemaleMeshList[i] );
	}
}

void ioDruidObjectItem::ReleaseMesh( ioBaseChar* pOwner )
{
	if( pOwner->IsMale() )
	{
		for( int i = 0; i < (int)m_EquipMaleMeshList.size(); ++i )
			pOwner->DestroyEntity( m_EquipMaleMeshList[i] );
	}
	else
	{
		for( int i = 0; i < (int)m_EquipFemaleMeshList.size(); ++i )
			pOwner->DestroyEntity( m_EquipFemaleMeshList[i] );
	}

	pOwner->SetInvisibleState( false, true );

	if( pOwner->IsInvisibleFace() )
		pOwner->ShowFaceMesh( false );
}

//--------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

void ioDruidObjectItem::ChangeToEquipObjectState( int eState )
{
	m_EquipObjectState = eState;
}

//--------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

void ioDruidObjectItem::SetNormalAttack( ioBaseChar *pOwner, int iCurCombo )
{
	ClearCancelInfo();

	ChargeState eCurrState	= m_ChargeState;
	int iPreGatherCnt		= m_PreAnimationList.size();

	m_ChargeState			= CS_CHARGING;
	m_dwAttackStartTime		= FRAMEGETTIME();
	m_iCurCombo				= iCurCombo;
	m_bTargetRot			= false;

	pOwner->SetState( CS_ATTACK );
	pOwner->SetChargingState( true );
	pOwner->SetAttackMoveEnable( false );

	ioEntityGroup *pGrp = pOwner->GetGroup();
	pGrp->ClearAllActionAni( FLOAT100, true );
	pGrp->ClearAllLoopAni( FLOAT100, true );

	if( eCurrState == CS_CROUCH_GATHERING )
	{
		ChangeToCrouchAttackFire( pOwner );
	}
	else
	{
		//오브젝트 아이템이 생성될때 ioDruidItem의 EquipObjectItemByAttakable() 함수가 아래 m_EquipObjectState를 갱신
		switch( m_EquipObjectState )
		{
		case ioDruidItem::EOS_NORMAL:
			{
				FindGrappling( pOwner );
				ChangeToExtendAttackFire( pOwner );
			}
			break;
		case ioDruidItem::EOS_DASH:
			ChangeToCrouchAttackFire( pOwner );
			break;
		default:
			{
				FindGrappling( pOwner );
				if( 0 < iPreGatherCnt && COMPARE( iCurCombo, 0, iPreGatherCnt ) )
				{
					int iAniID = pGrp->GetAnimationIdx( m_PreAnimationList[iCurCombo] );
					float fTimeRate = (float)m_dwChargeTime / pGrp->GetAnimationFullTime( iAniID );
					pGrp->SetActionAni( iAniID, FLOAT100, FLOAT100, FLOAT1, FLOAT1/fTimeRate, 0.0f, 0.0f, true );
				}
				break;
			}
		}
		m_EquipObjectState = ioDruidItem::EOS_NONE;
	}
}

void ioDruidObjectItem::CheckNormalAttack( ioBaseChar *pOwner )
{
	DWORD dwCurTime = FRAMEGETTIME();

	switch( m_ChargeState )
	{
	case CS_CHARGING:
		OnCharging( pOwner );
		break;
	case CS_ATTACK_FIRE:
	case CS_EXTEND_ATTACK:
	case CS_GRAPPED_ATTACK:
	case CS_CROUCH_ATTACK:
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
	}
}

void ioDruidObjectItem::OnCharging( ioBaseChar *pOwner )
{
	if( !pOwner->IsNeedProcess() )
		return;

	if( pOwner->IsAttackKeyDown() )
	{
		if( m_dwAttackStartTime + m_dwChargeTime <= FRAMEGETTIME() )
		{
			ChangeToExtendAttackFire( pOwner );
		}
	}
	else
	{
		ChangeToAttackFire( pOwner );
	}
}

void ioDruidObjectItem::ChangeToAttackFire( ioBaseChar *pOwner )
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

		m_szCurAni		= pAttribute->m_AttackAnimation;
		m_fCurAniRate	= pAttribute->m_fAttackAniRate;

		m_dwInputCancelCheckTime = dwCurTime;
		m_dwCancelCheckTime = dwCurTime;
	}

	pOwner->ApplyNormalAttackResult( m_iCurCombo );
	m_ChargeState = CS_ATTACK_FIRE;

	if( pOwner->IsNeedSendNetwork() )
	{
		SP2Packet kPacket( CUPK_ITEM_GAUGE_OK );
		FillItemGaugePacketHeader( pOwner, kPacket );
		kPacket << static_cast<byte>( SS_ATTACK_FIRE );
		kPacket << pOwner->GetTargetRot();
		kPacket << m_iCurCombo;
		P2PNetwork::SendToAllPlayingUser( kPacket );
	}
}

void ioDruidObjectItem::ChangeToExtendAttackFire( ioBaseChar *pOwner )
{
	if( !pOwner )
		return;

	ioEntityGroup *pGrp = pOwner->GetGroup();
	if( !pGrp )
		return;

	DWORD dwCurTime = FRAMEGETTIME();
	pOwner->InitExtendAttackTagInfo();
	ClearCancelInfo();
	m_ChargeState = CS_EXTEND_ATTACK;

	ioDruidItem* pDruid =ToDruidItem( pOwner->GetWeapon() );
	if( pDruid )
		pDruid->SetUseExtraGauge( pDruid->GetNeedExtendBullet() );

	pOwner->SetNormalAttackByAttribute( m_ExtendAttack );
	m_szCurAni					= m_ExtendAttack.m_AttackAnimation;
	m_fCurAniRate				= m_ExtendAttack.m_fAttackAniRate;
	m_dwInputCancelCheckTime	= dwCurTime;
	m_dwCancelCheckTime			= dwCurTime;
	pOwner->ApplyNormalAttackResult( m_iCurCombo );

	float fTimeRate		= m_ExtendAttack.m_fAttackAniRate;
	DWORD dwPreDelay	= m_ExtendAttack.m_dwPreDelay;
	int iAniID			= pGrp->GetAnimationIdx( m_ExtendAttack.m_AttackAnimation );

	pOwner->SetBaseAttackExtraAniJump( iAniID, fTimeRate, dwPreDelay );
	pOwner->SetExtendAttackEndJumpInfo( iAniID, fTimeRate, dwPreDelay );

	if( pOwner->IsNeedSendNetwork() )
	{
		SP2Packet kPacket( CUPK_ITEM_GAUGE_OK );
		FillItemGaugePacketHeader( pOwner, kPacket );
		kPacket << static_cast<byte>( SS_EXTEND_ATTACK );
		kPacket << pOwner->GetTargetRot();
		P2PNetwork::SendToAllPlayingUser( kPacket );
	}
}

void ioDruidObjectItem::FindGrappling( ioBaseChar *pOwner )
{
	if( !pOwner )
		return;

	ioPlayStage *pStage = pOwner->GetCreator();
	if( !pStage )
		return;

	m_GrapplingTargetName.Clear();
	m_GrappedTargetName.Clear();
	FD_GrapplingAutoTarget kAutoPred( pOwner, m_fGrapplingMinAngle, m_fGrapplingMaxAngle, m_fGrapplingRange );
	ioBaseChar *pTarget = pStage->FindUser( kAutoPred );
	if( pTarget )
	{
		m_GrapplingTargetName = pTarget->GetCharName();
	}
}

bool ioDruidObjectItem::IsEnableGrapplingState( ioBaseChar *pOwner, const ioHashString &szTarget )
{
	if( m_GrapplingTargetName.IsEmpty() )
		return false;

	if( m_GrapplingTargetName == szTarget )
		return true;

	return false;
}

void ioDruidObjectItem::SetGrapplingState( ioBaseChar *pOwner, const ioHashString &szTarget )
{
	if( !pOwner )
		return;

	ioEntityGroup *pGrp = pOwner->GetGroup();
	if( !pGrp )
		return;

	DWORD dwCurTime = FRAMEGETTIME();
	pGrp->ClearAllActionAni( FLOAT100, true );
	pGrp->ClearAllLoopAni( FLOAT100, true );
	
	int iAniID = pGrp->GetAnimationIdx( m_GrapplingBiteMotion );
	pGrp->SetActionAni( iAniID, FLOAT100, FLOAT100, FLOAT1, FLOAT1/m_GrapplingBiteMotionRate );

	m_GrapplingState		= GS_PULL;
	m_GrapplingBiteEndTime	= dwCurTime + pGrp->GetAnimationFullTime( iAniID ) * m_GrapplingBiteMotionRate;

	m_GrapplingTargetName.Clear();
	m_GrappedTargetName = szTarget;

	SetAniRotate( pOwner, iAniID, m_GrapplingBiteMotionRate );

	pOwner->SetState( CS_GRAPPLING );
	m_AttackKeyType = AKT_PRESS;

	if( pOwner->IsNeedSendNetwork() )
	{
		SP2Packet kPacket( CUPK_GRAPPLING_SYNC );
		kPacket << pOwner->GetCharName();
		kPacket << GRAPPLING_ATTACKER;
		kPacket << GS_PULL;
		kPacket << szTarget;
		kPacket << GetItemCode();
		P2PNetwork::SendToAllPlayingUser( kPacket );
	}
}

void ioDruidObjectItem::OnGrapplingBite( ioBaseChar *pOwner )
{
	DWORD dwCurTime = FRAMEGETTIME();

	if( m_GrapplingBiteEndTime < dwCurTime )
	{
		ChangeGrapplingLoop( pOwner );
	}
}

void ioDruidObjectItem::ChangeGrapplingLoop( ioBaseChar *pOwner )
{
	if( !pOwner )
		return;

	ioEntityGroup *pGrp = pOwner->GetGroup();
	if( !pGrp )
 return;

	DWORD dwCurTime = FRAMEGETTIME();

	pGrp->ClearAllActionAni( FLOAT10, true );
	pGrp->ClearAllLoopAni( FLOAT10, true );

	int iAniID = pGrp->GetAnimationIdx( m_GrapplingLoopMotion );
	pGrp->SetLoopAni( iAniID, FLOAT10 );

	m_GrapplingState			= GS_MOVE;
	m_dwGrapplingLoopEndTime	= dwCurTime + m_TargetGrapplingInfo.m_dwGrapplingMaxTime;

	D3DXQUATERNION qtCurRot = pOwner->GetTargetRot();
	if( pOwner->IsNeedSendNetwork() )
	{
		SP2Packet kPacket( CUPK_GRAPPLING_SYNC );
		kPacket << pOwner->GetCharName();
		kPacket << GRAPPLING_ATTACKER;
		kPacket << GS_MOVE;
		kPacket << pOwner->GetTargetRot();
		P2PNetwork::SendToAllPlayingUser( kPacket );
	}

	ioBaseChar *pTarget = pOwner->GetBaseChar( m_GrappedTargetName );
	if( pTarget )
	{	
		if( !P2PNetwork::IsNetworkPlaying() || !pOwner->GetCreator()->IsNetworkMode() || ToNpcChar( pTarget ) )
		{
			if( pTarget->GetState() == CS_GRAPPLING_WOUNDED )
			{
				pTarget->ChangeWoundGrapplingMoveState();
			}
		}
	}
}

void ioDruidObjectItem::OnGrapplingLoop( ioBaseChar *pOwner )
{
	if( !pOwner )
		return;

	DWORD dwCurTime = FRAMEGETTIME();
	if( pOwner->IsNeedProcess() && pOwner->IsAttackKeyDown() && m_AttackKeyType == AKT_NONE )
	{
		ChangeGrapplingPush( pOwner );
	}
	else if( m_dwGrapplingLoopEndTime < dwCurTime )
	{
		ChangeGrapplingCancel( pOwner );
	}
	else
	{
		CheckGrapplingMoveKey( pOwner );
		CheckGrapplingMoveDir( pOwner );
	}
}

void ioDruidObjectItem::CheckGrapplingMoveKey( ioBaseChar *pOwner )
{
	if( !pOwner )
		return;

	if( !pOwner->IsNeedProcess() )
		return;

	ioUserKeyInput::DirKeyInput eNewDirKey = pOwner->CheckCurDirKey();
	if( eNewDirKey != m_CurrDirectionKey )
	{
		m_bTargetRot		= false;
		m_CurrDirectionKey	= eNewDirKey;

		if( pOwner->IsNeedSendNetwork() )
		{
			SP2Packet kPacket( CUPK_ITEM_GAUGE_CANCEL );
			FillItemGaugePacketHeader( pOwner, kPacket );
			kPacket << pOwner->GetTargetRot();
			kPacket << (int)m_CurrDirectionKey;
			P2PNetwork::SendToAllPlayingUser( kPacket );
		}
	}
}

void ioDruidObjectItem::CheckGrapplingMoveDir( ioBaseChar *pOwner )
{
	if( m_dwGrapplingRotateSpeed == 0 )
		return;

	if( m_CurrDirectionKey == ioUserKeyInput::DKI_NONE )
		return;

	if( m_bTargetRot )
		return;

	float fFrameGap = g_FrameTimer.GetCurFrameGap();
	float fRate = fFrameGap / m_dwGrapplingRotateSpeed;
	float fRotateAngle = 360.0f * fRate;

	D3DXQUATERNION qtCurRot = pOwner->GetTargetRot();
	D3DXVECTOR3 vCurMoveDir = qtCurRot * D3DXVECTOR3(0.0f, 0.0f, -FLOAT1);
	D3DXVec3Normalize( &vCurMoveDir, &vCurMoveDir );

	// 목표에 도달체크 및 처리
	float fYawD = 180.0f + 45.0f * ( m_CurrDirectionKey - ioUserKeyInput::DKI_UP );
	fYawD = ioMath::ArrangeHead( fYawD );

	float fYawR = DEGtoRAD( fYawD );
	D3DXQUATERNION qtRotate;
	D3DXQuaternionRotationYawPitchRoll( &qtRotate, fYawR, 0.0f, 0.0f );

	D3DXVECTOR3 vTargetDir = qtRotate * D3DXVECTOR3(0.0f, 0.0f, -FLOAT1);
	D3DXVec3Normalize( &vTargetDir, &vTargetDir );

	float fDotValue = D3DXVec3Dot( &vCurMoveDir, &vTargetDir );
	float fGapAngle = RADtoDEG( acosf(fDotValue) );

	if( fDotValue >= FLOAT1 )
	{
		pOwner->SetTargetRotToRotate( qtRotate, true );
		return;
	}

	D3DXVECTOR3 vAxis;
	if( fDotValue <= -FLOAT1 )
	{
		vAxis = ioMath::UNIT_Y;
	}
	else
	{
		D3DXVec3Cross( &vAxis, &vTargetDir, &vCurMoveDir );
		D3DXVec3Normalize( &vAxis, &-vAxis );
	}

	D3DXQUATERNION qtNewRot;
	if( fGapAngle < fRotateAngle )
		D3DXQuaternionRotationAxis( &qtNewRot, &vAxis, DEGtoRAD(fGapAngle) );
	else
		D3DXQuaternionRotationAxis( &qtNewRot, &vAxis, DEGtoRAD(fRotateAngle) );

	D3DXVECTOR3 vNewDir = qtNewRot * vCurMoveDir;
	D3DXVec3Normalize( &vNewDir, &vNewDir );

	ioMath::CalcDirectionQuaternion( qtNewRot, -vNewDir );
	pOwner->SetTargetRotToRotate( qtNewRot, true );
}


void ioDruidObjectItem::ChangeGrapplingPush( ioBaseChar *pOwner )
{
	if( !pOwner )
		return;

	ioEntityGroup *pGrp = pOwner->GetGroup();
	if( !pGrp )
		return;

	DWORD dwCurTime = FRAMEGETTIME();

	pGrp->ClearAllActionAni( FLOAT100, true );
	pGrp->ClearAllLoopAni( FLOAT100, true );

	int iAniID = pGrp->GetAnimationIdx( m_GrapplingPushMotion );
	pGrp->SetActionAni( iAniID, FLOAT100, FLOAT100, FLOAT1, FLOAT1 / m_fGrapplingPushMotionRate );

	m_GrapplingState			= GS_PUSH;
	m_dwGrapplingPushEndTime	= dwCurTime + pGrp->GetAnimationFullTime( iAniID ) * m_fGrapplingPushMotionRate;
	m_GrapplingTargetName.Clear();

	if( pOwner->IsNeedSendNetwork() )
	{
		SP2Packet kPacket( CUPK_GRAPPLING_SYNC );
		kPacket << pOwner->GetCharName();
		kPacket << GRAPPLING_ATTACKER;
		kPacket << GS_PUSH;
		kPacket << pOwner->GetTargetRot();
		P2PNetwork::SendToAllPlayingUser( kPacket );
	}

	ioBaseChar *pTarget = pOwner->GetBaseChar( m_GrappedTargetName );
	if( pTarget )
	{	
		if( !P2PNetwork::IsNetworkPlaying() || !pOwner->GetCreator()->IsNetworkMode() || ToNpcChar( pTarget ) )
		{
			ioBaseChar *pTarget = pOwner->GetBaseChar( m_GrappedTargetName );
			if( pTarget->GetState() == CS_GRAPPLING_WOUNDED )
			{
				D3DXVECTOR3 vDir = pOwner->GetWorldPosition() - pTarget->GetWorldPosition();
				D3DXVec3Normalize( &vDir, &vDir );
				D3DXQUATERNION qtRot;
				ioMath::CalcDirectionQuaternion( qtRot, -vDir );
				pTarget->SetTargetRot( qtRot );
				pTarget->SetGrapplingPushedState( GetPushedGrapplingInfo() );
			}
		}
	}
}

void ioDruidObjectItem::OnGrapplingPush( ioBaseChar *pOwner )
{
	DWORD dwCurTime = FRAMEGETTIME();
	if( m_dwGrapplingPushEndTime < dwCurTime )
	{
		ChangeToGrappedAttackFire( pOwner );
	}
}

void ioDruidObjectItem::ChangeGrapplingCancel( ioBaseChar *pOwner )
{
	if( !pOwner )
		return;

	ioEntityGroup *pGrp = pOwner->GetGroup();
	if( !pGrp )
		return;

	if( m_GrapplingState == GS_CANCEL )
		return;

	DWORD dwCurTime = FRAMEGETTIME();
	pGrp->ClearAllActionAni( FLOAT100, true );
	pGrp->ClearAllLoopAni( FLOAT100, true );

	int iAniID = pGrp->GetAnimationIdx( m_GrapplingCancelMotion );
	pGrp->SetActionAni( iAniID, FLOAT100, FLOAT100, FLOAT1, FLOAT1 / m_fGrapplingCancelMotionRate );

	m_GrapplingState			= GS_CANCEL;
	m_dwGrapplingCancelEndTime	= dwCurTime + pGrp->GetAnimationFullTime( iAniID ) * m_fGrapplingCancelMotionRate;

	m_GrapplingTargetName.Clear();

	if( pOwner->IsNeedSendNetwork() )
	{
		SP2Packet kPacket( CUPK_GRAPPLING_SYNC );
		kPacket << pOwner->GetCharName();
		kPacket << GRAPPLING_ATTACKER;
		kPacket << GS_CANCEL;
		kPacket << pOwner->GetTargetRot();
		P2PNetwork::SendToAllPlayingUser( kPacket );
	}
}

bool ioDruidObjectItem::CheckGrapplingCancel( ioBaseChar *pOwner )
{
	if( m_GrapplingState != GS_CANCEL )
	{
		ioBaseChar *pTarget = pOwner->GetBaseChar( m_GrappedTargetName );
		if( pTarget )
		{
			if( m_GrapplingState == GS_MOVE )
			{
				if( pTarget->GetState() != CS_GRAPPLING_WOUNDED || pTarget->GetWoundGrapplingState() == GS_CANCEL )
				{
					ChangeGrapplingCancel( pOwner );
					return true;
				}
			}
		}
		else
		{
			ChangeGrapplingCancel( pOwner );
			return true;
		}
	}

	return false;
}

void ioDruidObjectItem::OnGrapplingCancel( ioBaseChar *pOwner )
{
	DWORD dwCurTime = FRAMEGETTIME();
	if( m_dwGrapplingCancelEndTime < dwCurTime )
	{
		pOwner->SetState( CS_DELAY );
		return;
	}
}

void ioDruidObjectItem::CheckGrapplingState( ioBaseChar *pOwner )
{
	if( !pOwner )
		return;

	if( CheckGrapplingCancel( pOwner ) )
		return;

	//m_AttackKeyType 타입 사용에 대한 부분은 헤더 참조
	if( !pOwner->IsAttackKeyDown() )
		m_AttackKeyType = AKT_NONE;

	switch( m_GrapplingState )
	{
	case GS_PULL:
		OnGrapplingBite( pOwner );
		break;
	case GS_MOVE:
		OnGrapplingLoop( pOwner );
		break;
	case GS_PUSH:
		OnGrapplingPush( pOwner );
		break;
	case GS_CANCEL:
		OnGrapplingCancel( pOwner );
		break;
	}
}

void ioDruidObjectItem::SetEndGrappling( ioBaseChar *pOwner )
{
	Init();
}

void ioDruidObjectItem::GetGrapplingDirection( OUT int &iCurDir )
{
	iCurDir = m_CurrDirectionKey;
}

const GrapplingWoundedInfo& ioDruidObjectItem::GetTargetGrapplingInfo() const
{
	return m_TargetGrapplingInfo; 
}

const GrapplingPushedInfo& ioDruidObjectItem::GetPushedGrapplingInfo() const 
{
	return m_PushedGrapplingInfo;
}

float ioDruidObjectItem::GetGrapplingGapRange() const
{
	return m_fGrapplingGapRange;
}

void ioDruidObjectItem::ChangeToGrappedAttackFire( ioBaseChar *pOwner )
{
	if( !pOwner )
		return;

	ioEntityGroup *pGrp = pOwner->GetGroup();
	if( !pGrp )
		return;

	m_GrapplingTargetName.Clear();
	m_GrappedTargetName.Clear();

	DWORD dwCurTime = FRAMEGETTIME();
	pOwner->InitExtendAttackTagInfo();
	ClearCancelInfo();
	pOwner->SetState( CS_ATTACK );
	pOwner->SetChargingState( true );
	pOwner->SetAttackMoveEnable( false );

	m_ChargeState = CS_GRAPPED_ATTACK;

	pOwner->SetNormalAttackByAttribute( m_GrappedAttack );
	m_szCurAni					= m_GrappedAttack.m_AttackAnimation;
	m_fCurAniRate				= m_GrappedAttack.m_fAttackAniRate;
	m_dwInputCancelCheckTime	= dwCurTime;
	m_dwCancelCheckTime			= dwCurTime;
	pOwner->ApplyNormalAttackResult( m_iCurCombo );

	float fTimeRate		= m_GrappedAttack.m_fAttackAniRate;
	DWORD dwPreDelay	= m_GrappedAttack.m_dwPreDelay;
	int iAniID			= pGrp->GetAnimationIdx( m_GrappedAttack.m_AttackAnimation );

	pOwner->SetBaseAttackExtraAniJump( iAniID, fTimeRate, dwPreDelay );
	pOwner->SetExtendAttackEndJumpInfo( iAniID, fTimeRate, dwPreDelay );

	if( pOwner->IsNeedSendNetwork() )
	{
		SP2Packet kPacket( CUPK_ITEM_GAUGE_OK );
		FillItemGaugePacketHeader( pOwner, kPacket );
		kPacket << static_cast<byte>( SS_GRAPPED_ATTACK );
		kPacket << pOwner->GetTargetRot();
		P2PNetwork::SendToAllPlayingUser( kPacket );
	}
}

void ioDruidObjectItem::ChangeToCrouchGathering( ioBaseChar *pOwner )
{
	pOwner->StopAutoTargetTracking();
	pOwner->InitExtendAttackTagInfo();
	ClearCancelInfo();
	pOwner->SetState( CS_DRUID_CROUCH_GATHERING_STATE );
	pOwner->SetAttackMoveEnable( true );

	ioEntityGroup *pGrp = pOwner->GetGroup();
	pGrp->ClearAllActionAni( FLOAT100, true );
	pGrp->ClearAllLoopAni( FLOAT100, true );
	pGrp->SetLoopAni( m_CrouchGatherAnimation, FLOAT100 );
	EndEffect( pOwner );
	
	ioEffect* pEffect = pOwner->AttachEffect( m_CrouchGatherEffect );
	if( pEffect )
	{
		m_CrouchGatherEffectID = pEffect->GetUniqueID();
	}

	m_ChargeState			= CS_CROUCH_GATHERING;
	m_CurrDirectionKey		= ioUserKeyInput::DKI_NONE;
	m_FireStartDirKey		= pOwner->GetCurDirKey();

	m_bFirstFireMoveMotion	= true;
	m_bSetChargingMoveAni	= false;

	if( pOwner->IsNeedSendNetwork() )
	{
		SP2Packet kPacket( CUPK_ITEM_GAUGE_OK );
		FillItemGaugePacketHeader( pOwner, kPacket );
		kPacket << static_cast<byte>( SS_CROUCH_GATHERING );
		kPacket << pOwner->GetTargetRot();
		P2PNetwork::SendToAllPlayingUser( kPacket );
	}
}

void ioDruidObjectItem::OnCrouchGathering( ioBaseChar *pOwner )
{
	if( !pOwner || !pOwner->IsNeedProcess() )
		return;

	if( pOwner->CheckCurDirKey() != ioUserKeyInput::DKI_NONE && m_CurrDirectionKey != pOwner->CheckCurDirKey() )
	{
		pOwner->SetTargetRotToDirKey( pOwner->CheckCurDirKey() );
		m_CurrDirectionKey = pOwner->CheckCurDirKey();

		if( pOwner->IsNeedSendNetwork() )
		{
			SP2Packet kPacket( CUPK_ITEM_GAUGE_CANCEL );
			FillItemGaugePacketHeader( pOwner, kPacket );
			kPacket << pOwner->GetTargetRot();
			kPacket << (int)m_CurrDirectionKey;
			P2PNetwork::SendToAllPlayingUser( kPacket );
		}
	}

	if( !pOwner->IsAttackKeyDown() )
	{
		pOwner->SetNormalAttack( m_iCurCombo );
	}
	else if( pOwner->IsJumpKeyDown() )
	{
		ChangeToCrouchGatherEndByJump( pOwner );
	}
	else if( pOwner->IsDefenseKeyDown() )
	{
		ChangeToCrouchGatherEndBySkill( pOwner );
	}
}

void ioDruidObjectItem::OnCrouchChargingMove( ioBaseChar *pOwner )
{
	if( !pOwner->IsCanAttackMove() )
		return;

	ioEntityGroup *pGrp = pOwner->GetGroup();
	if( !pGrp )
		return;

	int iAniID		= 0;
	DWORD dwCurTime	= FRAMEGETTIME();
	if( pOwner->IsSettedDirection() )
	{
		if( pOwner->IsChangeDirection() || m_bFirstFireMoveMotion )
		{
			pGrp->ClearAllLoopAni( FLOAT10 );
			m_bSetChargingMoveAni	= true;
			m_bFirstFireMoveMotion	= false;

			ioUserKeyInput::DirKeyInput eXformDir;
			eXformDir = pOwner->GetXformDir( m_FireStartDirKey );

			if( m_bChargeCharRotate )
			{
				pGrp->SetLoopAni( m_CrouchGatherMoveFront, FLOAT100, FLOAT1, m_fCrouchGatherRate );
			}
			else
			{
				switch( eXformDir )
				{
				case ioUserKeyInput::DKI_UP:
					pGrp->SetLoopAni( m_CrouchGatherMoveFront, FLOAT100, FLOAT1, m_fCrouchGatherRate );
					break;
				case ioUserKeyInput::DKI_RIGHTUP:
				case ioUserKeyInput::DKI_RIGHT:
				case ioUserKeyInput::DKI_RIGHTDOWN:
					pGrp->SetLoopAni( m_CrouchGatherMoveRight, FLOAT100, FLOAT1, m_fCrouchGatherRate );
					break;
				case ioUserKeyInput::DKI_DOWN:
					pGrp->SetLoopAni( m_CrouchGatherMoveBack, FLOAT100, FLOAT1, m_fCrouchGatherRate );
					break;
				case ioUserKeyInput::DKI_LEFTDOWN:
				case ioUserKeyInput::DKI_LEFT:
				case ioUserKeyInput::DKI_LEFTUP:
					pGrp->SetLoopAni( m_CrouchGatherMoveLeft, FLOAT100, FLOAT1, m_fCrouchGatherRate );
					break;
				}
			}
		}
	}
	else
	{
		if( m_bSetChargingMoveAni )
		{
			iAniID = pGrp->GetAnimationIdx( m_CrouchGatherAnimation );
			if( iAniID == -1 )
				return;

			pGrp->ClearAllActionAni( FLOAT100, true );
			pGrp->ClearAllLoopAni( FLOAT100, true );
			pGrp->SetLoopAni( iAniID, FLOAT100, FLOAT1, FLOAT1/m_fCrouchGatherRate );
			m_bSetChargingMoveAni = false;
		}
	}
}

void ioDruidObjectItem::ChangeToCrouchAttackFire( ioBaseChar *pOwner )
{
	if( !pOwner )
		return;

	ioEntityGroup *pGrp = pOwner->GetGroup();
	if( !pGrp )
		return;

	DWORD dwCurTime = FRAMEGETTIME();
	pOwner->InitExtendAttackTagInfo();
	ClearCancelInfo();
	pOwner->SetState( CS_ATTACK );
	pOwner->SetChargingState( false );
	pOwner->SetAttackMoveEnable( false );
	EndEffect( pOwner );

	m_ChargeState = CS_CROUCH_ATTACK;

	pOwner->SetNormalAttackByAttribute( m_CrouchAttack );
	m_szCurAni					= m_CrouchAttack.m_AttackAnimation;
	m_fCurAniRate				= m_CrouchAttack.m_fAttackAniRate;
	m_dwInputCancelCheckTime	= dwCurTime;
	m_dwCancelCheckTime			= dwCurTime;
	pOwner->ApplyNormalAttackResult( m_iCurCombo );

	float fTimeRate		= m_CrouchAttack.m_fAttackAniRate;
	DWORD dwPreDelay	= m_CrouchAttack.m_dwPreDelay;
	int iAniID			= pGrp->GetAnimationIdx( m_CrouchAttack.m_AttackAnimation );

	pOwner->SetBaseAttackExtraAniJump( iAniID, fTimeRate, dwPreDelay );
	pOwner->SetExtendAttackEndJumpInfo( iAniID, fTimeRate, dwPreDelay );
}

void ioDruidObjectItem::ChangeToCrouchGatherEndByJump( ioBaseChar *pOwner )
{
	SetNoneState( pOwner );
	pOwner->SetJumpState();

	if( pOwner->IsNeedSendNetwork() )
	{
		SP2Packet kPacket( CUPK_ITEM_GAUGE_OK );
		FillItemGaugePacketHeader( pOwner, kPacket );
		kPacket << static_cast<byte>( SS_CROUCH_GATHERING_END_BY_JUMP );
		kPacket << pOwner->GetTargetRot();
		P2PNetwork::SendToAllPlayingUser( kPacket );
	}
}

void ioDruidObjectItem::ChangeToCrouchGatherEndBySkill( ioBaseChar *pOwner )
{
	int iSkillNum = pOwner->GetSkillKeyInput();
	if( iSkillNum == 0 && !pOwner->IsBuffLimitSkill() && pOwner->SetUseSkill( 0, SUT_NORMAL ) )
	{
		SetNoneState( pOwner );
	}

	if( pOwner->IsNeedSendNetwork() )
	{
		SP2Packet kPacket( CUPK_ITEM_GAUGE_OK );
		FillItemGaugePacketHeader( pOwner, kPacket );
		kPacket << static_cast<byte>( SS_CROUCH_GATHERING_END_BY_SKILL );
		kPacket << pOwner->GetTargetRot();
		P2PNetwork::SendToAllPlayingUser( kPacket );
	}
}

void ioDruidObjectItem::ChangeToHuman( ioBaseChar *pOwner )
{
	m_bReserveRelease = true;
}

void ioDruidObjectItem::ChangeToEagle( ioBaseChar *pOwner )
{
	m_bReserveRelease = true;
}

bool ioDruidObjectItem::CheckReleaseProcess( ioPlayStage *pStage, ioBaseChar *pOwner )
{
	bool bRet = ioObjectItem::CheckReleaseProcess( pStage, pOwner );
	if( bRet )
		return true;

	return CheckReleaseBear( pStage, pOwner );
}

bool ioDruidObjectItem::CheckReleaseByNormalAttack( ioPlayStage *pStage, ioBaseChar *pOwner )
{
	if( pOwner->GetState() == CS_ATTACK || pOwner->GetState() == CS_GRAPPLING )
	{
		switch( m_ChargeState )
		{
		case CS_EXTEND_ATTACK:
		case CS_GRAPPED_ATTACK:
			return false;
		}
	}

	return IsReleaseItem( pOwner );
}

bool ioDruidObjectItem::CheckReleaseBear( ioPlayStage *pStage, ioBaseChar *pOwner )
{
	if( !pOwner )
		return false;
	
	ioWeaponItem *pItem = pOwner->GetWeapon();
	ioDruidItem* pDruid = ToDruidItem( pItem );

	//자신을 생성한 무기가 없는 상태에서는 무조건 릴리즈 되어야 함
	if( !pDruid )
		return true;

	if( IsReleaseItem( pOwner ) )
	{
		if( pOwner->GetState() == CS_OBJECT_EQUIP )
			return false;

		switch( m_ChargeState )
		{
		case CS_EXTEND_ATTACK:
		case CS_GRAPPED_ATTACK:
			return false;
		}

		return true;
	}

	return false;
}

bool ioDruidObjectItem::IsEnableDash() const
{
	return true;
}

bool ioDruidObjectItem::IsEnableDefense() const
{
	return true;
}

bool ioDruidObjectItem::IsChargeAttackState( ioBaseChar *pOwner )
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

bool ioDruidObjectItem::IsEnableAttackDashCheck( ioBaseChar *pOwner, bool bCheck )
{
	if( !IsChargeDash() )
		return false;

	if( !bCheck )
		return false;

	if( pOwner->GetState() == CS_ATTACK )
	{
		switch( m_ChargeState )
		{
		case CS_ATTACK_FIRE:
		case CS_EXTEND_ATTACK:
		case CS_GRAPPED_ATTACK:
			return true;
		}
	}

	return false;
}

bool ioDruidObjectItem::IsCanFire( const ioBaseChar* pOwner, int iFireCnt, bool bNormalAttack ) const
{
	if( IsReleaseItem( pOwner ) )
		return false;

	return true;
}

void ioDruidObjectItem::SetExtraInfoToWeapon( ioWeapon *pWeapon, bool bJumpAttack )
{
	ioObjectItem::SetExtraInfoToWeapon( pWeapon, bJumpAttack );
	if( pWeapon && pWeapon->CheckGrapplingState() == GPT_DRUID && !m_GrapplingTargetName.IsEmpty() )
	{
		pWeapon->SetGrapplingTargetName( m_GrapplingTargetName );
	}

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

void ioDruidObjectItem::OnItemGaugeOK( ioBaseChar *pOwner, SP2Packet &rkPacket )
{
	if( !pOwner )
		return;
	
	byte eType;
	rkPacket >> eType;

	D3DXQUATERNION qtRot;
	rkPacket >> qtRot;
	pOwner->SetTargetRot( qtRot );

	switch( eType )
	{
	case SS_ATTACK_FIRE:
		{
			rkPacket >> m_iCurCombo;
			ChangeToAttackFire( pOwner );
		}
		break;
	case SS_EXTEND_ATTACK:
		{
			ChangeToExtendAttackFire( pOwner );
		}
		break;
	case SS_GRAPPED_ATTACK:
		{
			ChangeToGrappedAttackFire( pOwner );
		}
		break;
	case SS_CROUCH_GATHERING:
		{
			ChangeToCrouchGathering( pOwner );
		}
		break;
	case SS_CROUCH_GATHERING_END_BY_JUMP:
		{
			ChangeToCrouchGatherEndByJump( pOwner );
		}
		break;
	case SS_CROUCH_GATHERING_END_BY_SKILL:
		{
			ChangeToCrouchGatherEndBySkill( pOwner );
		}
		break;
	}
}

void ioDruidObjectItem::OnItemGaugeCancel( ioBaseChar *pOwner, SP2Packet &rkPacket )
{
	D3DXQUATERNION qtRot;
	rkPacket >> qtRot;

	m_bTargetRot = false;
	pOwner->SetTargetRot( qtRot );

	int eDir;
	rkPacket >> eDir;
	m_CurrDirectionKey = static_cast<DIRKEY>( eDir );
}

//-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

ioDruidCrouchGatheringState::ioDruidCrouchGatheringState()
{
}

ioDruidCrouchGatheringState::~ioDruidCrouchGatheringState()
{
}

void ioDruidCrouchGatheringState::Initialize( ioBaseChar* pOwner, CharState eCurrState )
{
	ioSpecialStateBase::Initialize( pOwner, eCurrState );
}

void ioDruidCrouchGatheringState::Finalize( ioBaseChar* pOwner, CharState eCurrState, CharState eNewState )
{
	ioSpecialStateBase::Finalize( pOwner, eCurrState, eNewState );
}

void ioDruidCrouchGatheringState::CheckSpecialState( ioBaseChar* pOwner )
{
	ioDruidObjectItem *pDruid = ToDruidObjectItem( pOwner->GetPriorityItem( SPT_ATTACK ) );
	if( pDruid )
	{
		pDruid->OnCrouchGathering( pOwner );
		pDruid->OnCrouchChargingMove( pOwner );
	}
	else
	{
		pOwner->SetState( CS_DELAY );
		pOwner->SetFallState( true );
		return;
	}
}

void ioDruidCrouchGatheringState::ProcessKeyInput( ioBaseChar* pOwner )
{	
	pOwner->ChangeDirectionByInputDir( false );
}