#include "StdAfx.h"
#include "ioTargetTrace14Dummy.h"

#include "ioBaseChar.h"
#include "ioPlayStage.h"

#include "ioWeaponItem.h"

ioTargetTrace14Dummy::ioTargetTrace14Dummy( ioEntityGroup *pGrp, ioPlayMode *pMode ):
ioDummyChar( pGrp, pMode )
{
	m_TargetTraceState = TS_START;
	m_fCurOwnerAttachAngle = 0.0f;
	m_iAttackCount = 0;
	m_dwSecondTargetSensingStartTime = 0;
	m_dwCreateItemCode = 0;
}

ioTargetTrace14Dummy::~ioTargetTrace14Dummy(void)
{
}

void ioTargetTrace14Dummy::InitDummyCharInfo( int iIndex, DWORD dwCreateGapTime, const D3DXVECTOR3 &vPos, const D3DXVECTOR3 &vScale, float fStartAngle/*=0.0f*/, bool bNetWork /*= false */ )
{
	ioDummyChar::InitDummyCharInfo( iIndex, dwCreateGapTime, vPos, vScale, fStartAngle, bNetWork );

	ioBaseChar *pOwner = GetOwner();
	if ( !pOwner )
		return;

	ioWeaponItem *pItem = pOwner->GetWeapon();
	if( pItem )
		m_dwCreateItemCode = pItem->GetItemCode();
}

void ioTargetTrace14Dummy::LoadProperty( ioINILoader &rkLoader )
{
	char szBuf[MAX_PATH] = "";
	
	ioDummyChar::LoadProperty( rkLoader );

	m_dwLifeTime = (DWORD)rkLoader.LoadInt_e( "life_time", 0 );

	//Owner Rot
	rkLoader.LoadString_e( "owner_rot_ani", "", szBuf, MAX_PATH );
	m_szOwnerRotAni = szBuf;
	m_fOwnerRotAniRate = rkLoader.LoadFloat_e( "owner_rot_ani_rate", FLOAT1 );
	m_dwRotateSpeed = (DWORD)rkLoader.LoadInt_e( "owner_rotate_speed", 0 );
	m_vAttachOffset.x = rkLoader.LoadFloat_e( "attach_offset_x", 0.0f );
	m_vAttachOffset.y = rkLoader.LoadFloat_e( "attach_offset_y", 0.0f );
	m_vAttachOffset.z = rkLoader.LoadFloat_e( "attach_offset_z", 0.0f );
	
	//Target Trace
	rkLoader.LoadString_e( "trace_move_ani", "", szBuf, MAX_PATH );
	m_szTraceMoveAni = szBuf;
	m_fTraceMoveAniRate = rkLoader.LoadFloat_e( "trace_move_ani_rate", FLOAT1 );

	m_fTargetArriveDist = rkLoader.LoadFloat_e( "target_arrive_dist", 0.0f );
	m_fTraceTargetMoveSpeed = rkLoader.LoadFloat_e( "trace_target_move_speed", 0.0f );

	LoadAttackAttribute( "first_attack", m_FirstAttack, rkLoader );
	LoadAttackAttribute( "second_attack", m_SecondAttack, rkLoader );


	m_fTargetRotateMaxAngleMove = rkLoader.LoadFloat_e( "target_rot_max_angle_move", 180.0f );
	m_fTargetRotateSpeed = (DWORD)rkLoader.LoadInt_e( "target_rotate_speed", 0 );

	m_dwSecondTargetSensingTime = (DWORD)rkLoader.LoadInt_e( "second_target_sensing_time", 0 );

	m_bCheckOwnerItem = rkLoader.LoadBool_e( "check_owner_item", false );
}

