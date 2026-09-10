

#include "stdafx.h"

#include "ioIljimaeJump.h"
#include "ItemDefine.h"
#include "FindPredicateImpl.h"

ioIljimaeJump::ioIljimaeJump()
{
	m_bUsedDoubleJump = false;
	m_iCurJumpCnt = 1;

	m_iCurComboCnt = 0;
	m_dwEnableComboTime_S = 0;
	m_dwEnableComboTime_E = 0;
	m_dwNightmareKeyTime = 0;

	m_bReserveJumpAttack = false;

	m_ComboState = CST_NONE;
	m_JumpState  = PJS_NONE;

	m_dwComboDashStartTime = 0;
	m_dwComboDashEndTime = 0;

	m_bUsedComboDash = false;

	m_dwChargingStartTime = 0;

	m_dwMotionEndTime = 0;
	
	m_bUsedTeleport = false;
	m_bUsedAzazelAttack = false;
	m_bUsedChargeAttack = false;
	m_dwChargeAttackEndTime = 0;

	m_dwJumpDashAddAttackReserveTime = 0;
	m_iJumpDashAddAttackIndex = 0;

	m_bUsedDefense = false;
	m_dwDefenseEnableTime = 0;
	m_dwDefenseReleaseTime = 0;
	m_iCurDefenseEnableCount = 0;
}

ioIljimaeJump::ioIljimaeJump( const ioIljimaeJump &rhs )
: ioExtendJump( rhs ),
m_ComboAttackList( rhs.m_ComboAttackList ),
m_fDoubleJumpSpeedRateFR( rhs.m_fDoubleJumpSpeedRateFR ),
m_fDoubleJumpSpeedRateSI( rhs.m_fDoubleJumpSpeedRateSI ),
m_fDoubleJumpSpeedRateBG( rhs.m_fDoubleJumpSpeedRateBG ),
m_fExtendJumpSpeedRateFR( rhs.m_fExtendJumpSpeedRateFR ),
m_fExtendJumpSpeedRateSI( rhs.m_fExtendJumpSpeedRateSI ),
m_fExtendJumpSpeedRateBG( rhs.m_fExtendJumpSpeedRateBG ),
m_fExtraJumpPowerRate( rhs.m_fExtraJumpPowerRate ),
m_dwJumpChargeTime( rhs.m_dwJumpChargeTime ),
m_bEnableJumpDash( rhs.m_bEnableJumpDash ),
m_DashJumpAttack( rhs.m_DashJumpAttack ),
m_ComboDashLandAni( rhs.m_ComboDashLandAni ),
m_fComboDashLandAniRate( rhs.m_fComboDashLandAniRate ),
m_bComboDashLandEnable( rhs.m_bComboDashLandEnable ),
m_fComboDashGravityAmt( rhs.m_fComboDashGravityAmt ),
m_ComboDashType( rhs.m_ComboDashType ),
m_dwComboDashEndJumpPower( rhs.m_dwComboDashEndJumpPower ),
m_bUseNewJumpTypeDash( rhs.m_bUseNewJumpTypeDash ),
m_iMaxJumpCnt( rhs.m_iMaxJumpCnt ),
m_fDoubleJumpPower( rhs.m_fDoubleJumpPower ),
m_fDoubleJumpForce( rhs.m_fDoubleJumpForce ),
m_fDoubleJumpEnableHeight( rhs.m_fDoubleJumpEnableHeight ),
m_dwDoubleJumpEnableTime( rhs.m_dwDoubleJumpEnableTime ),
m_vDoubleJumpAniList( rhs.m_vDoubleJumpAniList ),
m_fDoubleJumpAniRate( rhs.m_fDoubleJumpAniRate ),
m_TeleportMotion( rhs.m_TeleportMotion ),
m_fTeleportMotionRate( rhs.m_fTeleportMotionRate ),
m_TeleportEndMotion( rhs.m_TeleportEndMotion ),
m_fTeleportEndMotionRate( rhs.m_fTeleportEndMotionRate ),
m_fTeleportRange( rhs.m_fTeleportRange ),
m_fTeleportAngle( rhs.m_fTeleportAngle ),
m_TargetWoundType( rhs.m_TargetWoundType ),
m_fMoveRange( rhs.m_fMoveRange ),
m_fMoveHeightRange( rhs.m_fMoveHeightRange ),
m_fChargeJumpPower( rhs.m_fChargeJumpPower ),
m_fChargeGravityRate( rhs.m_fChargeGravityRate ),
m_AzazelExtendAttack( rhs.m_AzazelExtendAttack ),
m_ExtendLandAttack( rhs.m_ExtendLandAttack ),
m_vJumpDashAddAttack( rhs.m_vJumpDashAddAttack ),
m_bEnableDefense( rhs.m_bEnableDefense ),
m_fDefenseEnableHeight( rhs.m_fDefenseEnableHeight ),
m_JumpDefenseAnimation( rhs.m_JumpDefenseAnimation ),
m_JumpAniDefenseWounded( rhs.m_JumpAniDefenseWounded),
m_JumpAniDefenseCriticalWounded( rhs.m_JumpAniDefenseCriticalWounded ),
m_fDefenseAniRate( rhs.m_fDefenseAniRate ),
m_dwDefenseDuration( rhs.m_dwDefenseDuration ),
m_DefenseLandAni( rhs.m_DefenseLandAni ),
m_fDefenseLandAniRate( rhs.m_fDefenseLandAniRate ),
m_fClearJumpPowerStartDefence( rhs.m_fClearJumpPowerStartDefence ),
m_iMaxDefenseEnableCount( rhs.m_iMaxDefenseEnableCount ),
m_fAzazelRange( rhs.m_fAzazelRange ),
m_fAzazelAngle( rhs.m_fAzazelAngle ),
m_fAzazelEndJumpAmt( rhs.m_fAzazelEndJumpAmt ),
m_vAzazelAttackOffset( rhs.m_vAzazelAttackOffset ),
m_bJumpChargeEnable( rhs.m_bJumpChargeEnable ),
m_dwJumpKeyChargingTime( rhs.m_dwJumpKeyChargingTime ),
m_vLupinBirdMoveNormalInfo( rhs.m_vLupinBirdMoveNormalInfo ),
m_LupinBirdMoveInfo( rhs.m_LupinBirdMoveInfo ),
m_LupinBird_Attack( rhs.m_LupinBird_Attack )
{
	m_iCurComboCnt = 0;
	m_dwEnableComboTime_S = 0;
	m_dwEnableComboTime_E = 0;
	m_dwNightmareKeyTime = 0;

	m_bReserveJumpAttack = false;

	m_JumpState  = PJS_NONE;
	m_ComboState = CST_NONE;
	m_dwComboDashStartTime = 0;
	m_dwComboDashEndTime = 0;

	m_bUsedComboDash = false;

	m_bUsedDoubleJump = false;
	m_iCurJumpCnt = 1;

	m_dwChargingStartTime = 0;

	m_dwMotionEndTime = 0;

	m_bUsedTeleport = false;
	m_bUsedAzazelAttack = false;
	m_bUsedChargeAttack = false;
	m_dwChargeAttackEndTime = 0;

	m_dwJumpDashAddAttackReserveTime = 0;
	m_iJumpDashAddAttackIndex = 0;

	m_bUsedDefense = false;
	m_dwDefenseEnableTime = 0;
	m_dwDefenseReleaseTime = 0;
	m_iCurDefenseEnableCount = 0;

	m_bUsedNightmare = false;
}

ioExtendJump* ioIljimaeJump::Clone()
{
	return new ioIljimaeJump( *this );
}

ioIljimaeJump::~ioIljimaeJump()
{
}

JumpType ioIljimaeJump::GetType() const
{
	return JT_ILJIMAE_JUMP;
}

void ioIljimaeJump::LoadProperty( ioINILoader &rkLoader )
{
	ioExtendJump::LoadProperty( rkLoader );

	char szBuf[MAX_PATH] = "";
	
	m_fDoubleJumpSpeedRateFR = rkLoader.LoadFloat_e( "double_jump_speed_rate_front", FLOAT1 );
	m_fDoubleJumpSpeedRateSI = rkLoader.LoadFloat_e( "double_jump_speed_rate_side", FLOAT1 );
	m_fDoubleJumpSpeedRateBG = rkLoader.LoadFloat_e( "double_jump_speed_rate_back", FLOAT1 );

	m_fExtendJumpSpeedRateFR = rkLoader.LoadFloat_e( "extend_jump_speed_rate_front", FLOAT1 );
	m_fExtendJumpSpeedRateSI = rkLoader.LoadFloat_e( "extend_jump_speed_rate_side", FLOAT1 );
	m_fExtendJumpSpeedRateBG = rkLoader.LoadFloat_e( "extend_jump_speed_rate_back", FLOAT1 );

	LoadComboList( rkLoader );

	m_fExtraJumpPowerRate = rkLoader.LoadFloat_e( "jump_extra_power_rate", FLOAT1 );
	m_dwJumpChargeTime = rkLoader.LoadInt_e( "jump_charge_time", 0 );

	// Dash JumpAttack
	m_bEnableJumpDash = rkLoader.LoadBool_e( "enable_jump_dash", false );
	m_ComboDashType = (ComboDashType)rkLoader.LoadInt_e( "jump_dash_type", CDT_GRAVITY );
	m_dwComboDashEndJumpPower = rkLoader.LoadFloat( "combo_dash_end_jump_power", 0.0f );

	m_DashJumpAttack.Init();
	LoadAttackAttribute_e( "dash_jump_attack", m_DashJumpAttack, rkLoader );

	rkLoader.LoadString_e( "jump_dash_land_ani", "", szBuf, MAX_PATH );
	m_ComboDashLandAni = szBuf;
	m_fComboDashLandAniRate = rkLoader.LoadFloat_e( "jump_dash_land_ani_rate", FLOAT1 );
	m_bComboDashLandEnable = rkLoader.LoadBool_e( "jump_dash_land_enable", false );

	m_fComboDashGravityAmt = rkLoader.LoadFloat_e( "jump_dash_gravity_amt", 0.0f );

	m_bUseNewJumpTypeDash = rkLoader.LoadBool_e( "use_new_jump_type_dash", false );

	// Double
	m_iMaxJumpCnt = rkLoader.LoadInt_e( "max_jump_cnt", 0 );
	m_iMaxJumpCnt = max( 1, m_iMaxJumpCnt );

	m_fDoubleJumpPower = rkLoader.LoadFloat_e( "double_jump_power", 0.0f );
	m_fDoubleJumpForce = rkLoader.LoadFloat_e( "double_jump_force", 0.0f );
	m_dwDoubleJumpEnableTime = rkLoader.LoadInt_e( "double_jump_enable_time", 0 );
	m_fDoubleJumpEnableHeight = rkLoader.LoadFloat_e( "double_jump_enable_height", 0.0f );

	m_fDoubleJumpAniRate = rkLoader.LoadFloat_e( "double_jump_ani_rate", FLOAT1 );

	m_vDoubleJumpAniList.clear();
	m_vDoubleJumpAniList.reserve( 4 );

	rkLoader.LoadString_e( "double_jump_animation_front", "", szBuf, MAX_PATH );
	m_vDoubleJumpAniList.push_back( ioHashString(szBuf) );
	rkLoader.LoadString_e( "double_jump_animation_right", "", szBuf, MAX_PATH );
	m_vDoubleJumpAniList.push_back( ioHashString(szBuf) );
	rkLoader.LoadString_e( "double_jump_animation_back", "", szBuf, MAX_PATH );
	m_vDoubleJumpAniList.push_back( ioHashString(szBuf) );
	rkLoader.LoadString_e( "double_jump_animation_left", "", szBuf, MAX_PATH );
	m_vDoubleJumpAniList.push_back( ioHashString(szBuf) );

	rkLoader.LoadString_e( "jump_teleport_motion", "", szBuf, MAX_PATH );
	m_TeleportMotion = szBuf;
	m_fTeleportMotionRate = rkLoader.LoadFloat_e( "jump_teleport_motion_rate", FLOAT1 );

	rkLoader.LoadString_e( "jump_teleport_end_motion", "", szBuf, MAX_PATH );
	m_TeleportEndMotion = szBuf;
	m_fTeleportEndMotionRate = rkLoader.LoadFloat_e( "jump_teleport_end_motion_rate", FLOAT1 );

	m_TargetWoundType = (TargetWoundType)rkLoader.LoadInt_e( "jump_target_wound_type", TWT_ALL );

	m_fTeleportRange = rkLoader.LoadFloat_e( "jump_teleport_target_range", 0.0f );
	m_fTeleportAngle = rkLoader.LoadFloat_e( "jump_teleport_target_angle", 0.0f );
	m_fMoveRange = rkLoader.LoadFloat_e( "jump_teleport_move_range", 0.0f );
	m_fMoveHeightRange = rkLoader.LoadFloat_e( "jump_teleport_move_height_range", 0.0f );

	m_fChargeJumpPower = rkLoader.LoadFloat_e( "jump_charge_jump_power", 0.0f );
	m_fChargeGravityRate = rkLoader.LoadFloat_e( "jump_charge_gravity_rate", FLOAT1 );

	m_AzazelExtendAttack.Init();
	LoadAttackAttribute_e( "azazel_extend_attack", m_AzazelExtendAttack, rkLoader );

	m_ExtendLandAttack.Init();
	LoadAttackAttribute_e( "jump_charge_land", m_ExtendLandAttack, rkLoader );

	char szKey[MAX_PATH] = "";
	int JumpDashAddAttackCnt = rkLoader.LoadInt_e( "dash_jump_attack_add_attack_cnt", 0 );
	for( int i=0; i<JumpDashAddAttackCnt; ++i )
	{
		AttackAttribute kAttack;
		kAttack.Init();

		wsprintf_e( szKey, "dash_jump_attack_add_attack%d", i+1 );
		LoadAttackAttribute( szKey, kAttack, rkLoader );

		m_vJumpDashAddAttack.push_back( kAttack );
	}

	m_bEnableDefense = rkLoader.LoadBool_e( "enable_defense", false );
	m_fDefenseEnableHeight = rkLoader.LoadFloat_e( "jump_defense_enable_hight", 0.0f );

	rkLoader.LoadString_e( "jump_defense_animation", "", szBuf, MAX_PATH );
	m_JumpDefenseAnimation = szBuf;
	rkLoader.LoadString_e( "jump_ani_defense_wounded", "", szBuf, MAX_PATH );
	m_JumpAniDefenseWounded = szBuf;
	rkLoader.LoadString_e( "jump_ani_defense_criticalwounded", "", szBuf, MAX_PATH );
	m_JumpAniDefenseCriticalWounded = szBuf;

	rkLoader.LoadString_e( "jump_defense_land_animation", "", szBuf, MAX_PATH );
	m_DefenseLandAni = szBuf;
	m_fDefenseLandAniRate = rkLoader.LoadFloat_e( "jump_defense_land_ani_rate", FLOAT1 );

	m_fDefenseAniRate = rkLoader.LoadFloat_e( "jump_defense_ani_rate", FLOAT1 );
	m_dwDefenseDuration = rkLoader.LoadInt_e( "jump_defense_duration", 0 );
	m_iMaxDefenseEnableCount = rkLoader.LoadInt_e( "max_defense_enable_count", 0 );
	m_fClearJumpPowerStartDefence = rkLoader.LoadBool_e( "clear_jump_power_start_defence", false );

	m_fAzazelAngle = rkLoader.LoadFloat_e( "azazel_dash_aim_angle", 45.0f );
	m_fAzazelRange = rkLoader.LoadFloat_e( "azazel_dash_aim_range", FLOAT1000 );
	m_fAzazelEndJumpAmt = rkLoader.LoadFloat_e( "azazel_end_jump_amount", FLOAT100 );

	m_vAzazelAttackOffset.x = rkLoader.LoadFloat_e( "azazel_jump_offset_x", 0.0f );
	m_vAzazelAttackOffset.y = rkLoader.LoadFloat_e( "azazel_jump_offset_y", 0.0f );
	m_vAzazelAttackOffset.z = rkLoader.LoadFloat_e( "azazel_jump_offset_z", 0.0f );

	m_bJumpChargeEnable = rkLoader.LoadBool_e( "jump_charge_enable", false );
	m_dwJumpKeyChargingTime = (DWORD)rkLoader.LoadInt_e( "jump_key_charging_time", 0 );

	LoadLupinBirdInfo( rkLoader );
}

