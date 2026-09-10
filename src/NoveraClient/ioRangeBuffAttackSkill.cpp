

#include "stdafx.h"

#include "ioRangeBuffAttackSkill.h"

#include "ioBaseChar.h"
#include "ioPlayStage.h"

ioRangeBuffAttackSkill::ioRangeBuffAttackSkill()
{
	m_bLoopMoveState = false;

	m_dwAreaWeaponIndex = 0;
	m_CurDirKey = ioUserKeyInput::DKI_NONE;
}

ioRangeBuffAttackSkill::ioRangeBuffAttackSkill( const ioRangeBuffAttackSkill &rhs )
: ioRangeSkill( rhs ),
 m_szLoopDelayAni( rhs.m_szLoopDelayAni ),
 m_szLoopMoveAni( rhs.m_szLoopMoveAni ),
 m_szPullActionAni( rhs.m_szPullActionAni ),
 m_fPullActionAniRate( rhs.m_fPullActionAniRate ),
 m_szPullLoopAni( rhs.m_szPullLoopAni ),
 m_dwPullLoopDuration( rhs.m_dwPullLoopDuration ),
 m_szPullEndAni( rhs.m_szPullEndAni ),
 m_fPullEndAniRate( rhs.m_fPullEndAniRate ),
 m_AreaWeaponName( rhs.m_AreaWeaponName )
{
	m_bLoopMoveState = false;

	m_dwAreaWeaponIndex = 0;
	m_CurDirKey = ioUserKeyInput::DKI_NONE;
}

ioRangeBuffAttackSkill::~ioRangeBuffAttackSkill()
{
}

void ioRangeBuffAttackSkill::LoadProperty( ioINILoader &rkLoader )
{
	ioRangeSkill::LoadProperty( rkLoader );

	char szBuf[MAX_PATH];

	// 1차 루프
	rkLoader.LoadString_e( "loop_delay_animation", "", szBuf, MAX_PATH );
	m_szLoopDelayAni = szBuf;
	rkLoader.LoadString_e( "loop_move_animation", "", szBuf, MAX_PATH );
	m_szLoopMoveAni = szBuf;

	// 2차 액션 & 루프
	rkLoader.LoadString_e( "pull_action_ani", "", szBuf, MAX_PATH );
	m_szPullActionAni = szBuf;

	m_fPullActionAniRate = rkLoader.LoadFloat_e( "pull_action_ani_rate", FLOAT1 );

	rkLoader.LoadString_e( "pull_loop_ani", "", szBuf, MAX_PATH );
	m_szPullLoopAni = szBuf;

	m_dwPullLoopDuration = (DWORD)rkLoader.LoadInt_e( "pull_loop_duration", 0 );

	// 2차 End
	rkLoader.LoadString_e( "pull_end_ani", "", szBuf, MAX_PATH );
	m_szPullEndAni = szBuf;
	m_fPullEndAniRate = rkLoader.LoadFloat_e( "pull_end_ani_rate", FLOAT1 );

	// areaweapon
	rkLoader.LoadString_e( "area_weapon_name", "", szBuf, MAX_PATH );
	m_AreaWeaponName = szBuf;
}

ioSkill* ioRangeBuffAttackSkill::Clone()
{
	return new ioRangeBuffAttackSkill( *this );
}

