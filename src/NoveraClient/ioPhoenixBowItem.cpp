#include "StdAfx.h"
#include "ioPhoenixBowItem.h"
#include "WeaponDefine.h"
#include "ioPhoenixJump.h"


ioPhoenixBowItem::ioPhoenixBowItem(void)
{
	Init();
}

ioPhoenixBowItem::ioPhoenixBowItem( const ioPhoenixBowItem &rhs ) : ioWeaponItem( rhs ), m_PhoenixState( rhs.m_PhoenixState ), m_dwHalfChargeTime( rhs.m_dwHalfChargeTime ), m_szDelayAnimation( rhs.m_szDelayAnimation ), m_HalfAttack( rhs.m_HalfAttack ),
	m_fChargingIncreaseGauge( rhs.m_fChargingIncreaseGauge ), m_iMultiShotMin( rhs.m_iMultiShotMin ), m_iMultiShotMax( rhs.m_iMultiShotMax ), m_iCurBullet( rhs.m_iCurBullet ), m_iMaxBullet( rhs.m_iMaxBullet ), m_iNeedBullet( rhs.m_iNeedBullet ),
	m_fBulletIncreaseRate( rhs.m_fBulletIncreaseRate ), m_fNeedGauge( rhs.m_fNeedGauge ), m_fTempBullet( rhs.m_fTempBullet ), m_szChargeAnimation( rhs.m_szChargeAnimation ), m_szChargePreAnimation( rhs.m_szChargePreAnimation ),
	m_szReadyCircleEffect( rhs.m_szReadyCircleEffect ), m_szEnableCircleEffect( rhs.m_szEnableCircleEffect ), m_fCircleRange( rhs.m_fCircleRange ), m_fCircleMoveSpeed( rhs.m_fCircleMoveSpeed ), m_fCircleHeightGap( rhs.m_fCircleHeightGap ),
	m_FullAttackMin( rhs.m_FullAttackMin ), m_FullAttackMax( rhs.m_FullAttackMax ),m_dwChargeAttackTerm( rhs.m_dwChargeAttackTerm ), m_fWeaponFloatPower( rhs.m_fWeaponFloatPower ), m_fWeaponFloatMinPower( rhs.m_fWeaponFloatMinPower ), 
	m_fWeaponGravityRate( rhs.m_fWeaponGravityRate ), m_fRevisionRate( rhs.m_fRevisionRate ), m_fWeaponMinSpeed( rhs.m_fWeaponMinSpeed ), m_fUpLimitRange( rhs.m_fUpLimitRange ), m_fUpLimitHeight( rhs.m_fUpLimitHeight ), 
	m_fWeaponGapDistance( rhs.m_fWeaponGapDistance ), m_AimEffect( rhs.m_AimEffect ), m_fAimAngle( rhs.m_fAimAngle ),	m_fAimRange( rhs.m_fAimRange ),	m_fAimMinRange( rhs.m_fAimMinRange ), m_JumpAimUp( rhs.m_JumpAimUp ), 
	m_JumpAimCenter( rhs.m_JumpAimCenter ), m_JumpAimDown( rhs.m_JumpAimDown ), m_AimUp( rhs.m_AimUp ), m_AimCenter( rhs.m_AimCenter ),	m_AimDown( rhs.m_AimDown )
{
	Init();
}

ioPhoenixBowItem::~ioPhoenixBowItem(void)
{
}

void	ioPhoenixBowItem::Init()
{
	m_PhoenixState	= PS_NONE;
	m_iCurCombo		= 0;
	m_iCurMultiShot	= 0;
	m_iCurBullet	= 0;
	m_fTempBullet	= 0.0f;

	m_fCurExtraGauge	= 0;
	m_dwChargeStartTime	= 0;
	m_dwChargeAttackStartTime	= 0;
	m_dwChargePreAnimationTime	= 0;
	m_fCurAngle	= 0.0f;

	m_CurrentChargeAttack	= NULL;

	m_bChargeLoopAniStart	= false;
	m_bAimEffect	= false;
}

void	ioPhoenixBowItem::LoadProperty( ioINILoader &rkLoader )
{
	ioWeaponItem::LoadProperty( rkLoader );

	m_dwHalfChargeTime	= rkLoader.LoadInt_e( "half_charging_time", 0 );
	m_fChargingIncreaseGauge	= rkLoader.LoadInt_e( "charging_increase_gauge", 0 );
	m_iMultiShotMin	= rkLoader.LoadInt_e( "multi_shot_min", 0 );
	m_iMultiShotMax	= rkLoader.LoadInt_e( "multi_shot_max", 0 );
	m_iMaxBullet	= rkLoader.LoadInt_e( "max_bullet", 0 );
	m_iNeedBullet	= rkLoader.LoadInt_e( "need_bullet", 0 );
	m_fBulletIncreaseRate	= rkLoader.LoadFloat_e( "bullet_increase_rate", FLOAT1 );
	m_fNeedGauge	= rkLoader.LoadFloat_e( "need_extra_gauge", FLOAT1 );
	m_fCircleRange	= rkLoader.LoadFloat_e( "magic_circle_range", FLOAT100 );
	m_fCircleMoveSpeed	= rkLoader.LoadFloat_e( "magic_circle_move_speed", FLOAT100 );
	m_fCircleHeightGap	= rkLoader.LoadFloat_e( "magic_circle_height_gap", FLOAT100 );
	m_dwChargeAttackTerm	= rkLoader.LoadInt_e( "full_attack_term", 0 );
	m_fWeaponFloatPower	= rkLoader.LoadFloat_e( "weapon_float_power", FLOAT1 );
	m_fWeaponFloatMinPower	= rkLoader.LoadFloat_e( "weapon_float_min_power", FLOAT1 );
	m_fWeaponGravityRate	= rkLoader.LoadFloat_e( "weapon_gravity_rate", FLOAT1 );
	m_fRevisionRate	= rkLoader.LoadFloat_e( "weapon_revision_rate", 0.0f );
	m_fWeaponMinSpeed	= rkLoader.LoadFloat_e( "weapon_min_speed", 0.0f );
	m_fUpLimitRange	= rkLoader.LoadFloat_e( "up_limit_range", 0.0f );
	m_fUpLimitHeight	= rkLoader.LoadFloat_e( "up_limit_height", 30.0f );
	m_fWeaponGapDistance	= rkLoader.LoadFloat_e( "weapon_gap_distance", 0.0f );
	m_fAimAngle	= rkLoader.LoadFloat_e( "aim_angle", 45.0f );
	m_fAimRange	= rkLoader.LoadFloat_e( "aim_range", FLOAT1000 );
	m_fAimMinRange	= rkLoader.LoadFloat_e( "aim_min_range", 0.0f );

	char szBuf[MAX_PATH];
	rkLoader.LoadString( "attack_ready_animation", "", szBuf, MAX_PATH );
	m_szDelayAnimation	= szBuf;
	rkLoader.LoadString( "charge_animation", "", szBuf, MAX_PATH );
	m_szChargeAnimation	= szBuf;
	rkLoader.LoadString( "charge_pre_animation", "", szBuf, MAX_PATH );
	m_szChargePreAnimation	= szBuf;
	rkLoader.LoadString( "ready_circle", "", szBuf, MAX_PATH );
	m_szReadyCircleEffect	= szBuf;
	rkLoader.LoadString( "enable_circle", "", szBuf, MAX_PATH );
	m_szEnableCircleEffect	= szBuf;
	rkLoader.LoadString_e( "aim_effect", "", szBuf, MAX_PATH );
	m_AimEffect = szBuf;
	rkLoader.LoadString_e( "aim_up_animation", "", szBuf, MAX_PATH );
	m_AimUp = szBuf;
	rkLoader.LoadString_e( "aim_center_animation", "", szBuf, MAX_PATH );
	m_AimCenter = szBuf;
	rkLoader.LoadString_e( "aim_down_animation", "", szBuf, MAX_PATH );
	m_AimDown = szBuf;
	rkLoader.LoadString_e( "jump_aim_up_animation", "", szBuf, MAX_PATH );
	m_JumpAimUp = szBuf;
	rkLoader.LoadString_e( "jump_aim_center_animation", "", szBuf, MAX_PATH );
	m_JumpAimCenter = szBuf;
	rkLoader.LoadString_e( "jump_aim_down_animation", "", szBuf, MAX_PATH );
	m_JumpAimDown = szBuf;

	LoadAttackAttribute( "half_attack", m_HalfAttack, rkLoader );
	LoadAttackAttribute( "full_attack_min", m_FullAttackMin, rkLoader );
	LoadAttackAttribute( "full_attack_max", m_FullAttackMax, rkLoader );
}

