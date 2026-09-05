#include "StdAfx.h"
#include "ioMadBlasterItem.h"
#include "WeaponDefine.h"


ioMadBlasterItem::ioMadBlasterItem(void)
{
	Init();
}

ioMadBlasterItem::ioMadBlasterItem( const ioMadBlasterItem& rhs ) : ioWeaponItem( rhs ),
	m_szDelayAnimation( rhs.m_szDelayAnimation ),
	m_szChargingAnimation( rhs.m_szChargingAnimation ),
	m_szChargeEndAnimation( rhs.m_szChargeEndAnimation ),
	m_szReadyCircleEffect( rhs.m_szReadyCircleEffect ),
	m_szEnableCircleEffect( rhs.m_szEnableCircleEffect ),
	m_fCircleRange( rhs.m_fCircleRange ),
	m_fRadius( rhs.m_fRadius ),
	m_fMoveSpeed( rhs.m_fMoveSpeed ),
	m_fHeightGap( rhs.m_fHeightGap ),
	m_fChargingAngle( rhs.m_fChargingAngle ),
	m_szFirePosEffectFirst( rhs.m_szFirePosEffectFirst ),
	m_szFirePosEffectSecond( rhs.m_szFirePosEffectSecond ),
	m_dwChargeAttackTerm( rhs.m_dwChargeAttackTerm ),
	m_ChargeAttack( rhs.m_ChargeAttack ),
	m_ChargeAttackCharge( rhs.m_ChargeAttackCharge ),
	m_fWeaponGravityRate( rhs.m_fWeaponGravityRate ),
	m_fWeaponFloatPower( rhs.m_fWeaponFloatPower ),
	m_fWeaponFloatMinPower( rhs.m_fWeaponFloatMinPower ),
	m_fWeaponMinSpeed( rhs.m_fWeaponMinSpeed ),
	m_fRevisionRate( rhs.m_fRevisionRate ),
	m_fUpLimitRange( rhs.m_fUpLimitRange ),
	m_fUpLimitHeight( rhs.m_fUpLimitHeight )
{
	Init();
}

ioMadBlasterItem::~ioMadBlasterItem(void)
{
}

void	ioMadBlasterItem::LoadProperty( ioINILoader &rkLoader )
{
	ioWeaponItem::LoadProperty( rkLoader );

	char szBuf[MAX_PATH];
	rkLoader.LoadString( "branch_delay_animation", "", szBuf, MAX_PATH );
	m_szDelayAnimation	= szBuf;
	rkLoader.LoadString( "charging_animation", "", szBuf, MAX_PATH );
	m_szChargingAnimation	= szBuf;
	rkLoader.LoadString( "charge_end_animation", "", szBuf, MAX_PATH );
	m_szChargeEndAnimation	= szBuf;

	rkLoader.LoadString( "ready_circle", "", szBuf, MAX_PATH );
	m_szReadyCircleEffect	= szBuf;
	rkLoader.LoadString( "enable_circle", "", szBuf, MAX_PATH );
	m_szEnableCircleEffect	= szBuf;
	m_fCircleRange	= rkLoader.LoadFloat_e( "magic_circle_range", FLOAT100 );
	m_fRadius		= rkLoader.LoadFloat_e( "magic_circle_radius", FLOAT1000 );
	m_fMoveSpeed	= rkLoader.LoadFloat_e( "magic_circle_move_speed", 350.0f );
	m_fHeightGap	= rkLoader.LoadFloat_e( "magic_circle_height_gap", 20.0f );
	m_fChargingAngle	= rkLoader.LoadFloat_e( "magic_circle_angle", 0.0f );

	rkLoader.LoadString( "circle1_effect", "", szBuf, MAX_PATH );
	m_szFirePosEffectFirst	= szBuf;
	rkLoader.LoadString( "circle2_effect", "", szBuf, MAX_PATH );
	m_szFirePosEffectSecond	= szBuf;

	m_dwChargeAttackTerm	= rkLoader.LoadInt_e( "charge_attack_term", 0 );
	m_fWeaponGravityRate	= rkLoader.LoadFloat_e( "weapon_gravity_rate", FLOAT1 );
	m_fWeaponFloatPower		= rkLoader.LoadFloat_e( "weapon_float_power", FLOAT1 );
	m_fWeaponFloatMinPower	= rkLoader.LoadFloat_e( "weapon_float_min_power", FLOAT1 );
	m_fWeaponMinSpeed		= rkLoader.LoadFloat_e( "weapon_min_speed", 0.0f );
	m_fRevisionRate			= rkLoader.LoadFloat_e( "weapon_revision_rate", 0.0f );
	m_fUpLimitRange			= rkLoader.LoadFloat_e( "up_limit_range", 0.0f );
	m_fUpLimitHeight		= rkLoader.LoadFloat_e( "up_limit_height", 30.0f );

	LoadAttackAttribute( "charge_attack", m_ChargeAttack, rkLoader );
	LoadAttackAttribute( "charge_attack_charge", m_ChargeAttackCharge, rkLoader );
}

ioItem*	ioMadBlasterItem::Clone()
{
	return	new	ioMadBlasterItem( *this );
}

ioWeaponItem::WeaponSubType	ioMadBlasterItem::GetSubType() const
{
	return	WST_MAD_BLASTER_ITEM;
}

