
#include "stdafx.h"

#include "ioTargetTrace10Dummy.h"
#include "WeaponDefine.h"

ioTargetTrace10Dummy::ioTargetTrace10Dummy( ioEntityGroup *pGrp, ioPlayMode *pMode )
	: ioDummyChar( pGrp, pMode )
{
	m_bSetUpKey	= false;
	m_bSetDownKey = false;
	m_CurDirKey = ioUserKeyInput::DKI_NONE;
	m_TraceState = TS_Start;
	dwCheckAttackTime = 0;
	m_nAttackCurCnt = 0;
}

ioTargetTrace10Dummy::~ioTargetTrace10Dummy()
{
}

void ioTargetTrace10Dummy::LoadProperty( ioINILoader &rkLoader )
{
	ioDummyChar::LoadProperty( rkLoader );

	char szBuf[MAX_PATH];
	rkLoader.LoadString_e( "trace_delay_move_ani", "", szBuf, MAX_PATH );
	m_szTraceDelayMoveAni = szBuf;
	m_fTraceDelayMoveAniRate = rkLoader.LoadFloat_e( "trace_delay_move_ani_rate", FLOAT1 );

	rkLoader.LoadString_e( "trace_target_move_ani", "", szBuf, MAX_PATH );
	m_szTraceTargetMoveAni = szBuf;
	m_fTraceTargetMoveAniRate = rkLoader.LoadFloat_e( "trace_target_move_ani_rate", FLOAT1 );

	m_fTraceDelayMoveSpeed = rkLoader.LoadFloat_e( "trace_delay_move_speed", 0.0f );
	m_fTraceTargetMoveSpeed = rkLoader.LoadFloat_e( "trace_target_move_speed", 0.0f );

	m_fTraceRange = rkLoader.LoadFloat_e( "trace_range", 0.0f );
	m_fTraceAngle = rkLoader.LoadFloat_e( "trace_angle", 0.0f );
	m_fTraceUpHeight = rkLoader.LoadFloat_e( "trace_up_height", 0.0f );
	m_fTraceUnderHeight = rkLoader.LoadFloat_e( "trace_under_height", 0.0f );

	m_fTargetMinHeight = rkLoader.LoadFloat_e( "target_min_height", 0.0f );
	m_fTargetArriveDist = rkLoader.LoadFloat_e( "target_arrive_dist", 0.0f );

	m_nAttackMaxCnt = rkLoader.LoadInt_e( "attack_max_cnt", 0 );
	dwAttackTime = rkLoader.LoadInt_e( "attack_time", 0 );
	m_nWeaponType = rkLoader.LoadInt_e( "weapon_type", NO_WEAPON_ATTRIBUTE );
}

void ioTargetTrace10Dummy::ProcessState( float fTimePerSec, DWORD dwPreTime )
{
	CheckLive( GetOwner() );

	DWORD dwCurTime = FRAMEGETTIME();
	switch( m_TraceState )
	{
	case TS_Start:
		if ( m_dwStartAniEndTime < dwCurTime )
			SetTraceDelayMoveState();
		break;
	case TS_Trace_Delay_Move:
		{
			if ( CheckFindTargetState( GetOwner() ) )
			{
				SetTraceTargetFindState( GetOwner() );
				return;
			}

			ProcessTraceDelayMove( fTimePerSec );
		}
		break;
	case TS_Trace_Target_Move:
		{
			if ( !CheckTarget( GetOwner() ) )
			{
				SetRestartState( GetOwner() );
				return;
			}

			if ( CheckTargetAttackState( GetOwner() ) )
			{
				SetTraceTargetAttackState( GetOwner() );
				return;
			}

			ProcessTraceTargetMove( GetOwner(), fTimePerSec );
		}
		break;
	case TS_Trace_Target_Attack:
		{
			if ( !CheckTarget( GetOwner() ) || m_nAttackCurCnt >= m_nAttackMaxCnt )
			{
				SetRestartState( GetOwner() );
				return;
			}

			SetTargetRotate( GetOwner() );
			CreateWeapon( GetOwner() );
		}
		break;
	case TS_End:
		SetDieState( true );
		break;
	}
}

