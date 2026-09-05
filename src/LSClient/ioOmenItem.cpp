#include "StdAfx.h"
#include "ioOmenItem.h"


ioOmenItem::ioOmenItem(void)
{
	Init();
}

ioOmenItem::ioOmenItem( const ioOmenItem& rhs ) : ioWeaponItem( rhs ),
m_BranchDelayAnimation( rhs.m_BranchDelayAnimation ),
m_iMaxBullet( rhs.m_iMaxBullet ), 
m_iNeedBulletDash( rhs.m_iNeedBulletDash ),
m_iNeedBulletCharge( rhs.m_iNeedBulletCharge ),
m_iNeedBulletDashFire( rhs.m_iNeedBulletDashFire ),
m_vChargeBuff( rhs.m_vChargeBuff ),
m_ChargeAttackStart( rhs.m_ChargeAttackStart ),
m_ChargeAttackInfo( rhs.m_ChargeAttackInfo ),
m_vChargeAttackOffset( rhs.m_vChargeAttackOffset ),
m_ChargeAttackSoundName( rhs.m_ChargeAttackSoundName ),
m_szChargeAttackEndAni( rhs.m_szChargeAttackEndAni ),
m_fChargeAttackEndAniRate( rhs.m_fChargeAttackEndAniRate ),
m_szChargeAttackLoopAni( rhs.m_szChargeAttackLoopAni ),
m_dwFireGapTime( rhs.m_dwFireGapTime ),
m_iChargeAttackCount( rhs.m_iChargeAttackCount ),
m_ChargeDelayAnimation( rhs.m_ChargeDelayAnimation ),
m_fExtraGaugeAttackIncRate( rhs.m_fExtraGaugeAttackIncRate ),
m_fExtraGaugeDelayIncRate( rhs.m_fExtraGaugeDelayIncRate ),
m_fExtraGaugeWoundedIncRate( rhs.m_fExtraGaugeWoundedIncRate )
//m_szChargeEffect( rhs.m_szChargeEffect ),
//m_dwChargeBuffMaintainTime( rhs.m_dwChargeBuffMaintainTime )
{
	Init();
}

ioOmenItem::~ioOmenItem(void)
{
}

