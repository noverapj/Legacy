

#include "stdafx.h"

#include "ioValkyrie.h"

#include "ioPlayStage.h"
#include "ioBaseChar.h"

#include "WeaponDefine.h"

ioValkyrie::ioValkyrie()
{
	Init();
}

ioValkyrie::ioValkyrie( const ioValkyrie &rhs )
	: ioWeaponItem( rhs ),		
	m_fNeedGauge( rhs.m_fNeedGauge ),
	m_fReUpNeedGauge( rhs.m_fReUpNeedGauge ),
	m_PreGatherAnimationList( rhs.m_PreGatherAnimationList ),
	m_ValkyrieUpInfo( rhs.m_ValkyrieUpInfo ),
	m_ValkyrieDownInfo( rhs.m_ValkyrieDownInfo ),
	m_ValkyrieReUpInfo( rhs.m_ValkyrieReUpInfo ),	
	m_LandAttribute( rhs.m_LandAttribute ),
	m_fTargetFindAngle( rhs.m_fTargetFindAngle ),
	m_fTargetFindMinRange( rhs.m_fTargetFindMinRange ),
	m_fTargetFindMaxAngle( rhs.m_fTargetFindMaxAngle ),
	m_bDownTarget( rhs.m_bDownTarget ),
	m_fChargeEndNextJumpAmt( rhs.m_fChargeEndNextJumpAmt ),
	m_fHighDecreaseHeight( rhs.m_fHighDecreaseHeight ),
	m_fHighDecreaseGauge( rhs.m_fHighDecreaseGauge ),
	m_szDoubleJumpFlyBuff( rhs.m_szDoubleJumpFlyBuff ),
	m_iBuffUseEnableJumpCount( rhs.m_iBuffUseEnableJumpCount ),
	m_ChargeEndNextJumpRemoveBuffList( rhs.m_ChargeEndNextJumpRemoveBuffList )
{
	Init();
}

ioValkyrie::~ioValkyrie()
{
}

void ioValkyrie::Init()
{
	m_ChargeState			= CS_NONE;
	m_FlyState				= FS_NONE;

	m_dwAttackStartTime		= 0;
	m_dwWaitTime			= 0;
	m_dwUpStartTime			= 0;
	m_dwDownStartTime		= 0;
	m_dwKeyReserveTime		= 0;

	m_iCurCombo				= 0;
	m_iCurAirJump			= 0;

	m_fAirJumpPowerRate		= 0.0f;
	m_fCurExtraGauge		= 0.0f;
	m_fUpCurrRange			= 0.0f;
	m_fDownCurrRange		= 0.0f;

	m_dwCurrWeaponIdx		= 0;

}

void ioValkyrie::LoadProperty( ioINILoader &rkLoader )
{
	ioWeaponItem::LoadProperty( rkLoader );

	char szBuf[MAX_PATH];

	m_fNeedGauge = rkLoader.LoadFloat_e( "need_extra_gauge", 0 );
	m_fReUpNeedGauge = rkLoader.LoadFloat_e( "need_reup_gauge", 0 );

	int iPreCount = rkLoader.LoadInt_e( "pre_gather_animation_cnt", 0 );
	for( int i = 0; i < iPreCount; ++i )
	{
		wsprintf_e( szBuf, "pre_gather_animation%d", i+1 );
		rkLoader.LoadString( szBuf, "", szBuf, MAX_PATH );
		m_PreGatherAnimationList.push_back( ioHashString( szBuf ) );
	}

	LoadFlyInfo( rkLoader, "up_fly", m_ValkyrieUpInfo );
	LoadFlyInfo( rkLoader, "down_fly", m_ValkyrieDownInfo );
	LoadFlyInfo( rkLoader, "re_up_fly", m_ValkyrieReUpInfo );

	m_fTargetFindAngle			= rkLoader.LoadFloat_e( "target_find_angle", 0 );
	m_fTargetFindMinRange		= rkLoader.LoadFloat_e( "target_find_min_range", 0 );
	m_fTargetFindMaxAngle		= rkLoader.LoadFloat_e( "target_find_max_range", 0 );
	m_bDownTarget				= rkLoader.LoadBool_e( "target_find_down_char", 0 );

	m_fChargeEndNextJumpAmt		= rkLoader.LoadFloat_e( "charge_end_next_jump_amt", FLOAT1000 );
	m_fHighDecreaseHeight		= rkLoader.LoadFloat_e( "high_decrease_height", FLOAT100 );
	m_fHighDecreaseGauge		= rkLoader.LoadFloat_e( "high_decrease_gauge", 3.0f );

	wsprintf_e( szBuf, "land" );
	LoadAttackAttribute( szBuf, m_LandAttribute, rkLoader );

	rkLoader.LoadString_e( "double_jump_fly_buff", "", szBuf, MAX_PATH );	
	m_szDoubleJumpFlyBuff = szBuf;

	m_iBuffUseEnableJumpCount	= rkLoader.LoadInt_e( "fly_buff_use_enable_jump_cnt", 1 );

	int iBuffCount = rkLoader.LoadInt_e( "charge_end_next_jump_remove_buff_cnt", 1 );
	for( int i = 0; i < iBuffCount; ++i )
	{
		wsprintf_e( szBuf, "charge_end_next_jump_remove_buff%d", i+1 );
		rkLoader.LoadString( szBuf, "", szBuf, MAX_PATH );
		m_ChargeEndNextJumpRemoveBuffList.push_back( ioHashString( szBuf ) );		
	}
}

