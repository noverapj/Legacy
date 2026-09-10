
#include "stdafx.h"

#include "ioDummySubCrane.h"
#include "WeaponDefine.h"
#include "ioDummyCrane.h"

ioDummySubCrane::ioDummySubCrane( ioEntityGroup *pGrp, ioPlayMode *pMode )
	: ioDummyChar( pGrp, pMode )
{
	m_DummySubCraneState = DSAS_Start;
	m_dwAttachWeaponIdx = 0;
	m_dwFireTime = 0;
}

ioDummySubCrane::~ioDummySubCrane()
{
}

void ioDummySubCrane::LoadProperty( ioINILoader &rkLoader )
{
	ioDummyChar::LoadProperty( rkLoader );

	char szBuf[MAX_PATH];
	
	rkLoader.LoadString_e( "delay_ani_name", "", szBuf, MAX_PATH );
	m_szDelayAniName = szBuf;
	
	rkLoader.LoadString_e( "crane_name", "", szBuf, MAX_PATH );
	m_szFindCraneDummy = szBuf;

	LoadAttackAttribute_e( "attack_info", m_AttackInfo, rkLoader );
	LoadAttackAttribute_e( "grap_info", m_GrapInfo, rkLoader );

	m_vDelaySubCraneOffset.x = rkLoader.LoadFloat_e( "delay_offset_x", 0.0f );
	m_vDelaySubCraneOffset.y = rkLoader.LoadFloat_e( "delay_offset_y", 0.0f );
	m_vDelaySubCraneOffset.z = rkLoader.LoadFloat_e( "delay_offset_z", 0.0f );

	m_fUpSpeed = rkLoader.LoadFloat_e( "up_speed", 0.0f );
	m_fDownSpeed = rkLoader.LoadFloat_e( "down_speed", 0.0f );

	m_fMaxDownHeight = rkLoader.LoadFloat_e( "max_down_height", 0.0f );
}

void ioDummySubCrane::ProcessState( float fTimePerSec, DWORD dwPreTime )
{
	ioBaseChar *pOwner = GetOwner();
	if ( !pOwner )
	{
		SetDieState( true );
		return;
	}

	switch( m_DummySubCraneState )
	{
	case DSAS_Start:
		{
			if( !m_DummyEffect.IsEmpty() )
			{
				m_pGroup->AttachEffect( m_DummyEffect, NULL );
				m_bMoveEffectCreated = true;
			}

			SetDelayState( pOwner );
		}
		break;
	case DSAS_Delay:
		ProcessDelayState( pOwner );
		break;
	case DSAS_Attack:
		ProcessAttackState( pOwner, fTimePerSec );
		break;
	case DSAS_Grap:
		ProcessGrapState( pOwner, fTimePerSec );
		break;
	}
}

void ioDummySubCrane::ApplyDummyCharSync( ioPlayStage *pStage, SP2Packet &rkPacket )
{
	ioBaseChar *pOwner = GetOwner();
	if ( !pOwner )
		return;

	if( !pStage ) 
		return;	
	
	int iType;
	rkPacket >> iType;	

	switch ( iType )
	{
	case DSAS_Delay:
		SetDelayState( pOwner );
		break;
	}
}

bool ioDummySubCrane::IsActivateWeapon()
{
	return true;
}

