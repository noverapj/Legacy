#include "StdAfx.h"
#include "ioControlFireSkill5.h"

#include "ioBaseChar.h"
#include "ioPlayStage.h"
#include "ioPoleDummy.h"

ioControlFireSkill5::ioControlFireSkill5(void)
{
	ClearData();
}

ioControlFireSkill5::ioControlFireSkill5( const ioControlFireSkill5 &rhs ) :
ioAttackSkill( rhs ),
//Start
m_fStartAngle( rhs. m_fStartAngle ),
m_fFlyMaxRange( rhs.m_fFlyMaxRange ),
m_dwFlyDuration( rhs.m_dwFlyDuration ),
m_szFlyMotion( rhs.m_szFlyMotion ),
m_fFlyMotionRate( rhs.m_fFlyMotionRate ),
//Loop
m_dwLoopDuration( rhs.m_dwLoopDuration ),
m_SkillLoopMotion( rhs.m_SkillLoopMotion ),
m_SkillUpLoopMotion( rhs.m_SkillUpLoopMotion ),
m_SkillDownLoopMotion( rhs.m_SkillDownLoopMotion ),

m_dwDirKeyRotateSpeed( rhs.m_dwDirKeyRotateSpeed ),
m_dwUpDownRotateSpeed( rhs.m_dwUpDownRotateSpeed ),
m_fMaxLimitAngle( rhs.m_fMaxLimitAngle ),
m_fMinLimitAngle( rhs.m_fMinLimitAngle ),
m_DummyCharName( rhs.m_DummyCharName ),
m_UpDownKeyType( rhs.m_UpDownKeyType ),

m_bCheckDummyDie( rhs.m_bCheckDummyDie ),
m_dwFlyProtectTime( rhs.m_dwFlyProtectTime ),

m_iMaxAttackCount( rhs.m_iMaxAttackCount )
{
	ClearData();
}

ioControlFireSkill5::~ioControlFireSkill5(void)
{
}

ioSkill* ioControlFireSkill5::Clone()
{
	return new ioControlFireSkill5( *this );
}

void ioControlFireSkill5::LoadProperty( ioINILoader &rkLoader )
{
	ioAttackSkill::LoadProperty( rkLoader );
	char szBuf[MAX_PATH];

	//fly
	m_fStartAngle = rkLoader.LoadFloat_e( "start_angle", 0.0f );
	m_fFlyMaxRange = rkLoader.LoadFloat_e( "fly_max_range", 0.0f );
	m_dwFlyDuration = (DWORD)rkLoader.LoadInt_e( "fly_duration", 0 );
	rkLoader.LoadString_e( "fly_motion", "", szBuf, MAX_PATH );
	m_szFlyMotion = szBuf;
	m_fFlyMotionRate = rkLoader.LoadFloat_e( "fly_motion_rate", FLOAT1 );
	m_dwFlyProtectTime = (DWORD)rkLoader.LoadInt_e( "fly_protect_time", 0 );

	//Loop
	m_dwLoopDuration = rkLoader.LoadInt_e( "skill_loop_duration", 0 );
	rkLoader.LoadString_e( "skill_loop_motion", "", szBuf, MAX_PATH );
	m_SkillLoopMotion = szBuf;
	rkLoader.LoadString_e( "skill_up_loop_motion", "", szBuf, MAX_PATH );
	m_SkillUpLoopMotion = szBuf;
	rkLoader.LoadString_e( "skill_down_loop_motion", "", szBuf, MAX_PATH );
	m_SkillDownLoopMotion = szBuf;

	//Rotate
	m_UpDownKeyType = (UpDownKeyType)rkLoader.LoadInt_e( "up_down_key_type", UDKT_NONE );
	m_dwDirKeyRotateSpeed = rkLoader.LoadInt_e( "dir_key_rotate_speed", 0 );
	m_dwUpDownRotateSpeed = rkLoader.LoadInt_e( "up_down_rotate_speed", 0 );

	// Limit Angle
	m_fMaxLimitAngle = rkLoader.LoadFloat_e( "max_limit_angle", 0.0f );
	m_fMinLimitAngle = rkLoader.LoadFloat_e( "min_limit_angle", 0.0f );

	rkLoader.LoadString_e( "dummy_char_name", "", szBuf, MAX_PATH );
	m_DummyCharName = szBuf;

	m_bCheckDummyDie = rkLoader.LoadBool_e( "check_dummy_die", false );

	//Attack
	m_iMaxAttackCount = rkLoader.LoadInt_e( "max_attack_count", 0 );
}

