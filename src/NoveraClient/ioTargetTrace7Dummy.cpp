#include "StdAfx.h"
#include "ioTargetTrace7Dummy.h"

#include "ioPlayStage.h"
#include "ioBaseChar.h"

ioTargetTrace7Dummy::ioTargetTrace7Dummy( ioEntityGroup *pGrp, ioPlayMode *pMode )
	: ioDummyChar( pGrp, pMode )
{
	m_fRadiansForHeight	= 0.0f;
	m_dwSeed = 0;
}


ioTargetTrace7Dummy::~ioTargetTrace7Dummy(void)
{
}

void ioTargetTrace7Dummy::LoadProperty( ioINILoader &rkLoader )
{
	ioDummyChar::LoadProperty( rkLoader );

	LoadInitProperty( rkLoader );
}

void ioTargetTrace7Dummy::LoadInitProperty( ioINILoader &rkLoader )
{
	char szKey[MAX_PATH], szBuf[MAX_PATH];
	m_nAttackIndex = rkLoader.LoadInt_e( "destroy_attack_type", NO_WEAPON_ATTRIBUTE );
	
	rkLoader.LoadString_e( "target_attach_ani", "", szBuf, MAX_PATH );
	m_AttachAni = szBuf;
	m_fAttachAniRate = rkLoader.LoadFloat_e( "target_attach_ani_rate", FLOAT1 );

	m_dwUpDownSpeed = (DWORD)rkLoader.LoadInt_e( "dummy_updown_speed", 1000 );
	m_fUpDownOffset = rkLoader.LoadFloat_e( "dummy_updown_offset", 0.0f);

	int iCnt = rkLoader.LoadInt_e( "dummy_position_cnt", 0 );
	for ( int i = 0; i < iCnt; ++i )
	{
		DummyTargetAttachInfo kInfo;
		wsprintf_e( szKey, "dummy_position%d_angle", i+1 );
		kInfo.m_fAngle = rkLoader.LoadFloat( szKey, i+1 );
		wsprintf_e( szKey, "dummy_position%d_range_offset", i+1 );
		kInfo.m_fRangeOffset = rkLoader.LoadFloat( szKey,  i+1 );
		wsprintf_e( szKey, "dummy_position%d_height_offset", i+1 );
		kInfo.m_fHeightOffset = rkLoader.LoadFloat( szKey,  i+1 );
		m_DummyTAInfoList.push_back( kInfo );
	}
}

void ioTargetTrace7Dummy::ProcessState( float fTimePerSec, DWORD dwPreTime )
{
	switch( m_State )
	{
	case DCS_START:
		ProcessStartState( fTimePerSec );
		break;
	case DCS_DELAY:
		ProcessDelayState( fTimePerSec );
		break;
	case DCS_TRACE:
		ProcessTraceState( fTimePerSec, dwPreTime );
		break;
	case DCS_TARGET_ATTACH:
		ProcessTargetAttachState( fTimePerSec, dwPreTime );
		break;
	}

	if( m_dwLifeTime && (DWORD)(m_dwStartTime + m_dwLifeTime) < FRAMEGETTIME() && m_State != DCS_ATTACK )
		SendDieState( true );

	ProcessPos( fTimePerSec );
	ProcessEffectUpdate( fTimePerSec );
}

void ioTargetTrace7Dummy::ProcessStartState( float fTimePerSec )
{
	DWORD dwCurTime = FRAMEGETTIME();

	if( m_dwStartAniEndTime == 0 || m_dwStartAniEndTime < dwCurTime )
	{
		SetDelayState( false );
		return;
	}
}

void ioTargetTrace7Dummy::ProcessDelayState( float fTimePerSec )
{
	DWORD dwCurTime = FRAMEGETTIME();

	if( m_dwDelayStartTime > 0 && m_dwDelayStartTime + m_dwDelayDuration < dwCurTime )
	{
		m_dwDelayStartTime = 0;
		SetTraceState( DCTS_SENSING );
	}
}

