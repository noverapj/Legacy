#include "stdafx.h"

#include "ioChargeComboCounter.h"

#include "ioBaseChar.h"
#include "ioPlayStage.h"

#include "ioLeeSunSin.h"
#include "ioRedHoodItem.h"

ioChargeComboCounter::ioChargeComboCounter()
{
	m_pExtendCounter = NULL;
}

ioChargeComboCounter::ioChargeComboCounter( const ioChargeComboCounter &rhs )
	: ioCounterAttack( rhs ),	
	m_CounterExtendType( rhs.m_CounterExtendType ),
	m_CounterDelay( rhs.m_CounterDelay ),
	m_CounterCharge( rhs.m_CounterCharge ),
	m_CounterComboAttList( rhs.m_CounterComboAttList ),
	m_nCounterMaxCombo( rhs.m_nCounterMaxCombo ),
	m_dwChargeTime( rhs.m_dwChargeTime ),
	m_fEndJumpPower( rhs.m_fEndJumpPower ),
	m_fJumpHeightGap( rhs.m_fJumpHeightGap ),
	m_vBranch( rhs.m_vBranch )
{
	if( rhs.m_pExtendCounter )
		m_pExtendCounter = rhs.m_pExtendCounter->Clone();
	else
		m_pExtendCounter = NULL;
}

ioChargeComboCounter::~ioChargeComboCounter()
{
	SAFEDELETE( m_pExtendCounter );
}

void ioChargeComboCounter::LoadProperty( ioINILoader &rkLoader )
{
	char szBuf[MAX_PATH];
	char szKey[MAX_PATH];

	ioCounterAttack::LoadProperty( rkLoader );

	LoadAttackAttribute_e( "counter_delay", m_CounterDelay, rkLoader );
	LoadAttackAttribute_e( "counter_charge_attack", m_CounterCharge, rkLoader );

	m_CounterExtendType	= (CounterExtendType)rkLoader.LoadInt_e( "counter_extend_type", CET_Default );
	m_nCounterMaxCombo	= rkLoader.LoadInt_e( "counter_combo_cnt", 0 );

	m_CounterComboAttList.clear();
	for( int i=0; i < m_nCounterMaxCombo; ++i )
	{
		AttackAttribute kAttribute;
		wsprintf_e( szBuf, "counter_combo_attack%d", i+1 );
		LoadAttackAttribute( szBuf, kAttribute, rkLoader );
		m_CounterComboAttList.push_back( kAttribute );
	}

	m_dwChargeTime		= rkLoader.LoadInt_e( "counter_charge_time", 0 );
	m_fEndJumpPower		= rkLoader.LoadFloat_e( "counter_end_jump_power", 0.0f );
	m_fJumpHeightGap	= rkLoader.LoadFloat_e( "counter_jump_height_gap", 0.0f );

	//////////////////////////////////////////////////////////////////////////

	int nBranChCnt = rkLoader.LoadInt_e( "branch_max_cnt", 0 );
	for( int i = 0; i < nBranChCnt; i++ )
	{
		CounterBranchAniInfo info;

		wsprintf( szKey, "branch_ani%d", i+1 );
		rkLoader.LoadString( szKey, "", szBuf, MAX_PATH );
		info.Ani = szBuf;

		wsprintf( szKey, "branch_ani_rate%d", i+1 );
		info.rate= rkLoader.LoadFloat( szKey, FLOAT1 );

		m_vBranch.push_back( info );
	}

	switch( m_CounterExtendType )
	{
	case CET_ShadowRanger:
		m_pExtendCounter = new ioShadowChargeComboCounter;
		break;
	case CET_LeeSunSin:
		m_pExtendCounter = new ioLeeSunSinExtendCounter;
		break;
	case CET_RedHood:
		m_pExtendCounter = new ioRedHoodExtendCounter;
		break;
	}

	if( m_pExtendCounter )
		m_pExtendCounter->LoadProperty( rkLoader );
}

ioCounterAttack* ioChargeComboCounter::Clone()
{
	return new ioChargeComboCounter( *this );
}

CounterAttackType ioChargeComboCounter::GetType() const
{
	return CAT_CHARGE_COMBO;
}

CounterExtendType ioChargeComboCounter::GetExtendType() const
{
	return m_CounterExtendType;
}

ExtendCounter* ioChargeComboCounter::GetExtendCounter() const
{
	return m_pExtendCounter;
}

void ioChargeComboCounter::Init()
{
	m_ChargeComboState			= CCS_None;
	m_dwCounterStartTime		= 0;
	m_dwEnableReserveKeyTime	= 0;
	m_dwMotionEndTime			= 0;
	m_nCurCounterCombo			= 0;

	m_bAttackKeyReserved		= false;
	m_bDefenseKeyReserved		= false;
	m_bJumpKeyReserved			= false;

	m_dwRedHoodSpecialMoveMacroTime_S = 0;
	m_dwRedHoodSpecialMoveMacroTime_E = 0;
}

bool ioChargeComboCounter::StartCAT( ioBaseChar *pOwner, int iCounterAttackKeyState )
{
	if ( iCounterAttackKeyState == CAKS_ATTACK_KEY )
	{
		Init();

		pOwner->ClearReserveKeyInput();

		if ( !SetCounterDelayState( pOwner ) )
			return false;

		return true;
	}
	else
		return false;
	
}

