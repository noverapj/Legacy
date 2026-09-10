#include "StdAfx.h"
#include "ioCannonWeaponDummy.h"
#include "ioNpcChar.h"

ioCannonWeaponDummy::ioCannonWeaponDummy( ioEntityGroup *pGrp, ioPlayMode *pMode )
: ioDummyChar( pGrp, pMode )
{
	m_CannonDummyState = CDS_START;
	m_dwMotionEndTime = 0;
	m_bEnableTranslate = true;
}

ioCannonWeaponDummy::~ioCannonWeaponDummy(void)
{
}

void ioCannonWeaponDummy::LoadProperty( ioINILoader &rkLoader )
{
	char szBuf[MAX_PATH] = "";

	ioDummyChar::LoadProperty( rkLoader );

	m_fCheckColskipRange = rkLoader.LoadFloat_e( "check_col_skip_ragne", FLOAT500 );

	rkLoader.LoadString_e( "cannon_charge_ani", "", szBuf, MAX_PATH );
	m_ChargeAnimation = szBuf;
	m_fChargeAniRate = rkLoader.LoadFloat_e( "cannon_charge_ani_rate", FLOAT1 );

	rkLoader.LoadString_e( "cannon_attack_ani", "", szBuf, MAX_PATH );
	m_AttackAnimation = szBuf;
	m_fAttackAniRate = rkLoader.LoadFloat_e( "cannon_attack_ani_rate", FLOAT1 );

	rkLoader.LoadString_e( "cannon_full_attack_ani", "", szBuf, MAX_PATH );
	m_FullAttackAnimation = szBuf;
	m_fFullAttackAniRate = rkLoader.LoadFloat_e( "cannon_full_attack_ani_rate", FLOAT1 );

	rkLoader.LoadString_e( "cannon_deadline_ani", "", szBuf, MAX_PATH );
	m_DeadLIneAnimation = szBuf;
	m_fDeadLIneAniRate = rkLoader.LoadFloat_e( "cannon_deadline_ani_rate", FLOAT1 );

	rkLoader.LoadString_e( "attack_fail_ani", "", szBuf, MAX_PATH );
	m_AttackFailAni = szBuf;
	m_fAttackFailAniRate = rkLoader.LoadFloat_e( "attack_fail_ani_rate", FLOAT1 );

	rkLoader.LoadString_e( "check_dummy", "", szBuf, MAX_PATH );
	m_szCheckDummy = szBuf;

	rkLoader.LoadString_e( "cannon_full_charge_effect", "", szBuf, MAX_PATH );
	m_szCannonFullChargeEffect = szBuf;

	m_bCheckTeamAttack = rkLoader.LoadBool_e( "check_team_attack", false );
}

void ioCannonWeaponDummy::Translate( const D3DXVECTOR3 &vMove )
{
	if( !m_bEnableTranslate )
		return;

	if( m_bMountEnable )
	{
		UpdateMountedEntityList();
		UpdateMountedWeaponList();

		TranslateMountEntity( vMove );
	}

	ioPlayEntity::Translate( vMove );
}

bool ioCannonWeaponDummy::IsMountingAvailable() const
{
	return m_bMountEnable;
}

bool ioCannonWeaponDummy::IsMountAvailable() const
{
	return m_bMountEnable;
}

bool ioCannonWeaponDummy::IsNowMountEnable() const
{
	if( m_bCurMountEnable && m_bMountEnable )
		return true;

	return false;
}

int ioCannonWeaponDummy::DontMoveEntityLevel() const
{
	return 1;
}

void ioCannonWeaponDummy::NotifyDontMoveCollisionPushingSkip( ioGameEntity* pColEntity )
{
	ioNpcChar* pNpc = ToNpcChar( ToBaseChar( pColEntity ) );
	if( pNpc && COMPARE( pNpc->GetNpcType(), NT_BIGTOWER, END_FIXING_NPC ) )
	{
		m_CannonDummyState = CDS_END;
	}
}

