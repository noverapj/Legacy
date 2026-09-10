

#include "stdafx.h"

#include "ioMidnightItem.h"
#include "ioMidnightScytheObjectItem.h"
#include "ioMidnightGunObjectItem.h"
#include "ioIljimaeJump.h"

#include "ioPlayStage.h"
#include "ioBaseChar.h"
#include "ioDrinkItem.h"

#include "WeaponDefine.h"

ioMidnightItem::ioMidnightItem()
{
	Init();
}

ioMidnightItem::ioMidnightItem( const ioMidnightItem &rhs )
	: ioChargeCommandItem( rhs ),		
	m_fNeedChargeBullet( rhs.m_fNeedChargeBullet ),
	m_fExtraGaugeJumpInc( rhs.m_fExtraGaugeJumpInc ), 
	m_dwScytheObjectItem( rhs.m_dwScytheObjectItem ),	
	m_dwGunObjectItem( rhs.m_dwGunObjectItem ),
	m_PreGatherAnimationList( rhs.m_PreGatherAnimationList ),
	m_szRleaseObjectEffect( rhs.m_szRleaseObjectEffect ),
	m_CancelDashAttribute( rhs.m_CancelDashAttribute ),
	m_CancelDashAddAttackAttribute( rhs.m_CancelDashAddAttackAttribute ),
	m_JumpDashAttribute( rhs.m_JumpDashAttribute ),
	m_fMaxBullet( rhs.m_fMaxBullet ),
	m_fIncreseGaugeTic( rhs.m_fIncreseGaugeTic ),
	m_ChargeAutoAttribute( rhs.m_ChargeAutoAttribute ),
	m_szChargeAttackLoopingSound( rhs.m_szChargeAttackLoopingSound ),
	m_ChargeRunningAni( rhs.m_ChargeRunningAni ),
	m_fChargeRunningAniRate( rhs.m_fChargeRunningAniRate ),
	m_dwChargeRunningDuration( rhs.m_dwChargeRunningDuration ),
	m_fChargeRunSpeed( rhs.m_fChargeRunSpeed ),
	m_dwChargeRotateTime( rhs.m_dwChargeRotateTime ),
	m_ChargeJumpCmdInfoList( rhs.m_ChargeJumpCmdInfoList ),
	m_ClawAttackInfo( rhs.m_ClawAttackInfo ),
	m_CommandWeapon( rhs.m_CommandWeapon ),
	m_JumpCommandWeapon( rhs.m_JumpCommandWeapon ),
	m_fJumpDashGravityAmt( rhs.m_fJumpDashGravityAmt ),
	m_fCancelDashEndJumpAmt( rhs.m_fCancelDashEndJumpAmt ),
	m_fCancelDashRandRate( rhs.m_fCancelDashRandRate )
{
	Init();
}

ioMidnightItem::~ioMidnightItem()
{
}

void	ioMidnightItem::Init()
{
	m_MidnightState				= MS_NONE;
	m_ChargeState				= CS_NONE;
	m_ChainAttackState			= CAS_NONE;
	m_ObjectEquipState			= OES_NONE;

	m_dwAttackStartTime			= 0;
	m_iCurCombo					= 0;
	m_iCurAirJump				= 0;
	m_dwMotionEndTime			= 0;
	m_dwHitedTime				= 0;
	m_dwWeaponIndex				= 0;
	m_dwCancelDashMotionEndTime	= 0;
	m_dwCancelDashAttackMotionEndTime	= 0;
	m_dwCancelDashAttackTime	= 0;
	m_dwJumpDashMotionEndTime	= 0;
	m_ChargeAttackLoopingSoundID	= 0;
	m_dwRunningEndTime			= 0;
	m_CurDirKey					= ioUserKeyInput::DKI_NONE;
	m_bTargetRot				= false;
	m_dwChargeAttackEndTime		= 0;
	m_bObjectReleaseSend		= false;
	m_bObjectReleaseAutoJump	= false;
	m_bJumpDashEnable			= true;

	m_fAirJumpPowerRate			= 0.0f;
	m_fCurExtraGauge			= 0.0f;
	m_fCurBullet				= 0.0f;

	m_Target.Clear();

	m_CurAttackInfo.Init();
}

void	ioMidnightItem::LoadProperty( ioINILoader &rkLoader )
{
	ioChargeCommandItem::LoadProperty( rkLoader );

	char szBuf[MAX_PATH];

	m_fNeedChargeBullet		= rkLoader.LoadFloat_e( "need_charge_bullet", 0 );
	m_fExtraGaugeJumpInc	= rkLoader.LoadFloat_e( "extra_gauge_jump_inc_rate", 0.0f );

	m_dwScytheObjectItem	= static_cast<DWORD>( rkLoader.LoadInt_e( "scythe_object_item", 0 ) );
	m_dwGunObjectItem		= static_cast<DWORD>( rkLoader.LoadInt_e( "gun_object_item", 0 ) );

	int iPreCount	= rkLoader.LoadInt_e( "pre_gather_animation_cnt", 0 );
	for( int i = 0; i < iPreCount; ++i )
	{
		wsprintf_e( szBuf, "pre_gather_animation%d", i+1 );
		rkLoader.LoadString( szBuf, "", szBuf, MAX_PATH );
		m_PreGatherAnimationList.push_back( ioHashString( szBuf ) );
	}

	rkLoader.LoadString_e( "object_release_effect", "", szBuf, MAX_PATH );
	m_szRleaseObjectEffect = szBuf;

	LoadAttackAttribute_e( "cancel_dash", m_CancelDashAttribute, rkLoader );
	LoadAttackAttribute_e( "cancel_dash_attack", m_CancelDashAddAttackAttribute, rkLoader );
	LoadAttackAttribute_e( "jump_dash", m_JumpDashAttribute, rkLoader );
	m_fJumpDashGravityAmt = rkLoader.LoadFloat_e( "jump_dash_gravity_amt", 0.0f );
	m_fCancelDashEndJumpAmt = rkLoader.LoadFloat_e( "cancel_dash_end_jump_amt", 0.0f );
	m_fCancelDashRandRate   = rkLoader.LoadFloat_e( "cancel_dash_rand_rate", FLOAT1 );

	m_fMaxBullet = rkLoader.LoadFloat_e( "max_gauge", 0 );
	m_fIncreseGaugeTic = rkLoader.LoadFloat_e( "increase_gauge_tic", 0.0f );

	wsprintf_e( szBuf, "charge_auto_attack" );
	LoadAttackAttribute( szBuf, m_ChargeAutoAttribute, rkLoader );

	rkLoader.LoadString_e( "charge_running_ani", "", szBuf, MAX_PATH );
	m_ChargeRunningAni	= szBuf;
	m_fChargeRunningAniRate	= rkLoader.LoadFloat_e( "charge_running_ani_rate", FLOAT1 );
	m_fChargeRunSpeed	= rkLoader.LoadFloat_e( "charge_run_speed", FLOAT1 );
	m_dwChargeRunningDuration	= (DWORD)rkLoader.LoadInt_e( "charge_running_duration", 0 );
	m_dwChargeRotateTime	= (DWORD)rkLoader.LoadInt_e( "charge_run_rotate_speed", 0 );
}

ioItem*	ioMidnightItem::Clone()
{
	return	new	ioMidnightItem( *this );
}

ioWeaponItem::WeaponSubType	ioMidnightItem::GetSubType() const
{
	return	WST_MIDNIGHT_ITEM;
}

void	ioMidnightItem::LoadCmdInfoList( ioINILoader &rkLoader )
{
	char szBuf[MAX_PATH];
	char szKey[MAX_PATH];

	int iCmdInoCnt = rkLoader.LoadInt_e( "max_cmd_info_cnt", 0 );
	if( iCmdInoCnt > 0 )
	{
		for( int i=0; i < iCmdInoCnt; ++i )
		{
			ChargeCmdInfo kCmdInfo;

			wsprintf_e( szKey, "cmd_info%d", i+1 );
			rkLoader.LoadString( szKey, "", szBuf, MAX_COMMAND_CNT );
			kCmdInfo.m_Cmd = szBuf;

			wsprintf_e( szKey, "cmd_info%d_use_type", i+1 );
			kCmdInfo.m_CmdUseType = (CmdUseType)rkLoader.LoadInt( szKey, CUT_NORMAL );

			wsprintf_e( szKey, "cmd_info%d_skill_slot", i+1 );
			kCmdInfo.m_iSkillSlot = rkLoader.LoadInt( szKey, 0 );

			wsprintf_e( szKey, "cmd_info%d_combo_cnt", i+1 );
			int iMaxCmdCombo = rkLoader.LoadInt( szKey, 0 );
			if( iMaxCmdCombo > 0 )
				kCmdInfo.m_SubInfoList.reserve( iMaxCmdCombo );

			for( int j=0; j < iMaxCmdCombo; ++j )
			{
				ChargeCmdSubInfo kSubInfo;

				wsprintf_e( szKey, "cmd_info%d_combo%d", i+1, j+1 );
				LoadAttackAttribute( szKey, kSubInfo.m_CmdAttack, rkLoader );

				wsprintf_e( szKey, "cmd_info%d_combo%d_use_d", i+1, j+1 );
				kSubInfo.m_bCmdAttack_D = rkLoader.LoadBool( szKey, false );

				wsprintf_e( szKey, "cmd_info%d_combo%d_use_s", i+1, j+1 );
				kSubInfo.m_bCmdAttack_S = rkLoader.LoadBool( szKey, false );

				wsprintf_e( szKey, "cmd_info%d_combo%d_d", i+1, j+1 );
				LoadAttackAttribute( szKey, kSubInfo.m_CmdAttack_D, rkLoader );

				wsprintf_e( szKey, "cmd_info%d_combo%d_s", i+1, j+1 );
				LoadAttackAttribute( szKey, kSubInfo.m_CmdAttack_S, rkLoader );

				kCmdInfo.m_SubInfoList.push_back( kSubInfo );
			}

			m_ChargeCmdInfoList.push_back( kCmdInfo );

			CommandWeaponInfo	commandWeaponInfo;
			wsprintf_e( szKey, "cmd_info%d_weapon", i+1 );
			commandWeaponInfo.m_CommandWeapon	= (CommandWeapon)rkLoader.LoadInt( szKey, 0 );
			commandWeaponInfo.m_Cmd	= kCmdInfo.m_Cmd;
			m_CommandWeapon.push_back( commandWeaponInfo );
		}
	}

	iCmdInoCnt = rkLoader.LoadInt_e( "max_jump_cmd_info_cnt", 0 );
	if( iCmdInoCnt > 0 )
	{
		for( int i=0; i < iCmdInoCnt; ++i )
		{
			ChargeCmdInfo kCmdInfo;

			wsprintf_e( szKey, "jump_cmd_info%d", i+1 );
			rkLoader.LoadString( szKey, "", szBuf, MAX_COMMAND_CNT );
			kCmdInfo.m_Cmd = szBuf;

			wsprintf_e( szKey, "jump_cmd_info%d_use_type", i+1 );
			kCmdInfo.m_CmdUseType = (CmdUseType)rkLoader.LoadInt( szKey, CUT_NORMAL );

			wsprintf_e( szKey, "jump_cmd_info%d_skill_slot", i+1 );
			kCmdInfo.m_iSkillSlot = rkLoader.LoadInt( szKey, 0 );

			wsprintf_e( szKey, "jump_cmd_info%d_combo_cnt", i+1 );
			int iMaxCmdCombo = rkLoader.LoadInt( szKey, 0 );
			if( iMaxCmdCombo > 0 )
				kCmdInfo.m_SubInfoList.reserve( iMaxCmdCombo );

			for( int j=0; j < iMaxCmdCombo; ++j )
			{
				ChargeCmdSubInfo kSubInfo;

				wsprintf_e( szKey, "jump_cmd_info%d_combo%d", i+1, j+1 );
				LoadAttackAttribute( szKey, kSubInfo.m_CmdAttack, rkLoader );

				wsprintf_e( szKey, "jump_cmd_info%d_combo%d_use_d", i+1, j+1 );
				kSubInfo.m_bCmdAttack_D = rkLoader.LoadBool( szKey, false );

				wsprintf_e( szKey, "jump_cmd_info%d_combo%d_use_s", i+1, j+1 );
				kSubInfo.m_bCmdAttack_S = rkLoader.LoadBool( szKey, false );

				wsprintf_e( szKey, "jump_cmd_info%d_combo%d_d", i+1, j+1 );
				LoadAttackAttribute( szKey, kSubInfo.m_CmdAttack_D, rkLoader );

				wsprintf_e( szKey, "jump_cmd_info%d_combo%d_s", i+1, j+1 );
				LoadAttackAttribute( szKey, kSubInfo.m_CmdAttack_S, rkLoader );

				kCmdInfo.m_SubInfoList.push_back( kSubInfo );
			}

			m_ChargeJumpCmdInfoList.push_back( kCmdInfo );

			CommandWeaponInfo	commandWeaponInfo;
			wsprintf_e( szKey, "jump_cmd_info%d_weapon", i+1 );
			commandWeaponInfo.m_CommandWeapon	= (CommandWeapon)rkLoader.LoadInt( szKey, 0 );
			commandWeaponInfo.m_Cmd	= kCmdInfo.m_Cmd;
			m_JumpCommandWeapon.push_back( commandWeaponInfo );
		}
	}

	LoadClawAttackInfo( rkLoader );
}

