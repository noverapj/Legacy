

#include "stdafx.h"
#include "ioDummyCharItem2.h"

#include "WeaponDefine.h"
#include "DummyCharDefine.h"

ioDummyCharItem2::ioDummyCharItem2()
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

	m_bCharged = false;
	m_iCurDummyCharIndex = -1;

	m_dwAniRotateTime = 0;
	m_fAniRotateAngle = 0.0f;

	m_iCurAniRotate = 0;
	m_vAniRotateInfoList.clear();

	m_itemMeshType = IMT_NORMAL;

	m_fCurGauge = 0.0f;
	m_dwMotionEndTime = 0;
}

ioDummyCharItem2::ioDummyCharItem2( const ioDummyCharItem2 &rhs )
	: ioWeaponItem( rhs ),
	m_AttackReadyAniList( rhs.m_AttackReadyAniList ),
	m_DummyCharName( rhs.m_DummyCharName ),
	m_vDummyCharOffset( rhs.m_vDummyCharOffset ),
	m_vDestroyDummyCharList( rhs.m_vDestroyDummyCharList ),
	m_stExtracEffectMesh( rhs.m_stExtracEffectMesh ),
	m_fMaxGauge( rhs.m_fMaxGauge ),
	m_fNeedGauge( rhs.m_fNeedGauge ),
	m_fIncreaseGauge( rhs.m_fIncreaseGauge ),
	m_fDecreaseGauge( rhs.m_fDecreaseGauge ),
	m_stChangeExtraMotion( rhs.m_stChangeExtraMotion ),
	m_fChangeExtraMotionRate( rhs.m_fChangeExtraMotionRate ),
	m_vTopMoveInfo( rhs.m_vTopMoveInfo ),
	m_TopMoveInfo_Charge( rhs.m_TopMoveInfo_Charge ),
	m_ChargeAttackAtt( rhs.m_ChargeAttackAtt ),
	m_dwChargeAttackDummyDelayTime( rhs.m_dwChargeAttackDummyDelayTime ),
	m_stSpecialEffect( rhs.m_stSpecialEffect ),
	m_DelayAnimation_extra( rhs.m_DelayAnimation_extra ),
	m_RunAnimation_extra( rhs.m_RunAnimation_extra ),
	m_DashAnimation_extra( rhs.m_DashAnimation_extra )
{
	m_ChargeState = CS_NONE;
	m_dwAttackStartTime = 0;
	m_iCurCombo = 0;

	m_iCurAirJump = 0;
	m_fAirJumpPowerRate = 0.0f;

	m_bCharged = false;
	m_iCurDummyCharIndex = -1;

	m_dwAniRotateTime = 0;
	m_fAniRotateAngle = 0.0f;

	m_iCurAniRotate = 0;
	m_vAniRotateInfoList.clear();

	m_fCurGauge = 0.0f;
	m_dwMotionEndTime = 0;

	m_dwChargeAttackDummyFireTime = 0;

	m_dwEffect = 0;
}

ioDummyCharItem2::~ioDummyCharItem2()
{
	m_vDestroyDummyCharList.clear();
}