void ioTargetTrace14Dummy::ProcessState( float fTimerPerSec, DWORD dwPreTime )
{
	ioBaseChar *pOwner = GetOwner();
	if ( !pOwner )
	{
		SetDieState( true );
		return;
	}

	if ( !CheckLive( pOwner ) )
	{
		SetDieState( true );
		return;
	}

	DWORD dwCurTime = FRAMEGETTIME();
	switch( m_TargetTraceState )
	{
	case TS_START:
		ProcessStartState( pOwner );
		break;
	case TS_OWNER_ROTATE_MOVE:
		ProcessOwnerRotateMoveState( pOwner );
		break;
	case TS_TARGET_TRACE:
		ProcessTargetTraceState( pOwner, fTimerPerSec );
		break;
	case TS_TARGET_FIRST_ATTACK:
		ProcessTargetFirstAttackState( pOwner );
		break;
	case TS_TARGET_ROTATE:
		ProcessTargetRotateState( pOwner );
		break;
	case TS_TARGET_SENSING_STATE:
		ProcessTargetSensingState( pOwner );
		break;
	case TS_TARGET_SECOND_ATTACK:
		ProcessTargetSecondAttackState( pOwner );
		break;
	}

	ProcessAttackState( fTimerPerSec, dwPreTime );
}

bool ioTargetTrace14Dummy::CheckLive( ioBaseChar *pOwner )
{
	if ( !pOwner )
		return false;

	DWORD dwCurTime = FRAMEGETTIME();
	if ( m_dwLifeTime && m_dwCreateTime + m_dwLifeTime < dwCurTime )
	{
		if ( m_TargetTraceState != TS_TARGET_FIRST_ATTACK && m_TargetTraceState != TS_TARGET_SECOND_ATTACK )
			return false;
	}

	if ( m_bCheckOwnerItem )
	{
		if ( m_dwCreateItemCode == 0 )
			return false;

		ioWeaponItem *pWeapon = pOwner->GetWeapon();
		if ( !pWeapon )
			return false;
		if ( pWeapon->GetItemCode() != m_dwCreateItemCode )
			return false;
	}

	if ( pOwner->IsNeedProcess() )
	{
		if ( CheckOwnerDestroyState( pOwner ) )
			return false;
	}

	return true;
}

bool ioTargetTrace14Dummy::CheckOwnerDestroyState( ioBaseChar *pOwner )
{
	if ( !pOwner )
		return true;

	CharState eState = pOwner->GetState();
	if( eState == CS_DROP_ZONE_DOWN )
		return true;

	return false;
}

void ioTargetTrace14Dummy::ProcessStartState( ioBaseChar *pOwner )
{
	if ( !pOwner )
		return;
	
	DWORD dwCurTime = FRAMEGETTIME();

	if ( pOwner->IsNeedProcess() && m_dwStartAniEndTime < dwCurTime )
	{
		SetOwnerRotateMoveState( pOwner );
		return;
	}
}

void ioTargetTrace14Dummy::SetOwnerRotateMoveState( ioBaseChar *pOwner )
{
	if ( !pOwner )
		return;

	m_TargetTraceState = TS_OWNER_ROTATE_MOVE;

	m_szTargetName.Clear();

	SetLoopAni( m_szOwnerRotAni, m_fOwnerRotAniRate );

	if ( pOwner->IsNeedProcess() )
		m_fCurOwnerAttachAngle = RADtoDEG( ioMath::QuaterToYaw( GetWorldOrientation() ) );

	if ( pOwner->IsNeedSendNetwork() )
	{
		SP2Packet kPacket( CUPK_DUMMY_CHAR_SYNC );
		kPacket << m_OwnerName;
		kPacket << m_iCreateIndex;
		kPacket << static_cast<BYTE>(m_TargetTraceState);
		kPacket << GetWorldOrientation();
		kPacket << GetWorldPosition();
		kPacket << m_fCurOwnerAttachAngle;
		P2PNetwork::SendToAllPlayingUser( kPacket );
	}
}