ioItem*	ioPhoenixBowItem::Clone()
{
	return	new	ioPhoenixBowItem( *this );
}

ioWeaponItem::WeaponSubType	ioPhoenixBowItem::GetSubType() const
{
	return WST_PHOENIXBOW_ITEM;
}

bool	ioPhoenixBowItem::IsEnableAttackDashCheck( ioBaseChar *pOwner, bool bCheck )
{
	if( pOwner->GetState() == CS_JUMP )
		return	true;

	return	false;
}

void	ioPhoenixBowItem::SetExtraInfoToWeapon( ioWeapon *pWeapon, bool bJumpAttack )
{
	if( !pWeapon || !pWeapon->CheckWeaponByJump() )
		return;

	ioBaseChar *pOwner = pWeapon->GetOwner();
	if( !pOwner )
		return;

	D3DXVECTOR3	vAimedDir	= ReCalcMoveDir( pOwner, pWeapon );
	pWeapon->SetMoveDir( vAimedDir );
}

void	ioPhoenixBowItem::UpdateExtraData( ioBaseChar *pOwner )
{
	// 여기에서 bullet 업데이트를 한다.
	if( !pOwner )
		return;

	if( m_iCurBullet >= m_iMaxBullet )
		return;

	switch( pOwner->GetState() )
	{
	case CS_DELAY:
	case CS_DASH:
	case CS_RUN:
		{
			float fTimePerSec = g_FrameTimer.GetSecPerFrame();
			m_fTempBullet	+= fTimePerSec * m_fBulletIncreaseRate;
			m_iCurBullet	= (int)m_fTempBullet;
		}
		break;
	}
}

void	ioPhoenixBowItem::CheckNormalAttack( ioBaseChar *pOwner )
{
	switch( m_PhoenixState )
	{
	case	PS_CHARGING:
		{
			OnCharging( pOwner );
		}
		break;

	case	PS_ATTACK_FIRE:
	case	PS_HALF_ATTACK_FIRE:
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

	case	PS_FULL_ATTACK_FIRE:
		{
			SetFullAttackFire( pOwner );
		}
		break;

	case	PS_CIRCLE_ACTION:
		{
			SetCircleAction( pOwner );
		}
		break;
	}
}

void	ioPhoenixBowItem::OnItemGaugeOK( ioBaseChar *pOwner, SP2Packet &rkPacket )
{
	if( !pOwner ) return;

	int iState;
	rkPacket >> iState;
	rkPacket >> m_iCurCombo;
	m_PhoenixState	= (PhoenixState)iState;

	switch( iState )
	{
	case PS_DELAY:
		{
			ioEntityGroup *pGrp = pOwner->GetGroup();
			if( pGrp )
			{
				pGrp->ClearAllLoopAni( FLOAT100, true );
				pGrp->ClearAllActionAni( FLOAT100, true );
			}

			pOwner->SetState( CS_DELAY );
			m_CurrentChargeAttack	= NULL;
			m_bChargeLoopAniStart	= false;
		}
		break;
	case	PS_CHARGING:
		{
			ChangeToCharging( pOwner );
		}
		break;

	case	PS_ATTACK_FIRE:
		{
			ChangeToAttackFire( pOwner );
		}
		break;

	case	PS_HALF_ATTACK_FIRE:
		{
			ChangeToHalfAttackFire( pOwner );
		}
		break;

	case	PS_FULL_ATTACK_FIRE:
		{
			ChangeToFullAttackFire( pOwner );
		}
		break;

	case	PS_CIRCLE_ACTION:
		{
			int	iTemp;
			PhoenixJumpChargeFireType	chargeType;
			D3DXVECTOR3 vPos, vDir, vTarget;
			D3DXQUATERNION	qtRot;
			rkPacket >> iTemp;
			rkPacket >> vPos;
			rkPacket >> vDir;
			rkPacket >> vTarget;
			chargeType	= (PhoenixJumpChargeFireType)iTemp;
			OnCircleAction( pOwner, vTarget, vPos, vDir, chargeType );
		}
		break;
	}
}