void ioTargetTrace10Dummy::ApplyDummyCharSync( ioPlayStage *pStage, SP2Packet &rkPacket )
{
	if( !pStage ) 
		return;	

	int nType;
	rkPacket >> nType;	
	
	switch ( nType )
	{
	case TS_Trace_Target_Move:
		{
			D3DXQUATERNION qtNewRot;
			D3DXVECTOR3 vNewPos;

			rkPacket >> qtNewRot;
			rkPacket >> vNewPos;			
			rkPacket >> m_szTraceTarget;
			
			SetWorldOrientationAndDir( qtNewRot );
			SetWorldPosition( vNewPos );

			if ( !m_szTraceTarget.IsEmpty() )
			{
				SetLoopAni( m_szTraceTargetMoveAni, m_fTraceTargetMoveAniRate );
				m_TraceState = TS_Trace_Target_Move;
			}
			else
				SetTraceDelayMoveState();
		}
		break;
	case TS_Trace_Target_Attack:
		{
			D3DXQUATERNION qtNewRot;
			D3DXVECTOR3 vNewPos;

			rkPacket >> qtNewRot;
			rkPacket >> vNewPos;

			SetWorldOrientationAndDir( qtNewRot );
			SetWorldPosition( vNewPos );

			m_TraceState = TS_Trace_Target_Attack;
		}
		break;
	case TS_Create_Weapon:
		{
			ioBaseChar *pOwner = GetOwner();
			if ( !pOwner )
				return;

			D3DXQUATERNION qtNewRot;
			D3DXVECTOR3 vNewPos, vWeaponDir;
			DWORD dwBaseWeaponIndex;

			rkPacket >> qtNewRot;
			rkPacket >> vNewPos;

			rkPacket >> dwBaseWeaponIndex;
			rkPacket >> vWeaponDir;

			SetWorldOrientationAndDir( qtNewRot );
			SetWorldPosition( vNewPos );			

			pOwner->SetWeaponIndexBase( dwBaseWeaponIndex );

			FireTime kFireTime;
			kFireTime.dwStart = FRAMEGETTIME();
			kFireTime.iAttributeIdx = m_nWeaponType;
			kFireTime.iResistanceIdx = 0;
			kFireTime.szWoundedAni  = "";
			kFireTime.dwWoundedAniDuration = 0;
			kFireTime.bLoopWoundedAni = false;
			kFireTime.dwWeaponIdx = dwBaseWeaponIndex;
			kFireTime.eFireTimeType = FTT_DUMMY_CHAR_ATTACK;

			D3DXVECTOR3 vPos = GetMidPositionByRate();
			ioWeapon *pWeapon = pOwner->ExtendFireExplicit( kFireTime, vPos, vWeaponDir, "" );
			if ( pWeapon )
			{
				pWeapon->SetPosition( vPos );
				pWeapon->SetMoveDir( vWeaponDir );
			}
		}
		break;
	case TS_Restart:
		{
			D3DXQUATERNION qtNewRot;
			D3DXVECTOR3 vNewPos;

			rkPacket >> qtNewRot;
			rkPacket >> vNewPos;

			SetWorldOrientationAndDir( qtNewRot );
			SetWorldPosition( vNewPos );

			SetTraceDelayMoveState();
		}
		break;
	case TS_End:
		m_TraceState = TS_End;
		break;
	}
}

bool ioTargetTrace10Dummy::IsActivateWeapon()
{
	return true;
}

ApplyWeaponExp ioTargetTrace10Dummy::ApplyWeapon( ioWeapon *pWeapon )
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

void ioTargetTrace10Dummy::TranslateByConveyer( const D3DXVECTOR3 &vMove )
{
}

void ioTargetTrace10Dummy::SetDieState( bool bEffect, bool bWoundedDie /*= false */ )
{
	ioDummyChar::SetDieState( bEffect, bWoundedDie );
}