void ioDummyCharItem2::LoadProperty( ioINILoader &rkLoader )
{
	ioWeaponItem::LoadProperty( rkLoader );

	int i;
	char szBuf[MAX_PATH];
	char szKey[MAX_PATH];

	int iExtendCnt = rkLoader.LoadInt_e( "max_combo", 0 );
	m_AttackReadyAniList.clear();
	for( i=0 ; i<iExtendCnt ; i++ )
	{
		wsprintf_e( szBuf, "attack_ready_animation%d", i+1 );
		rkLoader.LoadString( szBuf, "", szBuf, MAX_PATH );
		m_AttackReadyAniList.push_back( ioHashString(szBuf) );
	}

	rkLoader.LoadString_e( "special_state_effect", "", szBuf, MAX_PATH );
	m_stSpecialEffect = szBuf;

	rkLoader.LoadString_e( "dummy_char_name", "", szBuf, MAX_PATH );
	m_DummyCharName = szBuf;
	m_vDummyCharOffset.x = rkLoader.LoadFloat_e( "dummy_char_offset_x", 0.0f );
	m_vDummyCharOffset.y = rkLoader.LoadFloat_e( "dummy_char_offset_y", 0.0f );
	m_vDummyCharOffset.z = rkLoader.LoadFloat_e( "dummy_char_offset_z", 0.0f );

	int iCnt = rkLoader.LoadInt_e( "destroy_dummy_cnt", 0 );
	m_vDestroyDummyCharList.clear();
	for( int i=0; i < iCnt; ++i )
	{
		wsprintf_e( szKey, "destroy_dummy%d_name", i+1 );

		rkLoader.LoadString( szKey, "", szBuf, MAX_PATH );

		m_vDestroyDummyCharList.push_back( szBuf );
	}

	m_fMaxGauge = rkLoader.LoadFloat_e( "max_gauge", FLOAT100 );
	m_fNeedGauge = rkLoader.LoadFloat_e( "need_gauge", FLOAT100 );

	m_fIncreaseGauge = rkLoader.LoadFloat_e( "increase_gauge", FLOAT1 );
	m_fDecreaseGauge = rkLoader.LoadFloat_e( "decrease_gauge", 2.0f );

	rkLoader.LoadString_e( "extra_attack_mesh", "", szBuf, MAX_PATH );
	m_stExtracEffectMesh = szBuf;

	rkLoader.LoadString_e( "change_extra_motion", "", szBuf, MAX_PATH );
	m_stChangeExtraMotion = szBuf;
	m_fChangeExtraMotionRate = rkLoader.LoadFloat_e( "change_extra_motion_rate", FLOAT1 );

	iCnt = rkLoader.LoadInt_e( "top_move_cnt", 0 );
	for( int i = 0; i < iCnt; ++i )
	{
		StTopMoveInfo info;
		wsprintf( szKey, "top_rotate_x%d", i+1 );
		info.fRotate_x = rkLoader.LoadFloat( szKey, 0.f );

		wsprintf( szKey, "top_rotate_y%d", i+1 );
		info.fRotate_y = rkLoader.LoadFloat( szKey, 0.f );

		wsprintf( szKey, "top_move_length%d", i+1 );
		info.fLength = rkLoader.LoadFloat( szKey, 0.f );

		wsprintf( szKey, "top_create%d", i+1 );
		info.bCreate = rkLoader.LoadBool( szKey, false );

		m_vTopMoveInfo.push_back( info );
	}

	m_TopMoveInfo_Charge.fRotate_x	= rkLoader.LoadFloat_e( "charge_top_rotate_x", 0.f );
	m_TopMoveInfo_Charge.fRotate_y	= rkLoader.LoadFloat_e( "charge_top_rotate_y", 0.f );
	m_TopMoveInfo_Charge.fLength	= rkLoader.LoadFloat_e( "charge_top_move_length", 0.f );
	m_TopMoveInfo_Charge.bCreate	= rkLoader.LoadBool_e(  "charge_top_create", false );

	LoadAttackAttribute( "extend_charge_attack", m_ChargeAttackAtt, rkLoader );
	m_dwChargeAttackDummyDelayTime = rkLoader.LoadInt_e( "extend_charge_attack_wait_time", 0 );
	m_dwChargeAttackDummyFireTime = 0;

	rkLoader.LoadString_e( "delay_animation_extend", "", szBuf, MAX_PATH );
	m_DelayAnimation_extra = szBuf;

	rkLoader.LoadString_e( "run_animation_extend", "", szBuf, MAX_PATH );
	m_RunAnimation_extra = szBuf;

	rkLoader.LoadString_e( "dash_animation_extend", "", szBuf, MAX_PATH );
	m_DashAnimation_extra = szBuf;
}

ioItem* ioDummyCharItem2::Clone()
{
	return new ioDummyCharItem2( *this );
}

ioWeaponItem::WeaponSubType ioDummyCharItem2::GetSubType() const
{
	return WST_DUMMY_CHAR2;
}

void ioDummyCharItem2::OnEquiped( ioBaseChar *pOwner )
{
	ioWeaponItem::OnEquiped( pOwner );

	m_iCurDummyCharIndex = -1;

	SetExtendEquipEffect( pOwner );
}

