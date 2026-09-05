

#include "stdafx.h"

#include "ioKungfuMasterItem.h"
#include "WeaponDefine.h"

ioKungfuMasterItem::ioKungfuMasterItem()
{
	m_dwChargeStartTime = 0;
	m_dwPoseChangeStartTime = 0;
	m_iCurCombo = 0;
	m_KungfuState = KMS_NONE;

	m_bEquipedOnHand = false;
	m_fCurExtraGauge = 0.0f;
}

ioKungfuMasterItem::ioKungfuMasterItem( const ioKungfuMasterItem &rhs )
	: ioWeaponItem( rhs ),
	m_BranchDelayAnimation( rhs.m_BranchDelayAnimation ),
	m_CraneDelayAnimation( rhs.m_CraneDelayAnimation ),
	m_TigerDelayAnimation( rhs.m_TigerDelayAnimation ),
	m_SnakeDelayAnimation( rhs.m_SnakeDelayAnimation ),
	m_CraneRunAnimation( rhs.m_CraneRunAnimation ),
	m_TigerRunAnimation( rhs.m_TigerRunAnimation ),
	m_SnakeRunAnimation( rhs.m_SnakeRunAnimation ),
	m_CraneDefenseAnimation( rhs.m_CraneDefenseAnimation ),
	m_TigerDefenseAnimation( rhs.m_TigerDefenseAnimation ),
	m_SnakeDefenseAnimation( rhs.m_SnakeDefenseAnimation ),
	m_CraneOwnerBuffName( rhs.m_CraneOwnerBuffName ),
	m_TigerOwnerBuffName( rhs.m_TigerOwnerBuffName ),
	m_SnakeOwnerBuffName( rhs.m_SnakeOwnerBuffName ),
	m_vCraneAttributeList( rhs.m_vCraneAttributeList ),
	m_vTigerAttributeList( rhs.m_vTigerAttributeList ),
	m_vSnakeAttributeList( rhs.m_vSnakeAttributeList ),
	m_SleepAttackAttribute( rhs.m_SleepAttackAttribute ),
	m_SleepDashAttribute( rhs.m_SleepDashAttribute ),
	m_stFullChargeBuffName( rhs.m_stFullChargeBuffName ),
	m_stFullChargeAni( rhs.m_stFullChargeAni ),
	m_fFullChargeAniRate( rhs.m_fFullChargeAniRate ),
	m_fIncreaseExtraGauge( rhs.m_fIncreaseExtraGauge ),
	m_fNeedGauge( rhs.m_fNeedGauge ),
	m_SleepAni( rhs.m_SleepAni ),
	m_SleepStartAni( rhs.m_SleepStartAni ),
	m_fSleepStartAniRate( rhs.m_fSleepStartAniRate ),
	m_dwSleepTime( rhs.m_dwSleepTime ),
	m_GetUpAni( rhs.m_GetUpAni ),
	m_fGetUpAniRate( rhs.m_fGetUpAniRate ),
	m_dwPoseChangeTime( rhs.m_dwPoseChangeTime )
{
	m_iCurCombo = 0;

	m_bEquipedOnHand = false;
	m_dwChargeStartTime = 0;
	m_dwPoseChangeStartTime = 0;
	m_dwMotionEndTime = 0;
	m_dwSleepMacroTime_S = 0;

	m_KungfuState = KMS_NONE;
	m_iPoseIndex = KP_NONE;
	m_iReadyPoseIndex = KP_NONE;
	m_bPoseChange = false;
	m_fCurExtraGauge = 0.0f;
}

ioKungfuMasterItem::~ioKungfuMasterItem()
{
}