void	ioOmenItem::LoadProperty( ioINILoader &rkLoader )
{
	ioWeaponItem::LoadProperty( rkLoader );

	char	szBuf[MAX_PATH]	= "";
	char	szBufName[MAX_PATH]	= "";

	rkLoader.LoadString_e( "branch_delay_animation", "", szBuf, MAX_PATH );
	m_BranchDelayAnimation	= szBuf;
	rkLoader.LoadString_e( "charge_delay_animation", "", szBuf, MAX_PATH );
	m_ChargeDelayAnimation	= szBuf;

	int	iCount	= rkLoader.LoadInt_e( "charge_buff_count", 0 );
	for( int i = 0; i < iCount; i++ )
	{
		sprintf_s( szBufName, "charge_buff%d", i + 1 );
		rkLoader.LoadString( szBufName, "", szBuf, MAX_PATH );
		m_vChargeBuff.push_back( szBuf );
	}

	m_iMaxBullet	= rkLoader.LoadInt_e( "max_bullet", 0 );
	m_iNeedBulletDash	= rkLoader.LoadInt_e( "need_bullet_dash", 0 );
	m_iNeedBulletDashFire	= rkLoader.LoadInt_e( "need_bullet_dash_fire", 0 );
	m_iNeedBulletCharge	= rkLoader.LoadInt_e( "need_bullet_charge", 0 );

	LoadAttackAttribute( "charge_effect_attack_start", m_ChargeAttackStart, rkLoader );
	m_iChargeAttackCount	= rkLoader.LoadInt_e( "cahrge_effect_attack_count", 0 );

	D3DXVECTOR3	vOffset;
	WeaponInfo	attackInfo;
	for( int i = 0; i < m_iChargeAttackCount; i++ )
	{
		sprintf_s( szBuf, "charge_effect_attack_offset%d_x", i + 1 );
		vOffset.x	= rkLoader.LoadFloat( szBuf, 0.0f );
		sprintf_s( szBuf, "charge_effect_attack_offset%d_y", i + 1 );
		vOffset.y	= rkLoader.LoadFloat( szBuf, 0.0f );
		sprintf_s( szBuf, "charge_effect_attack_offset%d_z", i + 1 );
		vOffset.z	= rkLoader.LoadFloat( szBuf, 0.0f );

		m_vChargeAttackOffset.push_back( vOffset );

		sprintf_s( szBuf, "charge_effect_attack%d", i + 1 );
		LoadWeaponInfo( szBuf, attackInfo, rkLoader );

		m_ChargeAttackInfo.push_back( attackInfo );
	}

	rkLoader.LoadString_e( "charge_effect_attack_sound", "", szBuf, MAX_PATH );
	m_ChargeAttackSoundName = szBuf;
	rkLoader.LoadString_e( "charge_effect_attack_end_ani", "", szBuf, MAX_PATH );
	m_szChargeAttackEndAni = szBuf;
	m_fChargeAttackEndAniRate = rkLoader.LoadFloat_e( "charge_effect_attack_end_ani_rate", FLOAT1 );
	rkLoader.LoadString_e( "charge_effect_attack_loop_ani", "", szBuf, MAX_PATH );
	m_szChargeAttackLoopAni = szBuf;
	m_dwFireGapTime	= rkLoader.LoadInt_e( "fire_gap_time", 0 );

	m_fExtraGaugeAttackIncRate = rkLoader.LoadFloat_e( "extra_gauge_attack_inc_rate", FLOAT1 );
	m_fExtraGaugeDelayIncRate = rkLoader.LoadFloat_e( "extra_gauge_delay_inc_rate", FLOAT1 );
	m_fExtraGaugeWoundedIncRate = rkLoader.LoadFloat_e( "extra_gauge_wounded_inc_rate", FLOAT1 );
}

ioItem*	ioOmenItem::Clone()
{
	return	new	ioOmenItem( *this );
}

ioWeaponItem::WeaponSubType	ioOmenItem::GetSubType() const
{
	return WST_OMEN_ITEM;
}

int		ioOmenItem::GetCurBullet()
{
	return	m_iCurBullet;
}

int		ioOmenItem::GetMaxBullet()
{
	return	m_iMaxBullet;
}

int		ioOmenItem::GetNeedBullet()
{
	if( m_OmenState == OS_DASH_CHARGE )
		return	m_iNeedBulletDash;

	return	m_iNeedBulletCharge;
}

void	ioOmenItem::SetCurBullet( int iCurBullet )
{
	m_iCurBullet	= iCurBullet;
	m_fCurBullet	= (float)iCurBullet;
	m_fCurBullet	= max( m_fCurBullet, 0 );
	//m_fTempBullet	= (float)iCurBullet;
}

void	ioOmenItem::SetCurExtraGauge( float fGauge )
{
	m_fCurExtraGauge	= fGauge;
	m_fCurExtraGauge	= min( m_fMaxExtraGauge, m_fCurExtraGauge );
}

float	ioOmenItem::GetCurExtraGauge()
{
	return	m_fCurExtraGauge;
}

float	ioOmenItem::GetNeedExtraGauge()
{
	return	0.0f;
}

int		ioOmenItem::GetNeedBulletDashChargeAttack()
{
	return	m_iNeedBulletDash;
}