void	ioMadBlasterItem::Init()
{
	m_MadBlasterState	= MS_DELAY;
	m_iCurCombo	= 0;
	m_bChargeLoopAniStart	= false;
	m_dwChargeStartTime	= 0;
	m_CircleEffectFirst	= -1;
	m_CircleEffectSecond	= -1;
	m_dwChargeAttackStartTime	= 0;
	m_fChargeAttackMotionEndTime	= 0.0f;
	m_fChargeEndMotionEndTime	= 0.0f;
	m_iCurChargeAttack	= 0;
	m_fLimitLowAngle	= 0.0f;
	m_fLimitHighAngle	= 0.0f;
	m_vFirePos.clear();
}

void	ioMadBlasterItem::CheckNormalAttack( ioBaseChar* pOwner )
{
	switch( m_MadBlasterState )
	{
	case	MS_CHARGING:
		{
			OnCharging( pOwner );
		}
		break;

	case	MS_FIRE:
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

	case	MS_CREATE_CIRCLE1:
		{
			OnCreateCircleFirst( pOwner );
		}
		break;

	case	MS_CREATE_CIRCLE2:
		{
			OnCreateCircleSecond( pOwner );
		}
		break;

	case	MS_CREATE_CIRCLE3:
		{
			OnCreateCircleThird( pOwner );
		}
		break;

	case	MS_PROGRESSIVELY_FIRE:
		{
			OnProgressivelyFire( pOwner );
		}
		break;

	case	MS_VOLLEY_FIRE:
		{
			OnVolleyFire( pOwner );
		}
		break;

	case	MS_READY_TO_FIRE:
		{
			OnReadyToFire( pOwner );
		}
		break;

	case	MS_END_MOTION:
		{
			OnEndMotion( pOwner );
		}
		break;
	}
}

void	ioMadBlasterItem::SetNormalAttack( ioBaseChar* pOwner, int iCurCombo )
{
	ClearCancelInfo();

	DWORD dwCurTime = FRAMEGETTIME();
	int nComboCnt = GetMaxCombo();
	if( !COMPARE( iCurCombo, 0, nComboCnt ) )
	{
		ioWeaponItem::SetNormalAttack( pOwner, iCurCombo );

		pOwner->SetAttackMoveEnable( false );
		pOwner->SetChargingState( false );

		m_iCurCombo	= 0;
		m_MadBlasterState = MS_DELAY;
		m_bChargeLoopAniStart	= false;
	}
	else
	{
		m_iCurCombo	= iCurCombo;
		ChangeToCharging( pOwner );
	}
}

bool	ioMadBlasterItem::SetJumpAttack( ioBaseChar* pOwner, int iAniID, float fTimeRate, AttackType eType )
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

void	ioMadBlasterItem::OnItemGaugeOK( ioBaseChar *pOwner, SP2Packet &rkPacket )
{
	if( !pOwner ) return;

	int iState;
	rkPacket >> iState;
	rkPacket >> m_iCurCombo;
	m_MadBlasterState	= (MadBlasterState)iState;

	switch( iState )
	{
	case	MS_DELAY:
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

	case	MS_CHARGING:
		{
			ChangeToCharging( pOwner );
		}
		break;

	case	MS_FIRE:
		{
			ChangeToFire( pOwner );
		}
		break;

	case	MS_CREATE_CIRCLE1:
		{
			if( !pOwner )
				return;

			ioEntityGroup *pGrp = pOwner->GetGroup();
			if( !pGrp )
				return;

			int iAniID = pGrp->GetAnimationIdx( m_szChargingAnimation );

			pGrp->ClearAllLoopAniExceptOne( iAniID, FLOAT100, true );
			pGrp->ClearAllActionAni( FLOAT10, true );
			pGrp->SetLoopAni( iAniID, FLOAT100 );
			pOwner->SetAttackMoveEnable( false );
			pOwner->ChangeDirectionByInputDir( true );
		}
		break;

	case	MS_PROGRESSIVELY_FIRE:
		{
			D3DXVECTOR3	vPos;
			rkPacket >> vPos;

			OnProgressivelyFireBySync( pOwner, vPos );
		}
		break;

	case	MS_VOLLEY_FIRE:
		{
			m_vFirePos.clear();
			
			D3DXVECTOR3	vPos1, vPos2, vPos3;
			rkPacket >> vPos1;
			m_vFirePos.push_back( vPos1 );
			rkPacket >> vPos2;
			m_vFirePos.push_back( vPos2 );
			rkPacket >> vPos3;
			m_vFirePos.push_back( vPos3 );

			OnVolleyFireBySync( pOwner );
		}
		break;

	case	MS_END_MOTION:
		{
			ChangeToEndMotion( pOwner );
		}
		break;
	}
}

bool	ioMadBlasterItem::CheckWoundAfterUseExtendState( ioBaseChar* pOwner, BlowTargetType eType )
{
	if( !pOwner || !pOwner->IsNeedProcess() )
		return false;

	// 마땅히 처리할 곳이 없어서 여기에서 피격후 이펙트 삭제 처리를 한다.
	if( m_CircleEffectFirst != 0 )
		RemoveEffect( m_CircleEffectFirst, pOwner );

	if( m_CircleEffectSecond != 0 )
		RemoveEffect( m_CircleEffectSecond, pOwner );

	m_vFirePos.clear();
	m_fChargeAttackMotionEndTime	= 0.0f;

	return	false;
}