void ioRangeBuffAttackSkill::SetBuffList( ioBaseChar *pOwner, ioPlayStage *pStage )
{
	if( !pOwner ) return;

	BaseCharList vTargetList;
	D3DXVECTOR3 vPos = pOwner->GetMidPositionByRate( FLOAT08 );
	vPos += m_vEffectDir * m_fOffSet;

	// 충돌타입에 따라 리스트 가져오기
	switch( m_TargetColType )
	{
	case TCT_SPHERE:
		pOwner->GetTargetCharListBySphere( vPos, m_fSkillRange, m_fSkillAngle, vTargetList );
		break;
	case TCT_CYLINDER:
		pOwner->GetTargetCharListByCylinder( vPos, m_fSkillRange, m_fSkillAngle, m_fUpHeight, m_fUnderHeight, vTargetList );
		break;
	case TCT_BOX:
		// 사각형의 center을 vPos로 설정하게 되므로 여기서 캐릭터앞쪽에 생성하고 높이를 보정한다.
		vPos += m_vEffectDir * m_fSkillRange * FLOAT05; 
		D3DXVECTOR3 vOffset( 0.0f, ( ( m_fUpHeight + m_fUnderHeight ) * FLOAT05 ) - m_fUpHeight, 0.0f );
		vPos -= vOffset;
		pOwner->GetTargetCharListByBox( vPos, m_fSkillRange, m_fWidth, m_fUpHeight, m_fUnderHeight, vTargetList );
		break;
	}

	if( pOwner->GetExperienceMode() == EMS_EXPERIENCE )
		vTargetList.clear();

	BaseCharList::iterator iter;
	for( iter = vTargetList.begin(); iter != vTargetList.end(); ++iter )
	{
		ioBaseChar *pChar = (*iter);

		if( !pChar || IsCollisioned(pChar) )	continue;
		if( !pChar->IsNeedProcess() )			continue;
		if( pChar->GetState() == CS_SKIP_STUN )	continue;
		if( Help::CheckTargetNoCollision( pChar->GetState() ) ) continue;
		if( !pChar->HasCollisionAvailableBuff() ) continue;
		if( !pStage->IsCollisionCheckByMode() )	continue;
		if( pOwner->GetCharName() == pChar->GetCharName() )
			continue;
		if( pChar->GetExperienceMode() == EMS_EXPERIENCE ) continue;
		if( pChar->IsSkipState() ) continue;

		D3DXVECTOR3 vStartPos = pOwner->GetMidPositionByRate();
		D3DXVECTOR3 vEndPos = pChar->GetMidPositionByRate();
		if( m_bCheckCollisionLine && !pStage->CheckCollisionLine( vStartPos, vEndPos ) )
			continue;

		if( !pChar->CheckTargetWoundType(m_TargetWoundType) )
			continue;

		bool bEnableTeamAttack = pChar->IsEnableTeamAttack();
		if( (bEnableTeamAttack || pOwner->GetTeam() != pChar->GetTeam()) )	// Enemy
		{
			bool bEnemyBuffSuccess = false;

			if( m_EnemyBuffType == EBT_NORMAL )
			{
				bEnemyBuffSuccess = SetEnemyBuffList( pOwner, pChar );
			}
			else if( m_EnemyBuffType == EBT_RANDOM )
			{
				bEnemyBuffSuccess = SetEnemyRandomBuffList( pOwner, pChar );
			}

			if( bEnemyBuffSuccess && m_bEnableAttackCnt )
			{
				pOwner->CheckComboSkillHit();
				pOwner->CheckAttackCnt( pChar );
			}
		}
	}
}

