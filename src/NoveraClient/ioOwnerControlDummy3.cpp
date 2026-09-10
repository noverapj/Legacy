
#include "stdafx.h"

#include "ioOwnerControlDummy3.h"
#include "BuffDefine.h"

ioOwnerControlDummy3::ioOwnerControlDummy3( ioEntityGroup *pGrp, ioPlayMode *pMode )
: ioDummyChar( pGrp, pMode )
{
	m_bStartInit = false;
	m_bTargetRot = false;
	m_bLeftRot = false;
	m_CurDirKey = ioUserKeyInput::DKI_NONE;
	m_dwCheckWeaponTime = 0;
	m_CreateWeaponList.clear();
	m_bSetDownKey = false;
	m_bSetUpKey = false;
}

ioOwnerControlDummy3::~ioOwnerControlDummy3()
{
	m_CreateWeaponList.clear();
}

void ioOwnerControlDummy3::LoadProperty( ioINILoader &rkLoader )
{
	ioDummyChar::LoadProperty( rkLoader );
	
	LoadInitProperty( rkLoader );
}

void ioOwnerControlDummy3::LoadInitProperty( ioINILoader &rkLoader )
{
	//이동 속도
	m_fMoveSpeed  = rkLoader.LoadFloat_e( "move_speed", FLOAT1 );
	//회전 속도
	m_dwRotateTime = rkLoader.LoadInt_e( "rotate_speed", 0 );

	//무기 인덱스, 재생성 시간
	m_nCreateWeaponIndex = rkLoader.LoadInt_e( "attack_type", NO_WEAPON_ATTRIBUTE );
	m_dwCreateWeaponTime = rkLoader.LoadInt_e( "attack_reload", 0 );

	//오프셋
	m_vCreateWeaponOffset.x = rkLoader.LoadFloat_e( "attack_offset_x", 0.0f );
	m_vCreateWeaponOffset.y = rkLoader.LoadFloat_e( "attack_offset_y", 0.0f );
	m_vCreateWeaponOffset.z = rkLoader.LoadFloat_e( "attack_offset_z", 0.0f );

	//각도
	m_fCreateWeaponDegree = rkLoader.LoadFloat_e( "attack_degree", FLOAT1 );

	//마지막 공격 타입
	m_nLastAttackIndex = rkLoader.LoadInt_e( "last_attack_type", NO_WEAPON_ATTRIBUTE );

	//더미 높이 조정 
	m_bSetHeightAdjust = rkLoader.LoadBool_e( "set_height_adjust", false );
}

void ioOwnerControlDummy3::ProcessState( float fTimePerSec, DWORD dwPreTime )
{
	switch( m_State )
	{
	case DCS_START:
		ProcessStartState( fTimePerSec );
		break;
	case DCS_MOVE:
		ProcessMoveState( fTimePerSec );
		break;
	}
}

void ioOwnerControlDummy3::ProcessStartState( float fTimePerSec )
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
	 	SetMoveState();		
		return;
	}
}

void ioOwnerControlDummy3::ProcessMoveState( float fTimePerSec )
{
	ioBaseChar *pOwner = GetOwner();

	if( !pOwner )
		return;
	
	//무기 발사
	FireWeapon();
	//키입력
	CheckInputKey( fTimePerSec );
	//회전
	ProcessRotate();
	//자동 이동
	AutoMove( fTimePerSec );	
}

//상태 변경
void ioOwnerControlDummy3::SetMoveState()
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

ApplyWeaponExp ioOwnerControlDummy3::ApplyWeapon( ioWeapon *pWeapon )
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

void ioOwnerControlDummy3::ApplyDummyCharSync( ioPlayStage *pStage, SP2Packet &rkPacket )
{
	if( !pStage ) return;	
	
	int iType;
	rkPacket >> iType;

	switch( iType )
	{	
	case DS_Rotate:
		{
			//방향 전환
			int nDir;
			rkPacket >> nDir;
			rkPacket >> m_bLeftRot;

			m_CurDirKey = (ioUserKeyInput::DirKeyInput)nDir;
			if ( m_CurDirKey != ioUserKeyInput::DKI_NONE)
			{
				m_bTargetRot = false;
			}

			D3DXQUATERNION qtRot;
			rkPacket >> qtRot;

			SetWorldOrientation( qtRot );
		}
		break;
	case DS_Height:
		{						
			rkPacket >> m_bSetUpKey;
			rkPacket >> m_bSetDownKey;
		}
		break;
	}
}
//더미 죽는 상태 설정
void ioOwnerControlDummy3::SetDieState( bool bEffect, bool bWoundedDie /*= false */ )
{
	ioDummyChar::SetDieState( bEffect, bWoundedDie );
	
	//생성한 무기 제거
	RemoveWeapon();

	//마지막 공격
	LastFireWeapon();
}

