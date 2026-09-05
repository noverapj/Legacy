
#include "stdafx.h"

#include "ioRunningAttackSkill9.h"

#include "ioNpcChar.h"
#include "ioPlayStage.h"

#include "WeaponDefine.h"

ioRunningAttackSkill9::ioRunningAttackSkill9()
{
	Init();
}

ioRunningAttackSkill9::ioRunningAttackSkill9( const ioRunningAttackSkill9 &rhs )
	: ioAttackSkill( rhs ),
	  m_dwSkillProtectTime( rhs.m_dwSkillProtectTime ),
	  m_szRunReadyAni( rhs.m_szRunReadyAni ),
	  m_fRunReadyAniRate( rhs.m_fRunReadyAniRate ),
	  m_szRunLoopAni( rhs.m_szRunLoopAni ),
	  m_fRunLoopAniRate( rhs.m_fRunLoopAniRate ),
	  m_szRunChargeEndLoopAni( rhs.m_szRunChargeEndLoopAni ),
	  m_fRunChargeEndLoopAniRate( rhs.m_fRunChargeEndLoopAniRate ),
	  m_szRunEndAni( rhs.m_szRunEndAni ),
	  m_fRunEndAniRate( rhs.m_fRunEndAniRate ),
	  m_fNormalMoveSpeed( rhs.m_fNormalMoveSpeed ),
	  m_fChargeMoveSpeed( rhs.m_fChargeMoveSpeed),
	  m_fEndMoveSpeed( rhs.m_fEndMoveSpeed ),
	  m_fForceSameMoveSpeed( rhs.m_fForceSameMoveSpeed ),
	  m_dwNormalRotateSpeed( rhs.m_dwNormalRotateSpeed ),
	  m_dwChargeRotateSpeed( rhs.m_dwChargeRotateSpeed ),
	  m_dwEndRotateSpeed( rhs.m_dwEndRotateSpeed ),
	  m_dwForceSameRotateSpeed( rhs.m_dwForceSameRotateSpeed ),
	  m_dwMaxRunLoopTime( rhs.m_dwMaxRunLoopTime ),
	  m_dwMaxChargeTime( rhs.m_dwMaxChargeTime ),
	  m_dwMaxChargeRunLoopTime( rhs.m_dwMaxChargeRunLoopTime ),
	  m_fJumpPower( rhs.m_fJumpPower ),
	  m_dwJumpTime( rhs.m_dwJumpTime ),
	  m_fJumpHeightGap( rhs.m_fJumpHeightGap ),
	  m_AreaWeaponList( rhs.m_AreaWeaponList ),
	  m_dwCreateAreaTime( rhs.m_dwCreateAreaTime ),
	  m_bNoDropState( rhs.m_bNoDropState ),
	  m_bSetForceSameMove( rhs.m_bSetForceSameMove )
{
	Init();
}

ioRunningAttackSkill9::~ioRunningAttackSkill9()
{
}