bool ioCannonWeaponDummy::IsEntityCollisionSkipState( ioGameEntity* pColEntity ) const
{
	ioNpcChar* pNpc = ToNpcChar( ToBaseChar( pColEntity ) );
	if( pNpc && COMPARE( pNpc->GetNpcType(), NT_BIGTOWER, END_FIXING_NPC ) )
	{
		D3DXVECTOR3 vDiff = GetWorldPosition() - pNpc->GetWorldPosition();		
		if( D3DXVec3LengthSq( &vDiff ) < m_fCheckColskipRange * m_fCheckColskipRange && ioMath::TestIntersection( GetWorldAxisBox(), pNpc->GetWorldAxisBox() ) )
			return true;
	}

	return false;
}

bool ioCannonWeaponDummy::IsCollisionAvailableWeapon( ioWeapon *pWeapon )
{
	if( !pWeapon )
		return false;

	if( m_State == DCS_DIE )
		return false;

	if( !IsCollisionAvailableRange( pWeapon ) )
		return false;

	if( pWeapon->IsAlreadyCollisioned( this ) )
		return false;

	ioBaseChar *pOwner = GetOwner();
	if( !pOwner )
		return false;

	ioBaseChar *pWeaponOwner = pWeapon->GetOwner();
	if( !pWeaponOwner )
		return false;

	if( pOwner->GetTeam() == pWeaponOwner->GetTeam() )
	{
		if ( m_bCheckTeamAttack )
		{
			if ( !pWeapon->IsTeamAttack() && !pOwner->IsEnableTeamAttack() )
				return pWeapon->HasFeature( WF_TEAM_ATTACK_NO_DAMAGE );
		}
		else
			return false;
	}
	return true;
}

void ioCannonWeaponDummy::ProcessState( float fTimerPerSec, DWORD dwPreTime )
{
	if( !CheckLive() )
	{
		SetDieState( true, true );
		return;
	}

	switch( m_CannonDummyState )
	{
	case CDS_START:
		ProcessCannonStartState();
		break;
	case CDS_ATTACK:
		ProcessCannonAttackState();
		break;
	case CDS_DEADLINE:
		ProcessCannonDeadLineState();
		break;
	}
}

void ioCannonWeaponDummy::DropZoneCheck()
{
	if( m_dwDropZoneLiveTime > 0 )
	{
		bool bCheckDropZone = false;
		if( m_dwDropZoneCheckStartTime <= 0 )
			m_dwDropZoneCheckStartTime = FRAMEGETTIME();

		D3DXVECTOR3 vCurPos = GetWorldPosition();

		if( GetOwner() &&
			GetOwner()->GetCreator() &&
			GetOwner()->GetCreator()->GetMapHeightNoneEntity( vCurPos.x, vCurPos.z, vCurPos.y+3.0f ) <= 0.0f )
			bCheckDropZone = true;

		if( !bCheckDropZone )
			m_dwDropZoneCheckStartTime = 0;

		if( bCheckDropZone && m_dwDropZoneCheckStartTime > 0 && m_dwDropZoneCheckStartTime + m_dwDropZoneLiveTime <= FRAMEGETTIME() )
		{
			m_HP.m_fCurValue = 0.0f;
			m_LastAttacker = m_OwnerName;

			SetDieState( true, true );
		}
	}

	switch( m_State )
	{
	case DCS_START:
	case DCS_DIE:
		return;
	}

	D3DXVECTOR3 vPos = GetWorldPosition();

	if( m_HP.m_fCurValue > 0.0f && vPos.y <= 0.0f )
	{
		m_HP.m_fCurValue = 0.0f;
		m_LastAttacker = m_OwnerName;

		SetDieState( true, true );
	}
}

void ioCannonWeaponDummy::SetDieState( bool bEffect, bool bWoundedDie /*= false */ )
{
	ioDummyChar::SetDieState( bEffect, bWoundedDie );
	DeleteFullChargeEffect();
}

bool ioCannonWeaponDummy::CheckLive()
{
	if( !GetOwner() || ( GetOwner()->GetState() != CS_CANNON_SPECIAL && m_CannonDummyState != CDS_DEADLINE ) )
		return false;
	
	if ( !m_szCheckDummy.IsEmpty() )
	{
		ioDummyChar *pCheckDummy = g_DummyCharMgr.FindDummyCharToName( GetOwner(), m_szCheckDummy );
		if ( !pCheckDummy )
			return false;
	}

	if( m_dwLifeTime && (DWORD)(m_dwStartTime + m_dwLifeTime) < FRAMEGETTIME() )
	{
		switch( m_CannonDummyState )
		{
		case CDS_DELAY:
		case CDS_CHARGE:
			return false;
		}
	}

	return true;
}