void	ioMidnightItem::LoadClawAttackInfo( ioINILoader &rkLoader )
{
	char szBuf[MAX_PATH] = "";
	char szKey[MAX_PATH] = "";

	m_ClawAttackInfo.Init();

	// start
	rkLoader.LoadString_e( "first_attack_actionAni_Center", "", szBuf, MAX_PATH );
	m_ClawAttackInfo.m_StartAniCenter = szBuf;
	rkLoader.LoadString_e( "first_attack_actionAni_Up", "", szBuf, MAX_PATH );
	m_ClawAttackInfo.m_StartAniUp = szBuf;
	rkLoader.LoadString_e( "first_attack_actionAni_Down", "", szBuf, MAX_PATH );
	m_ClawAttackInfo.m_StartAniDown = szBuf;

	//시작
	wsprintf_e( szKey, "first_attack" );
	LoadAttackAttribute( szKey, m_ClawAttackInfo.m_FirstAttack, rkLoader );

	// 타겟 대상 검색용
	m_ClawAttackInfo.m_TargetWoundType = (TargetWoundType)rkLoader.LoadInt_e( "target_wound_type", TWT_ALL );
	m_ClawAttackInfo.m_fTargetAngle = rkLoader.LoadFloat_e( "target_angle", 0.0f );
	m_ClawAttackInfo.m_fTargetRange = rkLoader.LoadFloat_e( "target_range", 0.0f );
	m_ClawAttackInfo.m_fTargetUpHeight = rkLoader.LoadFloat_e( "target_up_height", 0.0f );
	m_ClawAttackInfo.m_fTargetDownHeight = rkLoader.LoadFloat_e( "target_down_height", 0.0f );

	rkLoader.LoadString_e( "delay_attack_actionAni_Center", "", szBuf, MAX_PATH );
	m_ClawAttackInfo.m_AttackLoopAniCenter = szBuf;
	rkLoader.LoadString_e( "delay_attack_actionAni_Up", "", szBuf, MAX_PATH );
	m_ClawAttackInfo.m_AttackLoopAniUp = szBuf;
	rkLoader.LoadString_e( "delay_attack_actionAni_Down", "", szBuf, MAX_PATH );
	m_ClawAttackInfo.m_AttackLoopAniDown = szBuf;
	m_ClawAttackInfo.m_fAttackLoopAniRate = rkLoader.LoadFloat_e( "delay_attack_actionAni_Rate", FLOAT1 );

	m_ClawAttackInfo.m_dwWaitMaxDelayTime = (DWORD)rkLoader.LoadInt_e( "delay_attack_max_wait_time", 0 );

	rkLoader.LoadString_e( "add_attack_actionAni", "", szBuf, MAX_PATH );
	m_ClawAttackInfo.m_AddAttackAni = szBuf;

	// attack move
	wsprintf_e( szKey, "add_attack" );
	LoadAttackAttribute( szKey, m_ClawAttackInfo.m_AddAttack, rkLoader );
}

void	ioMidnightItem::OnReleased( ioBaseChar *pOwner )
{
	if( IsEquipObject( pOwner ) )
		ReleaseObject( pOwner );

	m_fCurExtraGauge	= 0.0f;

	ioAttackableItem::OnReleased( pOwner );
}

void	ioMidnightItem::SetNormalAttack( ioBaseChar *pOwner, int iCurCombo )
{
	ClearCancelInfo();

	int iPreGatherCnt	= m_PreGatherAnimationList.size();

	m_ChargeState		= CS_CHARGING;
	m_dwAttackStartTime	= FRAMEGETTIME();
	m_iCurCombo			= iCurCombo;
	m_CurDirKey			= ioUserKeyInput::DKI_NONE;

	pOwner->SetState( CS_ATTACK );
	pOwner->SetChargingState( true );
	pOwner->SetAttackMoveEnable( false );

	ioEntityGroup *pGrp	= pOwner->GetGroup();
	pGrp->ClearAllActionAni( FLOAT100, true );
	pGrp->ClearAllLoopAni( FLOAT100, true );

	if( 0 < iPreGatherCnt && COMPARE( iCurCombo, 0, iPreGatherCnt ) )
	{
		int	iAniID	= pGrp->GetAnimationIdx( m_PreGatherAnimationList[iCurCombo] );
		float	fTimeRate	= (float)m_dwChargeTime / pGrp->GetAnimationFullTime( iAniID );
		pGrp->SetActionAni( iAniID, FLOAT100, FLOAT100, FLOAT1, FLOAT1/fTimeRate, 0.0f, 0.0f, true );
	}
}

void	ioMidnightItem::CheckNormalAttack( ioBaseChar *pOwner )
{
	DWORD	dwCurTime	= FRAMEGETTIME();

	switch( m_ChargeState )
	{
	case	CS_CHARGING:
		OnCharging( pOwner );
		break;

	case	CS_ATTACK_FIRE:
		{
			if( pOwner->IsNeedProcess() )
			{
				CheckCancelReserveByConditionalSpecialDash( pOwner );
				//Enable GaugeCheck
				if( ProcessCancelBySpeicalDash( pOwner ) )
					return;

				CheckCancelReserve( pOwner );
				if( ProcessCancel(pOwner) )
					return;

				if( CheckCmdInputState( pOwner ) )
					return;

				if( CheckSkillInputState( pOwner ) )
					return;
			}
			ioWeaponItem::CheckNormalAttack( pOwner );
		}
		break;

	case	CS_AUTO_ATTACK:
		{
			OnChargeAttack( pOwner );
		}
		break;
	}
}

void	ioMidnightItem::SetExtraInfoToWeapon( ioWeapon *pWeapon, bool bJumpAttack )
{
	ioAttackableItem::SetExtraInfoToWeapon( pWeapon, bJumpAttack );

	ioUroborusWeapon4 *pUroborus4 = ToUroborusWeapon4( pWeapon );
	if( pUroborus4 )
	{
		m_dwWeaponIndex = pWeapon->GetWeaponIndex();

		ioBaseChar *pChar = pWeapon->GetOwner();
		if( pChar )
		{
			D3DXVECTOR3 vDir = pChar->GetTargetRot() * D3DXVECTOR3( 0, 0, -1 );
			D3DXVec3Normalize( &vDir, &vDir );

			if( !m_Target.IsEmpty() )
			{
				ioBaseChar *pTargetChar = pChar->GetBaseChar( m_Target );
				if( pTargetChar )
				{
					vDir = pTargetChar->GetWorldPosition() - pChar->GetWorldPosition();
					D3DXVec3Normalize( &vDir, &vDir );
				}
			}	

			pUroborus4->SetMoveDir( vDir );
		}
	}

	ioMissileWeapon *pMissile = ToMissileWeapon( pWeapon );
	if( pMissile )
	{
		ioBaseChar *pChar = pWeapon->GetOwner();
		if( pChar )
		{
			D3DXVECTOR3 vDir = pChar->GetTargetRot() * D3DXVECTOR3( 0, 0, -1 );
			D3DXVec3Normalize( &vDir, &vDir );

			if( !m_Target.IsEmpty() )
			{
				ioBaseChar *pTargetChar = pChar->GetBaseChar( m_Target );
				if( pTargetChar )
				{
					D3DXVECTOR3 vPos = pTargetChar->GetWorldPosition();
					vPos.y = pTargetChar->GetMidHeightByRate();
					pMissile->SetPosition( vPos );
				}
			}	
		}
	}

	ioContactWeapon *pContact = ToContactWeapon( pWeapon );
	if( pContact && m_ChainAttackState == CAS_ATTACK_ADD )
	{
		ioBaseChar *pChar = pWeapon->GetOwner();
		if( pChar && !m_Target.IsEmpty() )
		{
			ioBaseChar *pTargetChar = pChar->GetBaseChar( m_Target );
			if( pTargetChar )
			{
				D3DXVECTOR3 vPos = pTargetChar->GetWorldPosition();
				vPos.y = pTargetChar->GetMidHeightByRate();
				pContact->SetPosition( vPos );
			}
		}
	}

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

	ioOwnerAttackWeapon* pOwnerWepon = ToOwnerAttackWeapon(pWeapon);
	if( pOwnerWepon && !m_Target.IsEmpty() )
	{
		ioBaseChar *pChar = pWeapon->GetOwner();
		if( pChar )
		{
			ioPlayStage* pStage = pChar->GetCreator();
			if( pStage )
				pOwnerWepon->SetTargetName( m_Target, pStage );
		}

	}
}

void	ioMidnightItem::OnCharging( ioBaseChar *pOwner )
{
	if( !pOwner->IsNeedProcess() )
		return;

	if( pOwner->IsAttackKeyDown() && GetNeedBullet() <= GetCurBullet() )
	{
		if( m_dwAttackStartTime + m_dwChargeTime <= FRAMEGETTIME() )
		{
			ChangeToChargeAttack( pOwner );
		}
	}
	else
	{
		ChangeToAttackFire( pOwner );
	}
}

void	ioMidnightItem::ChangeToAttackFire( ioBaseChar *pOwner )
{
	if( !pOwner )
		return;

	DWORD	dwCurTime	= FRAMEGETTIME();

	pOwner->InitExtendAttackTagInfo();

	ClearCancelInfo();

	const	AttackAttribute*	pAttribute = m_FormalAttack.GetAttribute( AT_NORMAL, m_iCurCombo );
	if( pAttribute )
	{
		pOwner->SetNormalAttackByAttribute( *pAttribute );

		m_szCurAni		= pAttribute->m_AttackAnimation;
		m_fCurAniRate	= pAttribute->m_fAttackAniRate;

		m_dwInputCancelCheckTime	= dwCurTime;
		m_dwCancelCheckTime	= dwCurTime;
	}

	pOwner->ApplyNormalAttackResult( m_iCurCombo );
	m_ChargeState	= CS_ATTACK_FIRE;

	if( pOwner->IsNeedSendNetwork() )
	{
		SP2Packet kPacket( CUPK_ITEM_GAUGE_OK );
		FillItemGaugePacketHeader( pOwner, kPacket );
		kPacket << ST_ATTACK;
		kPacket << m_iCurCombo;
		kPacket << pOwner->GetTargetRot();
		P2PNetwork::SendToAllPlayingUser( kPacket );
	}
}

void	ioMidnightItem::ChangeToChargeAttack( ioBaseChar* pOwner )
{
	if( !pOwner ) 
		return;

	ioEntityGroup *pGrp = pOwner->GetGroup();
	if( !pGrp )
		return;

	DWORD	dwCurTime	= FRAMEGETTIME();
	m_dwRunningEndTime	= dwCurTime;

	m_ChargeState	= CS_AUTO_ATTACK;

	DecreaseBullet();

	if( !m_szChargeAttackLoopingSound.IsEmpty() )
	{
		if( m_ChargeAttackLoopingSoundID != 0 )
			g_SoundMgr.StopSound( m_szChargeAttackLoopingSound, m_ChargeAttackLoopingSoundID );

		ioEntityGroup *pGrp = pOwner->GetGroup();
		if( pGrp )
			m_ChargeAttackLoopingSoundID = g_SoundMgr.PlaySound( m_szChargeAttackLoopingSound, pGrp->GetParentSceneNode(), PSM_LOOP | PSM_THREAD );
	}

	pGrp->ClearAllActionAni( FLOAT100 );
	pGrp->ClearAllLoopAni( FLOAT100 );

	int iAniID = 0;
	iAniID = pGrp->GetAnimationIdx( m_ChargeRunningAni );

	pGrp->SetLoopAni( iAniID, FLOAT100, FLOAT1, FLOAT1 / m_fChargeRunningAniRate );
	m_dwRunningEndTime += m_dwChargeRunningDuration;

	pOwner->SetTargetRotToRotate( pOwner->GetTargetRot(), true, false );

	if ( pOwner->IsNeedSendNetwork() )
	{
		SP2Packet kPacket( CUPK_ITEM_GAUGE_OK );
		FillItemGaugePacketHeader( pOwner, kPacket );		
		kPacket << ST_AUTO_ATTACK;
		kPacket << m_iCurCombo;
		kPacket << pOwner->GetTargetRot();
		P2PNetwork::SendToAllPlayingUser( kPacket );
	}
}

void	ioMidnightItem::OnChargeAttack( ioBaseChar* pOwner )
{
	if( !pOwner ) 
		return;

	ioEntityGroup *pGrp = pOwner->GetGroup();
	if( !pGrp )
		return;

	DWORD	dwCurTime	= FRAMEGETTIME();

	if( m_dwRunningEndTime != 0 && m_dwRunningEndTime < dwCurTime )
	{
		pOwner->SetState( CS_DELAY );
		m_ChargeState	= CS_NONE;
		return;
	}

	ioPlayStage *pStage = g_ItemMaker.GetPlayStage();
	if( pOwner->IsNeedProcess() && pStage )
		pStage->SetTargetLookAt( true );

	pOwner->SetCurMoveSpeed( m_fChargeRunSpeed );

	CheckKeyInput( pOwner );
	ProcessRotate( pOwner );

	if( m_dwChargeAttackEndTime < dwCurTime )
	{
		pGrp->ClearAllActionAni( FLOAT100 );

		int iAniID = pGrp->GetAnimationIdx( m_ChargeAutoAttribute.m_AttackAnimation );
		float fAniRate = m_ChargeAutoAttribute.m_fAttackAniRate;

		if( fAniRate < 0.0f )
			fAniRate = FLOAT1;

		pGrp->SetActionAni( iAniID, FLOAT100, FLOAT100, FLOAT1, FLOAT1 / fAniRate );

		pOwner->RefreshFireTimeList( iAniID, m_ChargeAutoAttribute.m_vWeaponInfoList, FTT_NORMAL_ATTACK, fAniRate, 0 );
		pOwner->IncreaseWeaponIndexBase();
		pOwner->SetReservedSliding( m_ChargeAutoAttribute.m_vForceInfoList, iAniID, fAniRate, 0 );

		m_dwChargeAttackEndTime = dwCurTime + pGrp->GetAnimationFullTime(iAniID) * fAniRate;

		/*float fKeyReserve = pGrp->GetAnimationEventTime_e( iAniID, "key_reserve" ) * fAniRate;
		if( fKeyReserve > 0.0f )
			m_dwKeyReserveTime = dwCurTime + fKeyReserve;*/
	}
}