void ioRangeBuffAttackSkill::OnProcessState( ioBaseChar *pChar, ioPlayStage *pStage )
{
	DWORD dwCurTime = FRAMEGETTIME();
	DWORD dwPreDelayTime = GetPreDelayTime();

	switch( m_RangeState )
	{
	case RS_NONE:
		if( m_dwPreDelayStartTime > 0 && m_dwPreDelayStartTime + dwPreDelayTime < dwCurTime )
		{
			m_dwPreDelayStartTime = 0;
			EndPreDelayEffect( pChar );
			SetActionState( pChar );
		}
		break;
	case RS_ACTION:
		if( m_dwMotionEndTime > 0 &&m_dwMotionEndTime < dwCurTime )
		{
			m_dwMotionStartTime = 0;
			SetLoopState( pChar, pStage );
		}
		break;
	case RS_LOOP:
		if( m_dwLoopStartTime > 0 && m_dwLoopStartTime+m_dwLoopTime < dwCurTime )
		{
			m_dwLoopStartTime = 0;
			SetEndState( pChar );
			return;
		}
		else if( pChar->IsNeedProcess() )
		{
			if( pChar->IsAttackKey() )
			{
				SetActionState2( pChar, pStage );
				return;
			}
		}

		CheckLoopMoveAniState( pChar );
		break;
	case RS_ACTION2:
		if( m_dwMotionEndTime == 0 || m_dwMotionEndTime < dwCurTime )
		{
			m_dwMotionStartTime = 0;
			SetLoopState2( pChar, pStage );
		}
		break;
	case RS_LOOP2:
		if( m_dwLoopStartTime > 0 && m_dwLoopStartTime+m_dwPullLoopDuration < dwCurTime )
		{
			m_dwLoopStartTime = 0;
			SetEndState2( pChar, false );
			return;
		}
		else if( pChar->IsNeedProcess() )
		{
			if( !pChar->IsAttackKeyDown() )
			{
				SetEndState2( pChar, true );
				return;
			}
		}

		CheckKeyRot( pChar );
		break;
	case RS_END2:
		break;
	}
}

void ioRangeBuffAttackSkill::SetLoopState( ioBaseChar *pChar, ioPlayStage *pStage )
{
	if( !pChar )
		return;

	ioEntityGroup *pGrp = pChar->GetGroup();
	if( !pGrp )
		return;

	DWORD dwCurTime = FRAMEGETTIME();

	pGrp->ClearAllLoopAni( FLOAT100, true );
	pGrp->ClearAllActionAni( FLOAT100, true );

	m_bLoopMoveState = false;
	pGrp->SetLoopAni( m_szLoopDelayAni, FLOAT100 );

	CheckLoopMoveAniState( pChar );

	pChar->SetSkillMoveEnable( true, true );

	m_dwLoopStartTime = dwCurTime;
	m_dwProtectTime = m_dwLoopStartTime + m_dwLoopProtectDuration;

	m_RangeState = RS_LOOP;
}

bool ioRangeBuffAttackSkill::OnSkillStart( ioBaseChar *pChar )
{
	if( !ioRangeSkill::OnSkillStart(pChar) )
		return false;

	m_bLoopMoveState = false;
	m_dwAreaWeaponIndex = 0;
	m_CurDirKey = ioUserKeyInput::DKI_NONE;

	pChar->SetSkillMoveEnable( false, false );

	return true;
}

void ioRangeBuffAttackSkill::OnSkillEnd( ioBaseChar *pChar )
{
	if( !m_bReduceGauge && pChar )
	{
		pChar->ReduceNeedGaugeBySkill( this, GetNeedGauge(pChar->GetCharName()) );
		m_bReduceGauge = true;
	}

	m_CurDirKey = ioUserKeyInput::DKI_NONE;

	ioRangeSkill::OnSkillEnd( pChar );

	pChar->SetSkillMoveEnable( false, false );
	RemoveOwnerBuffList( pChar );

	if( m_dwAreaWeaponIndex > 0 )
	{
		g_AreaWeaponMgr.DestroyAreaWeapon( m_dwAreaWeaponIndex );
		m_dwAreaWeaponIndex = 0;
	}
}

void ioRangeBuffAttackSkill::ApplyExtraSkillInfo( ioBaseChar *pOwner, ioPlayStage *pStage, SP2Packet &rkPacket )
{
	int iSyncState;
	rkPacket >> iSyncState;

	switch( iSyncState )
	{
	case SS_STATE:
		{
			int iState;
			rkPacket >> iState;

			D3DXQUATERNION qtRot;
			rkPacket >> qtRot;

			switch( iState )
			{
			case RS_ACTION2:
				pOwner->SetTargetRotToRotate( qtRot, true );
				SetActionState2( pOwner, pStage );
				break;
			case RS_END2:
				pOwner->SetTargetRotToRotate( qtRot, true );
				SetEndState2( pOwner, false );
				break;
			}
		}
		break;
	case SS_KEY_ROT:
		{
			int nCurKey;
			rkPacket >>	nCurKey;
			m_CurDirKey = (ioUserKeyInput::DirKeyInput)nCurKey;
			pOwner->SetTargetRotToDirKey( m_CurDirKey );
		}
		break;
	}
}

