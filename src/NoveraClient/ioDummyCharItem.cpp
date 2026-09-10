

#include "stdafx.h"

#include "ioDummyCharItem.h"

#include "ioPlayStage.h"
#include "ioBaseChar.h"

#include "WeaponDefine.h"
#include "ItemDefine.h"

ioDummyCharItem::ioDummyCharItem()
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
}

ioDummyCharItem::ioDummyCharItem( const ioDummyCharItem &rhs )
: ioWeaponItem( rhs ),
  m_AttackReadyAniList( rhs.m_AttackReadyAniList ),
  m_DummyCharName( rhs.m_DummyCharName ),
  m_fDummyCharOffset( rhs.m_fDummyCharOffset ),
  m_CreateDummyAttack( rhs.m_CreateDummyAttack ),
  m_vDestroyDummyCharList( rhs.m_vDestroyDummyCharList )
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
}

ioDummyCharItem::~ioDummyCharItem()
{
	m_vDestroyDummyCharList.clear();
}

void ioDummyCharItem::LoadProperty( ioINILoader &rkLoader )
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

	//
	rkLoader.LoadString_e( "dummy_char_name", "", szBuf, MAX_PATH );
	m_DummyCharName = szBuf;
	m_fDummyCharOffset = rkLoader.LoadFloat_e( "dummy_char_offset", 0.0f );

	LoadAttackAttribute_e( "dummy_char_create", m_CreateDummyAttack, rkLoader );

	int iCnt = rkLoader.LoadInt_e( "destroy_dummy_cnt", 0 );
	m_vDestroyDummyCharList.clear();
	for( int i=0; i < iCnt; ++i )
	{
		wsprintf_e( szKey, "destroy_dummy%d_name", i+1 );

		rkLoader.LoadString( szKey, "", szBuf, MAX_PATH );

		m_vDestroyDummyCharList.push_back( szBuf );
	}
}

ioItem* ioDummyCharItem::Clone()
{
	return new ioDummyCharItem( *this );
}

ioWeaponItem::WeaponSubType ioDummyCharItem::GetSubType() const
{
	return WST_DUMMY_CHAR;
}

void ioDummyCharItem::OnEquiped( ioBaseChar *pOwner )
{
	ioWeaponItem::OnEquiped( pOwner );

	m_iCurDummyCharIndex = -1;

	SetExtendEquipEffect( pOwner );
}

void ioDummyCharItem::OnReleased( ioBaseChar *pOwner )
{
	ioWeaponItem::OnReleased( pOwner );

	ReleaseExtendEquipEffect( pOwner );

	m_iCurDummyCharIndex = -1;
}

