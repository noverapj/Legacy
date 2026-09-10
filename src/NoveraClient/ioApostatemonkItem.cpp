#include "StdAfx.h"
#include "ioApostatemonkItem.h"

#include "ioPlayStage.h"
#include "ioBaseChar.h"

#include "ioDrinkItem.h"
#include "ioZoneEffectWeapon.h"


ioApostatemonkItem::ioApostatemonkItem(void)
{
	ClearData();
	m_MonkState = MS_NORMAL;
	m_fCurBullet = 0.0f;
}

ioApostatemonkItem::ioApostatemonkItem( const ioApostatemonkItem &rhs ) :
ioWeaponItem( rhs ),
m_szRemoveBuffList( rhs.m_szRemoveBuffList ),
//Normal
m_AttackReadyAniList( rhs.m_AttackReadyAniList ),
m_iMaxApostateApostateReadyCount( rhs.m_iMaxApostateApostateReadyCount ),
m_vApostateReadyList( rhs.m_vApostateReadyList ),
m_szCheckApostateBuff( rhs.m_szCheckApostateBuff ),
m_szCheckRelaxBuff( rhs.m_szCheckRelaxBuff ),
m_ApostateAttack( rhs.m_ApostateAttack ),
m_dwApostateAttackRotateSpeed( rhs.m_dwApostateAttackRotateSpeed ),
m_fApostateAttackRunSpeed( rhs.m_fApostateAttackRunSpeed ),
m_ApostateAddAttack( rhs.m_ApostateAddAttack ),
m_szApostateAttackEndAni( rhs.m_szApostateAttackEndAni ),
m_fApostateAttackEndAniRate( rhs.m_fApostateAttackEndAniRate ),
m_RelaxAttack( rhs.m_RelaxAttack ),
m_ExtendDash( rhs.m_ExtendDash ),
m_ApostateExtendDashList( rhs.m_ApostateExtendDashList ),
m_dwApostateExtendDashChargeTime( rhs.m_dwApostateExtendDashChargeTime ),
m_JumpDashAttack( rhs.m_JumpDashAttack ),
m_JumpDashLandAttack( rhs.m_JumpDashLandAttack ),
m_dwLandAttackChargeTime( rhs.m_dwLandAttackChargeTime ),
m_fJumpDashEnableHeightGap( rhs.m_fJumpDashEnableHeightGap ),
m_fJumpDashAttackGravity( rhs.m_fJumpDashAttackGravity ),
m_fMaxBullet( rhs.m_fMaxBullet ),
m_fIncreaseApostateReadyGauge( rhs.m_fIncreaseApostateReadyGauge ),
m_fDecreaseRelaxStateGaugeTic( rhs.m_fDecreaseRelaxStateGaugeTic ),
m_szAfterEffectBuff( rhs.m_szAfterEffectBuff )
{
	ClearData();
	m_MonkState = MS_NORMAL;
	m_fCurBullet = 0.0f;
}

ioApostatemonkItem::~ioApostatemonkItem(void)
{
}

ioItem* ioApostatemonkItem::Clone()
{
	return new ioApostatemonkItem( *this );
}

ioWeaponItem::WeaponSubType ioApostatemonkItem::GetSubType() const
{
	return WST_APOSTATE_MONK_ITEM;
}

void ioApostatemonkItem::ClearData()
{
	m_iCurCombo = 0;
	m_ChargeState = CS_NONE;
	m_ApostateSpecialState = ASS_NONE;
	m_dwMotionStartTime = 0;
	m_dwMotionEndTime = 0;
	m_CurDirKey = ioUserKeyInput::DKI_NONE;
	m_iCurApostateExtendCombo = 0;
	m_dwApostateExtendDashChargeStartTime = 0;
	m_dwLandAttackChargeStartTime = 0;
	m_bReserveApostateAttack = false;
}

void ioApostatemonkItem::LoadProperty( ioINILoader &rkLoader )
{
	ioWeaponItem::LoadProperty( rkLoader );

	char szBuf[MAX_PATH] = "";
	char szKey[MAX_PATH] = "";

	//Remove Buff
	int iBuffCount = rkLoader.LoadInt_e( "remove_buff_count", 0 );
	if ( iBuffCount != 0 )
	{
		m_szRemoveBuffList.reserve( iBuffCount );
		for ( int i=0; i<iBuffCount ; ++i )
		{
			wsprintf_e( szBuf, "remove_buff%d", i+1 );
			rkLoader.LoadString( szBuf, "", szKey, MAX_PATH );
			m_szRemoveBuffList.push_back( ioHashString(szKey) );
		}
	}
	

	//Normal
	int iMaxCombo = GetMaxCombo();
	m_AttackReadyAniList.clear();
	if ( iMaxCombo > 0 )
	{
		m_AttackReadyAniList.reserve( iMaxCombo );
		for( int i=0; i < iMaxCombo; ++i )
		{
			wsprintf_e( szBuf, "attack_ready_animation%d", i+1 );
			rkLoader.LoadString( szBuf, "", szKey, MAX_PATH );
			m_AttackReadyAniList.push_back( ioHashString(szKey) );
		}
	}

	LoadApostateReadyState( rkLoader );

	//Apostate
	rkLoader.LoadString_e( "check_apostate_buff_name", "", szBuf, MAX_PATH );
	m_szCheckApostateBuff = szBuf;
	LoadAttackAttribute_e( "apostate_attack", m_ApostateAttack, rkLoader );
	LoadAttackAttribute_e( "apostate_add_attack", m_ApostateAddAttack, rkLoader );
	m_dwApostateAttackRotateSpeed = (DWORD)rkLoader.LoadInt_e( "apostate_attack_rotate_speed", 0 );
	m_fApostateAttackRunSpeed = rkLoader.LoadFloat_e( "apostate_attack_run_speed", 0.0f );
	
	rkLoader.LoadString_e( "apostate_attack_end_ani", "", szBuf, MAX_PATH );
	m_szApostateAttackEndAni = szBuf;
	m_fApostateAttackEndAniRate = rkLoader.LoadFloat_e( "apostate_attack_end_ani_rate", 0.0f );

	//Apostate Relax State
	rkLoader.LoadString_e( "check_relax_buff_name", "", szBuf, MAX_PATH );
	m_szCheckRelaxBuff = szBuf;
	LoadAttackAttribute_e( "relax_attack", m_RelaxAttack, rkLoader );

	//Extend Dash
	LoadAttackAttribute_e( "extend_dash", m_ExtendDash, rkLoader );
	
	//Apostate Extend Dash
	for ( int i=0 ; i<10 ; ++i )
	{
		wsprintf_e( szBuf, "apostate_extend_dash%d", i+1 );
		rkLoader.LoadString( szBuf, "", szKey, MAX_PATH );
		if( strcmp( szKey, "" ) == 0) 
			break;
		AttackAttribute att;
		LoadAttackAttribute( szBuf, att, rkLoader );
		m_ApostateExtendDashList.push_back( att );
	}

	LoadAttackAttribute_e( "jump_dash_attack", m_JumpDashAttack, rkLoader );
	LoadAttackAttribute_e( "jump_dash_land_attack", m_JumpDashLandAttack, rkLoader );

	m_dwLandAttackChargeTime = rkLoader.LoadInt_e( "land_attack_charge_time", 0 );
	m_fJumpDashEnableHeightGap = rkLoader.LoadFloat_e( "jump_dash_enable_height_gap", 0.0f );
	m_fJumpDashAttackGravity = rkLoader.LoadFloat_e( "jump_dash_attack_gravity", 0.0f );

	//Gauge
	m_fMaxBullet = rkLoader.LoadFloat_e( "max_gauge", 0 );
	m_fIncreaseApostateReadyGauge = rkLoader.LoadFloat_e( "increase_apostate_ready_gauge", 0.0f );
	m_fDecreaseRelaxStateGaugeTic = rkLoader.LoadFloat_e( "decrease_relax_state_gauge_tic", 0.0f );


	//후유증
	rkLoader.LoadString_e( "after_effect_buff_name", "", szBuf, MAX_PATH );
	m_szAfterEffectBuff = szBuf;
}