void ioControlFireSkill5::ClearData()
{
	m_SkillState = SS_NONE;

	//Loop
	m_dwLoopStartTime = 0;

	//Fly
	m_fFlyCurRange = 0.0f;

	//Rotate
	m_fCurPositionOffset = 0.0f;
	m_CurDirKey = ioUserKeyInput::DKI_NONE;
	m_bMotionSetted = false;
	//m_vAttackDir = ioMath::VEC3_ZERO;
	//m_vTestDir = ioMath::VEC3_ZERO;
	m_bLeftRot = false;
	m_bTargetRot = false;
	m_fCurMotionRate = 0.0f;

	//sync
	m_bDir = false;
	m_bUp = false;
	m_bDown = false;

	m_fTargetAttachAngleY = 0.0f;
	m_fTargetAttachAngleX = 270.0f;

	m_iDummyIndex = -1;
	m_bDieDummy = false;

	m_dwProtectTime = 0;

	//Attack
	m_iCurAttackCount = 0;
}

bool ioControlFireSkill5::OnSkillStart( ioBaseChar *pChar )
{
	if( !ioAttackSkill::OnSkillStart( pChar ) )
		return false;

	ClearData();

	m_bSetHandMesh = false;
	if( m_bUseItemMesh )
	{
		pChar->ShowWeaponItemMesh( false );
	}

	if( !m_bSetHandMesh && m_bUseItemMesh && !m_HandMesh.IsEmpty() )
	{
		pChar->ChangeSkillWeaponMesh( this, m_HandMesh, m_bVisibleEquipMesh );
		m_bSetHandMesh = true;
	}

	SetCameraBuff( pChar );

	//기본 게이지 감소 (마법진 게이지)
	if ( !m_bReduceGauge )
	{
		pChar->ReduceNeedGaugeBySkill( this, GetNeedGauge(pChar->GetCharName()) );
		m_bReduceGauge = true;
	}

	m_vSkillStartPosition = pChar->GetWorldPosition();
	m_fTargetAttachAngleY = RADtoDEG( ioMath::QuaterToYaw( pChar->GetWorldOrientation() ) );
	m_fTargetAttachAngleX = m_fStartAngle;
	if ( m_fTargetAttachAngleX ==FLOAT90 )
		m_fTargetAttachAngleX = 89.5f;
	else if ( m_fTargetAttachAngleX == 270.0f )
		m_fTargetAttachAngleX = 269.5f;

	if ( m_UpDownKeyType == UDKT_NONE )
		m_bUp = false;
	else if ( m_UpDownKeyType == UDKT_AUTO_UP_KEY )
		m_bUp = true;

	pChar->SetForcePowerAmt( 0.0f );
	pChar->SetGravityAmt( 0.0f );
	pChar->SetCurMoveSpeed( 0.0f );

	return true;
}

void ioControlFireSkill5::OnProcessState( ioBaseChar *pChar, ioPlayStage *pStage )
{
	DWORD dwCurTime = FRAMEGETTIME();

	switch( m_SkillState )
	{
	case SS_NONE:
		{
			DWORD dwPreDelay = GetPreDelayTime();
			if( m_dwPreDelayStartTime + dwPreDelay < dwCurTime )
			{
				m_dwPreDelayStartTime = 0;
				EndPreDelayEffect( pChar );
				SetFlyState( pChar );
				CreateDummy( pChar, pStage );
			}
		}
		break;
	case SS_FLY:
		{
			if( !ProcessFlyStart( pChar ) )
			{
				SetLoopState( pChar, pStage );
				
				return;
			}
		}
		break;
	case SS_LOOP:
		{
			if( m_dwLoopStartTime+m_dwLoopDuration < dwCurTime )
			{
				SetEndState( pChar );
				return;
			}
			else if( pChar->IsNeedProcess() && pChar->IsAttackKey() )
			{
				SetFireState( pChar );
				return;
			}

			CheckKeyInput( pChar );
			CheckRotate( pChar, pStage );
			CheckDummy( pChar, pStage );

			if ( m_bCheckDummyDie && m_bDieDummy )
			{
				SetEndState( pChar );
				return;
			}
		}
		break;
	case SS_FIRE:
		{
			if ( m_dwMotionEndTime && m_dwMotionEndTime < FRAMEGETTIME() )
			{
				if ( m_iMaxAttackCount > 0 && m_iCurAttackCount >= m_iMaxAttackCount )
					SetEndState( pChar );
				else
					SetLoopState( pChar, pStage );
				return;
			}

			CheckDummy( pChar, pStage );
			if ( m_bCheckDummyDie && m_bDieDummy )
			{
				SetEndState( pChar );
				return;
			}
		}
		break;
	}
}

