
#include "stdafx.h"

#include "ioBaseChar.h"

#include "ioObjectAttachItem.h"

#include "WeaponDefine.h"

ioObjectAttachItem::ioObjectAttachItem()
{
	Init();
}

ioObjectAttachItem::ioObjectAttachItem( const ioObjectAttachItem &rhs )
	: ioObjectItem( rhs ),
	  m_dwProtectTime( rhs.m_dwProtectTime ),
	  m_NormalAttackInfo( rhs.m_NormalAttackInfo ),
	  m_ChargeAttackInfo( rhs.m_ChargeAttackInfo ),
	  m_szDelayFrontMove( rhs.m_szDelayFrontMove ),
	  m_szDelayBackMove( rhs.m_szDelayBackMove ),
	  m_szDelayRightMove( rhs.m_szDelayRightMove ),
	  m_szDelayLeftMove( rhs.m_szDelayLeftMove ),
	  m_szCheckBuff( rhs.m_szCheckBuff ),
	  m_dwEnableInputKeyTime( rhs.m_dwEnableInputKeyTime ),
	  m_bCharSkipState( rhs.m_bCharSkipState )
{
	Init();
}

ioObjectAttachItem::~ioObjectAttachItem()
{
}

void ioObjectAttachItem::LoadProperty( ioINILoader &rkLoader )
{
	ioObjectItem::LoadProperty( rkLoader );
	
	m_dwProtectTime = (DWORD)rkLoader.LoadInt_e( "protect_time", 0 );

	char szBuf[MAX_PATH];
	LoadAttackAttribute( "normal_attack_info", m_NormalAttackInfo, rkLoader );
	LoadAttackAttribute( "charge_attack_info", m_ChargeAttackInfo, rkLoader );
	
	rkLoader.LoadString_e( "delay_front_move", "", szBuf, MAX_PATH );
	m_szDelayFrontMove = szBuf;
	rkLoader.LoadString_e( "delay_back_move", "", szBuf, MAX_PATH );
	m_szDelayBackMove = szBuf;
	rkLoader.LoadString_e( "delay_right_move", "", szBuf, MAX_PATH );
	m_szDelayRightMove = szBuf;
	rkLoader.LoadString_e( "delay_left_move", "", szBuf, MAX_PATH );
	m_szDelayLeftMove = szBuf;

	rkLoader.LoadString_e( "check_buff", "", szBuf, MAX_PATH );
	m_szCheckBuff = szBuf;

	m_dwEnableInputKeyTime = (DWORD)rkLoader.LoadInt_e( "key_enable_time", 0 );
	m_bCharSkipState = rkLoader.LoadBool_e( "set_char_skip_state", false );
}

ioItem* ioObjectAttachItem::Clone()
{
	return new ioObjectAttachItem( *this );
}

ioObjectItem::ObjectSubType ioObjectAttachItem::GetObjectSubType() const
{
	return OST_ATTACH;
}

//공격 설정(초기화)
void ioObjectAttachItem::SetNormalAttack( ioBaseChar *pOwner, int iCurCombo )
{
}

//차징 공격 확인
void ioObjectAttachItem::CheckNormalAttack( ioBaseChar *pOwner )
{	
}

//점프 공격 설정(초기화)
bool ioObjectAttachItem::SetJumpAttack( ioBaseChar *pOwner, int iAniID, float fTimeRate, AttackType eType )
{
	if( !pOwner ) 
		return false;

	ClearAirJump();

	AttackType eExtendType = AT_JUMP_ATTACK;

	const AttackAttribute *pAttr = GetAttackAttribute( eExtendType );
	if( !pAttr )
	{
		LOG.PrintTimeAndLog( 0, "ioObjectAttachItem::SetJumpAttack - AttackAttribute Not Exist" );
		return false;
	}

	ioEntityGroup *pGrp = pOwner->GetGroup();
	iAniID = pGrp->GetAnimationIdx( pAttr->m_AttackAnimation );
	if( iAniID == -1 )
	{
		LOG.PrintTimeAndLog( 0, "ioObjectAttachItem::SetJumpAttack - JumpAttack Animation Not Exist" );
		return false;
	}
	
	fTimeRate = pAttr->m_fAttackAniRate;

	SetAirJump( pOwner, iAniID, fTimeRate );

	return false;
}

