

#include "stdafx.h"

#include "ioChargeCommandItem.h"

#include "ioBaseChar.h"

#include "WeaponDefine.h"
#include "ItemDefine.h"

ioChargeCommandItem::ioChargeCommandItem()
{
	m_iCurCombo = 0;

	ClearState();
	ClearCmdInfo();

	m_bSetExtraGauge = false;
	m_fCurExtraGauge = 0.0f;
}

ioChargeCommandItem::ioChargeCommandItem( const ioChargeCommandItem &rhs )
: ioWeaponItem( rhs ),
  m_AttackReadyAniList( rhs.m_AttackReadyAniList ),
  m_vExtendAttributeList( rhs.m_vExtendAttributeList ),
  m_ChargeCmdInfoList( rhs.m_ChargeCmdInfoList ),
  m_dwInputMaxTime( rhs.m_dwInputMaxTime ),
  m_dwInputStartGapTime( rhs.m_dwInputStartGapTime ),
  m_dwInputGapTime( rhs.m_dwInputGapTime ),
  m_bUseExtraGauge( rhs.m_bUseExtraGauge ),
  m_fDecreaseExtraGauge( rhs.m_fDecreaseExtraGauge ),
  m_ExtraGaugeEffect( rhs.m_ExtraGaugeEffect ),
  m_ExtraGaugeBuffList( rhs.m_ExtraGaugeBuffList ),
  m_fIncreaseExtraGauge( rhs.m_fIncreaseExtraGauge ),
  m_fNeedGauge( rhs.m_fNeedGauge )
{
	m_iCurCombo = 0;

	ClearState();
	ClearCmdInfo();

	m_bSetExtraGauge = false;
	m_fCurExtraGauge = 0.0f;
}

ioChargeCommandItem::~ioChargeCommandItem()
{
}

void ioChargeCommandItem::LoadProperty( ioINILoader &rkLoader )
{
	ioWeaponItem::LoadProperty( rkLoader );

	char szBuf[MAX_PATH] = "";
	char szKey[MAX_PATH] = "";

	// ready ani
	int iMaxCombo = GetMaxCombo();
	m_vExtendAttributeList.clear();
	if( iMaxCombo > 0 )
		m_vExtendAttributeList.reserve( iMaxCombo );

	for( int i=0; i < iMaxCombo; ++i )
	{
		wsprintf_e( szBuf, "attack_ready_animation%d", i+1 );
		rkLoader.LoadString( szBuf, "", szBuf, MAX_PATH );
		m_AttackReadyAniList.push_back( ioHashString(szBuf) );

		AttackAttribute kAttribute;
		wsprintf_e( szBuf, "extend_attack0%d", i+1 );
		LoadAttackAttribute( szBuf, kAttribute, rkLoader );
		m_vExtendAttributeList.push_back( kAttribute );
	}

	//
	LoadCmdInfoList( rkLoader );

	//
	m_dwInputMaxTime = (DWORD)rkLoader.LoadInt_e( "input_max_time", 0 );
	m_dwInputGapTime = (DWORD)rkLoader.LoadInt_e( "input_gap_time", 0 );
	m_dwInputStartGapTime = (DWORD)rkLoader.LoadInt_e( "input_start_gap_time", 0 );

	// extragauge
	m_bUseExtraGauge = rkLoader.LoadBool_e( "use_extra_gauge", false );
	m_fDecreaseExtraGauge = rkLoader.LoadFloat_e( "decrease_extra_gauge", FLOAT10 );

	//추가///////////////////////////////////////////////////////////////////////
	m_fNeedGauge = rkLoader.LoadFloat_e( "need_gauge", FLOAT100 );
	m_fIncreaseExtraGauge = rkLoader.LoadFloat_e( "increase_gauge", .0f );
	/////////////////////////////////////////////////////////////////////////////

	rkLoader.LoadString_e( "extra_gauge_effect", "", szBuf, MAX_PATH );
	m_ExtraGaugeEffect = szBuf;

	m_ExtraGaugeBuffList.clear();
	int iMaxBuffCnt = rkLoader.LoadInt_e( "max_extra_gauge_buff", 0 );
	if( iMaxBuffCnt )
		m_ExtraGaugeBuffList.reserve( iMaxBuffCnt );

	for( int i=0; i < iMaxBuffCnt; ++i )
	{
		wsprintf_e( szKey, "extra_gauge_buff%d", i+1 );
		rkLoader.LoadString( szKey, "", szBuf, MAX_PATH );
		m_ExtraGaugeBuffList.push_back( ioHashString(szBuf) );
	}
}