void	ioMidnightItem::OnItemGaugeOK( ioBaseChar *pOwner, SP2Packet &rkPacket )
{
	if( !pOwner ) 
		return;

	int iSyncType;
	rkPacket >> iSyncType;

	switch( iSyncType )
	{
	case	ST_CMD_STATE:
		{
			rkPacket >> m_iCurCombo;

			ioHashString szCmdInfo;
			int iCmdCombo;
			rkPacket >> szCmdInfo >> iCmdCombo;

			ApplyCmdAttack( pOwner, OES_GROUND );
		}
		break;

	case	ST_JUMP_CMD_STATE:
		{
			rkPacket >> m_iCurCombo;

			ioHashString szCmdInfo;
			int iCmdCombo;
			rkPacket >> szCmdInfo >> iCmdCombo;

			ApplyCmdAttack( pOwner, OES_JUMP );
		}
		break;

	case	ST_ATTACK:
		{
			D3DXQUATERNION qtRot;
			rkPacket >> m_iCurCombo;
			rkPacket >> qtRot;

			pOwner->SetTargetRot( qtRot );
			ChangeToAttackFire( pOwner );
		}
		break;

	case	ST_OBJECT_EQUIP:
		{
			byte eObjectState, eObjectEquipState;
			DWORD dwObject;
			D3DXVECTOR3 vPos;
			D3DXQUATERNION qtRot;
			rkPacket >> eObjectState;
			rkPacket >> eObjectEquipState;
			rkPacket >> dwObject;
			rkPacket >> vPos;
			rkPacket >> qtRot;

			pOwner->SetWorldPosition( vPos );
			pOwner->SetWorldOrientation( qtRot );

			OnChangeEquipObjectState( pOwner, dwObject, static_cast<EquipObjectState>( eObjectState ), static_cast<ObjectEquipState>( eObjectEquipState ) );
		}
		break;

	case	ST_OBJECT_RELEASE:
		{
			D3DXVECTOR3 vPos;
			D3DXQUATERNION qtRot;
			rkPacket >> vPos;
			rkPacket >> qtRot;

			pOwner->SetWorldPosition( vPos );
			pOwner->SetWorldOrientation( qtRot );

			OnReleaseObject( pOwner );
		}
		break;

	case	ST_AUTO_ATTACK:
		{
			D3DXQUATERNION qtRot;
			rkPacket >> m_iCurCombo;
			rkPacket >> qtRot;

			pOwner->SetTargetRot( qtRot );
			ChangeToChargeAttack( pOwner );
		}
		break;

	case	ST_ROTATE:
		{
			int iDir;
			rkPacket >> iDir;

			DWORD dwCurTime = FRAMEGETTIME();
			int iAniID = 0;

			if( iDir == ioUserKeyInput::DKI_NONE )
			{
				m_CurDirKey = ioUserKeyInput::DKI_NONE;
			}
			else
			{
				m_bTargetRot = false;
				m_CurDirKey = (ioUserKeyInput::DirKeyInput)iDir;
			}
		}
		break;

	case	ST_CANCEL_DASH:
		{
			D3DXVECTOR3 vPos;
			rkPacket >> vPos;
			pOwner->SetWorldPosition( vPos );

			int eCurrDir, eCallerType;
			rkPacket >> eCurrDir;
			rkPacket >> eCallerType;
			SetChangeConditionalSpecialDash( pOwner, static_cast<ioUserKeyInput::DirKeyInput>( eCurrDir ), eCallerType );
		}
		break;

	case	ST_CANCEL_DASH_ATTACK:
		{
			D3DXVECTOR3 vPos;
			rkPacket >> vPos;
			pOwner->SetWorldPosition( vPos );

			int eCurrDir;
			rkPacket >> eCurrDir;
			SetAttackAfterSpecialDash( pOwner, static_cast<ioUserKeyInput::DirKeyInput>( eCurrDir ) );
		}
		break;

	case	ST_JUMP_DASH:
		{
			D3DXVECTOR3 vPos;
			rkPacket >> vPos;
			pOwner->SetWorldPosition( vPos );

			int eCurrDir;
			rkPacket >> eCurrDir;
			ChangeToJumpDashState( pOwner, static_cast<ioUserKeyInput::DirKeyInput>( eCurrDir ) );
		}
		break;

	case	ST_CHAIN_ATTACK_ADD:
		{
			int eChainAttackState;
			rkPacket >> eChainAttackState;

			D3DXQUATERNION	qtRot;
			rkPacket >> qtRot;

			pOwner->SetTargetRot( qtRot );
			ChangeAttackAdd( pOwner );
		}
		break;

	case	ST_CLAW_ATTACK:
		{
			D3DXQUATERNION	qtRot;
			rkPacket >> qtRot;
			rkPacket >> m_Target;

			pOwner->SetTargetRot( qtRot );
			//SetTarget( szTarget, pOwner );
			ChangeToClawAttack( pOwner );
		}
		break;

	case	ST_END_STATE:
		{
			D3DXVECTOR3 vPos;
			rkPacket >> vPos;
			pOwner->SetWorldPosition( vPos );

			SetChangeEndState( pOwner );
		}
		break;
	}
}

void	ioMidnightItem::OnItemGaugeCancel( ioBaseChar *pOwner, SP2Packet &rkPacket )
{
	if( !pOwner ) 
		return;

	
}

bool	ioMidnightItem::IsChargeAttackState( ioBaseChar *pOwner )
{
	if( pOwner->GetState() == CS_ATTACK )
	{
		if( m_ChargeState == CS_CHARGING )
			return true;
	}

	return	false;
}

bool	ioMidnightItem::IsConditionalSpecialDashReserve( ioBaseChar *pOwner )
{
	switch( m_MidnightState )
	{
	case MS_CANCEL_DASH:
		return false;
	}

	if( pOwner->GetState() == CS_ATTACK || pOwner->GetState() == CS_EXTEND_DASH )
		return true;

	return false;
}

bool	ioMidnightItem::IsEnableAttackDashCheck( ioBaseChar *pOwner, bool bCheck )
{
	if( !pOwner ) 
		return	false;

	if( pOwner->GetState() == CS_ATTACK )
	{
		switch( m_ChargeState )
		{
		case	CS_ATTACK_FIRE:
			if( !bCheck )
				return	false;

			return	true;
		}
	}
	return	false;
}

bool	ioMidnightItem::SetJumpAttack( ioBaseChar *pOwner, int iAniID, float fTimeRate, AttackType eType )
{
	if( !pOwner ) 
		return	false;

	DWORD	dwCurTime	= FRAMEGETTIME();
	ClearAirJump();
	ClearJumpCmdInfo();

	AttackType	eExtendType	= AT_JUMP_ATTACK;

	const	AttackAttribute*	pAttr = GetAttackAttribute( eExtendType );
	if( !pAttr )
		return	false;

	ioEntityGroup*	pGrp	= pOwner->GetGroup();
	iAniID	= pGrp->GetAnimationIdx( pAttr->m_AttackAnimation );
	if( iAniID == -1 )
		return	false;

	fTimeRate	= pAttr->m_fAttackAniRate;
	DWORD	dwPreDelay	= pAttr->m_dwPreDelay;

	// For AirJumpPower
	SetAirJump( pOwner, iAniID, fTimeRate );

	return	false;
}

void	ioMidnightItem::ProcessJumpAttack( ioBaseChar *pOwner, float fHeightGap )
{
	if( m_pExtendJump )
	{
		m_pExtendJump->ProcessJumpAttack( pOwner, fHeightGap );
	}

	CheckAirJump( pOwner );
}

void	ioMidnightItem::UpdateExtraData( ioBaseChar *pOwner )
{
	if( !pOwner )
		return;

	float	fTimePerSec	= g_FrameTimer.GetSecPerFrame();
	if( pOwner->IsSystemState() )
		return;

	if( m_bObjectReleaseAutoJump )
	{
		pOwner->SetCurMoveSpeed( 0.0f );
		pOwner->SetExtendAttackEndJump( m_fCancelDashEndJumpAmt, m_fCancelDashRandRate, true, true, true );
		SetJumpState();
		m_bObjectReleaseAutoJump	= false;
	}

	float fHeightGap = pOwner->GetWorldPosition().y - pOwner->GetBottomHeight();
	if( fHeightGap < FLOAT1 )
		m_bJumpDashEnable	= true;

	if( m_ObjectEquipState != OES_NONE )
	{
		float	fGauge	= m_fExtraGaugeBaseDec * fTimePerSec;
		m_fCurExtraGauge	-= fGauge;
		m_fCurExtraGauge	= max( m_fCurExtraGauge, 0 );
	}
	else
	{
		switch( pOwner->GetState() )
		{
		case CS_DELAY:
		case CS_DASH:
		case CS_RUN:
		case CS_DEFENSE:
		case CS_SPECIAL_MOTION:
			if( m_fCurBullet < m_fMaxBullet )
			{
				float fGauge = m_fIncreseGaugeTic * fTimePerSec;

				m_fCurBullet += fGauge;
				m_fCurBullet = min( m_fCurBullet, m_fMaxBullet );
			}
			return;
		case CS_JUMP:
			{
				if ( pOwner->GetJumpState() != JS_JUMPPING )
					return;

				if ( pOwner->GetUsedBuffJump() != BJUT_NONE )
					return;
				if ( pOwner->GetUsedBuffFlyJump() != BJUT_NONE )
					return;
				if ( pOwner->GetUsedBuffMultiFlyJump() != BJUT_NONE )
					return;

				if( m_fCurBullet < m_fMaxBullet )
				{
					float fGauge = m_fIncreseGaugeTic * fTimePerSec;

					m_fCurBullet += fGauge;
					m_fCurBullet = min( m_fCurBullet, m_fMaxBullet );
				}
			}
			return;
		default:
			if ( pOwner->IsWoundedState() && m_fCurBullet < m_fMaxBullet )
			{
				float fGauge = m_fIncreseGaugeTic * fTimePerSec;

				m_fCurBullet += fGauge;
				m_fCurBullet = min( m_fCurBullet, m_fMaxBullet );
			}
			return;
		}
	}
}

void	ioMidnightItem::SetChangeConditionalSpecialDash( ioBaseChar *pOwner, ioUserKeyInput::DirKeyInput eCurrDir, int eCallerType )
{
	if( !pOwner || eCurrDir == ioUserKeyInput::DKI_NONE )
		return;

	ioEntityGroup *pGrp = pOwner->GetGroup();
	if( !pGrp )
		return;

	int iAniID = pGrp->GetAnimationIdx( m_CancelDashAttribute.m_AttackAnimation );
	if ( iAniID == -1 )
		return;

	DWORD dwPreDelay = m_CancelDashAttribute.m_dwPreDelay;
	DWORD dwEndDelay = m_CancelDashAttribute.m_dwEndDelay;
	DWORD dwStartTime = FRAMEGETTIME() + dwPreDelay;

	ClearCancelInfo();
	pOwner->ClearDirDoubleClick();
	pOwner->ClearReserveKeyInput();
	pOwner->ClearReservedSliding();
	pOwner->SetCurJumpPowerAmt( 0.0f );
	pOwner->SetForcePowerAmt( 0.0f );
	pOwner->SetGravityAmt( 0.0f );
	pOwner->SetChargingState( false );
	pOwner->SetState( CS_MIDNIGHT_SPECIAL );

	m_MidnightState = MS_CANCEL_DASH;
	float fYawD = 180.0f + 45.0f * ( eCurrDir - ioUserKeyInput::DKI_UP );
	fYawD = ioMath::ArrangeHead( fYawD );

	float fYawR = DEGtoRAD( fYawD );
	D3DXQUATERNION qtRotate;
	D3DXQuaternionRotationYawPitchRoll( &qtRotate, fYawR, 0.0f, 0.0f );

	pOwner->SetTargetRotToRotate( qtRotate, true, false );

	float fTimeRate = m_CancelDashAttribute.m_fAttackAniRate;
	if( fTimeRate <= 0.0f )
		fTimeRate = FLOAT1;

	pGrp->ClearAllActionAni( 0.0f, true );
	pGrp->ClearAllLoopAni( 0.0f, true );
	pGrp->SetActionAni( iAniID, FLOAT100, FLOAT100, FLOAT1, FLOAT1/fTimeRate, 0.0f, dwPreDelay );

	pOwner->SetReservedSliding( m_CancelDashAttribute.m_vForceInfoList, iAniID, fTimeRate, dwPreDelay );

	DWORD dwCurBaseIndex = pOwner->IncreaseWeaponIndexBase();
	pOwner->RefreshFireTimeListWithBase( dwCurBaseIndex,
		iAniID,
		m_CancelDashAttribute.m_vWeaponInfoList,
		FTT_NORMAL_ATTACK,
		fTimeRate,
		dwPreDelay );

	pOwner->CheckCharColSkipTime( iAniID, fTimeRate, dwPreDelay );

	m_dwCancelDashMotionEndTime = dwStartTime + pGrp->GetAnimationFullTime(iAniID) * fTimeRate + dwEndDelay;

	DWORD dwKeyReserve = pGrp->GetAnimationEventTime_e( iAniID, "key_reserve" ) * fTimeRate;
	if( dwKeyReserve > 0 )
		m_dwKeyReserveTime = dwStartTime + dwKeyReserve;

	DWORD dwInputSpecialDashTime = pGrp->GetAnimationEventTime_e( iAniID, "input_dash_s" ) * fTimeRate;
	if( dwInputSpecialDashTime > 0 )
		m_dwCancelDashAttackTime = dwStartTime + dwInputSpecialDashTime;

	if ( pOwner->IsNeedSendNetwork() )
	{
		SP2Packet kPacket( CUPK_ITEM_GAUGE_OK );
		FillItemGaugePacketHeader( pOwner, kPacket );		
		kPacket << ST_CANCEL_DASH;
		kPacket << pOwner->GetWorldPosition();
		kPacket << static_cast<int>( eCurrDir );
		kPacket << eCallerType;
		P2PNetwork::SendToAllPlayingUser( kPacket );
	}
}

bool	ioMidnightItem::ProcessCancelBySpeicalDash( ioBaseChar *pOwner )
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

void	ioMidnightItem::SetAttackAfterSpecialDash( ioBaseChar *pOwner, ioUserKeyInput::DirKeyInput eCurrDir )
{
	if( !pOwner )
		return;

	ioEntityGroup *pGrp = pOwner->GetGroup();
	if( !pGrp )
		return;

	int iAniID = pGrp->GetAnimationIdx( m_CancelDashAddAttackAttribute.m_AttackAnimation );
	if ( iAniID == -1 )
		return;

	DWORD dwPreDelay = m_CancelDashAddAttackAttribute.m_dwPreDelay;
	DWORD dwEndDelay = m_CancelDashAddAttackAttribute.m_dwEndDelay;
	DWORD dwStartTime = FRAMEGETTIME() + dwPreDelay;

	m_MidnightState = MS_CANCEL_DASH_ADD_ATTACK;

	if( eCurrDir != ioUserKeyInput::DKI_NONE )
	{
		float fYawD = 180.0f + 45.0f * ( eCurrDir - ioUserKeyInput::DKI_UP );
		fYawD = ioMath::ArrangeHead( fYawD );

		float fYawR = DEGtoRAD( fYawD );
		D3DXQUATERNION qtRotate;
		D3DXQuaternionRotationYawPitchRoll( &qtRotate, fYawR, 0.0f, 0.0f );

		pOwner->SetTargetRotToRotate( qtRotate, true, false );
	}

	float fTimeRate = m_CancelDashAddAttackAttribute.m_fAttackAniRate;
	if( fTimeRate <= 0.0f )
		fTimeRate = FLOAT1;

	pGrp->ClearAllActionAni( 0.0f, true );
	pGrp->ClearAllLoopAni( 0.0f, true );
	pGrp->SetActionAni( iAniID, FLOAT100, FLOAT100, FLOAT1, FLOAT1/fTimeRate, 0.0f, dwPreDelay );

	pOwner->SetReservedSliding( m_CancelDashAddAttackAttribute.m_vForceInfoList, iAniID, fTimeRate, dwPreDelay );

	DWORD dwCurBaseIndex = pOwner->IncreaseWeaponIndexBase();
	pOwner->RefreshFireTimeListWithBase( dwCurBaseIndex,
		iAniID,
		m_CancelDashAddAttackAttribute.m_vWeaponInfoList,
		FTT_NORMAL_ATTACK,
		fTimeRate,
		dwPreDelay );

	//pOwner->CheckCharColSkipTime( iAniID, fTimeRate, dwPreDelay );

	m_dwCancelDashAttackMotionEndTime = dwStartTime + pGrp->GetAnimationFullTime(iAniID) * fTimeRate + dwEndDelay;

	if ( pOwner->IsNeedSendNetwork() )
	{
		SP2Packet kPacket( CUPK_ITEM_GAUGE_OK );
		FillItemGaugePacketHeader( pOwner, kPacket );		
		kPacket << ST_CANCEL_DASH_ATTACK;
		kPacket << pOwner->GetWorldPosition();
		kPacket << static_cast<int>( eCurrDir );
		P2PNetwork::SendToAllPlayingUser( kPacket );
	}
}

