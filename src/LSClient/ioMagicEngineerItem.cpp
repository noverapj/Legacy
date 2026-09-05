
#include "stdafx.h"

#include "WeaponDefine.h"
#include "ItemDefine.h"
#include "BuffDefine.h"
#include "DummyCharDefine.h"

ioMagicEngineerItem::ioMagicEngineerItem()
{
	m_ChargeState = CS_NONE;

	m_dwAttackStartTime = 0;
	m_iCurCombo = 0;
	m_iMoveIndex = 0;

	m_fCurChargeRate = 0.0f;
	m_fCurChargeRateForSkill = 0.0f;

	m_CurAttackInfo.Init();

	m_iCurDummyCharIndex = -1;
	m_bCreateDummy = false;

	m_dwSpecialMoveMacroTime_S = 0;
	m_dwSpecialMoveMacroTime_E = 0;
	m_iSpecialMoveDir = 0;

	m_fCurGauge = 0.0f;
	m_bJump = false;
}

ioMagicEngineerItem::ioMagicEngineerItem( const ioMagicEngineerItem &rhs )
: ioWeaponItem( rhs ),
m_AttackReadyAniList( rhs.m_AttackReadyAniList ),
m_vDestroyDummyCharList( rhs.m_vDestroyDummyCharList ),
m_vMoveInfo( rhs.m_vMoveInfo ),
m_AttackInfo( rhs.m_AttackInfo ),
m_HalfAttackInfo( rhs.m_HalfAttackInfo ),
m_szGatheringAni( rhs.m_szGatheringAni ),
m_fGatheringAniRate( rhs.m_fGatheringAniRate ),
m_szLandingAni( rhs.m_szLandingAni ),
m_fLandingAniRate( rhs.m_fLandingAniRate ),
m_dwFullChargeTime( rhs.m_dwFullChargeTime ),
m_JumpHalfChargeInfo( rhs.m_JumpHalfChargeInfo ),
m_JumpFullChargeInfo( rhs.m_JumpFullChargeInfo ),
m_DashHalfChargeInfo( rhs.m_DashHalfChargeInfo ),
m_DashFullChargeInfo( rhs.m_DashFullChargeInfo ),
m_fMaxGauge( rhs.m_fMaxGauge ),
m_fNeedGauge( rhs.m_fNeedGauge ),
m_fIncreaseGauge( rhs.m_fIncreaseGauge ),
m_fJumpHeightGap( rhs.m_fJumpHeightGap )
{
	m_ChargeState = CS_NONE;

	m_dwAttackStartTime = 0;
	m_iCurCombo = 0;
	m_iMoveIndex = 0;

	m_fCurChargeRate = 0.0f;
	m_fCurChargeRateForSkill = 0.0f;

	m_CurAttackInfo.Init();

	m_dwMotionEndTime = 0;
	m_iCurDummyCharIndex = -1;
	m_bCreateDummy = false;
	m_fCurGauge = 0.0f;
	m_bJump = false;

	for( int i = 0; i < 2; i++ )
		m_SpecialMoveInfo[i] = rhs.m_SpecialMoveInfo[i];
}

ioMagicEngineerItem::~ioMagicEngineerItem()
{
	m_vDestroyDummyCharList.clear();
}