void ioKungfuMasterItem::LoadProperty( ioINILoader &rkLoader )
{
	ioWeaponItem::LoadProperty( rkLoader );

	char szBuf[MAX_PATH];

	int iCraneCnt = rkLoader.LoadInt_e( "crane_attack_cnt", 0 );
	m_vCraneAttributeList.clear();
	m_vCraneAttributeList.reserve( iCraneCnt );
	for( int i = 0 ;i < iCraneCnt ;i++ )
	{
		AttackAttribute kAttribute;
		wsprintf( szBuf, "crane_attack0%d", i+1 );
		LoadAttackAttribute( szBuf, kAttribute, rkLoader );
		m_vCraneAttributeList.push_back( kAttribute );
	}
	int iTigerCnt = rkLoader.LoadInt_e( "tiger_attack_cnt", 0 );
	m_vTigerAttributeList.clear();
	m_vTigerAttributeList.reserve( iTigerCnt );
	for( int i = 0 ;i < iTigerCnt ;i++ )
	{
		AttackAttribute kAttribute;
		wsprintf( szBuf, "tiger_attack0%d", i+1 );
		LoadAttackAttribute( szBuf, kAttribute, rkLoader );
		m_vTigerAttributeList.push_back( kAttribute );
	}
	int iSnakeCnt = rkLoader.LoadInt_e( "snake_attack_cnt", 0 );
	m_vSnakeAttributeList.clear();
	m_vSnakeAttributeList.reserve( iSnakeCnt );
	for( int i = 0 ;i < iSnakeCnt ;i++ )
	{
		AttackAttribute kAttribute;
		wsprintf( szBuf, "snake_attack0%d", i+1 );
		LoadAttackAttribute( szBuf, kAttribute, rkLoader );
		m_vSnakeAttributeList.push_back( kAttribute );
	}
	LoadAttackAttribute_e( "sleep_attack", m_SleepAttackAttribute, rkLoader );
	LoadAttackAttribute_e( "sleep_dash", m_SleepDashAttribute, rkLoader );

	rkLoader.LoadString_e( "crane_delay_animation", "", szBuf, MAX_PATH );
	m_CraneDelayAnimation = szBuf;
	rkLoader.LoadString_e( "tiger_delay_animation", "", szBuf, MAX_PATH );
	m_TigerDelayAnimation = szBuf;
	rkLoader.LoadString_e( "snake_delay_animation", "", szBuf, MAX_PATH );
	m_SnakeDelayAnimation = szBuf;
	rkLoader.LoadString_e( "crane_run_animation", "", szBuf, MAX_PATH );
	m_CraneRunAnimation = szBuf;
	rkLoader.LoadString_e( "tiger_run_animation", "", szBuf, MAX_PATH );
	m_TigerRunAnimation = szBuf;
	rkLoader.LoadString_e( "snake_run_animation", "", szBuf, MAX_PATH );
	m_SnakeRunAnimation = szBuf;
	rkLoader.LoadString_e( "crane_defense_animation", "", szBuf, MAX_PATH );
	m_CraneDefenseAnimation = szBuf;
	rkLoader.LoadString_e( "tiger_defense_animation", "", szBuf, MAX_PATH );
	m_TigerDefenseAnimation = szBuf;
	rkLoader.LoadString_e( "snake_defense_animation", "", szBuf, MAX_PATH );
	m_SnakeDefenseAnimation = szBuf;
	rkLoader.LoadString_e( "crane_owner_buff", "", szBuf, MAX_PATH );
	m_CraneOwnerBuffName = szBuf;
	rkLoader.LoadString_e( "tiger_owner_buff", "", szBuf, MAX_PATH );
	m_TigerOwnerBuffName = szBuf;
	rkLoader.LoadString_e( "snake_owner_buff", "", szBuf, MAX_PATH );
	m_SnakeOwnerBuffName = szBuf;

	rkLoader.LoadString_e( "branch_delay_animation", "", szBuf, MAX_PATH );
	m_BranchDelayAnimation = szBuf;

	rkLoader.LoadString_e( "full_charge_buff", "", szBuf, MAX_PATH );
	m_stFullChargeBuffName = szBuf;

	rkLoader.LoadString_e( "full_charge_ani", "", szBuf, MAX_PATH );
	m_stFullChargeAni = szBuf;
	m_fFullChargeAniRate = rkLoader.LoadFloat_e( "full_charge_ani_rate", FLOAT1 );

	rkLoader.LoadString_e( "sleep_ani", "", szBuf, MAX_PATH );
	m_SleepAni = szBuf;
	rkLoader.LoadString_e( "sleep_start_ani", "", szBuf, MAX_PATH );
	m_SleepStartAni = szBuf;
	m_fSleepStartAniRate = rkLoader.LoadFloat_e( "sleep_start_ani_rate", FLOAT1 );
	m_dwSleepTime = rkLoader.LoadInt_e( "sleep_time", FLOAT1 );

	rkLoader.LoadString_e( "getup_ani", "", szBuf, MAX_PATH );
	m_GetUpAni = szBuf;
	m_fGetUpAniRate = rkLoader.LoadFloat_e( "getup_ani_rate", FLOAT1 );

	m_fNeedGauge = rkLoader.LoadFloat_e( "need_gauge", FLOAT100 );
	m_fIncreaseExtraGauge = rkLoader.LoadFloat_e( "increase_gauge", .0f );

	m_dwPoseChangeTime = rkLoader.LoadInt_e( "pose_change_time", FLOAT1 );
}

ioItem* ioKungfuMasterItem::Clone()
{
	return new ioKungfuMasterItem( *this );
}

bool ioKungfuMasterItem::IsCanFire( const ioBaseChar* pOwner, int iFireCnt, bool bNormalAttack ) const
{
	DWORD dwCurTime = FRAMEGETTIME();
	if( m_dwPoseChangeStartTime != 0 && m_dwPoseChangeTime + m_dwPoseChangeStartTime > dwCurTime )
		return false;

	return true;
}

int ioKungfuMasterItem::GetMaxCombo() const
{
	int iMaxCombo = ioAttackableItem::GetMaxCombo();
	if( m_iPoseIndex == KP_CRANE )
		iMaxCombo = m_vCraneAttributeList.size();
	else if( m_iPoseIndex == KP_TIGER )
		iMaxCombo = m_vTigerAttributeList.size();
	else if( m_iPoseIndex == KP_SNAKE )
		iMaxCombo = m_vSnakeAttributeList.size();

	return iMaxCombo;
}