void ioIljimaeJump::LoadLupinBirdInfo( ioINILoader &rkLoader )
{
	char szKey[MAX_PATH];
	char szBuf[MAX_PATH];

	int iCnt = rkLoader.LoadInt_e( "jump_normal_move_cnt", 0 );
	for( int i = 0; i < iCnt; ++i )
	{
		StTopMoveInfo info;
		wsprintf( szKey, "jump_bird_combo_rotate_x%d", i+1 );
		info.fRotate_x = rkLoader.LoadFloat( szKey, 0.f );

		wsprintf( szKey, "jump_bird_combo_rotate_y%d", i+1 );
		info.fRotate_y = rkLoader.LoadFloat( szKey, 0.f );

		wsprintf( szKey, "jump_bird_combo_move_length%d", i+1 );
		info.fLength = rkLoader.LoadFloat( szKey, 0.f );

		wsprintf( szKey, "jump_bird_combo_create%d", i+1 );
		info.bCreate = rkLoader.LoadBool( szKey, false );

		m_vLupinBirdMoveNormalInfo.push_back( info );
	}

	m_LupinBirdMoveInfo.fRotate_x = rkLoader.LoadFloat_e( "jump_bird_normal_rotate_x", 0.f );
	m_LupinBirdMoveInfo.fRotate_y = rkLoader.LoadFloat_e( "jump_bird_normal_rotate_y", 0.f );
	m_LupinBirdMoveInfo.fLength = rkLoader.LoadFloat_e( "jump_bird_normal_length", 0.f );
	m_LupinBirdMoveInfo.bCreate = rkLoader.LoadBool_e( "jump_bird_normal_create", false );

	rkLoader.LoadString_e( "jump_top_normal_return_attack_ani", "", szBuf, MAX_PATH );
	m_LupinBird_Attack.m_AttackAnimation = szBuf;
	m_LupinBird_Attack.m_fAttackAniRate = rkLoader.LoadFloat_e( "jump_top_normal_return_attack_rate", FLOAT1 );
}

void ioIljimaeJump::LoadComboList( ioINILoader &rkLoader )
{
	char szBuf[MAX_PATH];
	int iMaxCombo = rkLoader.LoadInt_e( "jump_max_combo", 0 );

	m_ComboAttackList.clear();
	for( int i=0 ; i<iMaxCombo ; i++ )
	{
		AttackAttribute kAttribute;

		wsprintf_e( szBuf, "jump_attack%d", i+1 );
		LoadAttackAttribute( szBuf, kAttribute, rkLoader );
		m_ComboAttackList.push_back( kAttribute );
	}
}

void ioIljimaeJump::InitJump()
{
	m_bUsedNightmare = false;
}

void ioIljimaeJump::SetJumpping( ioBaseChar *pOwner, bool bFullTime )
{
	m_iCurComboCnt = 0;
	m_dwEnableComboTime_S = 0;
	m_dwEnableComboTime_E = 0;
	m_dwNightmareKeyTime = 0;

	m_bReserveJumpAttack = false;
	m_bUsedChargeAttack = false;
	m_dwChargeAttackEndTime = 0;

	m_ComboState = CST_NONE;

	m_bUsedDoubleJump = false;
	m_iCurJumpCnt = 1;
	m_iJumpDashAddAttackIndex = 0;

	m_dwChargingStartTime = 0;

	m_dwMotionEndTime = 0;

	ioItem *pItem = pOwner->GetEquipedItem(ES_WEAPON);
	ioIljimaeItem *pIljimaeItem = ToIljimaeItem( pItem );
	if( pIljimaeItem )
	{
		m_ReturnMark = pIljimaeItem->GetReturnMarkEffet();
		m_dwReturnMarkDuration = pIljimaeItem->GetReturnMarkDuration();
	}

	m_bUsedDefense = false;
	m_dwDefenseEnableTime = 0;
	m_dwDefenseReleaseTime = 0;
	m_iCurDefenseEnableCount = 0;
}

void ioIljimaeJump::ProcessJumpping( ioBaseChar *pOwner, float fHeightGap, float fNeedGauge )
{
	if ( m_dwChargingStartTime > 0 && CheckJumpCharging( pOwner ) )
		return;

	ioItem *pItem = pOwner->GetEquipedItem(ES_WEAPON);
	iolupinItem *pLupinItem = ToLupinItem( pItem );
	if( pLupinItem && pOwner->IsNeedProcess() && !pLupinItem->IsEnableLupinJumpAction() )
		return;

	if( m_bEnableJumpDash && IsCanComboDash( pOwner, fHeightGap ) )
	{
		SetComboDash( pOwner, fHeightGap );
		return;
	}

	if( CheckDefenseRelease( pOwner ) )
	{
		ReleaseDefense( pOwner );
		return;
	}
	if( IsCanDefense( pOwner, fHeightGap ) && !pOwner->IsBuffLimitDefense() )
	{
		SetDefense( pOwner );
		return;
	}

	if( IsCanJumpCharge( pOwner, fHeightGap ) )
	{
		SetJumpChargingState( pOwner );
	}
	else if( IsCanDoubleJump( pOwner, fHeightGap, false ) && m_fDoubleJumpPower >= 0.0f )
	{
		SetDoubleJump( pOwner );
	}

	if( m_bReserveJumpAttack )
	{
		ChangeToJumpAttack( pOwner );
	}
}

void ioIljimaeJump::ProcessJumpAttack( ioBaseChar *pOwner, float fHeightGap )
{
	if( !pOwner )
		return;

	DWORD dwCurTime = FRAMEGETTIME();

	if( m_ComboState == CST_NEW_JUMP )
	{
		switch( m_JumpState )
		{
		case PJS_DASH_ATTACK:
			if( m_dwJumpDashAddAttackReserveTime > 0 && m_dwJumpDashAddAttackReserveTime < dwCurTime && pOwner->IsAttackKey() && 
				m_iJumpDashAddAttackIndex < (int)m_vJumpDashAddAttack.size() )
			{
				m_iJumpDashAddAttackIndex = 0;
				m_JumpState = PJS_DASH_ADD_ATTACK;
				SetJumpDashAddAttack( pOwner, m_iJumpDashAddAttackIndex );
			}
			break;
		case PJS_DASH_ADD_ATTACK:
			if( m_dwComboDashEndTime > 0 && m_dwComboDashEndTime < dwCurTime )
			{
				m_dwComboDashEndTime = 0;
			}
			else if( m_dwJumpDashAddAttackReserveTime > 0 && m_dwJumpDashAddAttackReserveTime < dwCurTime && pOwner->IsAttackKey() && 
					 m_iJumpDashAddAttackIndex < (int)m_vJumpDashAddAttack.size() )
			{
				m_JumpState = PJS_DASH_ADD_ATTACK;
				SetJumpDashAddAttack( pOwner, m_iJumpDashAddAttackIndex );
			}
			break;
		}

		return;
	}

	switch( m_JumpState )
	{
	case PJS_NONE:
		if( m_bReserveJumpAttack )
		{
			ChangeToJumpAttack( pOwner );
		}
		break;
	case PJS_NORMAL_ATTACK:
		{
			ioItem *pItem = pOwner->GetEquipedItem(ES_WEAPON);
			ioIljimaeItem *pIljimaeItem = ToIljimaeItem( pItem );
			ioSasinmuItem *pSasinmuItem = ToSasinmuItem( pItem );
			ioMephistoItem *pMephistoItem = ToMephistoItem( pItem );
			ioBangtanItem *pBangtanItem = ToBangtanItem( pItem );
			ioNightmareItem *pNightmare = ToNightmareItem( pItem );
			ioAzazelItem *pAzazel = ToAzazelItem( pItem );
			ioSummonChargeItem3 *pSummonChargeItem3 = ToSummonChargeItem3( pItem );
			ioAutomatonItem *pAutomatonItem = ToAutomatonItem( pItem );
			ioBubbleItem *pBubble = ToBubbleItem( pItem );
			ioChangeOnItem* pChangeOn = ToChangeOnItem( pItem );
			ioHanzoItem* pHanzo = ToHanzoItem( pItem );
			iolupinItem *pLupinItem = ToLupinItem( pItem );
			ioMidnightItem* pMidnight = ToMidnightItem( pItem );
			ioTyrItem* pTyrItem = ToTyrItem( pItem );

			int iMaxCnt = m_ComboAttackList.size();
			if( pIljimaeItem && pOwner->IsNeedProcess() && pOwner->IsAttackKeyDown() && COMPARE( m_iCurComboCnt, 0, iMaxCnt ) )
			{
				if( m_dwChargingStartTime + m_dwJumpChargeTime < dwCurTime )
				{
					m_dwChargingStartTime = 0;

					float fCurCoolTime, fUseCoolTime;
					fCurCoolTime = pIljimaeItem->GetCurBullet();
					fUseCoolTime = pIljimaeItem->GetUseBullet();

					if( fCurCoolTime >= fUseCoolTime )
					{
						pIljimaeItem->DecreaseCoolTime( pIljimaeItem->GetUseBullet() );
						ChangeToTeleport( pOwner );

						if( pOwner->IsNeedSendNetwork() )
						{
							SP2Packet kPacket( CUPK_EXTEND_JUMP );
							kPacket << pOwner->GetCharName();
							kPacket << (int)GetType();
							kPacket << pOwner->GetHP().m_fCurValue;
							kPacket << SSC_CHANGE_TELEPORT;
							kPacket << pOwner->GetTargetRot();
							pOwner->FillAutoTargetInfo( kPacket );
							P2PNetwork::SendToAllPlayingUser( kPacket );
						}
					}
				}
			}
			else if( pMephistoItem && pMephistoItem->EnableMephistoFlyState() && pOwner->IsNeedProcess() && pOwner->IsAttackKeyDown() && COMPARE( m_iCurComboCnt, 0, iMaxCnt ) )
			{
				if( m_dwChargingStartTime + m_dwJumpChargeTime < dwCurTime )
				{
					pMephistoItem->SetMephistoFlyStartState( pOwner );
					m_dwChargingStartTime = 0;
				}
			}
			else if( ( pSasinmuItem || pBangtanItem || pSummonChargeItem3 || pBubble ) && 
				pOwner->IsNeedProcess() && pOwner->IsAttackKeyDown() && COMPARE( m_iCurComboCnt, 0, iMaxCnt ) )
			{
				if( m_dwChargingStartTime + m_dwJumpChargeTime < dwCurTime )
					ChangeToPowerChargeJumpAttack( pOwner );
			}
			else if( pChangeOn && pChangeOn->IsEnableGauge() && pOwner->IsNeedProcess() && pOwner->IsAttackKeyDown() && COMPARE( m_iCurComboCnt, 0, iMaxCnt ) )
			{
				if( m_dwChargingStartTime + m_dwJumpChargeTime < dwCurTime )
					SetChangeOnGriffinState( pOwner );
			}
			else if( pChangeOn && pOwner->IsNeedProcess() && pOwner->IsDefenseKey() && m_dwReserveEnableTime < dwCurTime )
			{
				ioObjectWereWolfItem *pObjectWere = ToObjectWereWolfItem( pOwner->GetPriorityItem( SPT_ATTACK ) );
				if( pObjectWere && pObjectWere->IsEnableDefenseRelease() )
				{
					pObjectWere->ChangeToRelease( pOwner );
					return;
				}
			}
			else if( pNightmare && pOwner->IsNeedProcess() && pOwner->IsEnableNightmareFly() && pOwner->IsAttackKeyDown() && COMPARE( m_iCurComboCnt, 0, iMaxCnt ) )
			{
				if( m_dwChargingStartTime + m_dwJumpChargeTime < dwCurTime )
					SetNightmareState( pOwner );
			}
			else if( pAzazel && pOwner->IsNeedProcess() && pOwner->IsEnableAzazelSpecialAttack() && pOwner->IsAttackKeyDown() && COMPARE( m_iCurComboCnt, 0, iMaxCnt ) )
			{
				if( m_dwChargingStartTime + m_dwJumpChargeTime < dwCurTime )
					SetAzazelPushState( pOwner );
			}
			else if( pAzazel && pOwner->IsNeedProcess() && pOwner->IsEnableAzazelSpecialAttack() && pOwner->IsJumpKeyDown() && COMPARE( m_iCurComboCnt, 0, iMaxCnt ) )
			{
				if( m_dwChargingStartTime + m_dwJumpKeyChargingTime < dwCurTime )
					SetAzazelTeleportState( pOwner );
			}
			else if( pAzazel && !m_bUsedAzazelAttack && pOwner->IsNeedProcess() && pOwner->IsEnableAzazelAttack() && pOwner->IsAttackKeyDown() && COMPARE( m_iCurComboCnt, 0, iMaxCnt ) )
			{
				if( m_dwChargingStartTime + m_dwJumpChargeTime < dwCurTime )
					SetAzazelState( pOwner, true );
			}
			else if( pAutomatonItem && pOwner->IsNeedProcess() && pAutomatonItem->IsEnableGauge() && pOwner->IsAttackKeyDown() && COMPARE( m_iCurComboCnt, 0, iMaxCnt ) )
			{
				if( m_dwChargingStartTime + m_dwJumpChargeTime < dwCurTime )
					SetAutomatonState( pOwner );
			}
			else if( pHanzo && pOwner->IsNeedProcess() && pHanzo->IsEnableJumpGathering() && pOwner->IsAttackKeyDown() && COMPARE( m_iCurComboCnt, 0, iMaxCnt ) )
			{
				if( m_dwChargingStartTime + m_dwJumpChargeTime < dwCurTime )
					SetHanzoState( pOwner );
			}
			else if( pMidnight && pOwner->IsNeedProcess() && pOwner->IsAttackKeyDown() && pMidnight->IsJumpDashEnable() && COMPARE( m_iCurComboCnt, 0, iMaxCnt ) )
			{
				if( m_dwChargingStartTime + m_dwJumpChargeTime < dwCurTime )
					SetMidnightState( pOwner );
			}
			else if( pTyrItem && pTyrItem->IsEnoughGaugeJumpCharge() && pOwner->IsNeedProcess() && pOwner->IsAttackKeyDown() && COMPARE( m_iCurComboCnt, 0, iMaxCnt + 1 ) )
			{
				if( m_dwChargingStartTime + m_dwJumpChargeTime < dwCurTime )
					SetTyrState( pOwner );
			}
			else
			{
				if( pLupinItem && pOwner->IsNeedProcess() && !pLupinItem->IsEnableLupinJumpAction() )
					return;

				m_dwChargingStartTime = FRAMEGETTIME();
				
				DWORD dwLooseTime = pOwner->GetActionStopTotalDelay();

				if( pMidnight && pOwner->IsNeedProcess() )
					pMidnight->CheckJumpCmdInputState( pOwner );

				if( m_bEnableJumpDash && IsCanComboDash( pOwner, fHeightGap ) )
				{
					if( pBangtanItem || pTyrItem )
					{
						SetComboDash( pOwner, fHeightGap );
						return;
					}
					else if( m_dwEnableComboTime_E > 0 && m_dwEnableComboTime_E + dwLooseTime < dwCurTime )
					{
						if( CheckAliceGun( pOwner ) )
						{
							SetComboDash( pOwner, fHeightGap );
						}
						return;
					}
					else if( m_dwNightmareKeyTime > 0 && m_dwNightmareKeyTime + dwLooseTime < dwCurTime )
					{
						SetComboDash( pOwner, fHeightGap );
						return;
					}
				}

				CheckAirJump( pOwner );

				if( m_bReserveJumpAttack )
				{
					if( dwCurTime >= m_dwEnableComboTime_S+dwLooseTime )
					{
						if( CheckAliceGun( pOwner ) )
						{
							SetComboJumpAttack( pOwner );
						}						
					}
				}
				float fHeightGap = pOwner->GetWorldPosition().y - pOwner->GetBottomHeight();
				if( IsCanJumpCharge( pOwner, fHeightGap ) )
				{
					SetJumpChargingState( pOwner );
					return;
				}
			}
		}
		break;
	case PJS_TELEPORT:
		ProcessTeleport( pOwner );
		break;
	case PJS_EXTEND_ATTACK:
		CheckAirJump( pOwner );
		break;
	case PJS_END:
		if( m_dwMotionEndTime > 0 && m_dwMotionEndTime <= FRAMEGETTIME() )
		{
			pOwner->CheckExtendAttackEndJump();
		}
		break;
	case PJS_AZAZEL:
		ProcessAzazelAttack( pOwner );
		break;
	}
}