void	ioOmenItem::ChangeToDashChargeAttack( ioBaseChar* pOwner )
{
	if ( !pOwner )
		return;

	ioEntityGroup *pGrp = pOwner->GetGroup();
	if( !pGrp )
		return;

	if( pOwner->GetState() != CS_ATTACK )
	{
		pOwner->SetState( CS_ATTACK );
		pOwner->SetCurNormalAttackItem( this );
	}
	if ( pOwner->IsNeedProcess() )
	{
		if ( pOwner->IsSettedDirection() )
			pOwner->SetTargetRotToDirKey( pOwner->CheckCurDirKey() );
	}

	int iAniID = pGrp->GetAnimationIdx( m_ChargeAttackStart.m_AttackAnimation );
	if ( iAniID == -1 )
	{
		LOG.PrintTimeAndLog( 0, "%s - ani Name is not exist", __FUNCTION__ );
		return;
	}

	DWORD dwPreDelay = m_ChargeAttackStart.m_dwPreDelay;
	float fTimeRate = m_ChargeAttackStart.m_fAttackAniRate;
	if( fTimeRate <= 0.0f )
		fTimeRate = FLOAT1;

	pGrp->ClearAllActionAni( 0.0f, true );
	pGrp->ClearAllLoopAni( 0.0f, true );
	pGrp->SetActionAni( iAniID, FLOAT100, FLOAT100, FLOAT1, FLOAT1/fTimeRate );

	pOwner->RefreshFireTimeList( iAniID, m_ChargeAttackStart.m_vWeaponInfoList, FTT_NORMAL_ATTACK, fTimeRate, dwPreDelay );
	pOwner->IncreaseWeaponIndexBase();
	pOwner->SetReservedSliding( m_ChargeAttackStart.m_vForceInfoList, iAniID, fTimeRate, 0 );
	pOwner->CheckCharColSkipTime( iAniID, fTimeRate, dwPreDelay );

	m_vAttackDir = pOwner->GetTargetRot() * D3DXVECTOR3( 0, 0, -1 );
	D3DXVec3Normalize( &m_vAttackDir, &m_vAttackDir );

	m_dwMotionEndTime = FRAMEGETTIME() + pGrp->GetAnimationFullTime(iAniID) * fTimeRate;
	m_OmenState		= OS_DASH_CHARGE;

	if( pOwner->IsNeedSendNetwork() )
	{
		SP2Packet kPacket( CUPK_ITEM_GAUGE_OK );
		FillItemGaugePacketHeader( pOwner, kPacket );
		kPacket << (int)m_OmenState;
		kPacket << m_iCurCombo;
		kPacket << pOwner->GetWorldPosition();
		kPacket << pOwner->GetTargetRot();
		P2PNetwork::SendToAllPlayingUser( kPacket );
	}
}

void	ioOmenItem::DecreaseBullet( int iBullet )
{
	m_fCurBullet	-= iBullet;
	m_fCurBullet	= max( m_fCurBullet, 0.0f );
}

void	ioOmenItem::Init()
{
	m_dwChargeStartTime	= 0;
	m_iCurCombo	= 0;
	m_iCurBullet	= 0;
	m_fCurBullet	= 0.0f;
	m_dwChargeEffectIndex	= -1;
	m_dwChargeBuffStartTime	= 0;
	m_dwFireTime	= 0;
	m_vAttackDir	= ioMath::VEC3_ZERO;
}

