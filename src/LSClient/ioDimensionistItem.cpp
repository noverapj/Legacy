

#include "stdafx.h"

#include "ioDimensionistItem.h"
#include "WeaponDefine.h"

#include "ioTeleportDash.h"

ioDimensionistItem::ioDimensionistItem()
{
	m_dwChargeStartTime = 0;
	m_iCurCombo = 0;
	m_DimensionistState = KMS_NONE;
	m_UniqueObjID = -1;

	m_fTempBullet = 0.0f;

	m_iCurBullet = 0;

	m_dwSpecialStateChargeStartTime = 0;
	m_dwSpecialStateEndStartTime	= 0;
	m_iSpecialAttackCount			= 0;

	m_vCreateDummyPos = ioMath::VEC3_ZERO;
}

ioDimensionistItem::ioDimensionistItem( const ioDimensionistItem &rhs )
	: ioWeaponItem( rhs ),
	m_BranchDelayAnimation( rhs.m_BranchDelayAnimation ),
	m_stFullChargeAni( rhs.m_stFullChargeAni ),
	m_fFullChargeAniRate( rhs.m_fFullChargeAniRate ),
	m_ReadyCircle( rhs.m_ReadyCircle ),
	m_EnableCircle( rhs.m_EnableCircle ),
	m_fMaxGauge( rhs.m_fMaxGauge ),
	m_fNeedGauge( rhs.m_fNeedGauge ),
	m_fIncreaseGauge( rhs.m_fIncreaseGauge ),
	m_iMaxBullet( rhs.m_iMaxBullet ),
	m_iDashNeedBullet( rhs.m_iDashNeedBullet ),
	m_iSpecialAttackNeedBullet( rhs.m_iSpecialAttackNeedBullet ),
	m_fBulletIncreaseRate( rhs.m_fBulletIncreaseRate ),
	m_dwSpecialStateChargeTime( rhs.m_dwSpecialStateChargeTime ),
	m_cCircleInfo( rhs.m_cCircleInfo )
{
	m_iCurCombo = 0;
	m_dwChargeStartTime = 0;

	m_DimensionistState = KMS_NONE;
	m_UniqueObjID = -1;
	m_fCurGauge = 0.f;

	m_fTempBullet = 0.0f;

	m_iCurBullet = 0;

	m_dwSpecialStateChargeStartTime = 0;
	m_dwSpecialStateEndStartTime	= 0;
	m_iSpecialAttackCount			= 0;

	m_vCreateDummyPos = ioMath::VEC3_ZERO;
}

ioDimensionistItem::~ioDimensionistItem()
{
}