void ioChargeCommandItem::LoadCmdInfoList( ioINILoader &rkLoader )
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
		}
	}
}

ioItem* ioChargeCommandItem::Clone()
{
	return new ioChargeCommandItem( *this );
}

ioWeaponItem::WeaponSubType ioChargeCommandItem::GetSubType() const
{
	return WST_CHARGE_COMMAND;
}

void ioChargeCommandItem::SetNormalAttack( ioBaseChar *pOwner, int iCurCombo )
{
	ClearCancelInfo();

	ClearState();
	ClearCmdInfo();

	InitAniRotate();

	DWORD dwCurTime = FRAMEGETTIME();
	int iComboCnt = GetMaxCombo();
	if( !COMPARE( iCurCombo, 0, iComboCnt ) )
	{
		ioWeaponItem::SetNormalAttack( pOwner, iCurCombo );

		m_ChargeState = CS_NONE;
		m_iCurCombo = 0;

		pOwner->SetAttackMoveEnable( false );
		pOwner->SetChargingState( false );
	}
	else
	{
		ioEntityGroup *pGrp = pOwner->GetGroup();
		pGrp->ClearAllLoopAni( FLOAT100, true );
		pGrp->ClearAllActionAni( FLOAT100, true );

		int iAniID = pGrp->GetAnimationIdx( m_AttackReadyAniList[iCurCombo] );
		float fTimeRate = (float)m_dwChargeTime / pGrp->GetAnimationFullTime( iAniID );
		pGrp->SetActionAni( iAniID, FLOAT100, FLOAT100, FLOAT1, FLOAT1/fTimeRate, 0.0f, 0.0f, true );

		m_ChargeState = CS_CHARGING;
		m_dwAttackStartTime = dwCurTime;
		m_iCurCombo = iCurCombo;
		pOwner->SetState( CS_ATTACK );
		pOwner->SetAttackMoveEnable( false );
		pOwner->SetChargingState( true );

		DWORD dwTrackingTime = dwCurTime;
		dwTrackingTime += pGrp->GetAnimationEventTime_e( iAniID, "tracking_e" ) * fTimeRate;
		pOwner->SetAutoTargetTrackingEndTime( dwTrackingTime );
	}
}

