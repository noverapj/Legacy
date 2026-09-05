
#include "stdafx.h"

#include "ioOniMushaObjectItem.h"

#include "ioOniMushaItem.h"

#include "ioPlayStage.h"
#include "ioBaseChar.h"

#include "WeaponDefine.h"

ioOniMushaObjectItem::ioOniMushaObjectItem()
{
	Init();
}

ioOniMushaObjectItem::ioOniMushaObjectItem( const ioOniMushaObjectItem &rhs )
	: ioObjectItem( rhs ),
      m_AttackReadyAniList( rhs.m_AttackReadyAniList ),
	  m_dwProtectTime( rhs.m_dwProtectTime ),
	  m_EquipMaleMeshList( rhs.m_EquipMaleMeshList ),
	  m_EquipFemaleMeshList( rhs.m_EquipFemaleMeshList ),
	  m_szEquipEffect( rhs.m_szEquipEffect ),
	  m_fRunSpeed( rhs.m_fRunSpeed ),
	  m_dwRunRotateTime( rhs.m_dwRunRotateTime ),
	  m_RunWaitAni( rhs.m_RunWaitAni ),
	  m_fRunWaitAniRate( rhs.m_fRunWaitAniRate ),
	  m_RunLoopAni( rhs.m_RunLoopAni ),
	  m_fRunLoopAniRate( rhs.m_fRunLoopAniRate ),
	  m_dwRunLoopTime( rhs.m_dwRunLoopTime ),
	  m_RunExtraAni( rhs.m_RunExtraAni ),
	  m_fRunExtraAniRate( rhs.m_fRunExtraAniRate ),
	  m_fRunExtraEndJumpPower( rhs.m_fRunExtraEndJumpPower ),
	  m_fRunExtraAttackEndJumpPower( rhs.m_fRunExtraAttackEndJumpPower ),
	  m_RunAttack( rhs.m_RunAttack ),
	  m_RunEndAttack( rhs.m_RunEndAttack ),
	  m_RunExtraAttack( rhs.m_RunExtraAttack ),
	  m_RunBuffList( rhs.m_RunBuffList ),
	  m_bOnlyTeamCharColSkip( rhs.m_bOnlyTeamCharColSkip ),
	  m_fChargeUseGauge( rhs.m_fChargeUseGauge ),
	  m_fRunExtraJumpAngle( rhs.m_fRunExtraJumpAngle ),
	  m_fRunExtraJumpSpeed( rhs.m_fRunExtraJumpSpeed ),
	  m_fRunExtraJumpMaxRange( rhs.m_fRunExtraJumpMaxRange )
{
	Init();
}

ioOniMushaObjectItem::~ioOniMushaObjectItem()
{
}

void ioOniMushaObjectItem::LoadProperty( ioINILoader &rkLoader )
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

	m_fRunSpeed = rkLoader.LoadFloat_e( "charge_run_speed", 0.0f );
	m_dwRunRotateTime = (DWORD)rkLoader.LoadInt_e( "charge_run_rotate_speed", 0 );
	m_dwRunLoopTime = (DWORD)rkLoader.LoadInt_e( "charge_run_duration", 0 );

	rkLoader.LoadString_e( "charge_run_wait_ani", "", szBuf, MAX_PATH );
	m_RunWaitAni = szBuf;
	m_fRunWaitAniRate = rkLoader.LoadFloat_e( "charge_run_wait_ani_rate", FLOAT1 );

	rkLoader.LoadString_e( "charge_run_loop_ani", "", szBuf, MAX_PATH );
	m_RunLoopAni = szBuf;
	m_fRunLoopAniRate = rkLoader.LoadFloat_e( "charge_run_loop_ani_rate", FLOAT1 );
	
	rkLoader.LoadString_e( "charge_run_extra_ani", "", szBuf, MAX_PATH );
	m_RunExtraAni = szBuf;
	m_fRunExtraAniRate = rkLoader.LoadFloat_e( "charge_run_extra_ani_rate", FLOAT1 );

	m_fRunExtraEndJumpPower = rkLoader.LoadFloat_e( "charge_run_extra_end_jump_power", 0.0f );
	m_fRunExtraAttackEndJumpPower = rkLoader.LoadFloat_e( "charge_run_extra_attack_end_jump_power", 0.0f );

	LoadAttackAttribute( "charge_run_attack", m_RunAttack, rkLoader );
	LoadAttackAttribute( "charge_run_end_attack", m_RunEndAttack, rkLoader );
	LoadAttackAttribute( "charge_run_extra_attack", m_RunExtraAttack, rkLoader );
	LoadRunBuff( rkLoader, "charge_run", m_RunBuffList );

	m_bOnlyTeamCharColSkip = rkLoader.LoadBool_e( "only_team_char_col_skip", false );

	m_fChargeUseGauge = rkLoader.LoadFloat_e( "use_charge_gauge", 0.0f );

	m_fRunExtraJumpAngle = rkLoader.LoadFloat_e( "charge_run_extra_jump_angle", 0.0f );
	m_fRunExtraJumpSpeed = rkLoader.LoadFloat_e( "charge_run_extra_jump_speed", 0.0f );
	m_fRunExtraJumpMaxRange = rkLoader.LoadFloat_e( "charge_run_extra_jump_max_range", 0.0f );
}

ioItem* ioOniMushaObjectItem::Clone()
{
	return new ioOniMushaObjectItem( *this );
}