void ioDimensionistItem::LoadProperty( ioINILoader &rkLoader )
{
	ioWeaponItem::LoadProperty( rkLoader );

	char szBuf[MAX_PATH] = "";
	char szKey[MAX_PATH] = "";
	rkLoader.LoadString_e( "branch_delay_animation", "", szBuf, MAX_PATH );
	m_BranchDelayAnimation = szBuf;

	rkLoader.LoadString_e( "full_charge_ani", "", szBuf, MAX_PATH );
	m_stFullChargeAni = szBuf;
	m_fFullChargeAniRate = rkLoader.LoadFloat_e( "full_charge_ani_rate", FLOAT1 );

	rkLoader.LoadString_e( "ready_circle", "", szBuf, MAX_PATH );
	m_ReadyCircle = szBuf;
	rkLoader.LoadString_e( "enable_circle", "", szBuf, MAX_PATH );
	m_EnableCircle = szBuf;

	// 공격 더미 생성 관련
	rkLoader.LoadString( "attack_circle_loop_ani", "", szBuf, MAX_PATH );
	m_cCircleInfo.m_szCircleLoopAni = szBuf;

	rkLoader.LoadString( "attack_dummy_name", "", szBuf, MAX_PATH );
	m_cCircleInfo.m_szDummyName = szBuf;

	rkLoader.LoadString( "attack_map_effect", "", szBuf, MAX_PATH );
	m_cCircleInfo.m_szMapEffect = szBuf;

	rkLoader.LoadString( "attack_area_weapon", "", szBuf, MAX_PATH );
	m_cCircleInfo.m_szAreaWeaponName = szBuf;

	LoadAttackAttribute( "attack_circle_attack", m_cCircleInfo.m_ActionAttack, rkLoader );

	m_cCircleInfo.m_fRange = rkLoader.LoadFloat( "attack_circle_range", FLOAT1 ); 
	m_cCircleInfo.m_fMoveSpeed = rkLoader.LoadFloat( "attack_circle_move_speed", FLOAT1 ); 
	m_cCircleInfo.m_fHeightGap = rkLoader.LoadFloat( "attack_circle_height_gap", FLOAT1 ); 
	m_cCircleInfo.m_dwDummyCreateTime = rkLoader.LoadInt( "attack_dummy_create_time", FLOAT1 ); 

	m_fMaxGauge = rkLoader.LoadFloat_e( "max_gauge", FLOAT100 );
	m_fNeedGauge = rkLoader.LoadFloat_e( "need_gauge", FLOAT100 );
	m_fIncreaseGauge = rkLoader.LoadFloat_e( "increase_gauge", FLOAT100 );

	m_iMaxBullet				= rkLoader.LoadInt_e( "max_bullet", 100 );
	m_iDashNeedBullet			= rkLoader.LoadInt_e( "need_dash_bullet", 10 );
	m_iSpecialAttackNeedBullet	= rkLoader.LoadInt_e( "need_special_attack_bullet", 30 );
	m_fBulletIncreaseRate		= rkLoader.LoadFloat_e( "bullet_increase_rate", FLOAT1 );

	m_dwSpecialStateChargeTime	= (DWORD)rkLoader.LoadInt_e( "special_state_charge_time", 100 );
}

ioItem* ioDimensionistItem::Clone()
{
	return new ioDimensionistItem( *this );
}

void ioDimensionistItem::SetNormalAttack( ioBaseChar *pOwner, int iCurCombo )
{
	ClearCancelInfo();
	ClearKeyReserve();
	InitAniRotate();

	int iMaxCombo = GetMaxCombo();
	if( COMPARE( iCurCombo, 0, iMaxCombo ) )
	{
		m_DimensionistState = KMS_CHARGING;
		m_dwChargeStartTime = FRAMEGETTIME();
		m_iCurCombo = iCurCombo;

		ioEntityGroup *pGrp = pOwner->GetGroup();
		pGrp->ClearAllActionAni( FLOAT100, true );
		pGrp->ClearAllLoopAni( FLOAT100, true );

		int iAniID = pGrp->GetAnimationIdx( m_BranchDelayAnimation );
		float fTimeRate = (float)m_dwChargeTime / pGrp->GetAnimationFullTime( iAniID );
		pGrp->SetActionAni( iAniID, FLOAT100, FLOAT100, FLOAT1, FLOAT1/fTimeRate, 0.0f, 0.0f, true );

		pOwner->SetState( CS_ATTACK );
		pOwner->SetChargingState( true );
	}
	else
	{
		ioAttackableItem::SetNormalAttack( pOwner, iCurCombo );
		pOwner->SetAttackMoveEnable( false );
		pOwner->ChangeDirectionByInputDir( true );
	}
}

void ioDimensionistItem::OnEndNormalAttack( ioBaseChar *pOwner )
{
	ioWeaponItem::OnEndNormalAttack( pOwner );
	
	m_DimensionistState = KMS_NONE;
	m_iSpecialAttackCount = 0;
	m_vCreateDummyPos = ioMath::VEC3_ZERO;

	ioPlayStage *pStage = pOwner->GetCreator();
	if( pStage && m_UniqueObjID > 0 )
	{
		pStage->DestroyMapEffect( m_UniqueObjID );
		m_UniqueObjID = -1;
	}
}