ApplyWeaponExp ioDummySubCrane::ApplyWeapon( ioWeapon *pWeapon )
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

	bool bContactType = false;
	if( pWeapon->GetType() == ioWeapon::WT_CONTACT ||
		pWeapon->GetType() == ioWeapon::WT_RUN_CONTACT ||
		pWeapon->GetType() == ioWeapon::WT_CONTACT2 )
		bContactType = true;

	bool bStructWeapon = false;
	if( pWeapon->CheckWeaponByStruct() || pWeapon->CheckWeaponByDummyChar() )
		bStructWeapon = true;

	D3DXVECTOR3 vAttackDir = pWeapon->GetAttackDir(this);

	if( bContactType && !bStructWeapon && !pWeapon->CheckWeaponBySkill() && !m_BlockBuff.IsEmpty() )
	{
		if( pAttacker->IsEnableAttackCancelState() && !pWeapon->IsChangeJumpping( false, false )
			&& !pAttacker->IgnoreBlock() )
		{
			pAttacker->AddNewBuff( m_BlockBuff, "", "", NULL );
			pAttacker->ReverseReflectPush( vAttackDir, fPushPower * FLOAT05 );
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

void ioDummySubCrane::TranslateByConveyer( const D3DXVECTOR3 &vMove )
{
}

void ioDummySubCrane::SetDieState( bool bEffect, bool bWoundedDie /*= false */ )
{
	ProcessEnd();

	ioDummyChar::SetDieState( bEffect, bWoundedDie );
}

bool ioDummySubCrane::IsNeedCheckMapCollision() const
{
	return !m_bDisableMapCollision;
}

int ioDummySubCrane::DontMoveEntityLevel() const
{
	return 1;
}

void ioDummySubCrane::SetDelayState( ioBaseChar *pOwner )
{
	if ( !m_pGroup )
		return;

	int iAniID = m_pGroup->GetAnimationIdx( m_szDelayAniName );
	if( iAniID == -1 )
		return;

	m_pGroup->ClearAllActionAni( FLOAT100, true );
	m_pGroup->ClearAllLoopAni( FLOAT100, true );

	m_pGroup->SetLoopAni( iAniID, FLOAT100 );

	m_DummySubCraneState = DSAS_Delay;
}

void ioDummySubCrane::SetGrapState( ioBaseChar *pOwner )
{
	if ( !m_pGroup )
		return;

	int nAniID = m_pGroup->GetAnimationIdx( m_GrapInfo.m_AttackAnimation );
	if( nAniID == -1 )	
		return;

	float fTimeRate = m_GrapInfo.m_fAttackAniRate;
	DWORD dwPreDelay = m_GrapInfo.m_dwPreDelay;

	m_pGroup->ClearAllLoopAni( FLOAT100, true );
	m_pGroup->ClearAllActionAni( FLOAT100, true );

	m_pGroup->SetActionAni( nAniID, FLOAT100, FLOAT100, FLOAT1, FLOAT1/fTimeRate, 0.0f, dwPreDelay );

	m_dwFireTime = m_pGroup->GetAnimationEventTime_e( nAniID, "fire_s" ) * fTimeRate;

	m_DummySubCraneState = DSAS_Grap;

	ioWeapon *pAttachWeapon = g_WeaponMgr.FindWeapon( pOwner, m_dwAttachWeaponIdx );
	if ( pAttachWeapon )
		pAttachWeapon->SetWeaponDead();
}

void ioDummySubCrane::SetAttackState()
{
	if ( !m_pGroup )
		return;
	
	int nAniID = m_pGroup->GetAnimationIdx( m_AttackInfo.m_AttackAnimation );
	if( nAniID == -1 )	
		return;

	float fTimeRate = m_AttackInfo.m_fAttackAniRate;
	DWORD dwPreDelay = m_AttackInfo.m_dwPreDelay;

	m_pGroup->ClearAllLoopAni( FLOAT100, true );
	m_pGroup->ClearAllActionAni( FLOAT100, true );

	m_pGroup->SetActionAni( nAniID, FLOAT100, FLOAT100, FLOAT1, FLOAT1/fTimeRate, 0.0f, dwPreDelay );

	m_dwFireTime = m_pGroup->GetAnimationEventTime_e( nAniID, "fire_s" ) * fTimeRate;

	m_DummySubCraneState = DSAS_Attack;
	m_dwAttachWeaponIdx = 0;
}

void ioDummySubCrane::ProcessDelayState( ioBaseChar *pOwner )
{
	if ( !pOwner )
		return;

	ioDummyCrane *pDummyCrane = ToDummyCrane( g_DummyCharMgr.FindDummyCharToName( pOwner, m_szFindCraneDummy ) );
	if ( !pDummyCrane )
	{
		SetDieState( true );
		return;
	}

	SetWorldOrientationAndDir( pDummyCrane->GetWorldOrientation() );

	D3DXVECTOR3 vPos = pDummyCrane->GetWorldPosition() + GetWorldOrientation() * m_vDelaySubCraneOffset;
	SetWorldPosition( vPos );
}

void ioDummySubCrane::ProcessAttackState( ioBaseChar *pOwner, float fTimePerSec )
{
	if ( !pOwner )
		return;

	DWORD dwCurTime = FRAMEGETTIME();
	if ( m_dwFireTime > 0 && m_dwFireTime < dwCurTime )
	{
		CreateWeapon( pOwner, m_AttackInfo, true );
		m_dwFireTime = 0;
	}

	CheckDown( fTimePerSec );
}

void ioDummySubCrane::ProcessGrapState( ioBaseChar *pOwner, float fTimePerSec )
{
	if ( !pOwner )
		return;

	DWORD dwCurTime = FRAMEGETTIME();
	if ( m_dwFireTime > 0 && m_dwFireTime < dwCurTime )
	{
		CreateWeapon( pOwner, m_GrapInfo, false );
		m_dwFireTime = 0;
	}

	CheckUp( fTimePerSec );
}

void ioDummySubCrane::ProcessEnd()
{
	ioBaseChar *pOwner = GetOwner();
	if ( !pOwner )
		return;

	ioPlayStage *pStage = pOwner->GetCreator();
	if ( !pStage )
		return;		
}

void ioDummySubCrane::CreateWeapon( ioBaseChar *pOwner, const AttackAttribute &sCurInfo, bool bAttachIdx )
{
	if ( !pOwner )
		return;
	
	if ( sCurInfo.m_vWeaponInfoList.empty() )
		return;

	WeaponInfo kInfo = sCurInfo.m_vWeaponInfoList[0];
	DWORD dwWeaponBaseIndex = pOwner->IncreaseWeaponIndexBase();

	FireTime kFireTime;
	kFireTime.dwStart = FRAMEGETTIME();
	kFireTime.iAttributeIdx  = kInfo.m_iWeaponIdx;
	kFireTime.iResistanceIdx = kInfo.m_iResistanceIdx;
	kFireTime.dwWeaponIdx = dwWeaponBaseIndex + 1;

	kFireTime.szWoundedAni   = kInfo.m_WoundedAnimation;
	kFireTime.dwWoundedAniDuration  = kInfo.m_dwWoundedDuration;
	kFireTime.bLoopWoundedAni = kInfo.m_bWoundedLoopAni;

	kFireTime.eFireTimeType = FTT_DUMMY_CHAR_ATTACK;
	kFireTime.m_iExtraValue = m_iCreateIndex;

	D3DXVECTOR3 vCurDir = GetWorldOrientation() * D3DXVECTOR3( 0.0f, 0.0f, -FLOAT1 );
	D3DXVec3Normalize( &vCurDir, &vCurDir );

	D3DXVECTOR3 vCurPos = GetMidPositionByRate();
	ioWeapon *pWeapon = pOwner->ExtendFireExplicit( kFireTime, vCurPos, vCurDir, "" );
	if( pWeapon )
	{
		if ( bAttachIdx )
		{
			m_dwAttachWeaponIdx = pWeapon->GetWeaponIndex();

			ioZoneEffectWeapon *pZone = ToZoneEffectWeapon( pWeapon );
			if( pZone )
			{
				pZone->SetCollisionDir( vCurDir );
				pZone->SetZoneValue( true, false );
			}
		}
	}
}

void ioDummySubCrane::CheckUp( float fTimePerSec )
{	
	ioBaseChar *pOwner = GetOwner();
	if( !pOwner )	
		return;

	ioPlayStage *pStage = pOwner->GetCreator();
	if ( !pStage )
		return;

	ioDummyCrane *pDummyCrane = ToDummyCrane( g_DummyCharMgr.FindDummyCharToName( pOwner, m_szFindCraneDummy ) );
	if ( !pDummyCrane )
		return;

	D3DXVECTOR3 vCurPos = GetWorldPosition();
	vCurPos.y += ( m_fUpSpeed * fTimePerSec );

	bool bDelay = false;
	D3DXVECTOR3 vMaxOffsetPos = pDummyCrane->GetWorldPosition() + GetWorldOrientation() * m_vDelaySubCraneOffset;
	if ( vCurPos.y > vMaxOffsetPos.y )
	{
		vCurPos.y = vMaxOffsetPos.y;
		bDelay = true;
	}

	SetWorldPosition( vCurPos );

	if ( bDelay )
	{
		SetDelayState( pOwner );
		SetCraneDelayState( pOwner );
	}
}

void ioDummySubCrane::CheckDown( float fTimePerSec )
{		
	ioBaseChar *pOwner = GetOwner();
	if( !pOwner )	
		return;

	ioPlayStage *pStage = pOwner->GetCreator();
	if ( !pStage )
		return;

	ioDummyCrane *pDummyCrane = ToDummyCrane( g_DummyCharMgr.FindDummyCharToName( pOwner, m_szFindCraneDummy ) );
	if ( !pDummyCrane )
		return;

	float fMapHeight = GetBottomHeight();
	D3DXVECTOR3 vCurPos = GetWorldPosition();
	vCurPos.y -= ( m_fDownSpeed * fTimePerSec );

	bool bLand = false;
	if( vCurPos.y < fMapHeight )
	{
		vCurPos.y = fMapHeight;
		bLand = true;
	}

	pStage->CheckFlyMapLimit( &vCurPos );
	pStage->CheckAxisLimit( &vCurPos );
	SetWorldPosition( vCurPos );

	D3DXVECTOR3 vMaxOffsetPos = pDummyCrane->GetWorldPosition() + GetWorldOrientation() * m_vDelaySubCraneOffset;
	D3DXVECTOR3 vDiff = vMaxOffsetPos - GetWorldPosition();
	if ( D3DXVec3Length( &vDiff ) >= m_fMaxDownHeight )
		bLand = true;

	if ( bLand )
		SetGrapState( pOwner );
}

void ioDummySubCrane::SetCraneDelayState( ioBaseChar *pOwner )
{
	if( !pOwner )	
		return;
	
	ioDummyCrane *pDummyCrane = ToDummyCrane( g_DummyCharMgr.FindDummyCharToName( pOwner, m_szFindCraneDummy ) );
	if ( !pDummyCrane )
		return;

	pDummyCrane->ResetCraneState();
}