void ioChargeCommandItem::CheckNormalAttack( ioBaseChar *pOwner )
{
	DWORD dwCurTime = FRAMEGETTIME();

	switch( m_ChargeState )
	{
	case CS_CHARGING:
		OnCharging( pOwner );
		break;
	case CS_NORMAL_ATTACK:
		{
			if( pOwner->IsNeedProcess() )
			{
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
	case CS_ATTACK_FIRE:
		{
			pOwner->CheckExtraAniJump();

			if( pOwner->IsNeedProcess() )
			{
				if( pOwner->CheckExtendAttackEndJump() )
					return;

				CheckCancelReserve( pOwner );

				if( ProcessCancel(pOwner) )
					return;

				if( CheckCmdInputState( pOwner ) )
					return;

				if( CheckSkillInputState( pOwner ) )
					return;
			}

			CheckAniRotate( pOwner );
			ioWeaponItem::CheckNormalAttack( pOwner );
		}
		break;
	}
}

void ioChargeCommandItem::OnCharging( ioBaseChar *pOwner )
{
	if( !pOwner->IsNeedProcess() )	return;

	DWORD dwCurTime = FRAMEGETTIME();
	if( pOwner->IsAttackKeyDown() )
	{
		if( m_dwAttackStartTime + m_dwChargeTime <= dwCurTime )
		{
			ChangeToAttackFire( pOwner );
		}
	}
	else
	{
		ChangeToNormalAttack( pOwner );
	}
}

void ioChargeCommandItem::ChangeToNormalAttack( ioBaseChar *pOwner )
{
	if( !pOwner ) return;

	DWORD dwCurTime = FRAMEGETTIME();

	pOwner->InitExtendAttackTagInfo();

	ClearCancelInfo();

	const AttackAttribute *pAttribute = m_FormalAttack.GetAttribute( AT_NORMAL, m_iCurCombo );
	if( pAttribute )
	{
		pOwner->SetNormalAttackByAttribute( *pAttribute );

		m_szCurAni = pAttribute->m_AttackAnimation;
		m_fCurAniRate = pAttribute->m_fAttackAniRate;

		m_dwInputCancelCheckTime = dwCurTime;
		m_dwCancelCheckTime = dwCurTime;
	}

	pOwner->ApplyNormalAttackResult( m_iCurCombo );
	m_ChargeState = CS_NORMAL_ATTACK;

	if( pOwner->IsNeedSendNetwork() )
	{
		SP2Packet kPacket( CUPK_ITEM_GAUGE_OK );
		FillItemGaugePacketHeader( pOwner, kPacket );
		kPacket << ST_ATTACK_STATE;
		kPacket << m_ChargeState;
		kPacket << m_iCurCombo;
		kPacket << pOwner->GetTargetRot();
		P2PNetwork::SendToAllPlayingUser( kPacket );
	}
}

void ioChargeCommandItem::ChangeToAttackFire( ioBaseChar *pOwner )
{
	if( !pOwner ) return;

	DWORD dwCurTime = FRAMEGETTIME();
	pOwner->InitExtendAttackTagInfo();

	ClearCancelInfo();

	int iExtendCnt = m_vExtendAttributeList.size();
	if( COMPARE( m_iCurCombo, 0, iExtendCnt ) )
	{
		m_szCurAni = m_vExtendAttributeList[m_iCurCombo].m_AttackAnimation;
		m_fCurAniRate = m_vExtendAttributeList[m_iCurCombo].m_fAttackAniRate;

		m_dwInputCancelCheckTime = dwCurTime;
		m_dwCancelCheckTime = dwCurTime;

		pOwner->SetNormalAttackByAttribute( m_vExtendAttributeList[m_iCurCombo] );

		ioEntityGroup *pGrp = pOwner->GetGroup();
		int iAniID	     = pGrp->GetAnimationIdx( m_vExtendAttributeList[m_iCurCombo].m_AttackAnimation );
		float fTimeRate  = m_vExtendAttributeList[m_iCurCombo].m_fAttackAniRate;
		DWORD dwPreDelay = m_vExtendAttributeList[m_iCurCombo].m_dwPreDelay;

		pOwner->SetBaseAttackExtraAniJump( iAniID, fTimeRate, dwPreDelay );
		pOwner->SetExtendAttackEndJumpInfo( iAniID, fTimeRate, dwPreDelay );

		SetAniRotate( pOwner, iAniID, fTimeRate );
	}

	if( iExtendCnt <= m_iCurCombo + 1 )
	{
		pOwner->ApplyNormalAttackResult( m_iCurCombo, true );
	}
	else
	{
		pOwner->ApplyNormalAttackResult( m_iCurCombo );
	}

	m_ChargeState = CS_ATTACK_FIRE;

	if( pOwner->IsNeedSendNetwork() )
	{
		SP2Packet kPacket( CUPK_ITEM_GAUGE_OK );
		FillItemGaugePacketHeader( pOwner, kPacket );
		kPacket << ST_ATTACK_STATE;
		kPacket << m_ChargeState;
		kPacket << m_iCurCombo;
		kPacket << pOwner->GetTargetRot();
		P2PNetwork::SendToAllPlayingUser( kPacket );
	}
}

void ioChargeCommandItem::OnItemGaugeOK( ioBaseChar *pOwner, SP2Packet &rkPacket )
{
	if( !pOwner ) return;

	int iSyncType;
	rkPacket >> iSyncType;

	switch( iSyncType )
	{
	case ST_ATTACK_STATE:
		ApplyAttackState( pOwner, rkPacket );
		break;
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

void ioChargeCommandItem::OnItemGaugeCancel( ioBaseChar *pOwner, SP2Packet &rkPacket )
{
	if( !pOwner )
		return;

	int iSyncType;
	rkPacket >> iSyncType;

	switch( iSyncType )
	{
	case ST_EXTRA_ADD:
		m_bSetExtraGauge = true;
		m_fCurExtraGauge = m_fMaxExtraGauge;
		SetExtraGaugeBuff( pOwner );
		break;
	case ST_EXTRA_REMOVE:
		m_bSetExtraGauge = false;
		m_fCurExtraGauge = 0.0f;
		ReleaseExtraGaugeBuff( pOwner, false );
		break;
	}
}

void ioChargeCommandItem::SetExtraInfoToWeapon( ioWeapon *pWeapon, bool bJumpAttack )
{
	ioAttackableItem::SetExtraInfoToWeapon( pWeapon, bJumpAttack );

	//
	ioZoneEffectWeapon *pZone = ToZoneEffectWeapon( pWeapon );
	if( pZone )
	{
		ioBaseChar *pChar = pZone->GetOwner();
		if( pChar )
		{
			D3DXVECTOR3 vDir = pChar->GetTargetRot() * D3DXVECTOR3( 0.0f, 0.0f, -FLOAT1 );
			pZone->SetCollisionDir( vDir );

			if( pChar->GetState() == CS_CMD_STATE )
				pZone->SetZoneValue( true, false );
			else
				pZone->SetZoneValue( false, false );
		}
	}
}

bool ioChargeCommandItem::IsChargeAttackState( ioBaseChar *pOwner )
{
	if( pOwner->GetState() == CS_ATTACK )
	{
		switch( m_ChargeState )
		{
		case CS_CHARGING:
		case CS_ATTACK_FIRE:
			return true;
		}
	}

	return false;
}

bool ioChargeCommandItem::IsEnableAttackDashCheck( ioBaseChar *pOwner, bool bCheck )
{
	if( !IsChargeDash() ) return false;

	return false;
}

bool ioChargeCommandItem::SetJumpAttack( ioBaseChar *pOwner, int iAniID, float fTimeRate, AttackType eType )
{
	if( !pOwner ) return false;

	DWORD dwCurTime = FRAMEGETTIME();
	ClearAirJump();

	AttackType eExtendType = AT_JUMP_ATTACK;

	const AttackAttribute *pAttr = GetAttackAttribute( eExtendType );
	if( !pAttr )
	{
		LOG.PrintTimeAndLog( 0, "ioChargeCommandItem::SetJumpAttack - AttackAttribute Not Exist" );
		return false;
	}

	ioEntityGroup *pGrp = pOwner->GetGroup();
	iAniID = pGrp->GetAnimationIdx( pAttr->m_AttackAnimation );
	if( iAniID == -1 )
	{
		LOG.PrintTimeAndLog( 0, "ioChargeCommandItem::SetJumpAttack - JumpAttack Animation Not Exist" );
		return false;
	}

	fTimeRate   = pAttr->m_fAttackAniRate;
	DWORD dwPreDelay  = pAttr->m_dwPreDelay;

	// For AirJumpPower
	SetAirJump( pOwner, iAniID, fTimeRate );

	return false;
}

void ioChargeCommandItem::ProcessJumpAttack( ioBaseChar *pOwner, float fHeightGap )
{
	if( m_pExtendJump )
	{
		m_pExtendJump->ProcessJumpAttack( pOwner, fHeightGap );
	}

	CheckAirJump( pOwner );
}

void ioChargeCommandItem::ProcessReserveKeyInput( ioBaseChar *pOwner )
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
				pOwner->SetNormalAttack( 0 );
			}
		}
		return;
	}

	// 대기
	pOwner->SetState( CS_DELAY );
}