void ioDimensionistItem::CheckNormalAttack( ioBaseChar *pOwner )
{
	switch( m_DimensionistState )
	{
	case KMS_CHARGING:
		OnCharging( pOwner );
		break;
	case KMS_NORMAL_ATTACK:
		if( pOwner->IsNeedProcess() )
		{
			CheckCancelReserveByConditionalSpecialDash( pOwner );

			if( ProcessCancelBySpeicalDash(pOwner) )
				return;

			CheckCancelReserve( pOwner );

			if( ProcessCancel(pOwner) )
				return;
		}
		ioAttackableItem::CheckNormalAttack( pOwner );
		break;
	case KMS_FULL_CHARGE_SELLECT:
		OnFullChargeState( pOwner );
		break;
	case KMS_CIRCLE_LOOP:
		OnCircleLoop( pOwner );
		break;
	case KMS_CIRCLE_ACTION:
		OnCircleAction( pOwner );
		break;
	}
}

void ioDimensionistItem::OnCharging( ioBaseChar *pOwner )
{
	if( !pOwner->IsNeedProcess() )	
		return;
	if( m_dwChargeStartTime == 0 )
		return;

	DWORD dwCurTime = FRAMEGETTIME();
	if( pOwner->IsAttackKeyDown() && CheckEnoughBulletsToDoSpecialAttack() )
	{
		if( m_dwChargeStartTime + m_dwChargeTime <= dwCurTime )
		{
			ChangeFullChargeState( pOwner );
		}
	}
	else
	{
		pOwner->SetChargingState( false );
		pOwner->SetAttackMoveEnable( false );
		ChangeToNormalAttack( pOwner );
	}
}

void ioDimensionistItem::ChangeFullChargeState( ioBaseChar* pOwner )
{
	ioEntityGroup *pGrp = pOwner->GetGroup();
	if( !pGrp )
		return;

	m_DimensionistState = KMS_FULL_CHARGE_SELLECT;

	int iAniID = pGrp->GetAnimationIdx( m_stFullChargeAni );

	pGrp->ClearAllLoopAniExceptOne( iAniID, FLOAT100, true );
	pGrp->ClearAllActionAni( FLOAT10, true );
	pGrp->SetLoopAni( iAniID, FLOAT100 );
	pOwner->SetAttackMoveEnable( false );
	pOwner->ChangeDirectionByInputDir( true );

	if( pOwner->IsNeedSendNetwork() )
	{
		SP2Packet kPacket( CUPK_ITEM_GAUGE_CANCEL );
		FillItemGaugePacketHeader( pOwner, kPacket );
		kPacket << (int)m_DimensionistState;
		kPacket << m_iCurCombo;
		P2PNetwork::SendToAllPlayingUser( kPacket );
	}
}

void ioDimensionistItem::OnFullChargeState( ioBaseChar* pOwner )
{
	if( !pOwner->IsNeedProcess() )
		return;
	
	if( !pOwner->IsAttackKeyDown() )
	{
		if( IsEnableGauge() )
			ChangeToCircleLoop( pOwner );
		else
		{
			InitExtraGauge();
			pOwner->SetState( CS_DELAY );
		}
	}
	else if( !pOwner->IsAttackKeyDown() && !pOwner->IsDefenseKeyDown() && !pOwner->IsJumpKeyDown() )
	{
		if( IsEnableGauge() )
			ChangeToCircleLoop( pOwner );
		else
		{
			InitExtraGauge();
			pOwner->SetState( CS_DELAY );
		}
	}
}

void ioDimensionistItem::OnCircleLoop( ioBaseChar *pOwner )
{
	if( !pOwner || !pOwner->IsNeedProcess() )
		return;

	DWORD dwCurTime = FRAMEGETTIME();

	if( pOwner->IsAttackKeyRelease() )
	{
		SetActionState( pOwner, true );
		return;
	}

	if( pOwner->IsAttackKeyDown() )
	{
		if( m_dwSpecialStateEndStartTime == 0 )
			m_dwSpecialStateEndStartTime = dwCurTime;
		// KMS_CIRCLE_LOOP 상태에서 다시 D~할때 더미 발사 후 상태 해제
		if( m_dwSpecialStateEndStartTime > 0 && m_dwSpecialStateEndStartTime + m_dwSpecialStateChargeTime < dwCurTime )
		{
			SetActionState( pOwner, true );
		}
	}

	if( pOwner->CheckMagicCircle() )
		pOwner->UpdateMagicCirclePos( m_cCircleInfo.m_fMoveSpeed, m_cCircleInfo.m_fRange, m_cCircleInfo.m_fHeightGap, false );
}