bool ioOwnerControlDummy3::IsActivateWeapon()
{
	if( m_State == DCS_DELAY )
		return false;

	return true;
}

void ioOwnerControlDummy3::CheckCollisionWeapon( ioBaseChar *pChar )
{
	if( pChar )
		m_vAttackDir = pChar->GetMoveDir();
}

//무기 생성
void ioOwnerControlDummy3::CreateWeapon( int nAttackIndex, const D3DXVECTOR3 &vPos, const D3DXVECTOR3 &vDir )
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

		//인덱스 얻기
		int nIndex = pWeapon->GetWeaponIndex();
		//생성한 무기 인덱스 설정
		m_CreateWeaponList.push_back( nIndex );
	}	
}

//무기 발사
void ioOwnerControlDummy3::FireWeapon()
{
	if ( m_nCreateWeaponIndex == NO_WEAPON_ATTRIBUTE || 
		m_dwCreateWeaponTime == 0 || 
		m_dwCheckWeaponTime + m_dwCreateWeaponTime > FRAMEGETTIME() )
		return;

	//위치, 방향 설정
	D3DXQUATERNION qtRot = GetWorldOrientation();
	D3DXVECTOR3 vPos = GetWorldPosition() + ( qtRot * m_vCreateWeaponOffset );

	D3DXVECTOR3 vDir = qtRot * D3DXVECTOR3(0.0f,0.0f,-FLOAT1);
	D3DXVec3Normalize( &vDir, &vDir);

	//방향 설정
	float fYawR = DEGtoRAD( m_fCreateWeaponDegree );
	D3DXQUATERNION qtRotate;
	D3DXQuaternionRotationYawPitchRoll( &qtRotate, fYawR, 0.0f, 0.0f );

	//방향 설정
	vDir = qtRotate * vDir;

	//무기 생성
	CreateWeapon( m_nCreateWeaponIndex, vPos, vDir );

	m_dwCheckWeaponTime = FRAMEGETTIME();
}

//마지막 공격
void ioOwnerControlDummy3::LastFireWeapon()
{
	if ( m_nLastAttackIndex == NO_WEAPON_ATTRIBUTE )
		return;

	D3DXVECTOR3 vDir = GetWorldOrientation() * D3DXVECTOR3(0.0f,0.0f,-FLOAT1);
	D3DXVec3Normalize( &vDir, &vDir);
	D3DXVECTOR3 vPos = GetWorldPosition();

	CreateWeapon( m_nLastAttackIndex, vPos, vDir );
}

//무기 제거
void ioOwnerControlDummy3::RemoveWeapon()
{
	//무기 리스트 얻기
	int nWeaponCnt = (int)m_CreateWeaponList.size();

	for (int i=0; i<nWeaponCnt; ++i)
	{
		ioWeapon *pWeapon = g_WeaponMgr.FindWeapon( GetOwner(), m_CreateWeaponList[i] );
		if ( pWeapon && pWeapon->IsLive() )
		{
			pWeapon->SetWeaponDead();
		}
	}

	//리스트 비움
	m_CreateWeaponList.clear();
}

