
#include "stdafx.h"

#include "ioBunkerDummy.h"

ioBunkerDummy::ioBunkerDummy( ioEntityGroup *pGrp, ioPlayMode *pMode )
	: ioDummyChar( pGrp, pMode )
{
	m_DummyBunkerState = DBS_DELAY;
	m_dwAttackTime = 0;
	m_bHasOwner = false;
}

ioBunkerDummy::~ioBunkerDummy()
{
}

void ioBunkerDummy::LoadProperty( ioINILoader &rkLoader )
{
	ioDummyChar::LoadProperty( rkLoader );

	char szBuf[MAX_PATH];
	char szKey[MAX_PATH] = "";

	m_nWeaponType = rkLoader.LoadInt_e( "weapon_type", NO_WEAPON_ATTRIBUTE );

	rkLoader.LoadString_e( "delay_ani_name", "", szBuf, MAX_PATH );
	m_szDelayAniName = szBuf;
	rkLoader.LoadString_e( "fire_ani_name", "", szBuf, MAX_PATH );
	m_szFireAniName = szBuf;

	rkLoader.LoadString( "fire_sound", "", szBuf, MAX_PATH );
	m_stAttackSound = szBuf;
	
	m_dwCheckAttackTime = rkLoader.LoadInt_e( "check_attack_time", 0 );

	m_bCreatorBunker = rkLoader.LoadBool( "only_creator_bunker", false );

	m_fOwnerFindRange = rkLoader.LoadFloat( "owner_find_range", 0.f );
	m_fEndJumpPower = rkLoader.LoadFloat( "end_jump_power", 0.f );

	m_fAttackBulletHeightOffset = rkLoader.LoadFloat( "attack_fire_height_offset", 0.f );

	m_fAttackFindRange = rkLoader.LoadFloat( "attack_find_range", 0.f );
	m_fAttackFindAngle = rkLoader.LoadFloat( "attack_find_angle", 0.f );


	int nBuffCnt = rkLoader.LoadInt( "max_owner_buff_cnt", 0);
	for( int i=0; i < nBuffCnt; ++i )
	{
		wsprintf_e( szKey, "start_add_buff_name%d", i+1 );
		rkLoader.LoadString( szKey, "", szBuf, MAX_PATH );

		ioHashString szBuffName = szBuf;
		m_stOwnerAddBuffs.push_back( szBuffName );
	}
}

void ioBunkerDummy::ProcessState( float fTimePerSec, DWORD dwPreTime )
{
	DWORD dwCurTime = FRAMEGETTIME();

	ioBaseChar *pOwner = GetOwner();
	if ( !pOwner )
	{
		SetDieState( false );
		return;
	}

	if( m_State == DCS_DIE )
		return;

	switch( m_DummyBunkerState )
	{
	case DBS_DELAY:
		if( pOwner && pOwner->IsNeedProcess() )
			FindBunkerOwner();
		break;
	case DBS_START:
		{
			ioBaseChar* pAttachOwner = pOwner->GetBaseChar( m_stAttachOwnerName );
			if ( !CheckOwnerState(pAttachOwner) )
			{
				SetDieState( true );
				return;
			}

			if( pAttachOwner->IsNeedProcess() )
			{
				if( pAttachOwner->IsAttackKey() && dwCurTime > m_dwNextAttackTime )
				{
					CreateWeapon( pAttachOwner );
					m_dwNextAttackTime = dwCurTime + m_dwCheckAttackTime;
				}
			}
		}
		break;
	}
}

void ioBunkerDummy::FindBunkerOwner()
{
	ioBaseChar* pOwner = GetOwner();
	if( !pOwner )
		return;

	ioPlayStage *pStage = pOwner->GetCreator();
	if( !pStage )
		return;
	
	if( m_bCreatorBunker )
	{
		SetOwner( pOwner );
		return;
	}

	bool bFind = false;

	D3DXVECTOR3 vCurPos = GetMidPositionByRate();
	D3DXVECTOR3 vDiff= ioMath::VEC3_ZERO;
	float fTargetRangeSq = m_fOwnerFindRange * m_fOwnerFindRange;
	
	const BaseCharList &rkCharList = pStage->GetBaseCharList();
	BaseCharList::const_iterator iter;

	for( iter=rkCharList.begin() ; iter!=rkCharList.end() ; ++iter )
	{
		ioBaseChar *pTarget = *iter;
		if( !pTarget ) 
			continue;

		if( pTarget->GetTeam() != pOwner->GetTeam() ) 
			continue;

		if( pTarget->GetCharName() == GetOwnerName() ) 
			continue;

		if( pTarget->GetState() != CS_JUMP )
			continue;

		D3DXVECTOR3 vTargetPos = pTarget->GetMidPositionByRate();
		vDiff = vTargetPos - vCurPos;
		float fDiffRangeSq = D3DXVec3LengthSq( &vDiff );
		if( fDiffRangeSq < fTargetRangeSq )
		{
			SetOwner( pTarget );
			break;
		}
	}
}