void ioObjectAttachItem::ProcessJumpAttack( ioBaseChar *pOwner, float fHeightGap )
{
	if( m_pExtendJump )
	{
		m_pExtendJump->ProcessJumpAttack( pOwner, fHeightGap );
	}

	CheckAirJump( pOwner );
}

//동기화
void ioObjectAttachItem::OnItemGaugeOK( ioBaseChar *pOwner, SP2Packet &rkPacket )
{
	if( !pOwner ) return;

	int iState;
	rkPacket >> iState;

	switch ( iState )
	{
	case PS_NORMAL:
		SetNormalAttackState( pOwner );
		break;
	case PS_CHARGE:
		SetChargeAttackState( pOwner );
		break;
	case PS_CHARGE_END:
		SetChargeEndState( pOwner );
		break;
	}
}

//아이템 장착 시(초기화)
void ioObjectAttachItem::OnEquiped( ioBaseChar *pOwner )
{
	if( !pOwner )
		return;

	ioObjectItem::OnEquiped( pOwner );

	Init();

	//무적 시간 초기화
	DWORD dwCurTime = FRAMEGETTIME();
	m_dwCheckProtectTime = dwCurTime;
}

void ioObjectAttachItem::OnReleased( ioBaseChar *pOwner )
{
	if( !pOwner )
		return;

	ioObjectItem::OnReleased( pOwner );
}

//추가 무기 설정
void ioObjectAttachItem::SetExtraInfoToWeapon( ioWeapon *pWeapon, bool bJumpAttack )
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

//무적 상태인가
bool ioObjectAttachItem::IsProtectState()
{
	if( m_dwProtectTime > 0 && m_dwCheckProtectTime + m_dwProtectTime > FRAMEGETTIME() )
		return true;

	return false;
}

bool ioObjectAttachItem::IsOneTimeWeaponItem() const
{
	return false;
}