void ioIljimaeJump::ProcessComboDash( ioBaseChar *pOwner )
{
	if( !pOwner ) return;

	if( m_ComboDashType != CDT_NO_GRAVITY )
		return;
	
	DWORD dwCurTime = FRAMEGETTIME();

	switch( m_ComboState )
	{
	case CST_DASH:
		if( m_dwComboDashEndTime > 0 && m_dwComboDashEndTime < dwCurTime )
		{
			m_dwComboDashEndTime = 0;
			m_ComboState = CST_DASH_END;

			if( m_dwComboDashEndJumpPower > 0.0f )
			{
				pOwner->SetSKillEndJumpState( m_dwComboDashEndJumpPower );
			}
			else
			{
				int iAniID = pOwner->GetJumppingAnimationIdx();
				ioEntityGroup *pGrp = pOwner->GetGroup();
				if( pGrp )
				{
				pGrp->ClearAllActionAni( FLOAT100, true );
				pGrp->ClearAllLoopAni( FLOAT100, true );
				pGrp->SetActionAni( iAniID,	0.0f, 0.0f, FLOAT1, FLOAT1,	0.0f, 0.0f,	true );
				}
			}
		}
		break;
	case CST_DASH_END:
		break;
	}
}

void ioIljimaeJump::CheckComboJumpAttack( ioBaseChar *pOwner )
{
	int iMaxCnt = m_ComboAttackList.size();
	if( !COMPARE( m_iCurComboCnt, 0, iMaxCnt ) )
	{
		return;
	}

	if( m_bReserveJumpAttack )
		return;

	DWORD dwCurTime = FRAMEGETTIME();
	DWORD dwLooseTime = 0;

	if( pOwner )
		dwLooseTime = pOwner->GetActionStopTotalDelay();

	if( m_iCurComboCnt > 0 && !COMPARE( dwCurTime, m_dwEnableComboTime_S+dwLooseTime, m_dwEnableComboTime_E+dwLooseTime ) )
		return;

	if( m_iCurComboCnt > 0 && pOwner->IsHasCrown() )
		return;

	m_bReserveJumpAttack = true;
}

void ioIljimaeJump::SetComboJumpAttack( ioBaseChar *pOwner )
{
	int iMaxCnt = m_ComboAttackList.size();
	if( !COMPARE( m_iCurComboCnt, 0, iMaxCnt ) )
		return;

	DWORD dwCurTime = FRAMEGETTIME();

	if( pOwner->IsNeedProcess() && pOwner->IsCanRotateJumpAttack() )
	{
		ioUserKeyInput::DirKeyInput eDir = pOwner->CheckCurDirKey();
		if( eDir != ioUserKeyInput::DKI_NONE )
		{
			float fYawD = 180.0f + 45.0f * ( eDir - ioUserKeyInput::DKI_UP );
			fYawD = ioMath::ArrangeHead( fYawD );

			float fYawR = DEGtoRAD( fYawD );
			D3DXQUATERNION qtRotate;
			D3DXQuaternionRotationYawPitchRoll( &qtRotate, fYawR, 0.0f, 0.0f );

			pOwner->SetTargetRotToRotate( qtRotate, false, false );
			pOwner->SetInputDirection( pOwner->GetMoveDir() );
		}
	}

	ioEntityGroup *pGrp = pOwner->GetGroup();
	pGrp->ClearAllActionAni( FLOAT100, true );
	pGrp->ClearAllLoopAni( FLOAT100, true );

	if( pOwner && !pOwner->IsActionStop() )
		pOwner->InitActionStopDelay( true );
	else
		pOwner->InitActionStopDelay( false );

	int iAniID = pGrp->GetAnimationIdx( m_ComboAttackList[m_iCurComboCnt].m_AttackAnimation );
	float fTimeRate = m_ComboAttackList[m_iCurComboCnt].m_fAttackAniRate;
	DWORD dwPreDelay = m_ComboAttackList[m_iCurComboCnt].m_dwPreDelay;

	DWORD dwStartTime = dwCurTime + dwPreDelay;
	m_dwEnableComboTime_E = dwStartTime + pGrp->GetAnimationFullTime( iAniID ) * fTimeRate;
	DWORD dwKeyTime = pGrp->GetAnimationEventTime_e( iAniID, "nightday_dash" ) * fTimeRate;
	if ( dwKeyTime != 0 )
		m_dwNightmareKeyTime = dwStartTime + dwKeyTime;

	pOwner->SetAutoTarget( ATT_JUMP );

	pGrp->SetActionAni( iAniID, FLOAT100, FLOAT100, FLOAT1, FLOAT1/fTimeRate, 0.0f, dwPreDelay, true );		
	pOwner->RefreshFireTimeList( iAniID,
								 m_ComboAttackList[m_iCurComboCnt].m_vWeaponInfoList,
								 FTT_JUMP_ATTACK,
								 fTimeRate,
								 dwPreDelay );

	pOwner->IncreaseWeaponIndexBase();

	DWORD dwTrackingTime = dwStartTime;
	dwTrackingTime += pGrp->GetAnimationEventTime_e( iAniID, "tracking_e" ) * fTimeRate;

	m_dwEnableComboTime_S = dwStartTime + CheckKeyReserveTime( pOwner, iAniID, fTimeRate );

	pOwner->SetAutoTargetTrackingEndTime( dwTrackingTime );

	// For AirJumpPower
	SetAirJump( pOwner, iAniID, fTimeRate );

	ioAliceObjectItem *pAliceGun = ToAliceObjectItem( pOwner->GetPriorityItem( SPT_ATTACK ) );
	if( pAliceGun )
	{	
		pAliceGun->NotifyJumpAttack( pOwner, m_iCurComboCnt );
	}

	if( pOwner->IsNeedSendNetwork() )
	{
		SP2Packet kPacket( CUPK_EXTEND_JUMP );
		kPacket << pOwner->GetCharName();
		kPacket << (int)GetType();
		kPacket << pOwner->GetHP().m_fCurValue;
		kPacket << SSC_ATTACK;
		kPacket << m_iCurComboCnt;
		kPacket << pOwner->GetTargetRot();
		kPacket << pOwner->GetMoveDir();
		kPacket << pOwner->GetInputDir();
		pOwner->FillAutoTargetInfo( kPacket );
		P2PNetwork::SendToAllPlayingUser( kPacket );
	}

	m_bReserveJumpAttack = false;
	m_bUseExtendJump = true;

	pOwner->SetEnableHide( false );
	pOwner->SetNoWoundState( false );
	pOwner->SetEnableChangeTeam( false );
	
	pOwner->SetChangeJumpMotion( false, "", "", "", 0.0f );

	m_iCurComboCnt++;
	pOwner->SetJumpAttackState();
}

void ioIljimaeJump::SetLupinBirdNormalAttack( ioBaseChar *pOwner, int nCombo )
{
	ioItem *pItem = pOwner->GetEquipedItem( ES_WEAPON );
	iolupinItem *pLupin = ToLupinItem( pItem );
	if( !pLupin )
		return;

	if( (int)m_vLupinBirdMoveNormalInfo.size() > nCombo && nCombo >= 0)
	{
		if( m_vLupinBirdMoveNormalInfo[nCombo].bCreate )
		{
			if( pOwner->IsNeedProcess() )
				pLupin->ChangeToDummyChar( pOwner, false );
		}

		pLupin->SetMoveDummyPos( pOwner, m_vLupinBirdMoveNormalInfo[nCombo], false );
	}
}

bool ioIljimaeJump::CheckAliceGun( ioBaseChar *pOwner )
{
	ioAliceObjectItem *pAliceGun = ToAliceObjectItem( pOwner->GetPriorityItem( SPT_ATTACK ) );
	if( pAliceGun && pAliceGun->IsReleaseItem( pOwner ) )
	{
		ioAliceItem* pAlice = ToAliceItem( pOwner->GetWeapon() );
		if( pAlice )
		{
			return false;
		}
	}

	return true;
}

