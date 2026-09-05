

#include "stdafx.h"

#include "ioBaseChar.h"
#include "ioVehicleDummy.h"

#include "ioVehicleDriverObjectItem.h"
#include "ioPassengerBuff.h"

#include "ioPlayStage.h"

ioPassengerBuff::ioPassengerBuff()
{
	Init();
}

ioPassengerBuff::ioPassengerBuff( const ioPassengerBuff &rhs )
	: ioBuff( rhs ),
	m_iOffsetY( rhs.m_iOffsetY ),
	m_iOffsetZ( rhs.m_iOffsetZ ),
	m_szGetOnEffect( rhs.m_szGetOnEffect ),
	m_szWeaponEffect( rhs.m_szWeaponEffect ),
	m_MoveAnimation( rhs.m_MoveAnimation ),
	m_fMoveAniRate( rhs.m_fMoveAniRate ),
	m_JumpReadyAnimation( rhs.m_JumpReadyAnimation ),
	m_fJumpReadyAniRate( rhs.m_fJumpReadyAniRate ),
	m_JumppingAnimation( rhs.m_JumppingAnimation ),
	m_fJumppingAniRate( rhs.m_fJumppingAniRate ),
	m_JumpLandingAnimation( rhs.m_JumpLandingAnimation ),
	m_fJumpLandingAniRate( rhs.m_fJumpLandingAniRate ),
	m_DriftLeftAnimation( rhs.m_DriftLeftAnimation ),
	m_fDriftLeftAniRate( rhs.m_fDriftLeftAniRate ),
	m_DrifRightAnimation( rhs.m_DrifRightAnimation ),
	m_fDrifRightAniRate( rhs.m_fDrifRightAniRate ),
	m_LeftWeaponInfo( rhs.m_LeftWeaponInfo ),
	m_RightWeaponInfo( rhs.m_RightWeaponInfo ),
	m_szLBipedName( rhs.m_szLBipedName ),
	m_szRBipedName( rhs.m_szRBipedName ),
	m_dwFireGapTime( rhs.m_dwFireGapTime ),
	m_fEndJumpPower( rhs.m_fEndJumpPower ),
	m_iPassengerMax( rhs.m_iPassengerMax ),
	m_dwJumpKeyInputCheckTime( rhs.m_dwJumpKeyInputCheckTime ),
	m_GetOnDisableBuffList( rhs.m_GetOnDisableBuffList ),
	m_GetOnAfterList( rhs.m_GetOnAfterList )
{
	Init();
}

ioPassengerBuff::~ioPassengerBuff()
{
}