void	ioMadBlasterItem::OnReleased( ioBaseChar *pOwner )
{
	RemoveEffect( m_CircleEffectFirst, pOwner );
	RemoveEffect( m_CircleEffectSecond, pOwner );

	ioAttackableItem::OnReleased( pOwner );
}

void	ioMadBlasterItem::ChangeToCharging( ioBaseChar* pOwner )
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

	m_MadBlasterState = MS_CHARGING;
	m_dwChargeStartTime	= FRAMEGETTIME();

	if( pOwner->IsNeedSendNetwork() )
	{
		SP2Packet kPacket( CUPK_ITEM_GAUGE_OK );
		FillItemGaugePacketHeader( pOwner, kPacket );
		kPacket << (int)m_MadBlasterState;
		kPacket << m_iCurCombo;
		P2PNetwork::SendToAllPlayingUser( kPacket );
	}
}

void	ioMadBlasterItem::OnCharging( ioBaseChar* pOwner )
{
	if( !pOwner )
		return;

	if( !pOwner->IsNeedProcess() )
		return;

	DWORD dwCurTime = FRAMEGETTIME();
	if( pOwner->IsAttackKeyDown() )
	{
		if( m_dwChargeStartTime + m_dwChargeTime <= dwCurTime )
		{
			ChangeToCreateCircleFirst( pOwner );
		}
	}
	else
	{
		ChangeToFire( pOwner );
	}
}

void	ioMadBlasterItem::ChangeToFire( ioBaseChar* pOwner )
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
	m_MadBlasterState = MS_FIRE;

	if( pOwner->IsNeedSendNetwork() )
	{
		SP2Packet kPacket( CUPK_ITEM_GAUGE_OK );
		FillItemGaugePacketHeader( pOwner, kPacket );
		kPacket << (int)m_MadBlasterState;
		kPacket << m_iCurCombo;
		P2PNetwork::SendToAllPlayingUser( kPacket );
	}
}

void	ioMadBlasterItem::ChangeToCreateCircleFirst( ioBaseChar* pOwner )
{
	if( !pOwner ) return;

	if( !pOwner->IsNeedProcess() )
		return;

	ioEntityGroup *pGrp = pOwner->GetGroup();
	if( !pGrp )
		return;

	m_MadBlasterState	= MS_CREATE_CIRCLE1;

	ioHashString szDummyCircle;
	pOwner->CreateMagicCircle( m_szReadyCircleEffect, m_szEnableCircleEffect, szDummyCircle, m_fMagicCircleOffSet );

	int iAniID = pGrp->GetAnimationIdx( m_szChargingAnimation );

	pGrp->ClearAllLoopAniExceptOne( iAniID, FLOAT100, true );
	pGrp->ClearAllActionAni( FLOAT10, true );
	pGrp->SetLoopAni( iAniID, FLOAT100 );
	pOwner->SetAttackMoveEnable( false );
	pOwner->ChangeDirectionByInputDir( true );

	D3DXQUATERNION	qtOwnerRot = pOwner->GetTargetRot();
	D3DXVECTOR3	angle;

	Quat2Angle( qtOwnerRot, angle );
	float fAngle	= RADtoDEG( angle.y );
	if( fAngle < 0.0f )
	{
		fAngle	= 360.0f - abs( fAngle );
	}

	m_fLimitLowAngle	= fAngle - m_fChargingAngle;
	m_fLimitHighAngle	= fAngle + m_fChargingAngle;

	if( m_fLimitLowAngle < 0.0f )
		m_fLimitLowAngle	+= 360.0f;
	
	if( m_fLimitHighAngle > 360.0f )
		m_fLimitHighAngle	-= 360.0f;

	if( pOwner->IsNeedSendNetwork() )
	{
		SP2Packet kPacket( CUPK_ITEM_GAUGE_OK );
		FillItemGaugePacketHeader( pOwner, kPacket );
		kPacket << (int)m_MadBlasterState;
		kPacket << m_iCurCombo;
		P2PNetwork::SendToAllPlayingUser( kPacket );
	}
}

void	ioMadBlasterItem::OnCreateCircleFirst( ioBaseChar* pOwner )
{
	if( !pOwner ) return;

	if( !pOwner->IsNeedProcess() )
		return;

	if( pOwner->CheckMagicCircle() )
	{
		D3DXVECTOR3 vCharPos = pOwner->GetWorldPosition();
		D3DXVECTOR3 vPos = pOwner->GetMagicCirclePos();
		pOwner->UpdateMagicCirclePos( m_fMoveSpeed, m_fRadius, m_fCircleRange, m_fHeightGap, false, vCharPos );

		D3DXVECTOR3 vCharDir = pOwner->GetMagicCirclePos() - vCharPos;
		vCharDir.y = 0.0f;

		D3DXVec3Normalize( &vCharDir, &vCharDir );

		pOwner->SetTargetRotToDir( vCharDir, true );
		pOwner->ChangeDirectionByInputDir( true );
	}

	if( pOwner->IsAttackKey() )
	{
		ChangeToCreateCircleSecond( pOwner );
	}
}