void ioDimensionistItem::OnCircleAction( ioBaseChar *pOwner )
{
	DWORD dwCurTime = FRAMEGETTIME();
	if( m_dwFireStartTime > 0 && m_dwFireStartTime < dwCurTime )
	{
		CreateWeapon( pOwner );
		m_dwFireStartTime = 0;
	}
	else if( m_dwCreateDummyTime > 0 && m_dwCreateDummyTime < dwCurTime )
	{
		if( pOwner->IsNeedProcess() )
		{
			CreateDummyChar( pOwner );

			if( pOwner->IsNeedSendNetwork() )
			{
				SP2Packet kPacket( CUPK_ITEM_GAUGE_CANCEL );
				FillItemGaugePacketHeader( pOwner, kPacket );
				kPacket << (int)KMS_CREATE_DUMMY;
				kPacket << m_iCurCombo;
				kPacket << m_vCirclePos;
				P2PNetwork::SendToAllPlayingUser( kPacket );
			}
		}
		m_dwCreateDummyTime = 0;
	}
	else if( m_dwMotionEndTime < dwCurTime )
	{
		ioPlayStage *pStage = pOwner->GetCreator();
		if( pStage )
		{
			pStage->DestroyMapEffect( m_UniqueObjID );
			m_UniqueObjID = -1;
		}

		if( pOwner->IsNeedProcess() )
		{
			if( !CheckEnoughBulletsToDoSpecialAttack() || ( m_dwSpecialStateEndStartTime > 0 && m_dwSpecialStateEndStartTime + m_dwSpecialStateChargeTime < dwCurTime ) )
			{
				pOwner->DestroyMagicCircle();
				m_DimensionistState = KMS_NONE;
				pOwner->SetState( CS_DELAY );
				m_iSpecialAttackCount = 0;
				m_vCreateDummyPos = ioMath::VEC3_ZERO;

				if( pOwner->IsNeedSendNetwork() )
				{
					SP2Packet kPacket( CUPK_ITEM_GAUGE_CANCEL );
					FillItemGaugePacketHeader( pOwner, kPacket );
					kPacket << (int)m_DimensionistState;
					kPacket << m_iCurCombo;
					P2PNetwork::SendToAllPlayingUser( kPacket );
				}
			}
			else
				ChangeToCircleLoop( pOwner );
		}

		m_dwSpecialStateEndStartTime = 0;

		m_dwSpecialStateChargeStartTime = 0;
	}
}

void ioDimensionistItem::ReadyDimensionistCircleType( ioBaseChar *pOwner )
{
	m_iCurCombo = 0;
}

void ioDimensionistItem::ChangeToCircleLoop( ioBaseChar *pOwner )
{
	pOwner->StopAutoTargetTracking();

	m_DimensionistState = KMS_CIRCLE_LOOP;
	m_dwChargeStartTime = 0;
	m_iCurCombo = 0;

	ioEntityGroup *pGrp = pOwner->GetGroup();
	pGrp->ClearAllActionAni( FLOAT100, true );
	pGrp->ClearAllLoopAni( FLOAT100, true );
	pGrp->SetLoopAni( m_cCircleInfo.m_szCircleLoopAni, FLOAT100 );

	if( m_iSpecialAttackCount < 3)
		m_iSpecialAttackCount++;

	ioHashString szDummyCircle;
	if( m_iSpecialAttackCount == 1 )
		pOwner->CreateMagicCircle( m_ReadyCircle, m_EnableCircle, szDummyCircle, m_fMagicCircleOffSet );
	else if( m_iSpecialAttackCount <= 3 )
		pOwner->CreateMagicCircle( m_ReadyCircle, m_EnableCircle, szDummyCircle, m_vCreateDummyPos, 0.0f );

	if( pOwner->IsNeedSendNetwork() )
	{
		SP2Packet kPacket( CUPK_ITEM_GAUGE_CANCEL );
		FillItemGaugePacketHeader( pOwner, kPacket );
		kPacket << (int)m_DimensionistState;
		kPacket << m_iCurCombo;
		P2PNetwork::SendToAllPlayingUser( kPacket );
	}
}