void ioControlFireSkill5::OnSkillEnd( ioBaseChar *pChar )
{
	ioAttackSkill::OnSkillEnd( pChar );

	ClearData();

	EndCameraBuff( pChar );

	// 기본무기 장착
	if( m_bUseItemMesh )
		pChar->ShowWeaponItemMesh( true );

	if( !m_HandMesh.IsEmpty() )
		pChar->RestoreSkillWeaponMesh( this, m_HandMesh, m_bVisibleEquipMesh );

	g_DummyCharMgr.DestroyDummyCharByName( pChar, m_DummyCharName );
}

void ioControlFireSkill5::SetLoopState( ioBaseChar *pChar, ioPlayStage *pStage )
{
	if ( !pChar || !pStage )
		return;

	ioEntityGroup *pGrp = pChar->GetGroup();
	if( !pGrp ) 
		return;

	DWORD dwCurTime = FRAMEGETTIME();
	if ( m_dwLoopStartTime == 0 )
	{
		m_dwLoopStartTime = dwCurTime;
		m_fCurPositionOffset = D3DXVec3Length( &( pChar->GetWorldPosition() - m_vSkillStartPosition ) );
	}

	m_SkillState = SS_LOOP;
	
	//m_fCurMotionRate = 0.0f;

	pGrp->ClearAllActionAni( FLOAT100 );
	pGrp->ClearAllLoopAni( FLOAT100 );

	//pGrp->SetLoopAni( m_SkillDownLoopMotion, 0.0f, 0.0f );
	pGrp->SetLoopAni( m_SkillLoopMotion, 0.0f, FLOAT1 );
	
}

void ioControlFireSkill5::CheckKeyInput( ioBaseChar *pChar )
{
	if( !pChar || !pChar->IsNeedProcess() ) 
		return;

	bool bUpDownChange = CheckUpDownKey( pChar );
	bool bDirChange = CheckDirKey( pChar );

	if ( bUpDownChange || bDirChange )
	{
		if( pChar->IsNeedSendNetwork() )
		{
			SP2Packet kPacket( CUPK_SKILL_EXTRA_INFO );
			kPacket << pChar->GetCharName();
			kPacket << GetName();
			kPacket << static_cast<BYTE>( SST_DIRCHANGE );
			kPacket << m_bUp;
			kPacket << m_bDown;
			kPacket << m_bDir;
			kPacket << (int)m_CurDirKey;
			kPacket << m_bLeftRot;
			kPacket << m_bTargetRot;
			//kPacket << m_fTargetAttachAngleX;
			//kPacket << m_fTargetAttachAngleY;
			P2PNetwork::SendToAllPlayingUser( kPacket );
		}
	}
}

bool ioControlFireSkill5::CheckUpDownKey( ioBaseChar *pChar )
{
	if ( m_UpDownKeyType == UDKT_NONE )
	{
		//Up Key Check
		if ( m_bUp )
		{
			if ( !pChar->IsJumpKeyDown() )
			{
				m_bUp = false;
				return true;
			}
		}
		else if( pChar->IsJumpKeyDown() )
		{
			m_bUp = true;
			return true;
		}

		//Down Key Check
		if ( m_bDown )
		{
			if ( !pChar->IsDefenseKeyDown() )
			{
				m_bDown = false;
				return true;
			}
		}
		else if( !m_bUp && pChar->IsDefenseKeyDown() )
		{
			m_bDown = true;
			return true;
		}
	}
	else if ( m_UpDownKeyType = UDKT_AUTO_UP_KEY )
	{
		//Down Key Check
		if ( m_bDown )
		{
			if ( !pChar->IsDefenseKeyDown() )
			{
				m_bDown = false;
				m_bUp = true;
				return true;
			}
		}
		else if( pChar->IsDefenseKeyDown() )
		{
			m_bDown = true;
			m_bUp = false;
			return true;
		}		
	}

	return false;
}