void ioPassengerBuff::LoadProperty( ioINILoader &rkLoader )
{
	ioBuff::LoadProperty( rkLoader );

	char szBuf[MAX_PATH] = "";
	char szKey[MAX_PATH] = "";
	
	m_iOffsetY = rkLoader.LoadInt_e( "offset_y", 0 );
	m_iOffsetZ = rkLoader.LoadInt_e( "offset_z", 0 );

	rkLoader.LoadString_e( "get_on_effect", "", szBuf, MAX_PATH );
	m_szGetOnEffect = szBuf;

	rkLoader.LoadString_e( "weapon_effect", "", szBuf, MAX_PATH );
	m_szWeaponEffect = szBuf;

	rkLoader.LoadString_e( "move_animation", "", szBuf, MAX_PATH );
	m_MoveAnimation = szBuf;
	m_fMoveAniRate = rkLoader.LoadFloat_e( "move_ani_rate", FLOAT1 );

	rkLoader.LoadString_e( "jump_ready_animation", "", szBuf, MAX_PATH );
	m_JumpReadyAnimation = szBuf;
	m_fJumpReadyAniRate  = rkLoader.LoadFloat_e( "jump_ready_animation_rate", FLOAT1 );

	rkLoader.LoadString_e( "jumpping_animation", "", szBuf, MAX_PATH );
	m_JumppingAnimation = szBuf;
	m_fJumppingAniRate  = rkLoader.LoadFloat_e( "jumpping_animation_rate", FLOAT1 );

	rkLoader.LoadString_e( "jump_landing_animation", "", szBuf, MAX_PATH );
	m_JumpLandingAnimation = szBuf;
	m_fJumpLandingAniRate  = rkLoader.LoadFloat_e( "jump_landing_animation_rate", FLOAT1 );

	rkLoader.LoadString_e( "drift_left_animation", "", szBuf, MAX_PATH );
	m_DriftLeftAnimation = szBuf;
	m_fDriftLeftAniRate  = rkLoader.LoadFloat_e( "drift_left_animation_rate", FLOAT1 );

	rkLoader.LoadString_e( "drift_right_animation", "", szBuf, MAX_PATH );
	m_DrifRightAnimation = szBuf;
	m_fDrifRightAniRate  = rkLoader.LoadFloat_e( "drift_right_animation_rate", FLOAT1 );

	LoadWeaponInfo( "left_attack", m_LeftWeaponInfo, rkLoader );
	LoadWeaponInfo( "right_attack", m_RightWeaponInfo, rkLoader );

	rkLoader.LoadString_e( "left_biped_name", "Bip01 L Hand", szBuf, MAX_PATH );
	m_szLBipedName = szBuf;

	rkLoader.LoadString_e( "right_biped_name", "Bip01 R Hand", szBuf, MAX_PATH );
	m_szRBipedName = szBuf;

	m_dwFireGapTime				= rkLoader.LoadInt_e( "fire_gap_time", 300 );
	m_fEndJumpPower				= rkLoader.LoadFloat_e( "end_jump_power", 0.0f );

	m_iPassengerMax				= rkLoader.LoadInt_e( "passenger_max", 1 );
	m_dwJumpKeyInputCheckTime	= rkLoader.LoadInt_e( "check_jump_key_input_time", 400 );

	int iBuffListMax = rkLoader.LoadInt_e( "get_on_disable_buff_max", 0 );
	for( int i = 0; i < iBuffListMax; ++i )
	{
		wsprintf_e( szKey, "get_on_disable_buff%d", i + 1 );
		rkLoader.LoadString( szKey, "", szBuf, MAX_PATH );
		m_GetOnDisableBuffList.push_back( szBuf );
	}

	iBuffListMax = rkLoader.LoadInt_e( "get_on_after_buff_max", 0 );
	for( int i = 0; i < iBuffListMax; ++i )
	{
		wsprintf_e( szKey, "get_on_after%d", i + 1 );
		rkLoader.LoadString( szKey, "", szBuf, MAX_PATH );
		m_GetOnAfterList.push_back( szBuf );
	}
}

ioBuff* ioPassengerBuff::Clone()
{
	return new ioPassengerBuff( *this );
}

void ioPassengerBuff::Init()
{
	m_PassengerState		= PS_NONE;
	m_eDriverState			= ioVehicleDriverObjectItem::VDS_NONE;
	m_dwGetOnReadyStartTime	= 0;
	m_dwFireStartTime		= 0;
	m_WeaponEffectID		= -1;
	m_dwJumpKeyInputStart	= 0;
}

void ioPassengerBuff::StartBuff( ioBaseChar *pOwner )
{
	ioBuff::StartBuff( pOwner );

	Init();
	ChangeToGetOnReady( pOwner );
}

bool ioPassengerBuff::ApplyStartBuff(ioBaseChar *pOwner, DWORD dwGlobalTime, DWORD dwGapTime )
{
	if( !ioBuff::ApplyStartBuff( pOwner, dwGlobalTime, dwGapTime ) )
		return false;

	Init();
	ChangeToGetOnReady( pOwner );

	return true;
}

void ioPassengerBuff::ProcessBuff( float fTimePerSec )
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
		ProcessDelay( fTimePerSec );
		break;
	case OT_ACTIVE:
		ProcessActive( fTimePerSec );
		break;
	case OT_PASSIVE:
		CheckCallingAreaWeapon();
		break;
	}

	ProcesssState( fTimePerSec );
}


void ioPassengerBuff::ProcessDelay( float fTimePerSec )
{
	DWORD dwCurTime = FRAMEGETTIME();

	if( CheckEnableDelayState() )
		return;

	if( GetBuffDuration() == 0 )
	{
		if( !m_bBuffStart && m_dwBuffStartTime < dwCurTime )
		{
			SetAction();
		}
		return;
	}

	if( !m_bBuffStart && m_dwBuffStartTime < dwCurTime )
	{
		SetAction();
	}

	if( m_dwBuffEndTime <= dwCurTime )
	{		
		SetReserveEndBuff();
		return;
	}
}

void ioPassengerBuff::ProcessActive( float fTimePerSec )
{
	if( CheckEnableDelayState() )
		return;
	
	if( 0 < GetBuffDuration() && m_dwBuffEndTime <= FRAMEGETTIME() )
	{		
		SetReserveEndBuff();
		return;
	}
}

