
#include "stdafx.h"

#include "ioThunderBirdHoldBuff.h"

#include "ioPlayStage.h"
#include "ioBaseChar.h"

ioThunderBirdHoldBuff::ioThunderBirdHoldBuff()
{
	m_bSetTeamAttack = false;
}

ioThunderBirdHoldBuff::ioThunderBirdHoldBuff( const ioThunderBirdHoldBuff &rhs )
	: ioBuff( rhs ),
	  m_FloatAnimation( rhs.m_FloatAnimation ),
	  m_bDisableDownState( rhs.m_bDisableDownState ),
	  m_OwnerBuffList( rhs.m_OwnerBuffList ),
	  m_bNoCancel( rhs.m_bNoCancel ),
	  m_fEndJumpPower( rhs.m_fEndJumpPower ),
	  m_fKeyInputMoveSpeed( rhs.m_fKeyInputMoveSpeed ),
	  m_bIgnoreWeaponItem( rhs.m_bIgnoreWeaponItem )
{
}

ioThunderBirdHoldBuff::~ioThunderBirdHoldBuff()
{
}

void ioThunderBirdHoldBuff::LoadProperty( ioINILoader &rkLoader )
{
	ioBuff::LoadProperty( rkLoader );

	char szBuf[MAX_PATH] = "";
	rkLoader.LoadString_e( "float_animation", "", szBuf, MAX_PATH );
	m_FloatAnimation = szBuf;
	
	m_bDisableDownState  = rkLoader.LoadBool_e( "disable_down_state", false );
	
	m_bNoCancel = rkLoader.LoadBool_e( "no_cancel", false );
	m_fEndJumpPower = rkLoader.LoadFloat_e( "jump_power", 0.0f );

	m_fKeyInputMoveSpeed = rkLoader.LoadFloat_e( "key_input_move_speed", 0.0f );
	m_bIgnoreWeaponItem = rkLoader.LoadBool_e( "ignore_weapon_item", false );
	
	
	LoadOwnerBuffList( rkLoader );
}