void ioMagicEngineerItem::LoadProperty( ioINILoader &rkLoader )
{
	ioWeaponItem::LoadProperty( rkLoader );

	char szBuf[MAX_PATH];
	char szKey[MAX_PATH] = "";

	int iMaxCombo = GetMaxCombo();
	m_AttackReadyAniList.clear();
	for( int i=0; i < iMaxCombo; ++i )
	{
		wsprintf_e( szBuf, "attack_ready_animation%d", i+1 );
		rkLoader.LoadString( szBuf, "", szBuf, MAX_PATH );
		m_AttackReadyAniList.push_back( ioHashString(szBuf) );
	}

	rkLoader.LoadString_e( "gathering_animation", "", szBuf, MAX_PATH );
	m_szGatheringAni = szBuf;
	m_fGatheringAniRate = rkLoader.LoadFloat_e( "gathering_ani_rate", FLOAT1 );

	rkLoader.LoadString_e( "landing_animation", "", szBuf, MAX_PATH );
	m_szLandingAni = szBuf;
	m_fLandingAniRate = rkLoader.LoadFloat_e( "landing_ani_rate", FLOAT1 );

	m_AttackInfo.Init();
	m_HalfAttackInfo.Init();
	m_JumpHalfChargeInfo.Init();
	m_JumpFullChargeInfo.Init();
	m_DashHalfChargeInfo.Init();
	m_DashFullChargeInfo.Init();

	rkLoader.LoadString_e( "full_actionAni", "", szBuf, MAX_PATH );
	m_AttackInfo.m_StartAni = szBuf;
	m_AttackInfo.m_fAniRate = rkLoader.LoadInt_e( "full_actionAni_rate", 0 );

	rkLoader.LoadString_e( "half_actionAni", "", szBuf, MAX_PATH );
	m_HalfAttackInfo.m_StartAni = szBuf;
	m_HalfAttackInfo.m_fAniRate = rkLoader.LoadInt_e( "half_actionAni_rate", 0 );

	rkLoader.LoadString_e( "jump_half_actionAni", "", szBuf, MAX_PATH );
	m_JumpHalfChargeInfo.m_StartAni = szBuf;
	m_JumpHalfChargeInfo.m_fAniRate = rkLoader.LoadInt_e( "jump_half_actionAni_rate", 0 );

	rkLoader.LoadString_e( "jump_full_actionAni", "", szBuf, MAX_PATH );
	m_JumpFullChargeInfo.m_StartAni = szBuf;
	m_JumpFullChargeInfo.m_fAniRate = rkLoader.LoadInt_e( "jump_full_actionAni_rate", 0 );

	rkLoader.LoadString_e( "dash_half_actionAni", "", szBuf, MAX_PATH );
	m_DashHalfChargeInfo.m_StartAni = szBuf;
	m_DashHalfChargeInfo.m_fAniRate = rkLoader.LoadInt_e( "dash_half_actionAni_rate", 0 );

	rkLoader.LoadString_e( "dash_full_actionAni", "", szBuf, MAX_PATH );
	m_DashFullChargeInfo.m_StartAni = szBuf;
	m_DashFullChargeInfo.m_fAniRate = rkLoader.LoadInt_e( "dash_full_actionAni_rate", 0 );

	int iCnt = rkLoader.LoadInt_e( "destroy_dummy_cnt", 0 );
	m_vDestroyDummyCharList.clear();
	for( int i=0; i < iCnt; ++i )
	{
		wsprintf_e( szKey, "destroy_dummy%d_name", i+1 );
		rkLoader.LoadString( szKey, "", szBuf, MAX_PATH );

		m_vDestroyDummyCharList.push_back( szBuf );
	}

	ioHashString szStateText[6] = { "normal", "half", "jump", "jumphalf", "dash", "dashhalf" };
	for( int i = 0; i < 6; ++i )
	{
		MEDummyMoveInfo cInfo;
		wsprintf( szKey, "%s_dummy_char_name", szStateText[i].c_str() );
		rkLoader.LoadString( szKey, "", szBuf, MAX_PATH );
		cInfo.m_DummyCharName = szBuf;
		if( cInfo.m_DummyCharName.IsEmpty() )
			continue;

		wsprintf( szKey, "%s_dummy_char_offset_x", szStateText[i].c_str() );
		cInfo.m_vDummyCharOffset.x = rkLoader.LoadFloat( szKey, 0.0f );
		wsprintf( szKey, "%s_dummy_char_offset_y", szStateText[i].c_str() );
		cInfo.m_vDummyCharOffset.y = rkLoader.LoadFloat( szKey, 0.0f );
		wsprintf( szKey, "%s_dummy_char_offset_z", szStateText[i].c_str() );
		cInfo.m_vDummyCharOffset.z = rkLoader.LoadFloat( szKey, 0.0f );

		wsprintf( szKey, "%s_dummy_rotate_x", szStateText[i].c_str() );
		cInfo.m_fRotate_x = rkLoader.LoadFloat( szKey, 0.f );
		wsprintf( szKey, "%s_dummy_rotate_y", szStateText[i].c_str() );
		cInfo.m_fRotate_y = rkLoader.LoadFloat( szKey, 0.f );
		wsprintf( szKey, "%s_dummy_move_length", szStateText[i].c_str() );
		cInfo.m_fLength = rkLoader.LoadFloat( szKey, 0.f );

		m_vMoveInfo.insert( std::map< int, MEDummyMoveInfo >::value_type( i, cInfo ) );
	}

	ioHashString szDirText[2] = { "up", "down" };
	for( int i = 0; i < 2; ++i )
	{
		wsprintf( szKey, "special_move_ani_%s", szDirText[i].c_str() );
		rkLoader.LoadString( szKey, "", szBuf, MAX_PATH );
		m_SpecialMoveInfo[i].m_StartAni = szBuf;

		wsprintf( szKey, "special_move_ani_rate_%s", szDirText[i].c_str() );
		m_SpecialMoveInfo[i].m_fStartAniRate = rkLoader.LoadFloat( szKey, FLOAT1 );
		wsprintf( szKey, "special_move_angle_%s", szDirText[i].c_str() );
		m_SpecialMoveInfo[i].m_fStartAngle = rkLoader.LoadFloat( szKey, 0.0f );
		wsprintf( szKey, "special_move_max_range_%s", szDirText[i].c_str() );
		m_SpecialMoveInfo[i].m_fStartMaxRange = rkLoader.LoadFloat( szKey, 0.0f );
		wsprintf( szKey, "special_move_speed_%s", szDirText[i].c_str() );
		m_SpecialMoveInfo[i].m_fStartSpeed = rkLoader.LoadFloat( szKey, 0.0f );
		wsprintf( szKey, "special_move_end_jump_amt_%s", szDirText[i].c_str() );
		m_SpecialMoveInfo[i].m_fEndJumpAmt = rkLoader.LoadFloat( szKey, 0.f );
	}
	m_fMaxGauge = rkLoader.LoadFloat_e( "max_gauge", FLOAT100 );
	m_fNeedGauge = rkLoader.LoadFloat_e( "need_gauge", FLOAT100 );
	m_fIncreaseGauge = rkLoader.LoadFloat_e( "increase_gauge", FLOAT1 );
	m_fJumpHeightGap = rkLoader.LoadFloat_e( "jump_height_gap", 0.0f );

	m_dwFullChargeTime = (DWORD)rkLoader.LoadInt_e( "full_charge_time", 0 );
}

ioItem* ioMagicEngineerItem::Clone()
{
	return new ioMagicEngineerItem( *this );
}

ioWeaponItem::WeaponSubType ioMagicEngineerItem::GetSubType() const
{
	return WST_MAGICENGINEER_ITEM;
}

