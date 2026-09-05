
#include "stdafx.h"

#include "ioRunningAttackSkill11.h"

#include "ioBaseChar.h"
#include "ioPlayStage.h"
#include "ioGrowthUpInfo.h"

#include "WeaponDefine.h"

ioRunningAttackSkill11::ioRunningAttackSkill11()
{
	Init();
}

ioRunningAttackSkill11::ioRunningAttackSkill11( const ioRunningAttackSkill11 &rhs )
	: ioAttackSkill( rhs ),
	  m_dwSkillProtectTime( rhs.m_dwSkillProtectTime ),
	  m_bDisableGrowth( rhs.m_bDisableGrowth ),
	  m_fGaugePerTic( rhs.m_fGaugePerTic ),
	  m_dwTicTime( rhs.m_dwTicTime ),
	  m_szRunReadyAni( rhs.m_szRunReadyAni ),
	  m_fRunReadyAniRate( rhs.m_fRunReadyAniRate ),
	  m_szRunLoopAni( rhs.m_szRunLoopAni ),
	  m_fRunLoopAniRate( rhs.m_fRunLoopAniRate ),
	  m_szRunChargeEndLoopAni( rhs.m_szRunChargeEndLoopAni ),
	  m_fRunChargeEndLoopAniRate( rhs.m_fRunChargeEndLoopAniRate ),
	  m_szRunEndAni( rhs.m_szRunEndAni ),
	  m_fRunEndAniRate( rhs.m_fRunEndAniRate ),
	  m_fRunMoveSpeed( rhs.m_fRunMoveSpeed ),
	  m_dwRunRotateSpeed( rhs.m_dwRunRotateSpeed ),
	  m_bNoDropState( rhs.m_bNoDropState ),
	  m_szReadyEffect( rhs.m_szReadyEffect ),
	  m_szAimedEffect( rhs.m_szAimedEffect ),
	  m_fTargetMinRange( rhs.m_fTargetMinRange ),
	  m_fTargetRange( rhs.m_fTargetRange ),
	  m_fTargetAngle( rhs.m_fTargetAngle ),
	  m_bEnableDownTarget( rhs.m_bEnableDownTarget ),
	  m_nMaxAttackCnt( rhs.m_nMaxAttackCnt ),
	  m_vAttackList( rhs.m_vAttackList ),
	  m_bSetUseCount( rhs.m_bSetUseCount )
{
	Init();
}

ioRunningAttackSkill11::~ioRunningAttackSkill11()
{
}

void ioRunningAttackSkill11::LoadProperty( ioINILoader &rkLoader )
{
	ioAttackSkill::LoadProperty( rkLoader );

	char szBuf[MAX_PATH];

	m_dwSkillProtectTime = (DWORD)rkLoader.LoadInt_e( "skill_protect_time", 0 );
	m_bDisableGrowth = rkLoader.LoadBool_e( "disable_growth", false );
	m_fGaugePerTic = rkLoader.LoadFloat_e( "gauge_per_tick", 0.0f );
	m_dwTicTime = (DWORD)rkLoader.LoadInt_e( "tick_time", 0 );

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

	m_fRunMoveSpeed = rkLoader.LoadFloat_e( "run_move_speed", 0.0f );
	m_dwRunRotateSpeed = rkLoader.LoadInt_e( "run_rotate_speed", 0 );
	
	m_bNoDropState = rkLoader.LoadBool_e( "no_drop_state", false );

	rkLoader.LoadString_e( "ready_marker", "", szBuf, MAX_PATH );
	m_szReadyEffect = szBuf;

	rkLoader.LoadString_e( "aimed_marker", "", szBuf, MAX_PATH );
	m_szAimedEffect = szBuf;

	m_fTargetMinRange = rkLoader.LoadFloat_e( "target_min_range", 0.0f );
	m_fTargetRange = rkLoader.LoadFloat_e( "target_range", 0.0f );
	m_fTargetAngle = rkLoader.LoadFloat_e( "target_angle", 0.0f );
	m_bEnableDownTarget = rkLoader.LoadBool_e( "target_enable_down", false );

	m_vAttackList.clear();
	m_nMaxAttackCnt = rkLoader.LoadInt_e( "max_attack_cnt", 0 );
	for ( int i=0; i<m_nMaxAttackCnt; i++ )
	{
		char szBuf[MAX_PATH] = "";
		wsprintf( szBuf, "attack%d_info", i+1 );

		AttackAttribute sAttackInfo;
		LoadAttackAttribute( szBuf, sAttackInfo, rkLoader );
		m_vAttackList.push_back( sAttackInfo );
	}

	m_bSetUseCount = rkLoader.LoadBool_e( "set_use_count", false );
}