bool ioControlFireSkill5::CheckDirKey( ioBaseChar *pChar )
{
	m_bDir = false;
	// Horizen Check
	if( pChar->IsSettedDirection() )
	{
		ioUserKeyInput::DirKeyInput eNewDirKey = pChar->CheckCurDirKey();
		if( eNewDirKey != m_CurDirKey )
		{
			float fYawD = 180.0f + 45.0f * ( eNewDirKey - ioUserKeyInput::DKI_UP );
			fYawD = ioMath::ArrangeHead( fYawD );

			float fYawR = DEGtoRAD( fYawD );
			D3DXQUATERNION qtRotate;
			D3DXQuaternionRotationYawPitchRoll( &qtRotate, fYawR, 0.0f, 0.0f );

			D3DXVECTOR3 vTargetDir = qtRotate * D3DXVECTOR3( 0.0f, 0.0f, -FLOAT1 );
			D3DXQUATERNION qtCurRot = pChar->GetTargetRot();
			D3DXVECTOR3 vDir = qtCurRot * D3DXVECTOR3( 0.0f, 0.0f, -FLOAT1 );

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

			// 모션처리
			m_bMotionSetted = false;

			m_CurDirKey = eNewDirKey;
			m_bDir = true;
			return true;
		}
	}
	else
	{
		if( !m_bMotionSetted )
		{
			m_bMotionSetted = true;
			m_CurDirKey = ioUserKeyInput::DKI_NONE;

			m_bDir = false;
			return true;
		}
	}

	return false;
}

bool ioControlFireSkill5::IsAttackEndState() const
{
	return IsEndState();
}

bool ioControlFireSkill5::IsEndState() const
{
	if ( m_SkillState == SS_END && m_dwFireMotionEndTime < FRAMEGETTIME() )
		return true;

	return false;
}

bool ioControlFireSkill5::IsSkillMotionEnd( DWORD dwActionStop, ioBaseChar* pOwner ) const
{
	if ( m_SkillState == SS_END && m_dwMotionEndTime < FRAMEGETTIME() )
		return true;
	return false;
}

bool ioControlFireSkill5::IsNoDropState() const
{
	return true;
}

void ioControlFireSkill5::CheckRotate( ioBaseChar *pChar, ioPlayStage *pStage )
{
	if ( !pChar || !pStage )
		return;

	float fPreAngleY = m_fTargetAttachAngleY;
	float fPreAngleX = m_fTargetAttachAngleX;
	bool bChangeHDir = CheckHDir( pChar );
	bool bChangeVDir = CheckVDir( pChar );

	if ( bChangeHDir || bChangeVDir )
	{
		D3DXQUATERNION qtRot;
		D3DXQuaternionRotationYawPitchRoll( &qtRot, DEGtoRAD( m_fTargetAttachAngleY ) ,DEGtoRAD( m_fTargetAttachAngleX ), 0.0f );
		D3DXVECTOR3 vPos = m_vSkillStartPosition + ( qtRot * D3DXVECTOR3( 0, 0, m_fCurPositionOffset ) );
		//Check MapCollision
		ioOrientBox kEntityBox = pChar->GetCollisionBox();
		kEntityBox.Transform( vPos,	pChar->GetWorldScale(), pChar->GetWorldOrientation() );
		if ( pStage->TestWorldMeshCollision( kEntityBox ) )
		{
			m_fTargetAttachAngleY = fPreAngleY;
			m_fTargetAttachAngleX = fPreAngleX;
		}
		else
		{
			//Direction
			if ( bChangeHDir )
			{
				D3DXVECTOR3 vDir = qtRot * D3DXVECTOR3( 0, 0, FLOAT1 );
				D3DXVec3Normalize( &vDir, &vDir );
				vDir.y = 0;
				D3DXVec3Normalize( &vDir, &vDir );
				pChar->SetTargetRotToDir( vDir, true );
			}
			//Position
			pChar->SetWorldPosition( vPos );
		}
		
	}
	else
	{
		m_fTargetAttachAngleY = fPreAngleY;
		m_fTargetAttachAngleX = fPreAngleX;
	}
}