void ioDimensionistItem::ChangeToNormalAttack( ioBaseChar *pOwner )
{
	if( !pOwner )
		return;

	ioEntityGroup *pGrp = pOwner->GetGroup();
	if( !pGrp )
		return;

	ClearCancelInfo();

	DWORD dwCurTime = FRAMEGETTIME();
	const AttackAttribute *pAttribute = m_FormalAttack.GetAttribute( AT_NORMAL, m_iCurCombo );
	if( pAttribute )
	{
		pOwner->SetNormalAttackByAttribute( *pAttribute );

		m_szCurAni = pAttribute->m_AttackAnimation;
		m_fCurAniRate = pAttribute->m_fAttackAniRate;

		if( CheckEnoughBulletsToDoDash() )
			m_dwInputCancelCheckTime = dwCurTime;
		m_dwCancelCheckTime = dwCurTime;
	}
	pOwner->SetAutoTarget( ATT_NORMAL );
	pOwner->ApplyNormalAttackResult( m_iCurCombo );
	m_DimensionistState = KMS_NORMAL_ATTACK;

	if( pOwner->IsNeedSendNetwork() )
	{
		SP2Packet kPacket( CUPK_ITEM_GAUGE_CANCEL );
		FillItemGaugePacketHeader( pOwner, kPacket );
		kPacket << (int)m_DimensionistState;
		kPacket << m_iCurCombo;
		P2PNetwork::SendToAllPlayingUser( kPacket );
	}
}

void ioDimensionistItem::SetExtraInfoToWeapon( ioWeapon *pWeapon, bool bJumpAttack )
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

void ioDimensionistItem::OnItemGaugeCancel( ioBaseChar *pOwner, SP2Packet &rkPacket )
{
	int iCircleType;
	int iState;
	rkPacket >> iState;
	rkPacket >> m_iCurCombo;

	bool bEquipHand = false;

	switch( iState )
	{
	case KMS_NORMAL_ATTACK:
		ChangeToNormalAttack( pOwner );
		break;
	case KMS_FULL_CHARGE_SELLECT:
		ChangeFullChargeState( pOwner );
		break;
	case KMS_CIRCLE_LOOP:
		ChangeToCircleLoop( pOwner );
		break;
	case KMS_CIRCLE_ACTION:
		{
			rkPacket >> m_vCirclePos;

			// 이전 더미 생성 시 나타낸 이펙트 삭제
			ioPlayStage *pStage = pOwner->GetCreator();
			if( pStage && m_UniqueObjID != -1 )
			{
				pStage->DestroyMapEffect( m_UniqueObjID );
				m_UniqueObjID = -1;
			}

			SetActionState( pOwner, false );
		}
		break;
	case KMS_NONE:
		{
			pOwner->DestroyMagicCircle();
			m_DimensionistState = KMS_NONE;
			m_iSpecialAttackCount = 0;
			m_vCreateDummyPos = ioMath::VEC3_ZERO;
			pOwner->SetState( CS_DELAY );
		}
		break;
	case KMS_CANCEL_DASH:
		{
			ioTeleportDash *pTeleportDash = ToTeleportDash( GetCurExtendDash() );
			if ( !pTeleportDash )
				return;

			int iDirKey = 0;;
			rkPacket >> iDirKey;
			SetChangeConditionalSpecialDash( pOwner, (ioUserKeyInput::DirKeyInput)iDirKey, ioAttackableItem::CCSD_NORMAL );
		}
		break;
	case KMS_CREATE_DUMMY:
		{
			rkPacket >> m_vCirclePos;
			CreateDummyChar( pOwner );
		}
		break;
	}
}

ioWeaponItem::WeaponSubType ioDimensionistItem::GetSubType() const
{
	return WST_DIMENSIONIST_ITEM;
}

void ioDimensionistItem::OnReleased( ioBaseChar *pOwner )
{
	ioWeaponItem::OnReleased( pOwner );

	m_DimensionistState = KMS_NONE;
	m_iSpecialAttackCount = 0;
	m_vCreateDummyPos = ioMath::VEC3_ZERO;

	pOwner->SetChargingState( false );
}