void ioPassengerBuff::ProcesssState( float fTimePerSec )
{
	ioBaseChar* pOwner		= GetOwner();
	ioBaseChar* pCreator	= GetCreator();

	if( !pOwner || !pCreator )
	{
		SetReserveEndBuff();
		return;
	}

	if( m_PassengerState == PS_GET_ON && pCreator->GetState() != CS_VEHICLE_DRIVER )
	{
		SetReserveEndBuff();
		return;
	}

	switch( m_PassengerState )
	{
	case PS_GET_ON_READY:
		{
			CheckGetOnReady( pOwner );
		}
		break;
	case PS_GET_ON:
		{
			CheckGetOn( pOwner, pCreator );
		}
		break;
	}
}

void ioPassengerBuff::ChangeToGetOnReady( ioBaseChar* pOwner )
{
	m_PassengerState = PS_GET_ON_READY;
}

void ioPassengerBuff::CheckGetOnReady( ioBaseChar* pOwner )
{
	if( !pOwner->IsNeedProcess() )
		return;

	DWORD dwCurTime = FRAMEGETTIME();
	if( m_dwJumpKeyInputStart == 0 )
	{
		if( pOwner->IsJumpKeyDown() )
		{
			m_dwJumpKeyInputStart = dwCurTime;
		}
	}
	else if( m_dwJumpKeyInputStart + m_dwJumpKeyInputCheckTime < dwCurTime )
	{
		m_dwJumpKeyInputStart = 0;
	}
	else
	{
		ioObjectItem* pObject = pOwner->GetObject();

		//오브젝트 장착한 유저는 탑승불가
		if( pObject )
			return;
		
		//탑승불가 대상 버프를 가지고 있는 유저 검사(EX 날개류 등)
		for( ioHashStringVec::iterator iter = m_GetOnDisableBuffList.begin(); iter != m_GetOnDisableBuffList.end(); ++iter )
		{
			const ioHashString& szBuffName = *iter;
			if( pOwner->HasBuff( *iter ) )
				return;
		}

		//점프키 입력후 점프상태가 된유저만 탑승
		if( IsEnableGetOn( pOwner, pOwner->GetCreator() ) && pOwner->GetState() == CS_JUMP )
			ChangeToGetOn( pOwner );
	}
}

bool ioPassengerBuff::IsEnableGetOn( ioBaseChar* pOwner, ioPlayStage* pStage )
{
	if( !pStage || !pOwner || !pOwner->IsNeedProcess() )
		return false;


#ifndef SHIPPING
	//테스트용
	if( m_iPassengerMax == 0 )
		return true;
#endif

	//BaseCharList는 const 형태만 제공함으로 for문을 두번나누어서 탑승유저의 ioBaseChar 포인터에 접근
	ioHashStringVec CandidateList;
	const BaseCharList& rkList = pStage->GetBaseCharList();
	for( BaseCharList::const_iterator iter = rkList.begin(); iter != rkList.end(); ++iter )
	{
		const ioBaseChar* pTarget = *iter;
		if( !pTarget )
			continue;

		if( !pTarget->HasBuff( BT_PASSENGER_BUFF ) )
			continue;

		if( pTarget->GetCharName() == pOwner->GetCharName() )
			continue;

		CandidateList.push_back( pTarget->GetCharName() );
	}
	
	int iBuffUserCount = 0;
	for( ioHashStringVec::iterator iter = CandidateList.begin(); iter != CandidateList.end(); ++iter )
	{
		ioBaseChar* pTarget = pStage->GetBaseChar( *iter );
		if( !pTarget )
			continue;

		ioPassengerBuff* pBuff = ToPassengerBuff( pTarget->GetBuff( BT_PASSENGER_BUFF ) );
		if( !pBuff )
			continue;

		//탑승했는지 여부 체크
		if( pBuff->GetPassengerState() != ioPassengerBuff::PS_GET_ON )
			continue;

		iBuffUserCount++;
	}

	if( iBuffUserCount < m_iPassengerMax )
		return true;

	return false;
}