void ioMagicEngineerItem::SetNormalAttack( ioBaseChar *pOwner, int iCurCombo )
{
	if( !pOwner ) return;

	m_fCurChargeRate = 0.0f;
	m_fCurChargeRateForSkill = 0.0f;
	m_dwSpecialMoveMacroTime_S = 0;
	m_dwSpecialMoveMacroTime_E = 0;

	m_iSkillInput = -1;
	m_bEnableDefenseState = false;
	m_bEnableAttackState = false;
	m_bEnableJumpState = false;

	ClearAttackState( pOwner );

	int iComboCnt = GetMaxCombo();
	if( !COMPARE( iCurCombo, 0, iComboCnt ) )
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

		int iAniID = pGrp->GetAnimationIdx( m_AttackReadyAniList[iCurCombo] );
		float fTimeRate = (float)m_dwChargeTime / pGrp->GetAnimationFullTime( iAniID );
		pGrp->SetActionAni( iAniID, FLOAT100, FLOAT100, FLOAT1, FLOAT1/fTimeRate, 0.0f, 0.0f, true );

		m_ChargeState = CS_CHARGING;
		m_dwAttackStartTime = FRAMEGETTIME();
		m_iCurCombo = iCurCombo;
		pOwner->SetState( CS_ATTACK );
		pOwner->SetAttackMoveEnable( false );
		pOwner->SetChargingState( true );
	}
}

void ioMagicEngineerItem::SetMacroTime( ioEntityGroup* pGrp, int iAniID, float fTimeRate, DWORD dwEndTime )
{
	m_dwSpecialMoveMacroTime_S = 0;
	m_dwSpecialMoveMacroTime_E = 0;
	if( !pGrp )
		return;

	DWORD dwCurTime = FRAMEGETTIME();
	DWORD dwSpecialMoveMacroTime_S = pGrp->GetAnimationEventTime_e( iAniID, "magic_eng_s" ) * fTimeRate;
	DWORD dwSpecialMoveMacroTime_E = pGrp->GetAnimationEventTime_e( iAniID, "magic_eng_e" ) * fTimeRate;
	if( dwSpecialMoveMacroTime_S )
	{
		m_dwSpecialMoveMacroTime_S = dwCurTime + dwSpecialMoveMacroTime_S;
		m_dwSpecialMoveMacroTime_E = dwCurTime + dwSpecialMoveMacroTime_E + dwEndTime;
	}
}

void ioMagicEngineerItem::CheckNormalAttack( ioBaseChar *pOwner )
{
	DWORD dwCurTime = FRAMEGETTIME();
	if( pOwner->IsNeedProcess() && m_dwSpecialMoveMacroTime_S && m_dwSpecialMoveMacroTime_S < dwCurTime && m_dwSpecialMoveMacroTime_E > dwCurTime )
	{
		if( pOwner->IsJumpKey() )
		{
			SetSpecailMoveState( pOwner, true, SM_UP );
			return;
		}
	}

	switch( m_ChargeState )
	{
	case CS_CHARGING:
		OnCharging( pOwner );
		break;
	case CS_GATHERING:
		OnGathering( pOwner );
		break;
	case CS_NORMAL_ATTACK:
		if( pOwner->IsNeedProcess() )
		{
			CheckCancelReserve( pOwner );

			if( ProcessCancel(pOwner) )
				return;
		}

		ioWeaponItem::CheckNormalAttack( pOwner );
		break;
	}
}

void ioMagicEngineerItem::OnCharging( ioBaseChar *pOwner )
{
	if( !pOwner ) 
		return;

	if( !pOwner->IsNeedProcess() )	
		return;

	if( pOwner->IsAttackKeyDown() && !pOwner->IsHasCrown() && m_iCurDummyCharIndex == -1 )
	{
		if( m_dwAttackStartTime + m_dwChargeTime <= FRAMEGETTIME() )
		{
			ChangeGatheringState( pOwner );
			return;
		}
	}
	else
	{
		ChangeToNormalAttack( pOwner );
	}
}

void ioMagicEngineerItem::OnGathering( ioBaseChar *pOwner )
{
	if( !pOwner )
		return;

	if( !pOwner->IsNeedProcess() )	
		return;

	if( pOwner->IsAttackKeyDown() && !pOwner->IsHasCrown() )
	{
		if( m_dwMotionEndTime <= FRAMEGETTIME() )
			SetAttackState( pOwner, true, false, false, false );
	}
	else
		SetAttackState( pOwner, true, false, false, true );
}

void ioMagicEngineerItem::ChangeToNormalAttack( ioBaseChar *pOwner )
{
	ioEntityGroup *pGrp = pOwner->GetGroup();
	if( !pGrp )
		return;

	ClearCancelInfo();

	pOwner->SetAttackMoveEnable( false );
	pOwner->SetChargingState( false );

	m_fCurChargeRate = 0.0f;
	m_fCurChargeRateForSkill = 0.0f;

	const AttackAttribute *pAttribute = m_FormalAttack.GetAttribute( AT_NORMAL, m_iCurCombo );
	if( pAttribute )
	{
		pOwner->SetNormalAttackByAttribute( *pAttribute );

		SetCurAttackAniForInputJump( pAttribute->m_AttackAnimation, pAttribute->m_fAttackAniRate );

		int iAniID = pGrp->GetAnimationIdx( pAttribute->m_AttackAnimation );
		SetMacroTime( pGrp, iAniID, pAttribute->m_fAttackAniRate, pAttribute->m_dwEndDelay );
	}

	pOwner->ApplyNormalAttackResult( m_iCurCombo );

	m_ChargeState = CS_NORMAL_ATTACK;

	if( pOwner->IsNeedSendNetwork() )
	{
		SP2Packet kPacket( CUPK_ITEM_GAUGE_OK );
		FillItemGaugePacketHeader( pOwner, kPacket );
		kPacket << m_ChargeState;
		kPacket << m_iCurCombo;
		P2PNetwork::SendToAllPlayingUser( kPacket );
	}
}