void ioValkyrie::LoadFlyInfo( ioINILoader &rkLoader, const char* szName, ValkyrieFlyInfo& DestInfo )
{
	DestInfo.Init();

	char szBuf[MAX_PATH];
	char szKey[MAX_PATH];

	wsprintf_e( szKey, "%s_wait_ani", szName );
	rkLoader.LoadString( szKey, "", szBuf, MAX_PATH );
	DestInfo.m_WaitAnimation = szBuf;

	wsprintf_e( szKey, "%s_wait_duration", szName );
	DestInfo.m_dwWaitDuration = (DWORD)rkLoader.LoadInt( szKey, 0 );

	wsprintf_e( szKey, "%s_wait_duration", szName );
	DestInfo.m_dwChargeFlyMinSpeedDuration = (DWORD)rkLoader.LoadInt(szKey, 0 );

	wsprintf_e( szKey, "%s_rotate_speed", szName );
	DestInfo.m_dwRotateTime = (DWORD)rkLoader.LoadInt( szKey, 0 );

	wsprintf_e( szKey, "%s_angle", szName );
	DestInfo.m_fChargeFlyAngle = rkLoader.LoadFloat( szKey, 0.0f );

	wsprintf_e( szKey, "%s_max_range", szName );
	DestInfo.m_fChargeFlyMaxRange = rkLoader.LoadFloat( szKey, 0.0f );

	wsprintf_e( szKey, "%s_max_speed", szName );
	DestInfo.m_fChargeFlyMaxSpeed = rkLoader.LoadFloat( szKey, 0.0f );

	wsprintf_e( szKey, "%s_min_speed", szName );
	DestInfo.m_fChargeFlyMinSpeed = rkLoader.LoadFloat( szKey, 0.0f );

	wsprintf_e( szKey, "%s_accel_speed", szName );
	DestInfo.m_fChargeFlyAccelSpeed = rkLoader.LoadFloat( szKey, 0.0f );

	wsprintf_e( szKey, "%s_jump_amt", szName );
	DestInfo.m_fChargeFlyJumpAmt = rkLoader.LoadFloat( szKey, 0.0f );

	wsprintf_e( szKey, "%s_attack", szName );
	LoadAttackAttribute( szKey, DestInfo.m_Attribute, rkLoader );
}

ioItem* ioValkyrie::Clone()
{
	return new ioValkyrie( *this );
}

ioWeaponItem::WeaponSubType ioValkyrie::GetSubType() const
{
	return WST_VALKYRIE;
}

const char* ioValkyrie::GetAttackStateText() const
{
	if( m_ChargeState == CS_FLY )
	{
		switch( m_FlyState )
		{
		case FS_NONE:
			return "FS_NONE";
		case FS_WAIT:
			return "FS_WAIT";
		case FS_UP:
			return "FS_UP";
		case FS_TARGET_FIND:
			return "FS_TARGET_FIND";
		case FS_DOWN:
			return "FS_DOWN";
		case FS_REUP:
			return "FS_REUP";
		case FS_LANDING:
			return "FS_LANDING";
		case FS_END:
			return "FS_END";
		}
	}

	return ioAttackableItem::GetAttackStateText();
}

void ioValkyrie::OnReleased( ioBaseChar *pOwner )
{
	ioAttackableItem::OnReleased( pOwner );

	if( pOwner )
	{
		ioBuff* pBuff = pOwner->GetBuff( GetDoubleJumpFlyBuff() );
		if( pBuff )
			pBuff->SetReserveEndBuff();
	}

	ClearState( pOwner );
}

void ioValkyrie::ClearWeapon( ioBaseChar* pOwner )
{
	ioZoneEffectWeapon* pZone = ToZoneEffectWeapon( g_WeaponMgr.FindWeapon( pOwner, m_dwCurrWeaponIdx ) );
	if( pZone )
		pZone->SetWeaponDead();
}