void ioApostatemonkItem::LoadApostateReadyState( ioINILoader &rkLoader )
{
	char szBuf[MAX_PATH] = "";
	char szKey[MAX_PATH] = "";
	m_iMaxApostateApostateReadyCount = rkLoader.LoadInt_e( "max_apostate_ready_count", 0 );
	if ( m_iMaxApostateApostateReadyCount <= 0 )
	{
		LOG.PrintTimeAndLog( 0, "%s : max_apostate_ready_count value is empty ", __FUNCTION__ );
		return;
	}

	//Apostate Ready
	m_vApostateReadyList.clear();
	m_vApostateReadyList.reserve( m_iMaxApostateApostateReadyCount );
	for ( int i=0 ; i<m_iMaxApostateApostateReadyCount ; ++i )
	{
		ApostateReady info;

		//Ani
		wsprintf_e( szBuf, "apostate_ready%d_animation", i+1 );
		rkLoader.LoadString( szBuf, "", szKey, MAX_PATH );
		info.m_szReayAni = szKey;

		wsprintf_e( szBuf, "apostate_ready%d_ani_rate", i+1 );
		info.m_fReayAniRate = rkLoader.LoadFloat( szBuf, 0.0f );

		//Check Buff
		wsprintf_e( szBuf, "apostate_ready%d_check_buff", i+1 );
		rkLoader.LoadString( szBuf, "", szKey, MAX_PATH );
		info.m_szCheckBuff = szKey;

		//Start Buff List
		wsprintf_e( szBuf, "apostate_ready%d_start_buff_count", i+1 );
		int	iMaxStartBuffCount = rkLoader.LoadInt( szBuf, 0 );
		if ( iMaxStartBuffCount <= 0 )
		{
			LOG.PrintTimeAndLog( 0, "%s : %s value is empty ", __FUNCTION__, szBuf );
			//continue;
		}

		info.m_szStartBuffList.clear();
		info.m_szStartBuffList.reserve( iMaxStartBuffCount );
		for( int j=0 ; j< iMaxStartBuffCount ; ++j )
		{
			wsprintf_e( szBuf, "apostate_ready%d_start_buff%d", i+1, j+1 );
			rkLoader.LoadString( szBuf, "", szKey, MAX_PATH );
			info.m_szStartBuffList.push_back( szKey );
		}

		//End Buff List
		wsprintf_e( szBuf, "apostate_ready%d_end_buff_count", i+1 );
		int	iMaxEndBuffCount = rkLoader.LoadInt( szBuf, 0 );
		if ( iMaxEndBuffCount <= 0 )
		{
			LOG.PrintTimeAndLog( 0, "%s : %s value is empty ", __FUNCTION__, szBuf );
			//continue;
		}

		info.m_szEndBuffList.clear();
		info.m_szEndBuffList.reserve( iMaxEndBuffCount );
		for( int j=0 ; j< iMaxEndBuffCount ; ++j )
		{
			wsprintf_e( szBuf, "apostate_ready%d_end_buff%d", i+1, j+1 );
			rkLoader.LoadString( szBuf, "", szKey, MAX_PATH );
			info.m_szEndBuffList.push_back( szKey );
		}

		m_vApostateReadyList.push_back( info );
	}

	m_dwApostateExtendDashChargeTime = (DWORD)rkLoader.LoadInt_e( "apostate_extend_dash_charge_time", 0 );
}

void ioApostatemonkItem::OnEquiped( ioBaseChar *pOwner )
{
	SetCurBullet( 0 );
	ioWeaponItem::OnEquiped( pOwner );
}

void ioApostatemonkItem::OnReleased( ioBaseChar *pOwner )
{
	SetCurBullet( 0 );
	
	//광분상태일 경우만
	if ( pOwner->HasBuff( pOwner->GetCharName(), m_szCheckApostateBuff ) )
		AddBuff( pOwner, m_szAfterEffectBuff );

	RemoveBuffList( pOwner, m_szRemoveBuffList );

	ioWeaponItem::OnReleased( pOwner );
}

//Buff
void ioApostatemonkItem::AddBuff( ioBaseChar *pOwner, const ioHashString& szBuffName )
{
	if ( !pOwner )
		return;

	pOwner->AddNewBuff( szBuffName, pOwner->GetCharName(), GetName(), NULL, false );
}

void ioApostatemonkItem::AddBuffList( ioBaseChar *pOwner, const ioHashStringVec& vBuffList )
{
	if ( !pOwner )
		return;

	for ( int i=0 ; i<(int)vBuffList.size() ; ++i )
		AddBuff( pOwner, vBuffList[i] );
}

void ioApostatemonkItem::RemoveBuff( ioBaseChar *pOwner, const ioHashString& szBuffName )
{
	if ( !pOwner )
		return;

	pOwner->RemoveBuff( szBuffName );
}

void ioApostatemonkItem::RemoveBuffList( ioBaseChar *pOwner, const ioHashStringVec& vBuffList )
{
	if ( !pOwner )
		return;

	for ( int i=0 ; i<(int)vBuffList.size() ; ++i )
		RemoveBuff( pOwner, vBuffList[i] );
}
//

int ioApostatemonkItem::SetAttackAni( ioBaseChar *pOwner, const AttackAttribute& rkAtt, bool bUseTracking )
{
	if ( !pOwner )
		return -1;

	ioEntityGroup *pGrp = pOwner->GetGroup();
	if( !pGrp )
		return -1;

	int iAniID = pGrp->GetAnimationIdx( rkAtt.m_AttackAnimation );
	if ( iAniID == -1 )
		return -1;

	DWORD dwCurTime = FRAMEGETTIME();

	float fTimeRate = rkAtt.m_fAttackAniRate;
	if( fTimeRate <= 0.0f )
		fTimeRate = FLOAT1;

	DWORD dwPreDelay = rkAtt.m_dwPreDelay;
	DWORD dwStartTime = dwCurTime + dwPreDelay;

	pGrp->ClearAllLoopAni( FLOAT10, true );
	pGrp->ClearAllActionAni( FLOAT10, true );
	pGrp->SetActionAni( iAniID, FLOAT100, FLOAT100, FLOAT1, FLOAT1/fTimeRate, 0.0f, dwPreDelay );

	pOwner->SetReservedSliding( rkAtt.m_vForceInfoList, iAniID, fTimeRate, dwPreDelay );

	DWORD dwCurBaseIndex = pOwner->IncreaseWeaponIndexBase();

	pOwner->RefreshFireTimeListWithBase( dwCurBaseIndex,
		iAniID,
		rkAtt.m_vWeaponInfoList,
		FTT_NORMAL_ATTACK,
		fTimeRate,
		dwPreDelay );

	m_dwMotionEndTime = dwStartTime;
	m_dwMotionEndTime += pGrp->GetAnimationFullTime( iAniID ) * fTimeRate;
	m_dwMotionEndTime += rkAtt.m_dwEndDelay;

	m_dwKeyReserveTime = pGrp->GetAnimationEventTime_e( iAniID, "key_reserve" ) * fTimeRate;
	if( m_dwKeyReserveTime > 0 )
		m_dwKeyReserveTime += dwStartTime;

	SetCurAttackAniForInputJump( rkAtt.m_AttackAnimation, fTimeRate );

	pOwner->CheckCharColSkipTime( iAniID, fTimeRate, dwPreDelay );

	pOwner->SetAutoTarget( ATT_NORMAL );
	DWORD dwTrackingTime = dwCurTime;
	dwTrackingTime += pGrp->GetAnimationEventTime_e( iAniID, "tracking_e" ) * fTimeRate;
	pOwner->SetAutoTargetTrackingEndTime( dwTrackingTime );

	return iAniID;
}

void ioApostatemonkItem::CheckMonkState( ioBaseChar *pOwner )
{
	m_MonkState = MS_NORMAL;
	if ( !pOwner )
		return;

	//광분 상태 체크
	if ( pOwner->HasBuff( pOwner->GetCharName(), m_szCheckApostateBuff ) )
		m_MonkState = MS_APOSTATE;
	//휴식 상태 체크
	else if ( pOwner->HasBuff( pOwner->GetCharName(), m_szCheckRelaxBuff ) )
		m_MonkState = MS_RELAX;

	if( pOwner->IsNeedSendNetwork() )
	{
		SP2Packet kPacket( CUPK_ITEM_GAUGE_OK );
		FillItemGaugePacketHeader( pOwner, kPacket );
		kPacket << static_cast<BYTE>( SS_CHECK_MONK_STATE );
		kPacket << static_cast<BYTE>( m_MonkState );
		P2PNetwork::SendToAllPlayingUser( kPacket );
	}
}