void ioKungfuMasterItem::SetNormalAttack( ioBaseChar *pOwner, int iCurCombo )
{
	ClearCancelInfo();
	ClearKeyReserve();
	InitAniRotate();

	m_dwSleepMacroTime_S = 0;

	ioHashString szDelayAnimation = m_BranchDelayAnimation;
	if( m_iPoseIndex == KP_CRANE )
		szDelayAnimation = m_CraneDelayAnimation;
	else if( m_iPoseIndex == KP_TIGER )
		szDelayAnimation = m_TigerDelayAnimation;
	else if( m_iPoseIndex == KP_SNAKE )
		szDelayAnimation = m_SnakeDelayAnimation;

	int iMaxCombo = GetMaxCombo();
	if( COMPARE( iCurCombo, 0, iMaxCombo ) )
	{
		m_KungfuState = KMS_CHARGING;
		m_dwChargeStartTime = FRAMEGETTIME();
		m_dwPoseChangeStartTime = 0;
		m_iCurCombo = iCurCombo;

		ioEntityGroup *pGrp = pOwner->GetGroup();
		pGrp->ClearAllActionAni( FLOAT100, true );
		pGrp->ClearAllLoopAni( FLOAT100, true );

		int iAniID = pGrp->GetAnimationIdx( szDelayAnimation );
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

void ioKungfuMasterItem::OnEndNormalAttack( ioBaseChar *pOwner )
{
	ioWeaponItem::OnEndNormalAttack( pOwner );
	
	pOwner->RemoveBuff( m_stFullChargeBuffName );

	m_KungfuState = KMS_NONE;
	if( !m_bPoseChange )
	{
		pOwner->RemoveBuff( m_CraneOwnerBuffName );
		pOwner->RemoveBuff( m_TigerOwnerBuffName );
		pOwner->RemoveBuff( m_SnakeOwnerBuffName );

		m_iPoseIndex = KP_NONE;
		m_iReadyPoseIndex = KP_NONE;
		m_bEquipedOnHand = false;
		m_dwPoseChangeStartTime = 0;
	}
	m_bPoseChange = false;
}

void ioKungfuMasterItem::CheckNormalAttack( ioBaseChar *pOwner )
{
	DWORD dwCurTime = FRAMEGETTIME();
	if( pOwner->IsNeedProcess() && EnableGauge() && m_dwSleepMacroTime_S != 0 && m_dwSleepMacroTime_S < dwCurTime )
	{
		if( pOwner->IsDefenseKey() )
		{
			SetSleepStart( pOwner, true );
			return;
		}
	}

	switch( m_KungfuState )
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
	}
}

void ioKungfuMasterItem::SetSleep( ioBaseChar *pOwner, bool bSendNet )
{
	if( !pOwner )
		return;

	ioEntityGroup *pGrp = pOwner->GetGroup();
	if( !pGrp )
		return;

	m_KungfuState = KMS_SLEEP;

	pOwner->ClearReserveKeyInput();
	pOwner->InitExtendAttackTagInfo();
	pOwner->SetAttackMoveEnable( false );

	int iAniID = pGrp->GetAnimationIdx( m_SleepAni );

	pGrp->ClearAllLoopAniExceptOne( iAniID, FLOAT100, true );
	pGrp->ClearAllActionAni( FLOAT10, true );
	pGrp->SetLoopAni( iAniID, FLOAT100 );

	m_dwSleepStartTime = FRAMEGETTIME();

	if(pOwner->IsNeedProcess() && bSendNet )
	{
		//Send
		SP2Packet kPacket( CUPK_ITEM_GAUGE_OK );
		FillItemGaugePacketHeader( pOwner, kPacket );
		kPacket << (int)m_KungfuState;
		kPacket << pOwner->GetWorldPosition();
		kPacket << pOwner->GetTargetRot();
		P2PNetwork::SendToAllPlayingUser( kPacket );
	}
}

void ioKungfuMasterItem::SetSleepStart( ioBaseChar *pOwner, bool bSendNet )
{
	if( !pOwner )
		return;

	ioEntityGroup *pGrp = pOwner->GetGroup();
	if( !pGrp )
		return;

	if( pOwner->IsNeedProcess() && pOwner->IsSettedDirection() )
	{
		ioUserKeyInput::DirKeyInput eNewDirKey = pOwner->CheckCurDirKey();
		pOwner->SetTargetRotToDirKey( eNewDirKey );
	}
	
	pOwner->SetState( CS_KUNGFU_SPECIAL );
	m_KungfuState = KMS_SLEEPSTART;

	DecreaseGauge();

	pGrp->ClearAllLoopAni( FLOAT100, true );
	pGrp->ClearAllActionAni( FLOAT100, true );

	int iAniID = pGrp->GetAnimationIdx( m_SleepStartAni );
	pGrp->SetActionAni( iAniID, FLOAT100, FLOAT100, FLOAT1, FLOAT1/m_fSleepStartAniRate, 0.0f, 0.0f, true );
	
	m_dwMotionEndTime = FRAMEGETTIME() + ( pGrp->GetAnimationFullTime( iAniID ) * m_fSleepStartAniRate );

	if( bSendNet && pOwner->IsNeedSendNetwork() )
	{
		SP2Packet kPacket( CUPK_ITEM_GAUGE_OK );
		FillItemGaugePacketHeader( pOwner, kPacket );
		kPacket << (int)m_KungfuState;
		kPacket << pOwner->GetWorldPosition();
		kPacket << pOwner->GetTargetRot();
		P2PNetwork::SendToAllPlayingUser( kPacket );
	}
}

void ioKungfuMasterItem::OnCharging( ioBaseChar *pOwner )
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

void ioKungfuMasterItem::ChangeFullChargeState( ioBaseChar* pOwner )
{
	ioEntityGroup *pGrp = pOwner->GetGroup();
	if( !pGrp )
		return;

	m_KungfuState = KMS_FULL_CHARGE_SELLECT;
	m_dwSleepMacroTime_S = 0;

	pOwner->RemoveBuff( m_CraneOwnerBuffName );
	pOwner->RemoveBuff( m_TigerOwnerBuffName );
	pOwner->RemoveBuff( m_SnakeOwnerBuffName );
	pOwner->RemoveBuff( m_stFullChargeBuffName );
	if( m_iPoseIndex == KP_NONE )
		pOwner->AddNewBuff( m_stFullChargeBuffName, pOwner->GetCharName(), GetName(), NULL );

	ioHashString szChargeAnimation = m_stFullChargeAni;
	if( m_iPoseIndex == KP_CRANE )
		szChargeAnimation = m_CraneDelayAnimation;
	else if( m_iPoseIndex == KP_TIGER )
		szChargeAnimation = m_TigerDelayAnimation;
	else if( m_iPoseIndex == KP_SNAKE )
		szChargeAnimation = m_SnakeDelayAnimation;

	int iAniID = pGrp->GetAnimationIdx( szChargeAnimation );

	pGrp->ClearAllLoopAniExceptOne( iAniID, FLOAT100, true );
	pGrp->ClearAllActionAni( FLOAT10, true );
	pGrp->SetLoopAni( iAniID, FLOAT100 );
	pOwner->SetAttackMoveEnable( false );
	pOwner->ChangeDirectionByInputDir( true );

	if( pOwner->IsNeedSendNetwork() )
	{
		SP2Packet kPacket( CUPK_ITEM_GAUGE_CANCEL );
		FillItemGaugePacketHeader( pOwner, kPacket );
		kPacket << (int)m_KungfuState;
		kPacket << m_iCurCombo;
		P2PNetwork::SendToAllPlayingUser( kPacket );
	}
}

void ioKungfuMasterItem::OnFullChargeState( ioBaseChar* pOwner )
{
	if( !pOwner->IsNeedProcess() )
		return;

	if( !pOwner->IsAttackKeyDown() && m_iReadyPoseIndex == KP_NONE )
	{
		ChangeKungfuPose( pOwner, KP_SNAKE );
		return;
	}

	if( pOwner->IsDefenseKeyDown() )
		ReadyKungfuPose( pOwner, KP_TIGER );
	else if( pOwner->IsJumpKeyDown() )
		ReadyKungfuPose( pOwner, KP_CRANE );

	if( !pOwner->IsAttackKeyDown() && !pOwner->IsDefenseKeyDown() && !pOwner->IsJumpKeyDown() )
		ChangeKungfuPose( pOwner, m_iReadyPoseIndex );		
}

void ioKungfuMasterItem::ReadyKungfuPose( ioBaseChar *pOwner, int iPose )
{
	m_bEquipedOnHand = true;
	m_bPoseChange = true;
	m_iCurCombo = 0;

	m_iReadyPoseIndex = iPose;
}

void ioKungfuMasterItem::ChangeKungfuPose( ioBaseChar *pOwner, int iPose )
{
	pOwner->StopAutoTargetTracking();

	if( m_iPoseIndex == KP_NONE )
		m_dwPoseChangeStartTime = FRAMEGETTIME();

	m_iReadyPoseIndex = KP_NONE;
	m_iPoseIndex = iPose;
	m_bPoseChange = true;
	m_dwChargeStartTime = 0;

	switch( m_iPoseIndex )
	{
	case KP_CRANE:
		pOwner->AddNewBuff( m_CraneOwnerBuffName, pOwner->GetCharName(), GetName(), NULL );
		break;
	case KP_TIGER:
		pOwner->AddNewBuff( m_TigerOwnerBuffName, pOwner->GetCharName(), GetName(), NULL );
		break;
	case KP_SNAKE:
		pOwner->AddNewBuff( m_SnakeOwnerBuffName, pOwner->GetCharName(), GetName(), NULL );
		break;
	}

	m_bEquipedOnHand = true;
	m_iCurCombo = 0;

	pOwner->RemoveBuff( m_stFullChargeBuffName );
	pOwner->SetAttackMoveEnable( true );
	pOwner->SetState( CS_DELAY );	

	if( pOwner->IsNeedSendNetwork() )
	{
		SP2Packet kPacket( CUPK_ITEM_GAUGE_CANCEL );
		FillItemGaugePacketHeader( pOwner, kPacket );
		kPacket << (int)KMS_CHANGE_POSE;
		kPacket << m_iCurCombo;
		kPacket << m_bEquipedOnHand;
		kPacket << iPose;
		P2PNetwork::SendToAllPlayingUser( kPacket );
	}
}

void ioKungfuMasterItem::ChangeToNormalAttack( ioBaseChar *pOwner )
{
	if( !pOwner )
		return;

	ioEntityGroup *pGrp = pOwner->GetGroup();
	if( !pGrp )
		return;

	ClearCancelInfo();

	DWORD dwCurTime = FRAMEGETTIME();
	const AttackAttribute *pAttribute = m_FormalAttack.GetAttribute( AT_NORMAL, m_iCurCombo );
	if( m_iPoseIndex == KP_CRANE )
		pAttribute = &m_vCraneAttributeList[m_iCurCombo];
	else if( m_iPoseIndex == KP_TIGER )
		pAttribute = &m_vTigerAttributeList[m_iCurCombo];
	else if( m_iPoseIndex == KP_SNAKE )
		pAttribute = &m_vSnakeAttributeList[m_iCurCombo];
	
	if( pAttribute )
	{
		pOwner->SetNormalAttackByAttribute( *pAttribute );

		m_szCurAni = pAttribute->m_AttackAnimation;
		m_fCurAniRate = pAttribute->m_fAttackAniRate;

		m_dwInputCancelCheckTime = dwCurTime;
		m_dwCancelCheckTime = dwCurTime;

		int iAniID = pGrp->GetAnimationIdx( pAttribute->m_AttackAnimation );
		SetMacroTime( pGrp, iAniID, pAttribute->m_fAttackAniRate, pAttribute->m_dwEndDelay );
	}
	pOwner->SetAutoTarget( ATT_NORMAL );
	pOwner->ApplyNormalAttackResult( m_iCurCombo );
	m_KungfuState = KMS_NORMAL_ATTACK;

	if( pOwner->IsNeedSendNetwork() )
	{
		SP2Packet kPacket( CUPK_ITEM_GAUGE_CANCEL );
		FillItemGaugePacketHeader( pOwner, kPacket );
		kPacket << (int)m_KungfuState;
		kPacket << m_iCurCombo;
		P2PNetwork::SendToAllPlayingUser( kPacket );
	}
}

void ioKungfuMasterItem::SetExtraInfoToWeapon( ioWeapon *pWeapon, bool bJumpAttack )
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
	pWeapon->GetOwner()->SetChargingState( false );
}