bool ioDimensionistItem::IsChargeAttackState( ioBaseChar *pOwner )
{
	if( pOwner->GetState() == CS_ATTACK )
	{
		if( m_DimensionistState == KMS_CHARGING || m_DimensionistState == KMS_FULL_CHARGE_SELLECT ||
			m_DimensionistState == KMS_CIRCLE_LOOP || m_DimensionistState == KMS_CIRCLE_ACTION )
			return true;
	}

	return false;
}

bool ioDimensionistItem::IsEnableAttackRotateCheck( ioBaseChar *pOwner )
{
	if( pOwner->GetState() == CS_ATTACK )
	{
		switch( m_DimensionistState )
		{
		case KMS_NONE:
		case KMS_CHARGING:
		case KMS_FULL_CHARGE_SELLECT:
			return true;
		}
	}

	return false;
}

ioDimensionistItem::DimensionistState ioDimensionistItem::GetDimensionistState()
{
	return m_DimensionistState;
}

void ioDimensionistItem::SetActionState( ioBaseChar *pChar, bool bSend )
{
	m_DimensionistState = KMS_CIRCLE_ACTION;

	if( pChar->CheckMagicCircle() )
		pChar->UpdateMagicCirclePos( m_cCircleInfo.m_fMoveSpeed, m_cCircleInfo.m_fRange, m_cCircleInfo.m_fHeightGap, false );

	if( bSend )
		m_vCirclePos = pChar->GetMagicCirclePos();

	ioMapEffect* pMapEffect = pChar->CreateMapEffectBySkill( m_cCircleInfo.m_szMapEffect, m_vCirclePos );
	if( pMapEffect )
		m_UniqueObjID = pMapEffect->GetUniqueID();

	ioEntityGroup *pGrp = pChar->GetGroup();

	int iCurAniID = pGrp->GetAnimationIdx( m_cCircleInfo.m_ActionAttack.m_AttackAnimation );
	if( iCurAniID == -1 )	return;

	float fAniRate = m_cCircleInfo.m_ActionAttack.m_fAttackAniRate;
	if( fAniRate <= 0.f )
		fAniRate = FLOAT1;
		
	pGrp->ClearAllLoopAni( FLOAT100, true );
	pGrp->ClearAllActionAni( FLOAT100, true );
	pGrp->SetActionAni( iCurAniID, FLOAT100, FLOAT100, FLOAT1, FLOAT1/fAniRate );
	
	DWORD dwCurTime = FRAMEGETTIME();
	m_dwMotionEndTime = dwCurTime + pGrp->GetAnimationFullTime( iCurAniID ) * fAniRate;
	m_dwFireStartTime = dwCurTime + pGrp->GetAnimationEventTime_e( iCurAniID, "fire_s" ) * fAniRate;
	m_dwCreateDummyTime = dwCurTime + m_cCircleInfo.m_dwDummyCreateTime;

	if( pChar->IsNeedSendNetwork() )
	{
		SP2Packet kPacket( CUPK_ITEM_GAUGE_CANCEL );
		FillItemGaugePacketHeader( pChar, kPacket );
		kPacket << (int)m_DimensionistState;
		kPacket << m_iCurCombo;
		kPacket << m_vCirclePos;
		P2PNetwork::SendToAllPlayingUser( kPacket );
	}
}

int ioDimensionistItem::GetMaxBullet()
{
	return (int)m_fMaxGauge;
}

int ioDimensionistItem::GetNeedBullet()
{
	return m_iDashNeedBullet;
}

int ioDimensionistItem::GetCurBullet()
{
	return m_iCurBullet;
}

void ioDimensionistItem::SetCurBullet( int iCurBullet )
{
	if( iCurBullet < 0 )
		return;

	m_iCurBullet = max( 0, min( iCurBullet, m_iMaxBullet ) );
	m_fTempBullet = (float)m_iCurBullet;
}

int ioDimensionistItem::GetSpecialAttackNeedBullet()
{
	return m_iSpecialAttackNeedBullet;
}

bool ioDimensionistItem::CheckEnoughBulletsToDoDash()
{
	if( m_iCurBullet >= m_iDashNeedBullet )
		return true;
	return false;
}

