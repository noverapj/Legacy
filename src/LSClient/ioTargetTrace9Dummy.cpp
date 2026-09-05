
#include "stdafx.h"

#include "ioTargetTrace9Dummy.h"
#include "ioDummyAttachBuff3.h"

ioTargetTrace9Dummy::ioTargetTrace9Dummy( ioEntityGroup *pGrp, ioPlayMode *pMode )
	: ioDummyChar( pGrp, pMode )
{
	m_bSetUpKey	= false;
	m_bSetDownKey = false;
	m_CurDirKey = ioUserKeyInput::DKI_NONE;
	m_TraceState = TS_Start;
	dwCheckTraceTime = 0;
	dwCheckAttackTime = 0;
}

ioTargetTrace9Dummy::~ioTargetTrace9Dummy()
{
}

void ioTargetTrace9Dummy::LoadProperty( ioINILoader &rkLoader )
{
	ioDummyChar::LoadProperty( rkLoader );
	
	m_fTraceRange = rkLoader.LoadFloat_e( "trace_range", 0.0f );
	m_fTraceAngle = rkLoader.LoadFloat_e( "trace_angle", 0.0f );
	m_fTraceUpHeight = rkLoader.LoadFloat_e( "trace_up_height", 0.0f );
	m_fTraceUnderHeight = rkLoader.LoadFloat_e( "trace_under_height", 0.0f );
	dwTraceTime = rkLoader.LoadInt_e( "trace_time", 0 );

	m_fAttackRange = rkLoader.LoadFloat_e( "attack_range", 0.0f );
	m_fAttackAngle = rkLoader.LoadFloat_e( "attack_angle", 0.0f );
	m_fAttackUpHeight = rkLoader.LoadFloat_e( "attack_up_height", 0.0f );
	m_fAttackUnderHeight = rkLoader.LoadFloat_e( "attack_under_height", 0.0f );

	m_nAttackMaxCnt = rkLoader.LoadInt_e( "attack_max_cnt", 0 );
	dwAttackTime = rkLoader.LoadInt_e( "attack_time", 0 );
	m_nWeaponType = rkLoader.LoadInt_e( "weapon_type", NO_WEAPON_ATTRIBUTE );
}

void ioTargetTrace9Dummy::ProcessState( float fTimePerSec, DWORD dwPreTime )
{
	CheckLive( GetOwner() );

	switch( m_TraceState )
	{
	case TS_Start:
		SetFindState();
		break;
	case TS_Find_Trace_Target:
		SetMoveState( GetOwner() );
		break;
	case TS_Move:
		{
			if ( !ChangeTraceTarget() )
				ProcessMove( GetOwner(), fTimePerSec );

			CreateWeapon( GetOwner() );
		}
		break;
	case TS_End:
		SetDieState( true );
		break;
	}
}

void ioTargetTrace9Dummy::ApplyDummyCharSync( ioPlayStage *pStage, SP2Packet &rkPacket )
{
	if( !pStage ) 
		return;	

	int iType;
	rkPacket >> iType;	

	if ( m_TraceState == TS_Start )
	{
		SetFindState();
	}

	switch ( iType )
	{
	case TS_Find_Trace_Target:
		{
			rkPacket >> m_szTraceTarget;
			dwCheckTraceTime = FRAMEGETTIME();
			m_TraceState = TS_Move;
		}
		break;
	case TS_End:
		m_TraceState = TS_End;
		break;
	}
}

bool ioTargetTrace9Dummy::IsActivateWeapon()
{
	return true;
}

ApplyWeaponExp ioTargetTrace9Dummy::ApplyWeapon( ioWeapon *pWeapon )
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

void ioTargetTrace9Dummy::TranslateByConveyer( const D3DXVECTOR3 &vMove )
{
}

void ioTargetTrace9Dummy::SetDieState( bool bEffect, bool bWoundedDie /*= false */ )
{
	ioDummyChar::SetDieState( bEffect, bWoundedDie );
}

