
#include "stdafx.h"

#include "ioOwnerAttach5Dummy.h"
#include "WeaponDefine.h"
#include "BuffDefine.h"

ioOwnerAttach5Dummy::ioOwnerAttach5Dummy( ioEntityGroup *pGrp, ioPlayMode *pMode )
: ioDummyChar( pGrp, pMode )
{
	m_AttachState = AS_None;
	m_dwFireStart = 0;
	m_dwAttackEndTime = 0;
	m_bAutoFollow = false;
	m_bCreateWeapon = false;
	m_dwCheckRefreshTime = 0;
}

ioOwnerAttach5Dummy::~ioOwnerAttach5Dummy()
{
}

void ioOwnerAttach5Dummy::LoadProperty( ioINILoader &rkLoader )
{
	ioDummyChar::LoadProperty( rkLoader );
	
	LoadInitProperty( rkLoader );
}

void ioOwnerAttach5Dummy::LoadInitProperty( ioINILoader &rkLoader )
{
	m_fRunMoveSpeed = rkLoader.LoadFloat_e( "run_move_speed", 0.0f );
	m_fDashMoveSpeed = rkLoader.LoadFloat_e( "dash_move_speed", 0.0f );

	m_fFollowRange = rkLoader.LoadFloat_e( "follow_range", 0.0f ); 

	char szBuf[MAX_PATH], szKey[MAX_PATH];
	rkLoader.LoadString_e( "check_buff", "", szBuf, MAX_PATH );
	m_szCheckBuff = szBuf;

	m_vTargetOffset.x = rkLoader.LoadFloat_e( "target_offset_x", 0.0f );
	m_vTargetOffset.y = rkLoader.LoadFloat_e( "target_offset_y", 0.0f );
	m_vTargetOffset.z = rkLoader.LoadFloat_e( "target_offset_z", 0.0f );

	m_vWeaponOffset.x = rkLoader.LoadFloat_e( "weapon_offset_x", 0.0f );
	m_vWeaponOffset.y = rkLoader.LoadFloat_e( "weapon_offset_y", 0.0f );
	m_vWeaponOffset.z = rkLoader.LoadFloat_e( "weapon_offset_z", 0.0f );

	rkLoader.LoadString_e( "attack_animation", "", szBuf, MAX_PATH );
	m_szAttackAnimation = szBuf;

	m_fAttackAniRate = rkLoader.LoadFloat_e( "attack_ani_rate", FLOAT1 );	

	m_bSetDownAim = rkLoader.LoadBool_e( "set_down_target", false );

	//타겟 설정
	m_fColAngle = rkLoader.LoadFloat_e( "target_angle", 0.0f );
	m_fColRange = rkLoader.LoadFloat_e( "target_range", 0.0f );
	
	m_dwRefreshTime = rkLoader.LoadInt_e( "search_refresh_time", 0 );

	m_nAttackWeaponNum = rkLoader.LoadInt_e( "attack_weapon_num", NO_WEAPON_ATTRIBUTE );

	int nBuffCnt = rkLoader.LoadInt_e( "check_buff_cnt", 0 );
	m_vCheckBuffList.clear();
	m_vCheckBuffList.reserve( nBuffCnt );
	for( int i=0; i < nBuffCnt; ++i )
	{
		wsprintf_e( szKey, "check_buff_name%d", i+1 );
		rkLoader.LoadString( szKey, "", szBuf, MAX_PATH );
		m_vCheckBuffList.push_back( szBuf );
	}
}

void ioOwnerAttach5Dummy::ProcessState( float fTimePerSec, DWORD dwPreTime )
{
	ioBaseChar *pOwner = GetOwner();
	if( !pOwner || CheckDropZone( pOwner ) )
	{
		SetDieState( true );
		return;
	}

	if ( pOwner->GetState() == CS_BLOW_WOUNDED )
	{
		SendDieState( true );
		return;
	}

	switch( m_AttachState )
	{
	case AS_None:
		SetStartState( pOwner );
		break;
	case AS_Start:
		ProcessStartState( pOwner, fTimePerSec );
		break;
	case AS_Delay:
		ProcessDelayState( pOwner, fTimePerSec );
		break;
	case AS_Follow:
		ProcessFollowState( pOwner, fTimePerSec );
		break;
	case AS_Attack:
		ProcessAttackState( pOwner, fTimePerSec );
		break;
	}
}