void ioIljimaeJump::ApplyJump( ioBaseChar *pOwner, SP2Packet &rkPacket )
{
	if( !pOwner ) return;
	
	ioEntityGroup *pGrp = pOwner->GetGroup();
	if( !pGrp ) return;

	int iState;
	rkPacket >> iState;

	switch( iState )
	{
	case SSC_ATTACK:
		{
			rkPacket >> m_iCurComboCnt;

			D3DXQUATERNION qtRot;
			rkPacket >> qtRot;

			D3DXVECTOR3 vMoveDir, vInputDir;
			rkPacket >> vMoveDir >> vInputDir;

			pOwner->SetJumpAttckTargetRot( qtRot, vMoveDir, vInputDir );

			if( !COMPARE( m_iCurComboCnt, 0, (int) m_ComboAttackList.size() ) )
				return;

			pGrp->ClearAllActionAni( FLOAT100, true );
			pGrp->ClearAllLoopAni( FLOAT100, true );

			if( pOwner && !pOwner->IsActionStop() )
				pOwner->InitActionStopDelay( true );
			else
				pOwner->InitActionStopDelay( false );

			DWORD dwCurTime = FRAMEGETTIME();

			int iAniID = pGrp->GetAnimationIdx( m_ComboAttackList[m_iCurComboCnt].m_AttackAnimation );
			float fTimeRate = m_ComboAttackList[m_iCurComboCnt].m_fAttackAniRate;
			DWORD dwPreDelay = m_ComboAttackList[m_iCurComboCnt].m_dwPreDelay;
			m_dwEnableComboTime_E = dwCurTime + pGrp->GetAnimationFullTime( iAniID ) * fTimeRate;
			DWORD dwKeyTime = pGrp->GetAnimationEventTime_e( iAniID, "nightday_dash" ) * fTimeRate;
			if ( dwKeyTime != 0 )
				m_dwNightmareKeyTime = dwCurTime + dwKeyTime;

			pOwner->SetAutoTarget( ATT_JUMP );

			pGrp->SetActionAni( iAniID, FLOAT100, FLOAT100, FLOAT1, FLOAT1/fTimeRate, 0.0f, dwPreDelay, true );
			pOwner->RefreshFireTimeList( iAniID,
				m_ComboAttackList[m_iCurComboCnt].m_vWeaponInfoList,
				FTT_JUMP_ATTACK,
				fTimeRate,
				dwPreDelay );

			pOwner->IncreaseWeaponIndexBase();

			DWORD dwStartTime = dwCurTime + dwPreDelay;
			DWORD dwTrackingTime = dwStartTime;
			dwTrackingTime += pGrp->GetAnimationEventTime_e( iAniID, "tracking_e" ) * fTimeRate;

			m_dwEnableComboTime_S = dwStartTime + CheckKeyReserveTime( pOwner, iAniID, fTimeRate );

			pOwner->SetAutoTargetTrackingEndTime( dwTrackingTime );

			// For AirJumpPower
			SetAirJump( pOwner, iAniID, fTimeRate );

			pOwner->ApplyAutoTargetInfo( rkPacket );

			m_bReserveJumpAttack = false;
			m_bUseExtendJump = true;

			pOwner->SetEnableHide( false );
			pOwner->SetNoWoundState( false );
			pOwner->SetEnableChangeTeam( false );

			pOwner->SetChangeJumpMotion( false, "", "", "", 0.0f );

			ioAliceObjectItem *pAliceGun = ToAliceObjectItem( pOwner->GetPriorityItem( SPT_ATTACK ) );
			if( pAliceGun )
			{
				pAliceGun->NotifyJumpAttack( pOwner, m_iCurComboCnt );
			}

			SetLupinBirdNormalAttack( pOwner, m_iCurComboCnt );

			m_iCurComboCnt++;
			m_JumpState = PJS_NORMAL_ATTACK;
			pOwner->SetJumpAttackState();
		}
		break;
	case SSC_DASH:
		{
			D3DXVECTOR3 vDir;
			rkPacket >> vDir;

			pOwner->SetMoveDirection( vDir );

			D3DXQUATERNION qtRotate;
			rkPacket >> qtRotate;

			pOwner->SetTargetRotAndMoveDirChange( qtRotate );

			ioEntityGroup *pGrp = pOwner->GetGroup();
			DWORD dwCurTime = FRAMEGETTIME();

			int iAniID = pGrp->GetAnimationIdx( m_DashJumpAttack.m_AttackAnimation );
			float fTimeRate = m_DashJumpAttack.m_fAttackAniRate;
			DWORD dwPreDelay = m_DashJumpAttack.m_dwPreDelay;
			DWORD dwEndDelay = m_DashJumpAttack.m_dwEndDelay;

			pGrp->ClearAllActionAni( FLOAT100, true );
			pGrp->ClearAllLoopAni( FLOAT100, true );

			pOwner->SetAutoTarget( ATT_JUMP );

			pGrp->SetActionAni( iAniID, FLOAT100, FLOAT100, FLOAT1, FLOAT1/fTimeRate, 0.0f, dwPreDelay, true );		

			pOwner->RefreshFireTimeList( iAniID,
										 m_DashJumpAttack.m_vWeaponInfoList,
										 FTT_JUMP_ATTACK,
										 fTimeRate,
										 dwPreDelay );

			pOwner->IncreaseWeaponIndexBase();

			m_dwComboDashStartTime = dwCurTime;
			m_dwComboDashEndTime = dwCurTime + dwPreDelay + dwEndDelay + pGrp->GetAnimationFullTime( iAniID ) * fTimeRate;

			pOwner->CheckCharColSkipTime( iAniID, fTimeRate, dwPreDelay );
			pOwner->SetReservedSliding( m_DashJumpAttack.m_vForceInfoList, iAniID, fTimeRate, dwPreDelay );

			DWORD dwKeyReserveTime = dwCurTime + CheckKeyReserveTime( pOwner, iAniID, fTimeRate );

			DWORD dwTrackingTime = dwCurTime;
			dwTrackingTime += pGrp->GetAnimationEventTime_e( iAniID, "tracking_e" ) * fTimeRate;
			pOwner->SetAutoTargetTrackingEndTime( dwTrackingTime );

			m_bUseExtendJump = true;

			pOwner->SetEnableHide( false );
			pOwner->SetNoWoundState( false );
			pOwner->SetEnableChangeTeam( false );
			pOwner->SetChangeJumpMotion( false, "", "", "", 0.0f );
			pOwner->SetGravityAmt( 0.0f );
			pOwner->SetForcePowerAmt( 0.0f );
			pOwner->SetCurJumpPowerAmt( 0.0f );

			m_ComboState = CST_DASH;
			pOwner->SetComboDashState();
			pOwner->ApplyAutoTargetInfo( rkPacket );

			if( m_bUseNewJumpTypeDash )
			{
				SetNewJumpTypeDashAttack( pOwner, m_dwComboDashEndTime, dwKeyReserveTime );
			}

			iolupinItem *pLupin = ToLupinItem( pOwner->GetPriorityItem( SPT_ATTACK ) );
			if( pLupin )
				pLupin->SetMoveDummyPos( pOwner, false, false, false );
		}
		break;
	case SSC_DOUBLE_JUMP:
		{
			float fJumpPower, fJumpForce;
			D3DXQUATERNION qtRotate;

			rkPacket >> fJumpPower >> fJumpForce;
			rkPacket >> qtRotate;

			if( fJumpPower >= 0.0f )
			{
				m_bUsedDoubleJump = true;
				m_bUseExtendJump = true;

				pOwner->SetCurJumpPowerAmt( 0.0f );
				pOwner->SetForcePowerAmt( 0.0f );
				pOwner->SetGravityAmt( 0.0f );
				pOwner->SetTargetRot( qtRotate );
				pOwner->SetJumpPower( fJumpPower );

				D3DXVECTOR3 vJumpDir = pOwner->GetMoveDir();
				if( m_fDoubleJumpForce > 0.0f )
					pOwner->SetForcePower( vJumpDir, fabs(m_fDoubleJumpForce), FLOAT1, true );
				else
					pOwner->SetForcePower( -vJumpDir, fabs(m_fDoubleJumpForce), FLOAT1, true );

				SetDoubleJumpAni( pOwner );
			}
		}
		break;
	case SSC_CHANGE_TELEPORT:
		{
			ChangeToTeleport( pOwner );
			D3DXQUATERNION qtRotate;
			rkPacket >> qtRotate;
			pOwner->SetTargetRot( qtRotate );
		}
		break;
	case SSC_TELEPORT:
		{
			D3DXQUATERNION qtRot;
			D3DXVECTOR3 vPos;
			int iJumpState;

			rkPacket >> qtRot;
			rkPacket >> vPos;
			rkPacket >> iJumpState;
			m_JumpState = (ProcessJumpState)iJumpState;
			pOwner->SetTargetRotAndMoveDirChange( qtRot );
			pOwner->SetWorldPosition( vPos );

			// 텔레포트 종료 모션으로 전환
			pOwner->SetCurMoveSpeed( 0.0f );

			ioEntityGroup *pGrp = pOwner->GetGroup();
			pGrp->ClearAllActionAni( FLOAT100, true );
			pGrp->ClearAllLoopAni( FLOAT100, true );

			float fCurRate = FLOAT1;
			if( m_fTeleportEndMotionRate > 0.0f )
				fCurRate = m_fTeleportEndMotionRate;

			int iAniID = pGrp->GetAnimationIdx( m_TeleportEndMotion );
			pGrp->SetActionAni( iAniID, FLOAT100, FLOAT100, FLOAT1, FLOAT1/fCurRate );

			m_dwMotionEndTime = FRAMEGETTIME();
			m_dwMotionEndTime += pGrp->GetAnimationFullTime(iAniID) * fCurRate;

			pOwner->SetExtendAttackEndJumpInfo( iAniID, fCurRate, 0 );
		}
		break;
	case SSC_EXTEND_ATTACK:
		{
			D3DXQUATERNION qtRot;
			rkPacket >> qtRot;

			pOwner->SetTargetRotAndMoveDirChange( qtRot );

			ChangeToJumpAttack( pOwner );
			ChangeToPowerChargeJumpAttack( pOwner );

			pOwner->ApplyAutoTargetInfo( rkPacket );
		}
		break;
	case SSC_DASH_ADD_ATTACK:
		{
			int iAttackIndex;
			D3DXVECTOR3 vPos;
			D3DXQUATERNION qtRot;

			rkPacket >> iAttackIndex;
			rkPacket >> vPos;
			rkPacket >> qtRot;

			pOwner->SetWorldPosition( vPos );
			pOwner->SetTargetRotAndMoveDirChange( qtRot );
			SetJumpDashAddAttack( pOwner, iAttackIndex );

			pOwner->ApplyAutoTargetInfo( rkPacket );
		}
		break;
	case SSC_FLY:
		{
			D3DXQUATERNION qtRot;
			rkPacket >> qtRot;
			pOwner->SetTargetRotAndMoveDirChange( qtRot );

			SetNightmareState( pOwner );
		}
		break;
	case SSC_DEFENSE:
		{
			SetDefense( pOwner );
		}
		break;
	case SSC_RELEASE_DEFENSE:
		{
			ReleaseDefense( pOwner );
		}
		break;
	case SSC_AZAZEL:
		{
			D3DXQUATERNION qtRot;
			rkPacket >> qtRot;
			pOwner->SetTargetRotAndMoveDirChange( qtRot );

			rkPacket >> m_AzazelAimedTarget;
			SetAzazelState( pOwner, false );
		}
		break;
	case SSC_CHANGE_ON:
		{
			D3DXQUATERNION qtRot;
			rkPacket >> qtRot;
			pOwner->SetTargetRotAndMoveDirChange( qtRot );

			SetChangeOnGriffinState( pOwner );
		}
		break;
	}
}

float ioIljimaeJump::GetExtraJumpSpeedRate( const D3DXVECTOR3 &vDir, const D3DXQUATERNION &qtRot ) const
{
	D3DXVECTOR3 vCurDir = vDir;

	D3DXVECTOR3 vXZDir( vCurDir.x, 0.0f, vCurDir.z );
	D3DXVec3Normalize( &vXZDir, &vXZDir );

	D3DXQUATERNION qtTargetRot;
	ioMath::CalcDirectionQuaternion( qtTargetRot, -vXZDir );

	float fCurYaw, fTargetYaw;
	fTargetYaw = RADtoDEG( ioMath::QuaterToYaw( qtTargetRot ) );
	fTargetYaw = ioMath::ArrangeHead( fTargetYaw );
	fCurYaw = RADtoDEG( ioMath::QuaterToYaw( qtRot ) );
	fCurYaw = ioMath::ArrangeHead( fCurYaw );

	float fAngleGap = ioMath::AngleGap( fCurYaw, fTargetYaw );
	fAngleGap += 360.0f;

	int iTest = (int)fAngleGap % 360;
	iTest /= 45;

	if( m_bUsedDoubleJump )
	{
		switch( iTest )
		{
		case 0:
		case 7:
			return m_fDoubleJumpSpeedRateFR;
		case 1:
		case 2:
		case 5:
		case 6:
			return m_fDoubleJumpSpeedRateSI;
		case 3:
		case 4:
			return m_fDoubleJumpSpeedRateBG;
		}
	}
	else if( m_bUseExtendJump )
	{
		switch( iTest )
		{
		case 0:
		case 7:
			return m_fExtendJumpSpeedRateFR;
		case 1:
		case 2:
		case 5:
		case 6:
			return m_fExtendJumpSpeedRateSI;
		case 3:
		case 4:
			return m_fExtendJumpSpeedRateBG;
		}
	}

	return m_fJumpSpeedRate;
}

bool ioIljimaeJump::IsCanJumpingSkill( ioBaseChar *pOwner )
{
	int iMaxCnt = m_ComboAttackList.size();
	if( COMPARE( m_iCurComboCnt, 0, iMaxCnt ) )
		return true;
	return false;
}

DWORD ioIljimaeJump::GetChargingTime( ioBaseChar *pChar )
{
	if( m_bJumpChargeEnable )
		return m_dwJumpKeyChargingTime;

	return 0;
}

bool ioIljimaeJump::IsComboDashState()
{
	switch( m_ComboState )
	{
	case CST_DASH:
	case CST_DASH_END:
		return true;
	}

	return false;
}

bool ioIljimaeJump::IsCanComboDash( ioBaseChar *pOwner, float fHeightGap )
{
	if( !pOwner ) return false;
	if( !pOwner->IsNeedProcess() ) return false;

	if( m_bUsedComboDash && ToBangtanItem( pOwner->GetEquipedItem(ES_WEAPON) ) )
		return false;
	if( m_bUsedComboDash && ToBubbleItem( pOwner->GetEquipedItem(ES_WEAPON) ) )
		return false;
	if( (m_bUsedComboDash || m_bUsedNightmare) && ToNightmareItem( pOwner->GetEquipedItem(ES_WEAPON) ) )
		return false;

	if( pOwner->IsHasCrown() ||
		pOwner->IsCatchMode() ||
		pOwner->IsPrisonerMode() ||
		(pOwner->GetUsedBuffJump() != BJUT_NONE) ||
		(pOwner->GetUsedBuffMultiFlyJump() != BJUT_NONE) ||
		(pOwner->GetUsedBuffFlyJump() != BJUT_NONE) )
		return false;

	if( pOwner->IsChatModeState( true ) )
		return false;

	if( pOwner->IsBuffLimitExtendJump() )
		return false;

	if( pOwner->HasBuff(BT_AIR_ATTACK) )
		return false;

	if( m_ComboState != CST_NONE )
		return false;

	if( !pOwner->IsCanJumpAttackEnableTime() )
		return false;

	if( !CheckAliceGun( pOwner ) )
		return false;

	DWORD dwMoveStartTime = pOwner->GetJumpMoveStartTime();
	if( dwMoveStartTime == 0 )
		return false;

	if( !pOwner->IsDirKeyDoubleClick() )
		return false;

	return true;
}