void ioMagicEngineerItem::ChangeGatheringState( ioBaseChar *pOwner )
{
	if( !pOwner ) return;

	ioEntityGroup *pGrp = pOwner->GetGroup();
	if( !pGrp )
		return;

	DWORD dwCurTime = FRAMEGETTIME();
	m_dwSpecialMoveMacroTime_S = 0;
	m_dwSpecialMoveMacroTime_E = 0;

	pGrp->ClearAllLoopAni( FLOAT100, true );
	pGrp->ClearAllActionAni( FLOAT100, true );

	int iAniID	     = pGrp->GetAnimationIdx( m_szGatheringAni );
	float fTimeRate  = m_fGatheringAniRate;

	pGrp->SetActionAni( iAniID, FLOAT100, FLOAT100, FLOAT1, FLOAT1/fTimeRate, 0.0f, 0.0f, true );

	m_ChargeState = CS_GATHERING;
	if( m_dwFullChargeTime == 0 )
		m_dwMotionEndTime = dwCurTime + pGrp->GetAnimationFullTime( iAniID ) * fTimeRate;
	else
		m_dwMotionEndTime = dwCurTime + m_dwFullChargeTime;

	if( pOwner->IsNeedSendNetwork() )
	{
		SP2Packet kPacket( CUPK_ITEM_GAUGE_OK );
		FillItemGaugePacketHeader( pOwner, kPacket );
		kPacket << m_ChargeState;
		kPacket << m_iCurCombo;
		kPacket << pOwner->GetWorldPosition();
		kPacket << pOwner->GetTargetRot();
		P2PNetwork::SendToAllPlayingUser( kPacket );
	}
}

void ioMagicEngineerItem::OnItemGaugeOK( ioBaseChar *pOwner, SP2Packet &rkPacket )
{
	int iState;
	D3DXVECTOR3 vPos;
	D3DXQUATERNION qRot;

	rkPacket >> iState;
	rkPacket >> m_iCurCombo;

	switch( iState )
	{
	case CS_NONE:
		pOwner->SetAttackMoveEnable( false );
		pOwner->SetChargingState( false );
		m_ChargeState = CS_NONE;
		break;
	case CS_NORMAL_ATTACK:
		ChangeToNormalAttack( pOwner );
		break;
	case CS_GATHERING:
		rkPacket >> vPos;
		rkPacket >> qRot;
		pOwner->SetWorldPosition( vPos );
		pOwner->SetTargetRotToRotate( qRot, true );
		ChangeGatheringState( pOwner );
		break;
	case CS_FLY:
		rkPacket >> vPos;
		rkPacket >> qRot;
		rkPacket >> m_iSpecialMoveDir;
		pOwner->SetWorldPosition( vPos );
		pOwner->SetTargetRotToRotate( qRot, true );
		SetSpecailMoveState( pOwner, false, m_iSpecialMoveDir );
		break;
	}
}

void ioMagicEngineerItem::OnItemGaugeCancel( ioBaseChar *pOwner, SP2Packet &rkPacket )
{
	if( !pOwner )
		return;

	int iState;
	D3DXVECTOR3 vPos;
	D3DXQUATERNION qtRot;

	rkPacket >> iState;
	rkPacket >> vPos;
	rkPacket >> qtRot;

	pOwner->SetWorldPosition( vPos );
	pOwner->SetTargetRotToRotate( qtRot, true );

	switch( iState )
	{
	case MFS_START:
		{
			bool bJump;
			bool bDash;
			bool bHalf;
			rkPacket >> bJump;
			rkPacket >> bDash;
			rkPacket >> bHalf;

			SetAttackState( pOwner, false, bJump, bDash, bHalf );
		}
		break;
	case MFS_END:
		SetChangeEndState( pOwner, false );
		break;
	}
}

void ioMagicEngineerItem::SetAttackState( ioBaseChar *pOwner, bool bSendNet, bool bJump, bool bDash, bool bHalf )
{
	if( !pOwner )
		return;

	ioEntityGroup *pGrp = pOwner->GetGroup();
	if( !pGrp )
		return;

	pOwner->SetState( CS_MAGICENGINEER_SPECIAL );
	m_bJump = bJump;

	if( bJump )
	{
		if( bHalf )
		{
			m_CurAttackInfo = m_JumpHalfChargeInfo;
			m_iMoveIndex = DMS_JUMPHALF;
		}
		else
		{
			m_CurAttackInfo = m_JumpFullChargeInfo;
			m_iMoveIndex = DMS_JUMP;
		}
	}
	else if( bDash )
	{
		if( bHalf )
		{
			m_CurAttackInfo = m_DashHalfChargeInfo;
			m_iMoveIndex = DMS_DASHHALF;
		}
		else
		{
			m_CurAttackInfo = m_DashFullChargeInfo;
			m_iMoveIndex = DMS_DASH;
		}
	}
	else
	{
		if( bHalf )
		{
			m_CurAttackInfo = m_HalfAttackInfo;
			m_iMoveIndex = DMS_HALF;
		}
		else
		{
			m_CurAttackInfo = m_AttackInfo;
			m_iMoveIndex = DMS_NORMAL;
		}
	}
	if( pOwner->IsNeedProcess() )
	{
		ioUserKeyInput::DirKeyInput eNewDirKey = pOwner->CheckCurDirKey();
		if( eNewDirKey != ioUserKeyInput::DKI_NONE )
			pOwner->SetTargetRotToDirKey( eNewDirKey );
	}
	ChangeAttackStart( pOwner );

	if( bSendNet && pOwner->IsNeedSendNetwork() )
	{
		SP2Packet kPacket( CUPK_ITEM_GAUGE_CANCEL );
		FillItemGaugePacketHeader( pOwner, kPacket );
		kPacket << m_MagicEngineerAttackState;
		kPacket << pOwner->GetWorldPosition();
		kPacket << pOwner->GetTargetRot();
		kPacket << bJump;
		kPacket << bDash;
		kPacket << bHalf;
		P2PNetwork::SendToAllPlayingUser( kPacket );
	}
}