ioObjectItem::ObjectSubType ioOniMushaObjectItem::GetObjectSubType() const
{
	return OST_ONIMUSHA;
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

const char* ioOniMushaObjectItem::GetAttackStateText() const
{
	switch( m_WeaponState )
	{
	case WS_NONE:
		return "WS_NONE";
	case WS_CHECK_CHARGING:
		return "WS_CHECK_CHARGING";
	case WS_NORMAL_ATTACK:
		return "WS_NORMAL_ATTACK";
	case WS_RUN:
		return "WS_RUN";
	case WS_END:
		return "WS_END";
	}

	return "";
}

////////////////////////////////////////////////////////////////////////////////////////////////////////

void ioOniMushaObjectItem::SetNormalAttack( ioBaseChar *pOwner, int iCurCombo )
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

void ioOniMushaObjectItem::CheckNormalAttack( ioBaseChar *pOwner )
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
	case WS_RUN:
		OnRunState( pOwner );
		break;
	}
}

bool ioOniMushaObjectItem::SetJumpAttack( ioBaseChar *pOwner, int iAniID, float fTimeRate, AttackType eType )
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

void ioOniMushaObjectItem::ProcessJumpAttack( ioBaseChar *pOwner, float fHeightGap )
{
	if( m_pExtendJump )
		m_pExtendJump->ProcessJumpAttack( pOwner, fHeightGap );

	CheckAirJump( pOwner );
}

////////////////////////////////////////////////////////////////////////////////////////////////////////

void ioOniMushaObjectItem::OnItemGaugeOK( ioBaseChar *pOwner, SP2Packet &rkPacket )
{
	if( !pOwner ) 
		return;

	byte eState;
	rkPacket >> eState;

	switch( eState )
	{
	case ST_ATTACK:
		{
			rkPacket >> m_nNormalCombo;

			D3DXQUATERNION qtRot;
			rkPacket >> qtRot;

			pOwner->SetTargetRotToRotate( qtRot, true );
			SetNormalAttackState( pOwner );
		}
		break;
	case ST_RUN_START:
		{
			D3DXVECTOR3 vPos;
			rkPacket >> vPos;

			pOwner->SetWorldPosition( vPos );

			D3DXQUATERNION qtRot;
			rkPacket >> qtRot;

			pOwner->SetTargetRotAndMoveDirChange( qtRot );

			ChangeToRunState( pOwner );
		}
		break;
	case ST_RUN_END:
		{
			D3DXVECTOR3 vPos;
			rkPacket >> vPos;

			pOwner->SetWorldPosition( vPos );

			D3DXQUATERNION qtRot;
			rkPacket >> qtRot;

			pOwner->SetTargetRotAndMoveDirChange( qtRot );

			pOwner->SetState( CS_DELAY );
		}
		break;
	case ST_INPUT_JUMP:
		{
			D3DXVECTOR3 vPos;
			rkPacket >> vPos;

			pOwner->SetWorldPosition( vPos );

			D3DXQUATERNION qtRot;
			rkPacket >> qtRot;

			pOwner->SetTargetRotAndMoveDirChange( qtRot );

			Init();

			RemoveRunBuff( pOwner );
			pOwner->SetCurMoveSpeed( 0.0f );
			pOwner->SetForcePowerAmt( 0.0f );
			pOwner->SetCurJumpPowerAmt( 0.0f );
			pOwner->SetGravityAmt( 0.0f );
		}
		break;
	case ST_INPUT_ATTACK:
		{
			D3DXVECTOR3 vPos;
			rkPacket >> vPos;

			pOwner->SetWorldPosition( vPos );

			D3DXQUATERNION qtRot;
			rkPacket >> qtRot;

			pOwner->SetTargetRotAndMoveDirChange( qtRot );

			RemoveRunBuff( pOwner );
			pOwner->SetCurMoveSpeed( 0.0f );
			pOwner->SetForcePowerAmt( 0.0f );
			pOwner->SetCurJumpPowerAmt( 0.0f );
			pOwner->SetGravityAmt( 0.0f );

			ChangeToRunAttack( pOwner );
		}
		break;
	case ST_ROTATE:
		{
			int nDir;
			rkPacket >> nDir;
			rkPacket >> m_bLeftRot;

			m_CurDirKey = (ioUserKeyInput::DirKeyInput)nDir;
			m_bTargetRot = false;

			D3DXVECTOR3 vNewPos;
			rkPacket >> vNewPos;

			D3DXQUATERNION qtNewRot;
			rkPacket >> qtNewRot;

			pOwner->SetWorldPosition( vNewPos );
			pOwner->SetTargetRotAndMoveDirChange( qtNewRot );
		}
		break;
	case ST_RUN_EXTRA:
		{
			D3DXVECTOR3 vPos;
			rkPacket >> vPos;

			pOwner->SetWorldPosition( vPos );

			D3DXQUATERNION qtRot;
			rkPacket >> qtRot;

			pOwner->SetTargetRotAndMoveDirChange( qtRot );

			ChangeToRunExtra( pOwner );
		}
		break;
	case ST_RUN_EXTRA_ATTACK:
		{
			D3DXVECTOR3 vPos;
			rkPacket >> vPos;

			pOwner->SetWorldPosition( vPos );

			int nCurKey;
			rkPacket >> nCurKey;

			if ( (ioUserKeyInput::DirKeyInput)nCurKey != ioUserKeyInput::DKI_NONE )
				pOwner->SetTargetRotToDirKey( (ioUserKeyInput::DirKeyInput)nCurKey );
			
			ChangeToRunExtraAttack( pOwner );
		}
		break;
	}
}

void ioOniMushaObjectItem::OnEquiped( ioBaseChar *pOwner )
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

void ioOniMushaObjectItem::OnReleased( ioBaseChar *pOwner )
{
	ioObjectItem::OnReleased( pOwner );

	// 오브젝트 아이템 해제시 메쉬 삭제 및 실제 캐릭터 보이게 처리.
	ReleaseChangeMesh( pOwner );
}

////////////////////////////////////////////////////////////////////////////////////////////////////////

void ioOniMushaObjectItem::SetExtraInfoToWeapon( ioWeapon *pWeapon, bool bJumpAttack )
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

void ioOniMushaObjectItem::OnEndExtendAttack( ioBaseChar *pOwner, bool bReleaseItem )
{
	RemoveRunBuff( pOwner );
}

