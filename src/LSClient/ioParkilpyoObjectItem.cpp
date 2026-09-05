
#include "stdafx.h"

#include "ioBaseChar.h"

#include "ioParkilpyoObjectItem.h"
#include "ioParkilpyoItem.h"

#include "WeaponDefine.h"

ioParkilpyoObjectItem::ioParkilpyoObjectItem()
{
	Init();
}

ioParkilpyoObjectItem::ioParkilpyoObjectItem( const ioParkilpyoObjectItem &rhs )
	: ioObjectItem( rhs ),
      m_AttackReadyAniList( rhs.m_AttackReadyAniList ),
	  m_dwProtectTime( rhs.m_dwProtectTime ),
	  m_fChargeUseGauge( rhs.m_fChargeUseGauge ),
	  m_ChargeAttack( rhs.m_ChargeAttack ),
	  m_fEquipJumpPower( rhs.m_fEquipJumpPower ),
	  m_EquipMaleMeshList( rhs.m_EquipMaleMeshList ),
	  m_EquipFemaleMeshList( rhs.m_EquipFemaleMeshList ),
	  m_szEquipEffect( rhs.m_szEquipEffect )
{
	Init();
}

ioParkilpyoObjectItem::~ioParkilpyoObjectItem()
{
}

void ioParkilpyoObjectItem::LoadProperty( ioINILoader &rkLoader )
{
	ioObjectItem::LoadProperty( rkLoader );
	
	m_dwProtectTime = (DWORD)rkLoader.LoadInt_e( "protect_time", 0 );

	char szBuf[MAX_PATH];
	char szKey[MAX_PATH];
	m_AttackReadyAniList.clear();

	int nMaxCombo = GetMaxCombo();
	for( int i=0; i<nMaxCombo; ++i )
	{
		wsprintf_e( szBuf, "attack_ready_animation%d", i+1 );
		rkLoader.LoadString( szBuf, "", szBuf, MAX_PATH );
		m_AttackReadyAniList.push_back( ioHashString(szBuf) );
	}

	m_fChargeUseGauge = rkLoader.LoadFloat_e( "use_charge_gauge", 0.0f );
	m_fEquipJumpPower = rkLoader.LoadFloat_e( "jump_state_equip_jump_power", 0.0f );

	LoadAttackAttribute( "charge_attack", m_ChargeAttack, rkLoader );
	
	rkLoader.LoadString_e( "equip_effect", "", szBuf, MAX_PATH );
	m_szEquipEffect = szBuf;
		
	int nMeshCnt = rkLoader.LoadInt_e( "equip_male_mesh_cnt", 0 );
	for( int i=0; i<nMeshCnt; ++i )
	{
		wsprintf_e( szKey, "equip_male_mesh%d", i+1 );
		rkLoader.LoadString( szKey, "", szBuf, MAX_PATH );
		m_EquipMaleMeshList.push_back( szBuf );
	}

	nMeshCnt = rkLoader.LoadInt_e( "equip_female_mesh_cnt", 0 );
	for( int i=0; i<nMeshCnt; ++i )
	{
		wsprintf_e( szKey, "equip_female_mesh%d", i+1 );
		rkLoader.LoadString( szKey, "", szBuf, MAX_PATH );
		m_EquipFemaleMeshList.push_back( szBuf );
	}
}

ioItem* ioParkilpyoObjectItem::Clone()
{
	return new ioParkilpyoObjectItem( *this );
}

ioObjectItem::ObjectSubType ioParkilpyoObjectItem::GetObjectSubType() const
{
	return OST_PARKILPYO;
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

const char* ioParkilpyoObjectItem::GetAttackStateText() const
{
	switch( m_WeaponState )
	{
	case WS_NONE:
		return "WS_NONE";
	case WS_CHECK_CHARGING:
		return "WS_CHECK_CHARGING";
	case WS_NORMAL_ATTACK:
		return "WS_NORMAL_ATTACK";
	case WS_CHARGE_ATTACK:
		return "WS_CHARGE_ATTACK";
	case WS_END:
		return "WS_END";
	}

	return "";
}

////////////////////////////////////////////////////////////////////////////////////////////////////////

void ioParkilpyoObjectItem::SetNormalAttack( ioBaseChar *pOwner, int iCurCombo )
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

		m_WeaponState = WS_NONE;
	}
	else
	{
		//공격 준비 애니메이션 설정
		ioEntityGroup *pGrp = pOwner->GetGroup();
		pGrp->ClearAllLoopAni( FLOAT100, true );
		pGrp->ClearAllActionAni( FLOAT100, true );

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

		m_WeaponState = WS_CHECK_CHARGING;
	}
}

void ioParkilpyoObjectItem::CheckNormalAttack( ioBaseChar *pOwner )
{
	DWORD dwCurTime = FRAMEGETTIME();

	switch( m_WeaponState )
	{
	case WS_CHECK_CHARGING:
		CheckCharging( pOwner );
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
	case WS_CHARGE_ATTACK:
		{
			if( pOwner->IsNeedProcess() )
			{
				CheckCancelReserve( pOwner );

				if( ProcessCancel(pOwner) )
					return;
			}

			if( m_dwMotionEndTime < dwCurTime )
			{
				pOwner->SetState( CS_DELAY, false );
				return;
			}	
		}
		break;
	}
}