void ioKungfuMasterItem::OnItemGaugeCancel( ioBaseChar *pOwner, SP2Packet &rkPacket )
{
	int iPose;
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
	case KMS_CHANGE_POSE:
		rkPacket >> m_bEquipedOnHand;
		rkPacket >> iPose;
		ChangeKungfuPose( pOwner, iPose );
		break;
	}
}

void ioKungfuMasterItem::OnItemGaugeOK( ioBaseChar *pOwner, SP2Packet &rkPacket )
{
	if( !pOwner )
		return;
	
	int iState;
	rkPacket >> iState;

	D3DXVECTOR3 vPos;
	rkPacket >> vPos;
	pOwner->SetWorldPosition( vPos );

	D3DXQUATERNION qtRot;
	rkPacket >> qtRot;
	pOwner->SetTargetRotAndMoveDirChange( qtRot );

	switch( iState )
	{
	case KMS_SLEEP:
		SetSleep( pOwner, false );
		break;
	case KMS_SLEEPSTART:
		SetSleepStart( pOwner, false );
		break;
	case KMS_SLEEPEND:
		SetChangeEndState( pOwner, false );
		break;
	case KMS_GETUP:
		SetGetUpState( pOwner, false );
		break;
	case KMS_SLEEPATTACK:
		{
			int iDir;
			rkPacket >> iDir;
			SetSleepAttackState( pOwner, iDir, false );
		}
		break;
	case KMS_SLEEPDASH:
		{
			int iDir;
			rkPacket >> iDir;
			SetSleepDashState( pOwner, iDir, false );
		}
		break;
	}
}