ApplyWeaponExp ioOwnerAttach5Dummy::ApplyWeapon( ioWeapon *pWeapon )
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

		// 여기서 스킬 캔슬 처리 필요
		switch( m_DummyCharType )
		{
		case DCT_OWNER_CONTROL:
		case DCT_OWNER_CONTROL3:
		case DCT_OWNER_ATTACH:
			if( !m_ControlSkillName.IsEmpty() )
			{
				if( GetOwner() && GetOwner()->IsCurUsingSkill( m_ControlSkillName ) )
				{
					GetOwner()->ClearAttackFireTimeAndSkill( CAFT_APPLY_WOUNDED_BY_DUMMY );
					m_ControlSkillName.Clear();
				}
			}
			break;
		}
	}

	if( pWeapon )
	{
		int iDummyIndex = pWeapon->GetWeaponByDummyChar();
		g_DummyCharMgr.CheckEntityCollision( pAttacker, this, iDummyIndex, false );
	}

	return AWE_EXPLOSION;
}

void ioOwnerAttach5Dummy::ApplyDummyCharSync( ioPlayStage *pStage, SP2Packet &rkPacket )
{
	ioBaseChar *pOwner = GetOwner();
	if ( !pOwner )
		return;

	if( !pStage ) 
		return;	

	int nState;
	rkPacket >> nState;

	switch ( (AttachState)nState )
	{
	case AS_Delay:
		SetDelayState( pOwner );
		break;
	case AS_Follow:
		SetFollowState( pOwner );
		break;
	}
}

void ioOwnerAttach5Dummy::SetDieState( bool bEffect, bool bWoundedDie /*= false */ )
{
	ioDummyChar::SetDieState( bEffect, bWoundedDie );
}

bool ioOwnerAttach5Dummy::IsActivateWeapon()
{
	if( m_AttachState != AS_Attack )
		return false;

	return true;
}

void ioOwnerAttach5Dummy::SetStartState( ioBaseChar *pOwner )
{
	if ( !pOwner )
		return;

	ioPlayStage *pStage = pOwner->GetCreator();
	if ( !pStage )
		return;

	D3DXVECTOR3 vPos = GetWorldPosition();

	float fMapHeight = pStage->GetMapHeightOnlyTerrain( vPos.x, vPos.z );
	vPos.y = fMapHeight;
	SetWorldPosition( vPos );

	m_AttachState = AS_Start;
}

void ioOwnerAttach5Dummy::ProcessStartState( ioBaseChar *pOwner, float fTimePerSec )
{	
	if ( !pOwner )
		return;

	//이동 방향으로 회전
	SetRotateToOwner( pOwner );

	if( m_dwStartAniEndTime == 0 || m_dwStartAniEndTime < FRAMEGETTIME() )
	{
		SetDelayState( pOwner );
	}
}

