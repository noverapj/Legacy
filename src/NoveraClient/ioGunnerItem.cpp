

#include "stdafx.h"

#include "ioGunnerItem.h"
#include "ioBaseChar.h"


#include "WeaponDefine.h"

ioGunnerItem::ioGunnerItem()
{
	m_dwChargeStartTime = 0;
	m_iCurCombo = 0;
	m_GunnerState = HS_NONE;

	m_dwExplosionDuration = 0;

	m_iWoundedDropRate = 100;

	m_dwExplosionStartTime = 0;
	m_bWoundedDrop = false;
	m_bEquipedOnHand = false;
	m_bSetChargingMoveAni = false;

	m_dwEquipBombTime = 0;
	m_dwEquipEndTime = 0;

	m_fCurExtraGauge = 0.0f;
}

ioGunnerItem::ioGunnerItem( const ioGunnerItem &rhs )
	: ioWeaponItem( rhs ),
	m_EquipAnimation( rhs.m_EquipAnimation ),
	m_BranchDelayAnimation( rhs.m_BranchDelayAnimation ),
	m_vExtendAttributeList( rhs.m_vExtendAttributeList ),
	//m_BombEquipMesh( rhs.m_BombEquipMesh ),
	m_BombEquipMesh_Left( rhs.m_BombEquipMesh_Left ),
	m_BombEquipMesh_Up( rhs.m_BombEquipMesh_Up ),
	m_BombEquipMesh_Right( rhs.m_BombEquipMesh_Right ),
	m_BombEquipEffect( rhs.m_BombEquipEffect ),
	m_fEquipTimeRate( rhs.m_fEquipTimeRate ),
	m_ChargingMoveAnimation( rhs.m_ChargingMoveAnimation ),
	m_HalfChargeAttack( rhs.m_HalfChargeAttack ),
	m_dwHalfChargeTime( rhs.m_dwHalfChargeTime ),
	m_stHalfChargeBuffName( rhs.m_stHalfChargeBuffName ),
	m_stFullChargeBuffName( rhs.m_stFullChargeBuffName ),
	m_stFullChargeAni( rhs.m_stFullChargeAni ),
	m_fFullChargeAniRate( rhs.m_fFullChargeAniRate ),
	m_EquipAnimation_Left( rhs.m_EquipAnimation_Left ),
	m_EquipAnimation_Up( rhs.m_EquipAnimation_Up ),
	m_EquipAnimation_Right( rhs.m_EquipAnimation_Right ),
	m_NormalDownAttributeList( rhs.m_NormalDownAttributeList ),
	m_ChargeCmdInfoList( rhs.m_ChargeCmdInfoList ),
	m_dwInputMaxTime( rhs.m_dwInputMaxTime ),
	m_dwInputStartGapTime( rhs.m_dwInputStartGapTime ),
	m_dwInputGapTime( rhs.m_dwInputGapTime ),
	m_fIncreaseExtraGauge( rhs.m_fIncreaseExtraGauge ),
	m_fNeedGauge( rhs.m_fNeedGauge )
{
	m_dwExplosionDuration = rhs.m_dwExplosionDuration;

	m_iWoundedDropRate = rhs.m_iWoundedDropRate;

	m_iCurCombo = 0;

	m_dwExplosionStartTime = 0;
	m_bWoundedDrop = false;
	m_bEquipedOnHand = false;
	m_bSetChargingMoveAni = false;

	m_dwChargeStartTime = 0;

	m_GunnerState = HS_NONE;
	m_dwEquipBombTime = 0;
	m_dwEquipEndTime = 0;

	ClearCmdInfo();

	m_nSelectedBombIndex = 0;

	m_fGrapOffsetX = 0;
	m_fGrapOffsetY = 0;

	m_fCurExtraGauge = 0.0f;
}

ioGunnerItem::~ioGunnerItem()
{
}

void ioGunnerItem::LoadProperty( ioINILoader &rkLoader )
{
	ioWeaponItem::LoadProperty( rkLoader );

	char szBuf[MAX_PATH];

	rkLoader.LoadString( "bomb_equip_mesh_left", "",szBuf, MAX_PATH );
	m_BombEquipMesh_Left = szBuf;
	rkLoader.LoadString( "bomb_equip_mesh_up", "",szBuf, MAX_PATH );
	m_BombEquipMesh_Up = szBuf;
	rkLoader.LoadString( "bomb_equip_mesh_right", "",szBuf, MAX_PATH );
	m_BombEquipMesh_Right = szBuf;
	//m_BombEquipMesh = szBuf;
	rkLoader.LoadString( "bomb_equip_effect", "", szBuf, MAX_PATH );
	m_BombEquipEffect = szBuf;

	m_dwExplosionDuration = rkLoader.LoadInt( "explosion_duration", 5000 );

	m_iWoundedDropRate		= rkLoader.LoadInt( "wounded_drop_rate", 100 );

	int iExtendCnt = rkLoader.LoadInt( "extend_attack_cnt", 0 );
	m_vExtendAttributeList.clear();
	m_vExtendAttributeList.reserve( iExtendCnt );
	for( int i=0 ; i<iExtendCnt ; i++ )
	{
		AttackAttribute kAttribute;
		wsprintf( szBuf, "extend_attack0%d", i+1 );
		LoadAttackAttribute( szBuf, kAttribute, rkLoader );
		m_vExtendAttributeList.push_back( kAttribute );
	}
	
	rkLoader.LoadString( "equip_animation", "", szBuf, MAX_PATH );
	m_EquipAnimation = szBuf;

	rkLoader.LoadString( "branch_delay_animation", "", szBuf, MAX_PATH );
	m_BranchDelayAnimation = szBuf;
	rkLoader.LoadString( "attack_move_ani", "", szBuf, MAX_PATH );
	m_ChargingMoveAnimation = szBuf;

	m_fEquipTimeRate = rkLoader.LoadFloat( "equip_time_rate", FLOAT1 );

	//////////////////////////////////////////////////////////////////////////
	int iMaxCombo = rkLoader.LoadInt( "max_combo", 1 );
	m_NormalDownAttributeList.clear();
	for(int i = 0;i < iMaxCombo;i++)
	{
		AttackAttribute kAttribute;

		wsprintf( szBuf, "down_normal_attack0%d", i+1 );
		LoadAttackAttribute( szBuf, kAttribute, rkLoader );		
		m_NormalDownAttributeList.push_back( kAttribute );
	}

	rkLoader.LoadString( "equip_animation_left", "", szBuf, MAX_PATH );
	m_EquipAnimation_Left = szBuf;
	rkLoader.LoadString( "equip_animation_up", "", szBuf, MAX_PATH );
	m_EquipAnimation_Up = szBuf;
	rkLoader.LoadString( "equip_animation_right", "", szBuf, MAX_PATH );
	m_EquipAnimation_Right = szBuf;

	rkLoader.LoadString( "half_charge_buff", "", szBuf, MAX_PATH );
	m_stHalfChargeBuffName = szBuf;
	m_dwHalfChargeTime = rkLoader.LoadInt("half_charge_time", 10000);
	wsprintf( szBuf, "half_charge_attack" );
	LoadAttackAttribute( szBuf, m_HalfChargeAttack, rkLoader );

	rkLoader.LoadString( "full_charge_buff", "", szBuf, MAX_PATH );
	m_stFullChargeBuffName = szBuf;

	rkLoader.LoadString( "full_charge_ani", "", szBuf, MAX_PATH );
	m_stFullChargeAni = szBuf;
	m_fFullChargeAniRate = rkLoader.LoadFloat( "full_charge_ani_rate", FLOAT1 );
	//////////////////////////////////////////////////////////////////////////
	LoadCmdInfoList( rkLoader );

	m_fNeedGauge = rkLoader.LoadFloat_e( "need_gauge", FLOAT100 );
	m_fIncreaseExtraGauge = rkLoader.LoadFloat_e( "increase_gauge", .0f );
}