void	ioMadBlasterItem::ChangeToCreateCircleSecond( ioBaseChar* pOwner )
{
	if( !pOwner ) return;

	if( !pOwner->IsNeedProcess() )
		return;

	m_MadBlasterState	= MS_CREATE_CIRCLE2;

	m_vFirePos.push_back( pOwner->GetMagicCirclePos() );
	pOwner->DestroyMagicCircle();

	ioPlayStage* pStage = pOwner->GetCreator();
	if( pOwner->IsOwnerChar() || ( pStage && pStage->GetOwnerTeam() == pOwner->GetTeam() ) )
		m_CircleEffectFirst	= AddEffect( m_szFirePosEffectFirst, m_vFirePos[0], pOwner );

	ioHashString szDummyCircle;
	pOwner->CreateMagicCircle( m_szReadyCircleEffect, m_szEnableCircleEffect, szDummyCircle, m_vFirePos[0], 0.0f );
}

void	ioMadBlasterItem::OnCreateCircleSecond( ioBaseChar* pOwner )
{
	if( !pOwner ) return;

	if( !pOwner->IsNeedProcess() )
		return;

	if( pOwner->CheckMagicCircle() )
	{
		D3DXVECTOR3 vCharPos = pOwner->GetWorldPosition();
		pOwner->UpdateMagicCirclePos( m_fMoveSpeed, m_fRadius, m_fCircleRange, m_fHeightGap, false, vCharPos );

		D3DXVECTOR3 vCharDir = pOwner->GetMagicCirclePos() - vCharPos;
		vCharDir.y = 0.0f;

		D3DXVec3Normalize( &vCharDir, &vCharDir );

		pOwner->SetTargetRotToDir( vCharDir, true );
		pOwner->ChangeDirectionByInputDir( true );
	}

	if( pOwner->IsAttackKey() )
	{
		ChangeToCreateCircleThird( pOwner );
	}
}

void	ioMadBlasterItem::ChangeToCreateCircleThird( ioBaseChar* pOwner )
{
	if( !pOwner ) return;

	if( !pOwner->IsNeedProcess() )
		return;

	m_MadBlasterState	= MS_CREATE_CIRCLE3;

	m_vFirePos.push_back( pOwner->GetMagicCirclePos() );
	pOwner->DestroyMagicCircle();

	ioPlayStage* pStage = pOwner->GetCreator();
	if( pOwner->IsOwnerChar() || ( pStage && pStage->GetOwnerTeam() == pOwner->GetTeam() ) )
		m_CircleEffectSecond	= AddEffect( m_szFirePosEffectSecond, m_vFirePos[1], pOwner );

	ioHashString szDummyCircle;
	pOwner->CreateMagicCircle( m_szReadyCircleEffect, m_szEnableCircleEffect, szDummyCircle, m_vFirePos[1], 0.0f );
}

void	ioMadBlasterItem::OnCreateCircleThird( ioBaseChar* pOwner )
{
	if( !pOwner ) return;

	if( !pOwner->IsNeedProcess() )
		return;

	if( pOwner->CheckMagicCircle() )
	{
		D3DXVECTOR3 vCharPos = pOwner->GetWorldPosition();
		pOwner->UpdateMagicCirclePos( m_fMoveSpeed, m_fRadius, m_fCircleRange, m_fHeightGap, false, vCharPos );

		D3DXVECTOR3 vCharDir = pOwner->GetMagicCirclePos() - vCharPos;
		vCharDir.y = 0.0f;

		D3DXVec3Normalize( &vCharDir, &vCharDir );

		pOwner->SetTargetRotToDir( vCharDir, true );
		pOwner->ChangeDirectionByInputDir( true );
	}

	if( pOwner->IsAttackKey() )
	{
		ChangeToReadyToFire( pOwner );
	}
}

void	ioMadBlasterItem::ChangeToReadyToFire( ioBaseChar* pOwner )
{
	if( !pOwner ) return;

	if( !pOwner->IsNeedProcess() )
		return;

	m_MadBlasterState	= MS_READY_TO_FIRE;
	m_dwChargeStartTime	= FRAMEGETTIME();
}

void	ioMadBlasterItem::OnReadyToFire( ioBaseChar* pOwner )
{
	if( !pOwner ) return;

	if( !pOwner->IsNeedProcess() )
		return;

	DWORD dwCurTime = FRAMEGETTIME();

	if( pOwner->IsAttackKeyDown() )
	{
		if( m_dwChargeStartTime + m_dwChargeTime <= dwCurTime )
		{
			ChangeToVolleyFire( pOwner );
		}
	}
	else
	{
		ChangeToProgressivelyFire( pOwner );
	}
}

void	ioMadBlasterItem::ChangeToProgressivelyFire( ioBaseChar* pOwner )
{
	if( !pOwner ) return;

	if( !pOwner->IsNeedProcess() )
		return;

	m_MadBlasterState	= MS_PROGRESSIVELY_FIRE;
	m_dwChargeAttackStartTime	= FRAMEGETTIME();
	m_iCurChargeAttack	= 0;

	m_vFirePos.push_back( pOwner->GetMagicCirclePos() );
	pOwner->DestroyMagicCircle();

	RemoveEffect( m_CircleEffectFirst, pOwner );
	RemoveEffect( m_CircleEffectSecond, pOwner );
}