void ioOwnerAttach5Dummy::SetDelayState( ioBaseChar *pOwner )
{
	if ( !pOwner )
		return;

	if( m_AttachState == AS_Delay )
		return;

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

void ioOwnerAttach5Dummy::ProcessDelayState( ioBaseChar *pOwner, float fTimePerSec )
{	
	if ( !pOwner )
		return;

	if ( !CheckControllRange( pOwner ) || 
		( m_dwLifeTime > 0 && ( m_dwStartTime + m_dwLifeTime ) < FRAMEGETTIME() ))
	{
		SetDieState( true );
		return;
	}
	
	//타겟 찾기
	if ( FindTarget( pOwner ) )
	{
		SetRotateToTarget( pOwner );
		SetAttackState( pOwner );
	}
	else
	{
		//이동 방향으로 회전
		SetRotateToOwner( pOwner );
		//키입력 확인
		CheckInputKey( pOwner );
	}
	
	if ( CheckFollowRange( pOwner ) )
	{
		SetFollowState( pOwner );
		m_bAutoFollow = true;
	}
}

void ioOwnerAttach5Dummy::SetFollowState( ioBaseChar *pOwner )
{
	if ( !pOwner )
		return;

	if( m_AttachState == AS_Follow )
		return;

	if( !m_MoveAnimation.IsEmpty() )
	{
		int iAniID = m_pGroup->GetAnimationIdx( m_MoveAnimation );
		if( iAniID != -1 )
		{
			m_pGroup->ClearAllActionAni( FLOAT10, true );
			m_pGroup->ClearAllLoopAni( FLOAT10, true );

			float fAniRate = m_fMoveAniRate;

			m_pGroup->SetLoopAni( iAniID, FLOAT10, FLOAT1, FLOAT1/fAniRate );
		}
	}

	m_AttachState = AS_Follow;
}

void ioOwnerAttach5Dummy::ProcessFollowState( ioBaseChar *pOwner, float fTimePerSec )
{
	if( !pOwner )
		return;
	
	if ( !CheckControllRange( pOwner ) || 
		( m_dwLifeTime > 0 && ( m_dwStartTime + m_dwLifeTime ) < FRAMEGETTIME() ))
	{
		SetDieState( true );
		return;
	}

	//타겟 찾기
	if ( FindTarget( pOwner ) )
	{
		SetRotateToTarget( pOwner );
		SetAttackState( pOwner );
	}
	else
	{
		//더미 이동
		FollowOwner( pOwner, fTimePerSec );
		//키입력 확인
		CheckInputKey( pOwner );
	}
}

void ioOwnerAttach5Dummy::SetAttackState( ioBaseChar *pOwner )
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
}

void ioOwnerAttach5Dummy::ProcessAttackState( ioBaseChar *pOwner, float fTimePerSec )
{
	if( !pOwner )
		return;

	DWORD dwCurTime = FRAMEGETTIME();

	CreateWeapon( pOwner );

	if ( m_dwAttackEndTime < dwCurTime )
	{
		if ( !CheckControllRange( pOwner ) || 
			( m_dwLifeTime > 0 && ( m_dwStartTime + m_dwLifeTime ) < FRAMEGETTIME() ))
		{
			SetDieState( true );
			return;
		}

		SetFollowState( pOwner );
		m_bAutoFollow = true;
		m_bCreateWeapon = false;
		m_dwCheckRefreshTime = dwCurTime;
	}	
}