bool ioParkilpyoObjectItem::SetJumpAttack( ioBaseChar *pOwner, int iAniID, float fTimeRate, AttackType eType )
{
	if( !pOwner ) 
		return false;

	ClearAirJump();

	AttackType eExtendType = AT_JUMP_ATTACK;

	const AttackAttribute *pAttr = GetAttackAttribute( eExtendType );
	if( !pAttr )
	{
		LOG.PrintTimeAndLog( 0, "ioObjectAttachItem::SetJumpAttack - AttackAttribute Not Exist" );
		return false;
	}

	ioEntityGroup *pGrp = pOwner->GetGroup();
	iAniID = pGrp->GetAnimationIdx( pAttr->m_AttackAnimation );
	if( iAniID == -1 )
	{
		LOG.PrintTimeAndLog( 0, "ioObjectAttachItem::SetJumpAttack - JumpAttack Animation Not Exist" );
		return false;
	}

	fTimeRate = pAttr->m_fAttackAniRate;

	SetAirJump( pOwner, iAniID, fTimeRate );

	return false;
}

void ioParkilpyoObjectItem::ProcessJumpAttack( ioBaseChar *pOwner, float fHeightGap )
{
	if( m_pExtendJump )
		m_pExtendJump->ProcessJumpAttack( pOwner, fHeightGap );

	CheckAirJump( pOwner );
}

////////////////////////////////////////////////////////////////////////////////////////////////////////

void ioParkilpyoObjectItem::OnItemGaugeOK( ioBaseChar *pOwner, SP2Packet &rkPacket )
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
	case WS_CHARGE_ATTACK:
		{
			D3DXQUATERNION qtRot;
			rkPacket >> qtRot;

			pOwner->SetTargetRotToRotate( qtRot, true );
			ChangeToChargeAttack( pOwner );
		}
	}
}

void ioParkilpyoObjectItem::OnEquiped( ioBaseChar *pOwner )
{
	if ( !pOwner )
		return;

	ioObjectItem::OnEquiped( pOwner );

	Init();

	//무적 시간 초기화
	DWORD dwCurTime = FRAMEGETTIME();
	m_dwCheckProtectTime = dwCurTime;
	
	// 오브젝트 아이템 장착시 메쉬 추가 및 실제 캐릭터 안보이게 처리.
	// 외형 메쉬를 변경하는 버프가 걸릴 경우 버프 종료 후 보이기 처리가 되어있다면 실제 캐릭터 메쉬가 보일 문제가 있음.
	// 더미 버프를 통해 외형 메쉬 변경 버프가 걸리지 않도록 Sucide 추가
	EquipChangeMesh( pOwner );
}

void ioParkilpyoObjectItem::OnReleased( ioBaseChar *pOwner )
{
	ioObjectItem::OnReleased( pOwner );

	// 오브젝트 아이템 해제시 메쉬 삭제 및 실제 캐릭터 보이게 처리.
	ReleaseChangeMesh( pOwner );
}

////////////////////////////////////////////////////////////////////////////////////////////////////////

void ioParkilpyoObjectItem::SetExtraInfoToWeapon( ioWeapon *pWeapon, bool bJumpAttack )
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

////////////////////////////////////////////////////////////////////////////////////////////////////////

bool ioParkilpyoObjectItem::IsProtectState()
{
	if( m_dwProtectTime > 0 && m_dwCheckProtectTime + m_dwProtectTime > FRAMEGETTIME() )
	{
		return true;
	}

	return false;
}

bool ioParkilpyoObjectItem::IsOneTimeWeaponItem() const
{
	return false;
}

bool ioParkilpyoObjectItem::IsEnableDash() const
{
	return true;
}

bool ioParkilpyoObjectItem::IsEnableDefense() const
{
	return true;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////

void ioParkilpyoObjectItem::CheckCharging( ioBaseChar *pOwner )
{
	if( !pOwner || !pOwner->IsNeedProcess() )	
		return;

	if( pOwner->IsAttackKeyDown() )
	{
		if( m_dwAttackStartTime + m_dwChargeTime <= FRAMEGETTIME() )
			ChangeToChargeAttack( pOwner );
	}
	else
		SetNormalAttackState( pOwner );
}

void ioParkilpyoObjectItem::SetNormalAttackState( ioBaseChar *pOwner )
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

		SetCurAttackAniForInputJump( pAttribute->m_AttackAnimation, pAttribute->m_fAttackAniRate );
	}

	pOwner->ApplyNormalAttackResult( m_nNormalCombo );
	m_WeaponState = WS_NORMAL_ATTACK;

	if( pOwner->IsNeedSendNetwork() )
	{
		SP2Packet kPacket( CUPK_ITEM_GAUGE_OK );
		FillItemGaugePacketHeader( pOwner, kPacket );
		kPacket << m_WeaponState;
		kPacket << m_nNormalCombo;
		kPacket << pOwner->GetTargetRot();
		P2PNetwork::SendToAllPlayingUser( kPacket );
	}
}