//캐릭터 통과 상태
bool ioObjectAttachItem::IsCharCollisionSkipState()
{
	return m_bCharSkipState;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////
//초기화
void ioObjectAttachItem::Init()
{
	m_WeaponState = WS_NONE;
	m_ChargingState = CS_NONE;
	m_iCurAirJump = 0;
	m_fAirJumpPowerRate = 0.0f;

	m_dwCheckProtectTime = 0;
	m_dwAttackStartTime = 0;

	m_StartDirKey = ioUserKeyInput::DKI_NONE;
	m_bSetDelay = false;
}

//특별 프로세스
void ioObjectAttachItem::ProcessState( ioBaseChar *pOwner )
{
	if( !pOwner )
		return;

	DWORD dwCurTime = FRAMEGETTIME();

	switch( m_ChargingState )
	{
	case CS_NONE:
		SetDeleayState( pOwner );
		break;
	case CS_DELAY:
		{
			CheckDelayMoveAni( pOwner );
			CheckInputKey( pOwner );
		}
		break;
	case CS_Check_Input:
		{
			if ( pOwner->IsNeedProcess() )
			{
				pOwner->ChangeDirectionByInputDir( true );

				if ( pOwner->IsAttackKeyDown() )
				{
					if ( m_dwAttackStartTime + m_dwChargeTime <= dwCurTime )
						SetChargeAttackState( pOwner );
				}
				else
					SetNormalAttackState( pOwner );
			}
		}
		break;
	case CS_Normal_Attack:
		{
			if ( m_dwMotionEndTime > 0 && m_dwMotionEndTime < dwCurTime )
			{
				SetDeleayState( pOwner );
				return;
			}
		}
		break;
	case CS_Charge_Attack:
		{
			if ( m_dwMotionEndTime > 0 && m_dwMotionEndTime < dwCurTime )
			{
				SetChargeEndState( pOwner );
				return;
			}
		}
		break;
	case CS_END:
		break;
	}
}

void ioObjectAttachItem::SetDeleayState( ioBaseChar *pOwner )
{
	if ( !pOwner )
		return;

	m_StartDirKey = pOwner->GetCurDirKey();

	ioEntityGroup *pGrp = pOwner->GetGroup();
	pGrp->ClearAllActionAni( FLOAT100, true );
	pGrp->ClearAllLoopAni( FLOAT100, true );

	if ( pOwner->IsSettedDirection() )
	{
		m_bSetDelay = false;

		ioUserKeyInput::DirKeyInput eXformDir;
		eXformDir = pOwner->GetXformDir( m_StartDirKey );

		switch( eXformDir )
		{
		case ioUserKeyInput::DKI_UP:
			pGrp->SetLoopAni( m_szDelayFrontMove, FLOAT100 );
			break;
		case ioUserKeyInput::DKI_RIGHTUP:
		case ioUserKeyInput::DKI_RIGHT:
		case ioUserKeyInput::DKI_RIGHTDOWN:
			pGrp->SetLoopAni( m_szDelayRightMove, FLOAT100 );
			break;
		case ioUserKeyInput::DKI_DOWN:
			pGrp->SetLoopAni( m_szDelayBackMove, FLOAT100 );
			break;
		case ioUserKeyInput::DKI_LEFTDOWN:
		case ioUserKeyInput::DKI_LEFT:
		case ioUserKeyInput::DKI_LEFTUP:
			pGrp->SetLoopAni( m_szDelayLeftMove, FLOAT100 );
			break;
		}
	}
	else
	{
		m_bSetDelay = true;
		pGrp->SetLoopAni( m_DelayAnimation, FLOAT100 );
	}

	m_ChargingState = CS_DELAY;

	if( pOwner->IsNeedProcess() )
		pOwner->ChangeDirectionByInputDir( true );
}

//일반 공격
void ioObjectAttachItem::SetNormalAttackState( ioBaseChar *pOwner )
{
	if( !pOwner ) 
		return;

	ClearCancelInfo();

	ioEntityGroup *pGrp = pOwner->GetGroup();

	int nAniID = pGrp->GetAnimationIdx( m_NormalAttackInfo.m_AttackAnimation );
	if( nAniID == -1 )
		return;

	float fTimeRate = m_NormalAttackInfo.m_fAttackAniRate;
	DWORD dwPreDelay = m_NormalAttackInfo.m_dwPreDelay;

	pGrp->ClearAllActionAni( FLOAT100, true );
	pGrp->ClearAllLoopAni( FLOAT100, true );

	pGrp->SetActionAni( nAniID, FLOAT100, FLOAT100, FLOAT1, FLOAT1/fTimeRate, 0.0f, dwPreDelay );

	DWORD dwCurTime = FRAMEGETTIME();	
	m_dwMotionEndTime = dwCurTime;

	//종료 시간 설정
	m_dwMotionEndTime += pGrp->GetAnimationFullTime( nAniID ) * fTimeRate;
	m_dwMotionEndTime += dwPreDelay;
	
	pOwner->ClearReservedSliding();

	//캐릭터 충돌 해제
	pOwner->CheckCharColSkipSkillTime( nAniID, fTimeRate, dwPreDelay );
	
	//무기생성
	pOwner->RefreshFireTimeListWithBase( pOwner->IncreaseWeaponIndexBase(), nAniID, m_NormalAttackInfo.m_vWeaponInfoList, FTT_NORMAL_ATTACK, fTimeRate, dwPreDelay );

	//슬라이딩 설정
	pOwner->SetReservedSliding( m_NormalAttackInfo.m_vForceInfoList, nAniID, fTimeRate, dwPreDelay );
	
	//이동 방향 재설정
	pOwner->SetMoveDirByRotate( pOwner->GetWorldOrientation() );

	m_ChargingState = CS_Normal_Attack;

	if( pOwner->IsNeedSendNetwork() )
	{
		SP2Packet kPacket( CUPK_ITEM_GAUGE_OK );
		FillItemGaugePacketHeader( pOwner, kPacket );
		kPacket << PS_NORMAL;
		P2PNetwork::SendToAllPlayingUser( kPacket );
	}
}

void ioObjectAttachItem::SetChargeAttackState( ioBaseChar *pOwner )
{
	if ( !pOwner )
		return;

	ioEntityGroup *pGrp = pOwner->GetGroup();

	int nAniID = pGrp->GetAnimationIdx( m_ChargeAttackInfo.m_AttackAnimation );
	if( nAniID == -1 )
		return;

	float fTimeRate = m_ChargeAttackInfo.m_fAttackAniRate;
	DWORD dwPreDelay = m_ChargeAttackInfo.m_dwPreDelay;

	pGrp->ClearAllActionAni( FLOAT100, true );
	pGrp->ClearAllLoopAni( FLOAT100, true );

	pGrp->SetActionAni( nAniID, FLOAT100, FLOAT100, FLOAT1, FLOAT1/fTimeRate, 0.0f, dwPreDelay );

	DWORD dwCurTime = FRAMEGETTIME();	
	m_dwMotionEndTime = dwCurTime;

	//종료 시간 설정
	m_dwMotionEndTime += pGrp->GetAnimationFullTime( nAniID ) * fTimeRate;
	m_dwMotionEndTime += dwPreDelay;
	
	pOwner->ClearReservedSliding();

	//캐릭터 충돌 해제
	pOwner->CheckCharColSkipSkillTime( nAniID, fTimeRate, dwPreDelay );

	//무기생성
	pOwner->RefreshFireTimeListWithBase( pOwner->IncreaseWeaponIndexBase(), nAniID, m_ChargeAttackInfo.m_vWeaponInfoList, FTT_NORMAL_ATTACK, fTimeRate, dwPreDelay );

	//슬라이딩 설정
	pOwner->SetReservedSliding( m_ChargeAttackInfo.m_vForceInfoList, nAniID, fTimeRate, dwPreDelay );

	m_ChargingState = CS_Charge_Attack;

	if( pOwner->IsNeedSendNetwork() )
	{
		SP2Packet kPacket( CUPK_ITEM_GAUGE_OK );
		FillItemGaugePacketHeader( pOwner, kPacket );
		kPacket << PS_CHARGE;
		P2PNetwork::SendToAllPlayingUser( kPacket );
	}
}

void ioObjectAttachItem::SetChargeEndState( ioBaseChar *pOwner )
{
	if ( !pOwner )
		return;

	m_ChargingState = CS_END;

	pOwner->SetState( CS_DELAY );

	//pOwner->RemoveBuff( m_szCheckBuff );
	
	if( pOwner->IsNeedProcess() )
		pOwner->ChangeDirectionByInputDir( false );

	if( pOwner->IsNeedSendNetwork() )
	{
		SP2Packet kPacket( CUPK_ITEM_GAUGE_OK );
		FillItemGaugePacketHeader( pOwner, kPacket );
		kPacket << PS_CHARGE_END;
		P2PNetwork::SendToAllPlayingUser( kPacket );
	}
}

bool ioObjectAttachItem::IsEnableMoveState()
{
	if( m_ChargingState == CS_DELAY )
		return true;
	
	return false;
}

void ioObjectAttachItem::CheckInputKey( ioBaseChar *pOwner )
{
	if ( !pOwner || !pOwner->IsNeedProcess() )
		return;
	
	DWORD dwCurTime = FRAMEGETTIME();

	if( pOwner->IsJumpKey() && !pOwner->IsBuffLimitJump() )
	{
		if( pOwner->IsCanJumpState() )
		{
			pOwner->SetJumpState();
			return;
		}
	}

	if ( m_dwAttackStartTime + m_dwEnableInputKeyTime < dwCurTime &&
		 pOwner->IsAttackKeyDown() )
	{
		m_dwAttackStartTime = dwCurTime;
		m_ChargingState = CS_Check_Input;
		return;
	}
}

void ioObjectAttachItem::CheckDelayMoveAni( ioBaseChar *pOwner )
{
	if( pOwner->IsSettedDirection() )
	{
		bool bChangeDir = pOwner->IsChangeDirection();
		if( bChangeDir )
		{	
			m_bSetDelay = false;

			ioEntityGroup *pGrp = pOwner->GetGroup();
			pGrp->ClearAllActionAni( FLOAT100, true );
			pGrp->ClearAllLoopAni( FLOAT100, true );
			
			ioUserKeyInput::DirKeyInput eXformDir;
			eXformDir = pOwner->GetXformDir( m_StartDirKey );

			switch( eXformDir )
			{
			case ioUserKeyInput::DKI_UP:
				pGrp->SetLoopAni( m_szDelayFrontMove, FLOAT100 );
				break;
			case ioUserKeyInput::DKI_RIGHTUP:
			case ioUserKeyInput::DKI_RIGHT:
			case ioUserKeyInput::DKI_RIGHTDOWN:
				pGrp->SetLoopAni( m_szDelayRightMove, FLOAT100 );
				break;
			case ioUserKeyInput::DKI_DOWN:
				pGrp->SetLoopAni( m_szDelayBackMove, FLOAT100 );
				break;
			case ioUserKeyInput::DKI_LEFTDOWN:
			case ioUserKeyInput::DKI_LEFT:
			case ioUserKeyInput::DKI_LEFTUP:
				pGrp->SetLoopAni( m_szDelayLeftMove, FLOAT100 );
				break;
			}
		}
	}
	else
	{		
		if( !m_bSetDelay )
		{
			ioEntityGroup *pGrp = pOwner->GetGroup();
			pGrp->ClearAllActionAni( FLOAT100, true );
			pGrp->ClearAllLoopAni( FLOAT100, true );
			pGrp->SetLoopAni( m_DelayAnimation, FLOAT100 );

			m_bSetDelay = true;
		}
	}
}

//-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

ioObjectAttachSpecialState::ioObjectAttachSpecialState()
{
}

ioObjectAttachSpecialState::~ioObjectAttachSpecialState()
{
}

void ioObjectAttachSpecialState::Initialize( ioBaseChar* pOwner, CharState eCurrState )
{
	ioSpecialStateBase::Initialize( pOwner, eCurrState );
}

void ioObjectAttachSpecialState::Finalize( ioBaseChar* pOwner, CharState eCurrState, CharState eNewState )
{
	ioSpecialStateBase::Finalize( pOwner, eCurrState, eNewState );
}

bool ioObjectAttachSpecialState::IsCharCollisionSkipState( const ioBaseChar *const pOwner, DWORD dwStartTime, DWORD dwEndTime, DWORD dwSkipType, bool bTeamOnly, bool bDefense, TeamType eOwnerTeam, TeamType eOtherTeam ) const
{
	ioObjectItem *pObjectItem = pOwner->GetObject();
	if ( pObjectItem )
	{
		ioObjectAttachItem *pAttachItem = ToObjectAttachItem( pObjectItem );
		if( pAttachItem && pAttachItem->IsCharCollisionSkipState() )
			return true;
	}
	return false;
}

void ioObjectAttachSpecialState::CheckSpecialState( ioBaseChar* pOwner )
{
	ioObjectAttachItem *pAttachItem = ToObjectAttachItem( pOwner->GetPriorityItem( SPT_ATTACK ) );
	if( pAttachItem )
	{
		int iSkillNum = pOwner->GetSkillKeyInput();
		if( iSkillNum >= 0 && !pOwner->IsBuffLimitSkill() )
		{
			pOwner->SetUseSkill( iSkillNum, SUT_NORMAL );
			return;
		}

		pAttachItem->ProcessState( pOwner );
	}
	else
	{
		pOwner->SetState( CS_DELAY );
		pOwner->SetFallState( true );
		return;
	}	
}

void ioObjectAttachSpecialState::CheckFallStateChange( ioBaseChar* pOwner, float fMinFallHeightRate, float fBottomHeight, bool bBottomMaxCheck, float fMaxBottomHeight )
{
	return;
}

void ioObjectAttachSpecialState::ProcessKeyInput( ioBaseChar* pOwner )
{
	pOwner->ChangeDirectionByInputDir( true );
}