void ioDummyCharItem::SetNormalAttack( ioBaseChar *pOwner, int iCurCombo )
{
	m_bEnableJumpState = false;
	m_bEnableAttackState = false;
	m_bEnableDefenseState = false;
	m_iSkillInput = -1;

	m_bCharged = false;

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

void ioDummyCharItem::CheckNormalAttack( ioBaseChar *pOwner )
{
	switch( m_ChargeState )
	{
	case CS_CHARGING:
		OnCharging( pOwner );
		break;
	case CS_ATTACK_FIRE:
		ioWeaponItem::CheckNormalAttack( pOwner );
		break;
	case CS_DUMMY_CHAR:
		CheckAniRotate( pOwner );
		ioWeaponItem::CheckNormalAttack( pOwner );
		break;
	}
}

void ioDummyCharItem::OnCharging( ioBaseChar *pOwner )
{
	if( !pOwner->IsNeedProcess() )	return;

	if( pOwner->IsAttackKeyDown() )
	{
		if( m_dwAttackStartTime + m_dwChargeTime <= FRAMEGETTIME() )
		{
			ChangeToDummyChar( pOwner );
			g_TutorialMgr.ActionChargeAttack();
		}
	}
	else
	{
		ChangeToAttackFire( pOwner, false );
	}
}

void ioDummyCharItem::ChangeToAttackFire( ioBaseChar *pOwner, bool bCharged )
{
	if( !pOwner ) return;

	DWORD dwCurTime = FRAMEGETTIME();
	m_bCharged = bCharged;

	pOwner->InitExtendAttackTagInfo();

	const AttackAttribute *pAttribute = m_FormalAttack.GetAttribute( AT_NORMAL, m_iCurCombo );
	if( pAttribute )
	{
		pOwner->SetNormalAttackByAttribute( *pAttribute );
	}

	pOwner->ApplyNormalAttackResult( m_iCurCombo );

	m_ChargeState = CS_ATTACK_FIRE;

	if( pOwner->IsNeedSendNetwork() )
	{
		SP2Packet kPacket( CUPK_ITEM_GAUGE_OK );
		FillItemGaugePacketHeader( pOwner, kPacket );
		kPacket << (int)m_ChargeState;
		kPacket << m_iCurCombo;
		kPacket << pOwner->GetTargetRot();
		P2PNetwork::SendToAllPlayingUser( kPacket );
	}
}

void ioDummyCharItem::ChangeToDummyChar( ioBaseChar *pOwner )
{
	if( !pOwner ) return;
	if( !pOwner->IsNeedProcess() ) return;

	ioEntityGroup *pGrp = pOwner->GetGroup();
	if( !pGrp ) return;

	int iAniID = -1;
	float fAniRate = FLOAT1;
	DWORD dwCurTime = FRAMEGETTIME();

	pOwner->SetNormalAttackByAttribute( m_CreateDummyAttack );
	pOwner->ApplyNormalAttackResult( m_iCurCombo, true );

	iAniID = pGrp->GetAnimationIdx( m_CreateDummyAttack.m_AttackAnimation );
	if( m_CreateDummyAttack.m_fAttackAniRate > 0.0f )
	{
		fAniRate = m_CreateDummyAttack.m_fAttackAniRate;
	}

	m_ChargeState = CS_DUMMY_CHAR;
	SetAniRotate( pOwner, iAniID, fAniRate );

	g_DummyCharMgr.DestroyDummyCharByNameList( pOwner, m_vDestroyDummyCharList );

	int iIndex = -1;
	D3DXVECTOR3 vTargetPos = ioMath::VEC3_ZERO;

	CreateDummyCharState( pOwner, vTargetPos, iIndex );

	if( pOwner->IsNeedSendNetwork() )
	{
		SP2Packet kPacket( CUPK_ITEM_GAUGE_OK );
		FillItemGaugePacketHeader( pOwner, kPacket );
		kPacket << (int)m_ChargeState;
		kPacket << pOwner->GetTargetRot();
		kPacket << vTargetPos;
		kPacket << iIndex;
		P2PNetwork::SendToAllPlayingUser( kPacket );
	}
}

void ioDummyCharItem::OnItemGaugeOK( ioBaseChar *pOwner, SP2Packet &rkPacket )
{
	if( !pOwner ) return;

	int iState;
	rkPacket >> iState;

	switch( iState )
	{
	case CS_ATTACK_FIRE:
		{
			rkPacket >> m_iCurCombo;

			D3DXQUATERNION qtRot;
			rkPacket >> qtRot;
			pOwner->SetTargetRotToRotate( qtRot, true );

			ChangeToAttackFire( pOwner, false );
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
	}
}

void ioDummyCharItem::ApplyChangeToDummyChar( ioBaseChar *pOwner, SP2Packet &rkPacket )
{
	if( !pOwner ) return;
	ioEntityGroup *pGrp = pOwner->GetGroup();
	if( !pGrp ) return;

	int iAniID = -1;
	float fAniRate = FLOAT1;

	pOwner->SetNormalAttackByAttribute( m_CreateDummyAttack );
	pOwner->ApplyNormalAttackResult( m_iCurCombo, true );

	iAniID = pGrp->GetAnimationIdx( m_CreateDummyAttack.m_AttackAnimation );
	if( m_CreateDummyAttack.m_fAttackAniRate > 0.0f )
	{
		fAniRate = m_CreateDummyAttack.m_fAttackAniRate;
	}

	DWORD dwCurTime = FRAMEGETTIME();
	m_ChargeState = CS_DUMMY_CHAR;
	SetAniRotate( pOwner, iAniID, fAniRate );

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

void ioDummyCharItem::SetExtraInfoToWeapon( ioWeapon *pWeapon, bool bJumpAttack )
{
	ioAttackableItem::SetExtraInfoToWeapon( pWeapon, bJumpAttack );

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

bool ioDummyCharItem::IsChargeAttackState( ioBaseChar *pOwner )
{
	if( pOwner->GetState() == CS_ATTACK )
	{
		if( m_ChargeState == CS_CHARGING )
			return true;
	}

	return false;
}

bool ioDummyCharItem::IsEnableAttackDashCheck( ioBaseChar *pOwner, bool bCheck )
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

void ioDummyCharItem::CheckReserve( ioBaseChar *pOwner )
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

bool ioDummyCharItem::SetJumpAttack( ioBaseChar *pOwner, int iAniID, float fTimeRate, AttackType eType )
{
	if( !pOwner ) return false;

	DWORD dwCurTime = FRAMEGETTIME();
	ClearAirJump();

	AttackType eExtendType = AT_JUMP_ATTACK;

	const AttackAttribute *pAttr = GetAttackAttribute( eExtendType );
	if( !pAttr )
	{
		LOG.PrintTimeAndLog( 0, "ioDummyCharItem::SetJumpAttack - AttackAttribute Not Exist" );
		return false;
	}

	ioEntityGroup *pGrp = pOwner->GetGroup();
	iAniID = pGrp->GetAnimationIdx( pAttr->m_AttackAnimation );
	if( iAniID == -1 )
	{
		LOG.PrintTimeAndLog( 0, "ioDummyCharItem::SetJumpAttack - JumpAttack Animation Not Exist" );
		return false;
	}

	fTimeRate   = pAttr->m_fAttackAniRate;
	DWORD dwPreDelay  = pAttr->m_dwPreDelay;

	// For AirJumpPower
	SetAirJump( pOwner, iAniID, fTimeRate );

	return false;
}

void ioDummyCharItem::ProcessJumpAttack( ioBaseChar *pOwner, float fHeightGap )
{
	if( m_pExtendJump )
	{
		m_pExtendJump->ProcessJumpAttack( pOwner, fHeightGap );
	}

	CheckAirJump( pOwner );
}

void ioDummyCharItem::CreateDummyCharState( IN ioBaseChar *pOwner, OUT D3DXVECTOR3& vPos, OUT int& iIndex )
{
	if( m_iCurDummyCharIndex != -1 ) return;

	if( !pOwner ) return;
	ioPlayStage *pStage = pOwner->GetCreator();
	if( !pStage ) return;

	D3DXVECTOR3 vDir = pOwner->GetTargetRot() * D3DXVECTOR3( 0.0f, 0.0f, -FLOAT1 );
	D3DXVECTOR3 vTargetPos = pOwner->GetWorldPosition() + (m_fDummyCharOffset * vDir);
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

void ioDummyCharItem::CheckDummyCharState( ioBaseChar *pOwner, int iIndex )
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

void ioDummyCharItem::SetCurDummyCharIndex( ioBaseChar *pOwner, int iCurIndex )
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