void ioRunningAttackSkill9::LoadProperty( ioINILoader &rkLoader )
{
	ioAttackSkill::LoadProperty( rkLoader );

	char szBuf[MAX_PATH], szKey[MAX_PATH];

	m_dwSkillProtectTime = (DWORD)rkLoader.LoadInt_e( "skill_protect_time", 0 );

	rkLoader.LoadString_e( "run_ready_ani", "", szBuf, MAX_PATH );
	m_szRunReadyAni = szBuf;
	m_fRunReadyAniRate = rkLoader.LoadFloat_e( "run_ready_ani_rate", FLOAT1 );

	rkLoader.LoadString_e( "run_loop_ani", "", szBuf, MAX_PATH );
	m_szRunLoopAni = szBuf;
	m_fRunLoopAniRate = rkLoader.LoadFloat_e( "run_loop_ani_rate", FLOAT1 );

	rkLoader.LoadString_e( "run_charge_end_ani", "", szBuf, MAX_PATH );
	m_szRunChargeEndLoopAni = szBuf;
	m_fRunChargeEndLoopAniRate = rkLoader.LoadFloat_e( "run_charge_end_ani_rate", FLOAT1 );

	rkLoader.LoadString_e( "run_end_ani", "", szBuf, MAX_PATH );
	m_szRunEndAni = szBuf;
	m_fRunEndAniRate = rkLoader.LoadFloat_e( "run_end_ani_rate", FLOAT1 );

	m_fNormalMoveSpeed = rkLoader.LoadFloat_e( "run_normal_move_speed", 0.0f );
	m_fChargeMoveSpeed = rkLoader.LoadFloat_e( "run_charge_move_speed", 0.0f );
	m_fEndMoveSpeed = rkLoader.LoadFloat_e( "run_end_move_speed", 0.0f );
	m_fForceSameMoveSpeed = rkLoader.LoadFloat_e( "run_same_move_speed", 0.0f );

	m_dwNormalRotateSpeed = rkLoader.LoadInt_e( "run_normal_rotate_speed", 0 );
	m_dwChargeRotateSpeed = rkLoader.LoadInt_e( "run_charge_rotate_speed", 0 );
	m_dwEndRotateSpeed = rkLoader.LoadInt_e( "run_end_rotate_speed", 0 );
	m_dwForceSameRotateSpeed = rkLoader.LoadInt_e( "run_same_rotate_speed", 0 );

	m_dwMaxRunLoopTime = rkLoader.LoadInt_e( "run_loop_time", 0 );
	m_dwMaxChargeTime = rkLoader.LoadInt_e( "max_charge_time", 0 );
	m_dwMaxChargeRunLoopTime = rkLoader.LoadInt_e( "charge_run_loop_time", 0 );

	m_fJumpPower = rkLoader.LoadFloat_e( "run_charge_jump_power", FLOAT1 );
	m_dwJumpTime = rkLoader.LoadInt_e( "run_charge_jump_time", 0 );
	m_dwCreateAreaTime = rkLoader.LoadInt_e( "create_area_weapon_time", 0 );
	m_fJumpHeightGap = rkLoader.LoadFloat_e( "jump_height_gap", 0.0f );

	m_bNoDropState = rkLoader.LoadBool_e( "no_drop_state", false );
	m_bSetForceSameMove = rkLoader.LoadBool_e( "set_same_move", false );

	int nMaxAreaCnt = rkLoader.LoadInt_e( "max_area_weapon_cnt", 0 );
	for ( int i=0; i<nMaxAreaCnt; i++ )
	{
		wsprintf_e( szKey, "area_weapon%d", i+1 );
		rkLoader.LoadString( szKey, "", szBuf, MAX_PATH );
		m_AreaWeaponList.push_back( szBuf );
	}
}