void	ioPhoenixBowItem::OnItemGaugeCancel( ioBaseChar *pOwner, SP2Packet &rkPacket )
{
	if( !pOwner ) return;

	int iState;
	rkPacket >> iState;
	rkPacket >> m_iCurCombo;

	switch( iState )
	{
	case	PSS_Full_ATTACK_FIRE:
		{
			OnFullAttackFire( pOwner );
		}
		break;
	}
}

void	ioPhoenixBowItem::SetNormalAttack( ioBaseChar *pOwner, int iCurCombo )
{
	ClearCancelInfo();

	if( m_bAimEffect )
	{
		m_bAimEffect = false;
		pOwner->EndEffect( m_AimEffect, false );
	}

	DWORD dwCurTime = FRAMEGETTIME();
	int nComboCnt = GetMaxCombo();
	if( !COMPARE( iCurCombo, 0, nComboCnt ) )
	{
		ioWeaponItem::SetNormalAttack( pOwner, iCurCombo );

		pOwner->SetAttackMoveEnable( false );
		pOwner->SetChargingState( false );

		m_iCurCombo	= 0;
		m_PhoenixState = PS_DELAY;
		m_bChargeLoopAniStart	= false;
	}
	else
	{
		m_iCurCombo	= iCurCombo;
		ChangeToCharging( pOwner );
	}
}

void	ioPhoenixBowItem::OnCharging( ioBaseChar *pOwner )
{
	if( !pOwner->IsNeedProcess() )
		return;

	DWORD dwCurTime = FRAMEGETTIME();
	if( pOwner->IsAttackKeyDown() )
	{
		if( m_dwChargeStartTime + m_dwChargeTime <= dwCurTime )
		{
			ChangeToFullAttackFire( pOwner );
		}
	}
	else
	{
		if( m_dwChargeStartTime + m_dwHalfChargeTime <= dwCurTime )
		{
			ChangeToHalfAttackFire( pOwner );
		}
		else
		{
			ChangeToAttackFire( pOwner );
		}
	}
}

void	ioPhoenixBowItem::ChangeToCharging( ioBaseChar *pOwner )
{
	if( !pOwner )
		return;

	//공격 준비 애니메이션 설정
	ioEntityGroup *pGrp = pOwner->GetGroup();
	if( !pGrp )
		return;

	pGrp->ClearAllLoopAni( FLOAT100, true );
	pGrp->ClearAllActionAni( FLOAT100, true );

	int iAniID = pGrp->GetAnimationIdx( m_szDelayAnimation );

	float fTimeRate = (float)m_dwChargeTime / pGrp->GetAnimationFullTime( iAniID );
	pGrp->SetActionAni( iAniID, FLOAT100, FLOAT100, FLOAT1, FLOAT1/fTimeRate, 0.0f, 0.0f, true );

	pOwner->SetState( CS_ATTACK );
	pOwner->SetAttackMoveEnable( false );
	pOwner->SetChargingState( true );

	m_PhoenixState = PS_CHARGING;
	m_dwChargeStartTime	= FRAMEGETTIME();

	if( pOwner->IsNeedSendNetwork() )
	{
		SP2Packet kPacket( CUPK_ITEM_GAUGE_OK );
		FillItemGaugePacketHeader( pOwner, kPacket );
		kPacket << (int)m_PhoenixState;
		kPacket << m_iCurCombo;
		P2PNetwork::SendToAllPlayingUser( kPacket );
	}
}

void	ioPhoenixBowItem::ChangeToHalfAttackFire( ioBaseChar *pOwner )
{
	DWORD dwCurTime = FRAMEGETTIME();
	pOwner->InitExtendAttackTagInfo();
	ClearCancelInfo();

	pOwner->SetNormalAttackByAttribute( m_HalfAttack );
	m_szCurAni					= m_HalfAttack.m_AttackAnimation;
	m_fCurAniRate				= m_HalfAttack.m_fAttackAniRate;
	m_dwInputCancelCheckTime	= dwCurTime;
	m_dwCancelCheckTime			= dwCurTime;
	pOwner->ApplyNormalAttackResult( m_iCurCombo );
	m_PhoenixState = PS_HALF_ATTACK_FIRE;

	if( pOwner->IsNeedSendNetwork() )
	{
		SP2Packet kPacket( CUPK_ITEM_GAUGE_OK );
		FillItemGaugePacketHeader( pOwner, kPacket );
		kPacket << (int)m_PhoenixState;
		kPacket << m_iCurCombo;
		P2PNetwork::SendToAllPlayingUser( kPacket );
	}
}

void	ioPhoenixBowItem::ChangeToFullAttackFire( ioBaseChar *pOwner )
{
	if( !pOwner )
		return;
	//if( !pOwner || !pOwner->IsNeedProcess() )
		//return;

	ioEntityGroup *pGrp = pOwner->GetGroup();
	if( !pGrp )
		return;

	m_PhoenixState	= PS_FULL_ATTACK_FIRE;

	int iAniID = pGrp->GetAnimationIdx( m_szChargePreAnimation );
	pGrp->ClearAllActionAni( FLOAT10, true );
	pGrp->SetActionAni( iAniID, FLOAT100, FLOAT100 );
	m_dwChargePreAnimationTime	= FRAMEGETTIME() + pGrp->GetAnimationFullTime( iAniID );

	//if( pOwner->IsNeedProcess() )
	{
		ioHashString szDummyCircle;
		pOwner->CreateMagicCircle( m_szReadyCircleEffect, m_szEnableCircleEffect, szDummyCircle, m_fMagicCircleOffSet );
	}

	if( pOwner->IsNeedSendNetwork() )
	{
		SP2Packet kPacket( CUPK_ITEM_GAUGE_OK );
		FillItemGaugePacketHeader( pOwner, kPacket );
		kPacket << (int)m_PhoenixState;
		kPacket << m_iCurCombo;
		P2PNetwork::SendToAllPlayingUser( kPacket );
	}
}

void	ioPhoenixBowItem::ChangeToPreFullAttackFire( ioBaseChar *pOwner )
{

}