void ioDummyCharItem2::OnReleased( ioBaseChar *pOwner )
{
	ioWeaponItem::OnReleased( pOwner );

	ReleaseExtendEquipEffect( pOwner );

	if( !m_stExtracEffectMesh.IsEmpty() )
		pOwner->DestroyEntity( m_stExtracEffectMesh );

	//이건 이펙트
	if( m_dwEffect != 0 )
	{
		pOwner->EndEffect(m_dwEffect, false );
		m_dwEffect = 0;
	}

	m_iCurDummyCharIndex = -1;
}

void ioDummyCharItem2::Initialize( ioBaseChar *pOwner, ioPlayStage* pStage )
{
	if( pOwner  )
		ChangeAttackType(pOwner, IMT_NORMAL, false, true);
}

void ioDummyCharItem2::SetInitialize( const ioHashString& szName, ioPlayStage* pStage, int nMeshType )
{
	ioBaseChar *pChar = pStage->GetBaseChar( szName );
	ChangeAttackType(pChar, (ItemMeshType)nMeshType, false, true);
}

void ioDummyCharItem2::SetNormalAttack( ioBaseChar *pOwner, int iCurCombo )
{
	m_bEnableJumpState = false;
	m_bEnableAttackState = false;
	m_bEnableDefenseState = false;
	m_iSkillInput = -1;

	m_bCharged = false;

	//예외 하드 코딩( ioChargeComboDash::SetTopBlExtendMod 참고 )
	if( iCurCombo == -1 )
	{
		pOwner->SetState( CS_ATTACK );
		return;
	}

	int iMaxCombo = GetMaxCombo();
	if( !COMPARE( iCurCombo, 0, iMaxCombo ) )
	{
		ioWeaponItem::SetNormalAttack( pOwner, iCurCombo );

		m_ChargeState = CS_NONE;
		m_iCurCombo = iCurCombo;
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

		DWORD dwTrackingTime = FRAMEGETTIME();
		dwTrackingTime += pGrp->GetAnimationEventTime_e( iAniID, "tracking_e" ) * fTimeRate;
		pOwner->SetAutoTargetTrackingEndTime( dwTrackingTime );
	}
}

void ioDummyCharItem2::CheckNormalAttack( ioBaseChar *pOwner )
{
	DWORD dwCurTime = FRAMEGETTIME();

	switch( m_ChargeState )
	{
	case CS_CHARGING:
		OnCharging( pOwner );
		break;
	case CS_ATTACK_FIRE:

		if( pOwner->IsNeedProcess() )
		{
			CheckCancelReserve( pOwner );

			if( ProcessCancel(pOwner) )
				return;
		}

		ioWeaponItem::CheckNormalAttack( pOwner );

		if( m_dwChargeAttackDummyFireTime && m_dwChargeAttackDummyFireTime < dwCurTime )
		{
			m_dwChargeAttackDummyFireTime = 0;
			if( m_itemMeshType == IMT_EXTRA && m_TopMoveInfo_Charge.bCreate )
				ChangeToDummyChar( pOwner );

			SetMoveDummyPos( pOwner, m_TopMoveInfo_Charge );
		}

		break;
	case CS_CHANGE_EXTRA:
		OnChangingExtraMotion( pOwner );
		break;
	case CS_DUMMY_CHAR:
		CheckAniRotate( pOwner );
		ioWeaponItem::CheckNormalAttack( pOwner );
		break;
	}
}

bool ioDummyCharItem2::IsExtracChangeMotion()
{
	if( m_ChargeState == CS_CHANGE_EXTRA )
		return true;

	return false;
}

void ioDummyCharItem2::OnCharging( ioBaseChar *pOwner )
{
	if( !pOwner->IsNeedProcess() )	return;

	if( pOwner->IsAttackKeyDown() )
	{
		if( m_itemMeshType == IMT_NORMAL )
		{
			if( !IsEnableGauge() )
			{
				ChangeToAttackFire( pOwner, false );
			}
			else if( m_dwAttackStartTime + m_dwChargeTime <= FRAMEGETTIME() )
			{
				ChangeExtaMotion( pOwner );
			}
		}
		else//IMT_EXTRA
		{
			if( m_dwAttackStartTime + m_dwChargeTime <= FRAMEGETTIME() )
			{
				//돌아 오는 공격
				ChangeToAttackFire( pOwner, true );
			}
		}
	}
	else
	{
		ChangeToAttackFire( pOwner, false );
	}
}