void ioCannonWeaponDummy::ProcessCannonStartState()
{
	if( m_dwStartAniEndTime == 0 || m_dwStartAniEndTime < FRAMEGETTIME() )
	{
		SetCannonDelayState();
		return;
	}
}

void ioCannonWeaponDummy::SetCannonDelayState()
{
	if( m_DelayAnimation.IsEmpty() )
		return;

	int iAniID = m_pGroup->GetAnimationIdx( m_DelayAnimation );
	if( iAniID == -1 )
		return;
	float fAniRate = FLOAT1;
	if( m_fDelayAniRate > 0.0f )
		fAniRate = m_fDelayAniRate;

	m_CannonDummyState = CDS_DELAY;
	m_pGroup->ClearAllActionAni( FLOAT10, true );
	m_pGroup->ClearAllLoopAni( FLOAT10, true );

	m_pGroup->SetLoopAni( iAniID, FLOAT10, FLOAT1, FLOAT1/fAniRate );
	DeleteFullChargeEffect();
}

void ioCannonWeaponDummy::SetCannonChargeState()
{
	if( m_ChargeAnimation.IsEmpty() )
		return;

	int iAniID = m_pGroup->GetAnimationIdx( m_ChargeAnimation );
	if( iAniID == -1 )
		return;

	float fAniRate = FLOAT1;
	if( m_fChargeAniRate > 0.0f )
		fAniRate = m_fChargeAniRate;

	m_CannonDummyState = CDS_CHARGE;
	m_pGroup->ClearAllActionAni( FLOAT10, true );
	m_pGroup->ClearAllLoopAni( FLOAT10, true );

	m_pGroup->SetLoopAni( iAniID, FLOAT10, FLOAT1, FLOAT1/fAniRate );
	DeleteFullChargeEffect();
}

void ioCannonWeaponDummy::SetCannonAttackState( bool bCharge, DWORD& dwEndTime, DWORD& dwFireStartTime )
{
	ioHashString szCurAni;
	float fAniRate = FLOAT1;
	if ( bCharge )
	{
		szCurAni = m_FullAttackAnimation;
		fAniRate = m_fFullAttackAniRate;
		if ( fAniRate <= 0.0f )
			fAniRate = FLOAT1;
	}
	else
	{
		szCurAni = m_AttackAnimation;
		fAniRate = m_fAttackAniRate;
		if ( fAniRate <= 0.0f )
			fAniRate = FLOAT1;
	}

	int iAniID = m_pGroup->GetAnimationIdx( szCurAni );
	if( iAniID == -1 )
		return;

	m_CannonDummyState = CDS_ATTACK;
	m_pGroup->ClearAllActionAni( FLOAT10, true );
	m_pGroup->ClearAllLoopAni( FLOAT10, true );

	DWORD dwCurTime = FRAMEGETTIME();

	dwFireStartTime = (DWORD)m_pGroup->GetAnimationEventTime_e( iAniID, "fire_s" ) * fAniRate;
	m_dwMotionEndTime = dwEndTime = m_pGroup->GetAnimationFullTime(iAniID) * fAniRate;
	m_dwMotionEndTime+= dwCurTime; 
	m_pGroup->SetActionAni( iAniID, FLOAT10, FLOAT1, FLOAT1/fAniRate );

	DeleteFullChargeEffect();
}

void ioCannonWeaponDummy::ProcessCannonAttackState()
{
	if ( m_dwMotionEndTime < FRAMEGETTIME() )
	{
		SetCannonDelayState();
		return;
	}
}