bool ioChargeCommandItem::ChangeToNewCmdSet( ioBaseChar *pOwner )
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
				if( CheckMamaHahaCmd( rkInfo.m_CmdUseType ) )
				{
					if( !CheckEnableMamaHahaCmd( pOwner, rkInfo.m_CmdUseType ) )
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

				pOwner->SetCmdAttackState();
				SetCmdAttack( pOwner, rkSubInfoList[m_iCurCmdCombo].m_CmdAttack );

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

bool ioChargeCommandItem::ChangeToComboCmdSet( ioBaseChar *pOwner )
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

	if( rkSubInfoList[m_iCurCmdCombo].m_bCmdAttack_D && m_bCmdInputD )
	{
		pOwner->SetCmdAttackState();
		SetCmdAttack( pOwner, rkSubInfoList[m_iCurCmdCombo].m_CmdAttack_D );

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
	else if( rkSubInfoList[m_iCurCmdCombo].m_bCmdAttack_S && m_bCmdInputS )
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
	else
	{
		if( CheckMamaHahaCmd( m_CurChargeCmdInfo.m_CmdUseType ) && !CheckEnableMamaHahaCmd( pOwner, m_CurChargeCmdInfo.m_CmdUseType ) )
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


	pOwner->SetCmdAttackState();
	SetCmdAttack( pOwner, rkSubInfoList[m_iCurCmdCombo].m_CmdAttack );

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

bool ioChargeCommandItem::ChangeToCmdSet( ioBaseChar *pOwner )
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

bool ioChargeCommandItem::CheckCmdInput( ioBaseChar *pOwner )
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

void ioChargeCommandItem::SetCmdAttack( ioBaseChar *pOwner, const AttackAttribute &rkAttr, float fExtraAniRate, float fExtraForceRate )
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

	DecreaseGauge();
}

void ioChargeCommandItem::ClearState()
{
	m_ChargeState = CS_NONE;

	m_dwAttackStartTime = 0;

	m_iCurAirJump = 0;
	m_fAirJumpPowerRate = 0.0f;
}

void ioChargeCommandItem::ClearCmdInfo()
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
	m_CurChargeCmdInfo.Init();
}

void ioChargeCommandItem::OnEndExtendAttack( ioBaseChar *pOwner, bool bReleaseItem )
{
	if( !pOwner )
		return;

	pOwner->SetAttackMoveEnable( false );
	pOwner->SetChargingState( false );

	ClearState();
}

void ioChargeCommandItem::ApplyAttackState( ioBaseChar *pOwner, SP2Packet &rkPacket )
{
	int iState;
	rkPacket >> iState;
	rkPacket >> m_iCurCombo;

	switch( iState )
	{
	case CS_NONE:
		{
			ClearState();
			ClearCmdInfo();
			m_ChargeState = CS_NONE;
		}
		break;
	case CS_NORMAL_ATTACK:
		{
			D3DXQUATERNION qtRot;
			rkPacket >> qtRot;

			pOwner->SetTargetRotToRotate( qtRot, true );
			ChangeToNormalAttack( pOwner );
		}
		break;
	case CS_ATTACK_FIRE:
		{
			D3DXQUATERNION qtRot;
			rkPacket >> qtRot;

			pOwner->SetTargetRotToRotate( qtRot, true );
			ChangeToAttackFire( pOwner );
		}
		break;
	}
}

void ioChargeCommandItem::ApplyCmdSet( ioBaseChar *pOwner, ioHashString szCmdInfo, int iCmdCombo )
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

			if( CheckMamaHahaCmd( rkInfo.m_CmdUseType ) && !CheckEnableMamaHahaCmd( pOwner, rkInfo.m_CmdUseType ) )
				return;

			ChargeCmdSubInfoList &rkSubInfoList = rkInfo.m_SubInfoList;
			if( !COMPARE( iCmdCombo, 0, (int)rkSubInfoList.size() ) )
				return;

			pOwner->SetCmdAttackState();
			SetCmdAttack( pOwner, rkSubInfoList[iCmdCombo].m_CmdAttack );
			return;
		}

		++iter;
	}
}