ioBuff* ioThunderBirdHoldBuff::Clone()
{
	return new ioThunderBirdHoldBuff( *this );
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void ioThunderBirdHoldBuff::StartBuff( ioBaseChar *pOwner )
{
	ioBuff::StartBuff( pOwner );

	m_bSetTeamAttack = false;
	m_bFloating = false;
	SetFloatState( pOwner );
}

bool ioThunderBirdHoldBuff::ApplyStartBuff(ioBaseChar *pOwner, DWORD dwGlobalTime, DWORD dwGapTime )
{
	if( !ioBuff::ApplyStartBuff( pOwner, dwGlobalTime, dwGapTime ) )
		return false;

	m_bSetTeamAttack = false;
	m_bFloating = false;
	SetFloatState( pOwner );

	return true;
}

void ioThunderBirdHoldBuff::ProcessBuff( float fTimePerSec )
{
	switch( m_iOperationType )
	{
	case OT_SWITCH:
		if( !CheckSwitchType( fTimePerSec ) )
		{
			SetReserveEndBuff();
			return;
		}

		CheckCallingProcess();
		break;
	case OT_DELAY:
	case OT_ACTIVE:
		ioBuff::ProcessBuff( fTimePerSec );
		break;
	case OT_PASSIVE:
		CheckCallingAreaWeapon();
		break;
	}

	if ( CheckOwnerStateCheck( m_pOwner ) )
	{
		SetReserveEndBuff();
		return;
	}
}

void ioThunderBirdHoldBuff::EndBuff()
{
	ProcessEnd( m_pOwner );

	ioBuff::EndBuff();
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

//버프 리스트 읽기
void ioThunderBirdHoldBuff::LoadOwnerBuffList( ioINILoader &rkLoader )
{
	int iCnt = rkLoader.LoadInt_e( "owner_buff_cnt", 0 );
	m_OwnerBuffList.clear();

	if( iCnt <= 0 ) 
		return;

	m_OwnerBuffList.reserve( iCnt );

	char szTitle[MAX_PATH], szBuf[MAX_PATH];
	for( int i=0; i < iCnt; ++i )
	{
		wsprintf_e( szTitle, "owner_buff%d", i+1 );
		rkLoader.LoadString( szTitle, "", szBuf, MAX_PATH );

		m_OwnerBuffList.push_back( szBuf );
	}
}

void ioThunderBirdHoldBuff::SetOwnerBuffList( ioBaseChar *pOwner )
{
	if ( !pOwner )
		return;

	ioPlayStage *pStage = pOwner->GetCreator();
	if ( !pStage )
		return;

	if( m_OwnerBuffList.empty() ) 
		return;

	int nBuffCnt = m_OwnerBuffList.size();

	//해당 버프를 생성
	for( int i=0; i < nBuffCnt; ++i )
	{
		ioHashString szBuffName = m_OwnerBuffList[i];

		if( !szBuffName.IsEmpty() )
			//pOwner->AddNewBuff( szBuffName, pOwner->GetCharName(), "", NULL );	
			pOwner->ReserveAddNewBuff( szBuffName, pOwner->GetCharName(), "", NULL );
	}
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void ioThunderBirdHoldBuff::SetFloatState( ioBaseChar *pOwner )
{
	if ( !pOwner )
		return;

	if( m_FloatAnimation.IsEmpty() )
		return;

	ioEntityGroup *pGrp = pOwner->GetGroup();
	pGrp->ClearAllLoopAni( 0.0f, true );
	pGrp->ClearAllActionAni( 0.0f, true );
	pGrp->SetLoopAni( m_FloatAnimation, FLOAT100 );

	pOwner->SetState( CS_THUNDER_BIRD_HOLD );

	SetOwnerBuffList( pOwner );
}

bool ioThunderBirdHoldBuff::CheckOwnerStateCheck( ioBaseChar *pOwner )
{
	if( !pOwner ) 
		return false;

	CharState eState = pOwner->GetState();

	bool bDown = false;
	if( m_bDisableDownState )
	{
		if( eState == CS_FROZEN && pOwner->GetBlowFrozenState() )
			bDown = true;
		else if( eState == CS_BLOW_WOUNDED && !pOwner->IsFloatingState() )
			bDown = true;
		else if( eState == CS_BOUND_BLOW_WOUNDED && !pOwner->IsFloatingState() )
			bDown = true;
		else if( eState == CS_ETC_STATE && pOwner->IsApplyDownState(false) )
			bDown = true;
		else if( eState == CS_STOP_MOTION && pOwner->GetBlowStopMotionState() )
			bDown = true;
		else if( eState == CS_DROP_ZONE_DOWN )
			bDown = true;
		else if( eState == CS_ICE_STATE && pOwner->GetBlowStopMotionState() )
			bDown = true;

		if( bDown )
			return true;
	}

	bool bWound = false;
	if( m_bEnableWoundCancel )
	{
		if( eState == CS_BLOW_WOUNDED )
			bWound = true;
		else if( eState == CS_BOUND_BLOW_WOUNDED )
			bWound = true;
		else if( eState == CS_WOUNDED )
			bWound = true;
		else if( eState == CS_ETC_STATE )
			bWound = true;

		if( bWound )
			return true;
	}

	return false;
}

void ioThunderBirdHoldBuff::ProcessEnd( ioBaseChar *pOwner )
{
	if ( !pOwner )
		return;
	/*
	int nBuffCnt = (int)m_OwnerBuffList.size();	
	for ( int i=0; i<nBuffCnt; ++i )
		pOwner->RemoveBuff( m_OwnerBuffList[i], true );	
	*/
	if( pOwner->GetState() == CS_THUNDER_BIRD_HOLD )
	{
		if ( m_bSetTeamAttack )
		{
			if ( m_bFloating )
				pOwner->SetSKillEndJumpState( m_fEndJumpPower, false, true, false, true );
			else
				pOwner->SetState( CS_DELAY );
		}
		else
		{
			pOwner->SetCurJumpPowerAmt( 0.0f );
			pOwner->SetForcePowerAmt( 0.0f );
			pOwner->SetGravityAmt( 0.0f );

			BlowWoundAniInfo kInfo;
			pOwner->SetBlowWoundedState( true, kInfo );
		}
	}
}

bool ioThunderBirdHoldBuff::IsNoCancel()
{
	return m_bNoCancel;
}

void ioThunderBirdHoldBuff::SetTeamAttack( bool bTeamAttack, bool bFloating )
{
	m_bSetTeamAttack = bTeamAttack;
	m_bFloating = bFloating;
}

//-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

ioThunderBirdHoldSpecialState::ioThunderBirdHoldSpecialState()
{
}

ioThunderBirdHoldSpecialState::~ioThunderBirdHoldSpecialState()
{
}

void ioThunderBirdHoldSpecialState::Initialize( ioBaseChar* pOwner, CharState eCurrState )
{
	ioSpecialStateBase::Initialize( pOwner, eCurrState );
}

void ioThunderBirdHoldSpecialState::Finalize( ioBaseChar* pOwner, CharState eCurrState, CharState eNewState )
{
	ioSpecialStateBase::Finalize( pOwner, eCurrState, eNewState );
}

void ioThunderBirdHoldSpecialState::CheckSpecialState( ioBaseChar* pOwner )
{
	int nSkillNum = pOwner->GetSkillKeyInput();
	if( nSkillNum >= 0 && !pOwner->IsBuffLimitSkill() )
		pOwner->SetUseSkill( nSkillNum, SUT_NORMAL );
}

void ioThunderBirdHoldSpecialState::CheckFallStateChange( ioBaseChar* pOwner, float fMinFallHeightRate, float fBottomHeight, bool bBottomMaxCheck, float fMaxBottomHeight )
{
	return;
}

bool ioThunderBirdHoldSpecialState::ProcessMove( ioBaseChar* pOwner, float fTimePerSec, DWORD dwPreTime, DWORD dwCurTime )
{
	ioThunderBirdHoldBuff *pHodeBuff = ToThunderBirdHoldBuff( pOwner->GetBuff( BT_THUNDER_BIRD_HOLD ) );
	if ( pHodeBuff && pHodeBuff->GetKeyInputMoveSpeed() != 0.0f )
	{
		if ( pOwner->IsSettedDirection() )
		{
			ioUserKeyInput::DirKeyInput ePossessionKey = pOwner->GetCurDirKey();
			if( ePossessionKey != ioUserKeyInput::DKI_NONE )
				pOwner->SetCurMoveSpeed( pHodeBuff->GetKeyInputMoveSpeed() );
		}
		else
			pOwner->SetCurMoveSpeed( 0 );
	}

	return false;
}

void ioThunderBirdHoldSpecialState::ProcessKeyInput( ioBaseChar* pOwner )
{
	if ( pOwner->IsChangeDirection() )
	{
		ioThunderBirdHoldBuff *pHodeBuff = ToThunderBirdHoldBuff( pOwner->GetBuff( BT_THUNDER_BIRD_HOLD ) );
		if ( pHodeBuff && pHodeBuff->GetKeyInputMoveSpeed() != 0.0f )
			pOwner->ChangeDirectionByInputDir(false);
	}
}