void ioDummyCharItem2::ChangeAttackType( ioBaseChar *pOwner, ItemMeshType meshtype, bool bMustSend, bool bFirst, bool bMustChange )
{
	if( !pOwner)
	{
		LOG.PrintTimeAndLog( 0, "ChangeAttackType Char NULL"  );
		return;
	}

	//같으면 아무것도 하지 말자
	if( m_itemMeshType == meshtype && bFirst == false && bMustSend == false && !bMustChange )
		return;

	m_itemMeshType = meshtype;

	if( !m_stExtracEffectMesh.IsEmpty() )
		pOwner->DestroyEntity( m_stExtracEffectMesh );

	//이건 이펙트
	if( m_dwEffect != 0 )
	{
		pOwner->EndEffect(m_dwEffect, false );
		m_dwEffect = 0;
	}

	if( m_itemMeshType == IMT_EXTRA )
	{
		if( !m_stExtracEffectMesh.IsEmpty() )
			pOwner->AddEntity( m_stExtracEffectMesh );

		if( !m_stSpecialEffect.IsEmpty() )
		{
			ioEffect *pEffect = pOwner->AttachEffect(m_stSpecialEffect);
			if( pEffect )
			{
				m_dwEffect = pEffect->GetUniqueID();
			}
		}
	}
	else
	{
		DestroyDummyChar( pOwner );
	}

	//SendPacket
	if( pOwner->IsNeedSendNetwork() )
	{
		SP2Packet kPacket( CUPK_ITEM_GAUGE_OK );
		FillItemGaugePacketHeader( pOwner, kPacket );
		kPacket << CS_CHANGE_NORMAL;
		kPacket << (int)m_itemMeshType;
		kPacket << pOwner->GetTargetRot();
		P2PNetwork::SendToAllPlayingUser( kPacket );
	}
}

void ioDummyCharItem2::CallItemProcess( ioBaseChar *pOwner )
{
	if( !pOwner || !pOwner->IsNeedProcess() )
		return;

	if( m_itemMeshType != IMT_EXTRA )
		return;

	if( m_fCurGauge <= 0 )
		ChangeAttackType( pOwner, IMT_NORMAL, true, false, true );
}

void ioDummyCharItem2::ChangeToAttackFire( ioBaseChar *pOwner, bool bCharged )
{
	if( !pOwner ) return;

	DWORD dwCurTime = FRAMEGETTIME();
	m_bCharged = bCharged;

	pOwner->InitExtendAttackTagInfo();

	const AttackAttribute *pAttribute = NULL;
	if( bCharged )
		pAttribute = &m_ChargeAttackAtt;
	else
		pAttribute = m_FormalAttack.GetAttribute( AT_NORMAL, m_iCurCombo );

	if( pAttribute )
	{
		pOwner->SetNormalAttackByAttribute( *pAttribute );
		m_dwCancelCheckTime = dwCurTime;
		m_dwInputCancelCheckTime = dwCurTime;

		m_szCurAni = pAttribute->m_AttackAnimation;
	}

	pOwner->ApplyNormalAttackResult( m_iCurCombo );

	m_ChargeState = CS_ATTACK_FIRE;

	if( !bCharged )
	{
		if( (int)m_vTopMoveInfo.size() > m_iCurCombo && m_itemMeshType == IMT_EXTRA && m_vTopMoveInfo[m_iCurCombo].bCreate )
			ChangeToDummyChar( pOwner );
		m_dwChargeAttackDummyFireTime = 0;
	}
	else
	{
		m_dwChargeAttackDummyFireTime = dwCurTime + m_dwChargeAttackDummyDelayTime;
	}

	if( pOwner->IsNeedSendNetwork() )
	{
		SP2Packet kPacket( CUPK_ITEM_GAUGE_OK );
		FillItemGaugePacketHeader( pOwner, kPacket );
		kPacket << (int)m_ChargeState;
		kPacket << m_iCurCombo;
		kPacket << bCharged;
		kPacket << pOwner->GetTargetRot();
		P2PNetwork::SendToAllPlayingUser( kPacket );
	}

	if( !bCharged )
	{ 
		if( (int)m_vTopMoveInfo.size() > m_iCurCombo )
		{
			StTopMoveInfo info = m_vTopMoveInfo[m_iCurCombo];
			SetMoveDummyPos( pOwner, info );
		}
	}
}