DWORD ioCannonWeaponDummy::SetCannonDeadLineState( DWORD dwMotionEndTime )
{
	m_CannonDummyState = CDS_DEADLINE;
	m_dwMotionEndTime = dwMotionEndTime;

	if( m_DeadLIneAnimation.IsEmpty() )
		return m_dwMotionEndTime;

	int iAniID = m_pGroup->GetAnimationIdx( m_DeadLIneAnimation );
	if( iAniID == -1 )
		return m_dwMotionEndTime;

	float fAniRate = FLOAT1;
	if( m_fDeadLIneAniRate > 0.0f )
		fAniRate = m_fDeadLIneAniRate;

	m_pGroup->ClearAllActionAni( FLOAT10, true );
	m_pGroup->ClearAllLoopAni( FLOAT10, true );

	m_dwMotionEndTime = FRAMEGETTIME() +  m_pGroup->GetAnimationFullTime(iAniID) * fAniRate;
	m_pGroup->SetActionAni( iAniID, FLOAT10, FLOAT1, FLOAT1/fAniRate );
	DeleteFullChargeEffect();

	return m_dwMotionEndTime;
}

void ioCannonWeaponDummy::SetCannonAttackFailState( DWORD& dwEndTime )
{
	if( m_AttackFailAni.IsEmpty() )
		return;

	int iAniID = m_pGroup->GetAnimationIdx( m_AttackFailAni );
	if( iAniID == -1 )
		return;

	float fAniRate = FLOAT1;
	if( m_fAttackFailAniRate > 0.0f )
		fAniRate = m_fAttackFailAniRate;

	m_CannonDummyState = CDS_ATTACK_FAIL;

	m_pGroup->ClearAllActionAni( FLOAT10, true );
	m_pGroup->ClearAllLoopAni( FLOAT10, true );

	dwEndTime = FRAMEGETTIME() +  m_pGroup->GetAnimationFullTime(iAniID) * fAniRate;
	m_pGroup->SetActionAni( iAniID, FLOAT10, FLOAT1, FLOAT1/fAniRate );
	DeleteFullChargeEffect();
}

void ioCannonWeaponDummy::ProcessCannonDeadLineState()
{
	if ( m_dwMotionEndTime < FRAMEGETTIME() )
	{
		SetDieState( true );
		return;
	}
}