ioSkill* ioRunningAttackSkill11::Clone()
{
	return new ioRunningAttackSkill11( *this );
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

bool ioRunningAttackSkill11::OnSkillStart( ioBaseChar *pChar )
{
	m_bSetHandMesh = false;

	Init();

	m_bUseActiveCnt = m_bSetUseCount;

	if( m_dwSkillProtectTime > 0 )
		m_dwCurSkillProtectTime = FRAMEGETTIME() + m_dwSkillProtectTime;

	ioPlayStage *pStage = pChar->GetCreator();
	if ( !pStage )
		return false;

	// 성장관련 요소 추가.
	if( !m_bDisableGrowth )
	{
		if( pStage )
		{
			ioGrowthNormalUpInfo *pUpInfo = NULL;
			pUpInfo = ToNormalUpInfo( GetGrowthUpInfoByType(GT_SKILL_COOL_TIME, pChar->GetCharName()) );
			if( pUpInfo )
				m_fTicGaugeRateByGrowth = pUpInfo->GetValue( pStage );

			m_fTicGaugeRateByGrowth = min( m_fTicGaugeRateByGrowth, FLOAT100 );
			m_fTicGaugeRateByGrowth = FLOAT1 + ( m_fTicGaugeRateByGrowth/FLOAT100 );

			pUpInfo = ToNormalUpInfo( GetGrowthUpInfoByType(GT_BUFF_TIC_TIME, pChar->GetCharName()) );
			if( pUpInfo )
				m_dwTicTimeByGrowth = pUpInfo->GetValue( pStage );			
		}
	}

	if( !ioAttackSkill::OnSkillStart( pChar ) )
		return false;
	
	SetCameraBuff( pChar );

	if( m_bUseItemMesh )
		pChar->ShowWeaponItemMesh( false );

	if( !m_bReduceGauge )
	{
		pChar->ReduceNeedGaugeBySkill( this, GetNeedGauge(pChar->GetCharName()) );
		m_bReduceGauge = true;
	}

	return true;
}

void ioRunningAttackSkill11::OnProcessState( ioBaseChar *pChar, ioPlayStage *pStage )
{
	DWORD dwCurTime = FRAMEGETTIME();
	float fCurTicGauge = m_fGaugePerTic / m_fTicGaugeRateByGrowth;
	int nCurTicTime = m_dwTicTime + m_dwTicTimeByGrowth;

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
				SetRunningLoopState( pChar );
		}
		break;
	case SS_Run_Loop:
		{
			if ( pChar->IsNeedProcess() )
			{
				if( m_dwCheckGaugeTime + nCurTicTime <= dwCurTime )
				{
					m_dwCheckGaugeTime = dwCurTime;

					if( fCurTicGauge != 0.0f )
					{
						if( pChar->ReduceCurSkillGaugeBySkill( GetOwnerItem(), fCurTicGauge ) <= 0.0f )
						{
							SetRunningEndState( pChar );
							return;
						}
					}
				}
			}

			FindTarget( pChar );

			if ( CheckInputKey( pChar ) )
				return;
			
			ProcessLoopState( pChar, m_fRunMoveSpeed, m_dwRunRotateSpeed );
		}
		break;
	case SS_Attack:
		{
			if ( m_dwMotionEndTime < dwCurTime )
			{
				if (COMPARE( m_nCurAttackCnt, 0, m_nMaxAttackCnt ) )
					SetRunningLoopState( pChar );
				else
				{
					if ( pChar->IsNeedProcess() )
						SetRunningEndState( pChar );
				}
				return;
			}

			if ( m_dwWeaponFireTime > 0 && m_dwWeaponFireTime < dwCurTime )
			{
				if (COMPARE( m_nCurAttackCnt, 0, m_nMaxAttackCnt ) )
					CreateWeapon( pChar, m_vAttackList[m_nCurAttackCnt] );
				m_dwWeaponFireTime = 0;
			}
		}
		break;
	case SS_Run_End:
		{
			if ( m_dwMotionEndTime < dwCurTime )
				m_SkillState = SS_End;
		}
		break;
	}
}