bool ioTargetTrace10Dummy::IsNeedCheckMapCollision() const
{
	return !m_bDisableMapCollision;
}

int ioTargetTrace10Dummy::DontMoveEntityLevel() const
{
	return 1;
}

bool ioTargetTrace10Dummy::IsMountingAvailable() const
{	
	return false;
}

bool ioTargetTrace10Dummy::IsMountAvailable() const
{	
	return false;
}

bool ioTargetTrace10Dummy::IsNowMountEnable() const
{
	return false;
}

void ioTargetTrace10Dummy::SetLoopAni( ioHashString szLoopAni, float fTimeRate )
{
	if ( !m_pGroup )
		return;

	int nAniID = m_pGroup->GetAnimationIdx( szLoopAni );
	if( nAniID == -1 )
		return;

	m_pGroup->ClearAllActionAni( FLOAT10, true );
	m_pGroup->ClearAllLoopAni( FLOAT10, true );
	m_pGroup->SetLoopAni( nAniID, FLOAT100, FLOAT1, FLOAT1/fTimeRate );
}

void ioTargetTrace10Dummy::SetTraceDelayMoveState()
{
	SetLoopAni( m_szTraceDelayMoveAni, m_fTraceDelayMoveAniRate );
	m_fCurMoveRange = 0.0f;
	dwCheckAttackTime = 0;
	m_nAttackCurCnt = 0;
	m_TraceState = TS_Trace_Delay_Move;
}

void ioTargetTrace10Dummy::ProcessTraceDelayMove( float fTimePerSec )
{
	D3DXVECTOR3 vMoveDir = GetWorldOrientation() * D3DXVECTOR3( 0.0f, 0.0f, -FLOAT1 );
	vMoveDir.y = 0;
	D3DXVec3Normalize( &vMoveDir, &vMoveDir );

	float fMoveValue = fTimePerSec * m_fTraceDelayMoveSpeed;
	m_fCurMoveRange += fMoveValue;
	
	Translate( vMoveDir * fMoveValue );
}

void ioTargetTrace10Dummy::SetTraceTargetFindState( ioBaseChar *pOwner )
{
	if ( !pOwner || !pOwner->IsNeedProcess() )
		return;

	FindTarget( pOwner, m_fTraceRange, m_fTraceAngle, m_fTraceUpHeight, m_fTraceUnderHeight, m_szTraceTarget );

	if ( !m_szTraceTarget.IsEmpty() )
	{
		SetLoopAni( m_szTraceTargetMoveAni, m_fTraceTargetMoveAniRate );
		m_TraceState = TS_Trace_Target_Move;
	}
	else
	{
		SetRandomRotate( pOwner );
		SetTraceDelayMoveState();
	}

	if ( pOwner->IsNeedSendNetwork() )
	{
		SP2Packet kPacket( CUPK_DUMMY_CHAR_SYNC );
		kPacket << m_OwnerName;
		kPacket << m_iCreateIndex;
		kPacket << TS_Trace_Target_Move;
		kPacket << GetWorldOrientation();
		kPacket << GetWorldPosition();
		kPacket << m_szTraceTarget;
		P2PNetwork::SendToAllPlayingUser( kPacket );
	}
}

void ioTargetTrace10Dummy::ProcessTraceTargetMove( ioBaseChar *pOwner, float fTimePerSec )
{
	if ( !pOwner )
		return;

	D3DXVECTOR3 vMoveDir = GetWorldOrientation() * D3DXVECTOR3( 0.0f, 0.0f, -FLOAT1 );
	ioBaseChar *pTarget = pOwner->GetBaseChar( m_szTraceTarget );
	if ( pTarget )
		vMoveDir = pTarget->GetMidPositionByRate() - GetMidPositionByRate();
	D3DXVec3Normalize( &vMoveDir, &vMoveDir );

	D3DXQUATERNION qtTargetRot = Help::ConvertDirToQuaternion( vMoveDir );
	SetWorldOrientationAndDir( qtTargetRot );

	Translate( vMoveDir * ( m_fTraceTargetMoveSpeed * fTimePerSec ) );
}