ioWeaponItem::WeaponSubType ioKungfuMasterItem::GetSubType() const
{
	return WST_KUNGFU_ITEM;
}

void ioKungfuMasterItem::OnReleased( ioBaseChar *pOwner )
{
	ioWeaponItem::OnReleased( pOwner );

	m_KungfuState = KMS_NONE;
	m_iPoseIndex = KP_NONE;
	m_iReadyPoseIndex = KP_NONE;
	m_bEquipedOnHand = false;
	m_bPoseChange = false;
	m_dwPoseChangeStartTime = 0;

	pOwner->SetChargingState( false );
	pOwner->RemoveBuff( m_CraneOwnerBuffName );
	pOwner->RemoveBuff( m_TigerOwnerBuffName );
	pOwner->RemoveBuff( m_SnakeOwnerBuffName );
}

void ioKungfuMasterItem::FillNormalAttackExtraInfo( SP2Packet &rkPacket )
{
	ioAttackableItem::FillNormalAttackExtraInfo( rkPacket );
	rkPacket << m_bEquipedOnHand;
}

void ioKungfuMasterItem::ApplyNormalAttackExtraInfo( SP2Packet &rkPacket )
{
	rkPacket >> m_bEquipedOnHand;
}

bool ioKungfuMasterItem::CheckOnHand()
{
	return m_bEquipedOnHand;
}

bool ioKungfuMasterItem::IsChargeAttackState( ioBaseChar *pOwner )
{
	if( pOwner->GetState() == CS_ATTACK )
	{
		if( m_KungfuState == KMS_CHARGING )
			return true;
	}

	return false;
}

bool ioKungfuMasterItem::IsSleep( ioBaseChar *pOwner )
{
	if( pOwner->GetState() == CS_KUNGFU_SPECIAL )
	{
		if( m_KungfuState == KMS_SLEEP )
			return true;
	}
	return false;
}