bool ioTargetTrace14Dummy::CheckOwnerAttachPos( ioBaseChar *pOwner )
{
	if ( !pOwner )
		return false;

	if ( m_dwRotateSpeed == 0 )
		return false;

	float fFrameGap = g_FrameTimer.GetCurFrameGap();
	float fRate = fFrameGap / m_dwRotateSpeed;
	float fRotateAngle = 360.0f * fRate;
	m_fCurOwnerAttachAngle = RADtoDEG( ioMath::QuaterToYaw( GetWorldOrientation() ) );
	m_fCurOwnerAttachAngle += fRotateAngle;
	if ( m_fCurOwnerAttachAngle >= 360.0f )
	{
		m_fCurOwnerAttachAngle -= 360.0f;
		m_fCurOwnerAttachAngle = max( m_fCurOwnerAttachAngle, 0.0f );
	}

	float fYawR = DEGtoRAD( m_fCurOwnerAttachAngle );
	D3DXQUATERNION qtRotate;
	D3DXQuaternionRotationYawPitchRoll( &qtRotate, fYawR, 0.0f, 0.0f );

	D3DXVECTOR3 vNewPos = pOwner->GetWorldPosition() + qtRotate * m_vAttachOffset;
	SetWorldPosition( vNewPos );

	D3DXVECTOR3 vTargetDir = pOwner->GetMidPositionByRate() - GetWorldPosition();
	D3DXVec3Normalize( &vTargetDir, &vTargetDir );
	D3DXVECTOR3 vXZDir( vTargetDir.x, 0.0f, vTargetDir.z );
	D3DXVec3Normalize( &vXZDir, &vXZDir );

	D3DXQUATERNION qtRot;
	ioMath::CalcDirectionQuaternion( qtRot, -vXZDir );
	SetWorldOrientation( qtRot );
	return true;
}

void ioTargetTrace14Dummy::ProcessOwnerRotateMoveState( ioBaseChar *pOwner )
{
	if ( !pOwner )
		return;

	if ( FindTarget( pOwner ) )
	{
		SetTargetTraceState( pOwner );
		return;
	}

	CheckOwnerAttachPos( pOwner );
	
}