void	ioMadBlasterItem::OnProgressivelyFire( ioBaseChar* pOwner )
{
	if( !pOwner ) return;

	if( !pOwner->IsNeedProcess() )
		return;

	ioEntityGroup *pGrp = pOwner->GetGroup();
	if( !pGrp )
		return;

	DWORD dwCurTime = FRAMEGETTIME();

	if( m_iCurChargeAttack >= (int)m_vFirePos.size() )
	{
		if( m_fChargeAttackMotionEndTime != 0.0f && m_fChargeAttackMotionEndTime < dwCurTime )
			ChangeToEndMotion( pOwner );

		return;
	}

	if( m_dwChargeAttackStartTime + m_dwChargeAttackTerm < dwCurTime )
	{
		DWORD dwWeaponBaseIndex = pOwner->IncreaseWeaponIndexBase();
		D3DXVECTOR3 vPos = pOwner->GetMidPositionByRate( 1.0f );
		D3DXVECTOR3 vDir	= m_vFirePos[m_iCurChargeAttack] - pOwner->GetWorldPosition();

		D3DXVECTOR3	vWeaponDir;
		D3DXVec3Normalize( &vWeaponDir, &vDir );	

		FireTime kFireTime;
		kFireTime.dwStart	= FRAMEGETTIME();

		kFireTime.iAttributeIdx			= m_ChargeAttack.m_vWeaponInfoList[0].m_iWeaponIdx;
		kFireTime.iResistanceIdx		= m_ChargeAttack.m_vWeaponInfoList[0].m_iResistanceIdx;
		kFireTime.szWoundedAni			= m_ChargeAttack.m_vWeaponInfoList[0].m_WoundedAnimation;
		kFireTime.dwWoundedAniDuration	= m_ChargeAttack.m_vWeaponInfoList[0].m_dwWoundedDuration;
		kFireTime.bLoopWoundedAni		= m_ChargeAttack.m_vWeaponInfoList[0].m_bWoundedLoopAni;

		kFireTime.dwWeaponIdx			= dwWeaponBaseIndex++;
		kFireTime.eFireTimeType			= FTT_NORMAL_ATTACK;

		ioWeapon*	pWeapon	= pOwner->ExtendFireExplicit( kFireTime, vPos, vWeaponDir, "" );
		ioFloatThrowWeapon*	pFloatWeapon	= ToFloatThrowWeapon( pWeapon );
		if( pFloatWeapon )
		{
			float	fSpeed	= 0.0f;
			float	fFloating	= 0.0f;
			GetChargingAttackSpeedNFloatingPower( pOwner, m_vFirePos[m_iCurChargeAttack], vDir, fSpeed, fFloating );

			pFloatWeapon->SetMoveSpeed( fSpeed );
			pFloatWeapon->SetFloatPower( fFloating );
			pFloatWeapon->SetGravityRate( m_fWeaponGravityRate );
		}

		m_dwChargeAttackStartTime	= FRAMEGETTIME();

		// 에니메이션 처리
		int	iAniID = pGrp->GetAnimationIdx( m_ChargeAttack.m_AttackAnimation );
		if( iAniID == -1 )
			return;

		float	fTimeRate   = m_ChargeAttack.m_fAttackAniRate;
		DWORD	dwPreDelay  = m_ChargeAttack.m_dwPreDelay;

		pGrp->ClearAllLoopAni( FLOAT100, true );
		pGrp->ClearAllActionAni( FLOAT100, true );

		DWORD	dwFireEndTime	= pGrp->GetAnimationFullTime( iAniID ) * fTimeRate + dwPreDelay;

		if( m_ChargeAttack.m_AttackEndAnimation.IsEmpty() )
		{
			pGrp->SetActionAni( iAniID, FLOAT100, FLOAT100, FLOAT1, FLOAT1/fTimeRate, 0.0f, dwPreDelay, true );
		}
		else
		{
			pGrp->SetActionAni( iAniID, FLOAT100, 0.0f, FLOAT1, FLOAT1/fTimeRate, 0.0f, dwPreDelay );

			pGrp->SetActionAni( m_ChargeAttack.m_AttackEndAnimation,
				0.0f, FLOAT100,
				1.0f, FLOAT1/m_ChargeAttack.m_fAttackEndAniRate,
				dwFireEndTime, 0.0f, true );
		}

		pOwner->SetTargetRotToDir( vWeaponDir, true );

		m_fChargeAttackMotionEndTime	= dwCurTime + dwFireEndTime;

		if( pOwner->IsNeedSendNetwork() )
		{
			SP2Packet kPacket( CUPK_ITEM_GAUGE_OK );
			FillItemGaugePacketHeader( pOwner, kPacket );
			kPacket << (int)m_MadBlasterState;
			kPacket << m_iCurCombo;
			kPacket << m_vFirePos[m_iCurChargeAttack];
			P2PNetwork::SendToAllPlayingUser( kPacket );
		}

		m_iCurChargeAttack++;
	}
}