void ioChargeComboCounter::ProcessCAT( ioBaseChar *pOwner )
{
	if( !pOwner )
		return;

	DWORD dwCurTime = FRAMEGETTIME();
	switch ( m_ChargeComboState )
	{
	case CCS_Delay:
		if( m_CounterExtendType == CET_RedHood )
		{
			if( m_dwMotionEndTime < dwCurTime )
			{
				if( m_bAttackKeyReserved && m_nCurCounterCombo < (int)m_CounterComboAttList.size() )
					//SetNormalComboState( pOwner );
				{
					m_ChargeComboState = CCS_Delay_Charge;
					m_dwRedHoodComboStartTime = dwCurTime;

					//
					if( pOwner->IsNeedProcess() )
						SetBranChAni( pOwner, m_nCurCounterCombo, true );
				}
					//CheckCharging( pOwner);
				else
					CheckReserveState( pOwner );
				return;
			}

			if ( m_dwEnableReserveKeyTime > 0 && m_dwEnableReserveKeyTime < dwCurTime )
			{
				ReserveInputKey( pOwner, true, true, true );
				//CheckComboCharging( pOwner );
			}
		}
		else
			CheckCharging( pOwner );
		break;
	case CCS_Delay_Charge:
		if( m_CounterExtendType == CET_RedHood )
		{
			CheckchargingRedHood( pOwner );
		}
		break;
	case CCS_Normal_Charge_Attack:
		if ( pOwner->IsNeedProcess() )
		{
			if ( m_dwMotionEndTime < dwCurTime )
			{
				CheckReserveState( pOwner );
				return;
			}
			else
			{					
				if ( m_dwEnableReserveKeyTime > 0 && m_dwEnableReserveKeyTime < dwCurTime )
					ReserveInputKey( pOwner, true, true, true );
			}
		}
		break;
	case CCS_Normal_Combo:
		{
			if(m_CounterExtendType == CET_RedHood )
			{
				if( pOwner->IsNeedProcess() )
				{
					if( m_dwMotionEndTime < dwCurTime )
					{
						if( m_bAttackKeyReserved && m_nCurCounterCombo < (int)m_CounterComboAttList.size() )
						{
							m_ChargeComboState = CCS_Delay_Charge;
							m_dwRedHoodComboStartTime = dwCurTime;

							SetBranChAni( pOwner, m_nCurCounterCombo, true );
						}
							//SetNormalComboState( pOwner );
						else
							CheckReserveState( pOwner );
						return;
					}

					if ( m_dwEnableReserveKeyTime > 0 && m_dwEnableReserveKeyTime < dwCurTime )
					{
						ReserveInputKey( pOwner, true, true, true );
						//CheckComboCharging( pOwner );
					}
				}
			}
			else
			{
				if ( pOwner->IsNeedProcess() )
				{
					if ( m_dwMotionEndTime < dwCurTime )
					{
						CheckReserveState( pOwner );
						return;
					}
					else
					{					
						if ( m_dwEnableReserveKeyTime > 0 && m_dwEnableReserveKeyTime < dwCurTime )
							ReserveInputKey( pOwner, false, true, true );
					}

					if ( CheckComboInput( pOwner ) )
						return;
				}
			}
		}
		break;
	case CCS_Special_Charge_Attack:
		{
			ProcessSpecialCAT( pOwner );
		}
		break;
	case CCS_End:
		{
			if ( IsFloatState( pOwner ) )
			{	
				if ( pOwner->IsNeedProcess() )
				{
					pOwner->SetSKillEndJumpState( m_fEndJumpPower, false, false, true, true );

					if ( pOwner->IsNeedSendNetwork() )
					{
						SP2Packet kPacket( CUPK_COUNTER_ATTACK_SYNC );
						kPacket << pOwner->GetCharName();
						kPacket << (int)GetType();
						kPacket << pOwner->GetHP().m_fCurValue;
						kPacket << CCS_End_Jump;
						P2PNetwork::SendToAllPlayingUser( kPacket );
					}
				}
			}
			else
				pOwner->SetState( CS_DELAY );
		}
		break;
	}

	CheckAniRotate( pOwner );

	if( pOwner->IsNeedProcess() && m_CounterExtendType == CET_RedHood &&
		m_dwRedHoodSpecialMoveMacroTime_S && m_dwRedHoodSpecialMoveMacroTime_S < dwCurTime && m_dwRedHoodSpecialMoveMacroTime_E > dwCurTime && pOwner->IsDefenseKey() )
	{
		ioRedHoodItem* pRedHoodItem = ToRedHoodItem( pOwner->GetPriorityItem( SPT_ATTACK ) );
		if( pRedHoodItem->IsEnableGauge() )
		{
			pRedHoodItem->SetSpecailMoveState( pOwner, true );
			return;
		}
		
	}
}


void ioChargeComboCounter::SetBranChAni( ioBaseChar *pOwner, int nIndex, bool bSendNet )
{
	ioEntityGroup *pGrp = pOwner->GetGroup();
	if( !pGrp )
		return;

	if( nIndex >= (int)m_vBranch.size() )
		return;

	CounterBranchAniInfo info = m_vBranch[nIndex];

	int iAniID = pGrp->GetAnimationIdx( info.Ani );
	if ( iAniID == -1 )
		return;

	float fTimeRate  = info.rate;
	
	pGrp->ClearAllLoopAni( FLOAT100, true );
	pGrp->ClearAllActionAni( FLOAT100, true );

	pGrp->SetActionAni( iAniID, FLOAT100, FLOAT100, FLOAT1, FLOAT1/fTimeRate, 0.0f, 0.0f, true );

	if( pOwner->IsNeedProcess() && bSendNet )
	{
		SP2Packet kPacket( CUPK_COUNTER_ATTACK_SYNC );
		kPacket << pOwner->GetCharName();
		kPacket << (int)GetType();
		kPacket << pOwner->GetHP().m_fCurValue;
		kPacket << CCS_Branch_Ani;
		kPacket << nIndex;
		P2PNetwork::SendToAllPlayingUser( kPacket );
	}
}


void ioChargeComboCounter::ProcessSpecialCAT( ioBaseChar *pOwner )
{
	if( !pOwner )
		return;

	if( m_pExtendCounter )
		m_pExtendCounter->ProcessSpecialCAT( pOwner, this );
}