void ioValkyrie::ClearState( ioBaseChar* pOwner )
{
	ClearWeapon( pOwner );

	m_ChargeState		= CS_NONE;
	m_FlyState			= FS_NONE;
	m_dwCurrWeaponIdx	= 0;
	m_dwKeyReserveTime	= 0;
}

void ioValkyrie::SetNormalAttack( ioBaseChar *pOwner, int iCurCombo )
{
	ClearCancelInfo();

	int iPreGatherCnt = m_PreGatherAnimationList.size();

	m_ChargeState		= CS_CHARGING;
	m_dwAttackStartTime	= FRAMEGETTIME();
	m_iCurCombo			= iCurCombo;

	pOwner->SetState( CS_ATTACK );
	pOwner->SetChargingState( true );
	pOwner->SetAttackMoveEnable( false );

	ioEntityGroup *pGrp = pOwner->GetGroup();
	pGrp->ClearAllActionAni( FLOAT100, true );
	pGrp->ClearAllLoopAni( FLOAT100, true );

	if( 0 < iPreGatherCnt && COMPARE( iCurCombo, 0, iPreGatherCnt ) )
	{
		int iAniID = pGrp->GetAnimationIdx( m_PreGatherAnimationList[iCurCombo] );
		float fTimeRate = (float)m_dwChargeTime / pGrp->GetAnimationFullTime( iAniID );
		pGrp->SetActionAni( iAniID, FLOAT100, FLOAT100, FLOAT1, FLOAT1/fTimeRate, 0.0f, 0.0f, true );
	}
}

void ioValkyrie::CheckNormalAttack( ioBaseChar *pOwner )
{
	DWORD dwCurTime = FRAMEGETTIME();

	switch( m_ChargeState )
	{
	case CS_CHARGING:
		{
			OnCharging( pOwner );
		}
		break;
	case CS_ATTACK_FIRE:
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
	}
}

void ioValkyrie::OnCharging( ioBaseChar *pOwner )
{
	if( !pOwner->IsNeedProcess() )
		return;

	if( pOwner->IsAttackKeyDown() && GetNeedBullet() <= GetCurBullet() )
	{
		if( m_dwAttackStartTime + m_dwChargeTime <= FRAMEGETTIME() )
		{
			ChangeToSpecialState( pOwner );
		}
	}
	else
	{
		ChangeToAttackFire( pOwner );
	}
}

void ioValkyrie::ChangeToAttackFire( ioBaseChar *pOwner )
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

		m_szCurAni		= pAttribute->m_AttackAnimation;
		m_fCurAniRate	= pAttribute->m_fAttackAniRate;

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

void ioValkyrie::ChangeToSpecialState( ioBaseChar *pOwner )
{
	if( !pOwner )
		return;

	pOwner->ClearValkyrieDobuleJumpFly();
	pOwner->SetState( CS_VALKYRIE_FLY );

	if( pOwner->IsNeedProcess() )
	{
		D3DXQUATERNION qtRot = pOwner->GetTargetRot();
		pOwner->SetTargetRotToRotate( qtRot, true );
	}

	if( m_ValkyrieUpInfo.m_WaitAnimation.IsEmpty() )
	{
		ChangeToUpState( pOwner );
	}
	else
	{
		ChangeToWaitState( pOwner );
	}

	if( pOwner->IsNeedSendNetwork() )
	{
		SP2Packet kPacket( CUPK_ITEM_GAUGE_OK );
		FillItemGaugePacketHeader( pOwner, kPacket );
		kPacket << static_cast<byte>( CS_FLY );
		kPacket << static_cast<byte>( FS_NONE );
		kPacket << pOwner->GetWorldPosition();
		kPacket << pOwner->GetTargetRot();
		P2PNetwork::SendToAllPlayingUser( kPacket );
	}
}

void ioValkyrie::ChangeToWaitState( ioBaseChar *pOwner )
{
	ioEntityGroup *pGrp = pOwner->GetGroup();
	if( !pGrp )
		return;

	pGrp->ClearAllLoopAni( FLOAT100, true );
	pGrp->ClearAllActionAni( FLOAT100, true );

	int iAniID = pGrp->GetAnimationIdx( m_ValkyrieUpInfo.m_WaitAnimation );
	pGrp->SetLoopAni( iAniID, FLOAT10 );

	m_ChargeState	= CS_FLY;
	m_FlyState		= FS_WAIT;
	m_dwWaitTime	= FRAMEGETTIME();
	m_bRserveReUp	= false;
}

void ioValkyrie::ChangeToUpState( ioBaseChar *pOwner )
{
	SetAttackAttribute( pOwner, m_ValkyrieUpInfo.m_Attribute );

	m_FlyState			= FS_UP;
	m_dwWaitTime		= 0;
	m_dwUpStartTime		= FRAMEGETTIME();
	m_fUpCurrRange		= 0.0f;
	m_bRserveReUp		= false;
}