void ioPassengerBuff::ChangeToGetOn( ioBaseChar* pOwner )
{
	pOwner->AttachEffect( m_szGetOnEffect );
	ioEffect* pEffect = pOwner->AttachEffect( m_szWeaponEffect );
	if( pEffect )
	{
		m_WeaponEffectID = pEffect->GetUniqueID();
	}

	pOwner->SetState( CS_PASSENGER );
	m_PassengerState = PS_GET_ON;

	for( ioHashStringVec::iterator iter = m_GetOnAfterList.begin(); iter != m_GetOnAfterList.end(); ++iter )
	{
		const ioHashString& szBuffName = *iter;
		if( !szBuffName.IsEmpty() )
			pOwner->ReserveAddNewBuff( szBuffName, pOwner->GetCharName(), m_ItemName, NULL );
	}

	if ( pOwner->IsNeedSendNetwork() )
	{
		SP2Packet kPacket( CUPK_APPLY_EXTRA_BUFF_INFO );
		kPacket << pOwner->GetCharName();
		kPacket << GetName();
		kPacket << (byte)PSS_GET_ON;
		P2PNetwork::SendToAllPlayingUser( kPacket );
	}
}
void ioPassengerBuff::CheckGetOn( ioBaseChar* pOwner, ioBaseChar* pCreator )
{
	ProcessAttach( pOwner, pCreator );
	CheckCratorState( pOwner, pCreator );
	CheckAttackInput( pOwner );
}

void ioPassengerBuff::CheckCratorState( ioBaseChar* pOwner, ioBaseChar* pCreator )
{
	ioVehicleDirverState* pState = ToVehicleDirverState( pCreator->GetSpecialState() );
	if( pState && pState->GetDriverState() != m_eDriverState )
	{
		m_eDriverState = pState->GetDriverState();
		switch( m_eDriverState )
		{
		case ioVehicleDriverObjectItem::VDS_RUN:
			SetRunState( pOwner );
			break;			
		case ioVehicleDriverObjectItem::VDS_JUMP_READY:
			SetJumpReadyState( pOwner );
			break;
		case ioVehicleDriverObjectItem::VDS_JUMPPING:
			SetJummping( pOwner );
			break;
		case ioVehicleDriverObjectItem::VDS_LADING:
			SetLanding( pOwner );
			break;
		case ioVehicleDriverObjectItem::VDS_DRIFT:
			SetDrift( pOwner, pState->GetDriftLeft() );
			break;
		}
	}
}

void ioPassengerBuff::ProcessAttach( ioBaseChar* pOwner, ioBaseChar* pCreator )
{
	D3DXVECTOR3 vDir = pCreator->GetTargetRot() * D3DXVECTOR3(0.0f, 0.0f, -FLOAT1);
	D3DXVec3Normalize( &vDir, &vDir );

	D3DXVECTOR3 vPos = pCreator->GetWorldPosition();
	vPos	-= m_iOffsetZ * vDir;
	vPos.y	+= m_iOffsetY;

	pOwner->SetWorldPosition( vPos );	
	pOwner->SetTargetRotToRotate( pCreator->GetWorldOrientation(), true );
}

void ioPassengerBuff::SetRunState( ioBaseChar* pOwner )
{
	if( pOwner )
	{
		ioEntityGroup* pGroup = pOwner->GetGroup();
		if( pGroup )
		{
			if( !m_MoveAnimation.IsEmpty() )
			{
				int iAniID = pGroup->GetAnimationIdx( m_MoveAnimation );
				if( iAniID != -1 )
				{
					pGroup->ClearAllActionAni( FLOAT100, true );
					pGroup->ClearAllLoopAni( FLOAT100, true );
					pGroup->SetLoopAni( iAniID, FLOAT100, FLOAT1, m_fMoveAniRate );
				}
			}
		}
	}
}

void ioPassengerBuff::SetJumpReadyState( ioBaseChar* pOwner )
{
	if( pOwner )
	{
		ioEntityGroup* pGroup = pOwner->GetGroup();
		if( pGroup )
		{
			if( !m_JumpReadyAnimation.IsEmpty() )
			{
				int iAniID = pGroup->GetAnimationIdx( m_JumpReadyAnimation );
				if( iAniID != -1 )
				{
					pGroup->ClearAllActionAni( FLOAT100, true );
					pGroup->ClearAllLoopAni( FLOAT100, true );
					pGroup->SetActionAni( iAniID, FLOAT100, FLOAT1, m_fJumpReadyAniRate );
				}
			}
		}
	}
}