void ioDummyCharItem2::ChangeToDummyChar( ioBaseChar *pOwner )
{
	if( !pOwner ) return;

	ioEntityGroup *pGrp = pOwner->GetGroup();
	if( !pGrp ) return;

	int iAniID = -1;
	float fAniRate = FLOAT1;
	DWORD dwCurTime = FRAMEGETTIME();

	g_DummyCharMgr.DestroyDummyCharByNameList( pOwner, m_vDestroyDummyCharList );

	int iIndex = -1;
	D3DXVECTOR3 vTargetPos = ioMath::VEC3_ZERO;

	CreateDummyCharState( pOwner, vTargetPos, iIndex );
}

void ioDummyCharItem2::OnItemGaugeOK( ioBaseChar *pOwner, SP2Packet &rkPacket )
{
	if( !pOwner ) return;

	int iState;
	rkPacket >> iState;

	switch( iState )
	{
	case CS_ATTACK_FIRE:
		{
			bool bChargeInfo;
			rkPacket >> m_iCurCombo;
			rkPacket >> bChargeInfo;
			D3DXQUATERNION qtRot;
			rkPacket >> qtRot;
			pOwner->SetTargetRotToRotate( qtRot, true );

			ChangeToAttackFire( pOwner, bChargeInfo );
		}
		break;
	case CS_CHANGE_EXTRA:
		{
			D3DXQUATERNION qtRot;
			rkPacket >> qtRot;
			pOwner->SetTargetRotToRotate( qtRot, true );

			ChangeExtaMotion( pOwner );
		}
		break;
	case CS_DUMMY_CHAR:
		{
			D3DXQUATERNION qtRot;
			rkPacket >> qtRot;
			pOwner->SetTargetRotToRotate( qtRot, true );

			ApplyChangeToDummyChar( pOwner, rkPacket );
		}
		break;
	case CS_CHANGE_NORMAL:
		{
			int nMeshType;
			rkPacket >> nMeshType;

			D3DXQUATERNION qtRot;
			rkPacket >> qtRot;
			pOwner->SetTargetRotToRotate( qtRot, true );

			ChangeAttackType(pOwner, (ItemMeshType)nMeshType, false, false, true );
		}
		break;
	}
}

void ioDummyCharItem2::ApplyChangeToDummyChar( ioBaseChar *pOwner, SP2Packet &rkPacket )
{
	if( !pOwner ) return;
	ioEntityGroup *pGrp = pOwner->GetGroup();
	if( !pGrp ) return;

	int iAniID = -1;
	float fAniRate = FLOAT1;

	D3DXVECTOR3 vTargetPos = ioMath::VEC3_ZERO;
	int iIndex = -1;

	rkPacket >> vTargetPos;
	rkPacket >> iIndex;

	g_DummyCharMgr.DestroyDummyCharByNameList( pOwner, m_vDestroyDummyCharList );

	if( !m_CurExtendEquipEffect.IsEmpty() )
	{
		pOwner->EndEffect( m_CurExtendEquipEffect, false );
		m_CurExtendEquipEffect.Clear();
	}

	ioPlayStage *pStage = pOwner->GetCreator();
	if( !pStage ) return;

	ioDummyChar *pDummy = pStage->CreateDummyChar( m_DummyCharName, iIndex,
		pOwner->GetCharName(),
		vTargetPos,
		0.0f,
		0, false );

	if( pDummy )
	{
		m_iCurDummyCharIndex = iIndex;
	}
}

void ioDummyCharItem2::DestroyDummyChar( ioBaseChar *pOwner )
{
	g_DummyCharMgr.DestroyDummyCharByNameList( pOwner, m_vDestroyDummyCharList );
}

void ioDummyCharItem2::SetExtraInfoToWeapon( ioWeapon *pWeapon, bool bJumpAttack )
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

bool ioDummyCharItem2::IsChargeAttackState( ioBaseChar *pOwner )
{
	if( pOwner->GetState() == CS_ATTACK )
	{
		if( m_ChargeState == CS_CHARGING )
			return true;
	}

	return false;
}