void	ioMadBlasterItem::ChangeToVolleyFire( ioBaseChar* pOwner )
{
	if( !pOwner ) return;

	if( !pOwner->IsNeedProcess() )
		return;

	m_MadBlasterState	= MS_VOLLEY_FIRE;

	m_vFirePos.push_back( pOwner->GetMagicCirclePos() );
	pOwner->DestroyMagicCircle();

	RemoveEffect( m_CircleEffectFirst, pOwner );
	RemoveEffect( m_CircleEffectSecond, pOwner );
}

void	ioMadBlasterItem::OnVolleyFire( ioBaseChar* pOwner )
{
	if( !pOwner ) return;

	if( !pOwner->IsNeedProcess() )
		return;

	ioEntityGroup *pGrp = pOwner->GetGroup();
	if( !pGrp )
		return;

	DWORD	currentTime	= FRAMEGETTIME();
	if( m_fChargeAttackMotionEndTime != 0.0f )
	{
		if( m_fChargeAttackMotionEndTime < currentTime )
			ChangeToEndMotion( pOwner );
		return;
	}

	for( int i = 0; i < (int)m_vFirePos.size(); i++ )
	{
		DWORD dwWeaponBaseIndex = pOwner->IncreaseWeaponIndexBase();
		D3DXVECTOR3 vPos = pOwner->GetMidPositionByRate( 1.0f );
		D3DXVECTOR3 vDir	= m_vFirePos[i] - pOwner->GetWorldPosition();

		D3DXVECTOR3	vWeaponDir;
		D3DXVec3Normalize( &vWeaponDir, &vDir );	

		FireTime kFireTime;
		kFireTime.dwStart	= FRAMEGETTIME();

		kFireTime.iAttributeIdx			= m_ChargeAttackCharge.m_vWeaponInfoList[0].m_iWeaponIdx;
		kFireTime.iResistanceIdx		= m_ChargeAttackCharge.m_vWeaponInfoList[0].m_iResistanceIdx;
		kFireTime.szWoundedAni			= m_ChargeAttackCharge.m_vWeaponInfoList[0].m_WoundedAnimation;
		kFireTime.dwWoundedAniDuration	= m_ChargeAttackCharge.m_vWeaponInfoList[0].m_dwWoundedDuration;
		kFireTime.bLoopWoundedAni		= m_ChargeAttackCharge.m_vWeaponInfoList[0].m_bWoundedLoopAni;

		kFireTime.dwWeaponIdx			= dwWeaponBaseIndex++;
		kFireTime.eFireTimeType			= FTT_NORMAL_ATTACK;

		ioWeapon*	pWeapon	= pOwner->ExtendFireExplicit( kFireTime, vPos, vWeaponDir, "" );
		ioFloatThrowWeapon*	pFloatWeapon	= ToFloatThrowWeapon( pWeapon );
		if( pFloatWeapon )
		{
			float	fSpeed	= 0.0f;
			float	fFloating	= 0.0f;
			GetChargingAttackSpeedNFloatingPower( pOwner, m_vFirePos[i], vDir, fSpeed, fFloating );

			pFloatWeapon->SetMoveSpeed( fSpeed );
			pFloatWeapon->SetFloatPower( fFloating );
			pFloatWeapon->SetGravityRate( m_fWeaponGravityRate );
		}
	}

	// 에니메이션 처리
	int	iAniID = pGrp->GetAnimationIdx( m_ChargeAttackCharge.m_AttackAnimation );
	if( iAniID == -1 )
		return;

	float	fTimeRate   = m_ChargeAttackCharge.m_fAttackAniRate;
	DWORD	dwPreDelay  = m_ChargeAttackCharge.m_dwPreDelay;

	pGrp->ClearAllLoopAni( FLOAT100, true );
	pGrp->ClearAllActionAni( FLOAT100, true );

	DWORD	dwFireEndTime	= pGrp->GetAnimationFullTime( iAniID ) * fTimeRate + dwPreDelay;

	if( m_ChargeAttackCharge.m_AttackEndAnimation.IsEmpty() )
	{
		pGrp->SetActionAni( iAniID, FLOAT100, FLOAT100, FLOAT1, FLOAT1/fTimeRate, 0.0f, dwPreDelay, true );
	}
	else
	{
		pGrp->SetActionAni( iAniID, FLOAT100, 0.0f, FLOAT1, FLOAT1/fTimeRate, 0.0f, dwPreDelay );

		pGrp->SetActionAni( m_ChargeAttackCharge.m_AttackEndAnimation,
			0.0f, FLOAT100,
			1.0f, FLOAT1/m_ChargeAttackCharge.m_fAttackEndAniRate,
			dwFireEndTime, 0.0f, true );
	}

	m_fChargeAttackMotionEndTime	= currentTime + dwFireEndTime;

	if( pOwner->IsNeedSendNetwork() )
	{
		SP2Packet kPacket( CUPK_ITEM_GAUGE_OK );
		FillItemGaugePacketHeader( pOwner, kPacket );
		kPacket << (int)m_MadBlasterState;
		kPacket << m_iCurCombo;
		kPacket << m_vFirePos[0];
		kPacket << m_vFirePos[1];
		kPacket << m_vFirePos[2];
		P2PNetwork::SendToAllPlayingUser( kPacket );
	}

	//ChangeToEndMotion( pOwner );
}