void ioTargetTrace7Dummy::ProcessTraceState( float fTimePerSec, DWORD dwPreTime )
{
	ioBaseChar *pOwner = GetOwner();
	if( !pOwner ) return;

	ioPlayStage *pStage = pOwner->GetCreator();
	if( !pStage ) return;

	DWORD dwCurTime = FRAMEGETTIME();

	switch( m_TraceState )
	{
	case DCTS_NONE:
		break;
	case DCTS_SENSING:
		ProcessTraceSensing( fTimePerSec );
		break;
	case DCTS_TRACING:
		ProcessTraceTracingNoneTerrain( fTimePerSec );
		break;
	case DCTS_ATTACK:
		CheckReservedSliding();
		CheckExtraAniJump();
		ProcessTraceAttack( fTimePerSec, dwPreTime );
		break;
	}

	CheckTraceAttack();
	CheckTraceStateEnd();
}

void ioTargetTrace7Dummy::ProcessTraceAttack( float fTimePerSec, DWORD dwPreTime )
{
	static int i = 0;
	ioBaseChar *pOwner = GetOwner();
	if( !pOwner ) return;

	ioPlayStage *pStage = pOwner->GetCreator();
	if( !pStage ) return;

	DWORD dwCurTime = FRAMEGETTIME();

	D3DXVECTOR3 vCurDir = GetWorldOrientation() * D3DXVECTOR3( 0.0f, 0.0f, -FLOAT1 );
	D3DXVec3Normalize( &vCurDir, &vCurDir );

	D3DXVECTOR3 vCurPos = GetMidPositionByRate();

	DWORD dwLastFireTime = dwPreTime;
	FireTimeList::iterator iter = m_vAttackFireTimeList.begin();
	while( iter != m_vAttackFireTimeList.end() )
	{
		const FireTime &rkFire = *iter;

		DWORD dwFireTime = rkFire.dwStart;
		if( COMPARE( dwFireTime, dwPreTime, dwCurTime ) )
		{
			if( rkFire.iAttributeIdx != NO_WEAPON_ATTRIBUTE )
			{
				pOwner->ExtendFireExplicit( rkFire, vCurPos, vCurDir, "" );
			}

			iter = m_vAttackFireTimeList.erase( iter );
		}
		else
		{
			++iter;
		}
	}


	if( m_dwAttackAniEndTime == 0 || m_dwAttackAniEndTime < dwCurTime )
	{
		if( m_DummyCharType == DCT_TARGET_TRACE7 )
		{
			m_LastAttacker = m_OwnerName;
			SetTargetAttachState( false );
			return;
		}
	}

}


void ioTargetTrace7Dummy::SetDieState( bool bEffect, bool bWoundedDie /*= false */ )
{
	ioDummyChar::SetDieState( bEffect, bWoundedDie );
	FireWeapon();
}

void ioTargetTrace7Dummy::CreateWeapon( int nAttackIndex, const D3DXVECTOR3 &vPos, const D3DXVECTOR3 &vDir )
{
	ioBaseChar *pOwner = GetOwner();

	if ( !pOwner )
		return;

	m_dwBaseWeaponIndex = pOwner->GetWeaponIndexBase() + 1;

	FireTime kFireTime;
	kFireTime.dwStart = FRAMEGETTIME();

	kFireTime.iAttributeIdx = nAttackIndex;
	kFireTime.iResistanceIdx = 0;
	kFireTime.szWoundedAni  = "";
	kFireTime.dwWoundedAniDuration = 0;
	kFireTime.bLoopWoundedAni = false;
	kFireTime.dwWeaponIdx = m_dwBaseWeaponIndex;
	kFireTime.eFireTimeType = FTT_DUMMY_CHAR_ATTACK;

	pOwner->SetWeaponIndexBase( m_dwBaseWeaponIndex );

	ioWeapon *pWeapon = pOwner->ExtendFireExplicit( kFireTime, vPos, vDir, "" );
	if ( pWeapon )
	{
		pWeapon->SetPosition( vPos );
	}	
}