void ioIljimaeJump::SetComboDash( ioBaseChar *pOwner, float fHeightGap )
{
	ioEntityGroup *pGrp = pOwner->GetGroup();

	DWORD dwCurTime = FRAMEGETTIME();
	D3DXVECTOR3 vMoveDir = pOwner->GetMoveDir();

	int iAniID = pGrp->GetAnimationIdx( m_DashJumpAttack.m_AttackAnimation );
	float fTimeRate = m_DashJumpAttack.m_fAttackAniRate;
	DWORD dwPreDelay = m_DashJumpAttack.m_dwPreDelay;
	DWORD dwEndDelay = m_DashJumpAttack.m_dwEndDelay;

	m_bUseExtendJump = true;
	m_bUsedComboDash = true;
	m_bUsedComboDashEnd = false;

	pOwner->SetCurJumpPowerAmt( 0.0f );
	pOwner->SetForcePowerAmt( 0.0f );
	pOwner->SetGravityAmt( 0.0f );

	pOwner->SetEnableHide( false );
	pOwner->SetEnableChangeTeam( false );
	pOwner->SetChangeJumpMotion( false, "", "", "", 0.0f );

	pOwner->ChangeDirectionByInputDir( false );

	pGrp->ClearAllActionAni( FLOAT100, true );
	pGrp->ClearAllLoopAni( FLOAT100, true );

	pOwner->SetAutoTarget( ATT_JUMP );
	pGrp->SetActionAni( iAniID, FLOAT100, FLOAT100, FLOAT1, FLOAT1/fTimeRate, 0.0f, dwPreDelay, true );

	pOwner->RefreshFireTimeList( iAniID,
								 m_DashJumpAttack.m_vWeaponInfoList,
								 FTT_JUMP_ATTACK,
								 fTimeRate,
								 dwPreDelay );

	pOwner->IncreaseWeaponIndexBase();

	m_dwComboDashStartTime = dwCurTime;
	m_dwComboDashEndTime = dwCurTime + dwPreDelay + dwEndDelay + pGrp->GetAnimationFullTime( iAniID ) * fTimeRate;

	pOwner->CheckCharColSkipTime( iAniID, fTimeRate, dwPreDelay );
	pOwner->SetReservedSliding( m_DashJumpAttack.m_vForceInfoList, iAniID, fTimeRate, dwPreDelay );

	DWORD dwKeyReserveTime = dwCurTime + CheckKeyReserveTime( pOwner, iAniID, fTimeRate );

	DWORD dwTrackingTime = dwCurTime;
	dwTrackingTime += pGrp->GetAnimationEventTime_e( iAniID, "tracking_e" ) * fTimeRate;
	pOwner->SetAutoTargetTrackingEndTime( dwTrackingTime );

	m_ComboState = CST_DASH;
	m_JumpState = PJS_DASH_ATTACK;
	m_iJumpDashAddAttackIndex = 0;

	pOwner->SetComboDashState();

	if( pOwner->IsNeedSendNetwork() )
	{
		SP2Packet kPacket( CUPK_EXTEND_JUMP );
		kPacket << pOwner->GetCharName();
		kPacket << (int)GetType();
		kPacket << pOwner->GetHP().m_fCurValue;
		kPacket << SSC_DASH;
		kPacket << vMoveDir;
		kPacket << pOwner->GetTargetRot();
		pOwner->FillAutoTargetInfo( kPacket );
		P2PNetwork::SendToAllPlayingUser( kPacket );
	}

	iolupinItem *pLupin = ToLupinItem( pOwner->GetPriorityItem( SPT_ATTACK ) );
	if( pLupin )
		pLupin->SetMoveDummyPos( pOwner, false, false, true );

	if( m_bUseNewJumpTypeDash )
	{
		SetNewJumpTypeDashAttack( pOwner, m_dwComboDashEndTime, dwKeyReserveTime );
		return;
	}
}

void ioIljimaeJump::SetComboDashLand( ioBaseChar *pOwner, float fHeightGap )
{
	ioEntityGroup *pGrp = pOwner->GetGroup();

	DWORD dwCurTime = FRAMEGETTIME();
	D3DXVECTOR3 vMoveDir = pOwner->GetMoveDir();

	int iAniID = pGrp->GetAnimationIdx( m_ComboDashLandAni );
	float fTimeRate = m_fComboDashLandAniRate;

	m_bUseExtendJump = true;
	m_bUsedComboDash = true;
	m_bUsedComboDashEnd = true;

	pOwner->SetCurJumpPowerAmt( 0.0f );
	pOwner->SetForcePowerAmt( 0.0f );
	pOwner->SetGravityAmt( 0.0f );
	pOwner->SetEnableHide( false );
	pOwner->SetEnableChangeTeam( false );
	pOwner->SetChangeJumpMotion( false, "", "", "", 0.0f );

	pGrp->ClearAllActionAni( FLOAT100, true );
	pGrp->ClearAllLoopAni( FLOAT100, true );

	pOwner->SetAutoTarget( ATT_JUMP );
	pGrp->SetActionAni( iAniID, FLOAT100, FLOAT100, FLOAT1, FLOAT1/fTimeRate, 0.0f, 0, true );

	m_dwComboDashStartTime = dwCurTime;
	m_dwComboDashEndTime = dwCurTime + pGrp->GetAnimationFullTime( iAniID ) * fTimeRate;

	pOwner->CheckCharColSkipTime( iAniID, fTimeRate );

	DWORD dwKeyReserveTime = dwCurTime + CheckKeyReserveTime( pOwner, iAniID, fTimeRate );

	DWORD dwTrackingTime = dwCurTime;
	dwTrackingTime += pGrp->GetAnimationEventTime_e( iAniID, "tracking_e" ) * fTimeRate;
	pOwner->SetAutoTargetTrackingEndTime( dwTrackingTime );

	m_ComboState = CST_DASH;
	m_JumpState = PJS_DASH_ATTACK;
	m_iJumpDashAddAttackIndex = 0;

	if( pOwner->IsNeedSendNetwork() )
	{
		SP2Packet kPacket( CUPK_EXTEND_JUMP );
		kPacket << pOwner->GetCharName();
		kPacket << (int)GetType();
		kPacket << pOwner->GetHP().m_fCurValue;
		kPacket << SSC_DASH_END;
		kPacket << vMoveDir;
		kPacket << pOwner->GetTargetRot();
		pOwner->FillAutoTargetInfo( kPacket );
		P2PNetwork::SendToAllPlayingUser( kPacket );
	}

	if( m_bUseNewJumpTypeDash )
	{
		SetNewJumpTypeDashAttack( pOwner, m_dwComboDashEndTime, dwKeyReserveTime );
		return;
	}
}

float ioIljimaeJump::GetCurGravityAmt()
{
	if( m_ComboState == CST_DASH || m_ComboState == CST_NEW_JUMP )
	{
		if( m_ComboDashType == CDT_GRAVITY )
			return m_fComboDashGravityAmt;
		else if( m_ComboDashType == CDT_NO_GRAVITY )
			return -FLOAT1;
	}

	return 0.0f;
}

bool ioIljimaeJump::IsEnableJumpLandEndDash()
{
	if( m_ComboState == CST_DASH || m_ComboState == CST_DASH_END )
		return true;

	return false;
}

bool ioIljimaeJump::IsComboDashLandEnable()
{
	if( m_bComboDashLandEnable && !m_bUsedComboDashEnd )
		return true;

	return false;
}

bool ioIljimaeJump::IsComboDashLanded()
{
	if( m_bComboDashLandEnable && m_bUsedComboDashEnd )
		return true;

	return false;
}

bool ioIljimaeJump::IsEnableJumpDash( ioBaseChar *pOwner )
{
	return m_bEnableJumpDash;
}

bool ioIljimaeJump::IsCanDoubleJump( ioBaseChar *pOwner, float fHeightGap, bool bCharge )
{
	if( !pOwner ) return false;
	if( !pOwner->IsNeedProcess() ) return false;

	if( ( !bCharge && !pOwner->IsJumpKeyPress() ) ||
		pOwner->IsHasCrown() ||
		pOwner->IsCatchMode() ||
		pOwner->IsPrisonerMode() ||
		(pOwner->GetUsedBuffJump() != BJUT_NONE) ||
		(pOwner->GetUsedBuffMultiFlyJump() != BJUT_NONE) ||
		(pOwner->GetUsedBuffFlyJump() != BJUT_NONE) ||
		pOwner->GetUsedJumpAttack()	)
		return false;

	if( pOwner->HasBuff( BT_BUFF_FLY_JUMP ) || pOwner->HasBuff( BT_BUFF_FLY_JUMP2 ) )
		return false;

	if ( pOwner->HasBuff( BT_BUFF_AIR_WALK ) )
		return false;

	if( pOwner->IsBuffLimitExtendJump() )
		return false;

	if( m_iCurJumpCnt >= m_iMaxJumpCnt )
		return false;

	DWORD dwMoveStartTime = pOwner->GetJumpMoveStartTime();
	if( dwMoveStartTime == 0 )
		return false;

	if( FRAMEGETTIME() - dwMoveStartTime < m_dwDoubleJumpEnableTime )
		return false;

	if( fHeightGap < m_fDoubleJumpEnableHeight )
		return false;

	return true;
}

bool ioIljimaeJump::CheckJumpCharging( ioBaseChar *pOwner )
{
	if( !pOwner || !pOwner->IsNeedProcess() ) 
		return false;
	if( !m_bJumpChargeEnable )
		return false;

	DWORD dwCurTime = FRAMEGETTIME();
	if ( pOwner->IsJumpKeyDown() && !pOwner->IsHasCrown() && pOwner->IsEnableAzazelSpecialAttack() )
	{
		if( m_dwChargingStartTime + m_dwJumpKeyChargingTime < dwCurTime )
		{
			m_dwChargingStartTime = 0;
			SetAzazelTeleportState( pOwner );
			return true;
		}
	}
	else
	{
		float fHeightGap = pOwner->GetWorldPosition().y - pOwner->GetBottomHeight();
		if( IsCanDoubleJump( pOwner, fHeightGap, true ) && m_fDoubleJumpPower > 0.0f )
		{
			SetDoubleJump( pOwner );
			return true;
		}
		else
		{
			m_JumpState = PJS_NONE;
			m_dwChargingStartTime = 0;
		}
	}

	return false;
}

void ioIljimaeJump::SetJumpChargingState( ioBaseChar *pOwner )
{
	if( !pOwner ) return;

	ioEntityGroup *pGrp = pOwner->GetGroup();
	if( !pGrp ) return;

	DWORD dwCurTime = FRAMEGETTIME();

	m_bUseExtendJump = true;
	m_dwChargingStartTime = dwCurTime;
}

bool ioIljimaeJump::IsCanJumpCharge( ioBaseChar *pOwner, float fHeightGap )
{
	if( !m_bJumpChargeEnable )
		return false;
	if( !pOwner || !pOwner->IsNeedProcess() ) 
		return false;

	if( !pOwner->IsJumpKeyPress() ||
		pOwner->IsHasCrown() ||
		pOwner->IsCatchMode() ||
		pOwner->IsPrisonerMode() ||
		(pOwner->GetUsedBuffJump() != BJUT_NONE) ||
		(pOwner->GetUsedBuffMultiFlyJump() != BJUT_NONE) ||
		(pOwner->GetUsedBuffFlyJump() != BJUT_NONE) )
		return false;

	if( pOwner->HasBuff( BT_BUFF_FLY_JUMP ) || pOwner->HasBuff( BT_BUFF_FLY_JUMP2 ) )
		return false;

	if ( pOwner->HasBuff( BT_BUFF_AIR_WALK ) )
		return false;

	if( pOwner->IsBuffLimitExtendJump() )
		return false;

	DWORD dwMoveStartTime = pOwner->GetJumpMoveStartTime();
	if( dwMoveStartTime == 0 ) 
		return false;

	if( FRAMEGETTIME() - dwMoveStartTime < m_dwDoubleJumpEnableTime )
		return false;

	if( fHeightGap < m_fDoubleJumpEnableHeight )
		return false;

	return true;
}

void ioIljimaeJump::SetDoubleJump( ioBaseChar *pOwner )
{
	if( !pOwner ) 
		return;

	m_bUseExtendJump = true;
	m_bUsedDoubleJump = true;
	m_dwChargingStartTime = 0;
	m_iCurJumpCnt++;

	pOwner->SetCurJumpPowerAmt( 0.0f );
	pOwner->SetForcePowerAmt( 0.0f );
	pOwner->SetGravityAmt( 0.0f );
	pOwner->SetJumpPower( m_fDoubleJumpPower );

	D3DXVECTOR3 vJumpDir = pOwner->GetMoveDir();
	if( m_fDoubleJumpForce > 0.0f )
		pOwner->SetForcePower( vJumpDir, fabs(m_fDoubleJumpForce), FLOAT1, true );
	else
		pOwner->SetForcePower( -vJumpDir, fabs(m_fDoubleJumpForce), FLOAT1, true );

	pOwner->SetEnableHide( false );
	pOwner->SetEnableChangeTeam( false );
	pOwner->SetChangeJumpMotion( false, "", "", "", 0.0f );

	g_TutorialMgr.ActionDoubleJump();

	SetDoubleJumpAni( pOwner );

	if( pOwner->IsNeedSendNetwork() )
	{
		SP2Packet kPacket( CUPK_DOUBLE_JUMP );
		kPacket << pOwner->GetCharName();
		kPacket << (int)GetType();
		kPacket << vJumpDir;
		kPacket << SSC_DOUBLE_JUMP;
		kPacket << m_fDoubleJumpPower;
		kPacket << m_fDoubleJumpForce;
		kPacket << pOwner->GetTargetRot();
		P2PNetwork::SendToAllPlayingUser( kPacket );
	}
}