bool ioTargetTrace9Dummy::IsNeedCheckMapCollision() const
{
	return !m_bDisableMapCollision;
}

int ioTargetTrace9Dummy::DontMoveEntityLevel() const
{
	return 1;
}

void ioTargetTrace9Dummy::SetFindState()
{
	if( !m_DummyEffect.IsEmpty() )
	{
		m_pGroup->AttachEffect( m_DummyEffect, NULL );
		m_bMoveEffectCreated = true;
	}
	m_TraceState = TS_Find_Trace_Target;
}

void ioTargetTrace9Dummy::SetMoveState( ioBaseChar *pOwner )
{
	if ( !pOwner || !pOwner->IsNeedProcess() )
		return;

	m_szTraceTarget.Clear();

	ioHashStringVec szTraceTargetList;
	FindTarget( pOwner, m_fTraceRange, m_fTraceAngle, m_fTraceUpHeight, m_fTraceUnderHeight, szTraceTargetList, 1 );
	dwCheckTraceTime = FRAMEGETTIME();
	m_TraceState = TS_Move;

	if ( !szTraceTargetList.empty() )
		m_szTraceTarget = szTraceTargetList[0];

	if ( pOwner->IsNeedSendNetwork() )
	{
		SP2Packet kPacket( CUPK_DUMMY_CHAR_SYNC );
		kPacket << m_OwnerName;
		kPacket << m_iCreateIndex;
		kPacket << TS_Find_Trace_Target;
		kPacket << m_szTraceTarget;
		P2PNetwork::SendToAllPlayingUser( kPacket );
	}
}

void ioTargetTrace9Dummy::ProcessMove( ioBaseChar *pOwner, float fTimePerSec )
{
	if ( !pOwner )
		return;

	D3DXVECTOR3 vMoveDir = GetWorldOrientation() * D3DXVECTOR3( 0.0f, 0.0f, -FLOAT1 );

	if ( !m_szTraceTarget.IsEmpty() )
	{
		ioBaseChar *pTarget = pOwner->GetBaseChar( m_szTraceTarget );
		if ( pTarget )
			vMoveDir = pTarget->GetMidPositionByRate() - GetMidPositionByRate();
	}

	vMoveDir.y = 0;
	D3DXVec3Normalize( &vMoveDir, &vMoveDir );

	D3DXQUATERNION qtTargetRot;
	ioMath::CalcDirectionQuaternion( qtTargetRot, -vMoveDir );
	SetWorldOrientationAndDir( qtTargetRot );

	Translate( vMoveDir * ( m_fMoveSpeed * fTimePerSec ) );
}

void ioTargetTrace9Dummy::FindTarget( ioBaseChar *pOwner, float fTargetRange, float fTargetAngle, float fTargetUpheight, float fTargetUnderHeight, ioHashStringVec &szTargetList, int nMaxTargetCnt )
{
	if ( !pOwner )
		return;

	ioPlayStage *pStage = pOwner->GetCreator();
	if( !pStage ) 
		return;

	szTargetList.clear();

	BaseCharList vTargetList;
	BaseCharDistanceSqList vNewCharList;

	D3DXVECTOR3 vPos = GetMidPositionByRate();
	pOwner->GetTargetCharListByCylinder( vPos, fTargetRange, fTargetAngle, fTargetUpheight, fTargetUnderHeight, vTargetList );

	BaseCharList::iterator iter;
	for( iter = vTargetList.begin(); iter != vTargetList.end(); ++iter )
	{
		ioBaseChar *pTarget = (*iter);

		if( !pTarget )	
			continue;

		if( pOwner == pTarget )	
			continue;

		if( pOwner->GetTeam() == pTarget->GetTeam() )
			continue;

		if( pTarget->IsSystemState() )
			continue;
			
		if( !pTarget->IsEnableTarget() )
			continue;

		D3DXVECTOR3 vDiff = pTarget->GetMidPositionByRate() - vPos;
		float fDistanceSq = D3DXVec3LengthSq( &vDiff );

		BaseCharDistanceSq kTarget;
		kTarget.m_Name = pTarget->GetCharName();
		kTarget.m_fDistanceSq = fDistanceSq;

		vNewCharList.push_back( kTarget );
	}

	if( !vNewCharList.empty() )
	{
		std::sort( vNewCharList.begin(), vNewCharList.end(), BaseCharDistanceSqSort2() );
		for ( int i=0; i<nMaxTargetCnt; ++i )
		{
			if ( COMPARE( i, 0, (int)vNewCharList.size() ) )
				szTargetList.push_back( vNewCharList[i].m_Name );
		}
	}
}