void ioBunkerDummy::SetOwner( ioBaseChar* pOwner )
{
	m_bHasOwner = true;
	m_stAttachOwnerName = pOwner->GetCharName();
	pOwner->SetWorldPosition( GetWorldPosition() );
	m_DummyBunkerState = DBS_START;
	pOwner->SetState( CS_BUNKER_STATE );

	for( int i = 0; i < (int)m_stOwnerAddBuffs.size(); i++ )
	{
		pOwner->AddNewBuff( m_stOwnerAddBuffs[i], pOwner->GetCharName(), "", NULL );
	}	

	m_dwNextAttackTime = 0;

	//SendPacket
	if( GetOwner() && GetOwner()->IsNeedSendNetwork() )
	{
		SP2Packet kPacket( CUPK_DUMMY_CHAR_SYNC );
		kPacket << m_OwnerName;
		kPacket << m_iCreateIndex;
		kPacket << DBS_START;
		kPacket << pOwner->GetCharName();
		P2PNetwork::SendToAllPlayingUser( kPacket );
	}
}

void ioBunkerDummy::ApplyDummyCharSync( ioPlayStage *pStage, SP2Packet &rkPacket )
{
	ioBaseChar* pOwner = GetOwner();
	if( !pOwner )
		return;

	if( !pStage ) 
		return;	

	int iType;
	rkPacket >> iType;	

	switch ( iType )
	{
	case DBS_START:
		{
			ioHashString stName;
			rkPacket >> stName;
			ioBaseChar* pAttachOwner = pOwner->GetBaseChar( stName );
			if( pAttachOwner )
				SetOwner( pAttachOwner );
		}
		break;
	case DBS_CREATE_WEAPON:
		{
			DWORD dwBaseWeaponIndex;
			rkPacket >> dwBaseWeaponIndex;

			D3DXVECTOR3 vPos;
			rkPacket >> vPos;

			D3DXVECTOR3 vDir;
			rkPacket >> vDir;

			ioBaseChar *pOwner = GetOwner();
			if ( !pOwner )
				return;

			FireTime kFireTime;
			kFireTime.dwStart = FRAMEGETTIME();

			kFireTime.iAttributeIdx = m_nWeaponType;
			kFireTime.iResistanceIdx = 0;
			kFireTime.szWoundedAni  = "";
			kFireTime.dwWoundedAniDuration = 0;
			kFireTime.bLoopWoundedAni = false;
			kFireTime.dwWeaponIdx = dwBaseWeaponIndex;
			kFireTime.eFireTimeType = FTT_DUMMY_CHAR_ATTACK;

			ioWeapon *pWeapon = pOwner->ExtendFireExplicit( kFireTime, vPos, vDir, "" );
			if ( pWeapon )
				pWeapon->SetPosition( vPos );
		}
		break;
	}
}

bool ioBunkerDummy::IsActivateWeapon()
{
	return true;
}

ApplyWeaponExp ioBunkerDummy::ApplyWeapon( ioWeapon *pWeapon )
{
	ioBaseChar *pAttacker = pWeapon->GetOwner();
	if( !pAttacker )
	{
		LOG.PrintTimeAndLog( 0, "ioDummyChar::ApplyWeapon - %s Not Exist", pWeapon->GetOwnerName().c_str() );
		return AWE_EXPLOSION;
	}

	if( pWeapon->IsNoWoundedWeapon() )
		return AWE_NO_EXPLOSION;

	m_LastAttacker = pWeapon->GetOwnerName();

	float fDamage = pWeapon->GetDamage( this ) * m_fDamageRate;
	float fPushPower = pWeapon->GetDefensePushPower( fDamage );

	if( !m_bNoDamage && !m_bPiercingProtected )
		m_HP.m_fCurValue -= fDamage;
	AddHeadDamageInfo( (int)fDamage );
	
	if( m_HP.m_fCurValue <= 0.0f )
	{
		m_HP.m_fCurValue = 0.0f;
		SetDieState( true, true );
	}

	if( pWeapon )
	{
		int iDummyIndex = pWeapon->GetWeaponByDummyChar();
		g_DummyCharMgr.CheckEntityCollision( pAttacker, this, iDummyIndex, false );
	}

	return AWE_EXPLOSION;
}