bool ioTargetTrace14Dummy::FindTarget( ioBaseChar *pOwner )
{
	if ( !pOwner || !pOwner->IsNeedProcess() )
		return false;
	
	ioPlayStage *pStage = pOwner->GetCreator();
	if ( !pStage )
		return false;

	const BaseCharList &rkCharList = pStage->GetBaseCharList();
	BaseCharList::const_iterator iter;
	
	D3DXQUATERNION qtRot = GetWorldOrientation();
	D3DXVECTOR3 vDir = qtRot * D3DXVECTOR3( 0.0f, 0.0f, -FLOAT1 );
	D3DXVECTOR3 vCurPos = GetWorldPosition();
	
	float fSensingRangeSq = m_fSensingRange * m_fSensingRange;
	float fSensingHalfAnigle = m_fSensingAngle * FLOAT05;
	fSensingHalfAnigle = ioMath::ArrangeHead(fSensingHalfAnigle);
	float fHalfValue = cosf( DEGtoRAD(fSensingHalfAnigle) );
	
	float fCurGap = -FLOAT1;
	ioHashString szTarget;
	for( iter=rkCharList.begin() ; iter!=rkCharList.end() ; ++iter )
	{
		ioBaseChar *pTarget = *iter;
		if( !pTarget )
			continue;

		if( pOwner->GetCharName() == pTarget->GetCharName() )
			continue;

		if( pOwner->GetTeam() == pTarget->GetTeam() )
			continue;

		if( pTarget->IsSystemState() )
			continue;

		if( !pTarget->IsEnableTarget() )
			continue;

		if( !pTarget->CheckTargetWoundType( m_TargetWoundType ) )
			continue;

		
		D3DXVECTOR3 vTargetPos = pTarget->GetWorldPosition();
		D3DXVECTOR3 vDiff = vTargetPos - vCurPos;
		float fDiffRangeSq = D3DXVec3LengthSq( &vDiff );
		if( m_fSensingAngle != 0.0f )
		{
			D3DXVec3Normalize( &vDiff, &vDiff );

			float fCurValue = D3DXVec3Dot( &vDir, &vDiff );
			if( fCurValue < fHalfValue )
				continue;
		}

		// 芭府 眉农
		if( fDiffRangeSq < fSensingRangeSq )
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

	if ( szTarget.IsEmpty() )
		return false;

	m_szTargetName = szTarget;
	return true;
}

void ioTargetTrace14Dummy::SetTargetTraceState( ioBaseChar *pOwner )
{
	SetLoopAni( m_szTraceMoveAni, m_fTraceMoveAniRate );
	m_TargetTraceState = TS_TARGET_TRACE;

	if ( pOwner->IsNeedSendNetwork() )
	{
		SP2Packet kPacket( CUPK_DUMMY_CHAR_SYNC );
		kPacket << m_OwnerName;
		kPacket << m_iCreateIndex;
		kPacket << static_cast<BYTE>(m_TargetTraceState);
		kPacket << GetWorldOrientation();
		kPacket << GetWorldPosition();
		kPacket << m_szTargetName;
		P2PNetwork::SendToAllPlayingUser( kPacket );
	}
}

void ioTargetTrace14Dummy::SetTargetTraceState( ioBaseChar *pOwner, ioHashString szTargetName )
{
	m_szTargetName = szTargetName;
	SetLoopAni( m_szTraceMoveAni, m_fTraceMoveAniRate );
	m_TargetTraceState = TS_TARGET_TRACE;

	if ( pOwner->IsNeedSendNetwork() )
	{
		SP2Packet kPacket( CUPK_DUMMY_CHAR_SYNC );
		kPacket << m_OwnerName;
		kPacket << m_iCreateIndex;
		kPacket << static_cast<BYTE>(m_TargetTraceState);
		kPacket << GetWorldOrientation();
		kPacket << GetWorldPosition();
		kPacket << m_szTargetName;
		P2PNetwork::SendToAllPlayingUser( kPacket );
	}
}

void ioTargetTrace14Dummy::ProcessTargetTraceState( ioBaseChar *pOwner, float fTimerPerSec )
{
	if ( !CheckTarget( pOwner ) )
	{
		SetDieState( true );
		return;
	}

	if ( CheckTargetAttackState( pOwner ) )
	{
		if( m_iAttackCount == 0 )
			SetTargetFirstAttackState( pOwner );
		else if( m_iAttackCount == 1 )
			SetTargetSecondAttackState( pOwner );
		return;
	}

	ProcessTargetMoveState( pOwner, fTimerPerSec );
}

bool ioTargetTrace14Dummy::CheckTarget( ioBaseChar *pOwner )
{
	if ( !pOwner )
		return false;

	ioBaseChar *pTarget = pOwner->GetBaseChar( m_szTargetName );
	if ( !pTarget )
		return false;

	return true;
}

bool ioTargetTrace14Dummy::CheckTargetAttackState( ioBaseChar *pOwner )
{
	if ( !pOwner || !pOwner->IsNeedProcess() )
		return false;

	ioBaseChar *pTarget = pOwner->GetBaseChar( m_szTargetName );
	if ( pTarget )
	{
		D3DXVECTOR3 vDiff = pTarget->GetMidPositionByRate() - GetMidPositionByRate();
		if ( D3DXVec3LengthSq( &vDiff ) <= ( m_fTargetArriveDist * m_fTargetArriveDist ) )
			return true;
	}

	return false;
}

void ioTargetTrace14Dummy::ProcessTargetMoveState( ioBaseChar *pOwner, float fTimerPerSec )
{
	if ( !pOwner )
		return;

	D3DXVECTOR3 vMoveDir = GetWorldOrientation() * D3DXVECTOR3( 0.0f, 0.0f, -FLOAT1 );
	ioBaseChar *pTarget = pOwner->GetBaseChar( m_szTargetName );
	if ( pTarget )
		vMoveDir = pTarget->GetMidPositionByRate() - GetMidPositionByRate();
	D3DXVec3Normalize( &vMoveDir, &vMoveDir );

	D3DXQUATERNION qtTargetRot = Help::ConvertDirToQuaternion( vMoveDir );
	SetWorldOrientationAndDir( qtTargetRot );

	Translate( vMoveDir * ( m_fTraceTargetMoveSpeed * fTimerPerSec ) );
}

void ioTargetTrace14Dummy::SetTargetFirstAttackState( ioBaseChar *pOwner )
{
	if ( !pOwner )
		return;

	m_TargetTraceState = TS_TARGET_FIRST_ATTACK;

	SetActionAni( pOwner, m_FirstAttack );
	m_iAttackCount++;

	if ( pOwner->IsNeedSendNetwork() )
	{
		SP2Packet kPacket( CUPK_DUMMY_CHAR_SYNC );
		kPacket << m_OwnerName;
		kPacket << m_iCreateIndex;
		kPacket << static_cast<BYTE>(m_TargetTraceState);
		kPacket << GetWorldOrientation();
		kPacket << GetWorldPosition();
		P2PNetwork::SendToAllPlayingUser( kPacket );
	}
}

void ioTargetTrace14Dummy::ProcessTargetFirstAttackState( ioBaseChar *pOwner )
{
	if ( !pOwner )
		return;

	if ( pOwner->IsNeedProcess() && m_dwAttackAniEndTime < FRAMEGETTIME() )
	{
		SetTargetRotateState( pOwner );
		return;
	}

	if ( !CheckTarget( pOwner ) )
	{
		SetDieState( true );
		return;
	}

	SetTargetLook( pOwner );
}

void ioTargetTrace14Dummy::SetTargetLook( ioBaseChar *pOwner )
{
	D3DXVECTOR3 vMoveDir = GetWorldOrientation() * D3DXVECTOR3( 0.0f, 0.0f, -FLOAT1 );
	ioBaseChar *pTarget = pOwner->GetBaseChar( m_szTargetName );
	if ( pTarget )
		vMoveDir = pTarget->GetMidPositionByRate() - GetMidPositionByRate();
	
	D3DXVec3Normalize( &vMoveDir, &vMoveDir );

	D3DXQUATERNION qtTargetRot = Help::ConvertDirToQuaternion( vMoveDir );
	SetWorldOrientationAndDir( qtTargetRot );
}

void ioTargetTrace14Dummy::SetTargetRotateState( ioBaseChar *pOwner )
{
	if ( !pOwner )
	{
		SetDieState( true );
		return;
	}

	ioBaseChar *pTarget = pOwner->GetBaseChar( m_szTargetName );
	if ( !pTarget )
	{
		SetDieState( true );
		return;
	}

	m_TargetTraceState = TS_TARGET_ROTATE;

	m_fCurTargetRotateAngleMove = 0.0f;

	if( pOwner->IsNeedProcess() )
	{
		float fRoll;
		ioMath::QuaterToEuler( GetWorldOrientation(), m_fTargetRotateYaw, m_fCurTargetAttachAngle, fRoll );
		m_fCurTargetAttachAngle = ioMath::ArrangeHead( RADtoDEG( m_fCurTargetAttachAngle ) );
		m_vTargetCenterPos = pTarget->GetMidPositionByRate();
		D3DXVECTOR3 vMoveDir = GetMidPositionByRate() - pTarget->GetMidPositionByRate();
		m_fTargetRotateLength = D3DXVec3Length( &vMoveDir );
	}
	
	if ( pOwner->IsNeedSendNetwork() )
	{
		SP2Packet kPacket( CUPK_DUMMY_CHAR_SYNC );
		kPacket << m_OwnerName;
		kPacket << m_iCreateIndex;
		kPacket << static_cast<BYTE>(m_TargetTraceState);
		kPacket << GetWorldOrientation();
		kPacket << GetWorldPosition();
		kPacket << m_fTargetRotateYaw;
		kPacket << m_fCurTargetAttachAngle;
		kPacket << m_vTargetCenterPos;
		kPacket << m_fTargetRotateLength;
		P2PNetwork::SendToAllPlayingUser( kPacket );
	}

	
}

void ioTargetTrace14Dummy::ProcessTargetRotateState( ioBaseChar *pOwner )
{
	if ( pOwner->IsNeedProcess() && m_fCurTargetRotateAngleMove > m_fTargetRotateMaxAngleMove )
	{
		SetSensingState( pOwner );
		return;
	}
	CheckTargetRotatePos( pOwner );
}

void ioTargetTrace14Dummy::SetLoopAni( const ioHashString& szLoopAni, float fTimeRate )
{
	if ( !m_pGroup )
		return;

	int nAniID = m_pGroup->GetAnimationIdx( szLoopAni );
	if( nAniID == -1 )
		return;

	if ( fTimeRate <= 0.0f )
		fTimeRate = FLOAT1;
	m_pGroup->ClearAllActionAni( FLOAT10, true );
	m_pGroup->ClearAllLoopAni( FLOAT10, true );
	m_pGroup->SetLoopAni( nAniID, FLOAT100, FLOAT1, FLOAT1/fTimeRate );
}

void ioTargetTrace14Dummy::SetActionAni( ioBaseChar *pOwner, const AttackAttribute& rkCurAttribute )
{
	int iAniID = m_pGroup->GetAnimationIdx( rkCurAttribute.m_AttackAnimation );
	if ( iAniID == -1 )
		return;

	DWORD dwCurTime = FRAMEGETTIME();
	m_pGroup->ClearAllActionAni( FLOAT10, true );
	m_pGroup->ClearAllLoopAni( FLOAT10, true );

	float fAniRate = rkCurAttribute.m_fAttackAniRate;
	if( fAniRate <= 0.0f )
		fAniRate = FLOAT1;
		
	m_pGroup->SetActionAni( iAniID, FLOAT10, FLOAT10, FLOAT1, FLOAT1/fAniRate, 0.0f, 0.0f, true );

	//Ani 贸府
	DWORD dwStartTime = dwCurTime + rkCurAttribute.m_dwPreDelay;
	m_dwAttackAniEndTime = dwStartTime + m_pGroup->GetAnimationFullTime( iAniID ) * fAniRate + rkCurAttribute.m_dwEndDelay;

	// fire 贸府
	AniEventConstPtrList vFireStartList;
	m_pGroup->GetAnimationEventConstPtrList_e( iAniID, "fire_s", vFireStartList );

	std::list<float> vFireEndList;
	m_pGroup->GetAnimationEventTimeList_e( iAniID, "fire_e", vFireEndList );

	const WeaponInfoList &vInfoList = rkCurAttribute.m_vWeaponInfoList;
	if( vFireStartList.size() != vInfoList.size() )
		return;

	m_dwBaseWeaponIndex = pOwner->IncreaseWeaponIndexBase();
		
	int i=0;
	AniEventConstPtrList::iterator iter;

	std::list< float >::iterator iterEndTime = vFireEndList.begin();
	for( iter=vFireStartList.begin() ; iter!=vFireStartList.end() ; ++iter, i++ )
	{
		float fFireStart = (*iter)->fEventTime;

		FireTime kFireTime;
		kFireTime.dwStart = dwStartTime + fFireStart * fAniRate + vInfoList[i].m_dwCallTime;

		if( iterEndTime != vFireEndList.end() )
		{
			kFireTime.dwDuration = ( *iterEndTime - fFireStart ) * fAniRate;
			++iterEndTime;
		}

		kFireTime.iAttributeIdx  = vInfoList[i].m_iWeaponIdx;
		kFireTime.iResistanceIdx = vInfoList[i].m_iResistanceIdx;
		kFireTime.szWoundedAni   = vInfoList[i].m_WoundedAnimation;
		kFireTime.dwWoundedAniDuration  = vInfoList[i].m_dwWoundedDuration;
		kFireTime.bLoopWoundedAni = vInfoList[i].m_bWoundedLoopAni;

		kFireTime.dwWeaponIdx = m_dwBaseWeaponIndex + i;

		if( !(*iter)->szSubInfo.IsEmpty() )
		{
			const StringVector &vParam = ioStringConverter::Split( (*iter)->szSubInfo.c_str(), " " );
			if( vParam.size() >= 1 )
			{
				ioHashString szTypeName = vParam[0].c_str();
				kFireTime.eCollisionType = GetAniCollisionType( szTypeName );

				if( vParam.size() == 2 && kFireTime.eCollisionType == ACT_SPECIAL )
				{
					kFireTime.szSpecialCollisionBox = vParam[1].c_str();
				}
			}
		}

		kFireTime.eFireTimeType = FTT_DUMMY_CHAR_ATTACK;
		kFireTime.m_iExtraValue = m_iCreateIndex;

		m_vAttackFireTimeList.push_back( kFireTime );
	}

	SetReservedSliding( rkCurAttribute.m_vForceInfoList, iAniID, fAniRate, rkCurAttribute.m_dwPreDelay );
	// 扁鸥 贸府

	m_dwExtraAniJumpTime = 0;
	m_fExtraAniJumpPower = 0.0f;
	m_vExtraAniJumpList.clear();

	AniEventConstPtrList vExtraAniJumpList;
	m_pGroup->GetAnimationEventConstPtrList( iAniID, "extra_ani_jump", vExtraAniJumpList );

	AniEventConstPtrList::iterator iter_event = vExtraAniJumpList.begin();
	while( iter_event != vExtraAniJumpList.end() )
	{
		ExtraAniJump kInfo;

		kInfo.m_dwTime = (*iter_event)->fEventTime * fAniRate;
		kInfo.m_dwTime += dwCurTime;

		if( !(*iter_event)->szSubInfo.IsEmpty() )
		{
			const StringVector &vParam = ioStringConverter::Split( (*iter_event)->szSubInfo.c_str(), " " );
			if( vParam.size() == 1 )
			{
				kInfo.m_fPower = ioStringConverter::ParseFloat( vParam[0].c_str() );
			}
		}

		m_vExtraAniJumpList.push_back(kInfo);

		++iter_event;
	}

	if( !m_vExtraAniJumpList.empty() )
	{
		m_iCurExtraAniJump = 0;
		m_dwExtraAniJumpTime = m_vExtraAniJumpList[m_iCurExtraAniJump].m_dwTime;
		m_fExtraAniJumpPower = m_vExtraAniJumpList[m_iCurExtraAniJump].m_fPower;
	}
}

void ioTargetTrace14Dummy::ProcessAttackState( float fTimePerSec, DWORD dwPreTime )
{
	if ( m_TargetTraceState != TS_TARGET_FIRST_ATTACK && m_TargetTraceState != TS_TARGET_SECOND_ATTACK )
		return;

	ioBaseChar *pOwner = GetOwner();
	if( !pOwner )
		return;

	DWORD dwCurTime = FRAMEGETTIME();

	D3DXVECTOR3 vPos = GetMidPositionByRate();
	D3DXVECTOR3 vDir = GetWorldOrientation() * D3DXVECTOR3( 0.0f, 0.0f, -FLOAT1 );

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
				ioWeapon *pWeapon = NULL;

				ioBaseChar *pTarget = pOwner->GetBaseChar( m_szTargetName );
				if ( pTarget )
					vDir = pTarget->GetMidPositionByRate() - vPos;
				D3DXVec3Normalize( &vDir, &vDir );
				
				pWeapon = pOwner->ExtendFireExplicit( rkFire, vPos, vDir, "" );

				
				if ( pWeapon )
				{
					pWeapon->SetPosition( vPos );
					pWeapon->SetMoveDir( vDir );
				}
			}

			iter = m_vAttackFireTimeList.erase( iter );
		}
		else
		{
			++iter;
		}
	}
}