void ioTargetTrace10Dummy::SetTraceTargetAttackState( ioBaseChar *pOwner )
{
	if ( !pOwner || !pOwner->IsNeedProcess() )
		return;

	m_TraceState = TS_Trace_Target_Attack;

	if ( pOwner->IsNeedSendNetwork() )
	{
		SP2Packet kPacket( CUPK_DUMMY_CHAR_SYNC );
		kPacket << m_OwnerName;
		kPacket << m_iCreateIndex;
		kPacket << TS_Trace_Target_Attack;
		kPacket << GetWorldOrientation();
		kPacket << GetWorldPosition();
		P2PNetwork::SendToAllPlayingUser( kPacket );
	}
}

void ioTargetTrace10Dummy::SetRestartState( ioBaseChar *pOwner )
{
	if ( !pOwner || !pOwner->IsNeedProcess() )
		return;

	SetRandomRotate( pOwner );
	SetTraceDelayMoveState();

	if ( pOwner->IsNeedSendNetwork() )
	{
		SP2Packet kPacket( CUPK_DUMMY_CHAR_SYNC );
		kPacket << m_OwnerName;
		kPacket << m_iCreateIndex;
		kPacket << TS_Restart;
		kPacket << GetWorldOrientation();
		kPacket << GetWorldPosition();
		P2PNetwork::SendToAllPlayingUser( kPacket );
	}
}

void ioTargetTrace10Dummy::CheckLive( ioBaseChar *pOwner )
{
	if ( !pOwner )
	{
		m_TraceState = TS_End;
		return;
	}
	
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

bool ioTargetTrace10Dummy::CheckTarget( ioBaseChar *pOwner )
{
	if ( !pOwner )
		return false;

	ioBaseChar *pTarget = pOwner->GetBaseChar( m_szTraceTarget );
	if ( !pTarget )
		return false;

	return true;
}

bool ioTargetTrace10Dummy::CheckFindTargetState( ioBaseChar *pOwner )
{
	if ( !pOwner || !pOwner->IsNeedProcess() )
		return false;

	if( m_fCurMoveRange >= m_fDelayMoveRange )
		return true;

	return false;
}

bool ioTargetTrace10Dummy::CheckTargetAttackState( ioBaseChar *pOwner )
{
	if ( !pOwner || !pOwner->IsNeedProcess() )
		return false;

	ioBaseChar *pTarget = pOwner->GetBaseChar( m_szTraceTarget );
	if ( pTarget )
	{
		D3DXVECTOR3 vDiff = pTarget->GetMidPositionByRate() - GetMidPositionByRate();
		if ( D3DXVec3LengthSq( &vDiff ) <= ( m_fTargetArriveDist * m_fTargetArriveDist ) )
			return true;
	}

	return false;
}

void ioTargetTrace10Dummy::FindTarget( ioBaseChar *pOwner, float fTargetRange, float fTargetAngle, float fTargetUpheight, float fTargetUnderHeight, ioHashString &szTarget )
{
	if ( !pOwner )
		return;

	ioPlayStage *pStage = pOwner->GetCreator();
	if( !pStage ) 
		return;

	szTarget.Clear();

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

		D3DXVECTOR3 vTargetPos = pTarget->GetWorldPosition();
		float fMapHeight = pStage->GetMapHeightNoneEntity( vTargetPos.x, vTargetPos.z, vTargetPos.y+3.0f );
		if ( fabs( vTargetPos.y - fMapHeight ) < m_fTargetMinHeight )
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
		szTarget = vNewCharList[0].m_Name;
	}
}