//Normal Attack
void ioApostatemonkItem::SetNormalAttack( ioBaseChar *pOwner, int iCurCombo )
{
	if( !pOwner )
		return;

	m_fCurChargeRate = 0.0f;
	m_fCurChargeRateForSkill = 0.0f;

	ClearCancelInfo();
	ClearData();
	if ( pOwner->IsNeedProcess() )
		CheckMonkState( pOwner );
	

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
		m_dwMotionStartTime = FRAMEGETTIME();
		m_iCurCombo = iCurCombo;
		pOwner->SetState( CS_ATTACK );
		pOwner->SetAttackMoveEnable( false );
		pOwner->SetChargingState( true );

		if( m_bChargeAutoTarget )
		{
			DWORD dwTrackingTime = FRAMEGETTIME();
			dwTrackingTime += pGrp->GetAnimationEventTime_e( iAniID, "tracking_e" ) * fTimeRate;
			pOwner->SetAutoTargetTrackingEndTime( dwTrackingTime );
		}
		else
			pOwner->StopAutoTargetTracking();
	}
}

void ioApostatemonkItem::ProcessNormalAttackState( ioBaseChar *pOwner )
{
	if( pOwner->IsNeedProcess() )
	{	
		CheckCancelReserve( pOwner );		
		if( ProcessCancel(pOwner) )
			return;
	}

	ioWeaponItem::CheckNormalAttack( pOwner );
}

void ioApostatemonkItem::ChangeNormalAttackState( ioBaseChar *pOwner )
{
	DWORD dwCurTime = FRAMEGETTIME();

	pOwner->InitExtendAttackTagInfo();
	pOwner->InitActionStopDelay( true );
	ClearCancelInfo();

	pOwner->SetAttackMoveEnable( false );
	pOwner->SetChargingState( false );

	m_fCurChargeRate = 0.0f;
	m_fCurChargeRateForSkill = 0.0f;

	const AttackAttribute *pAttribute = m_FormalAttack.GetAttribute( AT_NORMAL, m_iCurCombo );
	if( pAttribute )
	{
		if( !m_bChargeAutoTarget )
			pOwner->SetAutoTarget( ATT_NORMAL );

		pOwner->SetNormalAttackByAttribute( *pAttribute );

		SetCurAttackAniForInputJump( pAttribute->m_AttackAnimation, pAttribute->m_fAttackAniRate );
	}

	pOwner->ApplyNormalAttackResult( m_iCurCombo );

	m_ChargeState = CS_NORMAL_ATTACK;

	if( pOwner->IsNeedSendNetwork() )
	{
		SP2Packet kPacket( CUPK_ITEM_GAUGE_OK );
		FillItemGaugePacketHeader( pOwner, kPacket );
		kPacket << static_cast<BYTE>( SS_NORMAL_ATTACK );
		kPacket << pOwner->GetWorldPosition();
		kPacket << pOwner->GetTargetRot();
		kPacket << m_iCurCombo;
		if( !m_bChargeAutoTarget )
			pOwner->FillAutoTargetInfo( kPacket );
		P2PNetwork::SendToAllPlayingUser( kPacket );
	}
}

void ioApostatemonkItem::CheckNormalAttack( ioBaseChar *pOwner )
{
	switch( m_MonkState )
	{
	case MS_NORMAL:
		ProcessNormalState( pOwner );
		break;
	case MS_APOSTATE:
		ProcessApostateState( pOwner );
		break;
	case MS_RELAX:
		ProcessRelaxState( pOwner );
		break;
	}
}

void ioApostatemonkItem::ProcessNormalState( ioBaseChar *pOwner )
{
	switch( m_ChargeState )
	{
	case CS_CHARGING:
		ProcessNormalChargingState( pOwner );
		break;
	case CS_NORMAL_ATTACK:
		ProcessNormalAttackState( pOwner );
		break;
	case CS_APOSTATE_READY:
		ProcessNormalApostateReadyState( pOwner );
		break;
	}
}

void ioApostatemonkItem::ProcessNormalChargingState( ioBaseChar *pOwner )
{
	if( !pOwner || !pOwner->IsNeedProcess() )
		return;

	if( pOwner->IsAttackKeyDown() && !pOwner->IsHasCrown() )
	{
		if( m_dwMotionStartTime + m_dwChargeTime <= FRAMEGETTIME() )
		{
			CheckApostateReadyCount( pOwner );
			ChangeNormalApostateReadyState( pOwner );
			return;
		}
	}
	else
		ChangeNormalAttackState( pOwner );
}

void ioApostatemonkItem::CheckApostateReadyCount( ioBaseChar *pOwner )
{
	if ( !pOwner )
		return;

	m_iCurApostateApostateReadyCount = 0;

	for ( int i=0 ; i<(int)m_vApostateReadyList.size() ; ++i )
	{
		if ( pOwner->HasBuff( pOwner->GetCharName(), m_vApostateReadyList[i].m_szCheckBuff ) )
		{
			m_iCurApostateApostateReadyCount = i + 1;
			return;
		}
	}
}

void ioApostatemonkItem::ChangeNormalApostateReadyState( ioBaseChar *pOwner )
{
	if ( !pOwner || !COMPARE( m_iCurApostateApostateReadyCount, 0, (int)m_vApostateReadyList.size() ) )
		return;

	ioEntityGroup *pGrp = pOwner->GetGroup();
	if ( !pGrp )
		return;

	const ApostateReady& rkApostateReady = m_vApostateReadyList[m_iCurApostateApostateReadyCount];
	int iAniID = pGrp->GetAnimationIdx( rkApostateReady.m_szReayAni );
	if ( iAniID == -1 )
		return;

	m_ChargeState = CS_APOSTATE_READY;
		
	ClearCancelInfo();
	pOwner->ClearReserveKeyInput();
	pOwner->ClearReservedSliding();
	pOwner->SetChargingState( false );

	float fTimeRate = rkApostateReady.m_fReayAniRate;
	if( fTimeRate <= 0.0f )
		fTimeRate = FLOAT1;

	DWORD dwCurTime = FRAMEGETTIME();

	pGrp->ClearAllLoopAni( FLOAT10, true );
	pGrp->ClearAllActionAni( FLOAT10, true );
	pGrp->SetActionAni( iAniID, FLOAT100, FLOAT100, FLOAT1, FLOAT1/fTimeRate, 0.0f, 0 );

	m_dwMotionEndTime = dwCurTime + pGrp->GetAnimationFullTime( iAniID ) * fTimeRate;

	SetCurAttackAniForInputJump( rkApostateReady.m_szReayAni, fTimeRate );

	pOwner->CheckCharColSkipTime( iAniID, fTimeRate, 0 );

	//Add New Start Buff
	AddBuffList( pOwner, m_vApostateReadyList[m_iCurApostateApostateReadyCount].m_szStartBuffList );

	if( pOwner->IsNeedSendNetwork() )
	{
		SP2Packet kPacket( CUPK_ITEM_GAUGE_OK );
		FillItemGaugePacketHeader( pOwner, kPacket );
		kPacket << static_cast<BYTE>( SS_APOSTATE_READY );
		kPacket << pOwner->GetWorldPosition();
		kPacket << pOwner->GetWorldOrientation();
		kPacket << m_iCurApostateApostateReadyCount;
		P2PNetwork::SendToAllPlayingUser( kPacket );
	}

	IncreaseGauge( m_fIncreaseApostateReadyGauge );
}

void ioApostatemonkItem::ProcessNormalApostateReadyState( ioBaseChar *pOwner )
{
	if ( !pOwner || !pOwner->IsNeedProcess() )
		return;

	DWORD dwCurTime = FRAMEGETTIME();
	if ( m_dwMotionEndTime < dwCurTime )
	{
		SetApostateReadyEndState( pOwner );
		return;
	}
}