void ioChargeCommandItem::ApplyCmdSetD( ioBaseChar *pOwner, ioHashString szCmdInfo, int iCmdCombo )
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

			if( CheckMamaHahaCmd( rkInfo.m_CmdUseType ) && !CheckEnableMamaHahaCmd( pOwner, rkInfo.m_CmdUseType ) )
				return;

			ChargeCmdSubInfoList &rkSubInfoList = rkInfo.m_SubInfoList;
			if( !COMPARE( iCmdCombo, 0, (int)rkSubInfoList.size() ) )
				return;

			if( !rkSubInfoList[iCmdCombo].m_bCmdAttack_D )
				return;

			pOwner->SetCmdAttackState();
			SetCmdAttack( pOwner, rkSubInfoList[iCmdCombo].m_CmdAttack_D );
			return;
		}

		++iter;
	}
}

void ioChargeCommandItem::ApplyCmdSetS( ioBaseChar *pOwner, ioHashString szCmdInfo, int iCmdCombo )
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

			if( CheckMamaHahaCmd( rkInfo.m_CmdUseType ) && !CheckEnableMamaHahaCmd( pOwner, rkInfo.m_CmdUseType ) )
				return;

			ChargeCmdSubInfoList &rkSubInfoList = rkInfo.m_SubInfoList;
			if( !COMPARE( iCmdCombo, 0, (int)rkSubInfoList.size() ) )
				return;

			if( !rkSubInfoList[iCmdCombo].m_bCmdAttack_S )
				return;

			pOwner->SetCmdAttackState();
			SetCmdAttack( pOwner, rkSubInfoList[iCmdCombo].m_CmdAttack_S );
			return;
		}

		++iter;
	}
}