bool ioTargetTrace14Dummy::CheckTargetRotatePos( ioBaseChar *pOwner )
{
	if ( m_vTargetCenterPos == ioMath::VEC3_ZERO )
		return false;

	if ( !pOwner )
		return false;

	if ( m_fTargetRotateSpeed == 0 )
		return false;

	float fFrameGap = g_FrameTimer.GetCurFrameGap();
	float fRate = fFrameGap / m_fTargetRotateSpeed;
	float fRotateAngle = 360.0f * fRate;
	m_fCurTargetAttachAngle -= fRotateAngle;
	m_fCurTargetAttachAngle = ioMath::ArrangeHead(m_fCurTargetAttachAngle);
	float fPitch = DEGtoRAD( m_fCurTargetAttachAngle );

	m_fCurTargetRotateAngleMove += fRotateAngle;

	D3DXQUATERNION qtRotate;
	D3DXQuaternionRotationYawPitchRoll( &qtRotate, m_fTargetRotateYaw, fPitch, 0.0f );

	D3DXVECTOR3 vOffDir = qtRotate * D3DXVECTOR3( 0.0f, 0.0f, FLOAT1 );
	D3DXVec3Normalize( &vOffDir, &vOffDir );

	D3DXVECTOR3 vNewPos = m_vTargetCenterPos + vOffDir * m_fTargetRotateLength;;
	SetWorldPosition( vNewPos );

	D3DXVECTOR3 vTargetDir = m_vTargetCenterPos - vNewPos;
	D3DXVec3Normalize( &vTargetDir, &vTargetDir );
	D3DXQUATERNION qtRot;
	ioMath::CalcDirectionQuaternion( qtRot, -vTargetDir );
	SetWorldOrientation( qtRot );

	return true;
}