void	ioPhoenixBowItem::SetFullAttackFire( ioBaseChar *pOwner )
{
	if( !pOwner )
		return;

	ioEntityGroup *pGrp = pOwner->GetGroup();
	if( !pGrp )
		return;

	// 매직 서클
	if( pOwner->CheckMagicCircle() )
	{
		pOwner->CheckChangeDirectionForMagicCircle();
		pOwner->UpdateMagicCirclePosAll( m_fCircleMoveSpeed, m_fCircleRange, m_fCircleHeightGap, false );

		D3DXVECTOR3 vCharPos = pOwner->GetWorldPosition();
		D3DXVECTOR3 vCharDir = pOwner->GetMagicCirclePos() - vCharPos;
		vCharDir.y = 0.0f;

		D3DXVec3Normalize( &vCharDir, &vCharDir );

		pOwner->SetTargetRotToDir( vCharDir, true );
	}

	// loop ani
	if( !m_bChargeLoopAniStart && m_dwChargePreAnimationTime < FRAMEGETTIME() )
	{
		int iAniID = pGrp->GetAnimationIdx( m_szChargeAnimation );

		pGrp->ClearAllLoopAniExceptOne( iAniID, FLOAT100, true );
		pGrp->ClearAllActionAni( FLOAT10, true );
		pGrp->SetLoopAni( iAniID, FLOAT100 );
		pOwner->SetAttackMoveEnable( false );
		pOwner->ChangeDirectionByInputDir( true );

		m_bChargeLoopAniStart	= true;
	}

	if( !pOwner->IsNeedProcess() )
		return;

	if( m_fCurExtraGauge < m_fMaxExtraGauge )
	{
		float fTimePerSec = g_FrameTimer.GetSecPerFrame();
		float fGauge = m_fCurExtraGauge + m_fChargingIncreaseGauge * fTimePerSec;
		SetCurExtraGauge( fGauge );

		if( m_fNeedGauge > m_fCurExtraGauge )
		{
			m_iCurMultiShot = 0;
			m_CurrentChargeAttack	= NULL;
		}
		else if( m_fMaxExtraGauge <= m_fCurExtraGauge )
		{
			m_iCurMultiShot	= m_iMultiShotMax;
			m_CurrentChargeAttack	= &m_FullAttackMax;
			MaxExtraGauge();
		}
		else
		{
			m_iCurMultiShot	= m_iMultiShotMin;
			m_CurrentChargeAttack	= &m_FullAttackMin;
		}
	}

	if( !pOwner->IsAttackKeyDown() )
	{
		if( m_iCurMultiShot > 0 )
		{
			ChangeToCircleAction( pOwner );
		}
		else
		{
			pOwner->SetState( CS_DELAY );
			m_PhoenixState	= PS_DELAY;
			m_iCurMultiShot	= 0;
			m_CurrentChargeAttack	= NULL;
			m_bChargeLoopAniStart	= false;
		}

		InitExtraGauge();
	}
}

void	ioPhoenixBowItem::ChangeToAttackFire( ioBaseChar *pOwner )
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

		m_szCurAni					= pAttribute->m_AttackAnimation;
		m_fCurAniRate				= pAttribute->m_fAttackAniRate;
		m_dwInputCancelCheckTime	= dwCurTime;
		m_dwCancelCheckTime			= dwCurTime;
	}
	pOwner->ApplyNormalAttackResult( m_iCurCombo );
	m_PhoenixState = PS_ATTACK_FIRE;

	if( pOwner->IsNeedSendNetwork() )
	{
		SP2Packet kPacket( CUPK_ITEM_GAUGE_OK );
		FillItemGaugePacketHeader( pOwner, kPacket );
		kPacket << (int)m_PhoenixState;
		kPacket << m_iCurCombo;
		P2PNetwork::SendToAllPlayingUser( kPacket );
	}
}

void	ioPhoenixBowItem::ChangeToCircleAction( ioBaseChar *pOwner )
{
	if( !pOwner || !pOwner->IsNeedProcess() )
		return;

	ioEntityGroup *pGrp = pOwner->GetGroup();
	if( !pGrp )
		return;

	m_PhoenixState	= PS_CIRCLE_ACTION;

	pOwner->SetAttackMoveEnable( false );
	pOwner->ChangeDirectionByInputDir( true );
	pOwner->InitExtendAttackTagInfo();

	m_dwChargeAttackStartTime	= FRAMEGETTIME();

	if( pOwner->IsNeedSendNetwork() )
	{
		SP2Packet kPacket( CUPK_ITEM_GAUGE_CANCEL );
		FillItemGaugePacketHeader( pOwner, kPacket );
		kPacket << (int)PSS_Full_ATTACK_FIRE;
		kPacket << m_iCurCombo;
		P2PNetwork::SendToAllPlayingUser( kPacket );
	}
}