void ioChargeCommandItem::CheckCmdState( ioBaseChar *pOwner )
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

			if( CheckSkillInputState( pOwner ) )
				return;
		}
	}
}

void ioChargeCommandItem::SetCmdInputStart( int iType, ioBaseChar *pOwner )
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

void ioChargeCommandItem::SetCmdInputEnd( ioBaseChar *pOwner )
{
	if( !pOwner ) return;
	if( !pOwner->IsNeedProcess() ) return;

	if( m_dwInputStartTime == 0 || m_dwInputCheckTime == 0 )
		return;

	ChangeToCmdSet( pOwner );
}

bool ioChargeCommandItem::CheckCmdInputState( ioBaseChar *pOwner )
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
		return ChangeToCmdSet( pOwner );
	}

	CheckCmdInput( pOwner );

	return false;
}

bool ioChargeCommandItem::CheckCmdInputTime()
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

void ioChargeCommandItem::SetSkillInputStart()
{
	m_dwInputSkillStartTime = FRAMEGETTIME();
}

bool ioChargeCommandItem::CheckSkillInputState( ioBaseChar *pOwner )
{
	if( !pOwner ) return false;
	if( !pOwner->IsNeedProcess() ) return false;

	if( m_dwInputSkillStartTime == 0 )
		return false;

	int iSkillNum = pOwner->GetSkillKeyInput();
	if( iSkillNum >= 0 && pOwner->GetUseEnableSkill( iSkillNum, SUT_NORMAL ) && !pOwner->IsBuffLimitSkill() )
	{
		pOwner->ClearAttackFireTimeAndSkill();
		pOwner->ClearReservedSliding();

		pOwner->SetUseSkill( iSkillNum, SUT_NORMAL );
		m_dwInputSkillStartTime = 0;
		return true;
	}

	return false;
}