void ioTargetTrace10Dummy::SetRandomRotate( ioBaseChar *pOwner )
{
	if ( !pOwner )
		return;

	DWORD dwSeed = timeGetTime();
	IORandom random;
	random.SetRandomSeed( dwSeed );

	float fRange = (float)random.Random(360);
	D3DXVECTOR3 vYAxis = ioMath::UNIT_Y;
	D3DXQUATERNION qtRandomRot;
	D3DXQuaternionRotationAxis( &qtRandomRot, &vYAxis, fRange );

	D3DXVECTOR3 vMoveDir = GetWorldOrientation() * D3DXVECTOR3( 0.0f, 0.0f, -FLOAT1 );
	vMoveDir = qtRandomRot * vMoveDir;
	vMoveDir.y = 0;
	D3DXVec3Normalize( &vMoveDir, &vMoveDir );

	D3DXQUATERNION qtTargetRot;
	ioMath::CalcDirectionQuaternion( qtTargetRot, -vMoveDir );
	SetWorldOrientationAndDir( qtTargetRot );
}

void ioTargetTrace10Dummy::SetTargetRotate( ioBaseChar *pOwner )
{
	if ( !pOwner )
		return;

	D3DXVECTOR3 vMoveDir = GetWorldOrientation() * D3DXVECTOR3( 0.0f, 0.0f, -FLOAT1 );
	ioBaseChar *pTarget = pOwner->GetBaseChar( m_szTraceTarget );
	if ( pTarget )
		vMoveDir = pTarget->GetMidPositionByRate() - GetMidPositionByRate();
	D3DXVec3Normalize( &vMoveDir, &vMoveDir );

	D3DXQUATERNION qtTargetRot = Help::ConvertDirToQuaternion( vMoveDir );
	SetWorldOrientationAndDir( qtTargetRot );
}

void ioTargetTrace10Dummy::CreateWeapon( ioBaseChar *pOwner )
{
	if ( !pOwner || !pOwner->IsNeedProcess() )
		return;

	DWORD dwCurTime = FRAMEGETTIME();
	if ( dwCheckAttackTime + dwAttackTime > dwCurTime )
		return;
	
	dwCheckAttackTime = dwCurTime;

	DWORD dwBaseWeaponIndex = pOwner->GetWeaponIndexBase()+1;
	pOwner->SetWeaponIndexBase( dwBaseWeaponIndex );

	FireTime kFireTime;
	kFireTime.dwStart = dwCurTime;
	kFireTime.iAttributeIdx = m_nWeaponType;
	kFireTime.iResistanceIdx = 0;
	kFireTime.szWoundedAni  = "";
	kFireTime.dwWoundedAniDuration = 0;
	kFireTime.bLoopWoundedAni = false;
	kFireTime.dwWeaponIdx = dwBaseWeaponIndex;
	kFireTime.eFireTimeType = FTT_DUMMY_CHAR_ATTACK;

	D3DXVECTOR3 vPos = GetMidPositionByRate();
	D3DXVECTOR3 vDir = GetWorldOrientation() * D3DXVECTOR3( 0.0f, 0.0f, -FLOAT1 );

	ioBaseChar *pTarget = pOwner->GetBaseChar( m_szTraceTarget );
	if ( pTarget )
		vDir = pTarget->GetMidPositionByRate() - vPos;
	D3DXVec3Normalize( &vDir, &vDir );

	ioWeapon *pWeapon = pOwner->ExtendFireExplicit( kFireTime, vPos, vDir, "" );
	if ( pWeapon )
	{
		pWeapon->SetPosition( vPos );
		pWeapon->SetMoveDir( vDir );
	}

	m_nAttackCurCnt++;

	if ( pOwner->IsNeedSendNetwork() )
	{
		SP2Packet kPacket( CUPK_DUMMY_CHAR_SYNC );
		kPacket << m_OwnerName;
		kPacket << m_iCreateIndex;
		kPacket << TS_Create_Weapon;
		kPacket << GetWorldOrientation();
		kPacket << GetWorldPosition();
		kPacket << dwBaseWeaponIndex;
		kPacket << vDir;
		P2PNetwork::SendToAllPlayingUser( kPacket );
	}
}