void ioRangeBuffAttackSkill::SetActionState( ioBaseChar *pChar )
{
	ioEntityGroup *pGrp = pChar->GetGroup();
	ioHashString szSkillMotion = GetSkillMotionName();

	DWORD dwCurTime = FRAMEGETTIME();

	int iCurSkillAniID = pGrp->GetAnimationIdx( szSkillMotion );
	
	pGrp->ClearAllLoopAni( FLOAT100, true );
	pGrp->ClearAllActionAni( FLOAT100, true );

	if( iCurSkillAniID == -1 )
	{
		m_dwMotionStartTime = dwCurTime;
	}
	else
	{
		float fAniRate = GetSkillMotionRate();
		float fTimeRate = fAniRate * m_fExtraAniRate;

		pGrp->SetActionAni( iCurSkillAniID, FLOAT100, FLOAT100, FLOAT1, FLOAT1/fTimeRate );

		m_dwMotionStartTime = dwCurTime;
		m_dwMotionEndTime = m_dwMotionStartTime + pGrp->GetAnimationFullTime( iCurSkillAniID )*fTimeRate;
	}

	m_RangeState = RS_ACTION;

	pChar->AttachEffect( m_SkillEffectName );

	// create areaweapon & add ownerbuff
	CreateAreaWeapon( pChar );
	SetOwnerBuffList( pChar );

	// 사용 아이템 처리
	if( m_bUseItemMesh && !m_StartHandMesh.IsEmpty() )
	{
		pChar->ChangeSkillWeaponMesh( this, m_StartHandMesh, m_bVisibleEquipMesh );
		m_bSetHandMesh = true;
	}

	if( !m_bReduceGauge )
	{
		pChar->ReduceNeedGaugeBySkill( this, GetNeedGauge(pChar->GetCharName()) );
		m_bReduceGauge = true;
	}
}

void ioRangeBuffAttackSkill::SetEndState( ioBaseChar *pChar )
{
	DWORD dwCurTime = FRAMEGETTIME();
	ioEntityGroup *pGrp = pChar->GetGroup();
	int iCurSkillAniID = pGrp->GetAnimationIdx( m_EndAnimation );
	
	pGrp->ClearAllLoopAni( FLOAT100, true );
	pGrp->ClearAllActionAni( FLOAT100, true );

	if( iCurSkillAniID == -1 )
	{
		m_dwEndAniStartTime = dwCurTime;
		m_dwEndAniEndTime = m_dwEndAniStartTime;
	}
	else
	{
		pGrp->SetActionAni( iCurSkillAniID, FLOAT100, FLOAT100, FLOAT1, FLOAT1/m_fEndAniRate );
		m_dwEndAniStartTime = dwCurTime;
		m_dwEndAniEndTime = m_dwEndAniStartTime + pGrp->GetAnimationFullTime( iCurSkillAniID )*m_fEndAniRate;
	}

	m_RangeState = RS_END;
}