void ioChargeCommandItem::ClearCmdTime()
{
	m_dwInputStartTime = 0;
	m_dwInputSkillStartTime = 0;
}

void ioChargeCommandItem::SetExtraGaugeTypeByWeapon( ioBaseChar *pOwner, int iType, int iWGT, float fDamage )
{
	if( !pOwner )
		return;

	if( !m_bUseExtraGauge )
		return;

	if( m_bSetExtraGauge )
		return;

	float fTypeRate = 0.0f;
	switch( iType )
	{
	case EGT_ATTACK:
		if( iWGT == WGT_COMMAND )
			fTypeRate = m_fExtraGaugeAttack;
		break;
	case EGT_ATTACK_DEF:
		if( iWGT == WGT_COMMAND )
			fTypeRate = m_fExtraGaugeAttackDef;
		break;
	case EGT_WOUNDED:
		fTypeRate = m_fExtraGaugeWound;
		break;
	case EGT_WOUNDED_DEF:
		fTypeRate = m_fExtraGaugeWoundDef;
		break;
	}

	float fCurGauge = fTypeRate * fDamage;
	m_fCurExtraGauge += fCurGauge;
	if( m_fCurExtraGauge >= m_fMaxExtraGauge )
	{
		m_bSetExtraGauge = true;
		m_fCurExtraGauge = m_fMaxExtraGauge;

		if( pOwner->IsNeedProcess() )
			SetExtraGaugeBuff( pOwner );
	}
}

void ioChargeCommandItem::SetExtraGaugeBuff( ioBaseChar *pOwner )
{
	if( !pOwner ) return;

	int iBuffCnt = m_ExtraGaugeBuffList.size();
	for( int i=0; i < iBuffCnt; ++i )
	{
		pOwner->AddNewBuffWithAnimateTime( m_ExtraGaugeBuffList[i], pOwner->GetCharName(), m_Name, NULL, 0 );
	}

	if( !m_ExtraGaugeEffect.IsEmpty() )
	{
		pOwner->EndEffect( m_ExtraGaugeEffect, false );
		pOwner->AttachEffect( m_ExtraGaugeEffect );
	}

	if( pOwner->IsNeedSendNetwork() )
	{
		SP2Packet kPacket( CUPK_ITEM_GAUGE_CANCEL );
		FillItemGaugePacketHeader( pOwner, kPacket );
		kPacket << ST_EXTRA_ADD;
		P2PNetwork::SendToAllPlayingUser( kPacket );
	}
}

void ioChargeCommandItem::ReleaseExtraGaugeBuff( ioBaseChar *pOwner, bool bSend )
{
	if( !pOwner ) return;
	if( !m_bUseExtraGauge ) return;

	int iBuffCnt = m_ExtraGaugeBuffList.size();
	for( int i=0; i < iBuffCnt; ++i )
	{
		pOwner->RemoveBuff( m_ExtraGaugeBuffList[i] );
	}

	if( !m_ExtraGaugeEffect.IsEmpty() )
	{
		pOwner->EndEffect( m_ExtraGaugeEffect, false );
	}

	if( bSend && pOwner->IsNeedSendNetwork() )
	{
		SP2Packet kPacket( CUPK_ITEM_GAUGE_CANCEL );
		FillItemGaugePacketHeader( pOwner, kPacket );
		kPacket << ST_EXTRA_REMOVE;
		P2PNetwork::SendToAllPlayingUser( kPacket );
	}
}