ApplyWeaponExp ioCannonWeaponDummy::ApplyWeapon( ioWeapon *pWeapon )
{
	ioBaseChar *pAttacker = pWeapon->GetOwner();
	if( !pAttacker )
	{
		LOG.PrintTimeAndLog( 0, "ioDummyChar::ApplyWeapon - %s Not Exist", pWeapon->GetOwnerName().c_str() );
		return AWE_EXPLOSION;
	}

	if( pWeapon->IsNoWoundedWeapon() )
		return AWE_NO_EXPLOSION;

	if( P2PNetwork::IsNetworkPlaying() &&
		m_pModeParent->IsNetworkMode() &&
		!pAttacker->IsNeedProcess() )
		return AWE_EXPLOSION;

	if ( m_CannonDummyState == CDS_DEADLINE )
		return AWE_NO_EXPLOSION;

	m_LastAttacker = pWeapon->GetOwnerName();

	float fPreCurHP = m_HP.m_fCurValue;
	float fDamage = 0.0f;
	if( 0.0f < m_fAbsoluteDamage )
	{
		fDamage = m_fAbsoluteDamage;
	}
	else
	{
		fDamage = pWeapon->GetDamage( this ) * m_fDamageRate;
	}

	float fPushPower = pWeapon->GetDefensePushPower( fDamage );

	if( !m_bNoDamage && !m_bPiercingProtected )
		m_HP.m_fCurValue -= fDamage;
	AddHeadDamageInfo( (int)fDamage );

	bool bContactType = false;
	if( pWeapon->GetType() == ioWeapon::WT_CONTACT ||
		pWeapon->GetType() == ioWeapon::WT_RUN_CONTACT ||
		pWeapon->GetType() == ioWeapon::WT_CONTACT2 )
		bContactType = true;

	bool bStructWeapon = false;
	if( pWeapon->CheckWeaponByStruct() || pWeapon->CheckWeaponByDummyChar() )
		bStructWeapon = true;

	bool bOnlyEmoticon = true;
	D3DXVECTOR3 vAttackDir = pWeapon->GetAttackDir(this);
	if( bContactType && !bStructWeapon && !pWeapon->CheckWeaponBySkill() && !m_BlockBuff.IsEmpty() )
	{
		if( pAttacker->IsEnableAttackCancelState() && !pWeapon->IsChangeJumpping( false, false )
			&& !pAttacker->IgnoreBlock() )
		{
			pAttacker->AddNewBuff( m_BlockBuff, "", "", NULL );
			pAttacker->ReverseReflectPush( vAttackDir, fPushPower * FLOAT05 );

			bOnlyEmoticon = false;
		}
	}

	if( !m_BlockBuff.IsEmpty() )
		pAttacker->SetDisableAttackEmoticon();

	int iShakeCamera = pWeapon->GetShakeCamera();
	ActionStopType eActionStop = pWeapon->GetActionStopType();

	if( eActionStop == AST_BOTH || eActionStop == AST_ATTACKER )
	{
		pAttacker->SetActionStopDelay( fDamage, false, iShakeCamera );
		pWeapon->SetActionStopDuration( ioBaseChar::CalcActionStopDelay( fDamage, false ),
			ioBaseChar::GetActionStopLooseRate( false ) );
	}

	if( P2PNetwork::IsNetworkPlaying() && m_pModeParent->IsNetworkMode() )
	{
		SP2Packet kPacket( CUPK_DUMMY_CHAR_SYNC );
		kPacket << m_OwnerName;
		kPacket << m_iCreateIndex;
		kPacket << DUMMY_CHAR_DAMAGE;
		kPacket << pWeapon->GetOwnerName();
		kPacket << m_HP.m_fCurValue;
		kPacket << m_HP.m_fMaxValue;
		kPacket << bOnlyEmoticon;
		kPacket << vAttackDir;
		kPacket << fDamage;
		kPacket << fPushPower;
		kPacket << (int)eActionStop;
		kPacket << iShakeCamera;
		kPacket << pWeapon->GetWeaponIndex();
		P2PNetwork::SendToAllPlayingUser( kPacket );
	}

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

void ioCannonWeaponDummy::OnDamaged( SP2Packet &rkPacket, ioBaseChar *pAttacker )
{
	rkPacket >> m_HP.m_fCurValue >> m_HP.m_fMaxValue;

	bool bOnlyEmoticon;
	D3DXVECTOR3 vAttackDir;
	float fDamage, fPushPower;

	rkPacket >> bOnlyEmoticon;
	rkPacket >> vAttackDir;
	rkPacket >> fDamage;
	rkPacket >> fPushPower;

	if( fDamage > 0.0f )
	{
		AddHeadDamageInfo( fDamage );
	}

	if( !bOnlyEmoticon && !m_BlockBuff.IsEmpty() )
	{
		if( pAttacker->IsEnableAttackCancelState() && !pAttacker->IgnoreBlock() )
		{
			pAttacker->AddNewBuff( m_BlockBuff, "", "", NULL );
			pAttacker->ReverseReflectPush( vAttackDir, fPushPower * FLOAT05 );
		}
	}

	if( !m_BlockBuff.IsEmpty() )
		pAttacker->SetDisableAttackEmoticon();

	int iActionStop = 0;
	rkPacket >> iActionStop;

	int iShakeCamera = 0;
	rkPacket >> iShakeCamera;

	if( iActionStop == AST_BOTH || iActionStop == AST_ATTACKER )
	{
		pAttacker->SetActionStopDelay( fDamage, false, iShakeCamera );
	}

	if( m_HP.m_fCurValue <= 0.0f )
	{
		m_HP.m_fCurValue = 0.0f;
		SetDieState( true, true );
	}

	int iWeaponIndex = 0;
	rkPacket >> iWeaponIndex;

	ioWeapon *pWeapon = g_WeaponMgr.FindWeapon( pAttacker, iWeaponIndex );
	if( pWeapon )
	{
		int iDummyIndex = pWeapon->GetWeaponByDummyChar();
		g_DummyCharMgr.CheckEntityCollision( pAttacker, this, iDummyIndex, false );
	}
}

void ioCannonWeaponDummy::AddFullChargeEffect()
{
	if( !m_szCannonFullChargeEffect.IsEmpty() )
		m_pGroup->AttachEffect( m_szCannonFullChargeEffect, NULL );
}

void ioCannonWeaponDummy::DeleteFullChargeEffect()
{
	m_pGroup->EndEffect( m_DummyEffect, true );
	m_pGroup->EndEffect( m_szCannonFullChargeEffect, true );
}