void ioValkyrie::ChangeToTargetFindState( ioBaseChar *pOwner, ioPlayStage* pStage, ioUserKeyInput::DirKeyInput eCurrKey, D3DXVECTOR3 vTargetDir )
{
	if( !pStage )
		return;

	if( !pOwner )
		return;

	if( pOwner->GetState() != CS_VALKYRIE_FLY )
	{
		pOwner->ClearValkyrieDobuleJumpFly();
		pOwner->SetState( CS_VALKYRIE_FLY );
	}

	ioZoneEffectWeapon* pZone = ToZoneEffectWeapon( g_WeaponMgr.FindWeapon( pOwner, m_dwCurrWeaponIdx ) );
	if( pZone )
		pZone->SetWeaponDead();

	if( eCurrKey == ioUserKeyInput::DKI_NONE )
	{
		m_vTargetDir = vTargetDir;

		ioHashString szTarget;
		if( m_vTargetDir == ioMath::VEC3_ZERO &&pOwner->CheckAimedTargetByBow( m_fTargetFindAngle, m_fTargetFindMinRange, m_fTargetFindMaxAngle, szTarget, m_bDownTarget ) )
		{
			ioBaseChar* pTarget = pStage->GetBaseChar( szTarget );
			if( pTarget )
			{			
				m_vTargetDir = pTarget->GetWorldPosition() - pOwner->GetWorldPosition();
				D3DXVec3Normalize( &m_vTargetDir, &m_vTargetDir );

				pOwner->SetTargetRotToDir( m_vTargetDir, true, true );
			}
			else
			{
				m_vTargetDir = ioMath::VEC3_ZERO;
				pOwner->SetTargetRotToDirKey( eCurrKey );
			}
		}
	}
	else
	{
		m_vTargetDir = ioMath::VEC3_ZERO;
		pOwner->SetTargetRotToDirKey( eCurrKey );
	}

	D3DXVECTOR3 vCurPos = pOwner->GetWorldPosition();
	float fMapHeight = pStage->GetMapHeight( vCurPos.x, vCurPos.z, pOwner, false, pOwner->GetMidHeightByRate(1.0f) );
	float fGapHeight = fabs( vCurPos.y - fMapHeight );

	float fExtraGauge = 0.0f;
	if( m_fHighDecreaseHeight < fGapHeight )
		fExtraGauge = m_fHighDecreaseGauge;

	if( m_FlyState == FS_REUP )
	{
		SetUseExtraGauge( m_fReUpNeedGauge + fExtraGauge );
	}
	else
	{
		SetUseExtraGauge( GetNeedBullet() + fExtraGauge );
	}

	m_FlyState = FS_TARGET_FIND;

	if( pOwner->IsNeedSendNetwork() )
	{
		SP2Packet kPacket( CUPK_ITEM_GAUGE_OK );
		FillItemGaugePacketHeader( pOwner, kPacket );
		kPacket << static_cast<byte>( CS_FLY );
		kPacket << static_cast<byte>( FS_TARGET_FIND );
		kPacket << pOwner->GetWorldPosition();
		kPacket << pOwner->GetTargetRot();
		kPacket << static_cast<int>( eCurrKey );
		kPacket << m_vTargetDir;
		P2PNetwork::SendToAllPlayingUser( kPacket );
	}
}

void ioValkyrie::ChangeToDownState( ioBaseChar *pOwner )
{
	SetAttackAttribute( pOwner, m_ValkyrieDownInfo.m_Attribute );
	
	m_FlyState			= FS_DOWN;
	m_dwWaitTime		= 0;
	m_dwDownStartTime	= FRAMEGETTIME();
	m_fDownCurrRange	= 0.0f;
}

void ioValkyrie::ChangeToReUpState( ioBaseChar *pOwner )
{
	ClearWeapon( pOwner );

	SetAttackAttribute( pOwner, m_ValkyrieReUpInfo.m_Attribute );

	m_FlyState			= FS_REUP;
	m_dwWaitTime		= 0;
	m_dwUpStartTime		= FRAMEGETTIME();
	m_fUpCurrRange		= 0.0f;
	m_bRserveReUp		= false;

	if( pOwner->IsNeedSendNetwork() )
	{
		SP2Packet kPacket( CUPK_ITEM_GAUGE_OK );
		FillItemGaugePacketHeader( pOwner, kPacket );
		kPacket << static_cast<byte>( CS_FLY );
		kPacket << static_cast<byte>( FS_REUP );
		kPacket << pOwner->GetWorldPosition();
		kPacket << pOwner->GetTargetRot();
		P2PNetwork::SendToAllPlayingUser( kPacket );
	}
}

void ioValkyrie::ChangeToLanding( ioBaseChar *pOwner )
{
	SetAttackAttribute( pOwner, m_LandAttribute );

	m_FlyState = FS_LANDING;
}