void	ioMidnightItem::EquipObjectItemByAttakable( ioBaseChar* pOwner, ioItem* pObjectItem )
{
	m_bObjectReleaseSend	 = false;
	//pOwner->SetState( CS_MIDNIGHT_SPECIAL );
	//m_MidnightState	= MS_JUMP_COMMAND;
}

void	ioMidnightItem::SetExtraGaugeTypeByWeapon( ioBaseChar *pOwner, int iType, int iWGT, float fDamage )
{
	if( !pOwner )
		return;

	bool	bEquipObject	= IsEquipObject( pOwner );
	float	fRate	= 0.0f;
	switch( iType )
	{
	case	EGT_WOUNDED:
		if( bEquipObject )
			fRate	= m_fExtraGaugeWoundDec;
		else
			fRate	= 0.0f;
		break;
	}

	m_fCurExtraGauge	-= fRate;
	m_fCurExtraGauge	= max( 0.0f, m_fCurExtraGauge );
}

int	ioMidnightItem::GetMaxBullet()
{
	return	(int)m_fMaxExtraGauge;
}

int	ioMidnightItem::GetNeedBullet()
{
	return	(int)m_fNeedChargeBullet;
}

void	ioMidnightItem::DecreaseBullet()
{
	m_fCurBullet	-= m_fNeedChargeBullet;
	m_fCurBullet	= max( 0.0f, m_fCurBullet );
}

int	ioMidnightItem::GetCurBullet()
{
	float	fCurBullet	= m_fCurBullet;
	fCurBullet	= max( 0, min( fCurBullet, m_fMaxBullet ) );

	return	(int)fCurBullet;
}

void	ioMidnightItem::SetCurBullet( int iCurBullet )
{
	if( iCurBullet < 0 )
		return;

	float	fCurBullet	= iCurBullet;
	fCurBullet	= max( 0, min( fCurBullet, m_fMaxBullet ) );

	m_fCurBullet	= fCurBullet;
}

float	ioMidnightItem::GetMaxExtraGauge()
{
	return	m_fMaxExtraGauge;
}

float	ioMidnightItem::GetCurExtraGauge()
{
	float	fCurGauge	= m_fCurExtraGauge;
	fCurGauge	= max( 0, min( fCurGauge, m_fMaxExtraGauge ) );

	return	fCurGauge;
}

float	ioMidnightItem::GetNeedExtraGauge()
{
	return	0.0f;
}

bool	ioMidnightItem::IsEquipObject( ioBaseChar *pOwner )
{
	if( pOwner )
	{
		ioObjectItem*	pObjectItem	= pOwner->GetObject();
		if( pObjectItem && ( pObjectItem->GetItemCode() == m_dwScytheObjectItem || pObjectItem->GetItemCode() == m_dwGunObjectItem ) )
		{
			return	true;
		}
	}

	return	false;
}

void	ioMidnightItem::ChangeEquipObjectState( ioBaseChar *pOwner, DWORD dwObject, EquipObjectState eState /*= EOS_NONE*/ )
{
	if( !pOwner )
		return;

	if( !pOwner->IsNeedProcess() )
		return;

	m_dwInputCancelCheckTime	= FRAMEGETTIME();
	m_dwCancelCheckTime			= FRAMEGETTIME();

	//pOwner->SetObjectEquipState( &m_ChargeAutoAttribute );

	//m_ChargeState	= CS_EQUIP_OBJECT;
	ioPlayStage*	pStage	= g_ItemMaker.GetPlayStage();

	if( pStage && dwObject > 0 )
	{
		ioItem*	pItem	= pStage->CreateItem( dwObject );
		if( pItem )
		{
			ioObjectItem*	pObject	= ToObjectItem( pItem );
			if( pObject )
			{
				pObject->ReLoadProperty();

				EquipObjectItemByAttakable( pOwner, pObject );
				pObject->SetObjectCreateItem( GetName() );
				pObject->SetObejctCreateType( ioObjectItem::OCT_SOILDER );
				pObject->SetItemCreateIndex( 0 );
				pOwner->EquipItem( pObject );			
			}
		}

		if( pOwner->IsNeedSendNetwork() )
		{
			pOwner->GetEquipSlot()->SetEquipWaitSlot( ES_OBJECT, true );

			SP2Packet kUdpPacket( CUPK_ITEM_GAUGE_OK );
			FillItemGaugePacketHeader( pOwner, kUdpPacket );
			kUdpPacket << ST_OBJECT_EQUIP;
			kUdpPacket << static_cast<byte>( eState );
			kUdpPacket << static_cast<byte>( m_ObjectEquipState );
			kUdpPacket << dwObject;
			kUdpPacket << pOwner->GetWorldPosition();
			kUdpPacket << pOwner->GetWorldOrientation();
			P2PNetwork::SendToAllPlayingUser( kUdpPacket );

			//아래 패킷으로 ioAttackableItem::EquipObjectItemByAttakable()가 호출됨
			SP2Packet kPacket( CTPK_CREATE_OBJECTITEM );
			kPacket << pOwner->GetWorldPosition();
			kPacket << dwObject;
			kPacket << GetName();
			kPacket << pOwner->GetCharName();
			kPacket << true;
			kPacket << static_cast<int>( ioObjectItem::OCT_SOILDER );
			TCPNetwork::SendToServer( kPacket );
		}
	}
}

void	ioMidnightItem::OnChangeEquipObjectState( ioBaseChar* pOwner, DWORD dwObject, EquipObjectState eState /* = EOS_NORMAL */, ObjectEquipState eEquipState /* = OES_NONE */ )
{
	if( !pOwner )
		return;

	m_dwInputCancelCheckTime	= FRAMEGETTIME();
	m_dwCancelCheckTime			= FRAMEGETTIME();

	m_ObjectEquipState	= eEquipState;

	ioPlayStage*	pStage	= g_ItemMaker.GetPlayStage();

	if( pStage && dwObject > 0 )
	{
		ioItem*	pItem	= pStage->CreateItem( dwObject );
		if( pItem )
		{
			ioObjectItem*	pObject	= ToObjectItem( pItem );
			if( pObject )
			{
				pObject->ReLoadProperty();

				EquipObjectItemByAttakable( pOwner, pObject );
				pObject->SetObjectCreateItem( GetName() );
				pObject->SetObejctCreateType( ioObjectItem::OCT_SOILDER );
				pObject->SetItemCreateIndex( 0 );
				pOwner->EquipItem( pObject );			
			}
		}
	}
}

void	ioMidnightItem::ReleaseObjectItemByAttack( ioBaseChar* pOwner, ioItem* pObjectItem )
{
	ioMidnightScytheObjectItem	*pMIdnightScythe	= ToObjectMidnightScytheItem( pObjectItem );
	if( pMIdnightScythe )
	{
		ReleaseObject( pOwner );
	}

	ioMidnightGunObjectItem	*pMIdnightGun	= ToObjectMidnightGunItem( pObjectItem );
	if( pMIdnightGun )
	{
		ReleaseObject( pOwner );
	}
}

void	ioMidnightItem::ReleaseObject( ioBaseChar *pOwner, bool bNeedProcess /* = true */ )
{
	if( !pOwner )
		return;

	if( bNeedProcess && !pOwner->IsNeedProcess() )
		return;

	if( m_bObjectReleaseSend )
		return;

	m_ChargeState	= CS_NONE;
	pOwner->AttachEffect( m_szRleaseObjectEffect );

	if( pOwner->IsNeedSendNetwork() )
	{
		SP2Packet kUdpPacket( CUPK_ITEM_GAUGE_OK );
		FillItemGaugePacketHeader( pOwner, kUdpPacket );
		kUdpPacket << ST_OBJECT_RELEASE;
		kUdpPacket << pOwner->GetWorldPosition();
		kUdpPacket << pOwner->GetWorldOrientation();
		P2PNetwork::SendToAllPlayingUser( kUdpPacket );
	}

	ioObjectItem*	pObjectItem	= pOwner->GetObject();
	if( pObjectItem && ( pObjectItem->GetItemCode() == m_dwScytheObjectItem || pObjectItem->GetItemCode() == m_dwGunObjectItem ) )
	{
		//애니메이션 초기화
		if( pOwner->GetState() == CS_DELAY )
		{
			pOwner->ReleaseObjectItemImmediately( __FUNCTION__, CS_DELAY, true );
		}
		else if( pOwner->GetState() == CS_RUN )
		{
			pOwner->ReleaseObjectItemImmediately( __FUNCTION__, CS_RUN, true );
		}
		else if( pOwner->GetState() == CS_DASH )
		{
			pOwner->ReleaseObjectItemImmediately( __FUNCTION__, CS_DASH, true );
		}
		else
		{
			pOwner->ReleaseObjectItemImmediately( __FUNCTION__ );
		}
	}

	ResetObjectEquipState( pOwner, CS_DELAY );

	m_bObjectReleaseSend	= true;

	if( pObjectItem->GetItemCode() == m_dwGunObjectItem )
		pOwner->SetState( CS_DELAY );

	ioIljimaeJump*	pIljimaeJump	= ToIljimaeJump( m_pExtendJump );
	if( pIljimaeJump )
		pIljimaeJump->ResetJumpState();
}

void	ioMidnightItem::ReleaseObjectState( ioBaseChar* pOwner, ioObjectItem* pReleasedObject )
{	
	m_ChargeState	= CS_NONE;

	if( IsEquipObject( pOwner ) )
	{
		pOwner->AttachEffect( m_szRleaseObjectEffect );
	}
}

void	ioMidnightItem::ChangeAttackStart( ioBaseChar* pOwner )
{
	if( !pOwner )
		return;

	ioEntityGroup *pGrp = pOwner->GetGroup();
	if( !pGrp )
		return;

	m_ChainAttackState	= CAS_START;

	pOwner->InitExtendAttackTagInfo();

	pGrp->ClearAllLoopAni( FLOAT10, true );
	pGrp->ClearAllActionAni( FLOAT10, true );

	//트랙킹
	ChangeTrackingUD_Fire( pOwner );

	//상중하 다 시간은 같다.
	int iAniID	     = pGrp->GetAnimationIdx( m_CurAttackInfo.m_StartAniCenter );

	DWORD dwCurBaseIndex = pOwner->IncreaseWeaponIndexBase();
	DWORD dwPreDelay = m_CurAttackInfo.m_AddAttack.m_dwPreDelay;
	pOwner->RefreshFireTimeListWithBase( dwCurBaseIndex, iAniID, m_CurAttackInfo.m_FirstAttack.m_vWeaponInfoList, FTT_NORMAL_ATTACK, m_CurAttackInfo.m_FirstAttack.m_fAttackAniRate, dwPreDelay );

	m_dwAttackStartTime = FRAMEGETTIME(); 
	m_dwMotionEndTime = m_dwAttackStartTime;
	m_dwMotionEndTime += ( pGrp->GetAnimationFullTime( iAniID ) * m_CurAttackInfo.m_FirstAttack.m_fAttackAniRate );
}

void	ioMidnightItem::SetUseExtraGauge( float fAmt )
{
	m_fCurExtraGauge	-= fAmt;
	m_fCurExtraGauge	= max( 0.0f, m_fCurExtraGauge );
}

void	ioMidnightItem::ChangeToDashChargeAttack( ioBaseChar* pOwner, const MidnightAttackInfo& pAttackInfo )
{
	if ( !pOwner )
		return;

	ioEntityGroup *pGrp = pOwner->GetGroup();
	if( !pGrp )
		return;

	m_MidnightState	= MS_DASH_CHARGE;
	m_CurAttackInfo	= pAttackInfo;
	pOwner->SetState( CS_MIDNIGHT_SPECIAL );

	if( pOwner->IsNeedProcess() )
	{
		FindChainAttTarget( pOwner );
		if( CompareTarget( pOwner ) )
		{
			m_Target.Clear();
			m_bAimedBall = true;
		}
	}


	if( pOwner->IsNeedProcess() )
	{
		bool bTargetOk = false;
		D3DXVECTOR3 vTargetPos;
		D3DXVECTOR3 vDir;

		if( m_bAimedBall )
		{
			ioBall *pBall = pOwner->FindBall();
			if( pBall )
			{
				bTargetOk = true;
				vDir = pBall->GetMidPositionByRate() - pOwner->GetMidPositionByRate();
			}
		}
		else
		{
			ioBaseChar *pTarget = pOwner->GetBaseChar( m_Target );
			if( pTarget )			
			{
				bTargetOk = true;
				vDir = pTarget->GetMidPositionByRate(m_CurAttackInfo.m_fTargetHeightRate) - pOwner->GetMidPositionByRate(m_CurAttackInfo.m_fAttackerHeightRate);
			}
		}

		if( !bTargetOk )
		{
			vTargetPos = pOwner->GetMidPositionByRate();

			D3DXVECTOR3 vDir = pOwner->GetTargetRot() * D3DXVECTOR3( 0.0f, 0.0f, -FLOAT1 );
			D3DXVec3Normalize( &vDir, &vDir );

			//////////////////////////////////////////////////////////////////////////
			D3DXVECTOR3 vAxis;
			D3DXVec3Cross( &vAxis, &vDir, &D3DXVECTOR3( 0.0f, -FLOAT1, 0.0f ) );

			D3DXQUATERNION qtRot;
			D3DXQuaternionRotationAxis( &qtRot, &vAxis, DEGtoRAD(m_CurAttackInfo.m_fAttackMoveAngle) );
			vDir = qtRot * vDir;
			D3DXVec3Normalize( &vDir, &vDir );
			//////////////////////////////////////////////////////////////////////////
			//m_vFlyMoveDir = vDir;
		}
		else
		{
			float fRange = D3DXVec3Length( &vDir );
			D3DXVec3Normalize( &vDir, &vDir );

			D3DXVECTOR3 vRotDir = vDir;
			vRotDir.y = 0.0f;
			D3DXVec3Normalize( &vRotDir, &vRotDir );

			D3DXQUATERNION qtNewRot;
			ioMath::CalcDirectionQuaternion( qtNewRot, -vRotDir );
			pOwner->SetTargetRotToRotate( qtNewRot, true );

			//m_vFlyMoveDir = vDir;
		}
	}

	ChangeAttackStart( pOwner );
	m_dwHitedTime = 0;
}