void ioTargetTrace14Dummy::SetSensingState( ioBaseChar *pOwner )
{
	m_TargetTraceState = TS_TARGET_SENSING_STATE;
	m_szTargetName.Clear();
	m_dwSecondTargetSensingStartTime = FRAMEGETTIME();

	if ( pOwner->IsNeedSendNetwork() )
	{
		SP2Packet kPacket( CUPK_DUMMY_CHAR_SYNC );
		kPacket << m_OwnerName;
		kPacket << m_iCreateIndex;
		kPacket << static_cast<BYTE>(m_TargetTraceState);
		kPacket << GetWorldOrientation();
		kPacket << GetWorldPosition();
		P2PNetwork::SendToAllPlayingUser( kPacket );
	}
}

void ioTargetTrace14Dummy::ProcessTargetSensingState( ioBaseChar *pOwner )
{
	if ( !pOwner || !pOwner->IsNeedProcess() )
		return;

	if( m_dwSecondTargetSensingTime && m_dwSecondTargetSensingStartTime + m_dwSecondTargetSensingTime < FRAMEGETTIME() )
	{
		SetDieState( true );
		return;
	}

	if ( FindTarget( pOwner ) )
	{
		SetTargetTraceState( pOwner );
		return;
	}
}

void ioTargetTrace14Dummy::SetTargetSecondAttackState( ioBaseChar *pOwner )
{
	m_TargetTraceState = TS_TARGET_SECOND_ATTACK;

	SetActionAni( pOwner, m_SecondAttack );
	m_iAttackCount++;

	if ( pOwner->IsNeedSendNetwork() )
	{
		SP2Packet kPacket( CUPK_DUMMY_CHAR_SYNC );
		kPacket << m_OwnerName;
		kPacket << m_iCreateIndex;
		kPacket << static_cast<BYTE>(m_TargetTraceState);
		kPacket << GetWorldOrientation();
		kPacket << GetWorldPosition();
		P2PNetwork::SendToAllPlayingUser( kPacket );
	}
}