//자동이동
void ioOwnerControlDummy3::AutoMove( float fTimePerSec )
{	
	ioBaseChar *pOwner = GetOwner();
	if( !pOwner )	return;
	
	D3DXQUATERNION qtRot = GetWorldOrientation();
	D3DXVECTOR3 vDir = qtRot * D3DXVECTOR3(0.0f,0.0f,-FLOAT1);
	D3DXVec3Normalize(&vDir,&vDir);

	vDir = vDir * m_fMoveSpeed * fTimePerSec;
	
	bool bCol = false;
	ioPlayStage *pStage = pOwner->GetCreator();
	if( m_bMountEnable )
	{
		UpdateMountedEntityList();

		if( pStage && pStage->ReCalculateMoveVectorByTerrain( this, &vDir, bCol, m_vMountEntityList ) )
		{
			Translate( vDir );
		}
	}
	else
	{
		if( pStage && pStage->ReCalculateMoveVectorByTerrain( this, &vDir, bCol ) )
		{
			Translate( vDir );
		}
	}
	m_fGravityAmt = 0.0f;
	
	//높이 이동
	float fMapHeight = GetBottomHeight();
	float fHeightAmt = 0.0f;

	// 수직이동
	float fUpDownSpeed = m_fUpDownSpeed * fTimePerSec;
	float fHeightGap = ( GetWorldPosition().y + fUpDownSpeed ) - fMapHeight;

	// 위로
	if( m_bSetUpKey )
	{
		if( fHeightGap < m_fMaxHeight )
		{
			fHeightAmt += fUpDownSpeed;
		}
	}
	// 아래로
	if( m_bSetDownKey )
	{
		fHeightAmt -= fUpDownSpeed;
	}

	//위치 변경
	D3DXVECTOR3 vPos = GetWorldPosition();	
	vPos.y += fHeightAmt;
	
	ioOrientBox kBox = GetWorldCollisionBox();
	float fDownHeight = vPos.y - kBox.GetExtents(1);
	if( fDownHeight < fMapHeight )
	{
		vPos.y = fMapHeight + kBox.GetExtents(1);
	}

	if( pStage )
	{
		pStage->CheckFlyMapLimit( &vPos );
		pStage->CheckAxisLimit( &vPos );
	}

	D3DXVECTOR3 vPrePos = GetWorldPosition();
	SetWorldPosition( vPos );
	float fDuumyGap = fabs(vPrePos.y - vPos.y);			
	if( 0 < fDuumyGap )
		SetWorldPositionMountEntity( fHeightAmt );
}

//키입력
void ioOwnerControlDummy3::CheckInputKey( float fTimePerSec )
{
	ioBaseChar *pOwner = GetOwner();

	if( !pOwner || !pOwner->IsNeedProcess()) 
		return;

	bool bSendRotate = false;
	bool bSendHeight = false;

	//높이 조정
	if ( m_bSetHeightAdjust )
	{
		// Up 체크
		if ( !m_bSetDownKey )
		{
			if( !m_bSetUpKey && pOwner->IsJumpKeyDown() )
			{
				m_bSetUpKey = true;
				bSendHeight = true;
			}
			else if ( m_bSetUpKey && !pOwner->IsJumpKeyDown())
			{
				m_bSetUpKey = false;
				bSendHeight = true;
			}
		}

		if ( !m_bSetUpKey )
		{
			// Down 체크
			if( !m_bSetDownKey && pOwner->IsDefenseKeyDown() )
			{
				m_bSetDownKey = true;
				bSendHeight = true;
			}
			else if ( m_bSetDownKey && !pOwner->IsDefenseKeyDown())
			{
				m_bSetDownKey = false;
				bSendHeight = true;
			}
		}
	}

	//회전
	if( pOwner->IsSettedDirection() )
	{
		ioUserKeyInput::DirKeyInput eNewDirKey = pOwner->CheckCurDirKey();
		if( eNewDirKey != m_CurDirKey )
		{
			float fYawD = 180.0f + 45.0f * ( eNewDirKey - ioUserKeyInput::DKI_UP );
			fYawD = ioMath::ArrangeHead( fYawD );

			float fYawR = DEGtoRAD( fYawD );
			D3DXQUATERNION qtRotate;
			D3DXQuaternionRotationYawPitchRoll( &qtRotate, fYawR, 0.0f, 0.0f );

			D3DXVECTOR3 vTargetDir = qtRotate * D3DXVECTOR3( 0.0f, 0.0f, -FLOAT1 );
			D3DXVECTOR3 vDir = GetWorldOrientation() * D3DXVECTOR3( 0.0f, 0.0f, -FLOAT1 );
			D3DXQUATERNION qtCurRot = GetWorldOrientation();

			m_bTargetRot = false;
			m_bLeftRot = false;
			if( D3DXVec3Dot( &vTargetDir, &vDir ) > -FLOAT1 )
			{
				float fCurYaw, fTargetYaw;
				fCurYaw = RADtoDEG( ioMath::QuaterToYaw( qtCurRot ) );
				fTargetYaw = RADtoDEG( ioMath::QuaterToYaw( qtRotate ) );

				fCurYaw = ioMath::ArrangeHead( fCurYaw );
				fTargetYaw = ioMath::ArrangeHead( fTargetYaw );

				float fRotAngle = ioMath::AngleGap( fCurYaw, fTargetYaw );
				if( fRotAngle < 0.0f )
					m_bLeftRot = true;
			}

			m_CurDirKey = eNewDirKey;
			bSendRotate = true;			
		}
	}
	else
	{
		if( m_CurDirKey != ioUserKeyInput::DKI_NONE )
		{
			m_CurDirKey = ioUserKeyInput::DKI_NONE;
			bSendRotate = true;			
		}
	}

	if( pOwner->IsNeedSendNetwork() )
	{
		if ( bSendRotate )
		{
			D3DXQUATERNION qtRot = GetWorldOrientation();
			SP2Packet kPacket( CUPK_DUMMY_CHAR_SYNC );
			kPacket << m_OwnerName;
			kPacket << m_iCreateIndex;
			kPacket << DS_Rotate;
			kPacket << (int)m_CurDirKey;
			kPacket << m_bLeftRot;
			kPacket << qtRot;
			P2PNetwork::SendToAllPlayingUser( kPacket );
		}

		if ( bSendHeight )
		{
			SP2Packet kPacket( CUPK_DUMMY_CHAR_SYNC );
			kPacket << m_OwnerName;
			kPacket << m_iCreateIndex;
			kPacket << DS_Height;
			kPacket << m_bSetUpKey;
			kPacket << m_bSetDownKey;
			P2PNetwork::SendToAllPlayingUser( kPacket );
		}
	}
}