void	ioMidnightItem::ChangeToClawAttack( ioBaseChar* pOwner )
{
	if ( !pOwner )
		return;

	ioEntityGroup *pGrp = pOwner->GetGroup();
	if( !pGrp )
		return;

	pOwner->SetState( CS_MIDNIGHT_SPECIAL );
	m_MidnightState	= MS_CLAW_ATTACK;
	m_CurAttackInfo	= m_ClawAttackInfo;

	//if( pOwner->IsNeedProcess() )
	{
		FindChainAttTarget( pOwner );
		if( CompareTarget( pOwner ) )
		{
			m_Target.Clear();
			m_bAimedBall = true;
		}
	}


	//if( pOwner->IsNeedProcess() )
	{
		bool bTargetOk = false;
		D3DXVECTOR3 vTargetPos;
		D3DXVECTOR3 vDir;

		if( m_bAimedBall )
		{
			ioBall *pBall = pOwner->FindBall();
			if( pBall )
			{
				bTargetOk = true;
				vDir = pBall->GetMidPositionByRate() - pOwner->GetMidPositionByRate();
			}
		}
		else
		{
			ioBaseChar *pTarget = pOwner->GetBaseChar( m_Target );
			if( pTarget )			
			{
				bTargetOk = true;
				vDir = pTarget->GetMidPositionByRate(m_CurAttackInfo.m_fTargetHeightRate) - pOwner->GetMidPositionByRate(m_CurAttackInfo.m_fAttackerHeightRate);
			}
		}

		if( !bTargetOk )
		{
			vTargetPos = pOwner->GetMidPositionByRate();

			D3DXVECTOR3 vDir = pOwner->GetTargetRot() * D3DXVECTOR3( 0.0f, 0.0f, -FLOAT1 );
			D3DXVec3Normalize( &vDir, &vDir );

			//////////////////////////////////////////////////////////////////////////
			D3DXVECTOR3 vAxis;
			D3DXVec3Cross( &vAxis, &vDir, &D3DXVECTOR3( 0.0f, -FLOAT1, 0.0f ) );

			D3DXQUATERNION qtRot;
			D3DXQuaternionRotationAxis( &qtRot, &vAxis, DEGtoRAD(m_CurAttackInfo.m_fAttackMoveAngle) );
			vDir = qtRot * vDir;
			D3DXVec3Normalize( &vDir, &vDir );
			//////////////////////////////////////////////////////////////////////////
			//m_vFlyMoveDir = vDir;
		}
		else
		{
			float fRange = D3DXVec3Length( &vDir );
			D3DXVec3Normalize( &vDir, &vDir );

			D3DXVECTOR3 vRotDir = vDir;
			vRotDir.y = 0.0f;
			D3DXVec3Normalize( &vRotDir, &vRotDir );

			D3DXQUATERNION qtNewRot;
			ioMath::CalcDirectionQuaternion( qtNewRot, -vRotDir );
			pOwner->SetTargetRotToRotate( qtNewRot, true );

			//m_vFlyMoveDir = vDir;
		}
	}

	ChangeAttackStart( pOwner );
	m_dwHitedTime = 0;
}

bool	ioMidnightItem::SetTarget( const ioHashString &szTargetName, ioBaseChar *pOwner )
{
	//명중 
	m_Target = szTargetName;
	m_dwHitedTime = FRAMEGETTIME();
	ChangeAttackHited( pOwner );
	return true;
}

bool	ioMidnightItem::IsEnableTargetState( ioBaseChar *pTarget )
{
	if( !pTarget )
		return false;

	if( pTarget->IsSystemState() )
		return false;

	if( !pTarget->IsEnableTarget() )
		return false;

	// 쓰러짐 관련 체크
	if( !pTarget->CheckTargetWoundType( m_CurAttackInfo.m_TargetWoundType ) )
		return false;

	return true;
}

void	ioMidnightItem::FindChainAttTarget( ioBaseChar *pOwner )
{
	m_bAimedBall = false;
	m_Target.Clear();

	if( !pOwner )
		return;

	ioPlayStage *pStage = pOwner->GetCreator();
	if( !pStage )
		return;

	ioFootballMode *pFootBall = ToFootballMode( pStage->GetPlayMode() );
	if( pFootBall && pFootBall->IsDisableAttackMode() )
		return;

	if( pOwner->GetExperienceMode() != EMS_NONE )
		return;

	D3DXVECTOR3 vCurPos = pOwner->GetMidPositionByRate();
	D3DXVECTOR3 vDiff= ioMath::VEC3_ZERO;

	float fCurAngle = ioMath::ArrangeHead( m_CurAttackInfo.m_fTargetAngle );
	float fHalfValue = cosf( DEGtoRAD(fCurAngle*0.5f) );
	float fTargetRangeSq = m_CurAttackInfo.m_fTargetRange * m_CurAttackInfo.m_fTargetRange;

	const BaseCharList &rkCharList = pStage->GetBaseCharList();
	BaseCharList::const_iterator iter;

	ioHashString szTarget;
	float fCurGap = -FLOAT1;
	for( iter=rkCharList.begin() ; iter!=rkCharList.end() ; ++iter )
	{
		ioBaseChar *pTarget = *iter;
		if( !pTarget ) continue;

		if( pTarget->GetTeam() == pOwner->GetTeam() ) continue;
		if( pTarget->GetCharName() == GetOwnerName() ) continue;
		if( pTarget->GetExperienceMode() != EMS_NONE ) continue;

		if( !IsEnableTargetState( pTarget ) )
			continue;

		D3DXVECTOR3 vTargetPos = pTarget->GetMidPositionByRate();
		vDiff = vTargetPos - vCurPos;
		float fDiffRangeSq = D3DXVec3LengthSq( &vDiff );

		// 각도 체크
		if( fCurAngle != 0.0f )
		{
			D3DXQUATERNION qtRot = pOwner->GetTargetRot();
			D3DXVECTOR3 vDir = qtRot * D3DXVECTOR3( 0.0f, 0.0f, -FLOAT1 );
			D3DXVec3Normalize( &vDir, &vDir );
			D3DXVECTOR3 vDiff2 = vDiff;
			vDiff2.y = 0.0f;
			D3DXVec3Normalize( &vDiff2, &vDiff2 );

			float fCurValue = D3DXVec3Dot( &vDir, &vDiff2 );
			if( fCurValue < fHalfValue )
				continue;
		}

		// 높이 체크
		float fHeightGap = vTargetPos.y - vCurPos.y;
		if( fHeightGap > 0.0f && fHeightGap > m_CurAttackInfo.m_fTargetUpHeight )
			continue;

		if( fHeightGap < 0.0f && fabs(fHeightGap) > m_CurAttackInfo.m_fTargetDownHeight )
			continue;

		// 거리 체크
		if( fDiffRangeSq < fTargetRangeSq )
		{
			if( fCurGap == -FLOAT1 )
			{
				fCurGap = fDiffRangeSq;
				szTarget = pTarget->GetCharName();
			}
			else if( fCurGap >= 0.0f && fDiffRangeSq < fCurGap )
			{
				fCurGap = fDiffRangeSq;
				szTarget = pTarget->GetCharName();
			}
		}
	}

	if( !szTarget.IsEmpty() )
	{
		ioBaseChar *pTarget = pStage->GetBaseChar( szTarget );
		if( pTarget )
		{
			m_Target = szTarget;
		}
	}
}

bool	ioMidnightItem::CompareTarget( ioBaseChar *pOwner )
{
	if( !pOwner ) return false;

	ioPlayStage *pStage = pOwner->GetCreator();
	if( !pStage )
		return false;

	ioBall *pBall = pOwner->FindBall();
	if( !pBall ) return false;

	D3DXVECTOR3 vCurPos = pOwner->GetMidPositionByRate();
	D3DXVECTOR3 vDiff= ioMath::VEC3_ZERO;

	float fCurAngle = ioMath::ArrangeHead( m_CurAttackInfo.m_fTargetAngle );
	float fHalfValue = cosf( DEGtoRAD(fCurAngle*0.5f) );
	float fTargetRangeSq = m_CurAttackInfo.m_fTargetRange * m_CurAttackInfo.m_fTargetRange;

	D3DXVECTOR3 vTargetPos = pBall->GetMidPositionByRate();
	vDiff = vTargetPos - vCurPos;
	float fDiffRangeSq = D3DXVec3LengthSq( &vDiff );

	// 각도 체크
	if( fCurAngle != 0.0f )
	{
		D3DXQUATERNION qtRot = pOwner->GetTargetRot();
		D3DXVECTOR3 vDir = qtRot * D3DXVECTOR3( 0.0f, 0.0f, -FLOAT1 );
		D3DXVec3Normalize( &vDir, &vDir );
		D3DXVECTOR3 vDiff2 = vDiff;
		vDiff2.y = 0.0f;
		D3DXVec3Normalize( &vDiff2, &vDiff2 );

		float fCurValue = D3DXVec3Dot( &vDir, &vDiff2 );
		if( fCurValue < fHalfValue )
			return false;
	}

	// 높이 체크
	float fHeightGap = vTargetPos.y - vCurPos.y;
	if( fHeightGap > 0.0f && fHeightGap > m_CurAttackInfo.m_fTargetUpHeight )
		return false;

	if( fHeightGap < 0.0f && fabs(fHeightGap) > m_CurAttackInfo.m_fTargetDownHeight )
		return false;

	// 거리 체크
	if( fDiffRangeSq >= fTargetRangeSq )
		return false;

	return true;
}

void	ioMidnightItem::RemoveWeapon( ioBaseChar *pOwner )
{
	ioWeapon *pWeapon = g_WeaponMgr.FindWeapon( pOwner, m_dwWeaponIndex );

	if( pWeapon )
	{
		ioUroborusWeapon4 *pUroborus4 =	ToUroborusWeapon4( pWeapon );
		if( pUroborus4 )
			pUroborus4->SetEndState( false, false, false );
	}
}

void	ioMidnightItem::ClearAttackState( ioBaseChar *pOwner )
{
	m_ChainAttackState = CAS_NONE;
	m_MidnightState	= MS_NONE;

	// 순서중요
	m_CurAttackInfo.Init();

	m_dwAttackStartTime = 0;
	m_dwMotionEndTime = 0;

	m_Target.Clear();

	RemoveWeapon(pOwner);
}

void	ioMidnightItem::ChangeToJumpDashState( ioBaseChar* pOwner, ioUserKeyInput::DirKeyInput eCurrDir )
{
	if( !pOwner )
		return;

	ioEntityGroup *pGrp = pOwner->GetGroup();
	if( !pGrp )
		return;

	int iAniID = pGrp->GetAnimationIdx( m_JumpDashAttribute.m_AttackAnimation );
	if ( iAniID == -1 )
		return;

	DWORD dwPreDelay = m_JumpDashAttribute.m_dwPreDelay;
	DWORD dwEndDelay = m_JumpDashAttribute.m_dwEndDelay;
	DWORD dwStartTime = FRAMEGETTIME() + dwPreDelay;

	ClearCancelInfo();
	pOwner->ClearDirDoubleClick();
	pOwner->ClearReserveKeyInput();
	pOwner->ClearReservedSliding();
	pOwner->SetCurJumpPowerAmt( 0.0f );
	pOwner->SetForcePowerAmt( 0.0f );
	pOwner->SetGravityAmt( 0.0f );
	pOwner->SetChargingState( false );
	pOwner->SetState( CS_MIDNIGHT_SPECIAL );

	m_MidnightState = MS_JUMP_DASH;
	m_bJumpDashEnable	= false;

	if( eCurrDir != ioUserKeyInput::DKI_NONE )
	{
		float fYawD = 180.0f + 45.0f * ( eCurrDir - ioUserKeyInput::DKI_UP );
		fYawD = ioMath::ArrangeHead( fYawD );

		float fYawR = DEGtoRAD( fYawD );
		D3DXQUATERNION qtRotate;
		D3DXQuaternionRotationYawPitchRoll( &qtRotate, fYawR, 0.0f, 0.0f );

		pOwner->SetTargetRotToRotate( qtRotate, true, false );
	}
	
	float fTimeRate = m_JumpDashAttribute.m_fAttackAniRate;
	if( fTimeRate <= 0.0f )
		fTimeRate = FLOAT1;

	pGrp->ClearAllActionAni( 0.0f, true );
	pGrp->ClearAllLoopAni( 0.0f, true );
	pGrp->SetActionAni( iAniID, FLOAT100, FLOAT100, FLOAT1, FLOAT1/fTimeRate, 0.0f, dwPreDelay );

	pOwner->SetReservedSliding( m_JumpDashAttribute.m_vForceInfoList, iAniID, fTimeRate, dwPreDelay );

	DWORD dwCurBaseIndex = pOwner->IncreaseWeaponIndexBase();
	pOwner->RefreshFireTimeListWithBase( dwCurBaseIndex,
		iAniID,
		m_JumpDashAttribute.m_vWeaponInfoList,
		FTT_NORMAL_ATTACK,
		fTimeRate,
		dwPreDelay );

	pOwner->CheckCharColSkipTime( iAniID, fTimeRate, dwPreDelay );

	m_dwJumpDashMotionEndTime = dwStartTime + pGrp->GetAnimationFullTime(iAniID) * fTimeRate + dwEndDelay;

	if ( pOwner->IsNeedSendNetwork() )
	{
		SP2Packet kPacket( CUPK_ITEM_GAUGE_OK );
		FillItemGaugePacketHeader( pOwner, kPacket );		
		kPacket << ST_JUMP_DASH;
		kPacket << pOwner->GetWorldPosition();
		kPacket << static_cast<int>( eCurrDir );
		P2PNetwork::SendToAllPlayingUser( kPacket );
	}
}

bool	ioMidnightItem::IsJumpDashState()
{
	if( m_MidnightState == MS_JUMP_DASH )
		return	true;

	return	false;
}

float	ioMidnightItem::GetJumpDashGravityAmt()
{
	return	m_fJumpDashGravityAmt;
}

bool	ioMidnightItem::IsJumpDashEnable()
{
	return	m_bJumpDashEnable;
}