////////////////////////////////////////////////////////////////////////////////////////////////////////

bool ioOniMushaObjectItem::IsProtectState()
{
	if( m_dwProtectTime > 0 && m_dwCheckProtectTime + m_dwProtectTime > FRAMEGETTIME() )
	{
		return true;
	}

	return false;
}

bool ioOniMushaObjectItem::IsOneTimeWeaponItem() const
{
	return false;
}

bool ioOniMushaObjectItem::IsEnableDash() const
{
	return true;
}

bool ioOniMushaObjectItem::IsEnableDefense() const
{
	return true;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////

void ioOniMushaObjectItem::CheckCharging( ioBaseChar *pOwner )
{
	if( !pOwner || !pOwner->IsNeedProcess() )	
		return;

	if( pOwner->IsAttackKeyDown() && !pOwner->IsHasCrown() && !IsReleaseItem( pOwner ) )
	{
		if( m_dwAttackStartTime + m_dwChargeTime <= FRAMEGETTIME() )
			ChangeToRunState( pOwner );
	}
	else
		SetNormalAttackState( pOwner );
}

void ioOniMushaObjectItem::SetNormalAttackState( ioBaseChar *pOwner )
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
		kPacket << static_cast<byte>( ST_ATTACK );
		kPacket << m_nNormalCombo;
		kPacket << pOwner->GetTargetRot();
		P2PNetwork::SendToAllPlayingUser( kPacket );
	}
}

void ioOniMushaObjectItem::DecreaseGauge( ioBaseChar *pOwner, float fUseGauge )
{
	if ( !pOwner )
		return;

	ioOniMushaItem *pOniMushaItem = ToOniMushaItem( pOwner->GetWeapon() );
	if ( pOniMushaItem )
		pOniMushaItem->SetUseExtraGauge( fUseGauge );
}

////////////////////////////////////////////////////////////////////////////////////////////////////////