void ioGunnerItem::LoadCmdInfoList( ioINILoader &rkLoader )
{
	char szBuf[MAX_PATH];
	char szKey[MAX_PATH];

	m_dwInputMaxTime = (DWORD)rkLoader.LoadInt( "input_max_time", 0 );
	m_dwInputGapTime = (DWORD)rkLoader.LoadInt( "input_gap_time", 0 );
	m_dwInputStartGapTime = (DWORD)rkLoader.LoadInt( "input_start_gap_time", 0 );

	int iCmdInoCnt = rkLoader.LoadInt( "max_cmd_info_cnt", 0 );
	if( iCmdInoCnt > 0 )
	{
		for( int i=0; i < iCmdInoCnt; ++i )
		{
			ChargeCmdInfo kCmdInfo;

			wsprintf( szKey, "cmd_info%d", i+1 );
			rkLoader.LoadString( szKey, "", szBuf, MAX_COMMAND_CNT );
			kCmdInfo.m_Cmd = szBuf;

			wsprintf( szKey, "cmd_info%d_use_type", i+1 );
			kCmdInfo.m_CmdUseType = (CmdUseType)rkLoader.LoadInt( szKey, CUT_NORMAL );

			wsprintf( szKey, "cmd_info%d_skill_slot", i+1 );
			kCmdInfo.m_iSkillSlot = rkLoader.LoadInt( szKey, 0 );

			wsprintf( szKey, "cmd_info%d_combo_cnt", i+1 );
			int iMaxCmdCombo = rkLoader.LoadInt( szKey, 0 );
			if( iMaxCmdCombo > 0 )
				kCmdInfo.m_SubInfoList.reserve( iMaxCmdCombo );

			for( int j=0; j < iMaxCmdCombo; ++j )
			{
				ChargeCmdSubInfo kSubInfo;

				wsprintf( szKey, "cmd_info%d_combo%d", i+1, j+1 );
				LoadAttackAttribute( szKey, kSubInfo.m_CmdAttack, rkLoader );

				wsprintf( szKey, "cmd_info%d_combo%d_use_d", i+1, j+1 );
				kSubInfo.m_bCmdAttack_D = rkLoader.LoadBool( szKey, false );

				wsprintf( szKey, "cmd_info%d_combo%d_use_s", i+1, j+1 );
				kSubInfo.m_bCmdAttack_S = rkLoader.LoadBool( szKey, false );

				wsprintf( szKey, "cmd_info%d_combo%d_d", i+1, j+1 );
				LoadAttackAttribute( szKey, kSubInfo.m_CmdAttack_D, rkLoader );

				wsprintf( szKey, "cmd_info%d_combo%d_s", i+1, j+1 );
				LoadAttackAttribute( szKey, kSubInfo.m_CmdAttack_S, rkLoader );

				//////////////////////////////////////////////////////////////////////////
				wsprintf( szKey, "cmd_info%d_combo%d_grap_attcker_ani", i+1, j+1 );
				rkLoader.LoadString( szKey, "", szBuf, MAX_PATH );
				kSubInfo.m_stGrappAttakerAni = szBuf;

				wsprintf( szKey, "cmd_info%d_combo%d_grap_attcker_ani_rate", i+1, j+1 );
				kSubInfo.m_fGrappAttakerAniRate = rkLoader.LoadFloat( szKey, FLOAT1 );

				wsprintf( szKey, "cmd_info%d_combo%d_grap_wounder_ani", i+1, j+1 );
				rkLoader.LoadString( szKey, "", szBuf, MAX_PATH );
				kSubInfo.m_stGrappWounerAni = szBuf;

				wsprintf( szKey, "cmd_info%d_combo%d_grap_wounder_ani_rate", i+1, j+1 );
				kSubInfo.m_fGrappWounerAniRate = rkLoader.LoadFloat( szKey, FLOAT1 );

				wsprintf( szKey, "cmd_info%d_combo%d_grap_offset_x", i+1, j+1 );
				kSubInfo.m_fGrappOffSetX = rkLoader.LoadFloat( szKey, 0.0f );

				wsprintf( szKey, "cmd_info%d_combo%d_grap_offset_y", i+1, j+1 );
				kSubInfo.m_fGrappOffSetY = rkLoader.LoadFloat( szKey, 0.0f );

				wsprintf( szKey, "cmd_info%d_combo%d_id", i+1, j+1 );
				kSubInfo.m_nID = rkLoader.LoadInt( szKey, -1);

				wsprintf( szKey, "cmd_info%d_combo%d_max_repeate_cnt", i+1, j+1 );
				kSubInfo.m_nMaxRepeatCnt = rkLoader.LoadInt( szKey, 1000 );

				wsprintf( szKey, "cmd_info%d_combo%d_need_gauge", i+1, j+1 );
				kSubInfo.m_fNeedGauge = rkLoader.LoadFloat( szKey, 0.0f );


				//////////////////////////////////////////////////////////////////////////


				kCmdInfo.m_SubInfoList.push_back( kSubInfo );
			}

			m_ChargeCmdInfoList.push_back( kCmdInfo );
		}
	}
}

ioItem* ioGunnerItem::Clone()
{
	return new ioGunnerItem( *this );
}

void ioGunnerItem::SetNormalAttack( ioBaseChar *pOwner, int iCurCombo )
{
	ClearCmdInfo();
	ClearCancelInfo();
	ClearKeyReserve();

	InitAniRotate();

	m_nCurCmd_ID = -1;
	m_nCurCmd_RepeateCnt = 0;

	if( !m_bEquipedOnHand )
	{
		int iMaxCombo = GetMaxCombo();
		if( COMPARE( iCurCombo, 0, iMaxCombo ) )
		{
			m_GunnerState = HS_CHARGING;
			m_dwChargeStartTime = FRAMEGETTIME();
			m_dwExplosionStartTime = 0;
			m_dwEquipBombTime = 0;
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
			m_bSetChargingMoveAni = false;
		}
	}
	else
	{
		int iComboSize = m_vExtendAttributeList.size();
		if( COMPARE( m_nSelectedBombIndex, 0, iComboSize ) )
		{
			pOwner->SetNormalAttackByAttribute( m_vExtendAttributeList[m_nSelectedBombIndex] );
			pOwner->SetState( CS_ATTACK );
			pOwner->SetChargingState( false );
			m_GunnerState = HS_BOMB_THROW_FIRE;
		}
		else if( iComboSize > 0 )
		{
			pOwner->SetNormalAttackByAttribute( m_vExtendAttributeList[0] );
			pOwner->SetState( CS_ATTACK );
			pOwner->SetChargingState( false );
			m_GunnerState = HS_BOMB_THROW_FIRE;
		}
	}
}

void ioGunnerItem::CheckNormalAttack( ioBaseChar *pOwner )
{
	DWORD dwCurTime = FRAMEGETTIME();

	switch( m_GunnerState )
	{
	case HS_CHARGING:
		OnCharging( pOwner );
		break;
	case HS_NORMAL_ATTACK:
		if( pOwner->IsNeedProcess() )
		{
			CheckCancelReserve( pOwner );

			if( ProcessCancel(pOwner) )
				return;

			if( CheckCmdInputState( pOwner ) )
				return;
		}

		ioAttackableItem::CheckNormalAttack( pOwner );
		break;
	case HS_CMD_ATTACK:
		break;
	case HS_HALF_GATHER_ATTACK:
		if( pOwner->IsNeedProcess() )
		{
			if( dwCurTime > m_dwMotionEndTime )
			{
				pOwner->SetState( CS_DELAY, false );

				if( pOwner->IsNeedSendNetwork() )
				{
					SP2Packet kPacket( CUPK_ITEM_GAUGE_CANCEL );
					FillItemGaugePacketHeader( pOwner, kPacket );
					kPacket << (int)HS_DELAY;
					kPacket << m_iCurCombo;
					P2PNetwork::SendToAllPlayingUser( kPacket );
				}
				return;
			}

			if( CheckCmdInputState( pOwner ) )
				return;
		}

		break;
	case HS_FULL_CHARGE_SELLECT:
		OnFullChargeState( pOwner );
		break;
	case HS_BOMB_EQUIP_START:
		OnBombEquiping( pOwner );
		break;
	case HS_BOMB_EQUIP:
		OnBombEquip( pOwner );
		break;
	case HS_BOMB_THROW_FIRE:
		if( pOwner->IsNeedProcess() )
		{
			if( CheckCmdInputState( pOwner ) )
				return;

			ioAttackableItem::CheckNormalAttack( pOwner );
		}
		break;
	}

	CheckChargingMoveState( pOwner );
}