void ioChargeComboCounter::ApplyCAT( ioBaseChar *pOwner, SP2Packet &rkPacket )
{
	if( !pOwner )
		return;

	int nCounterState;
	rkPacket >> nCounterState;

	switch( nCounterState )
	{
	case CCS_Normal_Charge_Attack:
		{
			SetNormalChargeAttackState( pOwner );

			pOwner->ApplyAutoTargetInfo( rkPacket );
		}
		break;
	case CCS_Normal_Combo:
		{
			rkPacket >> m_nCurCounterCombo;

			D3DXQUATERNION qtRot;
			rkPacket >> qtRot;

			pOwner->SetTargetRotAndMoveDirChange( qtRot );

			SetNormalComboState( pOwner );

			pOwner->ApplyAutoTargetInfo( rkPacket );
		}
		break;
	case CCS_Special_Charge_Attack:
		{
			D3DXVECTOR3 vPos;
			rkPacket >> vPos;

			pOwner->SetWorldPosition( vPos );

			D3DXQUATERNION qtRot;
			rkPacket >> qtRot;

			pOwner->SetTargetRotAndMoveDirChange( qtRot );

			if( m_CounterExtendType == CET_RedHood )
				rkPacket >> m_nCurCounterCombo;

			SetChargeState( pOwner );

			pOwner->ApplyAutoTargetInfo( rkPacket );
		}
		break;
	case CCS_Special_Process:
		{
			ApplySpecialCAT( pOwner, rkPacket );
		}
		break;
	case CCS_End:
		{
			m_ChargeComboState = CCS_End;
		}
		break;
	case CCS_End_Jump:
		{
			pOwner->SetSKillEndJumpState( m_fEndJumpPower, false, false, false, true );
		}
		break;
	case CCS_Branch_Ani:
		{
			int nIndex;
			rkPacket >> nIndex;
			SetBranChAni( pOwner, nIndex, false );
		}
		break;
	}
}

void ioChargeComboCounter::ApplySpecialCAT( ioBaseChar *pOwner, SP2Packet &rkPacket )
{
	if ( !pOwner )
		return;

	if( m_pExtendCounter )
		m_pExtendCounter->ApplySpecialCAT( pOwner, this, rkPacket );
}

bool ioChargeComboCounter::IsNoDropState() const
{
	if( m_pExtendCounter )
		return m_pExtendCounter->IsNoDropState();

	return false;
}

bool ioChargeComboCounter::SetCounterDelayState( ioBaseChar *pOwner )
{
	if ( !pOwner )
		return false;;

	pOwner->SetNoWoundState( false );
	pOwner->InitExtendAttackTagInfo();
	pOwner->ClearReserveKeyInput();
	pOwner->ClearInputCancelInfo();

	ioEntityGroup *pGrp = pOwner->GetGroup();
	int iAniID = pGrp->GetAnimationIdx( m_CounterDelay.m_AttackAnimation );
	if ( iAniID == -1 )
		return false;

	float fTimeRate  = m_CounterDelay.m_fAttackAniRate;
	DWORD dwPreDelay = m_CounterDelay.m_dwPreDelay;

	pOwner->ClearDirDoubleClick();

	pGrp->ClearAllLoopAni( FLOAT100, true );
	pGrp->ClearAllActionAni( FLOAT100, true );

	pGrp->SetActionAni( iAniID, FLOAT100, FLOAT100, FLOAT1, FLOAT1/fTimeRate, 0.0f, 0.0f, true );

	pOwner->ClearReservedSliding();
	pOwner->SetForcePowerAmt( 0.0f );
	pOwner->SetReservedSliding( m_CounterDelay.m_vForceInfoList, iAniID, fTimeRate, 0 );

	pOwner->RefreshFireTimeList( iAniID, m_CounterDelay.m_vWeaponInfoList, FTT_COUNTER_ATTACK, fTimeRate, 0 );

	pOwner->CheckCharColSkipTime( iAniID, fTimeRate, dwPreDelay );

	DWORD dwCurTime = FRAMEGETTIME();
	m_dwCounterStartTime		= dwCurTime;
	m_dwEnableReserveKeyTime	= dwCurTime + pGrp->GetAnimationEventTime_e( iAniID, "key_reserve" ) * fTimeRate;
	m_dwMotionEndTime			= dwCurTime + pGrp->GetAnimationFullTime( iAniID ) * fTimeRate;
	m_ChargeComboState			= CCS_Delay;

	pOwner->SetState( CS_COUNTER_ATTACK );

	//RedHood
	SetRedHoodMacro( pGrp, iAniID, fTimeRate, m_CounterDelay.m_dwEndDelay );

	return true;
}

bool ioChargeComboCounter::SetChargeState( ioBaseChar *pOwner )
{
	if ( !pOwner )
		return false;

	if( m_pExtendCounter )
	{
		return m_pExtendCounter->SetChargeState( pOwner, this );
	}

	return false;
}

