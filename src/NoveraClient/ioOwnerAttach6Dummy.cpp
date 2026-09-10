
#include "stdafx.h"

#include "ioOwnerAttach6Dummy.h"
#include "WeaponDefine.h"
#include "BuffDefine.h"

ioOwnerAttach6Dummy::ioOwnerAttach6Dummy( ioEntityGroup *pGrp, ioPlayMode *pMode )
	: ioDummyChar( pGrp, pMode )
{
	m_AttachState = AS_None;
	m_dwFireStart = 0;
	m_dwAttackEndTime = 0;
	m_bCreateWeapon = false;
}

ioOwnerAttach6Dummy::~ioOwnerAttach6Dummy()
{
}

void ioOwnerAttach6Dummy::LoadProperty( ioINILoader &rkLoader )
{
	ioDummyChar::LoadProperty( rkLoader );

	LoadInitProperty( rkLoader );
}

void ioOwnerAttach6Dummy::LoadInitProperty( ioINILoader &rkLoader )
{
	m_fRunMoveSpeed = rkLoader.LoadFloat_e( "run_move_speed", 0.0f );
	
	char szBuf[MAX_PATH];

	m_vTargetOffset.x = rkLoader.LoadFloat_e( "target_offset_x", 0.0f );
	m_vTargetOffset.y = rkLoader.LoadFloat_e( "target_offset_y", 0.0f );
	m_vTargetOffset.z = rkLoader.LoadFloat_e( "target_offset_z", 0.0f );

	m_vWeaponOffset.x = rkLoader.LoadFloat_e( "weapon_offset_x", 0.0f );
	m_vWeaponOffset.y = rkLoader.LoadFloat_e( "weapon_offset_y", 0.0f );
	m_vWeaponOffset.z = rkLoader.LoadFloat_e( "weapon_offset_z", 0.0f );

	rkLoader.LoadString_e( "attack_animation", "", szBuf, MAX_PATH );
	m_szAttackAnimation = szBuf;

	m_fAttackAniRate = rkLoader.LoadFloat_e( "attack_ani_rate", FLOAT1 );	

	//타겟 설정
	m_fColAngle = rkLoader.LoadFloat_e( "target_angle", 0.0f );
	m_fColRange = rkLoader.LoadFloat_e( "target_range", 0.0f );

	m_nAttackWeaponNum = rkLoader.LoadInt_e( "attack_weapon_num", NO_WEAPON_ATTRIBUTE );

	rkLoader.LoadString_e( "search_add_buff", "", szBuf, MAX_PATH );
	m_stSearchAddBuff = szBuf;
	rkLoader.LoadString_e( "search_avoid_buff", "", szBuf, MAX_PATH );
	m_stSearchAvoidBuff = szBuf;

	m_fAttackDistance = rkLoader.LoadFloat( "attack_distance", 0.f );

	m_dwMaxDelayTwoTime = rkLoader.LoadInt( "attack_after_search_time", 100000 );
	m_dwMaxTraceTime = rkLoader.LoadInt( "chase_reset_max_time", 0 );
}

void ioOwnerAttach6Dummy::ProcessState( float fTimePerSec, DWORD dwPreTime )
{
	ioBaseChar *pOwner = GetOwner();
	if( !pOwner )
	{
		SetDieState( true );
		return;
	}

	switch( m_AttachState )
	{
	case AS_None:
		SetDelayState( pOwner );
		break;
	case AS_Delay:
		ProcessDelayState( pOwner, fTimePerSec );
		break;
	case AS_Trace:
		ProcessTrace( pOwner, fTimePerSec );
		break;
	case AS_Delay2:
		ProcessDelayTwoState( pOwner, fTimePerSec );
		break;
	case AS_Attack:
		ProcessAttackState( pOwner, fTimePerSec );
		break;
	case AS_Warp:
		SetDelayState( pOwner );
		break;
	}

	if( pOwner->IsNeedProcess() && m_dwLifeTime && (DWORD)(m_dwStartTime + m_dwLifeTime) < FRAMEGETTIME() && m_AttachState != AS_Attack )
		SendDieState( true );
}