void ioApostatemonkItem::SetApostateReadyEndState( ioBaseChar *pOwner )
{
	if ( !pOwner )
		return;

	//Delete before array end buff
	int iBeforeArray = m_iCurApostateApostateReadyCount - 1;
	if ( COMPARE( iBeforeArray, 0, (int)m_vApostateReadyList.size() ) )
		RemoveBuffList( pOwner, m_vApostateReadyList[iBeforeArray].m_szEndBuffList );

	if ( COMPARE( m_iCurApostateApostateReadyCount, 0, (int)m_vApostateReadyList.size() ) )
	{
		//Remove Start Buff
		RemoveBuffList( pOwner, m_vApostateReadyList[m_iCurApostateApostateReadyCount].m_szStartBuffList );

		//Add End Buff
		AddBuffList( pOwner, m_vApostateReadyList[m_iCurApostateApostateReadyCount].m_szEndBuffList );
	}
	
	pOwner->SetState( CS_DELAY, false );

	if( pOwner->IsNeedSendNetwork() )
	{
		SP2Packet kPacket( CUPK_ITEM_GAUGE_OK );
		FillItemGaugePacketHeader( pOwner, kPacket );
		kPacket << static_cast<BYTE>( SS_APOSTATE_READY_END );
		kPacket << m_iCurApostateApostateReadyCount;
		P2PNetwork::SendToAllPlayingUser( kPacket );
	}
}

//Apostate
bool ioApostatemonkItem::IsApostateState( ioBaseChar *pOwner )
{
	if ( !pOwner )
		return false;
	if ( !pOwner->HasBuff( pOwner->GetCharName(), m_szCheckApostateBuff ) )
		return false;

	return true;
}

void ioApostatemonkItem::ProcessApostateState( ioBaseChar *pOwner )
{
	switch( m_ChargeState )
	{
	case CS_CHARGING:
		ProcessApostateChargingState( pOwner );
		break;
	case CS_NORMAL_ATTACK:
		ProcessNormalAttackState( pOwner );
		break;
	}
}

void ioApostatemonkItem::ProcessApostateChargingState( ioBaseChar *pOwner )
{
	if( !pOwner || !pOwner->IsNeedProcess() )
		return;

	if( pOwner->IsAttackKeyDown() && !pOwner->IsHasCrown() )
	{
		if( m_dwMotionStartTime + m_dwChargeTime <= FRAMEGETTIME() )
		{
			ChangeApostateAttackState( pOwner );
			return;
		}
	}
	else
		ChangeNormalAttackState( pOwner );
}

void ioApostatemonkItem::ChangeApostateAttackState( ioBaseChar *pOwner )
{
	if ( !pOwner )
		return;

	ioEntityGroup *pGrp = pOwner->GetGroup();
	if( !pGrp )
		return;

	DWORD dwCurTime = FRAMEGETTIME();

	int iAniID = pGrp->GetAnimationIdx( m_ApostateAttack.m_AttackAnimation );
	if ( iAniID == -1 )
		return;

	ClearData();
	ClearCancelInfo();
	pOwner->ClearReserveKeyInput();
	pOwner->ClearReservedSliding();
	pOwner->SetChargingState( false );

	if ( pOwner->GetState() != CS_APOSTATE_MONK_SPECIAL )
		pOwner->SetState( CS_APOSTATE_MONK_SPECIAL );

	m_ApostateSpecialState = ASS_APOSTATE_ATTACK;

	if ( pOwner->IsNeedProcess() )
	{
		if ( pOwner->IsSettedDirection() )
			pOwner->SetTargetRotToDirKey( pOwner->CheckCurDirKey() );
		else
			pOwner->SetTargetRotToRotate( pOwner->GetTargetRot(), true );
	}

	SetAttackAni( pOwner, m_ApostateAttack, true );

	if( pOwner->IsNeedSendNetwork() )
	{
		SP2Packet kPacket( CUPK_ITEM_GAUGE_OK );
		FillItemGaugePacketHeader( pOwner, kPacket );
		kPacket << static_cast<BYTE>( SS_APOSTATE_ATTACK );
		kPacket << pOwner->GetWorldPosition();
		kPacket << pOwner->GetTargetRot();
		P2PNetwork::SendToAllPlayingUser( kPacket );
	}
}

void ioApostatemonkItem::ProcessApostateAttackState( ioBaseChar *pOwner )
{
	if ( !pOwner )
		return;

	if ( pOwner->IsNeedProcess() )
	{
		DWORD dwCurTime = FRAMEGETTIME();

		//Check DropZone
		float fHeight = pOwner->GetBottomHeight();
		if ( fHeight == 0.0f && !pOwner->CheckGhostState() )
		{
			pOwner->SetFallState( true );
			return;
			
		}

		if ( m_dwMotionEndTime < dwCurTime )
		{
			ChangeApostateAttackEndState( pOwner );
			return;
		}

		if ( m_dwKeyReserveTime > 0 && m_dwKeyReserveTime < dwCurTime )
		{
			if ( pOwner->IsAttackKey() )
			{
				ChangeApostateAddAttackState( pOwner );
				return;
			}
		}

		CheckApostateAttackKeyInput( pOwner );
	}

	ProcessApostateAttackRotate( pOwner );

	pOwner->SetCurMoveSpeed( m_fApostateAttackRunSpeed );
	
}

void ioApostatemonkItem::CheckApostateAttackKeyInput( ioBaseChar *pOwner )
{
	if( !pOwner )
		return;

	ioUserKeyInput::DirKeyInput eNewDirKey = pOwner->CheckCurDirKey();
	if( eNewDirKey != m_CurDirKey )
	{
		m_CurDirKey = eNewDirKey;
		if ( pOwner->IsNeedSendNetwork() )
		{
			SP2Packet kPacket( CUPK_ITEM_GAUGE_OK );
			FillItemGaugePacketHeader( pOwner, kPacket );
			kPacket << static_cast<BYTE>( SS_APOSATTE_ATTACK_CHANGE_DIR_KEY );
			kPacket << m_CurDirKey;
			P2PNetwork::SendToAllPlayingUser( kPacket );
		}
	}
}

void ioApostatemonkItem::ProcessApostateAttackRotate( ioBaseChar *pOwner )
{
	if( !pOwner )
		return;
	if( m_dwApostateAttackRotateSpeed <= 0 )
		return;

	if( m_CurDirKey == ioUserKeyInput::DKI_NONE )
		return;

	float fFrameGap = g_FrameTimer.GetCurFrameGap();
	float fRate = fFrameGap / m_dwApostateAttackRotateSpeed;
	float fRotateAngle = 360.0f * fRate;

	D3DXQUATERNION qtCurRot = pOwner->GetTargetRot();
	D3DXVECTOR3 vCurMoveDir = qtCurRot * D3DXVECTOR3(0.0f, 0.0f, -FLOAT1);
	D3DXVec3Normalize( &vCurMoveDir, &vCurMoveDir );

	// 목표에 도달체크 및 처리
	float fYawD = 180.0f + 45.0f * ( m_CurDirKey - ioUserKeyInput::DKI_UP );
	fYawD = ioMath::ArrangeHead( fYawD );

	float fYawR = DEGtoRAD( fYawD );
	D3DXQUATERNION qtRotate;
	D3DXQuaternionRotationYawPitchRoll( &qtRotate, fYawR, 0.0f, 0.0f );

	D3DXVECTOR3 vTargetDir = qtRotate * D3DXVECTOR3(0.0f, 0.0f, -FLOAT1);
	D3DXVec3Normalize( &vTargetDir, &vTargetDir );

	float fDotValue = D3DXVec3Dot( &vCurMoveDir, &vTargetDir );
	float fGapAngle = RADtoDEG( acosf(fDotValue) );

	if( fDotValue >= 0.98f )
	{
		pOwner->SetTargetRotToRotate( qtRotate, true );
		return;
	}

	D3DXVECTOR3 vAxis;
	if( fDotValue <= -0.98f )
	{
		vAxis = ioMath::UNIT_Y;
	}
	else
	{
		D3DXVec3Cross( &vAxis, &vTargetDir, &vCurMoveDir );
		D3DXVec3Normalize( &vAxis, &-vAxis );
	}

	D3DXQUATERNION qtNewRot;
	if( fGapAngle < fRotateAngle )
		D3DXQuaternionRotationAxis( &qtNewRot, &vAxis, DEGtoRAD(fGapAngle) );
	else
		D3DXQuaternionRotationAxis( &qtNewRot, &vAxis, DEGtoRAD(fRotateAngle) );

	D3DXVECTOR3 vNewDir = qtNewRot * vCurMoveDir;
	D3DXVec3Normalize( &vNewDir, &vNewDir );

	ioMath::CalcDirectionQuaternion( qtNewRot, -vNewDir );
	pOwner->SetTargetRotToRotate( qtNewRot, true );
}

