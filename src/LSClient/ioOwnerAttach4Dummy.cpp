
#include "stdafx.h"

#include "ioOwnerAttach4Dummy.h"
#include "BuffDefine.h"

ioOwnerAttach4Dummy::ioOwnerAttach4Dummy( ioEntityGroup *pGrp, ioPlayMode *pMode )
: ioDummyChar( pGrp, pMode )
{
	m_bStartInit = false;
	m_vTargetPos = ioMath::VEC3_ZERO;
	m_bIsHead = false;
	m_bSetDontMove = false;
	m_bSetFire = false;

	m_nCurSplitIndex = 0;
	m_nFinshCnt = 0;
}

ioOwnerAttach4Dummy::~ioOwnerAttach4Dummy()
{
}

void ioOwnerAttach4Dummy::LoadProperty( ioINILoader &rkLoader )
{
	ioDummyChar::LoadProperty( rkLoader );
	
	LoadInitProperty( rkLoader );
}

void ioOwnerAttach4Dummy::LoadInitProperty( ioINILoader &rkLoader )
{
	m_fMoveSpeed  = rkLoader.LoadFloat_e( "move_speed", FLOAT1 );

	char szBuf[MAX_PATH] = "";
	rkLoader.LoadString_e( "check_buff", "", szBuf, MAX_PATH );
	m_szCheckBuff = szBuf;

	m_CreateWeaponList.clear();

	m_nMaxSplitCnt = rkLoader.LoadInt_e( "max_split_cnt", 1);

	int nAttackCnt = rkLoader.LoadInt_e( "attack_cnt", 0);
	for ( int i=0; i<nAttackCnt; ++i )
	{
		CreateWeaponInfo sInfo;

		wsprintf_e( szBuf, "attack%d_type", i+1 );
		sInfo.nAttackIndex = rkLoader.LoadInt( szBuf, NO_WEAPON_ATTRIBUTE );

		wsprintf_e( szBuf, "attack%d_offset_x", i+1 );
		sInfo.vOffsetPos.x = rkLoader.LoadFloat( szBuf, 0.0f );
		wsprintf_e( szBuf, "attack%d_offset_y", i+1 );
		sInfo.vOffsetPos.y = rkLoader.LoadFloat( szBuf, 0.0f );
		wsprintf_e( szBuf, "attack%d_offset_z", i+1 );
		sInfo.vOffsetPos.z = rkLoader.LoadFloat( szBuf, 0.0f );

		wsprintf_e( szBuf, "attack%d_degree", i+1 );
		sInfo.fDegree = rkLoader.LoadFloat( szBuf, 0.0f );

		wsprintf_e( szBuf, "attack%d_reload", i+1 );
		sInfo.dwReloadTime = rkLoader.LoadInt( szBuf, 0 );

		wsprintf_e( szBuf, "attack%d_max_fire_cnt", i+1 );
		sInfo.nMaxFireCnt = rkLoader.LoadInt( szBuf, 1 );

		sInfo.dwCheckTime = 0;
		sInfo.nCurFireCnt = 0;

		m_CreateWeaponList.push_back( sInfo );
	}

	m_nWeaponSplitCnt = (int)m_CreateWeaponList.size() / m_nMaxSplitCnt;
}

void ioOwnerAttach4Dummy::ProcessState( float fTimePerSec, DWORD dwPreTime )
{
	switch( m_State )
	{
	case DCS_START:
		ProcessStartState( fTimePerSec );
		break;
	case DCS_DELAY:
		ProcessDelayState( fTimePerSec );
		break;
	case DCS_MOVE:
		ProcessMoveState( fTimePerSec );
		break;
	}
}

void ioOwnerAttach4Dummy::ProcessStartState( float fTimePerSec )
{
	ioBaseChar *pOwner = GetOwner();

	if( !pOwner )
		return;

	if ( !m_bStartInit )
	{
		SetAttachWeapon();

		//위치 오프셋 설정
		D3DXVECTOR3 vPos = GetWorldPosition();
		vPos.y += m_fHeightOffset;
		SetWorldPosition( vPos );
		m_bStartInit = true;
	}
	
	if( m_dwStartAniEndTime == 0 || m_dwStartAniEndTime < FRAMEGETTIME() )
	{
		SetDelayState();	
		return;
	}
}