void ioValkyrie::ChangeToEndState( ioBaseChar *pOwner )
{
	m_FlyState = FS_END;
	pOwner->SetSkillNextJumpAmt( m_fChargeEndNextJumpAmt );

	for( ioHashStringVec::iterator iter = m_ChargeEndNextJumpRemoveBuffList.begin(); iter != m_ChargeEndNextJumpRemoveBuffList.end(); ++iter )
	{
		ioBuff* pBuff = pOwner->GetBuff( *iter );
		if( pBuff )
			pBuff->SetReserveEndBuff();
	}
}

void ioValkyrie::SetAttackAttribute( ioBaseChar *pOwner, const AttackAttribute& Attr )
{
	if( !pOwner )
		return;

	ioEntityGroup *pGrp = pOwner->GetGroup();
	if( !pGrp )
		return;

	pOwner->InitExtendAttackTagInfo();

	pGrp->ClearAllLoopAni( FLOAT10, true );
	pGrp->ClearAllActionAni( FLOAT10, true );

	int iAniID	     = pGrp->GetAnimationIdx( Attr.m_AttackAnimation );
	float fTimeRate  = Attr.m_fAttackAniRate;
	DWORD dwPreDelay = Attr.m_dwPreDelay;
	DWORD dwEndDelay = Attr.m_dwEndDelay;

	pGrp->SetActionAni( iAniID, FLOAT10, FLOAT10, FLOAT1, FLOAT1/fTimeRate, 0.0f, dwPreDelay, true );
		
	pOwner->RefreshFireTimeList( iAniID, Attr.m_vWeaponInfoList, FTT_NORMAL_ATTACK, fTimeRate, dwPreDelay );
	m_dwCurrWeaponIdx = pOwner->GetWeaponIndexBase();

	pOwner->CheckCharColSkipTime( iAniID, fTimeRate, dwPreDelay );
	pOwner->SetReservedSliding( Attr.m_vForceInfoList, iAniID, fTimeRate, dwPreDelay );

	m_dwSpecialAttackStartTime	= FRAMEGETTIME();
	m_dwSpecialAttackEndTime	= FRAMEGETTIME() + pGrp->GetAnimationFullTime(iAniID) * fTimeRate + dwPreDelay;

	float fEventTime = pGrp->GetAnimationEventTime_e( iAniID, "key_reserve" ) * fTimeRate;
	if( fEventTime > 0 )
	{
		m_dwKeyReserveTime = m_dwSpecialAttackStartTime + fEventTime + dwPreDelay;
	}
	else
	{
		m_dwKeyReserveTime = m_dwSpecialAttackStartTime;
	}
}

void ioValkyrie::ProcessValkyrieFlyState( ioBaseChar *pOwner, ioPlayStage* pStage )
{
	if( !pOwner )
		return;

	DWORD dwCurTime = FRAMEGETTIME();
	switch( m_FlyState )
	{
	case FS_WAIT:
		if( m_dwWaitTime + m_ValkyrieUpInfo.m_dwWaitDuration < dwCurTime )
		{
			ChangeToUpState( pOwner );
		}
		break;
	case FS_UP:
		{
			ProcessUp( pOwner, pStage );
		}
		break;
	case FS_TARGET_FIND:
		{
			ChangeToDownState( pOwner );
		}
		break;
	case FS_DOWN:
		{
			ProcessDown( pOwner, pStage );
		}
		break;
	case FS_REUP:
		{
			ProcessReUp( pOwner, pStage );
		}
		break;
	case FS_LANDING:
		{
			ProcessLadning( pOwner, pStage );
		}
		break;
	}
}

void ioValkyrie::ProcessUp( ioBaseChar *pOwner, ioPlayStage* pStage )
{
	if( ProcessMove( pOwner, m_ValkyrieUpInfo, ioMath::VEC3_ZERO, m_dwUpStartTime, m_fUpCurrRange, false ) != MS_MOVE )
	{
		if( pOwner->IsNeedProcess() )
		{
			ChangeToTargetFindState( pOwner, pStage, pOwner->CheckCurDirKey(), ioMath::VEC3_ZERO );
		}
	}
}

void ioValkyrie::ProcessDown( ioBaseChar *pOwner, ioPlayStage* pStage )
{
	MoveState eMoveState = ProcessMove( pOwner, m_ValkyrieDownInfo, m_vTargetDir, m_dwDownStartTime, m_fDownCurrRange, true );
	if( eMoveState == MS_LANDING )
	{	
		ChangeToReUpState( pOwner );		
	}
	else if( eMoveState == MS_END )
	{
		ChangeToReUpState( pOwner );
	}	
}