void ioApostatemonkItem::ChangeApostateAddAttackState( ioBaseChar *pOwner )
{
	if ( !pOwner )
		return;

	ioEntityGroup *pGrp = pOwner->GetGroup();
	if( !pGrp )
		return;

	DWORD dwCurTime = FRAMEGETTIME();

	int iAniID = pGrp->GetAnimationIdx( m_ApostateAddAttack.m_AttackAnimation );
	if ( iAniID == -1 )
		return;

	g_WeaponMgr.DestroyReserveContactWeapon( pOwner->GetCharName() );
	ClearData();
	ClearCancelInfo();
	pOwner->InitExtendAttackTagInfo();
	pOwner->InitActionStopDelay( true );
	pOwner->ClearReserveKeyInput();
	pOwner->ClearReservedSliding();
	pOwner->SetForcePowerAmt( 0.0f );
	pOwner->SetCurMoveSpeed( 0.0f );

	m_ApostateSpecialState = ASS_APOSTATE_ADD_ATTACK;

	if ( pOwner->IsNeedProcess() )
	{
		if ( pOwner->IsSettedDirection() )
			pOwner->SetTargetRotToDirKey( pOwner->CheckCurDirKey() );
		else
			pOwner->SetTargetRotToRotate( pOwner->GetTargetRot(), true );
	}

	SetAttackAni( pOwner, m_ApostateAddAttack, true );

	if( pOwner->IsNeedSendNetwork() )
	{
		SP2Packet kPacket( CUPK_ITEM_GAUGE_OK );
		FillItemGaugePacketHeader( pOwner, kPacket );
		kPacket << static_cast<BYTE>( SS_APOSTATE_ADD_ATTACK );
		kPacket << pOwner->GetWorldPosition();
		kPacket << pOwner->GetTargetRot();
		P2PNetwork::SendToAllPlayingUser( kPacket );
	}
}

void ioApostatemonkItem::ProcessApostateAddAttackState( ioBaseChar *pOwner )
{
	if ( !pOwner || !pOwner->IsNeedProcess() )
		return;

	DWORD dwCurTime = FRAMEGETTIME();
	if ( m_dwMotionEndTime < dwCurTime)
	{
		DirectionRotationOwner( pOwner );
		ProcessReserveKeyInput( pOwner );
		return;
	}

	if ( m_dwKeyReserveTime > 0 && m_dwKeyReserveTime < dwCurTime )
		CheckReserve( pOwner );
}

void ioApostatemonkItem::ChangeApostateAttackEndState( ioBaseChar *pOwner )
{
	if ( !pOwner )
		return;

	ioEntityGroup *pGrp = pOwner->GetGroup();
	if( !pGrp )
		return;

	int iAniID = pGrp->GetAnimationIdx( m_szApostateAttackEndAni );
	if ( iAniID == -1 )
		return;

	DWORD dwCurTime = FRAMEGETTIME();
	ClearData();
	ClearCancelInfo();
	pOwner->ClearReserveKeyInput();
	pOwner->ClearReservedSliding();
	pOwner->SetCurMoveSpeed( 0.0f );
	pOwner->SetForcePowerAmt( 0.0f );

	m_ApostateSpecialState = ASS_APOSTATE_ATTACK_END;

	float fTimeRate = m_fApostateAttackEndAniRate;
	if( fTimeRate <= 0.0f )
		fTimeRate = FLOAT1;

	pGrp->ClearAllLoopAni( FLOAT100, true );
	pGrp->ClearAllActionAni( FLOAT100, true );
	pGrp->SetActionAni( iAniID, FLOAT100, FLOAT100, FLOAT1, FLOAT1/fTimeRate, 0.0f, 0 );

	m_dwMotionEndTime = dwCurTime;
	m_dwMotionEndTime += pGrp->GetAnimationFullTime( iAniID ) * fTimeRate;

	m_dwKeyReserveTime = pGrp->GetAnimationEventTime_e( iAniID, "key_reserve" ) * fTimeRate;
	if( m_dwKeyReserveTime > 0 )
		m_dwKeyReserveTime += dwCurTime;

	pOwner->CheckCharColSkipTime( iAniID, fTimeRate, 0 );

	if( pOwner->IsNeedSendNetwork() )
	{
		SP2Packet kPacket( CUPK_ITEM_GAUGE_OK );
		FillItemGaugePacketHeader( pOwner, kPacket );
		kPacket << static_cast<BYTE>( SS_APOSTATE_ATTACK_END );
		kPacket << pOwner->GetWorldPosition();
		kPacket << pOwner->GetTargetRot();
		P2PNetwork::SendToAllPlayingUser( kPacket );
	}
}

void ioApostatemonkItem::ProcessApostateAttackEndState( ioBaseChar *pOwner )
{
	if ( !pOwner || !pOwner->IsNeedProcess() )
		return;

	DWORD dwCurTime = FRAMEGETTIME();
	if ( m_dwMotionEndTime < dwCurTime)
	{
		DirectionRotationOwner( pOwner );
		ProcessReserveKeyInput( pOwner );
		return;
	}

	if ( m_dwKeyReserveTime > 0 && m_dwKeyReserveTime < dwCurTime )
		CheckReserve( pOwner );
}

void ioApostatemonkItem::DirectionRotationOwner( ioBaseChar *pOwner )
{
	if ( !pOwner || !pOwner->IsNeedProcess() )
		return;

	if ( pOwner->IsSettedDirection() )
	{
		pOwner->SetTargetRotToDirKey( pOwner->CheckCurDirKey() );

		if ( pOwner->IsNeedSendNetwork() )
		{
			SP2Packet kPacket( CUPK_ITEM_GAUGE_OK );
			FillItemGaugePacketHeader( pOwner, kPacket );
			kPacket << static_cast<BYTE>( SS_DIRECTION_ROTATION );
			kPacket << pOwner->GetTargetRot();
			P2PNetwork::SendToAllPlayingUser( kPacket );
		}
	}
}

void ioApostatemonkItem::ProcessReserveKeyInput( ioBaseChar *pOwner )
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

//Relax
void ioApostatemonkItem::ProcessRelaxState( ioBaseChar *pOwner )
{
	switch( m_ChargeState )
	{
	case CS_CHARGING:
		ProcessRelaxChargingState( pOwner );
		break;
	case CS_NORMAL_ATTACK:
		ProcessNormalAttackState( pOwner );
		break;
	case CS_RELAX_ATTACK:
		ProcessRelaxAttackState( pOwner );
		break;
	}
}

void ioApostatemonkItem::ProcessRelaxChargingState( ioBaseChar *pOwner )
{
	if( !pOwner || !pOwner->IsNeedProcess() )
		return;

	if( pOwner->IsAttackKeyDown() && !pOwner->IsHasCrown() )
	{
		if( m_dwMotionStartTime + m_dwChargeTime <= FRAMEGETTIME() )
		{
			ChangeRelaxAttackState( pOwner );
			return;
		}
	}
	else
		ChangeNormalAttackState( pOwner );
}