void ioPassengerBuff::SetJummping( ioBaseChar* pOwner )
{
	if( pOwner )
	{
		ioEntityGroup* pGroup = pOwner->GetGroup();
		if( pGroup )
		{
			if( !m_JumppingAnimation.IsEmpty() )
			{
				int iAniID = pGroup->GetAnimationIdx( m_JumppingAnimation );
				if( iAniID != -1 )
				{
					pGroup->ClearAllActionAni( FLOAT100, true );
					pGroup->ClearAllLoopAni( FLOAT100, true );
					pGroup->SetActionAni( iAniID, FLOAT100, FLOAT1, m_fJumppingAniRate );
				}
			}
		}
	}
}

void ioPassengerBuff::SetLanding( ioBaseChar* pOwner )
{
	if( pOwner )
	{
		ioEntityGroup* pGroup = pOwner->GetGroup();
		if( pGroup )
		{
			if( !m_JumpLandingAnimation.IsEmpty() )
			{
				int iAniID = pGroup->GetAnimationIdx( m_JumpLandingAnimation );
				if( iAniID != -1 )
				{
					pGroup->ClearAllActionAni( FLOAT100, true );
					pGroup->ClearAllLoopAni( FLOAT100, true );
					pGroup->SetActionAni( iAniID, FLOAT100, FLOAT1, m_fJumpLandingAniRate );
				}
			}
		}
	}
}

void ioPassengerBuff::SetDrift( ioBaseChar* pOwner, bool bLeft )
{
	ioHashString szDriftAnimation;
	float fDriftRate = FLOAT1;

	if( bLeft )
	{
		szDriftAnimation	= m_DriftLeftAnimation;
		fDriftRate			= m_fDriftLeftAniRate;
	}
	else
	{
		szDriftAnimation	= m_DrifRightAnimation;
		fDriftRate			= m_fDrifRightAniRate;
	}

	if( !szDriftAnimation.IsEmpty() )
	{
		if( pOwner )
		{
			ioEntityGroup* pGroup = pOwner->GetGroup();
			if( pGroup )
			{
				int iAniID = pGroup->GetAnimationIdx( szDriftAnimation );
				if( iAniID != -1 )
				{
					pGroup->ClearAllActionAni( FLOAT100, true );
					pGroup->ClearAllLoopAni( FLOAT100, true );
					pGroup->SetActionAni( iAniID, FLOAT100, FLOAT1, fDriftRate );
				}
			}
		}
	}
}

void ioPassengerBuff::SetFire( ioBaseChar* pOwner, const ioHashString& szBipedName, const WeaponInfo& Weapon, const D3DXVECTOR3& vAttackDir )
{
	if( !pOwner )
		return;

	ioEntityGroup* pGroup = pOwner->GetGroup();
	if( !pGroup )
		return;
	
	ioBiped *pHand = pGroup->GetBiped( szBipedName );
	if( pHand )
	{
		D3DXVECTOR3 vFirePos = pHand->GetDerivedPosition() * pOwner->GetWorldScale();
		vFirePos = pOwner->GetWorldOrientation() * vFirePos + pOwner->GetWorldPosition();
		AttackFire( pOwner, Weapon, vAttackDir, vFirePos );
	}
}

void ioPassengerBuff::AttackFire( IN ioBaseChar* pOwner, IN const WeaponInfo& rkAttribute, IN const D3DXVECTOR3& vFireDir, IN const D3DXVECTOR3& vFirePos )
{
	FireTime kFireTime;
	kFireTime.dwStart				= FRAMEGETTIME();
	kFireTime.dwDuration			= 1;
	kFireTime.iAttributeIdx			= rkAttribute.m_iWeaponIdx;
	kFireTime.iResistanceIdx		= rkAttribute.m_iResistanceIdx;
	kFireTime.szWoundedAni			= rkAttribute.m_WoundedAnimation;
	kFireTime.dwWoundedAniDuration	= rkAttribute.m_dwWoundedDuration;
	kFireTime.bLoopWoundedAni		= rkAttribute.m_bWoundedLoopAni;
	kFireTime.dwWeaponIdx			= pOwner->IncreaseWeaponIndexBase();
	kFireTime.eFireTimeType			= FTT_NORMAL_ATTACK;
	kFireTime.m_CreateBuffName		= GetName();
	kFireTime.eCollisionType		= ACT_NONE;

	pOwner->ExtendFireExplicit( kFireTime, vFirePos, vFireDir, "" );
}

