#include "stdafx.h"

#include "ioDragonSlayerItem.h"

#include "ioPlayStage.h"
#include "ioBaseChar.h"

#include "ioStatAttribute.h"
#include "WeaponDefine.h"

ioDragonSlayerItem::ioDragonSlayerItem()
{
	Init();
}

ioDragonSlayerItem::ioDragonSlayerItem( const ioDragonSlayerItem &rhs )
	: ioWeaponItem( rhs ),
	m_fNeedGauge( rhs.m_fNeedGauge ),
	m_dwComoboChargeTime( rhs.m_dwComoboChargeTime ),
	m_vExtendAttackReadyAniList( rhs.m_vExtendAttackReadyAniList ),	
	m_vComboAttackList( rhs.m_vComboAttackList ),
	m_FearAttr( rhs.m_FearAttr ),
	m_BreathAttr( rhs.m_BreathAttr ),
	m_vChargDashAttackList( rhs.m_vChargDashAttackList ),
	m_bBreathGaugeUse( rhs.m_bBreathGaugeUse )
{
	Init();
}

ioDragonSlayerItem::~ioDragonSlayerItem()
{
}

void ioDragonSlayerItem::Init()
{
	m_ChargeState					 = CS_NONE;
	m_ChargeComoboState				 = CCS_NONE;

	m_iCurCombo						 = 0;	
	m_iChargeNoramlCombo			 = 0;
	m_iChargeDashCombo				 = 0;
	m_bEndAttack					 = false;

	m_dwComboAttackStartTime		 = 0;
	m_dwComboAttackEndTime			 = 0;

	m_dwComboChargeNormalKeyTime	 = 0;
	m_dwComboChargeBreathDownKeyTime = 0;

	m_dwAttackStartTime				 = 0;
	m_CurDirKey						 = ioUserKeyInput::DKI_NONE;
}

void ioDragonSlayerItem::LoadProperty( ioINILoader &rkLoader )
{
	ioWeaponItem::LoadProperty( rkLoader );

	char szBuf[MAX_PATH];
	m_fNeedGauge		 = rkLoader.LoadFloat_e( "need_extra_gauge", FLOAT1 );
	m_dwComoboChargeTime = rkLoader.LoadFloat_e( "comobo_charge_time", 0.0f );
	
	//특별 - 노멀
	int iExtendCnt = rkLoader.LoadInt_e( "charge_combo_max_cnt", 0 );
	m_vExtendAttackReadyAniList.clear();
	for( int i=0 ; i<iExtendCnt ; i++ )
	{
		wsprintf_e( szBuf, "charge_combo_ready_ani%d", i+1 );
		rkLoader.LoadString( szBuf, "", szBuf, MAX_PATH );
		m_vExtendAttackReadyAniList.push_back( ioHashString(szBuf) );
	}

	m_vComboAttackList.clear();
	m_vComboAttackList.reserve( iExtendCnt );
	for( int i=0 ; i<iExtendCnt ; i++ )
	{
		AttackAttribute kAttribute;
		wsprintf_e( szBuf, "charge_combo_attack%d", i+1 );
		LoadAttackAttribute( szBuf, kAttribute, rkLoader );
		m_vComboAttackList.push_back( kAttribute );
	}

	LoadAttackAttribute_e( "charge_fear_attack", m_FearAttr, rkLoader );

	LoadAttackAttribute_e( "charge_breath_attack", m_BreathAttr, rkLoader );
	m_bBreathGaugeUse = rkLoader.LoadBool_e( "charge_breath_attack_gauge_use", false );
	
	int iDashCnt = rkLoader.LoadInt_e( "charge_dash_max_cnt", 0 );
	m_vChargDashAttackList.clear();
	m_vChargDashAttackList.reserve( iDashCnt );
	for( int i=0 ; i<iDashCnt ; i++ )
	{
		AttackAttribute kAttribute;
		wsprintf_e( szBuf, "charge_dash%d", i+1 );
		LoadAttackAttribute( szBuf, kAttribute, rkLoader );
		m_vChargDashAttackList.push_back( kAttribute );
	}
}

ioItem* ioDragonSlayerItem::Clone()
{
	return new ioDragonSlayerItem( *this );
}

ioWeaponItem::WeaponSubType ioDragonSlayerItem::GetSubType() const
{
	return WST_DRAGON_SLAYER_ITEM;
}