void ioApostatemonkItem::ChangeRelaxAttackState( ioBaseChar *pOwner )
{
	if ( !pOwner )
		return;

	ioEntityGroup *pGrp = pOwner->GetGroup();
	if( !pGrp )
		return;

	DWORD dwCurTime = FRAMEGETTIME();

	int iAniID = pGrp->GetAnimationIdx( m_RelaxAttack.m_AttackAnimation );
	if ( iAniID == -1 )
		return;

	ClearData();
	ClearCancelInfo();
	pOwner->ClearReserveKeyInput();
	pOwner->ClearReservedSliding();
	pOwner->SetChargingState( false );

	m_ChargeState = CS_RELAX_ATTACK;
	//pOwner->InitExtendAttackTagInfo();
	//pOwner->InitActionStopDelay( true );

	if ( pOwner->IsNeedProcess() )
	{
		if ( pOwner->IsSettedDirection() )
			pOwner->SetTargetRotToDirKey( pOwner->CheckCurDirKey() );
		else
			pOwner->SetTargetRotToRotate( pOwner->GetTargetRot(), true );
	}

	SetAttackAni( pOwner, m_RelaxAttack, true );

	if( pOwner->IsNeedSendNetwork() )
	{
		SP2Packet kPacket( CUPK_ITEM_GAUGE_OK );
		FillItemGaugePacketHeader( pOwner, kPacket );
		kPacket << static_cast<BYTE>( SS_RELAX_ATTACK );
		kPacket << pOwner->GetWorldPosition();
		kPacket << pOwner->GetTargetRot();
		P2PNetwork::SendToAllPlayingUser( kPacket );
	}
}

void ioApostatemonkItem::ProcessRelaxAttackState( ioBaseChar *pOwner )
{
	if ( !pOwner || !pOwner->IsNeedProcess() )
		return;

	DWORD dwCurTime = FRAMEGETTIME();
	if ( m_dwMotionEndTime < dwCurTime)
	{
		DirectionRotationOwner( pOwner );
		ProcessReserveKeyInput( pOwner );
		return;
	}

	if ( m_dwKeyReserveTime > 0 && m_dwKeyReserveTime < dwCurTime )
		CheckReserve( pOwner );
}

void ioApostatemonkItem::SetExtendDash( ioBaseChar *pOwner )
{
	if ( !pOwner )
		return;

	ioEntityGroup *pGrp = pOwner->GetGroup();
	if( !pGrp )
		return;

	DWORD dwCurTime = FRAMEGETTIME();

	int iAniID = pGrp->GetAnimationIdx( m_ExtendDash.m_AttackAnimation );
	if ( iAniID == -1 )
		return;

	ClearData();
	ClearCancelInfo();
	pOwner->ClearReserveKeyInput();
	pOwner->ClearReservedSliding();
	pOwner->SetState( CS_APOSTATE_MONK_SPECIAL );

	m_ApostateSpecialState = ASS_EXTEND_DASH;

	if ( pOwner->IsNeedProcess() )
	{
		if ( pOwner->IsSettedDirection() )
			pOwner->SetTargetRotToDirKey( pOwner->CheckCurDirKey() );
		else
			pOwner->SetTargetRotToRotate( pOwner->GetTargetRot(), true );
	}

	SetAttackAni( pOwner, m_ExtendDash, true );

	if( pOwner->IsNeedSendNetwork() )
	{
		SP2Packet kPacket( CUPK_ITEM_GAUGE_OK );
		FillItemGaugePacketHeader( pOwner, kPacket );
		kPacket << static_cast<BYTE>( SS_EXTEND_DASH );
		kPacket << pOwner->GetWorldPosition();
		kPacket << pOwner->GetTargetRot();
		P2PNetwork::SendToAllPlayingUser( kPacket );
	}
}

void ioApostatemonkItem::ProcessApostateMonkSpecialState( ioBaseChar *pOwner )
{
	switch( m_ApostateSpecialState )
	{
	case ASS_APOSTATE_ATTACK:
		ProcessApostateAttackState( pOwner );
		break;
	case ASS_APOSTATE_ADD_ATTACK:
		ProcessApostateAddAttackState( pOwner );
		break;
	case ASS_APOSTATE_ATTACK_END:
		ProcessApostateAttackEndState( pOwner );
		break;
	case ASS_EXTEND_DASH:
		ProcessExtendDash( pOwner );
		break;
	case ASS_APOSTATE_EXTEND_DASH:
		ProcessApostateExtendDash( pOwner );
		break;
	case ASS_JUMP_DASH:
		ProcessJumpDashState( pOwner );
		break;
	case ASS_LAND_ATTACK:
		ProcessLandAttackState( pOwner );
		break;
	}
}

void ioApostatemonkItem::ProcessExtendDash( ioBaseChar *pOwner )
{
	if ( !pOwner || !pOwner->IsNeedProcess() )
		return;

	DWORD dwCurTime = FRAMEGETTIME();
	if ( m_dwMotionEndTime < dwCurTime )
	{
		DirectionRotationOwner( pOwner );
		ProcessReserveKeyInput( pOwner );
		return;
	}

	if ( m_dwKeyReserveTime > 0 && m_dwKeyReserveTime < dwCurTime )
	{
		if ( m_dwApostateExtendDashChargeStartTime == 0 && IsApostateState( pOwner ) && pOwner->IsAttackKey())
			m_dwApostateExtendDashChargeStartTime = dwCurTime;

		CheckReserve( pOwner );
	}

	if ( m_dwApostateExtendDashChargeStartTime != 0 && pOwner->IsAttackKeyDown() 
		&& m_dwApostateExtendDashChargeStartTime + m_dwApostateExtendDashChargeTime < dwCurTime )
	{
		ChangeApostateExtendDashState( pOwner, 0 );
		return;
	}
}

void ioApostatemonkItem::ChangeApostateExtendDashState( ioBaseChar *pOwner, int iCombo )
{
	if ( !pOwner )
		return;

	ioEntityGroup *pGrp = pOwner->GetGroup();
	if( !pGrp )
		return;

	if ( !COMPARE( iCombo, 0, (int)m_ApostateExtendDashList.size() ) )
		return;

	iCombo = m_iCurApostateExtendCombo;
	const AttackAttribute& rkAttri = m_ApostateExtendDashList[m_iCurApostateExtendCombo];

	pOwner->InitExtendAttackTagInfo();
	pOwner->InitActionStopDelay( true );
	ClearCancelInfo();
	pOwner->ClearReserveKeyInput();
	pOwner->ClearReservedSliding();
	m_dwMotionEndTime = 0;
	m_dwApostateExtendDashChargeStartTime = 0;


	DWORD dwCurTime = FRAMEGETTIME();
	int iAniID = pGrp->GetAnimationIdx( rkAttri.m_AttackAnimation );
	if ( iAniID == -1 )
		return;

	m_ApostateSpecialState = ASS_APOSTATE_EXTEND_DASH;

	if ( pOwner->IsNeedProcess() )
	{
		if ( pOwner->IsSettedDirection() )
			pOwner->SetTargetRotToDirKey( pOwner->CheckCurDirKey() );
		else
			pOwner->SetTargetRotToRotate( pOwner->GetTargetRot(), true );
	}

	SetAttackAni( pOwner, rkAttri, true );

	if( pOwner->IsNeedSendNetwork() )
	{
		SP2Packet kPacket( CUPK_ITEM_GAUGE_OK );
		FillItemGaugePacketHeader( pOwner, kPacket );
		kPacket << static_cast<BYTE>( SS_APOSTATE_EXTEND_DASH );
		kPacket << pOwner->GetWorldPosition();
		kPacket << pOwner->GetTargetRot();
		kPacket << m_iCurApostateExtendCombo;
		P2PNetwork::SendToAllPlayingUser( kPacket );
	}

	m_iCurApostateExtendCombo++;
}

void ioApostatemonkItem::ProcessApostateExtendDash( ioBaseChar *pOwner )
{
	if ( !pOwner || !pOwner->IsNeedProcess() )
		return;

	DWORD dwCurTime = FRAMEGETTIME();
	if ( m_dwMotionEndTime < dwCurTime )
	{
		DirectionRotationOwner( pOwner );
		ProcessReserveKeyInput( pOwner );
		return;
	}

	if ( m_dwKeyReserveTime > 0 && m_dwKeyReserveTime < dwCurTime )
	{
		if ( m_dwApostateExtendDashChargeStartTime == 0 && IsApostateState( pOwner ) && pOwner->IsAttackKey())
			m_dwApostateExtendDashChargeStartTime = dwCurTime;

		CheckReserve( pOwner );
	}

	if ( m_dwApostateExtendDashChargeStartTime != 0 && pOwner->IsAttackKeyDown() 
		&& m_dwApostateExtendDashChargeStartTime + m_dwApostateExtendDashChargeTime < dwCurTime )
	{
		ChangeApostateExtendDashState( pOwner, m_iCurApostateExtendCombo );
		return;
	}
}