bool ioControlFireSkill5::CheckHDir( ioBaseChar *pChar )
{
	if ( !pChar )
		return false;
	if ( m_dwDirKeyRotateSpeed == 0 || m_bTargetRot )
		return false;
	if( m_CurDirKey == ioUserKeyInput::DKI_NONE )
		return false;

	float fFrameGap = g_FrameTimer.GetCurFrameGap();
	float fDirRate = fFrameGap / m_dwDirKeyRotateSpeed;
	float fDirRotateAngle = 360.0f * fDirRate;

	float fYawD = 180.0f + 45.0f * ( m_CurDirKey - ioUserKeyInput::DKI_UP );
	fYawD = ioMath::ArrangeHead( fYawD );

	float fRotAngle = ioMath::AngleGap( m_fTargetAttachAngleY, fYawD );
	if( fabs( fRotAngle ) < fDirRotateAngle )
	{
		m_bTargetRot = true;
		return false;
	}

	if ( m_bLeftRot )
		fDirRotateAngle *= -1;
	m_fTargetAttachAngleY += fDirRotateAngle;
	m_fTargetAttachAngleY = ioMath::ArrangeHead( m_fTargetAttachAngleY );

	return true;
}

bool ioControlFireSkill5::CheckVDir( ioBaseChar *pChar )
{
	if ( !pChar )
		return false;
	if( m_dwUpDownRotateSpeed == 0 )
		return false;

	if ( !m_bUp && !m_bDown )
		return false;

	float fFrameGap = g_FrameTimer.GetCurFrameGap();
	float fUpDownRate = fFrameGap / m_dwUpDownRotateSpeed;
	float fUpDownRotateAngle = 360.0f * fUpDownRate;

	D3DXQUATERNION qtRot;
	if( m_bUp ) // 위로
	{
		m_fTargetAttachAngleX += fUpDownRotateAngle;
		if ( m_fTargetAttachAngleX ==FLOAT90 )
			m_fTargetAttachAngleX = 90.5f;
		else if ( m_fTargetAttachAngleX == 270.0f )
			m_fTargetAttachAngleX = 270.5f;

		if ( m_fTargetAttachAngleX >= m_fMaxLimitAngle )
		{
			m_fTargetAttachAngleX = m_fMaxLimitAngle;
			return false;
		}

	}
	else if( m_bDown ) // 아래로
	{
		m_fTargetAttachAngleX -= fUpDownRotateAngle;
		if ( m_fTargetAttachAngleX ==FLOAT90 )
			m_fTargetAttachAngleX = 89.5f;
		else if ( m_fTargetAttachAngleX == 270.0f )
			m_fTargetAttachAngleX = 269.5f;

		if ( m_fTargetAttachAngleX <= m_fMinLimitAngle )
		{
			m_fTargetAttachAngleX = m_fMinLimitAngle;
			return false;
		}
	}

	m_fTargetAttachAngleX = ioMath::ArrangeHead( m_fTargetAttachAngleX );

	return true;
}

void ioControlFireSkill5::SetFlyState( ioBaseChar *pChar )
{
	if ( !pChar )
		return;

	ioEntityGroup *pGrp = pChar->GetGroup();
	if( !pGrp ) 
		return;

	m_SkillState = SS_FLY;

	pGrp->ClearAllActionAni( FLOAT100 );
	pGrp->ClearAllLoopAni( FLOAT100 );

	float fTimeRate = FLOAT1;
	if ( m_fFlyMotionRate > 0.0f )
		fTimeRate = m_fFlyMotionRate;

	pGrp->SetLoopAni( m_szFlyMotion, 0.0f, FLOAT1, FLOAT1/fTimeRate );

	m_dwProtectTime = FRAMEGETTIME() + m_dwFlyProtectTime;
}