void ioDragonSlayerItem::OnReleased( ioBaseChar *pOwner )
{	
	Init();
	ioAttackableItem::OnReleased( pOwner );
}

void ioDragonSlayerItem::SetNormalAttack( ioBaseChar *pOwner, int iCurCombo )
{
	Init();
	ClearCancelInfo();
	
	int iExtendCnt = m_vExtendAttackReadyAniList.size();
	if( !COMPARE( iCurCombo, 0, iExtendCnt ) )
	{
		ioWeaponItem::SetNormalAttack( pOwner, iCurCombo );

		m_ChargeState = CS_NONE;
		m_iCurCombo = iCurCombo;

		pOwner->SetAttackMoveEnable( false );
		pOwner->SetChargingState( false );
	}
	else
	{
		ioEntityGroup *pGrp = pOwner->GetGroup();
		pGrp->ClearAllLoopAni( FLOAT100, true );
		pGrp->ClearAllActionAni( FLOAT100, true );

		int iAniID = pGrp->GetAnimationIdx( m_vExtendAttackReadyAniList[iCurCombo] );
		float fTimeRate = (float)m_dwChargeTime / pGrp->GetAnimationFullTime( iAniID );
		pGrp->SetActionAni( iAniID, FLOAT100, FLOAT100, FLOAT1, FLOAT1/fTimeRate, 0.0f, 0.0f, true );

		DWORD dwTrackingTime = FRAMEGETTIME();
		dwTrackingTime += pGrp->GetAnimationEventTime_e( iAniID, "tracking_e" ) * fTimeRate;
		pOwner->SetAutoTargetTrackingEndTime( dwTrackingTime );

		m_ChargeState		= CS_CHARGING;
		m_dwAttackStartTime	= FRAMEGETTIME();
		m_iCurCombo			= iCurCombo;

		pOwner->SetState( CS_ATTACK );
		pOwner->SetAttackMoveEnable( false );
		pOwner->SetChargingState( false );
	}
}

void ioDragonSlayerItem::CheckNormalAttack( ioBaseChar *pOwner )
{
	DWORD dwCurTime = FRAMEGETTIME();

	switch( m_ChargeState )
	{
	case CS_ATTACK_FIRE:
		OnAttackFire( pOwner );
		break;
	case CS_CHARGING:
		OnCharging( pOwner );
		break;
	case CS_COMBO:
		OnCombo( pOwner );
		break;
	}
}

bool ioDragonSlayerItem::IsEnableExtendJump()
{
	if( GetCurBullet() < GetNeedBullet() )
		return false;

	return true;
}

void ioDragonSlayerItem::RecevieExtendJumpUse( ioBaseChar *pOwner )
{
	SetUseExtraGauge( GetNeedBullet() );
}

void ioDragonSlayerItem::OnAttackFire( ioBaseChar *pOwner )
{
	if( pOwner->IsNeedProcess() )
	{
		CheckCancelReserve( pOwner );
		if( ProcessCancel(pOwner) )
			return;
	}

	ioWeaponItem::CheckNormalAttack( pOwner );
}


void ioDragonSlayerItem::ChangeToAttackFire( ioBaseChar *pOwner )
{
	if( !pOwner ) return;

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
	m_ChargeState = CS_ATTACK_FIRE;

	if( pOwner->IsNeedSendNetwork() )
	{
		SP2Packet kPacket( CUPK_ITEM_GAUGE_OK );
		FillItemGaugePacketHeader( pOwner, kPacket );
		kPacket << static_cast<byte>( CS_ATTACK_FIRE );
		kPacket << m_iCurCombo;
		kPacket << pOwner->GetTargetRot();
		P2PNetwork::SendToAllPlayingUser( kPacket );
	}
}

void ioDragonSlayerItem::OnCharging( ioBaseChar *pOwner )
{
	if( !pOwner->IsNeedProcess() )
		return;

	if( pOwner->IsAttackKeyDown() && GetNeedBullet() <= GetCurBullet() )
	{
		if( m_dwAttackStartTime + m_dwChargeTime <= FRAMEGETTIME() )
		{
			ChangeToFearAttackState( pOwner );
		}
	}
	else
	{
		ChangeToAttackFire( pOwner );
	}
}