void ioChargeComboCounter::SetNormalComboState( ioBaseChar *pOwner )
{
	m_dwComboStartTime = FRAMEGETTIME();

	if ( !pOwner || m_CounterComboAttList.empty() )
		return;

	if ( !COMPARE( m_nCurCounterCombo, 0, (int)m_CounterComboAttList.size() ) )
		return;

	//연타시 4타 이상일때 타격 가능하게 만듬
	pOwner->InitActionStopDelay( true );

	const AttackAttribute &pAttribute = m_CounterComboAttList[m_nCurCounterCombo];

	//공격 준비 애니메이션 설정
	ioEntityGroup *pGrp = pOwner->GetGroup();

	int iAniID = pGrp->GetAnimationIdx( pAttribute.m_AttackAnimation );
	if ( iAniID == -1)
		return;

	pGrp->ClearAllLoopAni( FLOAT100, true );
	pGrp->ClearAllActionAni( FLOAT100, true );

	float fTimeRate  = pAttribute.m_fAttackAniRate;
	DWORD dwPreDelay = pAttribute.m_dwPreDelay;

	pGrp->SetActionAni( iAniID, FLOAT100, FLOAT100, FLOAT1, FLOAT1/fTimeRate, 0.0f, 0, true );	

	pOwner->SetReservedSliding( pAttribute.m_vForceInfoList, iAniID, fTimeRate, 0 );

	pOwner->RefreshFireTimeList( iAniID, pAttribute.m_vWeaponInfoList, FTT_NORMAL_ATTACK, fTimeRate, 0 );

	DWORD dwCurTime = FRAMEGETTIME();
	m_dwEnableReserveKeyTime = dwCurTime + pGrp->GetAnimationEventTime_e( iAniID, "key_reserve" ) * fTimeRate;
	m_dwMotionEndTime = dwCurTime + pGrp->GetAnimationFullTime( iAniID ) * fTimeRate;

	pOwner->SetAutoTarget( ATT_COUNTER );

	DWORD dwTrackingTime = dwCurTime + pGrp->GetAnimationEventTime_e( iAniID, "tracking_e" ) * fTimeRate + dwPreDelay;
	pOwner->SetAutoTargetTrackingEndTime( dwTrackingTime );

	m_ChargeComboState = CCS_Normal_Combo;

	m_bAttackKeyReserved = false;
	m_bDefenseKeyReserved = false;
	m_bJumpKeyReserved = false;

	//RedHood
	SetRedHoodMacro( pGrp, iAniID, fTimeRate, pAttribute.m_dwEndDelay );

	if ( pOwner->IsNeedProcess() )
	{
		ioUserKeyInput::DirKeyInput eNewDirKey = ioUserKeyInput::DKI_NONE;
		if ( pOwner->IsSettedDirection() )
		{
			eNewDirKey = pOwner->CheckCurDirKey();
			pOwner->SetTargetRotToDirKey( eNewDirKey );
		}

		if( pOwner->IsNeedSendNetwork() )
		{
			SP2Packet kPacket( CUPK_COUNTER_ATTACK_SYNC );
			kPacket << pOwner->GetCharName();
			kPacket << (int)GetType();
			kPacket << pOwner->GetHP().m_fCurValue;
			kPacket << CCS_Normal_Combo;
			kPacket << m_nCurCounterCombo;
			kPacket << pOwner->GetWorldOrientation();
			pOwner->FillAutoTargetInfo( kPacket );
			P2PNetwork::SendToAllPlayingUser( kPacket );
		}
	}

	m_nCurCounterCombo++;
}

void ioChargeComboCounter::SetNormalChargeAttackState( ioBaseChar *pOwner )
{
	if( !pOwner ) 
		return;

	ioEntityGroup *pGrp = pOwner->GetGroup();
	int iAniID = pGrp->GetAnimationIdx( m_CounterCharge.m_AttackAnimation );
	if ( iAniID == -1 )
		return;

	float fTimeRate  = m_CounterCharge.m_fAttackAniRate;
	DWORD dwPreDelay = m_CounterCharge.m_dwPreDelay;

	pGrp->ClearAllLoopAni( FLOAT100, true );
	pGrp->ClearAllActionAni( FLOAT100, true );
	pGrp->SetActionAni( iAniID, FLOAT100, FLOAT100, FLOAT1, FLOAT1/fTimeRate, 0.0f, dwPreDelay );	

	pOwner->ClearReservedSliding();
	pOwner->SetReservedSliding( m_CounterCharge.m_vForceInfoList, iAniID, fTimeRate, dwPreDelay );

	pOwner->CheckCharColSkipTime( iAniID, fTimeRate, dwPreDelay );
	pOwner->SetBaseAttackExtraAniJump( iAniID, fTimeRate, dwPreDelay );

	pOwner->RefreshFireTimeList( iAniID, m_CounterCharge.m_vWeaponInfoList, FTT_NORMAL_ATTACK, fTimeRate, dwPreDelay );

	DWORD dwCurTime = FRAMEGETTIME();
	m_dwMotionEndTime = dwCurTime + pGrp->GetAnimationFullTime( iAniID ) * fTimeRate;

	pOwner->SetAutoTarget( ATT_COUNTER );

	DWORD dwTrackingTime = dwCurTime + pGrp->GetAnimationEventTime_e( iAniID, "tracking_e" ) * fTimeRate + dwPreDelay;
	pOwner->SetAutoTargetTrackingEndTime( dwTrackingTime );

	m_ChargeComboState = CCS_Normal_Charge_Attack;

	if( pOwner->IsNeedSendNetwork() )
	{
		SP2Packet kPacket( CUPK_COUNTER_ATTACK_SYNC );
		kPacket << pOwner->GetCharName();
		kPacket << (int)GetType();
		kPacket << pOwner->GetHP().m_fCurValue;
		kPacket << CCS_Normal_Charge_Attack;
		pOwner->FillAutoTargetInfo( kPacket );
		P2PNetwork::SendToAllPlayingUser( kPacket );
	}
}

void ioChargeComboCounter::CheckCharging( ioBaseChar *pOwner )
{
	if( !pOwner->IsNeedProcess() )	
		return;

	if( pOwner->IsAttackKeyDown() && !pOwner->IsHasCrown() )
	{
		if( m_dwCounterStartTime + m_dwChargeTime <= FRAMEGETTIME() )
		{
			if( !SetChargeState( pOwner ) )
			{
				SetNormalComboState( pOwner );
			}
		}
	}
	else
	{
		SetNormalComboState( pOwner );
	}
}


void ioChargeComboCounter::CheckchargingRedHood( ioBaseChar *pOwner )
{
	if( !pOwner->IsNeedProcess() )	
		return;

	if( pOwner->IsAttackKeyDown() && !pOwner->IsHasCrown() )
	{
		if( m_dwRedHoodComboStartTime + m_dwChargeTime <= FRAMEGETTIME() )
		{
			if( !SetChargeState( pOwner ) )
			{
				SetNormalComboState( pOwner );
			}
		}
	}
	else
	{
		SetNormalComboState( pOwner );
	}
}