bool ioKungfuMasterItem::IsEnableAttackRotateCheck( ioBaseChar *pOwner )
{
	if( pOwner->GetState() == CS_ATTACK )
	{
		switch( m_KungfuState )
		{
		case KMS_NONE:
		case KMS_CHARGING:
		case KMS_FULL_CHARGE_SELLECT:
			return true;
		}
	}

	return false;
}

void ioKungfuMasterItem::ClearState()
{
	m_KungfuState = KMS_NONE;
}

ioKungfuMasterItem::KungfuMasterState ioKungfuMasterItem::GetKungfuState()
{
	return m_KungfuState;
}

bool ioKungfuMasterItem::EnableGauge()
{
	if( m_fCurExtraGauge >= m_fNeedGauge )
		return true;

	return false;
}

void ioKungfuMasterItem::UpdateExtraData( ioBaseChar *pOwner )
{
	if( !pOwner ) return;

	// check owner state
	float fTimePerSec = g_FrameTimer.GetSecPerFrame();

	if( pOwner->IsSystemState() )
		return;

	switch( pOwner->GetState() )
	{
	case CS_DELAY:
	case CS_RUN:
	case CS_DASH:
		if( m_fCurExtraGauge < m_fMaxExtraGauge )
		{
			float fGauge = m_fIncreaseExtraGauge * fTimePerSec;

			m_fCurExtraGauge += fGauge;
			m_fCurExtraGauge = min( m_fCurExtraGauge, m_fMaxExtraGauge );
		}
		return;
	}
}

int ioKungfuMasterItem::GetCurBullet()
{
	return (int)m_fCurExtraGauge;
}

int ioKungfuMasterItem::GetMaxBullet()
{
	return (int)m_fMaxExtraGauge;
}

void ioKungfuMasterItem::SetCurBullet( int iCurBullet )
{
	if( iCurBullet < 0 )
		return;

	float fCurBullet = iCurBullet;
	fCurBullet = max( 0, min( fCurBullet, m_fMaxExtraGauge ) );

	m_fCurExtraGauge = fCurBullet;
}

int ioKungfuMasterItem::GetNeedBullet()
{
	return (int)m_fNeedGauge;
}

void ioKungfuMasterItem::DecreaseGauge()
{
	m_fCurExtraGauge -= m_fNeedGauge;
	m_fCurExtraGauge = (float)max( 0, min( m_fCurExtraGauge, m_fMaxExtraGauge ) );
}

const ioHashString& ioKungfuMasterItem::GetDelayAnimation() const
{
	switch( m_iPoseIndex )
	{
	case KP_CRANE:		return m_CraneDelayAnimation;
	case KP_TIGER:		return m_TigerDelayAnimation;
	case KP_SNAKE:		return m_SnakeDelayAnimation;
	}
	return m_DelayAnimation;
}

const ioHashString& ioKungfuMasterItem::GetRunAnimation() const
{
	switch( m_iPoseIndex )
	{
	case KP_CRANE:		return m_CraneRunAnimation;
	case KP_TIGER:		return m_TigerRunAnimation;
	case KP_SNAKE:		return m_SnakeRunAnimation;
	}
	return m_RunAnimation;
}

ioHashString ioKungfuMasterItem::GetDefenseAnimation() const
{
	ioHashString szDefenseAnimation = m_DefenseAnimation;
	switch( m_iPoseIndex )
	{
	case KP_CRANE:		szDefenseAnimation = m_CraneDefenseAnimation;	break;
	case KP_TIGER:		szDefenseAnimation = m_TigerDefenseAnimation;	break;
	case KP_SNAKE:		szDefenseAnimation = m_SnakeDefenseAnimation;	break;
	}
	if( !szDefenseAnimation.IsEmpty() )
		return szDefenseAnimation;

	return m_DefenseAnimation;
}

bool ioKungfuMasterItem::SetExtendDashState( ioBaseChar *pOwner )
{
	m_KungfuState = KMS_NONE;
	m_iPoseIndex = KP_NONE;
	m_iReadyPoseIndex = KP_NONE;
	m_bEquipedOnHand = false;
	m_bPoseChange = false;
	m_dwPoseChangeStartTime = 0;

	pOwner->RemoveBuff( m_CraneOwnerBuffName );
	pOwner->RemoveBuff( m_TigerOwnerBuffName );
	pOwner->RemoveBuff( m_SnakeOwnerBuffName );

	return ioAttackableItem::SetExtendDashState( pOwner );
}

void ioKungfuMasterItem::SetJumpping( ioBaseChar *pOwner, bool bFullTime )
{
	m_KungfuState = KMS_NONE;
	m_iPoseIndex = KP_NONE;
	m_iReadyPoseIndex = KP_NONE;
	m_bEquipedOnHand = false;
	m_bPoseChange = false;
	m_dwPoseChangeStartTime = 0;

	pOwner->RemoveBuff( m_CraneOwnerBuffName );
	pOwner->RemoveBuff( m_TigerOwnerBuffName );
	pOwner->RemoveBuff( m_SnakeOwnerBuffName );

	ioAttackableItem::SetJumpping( pOwner, bFullTime );
}