void ioDragonSlayerItem::SetActionAnimation( ioBaseChar *pOwner, const AttackAttribute& rkAttr )
{
	DWORD dwCurTime				= FRAMEGETTIME();
	m_szCurAni					= rkAttr.m_AttackAnimation;
	m_fCurAniRate				= rkAttr.m_fAttackAniRate;
	m_dwInputCancelCheckTime	= dwCurTime;
	m_dwCancelCheckTime			= dwCurTime;
		
	pOwner->SetNormalAttackByAttribute( rkAttr );

	ioEntityGroup *pGrp = pOwner->GetGroup();
	int iAniID	     = pGrp->GetAnimationIdx( rkAttr.m_AttackAnimation );
	float fTimeRate  = rkAttr.m_fAttackAniRate;
	DWORD dwPreDelay = rkAttr.m_dwPreDelay;

	pOwner->SetBaseAttackExtraAniJump( iAniID, fTimeRate, dwPreDelay );
	pOwner->SetExtendAttackEndJumpInfo( iAniID, fTimeRate, dwPreDelay );
	pOwner->CheckCharColSkipTime( iAniID, fTimeRate );

	m_dwComboAttackStartTime	= dwCurTime;
	m_dwComboAttackEndTime		= dwCurTime + pGrp->GetAnimationFullTime(iAniID) * fTimeRate + dwPreDelay;

	m_dwKeyReserveTime = pGrp->GetAnimationEventTime_e( iAniID, "key_reserve" ) * fTimeRate + dwPreDelay;
	if( m_dwKeyReserveTime > 0 )
	{
		m_dwKeyReserveTime += dwCurTime;
	}

	DWORD dwTrackingTime = dwCurTime + dwPreDelay;
	dwTrackingTime += pGrp->GetAnimationEventTime_e( iAniID, "tracking_e" ) * fTimeRate;
	pOwner->SetAutoTargetTrackingEndTime( dwTrackingTime );	
}

void ioDragonSlayerItem::ChangeToFearAttackState( ioBaseChar *pOwner )
{
	if( !pOwner )
		return;

	DWORD dwCurTime = FRAMEGETTIME();

	pOwner->InitExtendAttackTagInfo();
	pOwner->ClearDirDoubleClick();
	pOwner->ClearReserveKeyInput();

	Init();
	ClearCancelInfo();
	ClearKeyReserve();
	SetUseExtraGauge( GetNeedBullet() );

	pOwner->InitActionStopDelay( true );
	pOwner->SetAutoTarget( ATT_NORMAL );

	SetActionAnimation( pOwner, m_FearAttr );
	pOwner->ApplyNormalAttackResult( 0, true );

	m_ChargeState		= CS_COMBO;
	m_ChargeComoboState = CCS_FEAR;

	if( pOwner->IsNeedSendNetwork() )
	{
		SP2Packet kPacket( CUPK_ITEM_GAUGE_OK );
		FillItemGaugePacketHeader( pOwner, kPacket );
		kPacket << static_cast<byte>( CS_COMBO );		
		kPacket << static_cast<byte>( CCS_FEAR );
		kPacket << pOwner->GetTargetRot();
		P2PNetwork::SendToAllPlayingUser( kPacket );
	}
}

void ioDragonSlayerItem::CheckDirectionComboKey( ioBaseChar *pOwner )
{
	if( !COMPARE( m_iChargeDashCombo, 0, static_cast<int>( m_vChargDashAttackList.size() ) ) )
		return;

	DWORD dwCurTime = FRAMEGETTIME();
	if( m_dwKeyReserveTime > 0 && m_dwKeyReserveTime < dwCurTime && pOwner->IsNeedProcess() )
	{
		if( pOwner->IsDirKeyDoubleClick() && !pOwner->HasBuff( BT_DISABLE_DASH ) )
		{
			m_CurDirKey							= pOwner->CheckCurDirKey();
			m_dwComboChargeNormalKeyTime		= 0;
			m_dwComboChargeBreathDownKeyTime	= 0;
			m_dwComboAttackEndTime				= dwCurTime;
		}
	}
}