void	ioMidnightItem::SetChangeEndState( ioBaseChar *pOwner )
{
	ClearAttackState( pOwner );

	float fHeightGap = pOwner->GetWorldPosition().y - pOwner->GetBottomHeight();
	if ( fHeightGap >= FLOAT1 )
		pOwner->SetExtendAttackEndJump( m_fCancelDashEndJumpAmt, m_fCancelDashRandRate, false, true, false, true );
	else
		pOwner->SetState( CS_DELAY );

	if ( pOwner->IsNeedSendNetwork() )
	{
		SP2Packet kPacket( CUPK_ITEM_GAUGE_OK );
		FillItemGaugePacketHeader( pOwner, kPacket );		
		kPacket << ST_END_STATE;
		kPacket << pOwner->GetWorldPosition();
		P2PNetwork::SendToAllPlayingUser( kPacket );
	}
}

void	ioMidnightItem::ChangeAttackAdd( ioBaseChar *pOwner )
{
	if( !pOwner )
		return;

	ioEntityGroup *pGrp = pOwner->GetGroup();
	if( !pGrp )
		return;

	DWORD dwCurTime = FRAMEGETTIME();

	pOwner->ClearReservedSliding();
	pGrp->ClearAllLoopAni( FLOAT10, true );
	pGrp->ClearAllActionAni( FLOAT10, true );

	int iAniID	     = pGrp->GetAnimationIdx( m_CurAttackInfo.m_AddAttack.m_AttackAnimation );
	float fTimeRate  = m_CurAttackInfo.m_AddAttack.m_fAttackAniRate;

	pGrp->SetActionAni( iAniID, FLOAT100, FLOAT100, FLOAT1, FLOAT1/fTimeRate, 0.0f, 0.0f, true );

	m_ChainAttackState = CAS_ATTACK_ADD;

	DWORD dwCurBaseIndex = pOwner->IncreaseWeaponIndexBase();
	DWORD dwPreDelay = m_CurAttackInfo.m_AddAttack.m_dwPreDelay;
	pOwner->RefreshFireTimeListWithBase( dwCurBaseIndex,
		iAniID,
		m_CurAttackInfo.m_AddAttack.m_vWeaponInfoList,
		FTT_NORMAL_ATTACK,
		fTimeRate,
		dwPreDelay );

	m_dwMotionEndTime = dwCurTime;
	m_dwMotionEndTime += ( pGrp->GetAnimationFullTime( iAniID ) * m_CurAttackInfo.m_AddAttack.m_fAttackAniRate );

	if( pOwner->IsNeedProcess() )
	{
		m_CurDirKey = pOwner->CheckCurDirKey();

		SP2Packet kPacket( CUPK_ITEM_GAUGE_OK );
		FillItemGaugePacketHeader( pOwner, kPacket );
		kPacket << (int)ST_CHAIN_ATTACK_ADD;
		kPacket << (int)m_ChainAttackState;
		kPacket << pOwner->GetTargetRot();

		P2PNetwork::SendToAllPlayingUser( kPacket );
	}

	RemoveWeapon( pOwner );
}

void	ioMidnightItem::CheckAttackHited( ioBaseChar *pOwner )
{
	ioBaseChar* pTarget = pOwner->GetBaseChar(m_Target);
	if( !pTarget )
	{
		SetChangeEndState( pOwner );
		return;
	}

	if( !IsMidnightLockState(pTarget) && m_ChainAttackState != CAS_ATTACK_ADD )
	{
		SetChangeEndState( pOwner );
		return;
	}

	if( !IsMidnightLockState(pTarget) )
		RemoveWeapon(pOwner);
}

void	ioMidnightItem::ChangeAttackHited( ioBaseChar *pOwner )
{
	if( !pOwner )
		return;

	ioEntityGroup *pGrp = pOwner->GetGroup();
	if( !pGrp )
		return;

	DWORD dwCurTime = FRAMEGETTIME();

	pOwner->ClearReservedSliding();
	pGrp->ClearAllLoopAni( FLOAT10, true );
	pGrp->ClearAllActionAni( FLOAT10, true );

	m_ChainAttackState = CAS_ATTACK_HITED;

	ChangeTrackingUD_Fire( pOwner );
}

void	ioMidnightItem::ProcessWeaponLine( ioBaseChar *pChar )
{
	if( !pChar )
		return;

	if( m_Target.IsEmpty() || m_dwWeaponIndex == 0 )
		return;


	ioBaseChar* pTargetDest = pChar->GetBaseChar( m_Target );
	ioWeapon *pWeapon = g_WeaponMgr.FindWeapon( pChar, m_dwWeaponIndex );
	ioUroborusWeapon4 *pUroborus4 =	ToUroborusWeapon4( pWeapon );
	if( !pUroborus4 )
		return;

	if( pTargetDest && IsMidnightLockState(pTargetDest) )
	{
		D3DXVECTOR3 vPosDest = pTargetDest->GetMidPositionByRate();
		pUroborus4->SetPosition( vPosDest );
	}

}

void	ioMidnightItem::ChangeTrackingUD_Fire( ioBaseChar* pOwner )
{
	ioEntityGroup *pGrp = pOwner->GetGroup();
	if( !pGrp )
		return;

	D3DXVECTOR3 vAimedDir;
	ioBaseChar *pTarget = pOwner->GetBaseChar( m_Target );
	if( pTarget )
		vAimedDir = pTarget->GetMidPositionByRate() - pOwner->GetMidPositionByRate();
	else
		vAimedDir = pOwner->GetTargetRot() * D3DXVECTOR3( 0, 0, -1 );

	D3DXVec3Normalize( &vAimedDir, &vAimedDir );

	float fTimeRate  = FLOAT1;//pAttr->m_fAttackAniRate;
	DWORD dwPreDelay = 0;//pAttr->m_dwPreDelay;

	if( m_ChainAttackState == CAS_START )
		fTimeRate = m_CurAttackInfo.m_FirstAttack.m_fAttackAniRate;
	else if( m_ChainAttackState == CAS_ATTACK_HITED )
		fTimeRate = m_CurAttackInfo.m_fAttackLoopAniRate;

	DWORD dwCurTime = FRAMEGETTIME();

	float fWeight = GetAimedUpDownWeight( vAimedDir, 85.0f );
	if( vAimedDir.y >= 0.0f )
		AimedFireToUp( pGrp, fWeight, fTimeRate, dwPreDelay, true );
	else
		AimedFireToDown( pGrp, fWeight, fTimeRate, dwPreDelay, true );
}

void	ioMidnightItem::AimedFireToUp( ioEntityGroup *pGrp, float fWeight, float fTimeRate, DWORD dwPreDelay, bool bHold )
{
	float fFadeOut = FLOAT100;
	if( !bHold )
		fFadeOut = 0.0f;

	bool bAniFx1 = false;
	bool bAniFx2 = true;

	if( fWeight > FLOAT05 )
	{
		bAniFx1 = true;
		bAniFx2 = false;
	}

	ioHashString szFireUp, szFireCenter;

	if( m_ChainAttackState == CAS_START )
	{
		szFireUp = m_CurAttackInfo.m_StartAniUp;
		szFireCenter = m_CurAttackInfo.m_StartAniCenter;
	}
	else if( m_ChainAttackState == CAS_ATTACK_HITED )
	{
		szFireUp = m_CurAttackInfo.m_AttackLoopAniUp;
		szFireCenter = m_CurAttackInfo.m_AttackLoopAniCenter;
	}

	if( fWeight > 0.0f )
	{
		if( m_ChainAttackState == CAS_START )
			pGrp->SetActionAni( szFireUp,FLOAT100, fFadeOut, fWeight, FLOAT1/fTimeRate,	0.0f, dwPreDelay,	bHold, bAniFx1 );
		else if( m_ChainAttackState == CAS_ATTACK_HITED )
			pGrp->SetLoopAni( szFireUp, FLOAT100, fWeight, FLOAT1/fTimeRate );

	}

	if( m_ChainAttackState == CAS_START )
		pGrp->SetActionAni( szFireCenter, FLOAT100, fFadeOut, FLOAT1 - fWeight, FLOAT1/fTimeRate, 0.0f, dwPreDelay, bHold, bAniFx2 );
	else if( m_ChainAttackState == CAS_ATTACK_HITED )
		pGrp->SetLoopAni( szFireCenter, FLOAT100, FLOAT1 - fWeight, FLOAT1/fTimeRate );
}

void	ioMidnightItem::AimedFireToDown( ioEntityGroup *pGrp, float fWeight, float fTimeRate, DWORD dwPreDelay, bool bHold )
{
	float fFadeOut = FLOAT100;
	if( !bHold )
		fFadeOut = 0.0f;
	bool bAniFx1 = true;
	bool bAniFx2 = false;

	if( fWeight > FLOAT05 )
	{
		bAniFx1 = false;
		bAniFx2 = true;
	}

	ioHashString szFireDown, szFireCenter;

	if( m_ChainAttackState == CAS_START )
	{
		szFireDown = m_CurAttackInfo.m_StartAniDown;
		szFireCenter = m_CurAttackInfo.m_StartAniCenter;
	}else if( m_ChainAttackState == CAS_ATTACK_HITED )
	{
		szFireDown = m_CurAttackInfo.m_AttackLoopAniDown;
		szFireCenter = m_CurAttackInfo.m_AttackLoopAniCenter;
	}

	if( fWeight > 0.0f )
	{
		if( m_ChainAttackState == CAS_START )
			pGrp->SetActionAni( szFireCenter, FLOAT100, fFadeOut, FLOAT1 - fWeight, FLOAT1/fTimeRate, 0.0f, dwPreDelay, bHold, bAniFx1 );
		else if( m_ChainAttackState == CAS_ATTACK_HITED )
			pGrp->SetLoopAni( szFireCenter, FLOAT100, FLOAT1 - fWeight, FLOAT1/fTimeRate );
	}

	if( m_ChainAttackState == CAS_START )
		pGrp->SetActionAni( szFireDown, FLOAT100, fFadeOut, fWeight, FLOAT1/fTimeRate, 0.0f, dwPreDelay, bHold, bAniFx2 );
	else if( m_ChainAttackState == CAS_ATTACK_HITED )
		pGrp->SetLoopAni( szFireDown, FLOAT100, fWeight, FLOAT1/fTimeRate );
}

float	ioMidnightItem::GetAimedUpDownWeight( const D3DXVECTOR3 &vAimDir, float fMaxAngle )
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

void	ioMidnightItem::CheckKeyInput( ioBaseChar *pOwner )
{
	if( !pOwner ) return;
	if( !pOwner->IsNeedProcess() ) return;

	if( pOwner->IsSettedDirection() )
	{
		ioUserKeyInput::DirKeyInput eNewDirKey = pOwner->CheckCurDirKey();
		if( eNewDirKey != m_CurDirKey )
		{
			m_bTargetRot = false;
			m_CurDirKey = eNewDirKey;

			if( pOwner->IsNeedSendNetwork() )
			{
				SP2Packet kPacket( CUPK_ITEM_GAUGE_OK );
				FillItemGaugePacketHeader( pOwner, kPacket );
				kPacket << ST_ROTATE;
				kPacket << (int)m_CurDirKey;
				P2PNetwork::SendToAllPlayingUser( kPacket );
			}
		}
	}
	else
	{
		if( m_CurDirKey != ioUserKeyInput::DKI_NONE )
		{
			m_CurDirKey = ioUserKeyInput::DKI_NONE;

			if( pOwner->IsNeedSendNetwork() )
			{
				SP2Packet kPacket( CUPK_ITEM_GAUGE_OK );
				FillItemGaugePacketHeader( pOwner, kPacket );
				kPacket << ST_ROTATE;
				kPacket << (int)m_CurDirKey;
				P2PNetwork::SendToAllPlayingUser( kPacket );
			}
		}
	}
}

void	ioMidnightItem::ProcessRotate( ioBaseChar *pOwner )
{
	if( !pOwner ) return;
	if( m_dwChargeRotateTime <= 0 ) return;
	if( m_bTargetRot ) return;

	if( m_CurDirKey == ioUserKeyInput::DKI_NONE )
		return;

	float fFrameGap = g_FrameTimer.GetCurFrameGap();
	float fRate = FLOAT1;
	fRate = fFrameGap / m_dwChargeRotateTime;

	float fRotateAngle = 360.0f * fRate;

	// 목표에 도달체크 및 처리
	float fYawD = 180.0f + 45.0f * ( m_CurDirKey - ioUserKeyInput::DKI_UP );
	fYawD = ioMath::ArrangeHead( fYawD );

	float fYawR = DEGtoRAD( fYawD );
	D3DXQUATERNION qtRotate;
	D3DXQuaternionRotationYawPitchRoll( &qtRotate, fYawR, 0.0f, 0.0f );

	D3DXQUATERNION qtCurRot = pOwner->GetTargetRot();

	float fCurYaw, fTargetYaw;
	fCurYaw = RADtoDEG( ioMath::QuaterToYaw( qtCurRot ) );
	fTargetYaw = RADtoDEG( ioMath::QuaterToYaw( qtRotate ) );

	fCurYaw = ioMath::ArrangeHead( fCurYaw );
	fTargetYaw = ioMath::ArrangeHead( fTargetYaw );

	float fGapRotAngle = ioMath::AngleGap( fCurYaw, fTargetYaw );
	if( fabs( fGapRotAngle ) < fRotateAngle )
	{
		m_bTargetRot = true;
		pOwner->SetTargetRotToRotate( qtRotate, true, false );
		return;
	}

	D3DXVECTOR3 vDir = pOwner->GetMoveDir();
	D3DXVECTOR3 vTargetDir = qtRotate * D3DXVECTOR3( 0.0f, 0.0f, -FLOAT1 );
	D3DXVec3Normalize( &vTargetDir, &vTargetDir );

	D3DXVECTOR3 vAxis;
	D3DXVec3Cross( &vAxis, &vDir, &vTargetDir );
	D3DXVec3Normalize( &vAxis, &vAxis );

	D3DXQUATERNION qtAngle;

	float fDotValue = D3DXVec3Dot( &vDir, &vTargetDir );
	if( fDotValue < -0.98f )
		D3DXQuaternionRotationAxis( &qtAngle, &ioMath::UNIT_Y, DEGtoRAD(fRotateAngle) );
	else
		D3DXQuaternionRotationAxis( &qtAngle, &vAxis, DEGtoRAD(fRotateAngle) );

	vDir = qtAngle * vDir;
	ioMath::CalcDirectionQuaternion( qtAngle, -vDir );

	pOwner->SetTargetRotToRotate( qtAngle, true, false );
}