void ioChargeComboCounter::CheckComboCharging( ioBaseChar *pOwner )
{
	if( !pOwner->IsNeedProcess() )	
		return;

	if( pOwner->IsAttackKeyDown() )
		m_bAttackKeyReserved = true;

	if( pOwner->IsAttackKeyDown() && !pOwner->IsHasCrown() )
	{
		if( m_dwComboStartTime + m_dwChargeTime <= FRAMEGETTIME() )
		{
			if( !SetChargeState( pOwner ) )
			{
				SetNormalComboState( pOwner );
			}
		}
	}
	else
	{
		m_dwComboStartTime = FRAMEGETTIME();
	}
}

bool ioChargeComboCounter::CheckComboInput( ioBaseChar *pOwner )
{
	if ( !pOwner || !pOwner->IsNeedProcess() )
		return false;

	DWORD dwCurTime = FRAMEGETTIME();
	if ( pOwner->IsAttackKey() && m_dwEnableReserveKeyTime > 0 && m_dwEnableReserveKeyTime < dwCurTime && m_nCurCounterCombo < m_nCounterMaxCombo )
	{
		SetNormalComboState( pOwner );
		return true;
	}

	return false;
}

void ioChargeComboCounter::ReserveInputKey(ioBaseChar *pOwner, bool bEnableAttack, bool bEnableDefence, bool bEnableJump )
{
	if ( !pOwner || !pOwner->IsNeedProcess() )
		return;

	if( bEnableAttack && pOwner->IsAttackKey() )
		m_bAttackKeyReserved = true;

	if( bEnableDefence && pOwner->IsDefenseKey() )
		m_bDefenseKeyReserved = true;

	if( bEnableJump && pOwner->IsJumpKey() )
		m_bJumpKeyReserved = true;
}

void ioChargeComboCounter::CheckReserveState( ioBaseChar *pOwner )
{
	if ( !pOwner || !pOwner->IsNeedProcess() )
		return;

	if( m_bAttackKeyReserved )
	{
		SetReserveAttack( pOwner );
		return;
	}
	else if( m_bDefenseKeyReserved )
	{
		SetReserveDefence( pOwner );
		return;
	}
	else if ( m_bJumpKeyReserved )
	{
		SetReserveJump( pOwner );
		return;
	}

	m_ChargeComboState = CCS_End;

	if( pOwner->IsNeedSendNetwork() )
	{
		SP2Packet kPacket( CUPK_COUNTER_ATTACK_SYNC );
		kPacket << pOwner->GetCharName();
		kPacket << (int)GetType();
		kPacket << pOwner->GetHP().m_fCurValue;
		kPacket << CCS_End;
		P2PNetwork::SendToAllPlayingUser( kPacket );
	}
}

void ioChargeComboCounter::SetReserveAttack( ioBaseChar *pOwner )
{
	if ( !pOwner || !pOwner->IsNeedProcess() )
		return;

	pOwner->SetNormalAttack(0);

	m_bAttackKeyReserved = false;
}

void ioChargeComboCounter::SetReserveDefence( ioBaseChar *pOwner )
{
	if ( !pOwner || !pOwner->IsNeedProcess() )
		return;

	pOwner->SetDefenseState();
	m_bDefenseKeyReserved = false;
}

void ioChargeComboCounter::SetReserveJump( ioBaseChar *pOwner )
{
	if ( !pOwner || !pOwner->IsNeedProcess() )
		return;

	pOwner->SetJumpState();
	m_bJumpKeyReserved = false;	
}

bool ioChargeComboCounter::IsFloatState( ioBaseChar *pOwner )
{	
	if ( !pOwner )
		return false;

	ioPlayStage *pStage = pOwner->GetCreator();
	if ( !pStage )
		return false;

	D3DXVECTOR3 vPos = pOwner->GetWorldPosition();
	float fMapHeight = pStage->GetMapHeightOnlyTerrain( vPos.x, vPos.z );

	if ( fabs( fMapHeight - vPos.y ) >= m_fJumpHeightGap )
	{
		return true;
	}

	return false;
}