void	ioPhoenixBowItem::SetCircleAction( ioBaseChar *pOwner )
{
	// 매직 서클
	if( pOwner->CheckMagicCircle() )
	{
		pOwner->CheckChangeDirectionForMagicCircle();
		pOwner->UpdateMagicCirclePosAll( m_fCircleMoveSpeed, m_fCircleRange, m_fCircleHeightGap, false );

		D3DXVECTOR3 vCharPos = pOwner->GetWorldPosition();
		D3DXVECTOR3 vCharDir = pOwner->GetMagicCirclePos() - vCharPos;
		vCharDir.y = 0.0f;

		D3DXVec3Normalize( &vCharDir, &vCharDir );

		pOwner->SetTargetRotToDir( vCharDir, true );
	}

	if( !pOwner || !pOwner->IsNeedProcess() )
		return;

	ioEntityGroup *pGrp = pOwner->GetGroup();
	if( !pGrp )
		return;

	DWORD dwCurTime = FRAMEGETTIME();
	if( m_iCurMultiShot <= 0 )
	{
		if( m_dwChargeAttackStartTime + m_dwChargeAttackTerm < dwCurTime )
		{
			m_PhoenixState	= PS_DELAY;

			pOwner->DestroyMagicCircle();
			pOwner->SetState( CS_DELAY );

			m_CurrentChargeAttack	= NULL;
			m_bChargeLoopAniStart	= false;

			if( pOwner->IsNeedSendNetwork() )
			{
				SP2Packet kPacket( CUPK_ITEM_GAUGE_OK );
				FillItemGaugePacketHeader( pOwner, kPacket );
				kPacket << (int)m_PhoenixState;
				kPacket << m_iCurCombo;
				P2PNetwork::SendToAllPlayingUser( kPacket );
			}
		}

		return;
	}

	if( m_dwChargeAttackStartTime + m_dwChargeAttackTerm < dwCurTime )
	{
		DWORD dwWeaponBaseIndex = pOwner->IncreaseWeaponIndexBase();
		D3DXVECTOR3 vPos = pOwner->GetMidPositionByRate( 1.0f );
		D3DXVECTOR3 vDir	= pOwner->GetMagicCircleRot() * D3DXVECTOR3( 0.0f, 0.0f, -FLOAT1 );

		vDir	= pOwner->GetMagicCirclePos() - pOwner->GetWorldPosition();

		D3DXVECTOR3	vWeaponDir;
		D3DXVec3Normalize( &vWeaponDir, &vDir );	

		if( !m_CurrentChargeAttack )
			return;

		FireTime kFireTime;
		kFireTime.dwStart	= FRAMEGETTIME();

		kFireTime.iAttributeIdx			= m_CurrentChargeAttack->m_vWeaponInfoList[0].m_iWeaponIdx;
		kFireTime.iResistanceIdx		= m_CurrentChargeAttack->m_vWeaponInfoList[0].m_iResistanceIdx;
		kFireTime.szWoundedAni			= m_CurrentChargeAttack->m_vWeaponInfoList[0].m_WoundedAnimation;
		kFireTime.dwWoundedAniDuration	= m_CurrentChargeAttack->m_vWeaponInfoList[0].m_dwWoundedDuration;
		kFireTime.bLoopWoundedAni		= m_CurrentChargeAttack->m_vWeaponInfoList[0].m_bWoundedLoopAni;

		kFireTime.dwWeaponIdx			= dwWeaponBaseIndex++;
		kFireTime.eFireTimeType			= FTT_NORMAL_ATTACK;

		ioWeapon*	pWeapon	= pOwner->ExtendFireExplicit( kFireTime, vPos, vWeaponDir, "" );
		ioFloatThrowWeapon*	pFloatWeapon	= ToFloatThrowWeapon( pWeapon );
		if( pFloatWeapon )
		{
			float	fSpeed	= 0.0f;
			float	fFloating	= 0.0f;
			GetChargingArrowSpeedNFloatingPower( pOwner, pOwner->GetMagicCirclePos(), vDir, fSpeed, fFloating );

			pFloatWeapon->SetMoveSpeed( fSpeed );
			pFloatWeapon->SetFloatPower( fFloating );
			pFloatWeapon->SetGravityRate( m_fWeaponGravityRate );
		}

		m_dwChargeAttackStartTime	= FRAMEGETTIME();
		m_iCurMultiShot--;

		// 에니메이션 처리
		int	iAniID = pGrp->GetAnimationIdx( m_CurrentChargeAttack->m_AttackAnimation );
		if( iAniID == -1 )
			return;

		float	fTimeRate   = m_CurrentChargeAttack->m_fAttackAniRate;
		DWORD	dwPreDelay  = m_CurrentChargeAttack->m_dwPreDelay;

		pGrp->ClearAllLoopAni( FLOAT100, true );
		pGrp->ClearAllActionAni( FLOAT100, true );

		DWORD	dwFireEndTime	= pGrp->GetAnimationFullTime( iAniID ) * fTimeRate + dwPreDelay;

		if( m_CurrentChargeAttack->m_AttackEndAnimation.IsEmpty() )
		{
			pGrp->SetActionAni( iAniID, FLOAT100, FLOAT100, FLOAT1, FLOAT1/fTimeRate, 0.0f, dwPreDelay, true );
		}
		else
		{
			pGrp->SetActionAni( iAniID, FLOAT100, 0.0f, FLOAT1, FLOAT1/fTimeRate, 0.0f, dwPreDelay );

			pGrp->SetActionAni( m_CurrentChargeAttack->m_AttackEndAnimation,
				0.0f, FLOAT100,
				1.0f, FLOAT1/m_CurrentChargeAttack->m_fAttackEndAniRate,
				dwFireEndTime, 0.0f, true );
		}

		PhoenixJumpChargeFireType	chargeType;
		if( m_CurrentChargeAttack == &m_FullAttackMax )
			chargeType	= PJCFT_MAX;
		else
			chargeType	= PJCFT_MIN;

		if( pOwner->IsNeedSendNetwork() )
		{
			SP2Packet kPacket( CUPK_ITEM_GAUGE_OK );
			FillItemGaugePacketHeader( pOwner, kPacket );
			kPacket << (int)m_PhoenixState;
			kPacket << m_iCurCombo;
			kPacket << (int)chargeType;
			kPacket << vPos;
			kPacket << vDir;
			kPacket << pOwner->GetMagicCirclePos();
			P2PNetwork::SendToAllPlayingUser( kPacket );
		}
	}
}