void ioIljimaeJump::SetDoubleJumpAni( ioBaseChar *pOwner )
{
	ioEntityGroup *pGrp = pOwner->GetGroup();
	D3DXVECTOR3 vSightDir = pOwner->GetMoveDir();

	D3DXVECTOR3 vXZDir( vSightDir.x, 0.0f, vSightDir.z );
	D3DXVec3Normalize( &vXZDir, &vXZDir );

	D3DXQUATERNION qtTargetRot;
	ioMath::CalcDirectionQuaternion( qtTargetRot, -vXZDir );

	float fCurYaw, fTargetYaw;
	fTargetYaw = RADtoDEG( ioMath::QuaterToYaw( qtTargetRot ) );
	fTargetYaw = ioMath::ArrangeHead( fTargetYaw );
	fCurYaw = RADtoDEG( ioMath::QuaterToYaw( pOwner->GetTargetRot() ) );
	fCurYaw = ioMath::ArrangeHead( fCurYaw );

	float fAngleGap = ioMath::AngleGap( fCurYaw, fTargetYaw );
	fAngleGap += 360.0f;
	int iTest = (int)fAngleGap % 360;

	pGrp->ClearAllActionAni( FLOAT100, true );

	float fRate = 0.0f;
	int iIndex = -1;
	int iIndex2 = -1;
	iIndex = iTest / 90;
	iTest = iTest % 90;

	int iCnt = m_vDoubleJumpAniList.size();
	if( COMPARE( iIndex, 0, iCnt ) )
	{
		fRate = (float)iTest /FLOAT90;
		iIndex2 = (iIndex+1) % iCnt;

		if( !m_vDoubleJumpAniList[iIndex].IsEmpty() && !m_vDoubleJumpAniList[iIndex2].IsEmpty() )
		{
			if( fRate <= FLOAT05 )
				pGrp->SetActionAni( m_vDoubleJumpAniList[iIndex], FLOAT100, FLOAT100, (1.0f - fRate), FLOAT1/m_fDoubleJumpAniRate, 0.0f, 0, true );
			else
				pGrp->SetActionAni( m_vDoubleJumpAniList[iIndex2], FLOAT100, FLOAT100, fRate, FLOAT1/m_fDoubleJumpAniRate, 0.0f, 0, true );
		}
		else if( !m_vDoubleJumpAniList[iIndex].IsEmpty() && m_vDoubleJumpAniList[iIndex2].IsEmpty() )
		{
			pGrp->SetActionAni( m_vDoubleJumpAniList[iIndex], FLOAT100, FLOAT100, FLOAT1, FLOAT1/m_fDoubleJumpAniRate, 0.0f, 0, true );
		}
		else if( !m_vDoubleJumpAniList[iIndex2].IsEmpty() && m_vDoubleJumpAniList[iIndex].IsEmpty() )
		{
			pGrp->SetActionAni( m_vDoubleJumpAniList[iIndex2], FLOAT100, FLOAT100, FLOAT1, FLOAT1/m_fDoubleJumpAniRate, 0.0f, 0, true );
		}
		else
		{
			int iAniID = pOwner->GetJumppingAnimationIdx();
			pGrp->SetActionAni( iAniID, FLOAT100, FLOAT100, FLOAT1, FLOAT1/m_fDoubleJumpAniRate, 0.0f, 0, true );
		}
	}
	else
	{
		int iAniID = pOwner->GetJumppingAnimationIdx();
		pGrp->SetActionAni( iAniID, FLOAT100, FLOAT100, FLOAT1, FLOAT1/m_fDoubleJumpAniRate, 0.0f, 0, true );
	}
}

void ioIljimaeJump::SetNewJumpTypeDashAttack( ioBaseChar *pOwner, DWORD dwEndTime, DWORD dwReserveTime )
{
	if( !pOwner ) return;

	m_bUseExtendJump = false;
	m_ComboState = CST_NEW_JUMP;
	m_dwJumpDashAddAttackReserveTime = dwReserveTime;

	pOwner->SetNewJumpAttackStateInfo( dwEndTime, dwReserveTime, m_fComboDashGravityAmt );

	ioAliceObjectItem *pAliceGun = ToAliceObjectItem( pOwner->GetPriorityItem( SPT_ATTACK ) );
	if( pAliceGun )
	{		
		pAliceGun->NotifyDashJumpAttack( pOwner, 0 );
	}
}

void ioIljimaeJump::SetJumpDashAddAttack( ioBaseChar *pOwner, int iAttackIndex )
{
	if( !pOwner )
		return;

	ioEntityGroup *pGrp = pOwner->GetGroup();

	DWORD dwCurTime = FRAMEGETTIME();
	D3DXVECTOR3 vMoveDir = pOwner->GetMoveDir();

	if( (int)m_vJumpDashAddAttack.size() <= iAttackIndex )
		return;

	int iAniID = pGrp->GetAnimationIdx( m_vJumpDashAddAttack[iAttackIndex].m_AttackAnimation );
	float fTimeRate = m_vJumpDashAddAttack[iAttackIndex].m_fAttackAniRate;
	DWORD dwPreDelay = m_vJumpDashAddAttack[iAttackIndex].m_dwPreDelay;
	DWORD dwEndDelay = m_vJumpDashAddAttack[iAttackIndex].m_dwEndDelay;

	pOwner->SetEnableHide( false );
	pOwner->SetEnableChangeTeam( false );
	pOwner->SetChangeJumpMotion( false, "", "", "", 0.0f );

	pOwner->ChangeDirectionByInputDir( false );

	pGrp->ClearAllActionAni( FLOAT100, true );
	pGrp->ClearAllLoopAni( FLOAT100, true );

	pOwner->SetAutoTarget( ATT_JUMP );
	pGrp->SetActionAni( iAniID, FLOAT100, FLOAT100, FLOAT1, FLOAT1/fTimeRate, 0.0f, dwPreDelay, true );

	pOwner->RefreshFireTimeList( iAniID,
		m_vJumpDashAddAttack[iAttackIndex].m_vWeaponInfoList,
		FTT_JUMP_ATTACK,
		fTimeRate,
		dwPreDelay );

	pOwner->IncreaseWeaponIndexBase();

	m_dwComboDashEndTime = dwCurTime + dwPreDelay + dwEndDelay + pGrp->GetAnimationFullTime( iAniID ) * fTimeRate;

	pOwner->CheckCharColSkipTime( iAniID, fTimeRate, dwPreDelay );
	pOwner->SetReservedSliding( m_vJumpDashAddAttack[iAttackIndex].m_vForceInfoList, iAniID, fTimeRate, dwPreDelay );

	m_dwJumpDashAddAttackReserveTime = dwCurTime + CheckKeyReserveTime( pOwner, iAniID, fTimeRate );

	DWORD dwTrackingTime = dwCurTime;
	dwTrackingTime += pGrp->GetAnimationEventTime_e( iAniID, "tracking_e" ) * fTimeRate;
	pOwner->SetAutoTargetTrackingEndTime( dwTrackingTime );

	pOwner->SetComboDashState();

	if( pOwner->IsNeedSendNetwork() )
	{
		SP2Packet kPacket( CUPK_EXTEND_JUMP );
		kPacket << pOwner->GetCharName();
		kPacket << (int)GetType();
		kPacket << pOwner->GetHP().m_fCurValue;
		kPacket << SSC_DASH_ADD_ATTACK;
		kPacket << iAttackIndex;
		kPacket << pOwner->GetWorldPosition();
		kPacket << pOwner->GetTargetRot();
		pOwner->FillAutoTargetInfo( kPacket );
		P2PNetwork::SendToAllPlayingUser( kPacket );
	}

	pOwner->SetNewJumpAttackStateInfo( m_dwComboDashEndTime, m_dwJumpDashAddAttackReserveTime, m_fComboDashGravityAmt );
	m_iJumpDashAddAttackIndex++;
}

void ioIljimaeJump::ChangeToJumpAttack( ioBaseChar *pOwner )
{
	m_JumpState = PJS_NORMAL_ATTACK;
	pOwner->SetJumpAttackState();
	m_dwChargingStartTime = FRAMEGETTIME();
}

void ioIljimaeJump::ChangeToTeleport( ioBaseChar *pOwner )
{
	if( !pOwner )
		return;

	m_bUsedTeleport = true;
	m_JumpState = PJS_TELEPORT;

	ioEntityGroup *pGrp = pOwner->GetGroup();
	pGrp->ClearAllActionAni( FLOAT100, true );
	pGrp->ClearAllLoopAni( FLOAT100, true );

	float fCurRate = FLOAT1;
	if( m_fTeleportMotionRate > 0.0f )
		fCurRate = m_fTeleportMotionRate;

	int iAniID = pGrp->GetAnimationIdx( m_TeleportMotion );
	pGrp->SetActionAni( iAniID, FLOAT100, FLOAT100, FLOAT1, FLOAT1/fCurRate );

	m_dwMotionEndTime = FRAMEGETTIME();
	m_dwMotionEndTime += pGrp->GetAnimationFullTime(iAniID) * fCurRate;
}

void ioIljimaeJump::ProcessTeleport( ioBaseChar *pOwner )
{
	if( !pOwner )	return;
	if( !pOwner->IsNeedProcess() )
		return;
	ioPlayStage *pStage = pOwner->GetCreator();
	if( !pStage )
		return;

	if( m_dwMotionEndTime > 0 && m_dwMotionEndTime <= FRAMEGETTIME() )
	{
		pOwner->SetCurJumpPowerAmt( 0.0f );
		pOwner->SetForcePowerAmt( 0.0f );
		pOwner->SetGravityAmt( 0.0f );

		m_JumpState = PJS_END;

		ioItem *pItem = pOwner->GetEquipedItem(ES_WEAPON);
		ioIljimaeItem *pIljimaeItem = ToIljimaeItem( pItem );
		if( !pIljimaeItem )	return;
		
		m_vTargetNameList.clear();
		pIljimaeItem->GetCurTargetList( m_vTargetNameList );
		if( m_PreTargetName.IsEmpty() )
			m_PreTargetName = pIljimaeItem->GetPreTarget();
		m_vReturnPosition = pIljimaeItem->GetReturnPosition();

		ioHashString szCurTarget;
		szCurTarget.Clear();

		ioBaseChar *pTarget = pOwner;

		int iCnt = m_vTargetNameList.size();
		float fPreDistSq = m_fTeleportRange * m_fTeleportRange;
		for( int i=0; i<iCnt; ++i )
		{
			if( m_vTargetNameList[i] == m_PreTargetName )
				continue;

			pTarget = pOwner->GetBaseChar( m_vTargetNameList[i] );
			if( !pTarget )
				continue;

			D3DXVECTOR3 vDiff = pTarget->GetWorldPosition() - pOwner->GetWorldPosition();
			float fDistSq = D3DXVec3LengthSq( &vDiff );
			if( fPreDistSq > fDistSq )
			{
				D3DXVECTOR3 vDir = vDiff;
				D3DXVec3Normalize( &vDir, &vDir );

				D3DXVECTOR3 vSearchDir = pOwner->GetTargetRot() * D3DXVECTOR3( 0.0f, 0.0f, -FLOAT1 );
				D3DXVec3Normalize( &vSearchDir, &vSearchDir );

				float fHalfCosine = cosf( DEGtoRAD( m_fTeleportAngle ) * FLOAT05 );

				if( D3DXVec3Dot( &vSearchDir, &vDir ) >= fHalfCosine )
				{
					szCurTarget = m_vTargetNameList[i];
					fPreDistSq = fDistSq;
				}
			}
		}

		m_PreTargetName = szCurTarget;
		pIljimaeItem->SetPreTarget( m_PreTargetName );
		pTarget = pOwner->GetBaseChar( szCurTarget );
		if( !pTarget )
		{
			pTarget = pOwner;
			m_PreTargetName.Clear();
			pIljimaeItem->SetPreTarget( "" );
		}

		D3DXVECTOR3 vTargetPos;
		if( szCurTarget.IsEmpty() )
		{
			if( m_vReturnPosition == ioMath::VEC3_ZERO )
			{
				vTargetPos = pOwner->GetWorldPosition();
				m_vReturnPosition = vTargetPos;
				m_vReturnPosition.y = pOwner->GetBottomHeight();
				pIljimaeItem->SetReturnPosition( m_vReturnPosition );
			}
			else
			{
				D3DXVECTOR3 vDiff = m_vReturnPosition - pOwner->GetWorldPosition();
				fPreDistSq = m_fTeleportRange * m_fTeleportRange;
				float fDistSq = D3DXVec3LengthSq( &vDiff );
				if( fPreDistSq > fDistSq )
				{
					vTargetPos = m_vReturnPosition;
					m_vReturnPosition = pOwner->GetWorldPosition();
					m_vReturnPosition.y = pOwner->GetBottomHeight();
					pIljimaeItem->SetReturnPosition( m_vReturnPosition );
				}
				else
				{
					vTargetPos = pOwner->GetWorldPosition();
					m_vReturnPosition = vTargetPos;
					m_vReturnPosition.y = pOwner->GetBottomHeight();
					pIljimaeItem->SetReturnPosition( m_vReturnPosition );
				}
			}
		}
		else
		{
			vTargetPos = pTarget->GetWorldPosition();
			m_vReturnPosition = pOwner->GetWorldPosition();
			m_vReturnPosition.y = pOwner->GetBottomHeight();
			pIljimaeItem->SetReturnPosition( m_vReturnPosition );
		}

		pOwner->SetRandomSeed( 0 );
		IORandom eRandom;
		DWORD dwSeed = pOwner->GetRandomSeed();

		D3DXVECTOR3 vNewPos = ioMath::VEC3_ZERO;
		Vector3Vec	vPosList;

		for( int i=0; i<8; ++i )
		{
			eRandom.SetRandomSeed( dwSeed );
			int iRand = eRandom.Random( 8 );

			float fCurAngle = 45.0f * iRand;
			D3DXQUATERNION qtRot;
			D3DXQuaternionRotationAxis( &qtRot, &ioMath::UNIT_Y, DEGtoRAD(fCurAngle) );

			D3DXVECTOR3 vTargetDir = pTarget->GetTargetRot() * D3DXVECTOR3( 0.0f, 0.0f, -FLOAT1 );
			vTargetDir = qtRot * vTargetDir;
			vNewPos = vTargetPos - ( vTargetDir * m_fMoveRange );

			ioPlayStage *pStage = pOwner->GetCreator();
			if( !pStage )
				vNewPos = pOwner->GetWorldPosition();
			else
			{
				float fMapHeight = pStage->GetMapHeightNoneEntity( vNewPos.x, vNewPos.z, vNewPos.y+3.0f );
				if( fMapHeight <= 0.0f )
					continue;
			}

			vPosList.push_back( vNewPos );
		}

		int iSize = vPosList.size();
		if( iSize == 0 )
		{
			D3DXVECTOR3 vTargetDir = pTarget->GetTargetRot() * D3DXVECTOR3( 0.0f, 0.0f, -FLOAT1 );
			vNewPos = pOwner->GetWorldPosition() - ( vTargetDir * m_fMoveRange );
		}
		else
		{
			DWORD dwReturnMarkEndTime = FRAMEGETTIME() + m_dwReturnMarkDuration;
			pOwner->SetReturnMark();
			if( pOwner->IsNeedProcess() )
			{
				ioItem *pItem = pOwner->GetEquipedItem(ES_WEAPON);
				ioIljimaeItem *pIljimaeItem = ToIljimaeItem( pItem );
				if( pIljimaeItem )
					pIljimaeItem->SetReturnMarkEndTime( dwReturnMarkEndTime );
			}
		}

		eRandom.SetRandomSeed( dwSeed );

		int iRandom = eRandom.Random( iSize );
		if( COMPARE( iRandom, 0, iSize ) )
		{
			vNewPos = vPosList[iRandom];
		}

		D3DXVECTOR3 vDir = vTargetPos - vNewPos;
		vDir.y = 0.0f;
		vNewPos.y += m_fMoveHeightRange;
		D3DXVec3Normalize( &vDir, &vDir );

		D3DXQUATERNION qtLookRot;
		ioMath::CalcDirectionQuaternion( qtLookRot, -vDir );
		pOwner->SetTargetRotAndMoveDirChange( qtLookRot );
		pOwner->SetWorldPosition( vNewPos );

		if( pOwner->IsNeedSendNetwork() )
		{
			SP2Packet kPacket( CUPK_EXTEND_JUMP );
			kPacket << pOwner->GetCharName();
			kPacket << (int)GetType();
			kPacket << pOwner->GetHP().m_fCurValue;
			kPacket << SSC_TELEPORT;
			kPacket << qtLookRot;
			kPacket << vNewPos;
			kPacket << (int)m_JumpState;
			pOwner->FillAutoTargetInfo( kPacket );
			P2PNetwork::SendToAllPlayingUser( kPacket );
		}

		// 텔레포트 종료 모션으로 전환
		pOwner->SetCurMoveSpeed( 0.0f );

		ioEntityGroup *pGrp = pOwner->GetGroup();
		pGrp->ClearAllActionAni( FLOAT100, true );
		pGrp->ClearAllLoopAni( FLOAT100, true );

		float fCurRate = FLOAT1;
		if( m_fTeleportEndMotionRate > 0.0f )
			fCurRate = m_fTeleportEndMotionRate;

		int iAniID = pGrp->GetAnimationIdx( m_TeleportEndMotion );
		pGrp->SetActionAni( iAniID, FLOAT100, FLOAT100, FLOAT1, FLOAT1/fCurRate );

		m_dwMotionEndTime = FRAMEGETTIME();
		m_dwMotionEndTime += pGrp->GetAnimationFullTime(iAniID) * fCurRate;

		pOwner->SetExtendAttackEndJumpInfo( iAniID, fCurRate, 0 );
	}
}