ioSkill* ioRunningAttackSkill9::Clone()
{
	return new ioRunningAttackSkill9( *this );
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

bool ioRunningAttackSkill9::OnSkillStart( ioBaseChar *pChar )
{
	m_bSetHandMesh = false;

	Init();

	if( m_dwSkillProtectTime > 0 )
		m_dwCurSkillProtectTime = FRAMEGETTIME() + m_dwSkillProtectTime;

	if( !ioAttackSkill::OnSkillStart( pChar ) )
		return false;
	
	if( m_bUseItemMesh )
		pChar->ShowWeaponItemMesh( false );
	
	SetCameraBuff( pChar );

	return true;
}

void ioRunningAttackSkill9::OnProcessState( ioBaseChar *pChar, ioPlayStage *pStage )
{
	DWORD dwCurTime = FRAMEGETTIME();
	switch( m_SkillState )
	{
	case SS_None:
		{
			DWORD dwPreDelay = GetPreDelayTime();
			if( m_dwPreDelayStartTime + dwPreDelay < dwCurTime )
			{
				m_dwPreDelayStartTime = 0;
				EndPreDelayEffect( pChar );
				SetRunningReadyState( pChar );
			}
		}
		break;
	case SS_Run_Ready:
		{
			if ( m_dwMotionEndTime < dwCurTime )
				SetRunningNormalLoopState( pChar );
		}
		break;
	case SS_Run_Normal_Loop:
		{
			if ( m_dwCheckMaxRunLoopTime + m_dwMaxRunLoopTime < dwCurTime )
			{
				SetRunningEndState( pChar );
				return;
			}

			if ( CheckAttackInput( pChar ) )
			{
				SetRunningChargeLoopState( pChar );
				return;
			}

			if ( !m_bSetForceSameMove )
				ProcessLoopState( pChar, m_fNormalMoveSpeed, m_dwNormalRotateSpeed );
		}
		break;
	case SS_Run_Charge_Loop:
		{
			CheckChargeAreaWeapon( pChar );
			CheckChargeJump( pChar );

			if ( !m_bSetForceSameMove )
				ProcessLoopState( pChar, m_fChargeMoveSpeed, m_dwChargeRotateSpeed );

			if ( CheckAttackRelease( pChar ) || m_dwCheckMaxChargeTime + m_dwMaxChargeTime < dwCurTime )
			{
				SetRunningChargeEndState( pChar );
				return;
			}
		}
		break;
	case SS_Run_Charge_End:
		{
			if ( !IsFloatState( pChar ) || m_dwCheckMaxChargeRunLoopTime + m_dwMaxChargeRunLoopTime < dwCurTime )
			{
				if ( m_dwAreaWeaponIndex != -1 )
				{
					g_AreaWeaponMgr.DestroyAreaWeapon( m_dwAreaWeaponIndex );
					m_dwAreaWeaponIndex = -1;
				}
				m_nAreaWeaponCnt = 0;

				SetRunningNormalLoopState( pChar );
				return;
			}

			if ( !m_bSetForceSameMove )
				ProcessLoopState( pChar, m_fEndMoveSpeed, m_dwEndRotateSpeed );
		}
		break;
	case SS_Run_End:
		{
			if ( m_dwMotionEndTime < dwCurTime )
				m_SkillState = SS_End;
		}
		break;
	}

	if ( m_bSetForceSameMove )
	{
		switch( m_SkillState )
		{
		case SS_Run_Normal_Loop:
		case SS_Run_Charge_Loop:
		case SS_Run_Charge_End:
			ProcessLoopState( pChar, m_fForceSameMoveSpeed, m_dwForceSameRotateSpeed );
			break;
		}
	}
}

void ioRunningAttackSkill9::OnSkillEnd( ioBaseChar *pChar )
{
	ioAttackSkill::OnSkillEnd( pChar );

	EndCameraBuff( pChar );

	if ( m_dwAreaWeaponIndex != -1 )
	{
		g_AreaWeaponMgr.DestroyAreaWeapon( m_dwAreaWeaponIndex );
		m_dwAreaWeaponIndex = -1;
	}

	if( m_bUseItemMesh )
		pChar->ShowWeaponItemMesh( true );

	m_bSetHandMesh = false;
	if( !m_HandMesh.IsEmpty() )
		pChar->RestoreSkillWeaponMesh( this, m_HandMesh, m_bVisibleEquipMesh );
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void ioRunningAttackSkill9::ApplyExtraSkillInfo( ioBaseChar *pChar, ioPlayStage *pStage, SP2Packet &rkPacket )
{
	if( !pChar || !pStage ) 
		return;

	int nState;
	rkPacket >> nState;

	switch( nState )
	{
	case SS_Rotate:
		{
			int nDir;
			rkPacket >> nDir;
			rkPacket >> m_bLeftRot;

			m_CurDirKey = (ioUserKeyInput::DirKeyInput)nDir;

			if( m_CurDirKey != ioUserKeyInput::DKI_NONE )
				m_bTargetRot = false;
		}
		break;
	case SS_Run_Charge_Loop:
		{
			D3DXQUATERNION qtRot;
			rkPacket >> qtRot;

			pChar->SetTargetRotAndMoveDirChange( qtRot );

			D3DXVECTOR3 vPos;
			rkPacket >> vPos;

			pChar->SetWorldPosition( vPos );

			SetRunningChargeLoopState( pChar );
		}
		break;
	case SS_Run_Charge_End:
		{
			D3DXQUATERNION qtRot;
			rkPacket >> qtRot;

			pChar->SetTargetRotAndMoveDirChange( qtRot );

			D3DXVECTOR3 vPos;
			rkPacket >> vPos;

			pChar->SetWorldPosition( vPos );

			SetRunningChargeEndState( pChar );
		}
		break;
	}
}

void ioRunningAttackSkill9::ApplyAttributeToWeapon( ioWeapon *pWeapon, ioPlayStage *pStage )
{
	ioSkill::ApplyAttributeToWeapon( pWeapon, pStage );

	ioZoneEffectWeapon *pZone = ToZoneEffectWeapon( pWeapon );
	if( pZone )
	{
		ioBaseChar *pChar = pZone->GetOwner();
		if( pChar )
		{
			D3DXVECTOR3 vDir = pChar->GetTargetRot() * D3DXVECTOR3( 0.0f, 0.0f, -FLOAT1 );
			pZone->SetCollisionDir( vDir );
			pZone->SetZoneValue( false, true );
		}
	}
}

bool ioRunningAttackSkill9::IsSkillMotionEnd( DWORD dwActionStop, ioBaseChar* pOwner ) const
{
	if( m_dwPreDelayStartTime > 0 )
		return false;

	switch( m_SkillState )
	{
	case SS_None:
	case SS_Run_Ready:
	case SS_Run_Normal_Loop:
	case SS_Run_Charge_Loop:
	case SS_Run_Charge_End:
	case SS_Run_End:
		return false;
	case SS_End:
		if( m_fSkillEndNextJumpAmt == 0.0f )
		{
			return true;
		}
		else
		{
			float fHeightGap = pOwner->GetWorldPosition().y - pOwner->GetBottomHeight();

			if( m_fSkillEndNextJumpAmt > 0.0f )
			{
				if( fHeightGap > FLOAT10 )
					return false;
			}
		}
	}

	if( m_dwMotionEndTime + dwActionStop >= FRAMEGETTIME() )
		return false;

	return true;
}

bool ioRunningAttackSkill9::IsAttackEndState() const
{
	if( m_SkillState == SS_End )
		return true;

	return false;
}

bool ioRunningAttackSkill9::IsProtected( int iDefenseBreakType ) const
{
	if( m_bDisableSkillProtect )
		return false;

	if( DBT_NONE != (DefenseBreakType)m_ProtectCancelType )
	{
		if( iDefenseBreakType == m_ProtectCancelType )   // 스킬 무적 파괴
			return false;
	}

	DWORD dwCurTime = FRAMEGETTIME();

	if( IsPreDelayProtect() )
	{
		if( m_dwPreDelayStartTime != 0 )
			return true;
	}
	
	if( m_dwPreDelayStartTime == 0 )
	{		
		if( m_dwCurSkillProtectTime > dwCurTime )
			return true;
	}

	return false;
}

SkillCharColType ioRunningAttackSkill9::IsCharSkipState() const
{
	switch( m_SkillState )
	{
	case SS_Run_Charge_Loop:
	case SS_Run_Charge_End:
		return SCCT_ALL;
	}

	return SCCT_NONE;
}

bool ioRunningAttackSkill9::IsEntityCollisionSkipState() const
{
	switch( m_SkillState )
	{
	case SS_Run_Charge_Loop:
	case SS_Run_Charge_End:
		return true;
	}

	return false;
}

bool ioRunningAttackSkill9::IsNoFallDropZone() const
{	
	return m_bNoDropState;
}

bool ioRunningAttackSkill9::IsCanSendControl() const
{
	switch ( m_SkillState )
	{
	case SS_Run_Charge_Loop:
	case SS_Run_Charge_End:
		return false;
	}

	return true;
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void ioRunningAttackSkill9::Init()
{
	m_SkillState = SS_None;

	m_dwCurSkillProtectTime = 0;
	m_dwCheckMaxRunLoopTime = 0;
	m_dwCheckMaxChargeRunLoopTime = 0;
	m_dwCheckJumpTime = 0;
	m_dwCheckAreaWeaponTime = 0;

	m_bTargetRot = false;
	m_bLeftRot = false;
	m_CurDirKey = ioUserKeyInput::DKI_NONE;

	m_dwAreaWeaponIndex = -1;
	m_nAreaWeaponCnt = 0;
}

void ioRunningAttackSkill9::SetRunningReadyState( ioBaseChar *pChar )
{
	if ( !pChar )
		return;

	ioEntityGroup *pGrp = pChar->GetGroup();
	int iAniID = pGrp->GetAnimationIdx( m_szRunReadyAni );
	if ( iAniID == -1 )
		return;

	float fTimeRate = m_fRunReadyAniRate;

	pGrp->ClearAllActionAni( FLOAT100, true );
	pGrp->ClearAllLoopAni( FLOAT100, true );

	pGrp->SetActionAni( iAniID, FLOAT100, FLOAT100, FLOAT1, FLOAT1/fTimeRate );

	pChar->ClearReservedSliding();
	pChar->SetCurJumpPowerAmt( 0.0f );
	pChar->SetForcePowerAmt( 0.0f );
	pChar->SetGravityAmt( 0.0f );

	DWORD dwCurTime = FRAMEGETTIME();
	m_dwMotionEndTime = dwCurTime;
	m_dwMotionEndTime += pGrp->GetAnimationFullTime(iAniID) * fTimeRate;

	m_dwCheckMaxRunLoopTime = dwCurTime;

	m_SkillState = SS_Run_Ready;

	if( !m_bReduceGauge )
	{
		pChar->ReduceNeedGaugeBySkill( this, GetNeedGauge(pChar->GetCharName()) );
		m_bReduceGauge = true;
	}
}

void ioRunningAttackSkill9::SetRunningNormalLoopState( ioBaseChar *pChar )
{
	if ( !pChar )
		return;
	
	ioEntityGroup *pGrp = pChar->GetGroup();
	int iAniID = pGrp->GetAnimationIdx( m_szRunLoopAni );
	if ( iAniID == -1 )
		return;

	float fTimeRate = m_fRunLoopAniRate;

	pGrp->ClearAllActionAni( FLOAT100, true );
	pGrp->ClearAllLoopAni( FLOAT100, true );

	pGrp->SetLoopAni( iAniID, FLOAT100, FLOAT1, FLOAT1/fTimeRate );
	
	m_SkillState = SS_Run_Normal_Loop;
}

void ioRunningAttackSkill9::SetRunningChargeLoopState( ioBaseChar *pChar )
{
	if ( !pChar )
		return;
	
	DWORD dwCurTime = FRAMEGETTIME();
	m_SkillState = SS_Run_Charge_Loop;
	m_dwCheckJumpTime = 0;
	m_dwCheckAreaWeaponTime = 0;
	m_dwCheckMaxChargeTime = dwCurTime;

	if( pChar->IsNeedSendNetwork() )
	{
		SP2Packet kPacket( CUPK_SKILL_EXTRA_INFO );
		kPacket << pChar->GetCharName();
		kPacket << GetName();
		kPacket << SS_Run_Charge_Loop;
		kPacket << pChar->GetWorldOrientation();
		kPacket << pChar->GetWorldPosition();
		P2PNetwork::SendToAllPlayingUser( kPacket );
	}
}

void ioRunningAttackSkill9::SetRunningChargeEndState( ioBaseChar *pChar )
{
	if ( !pChar )
		return;

	ioEntityGroup *pGrp = pChar->GetGroup();
	int iAniID = pGrp->GetAnimationIdx( m_szRunChargeEndLoopAni );
	if ( iAniID == -1 )
		return;

	float fTimeRate = m_fRunChargeEndLoopAniRate;

	pGrp->ClearAllActionAni( FLOAT100, true );
	pGrp->ClearAllLoopAni( FLOAT100, true );

	pGrp->SetLoopAni( iAniID, FLOAT100, FLOAT1, FLOAT1/fTimeRate );
	
	m_SkillState = SS_Run_Charge_End;
	
	m_dwCheckMaxChargeRunLoopTime = FRAMEGETTIME();

	if( pChar->IsNeedSendNetwork() )
	{
		SP2Packet kPacket( CUPK_SKILL_EXTRA_INFO );
		kPacket << pChar->GetCharName();
		kPacket << GetName();
		kPacket << SS_Run_Charge_End;
		kPacket << pChar->GetWorldOrientation();
		kPacket << pChar->GetWorldPosition();
		P2PNetwork::SendToAllPlayingUser( kPacket );
	}
}

void ioRunningAttackSkill9::SetRunningEndState( ioBaseChar *pChar )
{
	if ( !pChar )
		return;

	pChar->SetCurMoveSpeed( 0.0f );

	ioEntityGroup *pGrp = pChar->GetGroup();
	int iAniID = pGrp->GetAnimationIdx( m_szRunEndAni );
	if ( iAniID == -1 )
		return;

	float fTimeRate = m_fRunEndAniRate;

	pGrp->ClearAllActionAni( FLOAT100, true );
	pGrp->ClearAllLoopAni( FLOAT100, true );

	pGrp->SetActionAni( iAniID, FLOAT100, FLOAT100, FLOAT1, FLOAT1/fTimeRate );

	pChar->SetCurJumpPowerAmt( 0.0f );
	pChar->SetForcePowerAmt( 0.0f );
	pChar->SetGravityAmt( 0.0f );	

	DWORD dwCurTime = FRAMEGETTIME();
	m_dwMotionEndTime = dwCurTime;
	m_dwMotionEndTime += pGrp->GetAnimationFullTime(iAniID) * fTimeRate;

	m_SkillState = SS_Run_End;
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void ioRunningAttackSkill9::ProcessLoopState( ioBaseChar *pChar, float fMoveSpeed, DWORD dwRotateSpeed )
{
	if ( !pChar )
		return;

	CheckRotateInput( pChar );
	CheckRotate( pChar, dwRotateSpeed );
	pChar->SetCurMoveSpeed( fMoveSpeed );
}

void ioRunningAttackSkill9::CheckRotateInput( ioBaseChar *pChar )
{
	if( !pChar || !pChar->IsNeedProcess() ) 
		return;

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
			D3DXVECTOR3 vDir = pChar->GetMoveDir();
			D3DXQUATERNION qtCurRot = pChar->GetTargetRot();

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

			if( pChar->IsNeedSendNetwork() )
			{
				SP2Packet kPacket( CUPK_SKILL_EXTRA_INFO );
				kPacket << pChar->GetCharName();
				kPacket << GetName();
				kPacket << SS_Rotate;
				kPacket << (int)m_CurDirKey;
				kPacket << m_bLeftRot;
				P2PNetwork::SendToAllPlayingUser( kPacket );
			}
		}
	}
	else
	{
		if( m_CurDirKey != ioUserKeyInput::DKI_NONE )
		{
			m_CurDirKey = ioUserKeyInput::DKI_NONE;

			if( pChar->IsNeedSendNetwork() )
			{
				SP2Packet kPacket( CUPK_SKILL_EXTRA_INFO );
				kPacket << pChar->GetCharName();
				kPacket << GetName();
				kPacket << SS_Rotate;
				kPacket << (int)m_CurDirKey;
				kPacket << m_bLeftRot;
				P2PNetwork::SendToAllPlayingUser( kPacket );
			}
		}
	}
}

void ioRunningAttackSkill9::CheckRotate( ioBaseChar *pChar, DWORD dwRotateSpeed )
{
	if( !pChar ) 
		return;

	if( dwRotateSpeed <= 0 ) 
		return;

	if( m_bTargetRot ) 
		return;

	if( m_CurDirKey == ioUserKeyInput::DKI_NONE )
		return;

	float fFrameGap = g_FrameTimer.GetCurFrameGap();
	float fRate = fFrameGap / dwRotateSpeed;
	float fRotateAngle = 360.0f * fRate;

	// 목표에 도달체크 및 처리
	float fYawD = 180.0f + 45.0f * ( m_CurDirKey - ioUserKeyInput::DKI_UP );
	fYawD = ioMath::ArrangeHead( fYawD );

	float fYawR = DEGtoRAD( fYawD );
	D3DXQUATERNION qtRotate;
	D3DXQuaternionRotationYawPitchRoll( &qtRotate, fYawR, 0.0f, 0.0f );

	D3DXQUATERNION qtCurRot = pChar->GetTargetRot();

	float fCurYaw, fTargetYaw;
	fCurYaw = RADtoDEG( ioMath::QuaterToYaw( qtCurRot ) );
	fTargetYaw = RADtoDEG( ioMath::QuaterToYaw( qtRotate ) );

	fCurYaw = ioMath::ArrangeHead( fCurYaw );
	fTargetYaw = ioMath::ArrangeHead( fTargetYaw );

	float fRotAngle = ioMath::AngleGap( fCurYaw, fTargetYaw );
	if( fabs( fRotAngle ) < fRotateAngle )
	{
		m_bTargetRot = true;
		pChar->SetTargetRotToRotate( qtRotate, true, false );
		return;
	}

	if( m_bLeftRot )
		fRotateAngle *= -1;

	fRotateAngle = ioMath::ArrangeHead( fRotateAngle );
	D3DXVECTOR3 vDir = pChar->GetMoveDir();

	D3DXQUATERNION qtAngle;
	D3DXQuaternionRotationYawPitchRoll( &qtAngle, DEGtoRAD(fRotateAngle), 0.0f, 0.0f );

	vDir = qtAngle * vDir;
	ioMath::CalcDirectionQuaternion( qtAngle, -vDir );

	pChar->SetTargetRotToRotate( qtAngle, true, false );
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

bool ioRunningAttackSkill9::CheckAttackInput( ioBaseChar *pChar )
{
	if ( !pChar || !pChar->IsNeedProcess() )
		return false;

	if ( pChar->IsAttackKey() )
		return true;

	return false;
}

bool ioRunningAttackSkill9::CheckAttackRelease( ioBaseChar *pChar )
{
	if ( !pChar || !pChar->IsNeedProcess() )
		return false;

	if ( !pChar->IsAttackKeyDown() )
		return true;

	return false;
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void ioRunningAttackSkill9::CheckChargeJump( ioBaseChar *pChar )
{
	if ( !pChar )
		return;

	DWORD dwCurTime = FRAMEGETTIME();
	if ( m_dwCheckJumpTime + m_dwJumpTime < dwCurTime )
	{
		m_dwCheckJumpTime = dwCurTime;
		pChar->SetJumpPower( m_fJumpPower );
	}
}

void ioRunningAttackSkill9::CheckChargeAreaWeapon( ioBaseChar *pChar )
{
	if ( !pChar )
		return;

	DWORD dwCurTime = FRAMEGETTIME();
	if ( m_dwCheckAreaWeaponTime + m_dwCreateAreaTime > dwCurTime )
		return;

	m_dwCheckAreaWeaponTime = dwCurTime;

	if ( COMPARE( m_nAreaWeaponCnt, 0, (int)m_AreaWeaponList.size() ) )
	{
		if ( m_dwAreaWeaponIndex != -1 )
		{
			g_AreaWeaponMgr.DestroyAreaWeapon( m_dwAreaWeaponIndex );
			m_dwAreaWeaponIndex = -1;
		}

		ioAreaWeapon *pAreaWeapon = g_AreaWeaponMgr.CreateAreaWeapon( pChar->GetCharName(),	pChar->GetCharName(), "", m_AreaWeaponList[m_nAreaWeaponCnt], pChar->GetWorldPosition(), pChar->GetWorldOrientation(), ioAreaWeapon::CT_TARGET_ATTACHED );
		if( pAreaWeapon )
			m_dwAreaWeaponIndex = pAreaWeapon->GetAreaWeaponIndex();
		
		m_nAreaWeaponCnt++;
	}
}

bool ioRunningAttackSkill9::IsFloatState( ioBaseChar *pChar )
{	
	if ( !pChar )
		return false;

	ioPlayStage *pStage = pChar->GetCreator();
	if ( !pStage )
		return false;

	D3DXVECTOR3 vPos = pChar->GetWorldPosition();
	float fMapHeight = pStage->GetMapHeightOnlyTerrain( vPos.x, vPos.z );

	if ( fabs( fMapHeight - vPos.y ) >= m_fJumpHeightGap )
		return true;

	return false;
}