void ioPassengerBuff::CheckAttackInput( ioBaseChar *pOwner )
{
	if( pOwner->IsAttackKey() && m_dwFireStartTime + m_dwFireGapTime < FRAMEGETTIME() )
	{
		D3DXVECTOR3 vDir = pOwner->GetTargetRot() * D3DXVECTOR3(0.0f, 0.0f, -FLOAT1);
		D3DXVec3Normalize( &vDir, &vDir );

		D3DXVECTOR3 vFireDir;
		D3DXVec3Cross( &vFireDir, &vDir, &ioMath::UNIT_Y );
		D3DXVec3Normalize( &vFireDir, &vFireDir );

		SetFire( pOwner, m_szLBipedName, m_LeftWeaponInfo,	vFireDir );
		SetFire( pOwner, m_szRBipedName, m_RightWeaponInfo,	-vFireDir );
		m_dwFireStartTime = FRAMEGETTIME();

		if ( pOwner->IsNeedSendNetwork() )
		{
			SP2Packet kPacket( CUPK_APPLY_EXTRA_BUFF_INFO );
			kPacket << pOwner->GetCharName();
			kPacket << GetName();
			kPacket << (byte)PSS_ATTACK;
			kPacket << vFireDir;
			P2PNetwork::SendToAllPlayingUser( kPacket );
		}
	}
}

void ioPassengerBuff::ApplyExtraBuffInfo( SP2Packet &rkPacket )
{
	ioBaseChar *pOwner = GetOwner();
	if( !pOwner )
		return;

	ioPlayStage *pStage = pOwner->GetCreator();
	if( !pStage )
		return;

	byte iState;
	rkPacket >> iState;

	switch( iState )
	{
	case PSS_GET_ON:
		{
			ChangeToGetOn( pOwner );
		}
		break;
	case PSS_ATTACK:
		{
			D3DXVECTOR3 vFireDir;
			rkPacket >> vFireDir;
			SetFire( pOwner, m_szLBipedName, m_LeftWeaponInfo,	vFireDir );
			SetFire( pOwner, m_szRBipedName, m_RightWeaponInfo,	-vFireDir );
		}
		break;
	}
}

void ioPassengerBuff::EndBuff()
{
	ioBuff::EndBuff();

	ioBaseChar* pOwner = GetOwner();
	if( pOwner )
	{
		if( pOwner->GetState() == CS_PASSENGER )
			pOwner->SetState( CS_DELAY );		

		if( m_PassengerState == PS_GET_ON )
			pOwner->SetJumpPower( m_fEndJumpPower );

		pOwner->EndEffect( m_WeaponEffectID );

		for( ioHashStringVec::iterator iter = m_GetOnAfterList.begin(); iter != m_GetOnAfterList.end(); ++iter )
		{
			const ioHashString& szBuffName = *iter;
			if( !szBuffName.IsEmpty() )
			{
				pOwner->SetReserveEndBuff( szBuffName );
			}
		}
	}
}

//-----------------------------------------------------------------------------------------------------------

ioPassengerState::ioPassengerState()
{
}

ioPassengerState::~ioPassengerState()
{
}

void ioPassengerState::Initialize( ioBaseChar* pOwner, CharState eCurrState )
{
	ioSpecialStateBase::Initialize( pOwner, eCurrState );
}

void ioPassengerState::Finalize( ioBaseChar* pOwner, CharState eCurrState, CharState eNewState )
{
	ioSpecialStateBase::Finalize( pOwner, eCurrState, eNewState );
}

void ioPassengerState::ProcessKeyInput( ioBaseChar* pOwner )
{
}

void ioPassengerState::CheckFallStateChange( ioBaseChar* pOwner, float fMinFallHeightRate, float fBottomHeight, bool bBottomMaxCheck, float fMaxBottomHeight )
{

}

bool ioPassengerState::IsCharCollisionSkipState( const ioBaseChar *const pOwner, DWORD dwStartTime, DWORD dwEndTime, DWORD dwSkipType, bool bTeamOnly, bool bDefense,TeamType eOwnerTeam, TeamType eOtherTeam ) const
{
	//상대편은 밀림과 충돌 처리
	if( eOwnerTeam != eOtherTeam )
		return false;

	return true;
}

bool ioPassengerState::IsEntityCollisionSkipState( const ioBaseChar *const pOwner, ioGameEntity* pColEntity ) const
{
	//자신이 탑승한 Entity에 대한 충돌 및 밀림 처리를 못하도록 한다.
	ioDummyChar* pDummy = ToDummyChar( pColEntity );
	if( pDummy && ToVehicleDummy( pDummy ) )
	{
		return true;
	}

	return false;
}