void ioIljimaeJump::SetLandingState( ioBaseChar *pOwner )
{
	m_bUseExtendJump = false;
	m_bUsedTeleport = false;
	m_bUsedAzazelAttack = false;
	m_bReserveJumpAttack = false;
	m_bUsedComboDash = false;
	m_JumpState = PJS_NONE;

	ClearAirJump();

	pOwner->SetUsedFlashJump( false );
}

void ioIljimaeJump::ChangeToPowerChargeJumpAttack( ioBaseChar *pOwner )
{
	ioEntityGroup *pGrp = pOwner->GetGroup();
	if( !pGrp )
		return;

	const AttackAttribute *pAttr = pOwner->GetAttackAttribute( AT_EXTEND_JUMP_ATTACK );
	if( !pAttr )
		return;

	if( !pOwner->IsEnableExtendJump() )
		return;

	DWORD dwCurTime = FRAMEGETTIME();
	if( pOwner->IsNeedProcess() && pOwner->IsCanRotateJumpAttack() )
	{
		ioUserKeyInput::DirKeyInput eDir = pOwner->CheckCurDirKey();
		if( eDir != ioUserKeyInput::DKI_NONE )
		{
			float fYawD = 180.0f + 45.0f * ( eDir - ioUserKeyInput::DKI_UP );
			fYawD = ioMath::ArrangeHead( fYawD );

			float fYawR = DEGtoRAD( fYawD );
			D3DXQUATERNION qtRotate;
			D3DXQuaternionRotationYawPitchRoll( &qtRotate, fYawR, 0.0f, 0.0f );

			pOwner->SetTargetRotToRotate( qtRotate, false, false );
			pOwner->SetInputDirection( pOwner->GetMoveDir() );
		}
	}

	pOwner->SetAutoTarget( ATT_JUMP );

	int iAniID = pGrp->GetAnimationIdx( pAttr->m_AttackAnimation );
	float fTimeRate = pAttr->m_fAttackAniRate;
	DWORD dwPreDelay = pAttr->m_dwPreDelay;

	if( fTimeRate <= 0.0f )
		fTimeRate = FLOAT1;

	pGrp->ClearAllActionAni( FLOAT100, true );
	pGrp->ClearAllLoopAni( FLOAT100, true );

	pGrp->SetActionAni( iAniID, FLOAT100, FLOAT100, FLOAT1, FLOAT1/fTimeRate, 0.0f, dwPreDelay, true );

	pOwner->RefreshFireTimeList( iAniID, pAttr->m_vWeaponInfoList, FTT_JUMP_ATTACK, fTimeRate, dwPreDelay );

	pOwner->IncreaseWeaponIndexBase();
	
	m_JumpState = PJS_EXTEND_ATTACK;
	m_bUsedChargeAttack = true;
	m_dwChargingStartTime = 0;

	// For AirJumpPower
	DWORD dwStartTime = dwCurTime + dwPreDelay;

	SetAirJump( pOwner, iAniID, fTimeRate );

	DWORD dwTrackingTime = dwCurTime;
	dwTrackingTime += pGrp->GetAnimationEventTime_e( iAniID, "tracking_e" ) * fTimeRate;

	m_dwChargeAttackEndTime = dwCurTime + pGrp->GetAnimationFullTime( iAniID ) * fTimeRate;

	pOwner->SetAutoTargetTrackingEndTime( dwTrackingTime );

	pOwner->CheckCharColSkipTime( iAniID, fTimeRate, dwPreDelay );
	
	D3DXVECTOR3 vDir = pOwner->GetTargetRot() * D3DXVECTOR3( 0.0f, 0.0f, -FLOAT1 );
	vDir.y = 0.0f;
	D3DXVec3Normalize( &vDir, &vDir );

	D3DXQUATERNION qtRot;
	ioMath::CalcDirectionQuaternion( qtRot, -vDir );
	pOwner->SetTargetRotAndMoveDirChange( qtRot, false );
	pOwner->SetWorldOrientation( qtRot );
	pOwner->SetJumpPower( m_fChargeJumpPower );
	pOwner->NotifyExtendJumpUse();

	if( pOwner->IsNeedSendNetwork() )
	{
		SP2Packet kPacket( CUPK_EXTEND_JUMP );
		kPacket << pOwner->GetCharName();
		kPacket << (int)GetType();
		kPacket << pOwner->GetHP().m_fCurValue;
		kPacket << SSC_EXTEND_ATTACK;
		kPacket << pOwner->GetTargetRot();
		pOwner->FillAutoTargetInfo( kPacket );
		P2PNetwork::SendToAllPlayingUser( kPacket );
	}
}

void ioIljimaeJump::SetNightmareState( ioBaseChar *pOwner )
{
	if( !pOwner )
		return;

	m_JumpState = PJS_NONE;
	m_bUsedNightmare = true;
	m_dwChargingStartTime = 0;

	pOwner->SetCurJumpPowerAmt( 0.0f );
	pOwner->SetForcePowerAmt( 0.0f );
	pOwner->SetGravityAmt( 0.0f );
	bool bFly = pOwner->SetNightmareFlyState();

	if( bFly && pOwner->IsNeedSendNetwork() )
	{
		SP2Packet kPacket( CUPK_EXTEND_JUMP );
		kPacket << pOwner->GetCharName();
		kPacket << (int)GetType();
		kPacket << pOwner->GetHP().m_fCurValue;
		kPacket << SSC_FLY;
		kPacket << pOwner->GetTargetRot();
		P2PNetwork::SendToAllPlayingUser( kPacket );
	}
}

void ioIljimaeJump::SetAzazelPushState( ioBaseChar *pOwner )
{
	if( !pOwner )
		return;

	m_JumpState = PJS_NONE;
	m_dwChargingStartTime = 0;

	ioAttackableItem *pItem = pOwner->GetPriorityItem( SPT_ATTACK );
	ioAzazelItem *pAzazel = ToAzazelItem( pItem );
	if( !pAzazel )
		return;

	pAzazel->SetPushState( pOwner, true );
}

void ioIljimaeJump::SetAzazelTeleportState( ioBaseChar *pOwner )
{
	if( !pOwner )
		return;

	m_JumpState = PJS_NONE;
	m_dwChargingStartTime = 0;

	ioAttackableItem *pItem = pOwner->GetPriorityItem( SPT_ATTACK );
	ioAzazelItem *pAzazel = ToAzazelItem( pItem );
	if( !pAzazel )
		return;

	pAzazel->SetTeleportState( pOwner );
}

void ioIljimaeJump::SetAzazelState( ioBaseChar *pOwner, bool bAim )
{
	if( !pOwner )
		return;

	m_JumpState = PJS_AZAZEL;
	m_bUsedAzazelAttack = true;
	m_dwChargingStartTime = 0;

	pOwner->SetCurJumpPowerAmt( 0.0f );
	pOwner->SetForcePowerAmt( 0.0f );
	pOwner->SetGravityAmt( 0.0f );
	pOwner->SetJumpAttackState();

	ioEntityGroup *pGrp = pOwner->GetGroup();
	if ( !pGrp )
		return;

	int iAniID = pGrp->GetAnimationIdx( m_AzazelExtendAttack.m_AttackAnimation );
	if ( iAniID == -1 )
		return;

	ioAzazelItem *pAzazel = ToAzazelItem( pOwner->GetPriorityItem( SPT_ATTACK ) );
	if( pAzazel )
		pAzazel->CheckDecreaseGauge();

	pOwner->SetAttackMoveEnable( false );

	pGrp->ClearAllLoopAni( FLOAT100, true );
	pGrp->ClearAllActionAni( FLOAT100, true );
	pOwner->ClearReservedSliding();

	DWORD dwPreDelay = m_AzazelExtendAttack.m_dwPreDelay;
	float fTimeRate = 1.0f;
	if ( m_AzazelExtendAttack.m_fAttackAniRate > 0.0f )
		fTimeRate = m_AzazelExtendAttack.m_fAttackAniRate;

	DWORD dwStartTime = FRAMEGETTIME() + dwPreDelay;
	m_dwMotionEndTime = dwStartTime + pGrp->GetAnimationFullTime( iAniID ) * fTimeRate + m_AzazelExtendAttack.m_dwEndDelay;
	m_dwAzazelFireTime = dwStartTime + pGrp->GetAnimationEventTime_e( iAniID, "fire_s" ) * fTimeRate;

	pOwner->CheckCharColSkipTime( iAniID, fTimeRate, dwPreDelay );
	pOwner->SetReservedSliding( m_AzazelExtendAttack.m_vForceInfoList, iAniID, fTimeRate, dwPreDelay );

	pGrp->SetActionAni( iAniID, FLOAT100, FLOAT100, FLOAT1, FLOAT1/fTimeRate, 0.0f, dwPreDelay );

	if( bAim )
	{
		ioUserKeyInput::DirKeyInput eNewDirKey = ioUserKeyInput::DKI_NONE;
		if ( pOwner->IsSettedDirection() )
		{
			eNewDirKey = pOwner->CheckCurDirKey();
			pOwner->SetTargetRotToDirKey( eNewDirKey );
			pOwner->SetWorldOrientation( pOwner->GetTargetRot() );
		}

		FD_AimTarget_Bow kFindPred( pOwner, m_fAzazelAngle, 0.f, m_fAzazelRange, false );
		ioBaseChar *pTarget = pOwner->GetCreator()->FindUser( kFindPred );
		if( pTarget )
			m_AzazelAimedTarget = pTarget->GetCharName();
		else
			m_AzazelAimedTarget.Clear();
	}
	if( pOwner->IsNeedSendNetwork() )
	{
		SP2Packet kPacket( CUPK_EXTEND_JUMP );
		kPacket << pOwner->GetCharName();
		kPacket << (int)GetType();
		kPacket << pOwner->GetHP().m_fCurValue;
		kPacket << SSC_AZAZEL;
		kPacket << pOwner->GetTargetRot();
		kPacket << m_AzazelAimedTarget;
		P2PNetwork::SendToAllPlayingUser( kPacket );
	}
}