void ioRunningAttackSkill11::OnSkillEnd( ioBaseChar *pChar )
{
	ioAttackSkill::OnSkillEnd( pChar );

	EndCameraBuff( pChar );
	
	if( m_bUseItemMesh )
		pChar->ShowWeaponItemMesh( true );

	m_bSetHandMesh = false;
	if( !m_HandMesh.IsEmpty() )
		pChar->RestoreSkillWeaponMesh( this, m_HandMesh, m_bVisibleEquipMesh );

	pChar->SetTargetMarkerState( ioTargetMarker::MS_NONE, TT_OTHER );

	m_bUseActiveCnt = false;

	ioItem *pOwnerItem = GetOwnerItem();
	if ( pOwnerItem )
		pChar->SetCurItemSkillGauge( pOwnerItem->GetName(), 0.0f );
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void ioRunningAttackSkill11::ApplyExtraSkillInfo( ioBaseChar *pChar, ioPlayStage *pStage, SP2Packet &rkPacket )
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
	case SS_Attack:
		{
			D3DXVECTOR3 vPos;
			rkPacket >> vPos;

			pChar->SetWorldPosition( vPos );

			D3DXQUATERNION qtRot;
			rkPacket >> qtRot;

			pChar->SetTargetRotAndMoveDirChange( qtRot );

			rkPacket >> m_szTargetName;

			rkPacket >> m_nCurAttackCnt;
			
			if (COMPARE( m_nCurAttackCnt, 0, m_nMaxAttackCnt ) )
				SetAttackState( pChar, m_vAttackList[m_nCurAttackCnt] );
		}
		break;
	case SS_Run_End:
		{
			D3DXVECTOR3 vPos;
			rkPacket >> vPos;

			pChar->SetWorldPosition( vPos );

			D3DXQUATERNION qtRot;
			rkPacket >> qtRot;

			pChar->SetTargetRotAndMoveDirChange( qtRot );

			SetRunningEndState( pChar );
		}
		break;
	}
}

void ioRunningAttackSkill11::ApplyAttributeToWeapon( ioWeapon *pWeapon, ioPlayStage *pStage )
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

	ioBombMissileWeapon3* pBombMissileWeapon3 = ToBombMissileWeapon3( pWeapon );
	if( pBombMissileWeapon3 )
		pBombMissileWeapon3->SetDsiableChangeDir( true );
}

bool ioRunningAttackSkill11::IsSkillMotionEnd( DWORD dwActionStop, ioBaseChar* pChar ) const
{
	if( m_dwPreDelayStartTime > 0 )
		return false;

	switch( m_SkillState )
	{
	case SS_None:
	case SS_Run_Ready:
	case SS_Run_Loop:
	case SS_Run_End:
	case SS_Attack:
		return false;
	case SS_End:
		if( m_fSkillEndNextJumpAmt == 0.0f )
			return true;
		else
			return false;
	}

	if( m_dwMotionEndTime + dwActionStop >= FRAMEGETTIME() )
		return false;

	return true;
}

bool ioRunningAttackSkill11::IsAttackEndState() const
{
	if( m_SkillState == SS_End )
		return true;

	return false;
}

bool ioRunningAttackSkill11::IsProtected( int iDefenseBreakType ) const
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

bool ioRunningAttackSkill11::IsNoDropState() const
{
	return m_bNoDropState;
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void ioRunningAttackSkill11::Init()
{
	m_dwCurSkillProtectTime = 0;
	m_dwCheckGaugeTime = 0;
	m_fTicGaugeRateByGrowth	= FLOAT1;
	m_dwTicTimeByGrowth = 0;

	m_bSetHandMesh = false;

	m_SkillState = SS_None;

	m_dwCurSkillProtectTime = 0;

	m_bTargetRot = false;
	m_bLeftRot = false;
	m_CurDirKey = ioUserKeyInput::DKI_NONE;

	m_nCurAttackCnt = 0;

	m_dwMotionEndTime = 0;
	m_dwWeaponFireTime = 0;
	m_bUseActiveCnt = false;
	m_szTargetName.Clear();
}

void ioRunningAttackSkill11::SetRunningReadyState( ioBaseChar *pChar )
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
	
	m_SkillState = SS_Run_Ready;

	if( !m_bReduceGauge )
	{
		pChar->ReduceNeedGaugeBySkill( this, GetNeedGauge(pChar->GetCharName()) );
		m_bReduceGauge = true;
	}
	
	pChar->SetTargetMarker( ioTargetMarker::MT_OTHER_MARKER, m_szReadyEffect, "", m_szAimedEffect, "", "", TT_OTHER );
	pChar->SetTargetMarkerRange( m_fTargetRange, TT_OTHER );
	pChar->SetTargetMarkerState( ioTargetMarker::MS_NONE, TT_OTHER );
	pChar->SetTargetMarkerTarget( m_szTargetName, false, TT_OTHER );
}