void ioDragonSlayerItem::CheckNormalComboKey( ioBaseChar *pOwner )
{	
	if( !COMPARE( m_iChargeNoramlCombo, 0, static_cast<int>( m_vComboAttackList.size() ) ) )
		return;

	DWORD dwCurTime = FRAMEGETTIME();
	if( m_dwKeyReserveTime > 0 && m_dwKeyReserveTime < dwCurTime && pOwner->IsNeedProcess() )
	{
		if( pOwner->IsAttackKey() )
		{	
			m_CurDirKey							= ioUserKeyInput::DKI_NONE;
			m_dwComboChargeNormalKeyTime		= dwCurTime;
			m_dwComboChargeBreathDownKeyTime	= 0;
			m_dwComboAttackEndTime				= dwCurTime;
		}
		CheckReserve( pOwner );
	}
}

void ioDragonSlayerItem::CheckBreathComboKey( ioBaseChar *pOwner )
{
	DWORD dwCurTime = FRAMEGETTIME();
	if( pOwner->IsNeedProcess() && pOwner->IsDefenseKey() && ( GetNeedBullet() <= GetCurBullet() || !m_bBreathGaugeUse ) )
	{
		m_CurDirKey							= ioUserKeyInput::DKI_NONE;
		m_dwComboChargeNormalKeyTime		= 0;
		m_dwComboChargeBreathDownKeyTime	= dwCurTime;
		m_dwComboAttackEndTime				= dwCurTime;
	}
}

void ioDragonSlayerItem::OnCombo( ioBaseChar *pOwner )
{
	DWORD dwCurTime = FRAMEGETTIME();
	pOwner->CheckExtraAniJump();

	if( COMPARE( dwCurTime, m_dwComboAttackStartTime, m_dwComboAttackEndTime ) )
	{
		if( !m_bEndAttack )
		{
			CheckDirectionComboKey( pOwner );
			CheckNormalComboKey( pOwner );
			CheckBreathComboKey( pOwner );
		}
	}
	else
	{
		bool bComoboAttack = false;
		if( COMPARE( m_iChargeNoramlCombo, 0, static_cast<int>( m_vComboAttackList.size() ) ) )
			bComoboAttack = true;

		bool bComoboDash = false;
		if( COMPARE( m_iChargeDashCombo, 0, static_cast<int>( m_vChargDashAttackList.size() ) ) )
			bComoboDash = true;

		if( !m_bEndAttack )
		{
			if( pOwner->IsNeedProcess() )
			{
				if( bComoboAttack && bComoboDash && m_CurDirKey != ioUserKeyInput::DKI_NONE )
				{
					ChangeToChargeDash( pOwner );
				}
				else if( bComoboAttack && 0 < m_dwComboChargeNormalKeyTime )
				{
					ChangeToComboAttack( pOwner );
				}
				else if( 0 < m_dwComboChargeBreathDownKeyTime )
				{
					ChangeToBreathAttackState( pOwner );
				}
			}
		}
		else
		{
			ProcessReserveKeyInput( pOwner );
		}		
	}

	ioAttackableItem::CheckNormalAttack( pOwner );
}

void ioDragonSlayerItem::ChangeToComboAttack( ioBaseChar *pOwner )
{
	DWORD dwCurTime = FRAMEGETTIME();
	pOwner->InitExtendAttackTagInfo();
	pOwner->ClearReservedSliding();
	pOwner->ClearDirDoubleClick();
	pOwner->ClearReserveKeyInput();

	ClearCancelInfo();
	ClearKeyReserve();
	
	m_CurDirKey = pOwner->CheckCurDirKey();
	if( m_CurDirKey != ioUserKeyInput::DKI_NONE )
	{
		pOwner->SetTargetRotToDirKey( m_CurDirKey );		
	}

	m_ChargeState					 = CS_COMBO;
	m_ChargeComoboState				 = CCS_NORMAL_ATTACK;
	m_dwComboChargeNormalKeyTime	 = 0;
	m_dwComboChargeBreathDownKeyTime = 0;
	m_CurDirKey = ioUserKeyInput::DKI_NONE;

	pOwner->InitActionStopDelay( true );
	pOwner->SetAutoTarget( ATT_NORMAL );

	SetActionAnimation( pOwner, m_vComboAttackList[m_iChargeNoramlCombo] );
	pOwner->ApplyNormalAttackResult( m_iChargeNoramlCombo, true );

	if( pOwner->IsNeedSendNetwork() )
	{
		SP2Packet kPacket( CUPK_ITEM_GAUGE_OK );
		FillItemGaugePacketHeader( pOwner, kPacket );
		kPacket << static_cast<byte>( CS_COMBO );		
		kPacket << static_cast<byte>( CCS_NORMAL_ATTACK );		
		kPacket << static_cast<int>( m_CurDirKey );
		kPacket << pOwner->GetTargetRot();
		kPacket << m_iChargeNoramlCombo;
		P2PNetwork::SendToAllPlayingUser( kPacket );
	}
	m_iChargeNoramlCombo++;
	
}