void	ioPhoenixBowItem::OnCircleAction( ioBaseChar *pOwner, D3DXVECTOR3& target, D3DXVECTOR3& vPos, D3DXVECTOR3& vDir, PhoenixJumpChargeFireType chargeType )
{
	if( !pOwner )
		return;

	ioEntityGroup *pGrp = pOwner->GetGroup();
	if( !pGrp )
		return;

	DWORD dwWeaponBaseIndex = pOwner->IncreaseWeaponIndexBase();

	D3DXVECTOR3	vWeaponDir;
	D3DXVec3Normalize( &vWeaponDir, &vDir );	

	if( chargeType == PJCFT_MAX )
		m_CurrentChargeAttack	= &m_FullAttackMax;
	else
		m_CurrentChargeAttack	= &m_FullAttackMin;

	if( !m_CurrentChargeAttack )
		return;

	FireTime kFireTime;
	kFireTime.dwStart	= FRAMEGETTIME();

	kFireTime.iAttributeIdx			= m_CurrentChargeAttack->m_vWeaponInfoList[0].m_iWeaponIdx;
	kFireTime.iResistanceIdx		= m_CurrentChargeAttack->m_vWeaponInfoList[0].m_iResistanceIdx;
	kFireTime.szWoundedAni			= m_CurrentChargeAttack->m_vWeaponInfoList[0].m_WoundedAnimation;
	kFireTime.dwWoundedAniDuration	= m_CurrentChargeAttack->m_vWeaponInfoList[0].m_dwWoundedDuration;
	kFireTime.bLoopWoundedAni		= m_CurrentChargeAttack->m_vWeaponInfoList[0].m_bWoundedLoopAni;

	kFireTime.dwWeaponIdx			= dwWeaponBaseIndex++;
	kFireTime.eFireTimeType			= FTT_NORMAL_ATTACK;

	ioWeapon*	pWeapon	= pOwner->ExtendFireExplicit( kFireTime, vPos, vWeaponDir, "" );
	ioFloatThrowWeapon*	pFloatWeapon	= ToFloatThrowWeapon( pWeapon );
	if( pFloatWeapon )
	{
		float	fSpeed	= 0.0f;
		float	fFloating	= 0.0f;
		GetChargingArrowSpeedNFloatingPower( pOwner, target, vDir, fSpeed, fFloating );

		pFloatWeapon->SetMoveSpeed( fSpeed );
		pFloatWeapon->SetFloatPower( fFloating );
		pFloatWeapon->SetGravityRate( m_fWeaponGravityRate );
	}

	// 에니메이션 처리
	int	iAniID = pGrp->GetAnimationIdx( m_CurrentChargeAttack->m_AttackAnimation );
	if( iAniID == -1 )
		return;

	float	fTimeRate   = m_CurrentChargeAttack->m_fAttackAniRate;
	DWORD	dwPreDelay  = m_CurrentChargeAttack->m_dwPreDelay;

	pGrp->ClearAllLoopAni( FLOAT100, true );
	pGrp->ClearAllActionAni( FLOAT100, true );

	DWORD	dwFireEndTime	= pGrp->GetAnimationFullTime( iAniID ) * fTimeRate + dwPreDelay;

	if( m_CurrentChargeAttack->m_AttackEndAnimation.IsEmpty() )
	{
		pGrp->SetActionAni( iAniID, FLOAT100, FLOAT100, FLOAT1, FLOAT1/fTimeRate, 0.0f, dwPreDelay, true );
	}
	else
	{
		pGrp->SetActionAni( iAniID, FLOAT100, 0.0f, FLOAT1, FLOAT1/fTimeRate, 0.0f, dwPreDelay );

		pGrp->SetActionAni( m_CurrentChargeAttack->m_AttackEndAnimation,
			0.0f, FLOAT100,
			1.0f, FLOAT1/m_CurrentChargeAttack->m_fAttackEndAniRate,
			dwFireEndTime, 0.0f, true );
	}
}

void	ioPhoenixBowItem::OnFullAttackFire( ioBaseChar *pOwner )
{
	if( !pOwner )
		return;

	ioEntityGroup *pGrp = pOwner->GetGroup();
	if( !pGrp )
		return;

	pOwner->SetAttackMoveEnable( false );
	pOwner->ChangeDirectionByInputDir( true );
	pOwner->InitExtendAttackTagInfo();
}

void	ioPhoenixBowItem::GetChargingArrowSpeedNFloatingPower( ioBaseChar *pOwner, D3DXVECTOR3& target, D3DXVECTOR3& dir, float& speed, float& floatingPower )
{
	// CharDir
	D3DXVECTOR3 vCharPos = pOwner->GetWorldPosition();

	// WeaponMove
	D3DXVECTOR3 vTargetPos = target;

	float fRate = FLOAT1;
	float fRange = D3DXVec3Length( &dir );

	float fCurWeaponFloatPower = 0.0f;
	float fCurWeaponMoveSpeed = 0.0f;
	if( fRange > 0.0f )
	{
		fRate = fRange / ( m_fCircleRange /*+ m_fWeaponGapDistance*/ );

		// CurFloat
		fCurWeaponFloatPower = m_fWeaponFloatPower * fRate;
		fCurWeaponFloatPower = max( fCurWeaponFloatPower, m_fWeaponFloatMinPower );

		float fCurGravity = 0.0f;
		float fWeaponMoveSpeed = 0.0f;
		ioPlayStage *pStage = pOwner->GetCreator();
		if( pStage )
			fCurGravity = pStage->GetRoomGravity() * m_fWeaponGravityRate;

		float fFlightDuration = 0.0f;
		float fGapHeight = pOwner->GetMidHeightByRate(m_fRevisionRate) - vCharPos.y;
		float fGapHeight2 = vCharPos.y - vTargetPos.y;
		if( fGapHeight2 > 0.0f )
		{
			fGapHeight += fGapHeight2;
			fFlightDuration = GetFlightDuration( fCurGravity, fCurWeaponFloatPower, fGapHeight );

			if( fFlightDuration > 0.0f )
				fWeaponMoveSpeed = ( fRange /*+ m_fWeaponGapDistance*/ ) / (fFlightDuration / FLOAT1000);

			fCurWeaponMoveSpeed = max( m_fWeaponMinSpeed, fWeaponMoveSpeed );
		}
		else if( COMPARE(fGapHeight2, -m_fUpLimitHeight, -20.0f) && fRange > m_fUpLimitRange )
		{
			float fNewGap = fabs( fGapHeight2 ) - fGapHeight;
			fFlightDuration = GetTargetTime( fCurGravity, fNewGap, fCurWeaponFloatPower );
			fRange -= 35.0f;

			if( fFlightDuration > 0.0f )
				fCurWeaponMoveSpeed = ( fRange /*+ m_fWeaponGapDistance*/ ) / (fFlightDuration / FLOAT1000);
		}
		else
		{
			fFlightDuration = GetFlightDuration( fCurGravity, fCurWeaponFloatPower, fGapHeight );

			if( fFlightDuration > 0.0f )
				fWeaponMoveSpeed = ( fRange /*+ m_fWeaponGapDistance*/ ) / (fFlightDuration / FLOAT1000);

			fCurWeaponMoveSpeed = max( m_fWeaponMinSpeed, fWeaponMoveSpeed );
		}
	}

	speed	= fCurWeaponMoveSpeed;
	floatingPower	= fCurWeaponFloatPower;
}

float	ioPhoenixBowItem::GetFlightDuration( float fCurGravity, float fCurWeaponFloatPower, float fGapHeight )
{
	float fTopTime, fTopHeight, fGapTime;
	fTopTime = fTopHeight = fGapTime = 0.0f;

	float fNewGravity = fCurGravity;
	if( fNewGravity > 0.0f )
		fTopTime = fCurWeaponFloatPower / fNewGravity;

	float fCurFloatPower = -fCurWeaponFloatPower;
	float fCurGapHeight = 0.0f;
	while( fCurGapHeight < fGapHeight )
	{
		fCurGapHeight += fabs(fCurFloatPower * 0.01f);
		fCurFloatPower -= fNewGravity * 0.01f;
		fGapTime += 0.01f;
	}

	float fResult = 2 * fTopTime * FLOAT1000;
	fResult += fGapTime * FLOAT1000;
	return fResult;
}