void ioChargeComboCounter::SetRedHoodMacro( ioEntityGroup* pGrp, int iAniID, float fTimeRate, DWORD dwEndTime )
{
	m_dwRedHoodSpecialMoveMacroTime_S = 0;
	m_dwRedHoodSpecialMoveMacroTime_E = 0;
	if( !pGrp )
		return;

	DWORD dwCurTime = FRAMEGETTIME();
	DWORD dwSpecialMoveMacroTime_S = pGrp->GetAnimationEventTime_e( iAniID, "red_hood_s" ) * fTimeRate;
	DWORD dwSpecialMoveMacroTime_E = pGrp->GetAnimationEventTime_e( iAniID, "red_hood_e" ) * fTimeRate;
	if( dwSpecialMoveMacroTime_S )
	{
		m_dwRedHoodSpecialMoveMacroTime_S = dwCurTime + dwSpecialMoveMacroTime_S;
		m_dwRedHoodSpecialMoveMacroTime_E = dwCurTime + dwSpecialMoveMacroTime_E + dwEndTime;
	}
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// 반격 특별 기본 클래스

ExtendCounter::ExtendCounter()
{
	m_dwMotionEndTime = 0;
}

ExtendCounter::ExtendCounter( const ExtendCounter &rhs )
{
	m_dwMotionEndTime = 0;
}

ExtendCounter::~ExtendCounter()
{
}

ExtendCounter* ExtendCounter::Clone()
{
	return new ExtendCounter( *this );
}

void ExtendCounter::CounterAttack( ioBaseChar *pOwner, ioChargeComboCounter* pCounter, const AttackAttribute* pAttr,  bool bAmtReset /*= true */ )
{
	if( !pOwner || !pCounter || !pAttr )
		return;

	ioEntityGroup *pGrp = pOwner->GetGroup();
	int iAniID = pGrp->GetAnimationIdx( pAttr->m_AttackAnimation );
	if ( iAniID == -1 )
		return;

	pOwner->InitExtendAttackTagInfo();
	pOwner->InitActionStopDelay( true );

	float fTimeRate  = pAttr->m_fAttackAniRate;
	DWORD dwPreDelay = pAttr->m_dwPreDelay;

	pGrp->ClearAllLoopAni( FLOAT100, true );
	pGrp->ClearAllActionAni( FLOAT100, true );
	pGrp->SetActionAni( iAniID, FLOAT100, FLOAT100, FLOAT1, FLOAT1/fTimeRate, 0.0f, dwPreDelay );	

	pOwner->ClearReservedSliding();
	if( bAmtReset )
	{
		pOwner->SetCurJumpPowerAmt( 0.0f );
		pOwner->SetForcePowerAmt( 0.0f );
		pOwner->SetGravityAmt( 0.0f );
	}
	pOwner->SetReservedSliding( pAttr->m_vForceInfoList, iAniID, fTimeRate, dwPreDelay );	
	pOwner->CheckCharColSkipTime( iAniID, fTimeRate, dwPreDelay );
	pOwner->RefreshFireTimeList( iAniID, pAttr->m_vWeaponInfoList, FTT_COUNTER_ATTACK, fTimeRate, dwPreDelay );

	DWORD dwCurTime = FRAMEGETTIME();
	pCounter->SetEnableReserveKeyTime( dwCurTime + pGrp->GetAnimationEventTime_e( iAniID, "key_reserve" ) * fTimeRate );
	m_dwMotionEndTime = dwCurTime + pGrp->GetAnimationFullTime( iAniID ) * fTimeRate;

	pOwner->SetAutoTarget( ATT_COUNTER );

	DWORD dwTrackingTime = dwCurTime + pGrp->GetAnimationEventTime_e( iAniID, "tracking_e" ) * fTimeRate + dwPreDelay;
	pOwner->SetAutoTargetTrackingEndTime( dwTrackingTime );

	//RedHood
	pCounter->SetRedHoodMacro( pGrp, iAniID, fTimeRate, pAttr->m_dwEndDelay );

	//////////////////////////////////////////////////////////////////////////
	pOwner->SetBaseAttackExtraAniJump( iAniID, fTimeRate, dwPreDelay );
	pCounter->SetAniRotate( pOwner, iAniID, fTimeRate );
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// 쉐도우 레인져

ioShadowChargeComboCounter::ioShadowChargeComboCounter()
{

}

ioShadowChargeComboCounter::ioShadowChargeComboCounter( const ioShadowChargeComboCounter &rhs ) : ExtendCounter( rhs ),
	m_ShadowRangerTeleportMove( rhs.m_ShadowRangerTeleportMove ),
	m_nShadowRangerTeleportComboMaxCnt( rhs.m_nShadowRangerTeleportComboMaxCnt ),
	m_ShadowRangerComboAttList( rhs.m_ShadowRangerComboAttList ),
	m_vShadowRangerTeleportOffset( rhs.m_vShadowRangerTeleportOffset )
{

}

ioShadowChargeComboCounter::~ioShadowChargeComboCounter()
{

}

void ioShadowChargeComboCounter::LoadProperty( ioINILoader &rkLoader )
{
	char szBuf[MAX_PATH];

	LoadAttackAttribute_e( "counter_shadow_ranger_teleport_move", m_ShadowRangerTeleportMove, rkLoader );

	m_nShadowRangerTeleportComboMaxCnt = rkLoader.LoadInt_e( "counter_shadow_ranger_combo_cnt", 0 );
	m_ShadowRangerComboAttList.clear();
	for( int i = 0; i < m_nShadowRangerTeleportComboMaxCnt; ++i )
	{
		AttackAttribute kAttribute;
		wsprintf_e( szBuf, "counter_shadow_ranger_combo_attack%d", i+1 );
		LoadAttackAttribute( szBuf, kAttribute, rkLoader );
		m_ShadowRangerComboAttList.push_back( kAttribute );
	}
	
	m_vShadowRangerTeleportOffset.x = rkLoader.LoadFloat_e( "counter_shadow_ranger_teleport_move_offset_x", 0.0f );
	m_vShadowRangerTeleportOffset.y = rkLoader.LoadFloat_e( "counter_shadow_ranger_teleport_move_offset_y", 0.0f );
	m_vShadowRangerTeleportOffset.z = rkLoader.LoadFloat_e( "counter_shadow_ranger_teleport_move_offset_z", 0.0f );
}

ExtendCounter* ioShadowChargeComboCounter::Clone()
{
	return new ioShadowChargeComboCounter( *this );
}

bool ioShadowChargeComboCounter::SetChargeState( ioBaseChar *pOwner, ioChargeComboCounter* pCounter )
{
	if( !pOwner || !pCounter )
		return false;
	
	ioPlayStage *pStage = pOwner->GetCreator();
	if ( !pStage )
		return false;

	CounterAttack( pOwner, pCounter, &m_ShadowRangerTeleportMove );

	pCounter->SetChargeComboState( ioChargeComboCounter::CCS_Special_Charge_Attack );
	pCounter->SetAttackKeyReserved( false );
	pCounter->SetDefenseKeyReserved( false );
	pCounter->SetJumpKeyReserved( false );

	m_ShadowSpecialState				= SSS_Move;
	m_nShadowRangerTeleportComboCurCnt	= 0;

	if ( pOwner->IsNeedProcess() )
	{
		D3DXVECTOR3 vTeleportPos	= pOwner->GetWorldPosition();
		D3DXQUATERNION qtRot		= pOwner->GetWorldOrientation();

		vTeleportPos += ( qtRot * m_vShadowRangerTeleportOffset );

		float fMapHeight = pStage->GetMapHeight( vTeleportPos.x, vTeleportPos.z );
		if( fMapHeight > 0 && vTeleportPos.y < fMapHeight )
			vTeleportPos.y = fMapHeight;

		D3DXVECTOR3 vDir = qtRot * -ioMath::UNIT_Z;
		ioMath::CalcDirectionQuaternion( qtRot, vDir );

		pOwner->SetWorldPosition( vTeleportPos );
		pOwner->SetTargetRotAndMoveDirChange( qtRot );

		if( pOwner->IsNeedSendNetwork() )
		{
			SP2Packet kPacket( CUPK_COUNTER_ATTACK_SYNC );
			kPacket << pOwner->GetCharName();
			kPacket << (int)pCounter->GetType();
			kPacket << pOwner->GetHP().m_fCurValue;
			kPacket << ioChargeComboCounter::CCS_Special_Charge_Attack;
			kPacket << pOwner->GetWorldPosition();
			kPacket << pOwner->GetWorldOrientation();
			pOwner->FillAutoTargetInfo( kPacket );
			P2PNetwork::SendToAllPlayingUser( kPacket );
		}
	}

	return true;
}

void ioShadowChargeComboCounter::SetShadowRangerComboState( ioBaseChar *pOwner, ioChargeComboCounter* pCounter )
{
	if ( !pOwner || !pCounter || m_ShadowRangerComboAttList.empty() )
		return;

	if ( !COMPARE( m_nShadowRangerTeleportComboCurCnt, 0, (int)m_ShadowRangerComboAttList.size() ) )
		return;

	//연타시 4타 이상일때 타격 가능하게 만듬
	pOwner->InitActionStopDelay( true );

	const AttackAttribute &pAttribute = m_ShadowRangerComboAttList[m_nShadowRangerTeleportComboCurCnt];
	CounterAttack( pOwner, pCounter, &pAttribute, false );

	m_ShadowSpecialState = SSS_Combo;
		
	pCounter->SetAttackKeyReserved( false );
	pCounter->SetDefenseKeyReserved( false );
	pCounter->SetJumpKeyReserved( false );
	
	if ( pOwner->IsNeedProcess() )
	{
		ioUserKeyInput::DirKeyInput eNewDirKey = ioUserKeyInput::DKI_NONE;
		if ( pOwner->IsSettedDirection() )
		{
			eNewDirKey = pOwner->CheckCurDirKey();
			pOwner->SetTargetRotToDirKey( eNewDirKey );
		}
		
		if( pOwner->IsNeedSendNetwork() )
		{
			SP2Packet kPacket( CUPK_COUNTER_ATTACK_SYNC );
			kPacket << pOwner->GetCharName();
			kPacket << (int)pCounter->GetType();
			kPacket << pOwner->GetHP().m_fCurValue;
			kPacket << ioChargeComboCounter::CCS_Special_Process;
			kPacket << SSS_Combo;
			kPacket << m_nShadowRangerTeleportComboCurCnt;
			kPacket << pOwner->GetWorldOrientation();
			pOwner->FillAutoTargetInfo( kPacket );
			P2PNetwork::SendToAllPlayingUser( kPacket );
		}
	}

	m_nShadowRangerTeleportComboCurCnt++;
}

void ioShadowChargeComboCounter::ProcessSpecialCAT( ioBaseChar *pOwner, ioChargeComboCounter* pCounter )
{
	if( !pOwner || !pCounter )
		return;

	DWORD dwCurTime = FRAMEGETTIME();
	switch( m_ShadowSpecialState )
	{
	case SSS_Move:
	case SSS_Combo:
		{
			if( pOwner->IsNeedProcess() )
			{
				if( m_dwMotionEndTime < dwCurTime )
				{
					pCounter->CheckReserveState( pOwner );
				}
				else
				{
					if( pCounter->GetEnableReserveKeyTime() > 0 && pCounter->GetEnableReserveKeyTime() < dwCurTime )
						pCounter->ReserveInputKey( pOwner, false, true, true );
				}

				CheckShadowRangerComboInput( pOwner, pCounter );
			}
		}
		break;
	}
}

bool ioShadowChargeComboCounter::CheckShadowRangerComboInput( ioBaseChar *pOwner, ioChargeComboCounter* pCounter )
{
	if ( !pOwner || !pOwner->IsNeedProcess() )
		return false;
		
	if( pOwner->IsAttackKey() )
	{
		DWORD dwCurTime = FRAMEGETTIME();
		if( pCounter->GetEnableReserveKeyTime() > 0 && pCounter->GetEnableReserveKeyTime() < dwCurTime && m_nShadowRangerTeleportComboCurCnt < m_nShadowRangerTeleportComboMaxCnt )
		{
			SetShadowRangerComboState( pOwner, pCounter );
			return true;
		}
	}

	return false;
}

void ioShadowChargeComboCounter::ApplySpecialCAT( ioBaseChar *pOwner, ioChargeComboCounter* pCounter, SP2Packet &rkPacket )
{
	int nSpecialState;
	rkPacket >> nSpecialState;

	switch ( nSpecialState )
	{
	case SSS_Combo:
		{
			rkPacket >> m_nShadowRangerTeleportComboCurCnt;
			D3DXQUATERNION qtRot;
			rkPacket >> qtRot;

			pOwner->SetTargetRotAndMoveDirChange( qtRot );
			SetShadowRangerComboState( pOwner, pCounter );
			pOwner->ApplyAutoTargetInfo( rkPacket );
		}
		break;
	}
}

bool ioShadowChargeComboCounter::IsNoDropState() const
{
	switch ( m_ShadowSpecialState )
	{
	case SSS_Move:
	case SSS_Combo:
		return true;
	}

	return false;
}


///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// 이순신

ioLeeSunSinExtendCounter::ioLeeSunSinExtendCounter()
{
	
}

ioLeeSunSinExtendCounter::ioLeeSunSinExtendCounter( const ioLeeSunSinExtendCounter &rhs ) : ExtendCounter( rhs ),
	m_ExtendCounter( rhs.m_ExtendCounter ),
	m_FullExtendCounter( rhs.m_FullExtendCounter )
{

}

ioLeeSunSinExtendCounter::~ioLeeSunSinExtendCounter()
{

}

void ioLeeSunSinExtendCounter::LoadProperty( ioINILoader &rkLoader )
{	
	LoadAttackAttribute_e( "extend_counter", m_ExtendCounter, rkLoader );
	LoadAttackAttribute_e( "full_extend_counter", m_FullExtendCounter, rkLoader );
}

ExtendCounter* ioLeeSunSinExtendCounter::Clone()
{
	return new ioLeeSunSinExtendCounter( *this );
}


bool ioLeeSunSinExtendCounter::SetChargeState( ioBaseChar *pOwner, ioChargeComboCounter* pCounter )
{
	if( !pOwner || !pCounter )
		return false;

	ioLeeSunSin* pItem = ToLeeSunSin( ToAttackableItem( pOwner->GetPriorityItem( SPT_DEFENSE ) ) );
	if( !pItem || pItem->GetNeedBullet() > pItem->GetCurBullet() )
		return false;
	
	if( pItem->GetCurBullet() >= pItem->GetMaxBullet() )
	{
		CounterAttack( pOwner, pCounter, &m_FullExtendCounter );
		pItem->SetUseExtraGauge( pItem->GetNeedGaugeByMax() );
	}
	else
	{
		CounterAttack( pOwner, pCounter, &m_ExtendCounter );
		pItem->SetUseExtraGauge( pItem->GetNeedBullet() );
	}

	pCounter->SetChargeComboState( ioChargeComboCounter::CCS_Special_Charge_Attack );
	pCounter->SetAttackKeyReserved( false );
	pCounter->SetDefenseKeyReserved( false );
	pCounter->SetJumpKeyReserved( false );

	return true;
}

void ioLeeSunSinExtendCounter::ProcessSpecialCAT( ioBaseChar *pOwner, ioChargeComboCounter* pCounter )
{
	if( !pOwner || !pCounter )
		return;

	if( pOwner->IsNeedProcess() )
	{
			DWORD dwCurTime = FRAMEGETTIME();
		if( m_dwMotionEndTime < dwCurTime )
		{
			pCounter->CheckReserveState( pOwner );
		}
		else
		{
			if( pCounter->GetEnableReserveKeyTime() > 0 && pCounter->GetEnableReserveKeyTime() < dwCurTime )
				pCounter->ReserveInputKey( pOwner, false, true, true );
		}
	}
}

void ioLeeSunSinExtendCounter::ApplySpecialCAT( ioBaseChar *pOwner, ioChargeComboCounter* pCounter, SP2Packet &rkPacket )
{
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// RedHood

ioRedHoodExtendCounter::ioRedHoodExtendCounter()
{

}

ioRedHoodExtendCounter::ioRedHoodExtendCounter( const ioRedHoodExtendCounter &rhs ) : ExtendCounter( rhs ),
	m_ChargeAttackList( rhs.m_ChargeAttackList )
{

}

ioRedHoodExtendCounter::~ioRedHoodExtendCounter()
{

}

void ioRedHoodExtendCounter::LoadProperty( ioINILoader &rkLoader )
{
	char szBuf[MAX_PATH];
	int nMaxChargeCnt = rkLoader.LoadInt_e( "max_counter_charge_attack_cnt", 0 );

	m_ChargeAttackList.clear();
	for( int i=0; i < nMaxChargeCnt; ++i )
	{
		AttackAttribute kAttribute;
		wsprintf_e( szBuf, "counter_charge_attack%d", i+1 );
		LoadAttackAttribute( szBuf, kAttribute, rkLoader );
		m_ChargeAttackList.push_back( kAttribute );
	}
}

ExtendCounter* ioRedHoodExtendCounter::Clone()
{
	return new ioRedHoodExtendCounter( *this );
}


bool ioRedHoodExtendCounter::SetChargeState( ioBaseChar *pOwner, ioChargeComboCounter* pCounter )
{
	if( !pOwner || !pCounter )
		return false;

	if( (int)m_ChargeAttackList.size() <= pCounter->m_nCurCounterCombo )
		return false;

	const AttackAttribute &pAttribute = m_ChargeAttackList[pCounter->m_nCurCounterCombo];
	CounterAttack( pOwner, pCounter, &pAttribute, false );

	pCounter->SetChargeComboState( ioChargeComboCounter::CCS_Special_Charge_Attack );
	pCounter->SetAttackKeyReserved( false );
	pCounter->SetDefenseKeyReserved( false );
	pCounter->SetJumpKeyReserved( false );

	if( pOwner->IsNeedSendNetwork() )
	{
		SP2Packet kPacket( CUPK_COUNTER_ATTACK_SYNC );
		kPacket << pOwner->GetCharName();
		kPacket << (int)pCounter->GetType();
		kPacket << pOwner->GetHP().m_fCurValue;
		kPacket << ioChargeComboCounter::CCS_Special_Charge_Attack;
		kPacket << pOwner->GetWorldPosition();
		kPacket << pOwner->GetWorldOrientation();
		kPacket << pCounter->m_nCurCounterCombo;
		pOwner->FillAutoTargetInfo( kPacket );
		P2PNetwork::SendToAllPlayingUser( kPacket );
	}

	return true;
}

void ioRedHoodExtendCounter::ProcessSpecialCAT( ioBaseChar *pOwner, ioChargeComboCounter* pCounter )
{
	if( !pOwner || !pCounter )
		return;

	if( pOwner->IsNeedProcess() )
	{
		DWORD dwCurTime = FRAMEGETTIME();
		if( m_dwMotionEndTime < dwCurTime )
		{
			pCounter->CheckReserveState( pOwner );
		}
		else
		{
			if( pCounter->GetEnableReserveKeyTime() > 0 && pCounter->GetEnableReserveKeyTime() < dwCurTime )
				pCounter->ReserveInputKey( pOwner, false, true, true );
		}
	}
}

void ioRedHoodExtendCounter::ApplySpecialCAT( ioBaseChar *pOwner, ioChargeComboCounter* pCounter, SP2Packet &rkPacket )
{
}