void ioMagicEngineerItem::ChangeAttackStart( ioBaseChar *pOwner )
{
	if( !pOwner )
		return;

	ioEntityGroup *pGrp = pOwner->GetGroup();
	if( !pGrp )
		return;

	m_MagicEngineerAttackState = MFS_START;
	m_bCreateDummy = false;

	pGrp->ClearAllLoopAni( FLOAT10, true );
	pGrp->ClearAllActionAni( FLOAT10, true );

	int iAniID	     = pGrp->GetAnimationIdx( m_CurAttackInfo.m_StartAni );
	pGrp->SetActionAni( iAniID, FLOAT100, FLOAT100, FLOAT1, FLOAT1/m_CurAttackInfo.m_fAniRate, 0.0f, 0.0f, true );

	m_dwAttackStartTime = FRAMEGETTIME(); 
	m_dwFireSTime = m_dwAttackStartTime + ( pGrp->GetAnimationEventTime( iAniID, "fire_s" ) * m_CurAttackInfo.m_fAniRate );
	m_dwMotionEndTime = m_dwAttackStartTime + ( pGrp->GetAnimationFullTime( iAniID ) * m_CurAttackInfo.m_fAniRate );
}

void ioMagicEngineerItem::ProcessSpecialState( ioBaseChar *pOwner )
{
	if( !pOwner )
		return;

	ioPlayStage *pStage = pOwner->GetCreator();
	if( !pStage )
		return;

	DWORD dwCurTime = FRAMEGETTIME();

	switch( m_MagicEngineerAttackState )
	{
	case MFS_START:
		if( dwCurTime > m_dwFireSTime && !m_bCreateDummy )
		{
			MEDummyMoveInfo cInfo;
			if( m_vMoveInfo.find( m_iMoveIndex ) != m_vMoveInfo.end() )
				cInfo = m_vMoveInfo.find( m_iMoveIndex )->second;
			else if( m_vMoveInfo.find( DMS_NORMAL ) != m_vMoveInfo.end() )
				cInfo = m_vMoveInfo.find( DMS_NORMAL )->second;

			m_bCreateDummy = true;
			ChangeToDummyChar( pOwner, cInfo );
			SetMoveDummyPos( pOwner, cInfo );
		}
		if( m_iMoveIndex == DMS_JUMP || m_iMoveIndex == DMS_JUMPHALF )
		{
			pOwner->InitForcePower();
			pOwner->SetJumpPower( 0 );
			pOwner->SetAirJumpPower( 0 );
			pOwner->SetCurMoveSpeed( 0 );

			pOwner->SetCurJumpPowerAmt( 0.0f );
			pOwner->SetGravityAmt( 0.0f );
			pOwner->ClearReservedSliding();
			pOwner->SetForcePowerAmt( 0.0f );
		}
		if( dwCurTime > m_dwMotionEndTime )
			SetChangeEndState( pOwner, true );
		else if( m_bCreateDummy && IsEnableGauge() )
		{
			if( m_bJump && pOwner->IsDefenseKey() )
			{
				SetSpecailMoveState( pOwner, true, ioMagicEngineerItem::SM_DOWN );
				return;
			}
			else if( pOwner->IsJumpKey() )
			{
				SetSpecailMoveState( pOwner, true, ioMagicEngineerItem::SM_UP );
				return;
			}
		}
		break;

	case MFS_END:
		break;

	case MFS_FLY:
		pOwner->SetCurMoveSpeed( 0.f );
		if( !ProcessFlyStart(pOwner) )
		{
			ChangeSpecialMoveEndJump( pOwner );
		}
		break;
	case MFS_LAND:
		if ( m_dwMotionEndTime > 0 && m_dwMotionEndTime < FRAMEGETTIME() )
		{
			SetChangeEndState( pOwner, true );
		}
		break;
	}
}