float	ioPhoenixBowItem::GetTargetTime( IN float fCurGravity, IN float fGapHeight, OUT float &fFloatPower )
{
	float fGapTime = 0.0f;
	float fCurFloatPower = 0;
	float fCurGapHeight = 0.0f;

	float fRevisionH = FLOAT100;
	float fTopHeight = fGapHeight + fRevisionH;

	// Top Check
	float fTopTime = 0.0f;
	do
	{
		fTopTime = fGapTime;

		fCurGapHeight += fabs(fCurFloatPower * 0.01f);
		fCurFloatPower += fCurGravity * 0.01;
		fGapTime += 0.01;
	} while( fCurGapHeight < fTopHeight );

	fFloatPower = fCurFloatPower;


	// Target Check
	fGapTime = 0.0f;
	fCurFloatPower = 0;
	fCurGapHeight = 0.0f;

	float fTargetTime = 0.18f;

	fTargetTime += fTopTime;
	fTargetTime *= FLOAT1000;
	return fTargetTime;
}

bool ioPhoenixBowItem::SetJumpAttack( ioBaseChar *pOwner, int iAniID, float fTimeRate, AttackType eType )
{
	ClearAirJump();
	AttackType eExtendType = AT_JUMP_ATTACK;
	m_AimedTarget.Clear();

	const AttackAttribute *pAttr = GetAttackAttribute( eExtendType );
	if( !pAttr )
		return false;

	ioEntityGroup *pGrp = pOwner->GetGroup();
	iAniID = pGrp->GetAnimationIdx( pAttr->m_AttackAnimation );
	if( iAniID == -1 )
		return false;

	fTimeRate   = pAttr->m_fAttackAniRate;
	DWORD dwPreDelay  = pAttr->m_dwPreDelay;

	pOwner->RefreshFireTimeList( iAniID,
		pAttr->m_vWeaponInfoList,
		FTT_JUMP_ATTACK,
		fTimeRate,
		dwPreDelay );

	pOwner->IncreaseWeaponIndexBase();

	// For AirJumpPower
	SetAirJump( pOwner, iAniID, fTimeRate );

	// 에니메이션 처리
	pGrp->ClearAllLoopAni( FLOAT100, true );
	pGrp->ClearAllActionAni( FLOAT100, true );

	DWORD	dwFireEndTime	= pGrp->GetAnimationFullTime( iAniID ) * fTimeRate + dwPreDelay;

	if( pAttr->m_AttackEndAnimation.IsEmpty() )
	{
		pGrp->SetActionAni( iAniID, FLOAT100, FLOAT100, FLOAT1, FLOAT1/fTimeRate, 0.0f, dwPreDelay, true );
	}
	else
	{
		pGrp->SetActionAni( iAniID, FLOAT100, 0.0f, FLOAT1, FLOAT1/fTimeRate, 0.0f, dwPreDelay );

		pGrp->SetActionAni( pAttr->m_AttackEndAnimation,
			0.0f, FLOAT100,
			1.0f, FLOAT1/pAttr->m_fAttackEndAniRate,
			dwFireEndTime, 0.0f, true );
	}

	pOwner->SetChargingState( false );
	pOwner->SetAttackMoveEnable( false );

	return	true;
}

int		ioPhoenixBowItem::GetCurBullet()
{
	return	m_iCurBullet;
}

int		ioPhoenixBowItem::GetMaxBullet()
{
	return	m_iMaxBullet;
}

int		ioPhoenixBowItem::GetNeedBullet()
{
	return	m_iNeedBullet;
}

void	ioPhoenixBowItem::SetCurBullet( int iCurBullet )
{
	m_iCurBullet	= iCurBullet;
	m_fTempBullet	= (float)iCurBullet;
}

float	ioPhoenixBowItem::GetNeedExtraGauge()
{
	return	m_fNeedGauge;
}

ioHashString	ioPhoenixBowItem::FindAimTarget( ioBaseChar *pOwner, const ioHashString& stChecker )
{
	ioHashString szTarget;
	m_AimedTarget.Clear();

	float fAimAngle = m_fAimAngle;
	float fAimRange = m_fAimRange;

	if( pOwner->CheckAimedTargetByBow( fAimAngle, m_fAimMinRange, fAimRange, szTarget, false ) )
	{
		if( stChecker.IsEmpty() )
			m_AimedTarget = szTarget;
		else if( !stChecker.IsEmpty() && stChecker == szTarget )
			m_AimedTarget = szTarget;
		else 
			m_AimedTarget.Clear();
	}

	return m_AimedTarget;
}

void	ioPhoenixBowItem::TrackingTarget( ioBaseChar *pOwner, const ioHashString& stUp /* =  */, const ioHashString& stCenter /* =  */, const ioHashString& stDown /* = */ )
{
	bool bJumpState = false;
	if( pOwner->IsJumpState() )
		bJumpState = true;

	ioBall *pBall = pOwner->FindBall();
	ioBaseChar *pTarget = pOwner->GetBaseChar( m_AimedTarget );

	ioEntityGroup *pGrp = pOwner->GetGroup();
	ioAnimation *pUp, *pCenter, *pDown;

	if( bJumpState )
	{
		pUp = pGrp->GetLoopInPlay( stUp.IsEmpty() ? m_JumpAimUp : stUp );
		pCenter = pGrp->GetLoopInPlay( stCenter.IsEmpty() ?	m_JumpAimCenter : stCenter );
		pDown = pGrp->GetLoopInPlay( stDown.IsEmpty() ? m_JumpAimDown : stDown );
	}
	else
	{
		pUp = pGrp->GetLoopInPlay( m_AimUp );
		pCenter = pGrp->GetLoopInPlay( m_AimCenter );
		pDown = pGrp->GetLoopInPlay( m_AimDown );
	}

	D3DXVECTOR3 vAimedDir;

	D3DXQUATERNION qtRot = pOwner->GetTargetRot();
	vAimedDir = qtRot * D3DXVECTOR3( 0.0f, 0.0f, -FLOAT1 );

	if( pTarget )
		vAimedDir = pTarget->GetMidPositionByRate() - pOwner->GetMidPositionByRate();
	else if( m_bAimedBall && pBall )
		vAimedDir = pBall->GetMidPositionByRate() - pOwner->GetMidPositionByRate();

	D3DXVec3Normalize( &vAimedDir, &vAimedDir );
	float fAniWeight = GetAimedUpDownWeight( vAimedDir, 85.0f );
	if( vAimedDir.y >= 0.0f )	// up, center
	{
		if( !pUp || !pCenter || !pDown )
		{
			return;
		}

		pUp->ForceWeight( fAniWeight );
		pCenter->ForceWeight( FLOAT1 - fAniWeight );
		pDown->ForceWeight( 0.0f );
	}
	else	// center, down
	{
		if( !pUp || !pCenter || !pDown )
		{
			return;
		}
		pUp->ForceWeight( 0.0f );
		pCenter->ForceWeight( FLOAT1 - fAniWeight );
		pDown->ForceWeight( fAniWeight );
	}
}