bool ioOwnerAttach5Dummy::FindTarget( ioBaseChar *pOwner )
{	
	if( !pOwner ) 
		return false;
	
	if ( m_dwCheckRefreshTime + m_dwRefreshTime > FRAMEGETTIME() )
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
void ioOwnerAttach5Dummy::SortTargetList( ioBaseChar *pOwner, const BaseCharList &TargetList )
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
bool ioOwnerAttach5Dummy::IsEnableTarget( ioBaseChar *pTarget )
{
	if ( !pTarget || pTarget->HasHideBuff() || pTarget->IsPrisonerMode() )
		return false;

	int nBuffCnt = m_vCheckBuffList.size();
	for ( int i=0; i<nBuffCnt; i++)
	{
		if ( pTarget->HasBuff( m_vCheckBuffList[i] ))
			return false;
	}

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
void ioOwnerAttach5Dummy::SetRotateToTarget( ioBaseChar *pOwner )
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

//주인 방향으로 회전
void ioOwnerAttach5Dummy::SetRotateToOwner( ioBaseChar *pOwner )
{
	if ( !pOwner )
		return;

	SetOffsetPosAndDir( pOwner );

	if ( m_vMoveDir != ioMath::VEC3_ZERO )
	{
		D3DXVECTOR3 vRot = m_vMoveDir;
		D3DXVec3Normalize( &vRot, &vRot );

		D3DXQUATERNION qtRot = Help::ConvertDirToQuaternion( vRot );
		SetWorldOrientation( qtRot );
	}
}

void ioOwnerAttach5Dummy::SetOffsetPosAndDir( ioBaseChar *pOwner )
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

	m_vOffsetPos = vOnwerPos;
	m_vOffsetPos += (m_vTargetOffset.z * vDir);	
	m_vOffsetPos += (m_vTargetOffset.x * vSideDir);	

	m_vMoveDir = m_vOffsetPos - GetWorldPosition();
	m_vMoveDir.y = 0.0f;
}

//이동
void ioOwnerAttach5Dummy::FollowOwner( ioBaseChar *pOwner, float fTimePerSec )
{
	if ( !pOwner )
		return;
	
	ioPlayStage *pStage = pOwner->GetCreator();
	if ( !pStage )
		return;

	if ( pOwner->GetState() == CS_DASH )
		m_fMoveSpeed = m_fDashMoveSpeed;
	else 
		m_fMoveSpeed = m_fRunMoveSpeed;

	SetOffsetPosAndDir( pOwner );
	
	if ( D3DXVec3Length( &m_vMoveDir ) > FLOAT1 )
	{
		D3DXVECTOR3 vMoveDir = ioMath::VEC3_ZERO;
		D3DXVec3Normalize( &vMoveDir, &m_vMoveDir);

		Translate( vMoveDir * m_fMoveSpeed * fTimePerSec );
				
		D3DXVECTOR3 vPos = GetWorldPosition();
		float fMapHeight = pStage->GetMapHeightOnlyTerrain( vPos.x, vPos.z );
		if ( fMapHeight > 0.0f )
			vPos.y = fMapHeight;
		SetWorldPosition( vPos );

		D3DXVECTOR3 vRot = vMoveDir;
		D3DXQUATERNION qtRot = Help::ConvertDirToQuaternion( vRot );
		SetWorldOrientation( qtRot );
	}
	else
	{
		if ( m_bAutoFollow )
			m_bAutoFollow = false;
	}
}

void ioOwnerAttach5Dummy::CheckInputKey( ioBaseChar *pOwner )
{
	if ( !pOwner || !pOwner->IsNeedProcess() )
		return;

	if ( m_AttachState == AS_Delay && pOwner->IsSettedDirection() )
	{
		SetFollowState( pOwner );

		if ( pOwner->IsNeedSendNetwork() )
		{
			SP2Packet kPacket( CUPK_DUMMY_CHAR_SYNC );
			kPacket << m_OwnerName;
			kPacket << m_iCreateIndex;
			kPacket << AS_Follow;
			P2PNetwork::SendToAllPlayingUser( kPacket );
		}
	}
	else if ( m_AttachState == AS_Follow && !m_bAutoFollow && !pOwner->IsSettedDirection() )
	{
		SetDelayState( pOwner );

		if ( pOwner->IsNeedSendNetwork() )
		{
			SP2Packet kPacket( CUPK_DUMMY_CHAR_SYNC );
			kPacket << m_OwnerName;
			kPacket << m_iCreateIndex;
			kPacket << AS_Delay;
			P2PNetwork::SendToAllPlayingUser( kPacket );
		}
	}
}

bool ioOwnerAttach5Dummy::CheckControllRange( ioBaseChar *pOwner )
{
	if ( !pOwner )
		return false;

	D3DXVECTOR3 vDist = pOwner->GetWorldPosition() - GetWorldPosition();

	if ( D3DXVec3LengthSq( &vDist ) > m_fControlOutRange * m_fControlOutRange )
		return false;

	return true;
}

bool ioOwnerAttach5Dummy::CheckFollowRange( ioBaseChar *pOwner )
{
	if ( !pOwner )
		return false;

	D3DXVECTOR3 vDist = pOwner->GetWorldPosition() - GetWorldPosition();

	if ( D3DXVec3LengthSq( &vDist ) > m_fFollowRange * m_fFollowRange )
		return true;

	return false;
}

bool ioOwnerAttach5Dummy::CheckDropZone( ioBaseChar *pOwner )
{
	if ( !pOwner )
		return false;

	ioPlayStage *pStage = pOwner->GetCreator();
	if ( !pStage )
		return false;

	D3DXVECTOR3 vPos = GetWorldPosition();

	float fMapHeight = pStage->GetMapHeightOnlyTerrain( vPos.x, vPos.z );

	if ( fMapHeight <= 0.0f )
		return true;

	return false;
}

void ioOwnerAttach5Dummy::CreateWeapon( ioBaseChar *pOwner )
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

D3DXVECTOR3 ioOwnerAttach5Dummy::GetAttackDir( ioBaseChar *pOwner, const D3DXVECTOR3 &vPos )
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