bool ioDummyCharItem2::IsEnableAttackDashCheck( ioBaseChar *pOwner, bool bCheck )
{
	if( !IsChargeDash() ) return false;
	if( !m_bCharged ) return false;

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

void ioDummyCharItem2::CheckReserve( ioBaseChar *pOwner )
{
	bool bReserveSkill = false;
	int iSkillNum = pOwner->GetSkillKeyInput();
	if( iSkillNum >= 0 )
	{
		m_iSkillInput = iSkillNum;
		bReserveSkill = true;
	}

	if( pOwner->IsDefenseKeyDown() && !m_bEnableDefenseState && !bReserveSkill )
	{
		m_bEnableDefenseState = true;
		m_bEnableAttackState = false;
		m_bEnableJumpState = false;

		m_iSkillInput = -1;
	}
	else if( pOwner->IsAttackKey() && !m_bEnableAttackState && !bReserveSkill )
	{
		m_bEnableAttackState = true;
		m_bEnableDefenseState = false;
		m_bEnableJumpState = false;

		m_iSkillInput = -1;
	}
	else if( pOwner->IsJumpKey() && !m_bEnableJumpState && !bReserveSkill )
	{
		m_bEnableJumpState = true;
		m_bEnableAttackState = false;
		m_bEnableDefenseState = false;
		m_iSkillInput = -1;
	}
}

bool ioDummyCharItem2::SetJumpAttack( ioBaseChar *pOwner, int iAniID, float fTimeRate, AttackType eType )
{
	if( !pOwner ) return false;

	DWORD dwCurTime = FRAMEGETTIME();
	ClearAirJump();

	AttackType eExtendType = AT_JUMP_ATTACK;

	const AttackAttribute *pAttr = GetAttackAttribute( eExtendType );
	if( !pAttr )
	{
		LOG.PrintTimeAndLog( 0, "ioDummyCharItem2::SetJumpAttack - AttackAttribute Not Exist" );
		return false;
	}

	ioEntityGroup *pGrp = pOwner->GetGroup();
	iAniID = pGrp->GetAnimationIdx( pAttr->m_AttackAnimation );
	if( iAniID == -1 )
	{
		LOG.PrintTimeAndLog( 0, "ioDummyCharItem2::SetJumpAttack - JumpAttack Animation Not Exist" );
		return false;
	}

	fTimeRate   = pAttr->m_fAttackAniRate;
	DWORD dwPreDelay  = pAttr->m_dwPreDelay;

	// For AirJumpPower
	SetAirJump( pOwner, iAniID, fTimeRate );

	return false;
}

void ioDummyCharItem2::ProcessJumpAttack( ioBaseChar *pOwner, float fHeightGap )
{
	if( m_pExtendJump )
	{
		m_pExtendJump->ProcessJumpAttack( pOwner, fHeightGap );
	}

	CheckAirJump( pOwner );
}

void ioDummyCharItem2::CreateDummyCharState( IN ioBaseChar *pOwner, OUT D3DXVECTOR3& vPos, OUT int& iIndex )
{
	if( m_iCurDummyCharIndex != -1 ) return;

	if( !pOwner ) return;
	ioPlayStage *pStage = pOwner->GetCreator();
	if( !pStage ) return;

	D3DXVECTOR3 vNewOffSet = pOwner->GetTargetRot() * m_vDummyCharOffset;
	D3DXVECTOR3 vTargetPos = pOwner->GetMidPositionByRate() + vNewOffSet;

	int iCurIndex = g_DummyCharMgr.GetCurLocalIndex();

	vPos = vTargetPos;
	iIndex = iCurIndex;

	ioDummyChar *pDummy = pStage->CreateDummyChar( m_DummyCharName, iCurIndex,
		pOwner->GetCharName(),
		vTargetPos,
		0.0f,
		0, true );

	if( pDummy )
	{
		m_iCurDummyCharIndex = iCurIndex;
	}

	if( !m_CurExtendEquipEffect.IsEmpty() )
	{
		pOwner->EndEffect( m_CurExtendEquipEffect, false );
		m_CurExtendEquipEffect.Clear();
	}
}

void ioDummyCharItem2::CheckDummyCharState( ioBaseChar *pOwner, int iIndex )
{
	if( m_iCurDummyCharIndex == -1 )
		return;

	if( !pOwner ) return;

	if( m_iCurDummyCharIndex == iIndex )
	{
		m_iCurDummyCharIndex = -1;

		if( !m_CurExtendEquipEffect.IsEmpty() )
			pOwner->EndEffect( m_CurExtendEquipEffect, false );

		if( pOwner->IsMale() )
		{
			pOwner->AttachEffect( m_ExtendEquipEffect );
			m_CurExtendEquipEffect = m_ExtendEquipEffect;
		}
		else
		{
			pOwner->AttachEffect( m_ExtendEquipEffectHw );
			m_CurExtendEquipEffect = m_ExtendEquipEffectHw;
		}
	}
}

void ioDummyCharItem2::SetCurDummyCharIndex( ioBaseChar *pOwner, int iCurIndex )
{
	if( !pOwner ) return;

	ioDummyChar *pDummy = g_DummyCharMgr.FindDummyChar( pOwner, m_iCurDummyCharIndex );
	if( pDummy )
	{
		m_iCurDummyCharIndex = iCurIndex;
	}

	if( !m_CurExtendEquipEffect.IsEmpty() )
	{
		pOwner->EndEffect( m_CurExtendEquipEffect, false );
		m_CurExtendEquipEffect.Clear();
	}
}

void ioDummyCharItem2::UpdateExtraData( ioBaseChar *pOwner )
{
	if( !pOwner ) return;

	if( !pOwner->IsNeedProcess() )
		return;

	// check owner state
	float fTimePerSec = g_FrameTimer.GetSecPerFrame();

	if( m_itemMeshType == IMT_EXTRA )
	{
		float fGauge = m_fDecreaseGauge * fTimePerSec;
		m_fCurGauge -= fGauge;
		m_fCurGauge = max( 0.0f, m_fCurGauge );
	}
	else
	{
		if( m_fCurGauge < m_fMaxGauge )
		{
			float fGauge = m_fIncreaseGauge * fTimePerSec;

			m_fCurGauge += fGauge;
			m_fCurGauge = min( m_fCurGauge, m_fMaxGauge );
		}
	}
}

int ioDummyCharItem2::GetMaxBullet()
{
	return m_fMaxGauge;
}

int ioDummyCharItem2::GetNeedBullet()
{
	return m_fNeedGauge;
}

int ioDummyCharItem2::GetCurBullet()
{
	return m_fCurGauge;
}

void ioDummyCharItem2::SetCurBullet( int iCurBullet )
{
	if( iCurBullet < 0 )
		return;

	float fCurBullet = iCurBullet;
	fCurBullet = max( 0, min( fCurBullet, m_fMaxGauge ) );

	m_fCurGauge = fCurBullet;
}

void ioDummyCharItem2::InitGauge()
{
	m_fCurGauge = 0.0f;
}

void ioDummyCharItem2::MaxGauge()
{
	m_fCurGauge = m_fMaxGauge;
}

float ioDummyCharItem2::GetCurGauge()
{
	return m_fCurGauge;
}

void ioDummyCharItem2::SetCurGauge( float fGauge )
{
	m_fCurGauge = fGauge;
	m_fCurGauge = (float)max( 0, min( m_fCurGauge, m_fMaxGauge ) );
}

bool ioDummyCharItem2::IsEnableGauge()
{
	if( m_fCurGauge >= m_fNeedGauge )
		return true;

	return false;
}

void ioDummyCharItem2::ChangeExtaMotion( ioBaseChar *pOwner )
{
	if( !pOwner ) return;

	DWORD dwCurTime = FRAMEGETTIME();

	ioEntityGroup *pGrp = pOwner->GetGroup();
	if( !pGrp )
		return;

	pOwner->SetAttackMoveEnable( false );

	pGrp->ClearAllLoopAni( FLOAT100, true );
	pGrp->ClearAllActionAni( FLOAT100, true );

	int iAniID	     = pGrp->GetAnimationIdx( m_stChangeExtraMotion );
	float fTimeRate  = m_fChangeExtraMotionRate;
	if( fTimeRate <= 0.0f )
		fTimeRate = FLOAT1;

	pGrp->SetActionAni( iAniID, FLOAT100, FLOAT100, FLOAT1, FLOAT1/fTimeRate, 0.0f, 0.0f, true );

	m_dwMotionEndTime = dwCurTime + pGrp->GetAnimationFullTime( iAniID ) * fTimeRate;

	m_ChargeState = CS_CHANGE_EXTRA;

	if( pOwner->IsNeedSendNetwork() )
	{
		SP2Packet kPacket( CUPK_ITEM_GAUGE_OK );
		FillItemGaugePacketHeader( pOwner, kPacket );
		kPacket << m_ChargeState;
		kPacket << pOwner->GetTargetRot();
		P2PNetwork::SendToAllPlayingUser( kPacket );
	}
}

void ioDummyCharItem2::OnChangingExtraMotion( ioBaseChar *pOwner )
{
	DWORD dwCurTinme = FRAMEGETTIME();
	if( m_dwMotionEndTime < dwCurTinme )
	{
		ChangeAttackType( pOwner, IMT_EXTRA, true );
		pOwner->SetState( CS_DELAY, false );
	}
}

void ioDummyCharItem2::SetMoveDummyPos( ioBaseChar *pOwner, const StTopMoveInfo& info )
{
	ioDummyChar *pDummy = g_DummyCharMgr.FindDummyChar( pOwner, m_iCurDummyCharIndex );
	if( pDummy && pOwner )
	{
		ioMoveAttack3Dummy* pMoveDummy3 = ToioMoveAttack3Dummy( pDummy );
		if( pMoveDummy3 )
		{
			D3DXVECTOR3 vNewOffSet = pOwner->GetTargetRot() * m_vDummyCharOffset;
			D3DXVECTOR3 vPos = pOwner->GetMidPositionByRate() + vNewOffSet;

			D3DXVECTOR3 vDir = pOwner->GetTargetRot() * D3DXVECTOR3( 0.0f, 0.0f, -FLOAT1 );

			D3DXVec3Normalize(&vDir, &vDir);
			float fAngleH = ioMath::ArrangeHead( info.fRotate_x );
			float fAngleV = ioMath::ArrangeHead( info.fRotate_y );

			D3DXQUATERNION qtRot;
			D3DXVECTOR3 vAxis;
			D3DXVec3Cross( &vAxis, &vDir, &D3DXVECTOR3( 0.0f, -FLOAT1, 0.0f ) );

			// vertical
			D3DXQuaternionRotationAxis( &qtRot, &vAxis, DEGtoRAD(fAngleV) );
			vDir = qtRot * vDir;

			// horizon
			D3DXQuaternionRotationAxis( &qtRot, &ioMath::UNIT_Y, DEGtoRAD(fAngleH) );
			vDir = qtRot * vDir;

			pMoveDummy3->SetMovePosition( vPos + vDir * info.fLength );
		}
	}
}

bool ioDummyCharItem2::HasDelayAnimation() const
{
	if( m_itemMeshType == IMT_EXTRA )
		return !m_DelayAnimation_extra.IsEmpty();

	return __super::HasDelayAnimation();
}

const ioHashString& ioDummyCharItem2::GetDelayAnimation() const
{	
	if( m_itemMeshType == IMT_EXTRA )
		return m_DelayAnimation_extra;

	return __super::GetDelayAnimation();
}

bool ioDummyCharItem2::HasRunAnimation() const
{	
	if( m_itemMeshType == IMT_EXTRA )
		return !m_RunAnimation_extra.IsEmpty();

	return __super::HasRunAnimation();
}

const ioHashString& ioDummyCharItem2::GetRunAnimation() const
{	
	if( m_itemMeshType == IMT_EXTRA )
		return m_RunAnimation_extra;

	return __super::GetRunAnimation();
}

bool ioDummyCharItem2::HasDashRunAnimation() const
{	
	if( m_itemMeshType == IMT_EXTRA )
		return !m_DashAnimation_extra.IsEmpty();

	return __super::HasDashRunAnimation();
}

const ioHashString& ioDummyCharItem2::GetDashRunAnimation() const
{
	if( m_itemMeshType == IMT_EXTRA )
		return m_DashAnimation_extra;

	return __super::GetDashRunAnimation();
}