void	ioMidnightItem::ProcessMidnightState( ioBaseChar *pOwner )
{
	if( !pOwner )
		return;

	DWORD dwCurTime = FRAMEGETTIME();

	switch( m_ChainAttackState )
	{
	case CAS_START:
		{
			if( pOwner->IsNeedProcess() && (dwCurTime > m_dwMotionEndTime) )
			{
				SetChangeEndState( pOwner );
			}
		}
		break;

	case CAS_ATTACK_HITED:
		{
			if( pOwner->IsNeedProcess() && pOwner->IsAttackKey() )
				ChangeAttackAdd(pOwner);
		}
		break;
	case CAS_ATTACK_ADD:
		{
			if( pOwner->IsNeedProcess() && dwCurTime > m_dwMotionEndTime )
			{
				SetChangeEndState( pOwner );
			}
		}
		break;

	case CAS_END:
		break;
	}


	if( m_ChainAttackState == CAS_ATTACK_HITED || m_ChainAttackState == CAS_ATTACK_ADD )
		CheckAttackHited( pOwner );

	if( m_ChainAttackState == CAS_ATTACK_HITED )
		ProcessWeaponLine( pOwner );

	switch( m_MidnightState )
	{
	case MS_CANCEL_DASH:
		{
			ProcessCancelDashState( pOwner );
		}
		break;

	case MS_CANCEL_DASH_ADD_ATTACK:
		{
			ProcessCancelDashAttackState( pOwner );
		}
		break;

	case MS_JUMP_DASH:
		{
			ProcessJumpDash( pOwner );
		}
		break;
	}
}

bool	ioMidnightItem::IsCollisionSkipState()
{
	if( m_MidnightState == MS_CANCEL_DASH )
		return	true;

	return	false;
}

bool	ioMidnightItem::IsMidnightLockState( ioBaseChar *pOwner )
{
	if( !pOwner )
		return false;

	return pOwner->HasBuff( BT_HIT_COUNT_HOLD );
}

void	ioMidnightItem::ChangeReserveKeyInputState( ioBaseChar *pOwner, int iCurCombo /* = 0 */ )
{
	// 필살기 예약되었으면 필살기 발동
	if( m_iSkillInput >= 0 && !pOwner->IsBuffLimitSkill() && pOwner->SetUseSkill( m_iSkillInput, SUT_NORMAL ) )
	{
		m_iSkillInput = -1;
		return;
	}

	// 방어
	if( m_bEnableDefenseState && pOwner->IsEnableDefenseByObject() && !pOwner->IsBuffLimitDefense() )
	{
		pOwner->SetDefenseState();
		return;
	}

	// 점프
	if( m_bEnableJumpState && pOwner->IsCanJumpState() && !pOwner->IsBuffLimitJump() )
	{
		pOwner->SetJumpState();
		return;
	}

	// 공격
	if( m_bEnableAttackState )
	{
		ioObjectItem *pItem = pOwner->GetObject();

		if( ToDrinkItem( pItem ) )
		{
			pOwner->SetDrinkState();
		}
		else
		{
			if( pOwner->IsCanNormalAttack() && !pOwner->IsBuffLimitAttack() )
			{
				if ( !COMPARE( iCurCombo, 0, GetMaxCombo() ) )
					iCurCombo = 0;
				pOwner->SetNormalAttack( iCurCombo );
			}
		}
		return;
	}

	// 대기
	pOwner->SetState( CS_DELAY );
	m_MidnightState	= MS_NONE;
}

void	ioMidnightItem::ProcessCancelDashState( ioBaseChar* pOwner )
{
	if( !pOwner || !pOwner->IsNeedProcess() )
		return;

	DWORD dwCurTime = FRAMEGETTIME();
	if ( m_dwCancelDashMotionEndTime < dwCurTime )
	{
		float fHeightGap = pOwner->GetWorldPosition().y - pOwner->GetBottomHeight();
		if ( fHeightGap >= FLOAT1 )
			pOwner->SetExtendAttackEndJump( m_fCancelDashEndJumpAmt, m_fCancelDashRandRate, false, true, false, true );
		else
			ChangeReserveKeyInputState( pOwner );
		return;
	}

	if( pOwner->IsNeedProcess() && pOwner->IsAttackKey() && m_dwCancelDashAttackTime > 0 && m_dwCancelDashAttackTime < dwCurTime )
		SetAttackAfterSpecialDash( pOwner, pOwner->CheckCurDirKey() );

	if ( m_dwKeyReserveTime > 0 && m_dwKeyReserveTime < dwCurTime )
		CheckReserve( pOwner );
}

void	ioMidnightItem::ProcessCancelDashAttackState( ioBaseChar* pOwner )
{
	if( !pOwner || !pOwner->IsNeedProcess() )
		return;

	DWORD dwCurTime = FRAMEGETTIME();
	if( m_dwCancelDashAttackMotionEndTime < dwCurTime )
	{
		//pOwner->SetState( CS_DELAY );
		//m_MidnightState	= MS_NONE;
		float fHeightGap = pOwner->GetWorldPosition().y - pOwner->GetBottomHeight();
		if ( fHeightGap >= FLOAT1 )
			pOwner->SetExtendAttackEndJump( m_fCancelDashEndJumpAmt, m_fCancelDashRandRate, false, true, false, true );
		else
			ChangeReserveKeyInputState( pOwner );
		return;
	}
}

void	ioMidnightItem::ProcessJumpDash( ioBaseChar* pOwner )
{
	if( !pOwner )
		return;

	DWORD dwCurTime = FRAMEGETTIME();
	if( m_dwJumpDashMotionEndTime < dwCurTime )
	{
		pOwner->SetState( CS_DELAY );
		m_MidnightState	= MS_NONE;
	}

	if( !pOwner->IsNeedProcess() )
		return;

	if( pOwner->IsDirKeyDoubleClick() )
		SetChangeConditionalSpecialDash( pOwner, pOwner->CheckCurDirKey(), ioAttackableItem::CCSD_JUMP );
}

void	ioMidnightItem::ClearJumpCmdInfo()
{
	m_iCmdInputType = CIT_ALL;
	m_PreDirKey = ioUserKeyInput::DKI_NONE;

	m_iCurCmdIndex = 0;
	memset( m_CurCmdInfo, 0, MAX_COMMAND_CNT );

	m_bCmdInputD = false;
	m_bCmdInputS = false;

	m_dwCmdAttackEndTime = 0;
	m_dwInputStartTime = 0;
	m_dwInputCheckTime = 0;

	m_dwInputSkillStartTime = 0;

	m_iCurCmdCombo = 0;
	m_CurChargeJumpCmdInfo.Init();
}

bool	ioMidnightItem::CheckJumpCmdInputState( ioBaseChar* pOwner )
{
	if( !pOwner ) return false;
	if( !pOwner->IsNeedProcess() ) return false;

	if( m_dwInputStartTime == 0 || m_dwInputCheckTime == 0 )
		return false;

#ifdef BALANCE_RENEWAL
	//////////////////////////////////////////////////////////////////////////
	if( GetCurBullet() < GetNeedBullet() )
		return false;
	//////////////////////////////////////////////////////////////////////////
#endif

	if( CheckCmdInputTime() )
	{
		return ChangeToJumpCmdSet( pOwner );
	}

	CheckJumpCmdInput( pOwner );

	return false;
}

bool	ioMidnightItem::ChangeToJumpCmdSet( ioBaseChar* pOwner )
{
	if( !pOwner )
		return false;

	DWORD dwCurTime = FRAMEGETTIME();
	ClearCancelInfo();

	if( m_ChargeJumpCmdInfoList.empty() )
	{
		m_iCmdInputType = CIT_ALL;
		m_PreDirKey = ioUserKeyInput::DKI_NONE;

		m_iCurCmdIndex = 0;
		memset( m_CurCmdInfo, 0, MAX_COMMAND_CNT );

		m_bCmdInputD = false;
		m_bCmdInputS = false;

		m_dwInputStartTime = 0;
		m_dwInputCheckTime = 0;

		m_iCurCmdCombo = 0;
		m_CurChargeJumpCmdInfo.Init();

		return false;
	}


	if( m_CurChargeJumpCmdInfo.m_Cmd.IsEmpty() )
		return ChangeToNewJumpCmdSet( pOwner );

	return ChangeToComboJumpCmdSet( pOwner );
}

bool	ioMidnightItem::ChangeToNewJumpCmdSet( ioBaseChar* pOwner )
{
	if( !pOwner )
		return false;

	DWORD dwCurTime = FRAMEGETTIME();
	ClearCancelInfo();

	ChargeCmdInfoList::iterator iter = m_ChargeJumpCmdInfoList.begin();
	while( iter != m_ChargeJumpCmdInfoList.end() )
	{
		ChargeCmdInfo &rkInfo = (*iter);

		if( strcmp(rkInfo.m_Cmd.c_str(), m_CurCmdInfo) == 0 )
		{
			if( rkInfo.m_CmdUseType == CUT_SKILL )
			{
				if( !pOwner->IsBuffLimitSkill() && pOwner->SetUseSkill(rkInfo.m_iSkillSlot, SUT_NORMAL))
				{
					ClearState();
					m_ChargeState = CS_NONE;

					if( pOwner->IsNeedSendNetwork() )
					{
						SP2Packet kPacket( CUPK_ITEM_GAUGE_OK );
						FillItemGaugePacketHeader( pOwner, kPacket );
						kPacket << (int)m_ChargeState;
						kPacket << m_iCurCombo;
						P2PNetwork::SendToAllPlayingUser( kPacket );
					}

					return true;
				}

				m_iCmdInputType = CIT_ALL;
				m_PreDirKey = ioUserKeyInput::DKI_NONE;

				m_iCurCmdIndex = 0;
				memset( m_CurCmdInfo, 0, MAX_COMMAND_CNT );

				m_bCmdInputD = false;
				m_bCmdInputS = false;

				m_dwInputStartTime = 0;
				m_dwInputCheckTime = 0;

				return false;
			}
			else
			{
				ChargeCmdSubInfoList &rkSubInfoList = rkInfo.m_SubInfoList;

				if( rkSubInfoList.empty() )
				{
					m_iCmdInputType = CIT_ALL;
					m_PreDirKey = ioUserKeyInput::DKI_NONE;

					m_iCurCmdIndex = 0;
					memset( m_CurCmdInfo, 0, MAX_COMMAND_CNT );

					m_bCmdInputD = false;
					m_bCmdInputS = false;

					m_dwInputStartTime = 0;
					m_dwInputCheckTime = 0;
					return false;
				}

				m_iCurCmdCombo = 0;
				m_CurChargeJumpCmdInfo = rkInfo;

				//pOwner->SetState( CS_MIDNIGHT_SPECIAL );
				//pOwner->SetState( CS_NEW_JUMP_ATTACK );
				m_MidnightState	= MS_JUMP_COMMAND;
				SetJumpCmdAttack( pOwner, rkSubInfoList[m_iCurCmdCombo].m_CmdAttack );

				if( pOwner->IsNeedSendNetwork() )
				{
					SP2Packet kPacket( CUPK_ITEM_GAUGE_OK );
					FillItemGaugePacketHeader( pOwner, kPacket );
					kPacket << ST_JUMP_CMD_STATE;
					kPacket << m_iCurCombo;
					kPacket << rkInfo.m_Cmd;
					kPacket << m_iCurCmdCombo;
					P2PNetwork::SendToAllPlayingUser( kPacket );
				}

				// send after...
				m_iCmdInputType = CIT_ALL;
				m_PreDirKey = ioUserKeyInput::DKI_NONE;

				m_iCurCmdIndex = 0;
				memset( m_CurCmdInfo, 0, MAX_COMMAND_CNT );

				m_bCmdInputD = false;
				m_bCmdInputS = false;

				m_dwInputStartTime = 0;
				m_dwInputCheckTime = 0;

				return true;
			}
		}

		++iter;
	}

	//
	m_iCmdInputType = CIT_ALL;
	m_PreDirKey = ioUserKeyInput::DKI_NONE;

	m_iCurCmdIndex = 0;
	memset( m_CurCmdInfo, 0, MAX_COMMAND_CNT );

	m_bCmdInputD = false;
	m_bCmdInputS = false;

	m_dwInputStartTime = 0;
	m_dwInputCheckTime = 0;

	m_iCurCmdCombo = 0;
	m_CurChargeJumpCmdInfo.Init();

	return false;
}