void ioDimensionistItem::DecreaseBulletsToDoDash()
{
	if( m_iCurBullet >= m_iDashNeedBullet )
	{
		m_iCurBullet -= m_iDashNeedBullet;
		m_fTempBullet = (float)m_iCurBullet;
	}
}

bool ioDimensionistItem::CheckEnoughBulletsToDoSpecialAttack()
{
	if( m_iCurBullet >= m_iSpecialAttackNeedBullet )
		return true;
	return false;
}

void ioDimensionistItem::DecreaseBulletsToDoSpecialAttack()
{
	if( m_iCurBullet >= m_iSpecialAttackNeedBullet )
	{
		m_iCurBullet -= m_iSpecialAttackNeedBullet;
		m_fTempBullet = (float)m_iCurBullet;
	}
}

void ioDimensionistItem::DecreaseGauge()
{
	m_fCurGauge -= m_fNeedGauge;
	m_fCurGauge = (float)max( 0, min( m_fCurGauge, m_fMaxGauge ) );
}

bool ioDimensionistItem::IsEnableGauge()
{
	if( m_iCurBullet >= m_iSpecialAttackNeedBullet )
		return true;

	return false;
}

void ioDimensionistItem::CreateDummyChar( ioBaseChar *pOwner )
{
	if ( !pOwner || m_cCircleInfo.m_szDummyName.IsEmpty() )
		return;

	ioPlayStage *pStage = pOwner->GetCreator();
	if ( !pStage )
		return;
		
	if ( m_vCirclePos.y <= 0.0f )
		return;

	int iCurIndex = g_DummyCharMgr.GetCurLocalIndex();
	ioDummyChar *pDummy = pStage->CreateDummyChar( m_cCircleInfo.m_szDummyName, iCurIndex, pOwner->GetCharName(), m_vCirclePos, 0.f, 0, true );
	if ( pDummy )
	{
		m_vCreateDummyPos = m_vCirclePos;
		pDummy->SetWorldPosition( m_vCirclePos );
		pDummy->SetWorldOrientationAndDir( pOwner->GetWorldOrientation() );
		DecreaseBulletsToDoSpecialAttack();
	}
}

void ioDimensionistItem::CreateWeapon( ioBaseChar *pOwner )
{
	if ( !pOwner )
		return;

	int iAttackSize = m_cCircleInfo.m_ActionAttack.m_vWeaponInfoList.size();
	if( iAttackSize != 0 )
	{
		DWORD dwWeaponIndex = pOwner->IncreaseWeaponIndexBase();
		D3DXVECTOR3 vDir = pOwner->GetTargetRot() * D3DXVECTOR3( 0.0f, 0.0f, -FLOAT1 );
		for ( int i = 0; i<iAttackSize ; ++i )
		{
			WeaponInfo sWeaponInfo = m_cCircleInfo.m_ActionAttack.m_vWeaponInfoList[i];

			FireTime kFireTime;
			kFireTime.dwStart = FRAMEGETTIME();

			kFireTime.iAttributeIdx = sWeaponInfo.m_iWeaponIdx;
			kFireTime.iResistanceIdx = sWeaponInfo.m_iResistanceIdx;
			kFireTime.szWoundedAni = sWeaponInfo.m_WoundedAnimation;
			kFireTime.dwWoundedAniDuration = sWeaponInfo.m_dwWoundedDuration;
			kFireTime.bLoopWoundedAni = sWeaponInfo.m_bWoundedLoopAni;

			kFireTime.dwWeaponIdx = dwWeaponIndex++;
			kFireTime.eFireTimeType = FTT_NORMAL_ATTACK;
			
			pOwner->ExtendFireExplicit( kFireTime, m_vCirclePos, vDir, "" );
		}
	}
	if( !m_cCircleInfo.m_szAreaWeaponName.IsEmpty() )
	{
		g_AreaWeaponMgr.CreateAreaWeapon( pOwner->GetCharName(), 
											"", 
											GetName(),
											m_cCircleInfo.m_szAreaWeaponName,
											m_vCirclePos, 
											pOwner->GetWorldOrientation(),
											ioAreaWeapon::CT_NORMAL );
	}
}