void ioDragonSlayerItem::ChangeToBreathAttackState( ioBaseChar *pOwner )
{
	if( !pOwner )
		return;

	DWORD dwCurTime = FRAMEGETTIME();
	pOwner->InitExtendAttackTagInfo();
	pOwner->ClearReservedSliding();
	pOwner->ClearDirDoubleClick();
	pOwner->ClearReserveKeyInput();

	ClearCancelInfo();
	ClearKeyReserve();

	if( m_bBreathGaugeUse )
		SetUseExtraGauge( GetNeedBullet() );

	pOwner->InitActionStopDelay( true );
	pOwner->SetAutoTarget( ATT_NORMAL );

	SetActionAnimation( pOwner, m_BreathAttr );
	pOwner->ApplyNormalAttackResult( m_iChargeNoramlCombo, true );

	m_CurDirKey				= ioUserKeyInput::DKI_NONE;
	m_ChargeState			= CS_COMBO;
	m_ChargeComoboState		= CCS_BREATH;
	m_bEndAttack			= true;

	if( pOwner->IsNeedSendNetwork() )
	{
		SP2Packet kPacket( CUPK_ITEM_GAUGE_OK );
		FillItemGaugePacketHeader( pOwner, kPacket );
		kPacket << static_cast<byte>( CS_COMBO );		
		kPacket << static_cast<byte>( CCS_BREATH );
		kPacket << pOwner->GetTargetRot();
		kPacket << m_iChargeNoramlCombo;
		P2PNetwork::SendToAllPlayingUser( kPacket );
	}
	m_iChargeNoramlCombo++;
}

void ioDragonSlayerItem::ChangeToChargeDash( ioBaseChar* pOwner, float fExtraForceRate )
{
	if( !pOwner )
		return;

	DWORD dwCurTime = FRAMEGETTIME();
	pOwner->InitExtendAttackTagInfo();
	pOwner->ClearDirDoubleClick();
	pOwner->ClearReserveKeyInput();

	ClearCancelInfo();
	ClearKeyReserve();

	pOwner->InitActionStopDelay( true );
	pOwner->SetTargetRotToDirKey( m_CurDirKey );
	pOwner->ClearReservedSliding();
	pOwner->SetForcePowerAmt( 0.f );

	SetActionAnimation( pOwner, m_vChargDashAttackList[m_iChargeDashCombo] );
	pOwner->ApplyNormalAttackResult( m_iChargeDashCombo, true );

	if( pOwner->IsNeedSendNetwork() )
	{
		SP2Packet kPacket( CUPK_ITEM_GAUGE_OK );
		FillItemGaugePacketHeader( pOwner, kPacket );
		kPacket << static_cast<byte>( CS_COMBO );		
		kPacket << static_cast<byte>( CCS_DASH );
		kPacket << m_iChargeDashCombo;
		kPacket << static_cast<int>( m_CurDirKey );
		kPacket << pOwner->GetTargetRot();

		P2PNetwork::SendToAllPlayingUser( kPacket );
	}	

	m_CurDirKey						 = ioUserKeyInput::DKI_NONE;
	m_ChargeState					 = CS_COMBO;
	m_ChargeComoboState				 = CCS_DASH;
	m_dwComboChargeNormalKeyTime	 = 0;
	m_dwComboChargeBreathDownKeyTime = 0;

	m_iChargeDashCombo++;
}