void ioBunkerDummy::SetDieState( bool bEffect, bool bWoundedDie /*= false */ )
{
	ProcessEnd();

	ioDummyChar::SetDieState( bEffect, bWoundedDie );
}

bool ioBunkerDummy::IsNeedCheckMapCollision() const
{
	return !m_bDisableMapCollision;
}

int ioBunkerDummy::DontMoveEntityLevel() const
{
	return 1;
}

void ioBunkerDummy::SetState( ioHashString szAniName, DummyBunkerState eState )
{
	if ( !m_pGroup || szAniName.IsEmpty() )
		return;

	int iAniID = m_pGroup->GetAnimationIdx( szAniName );
	if( iAniID == -1 )
		return;

	m_pGroup->ClearAllActionAni( FLOAT100, true );
	m_pGroup->ClearAllLoopAni( FLOAT100, true );

	m_pGroup->SetLoopAni( iAniID, FLOAT100 );

	m_DummyBunkerState = eState;
}

void ioBunkerDummy::ProcessDelayState()
{
}

void ioBunkerDummy::ProcessFireState()
{
}

void ioBunkerDummy::ProcessEnd()
{
	if( !m_bHasOwner || m_stAttachOwnerName.IsEmpty() )
		return;

	ioBaseChar *pOwner = GetOwner();
	if ( !pOwner )
		return;

	ioBaseChar* pAttachOwner = pOwner->GetBaseChar( m_stAttachOwnerName );
	if( !pAttachOwner )
		return;

	ioPlayStage *pStage = pOwner->GetCreator();
	if ( !pStage )
		return;

	for( int i = 0; i < (int)m_stOwnerAddBuffs.size(); i++ )
	{
		pAttachOwner->RemoveBuff( m_stOwnerAddBuffs[i] );
	}

	pAttachOwner->SetCurMoveSpeed( 0.0f );
	pAttachOwner->SetCurJumpPowerAmt( 0.0f );
	pAttachOwner->SetGravityAmt( 0.0f );
	pAttachOwner->SetForcePowerAmt( 0.0f );

	pAttachOwner->SetSKillEndJumpState( m_fEndJumpPower, false, false, false, true );
	pAttachOwner->ClearDirDoubleClick();
}

bool ioBunkerDummy::CheckOwnerState( ioBaseChar* pAttachOwner )
{
	if( !pAttachOwner )
		return false;

	if ( pAttachOwner->GetState() != CS_BUNKER_STATE )
		return false;

	return true;
}

void ioBunkerDummy::CreateWeapon( ioBaseChar* pAttachOwner )
{
	if ( !pAttachOwner || !pAttachOwner->IsNeedProcess() )
		return;

	ioUserKeyInput::DirKeyInput eNewDirKey = ioUserKeyInput::DKI_NONE;
	if ( pAttachOwner->IsSettedDirection() )
	{
		eNewDirKey = pAttachOwner->CheckCurDirKey();
		pAttachOwner->SetTargetRotToDirKey( eNewDirKey );
		pAttachOwner->SetWorldOrientation( pAttachOwner->GetTargetRot() );
	}

	FindAttackSearchTarget( pAttachOwner );

	DWORD dwBaseWeaponIndex = pAttachOwner->GetWeaponIndexBase();

	FireTime kFireTime;
	kFireTime.dwStart = FRAMEGETTIME();

	kFireTime.iAttributeIdx = m_nWeaponType;
	kFireTime.iResistanceIdx = 0;
	kFireTime.szWoundedAni  = "";
	kFireTime.dwWoundedAniDuration = 0;
	kFireTime.bLoopWoundedAni = false;
	kFireTime.dwWeaponIdx = dwBaseWeaponIndex;
	kFireTime.eFireTimeType = FTT_DUMMY_CHAR_ATTACK;

	D3DXQUATERNION qtRot = pAttachOwner->GetTargetRot();
	D3DXVECTOR3 vPos = pAttachOwner->GetWorldPosition();
	vPos.y = vPos.y + m_fAttackBulletHeightOffset;

	D3DXVECTOR3 vDir = qtRot * D3DXVECTOR3( 0.0f, 0.0f, -FLOAT1 );
	ioWeapon *pWeapon = NULL;

	if( m_AimedTarget.IsEmpty() )
	{
		pWeapon = pAttachOwner->ExtendFireExplicit( kFireTime, vPos, vDir, "" );
	}
	else
	{
		ioBaseChar* pTargetChar = pAttachOwner->GetBaseChar( m_AimedTarget );
		if( !pTargetChar )
			return;

		vDir = pTargetChar->GetWorldPosition() - pAttachOwner->GetWorldPosition();
		D3DXVec3Normalize( &vDir, &vDir );
		pWeapon = pAttachOwner->ExtendFireExplicit( kFireTime, vPos, vDir, "" );
	}

	if ( pWeapon )
		pWeapon->SetPosition( vPos );

	PlaySound( pAttachOwner, m_stAttackSound, false );

	if ( pAttachOwner->IsNeedSendNetwork() )
	{
		SP2Packet kPacket( CUPK_DUMMY_CHAR_SYNC );
		kPacket << m_OwnerName;
		kPacket << m_iCreateIndex;
		kPacket << DBS_CREATE_WEAPON;
		kPacket << dwBaseWeaponIndex;
		kPacket << vPos;
		kPacket << vDir;
		P2PNetwork::SendToAllPlayingUser( kPacket );
	}
}