void ioGunnerItem::OnCharging( ioBaseChar *pOwner )
{
	if( !pOwner->IsNeedProcess() )	
		return;

	DWORD dwCurTime = FRAMEGETTIME();
	if( pOwner->IsAttackKeyDown() )
	{
		//풀D
		if( m_dwChargeStartTime + m_dwChargeTime <= dwCurTime )
		{
			if( EnableGauge( m_fNeedGauge ) )
			{
				//ChangeToBombEquip( pOwner );
				ChangeFullChargeState( pOwner );
			}
			else
			{
				ChangeHalfChargeAttack( pOwner );
			}
		}
	}
	else
	{
		// 약D
		if( m_dwChargeStartTime + m_dwHalfChargeTime <= dwCurTime )
		{
			ChangeHalfChargeAttack( pOwner );
		}
		else//노말타
		{
			pOwner->SetChargingState( false );
			pOwner->SetAttackMoveEnable( false );
			m_bSetChargingMoveAni = false;
			ChangeToNormalAttack( pOwner );
		}
	}
}


bool ioGunnerItem::CheckCmdInputState( ioBaseChar *pOwner )
{
	if( !pOwner ) return false;
	if( !pOwner->IsNeedProcess() ) return false;

	if( m_dwInputStartTime == 0 || m_dwInputCheckTime == 0 )
		return false;


	if( CheckCmdInputTime() )
	{
		return ChangeToCmdSet( pOwner );
	}

	CheckCmdInput( pOwner );

	return false;
}


bool ioGunnerItem::CheckCmdInputTime()
{
	DWORD dwCurTime = FRAMEGETTIME();

	if( m_dwInputStartTime+m_dwInputMaxTime < dwCurTime )
	{
		return true;
	}

	if( m_dwInputCheckTime < dwCurTime )
	{
		return true;
	}

	return false;
}


bool ioGunnerItem::ChangeToCmdSet( ioBaseChar *pOwner )
{
	if( !pOwner )
		return false;

	DWORD dwCurTime = FRAMEGETTIME();
	ClearCancelInfo();

	if( m_ChargeCmdInfoList.empty() )
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
		m_CurChargeCmdInfo.Init();

		return false;
	}

	if( m_CurChargeCmdInfo.m_Cmd.IsEmpty() )
		return ChangeToNewCmdSet( pOwner );

	return ChangeToComboCmdSet( pOwner );
}