void ioKungfuMasterItem::SetMacroTime( ioEntityGroup* pGrp, int iAniID, float fTimeRate, DWORD dwEndTime )
{
	m_dwSleepMacroTime_S = 0;
	if( !pGrp )
		return;

	DWORD dwCurTime = FRAMEGETTIME();
	DWORD dwMacroTime_S = pGrp->GetAnimationEventTime_e( iAniID, "key_reserve" ) * fTimeRate;
	if( dwMacroTime_S != 0 )
		m_dwSleepMacroTime_S = dwCurTime + dwMacroTime_S;
}

void ioKungfuMasterItem::ClearAttackState( ioBaseChar *pOwner )
{
	m_dwMotionEndTime = 0;
}

void ioKungfuMasterItem::ProcessSpecialState( ioBaseChar *pOwner )
{
	if( !pOwner )
		return;

	ioPlayStage *pStage = pOwner->GetCreator();
	if( !pStage )
		return;

	DWORD dwCurTime = FRAMEGETTIME();
	switch( m_KungfuState )
	{
	case KMS_SLEEP:
		if( dwCurTime > m_dwSleepStartTime + m_dwSleepTime )
		{
			SetGetUpState( pOwner, true );			
			return;
		}
		if( pOwner->IsAttackKey() )
			SetSleepAttackState( pOwner, ioUserKeyInput::DKI_NONE, true );
		else if( pOwner->IsDirKeyDoubleClick() )
		{
			ioUserKeyInput::DirKeyInput iDashDir = pOwner->CheckCurDirKey();
			SetSleepDashState( pOwner, iDashDir, true );
		}
		break;
	case KMS_SLEEPSTART:
		if( m_dwMotionEndTime > 0 && m_dwMotionEndTime < FRAMEGETTIME() )
		{
			SetSleep( pOwner, true );
		}
		break;
	case KMS_SLEEPATTACK:
	case KMS_SLEEPDASH:
	case KMS_GETUP:
		if( m_dwMotionEndTime > 0 && m_dwMotionEndTime < FRAMEGETTIME() )
		{
			SetChangeEndState( pOwner, true );
		}
		break;
	}
}

void ioKungfuMasterItem::SetChangeEndState( ioBaseChar *pOwner, bool bSendNet )
{
	if( !pOwner )
		return;

	ClearAttackState( pOwner );
	pOwner->SetState( CS_DELAY );

	if( pOwner->IsNeedSendNetwork() && bSendNet )
	{
		SP2Packet kPacket( CUPK_ITEM_GAUGE_OK );
		FillItemGaugePacketHeader( pOwner, kPacket );
		kPacket << (int)KMS_SLEEPEND;
		kPacket << pOwner->GetWorldPosition();
		kPacket << pOwner->GetTargetRot();
		P2PNetwork::SendToAllPlayingUser( kPacket );
	}
}

void ioKungfuMasterItem::SetGetUpState( ioBaseChar *pOwner, bool bSendNet )
{
	if( !pOwner )
		return;

	ioEntityGroup *pGrp = pOwner->GetGroup();
	if( !pGrp )
		return;

	pGrp->ClearAllLoopAni( FLOAT100, true );
	pGrp->ClearAllActionAni( FLOAT100, true );

	int iAniID = pGrp->GetAnimationIdx( m_GetUpAni );
	pGrp->SetActionAni( iAniID, FLOAT100, FLOAT100, FLOAT1, FLOAT1/m_fGetUpAniRate, 0.0f, 0.0f, true );
	
	m_KungfuState = KMS_GETUP;
	m_dwMotionEndTime = FRAMEGETTIME() + ( pGrp->GetAnimationFullTime( iAniID ) * m_fGetUpAniRate );

	if( bSendNet && pOwner->IsNeedSendNetwork() )
	{
		SP2Packet kPacket( CUPK_ITEM_GAUGE_OK );
		FillItemGaugePacketHeader( pOwner, kPacket );
		kPacket << (int)m_KungfuState;
		kPacket << pOwner->GetWorldPosition();
		kPacket << pOwner->GetTargetRot();
		P2PNetwork::SendToAllPlayingUser( kPacket );
	}
}

void ioKungfuMasterItem::SetSleepAttackState( ioBaseChar *pOwner, int iDir, bool bSendNet )
{
	if( !pOwner )
		return;

	ioEntityGroup *pGrp = pOwner->GetGroup();
	if( !pGrp )
		return;

	pOwner->SetAttackMoveEnable( false );
	pOwner->SetChargingState( false );
	m_KungfuState = KMS_SLEEPATTACK;

	if( pOwner->IsNeedProcess() )
	{
		iDir = pOwner->CheckCurDirKey();
		if( iDir != ioUserKeyInput::DKI_NONE )
			pOwner->SetTargetRotToDirKey( (ioUserKeyInput::DirKeyInput)iDir );
	}

	DWORD dwCurTime = FRAMEGETTIME();

	m_szCurAni = m_SleepAttackAttribute.m_AttackAnimation;
	m_dwInputCancelCheckTime = dwCurTime;

	pOwner->SetAutoTarget( ATT_NORMAL );
	pOwner->SetNormalAttackByAttribute( m_SleepAttackAttribute, true );

	int iAniID = pGrp->GetAnimationIdx( m_SleepAttackAttribute.m_AttackAnimation );
	float fTimeRate = m_SleepAttackAttribute.m_fAttackAniRate;
	DWORD dwPreDelay = m_SleepAttackAttribute.m_dwPreDelay;

	pOwner->SetBaseAttackExtraAniJump( iAniID, fTimeRate, dwPreDelay );

	m_dwMotionEndTime = dwCurTime + ( pGrp->GetAnimationFullTime( iAniID ) * m_SleepAttackAttribute.m_fAttackAniRate );

	if( bSendNet && pOwner->IsNeedSendNetwork() )
	{
		SP2Packet kPacket( CUPK_ITEM_GAUGE_OK );
		FillItemGaugePacketHeader( pOwner, kPacket );
		kPacket << (int)m_KungfuState;
		kPacket << pOwner->GetWorldPosition();
		kPacket << pOwner->GetTargetRot();
		kPacket << iDir;
		P2PNetwork::SendToAllPlayingUser( kPacket );
	}
}