void ioValkyrie::ProcessReUp( ioBaseChar *pOwner, ioPlayStage* pStage )
{
	if( ProcessMove( pOwner, m_ValkyrieReUpInfo, ioMath::VEC3_ZERO, m_dwUpStartTime, m_fUpCurrRange, false ) != MS_MOVE )
	{
		if( pOwner->IsNeedProcess() )
		{
			if( m_bRserveReUp )
				ChangeToTargetFindState( pOwner, pStage, pOwner->CheckCurDirKey(), ioMath::VEC3_ZERO );
			else
				ChangeToEndState( pOwner );
		}
	}
	else
	{
		if( pOwner->IsAttackKey() && m_fReUpNeedGauge <= GetCurBullet() && m_dwKeyReserveTime <= FRAMEGETTIME() )
		{
			m_bRserveReUp = true;
		}
	}
}

void ioValkyrie::ProcessLadning( ioBaseChar *pOwner, ioPlayStage* pStage )
{
	if( 0 < m_dwSpecialAttackEndTime && m_dwSpecialAttackEndTime < FRAMEGETTIME() )
	{
		pOwner->ClearReservedSliding();

		pOwner->SetCurMoveSpeed( 0.0F );
		pOwner->SetGravityAmt( 0.0f );
		pOwner->SetForcePowerAmt( 0.0f );
		pOwner->SetCurJumpPowerAmt( 0.0f );

		pOwner->SetState( CS_DELAY );

		m_ChargeState	= CS_NONE;
		m_FlyState		= FS_NONE;
	}
}

ioValkyrie::MoveState ioValkyrie::ProcessMove( ioBaseChar *pOwner, const ValkyrieFlyInfo& rkFlyInfo, const D3DXVECTOR3& vFindDir, DWORD& dwStartTime, float& fCurrRange, bool bCheckLanding )
{
	if( !pOwner )
		return MS_END;

	DWORD dwCurTime		= FRAMEGETTIME();
	float fTimePerSec	= g_FrameTimer.GetSecPerFrame();

	D3DXVECTOR3 vMoveDir = ioMath::VEC3_ZERO;
	if( vFindDir == ioMath::VEC3_ZERO )
	{
		D3DXVECTOR3 vTargetDir = pOwner->GetTargetRot() * D3DXVECTOR3( 0.0f, 0.0f, -FLOAT1 );
		D3DXVec3Normalize( &vTargetDir, &vTargetDir );

		D3DXVECTOR3 vAxis;
		D3DXVec3Cross( &vAxis, &vTargetDir, &ioMath::UNIT_Y );
		D3DXVec3Normalize( &vAxis, &vAxis );

		D3DXQUATERNION qtNewRot;
		float fCurAngle = ioMath::ArrangeHead( rkFlyInfo.m_fChargeFlyAngle );
		D3DXQuaternionRotationAxis( &qtNewRot, &vAxis, DEGtoRAD(fCurAngle) );

		vMoveDir = qtNewRot * vTargetDir;
		D3DXVec3Normalize( &vMoveDir, &vMoveDir );
	}
	else
	{
		vMoveDir = vFindDir;
	}

	float fCurSpeed = rkFlyInfo.m_fChargeFlyMinSpeed;
	DWORD dwGapTime = dwCurTime - ( dwStartTime + rkFlyInfo.m_dwChargeFlyMinSpeedDuration );

	if( dwStartTime + rkFlyInfo.m_dwChargeFlyMinSpeedDuration < dwCurTime )
	{
		float fTimeRate = (float)dwGapTime / FLOAT1000;
		float fCurAccel = rkFlyInfo.m_fChargeFlyAccelSpeed * fTimeRate;

		fCurSpeed += fCurAccel;
	}

	if( fCurSpeed >= rkFlyInfo.m_fChargeFlyMaxSpeed )
		fCurSpeed = rkFlyInfo.m_fChargeFlyMaxSpeed;

	float fDistance = fCurSpeed * fTimePerSec;

	MoveState eRangeLimit = MS_END;
	D3DXVECTOR3 vMoveAmt( 0.0f, 0.0f, 0.0f );
	if( fCurrRange + fDistance < rkFlyInfo.m_fChargeFlyMaxRange )
	{
		eRangeLimit = MS_MOVE;
		vMoveAmt = vMoveDir * fDistance;
		fCurrRange += fDistance;
	}
	else
	{
		eRangeLimit = MS_END;

		float fGapDistance = rkFlyInfo.m_fChargeFlyMaxRange - fCurrRange;
		vMoveAmt = vMoveDir * fGapDistance;
	}

	bool bCol = false;
	ioPlayStage *pStage = pOwner->GetCreator();
	if( pStage && pStage->ReCalculateMoveVectorByTerrain( pOwner, &vMoveAmt, bCol ) )
	{		
		pOwner->Translate( vMoveAmt );
	}
		
	D3DXVECTOR3 vPos = pOwner->GetWorldPosition();
	if( pOwner->CheckAxisLimit( &vPos ) )
	{
		pOwner->SetWorldPosition( vPos );
	}

	float fHeightGap = pOwner->GetWorldPosition().y - pOwner->GetBottomHeight();
	if( bCheckLanding && fHeightGap <= FLOAT10 )
	{
		return MS_LANDING;
	}

	return eRangeLimit;
}