void ioRunningAttackSkill11::SetRunningLoopState( ioBaseChar *pChar )
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
	
	m_SkillState = SS_Run_Loop;
	m_dwCheckGaugeTime = FRAMEGETTIME();
}

void ioRunningAttackSkill11::SetRunningEndState( ioBaseChar *pChar )
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
	pChar->SetCurMoveSpeed( 0 );

	DWORD dwCurTime = FRAMEGETTIME();
	m_dwMotionEndTime = dwCurTime;
	m_dwMotionEndTime += pGrp->GetAnimationFullTime(iAniID) * fTimeRate;

	m_SkillState = SS_Run_End;

	if ( pChar->IsNeedSendNetwork() )
	{
		SP2Packet kPacket( CUPK_SKILL_EXTRA_INFO );
		kPacket << pChar->GetCharName();
		kPacket << GetName();
		kPacket << SS_Run_End;
		kPacket << pChar->GetWorldPosition();
		kPacket << pChar->GetWorldOrientation();
		P2PNetwork::SendToAllPlayingUser( kPacket );
	}
}

void ioRunningAttackSkill11::SetAttackState( ioBaseChar *pChar, const AttackAttribute &sAttribute )
{
	if( !pChar )	
		return;

	ioEntityGroup *pGrp = pChar->GetGroup();
	int nAniID = pGrp->GetAnimationIdx( sAttribute.m_AttackAnimation );
	if( nAniID == -1 )	
		return;

	float fTimeRate  = sAttribute.m_fAttackAniRate;
	DWORD dwPreDelay = sAttribute.m_dwPreDelay;

	pGrp->ClearAllLoopAni( FLOAT100, true );
	pGrp->ClearAllActionAni( FLOAT100, true );

	pGrp->SetActionAni( nAniID, FLOAT100, FLOAT100, FLOAT1, FLOAT1/fTimeRate, 0.0f, dwPreDelay );

	pChar->SetCurJumpPowerAmt( 0.0f );
	pChar->SetForcePowerAmt( 0.0f );
	pChar->SetGravityAmt( 0.0f );
	pChar->SetCurMoveSpeed( 0 );

	DWORD dwCurTime = FRAMEGETTIME();
	m_dwMotionEndTime = dwCurTime;
	m_dwMotionEndTime += pGrp->GetAnimationFullTime( nAniID ) * fTimeRate;
	m_dwMotionEndTime += dwPreDelay;
	
	m_dwWeaponFireTime = dwCurTime;
	m_dwWeaponFireTime += pGrp->GetAnimationEventTime_e( nAniID, "fire_s" ) * fTimeRate;
	m_dwWeaponFireTime += dwPreDelay;

	m_SkillState = SS_Attack;

	pChar->SetTargetMarkerState( ioTargetMarker::MS_NONE, TT_OTHER );

	if ( pChar->IsNeedSendNetwork() )
	{
		SP2Packet kPacket( CUPK_SKILL_EXTRA_INFO );
		kPacket << pChar->GetCharName();
		kPacket << GetName();
		kPacket << SS_Attack;
		kPacket << pChar->GetWorldPosition();
		kPacket << pChar->GetWorldOrientation();
		kPacket << m_szTargetName;
		kPacket << m_nCurAttackCnt;
		P2PNetwork::SendToAllPlayingUser( kPacket );
	}
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void ioRunningAttackSkill11::ProcessLoopState( ioBaseChar *pChar, float fMoveSpeed, DWORD dwRotateSpeed )
{
	if ( !pChar )
		return;

	CheckRotateInput( pChar );
	CheckRotate( pChar, dwRotateSpeed );
	pChar->SetCurMoveSpeed( fMoveSpeed );
}

void ioRunningAttackSkill11::CheckRotateInput( ioBaseChar *pChar )
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

void ioRunningAttackSkill11::CheckRotate( ioBaseChar *pChar, DWORD dwRotateSpeed )
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

bool ioRunningAttackSkill11::CheckInputKey( ioBaseChar *pChar )
{
	if ( !pChar || !pChar->IsNeedProcess() )
		return false;

	ioItem *pOwnerItem = GetOwnerItem();
	if ( !pOwnerItem )
		return false;

	if (!COMPARE( m_nCurAttackCnt, 0, m_nMaxAttackCnt ) )
		return false;

	if ( pChar->GetSkillKeyInput() == pOwnerItem->GetType()-1 )
	{
		m_nCurAttackCnt = m_nMaxAttackCnt-1;
		SetAttackState( pChar, m_vAttackList[m_nCurAttackCnt] );
		return true;
	}
	else if ( pChar->IsAttackKey() )
	{
		SetAttackState( pChar, m_vAttackList[m_nCurAttackCnt] );
		return true;
	}

	return false;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void ioRunningAttackSkill11::FindTarget( ioBaseChar *pChar )
{
	if ( !pChar )
		return;
		
	ioHashString szTarget;
	if( pChar->CheckAimedTarget( m_fTargetAngle, m_fTargetMinRange, m_fTargetRange, szTarget, m_bEnableDownTarget ) )
	{
		if ( m_szTargetName == szTarget )
			return;

		m_szTargetName = szTarget;
		pChar->SetTargetMarkerTarget( m_szTargetName, false, TT_OTHER );
		pChar->SetTargetMarkerState( ioTargetMarker::MS_AIMED, TT_OTHER );		
	}
	else
	{
		m_szTargetName.Clear();
		pChar->SetTargetMarkerState( ioTargetMarker::MS_READY, TT_OTHER );
	}
}

void ioRunningAttackSkill11::CreateWeapon( ioBaseChar *pChar, const AttackAttribute &sAttribute )
{
	if ( !pChar )
		return;

	if ( sAttribute.m_vWeaponInfoList.empty() )
		return;

	WeaponInfo sWeaponInfo = sAttribute.m_vWeaponInfoList[0];

	DWORD dwWeaponIdx = pChar->IncreaseWeaponIndexBase();

	FireTime kFireTime;
	kFireTime.dwStart = FRAMEGETTIME();
	kFireTime.iAttributeIdx = sWeaponInfo.m_iWeaponIdx;
	kFireTime.iResistanceIdx = sWeaponInfo.m_iResistanceIdx;
	kFireTime.szWoundedAni = sWeaponInfo.m_WoundedAnimation;
	kFireTime.dwWoundedAniDuration = sWeaponInfo.m_dwWoundedDuration;
	kFireTime.bLoopWoundedAni = sWeaponInfo.m_bWoundedLoopAni;
	kFireTime.eFireTimeType = FTT_SKILL_ATTACK;
	kFireTime.dwWeaponIdx = dwWeaponIdx++;
	
	D3DXVECTOR3 vPos = pChar->GetMidPositionByRate();
	D3DXVECTOR3 vDir = pChar->GetWorldOrientation() * D3DXVECTOR3( 0.0f, 0.0f, -FLOAT1 );

	ioWeapon *pWeapon = pChar->ExtendFireExplicit( kFireTime, vPos, vDir, "" );
	if ( pWeapon )
	{
		if ( !m_szTargetName.IsEmpty() )
		{
			ioBaseChar *pTarget = pChar->GetBaseChar( m_szTargetName );
			if ( pTarget )
			{		
				D3DXVECTOR3 vFireDir =  pTarget->GetMidPositionByRate() - pWeapon->GetPosition();
				D3DXVec3Normalize( &vFireDir, &vFireDir );
				pWeapon->SetMoveDir( vFireDir );
			}
		}
	}

	m_nCurAttackCnt++;
	m_szTargetName.Clear();
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

bool ioRunningAttackSkill11::IsUseActiveCnt() const
{
	return m_bUseActiveCnt;
}

int ioRunningAttackSkill11::GetMaxActiveCnt( ioBaseChar *pChar ) const
{
	return m_nMaxAttackCnt;
}

int ioRunningAttackSkill11::GetCurActiveCnt( ioBaseChar *pChar ) const
{
	return m_nCurAttackCnt;
}