void	ioPhoenixBowItem::SetAimEffet( ioBaseChar *pOwner )
{
	if( !pOwner )
		return;

	if( !m_bAimEffect )
	{
		pOwner->AttachEffect( m_AimEffect );
		m_bAimEffect = true;
	}
}

void	ioPhoenixBowItem::SetEndAimEffect( ioBaseChar *pOwner )
{
	if( !pOwner )
		return;

	if( m_bAimEffect )
	{
		pOwner->EndEffect( m_AimEffect, false );
		m_bAimEffect	= false;
	}
}

float	ioPhoenixBowItem::GetAimedUpDownWeight( const D3DXVECTOR3 &vAimDir, float fMaxAngle )
{
	D3DXVECTOR3 vXZDir( vAimDir.x, 0.0f, vAimDir.z );
	D3DXVec3Normalize( &vXZDir, &vXZDir );

	float fAngle = D3DXVec3Dot( &vXZDir, &vAimDir );
	fAngle = max( -FLOAT1, min( fAngle, FLOAT1 ) );
	fAngle = acosf( fAngle );
	fAngle = RADtoDEG( fAngle );
	fAngle = max( 0.0f, min( fAngle, fMaxAngle ) );

	float fWeight = fAngle / fMaxAngle;

	return fWeight;
}

bool	ioPhoenixBowItem::CheckCurrentTargetValid( ioBaseChar *pOwner, bool bAngleAlsoCheck )
{
	ioBall *pBall = pOwner->FindBall();
	ioBaseChar *pTarget = pOwner->GetBaseChar( m_AimedTarget );
	if( !pTarget && !m_bAimedBall )
		return false;
	else if( !pTarget && !pBall )
	{
		m_bAimedBall = false;
		return false;
	}

	D3DXQUATERNION qtRot = pOwner->GetTargetRot();
	D3DXVECTOR3	vStartDir = qtRot * D3DXVECTOR3( 0.0f, 0.0f, -FLOAT1 );

	D3DXVECTOR3 vTargetPos;
	if( pTarget )
	{
		if( !pTarget->IsEnableAimTarget( false ) )
			return false;

		if( pTarget->HasHideBuff() )
			return false;

		if( pTarget->IsChatModeState( false ) )
			return false;

		vTargetPos = pTarget->GetWorldPosition();
	}
	else if( m_bAimedBall && pBall )
	{
		vTargetPos = pBall->GetWorldPosition();
	}

	float fAimRange = m_fAimRange;
	float fAimAngle = m_fAimAngle;

	D3DXVECTOR3 vDiff = vTargetPos - pOwner->GetWorldPosition();
	if( D3DXVec3LengthSq( &vDiff ) > fAimRange * fAimRange )
		return false;

	if( D3DXVec3LengthSq( &vDiff ) < m_fAimMinRange * m_fAimMinRange )
		return false;

	if( bAngleAlsoCheck )
	{
		vDiff.y = 0.0f;
		D3DXVec3Normalize( &vDiff, &vDiff );

		if( m_bChargeCharRotate )
		{
			if( pOwner->IsJumpState() )
			{
				float fHalfCosine = cosf( DEGtoRAD( m_fAimAngle * FLOAT05 ) );
				if( D3DXVec3Dot( &vStartDir, &vDiff ) < fHalfCosine )
					return false;
			}
			else
			{
				D3DXVECTOR3 vCharDir;
				D3DXQUATERNION qtRot = pOwner->GetTargetRot();
				vCharDir = qtRot * D3DXVECTOR3( 0.0f, 0.0f, -FLOAT1 );

				float fHalfCosine = cosf( DEGtoRAD( m_fAimAngle * FLOAT05 ) );
				if( D3DXVec3Dot( &vCharDir, &vDiff ) < fHalfCosine )
					return false;
			}
		}
		else
		{
			float fHalfCosine = cosf( DEGtoRAD( fAimAngle * FLOAT05 ) );
			if( D3DXVec3Dot( &vStartDir, &vDiff ) < fHalfCosine )
				return false;
		}
	}

	return true;
}

void	ioPhoenixBowItem::UpdateAimDir( ioBaseChar *pOwner, bool bJumpAim )
{
	D3DXVECTOR3 vTargetPos;
	if( !m_AimedTarget.IsEmpty() )
	{
		ioBaseChar *pTarget = pOwner->GetBaseChar( m_AimedTarget );
		vTargetPos = pTarget->GetWorldPosition();
		pOwner->SetTargetRotToTargetPos( vTargetPos, false );
	}
}

D3DXVECTOR3	ioPhoenixBowItem::ReCalcMoveDir( ioBaseChar *pOwner, ioWeapon *pWeapon )
{
	ioBaseChar *pTarget = pOwner->GetBaseChar( m_AimedTarget );
	if( !pTarget )
	{
		return	pOwner->GetTargetRot() * D3DXVECTOR3( 0.0f, 0.0f, -FLOAT1 );
	}

	D3DXVECTOR3 vTargetPos;
	D3DXVECTOR3 vWeaponPos = pWeapon->GetPosition();
	vTargetPos = pTarget->GetMidPositionByRate();

	D3DXVECTOR3 vAimedDir = vTargetPos - vWeaponPos;
	D3DXVec3Normalize( &vAimedDir, &vAimedDir );

	return vAimedDir;
}

bool	ioPhoenixBowItem::IsEnoughBullet()
{
	if( m_iNeedBullet <= m_iCurBullet )
		return	true;

	return	false;
}

void	ioPhoenixBowItem::DecreaseBullet()
{
	if( m_iNeedBullet <= m_iCurBullet )
		SetCurBullet( m_iCurBullet - m_iNeedBullet );
	else
		SetCurBullet( 0 );
}