void ioApostatemonkItem::SetJumpDashAttackState( ioBaseChar *pOwner )
{
	if ( !pOwner )
		return;

	ioEntityGroup *pGrp = pOwner->GetGroup();
	if( !pGrp )
		return;

	int iAniID = pGrp->GetAnimationIdx( m_JumpDashAttack.m_AttackAnimation );
	if ( iAniID == -1 )
		return;

	DWORD dwCurTime = FRAMEGETTIME();

	ClearData();
	ClearCancelInfo();
	pOwner->SetCurJumpPowerAmt( 0.0f );
	pOwner->SetForcePowerAmt( 0.0f );
	pOwner->SetGravityAmt( 0.0f );
	pOwner->ClearReserveKeyInput();
	pOwner->ClearReservedSliding();
	pOwner->ClearDirDoubleClick();
	pOwner->SetState( CS_APOSTATE_MONK_SPECIAL );

	m_ApostateSpecialState = ASS_JUMP_DASH;

	if ( pOwner->IsNeedProcess() )
	{
		if ( pOwner->IsSettedDirection() )
			pOwner->SetTargetRotToDirKey( pOwner->CheckCurDirKey() );
		else
			pOwner->SetTargetRotToRotate( pOwner->GetTargetRot(), true );
	}

	SetAttackAni( pOwner, m_JumpDashAttack, true );

	if( pOwner->IsNeedSendNetwork() )
	{
		SP2Packet kPacket( CUPK_ITEM_GAUGE_OK );
		FillItemGaugePacketHeader( pOwner, kPacket );
		kPacket << static_cast<BYTE>( SS_JUMP_DASH );
		kPacket << pOwner->GetWorldPosition();
		kPacket << pOwner->GetTargetRot();
		P2PNetwork::SendToAllPlayingUser( kPacket );
	}
}

void ioApostatemonkItem::ProcessJumpDashState( ioBaseChar *pOwner )
{
	if ( !pOwner || !pOwner->IsNeedProcess() )
		return;

	DWORD dwCurTime = FRAMEGETTIME();
	if ( m_dwMotionEndTime < dwCurTime )
	{
		pOwner->SetState( CS_DELAY );
		return;
	}

	if( CheckLanding( pOwner ) )
	{
		ChangeLandAttackState( pOwner );
		return;
	}


}

bool ioApostatemonkItem::CheckLanding( ioBaseChar *pOwner )
{
	if( !pOwner ) 
		return true;
	float fHeightGap = 0.0f;
	if( pOwner->CheckGhostState() )
		fHeightGap = pOwner->GetWorldPosition().y - pOwner->GetPreHeightForGhostState();
	else
		fHeightGap = pOwner->GetWorldPosition().y - pOwner->GetBottomHeight();

	if( fHeightGap > 0.0f || !pOwner->IsDownState() )
	{
		return false;
	}

	return true;
}

void ioApostatemonkItem::ChangeLandAttackState( ioBaseChar *pOwner )
{
	if ( !pOwner )
		return;

	ioEntityGroup *pGrp = pOwner->GetGroup();
	if( !pGrp )
		return;

	int iAniID = pGrp->GetAnimationIdx( m_JumpDashLandAttack.m_AttackAnimation );
	if ( iAniID == -1 )
		return;

	DWORD dwCurTime = FRAMEGETTIME();

	ClearData();
	ClearCancelInfo();
	pOwner->ClearReserveKeyInput();
	pOwner->ClearReservedSliding();

	m_ApostateSpecialState = ASS_LAND_ATTACK;

	SetAttackAni( pOwner, m_JumpDashLandAttack, true );

	if( pOwner->IsNeedSendNetwork() )
	{
		SP2Packet kPacket( CUPK_ITEM_GAUGE_OK );
		FillItemGaugePacketHeader( pOwner, kPacket );
		kPacket << static_cast<BYTE>( SS_LAND_ATTACK );
		kPacket << pOwner->GetWorldPosition();
		kPacket << pOwner->GetTargetRot();
		P2PNetwork::SendToAllPlayingUser( kPacket );
	}
}

void ioApostatemonkItem::ProcessLandAttackState( ioBaseChar *pOwner )
{
	if ( !pOwner || !pOwner->IsNeedProcess() )
		return;

	DWORD dwCurTime = FRAMEGETTIME();
	if ( m_dwMotionEndTime < dwCurTime )
	{
		if ( m_bReserveApostateAttack )
			ChangeApostateAttackState( pOwner );
		else
		{
			DirectionRotationOwner( pOwner );
			ProcessReserveKeyInput( pOwner );
		}
		return;
	}

	if ( m_dwKeyReserveTime > 0 && m_dwKeyReserveTime < dwCurTime )
	{
		if ( m_dwLandAttackChargeStartTime == 0 && IsApostateState( pOwner ) && pOwner->IsAttackKeyDown() )
			m_dwLandAttackChargeStartTime = dwCurTime;

		CheckReserve( pOwner );
	}

	if ( !m_bReserveApostateAttack && m_dwLandAttackChargeStartTime != 0 && pOwner->IsAttackKeyDown() 
		&& m_dwLandAttackChargeStartTime + m_dwLandAttackChargeTime < dwCurTime )
		m_bReserveApostateAttack = true;
}

void ioApostatemonkItem::SetExtraInfoToWeapon( ioWeapon *pWeapon, bool bJumpAttack )
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

bool ioApostatemonkItem::IsEnableJumpDashAttack( ioBaseChar *pOwner )
{
	//Check DropZone
	float fHeightGap = 0.0f;
	if( pOwner->CheckGhostState() )
		fHeightGap = pOwner->GetWorldPosition().y - pOwner->GetPreHeightForGhostState();
	else
		fHeightGap = pOwner->GetWorldPosition().y - pOwner->GetBottomHeight();

	if ( fHeightGap < m_fJumpDashEnableHeightGap )
		return false;

	return true;
}

bool ioApostatemonkItem::IsEnableBlowWoundedBySpecialState()
{
	if ( m_ApostateSpecialState == ASS_JUMP_DASH )
		return true;

	return false;
}

void ioApostatemonkItem::SetSpecialStateGravity( float &fCurGravity )
{
	if ( m_ApostateSpecialState == ASS_JUMP_DASH )
		fCurGravity = m_fJumpDashAttackGravity;
}

bool ioApostatemonkItem::IsNoDropSpecialState()
{
	if ( m_ApostateSpecialState == ASS_JUMP_DASH )
		return true;

	return false;
}