bool ioGunnerItem::CheckCmdInput( ioBaseChar *pOwner )
{
	if( !pOwner )
		return false;

	//
	if( m_iCmdInputType == CIT_ALL || m_iCmdInputType == CIT_ACTION )
	{
		if( m_bCmdInputD || m_bCmdInputS )
			return false;

		int iMaxCnt = m_CurChargeCmdInfo.m_SubInfoList.size();
		if( COMPARE( m_iCurCmdCombo, 0, iMaxCnt ) )
		{
			bool bEnableD = m_CurChargeCmdInfo.m_SubInfoList[m_iCurCmdCombo].m_bCmdAttack_D;
			bool bEnableS = m_CurChargeCmdInfo.m_SubInfoList[m_iCurCmdCombo].m_bCmdAttack_S;

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


bool ioGunnerItem::CheckCmdRepeate()
{
	int nNowId = m_CurChargeCmdInfo.m_SubInfoList[m_iCurCmdCombo].m_nID;
	int nNowRepeatteCnt = m_CurChargeCmdInfo.m_SubInfoList[m_iCurCmdCombo].m_nMaxRepeatCnt;

	if( nNowId == m_nCurCmd_ID )
	{
		m_nCurCmd_RepeateCnt++;
		if( m_nCurCmd_RepeateCnt > nNowRepeatteCnt )
			return false;
	}
	else
	{
		m_nCurCmd_ID = nNowId;
		m_nCurCmd_RepeateCnt = 1;
		
	}

	return true;
}


bool ioGunnerItem::ChangeToNewCmdSet( ioBaseChar *pOwner )
{
	if( !pOwner )
		return false;

	DWORD dwCurTime = FRAMEGETTIME();
	ClearCancelInfo();

	ChargeCmdInfoList::iterator iter = m_ChargeCmdInfoList.begin();
	while( iter != m_ChargeCmdInfoList.end() )
	{
		ChargeCmdInfo &rkInfo = (*iter);

		if( strcmp(rkInfo.m_Cmd.c_str(), m_CurCmdInfo) == 0 )
		{
			//if( rkInfo.m_CmdUseType == CUT_SKILL )
			//{
			//	if( !pOwner->IsBuffLimitSkill() && pOwner->SetUseSkill(rkInfo.m_iSkillSlot, SUT_NORMAL))
			//	{
			//		ClearState();
			//		m_GunnerState = HS_NONE;

			//		if( pOwner->IsNeedSendNetwork() )
			//		{
			//			SP2Packet kPacket( CUPK_ITEM_GAUGE_OK );
			//			FillItemGaugePacketHeader( pOwner, kPacket );
			//			//kPacket << (int)m_ChargeState;
			//			kPacket << m_iCurCombo;
			//			P2PNetwork::SendToAllPlayingUser( kPacket );
			//		}

			//		return true;
			//	}

			//	m_iCmdInputType = CIT_ALL;
			//	m_PreDirKey = ioUserKeyInput::DKI_NONE;

			//	m_iCurCmdIndex = 0;
			//	memset( m_CurCmdInfo, 0, MAX_COMMAND_CNT );

			//	m_bCmdInputD = false;
			//	m_bCmdInputS = false;

			//	m_dwInputStartTime = 0;
			//	m_dwInputCheckTime = 0;

			//	return false;
			//}
			//else
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
				m_CurChargeCmdInfo = rkInfo;

				if( !EnableGauge( rkSubInfoList[m_iCurCmdCombo].m_fNeedGauge ) )
				{
					return false;
				}

				//커맨드 반복 오버
				if( !CheckCmdRepeate() )
					break;

				pOwner->SetCmdAttackState();
				SetCmdAttack( pOwner, rkSubInfoList[m_iCurCmdCombo].m_CmdAttack, FLOAT1, FLOAT1, rkSubInfoList[m_iCurCmdCombo].m_fNeedGauge );

				if( pOwner->IsNeedSendNetwork() )
				{
					SP2Packet kPacket( CUPK_ITEM_GAUGE_OK );
					FillItemGaugePacketHeader( pOwner, kPacket );
					kPacket << ST_CMD_STATE;
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
	m_CurChargeCmdInfo.Init();

	return false;
}


bool ioGunnerItem::ChangeToComboCmdSet( ioBaseChar *pOwner )
{
	if( !pOwner )
		return false;

	DWORD dwCurTime = FRAMEGETTIME();
	ClearCancelInfo();

	if( m_CurChargeCmdInfo.m_Cmd.IsEmpty() )
	{
		return ChangeToNewCmdSet( pOwner );
	}

	if( !m_bCmdInputD && !m_bCmdInputS && strcmp(m_CurChargeCmdInfo.m_Cmd.c_str(), m_CurCmdInfo) != 0 )
	{
		return ChangeToNewCmdSet( pOwner );
	}

	ChargeCmdSubInfoList &rkSubInfoList = m_CurChargeCmdInfo.m_SubInfoList;
	if( !COMPARE( m_iCurCmdCombo, 0, (int)rkSubInfoList.size() ) )
	{
		return ChangeToNewCmdSet( pOwner );
	}

	if( !EnableGauge( rkSubInfoList[m_iCurCmdCombo].m_fNeedGauge ) )
	{
		return ChangeToNewCmdSet( pOwner );
	}

	if( rkSubInfoList[m_iCurCmdCombo].m_bCmdAttack_D && m_bCmdInputD && CheckCmdRepeate())
	{
		pOwner->SetCmdAttackState();
		SetCmdAttack( pOwner, rkSubInfoList[m_iCurCmdCombo].m_CmdAttack_D, FLOAT1, FLOAT1, rkSubInfoList[m_iCurCmdCombo].m_fNeedGauge );

		if( pOwner->IsNeedSendNetwork() )
		{
			SP2Packet kPacket( CUPK_ITEM_GAUGE_OK );
			FillItemGaugePacketHeader( pOwner, kPacket );
			kPacket << ST_CMD_STATE_D;
			kPacket << m_iCurCombo;
			kPacket << m_CurChargeCmdInfo.m_Cmd;
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
		m_CurChargeCmdInfo.Init();

		return true;
	}
	else if( rkSubInfoList[m_iCurCmdCombo].m_bCmdAttack_S && m_bCmdInputS && CheckCmdRepeate() )
	{
		pOwner->SetCmdAttackState();
		SetCmdAttack( pOwner, rkSubInfoList[m_iCurCmdCombo].m_CmdAttack_S );

		if( pOwner->IsNeedSendNetwork() )
		{
			SP2Packet kPacket( CUPK_ITEM_GAUGE_OK );
			FillItemGaugePacketHeader( pOwner, kPacket );
			kPacket << ST_CMD_STATE_S;
			kPacket << m_iCurCombo;
			kPacket << m_CurChargeCmdInfo.m_Cmd;
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
		m_CurChargeCmdInfo.Init();

		return true;
	}

	//
	m_iCurCmdCombo++;

	int iMaxCombo = rkSubInfoList.size();
	if( !COMPARE( m_iCurCmdCombo, 0, iMaxCombo ) )
		m_iCurCmdCombo = 0;

	if( !CheckCmdRepeate() )
		return false;

	pOwner->SetCmdAttackState();
	SetCmdAttack( pOwner, rkSubInfoList[m_iCurCmdCombo].m_CmdAttack, FLOAT1, FLOAT1, rkSubInfoList[m_iCurCmdCombo].m_fNeedGauge );

	if( pOwner->IsNeedSendNetwork() )
	{
		SP2Packet kPacket( CUPK_ITEM_GAUGE_OK );
		FillItemGaugePacketHeader( pOwner, kPacket );
		kPacket << ST_CMD_STATE;
		kPacket << m_iCurCombo;
		kPacket << m_CurChargeCmdInfo.m_Cmd;
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


void ioGunnerItem::SetCmdAttack( ioBaseChar *pOwner, const AttackAttribute &rkAttr, float fExtraAniRate/* =1.0f */, float fExtraForceRate/* =1.0f */, float fNeedGauge/* =0.0f */ )
{
	if( !pOwner ) return;

	ioEntityGroup *pGrp = pOwner->GetGroup();
	if( !pGrp ) return;

	DWORD dwCurTime = FRAMEGETTIME();

	InitAniRotate();
	pOwner->NormalAttackOver( false );
	pOwner->SetAutoTarget( ATT_NORMAL );

	int iAniID	     = pGrp->GetAnimationIdx( rkAttr.m_AttackAnimation );
	float fTimeRate  = rkAttr.m_fAttackAniRate * fExtraAniRate;
	DWORD dwPreDelay = rkAttr.m_dwPreDelay;
	DWORD dwEndDelay = rkAttr.m_dwEndDelay;

	pGrp->ClearAllLoopAni( FLOAT100, true );
	pGrp->ClearAllActionAni( FLOAT100, true );

	if( pOwner && !pOwner->IsActionStop() )
		pOwner->InitActionStopDelay( true );
	else
		pOwner->InitActionStopDelay( false );

	pGrp->SetActionAni( iAniID, FLOAT100, FLOAT100, FLOAT1, FLOAT1/fTimeRate, 0.0f, dwPreDelay, true );

	pOwner->RefreshFireTimeList( iAniID, rkAttr.m_vWeaponInfoList, FTT_NORMAL_ATTACK, fTimeRate, dwPreDelay );
	pOwner->IncreaseWeaponIndexBase();

	m_dwCmdAttackEndTime = dwCurTime + pGrp->GetAnimationFullTime( iAniID ) * fTimeRate + dwPreDelay + dwEndDelay;

	float fKeyReserve = pGrp->GetAnimationEventTime_e( iAniID, "key_reserve" ) * fTimeRate;
	if( fKeyReserve > 0.0f )
		m_dwKeyReserveTime = dwCurTime + fKeyReserve + dwPreDelay;

	pOwner->CheckCharColSkipTime( iAniID, fTimeRate, dwPreDelay );

	pOwner->SetReservedSliding( rkAttr.m_vForceInfoList, iAniID, fTimeRate, dwPreDelay, fExtraForceRate );

	DWORD dwTrackingTime = dwCurTime;
	dwTrackingTime += pGrp->GetAnimationEventTime_e( iAniID, "tracking_e" ) * fTimeRate + dwPreDelay;
	pOwner->SetAutoTargetTrackingEndTime( dwTrackingTime );

	pOwner->SetBaseAttackExtraAniJump( iAniID, fTimeRate, dwPreDelay );
	pOwner->SetExtendAttackEndJumpInfo( iAniID, fTimeRate, dwPreDelay );

	SetAniRotate( pOwner, iAniID, fTimeRate );

	pOwner->IncreaseWeaponIndexBase();

	DecreaseGauge( fNeedGauge );
}


void ioGunnerItem::SetCmdInputStart( int iType, ioBaseChar *pOwner )
{
	if( !pOwner )
		return;

	if( pOwner->IsChatModeState( false ) )
		return;

	DWORD dwCurTime = FRAMEGETTIME();

	m_PreDirKey = ioUserKeyInput::DKI_NONE;
	memset( m_CurCmdInfo, 0, MAX_COMMAND_CNT );

	m_bCmdInputD = false;
	m_bCmdInputS = false;

	m_iCurCmdIndex = 0;
	m_iCmdInputType = (CmdInputType)iType;

	m_dwInputStartTime = dwCurTime;
	m_dwInputCheckTime = dwCurTime+m_dwInputStartGapTime;
}

void ioGunnerItem::SetCmdInputEnd( ioBaseChar *pOwner )
{
	if( !pOwner ) return;
	if( !pOwner->IsNeedProcess() ) return;

	if( m_dwInputStartTime == 0 || m_dwInputCheckTime == 0 )
		return;

	ChangeToCmdSet( pOwner );
}


void ioGunnerItem::CheckCmdState( ioBaseChar *pOwner )
{
	if( !pOwner )
		return;

	DWORD dwCurTime = FRAMEGETTIME();

	pOwner->CheckExtraAniJump();

	if( pOwner->IsNeedProcess() )
	{
		if( pOwner->CheckExtendAttackEndJump() )
			return;
	}

	if( m_dwCmdAttackEndTime > 0 && m_dwCmdAttackEndTime < dwCurTime )
	{
		if( pOwner->IsNeedProcess() )
		{
			ProcessReserveKeyInput( pOwner );
		}
		else
		{
			ClearState();
			pOwner->SetState( CS_DELAY );
		}

		return;
	}
	else
	{
		CheckAniRotate( pOwner );

		if( pOwner->IsNeedProcess() )
		{
			if( m_dwKeyReserveTime > 0 && m_dwKeyReserveTime < dwCurTime )
				CheckReserve( pOwner );

			if( CheckCmdInputState( pOwner ) )
				return;
		}
	}
}

void ioGunnerItem::ChangeHalfChargeAttack( ioBaseChar* pOwner )
{
	if( !pOwner )
		return;

	DWORD dwCurTime = FRAMEGETTIME();

	pOwner->SetAutoTarget( ATT_NORMAL );

	int iAniID = -1;
	float fTimeRate = FLOAT1;
	DWORD dwPreDelay = 0;
	ioEntityGroup *pGrp = pOwner->GetGroup();


	pOwner->SetNormalAttackByAttribute( m_HalfChargeAttack, true, m_HalfChargeAttack.m_fAttackAniRate );

	iAniID = pGrp->GetAnimationIdx( m_HalfChargeAttack.m_AttackAnimation );
	fTimeRate = m_HalfChargeAttack.m_fAttackAniRate;
	dwPreDelay = m_HalfChargeAttack.m_dwPreDelay;

	pOwner->SetBaseAttackExtraAniJump( iAniID, fTimeRate, dwPreDelay );

	m_GunnerState = HS_HALF_GATHER_ATTACK;

	m_dwHalfChargeAttackStartTime = dwCurTime;

	m_dwMotionEndTime = dwCurTime + pGrp->GetAnimationFullTime( iAniID ) * fTimeRate;

	pOwner->RemoveBuff( m_stHalfChargeBuffName );

	pOwner->AddNewBuff( m_stHalfChargeBuffName, pOwner->GetCharName(), GetName(), NULL );


	if( pOwner->IsNeedSendNetwork() )
	{
		SP2Packet kPacket( CUPK_ITEM_GAUGE_CANCEL );
		FillItemGaugePacketHeader( pOwner, kPacket );
		kPacket << (int)m_GunnerState;
		kPacket << m_iCurCombo;
		P2PNetwork::SendToAllPlayingUser( kPacket );
	}
}


void ioGunnerItem::ChangeFullChargeState( ioBaseChar* pOwner )
{
	ioEntityGroup *pGrp = pOwner->GetGroup();
	pGrp->ClearAllActionAni( FLOAT100, true );
	pGrp->ClearAllLoopAni( FLOAT100, true );

	m_GunnerState = HS_FULL_CHARGE_SELLECT;

	if( pOwner->IsNeedProcess() )
	{
		pOwner->RemoveBuff( m_stFullChargeBuffName );
		pOwner->AddNewBuff( m_stFullChargeBuffName, pOwner->GetCharName(), GetName(), NULL );
	}

	pGrp->SetLoopAni( m_stFullChargeAni, FLOAT100, FLOAT1, FLOAT1/m_fFullChargeAniRate );

	pOwner->SetAttackMoveEnable( false );
	//SendPacket
	if( pOwner->IsNeedSendNetwork() )
	{
		SP2Packet kPacket( CUPK_ITEM_GAUGE_CANCEL );
		FillItemGaugePacketHeader( pOwner, kPacket );
		kPacket << (int)m_GunnerState;
		kPacket << m_iCurCombo;
		P2PNetwork::SendToAllPlayingUser( kPacket );
	}

	DecreaseGauge( m_fNeedGauge );
}


void ioGunnerItem::OnFullChargeState( ioBaseChar* pOwner )
{
	if( !pOwner->IsNeedProcess() )
		return;

	if( !pOwner->IsAttackKeyDown() )
	{
		ChangeToBombEquip( pOwner, 1 );
		return;
	}

	if( pOwner->IsSettedDirection() )
	{
		ioUserKeyInput::DirKeyInput eDir = pOwner->CheckCurDirKey();
		switch( eDir )
		{
		case ioUserKeyInput::DKI_LEFT:
			ChangeToBombEquip( pOwner, 0 );
			break;
		case ioUserKeyInput::DKI_UP:
			ChangeToBombEquip( pOwner, 1 );
			break;
		case ioUserKeyInput::DKI_RIGHT:
			ChangeToBombEquip( pOwner, 2 );
			break;
		}
	}
}


void ioGunnerItem::OnBombEquiping( ioBaseChar *pOwner )
{
	if( !pOwner->IsNeedProcess() )	return;

	DWORD dwCurTime = FRAMEGETTIME();
	if( m_dwEquipBombTime > 0 && m_dwEquipBombTime < dwCurTime )
	{
		ChangeToBombEquiped( pOwner );
		m_dwEquipBombTime = 0;
	}
}

void ioGunnerItem::OnBombEquip( ioBaseChar *pOwner )
{
	if( !pOwner->IsNeedProcess() )	return;

	DWORD dwCurTime = FRAMEGETTIME();

	if( m_dwEquipEndTime > 0 && m_dwEquipEndTime < dwCurTime )
	{
		ChangeToBombEquipEnd( pOwner );
	}
}

void ioGunnerItem::ChangeToBombEquip( ioBaseChar *pOwner, int nBomb )
{
	DWORD dwCurTime = FRAMEGETTIME();
	ioEntityGroup *pGrp = pOwner->GetGroup();

	pOwner->StopAutoTargetTracking();

	pGrp->ClearAllActionAni( FLOAT100, true );
	pGrp->ClearAllLoopAni( FLOAT100, true );

	m_nSelectedBombIndex = nBomb;

	ioHashString tmpEqupAnimation;
	switch( m_nSelectedBombIndex )
	{
	case 0:
		tmpEqupAnimation = m_EquipAnimation_Left;
		break;
	case 1:
		tmpEqupAnimation = m_EquipAnimation_Up;
		break;
	case 2:
		tmpEqupAnimation = m_EquipAnimation_Right;
		break;
	default: tmpEqupAnimation = m_EquipAnimation; 
		break;
	}

	int iAniID = -1;
	iAniID = pGrp->GetAnimationIdx( tmpEqupAnimation );
	if( pOwner->IsNeedProcess() )
	{
		pGrp->SetLoopAni( iAniID, FLOAT100, FLOAT1, FLOAT1/m_fEquipTimeRate );

		m_dwEquipEndTime = pGrp->GetAnimationFullTime( iAniID ) * m_fEquipTimeRate;
		m_dwEquipEndTime -= 100;
		m_dwEquipEndTime += dwCurTime;


		m_dwEquipBombTime = pGrp->GetAnimationEventTime_e( iAniID, "fire_s" ) * m_fEquipTimeRate;
		m_dwEquipBombTime += dwCurTime;
	}
	else
	{
		pGrp->SetLoopAni( iAniID, FLOAT100, FLOAT1, FLOAT1/m_fEquipTimeRate );
		m_dwEquipBombTime = 0;
	}

	m_GunnerState = HS_BOMB_EQUIP_START;
	//pOwner->SetAttackMoveEnable( true );
	m_bEquipedOnHand = false;

	pOwner->RemoveBuff( m_stFullChargeBuffName );

	if( pOwner->IsNeedSendNetwork() )
	{
		SP2Packet kPacket( CUPK_ITEM_GAUGE_CANCEL );
		FillItemGaugePacketHeader( pOwner, kPacket );
		kPacket << (int)m_GunnerState;
		kPacket << m_iCurCombo;
		kPacket << m_bEquipedOnHand;
		kPacket << nBomb;
		P2PNetwork::SendToAllPlayingUser( kPacket );
	}
}

void ioGunnerItem::ChangeToBombEquiped( ioBaseChar *pOwner )
{
	DWORD dwCurTime = FRAMEGETTIME();

	AddBombEntity( pOwner );

	m_bEquipedOnHand = true;

	m_GunnerState = HS_BOMB_EQUIP;

	if( pOwner->IsNeedSendNetwork() )
	{
		SP2Packet kPacket( CUPK_ITEM_GAUGE_CANCEL );
		FillItemGaugePacketHeader( pOwner, kPacket );
		kPacket << (int)m_GunnerState;
		kPacket << m_iCurCombo;
		kPacket << m_bEquipedOnHand;
		P2PNetwork::SendToAllPlayingUser( kPacket );
	}
}

void ioGunnerItem::ChangeToBombEquipEnd( ioBaseChar *pOwner )
{
	m_GunnerState = HS_NONE;
	m_dwEquipEndTime = 0;
	m_dwExplosionStartTime = FRAMEGETTIME();

	ioEntityGroup *pGrp = pOwner->GetGroup();
	pGrp->ClearAllLoopAni( FLOAT1, true );

	pOwner->SetAttackMoveEnable( false );
	m_bSetChargingMoveAni = false;
	pOwner->SetState( CS_DELAY );

	//SetVisible( pOwner, false );

	if( pOwner->IsNeedSendNetwork() )
	{
		SP2Packet kPacket( CUPK_ITEM_GAUGE_CANCEL );
		FillItemGaugePacketHeader( pOwner, kPacket );
		kPacket << (int)m_GunnerState;
		kPacket << m_iCurCombo;
		P2PNetwork::SendToAllPlayingUser( kPacket );
	}
}

void ioGunnerItem::ChangeToNormalAttack( ioBaseChar *pOwner )
{
	DWORD dwCurTime = FRAMEGETTIME();

	ClearCancelInfo();

	ioHashString szTarget;
	ioBaseChar* pTarget = NULL;
	bool bStandeNormal = true;
	if( pOwner->CheckAimedTargetByBow( m_fAutoTargetMaxAngle, 0, m_fAutoTargetRange, szTarget, true ) )
	{
		pTarget = pOwner->GetBaseChar( szTarget );

		/*if( pTarget )
		{
			;
		}*/
		if( pTarget && (pTarget->GetState() == CS_BLOW_WOUNDED || pTarget->GetState() == CS_BOUND_BLOW_WOUNDED) )
		{
			bStandeNormal = false;
			pOwner->SetTargetRotToTargetPos( pTarget, true );
		}
	}

	if( bStandeNormal )
	{
		//LOG.PrintConsoleLog(0, "Stand_Attack");
		const AttackAttribute *pAttribute = m_FormalAttack.GetAttribute( AT_NORMAL, m_iCurCombo );
		if( pAttribute )
		{
			pOwner->SetNormalAttackByAttribute( *pAttribute );

			m_szCurAni = pAttribute->m_AttackAnimation;
			m_fCurAniRate = pAttribute->m_fAttackAniRate;

			m_dwInputCancelCheckTime = dwCurTime;
			m_dwCancelCheckTime = dwCurTime;
		}
	}
	else
	{
		//LOG.PrintConsoleLog(0, "Down_Attack");
		const AttackAttribute *pAttribute = &m_NormalDownAttributeList[m_iCurCombo];
		if( pAttribute )
		{
			pOwner->SetNormalAttackByAttribute( *pAttribute );

			m_szCurAni = pAttribute->m_AttackAnimation;
			m_fCurAniRate = pAttribute->m_fAttackAniRate;

			m_dwInputCancelCheckTime = dwCurTime;
			m_dwCancelCheckTime = dwCurTime;
		}
	}

	pOwner->ApplyNormalAttackResult( m_iCurCombo );
	m_GunnerState = HS_NORMAL_ATTACK;

	if( pOwner->IsNeedSendNetwork() )
	{
		SP2Packet kPacket( CUPK_ITEM_GAUGE_CANCEL );
		FillItemGaugePacketHeader( pOwner, kPacket );
		kPacket << (int)m_GunnerState;
		kPacket << m_iCurCombo;
		P2PNetwork::SendToAllPlayingUser( kPacket );
	}
}

void ioGunnerItem::SetExtraInfoToWeapon( ioWeapon *pWeapon, bool bJumpAttack )
{
	ioAttackableItem::SetExtraInfoToWeapon( pWeapon, bJumpAttack );
	ioThrowBombWeapon *pBomb = ToThrowBombWeapon( pWeapon );
	ioBoundBombWeapon *pBound = ToBoundBombWeapon( pWeapon );

	//
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

	if( bJumpAttack && m_bEquipedOnHand )
	{
		if( pBomb )
		{
			pBomb->SetJumpThrow();
			g_TutorialMgr.ActionBombFire();
		}
		else if( pBound )
		{
			pBound->SetJumpThrow();
		}
	}
	else if( m_bWoundedDrop )
	{
		if( pBomb )
		{
			pBomb->SetBombDrop();
		}
		else if( pBound )
		{
			pBound->SetBombDrop();
		}

		pWeapon->SetMoveDir( m_vDropMoveDir );
		m_bWoundedDrop = false;
	}
	else
	{
		if( pBomb )
		{
			g_TutorialMgr.ActionBombFire();
		}
	}

	RemoveBombEntity( pWeapon->GetOwner() );
}

void ioGunnerItem::OnEndNormalAttack( ioBaseChar *pOwner )
{
	ioWeaponItem::OnEndNormalAttack( pOwner );

	pOwner->RemoveBuff( m_stFullChargeBuffName );

	if( m_GunnerState == HS_BOMB_THROW_FIRE )
	{
		RemoveBombEntity( pOwner );
	}

	m_GunnerState = HS_NONE;
}


void ioGunnerItem::OnItemGaugeOK( ioBaseChar *pOwner, SP2Packet &rkPacket )
{
	if( !pOwner ) return;

	int iSyncType;
	rkPacket >> iSyncType;

	switch( iSyncType )
	{
	case ST_CMD_STATE:
		{
			rkPacket >> m_iCurCombo;

			ioHashString szCmdInfo;
			int iCmdCombo;
			rkPacket >> szCmdInfo >> iCmdCombo;

			ApplyCmdSet( pOwner, szCmdInfo, iCmdCombo );
		}
		break;
	case ST_CMD_STATE_D:
		{
			rkPacket >> m_iCurCombo;

			ioHashString szCmdInfo;
			int iCmdCombo;
			rkPacket >> szCmdInfo >> iCmdCombo;

			ApplyCmdSetD( pOwner, szCmdInfo, iCmdCombo );
		}
		break;
	case ST_CMD_STATE_S:
		{
			rkPacket >> m_iCurCombo;

			ioHashString szCmdInfo;
			int iCmdCombo;
			rkPacket >> szCmdInfo >> iCmdCombo;

			ApplyCmdSetS( pOwner, szCmdInfo, iCmdCombo );
		}
		break;
	}
}


void ioGunnerItem::OnItemGaugeCancel( ioBaseChar *pOwner, SP2Packet &rkPacket )
{
	int iBomb;
	int iGunnerState;
	rkPacket >> iGunnerState;
	rkPacket >> m_iCurCombo;

	bool bEquipHand = false;

	switch( iGunnerState )
	{
	case HS_NORMAL_ATTACK:
		ChangeToNormalAttack( pOwner );
		break;
	case HS_HALF_GATHER_ATTACK:
		ChangeHalfChargeAttack( pOwner );
		break;
	case HS_FULL_CHARGE_SELLECT:
		ChangeFullChargeState( pOwner );
		break;
	case HS_BOMB_EQUIP_START:
		rkPacket >> m_bEquipedOnHand;
		rkPacket >> iBomb;
		ChangeToBombEquip( pOwner, iBomb );
		break;
	case HS_BOMB_EQUIP:
		ChangeToBombEquiped( pOwner );
		break;
	case HS_NONE:
		ChangeToBombEquipEnd( pOwner );
		break;
	case HS_DELAY:
		pOwner->SetState( CS_DELAY, false );
		break;
	case HS_CMD_ATTACK:
		{
			D3DXVECTOR3 vPos;
			D3DXQUATERNION qtRot;
			rkPacket >> m_iCurCmdCombo;
			rkPacket >> vPos;
			rkPacket >> qtRot;
			rkPacket >> m_GrapplingTargetName;

			pOwner->SetWorldPosition( vPos );
			pOwner->SetTargetRot( qtRot );

			SetItemGrapplingState( pOwner, m_GrapplingTargetName );
			break;
		}
	}
}

ioWeaponItem::WeaponSubType ioGunnerItem::GetSubType() const
{
	return WST_GUNNER;
}


void ioGunnerItem::ApplyCmdSet( ioBaseChar *pOwner, ioHashString szCmdInfo, int iCmdCombo )
{
	if( !pOwner ) return;

	ChargeCmdInfoList::iterator iter = m_ChargeCmdInfoList.begin();
	while( iter != m_ChargeCmdInfoList.end() )
	{
		ChargeCmdInfo &rkInfo = (*iter);
		if( strcmp(rkInfo.m_Cmd.c_str(), szCmdInfo.c_str()) == 0 )
		{
			if( rkInfo.m_CmdUseType == CUT_SKILL )
				return;

			ChargeCmdSubInfoList &rkSubInfoList = rkInfo.m_SubInfoList;
			if( !COMPARE( iCmdCombo, 0, (int)rkSubInfoList.size() ) )
				return;

			pOwner->SetCmdAttackState();

			m_CurChargeCmdInfo = rkInfo;

			SetCmdAttack( pOwner, rkSubInfoList[iCmdCombo].m_CmdAttack, FLOAT1, FLOAT1, rkSubInfoList[iCmdCombo].m_fNeedGauge );
			return;
		}

		++iter;
	}
}


void ioGunnerItem::ApplyCmdSetD( ioBaseChar *pOwner, ioHashString szCmdInfo, int iCmdCombo )
{
	if( !pOwner ) return;

	ChargeCmdInfoList::iterator iter = m_ChargeCmdInfoList.begin();
	while( iter != m_ChargeCmdInfoList.end() )
	{
		ChargeCmdInfo &rkInfo = (*iter);
		if( strcmp(rkInfo.m_Cmd.c_str(), szCmdInfo.c_str()) == 0 )
		{
			if( rkInfo.m_CmdUseType == CUT_SKILL )
				return;

			ChargeCmdSubInfoList &rkSubInfoList = rkInfo.m_SubInfoList;
			if( !COMPARE( iCmdCombo, 0, (int)rkSubInfoList.size() ) )
				return;

			if( !rkSubInfoList[iCmdCombo].m_bCmdAttack_D )
				return;

			m_CurChargeCmdInfo = rkInfo;

			pOwner->SetCmdAttackState();
			SetCmdAttack( pOwner, rkSubInfoList[iCmdCombo].m_CmdAttack_D );
			return;
		}

		++iter;
	}
}


void ioGunnerItem::ApplyCmdSetS( ioBaseChar *pOwner, ioHashString szCmdInfo, int iCmdCombo )
{
	if( !pOwner ) return;

	ChargeCmdInfoList::iterator iter = m_ChargeCmdInfoList.begin();
	while( iter != m_ChargeCmdInfoList.end() )
	{
		ChargeCmdInfo &rkInfo = (*iter);
		if( strcmp(rkInfo.m_Cmd.c_str(), szCmdInfo.c_str()) == 0 )
		{
			if( rkInfo.m_CmdUseType == CUT_SKILL )
				return;

			ChargeCmdSubInfoList &rkSubInfoList = rkInfo.m_SubInfoList;
			if( !COMPARE( iCmdCombo, 0, (int)rkSubInfoList.size() ) )
				return;

			if( !rkSubInfoList[iCmdCombo].m_bCmdAttack_S )
				return;

			m_CurChargeCmdInfo = rkInfo;

			pOwner->SetCmdAttackState();
			SetCmdAttack( pOwner, rkSubInfoList[iCmdCombo].m_CmdAttack_S );
			return;
		}

		++iter;
	}
}


bool ioGunnerItem::CheckExplosionOnHand( ioBaseChar *pOwner )
{
	if( m_dwExplosionStartTime == 0 )
		return false;

	if( m_bEquipedOnHand && GetRemainDuration() == 0 )
	{
		RemoveBombEntity( pOwner );
		return true;
	}

	return false;
}

const WeaponInfo& ioGunnerItem::GetExplosionAttributeIdx() const
{
	return m_vExtendAttributeList[m_nSelectedBombIndex].m_vWeaponInfoList.front();
}

DWORD ioGunnerItem::GetRemainDuration() const
{
	DWORD dwCurTime = FRAMEGETTIME();
	DWORD dwExplosionTime = m_dwExplosionStartTime + m_dwExplosionDuration;

	if( dwCurTime < dwExplosionTime )
		return dwExplosionTime - dwCurTime;

	return 0;
}

void ioGunnerItem::ExplosionOnHand( ioBaseChar *pOwner )
{
	pOwner->NormalAttackFireExplicit( GetExplosionAttributeIdx() );
	m_GunnerState = HS_NONE;
}

void ioGunnerItem::ChangeToBombOnDrop( const D3DXVECTOR3 &vMoveDir, ioBaseChar *pOwner )
{
	m_GunnerState = HS_NONE;
	m_bWoundedDrop = true;
	m_vDropMoveDir = vMoveDir;
}

void ioGunnerItem::AddBombEntity( ioBaseChar *pOwner )
{
	if( !pOwner )	return;

	if( m_nSelectedBombIndex == 0 )
	{
		if( !pOwner->HasEntity( m_BombEquipMesh_Left ) )
		{
			pOwner->AddEntity( m_BombEquipMesh_Left );
			pOwner->AttachEffect( m_BombEquipEffect );
		}
	}
	else if( m_nSelectedBombIndex == 1 )
	{
		if( !pOwner->HasEntity( m_BombEquipMesh_Up ) )
		{
			pOwner->AddEntity( m_BombEquipMesh_Up );
			pOwner->AttachEffect( m_BombEquipEffect );
		}
	}else if( m_nSelectedBombIndex == 2 )
	{
		if( !pOwner->HasEntity( m_BombEquipMesh_Right ) )
		{
			pOwner->AddEntity( m_BombEquipMesh_Right );
			pOwner->AttachEffect( m_BombEquipEffect );
		}
	}
	else 
	{
		if( !pOwner->HasEntity( m_BombEquipMesh_Up ) )
		{
			pOwner->AddEntity( m_BombEquipMesh_Up );
			pOwner->AttachEffect( m_BombEquipEffect );
		}
	}

	/*if( !pOwner->HasEntity( m_BombEquipMesh ) )
	{
		pOwner->AddEntity( m_BombEquipMesh );
		pOwner->AttachEffect( m_BombEquipEffect );
	}*/
}

void ioGunnerItem::RemoveBombEntity( ioBaseChar *pOwner )
{
	if( !pOwner )	return;

	while( pOwner->HasEntity( m_BombEquipMesh_Left ) )
	{
		pOwner->DestroyEntity( m_BombEquipMesh_Left );
	}

	while( pOwner->HasEntity( m_BombEquipMesh_Up ) )
	{
		pOwner->DestroyEntity( m_BombEquipMesh_Up );
	}

	while( pOwner->HasEntity( m_BombEquipMesh_Right ) )
	{
		pOwner->DestroyEntity( m_BombEquipMesh_Right );
	}

	m_bEquipedOnHand = false;

	pOwner->SetChargingState( false );
	pOwner->EndEffect( m_BombEquipEffect );
}

bool ioGunnerItem::CheckWoundedDropCondition()
{
	int iRand = rand() % 100 + 1;
	if( m_iWoundedDropRate >= iRand )
		return true;

	return false;
}

void ioGunnerItem::OnReleased( ioBaseChar *pOwner )
{
	ioWeaponItem::OnReleased( pOwner );

	m_GunnerState = HS_NONE;
	m_bWoundedDrop = false;

	RemoveBombEntity( pOwner );
}

void ioGunnerItem::FillNormalAttackExtraInfo( SP2Packet &rkPacket )
{
	ioAttackableItem::FillNormalAttackExtraInfo( rkPacket );
	rkPacket << m_bEquipedOnHand;
}

void ioGunnerItem::ApplyNormalAttackExtraInfo( SP2Packet &rkPacket )
{
	rkPacket >> m_bEquipedOnHand;
}

bool ioGunnerItem::CheckOnHand()
{
	return m_bEquipedOnHand;
}

bool ioGunnerItem::IsChargeAttackState( ioBaseChar *pOwner )
{
	if( pOwner->GetState() == CS_ATTACK )
	{
		if( m_GunnerState == HS_CHARGING ||
			m_GunnerState == HS_BOMB_EQUIP_START ||
			m_GunnerState == HS_BOMB_EQUIP )
			return true;
	}

	return false;
}

bool ioGunnerItem::IsEnableAttackDashCheck( ioBaseChar *pOwner, bool bCheck )
{
	if( !IsChargeDash() ) return false;

	if( pOwner->GetState() == CS_ATTACK )
	{
		if( m_GunnerState == HS_CHARGING ||
			m_GunnerState == HS_BOMB_EQUIP_START ||
			m_GunnerState == HS_BOMB_EQUIP )
			return true;
	}

	return false;
}

void ioGunnerItem::CheckChargingMoveState( ioBaseChar *pOwner )
{
	if( !pOwner->IsCanAttackMove() ) return;

	DWORD dwCurTime = FRAMEGETTIME();
	ioEntityGroup *pGrp = pOwner->GetGroup();
	int iAniID = 0;

	if( pOwner->IsSettedDirection() )
	{
		if( !m_bSetChargingMoveAni )
		{
			iAniID = pGrp->GetAnimationIdx(m_ChargingMoveAnimation);
			if( iAniID == -1 )
				return;

			DWORD dwPlayTime = 0;
			ioAnimation *pAni = pGrp->GetLoopInPlay( m_EquipAnimation );
			if( pAni )
			{
				dwPlayTime = pAni->GetCurTime();
			}

			pGrp->ClearAllActionAni( FLOAT100, true );
			pGrp->ClearAllLoopAni( FLOAT100, true );
			pGrp->SetLoopAniInMid( iAniID, dwPlayTime, FLOAT100, FLOAT1, FLOAT1/m_fEquipTimeRate );
			m_bSetChargingMoveAni = true;
		}
	}
	else
	{
		if( m_bSetChargingMoveAni )
		{
			iAniID = pGrp->GetAnimationIdx( m_EquipAnimation );
			if( iAniID == -1 )
				return;

			DWORD dwPlayTime = 0;
			ioAnimation *pAni = pGrp->GetLoopInPlay( m_ChargingMoveAnimation );
			if( pAni )
				dwPlayTime = pAni->GetCurTime();

			pGrp->ClearAllActionAni( FLOAT100, true );
			pGrp->ClearAllLoopAni( FLOAT100, true );
			pGrp->SetLoopAniInMid( iAniID, dwPlayTime, FLOAT100, FLOAT1, FLOAT1/m_fEquipTimeRate );
			m_bSetChargingMoveAni = false;
		}
	}
}


const ioHashString& ioGunnerItem::GetBombMeshName() const
{
	switch( m_nSelectedBombIndex )
	{
	case 0:
		return m_BombEquipMesh_Left;
		break;
	case 1:
		return m_BombEquipMesh_Up;
		break;
	case 2:
		return m_BombEquipMesh_Right;
		break;
	}

	return m_BombEquipMesh_Up;
}


void ioGunnerItem::ClearCmdInfo()
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

	m_iCurCmdCombo = 0;
	m_CurChargeCmdInfo.Init();

	m_nCurCmd_ID = -1;
	m_nCurCmd_RepeateCnt = 0;
}

void ioGunnerItem::ClearCmdTime()
{
	m_dwInputStartTime = 0;
}


void ioGunnerItem::ClearState()
{
	m_GunnerState = HS_NONE;
}

ioGunnerItem::GunnerState ioGunnerItem::GetGunnerState()
{
	return m_GunnerState;
}


D3DXVECTOR3 ioGunnerItem::GetGrapplingTargetPos( ioBaseChar *pOwner )
{
	//if( m_fGrpplingCharToCharGap > 0.0f )
	{
		D3DXVECTOR3 vPos = pOwner->GetMidPositionByRate( FLOAT05 );
		D3DXVECTOR3 vDir = pOwner->GetTargetRot() * D3DXVECTOR3( 0.0f, 0.0f, -FLOAT1 );
		D3DXVec3Normalize( &vDir, &vDir );


		/*float fX = 0.0f;
		float fY = 0.0f;
		if( !m_CurChargeCmdInfo.m_Cmd.IsEmpty())
		{
			fX = m_CurChargeCmdInfo.m_SubInfoList[m_iCurCmdCombo].m_fGrappOffSetX;
			fY = m_CurChargeCmdInfo.m_SubInfoList[m_iCurCmdCombo].m_fGrappOffSetY;
		}
		
		
		vPos += fX * vDir;
		vPos += D3DXVECTOR3( 0.0f, fY, 0.0f );*/

		/*float fX = 0.0f;
		float fY = 0.0f;
		if( !m_CurChargeCmdInfo.m_Cmd.IsEmpty())
		{
		fX = m_CurChargeCmdInfo.m_SubInfoList[m_iCurCmdCombo].m_fGrappOffSetX;
		fY = m_CurChargeCmdInfo.m_SubInfoList[m_iCurCmdCombo].m_fGrappOffSetY;
		}*/


		vPos += m_fGrapOffsetX * vDir;
		vPos += D3DXVECTOR3( 0.0f, m_fGrapOffsetY, 0.0f );

		return vPos;
	}

	return ioMath::VEC3_ZERO;
}


bool ioGunnerItem::SetItemGrapplingState( ioBaseChar *pOwner, const ioHashString &szTarget )
{
	if( !pOwner ) 
		return false;

	if( pOwner->IsNeedProcess() )
	{
		if( pOwner->GetState() != CS_CMD_STATE )
			return false;
	}

	ioEntityGroup *pGrp = pOwner->GetGroup();
	if( !pGrp ) 
		return false;

	m_GunnerState = HS_CMD_ATTACK;

	pGrp->ClearAllActionAni( FLOAT100, true );
	pGrp->ClearAllLoopAni( FLOAT100, true );

	m_GrapplingTargetName = szTarget;

	if( m_CurChargeCmdInfo.m_Cmd.IsEmpty() )
		return false;

	float fAniRate = m_CurChargeCmdInfo.m_SubInfoList[m_iCurCmdCombo].m_fGrappAttakerAniRate;
	int iAniID = pGrp->GetAnimationIdx( m_CurChargeCmdInfo.m_SubInfoList[m_iCurCmdCombo].m_stGrappAttakerAni );
	pGrp->SetActionAni( iAniID, FLOAT100, FLOAT100, FLOAT1, FLOAT1/fAniRate, 0.0f, 0.0f, true );

	m_fGrapOffsetX = m_CurChargeCmdInfo.m_SubInfoList[m_iCurCmdCombo].m_fGrappOffSetX;
	m_fGrapOffsetY = m_CurChargeCmdInfo.m_SubInfoList[m_iCurCmdCombo].m_fGrappOffSetY;

	if( pOwner->IsNeedSendNetwork() )
	{
		SP2Packet kPacket( CUPK_ITEM_GAUGE_CANCEL );
		FillItemGaugePacketHeader( pOwner, kPacket );
		kPacket << HS_CMD_ATTACK;
		kPacket << m_iCurCombo;
		kPacket << m_iCurCmdCombo;
		kPacket << pOwner->GetWorldPosition();
		kPacket << pOwner->GetTargetRot();
		kPacket << m_GrapplingTargetName;
		P2PNetwork::SendToAllPlayingUser( kPacket );
	}

	//////////////////////////////////////////////////////////////////////////
	iAniID	     = pGrp->GetAnimationIdx( m_CurChargeCmdInfo.m_SubInfoList[m_iCurCmdCombo].m_stGrappAttakerAni );
	float fTimeRate  = m_CurChargeCmdInfo.m_SubInfoList[m_iCurCmdCombo].m_fGrappAttakerAniRate;
	DWORD dwCurTime = FRAMEGETTIME();
	m_dwCmdAttackEndTime = dwCurTime + pGrp->GetAnimationFullTime( iAniID ) * fTimeRate;
	//////////////////////////////////////////////////////////////////////////

	ioBaseChar *pTarget = pOwner->GetBaseChar( m_GrapplingTargetName );
	if( !pTarget ) 
		return false;

	GrapplingSkillWoundedInfo info;

	if( (int)m_CurChargeCmdInfo.m_SubInfoList.size() >= m_iCurCmdCombo )
		info.m_GrapplingLoopMotion2 = m_CurChargeCmdInfo.m_SubInfoList[m_iCurCmdCombo].m_stGrappWounerAni;
	else
		info.m_GrapplingLoopMotion2 = m_CurChargeCmdInfo.m_SubInfoList[0].m_stGrappWounerAni;

	//info.
	pTarget->SetItemGrapplingState( pOwner->GetCharName(), info );

	return true;
}

bool ioGunnerItem::EnableGauge( float fNeedGauge )
{
	if( m_fCurExtraGauge >= fNeedGauge )
		return true;

	return false;
}

void ioGunnerItem::UpdateExtraData( ioBaseChar *pOwner )
{
	if( !pOwner ) return;

	// check owner state
	float fTimePerSec = g_FrameTimer.GetSecPerFrame();

	if( pOwner->IsSystemState() )
		return;

	if( m_bEquipedOnHand )
	{
		return;
	}

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

int ioGunnerItem::GetCurBullet()
{
	return (int)m_fCurExtraGauge;
}

int ioGunnerItem::GetMaxBullet()
{
	return (int)m_fMaxExtraGauge;
}

void ioGunnerItem::SetCurBullet( int iCurBullet )
{
	if( iCurBullet < 0 )
		return;

	float fCurBullet = iCurBullet;
	fCurBullet = max( 0, min( fCurBullet, m_fMaxExtraGauge ) );

	m_fCurExtraGauge = fCurBullet;
}

int ioGunnerItem::GetNeedBullet()
{
	return (int)m_fNeedGauge;
}

void ioGunnerItem::DecreaseGauge( float fNeedGauge )
{
	m_fCurExtraGauge -= fNeedGauge;
	m_fCurExtraGauge = (float)max( 0, min( m_fCurExtraGauge, m_fMaxExtraGauge ) );
}