void	ioMadBlasterItem::ChangeToEndMotion( ioBaseChar *pOwner )
{
	if( !pOwner ) return;

	//if( !pOwner->IsNeedProcess() )
		//return;

	ioEntityGroup *pGrp = pOwner->GetGroup();
	if( !pGrp )
		return;

	m_MadBlasterState	= MS_END_MOTION;

	int iAniID = pGrp->GetAnimationIdx( m_szChargeEndAnimation );

	pGrp->ClearAllLoopAniExceptOne( iAniID, FLOAT100, true );
	pGrp->ClearAllActionAni( FLOAT10, true );
	pGrp->SetLoopAni( iAniID, FLOAT100 );

	m_fChargeEndMotionEndTime	= pGrp->GetAnimationFullTime( iAniID ) + (float)FRAMEGETTIME();

	if( pOwner->IsNeedSendNetwork() )
	{
		SP2Packet kPacket( CUPK_ITEM_GAUGE_OK );
		FillItemGaugePacketHeader( pOwner, kPacket );
		kPacket << (int)m_MadBlasterState;
		kPacket << m_iCurCombo;
		P2PNetwork::SendToAllPlayingUser( kPacket );
	}
}

void	ioMadBlasterItem::OnEndMotion( ioBaseChar* pOwner )
{
	if( !pOwner ) return;

	//if( !pOwner->IsNeedProcess() )
		//return;

	if( m_fChargeEndMotionEndTime < (float)FRAMEGETTIME() )
		ChangeToDelay( pOwner );
}

void	ioMadBlasterItem::ChangeToDelay( ioBaseChar* pOwner )
{
	pOwner->SetState( CS_DELAY );
	m_MadBlasterState	= MS_DELAY;
	m_fChargeAttackMotionEndTime	= 0.0f;
	m_vFirePos.clear();
}

void	ioMadBlasterItem::OnProgressivelyFireBySync( ioBaseChar* pOwner, D3DXVECTOR3 vTargetPos )
{
	if( !pOwner ) return;

	ioEntityGroup *pGrp = pOwner->GetGroup();
	if( !pGrp )
		return;

	DWORD dwWeaponBaseIndex = pOwner->IncreaseWeaponIndexBase();
	D3DXVECTOR3 vPos = pOwner->GetMidPositionByRate( 1.0f );
	D3DXVECTOR3 vDir	= vTargetPos - pOwner->GetWorldPosition();

	D3DXVECTOR3	vWeaponDir;
	D3DXVec3Normalize( &vWeaponDir, &vDir );	

	FireTime kFireTime;
	kFireTime.dwStart	= FRAMEGETTIME();

	kFireTime.iAttributeIdx			= m_ChargeAttack.m_vWeaponInfoList[0].m_iWeaponIdx;
	kFireTime.iResistanceIdx		= m_ChargeAttack.m_vWeaponInfoList[0].m_iResistanceIdx;
	kFireTime.szWoundedAni			= m_ChargeAttack.m_vWeaponInfoList[0].m_WoundedAnimation;
	kFireTime.dwWoundedAniDuration	= m_ChargeAttack.m_vWeaponInfoList[0].m_dwWoundedDuration;
	kFireTime.bLoopWoundedAni		= m_ChargeAttack.m_vWeaponInfoList[0].m_bWoundedLoopAni;

	kFireTime.dwWeaponIdx			= dwWeaponBaseIndex++;
	kFireTime.eFireTimeType			= FTT_NORMAL_ATTACK;

	ioWeapon*	pWeapon	= pOwner->ExtendFireExplicit( kFireTime, vPos, vWeaponDir, "" );
	ioFloatThrowWeapon*	pFloatWeapon	= ToFloatThrowWeapon( pWeapon );
	if( pFloatWeapon )
	{
		float	fSpeed	= 0.0f;
		float	fFloating	= 0.0f;
		GetChargingAttackSpeedNFloatingPower( pOwner, vTargetPos, vDir, fSpeed, fFloating );

		pFloatWeapon->SetMoveSpeed( fSpeed );
		pFloatWeapon->SetFloatPower( fFloating );
		pFloatWeapon->SetGravityRate( m_fWeaponGravityRate );
	}

	// 에니메이션 처리
	int	iAniID = pGrp->GetAnimationIdx( m_ChargeAttack.m_AttackAnimation );
	if( iAniID == -1 )
		return;

	float	fTimeRate   = m_ChargeAttack.m_fAttackAniRate;
	DWORD	dwPreDelay  = m_ChargeAttack.m_dwPreDelay;

	pGrp->ClearAllLoopAni( FLOAT100, true );
	pGrp->ClearAllActionAni( FLOAT100, true );

	DWORD	dwFireEndTime	= pGrp->GetAnimationFullTime( iAniID ) * fTimeRate + dwPreDelay;

	if( m_ChargeAttack.m_AttackEndAnimation.IsEmpty() )
	{
		pGrp->SetActionAni( iAniID, FLOAT100, FLOAT100, FLOAT1, FLOAT1/fTimeRate, 0.0f, dwPreDelay, true );
	}
	else
	{
		pGrp->SetActionAni( iAniID, FLOAT100, 0.0f, FLOAT1, FLOAT1/fTimeRate, 0.0f, dwPreDelay );

		pGrp->SetActionAni( m_ChargeAttack.m_AttackEndAnimation,
			0.0f, FLOAT100,
			1.0f, FLOAT1/m_ChargeAttack.m_fAttackEndAniRate,
			dwFireEndTime, 0.0f, true );
	}

	pOwner->SetTargetRotToDir( vWeaponDir, true );
}