bool ioTargetTrace9Dummy::ChangeTraceTarget()
{
	DWORD dwCurTime = FRAMEGETTIME();

	if ( dwCheckTraceTime + dwTraceTime > dwCurTime )
		return false;

	m_TraceState = TS_Find_Trace_Target;
	return true;
}

void ioTargetTrace9Dummy::CreateWeapon( ioBaseChar *pOwner )
{
	if ( !pOwner )
		return;
	
	DWORD dwCurTime = FRAMEGETTIME();
	if ( dwCheckAttackTime + dwAttackTime > dwCurTime )
		return;

	ioHashStringVec szAttackTargetList;
	FindTarget( pOwner, m_fAttackRange, m_fAttackAngle, m_fAttackUpHeight, m_fAttackUnderHeight, szAttackTargetList, m_nAttackMaxCnt );

	if ( szAttackTargetList.empty() )
		return;

	dwCheckAttackTime = dwCurTime;

	DWORD dwBaseWeaponIndex = pOwner->GetWeaponIndexBase() + 1;
	for ( int i=0; i<(int)szAttackTargetList.size(); ++i )
	{
		FireTime kFireTime;
		kFireTime.dwStart = dwCurTime;
		kFireTime.iAttributeIdx = m_nWeaponType;
		kFireTime.iResistanceIdx = 0;
		kFireTime.szWoundedAni  = "";
		kFireTime.dwWoundedAniDuration = 0;
		kFireTime.bLoopWoundedAni = false;
		kFireTime.dwWeaponIdx = dwBaseWeaponIndex+i;
		kFireTime.eFireTimeType = FTT_DUMMY_CHAR_ATTACK;

		D3DXVECTOR3 vPos = GetMidPositionByRate();
		D3DXVECTOR3 vDir = GetWorldOrientation() * D3DXVECTOR3( 0.0f, 0.0f, -FLOAT1 );

		ioBaseChar *pTarget = pOwner->GetBaseChar( szAttackTargetList[i] );
		if ( pTarget )
			vDir = pTarget->GetMidPositionByRate() - vPos;

		D3DXVec3Normalize( &vDir, &vDir );
		ioWeapon *pWeapon = pOwner->ExtendFireExplicit( kFireTime, vPos, vDir, "" );
		if ( pWeapon )
		{
			pWeapon->SetPosition( vPos );
			pWeapon->SetMoveDir( vDir );
		}
	}
}

void ioTargetTrace9Dummy::CheckLive( ioBaseChar *pOwner )
{
	if ( !pOwner || !pOwner->IsNeedProcess() )
		return;

	DWORD dwCurTime = FRAMEGETTIME();
	if ( m_dwCreateTime + m_dwLifeTime > dwCurTime )
		return;

	m_TraceState = TS_End;

	if ( pOwner->IsNeedSendNetwork() )
	{
		SP2Packet kPacket( CUPK_DUMMY_CHAR_SYNC );
		kPacket << m_OwnerName;
		kPacket << m_iCreateIndex;
		kPacket << TS_End;
		P2PNetwork::SendToAllPlayingUser( kPacket );
	}
}

bool ioTargetTrace9Dummy::IsMountingAvailable() const
{	
	return false;
}

bool ioTargetTrace9Dummy::IsMountAvailable() const
{	
	return false;
}

bool ioTargetTrace9Dummy::IsNowMountEnable() const
{
	return false;
}