ApplyWeaponExp ioOwnerAttach6Dummy::ApplyWeapon( ioWeapon *pWeapon )
{
	ioBaseChar *pAttacker = pWeapon->GetOwner();
	if( !pAttacker )
	{
		LOG.PrintTimeAndLog( 0, "ioOwnerAttach6Dummy::ApplyWeapon - %s Not Exist", pWeapon->GetOwnerName().c_str() );
		return AWE_EXPLOSION;
	}

	if( pWeapon->IsNoWoundedWeapon() )
		return AWE_NO_EXPLOSION;

	m_LastAttacker = pWeapon->GetOwnerName();

	float fPreCurHP = m_HP.m_fCurValue;
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

void ioOwnerAttach6Dummy::ApplyDummyCharSync( ioPlayStage *pStage, SP2Packet &rkPacket )
{
	if( !pStage ) return;

	int iType;
	rkPacket >> iType;

	switch( iType )
	{
	case DUMMY_CHAR_SYNC:
		{
			int nLocalType;
			rkPacket >> nLocalType;
			if( nLocalType == AS_Trace )
			{
				ioHashString stTargetName;
				rkPacket >> stTargetName;

				D3DXVECTOR3 vPos;
				D3DXQUATERNION qt;
				rkPacket >> vPos;
				rkPacket >> qt;

				SetWorldPosition( vPos );
				SetWorldOrientation( qt );

				SetTrace( GetOwner(), stTargetName );
				break;
			}
			else if( nLocalType == AS_Warp )
			{
				SetWarp( GetOwner() );
				break;
			}
			else if( nLocalType == AS_Attack )
			{
				D3DXVECTOR3 vPos;
				D3DXQUATERNION qt;

				rkPacket >> vPos;
				rkPacket >> qt;

				SetWorldPosition( vPos );
				SetWorldOrientation( qt );

				SetAttackState( GetOwner() );
				break;
			}
			else if( nLocalType == AS_Delay2 )
			{
				D3DXVECTOR3 vPos;
				D3DXQUATERNION qt;

				rkPacket >> vPos;
				rkPacket >> qt;

				SetWorldPosition( vPos );
				SetWorldOrientation( qt );

				SetDelayTwoState( GetOwner() );
				break;
			}
		}
		break;
	default:
		ioDummyChar::ApplyDummyCharSyncByType( iType, rkPacket, pStage );
	}
}

void ioOwnerAttach6Dummy::SetDieState( bool bEffect, bool bWoundedDie /*= false */ )
{
	ioDummyChar::SetDieState( bEffect, bWoundedDie );
}

bool ioOwnerAttach6Dummy::IsActivateWeapon()
{
	if( m_AttachState != AS_Attack )
		return false;

	return true;
}

void ioOwnerAttach6Dummy::SetTrace( ioBaseChar *pOwner, ioHashString stTarget )
{
	m_szTargetName = stTarget;
	ioBaseChar* pTaget = pOwner->GetBaseChar( m_szTargetName );
	if( !pTaget )
	{
		SetWarp( pOwner );
		//워프
		return;
	}

	m_dwTraceStartTime = FRAMEGETTIME();

	pTaget->AddNewBuff( m_stSearchAddBuff, pOwner->GetCharName(), "", NULL );
	m_AttachState = AS_Trace;

	int iAniID = m_pGroup->GetAnimationIdx( m_MoveAnimation );
	if( iAniID != -1 )
	{
		m_pGroup->ClearAllActionAni( FLOAT10, true );
		m_pGroup->ClearAllLoopAni( FLOAT10, true );

		float fAniRate = FLOAT1;
		if( m_fMoveAniRate > 0.0f )
			fAniRate = m_fMoveAniRate;

		m_pGroup->SetLoopAni( iAniID, FLOAT10, FLOAT1, FLOAT1/fAniRate );
	}

	//sendpacket
	if( pOwner->IsNeedProcess() )
	{
		SP2Packet kPacket( CUPK_DUMMY_CHAR_SYNC );
		kPacket << m_OwnerName;
		kPacket << m_iCreateIndex;
		kPacket << DUMMY_CHAR_SYNC;
		kPacket << AS_Trace;
		kPacket << m_szTargetName;
		kPacket << GetWorldPosition();
		kPacket << GetWorldOrientation();
		P2PNetwork::SendToAllPlayingUser( kPacket );
	}
}

void ioOwnerAttach6Dummy::SetWarp( ioBaseChar *pOwner )
{
	m_AttachState = AS_Warp;

	ioMapEffect *pEffect = m_pModeParent->GetCreator()->CreateMapEffect( m_DestroyEffect,
		GetWorldPosition(),
		ioMath::UNIT_ALL );

	if( pEffect )
	{
		pEffect->SetWorldOrientation( GetWorldOrientation() );
	}

	//SendPacket
	if( pOwner->IsNeedProcess() )
	{
		SP2Packet kPacket( CUPK_DUMMY_CHAR_SYNC );
		kPacket << m_OwnerName;
		kPacket << m_iCreateIndex;
		kPacket << DUMMY_CHAR_SYNC;
		kPacket << AS_Warp;
		P2PNetwork::SendToAllPlayingUser( kPacket );
	}
}

void ioOwnerAttach6Dummy::ProcessTrace( ioBaseChar *pOwner, float fTimePerSec )
{
	ioPlayStage *pStage = pOwner->GetCreator();
	if( !pStage ) 
		return;

	DWORD dwCurTime = FRAMEGETTIME();

	ioBaseChar* pTarget = pOwner->GetBaseChar( m_szTargetName );
	if( pOwner->IsNeedProcess() && !pTarget )
	{
		SetWarp( pOwner );
		//워프
		return;
	}

	if( pOwner->IsNeedProcess() && !pTarget->HasBuff( pOwner->GetCharName(), m_stSearchAddBuff) )
	{
		SetWarp( pOwner );
		//워프
		return;
	}

	if( pOwner->IsNeedProcess() && !CheckOutRange(pOwner) )
	{
		SetWarp( pOwner );
		//워프
		return;
	}

	if( pOwner->IsNeedProcess() && m_dwMaxTraceTime && (m_dwTraceStartTime+m_dwMaxTraceTime) <  dwCurTime )
	{
		SetWarp( pOwner );
		//워프
		return;
	}

	if( !pTarget )
		return;

	m_vTracingMoveDir = pTarget->GetWorldPosition() - GetWorldPosition();
	D3DXVec3Normalize( &m_vTracingMoveDir, &m_vTracingMoveDir );

	float fMoveDist = m_fRunMoveSpeed * fTimePerSec;
	float fLength = 0.f;
	D3DXVECTOR3 vSrc = GetWorldPosition();
	D3DXVECTOR3 vDest = pTarget->GetWorldPosition();

	D3DXVECTOR3 vDiffLength = vDest - vSrc;
	fLength = D3DXVec3Length( &vDiffLength );

	D3DXVECTOR3 vQtVec = m_vTracingMoveDir;
	vQtVec.y = 0;
	D3DXVec3Normalize( &vQtVec, &vQtVec );

	D3DXQUATERNION	qtRot;
	ioMath::CalcDirectionQuaternion( qtRot, -vQtVec );
	SetWorldOrientation( qtRot );

	if( fLength < m_fAttackDistance )
	{
		if( pOwner->IsNeedProcess() )
			SetAttackState( pOwner );
	}
	else
		ProcessTraceMovingNoneTerrain( pStage, fMoveDist, fTimePerSec );
}

void ioOwnerAttach6Dummy::SetDelayState( ioBaseChar *pOwner )
{
	if ( !pOwner )
		return;

	if( m_AttachState == AS_Delay )
		return;

	if( !m_DummyEffect.IsEmpty() )
	{
		m_pGroup->EndEffect( m_DummyEffect, true );

		m_pGroup->AttachEffect( m_DummyEffect, NULL );
		m_bMoveEffectCreated = true;
	}

	m_szTargetName.Clear();

	if( !m_DelayAnimation.IsEmpty() )
	{
		int iAniID = m_pGroup->GetAnimationIdx( m_DelayAnimation );
		if( iAniID != -1 )
		{
			m_pGroup->ClearAllActionAni( FLOAT10, true );
			m_pGroup->ClearAllLoopAni( FLOAT10, true );

			float fAniRate = m_fMoveAniRate;

			m_pGroup->SetLoopAni( iAniID, FLOAT100, FLOAT1, FLOAT1/fAniRate );
		}
	}

	m_AttachState = AS_Delay;
}

void ioOwnerAttach6Dummy::ProcessDelayState( ioBaseChar *pOwner, float fTimePerSec )
{	
	if ( !pOwner )
		return;

	SetOffsetPosAndDir( pOwner );

	if( pOwner->IsNeedProcess() )
	{
		if( FindTarget( pOwner ) )
		{
			SetTrace( pOwner, m_szTargetName );
			return;
		}
	}
}

void ioOwnerAttach6Dummy::SetDelayTwoState( ioBaseChar *pOwner )
{
	if ( !pOwner )
		return;

	if( m_AttachState == AS_Delay2 )
		return;

	m_szTargetName.Clear();

	if( !m_DelayAnimation.IsEmpty() )
	{
		int iAniID = m_pGroup->GetAnimationIdx( m_DelayAnimation );
		if( iAniID != -1 )
		{
			m_pGroup->ClearAllActionAni( FLOAT10, true );
			m_pGroup->ClearAllLoopAni( FLOAT10, true );

			float fAniRate = m_fMoveAniRate;

			m_pGroup->SetLoopAni( iAniID, FLOAT100, FLOAT1, FLOAT1/fAniRate );
		}
	}

	m_AttachState = AS_Delay2;

	m_dwDelayTwoStartTime = FRAMEGETTIME();

	//sendpacket
	if( pOwner->IsNeedProcess() )
	{
		SP2Packet kPacket( CUPK_DUMMY_CHAR_SYNC );
		kPacket << m_OwnerName;
		kPacket << m_iCreateIndex;
		kPacket << DUMMY_CHAR_SYNC;
		kPacket << AS_Delay2;
		kPacket << GetWorldPosition();
		kPacket << GetWorldOrientation();
		P2PNetwork::SendToAllPlayingUser( kPacket );
	}
}

void ioOwnerAttach6Dummy::ProcessDelayTwoState( ioBaseChar *pOwner, float fTimePerSec )
{
	DWORD dwCurTime = FRAMEGETTIME();
	if( pOwner->IsNeedProcess() &&  (m_dwDelayTwoStartTime + m_dwMaxDelayTwoTime) < dwCurTime )
	{
		SetWarp( pOwner );
		return;
	}

	if( pOwner->IsNeedProcess() && !CheckOutRange(pOwner) )
	{
		SetWarp( pOwner );
		//워프
		return;
	}

	if( pOwner->IsNeedProcess() )
	{
		if( FindTarget( pOwner ) )
		{
			SetTrace( pOwner, m_szTargetName );
			return;
		}
	}
}

void ioOwnerAttach6Dummy::SetAttackState( ioBaseChar *pOwner )
{
	if ( !pOwner )
		return;

	if( m_AttachState == AS_Attack )
		return;

	if( !m_szAttackAnimation.IsEmpty() )
	{
		int iAniID = m_pGroup->GetAnimationIdx( m_szAttackAnimation );
		if( iAniID != -1 )
		{
			float fTimeRate = m_fAttackAniRate;

			m_pGroup->ClearAllActionAni( FLOAT100, true );
			m_pGroup->ClearAllLoopAni( FLOAT100, true );
			m_pGroup->SetActionAni( iAniID, FLOAT100, FLOAT100, FLOAT1, FLOAT1/fTimeRate );

			DWORD dwCurTime = FRAMEGETTIME();
			m_dwAttackEndTime = dwCurTime;
			m_dwAttackEndTime += m_pGroup->GetAnimationFullTime(iAniID) * fTimeRate;

			m_dwFireStart = dwCurTime;
			m_dwFireStart += m_pGroup->GetAnimationEventTime_e( iAniID, "fire_s" ) * fTimeRate;
		}
	}

	m_AttachState = AS_Attack;

	//sendpacket
	if( pOwner->IsNeedProcess() )
	{
		SP2Packet kPacket( CUPK_DUMMY_CHAR_SYNC );
		kPacket << m_OwnerName;
		kPacket << m_iCreateIndex;
		kPacket << DUMMY_CHAR_SYNC;
		kPacket << AS_Attack;
		kPacket << GetWorldPosition();
		kPacket << GetWorldOrientation();
		P2PNetwork::SendToAllPlayingUser( kPacket );
	}
}

void ioOwnerAttach6Dummy::ProcessAttackState( ioBaseChar *pOwner, float fTimePerSec )
{
	if( !pOwner )
		return;

	DWORD dwCurTime = FRAMEGETTIME();

	CreateWeapon( pOwner );

	if ( m_dwAttackEndTime < dwCurTime )
	{
		SetDelayTwoState( pOwner );
		m_bCreateWeapon = false;
	}	
}

bool ioOwnerAttach6Dummy::FindTarget( ioBaseChar *pOwner )
{	
	if( !pOwner ) 
		return false;

	//타겟 리스트
	BaseCharList vTargetList;

	//위치 설정
	D3DXVECTOR3 vPos = GetWorldPosition();

	//타겟 리스트 얻기
	pOwner->GetTargetCharListBySphere( vPos, m_fColRange, m_fColAngle, vTargetList );

	//타겟 리스트가 비어있다면
	if ( vTargetList.empty() )
		return false;

	//타겟 리스트 소팅
	SortTargetList( pOwner, vTargetList );

	//회전
	if ( !m_SortList.empty() )
	{
		ioBaseChar *pTarget = m_SortList[0].pChar;
		if ( pTarget )
		{
			m_szTargetName = pTarget->GetCharName();
			return true;
		}
	}

	return false;
}

//타겟 리스트 가까운 순서로 정렬
void ioOwnerAttach6Dummy::SortTargetList( ioBaseChar *pOwner, const BaseCharList &TargetList )
{
	if( !pOwner )	
		return;

	m_SortList.erase( m_SortList.begin(), m_SortList.end() );
	m_SortList.reserve( TargetList.size() );

	D3DXVECTOR3 vDiff;
	D3DXVECTOR3 vOwnerPos = pOwner->GetWorldPosition();

	BaseCharList::const_iterator iter;
	for( iter=TargetList.begin(); iter!=TargetList.end(); ++iter )
	{
		ioBaseChar *pCurChar = *iter;
		if( pCurChar != pOwner )
		{
			//팀이 같거나 타겟이 될수 없으면 리스트에 저장 안함
			if ( pOwner->GetTeam() == pCurChar->GetTeam() )
				continue;

			if ( !IsEnableTarget( pCurChar ) )
				continue;

			//이미 공격당한 상태..
			if( pCurChar->HasBuff( 	pOwner->GetCharName(), m_stSearchAvoidBuff ) )
				continue;

			vDiff = pCurChar->GetWorldPosition() - vOwnerPos;
			float fCurDistSq = D3DXVec3LengthSq( &vDiff );

			DistInfo sDist;
			sDist.pChar = pCurChar;
			sDist.fDistSq = fCurDistSq;
			m_SortList.push_back( sDist );
		}
	}

	std::sort( m_SortList.begin(), m_SortList.end() );
}

//타겟 가능한지 확인
bool ioOwnerAttach6Dummy::IsEnableTarget( ioBaseChar *pTarget )
{
	if ( !pTarget || pTarget->HasHideBuff() || pTarget->IsPrisonerMode() )
		return false;

	if( pTarget->IsSystemState() )
		return false;

	if( !pTarget->IsEnableTarget() )
		return false;

	CharState eTargetState = pTarget->GetState();
	switch( eTargetState )
	{
	case CS_BLOW_WOUNDED:
		{
			if ( !pTarget->IsFloatingState() && !m_bSetDownAim )
				return false;
		}
	case CS_BOUND_BLOW_WOUNDED:
		{
			if ( !pTarget->IsFloatingState() && !m_bSetDownAim )
				return false;
		}
	case CS_ETC_STATE:
		{
			if ( pTarget->IsApplyDownState(false) && !m_bSetDownAim )
				return false;
		}
	case CS_USING_SKILL:
		{
			if ( pTarget->IsApplyDownState(false) && !m_bSetDownAim )
				return false;
		}
	}

	return true;
}

//타겟 방향으로 회전
void ioOwnerAttach6Dummy::SetRotateToTarget( ioBaseChar *pOwner )
{
	if ( !pOwner || m_szTargetName.IsEmpty() )
		return;

	//회전 타겟 얻기
	ioBaseChar *pTarget = pOwner->GetBaseChar( m_szTargetName );
	if ( pTarget )
	{
		//타겟 위치 얻기
		D3DXVECTOR3 vTargetPos = pTarget->GetWorldPosition();

		//타겟 방향 설정
		D3DXVECTOR3 vTargetDir = vTargetPos - GetWorldPosition();

		vTargetDir.y = 0.0f;
		D3DXVec3Normalize( &vTargetDir, &vTargetDir );		

		D3DXQUATERNION qtRot = Help::ConvertDirToQuaternion( vTargetDir );
		SetWorldOrientation( qtRot );
	}
}

void ioOwnerAttach6Dummy::SetOffsetPosAndDir( ioBaseChar *pOwner )
{
	if ( !pOwner )
		return;

	D3DXQUATERNION qtOwnerRot = pOwner->GetWorldOrientation();
	D3DXVECTOR3 vOnwerPos = pOwner->GetWorldPosition();

	D3DXVECTOR3 vDir = qtOwnerRot * D3DXVECTOR3( 0.0f, 0.0f, -FLOAT1 );
	D3DXVec3Normalize( &vDir, &vDir );

	D3DXVECTOR3 vSideDir;
	D3DXVec3Cross( &vSideDir, &ioMath::UNIT_Y, &vDir );
	D3DXVec3Normalize( &vSideDir, &vSideDir );

	D3DXVECTOR3 vPos;

	vPos = vOnwerPos;
	vPos += (m_vTargetOffset.z * vDir);	
	vPos += (m_vTargetOffset.x * vSideDir);	
	vPos += (m_vTargetOffset.y * D3DXVECTOR3( 0, 1, 0) );

	SetWorldPosition( vPos );
	SetWorldOrientation( qtOwnerRot );
}

void ioOwnerAttach6Dummy::CreateWeapon( ioBaseChar *pOwner )
{
	if ( !pOwner || m_bCreateWeapon )
		return;

	DWORD dwCurTime = FRAMEGETTIME();
	if ( m_dwFireStart > dwCurTime )
		return;

	m_dwBaseWeaponIndex = pOwner->GetWeaponIndexBase() + 1;

	FireTime kFireTime;
	kFireTime.dwStart = dwCurTime;

	kFireTime.iAttributeIdx = m_nAttackWeaponNum;
	kFireTime.iResistanceIdx = 0;
	kFireTime.szWoundedAni  = "";
	kFireTime.dwWoundedAniDuration = 0;
	kFireTime.bLoopWoundedAni = false;
	kFireTime.dwWeaponIdx = m_dwBaseWeaponIndex;
	kFireTime.eFireTimeType = FTT_DUMMY_CHAR_ATTACK;

	pOwner->SetWeaponIndexBase( m_dwBaseWeaponIndex );

	SetRotateToTarget( pOwner );

	D3DXVECTOR3 vPos = GetWorldPosition() + GetWorldOrientation() * m_vWeaponOffset;
	D3DXVECTOR3 vDir = GetAttackDir( pOwner, vPos );

	ioWeapon *pWeapon = pOwner->ExtendFireExplicit( kFireTime, vPos, vDir, "" );
	if ( pWeapon )
	{
		pWeapon->SetPosition( vPos );
	}

	m_bCreateWeapon = true;
}

D3DXVECTOR3 ioOwnerAttach6Dummy::GetAttackDir( ioBaseChar *pOwner, const D3DXVECTOR3 &vPos )
{
	if ( !pOwner || m_szTargetName.IsEmpty() )
		return ioMath::VEC3_ZERO;

	//회전 타겟 얻기
	ioBaseChar *pTarget = pOwner->GetBaseChar( m_szTargetName );
	if ( pTarget )
	{
		//타겟 위치 얻기
		D3DXVECTOR3 vTargetPos = pTarget->GetMidPositionByRate();

		//타겟 방향 설정
		D3DXVECTOR3 vTargetDir = vTargetPos - vPos;
		D3DXVec3Normalize( &vTargetDir, &vTargetDir );

		return vTargetDir;
	}

	return ioMath::VEC3_ZERO;
}

bool ioOwnerAttach6Dummy::CheckOutRange( ioBaseChar* pOwner )
{
	if( !pOwner )
		return false;	

	D3DXVECTOR3 vDiff = GetWorldPosition() - pOwner->GetWorldPosition();
	float fDistance = D3DXVec3Length( &vDiff );

	if( m_fControlOutRange < fDistance )
		return false;

	return true;
}