void ioRangeBuffAttackSkill::SetActionState2( ioBaseChar *pChar, ioPlayStage *pStage )
{
	ioEntityGroup *pGrp = pChar->GetGroup();
	DWORD dwCurTime = FRAMEGETTIME();

	int iCurSkillAniID = pGrp->GetAnimationIdx( m_szPullActionAni );
	
	pGrp->ClearAllLoopAni( FLOAT100, true );
	pGrp->ClearAllActionAni( FLOAT100, true );

	m_dwMotionStartTime = dwCurTime;
	m_dwMotionEndTime = dwCurTime;

	m_CurDirKey = ioUserKeyInput::DKI_NONE;

	if( iCurSkillAniID != -1 )
	{
		float fTimeRate = m_fPullActionAniRate;
		if( fTimeRate <= 0.0f || fTimeRate > FLOAT100 )
			fTimeRate = FLOAT1;

		pGrp->SetActionAni( iCurSkillAniID, FLOAT100, FLOAT100, FLOAT1, FLOAT1/fTimeRate );

		m_dwMotionStartTime = dwCurTime;
		float fTest = pGrp->GetAnimationFullTime( iCurSkillAniID )*fTimeRate;
		m_dwMotionEndTime = m_dwMotionStartTime + fTest;

	}

	m_RangeState = RS_ACTION2;

	pChar->SetSkillMoveEnable( false, false );

	// remove areaweapon
	if( m_dwAreaWeaponIndex > 0 )
	{
		g_AreaWeaponMgr.DestroyAreaWeapon( m_dwAreaWeaponIndex );
		m_dwAreaWeaponIndex = 0;
	}

	// check range
	SetBuffList( pChar, pStage );

	if( pChar->IsNeedSendNetwork() )
	{
		SP2Packet kPacket( CUPK_SKILL_EXTRA_INFO );
		kPacket << pChar->GetCharName();
		kPacket << GetName();
		kPacket << SS_STATE;
		kPacket << m_RangeState;
		kPacket << pChar->GetTargetRot();
		P2PNetwork::SendToAllPlayingUser( kPacket );
	}
}

void ioRangeBuffAttackSkill::SetLoopState2( ioBaseChar *pChar, ioPlayStage *pStage )
{
	if( !pChar )
		return;

	ioEntityGroup *pGrp = pChar->GetGroup();
	if( !pGrp )
		return;

	DWORD dwCurTime = FRAMEGETTIME();

	pGrp->ClearAllLoopAni( FLOAT100, true );
	pGrp->ClearAllActionAni( FLOAT100, true );

	pGrp->SetLoopAni( m_szPullLoopAni, FLOAT100 );

	m_dwLoopStartTime = dwCurTime;
	m_dwProtectTime = m_dwLoopStartTime + m_dwLoopProtectDuration;

	m_RangeState = RS_LOOP2;
}

void ioRangeBuffAttackSkill::SetEndState2( ioBaseChar *pChar, bool bSendNet )
{
	/*if( m_RangeState != RS_LOOP2 )
		return;*/

	DWORD dwCurTime = FRAMEGETTIME();

	m_dwEndAniStartTime = dwCurTime;
	m_dwEndAniEndTime = m_dwEndAniStartTime;

	if( !pChar )
		return;

	ioEntityGroup *pGrp = pChar->GetGroup();
	if( !pGrp )
		return;

	int iCurSkillAniID = pGrp->GetAnimationIdx( m_szPullEndAni );
	float fTimeRate = m_fPullEndAniRate;
	if( fTimeRate <= 0.0f )
		fTimeRate = FLOAT1;
	
	pGrp->ClearAllLoopAni( FLOAT100, true );
	pGrp->ClearAllActionAni( FLOAT100, true );

	if( iCurSkillAniID != -1 )
	{
		pGrp->SetActionAni( iCurSkillAniID, FLOAT100, FLOAT100, FLOAT1, FLOAT1/fTimeRate, 0.0f, 0.0f, true );

		m_dwEndAniEndTime = m_dwEndAniStartTime + pGrp->GetAnimationFullTime( iCurSkillAniID )*fTimeRate;
	}

	m_RangeState = RS_END2;

	RemoveOwnerBuffList( pChar );

	if( pChar->IsNeedSendNetwork() && bSendNet )
	{
		SP2Packet kPacket( CUPK_SKILL_EXTRA_INFO );
		kPacket << pChar->GetCharName();
		kPacket << GetName();
		kPacket << SS_STATE;
		kPacket << m_RangeState;
		kPacket << pChar->GetTargetRot();
		P2PNetwork::SendToAllPlayingUser( kPacket );
	}
}

