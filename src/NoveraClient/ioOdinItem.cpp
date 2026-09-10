

#include "stdafx.h"

#include "ioOdinItem.h"
#include "WeaponDefine.h"

ioOdinItem::ioOdinItem()
{
	m_dwChargeStartTime = 0;
	m_iCurCombo = 0;
	m_OdinState = KMS_NONE;
	m_UniqueObjID = -1;
}

ioOdinItem::ioOdinItem( const ioOdinItem &rhs )
	: ioWeaponItem( rhs ),
	m_BranchDelayAnimation( rhs.m_BranchDelayAnimation ),
	m_stFullChargeAni( rhs.m_stFullChargeAni ),
	m_fFullChargeAniRate( rhs.m_fFullChargeAniRate ),
	m_ReadyCircle( rhs.m_ReadyCircle ),
	m_EnableCircle( rhs.m_EnableCircle ),
	m_fMaxGauge( rhs.m_fMaxGauge ),
	m_fNeedGauge( rhs.m_fNeedGauge ),
	m_fIncreaseGauge( rhs.m_fIncreaseGauge )
{
	m_iCurCombo = 0;
	m_dwChargeStartTime = 0;

	m_OdinState = KMS_NONE;
	m_iCircleType = OCT_SIZE;
	m_iReadyCircleType = OCT_SIZE;
	m_UniqueObjID = -1;
	m_fCurGauge = 0.f;

	for( int i = 0; i < OCT_SIZE; i++ )
		m_cCircleInfo[i] = rhs.m_cCircleInfo[i];
}

ioOdinItem::~ioOdinItem()
{
}

void ioOdinItem::LoadProperty( ioINILoader &rkLoader )
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

	ioHashString szTypeText[OCT_SIZE] = { "attack", "defense", "jump" };
	for( int i = 0; i < OCT_SIZE; ++i )
	{
		wsprintf( szKey, "%s_circle_loop_ani", szTypeText[i].c_str() );
		rkLoader.LoadString( szKey, "", szBuf, MAX_PATH );
		m_cCircleInfo[i].m_szCircleLoopAni = szBuf;
		wsprintf( szKey, "%s_dummy_name", szTypeText[i].c_str() );
		rkLoader.LoadString( szKey, "", szBuf, MAX_PATH );
		m_cCircleInfo[i].m_szDummyName = szBuf;
		wsprintf( szKey, "%s_map_effect", szTypeText[i].c_str() );
		rkLoader.LoadString( szKey, "", szBuf, MAX_PATH );
		m_cCircleInfo[i].m_szMapEffect = szBuf;
		wsprintf( szKey, "%s_area_weapon", szTypeText[i].c_str() );
		rkLoader.LoadString( szKey, "", szBuf, MAX_PATH );
		m_cCircleInfo[i].m_szAreaWeaponName = szBuf;

		wsprintf( szKey, "%s_circle_attack", szTypeText[i].c_str() );
		LoadAttackAttribute( szKey, m_cCircleInfo[i].m_ActionAttack, rkLoader );

		wsprintf( szKey, "%s_circle_range", szTypeText[i].c_str() );
		m_cCircleInfo[i].m_fRange = rkLoader.LoadFloat( szKey, FLOAT1 ); 
		wsprintf( szKey, "%s_circle_move_speed", szTypeText[i].c_str() );
		m_cCircleInfo[i].m_fMoveSpeed = rkLoader.LoadFloat( szKey, FLOAT1 ); 
		wsprintf( szKey, "%s_circle_height_gap", szTypeText[i].c_str() );
		m_cCircleInfo[i].m_fHeightGap = rkLoader.LoadFloat( szKey, FLOAT1 ); 
		wsprintf( szKey, "%s_dummy_create_time", szTypeText[i].c_str() );
		m_cCircleInfo[i].m_dwDummyCreateTime = rkLoader.LoadInt( szKey, FLOAT1 ); 
	}
	m_fMaxGauge = rkLoader.LoadFloat_e( "max_gauge", FLOAT100 );
	m_fNeedGauge = rkLoader.LoadFloat_e( "need_gauge", FLOAT100 );
	m_fIncreaseGauge = rkLoader.LoadFloat_e( "increase_gauge", FLOAT100 );
}