void ioOwnerAttach4Dummy::ProcessDelayState( float fTimePerSec )
{	
	ioBaseChar *pOwner = GetOwner();

	if ( !pOwner )
		return;

	//버프 확인
	if ( !CheckOwnerBuff() )
	{
		SetDieState(true);
		return;
	}

	//이동 확인
	if ( !m_bSetDontMove )
	{
		DummyMove( fTimePerSec );
	}

	//무기 발사
	FireWeapon();

	if ( pOwner->IsNeedProcess() )
	{
		if ( pOwner->IsSettedDirection() )
		{
			SetFollowState();

			if ( pOwner->IsNeedSendNetwork() )
			{
				SP2Packet kPacket( CUPK_DUMMY_CHAR_SYNC );
				kPacket << m_OwnerName;
				kPacket << m_iCreateIndex;
				kPacket << DCS_MOVE;
				P2PNetwork::SendToAllPlayingUser( kPacket );
			}
		}
	}
	
}

void ioOwnerAttach4Dummy::ProcessMoveState( float fTimePerSec )
{
	ioBaseChar *pOwner = GetOwner();

	if( !pOwner )
		return;

	//버프 확인
	if ( !CheckOwnerBuff() )
	{
		SetDieState(true);
		return;
	}
	
	//이동 확인
	if ( !m_bSetDontMove )
	{
		DummyMove( fTimePerSec );
	}
		
	//무기 발사
	FireWeapon();	

	if ( pOwner->IsNeedProcess() )
	{
		if ( !pOwner->IsSettedDirection() )
		{
			SetDelayState();

			if ( pOwner->IsNeedSendNetwork() )
			{
				SP2Packet kPacket( CUPK_DUMMY_CHAR_SYNC );
				kPacket << m_OwnerName;
				kPacket << m_iCreateIndex;
				kPacket << DCS_DELAY;
				P2PNetwork::SendToAllPlayingUser( kPacket );
			}
		}
	}
}

//상태 변경
void ioOwnerAttach4Dummy::SetDelayState()
{
	if( m_State == DCS_DELAY )
		return;

	m_State = DCS_DELAY;

	if( !m_DelayAnimation.IsEmpty() )
	{
		int iAniID = m_pGroup->GetAnimationIdx( m_DelayAnimation );
		if( iAniID != -1 )
		{
			m_pGroup->ClearAllActionAni( FLOAT10, true );
			m_pGroup->ClearAllLoopAni( FLOAT10, true );

			float fAniRate = FLOAT1;
			if( m_fMoveAniRate > 0.0f )
				fAniRate = m_fMoveAniRate;

			m_pGroup->SetLoopAni( iAniID, FLOAT10, FLOAT1, FLOAT1/fAniRate );
		}
	}
}

void ioOwnerAttach4Dummy::SetFollowState()
{
	if( m_State == DCS_MOVE )
		return;

	m_State = DCS_MOVE;
	if( !m_MoveAnimation.IsEmpty() )
	{
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
	}
}

ApplyWeaponExp ioOwnerAttach4Dummy::ApplyWeapon( ioWeapon *pWeapon )
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

void ioOwnerAttach4Dummy::ApplyDummyCharSync( ioPlayStage *pStage, SP2Packet &rkPacket )
{
	if( !pStage ) return;	

	int state;
	rkPacket >> state;

	switch ( (DummyCharState)state )
	{
	case DCS_DELAY:
		SetDelayState();
		break;
	case DCS_MOVE:
		SetFollowState();
		break;
	}
}

void ioOwnerAttach4Dummy::SetDieState( bool bEffect, bool bWoundedDie /*= false */ )
{
	ioDummyChar::SetDieState( bEffect, bWoundedDie );
}

bool ioOwnerAttach4Dummy::IsActivateWeapon()
{
	if( m_State == DCS_DELAY )
		return false;

	return true;
}

//버프 확인
bool ioOwnerAttach4Dummy::CheckOwnerBuff()
{
	ioBaseChar *pOwner = GetOwner();
	if( !pOwner )
		return false;

	if( !m_szCheckBuff.IsEmpty() && !pOwner->HasBuff( m_szCheckBuff ) )
		return false;

	return true;
}