bool ioControlFireSkill5::ProcessFlyStart( ioBaseChar *pChar )
{
	if( !pChar )
		return false;

	DWORD dwCurTime = FRAMEGETTIME();
	float fTimePerSec = g_FrameTimer.GetSecPerFrame();

	D3DXVECTOR3 vTargetDir = pChar->GetTargetRot() * D3DXVECTOR3( 0.0f, 0.0f, -FLOAT1 );
	D3DXVec3Normalize( &vTargetDir, &vTargetDir );

	D3DXVECTOR3 vAxis;
	D3DXVec3Cross( &vAxis, &vTargetDir, &ioMath::UNIT_Y );
	D3DXVec3Normalize( &vAxis, &vAxis );

	D3DXQUATERNION qtNewRot;
	float fAngle = 180.0f + m_fStartAngle;
	float fCurAngle = ioMath::ArrangeHead( fAngle );
	D3DXQuaternionRotationAxis( &qtNewRot, &vAxis, DEGtoRAD(fCurAngle) );

	D3DXVECTOR3 vMoveDir = qtNewRot * vTargetDir;
	D3DXVec3Normalize( &vMoveDir, &vMoveDir );

	if( vMoveDir == ioMath::VEC3_ZERO )
		return false;

	if( m_dwFlyDuration == 0 )
		return false;

	float fCurSpeed = m_fFlyMaxRange / (float)m_dwFlyDuration * FLOAT1000;
	fCurSpeed = max( 0.0f, fCurSpeed );

	float fDistance = fCurSpeed * fTimePerSec;

	bool bRangeLimit = false;
	D3DXVECTOR3 vMoveAmt( 0.0f, 0.0f, 0.0f );
	if( m_fFlyCurRange+fDistance < m_fFlyMaxRange )
	{
		bRangeLimit = true;
		vMoveAmt = vMoveDir * fDistance;

		m_fFlyCurRange += fDistance;
	}
	else
	{
		bRangeLimit = false;

		float fGapDistance = m_fFlyMaxRange - m_fFlyCurRange;
		vMoveAmt = vMoveDir * fGapDistance;
	}

	bool bCol = false;
	ioPlayStage *pStage = pChar->GetCreator();
	if( pStage && pStage->ReCalculateMoveVectorByTerrain( pChar, &vMoveAmt, bCol ) )
		pChar->Translate( vMoveAmt );

	D3DXVECTOR3 vPos = pChar->GetWorldPosition();
	if( pChar->CheckAxisLimit( &vPos ) )
		pChar->SetWorldPosition( vPos );

	return bRangeLimit;
}

void ioControlFireSkill5::SetFireState( ioBaseChar *pChar )
{
	if ( !pChar )
		return;
	m_SkillState = SS_FIRE;

	m_iCurAttackCount++;

	AttackSkillFire( pChar );

	if( pChar->IsNeedSendNetwork() )
	{
		SP2Packet kPacket( CUPK_SKILL_EXTRA_INFO );
		kPacket << pChar->GetCharName();
		kPacket << GetName();
		kPacket << static_cast<BYTE>(SST_FIRE);
		P2PNetwork::SendToAllPlayingUser( kPacket );
	}
}

void ioControlFireSkill5::CreateDummy( ioBaseChar *pChar, ioPlayStage *pStage )
{
	if( !pChar || !pChar->IsNeedProcess() || !pStage ) 
		return;

	if( m_DummyCharName.IsEmpty() ) 
		return;

	g_DummyCharMgr.DestroyDummyCharByName( pChar, m_DummyCharName );

	int iCurIndex = g_DummyCharMgr.GetCurLocalIndex();
	m_iDummyIndex = iCurIndex;

	ioDummyChar *pDummy = pStage->CreateDummyChar( m_DummyCharName, m_iDummyIndex,
		pChar->GetCharName(),
		m_vSkillStartPosition,
		0.0f,
		0, true );

	if( pDummy )
		pDummy->SetControlSkillName( GetName() );

	ioPoleDummy* pPoleDummy = ToPoleDummy( pDummy );
	if ( pPoleDummy )
		pPoleDummy->SetYAngle( m_fTargetAttachAngleX );

	if( pChar->IsNeedSendNetwork() )
	{
		SP2Packet kPacket( CUPK_SKILL_EXTRA_INFO );
		kPacket << pChar->GetCharName();
		kPacket << GetName();
		kPacket << static_cast<BYTE>(SST_CREAT_DUMMY);
		kPacket << m_iDummyIndex;
		kPacket << m_fTargetAttachAngleX;
		P2PNetwork::SendToAllPlayingUser( kPacket );
	}
}