void ioIljimaeJump::SetAzazelAttackState( ioBaseChar *pOwner, bool bSendPacket, D3DXVECTOR3 vPos, D3DXVECTOR3 vDir )
{
	if( !pOwner )
		return;

	int iAttackSize = m_AzazelExtendAttack.m_vWeaponInfoList.size();
	if ( iAttackSize <= 0 )
		return;

	ioPlayStage *pStage = pOwner->GetCreator();
	if( !pStage )
		return;

	DWORD dwWeaponBaseIndex = pOwner->IncreaseWeaponIndexBase();
	if( bSendPacket )
	{
		vPos = pOwner->GetMidPositionByRate();
		vDir = pOwner->GetTargetRot() * D3DXVECTOR3( 0.0f, 0.0f, -FLOAT1 );

		ioBaseChar* pTarget = pStage->GetBaseChar( m_AzazelAimedTarget );
		if( pTarget )
		{
			vDir = pTarget->GetWorldPosition() - pOwner->GetWorldPosition();
			vPos += m_vAzazelAttackOffset;
		}
	}

	D3DXVec3Normalize( &vDir, &vDir );	
	m_AzazelAimedTarget.Clear();

	for ( int i = 0; i<iAttackSize ; ++i )
	{
		WeaponInfo sWeaponInfo = m_AzazelExtendAttack.m_vWeaponInfoList[i];

		FireTime kFireTime;
		kFireTime.dwStart = FRAMEGETTIME();

		kFireTime.iAttributeIdx = sWeaponInfo.m_iWeaponIdx;
		kFireTime.iResistanceIdx = sWeaponInfo.m_iResistanceIdx;
		kFireTime.szWoundedAni = sWeaponInfo.m_WoundedAnimation;
		kFireTime.dwWoundedAniDuration = sWeaponInfo.m_dwWoundedDuration;
		kFireTime.bLoopWoundedAni = sWeaponInfo.m_bWoundedLoopAni;

		kFireTime.dwWeaponIdx = dwWeaponBaseIndex++;
		kFireTime.eFireTimeType = FTT_NORMAL_ATTACK;
			
		pOwner->ExtendFireExplicit( kFireTime, vPos, vDir, "" );
	}

	if( pOwner->IsNeedSendNetwork() )
	{
		SP2Packet kPacket( CUPK_EXTEND_JUMP );
		kPacket << pOwner->GetCharName();
		kPacket << (int)GetType();
		kPacket << pOwner->GetHP().m_fCurValue;
		kPacket << vPos;
		kPacket << vDir;
		P2PNetwork::SendToAllPlayingUser( kPacket );
	}
}

void ioIljimaeJump::ProcessAzazelAttack( ioBaseChar *pOwner )
{
	DWORD dwCurTime = FRAMEGETTIME();
	if( m_dwMotionEndTime < dwCurTime )
	{
		m_JumpState = PJS_NONE;
		pOwner->SetSKillEndJumpState( m_fAzazelEndJumpAmt, false, true, true, false, true );
		return;
	}

	pOwner->SetCurJumpPowerAmt( 0.0f );
	pOwner->SetForcePowerAmt( 0.0f );
	pOwner->SetGravityAmt( 0.0f );

	if( m_dwAzazelFireTime && m_dwAzazelFireTime < dwCurTime )
	{
		m_dwAzazelFireTime = 0;
		
		SetAzazelAttackState( pOwner, true );
	}
}

void ioIljimaeJump::SetAutomatonState( ioBaseChar *pOwner )
{
	if( !pOwner )
		return;

	m_JumpState = PJS_NONE;
	m_dwChargingStartTime = 0;

	ioAttackableItem *pItem = pOwner->GetPriorityItem( SPT_ATTACK );
	ioAutomatonItem *pAutomaton = ToAutomatonItem( pItem );
	if( !pAutomaton )
		return;

	pAutomaton->SetJumpSpecialState( pOwner, true );
}

void ioIljimaeJump::SetChangeOnGriffinState( ioBaseChar *pOwner )
{
	ioItem* pItem = pOwner->GetPriorityItem( SPT_JUMP );
	if( pItem )
	{
		switch( pItem->GetType() )
		{
		case ioItem::IT_WEAPON:
			{
				ioChangeOnItem* pChangeOn = ToChangeOnItem( pItem );
				if( pChangeOn )
				{
					if( pOwner->IsNeedSendNetwork() )
					{
						ioUserKeyInput::DirKeyInput eDir = pOwner->CheckCurDirKey();
						if( eDir != ioUserKeyInput::DKI_NONE )
						{
							float fYawD = 180.0f + 45.0f * ( eDir - ioUserKeyInput::DKI_UP );
							fYawD = ioMath::ArrangeHead( fYawD );

							float fYawR = DEGtoRAD( fYawD );
							D3DXQUATERNION qtRotate;
							D3DXQuaternionRotationYawPitchRoll( &qtRotate, fYawR, 0.0f, 0.0f );

							pOwner->SetTargetRotToRotate( qtRotate, true, true );
							pOwner->SetInputDirection( pOwner->GetMoveDir() );
						}

						SP2Packet kPacket( CUPK_EXTEND_JUMP );
						kPacket << pOwner->GetCharName();
						kPacket << (int)GetType();
						kPacket << pOwner->GetHP().m_fCurValue;
						kPacket << SSC_CHANGE_ON;
						kPacket << pOwner->GetTargetRot();
						P2PNetwork::SendToAllPlayingUser( kPacket );
					}

					pOwner->SetState( CS_CHANGE_ON_GRIFFIN_FLY );
					return;
				}
				else
				{
					SetComboJumpAttack( pOwner );
				}
			}
			break;
		case ioItem::IT_OBJECT:
			{
				ioObjectWereWolfItem *pObjectWere = ToObjectWereWolfItem( pItem );
				if( pObjectWere )
				{
					pObjectWere->ChangeToGriffin( pOwner );
				}
				else
				{
					SetComboJumpAttack( pOwner );
				}
			}
			break;
		default:
			SetComboJumpAttack( pOwner );
			break;
		}
	}
	else
	{
		SetComboJumpAttack( pOwner );
	}

	if( pOwner->IsNeedSendNetwork() )
	{
		SP2Packet kPacket( CUPK_EXTEND_JUMP );
		kPacket << pOwner->GetCharName();
		kPacket << (int)GetType();
		kPacket << pOwner->GetHP().m_fCurValue;
		kPacket << SSC_CHANGE_ON;
		kPacket << pOwner->GetTargetRot();
		P2PNetwork::SendToAllPlayingUser( kPacket );
	}
}

void ioIljimaeJump::SetHanzoState( ioBaseChar *pOwner )
{
	if( !pOwner )
		return;

	m_JumpState = PJS_NONE;
	m_dwChargingStartTime = 0;

	ioAttackableItem *pItem = pOwner->GetPriorityItem( SPT_ATTACK );
	ioHanzoItem* pHanzo = ToHanzoItem( pItem );
	if( !pHanzo )
		return;

	pHanzo->ChangeToJumpGatheringState( pOwner );
}

void ioIljimaeJump::SetMidnightState( ioBaseChar* pOwner )
{
	if( !pOwner )
		return;

	m_JumpState = PJS_NONE;
	m_dwChargingStartTime = 0;

	ioAttackableItem* pItem = pOwner->GetPriorityItem( SPT_ATTACK );
	ioMidnightItem* pMidnight = ToMidnightItem( pItem );
	if( !pMidnight )
		return;

	pMidnight->ChangeToJumpDashState( pOwner, pOwner->CheckCurDirKey() );
}

void ioIljimaeJump::SetTyrState( ioBaseChar* pOwner )
{
	if( !pOwner )
		return;

	m_JumpState = PJS_NONE;
	m_dwChargingStartTime = 0;

	ioAttackableItem* pItem = pOwner->GetPriorityItem( SPT_ATTACK );
	ioTyrItem* pTyrItem = ToTyrItem( pItem );
	if( !pTyrItem )
		return;

	pTyrItem->ChangeToJumpChargeState( pOwner );
}

bool ioIljimaeJump::IsChargeAttacked()
{
	return m_bUsedChargeAttack;
}

bool ioIljimaeJump::IsChargeEnd()
{
	DWORD dwCurTime = FRAMEGETTIME();
	if ( m_dwChargeAttackEndTime < dwCurTime )
		return true;

	m_fCurCharMoveSpeed = 0.0f;
	return false;
}

bool ioIljimaeJump::IsCanMoveState()
{
	if( m_JumpState == PJS_AZAZEL )
		return false;

	return true;
}

bool ioIljimaeJump::CheckDefenseRelease( ioBaseChar *pOwner )
{
	if( !m_bUsedDefense || pOwner->IsDefenseKeyDown() )
		return false;

	return true;
}

bool ioIljimaeJump::IsCanDefense( ioBaseChar *pOwner, float fHeightGap )
{
	if( !m_bEnableDefense )
		return false;

	if ( m_iMaxDefenseEnableCount > 0 && m_iMaxDefenseEnableCount <= m_iCurDefenseEnableCount )
	{
		return false;
	}

	if( pOwner->IsPrisonerMode() || pOwner->GetUsedJumpAttack() ||
		(pOwner->GetUsedBuffFlyJump() != BJUT_NONE) ||
		(pOwner->GetUsedBuffMultiFlyJump() != BJUT_NONE) ||
		(pOwner->GetUsedBuffJump() != BJUT_NONE) ||
		!pOwner->IsDefenseKeyDown() || m_bUsedDefense )
	{
		return false;
	}

	if( pOwner->IsJumpKeyDown() || pOwner->IsAttackKeyDown() )
		return false;

	if( fHeightGap < m_fDefenseEnableHeight )
		return false;

	if( m_dwDefenseReleaseTime > 0 && m_dwDefenseReleaseTime > FRAMEGETTIME() )
		return false;

	return true;
}

ioHashString ioIljimaeJump::GetJumpAniDefenseWounded()
{
	return m_JumpAniDefenseWounded;
}

ioHashString ioIljimaeJump::GetJumpAniDefenseCriticalWounded()
{
	return m_JumpAniDefenseCriticalWounded;
}

void ioIljimaeJump::SetDefense( ioBaseChar *pOwner )
{
	ioEntityGroup *pGrp = pOwner->GetGroup();
	pGrp->ClearAllActionAni( FLOAT100, true );
	pGrp->ClearAllLoopAni( FLOAT100, true );

	int iAniID = pGrp->GetAnimationIdx( m_JumpDefenseAnimation );
	float fAniRate = m_fDefenseAniRate;
	if( fAniRate <= 0.0f )
		fAniRate = FLOAT1;

	pGrp->SetActionAni( iAniID, FLOAT100, FLOAT100, FLOAT1, FLOAT1/fAniRate, 0.0f, 0.0f, true );

	DWORD dwKeyPressTime = FRAMEGETTIME();
	m_dwDefenseEnableTime  = dwKeyPressTime + pGrp->GetAnimationFullTime( iAniID ) * fAniRate;
	m_dwDefenseReleaseTime = m_dwDefenseEnableTime + m_dwDefenseDuration;

	m_bUsedDefense = true;
	m_bUseExtendJump = true;

	pOwner->SetEnableHide( false );
	pOwner->SetNoWoundState( false );
	pOwner->SetEnableChangeTeam( false );

	pOwner->SetChangeJumpMotion( false, "", "", "", 0.0f );
	pOwner->SetJumpDefense( dwKeyPressTime, m_dwDefenseEnableTime, m_dwDefenseReleaseTime );

	if( pOwner->IsNeedProcess() )
	{
		ioPowerChargeShieldItem *pPowerChargeShieldItem = ToPowerChargeShieldItem( pOwner->GetWeapon() );
		if ( pPowerChargeShieldItem && pPowerChargeShieldItem->GetNeedBullet() < pPowerChargeShieldItem->GetCurBullet() )
			pPowerChargeShieldItem->CreateShieldDummy( pOwner );
	}

	if ( m_fClearJumpPowerStartDefence )
	{
		pOwner->SetCurJumpPowerAmt( 0.0f );
		pOwner->SetGravityAmt( 0.0f );
	}

	m_iCurDefenseEnableCount++;

	if( pOwner->IsNeedSendNetwork() )
	{
		SP2Packet kPacket( CUPK_EXTEND_JUMP );
		kPacket << pOwner->GetCharName();
		kPacket << (int)GetType();
		kPacket << pOwner->GetHP().m_fCurValue;
		kPacket << SSC_DEFENSE;
		kPacket << dwKeyPressTime;
		kPacket << m_dwDefenseEnableTime;
		kPacket << m_dwDefenseReleaseTime;
		P2PNetwork::SendToAllPlayingUser( kPacket );
	}
}

void ioIljimaeJump::ReleaseDefense( ioBaseChar *pOwner )
{
	DWORD dwCurTime = FRAMEGETTIME();
	DWORD dwReleaseTime = dwCurTime + m_dwDefenseDuration;

	m_bUsedDefense = false;

	if( m_dwDefenseEnableTime < dwCurTime )
	{
		m_dwDefenseReleaseTime = dwReleaseTime;
		pOwner->ReleaseJumpDefense( dwReleaseTime );
	}
	else
	{
		pOwner->ReleaseJumpDefense( m_dwDefenseReleaseTime );
	}

	if ( pOwner->IsNeedProcess())
	{
		ioPowerChargeShieldItem *pPowerChargeShieldItem = ToPowerChargeShieldItem( pOwner->GetWeapon() );
		if ( pPowerChargeShieldItem )
			pPowerChargeShieldItem->DestoryShieldDummy( pOwner, false );
	}

	if( pOwner->IsNeedSendNetwork() )
	{
		SP2Packet kPacket( CUPK_EXTEND_JUMP );
		kPacket << pOwner->GetCharName();
		kPacket << (int)GetType();
		kPacket << pOwner->GetHP().m_fCurValue;
		kPacket << SSC_RELEASE_DEFENSE;				// defense
		P2PNetwork::SendToAllPlayingUser( kPacket );
	}
}

void ioIljimaeJump::ResetJumpState()
{
	m_iCurComboCnt = 0;
	m_dwEnableComboTime_S = 0;
	m_dwEnableComboTime_E = 0;

	m_bReserveJumpAttack = false;
	m_bUsedChargeAttack = false;
	m_dwChargeAttackEndTime = 0;

	m_ComboState = CST_NONE;

	m_bUsedDoubleJump = false;
	m_iCurJumpCnt = 1;
	m_iJumpDashAddAttackIndex = 0;

	m_dwChargingStartTime = 0;

	m_dwMotionEndTime = 0;

	m_bUsedDefense = false;
	m_dwDefenseEnableTime = 0;
	m_dwDefenseReleaseTime = 0;
	m_iCurDefenseEnableCount = 0;
}