void	ioMadBlasterItem::OnVolleyFireBySync( ioBaseChar* pOwner )
{
	if( !pOwner ) return;

	ioEntityGroup *pGrp = pOwner->GetGroup();
	if( !pGrp )
		return;

	for( int i = 0; i < (int)m_vFirePos.size(); i++ )
	{
#ifndef SHIPPING
		LOG.PrintConsoleLog( 0, "ioMadBlasterItem::OnVolleyFireBySync - i = %d", i );
#endif
		DWORD dwWeaponBaseIndex = pOwner->IncreaseWeaponIndexBase();
		D3DXVECTOR3 vPos = pOwner->GetMidPositionByRate( 1.0f );
		D3DXVECTOR3 vDir	= m_vFirePos[i] - pOwner->GetWorldPosition();

		D3DXVECTOR3	vWeaponDir;
		D3DXVec3Normalize( &vWeaponDir, &vDir );	

		FireTime kFireTime;
		kFireTime.dwStart	= FRAMEGETTIME();

		kFireTime.iAttributeIdx			= m_ChargeAttackCharge.m_vWeaponInfoList[0].m_iWeaponIdx;
		kFireTime.iResistanceIdx		= m_ChargeAttackCharge.m_vWeaponInfoList[0].m_iResistanceIdx;
		kFireTime.szWoundedAni			= m_ChargeAttackCharge.m_vWeaponInfoList[0].m_WoundedAnimation;
		kFireTime.dwWoundedAniDuration	= m_ChargeAttackCharge.m_vWeaponInfoList[0].m_dwWoundedDuration;
		kFireTime.bLoopWoundedAni		= m_ChargeAttackCharge.m_vWeaponInfoList[0].m_bWoundedLoopAni;

		kFireTime.dwWeaponIdx			= dwWeaponBaseIndex++;
		kFireTime.eFireTimeType			= FTT_NORMAL_ATTACK;

		ioWeapon*	pWeapon	= pOwner->ExtendFireExplicit( kFireTime, vPos, vWeaponDir, "" );
		ioFloatThrowWeapon*	pFloatWeapon	= ToFloatThrowWeapon( pWeapon );
		if( pFloatWeapon )
		{
			float	fSpeed	= 0.0f;
			float	fFloating	= 0.0f;
			GetChargingAttackSpeedNFloatingPower( pOwner, m_vFirePos[i], vDir, fSpeed, fFloating );

			pFloatWeapon->SetMoveSpeed( fSpeed );
			pFloatWeapon->SetFloatPower( fFloating );
			pFloatWeapon->SetGravityRate( m_fWeaponGravityRate );
		}
	}

	// 에니메이션 처리
	int	iAniID = pGrp->GetAnimationIdx( m_ChargeAttackCharge.m_AttackAnimation );
	if( iAniID == -1 )
		return;

	float	fTimeRate   = m_ChargeAttackCharge.m_fAttackAniRate;
	DWORD	dwPreDelay  = m_ChargeAttackCharge.m_dwPreDelay;

	pGrp->ClearAllLoopAni( FLOAT100, true );
	pGrp->ClearAllActionAni( FLOAT100, true );

	DWORD	dwFireEndTime	= pGrp->GetAnimationFullTime( iAniID ) * fTimeRate + dwPreDelay;

	if( m_ChargeAttackCharge.m_AttackEndAnimation.IsEmpty() )
	{
		pGrp->SetActionAni( iAniID, FLOAT100, FLOAT100, FLOAT1, FLOAT1/fTimeRate, 0.0f, dwPreDelay, true );
	}
	else
	{
		pGrp->SetActionAni( iAniID, FLOAT100, 0.0f, FLOAT1, FLOAT1/fTimeRate, 0.0f, dwPreDelay );

		pGrp->SetActionAni( m_ChargeAttackCharge.m_AttackEndAnimation,
			0.0f, FLOAT100,
			1.0f, FLOAT1/m_ChargeAttackCharge.m_fAttackEndAniRate,
			dwFireEndTime, 0.0f, true );
	}
}

DWORD	ioMadBlasterItem::AddEffect( const ioHashString& szEffectName, D3DXVECTOR3 vPos, ioBaseChar *pOwner )
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

	return pMapEffect->GetUniqueID();
}

void	ioMadBlasterItem::RemoveEffect( DWORD& rhID, ioBaseChar *pOwner )
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

void	ioMadBlasterItem::GetChargingAttackSpeedNFloatingPower( ioBaseChar *pOwner, D3DXVECTOR3& target, D3DXVECTOR3& dir, float& speed, float& floatingPower )
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

float	ioMadBlasterItem::GetFlightDuration( float fCurGravity, float fCurWeaponFloatPower, float fGapHeight )
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

float	ioMadBlasterItem::GetTargetTime( IN float fCurGravity, IN float fGapHeight, OUT float &fFloatPower )
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