ioItem* ioOdinItem::Clone()
{
	return new ioOdinItem( *this );
}

void ioOdinItem::SetNormalAttack( ioBaseChar *pOwner, int iCurCombo )
{
	ClearCancelInfo();
	ClearKeyReserve();
	InitAniRotate();

	int iMaxCombo = GetMaxCombo();
	if( COMPARE( iCurCombo, 0, iMaxCombo ) )
	{
		m_OdinState = KMS_CHARGING;
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

void ioOdinItem::OnEndNormalAttack( ioBaseChar *pOwner )
{
	ioWeaponItem::OnEndNormalAttack( pOwner );
	
	m_OdinState = KMS_NONE;
	m_iCircleType = OCT_SIZE;
	m_iReadyCircleType = OCT_SIZE;

	ioPlayStage *pStage = pOwner->GetCreator();
	if( pStage && m_UniqueObjID > 0 )
	{
		pStage->DestroyMapEffect( m_UniqueObjID );
		m_UniqueObjID = -1;
	}
}

void ioOdinItem::CheckNormalAttack( ioBaseChar *pOwner )
{
	switch( m_OdinState )
	{
	case KMS_CHARGING:
		OnCharging( pOwner );
		break;
	case KMS_NORMAL_ATTACK:
		if( pOwner->IsNeedProcess() )
		{
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

void ioOdinItem::OnCharging( ioBaseChar *pOwner )
{
	if( !pOwner->IsNeedProcess() )	
		return;
	if( m_dwChargeStartTime == 0 )
		return;

	DWORD dwCurTime = FRAMEGETTIME();
	if( pOwner->IsAttackKeyDown() )
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

void ioOdinItem::ChangeFullChargeState( ioBaseChar* pOwner )
{
	ioEntityGroup *pGrp = pOwner->GetGroup();
	if( !pGrp )
		return;

	m_OdinState = KMS_FULL_CHARGE_SELLECT;

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
		kPacket << (int)m_OdinState;
		kPacket << m_iCurCombo;
		P2PNetwork::SendToAllPlayingUser( kPacket );
	}
}

void ioOdinItem::OnFullChargeState( ioBaseChar* pOwner )
{
	if( !pOwner->IsNeedProcess() )
		return;

	if( m_fCurGauge < m_fMaxGauge )
	{
		float fTimePerSec = g_FrameTimer.GetSecPerFrame();
		float fGauge = m_fCurExtraGauge + m_fIncreaseGauge * fTimePerSec;
		SetCurExtraGauge( fGauge );

		if( m_fMaxExtraGauge == m_fCurExtraGauge )
		{
			int iCurBullet = GetCurBullet();
			SetCurBullet( iCurBullet + 1 );
			InitExtraGauge();
		}
	}

	if( pOwner->IsDefenseKeyDown() )
		ReadyOdinCircleType( pOwner, OCT_DEFENSE );
	else if( pOwner->IsJumpKeyDown() )
		ReadyOdinCircleType( pOwner, OCT_JUMP );

	if( !pOwner->IsAttackKeyDown() && m_iReadyCircleType == OCT_SIZE )
	{
		if( IsEnableGauge() )
			ChangeToCircleLoop( pOwner, OCT_ATTACK );
		else
		{
			InitExtraGauge();
			pOwner->SetState( CS_DELAY );
		}
	}
	else if( !pOwner->IsAttackKeyDown() && !pOwner->IsDefenseKeyDown() && !pOwner->IsJumpKeyDown() )
	{
		if( IsEnableGauge() )
			ChangeToCircleLoop( pOwner, m_iReadyCircleType );
		else
		{
			InitExtraGauge();
			pOwner->SetState( CS_DELAY );
		}
	}
}

void ioOdinItem::OnCircleLoop( ioBaseChar *pOwner )
{
	if( !pOwner || !pOwner->IsNeedProcess() )
		return;

	if( pOwner->IsAttackKey() )
	{
		SetActionState( pOwner, true );
		return;
	}

	if( pOwner->CheckMagicCircle() )
		pOwner->UpdateMagicCirclePos( m_cCircleInfo[m_iCircleType].m_fMoveSpeed, m_cCircleInfo[m_iCircleType].m_fRange, m_cCircleInfo[m_iCircleType].m_fHeightGap, false );
}

void ioOdinItem::OnCircleAction( ioBaseChar *pOwner )
{
	DWORD dwCurTime = FRAMEGETTIME();
	if( m_dwFireStartTime > 0 && m_dwFireStartTime < dwCurTime )
	{
		CreateWeapon( pOwner );
		m_dwFireStartTime = 0;
	}
	else if( m_dwCreateDummyTime > 0 && m_dwCreateDummyTime < dwCurTime )
	{
		CreateDummyChar( pOwner );
		m_dwCreateDummyTime = 0;
	}
	else if( m_dwMotionEndTime < dwCurTime )
	{
		m_OdinState = KMS_NONE;
		ioPlayStage *pStage = pOwner->GetCreator();
		if( pStage )
		{
			pStage->DestroyMapEffect( m_UniqueObjID );
			m_UniqueObjID = -1;
		}

		pOwner->DestroyMagicCircle();
		pOwner->SetState( CS_DELAY );
	}
}

void ioOdinItem::ReadyOdinCircleType( ioBaseChar *pOwner, int iCircleType )
{
	m_iCurCombo = 0;
	m_iReadyCircleType = iCircleType;
}

void ioOdinItem::ChangeToCircleLoop( ioBaseChar *pOwner, int iCircleType )
{
	pOwner->StopAutoTargetTracking();
	DecreaseGauge();

	m_OdinState = KMS_CIRCLE_LOOP;
	m_iReadyCircleType = OCT_SIZE;
	m_iCircleType = iCircleType;
	m_dwChargeStartTime = 0;
	m_iCurCombo = 0;

	ioEntityGroup *pGrp = pOwner->GetGroup();
	pGrp->ClearAllActionAni( FLOAT100, true );
	pGrp->ClearAllLoopAni( FLOAT100, true );
	pGrp->SetLoopAni( m_cCircleInfo[m_iCircleType].m_szCircleLoopAni, FLOAT100 );

	ioHashString szDummyCircle;
	pOwner->CreateMagicCircle( m_ReadyCircle, m_EnableCircle, szDummyCircle, m_fMagicCircleOffSet );

	if( pOwner->IsNeedSendNetwork() )
	{
		SP2Packet kPacket( CUPK_ITEM_GAUGE_CANCEL );
		FillItemGaugePacketHeader( pOwner, kPacket );
		kPacket << (int)m_OdinState;
		kPacket << m_iCurCombo;
		kPacket << iCircleType;
		P2PNetwork::SendToAllPlayingUser( kPacket );
	}
}

void ioOdinItem::ChangeToNormalAttack( ioBaseChar *pOwner )
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

		m_dwInputCancelCheckTime = dwCurTime;
		m_dwCancelCheckTime = dwCurTime;
	}
	pOwner->SetAutoTarget( ATT_NORMAL );
	pOwner->ApplyNormalAttackResult( m_iCurCombo );
	m_OdinState = KMS_NORMAL_ATTACK;

	if( pOwner->IsNeedSendNetwork() )
	{
		SP2Packet kPacket( CUPK_ITEM_GAUGE_CANCEL );
		FillItemGaugePacketHeader( pOwner, kPacket );
		kPacket << (int)m_OdinState;
		kPacket << m_iCurCombo;
		P2PNetwork::SendToAllPlayingUser( kPacket );
	}
}

void ioOdinItem::SetExtraInfoToWeapon( ioWeapon *pWeapon, bool bJumpAttack )
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

void ioOdinItem::OnItemGaugeCancel( ioBaseChar *pOwner, SP2Packet &rkPacket )
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
		rkPacket >> iCircleType;
		ChangeToCircleLoop( pOwner, iCircleType );
		break;
	case KMS_CIRCLE_ACTION:
		rkPacket >> m_iCircleType;
		rkPacket >> m_vCirclePos;
		SetActionState( pOwner, false );
		break;
	}
}

ioWeaponItem::WeaponSubType ioOdinItem::GetSubType() const
{
	return WST_ODIN_ITEM;
}

void ioOdinItem::OnReleased( ioBaseChar *pOwner )
{
	ioWeaponItem::OnReleased( pOwner );

	m_OdinState = KMS_NONE;
	m_iCircleType = OCT_SIZE;
	m_iReadyCircleType = OCT_SIZE;

	pOwner->SetChargingState( false );
}

bool ioOdinItem::IsChargeAttackState( ioBaseChar *pOwner )
{
	if( pOwner->GetState() == CS_ATTACK )
	{
		if( m_OdinState == KMS_CHARGING || m_OdinState == KMS_FULL_CHARGE_SELLECT ||
			m_OdinState == KMS_CIRCLE_LOOP || m_OdinState == KMS_CIRCLE_ACTION )
			return true;
	}

	return false;
}

bool ioOdinItem::IsEnableAttackRotateCheck( ioBaseChar *pOwner )
{
	if( pOwner->GetState() == CS_ATTACK )
	{
		switch( m_OdinState )
		{
		case KMS_NONE:
		case KMS_CHARGING:
		case KMS_FULL_CHARGE_SELLECT:
			return true;
		}
	}

	return false;
}

ioOdinItem::OdinState ioOdinItem::GetOdinState()
{
	return m_OdinState;
}

void ioOdinItem::SetActionState( ioBaseChar *pChar, bool bSend )
{
	m_OdinState = KMS_CIRCLE_ACTION;

	if( pChar->CheckMagicCircle() )
		pChar->UpdateMagicCirclePos( m_cCircleInfo[m_iCircleType].m_fMoveSpeed, m_cCircleInfo[m_iCircleType].m_fRange, m_cCircleInfo[m_iCircleType].m_fHeightGap, false );

	if( bSend )
		m_vCirclePos = pChar->GetMagicCirclePos();

	ioMapEffect* pMapEffect = pChar->CreateMapEffectBySkill( m_cCircleInfo[m_iCircleType].m_szMapEffect, m_vCirclePos );
	if( pMapEffect )
		m_UniqueObjID = pMapEffect->GetUniqueID();

	ioEntityGroup *pGrp = pChar->GetGroup();

	int iCurAniID = pGrp->GetAnimationIdx( m_cCircleInfo[m_iCircleType].m_ActionAttack.m_AttackAnimation );
	if( iCurAniID == -1 )	return;

	float fAniRate = m_cCircleInfo[m_iCircleType].m_ActionAttack.m_fAttackAniRate;
	if( fAniRate <= 0.f )
		fAniRate = FLOAT1;
		
	pGrp->ClearAllLoopAni( FLOAT100, true );
	pGrp->ClearAllActionAni( FLOAT100, true );
	pGrp->SetActionAni( iCurAniID, FLOAT100, FLOAT100, FLOAT1, FLOAT1/fAniRate );
	
	DWORD dwCurTime = FRAMEGETTIME();
	m_dwMotionEndTime = dwCurTime + pGrp->GetAnimationFullTime( iCurAniID ) * fAniRate;
	m_dwFireStartTime = dwCurTime + pGrp->GetAnimationEventTime_e( iCurAniID, "fire_s" ) * fAniRate;
	m_dwCreateDummyTime = dwCurTime + m_cCircleInfo[m_iCircleType].m_dwDummyCreateTime;

	if( pChar->IsNeedSendNetwork() )
	{
		SP2Packet kPacket( CUPK_ITEM_GAUGE_CANCEL );
		FillItemGaugePacketHeader( pChar, kPacket );
		kPacket << (int)m_OdinState;
		kPacket << m_iCurCombo;
		kPacket << m_iCircleType;
		kPacket << m_vCirclePos;
		P2PNetwork::SendToAllPlayingUser( kPacket );
	}
}

int ioOdinItem::GetMaxBullet()
{
	return (int)m_fMaxGauge;
}

int ioOdinItem::GetNeedBullet()
{
	return (int)m_fNeedGauge;
}

int ioOdinItem::GetCurBullet()
{
	float fCurCoolTime = m_fCurGauge;
	fCurCoolTime = max( 0, min( fCurCoolTime, m_fMaxGauge ) );

	return (int)fCurCoolTime;
}

void ioOdinItem::SetCurBullet( int iCurBullet )
{
	if( iCurBullet < 0 )
		return;

	float fCurBullet = iCurBullet;
	fCurBullet = max( 0, min( fCurBullet, m_fMaxGauge ) );

	m_fCurGauge = fCurBullet;
}

void ioOdinItem::DecreaseGauge()
{
	m_fCurGauge -= m_fNeedGauge;
	m_fCurGauge = (float)max( 0, min( m_fCurGauge, m_fMaxGauge ) );
}

bool ioOdinItem::IsEnableGauge()
{
	if( m_fCurGauge >= m_fNeedGauge )
		return true;

	return false;
}

void ioOdinItem::CreateDummyChar( ioBaseChar *pOwner )
{
	if ( !pOwner || m_cCircleInfo[m_iCircleType].m_szDummyName.IsEmpty() )
		return;

	ioPlayStage *pStage = pOwner->GetCreator();
	if ( !pStage )
		return;

	g_DummyCharMgr.DestroyDummyCharByName( pOwner, m_cCircleInfo[m_iCircleType].m_szDummyName );
		
	if ( m_vCirclePos.y <= 0.0f )
		return;

	int iCurIndex = g_DummyCharMgr.GetCurLocalIndex();
	ioDummyChar *pDummy = pStage->CreateDummyChar( m_cCircleInfo[m_iCircleType].m_szDummyName, iCurIndex, pOwner->GetCharName(), m_vCirclePos, 0.f, 0, true );
	if ( pDummy )
	{
		pDummy->SetWorldPosition( m_vCirclePos );
		pDummy->SetWorldOrientationAndDir( pOwner->GetWorldOrientation() );
	}
}

void ioOdinItem::CreateWeapon( ioBaseChar *pOwner )
{
	if ( !pOwner )
		return;

	int iAttackSize = m_cCircleInfo[m_iCircleType].m_ActionAttack.m_vWeaponInfoList.size();
	if( iAttackSize != 0 )
	{
		DWORD dwWeaponIndex = pOwner->IncreaseWeaponIndexBase();
		D3DXVECTOR3 vDir = pOwner->GetTargetRot() * D3DXVECTOR3( 0.0f, 0.0f, -FLOAT1 );
		for ( int i = 0; i<iAttackSize ; ++i )
		{
			WeaponInfo sWeaponInfo = m_cCircleInfo[m_iCircleType].m_ActionAttack.m_vWeaponInfoList[i];

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
	if( !m_cCircleInfo[m_iCircleType].m_szAreaWeaponName.IsEmpty() )
	{
		g_AreaWeaponMgr.CreateAreaWeapon( pOwner->GetCharName(), 
											"", 
											GetName(),
											m_cCircleInfo[m_iCircleType].m_szAreaWeaponName,
											m_vCirclePos, 
											pOwner->GetWorldOrientation(),
											ioAreaWeapon::CT_NORMAL );
	}
}