void ioValkyrie::WeaponMapColDead( ioWeapon *pWeapon )
{
	ioBaseChar *pOwner = pWeapon->GetOwner();
	if( pOwner && pWeapon )
	{
		ioZoneEffectWeapon *pZoneWeapon = ToZoneEffectWeapon( pWeapon );
		if( pZoneWeapon && pZoneWeapon->GetWoundItemType() == ioZoneEffectWeapon::WIT_VALKYRIE )
		{
			if( pOwner->GetWorldPosition().y - pOwner->GetBottomHeight() <= FLOAT10 )
			{
				ChangeToLanding( pOwner );
			}
			else
			{
				pOwner->SetSkillNextJumpAmt( m_fChargeEndNextJumpAmt );
				m_FlyState = FS_END;
			}
		}
	}
}

void ioValkyrie::OnItemGaugeOK( ioBaseChar *pOwner, SP2Packet &rkPacket )
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
	case CS_FLY:
		{
			byte eSubState;
			rkPacket >> eSubState;

			D3DXVECTOR3 vPos;
			D3DXQUATERNION qtRot;
			rkPacket >> vPos;
			rkPacket >> qtRot;

			pOwner->SetWorldPosition( vPos );
			pOwner->SetTargetRotToRotate( qtRot, true );

			switch( eSubState )
			{
			case FS_NONE:
				{
					ChangeToSpecialState( pOwner );
				}
				break;
			case FS_TARGET_FIND:
				{
					byte eCurrKey;
					D3DXVECTOR3 vTargetDir;
					rkPacket >> eCurrKey;
					rkPacket >> vTargetDir;
					ChangeToTargetFindState( pOwner, pOwner->GetCreator(), static_cast<ioUserKeyInput::DirKeyInput>( eCurrKey ), vTargetDir );
				}
				break;
			case FS_REUP:
				{
					ChangeToReUpState( pOwner );
				}
				break;
			}
		}
		break;
	}	
}

bool ioValkyrie::IsChargeAttackState( ioBaseChar *pOwner )
{
	if( pOwner->GetState() == CS_ATTACK )
	{
		if( m_ChargeState == CS_CHARGING )
			return true;
	}

	return false;
}