void ioParkilpyoObjectItem::ChangeToChargeAttack( ioBaseChar *pOwner )
{
	if( !pOwner ) 
		return;

	ioEntityGroup *pGrp = pOwner->GetGroup();
	int iAniID = pGrp->GetAnimationIdx( m_ChargeAttack.m_AttackAnimation );
	if ( iAniID == -1 )
		return;

	float fTimeRate  = m_ChargeAttack.m_fAttackAniRate;
	DWORD dwPreDelay = m_ChargeAttack.m_dwPreDelay;

	pGrp->ClearAllLoopAni( FLOAT100, true );
	pGrp->ClearAllActionAni( FLOAT100, true );
	pGrp->SetActionAni( iAniID, FLOAT100, FLOAT100, FLOAT1, FLOAT1/fTimeRate, 0.0f, dwPreDelay );	

	pOwner->ClearReservedSliding();
	pOwner->SetReservedSliding( m_ChargeAttack.m_vForceInfoList, iAniID, fTimeRate, dwPreDelay );

	pOwner->RefreshFireTimeList( iAniID, m_ChargeAttack.m_vWeaponInfoList, FTT_NORMAL_ATTACK, fTimeRate, dwPreDelay );

	DWORD dwCurTime = FRAMEGETTIME();
	m_dwMotionEndTime = dwCurTime + pGrp->GetAnimationFullTime( iAniID ) * fTimeRate;

	DWORD dwTrackingTime = dwCurTime + dwPreDelay;
	dwTrackingTime += pGrp->GetAnimationEventTime_e( iAniID, "tracking_e" ) * fTimeRate;
	pOwner->SetAutoTargetTrackingEndTime( dwTrackingTime );

	m_WeaponState = WS_CHARGE_ATTACK;

	DecreaseGauge( pOwner, m_fChargeUseGauge );

	SetCurAttackAniForInputJump( m_ChargeAttack.m_AttackAnimation, fTimeRate );

	if( pOwner->IsNeedSendNetwork() )
	{
		SP2Packet kPacket( CUPK_ITEM_GAUGE_OK );
		FillItemGaugePacketHeader( pOwner, kPacket );
		kPacket << m_WeaponState;
		kPacket << pOwner->GetTargetRot();
		P2PNetwork::SendToAllPlayingUser( kPacket );
	}
}

void ioParkilpyoObjectItem::DecreaseGauge( ioBaseChar *pOwner, float fUseGauge )
{
	if ( !pOwner )
		return;

	ioParkilpyoItem *pParkilpyoItem = ToParkilpyoItem( pOwner->GetWeapon() );
	if ( pParkilpyoItem )
		pParkilpyoItem->SetUseExtraGauge( fUseGauge );
}

////////////////////////////////////////////////////////////////////////////////////////////////////////

void ioParkilpyoObjectItem::Init()
{
	m_WeaponState = WS_NONE;

	m_iCurAirJump = 0;
	m_fAirJumpPowerRate = 0.0f;
		
	m_dwCheckProtectTime = 0;
	m_dwAttackStartTime = 0;
	m_dwMotionEndTime = 0;
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void ioParkilpyoObjectItem::EquipChangeMesh( ioBaseChar *pOwner )
{
	if( !pOwner ) 
		return;
	
	pOwner->SetInvisibleState( true, true, true );

	if( pOwner->IsMale() )
	{
		int nMeshCnt = m_EquipMaleMeshList.size();
		for( int i=0; i<nMeshCnt; ++i )
			pOwner->AddEntity( m_EquipMaleMeshList[i] );
	}
	else
	{
		int nMeshCnt = m_EquipFemaleMeshList.size();
		for( int i=0; i<nMeshCnt; ++i )
			pOwner->AddEntity( m_EquipFemaleMeshList[i] );
	}

	if( !m_szEquipEffect.IsEmpty() )
		pOwner->AttachEffect( m_szEquipEffect );
}

void ioParkilpyoObjectItem::ReleaseChangeMesh( ioBaseChar *pOwner )
{
	if( !pOwner ) 
		return;
	
	if( pOwner->IsMale() )
	{
		int nMeshCnt = m_EquipMaleMeshList.size();
		for( int i=0; i<nMeshCnt; ++i )
			pOwner->DestroyEntity( m_EquipMaleMeshList[i] );
	}
	else
	{
		int nMeshCnt = m_EquipFemaleMeshList.size();
		for( int i=0; i<nMeshCnt; ++i )
			pOwner->DestroyEntity( m_EquipFemaleMeshList[i] );
	}

	if( !m_szEquipEffect.IsEmpty() )
		pOwner->EndEffect( m_szEquipEffect, false );

	pOwner->SetInvisibleState( false, true );

	if( pOwner->IsInvisibleFace() )
		pOwner->ShowFaceMesh( false );
}