void ioOniMushaObjectItem::Init()
{
	m_iCurAirJump = 0;
	m_fAirJumpPowerRate = 0.0f;

	m_WeaponState = WS_NONE;
	m_RunState = RS_NONE;
		
	m_dwCheckProtectTime = 0;
	m_dwAttackStartTime = 0;
	m_dwMotionEndTime = 0;

	m_dwRunLoopEndTime = 0;

	m_bTargetRot = false;
	m_bLeftRot = false;
	m_CurDirKey = ioUserKeyInput::DKI_NONE;

	m_bAttackKeyReserved = false;
	m_bDefenseKeyReserved = false;
	m_bJumpKeyReserved = false;

	m_fCurRunExtraJumpRange = 0.0f;
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void ioOniMushaObjectItem::EquipChangeMesh( ioBaseChar *pOwner )
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

void ioOniMushaObjectItem::ReleaseChangeMesh( ioBaseChar *pOwner )
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

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

bool ioOniMushaObjectItem::IsMoveAttackState( ioBaseChar *pOwner )
{
	if( pOwner->GetState() == CS_ATTACK )
	{
		switch( m_WeaponState )
		{
		case WS_RUN:
			{
				if( m_RunState == RS_RUN )
					return true;
			}
			break;
		}
	}

	return false;
}

void ioOniMushaObjectItem::ChangeToRunState( ioBaseChar *pOwner )
{
	if( !pOwner ) 
		return;

	ioEntityGroup *pGrp = pOwner->GetGroup();
	if( !pGrp )
		return;

	int nAniID = pGrp->GetAnimationIdx( m_RunWaitAni );
	if ( nAniID == -1 )
		return;

	pGrp->ClearAllActionAni( FLOAT100 );
	pGrp->ClearAllLoopAni(  FLOAT100 );

	pGrp->SetActionAni( nAniID, FLOAT100, FLOAT100, FLOAT1, FLOAT1/m_fRunWaitAniRate );

	DWORD dwCurTime = FRAMEGETTIME();
	m_dwMotionEndTime = dwCurTime + pGrp->GetAnimationFullTime( nAniID ) * m_fRunWaitAniRate;

	pOwner->SetAttackMoveEnable( false );

	m_WeaponState = WS_RUN;
	m_RunState = RS_WAIT;
	m_CurDirKey = ioUserKeyInput::DKI_NONE;

	AddRunBuff( pOwner );

	DecreaseGauge( pOwner, m_fChargeUseGauge );

	pOwner->ClearAttackFireTimeAndSkill();
	pOwner->ClearReservedSliding();
	pOwner->SetForcePowerAmt( 0.0f );
	pOwner->SetCurJumpPowerAmt( 0.0f );
	pOwner->SetGravityAmt( 0.0f );

	if( pOwner->IsNeedSendNetwork() )
	{
		SP2Packet kPacket( CUPK_ITEM_GAUGE_OK );
		FillItemGaugePacketHeader( pOwner, kPacket );
		kPacket << static_cast<byte>( ST_RUN_START );
		kPacket << pOwner->GetWorldPosition();
		kPacket << pOwner->GetWorldOrientation();
		P2PNetwork::SendToAllPlayingUser( kPacket );
	}
}

void ioOniMushaObjectItem::ChangeToRunning( ioBaseChar *pOwner )
{
	if( !pOwner ) 
		return;

	ioEntityGroup *pGrp = pOwner->GetGroup();
	if( !pGrp )
		return;

	int nAniID = pGrp->GetAnimationIdx( m_RunLoopAni );
	if ( nAniID == -1 )
		return;

	pGrp->ClearAllActionAni( FLOAT100 );
	pGrp->ClearAllLoopAni( FLOAT100 );

	pGrp->SetLoopAni( nAniID, FLOAT100, FLOAT1, FLOAT1 / m_fRunLoopAniRate );

	DWORD dwCurTime = FRAMEGETTIME();
	m_dwRunLoopEndTime = dwCurTime + m_dwRunLoopTime;

	pOwner->SetTargetRotToRotate( pOwner->GetTargetRot(), true, false );
	pOwner->SetCharCollisionSkipTime( dwCurTime, m_dwRunLoopEndTime, m_bOnlyTeamCharColSkip );

	m_RunState = RS_RUN;
}

void ioOniMushaObjectItem::ChangeToRunAttack( ioBaseChar *pOwner )
{
	if( !pOwner ) 
		return;

	ioEntityGroup *pGrp = pOwner->GetGroup();
	if ( !pGrp )
		return;

	int nAniID = pGrp->GetAnimationIdx( m_RunAttack.m_AttackAnimation );
	if ( nAniID == -1 )
		return;

	float fTimeRate  = m_RunAttack.m_fAttackAniRate;
	DWORD dwPreDelay = m_RunAttack.m_dwPreDelay;

	pGrp->ClearAllLoopAni( FLOAT100, true );
	pGrp->ClearAllActionAni( FLOAT100, true );
	pGrp->SetActionAni( nAniID, FLOAT100, FLOAT100, FLOAT1, FLOAT1/fTimeRate, 0.0f, dwPreDelay );	

	pOwner->ClearReservedSliding();
	pOwner->SetReservedSliding( m_RunAttack.m_vForceInfoList, nAniID, fTimeRate, dwPreDelay );

	pOwner->RefreshFireTimeList( nAniID, m_RunAttack.m_vWeaponInfoList, FTT_NORMAL_ATTACK, fTimeRate, dwPreDelay );
	SetCurAttackAniForInputJump( m_RunAttack.m_AttackAnimation, fTimeRate );

	DWORD dwCurTime = FRAMEGETTIME();
	m_dwMotionEndTime = dwCurTime + pGrp->GetAnimationFullTime( nAniID ) * fTimeRate;

	pOwner->SetAutoTarget( ATT_NORMAL );

	DWORD dwTrackingTime = dwCurTime + dwPreDelay + pGrp->GetAnimationEventTime_e( nAniID, "tracking_e" ) * fTimeRate;
	pOwner->SetAutoTargetTrackingEndTime( dwTrackingTime );

	m_RunState = RS_RUN_ATTACK;
}

void ioOniMushaObjectItem::ChangeToRunEnd( ioBaseChar *pOwner )
{
	if( !pOwner ) 
		return;

	ioEntityGroup *pGrp = pOwner->GetGroup();
	if( !pGrp )
		return;

	int nAniID = pGrp->GetAnimationIdx( m_RunEndAttack.m_AttackAnimation );
	if ( nAniID == -1 )
		return;

	float fTimeRate  = m_RunEndAttack.m_fAttackAniRate;
	DWORD dwPreDelay = m_RunEndAttack.m_dwPreDelay;

	pGrp->ClearAllLoopAni( FLOAT100, true );
	pGrp->ClearAllActionAni( FLOAT100, true );
	pGrp->SetActionAni( nAniID, FLOAT100, FLOAT100, FLOAT1, FLOAT1/fTimeRate, 0.0f, dwPreDelay );

	pOwner->RefreshFireTimeList( nAniID, m_RunEndAttack.m_vWeaponInfoList, FTT_NORMAL_ATTACK, fTimeRate, dwPreDelay );
	SetCurAttackAniForInputJump( m_RunEndAttack.m_AttackAnimation, fTimeRate );

	DWORD dwCurTime = FRAMEGETTIME();
	m_dwMotionEndTime = dwCurTime + pGrp->GetAnimationFullTime( nAniID ) * fTimeRate;
	m_dwKeyReserveTime = dwCurTime + pGrp->GetAnimationEventTime_e( nAniID, "key_reserve" ) * fTimeRate;

	pOwner->SetAutoTarget( ATT_NORMAL );

	DWORD dwTrackingTime = dwCurTime + dwPreDelay + pGrp->GetAnimationEventTime_e( nAniID, "tracking_e" ) * fTimeRate;
	pOwner->SetAutoTargetTrackingEndTime( dwTrackingTime );

	pOwner->ClearReservedSliding();
	pOwner->SetCurMoveSpeed( 0.0f );
	pOwner->SetForcePowerAmt( 0.0f );
	pOwner->SetCurJumpPowerAmt( 0.0f );
	pOwner->SetGravityAmt( 0.0f );

	pOwner->SetCharCollisionSkipTime( 0, 0 );
	pOwner->SetChargingState( false );

	m_RunState = RS_END;

	RemoveRunBuff( pOwner );
}

void ioOniMushaObjectItem::OnRunState( ioBaseChar *pOwner )
{
	switch( m_RunState )
	{
	case RS_WAIT:
		OnRunWait( pOwner );
		break;
	case RS_RUN:
		OnRunning( pOwner );
		break;
	case RS_RUN_ATTACK:
		OnRunAttack( pOwner );
		break;
	case RS_END:
		OnRunEnd( pOwner );
		break;
	}
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void ioOniMushaObjectItem::OnRunWait( ioBaseChar *pOwner )
{
	if( !pOwner ) 
		return;

	DWORD dwCurTime = FRAMEGETTIME();

	if( m_dwMotionEndTime < dwCurTime )
		ChangeToRunning( pOwner );
}

void ioOniMushaObjectItem::OnRunning( ioBaseChar *pOwner )
{
	if( !pOwner ) 
		return;

	DWORD dwCurTime = FRAMEGETTIME();

	if( m_dwRunLoopEndTime > 0 && m_dwRunLoopEndTime < dwCurTime )
	{
		m_dwRunLoopEndTime = 0;
		ChangeToRunEnd( pOwner );
	}
	else
	{
		ioPlayStage *pStage = pOwner->GetCreator();
		if ( !pStage )
			return;

		if( pOwner->IsNeedProcess() )
			pStage->SetTargetLookAt( true );

		pOwner->SetCurMoveSpeed( m_fRunSpeed );

		CheckKeyInput( pOwner );
		ProcessRotate( pOwner );
	}
}

void ioOniMushaObjectItem::OnRunAttack( ioBaseChar *pOwner )
{
	if( !pOwner ) 
		return;

	DWORD dwCurTime = FRAMEGETTIME();

	if( pOwner->IsNeedProcess() )
	{
		CheckCancelReserve( pOwner );

		if( ProcessCancel(pOwner) )
			return;
	}

	if( m_dwMotionEndTime < dwCurTime )
		CheckReserveState( pOwner );
	else 
	{
		if( m_dwKeyReserveTime > 0 && m_dwKeyReserveTime < dwCurTime )
			ReserveInputKey( pOwner, true, true, false );
	}
}

void ioOniMushaObjectItem::OnRunEnd( ioBaseChar *pOwner )
{
	if( !pOwner ) 
		return;

	DWORD dwCurTime = FRAMEGETTIME();

	if( pOwner->IsNeedProcess() )
	{
		CheckCancelReserve( pOwner );

		if( ProcessCancel(pOwner) )
			return;
	}

	if( m_dwMotionEndTime < dwCurTime )
		CheckReserveState( pOwner );
	else 
	{
		if( m_dwKeyReserveTime > 0 && m_dwKeyReserveTime < dwCurTime )
			ReserveInputKey( pOwner, true, true, false );
	}
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void ioOniMushaObjectItem::CheckKeyInput( ioBaseChar *pOwner )
{
	if( !pOwner || !pOwner->IsNeedProcess() ) 
		return;

	if ( pOwner->IsJumpKey() )
	{
		Init();

		RemoveRunBuff( pOwner );
		pOwner->SetCurMoveSpeed( 0.0f );
		pOwner->SetForcePowerAmt( 0.0f );
		pOwner->SetCurJumpPowerAmt( 0.0f );
		pOwner->SetGravityAmt( 0.0f );

		if( pOwner->IsNeedSendNetwork() )
		{
			SP2Packet kPacket( CUPK_ITEM_GAUGE_OK );
			FillItemGaugePacketHeader( pOwner, kPacket );
			kPacket << static_cast<byte>( ST_INPUT_JUMP );
			kPacket << pOwner->GetWorldPosition();
			kPacket << pOwner->GetWorldOrientation();
			P2PNetwork::SendToAllPlayingUser( kPacket );
		}

		pOwner->SetJumpState();
		return;
	}
	else if ( pOwner->IsAttackKey() )
	{
		RemoveRunBuff( pOwner );
		pOwner->SetCurMoveSpeed( 0.0f );
		pOwner->SetForcePowerAmt( 0.0f );
		pOwner->SetCurJumpPowerAmt( 0.0f );
		pOwner->SetGravityAmt( 0.0f );

		if( pOwner->IsNeedSendNetwork() )
		{
			SP2Packet kPacket( CUPK_ITEM_GAUGE_OK );
			FillItemGaugePacketHeader( pOwner, kPacket );
			kPacket << static_cast<byte>( ST_INPUT_ATTACK );
			kPacket << pOwner->GetWorldPosition();
			kPacket << pOwner->GetWorldOrientation();
			P2PNetwork::SendToAllPlayingUser( kPacket );
		}

		ChangeToRunAttack( pOwner );
		return;
	}

	CheckDirKeyInput( pOwner );
}

void ioOniMushaObjectItem::CheckDirKeyInput( ioBaseChar *pOwner )
{
	if( !pOwner || !pOwner->IsNeedProcess() )	
		return;

	if( pOwner->IsSettedDirection() )
	{
		ioUserKeyInput::DirKeyInput eNewDirKey = pOwner->CheckCurDirKey();
		if( eNewDirKey != m_CurDirKey )
		{
			m_bTargetRot = false;
			m_bLeftRot = false;

			float fYawD = 180.0f + 45.0f * ( eNewDirKey - ioUserKeyInput::DKI_UP );
			fYawD = ioMath::ArrangeHead( fYawD );

			float fYawR = DEGtoRAD( fYawD );
			D3DXQUATERNION qtRotate;
			D3DXQuaternionRotationYawPitchRoll( &qtRotate, fYawR, 0.0f, 0.0f );

			D3DXVECTOR3 vDir = pOwner->GetMoveDir();
			D3DXVECTOR3 vTargetDir = qtRotate * D3DXVECTOR3( 0.0f, 0.0f, -FLOAT1 );

			if( D3DXVec3Dot( &vTargetDir, &vDir ) > -FLOAT1 )
			{
				D3DXQUATERNION qtCurRot = pOwner->GetTargetRot();
				float fCurYaw = RADtoDEG( ioMath::QuaterToYaw( qtCurRot ) );
				fCurYaw = ioMath::ArrangeHead( fCurYaw );

				float fTargetYaw = RADtoDEG( ioMath::QuaterToYaw( qtRotate ) );
				fTargetYaw = ioMath::ArrangeHead( fTargetYaw );

				float fRotAngle = ioMath::AngleGap( fCurYaw, fTargetYaw );
				if( fRotAngle < 0.0f )
					m_bLeftRot = true;
			}

			m_CurDirKey = eNewDirKey;

			if( pOwner->IsNeedSendNetwork() )
			{
				SP2Packet kPacket( CUPK_ITEM_GAUGE_OK );
				FillItemGaugePacketHeader( pOwner, kPacket );
				kPacket << static_cast<byte>( ST_ROTATE );
				kPacket << m_CurDirKey;
				kPacket << m_bLeftRot;
				kPacket << pOwner->GetWorldPosition();
				kPacket << pOwner->GetWorldOrientation();
				P2PNetwork::SendToAllPlayingUser( kPacket );
			}
		}
	}
	else
	{
		if( m_CurDirKey != ioUserKeyInput::DKI_NONE )
		{
			m_CurDirKey = ioUserKeyInput::DKI_NONE;

			if( pOwner->IsNeedSendNetwork() )
			{
				SP2Packet kPacket( CUPK_ITEM_GAUGE_OK );
				FillItemGaugePacketHeader( pOwner, kPacket );
				kPacket << static_cast<byte>( ST_ROTATE );
				kPacket << m_CurDirKey;
				kPacket << m_bLeftRot;
				kPacket << pOwner->GetWorldPosition();
				kPacket << pOwner->GetWorldOrientation();
				P2PNetwork::SendToAllPlayingUser( kPacket );
			}
		}
	}
}

void ioOniMushaObjectItem::ProcessRotate( ioBaseChar *pOwner )
{
	if( !pOwner ) 
		return;

	if( m_dwRunRotateTime <= 0 || m_bTargetRot || m_CurDirKey == ioUserKeyInput::DKI_NONE ) 
		return;

	float fRate = g_FrameTimer.GetCurFrameGap() / m_dwRunRotateTime;
	float fRotateAngle = 360.0f * fRate;

	float fYawD = 180.0f + 45.0f * ( m_CurDirKey - ioUserKeyInput::DKI_UP );
	fYawD = ioMath::ArrangeHead( fYawD );

	float fYawR = DEGtoRAD( fYawD );
	D3DXQUATERNION qtRotate;
	D3DXQuaternionRotationYawPitchRoll( &qtRotate, fYawR, 0.0f, 0.0f );

	D3DXQUATERNION qtCurRot = pOwner->GetTargetRot();

	float fCurYaw, fTargetYaw;
	fCurYaw = RADtoDEG( ioMath::QuaterToYaw( qtCurRot ) );
	fTargetYaw = RADtoDEG( ioMath::QuaterToYaw( qtRotate ) );

	fCurYaw = ioMath::ArrangeHead( fCurYaw );
	fTargetYaw = ioMath::ArrangeHead( fTargetYaw );

	float fRotAngle = ioMath::AngleGap( fCurYaw, fTargetYaw );
	if( fabs( fRotAngle ) < fRotateAngle )
	{
		m_bTargetRot = true;
		pOwner->SetTargetRotToRotate( qtRotate, true, false );
		return;
	}

	if( m_bLeftRot )
		fRotateAngle *= -1;

	fRotateAngle = ioMath::ArrangeHead( fRotateAngle );
	D3DXVECTOR3 vDir = pOwner->GetMoveDir();

	D3DXQUATERNION qtAngle;
	D3DXQuaternionRotationYawPitchRoll( &qtAngle, DEGtoRAD(fRotateAngle), 0.0f, 0.0f );

	vDir = qtAngle * vDir;
	ioMath::CalcDirectionQuaternion( qtAngle, -vDir );

	pOwner->SetTargetRotToRotate( qtAngle, true, false );
}

void ioOniMushaObjectItem::SetNextItemState( ioBaseChar *pWoundChar, ioBaseChar *pAttacker, ioWeapon *pWeapon, bool bDefense, float fDamage )
{
	if( pWoundChar->IsNeedProcess() && fDamage > 0.0f )
	{
		if ( pWoundChar && pAttacker )
		{		
			ChangeToRunExtra( pAttacker );

			if( pWoundChar->IsNeedSendNetwork() )
			{
				SP2Packet kPacket( CUPK_ITEM_GAUGE_OK );
				FillItemGaugePacketHeader( pAttacker, kPacket );
				kPacket << static_cast<byte>( ST_RUN_EXTRA );
				kPacket << pAttacker->GetWorldPosition();
				kPacket << pAttacker->GetWorldOrientation();
				P2PNetwork::SendToAllPlayingUser( kPacket );
			}
		}
	}
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void ioOniMushaObjectItem::ReserveInputKey(ioBaseChar *pOwner, bool bEnableAttack, bool bEnableDefence, bool bEnableJump )
{
	if ( !pOwner || !pOwner->IsNeedProcess() )
		return;

	if( bEnableAttack && pOwner->IsAttackKey() )
		m_bAttackKeyReserved = true;

	if( bEnableDefence && pOwner->IsDefenseKey() )
		m_bDefenseKeyReserved = true;

	if( bEnableJump && pOwner->IsJumpKey() )
		m_bJumpKeyReserved = true;
}

void ioOniMushaObjectItem::CheckReserveState( ioBaseChar *pOwner )
{
	if ( !pOwner || !pOwner->IsNeedProcess() )
		return;
	
	if( m_bAttackKeyReserved )
	{
		Init();
		SetReserveAttack( pOwner );
		return;
	}
	else if( m_bDefenseKeyReserved )
	{
		Init();
		SetReserveDefence( pOwner );
		return;
	}
	else if ( m_bJumpKeyReserved )
	{
		Init();
		SetReserveJump( pOwner );
		return;
	}

	Init();
	pOwner->SetState( CS_DELAY );

	if( pOwner->IsNeedSendNetwork() )
	{
		SP2Packet kPacket( CUPK_ITEM_GAUGE_OK );
		FillItemGaugePacketHeader( pOwner, kPacket );
		kPacket << static_cast<byte>( ST_RUN_END );
		kPacket << pOwner->GetWorldPosition();
		kPacket << pOwner->GetWorldOrientation();
		P2PNetwork::SendToAllPlayingUser( kPacket );
	}	
}

void ioOniMushaObjectItem::SetReserveAttack( ioBaseChar *pOwner )
{
	if ( !pOwner || !pOwner->IsNeedProcess() )
		return;

	pOwner->SetNormalAttack(0);

	m_bAttackKeyReserved = false;
}

void ioOniMushaObjectItem::SetReserveDefence( ioBaseChar *pOwner )
{
	if ( !pOwner || !pOwner->IsNeedProcess() )
		return;

	pOwner->SetDefenseState();
	m_bDefenseKeyReserved = false;
}

void ioOniMushaObjectItem::SetReserveJump( ioBaseChar *pOwner )
{
	if ( !pOwner || !pOwner->IsNeedProcess() )
		return;

	pOwner->SetJumpState();
	m_bJumpKeyReserved = false;	
}

bool ioOniMushaObjectItem::IsNoDropState( ioBaseChar *pOwner )
{
	if( m_WeaponState == WS_RUN )
		return true;

	return false;
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void ioOniMushaObjectItem::LoadRunBuff( ioINILoader &rkLoader, const char* szName, ioHashStringVec &BuffList )
{
	char szBuff[MAX_PATH], szKey[MAX_PATH];

	BuffList.clear();

	wsprintf_e( szKey, "%s_buff_cnt", szName );
	int nMaxCnt = rkLoader.LoadInt( szKey, 0 );
	for( int i=0; i<nMaxCnt; i++ )
	{
		wsprintf_e( szKey, "%s_buff%d", szName, i+1 );
		rkLoader.LoadString( szKey, "", szBuff, MAX_PATH );
		BuffList.push_back( szBuff );
	}
}

void ioOniMushaObjectItem::AddRunBuff( ioBaseChar *pOwner )
{
	if( !pOwner )	
		return;

	if( m_RunBuffList.empty() ) 
		return;

	int nMaxCnt = m_RunBuffList.size();
	for( int i=0; i<nMaxCnt; i++ )
		pOwner->AddNewBuff( m_RunBuffList[i], pOwner->GetCharName(), GetName(), NULL );
}

void ioOniMushaObjectItem::RemoveRunBuff( ioBaseChar *pOwner )
{
	if( !pOwner )	
		return;

	if( m_RunBuffList.empty() ) 
		return;

	int nMaxCnt = m_RunBuffList.size();
	for( int i=0; i<nMaxCnt; i++ )
	{
		ioBuff* pBuff = pOwner->GetBuff( m_RunBuffList[i] );
		if( pBuff )
			pBuff->SetReserveEndBuff();
	}
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void ioOniMushaObjectItem::ChangeToRunExtra( ioBaseChar *pOwner )
{
	if( !pOwner ) 
		return;

	ioEntityGroup *pGrp = pOwner->GetGroup();
	if( !pGrp )
		return;

	int nAniID = pGrp->GetAnimationIdx( m_RunExtraAni );
	if ( nAniID == -1 )
		return;

	pGrp->ClearAllActionAni( FLOAT100 );
	pGrp->ClearAllLoopAni( FLOAT100 );

	pGrp->SetActionAni( nAniID, FLOAT100, FLOAT100, FLOAT1, FLOAT1 / m_fRunExtraAniRate );

	DWORD dwCurTime = FRAMEGETTIME();
	m_dwMotionEndTime = dwCurTime + pGrp->GetAnimationFullTime( nAniID ) * m_fRunExtraAniRate;
	m_dwKeyReserveTime = dwCurTime + pGrp->GetAnimationEventTime_e( nAniID, "key_reserve" ) * m_fRunExtraAniRate;

	pOwner->ClearReservedSliding();
	pOwner->SetCurMoveSpeed( 0.0f );
	pOwner->SetForcePowerAmt( 0.0f );
	pOwner->SetCurJumpPowerAmt( 0.0f );
	pOwner->SetGravityAmt( 0.0f );

	pOwner->SetCharCollisionSkipTime( 0, 0 );
	pOwner->SetChargingState( false );

	m_RunExtraState = RES_Start;

	RemoveRunBuff( pOwner );

	pOwner->SetState( CS_ONIMUSHA_RUN_EXTRA_STATE );
	m_fCurRunExtraJumpRange = 0.0f;
}

void ioOniMushaObjectItem::ChangeToRunExtraAttack( ioBaseChar *pOwner )
{
	if( !pOwner ) 
		return;

	ioEntityGroup *pGrp = pOwner->GetGroup();
	if ( !pGrp )
		return;

	int nAniID = pGrp->GetAnimationIdx( m_RunExtraAttack.m_AttackAnimation );
	if ( nAniID == -1 )
		return;

	float fTimeRate  = m_RunExtraAttack.m_fAttackAniRate;
	DWORD dwPreDelay = m_RunExtraAttack.m_dwPreDelay;

	pGrp->ClearAllLoopAni( FLOAT100, true );
	pGrp->ClearAllActionAni( FLOAT100, true );
	pGrp->SetActionAni( nAniID, FLOAT100, FLOAT100, FLOAT1, FLOAT1/fTimeRate, 0.0f, dwPreDelay );	

	pOwner->ClearReservedSliding();
	pOwner->SetReservedSliding( m_RunExtraAttack.m_vForceInfoList, nAniID, fTimeRate, dwPreDelay );

	pOwner->RefreshFireTimeList( nAniID, m_RunExtraAttack.m_vWeaponInfoList, FTT_NORMAL_ATTACK, fTimeRate, dwPreDelay );

	DWORD dwCurTime = FRAMEGETTIME();
	m_dwMotionEndTime = dwCurTime + pGrp->GetAnimationFullTime( nAniID ) * fTimeRate;

	DWORD dwTrackingTime = dwCurTime + dwPreDelay + pGrp->GetAnimationEventTime_e( nAniID, "tracking_e" ) * fTimeRate;
	pOwner->SetAutoTargetTrackingEndTime( dwTrackingTime );

	m_RunExtraState = RES_Attack;

	if( pOwner->IsNeedProcess() )
	{
		ioUserKeyInput::DirKeyInput eNewDirKey = ioUserKeyInput::DKI_NONE;
		if ( pOwner->IsSettedDirection() )
		{
			eNewDirKey = pOwner->CheckCurDirKey();
			pOwner->SetTargetRotToDirKey( eNewDirKey );
		}

		if( pOwner->IsNeedSendNetwork() )
		{
			SP2Packet kPacket( CUPK_ITEM_GAUGE_OK );
			FillItemGaugePacketHeader( pOwner, kPacket );
			kPacket << static_cast<byte>( ST_RUN_EXTRA_ATTACK );
			kPacket << pOwner->GetWorldPosition();
			kPacket << (int)eNewDirKey;
			P2PNetwork::SendToAllPlayingUser( kPacket );
		}
	}
}

void ioOniMushaObjectItem::ProcessRunExtraJump( ioBaseChar *pOwner )
{
	if( !pOwner )
		return;

	ioPlayStage *pStage = pOwner->GetCreator();
	if ( !pStage )
		return;
	
	D3DXVECTOR3 vTargetDir = pOwner->GetWorldOrientation() * D3DXVECTOR3( 0.0f, 0.0f, -FLOAT1 );
	D3DXVec3Normalize( &vTargetDir, &vTargetDir );

	D3DXVECTOR3 vAxis;
	D3DXVec3Cross( &vAxis, &vTargetDir, &ioMath::UNIT_Y );
	D3DXVec3Normalize( &vAxis, &vAxis );

	D3DXQUATERNION qtNewRot;
	float fCurAngle = ioMath::ArrangeHead( m_fRunExtraJumpAngle );
	D3DXQuaternionRotationAxis( &qtNewRot, &vAxis, DEGtoRAD(fCurAngle) );

	D3DXVECTOR3 vMoveDir = qtNewRot * vTargetDir;
	D3DXVec3Normalize( &vMoveDir, &vMoveDir );

	if( vMoveDir == ioMath::VEC3_ZERO )
		return;

	if( m_fRunExtraJumpSpeed <= 0.0f )
		return;
	
	float fDistance = m_fRunExtraJumpSpeed * g_FrameTimer.GetSecPerFrame();
	if( m_fCurRunExtraJumpRange+fDistance >= m_fRunExtraJumpMaxRange )
		return;

	bool bCol = false;
	D3DXVECTOR3 vMoveAmt = vMoveDir * fDistance;
	m_fCurRunExtraJumpRange += fDistance;

	if( pStage->ReCalculateMoveVectorByTerrain( pOwner, &vMoveAmt, bCol ) )
		pOwner->Translate( vMoveAmt );

	D3DXVECTOR3 vPos = pOwner->GetWorldPosition();
	if( pOwner->CheckAxisLimit( &vPos ) )
		pOwner->SetWorldPosition( vPos );
}

void ioOniMushaObjectItem::OnRunExtra( ioBaseChar *pOwner )
{
	if( !pOwner ) 
		return;

	DWORD dwCurTime = FRAMEGETTIME();

	if( m_dwMotionEndTime < dwCurTime )
		pOwner->SetSKillEndJumpState( m_fRunExtraEndJumpPower, false, false, true, true );
	else 
	{
		ProcessRunExtraJump( pOwner );

		if ( pOwner->IsNeedProcess() )
		{
			if( m_dwKeyReserveTime > 0 && m_dwKeyReserveTime < dwCurTime && pOwner->IsAttackKey() )
				ChangeToRunExtraAttack( pOwner );
		}
	}
}

void ioOniMushaObjectItem::OnRunExtraAttack( ioBaseChar *pOwner )
{
	if( !pOwner ) 
		return;

	DWORD dwCurTime = FRAMEGETTIME();

	if( m_dwMotionEndTime < dwCurTime )
		pOwner->SetSKillEndJumpState( m_fRunExtraAttackEndJumpPower, false, false, true, true );
}

void ioOniMushaObjectItem::OnProcessRunExtraState( ioBaseChar *pOwner )
{
	if ( !pOwner )
		return;

	switch( m_RunExtraState )
	{
	case RES_Start:
		OnRunExtra( pOwner );
		break;
	case RES_Attack:
		OnRunExtraAttack( pOwner );
		break;
	}
}

bool ioOniMushaObjectItem::IsReleaseItem( const ioBaseChar* pOwner ) const
{
	if( pOwner )
	{
		ioOniMushaItem* pOniMushaItem = ToOniMushaItem( pOwner->GetWeapon() );
		if( pOniMushaItem && pOniMushaItem->GetCurBullet() > 0 )
			return false;
	}

	return true;
}

//-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

ioOnimushaRunExtraState::ioOnimushaRunExtraState()
{
}

ioOnimushaRunExtraState::~ioOnimushaRunExtraState()
{
}

void ioOnimushaRunExtraState::Initialize( ioBaseChar* pOwner, CharState eCurrState )
{
	ioSpecialStateBase::Initialize( pOwner, eCurrState );
}

void ioOnimushaRunExtraState::Finalize( ioBaseChar* pOwner, CharState eCurrState, CharState eNewState )
{
	ioSpecialStateBase::Finalize( pOwner, eCurrState, eNewState );
}

void ioOnimushaRunExtraState::CheckSpecialState( ioBaseChar* pOwner )
{
	ioOniMushaObjectItem *pOniMushaObjectItem = ToOniMushaObjectItem( pOwner->GetObject() );
	if( pOniMushaObjectItem )
		pOniMushaObjectItem->OnProcessRunExtraState( pOwner );
	else
	{
		pOwner->SetState( CS_DELAY );
		pOwner->SetFallState( true );
		return;
	}
}

void ioOnimushaRunExtraState::CheckFallStateChange( ioBaseChar* pOwner, float fMinFallHeightRate, float fBottomHeight, bool bBottomMaxCheck, float fMaxBottomHeight )
{
	return;
}

bool ioOnimushaRunExtraState::ProcessMove( ioBaseChar* pOwner, float fTimePerSec, DWORD dwPreTime, DWORD dwCurTime )
{
	return true;
}