void	ioOmenItem::SetNormalAttack( ioBaseChar* pOwner, int iCurCombo )
{
	ClearCancelInfo();
	ClearKeyReserve();

	int iMaxCombo = GetMaxCombo();
	if( COMPARE( iCurCombo, 0, iMaxCombo ) )
	{
		m_OmenState = OS_CHARGE;
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

void	ioOmenItem::CheckNormalAttack( ioBaseChar* pOwner )
{
	switch( m_OmenState )
	{
	case	OS_NORMAL_ATTACK:
		{
			if( pOwner->IsNeedProcess() )
			{
				CheckCancelReserve( pOwner );

				if( ProcessCancel(pOwner) )
					return;
			}
			ioAttackableItem::CheckNormalAttack( pOwner );
		}
		break;

	case	OS_DASH_CHARGE:
		{
			OnDashChargeAttack( pOwner );
		}
		break;

	case	OS_DASH_CHARGE_LOOP:
		{
			OnDashChargeAttackLoop( pOwner );
		}
		break;

	case	OS_DASH_CHARGE_END:
		{
			OnDashChargeAttackEnd( pOwner );
		}
		break;

	case	OS_CHARGE:
		{
			OnCharge( pOwner );
		}
		break;

	case	OS_CHARGE_BUFF:
		{
			OnChargeBuff( pOwner );
		}
		break;
	}
}

void	ioOmenItem::OnItemGaugeOK( ioBaseChar* pOwner, SP2Packet& rkPacket )
{
	int	iState;
	rkPacket >> iState;
	rkPacket >> m_iCurCombo;

	m_OmenState	= (OmenState)iState;

	switch( m_OmenState )
	{
	case	OS_NONE:
		{
			ioEntityGroup *pGrp = pOwner->GetGroup();
			if( pGrp )
			{
				pGrp->ClearAllLoopAni( FLOAT100, true );
				pGrp->ClearAllActionAni( FLOAT100, true );
			}

			pOwner->SetState( CS_DELAY );
		}
		break;

	case	OS_NORMAL_ATTACK:
		{
			ChangeToNormalAttack( pOwner );
		}
		break;

	case	OS_DASH_CHARGE:
		{
			D3DXVECTOR3	vPos;
			D3DXQUATERNION	vDir;
			rkPacket >> vPos >> vDir;

			pOwner->SetWorldPosition( vPos );
			pOwner->SetWorldOrientation( vDir );

			ChangeToDashChargeAttack( pOwner );
		}
		break;

	case	OS_DASH_CHARGE_LOOP:
		{
			ChangeToDashChargeAttackLoop( pOwner );
		}
		break;

	case	OS_DASH_CHARGE_END:
		{
			ChangeToDashChargeAttackEnd( pOwner );
		}
		break;

	case	OS_CHARGE_BUFF:
		{
			ChangeToChargeBuff( pOwner );
		}
	}
}

void	ioOmenItem::OnItemGaugeCancel( ioBaseChar* pOwner, SP2Packet& rkPacket )
{

}

void	ioOmenItem::UpdateExtraData( ioBaseChar *pOwner )
{
	if( !pOwner ) 
		return;

	ioPlayStage* pStage = pOwner->GetCreator();
	if ( !pStage )
		return;

	float fTimePerSec = g_FrameTimer.GetSecPerFrame();

	ioMapEffect *pEff = pStage->FindMapEffect( m_dwChargeEffectIndex );
	if( pEff )
	{
		pEff->SetWorldPosition( pOwner->GetWorldPosition() );
	}

	CharState	state	= pOwner->GetState();
	float fGauge	= 0.0f;
	switch( state )
	{
	case	CS_DELAY:
	case	CS_RUN:
	case	CS_DASH:
		{
			fGauge	= m_fExtraGaugeDelayIncRate * fTimePerSec;
		}
		break;

	case	CS_ATTACK:
	case	CS_DEFENSE:
		{
			fGauge	= m_fExtraGaugeAttackIncRate * fTimePerSec;
		}
		break;

	case	CS_WOUNDED:
		{
			fGauge	= m_fExtraGaugeWoundedIncRate * fTimePerSec;
		}
		break;
	}

	//float fGauge = m_fExtraGaugeBaseInc * fTimePerSec;
	m_fCurBullet += fGauge;
	m_fCurBullet = min( m_fCurBullet, m_iMaxBullet );
	m_iCurBullet	= (int)m_fCurBullet;
	m_iCurBullet	= max( m_iCurBullet, 0 );
}

void	ioOmenItem::SetExtraGaugeTypeByWeapon( ioBaseChar *pOwner, int iType, int iWGT, float fDamage )
{
	if( !pOwner )
		return;

	float fTypeRate = 0.0f;
	switch( iType )
	{
	case EGT_ATTACK:
		{
			fTypeRate = m_fExtraGaugeAttack;
		}
		break;
	case EGT_ATTACK_DEF:
		{
			fTypeRate = m_fExtraGaugeAttackDef;
		}
		break;
	case EGT_WOUNDED:
		{
			fTypeRate = m_fExtraGaugeWound;
		}
		break;
	case EGT_WOUNDED_DEF:
		{
			fTypeRate = m_fExtraGaugeWoundDef;
		}
		break;
	}

	float fCurGauge = fTypeRate * fDamage;
	if( fTypeRate <= 0.0f )
		return;

	m_fCurBullet += fCurGauge;
	m_fCurBullet = min( m_fCurBullet, m_iMaxBullet );
	m_iCurBullet	= (int)m_fCurBullet;
	m_iCurBullet	= max( m_iCurBullet, 0 );
}

void	ioOmenItem::OnCharge( ioBaseChar* pOwner )
{
	if( !pOwner )
		return;

	if( !pOwner->IsNeedProcess() )	
		return;

	if( m_dwChargeStartTime == 0 )
		return;

	DWORD dwCurTime = FRAMEGETTIME();
	if( pOwner->IsAttackKeyDown() && m_iCurBullet >= m_iNeedBulletCharge )
	{
		if( m_dwChargeStartTime + m_dwChargeTime <= dwCurTime )
		{
			ChangeToChargeBuff( pOwner );
		}
	}
	else
	{
		pOwner->SetChargingState( false );
		pOwner->SetAttackMoveEnable( false );
		ChangeToNormalAttack( pOwner );
	}
}

void	ioOmenItem::OnChargeBuff( ioBaseChar* pOwner )
{
	if( !pOwner )
		return;

	if( !pOwner->IsNeedProcess() )	
		return;

	if( m_dwChargeBuffStartTime < FRAMEGETTIME() )
	{
		pOwner->SetState( CS_DELAY );
		m_OmenState	= OS_NONE;
	}
}

void	ioOmenItem::OnDashChargeAttack( ioBaseChar* pOwner )
{
	if ( !pOwner || !pOwner->IsNeedProcess() )
		return;

	if ( m_dwMotionEndTime && m_dwMotionEndTime < FRAMEGETTIME() )
	{
		m_dwMotionEndTime = 0;
		ChangeToDashChargeAttackLoop( pOwner );
		return;
	}
}

void	ioOmenItem::OnDashChargeAttackLoop( ioBaseChar* pOwner )
{
	if( !pOwner )
		return;

	if( pOwner->IsNeedProcess() && ( m_iCurBullet < m_iNeedBulletDashFire || !pOwner->IsAttackKeyDown() ) )
	{
		ChangeToDashChargeAttackEnd( pOwner );
		//pOwner->SetState( CS_DELAY );
		return;
	}

	//m_iCurBullet--;
	//m_iCurBullet	= max( 0, m_iCurBullet );

	if( m_dwFireTime + m_dwFireGapTime < FRAMEGETTIME() )
	{
		FireWeapon( pOwner );
		m_dwFireTime	= FRAMEGETTIME();
		SetCurBullet( m_iCurBullet - m_iNeedBulletDashFire );
	}
}

void	ioOmenItem::OnDashChargeAttackEnd( ioBaseChar* pOwner )
{
	if ( !pOwner || !pOwner->IsNeedProcess() )
		return;

	if ( m_dwMotionEndTime && m_dwMotionEndTime < FRAMEGETTIME() )
	{
		m_dwMotionEndTime	= 0;
		pOwner->SetState( CS_DELAY );
		m_OmenState	= OS_NONE;
	}

	if( pOwner->IsNeedSendNetwork() )
	{
		SP2Packet kPacket( CUPK_ITEM_GAUGE_OK );
		FillItemGaugePacketHeader( pOwner, kPacket );
		kPacket << (int)m_OmenState;
		kPacket << m_iCurCombo;
		P2PNetwork::SendToAllPlayingUser( kPacket );
	}
}

void	ioOmenItem::ChangeToNormalAttack( ioBaseChar* pOwner )
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
	m_OmenState = OS_NORMAL_ATTACK;

	if( pOwner->IsNeedSendNetwork() )
	{
		SP2Packet kPacket( CUPK_ITEM_GAUGE_OK );
		FillItemGaugePacketHeader( pOwner, kPacket );
		kPacket << (int)m_OmenState;
		kPacket << m_iCurCombo;
		P2PNetwork::SendToAllPlayingUser( kPacket );
	}
}

void	ioOmenItem::ChangeToChargeBuff( ioBaseChar* pOwner )
{
	if( !pOwner )
		return;

	ioEntityGroup *pGrp = pOwner->GetGroup();
	if( !pGrp )
		return;

	m_OmenState	= OS_CHARGE_BUFF;
	/*pOwner->SetChargingState( true );
	pOwner->SetAttackMoveEnable( true );
	pOwner->ChangeDirectionExplicitIfDirChanged( true );*/
	//pOwner->SetState( CS_DELAY );
	m_dwChargeBuffStartTime	= FRAMEGETTIME();

	int iAniID = pGrp->GetAnimationIdx( m_ChargeDelayAnimation );
	//float fTimeRate = (float)m_dwChargeTime / pGrp->GetAnimationFullTime( iAniID );
	pGrp->SetActionAni( iAniID, FLOAT100, FLOAT100, FLOAT1, FLOAT1, 0.0f, 0.0f, true );
	m_dwChargeBuffStartTime	+= pGrp->GetAnimationFullTime( iAniID );

	for( int i = 0; i < (int)m_vChargeBuff.size(); i++ )
	{
		pOwner->AddNewBuff( m_vChargeBuff[i], pOwner->GetCharName(), GetName(), NULL );
	}

	//m_iCurBullet	-= m_iNeedBulletCharge;
	//m_iCurBullet	= max( 0, m_iCurBullet );
	SetCurBullet( m_iCurBullet - m_iNeedBulletCharge );

	//m_dwChargeEffectIndex	= AddEffect( m_szChargeEffect, pOwner->GetWorldPosition(), pOwner );

	if( pOwner->IsNeedSendNetwork() )
	{
		SP2Packet kPacket( CUPK_ITEM_GAUGE_OK );
		FillItemGaugePacketHeader( pOwner, kPacket );
		kPacket << (int)m_OmenState;
		kPacket << m_iCurCombo;
		P2PNetwork::SendToAllPlayingUser( kPacket );
	}
}

void	ioOmenItem::ChangeToDashChargeAttackLoop( ioBaseChar* pOwner )
{
	if( !pOwner )
		return;

	ioEntityGroup *pGrp = pOwner->GetGroup();
	if ( !pGrp )
		return;

	m_OmenState	= OS_DASH_CHARGE_LOOP;
	m_dwFireTime	= FRAMEGETTIME();
	//m_iCurBullet	-= m_iNeedBulletDash;
	//m_iCurBullet	= max( 0, m_iCurBullet );
	SetCurBullet( m_iCurBullet - m_iNeedBulletDash );

	FireWeapon( pOwner );

	pGrp->ClearAllActionAni( FLOAT100, true );
	pGrp->ClearAllLoopAni( FLOAT100, true );

	pGrp->SetLoopAni( m_szChargeAttackLoopAni, 0.0f, FLOAT1 );

	if( pOwner->IsNeedSendNetwork() )
	{
		SP2Packet kPacket( CUPK_ITEM_GAUGE_OK );
		FillItemGaugePacketHeader( pOwner, kPacket );
		kPacket << (int)m_OmenState;
		kPacket << m_iCurCombo;
		P2PNetwork::SendToAllPlayingUser( kPacket );
	}
}

void	ioOmenItem::ChangeToDashChargeAttackEnd( ioBaseChar* pOwner )
{
	if( !pOwner )
		return;

	ioEntityGroup *pGrp = pOwner->GetGroup();
	if( !pGrp )
		return;

	int iAniID = pGrp->GetAnimationIdx( m_szChargeAttackEndAni );
	if ( iAniID == -1 )
	{
		LOG.PrintTimeAndLog( 0, "%s - ani Name is not exist", __FUNCTION__ );
		return;
	}
	if( pOwner->IsAttackMoveEnable() )
		pOwner->SetAttackMoveEnable( false );

	float fTimeRate = m_fChargeAttackEndAniRate;
	if( fTimeRate <= 0.0f )
		fTimeRate = FLOAT1;

	pGrp->ClearAllActionAni( 0.0f, true );
	pGrp->ClearAllLoopAni( 0.0f, true );
	pGrp->SetActionAni( iAniID, FLOAT100, FLOAT100, FLOAT1, FLOAT1/fTimeRate );

	m_dwMotionEndTime = FRAMEGETTIME() + pGrp->GetAnimationFullTime(iAniID) * fTimeRate;
	m_OmenState = OS_DASH_CHARGE_END;

	if( pOwner->IsNeedSendNetwork() )
	{
		SP2Packet kPacket( CUPK_ITEM_GAUGE_OK );
		FillItemGaugePacketHeader( pOwner, kPacket );
		kPacket << (int)m_OmenState;
		kPacket << m_iCurCombo;
		kPacket << pOwner->GetWorldPosition();
		kPacket << pOwner->GetTargetRot();
		P2PNetwork::SendToAllPlayingUser( kPacket );
	}
}

void	ioOmenItem::FireWeapon( ioBaseChar* pOwner )
{
	if ( !pOwner )
		return;

	for( int i = 0; i < m_iChargeAttackCount; i++ )
	{
		D3DXVECTOR3 vPos = pOwner->GetMidPositionByRate();
		DWORD dwWeaponIndex = pOwner->IncreaseWeaponIndexBase();

		FireTime kFireTime;
		kFireTime.dwStart = FRAMEGETTIME() + m_ChargeAttackInfo[i].m_dwCallTime;
		kFireTime.iAttributeIdx = m_ChargeAttackInfo[i].m_iWeaponIdx;
		kFireTime.iResistanceIdx = m_ChargeAttackInfo[i].m_iResistanceIdx;
		kFireTime.szWoundedAni  = m_ChargeAttackInfo[i].m_WoundedAnimation;
		kFireTime.dwWoundedAniDuration = m_ChargeAttackInfo[i].m_dwWoundedDuration;
		kFireTime.bLoopWoundedAni = m_ChargeAttackInfo[i].m_bWoundedLoopAni;
		kFireTime.dwWeaponIdx = dwWeaponIndex;
		kFireTime.eFireTimeType = FTT_NORMAL_ATTACK;

		vPos += pOwner->GetWorldOrientation() * m_vChargeAttackOffset[i];
		ioWeapon* pWeapon	= pOwner->ExtendFireExplicit( kFireTime, vPos, m_vAttackDir, "" );
	}

	//m_iFireCount++;
	//WasteBullet();

	if( !m_ChargeAttackSoundName.IsEmpty() )
	{
		ioEntityGroup *pGrp = pOwner->GetGroup();
		if( pGrp )
			g_SoundMgr.PlaySound( m_ChargeAttackSoundName, pGrp->GetParentSceneNode() );
	}
}

DWORD	ioOmenItem::AddEffect( const ioHashString& szEffectName, D3DXVECTOR3 vPos, ioBaseChar *pOwner, float fWaitTime )
{
	if ( !pOwner )
		return -1;

	ioPlayStage* pStage = pOwner->GetCreator();
	if ( !pStage )
		return -1;

	D3DXVECTOR3 vScale( FLOAT1, FLOAT1, FLOAT1 );
	ioMapEffect *pMapEffect = pStage->CreateMapEffect( szEffectName, vPos, vScale );
	if ( !pMapEffect )
		return -1;

	if( fWaitTime != 0.0f )
		pMapEffect->GetEffect()->SetWaitTime( fWaitTime );

	return pMapEffect->GetUniqueID();
}

void	ioOmenItem::RemoveEffect( DWORD& rhID, ioBaseChar *pOwner )
{
	if ( rhID == -1 )
		return;

	if ( !pOwner )
		return;

	ioPlayStage* pStage = pOwner->GetCreator();
	if ( !pStage )
		return;

	ioMapEffect *pEff = pStage->FindMapEffect( rhID );
	if ( pEff )
		pEff->EndEffectForce();

	rhID = -1;
}