void ioKungfuMasterItem::SetSleepDashState( ioBaseChar *pOwner, int iDashDir, bool bSendNet )
{
	if( !pOwner )
		return;

	ioEntityGroup *pGrp = pOwner->GetGroup();
	if( !pGrp )
		return;

	pOwner->SetAttackMoveEnable( false );
	pOwner->SetChargingState( false );
	m_KungfuState = KMS_SLEEPDASH;

	pOwner->SetTargetRotToDirKey( (ioUserKeyInput::DirKeyInput)iDashDir );

	DWORD dwCurTime = FRAMEGETTIME();

	m_szCurAni = m_SleepDashAttribute.m_AttackAnimation;
	m_dwInputCancelCheckTime = dwCurTime;

	pOwner->SetAutoTarget( ATT_NORMAL );
	pOwner->SetNormalAttackByAttribute( m_SleepDashAttribute, true );

	int iAniID = pGrp->GetAnimationIdx( m_SleepDashAttribute.m_AttackAnimation );
	float fTimeRate = m_SleepDashAttribute.m_fAttackAniRate;
	DWORD dwPreDelay = m_SleepDashAttribute.m_dwPreDelay;

	pOwner->SetBaseAttackExtraAniJump( iAniID, fTimeRate, dwPreDelay );

	m_dwMotionEndTime = dwCurTime + ( pGrp->GetAnimationFullTime( iAniID ) * m_SleepDashAttribute.m_fAttackAniRate );

	if( bSendNet && pOwner->IsNeedSendNetwork() )
	{
		SP2Packet kPacket( CUPK_ITEM_GAUGE_OK );
		FillItemGaugePacketHeader( pOwner, kPacket );
		kPacket << (int)m_KungfuState;
		kPacket << pOwner->GetWorldPosition();
		kPacket << pOwner->GetTargetRot();
		kPacket << iDashDir;
		P2PNetwork::SendToAllPlayingUser( kPacket );
	}
}
//////////////////////////////////////////////////////////////////////////
ioKungfuSpecialState::ioKungfuSpecialState()
{
}

ioKungfuSpecialState::~ioKungfuSpecialState()
{
}

void ioKungfuSpecialState::Initialize( ioBaseChar* pOwner, CharState eCurrState )
{
	ioSpecialStateBase::Initialize( pOwner, eCurrState );
}

void ioKungfuSpecialState::Finalize( ioBaseChar* pOwner, CharState eCurrState, CharState eNewState )
{
	ioSpecialStateBase::Finalize( pOwner, eCurrState, eNewState );

	ioKungfuMasterItem *pKungfu = ToKungfuMasterItem( pOwner->GetPriorityItem( SPT_ATTACK ) );
	if( pKungfu )
		pKungfu->ClearAttackState( pOwner );
}

void ioKungfuSpecialState::CheckSpecialState( ioBaseChar* pOwner )
{
	ioKungfuMasterItem *pKungfu = ToKungfuMasterItem( pOwner->GetPriorityItem( SPT_ATTACK ) );
	if( pKungfu )
		pKungfu->ProcessSpecialState( pOwner );
	else
	{
		pOwner->SetState( CS_DELAY );
		pOwner->SetFallState( true );
		return;
	}
}

void ioKungfuSpecialState::CheckFallStateChange( ioBaseChar* pOwner, float fMinFallHeightRate, float fBottomHeight, bool bBottomMaxCheck, float fMaxBottomHeight )
{
}

bool ioKungfuSpecialState::IsEnableDashState( ioBaseChar* pOwner )
{
	ioKungfuMasterItem *pKungfu = ToKungfuMasterItem( pOwner->GetPriorityItem( SPT_ATTACK ) );
	if( pKungfu && pKungfu->IsSleep( pOwner ) )
		return true;

	return false;
}

bool ioKungfuSpecialState::IsCharCollisionSkipState( const ioBaseChar *const pOwner, DWORD dwStartTime, DWORD dwEndTime, DWORD dwSkipType, bool bTeamOnly, bool bDefense, TeamType eOwnerTeam, TeamType eOtherTeam ) const
{
	ioKungfuMasterItem *pKungfu = ToKungfuMasterItem( pOwner->GetPriorityItem( SPT_ATTACK ) );
	if( pKungfu && pKungfu->IsSleep( (ioBaseChar*)pOwner ) )
		return true;
	
	if( Help::CheckCharColState( dwStartTime, dwEndTime, dwSkipType, bTeamOnly, bDefense, eOwnerTeam, eOtherTeam ) )
		return true;

	return false;
}