void ioTargetTrace14Dummy::ProcessTargetSecondAttackState( ioBaseChar *pOwner )
{
	if ( !pOwner )
		return;

	if ( m_dwAttackAniEndTime < FRAMEGETTIME() )
	{
		SetDieState( true );
		return;
	}

	if ( !CheckTarget( pOwner ) )
	{
		SetDieState( true );
		return;
	}

	SetTargetLook( pOwner );
}

void ioTargetTrace14Dummy::ApplyDummyCharSync( ioPlayStage *pStage, SP2Packet &rkPacket )
{
	if( !pStage ) 
		return;	

	ioBaseChar *pOwner = GetOwner();
	if ( !pOwner )
		return;

	BYTE btType;
	rkPacket >> btType;

	D3DXQUATERNION qtRot;
	D3DXVECTOR3 vPos;
	rkPacket >> qtRot;
	rkPacket >> vPos;
	SetWorldOrientation( qtRot );
	SetWorldPosition( vPos );

	if ( btType == TS_OWNER_ROTATE_MOVE )
	{
		rkPacket >> m_fCurOwnerAttachAngle;
		SetOwnerRotateMoveState( pOwner );
	}
	else if ( btType == TS_TARGET_TRACE )
	{
		rkPacket >> m_szTargetName;
		SetTargetTraceState( pOwner );
	}
	else if ( btType == TS_TARGET_FIRST_ATTACK )
		SetTargetFirstAttackState( pOwner );
	else if ( btType == TS_TARGET_ROTATE )
	{
		rkPacket >> m_fTargetRotateYaw;
		rkPacket >> m_fCurTargetAttachAngle;
		rkPacket >> m_vTargetCenterPos;
		rkPacket >> m_fTargetRotateLength;
		SetTargetRotateState( pOwner );
	}
	else if ( btType == TS_TARGET_SENSING_STATE )
		SetSensingState( pOwner );
	else if ( btType == TS_TARGET_SECOND_ATTACK )
		SetTargetSecondAttackState( pOwner );
}