bool	ioMidnightItem::ChangeToComboJumpCmdSet( ioBaseChar* pOwner )
{
	if( !pOwner )
		return false;

	DWORD dwCurTime = FRAMEGETTIME();
	ClearCancelInfo();

	if( m_CurChargeJumpCmdInfo.m_Cmd.IsEmpty() )
	{
		return ChangeToNewJumpCmdSet( pOwner );
	}

	if( !m_bCmdInputD && !m_bCmdInputS && strcmp(m_CurChargeJumpCmdInfo.m_Cmd.c_str(), m_CurCmdInfo) != 0 )
	{
		return ChangeToNewJumpCmdSet( pOwner );
	}

	ChargeCmdSubInfoList &rkSubInfoList = m_CurChargeJumpCmdInfo.m_SubInfoList;
	if( !COMPARE( m_iCurCmdCombo, 0, (int)rkSubInfoList.size() ) )
	{
		return ChangeToNewJumpCmdSet( pOwner );
	}

	if( rkSubInfoList[m_iCurCmdCombo].m_bCmdAttack_D && m_bCmdInputD )
	{
		//pOwner->SetState( CS_MIDNIGHT_SPECIAL );
		//pOwner->SetState( CS_NEW_JUMP_ATTACK );
		m_MidnightState	= MS_JUMP_COMMAND;
		SetJumpCmdAttack( pOwner, rkSubInfoList[m_iCurCmdCombo].m_CmdAttack_D );

		if( pOwner->IsNeedSendNetwork() )
		{
			SP2Packet kPacket( CUPK_ITEM_GAUGE_OK );
			FillItemGaugePacketHeader( pOwner, kPacket );
			kPacket << ST_CMD_STATE_D;
			kPacket << m_iCurCombo;
			kPacket << m_CurChargeJumpCmdInfo.m_Cmd;
			kPacket << m_iCurCmdCombo;
			P2PNetwork::SendToAllPlayingUser( kPacket );
		}

		// send after...
		m_iCmdInputType = CIT_ALL;
		m_PreDirKey = ioUserKeyInput::DKI_NONE;

		m_iCurCmdIndex = 0;
		memset( m_CurCmdInfo, 0, MAX_COMMAND_CNT );

		m_bCmdInputD = false;
		m_bCmdInputS = false;

		m_dwInputStartTime = 0;
		m_dwInputCheckTime = 0;

		m_iCurCmdCombo = 0;
		m_CurChargeJumpCmdInfo.Init();

		return true;
	}
	else if( rkSubInfoList[m_iCurCmdCombo].m_bCmdAttack_S && m_bCmdInputS )
	{
		//pOwner->SetState( CS_MIDNIGHT_SPECIAL );
		//pOwner->SetState( CS_NEW_JUMP_ATTACK );
		m_MidnightState	= MS_JUMP_COMMAND;
		SetJumpCmdAttack( pOwner, rkSubInfoList[m_iCurCmdCombo].m_CmdAttack_S );

		if( pOwner->IsNeedSendNetwork() )
		{
			SP2Packet kPacket( CUPK_ITEM_GAUGE_OK );
			FillItemGaugePacketHeader( pOwner, kPacket );
			kPacket << ST_CMD_STATE_S;
			kPacket << m_iCurCombo;
			kPacket << m_CurChargeJumpCmdInfo.m_Cmd;
			kPacket << m_iCurCmdCombo;
			P2PNetwork::SendToAllPlayingUser( kPacket );
		}

		// send after...
		m_iCmdInputType = CIT_ALL;
		m_PreDirKey = ioUserKeyInput::DKI_NONE;

		m_iCurCmdIndex = 0;
		memset( m_CurCmdInfo, 0, MAX_COMMAND_CNT );

		m_bCmdInputD = false;
		m_bCmdInputS = false;

		m_dwInputStartTime = 0;
		m_dwInputCheckTime = 0;

		m_iCurCmdCombo = 0;
		m_CurChargeJumpCmdInfo.Init();

		return true;
	}
	else
	{
		if( CheckMamaHahaCmd( m_CurChargeJumpCmdInfo.m_CmdUseType ) && !CheckEnableMamaHahaCmd( pOwner, m_CurChargeJumpCmdInfo.m_CmdUseType ) )
		{
			m_iCmdInputType = CIT_ALL;
			m_PreDirKey = ioUserKeyInput::DKI_NONE;

			m_iCurCmdIndex = 0;
			memset( m_CurCmdInfo, 0, MAX_COMMAND_CNT );

			m_bCmdInputD = false;
			m_bCmdInputS = false;

			m_dwInputStartTime = 0;
			m_dwInputCheckTime = 0;

			return false;
		}
	}

	//
	m_iCurCmdCombo++;

	int iMaxCombo = rkSubInfoList.size();
	if( !COMPARE( m_iCurCmdCombo, 0, iMaxCombo ) )
		m_iCurCmdCombo = 0;


	//pOwner->SetState( CS_MIDNIGHT_SPECIAL );
	//pOwner->SetState( CS_NEW_JUMP_ATTACK );
	m_MidnightState	= MS_JUMP_COMMAND;
	SetJumpCmdAttack( pOwner, rkSubInfoList[m_iCurCmdCombo].m_CmdAttack );

	if( pOwner->IsNeedSendNetwork() )
	{
		SP2Packet kPacket( CUPK_ITEM_GAUGE_OK );
		FillItemGaugePacketHeader( pOwner, kPacket );
		kPacket << ST_JUMP_CMD_STATE;
		kPacket << m_iCurCombo;
		kPacket << m_CurChargeJumpCmdInfo.m_Cmd;
		kPacket << m_iCurCmdCombo;
		P2PNetwork::SendToAllPlayingUser( kPacket );
	}

	// send after...
	m_iCmdInputType = CIT_ALL;
	m_PreDirKey = ioUserKeyInput::DKI_NONE;

	m_iCurCmdIndex = 0;
	memset( m_CurCmdInfo, 0, MAX_COMMAND_CNT );

	m_bCmdInputD = false;
	m_bCmdInputS = false;

	m_dwInputStartTime = 0;
	m_dwInputCheckTime = 0;

	return true;
}

void	ioMidnightItem::SetJumpCmdAttack( ioBaseChar* pOwner, const AttackAttribute &rkAttr, float fExtraAniRate/* =1.0f */, float fExtraForceRate/* =1.0f */ )
{
	if( !pOwner ) 
		return;

	m_fCurExtraGauge	= m_fMaxExtraGauge;
	m_ObjectEquipState	= OES_JUMP;
	ChangeEquipObjectState( pOwner, GetCommandWeapon( true ), EOS_NORMAL );
	DecreaseBullet();
}

bool	ioMidnightItem::CheckJumpCmdInput( ioBaseChar* pOwner )
{
	if( !pOwner )
		return false;

	if( m_iCmdInputType == CIT_ALL || m_iCmdInputType == CIT_ACTION )
	{
		if( m_bCmdInputD || m_bCmdInputS )
			return false;

		int iMaxCnt = m_CurChargeJumpCmdInfo.m_SubInfoList.size();
		if( COMPARE( m_iCurCmdCombo, 0, iMaxCnt ) )
		{
			bool bEnableD = m_CurChargeJumpCmdInfo.m_SubInfoList[m_iCurCmdCombo].m_bCmdAttack_D;
			bool bEnableS = m_CurChargeJumpCmdInfo.m_SubInfoList[m_iCurCmdCombo].m_bCmdAttack_S;

			if( bEnableD && !m_bCmdInputD && pOwner->IsAttackKey() )
			{
				m_bCmdInputD = true;
				return true;
			}
			else if( bEnableS && !m_bCmdInputS && pOwner->IsDefenseKey() )
			{
				m_bCmdInputS = true;
				return true;
			}
		}
	}

	if( m_iCmdInputType == CIT_ALL || m_iCmdInputType == CIT_DIR )
	{
		int iCurMaxCmd = MAX_COMMAND_CNT - 1;
		iCurMaxCmd = max( 0, iCurMaxCmd );
		DWORD dwCurTime = FRAMEGETTIME();

		ioUserKeyInput::DirKeyInput eDirKey = pOwner->GetFourDirKey();
		if( m_PreDirKey == eDirKey )
			return true;

		char szChar = '\0';

		switch( eDirKey )
		{
		case ioUserKeyInput::DKI_NONE:
			m_PreDirKey = eDirKey;
			return true;
		case ioUserKeyInput::DKI_RIGHTUP:
		case ioUserKeyInput::DKI_RIGHTDOWN:
		case ioUserKeyInput::DKI_LEFTDOWN:
		case ioUserKeyInput::DKI_LEFTUP:
			return true;
		case ioUserKeyInput::DKI_UP:
			szChar = '1';
			break;
		case ioUserKeyInput::DKI_RIGHT:
			szChar = '2';
			break;
		case ioUserKeyInput::DKI_DOWN:
			szChar = '3';
			break;
		case ioUserKeyInput::DKI_LEFT:
			szChar = '4';
			break;
		}

		if( !COMPARE( m_iCurCmdIndex, 0, iCurMaxCmd ) )
			return false;

		m_CurCmdInfo[m_iCurCmdIndex] = szChar;

		m_PreDirKey = eDirKey;
		m_iCurCmdIndex++;
		m_dwInputCheckTime = dwCurTime+m_dwInputGapTime;

		return true;
	}

	return false;
}

void	ioMidnightItem::SetCmdAttack( ioBaseChar *pOwner, const AttackAttribute &rkAttr, float fExtraAniRate, float fExtraForceRate )
{
	if( !pOwner )
		return;

	m_fCurExtraGauge	= m_fMaxExtraGauge;
	m_ObjectEquipState	= OES_GROUND;
	ChangeEquipObjectState( pOwner, GetCommandWeapon( false ), EOS_NORMAL );
	DecreaseBullet();
}

bool	ioMidnightItem::IsEquipScytheObject( ioBaseChar *pOwner )
{
	if( pOwner )
	{
		ioObjectItem *pObjectItem = pOwner->GetObject();
		if( pObjectItem && pObjectItem->GetItemCode() == m_dwScytheObjectItem )
		{
			return true;
		}
	}

	return false;
}

bool	ioMidnightItem::IsEquipGunObject( ioBaseChar *pOwner )
{
	if( pOwner )
	{
		ioObjectItem *pObjectItem = pOwner->GetObject();
		if( pObjectItem && pObjectItem->GetItemCode() == m_dwGunObjectItem )
		{
			return true;
		}
	}

	return false;
}

void	ioMidnightItem::ResetObjectEquipState( ioBaseChar* pOwner, int iState )
{
	m_ObjectEquipState	= OES_NONE;
	m_fCurExtraGauge	= 0.0f;

	//if( pOwner )
		//pOwner->SetState( iState );
}

void	ioMidnightItem::OnReleaseObject( ioBaseChar* pOwner )
{
	if( !pOwner )
		return;

	m_ChargeState	= CS_NONE;
	pOwner->AttachEffect( m_szRleaseObjectEffect );

	ioObjectItem*	pObjectItem	= pOwner->GetObject();
	if( pObjectItem && ( pObjectItem->GetItemCode() == m_dwScytheObjectItem || pObjectItem->GetItemCode() == m_dwGunObjectItem ) )
	{
		//애니메이션 초기화
		if( pOwner->GetState() == CS_DELAY )
		{
			pOwner->ReleaseObjectItemImmediately( __FUNCTION__, CS_DELAY, false );
		}
		else if( pOwner->GetState() == CS_RUN )
		{
			pOwner->ReleaseObjectItemImmediately( __FUNCTION__, CS_RUN, false );
		}
		else if( pOwner->GetState() == CS_DASH )
		{
			pOwner->ReleaseObjectItemImmediately( __FUNCTION__, CS_DASH, false );
		}
		else
		{
			pOwner->ReleaseObjectItemImmediately( __FUNCTION__, -1, false );
		}
	}

	ResetObjectEquipState( pOwner, CS_DELAY );

	ioIljimaeJump*	pIljimaeJump	= ToIljimaeJump( m_pExtendJump );
	if( pIljimaeJump )
		pIljimaeJump->ResetJumpState();
}

DWORD	ioMidnightItem::GetCommandWeapon( bool bJump )
{
	CommandWeaponInfoList	commandWeapon;
	ChargeCmdInfo			curChargeCmdInfo;
	if( !bJump )
	{
		commandWeapon		= m_CommandWeapon;
		curChargeCmdInfo	= m_CurChargeCmdInfo;
	}
	else
	{
		commandWeapon		= m_JumpCommandWeapon;
		curChargeCmdInfo	= m_CurChargeJumpCmdInfo;
	}

	CommandWeaponInfoList::iterator iter = commandWeapon.begin();
	while( iter != commandWeapon.end() )
	{
		CommandWeaponInfo	commandWeaponInfo	= *iter;

		if( curChargeCmdInfo.m_Cmd == commandWeaponInfo.m_Cmd )
		{
			switch( commandWeaponInfo.m_CommandWeapon )
			{
			case CW_SCYTHE:
				{
					return	m_dwScytheObjectItem;
				}
				break;

			case CW_GUN:
				{
					return	m_dwGunObjectItem;
				}
				break;
			}
		}

		++iter;
	}

	return	0;
}

void	ioMidnightItem::ApplyCmdAttack( ioBaseChar* pOwner, ObjectEquipState objectEquipState )
{
	if( !pOwner )
		return;

	m_fCurExtraGauge	= m_fMaxExtraGauge;
	m_ObjectEquipState	= objectEquipState;
	ChangeEquipObjectState( pOwner, GetCommandWeapon( false ), EOS_NORMAL );
	DecreaseBullet();
}

//-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

ioMidnightSpecialState::ioMidnightSpecialState()
{
}

ioMidnightSpecialState::~ioMidnightSpecialState()
{
}

void	ioMidnightSpecialState::Initialize( ioBaseChar* pOwner, CharState eCurrState )
{
	ioSpecialStateBase::Initialize( pOwner, eCurrState );
}

void	ioMidnightSpecialState::Finalize( ioBaseChar* pOwner, CharState eCurrState, CharState eNewState )
{
	ioSpecialStateBase::Finalize( pOwner, eCurrState, eNewState );

	ioMidnightItem *pMidnight = ToMidnightItem( pOwner->GetPriorityItem( SPT_ATTACK ) );
	if( pMidnight )
		pMidnight->ClearAttackState( pOwner );
}
bool	ioMidnightSpecialState::IsCharCollisionSkipState( const ioBaseChar *const pOwner, DWORD dwStartTime, DWORD dwEndTime, DWORD dwSkipType, bool bTeamOnly, bool bDefense, TeamType eOwnerTeam, TeamType eOtherTeam ) const
{
	ioMidnightItem*	pMidnight	= ToMidnightItem( pOwner->GetWeapon() );
	if( pMidnight )
		return	pMidnight->IsCollisionSkipState();

	if( Help::CheckCharColState( dwStartTime, dwEndTime, dwSkipType, bTeamOnly, bDefense, eOwnerTeam, eOtherTeam ) )
		return true;

	return false;
}

void	ioMidnightSpecialState::CheckSpecialState( ioBaseChar* pOwner )
{
	//ioMidnightItem*	pMidnight	= ToMidnightItem( pOwner->GetPriorityItem( SPT_ATTACK ) );
	ioMidnightItem*	pMidnight	= ToMidnightItem( pOwner->GetWeapon() );
	if( pMidnight )
		pMidnight->ProcessMidnightState( pOwner );
	else
	{
		//if( pOwner->GetAnimationEndTime() < FRAMEGETTIME() )
		{
			pOwner->SetState( CS_DELAY );
			pOwner->SetFallState( true );
			return;
		}
	}
}

void	ioMidnightSpecialState::CheckFallStateChange( ioBaseChar* pOwner, float fMinFallHeightRate, float fBottomHeight, bool bBottomMaxCheck, float fMaxBottomHeight )
{
	return;
}

bool	ioMidnightSpecialState::ProcessMove( ioBaseChar* pOwner, float fTimePerSec, DWORD dwPreTime, DWORD dwCurTime )
{
	ioMidnightItem*	pMidnight	= ToMidnightItem( pOwner->GetWeapon() );
	if( !pMidnight )
		return	true;

	switch( pMidnight->GetMidnightState() )
	{
	case ioMidnightItem::MS_JUMP_COMMAND:
	case ioMidnightItem::MS_RELEASE_OBJECT:
	case ioMidnightItem::MS_CLAW_ATTACK:
		return	true;
	}

	return	false;
}

void	ioMidnightSpecialState::GetGravity( ioBaseChar* pOwner, float &fCurGravity )
{
	ioMidnightItem *pMidnight = ToMidnightItem( pOwner->GetPriorityItem( SPT_ATTACK ) );
	if( pMidnight )
	{
		if( pMidnight->IsJumpDashState() )
			fCurGravity	= pMidnight->GetJumpDashGravityAmt();
		else
			fCurGravity = 0.0f;
	}
}