void ioDragonSlayerItem::OnItemGaugeOK( ioBaseChar *pOwner, SP2Packet &rkPacket )
{
	if( !pOwner )
		return;

	byte eState;
	rkPacket >> eState;

	switch( eState )
	{
	case CS_ATTACK_FIRE:
		{
			D3DXQUATERNION qtRot;
			rkPacket >> m_iCurCombo;
			rkPacket >> qtRot;

			pOwner->SetTargetRot( qtRot );
			ChangeToAttackFire( pOwner );
		}
		break;
	case CS_COMBO:
		{
			byte eChargeComoboState;
			rkPacket >> eChargeComoboState;

			switch( eChargeComoboState )
			{
			case CCS_FEAR:
				{
					D3DXQUATERNION qtRot;
					rkPacket >> qtRot;
					pOwner->SetTargetRot( qtRot );

					ChangeToFearAttackState( pOwner );
				}
				break;
			case CCS_DASH:
				{					
					
					int eCurrKeyInput = 0;
					rkPacket >> m_iChargeDashCombo;
					rkPacket >> eCurrKeyInput;
					m_CurDirKey = static_cast<DirKey>( eCurrKeyInput );

					D3DXQUATERNION qtRot;
					rkPacket >> qtRot;
					pOwner->SetTargetRot( qtRot );

					ChangeToChargeDash( pOwner );
				}
				break;
			case CCS_NORMAL_ATTACK:
				{
					int eCurrKeyInput = 0;

					D3DXQUATERNION qtRot;
					rkPacket >> eCurrKeyInput;
					rkPacket >> qtRot;
					
					m_CurDirKey = static_cast<DirKey>( eCurrKeyInput );
					pOwner->SetTargetRot( qtRot );					

					rkPacket >> m_iChargeNoramlCombo;
					ChangeToComboAttack( pOwner );
				}
				break;
			case CCS_BREATH:
				{
					D3DXQUATERNION qtRot;
					rkPacket >> qtRot;
					pOwner->SetTargetRot( qtRot );
					 
					rkPacket >> m_iChargeNoramlCombo;
					ChangeToBreathAttackState( pOwner );
				}
				break;
			}
		}
		break;
	}	
}

bool ioDragonSlayerItem::IsChargeAttackState( ioBaseChar *pOwner )
{
	if( pOwner->GetState() == CS_ATTACK )
	{
		if( m_ChargeState == CS_CHARGING )
			return true;
	}

	return false;
}

bool ioDragonSlayerItem::IsEnableAttackDashCheck( ioBaseChar *pOwner, bool bCheck )
{
	if( pOwner->GetState() == CS_ATTACK )
	{
		switch( m_ChargeState )
		{
		case CS_ATTACK_FIRE:
			{
				if( !bCheck )
					return false;

				return true;
			}
		case CS_COMBO:
			return true;
		}
	}
	return false;
}

void ioDragonSlayerItem::UpdateExtraData( ioBaseChar *pOwner )
{
	if( !pOwner )
		return;
	
	float fTimePerSec = g_FrameTimer.GetSecPerFrame();
	if( m_fExtraGaugeBaseInc <= 0.0f )
		return;
	
	if ( pOwner->GetState() == CS_JUMP )	
		return;	

	if( pOwner->IsSystemState() )
		return;

	switch( pOwner->GetState() )
	{
	case CS_DELAY:
	case CS_DASH:
	case CS_RUN:
	case CS_JUMP:
		{
			float fGauge = m_fExtraGaugeBaseInc * fTimePerSec;
			m_fCurExtraGauge += fGauge;
			m_fCurExtraGauge = min( m_fCurExtraGauge, m_fMaxExtraGauge );
		}
	}
}

int ioDragonSlayerItem::GetMaxBullet()
{
	return (int)m_fMaxExtraGauge;
}

int ioDragonSlayerItem::GetNeedBullet()
{
	return (int)m_fNeedGauge;
}

int ioDragonSlayerItem::GetCurBullet()
{
	float fCurCoolTime = m_fCurExtraGauge;
	fCurCoolTime = max( 0, min( fCurCoolTime, m_fMaxExtraGauge ) );

	return (int)fCurCoolTime;
}

void ioDragonSlayerItem::SetCurBullet( int iCurBullet )
{
	if( iCurBullet < 0 )
		return;

	float fCurBullet = iCurBullet;
	fCurBullet = max( 0, min( fCurBullet, m_fMaxExtraGauge ) );

	m_fCurExtraGauge = fCurBullet;
}

void ioDragonSlayerItem::SetUseExtraGauge( float fAmt )
{
	m_fCurExtraGauge -= fAmt;
	m_fCurExtraGauge = max( 0.0f, m_fCurExtraGauge );
}

void ioDragonSlayerItem::SetExtraInfoToWeapon( ioWeapon *pWeapon, bool bJumpAttack )
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

bool ioDragonSlayerItem::IsCanSendControl() const
{
	if( m_ChargeState == CS_COMBO && m_ChargeComoboState == CCS_DASH )
		return false;

	return true;
}