void ioApostatemonkItem::OnItemGaugeOK( ioBaseChar *pOwner, SP2Packet &rkPacket )
{
	if ( !pOwner )
		return;

	BYTE btState;
	rkPacket >> btState;

	if ( btState == SS_CHECK_MONK_STATE )
	{
		BYTE btMonkState;
		rkPacket >> btMonkState;
		m_MonkState = (MonkState)btMonkState;
	}
	else if ( btState == SS_NORMAL_ATTACK )
	{
		D3DXVECTOR3 vPos;
		rkPacket >> vPos;
		pOwner->SetWorldPosition( vPos );
		D3DXQUATERNION qtRot;
		rkPacket >> qtRot;
		pOwner->SetTargetRotAndMoveDirChange( qtRot );

		rkPacket >> m_iCurCombo;
		if( !m_bChargeAutoTarget )
			pOwner->ApplyAutoTargetInfo( rkPacket );
		ChangeNormalAttackState( pOwner );
	}
	else if ( btState == SS_APOSTATE_READY )
	{
		D3DXVECTOR3 vPos;
		rkPacket >> vPos;
		pOwner->SetWorldPosition( vPos );
		D3DXQUATERNION qtRot;
		rkPacket >> qtRot;
		pOwner->SetTargetRotAndMoveDirChange( qtRot );

		rkPacket >> m_iCurApostateApostateReadyCount;
		ChangeNormalApostateReadyState( pOwner );
	}
	else if ( btState == SS_APOSTATE_READY_END )
	{
		rkPacket >> m_iCurApostateApostateReadyCount;
		SetApostateReadyEndState( pOwner );
	}
	else if ( btState == SS_APOSTATE_ATTACK )
	{
		D3DXVECTOR3 vPos;
		rkPacket >> vPos;
		pOwner->SetWorldPosition( vPos );
		D3DXQUATERNION qtRot;
		rkPacket >> qtRot;
		pOwner->SetTargetRotAndMoveDirChange( qtRot );
		ChangeApostateAttackState( pOwner );
	}
	else if ( btState == SS_APOSATTE_ATTACK_CHANGE_DIR_KEY )
	{
		int iDirKey;
		rkPacket >> iDirKey;
		m_CurDirKey = (ioUserKeyInput::DirKeyInput)iDirKey;
	}
	else if ( btState == SS_APOSTATE_ADD_ATTACK )
	{
		D3DXVECTOR3 vPos;
		rkPacket >> vPos;
		pOwner->SetWorldPosition( vPos );
		D3DXQUATERNION qtRot;
		rkPacket >> qtRot;
		pOwner->SetTargetRotAndMoveDirChange( qtRot );
		ChangeApostateAddAttackState( pOwner );
	}
	else if ( btState == SS_APOSTATE_ATTACK_END )
	{
		D3DXVECTOR3 vPos;
		rkPacket >> vPos;
		pOwner->SetWorldPosition( vPos );
		D3DXQUATERNION qtRot;
		rkPacket >> qtRot;
		pOwner->SetTargetRotAndMoveDirChange( qtRot );
		ChangeApostateAttackEndState( pOwner );
	}
	else if ( btState == SS_RELAX_ATTACK )
	{
		D3DXVECTOR3 vPos;
		rkPacket >> vPos;
		pOwner->SetWorldPosition( vPos );
		D3DXQUATERNION qtRot;
		rkPacket >> qtRot;
		pOwner->SetTargetRotAndMoveDirChange( qtRot );
		ChangeRelaxAttackState( pOwner );
	}
	else if ( btState == SS_EXTEND_DASH )
	{
		D3DXVECTOR3 vPos;
		rkPacket >> vPos;
		pOwner->SetWorldPosition( vPos );
		D3DXQUATERNION qtRot;
		rkPacket >> qtRot;
		pOwner->SetTargetRotAndMoveDirChange( qtRot );
		SetExtendDash( pOwner );
	}
	else if ( btState == SS_APOSTATE_EXTEND_DASH )
	{
		D3DXVECTOR3 vPos;
		rkPacket >> vPos;
		pOwner->SetWorldPosition( vPos );
		D3DXQUATERNION qtRot;
		rkPacket >> qtRot;
		pOwner->SetTargetRotAndMoveDirChange( qtRot );
		rkPacket >> m_iCurApostateExtendCombo;
		ChangeApostateExtendDashState( pOwner, m_iCurApostateExtendCombo );
	}
	else if ( btState == SS_JUMP_DASH )
	{
		D3DXVECTOR3 vPos;
		rkPacket >> vPos;
		pOwner->SetWorldPosition( vPos );
		D3DXQUATERNION qtRot;
		rkPacket >> qtRot;
		pOwner->SetTargetRotAndMoveDirChange( qtRot );
		SetJumpDashAttackState( pOwner );
	}
	else if ( btState == SS_LAND_ATTACK )
	{
		D3DXVECTOR3 vPos;
		rkPacket >> vPos;
		pOwner->SetWorldPosition( vPos );
		D3DXQUATERNION qtRot;
		rkPacket >> qtRot;
		pOwner->SetTargetRotAndMoveDirChange( qtRot );
		ChangeLandAttackState( pOwner );
	}
	else if ( btState == SS_DIRECTION_ROTATION )
	{
		D3DXQUATERNION qtRot;
		rkPacket >> qtRot;
		pOwner->SetTargetRotAndMoveDirChange( qtRot );
	}
}

void ioApostatemonkItem::OnItemGaugeCancel( ioBaseChar *pOwner, SP2Packet &rkPacket )
{

}

void ioApostatemonkItem::UpdateExtraData( ioBaseChar *pOwner )
{
	if( !pOwner )
		return;
	float fTimePerSec = g_FrameTimer.GetSecPerFrame();

	if( pOwner->HasBuff( pOwner->GetCharName(), m_szCheckRelaxBuff ) )
	{
		float fGauge = m_fDecreaseRelaxStateGaugeTic * fTimePerSec;

		m_fCurBullet -= fGauge;
		m_fCurBullet = max( 0, m_fCurBullet );
	}
}

int ioApostatemonkItem::GetNeedBullet()
{
	ioPlayStage *pStage = g_ItemMaker.GetPlayStage();
	ioBaseChar *pOwner = NULL;
	if( pStage )
	{
		//본부에는 NPC도 존재하기 때문에
		if ( pStage->GetModeType() == MT_HEADQUARTERS)
			pOwner = pStage->GetOwnerChar();
		else
			pOwner = pStage->GetBaseChar( m_OwnerName );
	}

	if( !pOwner )
		return 0;

	if( pOwner->HasBuff( pOwner->GetCharName(), m_szCheckRelaxBuff ) )
		return m_fMaxBullet;

	return 0;
}

void ioApostatemonkItem::SetCurBullet( int iCurBullet )
{
	if( iCurBullet < 0 )
		return;

	float fCurBullet = iCurBullet;
	fCurBullet = max( 0, min( fCurBullet, m_fMaxBullet ) );

	m_fCurBullet = fCurBullet;
}

void ioApostatemonkItem::IncreaseGauge( float fNeedGauge )
{
	m_fCurBullet += fNeedGauge;
	m_fCurBullet = min( m_fMaxBullet, m_fCurBullet );
}

//-----------------------------------------------------------------------------------------------------------

ioApostateMonkSpecialState::ioApostateMonkSpecialState()
{

}

ioApostateMonkSpecialState::~ioApostateMonkSpecialState()
{

}

void ioApostateMonkSpecialState::Finalize( ioBaseChar* pOwner, CharState eCurrState, CharState eNewState )
{
	if ( !pOwner )
		return;

	ioSpecialStateBase::Finalize( pOwner, eCurrState, eNewState );

	//ioApostatemonkItem *pApostatemon = ToApostatemonkItem( pOwner->GetPriorityItem( SPT_ATTACK ) );
	//if( pApostatemon && eNewState != CS_ATTACK )
//		pApostatemon->ClearData();
}

void ioApostateMonkSpecialState::CheckSpecialState( ioBaseChar* pOwner )
{
	if ( !pOwner )
		return;

	ioApostatemonkItem *pApostatemon = ToApostatemonkItem( pOwner->GetPriorityItem( SPT_ATTACK ) );
	if( pApostatemon )
		pApostatemon->ProcessApostateMonkSpecialState( pOwner );
	else
	{
		pOwner->SetState( CS_DELAY );
		return;
	}
}

void ioApostateMonkSpecialState::CheckFallStateChange( ioBaseChar* pOwner, float fMinFallHeightRate, float fBottomHeight, bool bBottomMaxCheck, float fMaxBottomHeight )
{
	return;
}

bool ioApostateMonkSpecialState::IsEnableBlowWounded( ioBaseChar *pOwner )
{
	ioApostatemonkItem *pApostatemon = ToApostatemonkItem( pOwner->GetPriorityItem( SPT_ATTACK ) );
	if( pApostatemon )
		return pApostatemon->IsEnableBlowWoundedBySpecialState();
	return true;
}

void ioApostateMonkSpecialState::GetGravity( ioBaseChar* pOwner, float &fCurGravity )
{
	if ( IsNoDropState( pOwner ) )
		fCurGravity = 0.0f;

	ioApostatemonkItem *pApostatemon = ToApostatemonkItem( pOwner->GetPriorityItem( SPT_ATTACK ) );
	if( pApostatemon )
		pApostatemon->SetSpecialStateGravity( fCurGravity );
}

bool ioApostateMonkSpecialState::IsNoDropState( ioBaseChar* pOwner )
{
	ioApostatemonkItem *pApostatemon = ToApostatemonkItem( pOwner->GetPriorityItem( SPT_ATTACK ) );
	if( !pApostatemon )
		return false;

	return pApostatemon->IsNoDropSpecialState();
}

//-----------------------------------------------------------------------------------------------------------