void ioRangeBuffAttackSkill::RemoveOwnerBuffList( ioBaseChar *pOwner )
{
	if( !pOwner ) return;

	int iCnt = m_OwnerBuffList.size();
	for( int i=0; i < iCnt; ++i )
	{
		pOwner->RemoveBuff( m_OwnerBuffList[i].m_BuffName );
	}
}

void ioRangeBuffAttackSkill::CheckLoopMoveAniState( ioBaseChar *pOwner )
{
	if( !pOwner )
		return;

	ioEntityGroup *pGrp = pOwner->GetGroup();
	if( !pGrp )
		return;

	if( pOwner->IsSettedDirection() )
	{
		if( !m_bLoopMoveState )
		{
			m_bLoopMoveState = true;

			float fTimeRate = pOwner->GetRunDashAniRate();
			if( fTimeRate <= 0.0f )
				fTimeRate = FLOAT1;

			pGrp->ClearAllLoopAni( FLOAT100, true );
			pGrp->SetLoopAni( m_szLoopMoveAni, FLOAT100, FLOAT1, fTimeRate );
		}
	}
	else
	{
		if( m_bLoopMoveState )
		{
			m_bLoopMoveState = false;

			pGrp->ClearAllLoopAni( FLOAT100, true );
			pGrp->SetLoopAni( m_szLoopDelayAni, FLOAT100 );
		}
	}
}

void ioRangeBuffAttackSkill::CreateAreaWeapon( ioBaseChar *pOwner )
{
	if( m_AreaWeaponName.IsEmpty() ) return;
	if( !pOwner ) return;

	D3DXVECTOR3 vPos = pOwner->GetMidPositionByRate();
	ioAreaWeapon *pArea = g_AreaWeaponMgr.CreateAreaWeapon( pOwner->GetCharName(),
															pOwner->GetCharName(),
															GetName(),
															m_AreaWeaponName,
															vPos,
															pOwner->GetTargetRot(),
															ioAreaWeapon::CT_ATTACHED );

	if( pArea )
	{
		m_dwAreaWeaponIndex = pArea->GetAreaWeaponIndex();
	}
}

bool ioRangeBuffAttackSkill::IsProtected( int iDefenseBreakType ) const
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

	if( m_RangeState == RS_ACTION || m_RangeState == RS_ACTION2 )
		return true;

	DWORD dwCurTime = FRAMEGETTIME();
	if( m_RangeState == RS_LOOP )
	{
		if( m_dwLoopProtectDuration == 0 )
			return true;

		if( m_dwProtectTime > dwCurTime )
			return true;
	}
	
	if( m_RangeState == RS_END )
		return false;

	return false;
}

bool ioRangeBuffAttackSkill::IsSkillMotionEnd( DWORD dwActionStop, ioBaseChar* pOwner ) const
{
	if( m_RangeState != RS_END && m_RangeState != RS_END2 )
		return false;

	if( m_dwEndAniStartTime > 0 && m_dwEndAniEndTime < FRAMEGETTIME() )
		return true;

	return false;
}

void ioRangeBuffAttackSkill::CheckKeyRot( ioBaseChar *pChar )
{
	if( pChar->IsNeedProcess() && pChar->IsSettedDirection() )
	{
		ioUserKeyInput::DirKeyInput eNewDirKey = pChar->CheckCurDirKey();
		if( m_CurDirKey == eNewDirKey )
			return;

		m_CurDirKey = eNewDirKey;
		pChar->SetTargetRotToDirKey( eNewDirKey );

		if( pChar->IsNeedSendNetwork() )
		{
			SP2Packet kPacket( CUPK_SKILL_EXTRA_INFO );
			kPacket << pChar->GetCharName();
			kPacket << GetName();
			kPacket << SS_KEY_ROT;
			kPacket << (int)m_CurDirKey;
			P2PNetwork::SendToAllPlayingUser( kPacket );
		}
	}
}