void ioOwnerAttach4Dummy::SetTargetPos( D3DXVECTOR3 vPos)
{
	m_vTargetPos = vPos;
}
//앞칸 설정
void ioOwnerAttach4Dummy::SetHead( bool bHead )
{
	m_bIsHead = bHead;
}
//앞칸 확인
bool ioOwnerAttach4Dummy::GetHead()
{
	return m_bIsHead;
}
//이동
void ioOwnerAttach4Dummy::DummyMove( float fTimePerSec )
{
	D3DXVECTOR3 vOffsetPos = GetWorldPosition();
	if ( m_bIsHead )
	{
		vOffsetPos.y -= m_fHeightOffset;
	}

	m_vMoveDir = m_vTargetPos - vOffsetPos;

	if ( D3DXVec3Length( &m_vMoveDir ) > abs( m_fFrontOffset ) )
	{
		D3DXVECTOR3 vMoveDir = ioMath::VEC3_ZERO;
		D3DXVec3Normalize( &vMoveDir, &m_vMoveDir);
		Translate( vMoveDir * m_fMoveSpeed * fTimePerSec );
	}

	D3DXVECTOR3 vRot = m_vMoveDir;
	vRot.y = 0.0f;

	D3DXQUATERNION qtRot = Help::ConvertDirToQuaternion( vRot );
	SetWorldOrientation( qtRot );
}
//이동 하지 않을 것인가 설정
void ioOwnerAttach4Dummy::SetDontMove( bool bDontMove )
{
	m_bSetDontMove = bDontMove;
}
//무기 생성
void ioOwnerAttach4Dummy::CreateWeapon( int nAttackIndex, const D3DXVECTOR3 &vPos, const D3DXVECTOR3 &vDir )
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
//무기 발사
void ioOwnerAttach4Dummy::FireWeapon()
{
	if ( !m_bSetFire )
		return;
	
	if (m_nWeaponSplitCnt <= 0)
		return;
		
	//분할 인덱스 한번 순환
	for (int i=0; i<m_nWeaponSplitCnt; ++i)
	{   
		//인덱스를 통해 데이터 접근
		CreateWeaponInfo &FireInfo = m_CreateWeaponList[i*m_nMaxSplitCnt + m_nCurSplitIndex];

		if ( FireInfo.nCurFireCnt < FireInfo.nMaxFireCnt && 
			( FireInfo.dwCheckTime + FireInfo.dwReloadTime < FRAMEGETTIME()) )
		{
			//위치, 방향 설정
			D3DXVECTOR3 vPos = GetWorldPosition() + FireInfo.vOffsetPos;
			D3DXVECTOR3 vDir = ioMath::VEC3_ZERO;
			D3DXVec3Normalize( &vDir, &m_vMoveDir);

			//방향 설정
			float fYawR = DEGtoRAD( FireInfo.fDegree );
			D3DXQUATERNION qtRotate;
			D3DXQuaternionRotationYawPitchRoll( &qtRotate, fYawR, 0.0f, 0.0f );
			
			//방향 설정
			vDir = qtRotate * vDir;
			
			//무기 생성
			CreateWeapon( FireInfo.nAttackIndex, vPos, vDir);
			FireInfo.dwCheckTime = FRAMEGETTIME();
			FireInfo.nCurFireCnt++;	

			//발사 개수 확인
			if ( FireInfo.nCurFireCnt >= FireInfo.nMaxFireCnt )
			{
				//피니시 카운트 계산
				m_nFinshCnt++;
			}		
		}
	}

	//모든 무기가 발사 완료 되면
	if ( m_nFinshCnt >= m_nWeaponSplitCnt )
	{
		//발사 개수 초기화
		for (int i=0; i<m_nWeaponSplitCnt; ++i)
		{   
			CreateWeaponInfo &FireInfo = m_CreateWeaponList[i*m_nMaxSplitCnt + m_nCurSplitIndex];
			FireInfo.nCurFireCnt = 0;
		}

		//분할 인덱스 증가
		m_nCurSplitIndex++;

		//모든 분할 인덱스를 돌았다면
		if (m_nCurSplitIndex >= m_nMaxSplitCnt)
		{
			m_nCurSplitIndex = 0;
		}

		//피니시 카운트 초기화
		m_nFinshCnt = 0;
				
		//무기 발사 종료
		m_bSetFire = false;
	}	
}

//발사 설정
void ioOwnerAttach4Dummy::SetFire( bool bFire )
{
	m_bSetFire = bFire;
}