void ioBunkerDummy::FindAttackSearchTarget( ioBaseChar *pAttachOwner )
{
	m_AimedTarget.Clear();

	if( !pAttachOwner )
		return;

	ioPlayStage *pStage = pAttachOwner->GetCreator();
	if( !pStage )
		return;

	D3DXVECTOR3 vCurPos = pAttachOwner->GetMidPositionByRate();
	D3DXVECTOR3 vDiff= ioMath::VEC3_ZERO;

	float fCurAngle = ioMath::ArrangeHead( m_fAttackFindAngle );
	float fHalfValue = cosf( DEGtoRAD(fCurAngle*0.5f) );
	float fTargetRangeSq = m_fAttackFindRange * m_fAttackFindRange;

	const BaseCharList &rkCharList = pStage->GetBaseCharList();
	BaseCharList::const_iterator iter;

	ioHashString szTarget;
	float fCurGap = -FLOAT1;
	for( iter=rkCharList.begin() ; iter!=rkCharList.end() ; ++iter )
	{
		ioBaseChar *pTarget = *iter;
		if( !pTarget ) continue;

		if( pTarget->GetTeam() == pAttachOwner->GetTeam() ) continue;
		if( pTarget->GetCharName() == pAttachOwner->GetCharName() ) continue;
		if( pTarget->GetExperienceMode() != EMS_NONE ) continue;

		if( !IsEnableTargetState( pTarget ) )
			continue;

		D3DXVECTOR3 vTargetPos = pTarget->GetMidPositionByRate();
		vDiff = vTargetPos - vCurPos;
		float fDiffRangeSq = D3DXVec3LengthSq( &vDiff );

		// 각도 체크
		if( fCurAngle != 0.0f )
		{
			D3DXQUATERNION qtRot = pAttachOwner->GetTargetRot();
			D3DXVECTOR3 vDir = qtRot * D3DXVECTOR3( 0.0f, 0.0f, -FLOAT1 );
			D3DXVec3Normalize( &vDir, &vDir );
			D3DXVECTOR3 vDiff2 = vDiff;
			vDiff2.y = 0.0f;
			D3DXVec3Normalize( &vDiff2, &vDiff2 );

			float fCurValue = D3DXVec3Dot( &vDir, &vDiff2 );
			if( fCurValue < fHalfValue )
				continue;
		}

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
			m_AimedTarget = szTarget;
		}
	}
}
//////////////////////////////////////////////////////////////////////////
ioBunkerAttachSpecialState::ioBunkerAttachSpecialState()
{
}

ioBunkerAttachSpecialState::~ioBunkerAttachSpecialState()
{
}

void ioBunkerAttachSpecialState::Initialize( ioBaseChar* pOwner, CharState eCurrState )
{
	ioSpecialStateBase::Initialize( pOwner, eCurrState );
}

void ioBunkerAttachSpecialState::Finalize( ioBaseChar* pOwner, CharState eCurrState, CharState eNewState )
{
	ioSpecialStateBase::Finalize( pOwner, eCurrState, eNewState );
}

void ioBunkerAttachSpecialState::CheckSpecialState( ioBaseChar* pOwner )
{	
}

void ioBunkerAttachSpecialState::CheckFallStateChange( ioBaseChar* pOwner, float fMinFallHeightRate, float fBottomHeight, bool bBottomMaxCheck, float fMaxBottomHeight )
{
	return;
}

bool ioBunkerAttachSpecialState::ProcessMove( ioBaseChar* pOwner, float fTimePerSec, DWORD dwPreTime, DWORD dwCurTime )
{
	return true;
}