bool ioMagicEngineerItem::ProcessFlyStart( ioBaseChar *pOwner )
{
	if( !pOwner )
		return false;

	DWORD dwCurTime = FRAMEGETTIME();
	float fTimePerSec = g_FrameTimer.GetSecPerFrame();

	D3DXVECTOR3 vTargetDir = pOwner->GetTargetRot() * D3DXVECTOR3( 0.0f, 0.0f, -FLOAT1 );
	D3DXVec3Normalize( &vTargetDir, &vTargetDir );

	D3DXVECTOR3 vAxis;
	D3DXVec3Cross( &vAxis, &vTargetDir, &ioMath::UNIT_Y );
	D3DXVec3Normalize( &vAxis, &vAxis );

	D3DXQUATERNION qtNewRot;
	float fCurAngle = ioMath::ArrangeHead( m_SpecialMoveInfo[m_iSpecialMoveDir].m_fStartAngle );
	D3DXQuaternionRotationAxis( &qtNewRot, &vAxis, DEGtoRAD(fCurAngle) );

	D3DXVECTOR3 vMoveDir = qtNewRot * vTargetDir;
	D3DXVec3Normalize( &vMoveDir, &vMoveDir );

	if( vMoveDir == ioMath::VEC3_ZERO )
		return false;

	if( m_SpecialMoveInfo[m_iSpecialMoveDir].m_fStartSpeed == 0.f )
		return false;

	float fCurSpeed = m_SpecialMoveInfo[m_iSpecialMoveDir].m_fStartSpeed;
	float fDistance = fCurSpeed * fTimePerSec;

	bool bRangeLimit = false;
	D3DXVECTOR3 vMoveAmt( 0.0f, 0.0f, 0.0f );
	if( m_fFlyCurRange+fDistance < m_SpecialMoveInfo[m_iSpecialMoveDir].m_fStartMaxRange )
	{
		bRangeLimit = true;
		vMoveAmt = vMoveDir * fDistance;

		m_fFlyCurRange += fDistance;
	}
	else
	{
		bRangeLimit = false;

		float fGapDistance = m_SpecialMoveInfo[m_iSpecialMoveDir].m_fStartMaxRange - m_fFlyCurRange;
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
		pOwner->SetWorldPosition( vPos );

	float fMapHeight = pStage->GetMapHeightOnlyTerrain( vPos.x, vPos.z );
	if ( vPos.y - fMapHeight < m_fJumpHeightGap )
	{
		SetChangeLandState( pOwner );
	}
	return bRangeLimit;
}

void ioMagicEngineerItem::ChangeSpecialMoveEndJump( ioBaseChar *pOwner )
{
	pOwner->SetCurJumpPowerAmt( 0.0f );
	pOwner->SetForcePowerAmt( 0.0f );
	pOwner->SetGravityAmt( 0.0f );
	pOwner->SetCurMoveSpeed( 0.0f );

	pOwner->SetSKillEndJumpState( m_SpecialMoveInfo[m_iSpecialMoveDir].m_fEndJumpAmt, false, false, false, true );
	pOwner->ClearDirDoubleClick();
}

void ioMagicEngineerItem::ClearAttackState( ioBaseChar *pOwner )
{
	m_MagicEngineerAttackState = MFS_NONE;

	m_CurAttackInfo.Init();
	m_dwAttackStartTime = 0;
	m_dwMotionEndTime = 0;
	m_dwFireSTime = 0;
}

void ioMagicEngineerItem::OnEquiped( ioBaseChar *pOwner )
{
	ioWeaponItem::OnEquiped( pOwner );

	m_iCurDummyCharIndex = -1;
}

void ioMagicEngineerItem::OnReleased( ioBaseChar *pOwner )
{
	ClearAttackState( pOwner );
	
	ioAttackableItem::OnReleased( pOwner );

	m_iCurDummyCharIndex = -1;
	m_fCurGauge = 0.0f;
}

void ioMagicEngineerItem::SetChangeEndState( ioBaseChar *pOwner, bool bSend )
{
	ClearAttackState( pOwner );
	pOwner->SetSKillEndJumpState( 0.1f, false, false, false );

	if( pOwner->IsNeedSendNetwork() && bSend )
	{
		SP2Packet kPacket( CUPK_ITEM_GAUGE_CANCEL );
		FillItemGaugePacketHeader( pOwner, kPacket );
		kPacket << MFS_END;
		kPacket << pOwner->GetWorldPosition();
		kPacket << pOwner->GetTargetRot();
		P2PNetwork::SendToAllPlayingUser( kPacket );
	}
}

void ioMagicEngineerItem::SetChangeLandState( ioBaseChar *pOwner )
{
	if( !pOwner )
		return;

	ioEntityGroup *pGrp = pOwner->GetGroup();
	if( !pGrp )
		return;

	ClearAttackState( pOwner );
	m_MagicEngineerAttackState = MFS_LAND;

	pGrp->ClearAllLoopAni( FLOAT10, true );
	pGrp->ClearAllActionAni( FLOAT10, true );

	int iAniID	     = pGrp->GetAnimationIdx( m_szLandingAni );
	pGrp->SetActionAni( iAniID, FLOAT100, FLOAT100, FLOAT1, FLOAT1/m_fLandingAniRate, 0.0f, 0.0f, true );

	m_dwMotionEndTime = FRAMEGETTIME() + ( pGrp->GetAnimationFullTime( iAniID ) * m_fLandingAniRate );
}

void ioMagicEngineerItem::ChangeToDummyChar( ioBaseChar *pOwner, const MEDummyMoveInfo& cInfo )
{
	if( !pOwner ) return;

	ioEntityGroup *pGrp = pOwner->GetGroup();
	if( !pGrp ) return;

	g_DummyCharMgr.DestroyDummyCharByNameList( pOwner, m_vDestroyDummyCharList );

	int iIndex = -1;
	D3DXVECTOR3 vTargetPos = ioMath::VEC3_ZERO;
	CreateDummyCharState( pOwner, cInfo, vTargetPos, iIndex );
}

void ioMagicEngineerItem::CreateDummyCharState( IN ioBaseChar *pOwner, const MEDummyMoveInfo& cInfo, OUT D3DXVECTOR3& vPos, OUT int& iIndex )
{
	if( m_iCurDummyCharIndex != -1 ) return;
	if( !pOwner ) return;

	ioPlayStage *pStage = pOwner->GetCreator();
	if( !pStage ) return;

	D3DXVECTOR3 vNewOffSet = pOwner->GetTargetRot() * cInfo.m_vDummyCharOffset;
	D3DXVECTOR3 vTargetPos = pOwner->GetMidPositionByRate() + vNewOffSet;

	int iCurIndex = g_DummyCharMgr.GetCurLocalIndex();

	vPos = vTargetPos;
	iIndex = iCurIndex;

	ioDummyChar *pDummy = pStage->CreateDummyChar( cInfo.m_DummyCharName, iCurIndex, 
		pOwner->GetCharName(), vTargetPos, 0.0f, 0, true );

	if( pDummy )
		m_iCurDummyCharIndex = iCurIndex;

	if( !m_CurExtendEquipEffect.IsEmpty() )
	{
		pOwner->EndEffect( m_CurExtendEquipEffect, false );
		m_CurExtendEquipEffect.Clear();
	}
}

void ioMagicEngineerItem::CheckDummyCharState( ioBaseChar *pOwner, int iIndex )
{
	if( m_iCurDummyCharIndex == -1 )
		return;
	if( !pOwner ) 
		return;

	if( m_iCurDummyCharIndex == iIndex )
		m_iCurDummyCharIndex = -1;
}

void ioMagicEngineerItem::DestroyDummyChar( ioBaseChar *pOwner )
{
	g_DummyCharMgr.DestroyDummyCharByNameList( pOwner, m_vDestroyDummyCharList );
}

void ioMagicEngineerItem::SetMoveDummyPos( ioBaseChar *pOwner, const MEDummyMoveInfo& cInfo )
{
	ioDummyChar *pDummy = g_DummyCharMgr.FindDummyChar( pOwner, m_iCurDummyCharIndex );
	if( pDummy && pOwner )
	{
		ioMoveAttack3Dummy* pMoveDummy3 = ToioMoveAttack3Dummy( pDummy );
		if( pMoveDummy3 )
		{
			D3DXVECTOR3 vNewOffSet = pOwner->GetTargetRot() * cInfo.m_vDummyCharOffset;
			D3DXVECTOR3 vPos = pOwner->GetMidPositionByRate() + vNewOffSet;

			D3DXVECTOR3 vDir = pOwner->GetTargetRot() * D3DXVECTOR3( 0.0f, 0.0f, -FLOAT1 );

			D3DXVec3Normalize(&vDir, &vDir);
			float fAngleH = ioMath::ArrangeHead( cInfo.m_fRotate_x );
			float fAngleV = ioMath::ArrangeHead( cInfo.m_fRotate_y );

			D3DXQUATERNION qtRot;
			D3DXVECTOR3 vAxis;
			D3DXVec3Cross( &vAxis, &vDir, &D3DXVECTOR3( 0.0f, -FLOAT1, 0.0f ) );

			// vertical
			D3DXQuaternionRotationAxis( &qtRot, &vAxis, DEGtoRAD(fAngleV) );
			vDir = qtRot * vDir;

			// horizon
			D3DXQuaternionRotationAxis( &qtRot, &ioMath::UNIT_Y, DEGtoRAD(fAngleH) );
			vDir = qtRot * vDir;

			pMoveDummy3->SetMovePosition( vPos + vDir * cInfo.m_fLength );
		}
	}
}

void ioMagicEngineerItem::SetSpecailMoveState( ioBaseChar *pOwner, bool bSendNet, int iDir )
{
	if( !pOwner )
		return;

	ioEntityGroup *pGrp = pOwner->GetGroup();
	if( !pGrp )
		return;

	m_bJump = false;
	if( pOwner->IsNeedProcess() && pOwner->IsSettedDirection() )
	{
		ioUserKeyInput::DirKeyInput eNewDirKey = pOwner->CheckCurDirKey();
		pOwner->SetTargetRotToDirKey( eNewDirKey );
	}
	
	pOwner->SetState( CS_MAGICENGINEER_SPECIAL );
	m_ChargeState = CS_FLY;
	m_MagicEngineerAttackState = MFS_FLY;
	m_iSpecialMoveDir = iDir;

	DecreaseGauge();
	pOwner->ClearReserveKeyInput();
	pOwner->InitExtendAttackTagInfo();

	pGrp->ClearAllLoopAni( FLOAT10, true );
	pGrp->ClearAllActionAni( FLOAT10, true );

	int iAniID	     = pGrp->GetAnimationIdx( m_SpecialMoveInfo[m_iSpecialMoveDir].m_StartAni );
	float fTimeRate  = m_SpecialMoveInfo[m_iSpecialMoveDir].m_fStartAniRate;
	DWORD dwPreDelay = 0;
	DWORD dwEndDelay = 0;

	pGrp->SetActionAni( iAniID, FLOAT10, FLOAT10, FLOAT1, FLOAT1/fTimeRate, 0.0f, dwPreDelay, true );
	m_dwSpecialMoveStartTime = FRAMEGETTIME();
	m_dwSpecialMoveEndTime = 0;	
	m_fFlyCurRange = 0.f;

	if(pOwner->IsNeedProcess() && bSendNet )
	{
		//Send
		SP2Packet kPacket( CUPK_ITEM_GAUGE_OK );
		FillItemGaugePacketHeader( pOwner, kPacket );
		kPacket << m_ChargeState;
		kPacket << m_iCurCombo;
		kPacket << pOwner->GetWorldPosition();
		kPacket << pOwner->GetTargetRot();
		kPacket << m_iSpecialMoveDir;
		P2PNetwork::SendToAllPlayingUser( kPacket );
	}
}

int ioMagicEngineerItem::GetMaxBullet()
{
	return (int)m_fMaxGauge;
}

int ioMagicEngineerItem::GetNeedBullet()
{
	return (int)m_fNeedGauge;
}

int ioMagicEngineerItem::GetCurBullet()
{
	float fCurCoolTime = m_fCurGauge;
	fCurCoolTime = max( 0, min( fCurCoolTime, m_fMaxGauge ) );

	return (int)fCurCoolTime;
}

void ioMagicEngineerItem::SetCurBullet( int iCurBullet )
{
	if( iCurBullet < 0 )
		return;

	float fCurBullet = iCurBullet;
	fCurBullet = max( 0, min( fCurBullet, m_fMaxGauge ) );

	m_fCurGauge = fCurBullet;
}

void ioMagicEngineerItem::InitExtraGauge()
{
	m_fCurGauge = 0.0f;
}

void ioMagicEngineerItem::MaxExtraGauge()
{
	m_fCurGauge = m_fMaxGauge;
}

float ioMagicEngineerItem::GetMaxExtraGauge()
{
	return m_fMaxGauge;
}

float ioMagicEngineerItem::GetCurExtraGauge()
{
	return m_fCurGauge;
}

void ioMagicEngineerItem::SetCurExtraGauge( float fGauge )
{
	m_fCurGauge = fGauge;
	m_fCurGauge = (float)max( 0, min( m_fCurGauge, m_fMaxGauge ) );
}

void ioMagicEngineerItem::DecreaseGauge()
{
	m_fCurGauge -= m_fNeedGauge;
	m_fCurGauge = (float)max( 0, min( m_fCurGauge, m_fMaxGauge ) );
}

bool ioMagicEngineerItem::IsEnableGauge()
{
	if( m_fCurGauge >= m_fNeedGauge )
		return true;

	return false;
}

void ioMagicEngineerItem::UpdateExtraData( ioBaseChar *pOwner )
{
	if( !pOwner ) return;

	// check owner state
	float fTimePerSec = g_FrameTimer.GetSecPerFrame();

	switch( pOwner->GetState() )
	{
	case CS_DIE:
	case CS_DROP_ZONE_DOWN:
	case CS_READY:
	case CS_VIEW:
	case CS_OBSERVER:
	case CS_LOADING:
	case CS_FISHING:
	case CS_STARTMOTION:
	case CS_ENDMOTION:
	case CS_EXCAVATING:
	case CS_CREATEMOTION:
	case CS_ROULETTE:
		return;
	case CS_DELAY:
	case CS_RUN:
	case CS_DASH:
		if( m_fCurGauge < m_fMaxGauge )
		{
			float fGauge = m_fIncreaseGauge * fTimePerSec;

			m_fCurGauge += fGauge;
			m_fCurGauge = min( m_fCurGauge, m_fMaxGauge );
		}
		return;
	}
}
//////////////////////////////////////////////////////////////////////////
ioMagicEngineerSpecialState::ioMagicEngineerSpecialState()
{
}

ioMagicEngineerSpecialState::~ioMagicEngineerSpecialState()
{
}

void ioMagicEngineerSpecialState::Initialize( ioBaseChar* pOwner, CharState eCurrState )
{
	ioSpecialStateBase::Initialize( pOwner, eCurrState );
}

void ioMagicEngineerSpecialState::Finalize( ioBaseChar* pOwner, CharState eCurrState, CharState eNewState )
{
	ioSpecialStateBase::Finalize( pOwner, eCurrState, eNewState );

	ioMagicEngineerItem *pMagicEngineer = ToMagicEngineerItem( pOwner->GetPriorityItem( SPT_ATTACK ) );
	if( pMagicEngineer )
		pMagicEngineer->ClearAttackState( pOwner );
}

void ioMagicEngineerSpecialState::CheckSpecialState( ioBaseChar* pOwner )
{
	ioMagicEngineerItem *pMagicEngineer = ToMagicEngineerItem( pOwner->GetPriorityItem( SPT_ATTACK ) );
	if( pMagicEngineer )
		pMagicEngineer->ProcessSpecialState( pOwner );
	else
	{
		pOwner->SetState( CS_DELAY );
		pOwner->SetFallState( true );
		return;
	}
}

void ioMagicEngineerSpecialState::CheckFallStateChange( ioBaseChar* pOwner, float fMinFallHeightRate, float fBottomHeight, bool bBottomMaxCheck, float fMaxBottomHeight )
{
}

bool ioMagicEngineerSpecialState::IsCharCollisionSkipState( const ioBaseChar *const pOwner, DWORD dwStartTime, DWORD dwEndTime, DWORD dwSkipType, bool bTeamOnly, bool bDefense,
	TeamType eOwnerTeam, TeamType eOtherTeam ) const
{
	ioMagicEngineerItem *pMagicEngineer = ToMagicEngineerItem( pOwner->GetPriorityItem( SPT_ATTACK ) );
	if( pMagicEngineer && pMagicEngineer->GetMagicEngineerState() == ioMagicEngineerItem::MFS_FLY )
		return true;

	return ioSpecialStateBase::IsCharCollisionSkipState( pOwner, dwStartTime, dwEndTime, dwSkipType, bTeamOnly, bDefense, eOwnerTeam, eOtherTeam );
}