void ioControlFireSkill5::CheckDummy( ioBaseChar *pChar, ioPlayStage *pStage )
{
	if ( !pChar || !pStage )
		return;

	if( m_DummyCharName.IsEmpty() || m_iDummyIndex == -1 )
		return;

	m_bDieDummy = true;

	ioDummyChar *pDummy = g_DummyCharMgr.FindDummyChar( pChar, m_iDummyIndex );
	if( pDummy )
	{
		ioPoleDummy* pPoleDummy = ToPoleDummy( pDummy );
		if ( pPoleDummy )
			pPoleDummy->SetYAngle( m_fTargetAttachAngleX );
		m_bDieDummy = false;
	}
}

void ioControlFireSkill5::SetEndState( ioBaseChar *pChar )
{
	if ( !pChar )
		return;

	m_SkillState = SS_END;
	SetMotionTime( 0, 0 );
	m_dwMotionEndTime = FRAMEGETTIME() + 100;
	m_dwFireMotionEndTime = FRAMEGETTIME();
}

bool ioControlFireSkill5::IsProtected( int iDefenseBreakType ) const
{
	if( m_bDisableSkillProtect )
		return false;

	if( DBT_NONE != (DefenseBreakType)m_ProtectCancelType )
	{
		if( iDefenseBreakType == m_ProtectCancelType )   // 스킬 무적 파괴
			return false;
	}

	if( IsPreDelayProtect() )
	{
		if( m_dwPreDelayStartTime != 0 )
			return true;
	}

	if( m_dwPreDelayStartTime == 0 )
	{
		if( m_dwFireMotionEndTime > FRAMEGETTIME() )
			return true;

		if ( m_dwProtectTime > FRAMEGETTIME() )
			return true;
	}

	return false;
}

void ioControlFireSkill5::ApplyExtraSkillInfo( ioBaseChar *pChar, ioPlayStage *pStage, SP2Packet &rkPacket )
{
	BYTE btState;

	rkPacket >> btState;
	switch( btState )
	{
	case SST_CREAT_DUMMY:
		{
			rkPacket >> m_iDummyIndex;
			rkPacket >> m_fTargetAttachAngleX;
			g_DummyCharMgr.DestroyDummyCharByName( pChar, m_DummyCharName );

			ioDummyChar *pDummy = pStage->CreateDummyChar( m_DummyCharName, m_iDummyIndex,
				pChar->GetCharName(),
				m_vSkillStartPosition,
				0.0f,
				0, true );

			if( pDummy )
				pDummy->SetControlSkillName( GetName() );

			ioPoleDummy* pPoleDummy = ToPoleDummy( pDummy );
			if ( pPoleDummy )
				pPoleDummy->SetYAngle( m_fTargetAttachAngleX );
		}
		break;
	case SST_FIRE:
		SetFireState( pChar );
		break;
	case SST_DIRCHANGE:
		{
			int iDir = 0;
			rkPacket >> m_bUp;
			rkPacket >> m_bDown;
			rkPacket >> m_bDir;
			rkPacket >> iDir;
			rkPacket >> m_bLeftRot;
			//rkPacket >> m_fTargetAttachAngleX;
			//rkPacket >> m_fTargetAttachAngleY;
			m_CurDirKey = (ioUserKeyInput::DirKeyInput)iDir;
			if( m_CurDirKey == ioUserKeyInput::DKI_NONE )
				m_bMotionSetted = true;
			else
			{
				m_bTargetRot = false;
				m_bMotionSetted = false;
			}
			//LOG.PrintTimeAndLog( 0, "Up : %d, Down : %d, iDir:%d ", m_bUp, m_bDown, iDir );
		}
		break;
	}
}