void ioTargetTrace7Dummy::FireWeapon()
{
	if ( m_nAttackIndex == NO_WEAPON_ATTRIBUTE )
		return;

	D3DXVECTOR3 vDir = GetWorldOrientation() * D3DXVECTOR3(0.0f,0.0f,-FLOAT1);
	D3DXVec3Normalize( &vDir, &vDir);
	D3DXVECTOR3 vPos = GetWorldPosition();

	CreateWeapon( m_nAttackIndex, vPos, vDir );
}

void ioTargetTrace7Dummy::SetTargetAttachState( bool bNet )
{
	m_State = DCS_TARGET_ATTACH;

	m_pGroup->ClearAllLoopAni( FLOAT100 );
	m_pGroup->ClearAllActionAni( FLOAT100 );

	int iAniID = m_pGroup->GetAnimationIdx( m_AttachAni );
	if( 0 <= iAniID )
	{
		float fRate = FLOAT1;
		if( m_fAttachAniRate > 0.0f )
			fRate = m_fAttachAniRate;

		m_pGroup->SetLoopAni( iAniID, FLOAT100, FLOAT1, FLOAT1/fRate );
	}
}

void ioTargetTrace7Dummy::ProcessTargetAttachState( float fTimePerSec, DWORD dwPreTime )
{
	ioBaseChar *pOwner = GetOwner();
	if( !pOwner ) 
	{
		SetDieState( true );
		return;
	}

	ioPlayStage *pStage = pOwner->GetCreator();
	if( !pStage ) return;

	ioBaseChar *pTarget = pStage->GetBaseChar( m_TargetTraceName );
	if ( pTarget )
	{
		D3DXVECTOR3 vPos = CalcTargetAttachDummyPosition( pTarget );
		if( vPos == ioMath::VEC3_ZERO )
			vPos = pTarget->GetWorldPosition();

		SetWorldOrientation( pTarget->GetTargetRot() );
		SetWorldPosition( vPos );		
	}
}

D3DXVECTOR3 ioTargetTrace7Dummy::CalcTargetAttachDummyPosition( ioBaseChar* pChar )
{
	if( !pChar )
		return ioMath::VEC3_ZERO;

	D3DXVECTOR3	vPos = pChar->GetWorldPosition();
	D3DXVECTOR3 vDir = pChar->GetTargetRot() * D3DXVECTOR3( 0.0f, 0.0f, -FLOAT1 );

	// 웨폰 이펙트의 상하 움직임.
	float fTimePerSec = g_FrameTimer.GetSecPerFrame();
	float fRate = ( fTimePerSec * 1000 ) / m_dwUpDownSpeed;
	float fAddHeight = 360.0f * fRate;

	m_fRadiansForHeight += fAddHeight;
	if( m_fRadiansForHeight > 360.0f )
		m_fRadiansForHeight = 0.0f;

	float fHeightOffset = sin( DEGtoRAD(m_fRadiansForHeight) );
	fHeightOffset *= m_fUpDownOffset;

	// 일반적으로 스케일은 z, y, z값이 동일하게 가므로 x성분만으로 체크한다.
	float fScaleOffset = pChar->GetWorldScale().x;
	float fCurAngle = 0.0f;
	float fCurRange = 0.0f;
	float fCurHeight = 0.0f;

	int iRandomCnt = 0;
	IORandom random;
	random.SetRandomSeed( m_dwSeed );
	iRandomCnt = random.Random( m_DummyTAInfoList.size() );

	fCurAngle = m_DummyTAInfoList[ iRandomCnt ].m_fAngle;
	fCurRange = m_DummyTAInfoList[ iRandomCnt ].m_fRangeOffset * fScaleOffset;
	fCurHeight = m_DummyTAInfoList[ iRandomCnt ].m_fHeightOffset * fScaleOffset;

	D3DXQUATERNION qtAngle;
	D3DXQuaternionRotationYawPitchRoll( &qtAngle, DEGtoRAD(fCurAngle), 0.0f, 0.0f );
	vDir = qtAngle * vDir;

	if( fCurRange > 0.0f )
		vPos += vDir * fCurRange;

	if( fCurHeight > 0.0f )
	{
		vPos.y += fCurHeight + fHeightOffset;
	}

	return vPos;
}