bool ioValkyrie::IsEnableAttackDashCheck( ioBaseChar *pOwner, bool bCheck )
{
	if( !IsChargeDash() )
		return false;

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


bool ioValkyrie::IsCharCollisionSkipState()
{
	if( m_ChargeState == CS_CHARGING && m_FlyState == FS_DOWN )
		return true;

	return false;
}

bool ioValkyrie::SetJumpAttack( ioBaseChar *pOwner, int iAniID, float fTimeRate, AttackType eType )
{
	if( !pOwner ) return false;

	DWORD dwCurTime = FRAMEGETTIME();
	ClearAirJump();

	AttackType eExtendType = AT_JUMP_ATTACK;

	const AttackAttribute *pAttr = GetAttackAttribute( eExtendType );
	if( !pAttr )
		return false;

	ioEntityGroup *pGrp = pOwner->GetGroup();
	iAniID = pGrp->GetAnimationIdx( pAttr->m_AttackAnimation );
	if( iAniID == -1 )
		return false;

	fTimeRate = pAttr->m_fAttackAniRate;
	DWORD dwPreDelay  = pAttr->m_dwPreDelay;

	// For AirJumpPower
	SetAirJump( pOwner, iAniID, fTimeRate );

	return false;
}

void ioValkyrie::ProcessJumpAttack( ioBaseChar *pOwner, float fHeightGap )
{
	if( m_pExtendJump )
	{
		m_pExtendJump->ProcessJumpAttack( pOwner, fHeightGap );
	}

	CheckAirJump( pOwner );
}

void ioValkyrie::UpdateExtraData( ioBaseChar *pOwner )
{
	if( !pOwner ) return;

	float fTimePerSec = g_FrameTimer.GetSecPerFrame();
	switch( pOwner->GetState() )
	{
	case CS_DELAY:
	case CS_RUN:
	case CS_DASH:
	case CS_ATTACK:
		{
			m_fCurExtraGauge += m_fExtraGaugeBaseInc * fTimePerSec;
			m_fCurExtraGauge = min( m_fCurExtraGauge, m_fMaxExtraGauge );
		}
		break;
	}
}

int ioValkyrie::GetMaxBullet()
{
	return (int)m_fMaxExtraGauge;
}

int ioValkyrie::GetNeedBullet()
{
	return (int)m_fNeedGauge;
}

int ioValkyrie::GetCurBullet()
{
	float fCurCoolTime = m_fCurExtraGauge;
	fCurCoolTime = max( 0, min( fCurCoolTime, m_fMaxExtraGauge ) );

	return (int)fCurCoolTime;
}

void ioValkyrie::SetCurBullet( int iCurBullet )
{
	if( iCurBullet < 0 )
		return;

	float fCurBullet = iCurBullet;
	fCurBullet = max( 0, min( fCurBullet, m_fMaxExtraGauge ) );

	m_fCurExtraGauge = fCurBullet;
}

void ioValkyrie::SetUseExtraGauge( float fAmt )
{
	m_fCurExtraGauge -= fAmt;
	m_fCurExtraGauge = max( 0.0f, m_fCurExtraGauge );
}

//-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

ioValkyrieFlySpecialState::ioValkyrieFlySpecialState()
{
}

ioValkyrieFlySpecialState::~ioValkyrieFlySpecialState()
{
}

void ioValkyrieFlySpecialState::Initialize( ioBaseChar* pOwner, CharState eCurrState )
{
	ioSpecialStateBase::Initialize( pOwner, eCurrState );
}

void ioValkyrieFlySpecialState::Finalize( ioBaseChar* pOwner, CharState eCurrState, CharState eNewState )
{
	ioSpecialStateBase::Finalize( pOwner, eCurrState, eNewState );

	ioValkyrie *pValkyrie = ToValkyrie( pOwner->GetPriorityItem( SPT_ATTACK ) );
	if( pValkyrie && eNewState != CS_ATTACK )
		pValkyrie->ClearState( pOwner );
}

bool ioValkyrieFlySpecialState::IsCharCollisionSkipState( const ioBaseChar *const pOwner, DWORD dwStartTime, DWORD dwEndTime, DWORD dwSkipType, bool bTeamOnly, bool bDefense, TeamType eOwnerTeam, TeamType eOtherTeam ) const
{
	ioValkyrie *pValkyrie = ToValkyrie( pOwner->GetPriorityItem(SPT_ATTACK) );
	if( pValkyrie && pValkyrie->IsCharCollisionSkipState() )
		return true;

	if( Help::CheckCharColState( dwStartTime, dwEndTime, dwSkipType, bTeamOnly, bDefense, eOwnerTeam, eOtherTeam ) )
		return true;

	return false;
}

void ioValkyrieFlySpecialState::CheckSpecialState( ioBaseChar* pOwner )
{
	ioValkyrie *pValkyrie = ToValkyrie( pOwner->GetPriorityItem( SPT_ATTACK ) );
	if( pValkyrie )
		pValkyrie->ProcessValkyrieFlyState( pOwner, pOwner->GetCreator() );
	else
	{
		pOwner->SetState( CS_DELAY );
		pOwner->SetFallState( true );
		return;
	}
}

void ioValkyrieFlySpecialState::CheckFallStateChange( ioBaseChar* pOwner, float fMinFallHeightRate, float fBottomHeight, bool bBottomMaxCheck, float fMaxBottomHeight )
{
	bool FallCheck = false;
	if( pOwner->GetState() == CS_VALKYRIE_FLY && GetValkyrieFlyState( pOwner ) == ioValkyrie::FS_END )
		FallCheck = true;

	if( FallCheck )
	{
		float fSkillNextJumpAmt = pOwner->GetSkillNextJumpAmt();
		if( fSkillNextJumpAmt > 0.0f )
			pOwner->SetSKillEndJumpState( fSkillNextJumpAmt );
		else
		{
			float fMinFallinRate = ( pOwner->GetWorldCollisionBox().GetExtents( 1 ) * 2.0f ) * pOwner->GetMinFallHeightRate();
			float fCharPosGap = pOwner->GetWorldPosition().y - fBottomHeight;
			if( fCharPosGap > fMinFallinRate )
				pOwner->SetFallState( true );
		}
	}
}

bool ioValkyrieFlySpecialState::ProcessMove( ioBaseChar* pOwner, float fTimePerSec, DWORD dwPreTime, DWORD dwCurTime )
{	
	return true;
}

int ioValkyrieFlySpecialState::GetValkyrieFlyState( ioBaseChar* pOwner )
{
	ioValkyrie *pValkyrie = ToValkyrie( pOwner->GetPriorityItem( SPT_ATTACK ) );
	if( !pValkyrie )
		return static_cast<int>( ioValkyrie::FS_NONE );

	return static_cast<int>( pValkyrie->GetFlyState() );
}