void ioChargeCommandItem::ProcessExtraGauge( ioBaseChar *pOwner )
{
	if( !m_bSetExtraGauge )
		return;

	float fTimePerSec = g_FrameTimer.GetSecPerFrame();
	float fCurDecrease = m_fDecreaseExtraGauge * fTimePerSec;

	m_fCurExtraGauge -= fCurDecrease;
	if( m_fCurExtraGauge <= 0.0f )
	{
		m_bSetExtraGauge = false;
		m_fCurExtraGauge = 0.0f;

		if( pOwner->IsNeedProcess() )
			ReleaseExtraGaugeBuff( pOwner, true );
	}

	m_fCurExtraGauge = (float)max( 0, min( m_fCurExtraGauge, m_fMaxExtraGauge ) );
}


#ifdef BALANCE_RENEWAL
int ioChargeCommandItem::GetMaxBullet()
{
	//if( m_bUseExtraGauge )
	return (int)m_fMaxExtraGauge;

	//return 0;
}

int ioChargeCommandItem::GetCurBullet()
{
	//if( m_bUseExtraGauge )
	return (int)m_fCurExtraGauge;

	//return -1;
}
#else
int ioChargeCommandItem::GetMaxBullet()
{
	if( m_bUseExtraGauge )
		return (int)m_fMaxExtraGauge;

	return 0;
}

int ioChargeCommandItem::GetCurBullet()
{
	if( m_bUseExtraGauge )
		return (int)m_fCurExtraGauge;

	return -1;
}
#endif

void ioChargeCommandItem::SetCurBullet( int iCurBullet )
{
	if( iCurBullet < 0 )
		return;

	float fCurBullet = iCurBullet;
	fCurBullet = max( 0, min( fCurBullet, m_fMaxExtraGauge ) );

	m_fCurExtraGauge = fCurBullet;
}

void ioChargeCommandItem::OnEquiped( ioBaseChar *pOwner )
{
	ioWeaponItem::OnEquiped( pOwner );

	m_bSetExtraGauge = false;
}

void ioChargeCommandItem::OnReleased( ioBaseChar *pOwner )
{
	ioWeaponItem::OnReleased( pOwner );

	m_bSetExtraGauge = false;
	//m_fCurExtraGauge = 0.0f;
	ReleaseExtraGaugeBuff( pOwner, false );
}

#ifdef BALANCE_RENEWAL
void ioChargeCommandItem::UpdateExtraData( ioBaseChar *pOwner )
{
	/*if( !pOwner ) return;

	if( m_bUseExtraGauge )
		ProcessExtraGauge( pOwner );*/

	if( !pOwner ) return;

	// check owner state
	float fTimePerSec = g_FrameTimer.GetSecPerFrame();

	if( pOwner->IsSystemState() )
		return;
		
	switch( pOwner->GetState() )
	{
	case CS_DELAY:
	case CS_RUN:
		if( m_fCurExtraGauge < m_fMaxExtraGauge )
		{
			float fGauge = m_fIncreaseExtraGauge * fTimePerSec;

			m_fCurExtraGauge += fGauge;
			m_fCurExtraGauge = min( m_fCurExtraGauge, m_fMaxExtraGauge );
		}
		return;
	}
}
#else
void ioChargeCommandItem::UpdateExtraData( ioBaseChar *pOwner )
{
	if( !pOwner ) return;

	if( m_bUseExtraGauge )
		ProcessExtraGauge( pOwner );
}
#endif

bool ioChargeCommandItem::CheckMamaHahaCmd( CmdUseType eUseType )
{
	switch( eUseType )
	{
	case CUT_RERA_SHU:
	case CUT_SHICHIKAPU_ETU:
	case CUT_SHICHIKAPU_ARM:
		return true;
	}

	return false;
}

bool ioChargeCommandItem::CheckEnableMamaHahaCmd( ioBaseChar *pOwner, CmdUseType eUseType )
{
	return false;
}

int ioChargeCommandItem::GetNeedBullet()
{
	return (int)m_fNeedGauge;
}


void ioChargeCommandItem::DecreaseGauge()
{
	m_fCurExtraGauge -= m_fNeedGauge;
	m_fCurExtraGauge = (float)max( 0, min( m_fCurExtraGauge, m_fMaxExtraGauge ) );
}