//회전
void ioOwnerControlDummy3::ProcessRotate()
{
	ioBaseChar *pOwner = GetOwner();

	if( !pOwner ) 
		return;

	if( m_dwRotateTime == 0 ) 
		return;

	if( m_bTargetRot ) 
		return;

	if( m_CurDirKey == ioUserKeyInput::DKI_NONE )
		return;

	float fFrameGap = g_FrameTimer.GetCurFrameGap();
	float fRate = fFrameGap / m_dwRotateTime;
	float fRotateAngle = 360.0f * fRate;

	// 목표에 도달체크 및 처리
	float fYawD = 180.0f + 45.0f * ( m_CurDirKey - ioUserKeyInput::DKI_UP );
	fYawD = ioMath::ArrangeHead( fYawD );

	float fYawR = DEGtoRAD( fYawD );
	D3DXQUATERNION qtRotate;
	D3DXQuaternionRotationYawPitchRoll( &qtRotate, fYawR, 0.0f, 0.0f );

	D3DXQUATERNION qtCurRot = GetWorldOrientation();

	float fCurYaw, fTargetYaw;
	fCurYaw = RADtoDEG( ioMath::QuaterToYaw( qtCurRot ) );
	fTargetYaw = RADtoDEG( ioMath::QuaterToYaw( qtRotate ) );

	fCurYaw = ioMath::ArrangeHead( fCurYaw );
	fTargetYaw = ioMath::ArrangeHead( fTargetYaw );

	float fRotAngle = ioMath::AngleGap( fCurYaw, fTargetYaw );
	if( fabs( fRotAngle ) < fRotateAngle )
	{
		m_bTargetRot = true;
		SetWorldOrientation( qtRotate );
		return;
	}

	if( m_bLeftRot )
		fRotateAngle *= -1;

	fRotateAngle = ioMath::ArrangeHead( fRotateAngle );
	D3DXVECTOR3 vDir = qtCurRot * D3DXVECTOR3(0.0f,0.0f,-FLOAT1);

	D3DXQUATERNION qtAngle;
	D3DXQuaternionRotationYawPitchRoll( &qtAngle, DEGtoRAD(fRotateAngle), 0.0f, 0.0f );

	vDir = qtAngle * vDir;
	ioMath::CalcDirectionQuaternion( qtAngle, -vDir );

	SetWorldOrientation( qtAngle );
}