void ioDimensionistItem::UpdateExtraData( ioBaseChar *pOwner )
{
	// 여기에서 bullet 업데이트를 한다.
	if( !pOwner )
		return;

	float fTimePerSec = g_FrameTimer.GetSecPerFrame();

	if( m_iCurBullet >= m_iMaxBullet )
	{
		m_iCurBullet = m_iMaxBullet;
		return;
	}

	switch( pOwner->GetState() )
	{
	case CS_DELAY:
	case CS_SPECIAL_MOTION:
	case CS_RUN:
		if( m_iCurBullet < m_iMaxBullet )
		{
			m_fTempBullet	+= fTimePerSec * m_fBulletIncreaseRate;
			m_iCurBullet	= (int)m_fTempBullet;

			m_fCurExtraGauge = m_fTempBullet;
			m_fCurExtraGauge = min( m_fCurExtraGauge, (float)m_iMaxBullet );
		}
		return;
	}
}

bool ioDimensionistItem::ProcessCancelBySpeicalDash( ioBaseChar *pOwner )
{
	if( m_szCurAni.IsEmpty() )
		return false;

	if( m_dwInputCancelCheckTime == 0 )
		return false;

	if( !pOwner )
		return false;

	ioEntityGroup *pGrp = pOwner->GetGroup();
	if( !pGrp )
		return false;

	DWORD dwCurTime = FRAMEGETTIME();
	int iAniID = pGrp->GetAnimationIdx( m_szCurAni );			
	DWORD dwInputSpecialDashTime = (DWORD)pGrp->GetAnimationEventTime_e( iAniID, "input_dash_s" );
	if( 0 < dwInputSpecialDashTime )
	{
		dwInputSpecialDashTime *= m_fCurAniRate;
		dwInputSpecialDashTime += m_dwInputCancelCheckTime;		
	}

	if( 0 < dwInputSpecialDashTime && dwCurTime > dwInputSpecialDashTime )
	{
		if( m_bEnableDashState && IsConditionalSpecialDashReserve( pOwner ) )
		{
			SetChangeConditionalSpecialDash( pOwner, m_ConditionalSpecialDashKey, ioAttackableItem::CCSD_NORMAL );
			return true;
		}
	}

	return false;
}

bool ioDimensionistItem::IsConditionalSpecialDashReserve( ioBaseChar *pOwner )
{
	return true;
}

void ioDimensionistItem::SetChangeConditionalSpecialDash( ioBaseChar *pOwner, ioUserKeyInput::DirKeyInput eCurrDir, int eCallerType )
{
	if( !pOwner || eCurrDir == ioUserKeyInput::DKI_NONE )
		return;

	ioEntityGroup *pGrp = pOwner->GetGroup();
	if( !pGrp )
		return;

	ioTeleportDash *pTeleportDash = ToTeleportDash( GetCurExtendDash() );
	if ( !pTeleportDash )
		return;

	ClearCancelInfo();
	pOwner->ClearDirDoubleClick();
	pOwner->ClearReserveKeyInput();
	pOwner->ClearReservedSliding();
	pOwner->SetCurJumpPowerAmt( 0.0f );
	pOwner->SetForcePowerAmt( 0.0f );
	pOwner->SetGravityAmt( 0.0f );
	pOwner->SetChargingState( false );

	float fYawD = 180.0f + 45.0f * ( eCurrDir - ioUserKeyInput::DKI_UP );
	fYawD = ioMath::ArrangeHead( fYawD );

	float fYawR = DEGtoRAD( fYawD );
	D3DXQUATERNION qtRotate;
	D3DXQuaternionRotationYawPitchRoll( &qtRotate, fYawR, 0.0f, 0.0f );

	pOwner->SetTargetRotToRotate( qtRotate, true, false );

	pTeleportDash->StartDash( pOwner );

	if ( pOwner->IsNeedSendNetwork() )
	{
		SP2Packet kPacket( CUPK_ITEM_GAUGE_CANCEL );
		FillItemGaugePacketHeader( pOwner, kPacket );
		kPacket << (int)KMS_CANCEL_DASH;
		kPacket << m_iCurCombo;
		kPacket << (int)eCurrDir;
		P2PNetwork::SendToAllPlayingUser( kPacket );
	}
}