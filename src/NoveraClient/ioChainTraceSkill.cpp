

#include "stdafx.h"

#include "ioChainTraceSkill.h"

#include "ioPlayStage.h"
#include "ioBaseChar.h"

#include "WeaponDefine.h"

ioChainTraceSkill::ioChainTraceSkill()
{
	m_dwFireStartTime = 0;
	m_dwFireMotionEndTime = 0;
	m_bSetHandMesh = false;
	m_dwFireEndTime = 0;
}

ioChainTraceSkill::ioChainTraceSkill( const ioChainTraceSkill &rhs )
	: ioAttackSkill( rhs ),
	m_fTargetRange( rhs.m_fTargetRange ),
	m_fTargetRange2( rhs.m_fTargetRange2 ),
	m_fTargetMinRange( rhs.m_fTargetMinRange ),
	m_fTargetAngle( rhs.m_fTargetAngle ),
	m_TargetWoundType( rhs.m_TargetWoundType ),
	m_nMaxHitCnt( rhs.m_nMaxHitCnt ),
	m_dwDelayHitTime( rhs.m_dwDelayHitTime ),
	m_stHitLoopAni( rhs.m_stHitLoopAni ),
	m_dwMaxWaitTime( rhs.m_dwMaxWaitTime ),
	m_stAddAttackAni( rhs.m_stAddAttackAni ),
	m_fAddAttackAniRate( rhs.m_fAddAttackAniRate ),
	m_dwAddAttackAniPreDelay( rhs.m_dwAddAttackAniPreDelay ),
	m_nAddAttackWeapon( rhs.m_nAddAttackWeapon ),
	m_dwProtectTime( rhs.m_dwProtectTime )
{
	m_dwFireStartTime = 0;
	m_dwFireMotionEndTime = 0;
	m_bSetHandMesh = false;
	m_dwFireEndTime = 0;
	m_state = SS_NONE;
	m_Target.Clear();
}

ioChainTraceSkill::~ioChainTraceSkill()
{
}

void ioChainTraceSkill::LoadProperty( ioINILoader &rkLoader )
{
	ioAttackSkill::LoadProperty( rkLoader );

	char szBuf[MAX_PATH] = "";

	m_fTargetAngle = rkLoader.LoadFloat_e( "target_angle", 0.0f );
	m_fTargetRange = rkLoader.LoadFloat_e( "target_max_range", 0.0f );
	m_fTargetRange2 = rkLoader.LoadFloat_e( "target_max_range2", 0.0f );
	m_fTargetMinRange = rkLoader.LoadFloat_e( "target_min_range", 0.0f );
	m_fTargetMinRange = min( m_fTargetMinRange, m_fTargetRange );

	m_nMaxHitCnt = rkLoader.LoadInt_e( "target_cnt", 1 );

	m_dwDelayHitTime = (DWORD)rkLoader.LoadInt_e( "loop_hit_next_time", 0 );
	m_dwMaxWaitTime = (DWORD)rkLoader.LoadInt_e( "loop_max_time", 0 );

	rkLoader.LoadString_e( "loop_attack_actionAni", "", szBuf, MAX_PATH );
	m_stHitLoopAni = szBuf;


	rkLoader.LoadString_e( "add_attack_actionAni", "", szBuf, MAX_PATH );
	m_stAddAttackAni = szBuf;
	m_fAddAttackAniRate = rkLoader.LoadFloat_e( "add_attack_actionAniRate", FLOAT1 );

	m_dwAddAttackAniPreDelay = (DWORD)rkLoader.LoadInt_e("add_attack_actionAniPreDelay", 0 );
	m_nAddAttackWeapon = rkLoader.LoadInt_e( "add_attack_action", 0 );
	

	m_TargetWoundType = (TargetWoundType)rkLoader.LoadInt_e( "target_wound_type", TWT_ALL );

	m_dwProtectTime = rkLoader.LoadInt_e( "skill_protect_time", 0 );
}

ioSkill* ioChainTraceSkill::Clone()
{
	return new ioChainTraceSkill( *this );
}

bool ioChainTraceSkill::OnSkillStart( ioBaseChar *pChar )
{
	if( !ioAttackSkill::OnSkillStart( pChar ) )
		return false;

	m_vTargetList.clear();
	m_CurTargetName.Clear();
	m_Target.Clear();
	m_vWeaponIndexList.clear();

	m_dwFireStartTime = 0;
	m_dwFireEndTime = 0;
	m_dwFireMotionEndTime = 0;
	m_nHitCnt = 0;
	m_dwHitTime = 0;
	m_dwFirstHitTime = 0;
	m_dwNextAutoFireTime = 0;
	m_fPrevHitHeight = 0.0f;
	m_dwAddAttackFireTime = 0;
	m_dwProtectEndTime = 0;

	m_CurDirKey = ioUserKeyInput::DKI_NONE;

	m_bSetHandMesh = false;

	// 기본 무기 손에서 제거
	if( m_bUseItemMesh )
	{
		pChar->ShowWeaponItemMesh( false );
	}

	// 동기화 되는 시간을 벌기 위해 predelay 시점에 타겟 선정 필요.
	//FindFirstTarget( pChar );

	m_state = SS_PREDELAY;
	
	m_dwProtectEndTime = FRAMEGETTIME() + GetPreDelayTime() + m_dwProtectTime;

	if( !m_bReduceGauge )
	{
		pChar->ReduceNeedGaugeBySkill( this, GetNeedGauge(pChar->GetCharName()) );
		m_bReduceGauge = true;
	}

	return true;
}

void ioChainTraceSkill::OnProcessState( ioBaseChar *pChar, ioPlayStage *pStage )
{
	if( !pChar )
		return;

	DWORD dwPreDelay = GetPreDelayTime();
	DWORD dwCurTime = FRAMEGETTIME();

	switch( m_state )
	{
	case SS_PREDELAY:
		if( pChar->IsNeedProcess() && (m_dwPreDelayStartTime == 0 || m_dwPreDelayStartTime + dwPreDelay < FRAMEGETTIME()) )
		{
			FindFirstTarget( pChar );
			SetStartState(pChar);
		}
		break;
	case SS_START:
		break;
	case SS_ACTION_HITED:
		if( pChar->IsNeedProcess() && pChar->IsAttackKey() )
		{
			SetExtraAcitonState( pChar );
			break;
		}

		if( pChar->IsNeedProcess() && ((m_dwFirstHitTime + m_dwMaxWaitTime) < dwCurTime) )
		{
			SetExtraAcitonState( pChar );
			break;
		}

		if( pChar->IsNeedProcess() && (m_dwNextAutoFireTime && m_dwNextAutoFireTime < dwCurTime) )
		{
			m_dwNextAutoFireTime = 0;
			if( FindAutoTarget( pChar ) )
				AutoFire( pChar );
			else
				SetWaitExtraActionState( pChar );
		}
		break;
	case SS_WAIT_EXTRA_TIME:
		if( pChar->IsNeedProcess() && pChar->IsAttackKey() )
		{
			SetExtraAcitonState( pChar );
			break;
		}

		if( pChar->IsNeedProcess() && ((m_dwFirstHitTime + m_dwMaxWaitTime) < dwCurTime) )
		{
			SetExtraAcitonState( pChar );
			break;
		}
		break;
	case SS_EXTRA_ACTION:
		if( pChar->IsNeedProcess() && (m_dwAddAttackFireTime < dwCurTime) )
			SetWaitAddAttackEndState(pChar);
		
		break;
	case SS_WAIT_END_ADD_ACTION:
		if( pChar->IsNeedProcess() && (m_dwMotionEndTime < dwCurTime) )
			SetEndState( pChar );
		break;
	case SS_END:
		break;
	}


	if( m_state == SS_ACTION_HITED || m_state == SS_WAIT_EXTRA_TIME || m_state == SS_EXTRA_ACTION )
		ProcessWeaponLine(pChar);
}

void ioChainTraceSkill::OnSkillEnd( ioBaseChar *pChar )
{
	ioAttackSkill::OnSkillEnd( pChar );

	if( GetOwnerItem() )
		GetOwnerItem()->ClearCurChargeRateForSkill();

	// 기본무기 장착
	if( m_bUseItemMesh )
	{
		pChar->ShowWeaponItemMesh( true );
	}


	if( !m_HandMesh.IsEmpty() )
	{
		pChar->RestoreSkillWeaponMesh( this, m_HandMesh, m_bVisibleEquipMesh );
	}
	m_bSetHandMesh = false;

	RemoveWeapon( pChar );
}


void ioChainTraceSkill::RemoveWeapon( ioBaseChar *pChar )
{
	for( size_t i = 0; i < m_vWeaponIndexList.size(); i++ )
	{
		DWORD weaponIndex = m_vWeaponIndexList[i];
		ioWeapon *pWeapon = g_WeaponMgr.FindWeapon( pChar, weaponIndex );
		ioUroborusWeapon4 *pUroborus4 =	ToUroborusWeapon4( pWeapon );
		if( pUroborus4 )
			pUroborus4->SetEndState( false, false, false );
	}
}


bool ioChainTraceSkill::CheckUseItemMesh()
{
	return m_bUseItemMesh;
}


void ioChainTraceSkill::SetSkillActionState( ioBaseChar *pChar )
{
	if( !m_bSetHandMesh && m_bUseItemMesh && !m_HandMesh.IsEmpty() )
	{
		pChar->ChangeSkillWeaponMesh( this, m_HandMesh, m_bVisibleEquipMesh );
		m_bSetHandMesh = true;
	}
}


void ioChainTraceSkill::AutoFire( ioBaseChar *pChar )
{
	DWORD dwCurBaseIndex = pChar->IncreaseWeaponIndexBase();

	ioBaseChar *pTarget = pChar->GetBaseChar( m_Target );
	if( !pTarget )
		return;

	D3DXVECTOR3 vPos = pTarget->GetWorldPosition();

	D3DXVECTOR3 vDir;
	//vDir = vPos - m_vPrevHitPosition;
	if( m_vTargetList.size() == 0 )
		return;

	ioHashString stPrevTargetName = m_vTargetList.back();
	if( stPrevTargetName.IsEmpty() )
		return;

	ioBaseChar *pPrevTarget = pChar->GetBaseChar( stPrevTargetName );
	if( !pPrevTarget )
		return;

	D3DXVECTOR3 vStartPos= pPrevTarget->GetWorldPosition();
	vStartPos.y = m_fPrevHitHeight;

	vDir = vPos - vStartPos;
	D3DXVec3Normalize( &vDir, &vDir );

	pChar->SkillFireExplicit( GetAttribute().m_vWeaponInfoList.front(), GetName(), dwCurBaseIndex, vStartPos, vDir );

	if( pChar->IsNeedSendNetwork() )
	{
		SP2Packet kPacket( CUPK_SKILL_EXTRA_INFO );
		kPacket << pChar->GetCharName();
		kPacket << GetName();

		kPacket << SS_AUTOFIRE_SYNC;
		kPacket << m_Target;

		P2PNetwork::SendToAllPlayingUser( kPacket );
	}
}


void ioChainTraceSkill::FindFirstTarget( ioBaseChar *pOwner )
{
	m_Target.Clear();

	if( !pOwner )
		return;

	ioPlayStage *pStage = pOwner->GetCreator();
	if( !pStage )
		return;

	ioFootballMode *pFootBall = ToFootballMode( pStage->GetPlayMode() );
	if( pFootBall && pFootBall->IsDisableAttackMode() )
		return;

	if( pOwner->GetExperienceMode() != EMS_NONE )
		return;

	D3DXVECTOR3 vCurPos = pOwner->GetMidPositionByRate();
	D3DXVECTOR3 vDiff= ioMath::VEC3_ZERO;

	float fCurAngle = ioMath::ArrangeHead( m_fTargetAngle );
	float fHalfValue = cosf( DEGtoRAD(fCurAngle*0.5f) );
	float fTargetRangeSq = m_fTargetRange * m_fTargetRange;

	const BaseCharList &rkCharList = pStage->GetBaseCharList();
	BaseCharList::const_iterator iter;

	ioHashString szTarget;
	float fCurGap = -FLOAT1;
	for( iter=rkCharList.begin() ; iter!=rkCharList.end() ; ++iter )
	{
		ioBaseChar *pTarget = *iter;
		if( !pTarget ) continue;

		if( pTarget->GetTeam() == pOwner->GetTeam() ) continue;
		if( pTarget->GetCharName() == pOwner->GetCharName() ) continue;
		if( pTarget->GetExperienceMode() != EMS_NONE ) continue;

		if( !IsEnableTargetState( pTarget, true ) )
			continue;

		D3DXVECTOR3 vTargetPos = pTarget->GetMidPositionByRate();
		vDiff = vTargetPos - vCurPos;
		float fDiffRangeSq = D3DXVec3LengthSq( &vDiff );

		// 각도 체크
		if( fCurAngle != 0.0f )
		{
			D3DXQUATERNION qtRot = pOwner->GetTargetRot();
			D3DXVECTOR3 vDir = qtRot * D3DXVECTOR3( 0.0f, 0.0f, -FLOAT1 );
			D3DXVec3Normalize( &vDir, &vDir );
			D3DXVECTOR3 vDiff2 = vDiff;
			vDiff2.y = 0.0f;
			D3DXVec3Normalize( &vDiff2, &vDiff2 );

			float fCurValue = D3DXVec3Dot( &vDir, &vDiff2 );
			if( fCurValue < fHalfValue )
				continue;
		}

		// 높이 체크
		/*float fHeightGap = vTargetPos.y - vCurPos.y;
		if( fHeightGap > 0.0f && fHeightGap > m_CurAttackInfo.m_fTargetUpHeight )
			continue;

		if( fHeightGap < 0.0f && fabs(fHeightGap) > m_CurAttackInfo.m_fTargetDownHeight )
			continue;*/

		// 거리 체크
		if( fDiffRangeSq < fTargetRangeSq )
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

	if( !szTarget.IsEmpty() )
	{
		ioBaseChar *pTarget = pStage->GetBaseChar( szTarget );
		if( pTarget )
		{
			m_Target = szTarget;
		}
	}
}


bool ioChainTraceSkill::FindAutoTarget( ioBaseChar *pOwner )
{
	if( !pOwner )
		return false;

	ioPlayStage *pStage = pOwner->GetCreator();
	if( !pStage )
		return false;

	ioFootballMode *pFootBall = ToFootballMode( pStage->GetPlayMode() );
	if( pFootBall && pFootBall->IsDisableAttackMode() )
		return false;

	if( pOwner->GetExperienceMode() != EMS_NONE )
		return false;

	ioBaseChar *pPrevTarget = pOwner->GetBaseChar( m_Target );
	if( !pPrevTarget )
		return false;

	m_Target.Clear();

	D3DXVECTOR3 vCurPos = pPrevTarget->GetMidPositionByRate();
	D3DXVECTOR3 vDiff= ioMath::VEC3_ZERO;

	float fCurAngle = ioMath::ArrangeHead( m_fTargetAngle );
	float fHalfValue = cosf( DEGtoRAD(fCurAngle*0.5f) );
	float fTargetRangeSq = m_fTargetRange2 * m_fTargetRange2;

	const BaseCharList &rkCharList = pStage->GetBaseCharList();
	BaseCharList::const_iterator iter;

	ioHashString szTarget;
	float fCurGap = -FLOAT1;
	for( iter=rkCharList.begin() ; iter!=rkCharList.end() ; ++iter )
	{
		ioBaseChar *pTarget = *iter;
		if( !pTarget ) continue;

		if( pTarget->GetTeam() == pOwner->GetTeam() ) continue;
		if( pTarget->GetCharName() == pOwner->GetCharName() ) continue;
		if( pTarget->GetExperienceMode() != EMS_NONE ) continue;

		bool bPass = false;
		for( size_t i = 0; i < m_vTargetList.size(); i++ )
		{
		
			const ioHashString& stTargetedName = m_vTargetList[i];
			if( stTargetedName == pTarget->GetCharName() )
			{
				bPass = true;
				break;
			}
		}

		if( bPass )
			continue;

		if( !IsEnableTargetState( pTarget, true ) )
			continue;

		D3DXVECTOR3 vTargetPos = pTarget->GetMidPositionByRate();
		vDiff = vTargetPos - vCurPos;
		float fDiffRangeSq = D3DXVec3LengthSq( &vDiff );

		// 각도 체크
		/*if( fCurAngle != 0.0f )
		{
			D3DXQUATERNION qtRot = pOwner->GetTargetRot();
			D3DXVECTOR3 vDir = qtRot * D3DXVECTOR3( 0.0f, 0.0f, -FLOAT1 );
			D3DXVec3Normalize( &vDir, &vDir );
			D3DXVECTOR3 vDiff2 = vDiff;
			vDiff2.y = 0.0f;
			D3DXVec3Normalize( &vDiff2, &vDiff2 );

			float fCurValue = D3DXVec3Dot( &vDir, &vDiff2 );
			if( fCurValue < fHalfValue )
				continue;
		}*/

		// 높이 체크
		/*float fHeightGap = vTargetPos.y - vCurPos.y;
		if( fHeightGap > 0.0f && fHeightGap > m_CurAttackInfo.m_fTargetUpHeight )
			continue;

		if( fHeightGap < 0.0f && fabs(fHeightGap) > m_CurAttackInfo.m_fTargetDownHeight )
			continue;*/

		// 거리 체크
		if( fDiffRangeSq < fTargetRangeSq )
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

	if( !szTarget.IsEmpty() )
	{
		ioBaseChar *pTarget = pStage->GetBaseChar( szTarget );
		if( pTarget )
		{
			m_Target = szTarget;
		}
	}

	//패킷을 여기서 보내자
	//////////////////////////////////////////////////////////////////////////


	//////////////////////////////////////////////////////////////////////////
	if( m_Target.IsEmpty() )
		return false;

	return true;
}


void ioChainTraceSkill::SetStartState( ioBaseChar *pOwner )
{
	if( !pOwner )
		return;

	ioEntityGroup *pGrp = pOwner->GetGroup();
	if( !pGrp )
		return;

	m_state = SS_START;

	pGrp->ClearAllLoopAni( FLOAT10, true );
	pGrp->ClearAllActionAni( FLOAT10, true );

	DWORD dwCurTime = FRAMEGETTIME();
	m_dwFireMotionEndTime = dwCurTime;

	int iAniID	     = pGrp->GetAnimationIdx( GetAttribute().m_AttackAnimation );
	if( iAniID == -1 )	
		return;
	float fTimeRate  = GetAttribute().m_fAttackAniRate;

	pGrp->SetActionAni( iAniID, FLOAT100, FLOAT100, FLOAT1, FLOAT1/fTimeRate, 0.0f, 0.0f, true );

	DWORD dwCurBaseIndex = pOwner->IncreaseWeaponIndexBase();
	DWORD dwPreDelay = GetAttribute().m_dwPreDelay;
	pOwner->RefreshFireTimeListWithBase( dwCurBaseIndex, iAniID, GetAttribute().m_vWeaponInfoList,
		FTT_SKILL_ATTACK, fTimeRate, dwPreDelay );

	m_dwMotionEndTime = dwCurTime;
	m_dwFireEndTime = dwCurTime;
	m_dwFireEndTime += pGrp->GetAnimationEventTime_e( iAniID, "fire_s" ) * fTimeRate;
	m_dwMotionEndTime += ( pGrp->GetAnimationFullTime( iAniID ) * GetAttribute().m_fAttackAniRate );	

	if( pOwner->IsNeedProcess() )
	{
		SP2Packet kPacket( CUPK_SKILL_EXTRA_INFO );
		kPacket << pOwner->GetCharName();
		kPacket << GetName();
		kPacket << m_state;
		kPacket << m_Target;
		P2PNetwork::SendToAllPlayingUser( kPacket );
	}

	pOwner->ReduceNeedGaugeBySkill( this, GetNeedGauge(pOwner->GetCharName()) );
}


void ioChainTraceSkill::SetHitState( ioBaseChar *pChar )
{
	m_dwHitTime = FRAMEGETTIME();
	m_state = SS_ACTION_HITED;
	m_nHitCnt++;

	if( m_nHitCnt == 1 )
	{
		m_dwFirstHitTime = m_dwHitTime;

		ioEntityGroup *pGrp = pChar->GetGroup();
		pGrp->ClearAllActionAni( FLOAT100, true );
		pGrp->ClearAllLoopAni( FLOAT100, true );
		pGrp->SetLoopAni( m_stHitLoopAni, FLOAT100 );
	}

	m_dwNextAutoFireTime = m_dwHitTime + m_dwDelayHitTime;

	if( m_nHitCnt >= m_nMaxHitCnt )
	{
		SetWaitExtraActionState( pChar );
		return;
	}
}


void ioChainTraceSkill::SetWaitExtraActionState( ioBaseChar *pChar )
{
	m_state = SS_WAIT_EXTRA_TIME;

	if( pChar->IsNeedProcess() )
	{
		SP2Packet kPacket( CUPK_SKILL_EXTRA_INFO );
		kPacket << pChar->GetCharName();
		kPacket << GetName();
		P2PNetwork::SendToAllPlayingUser( kPacket );
	}
}


void ioChainTraceSkill::SetExtraAcitonState( ioBaseChar *pOwner )
{
	m_state = SS_EXTRA_ACTION;

	if( !pOwner )
		return;

	ioEntityGroup *pGrp = pOwner->GetGroup();
	if( !pGrp )
		return;

	DWORD dwCurTime = FRAMEGETTIME();

	pOwner->ClearReservedSliding();
	pGrp->ClearAllLoopAni( FLOAT10, true );
	pGrp->ClearAllActionAni( FLOAT10, true );

	int iAniID	     = pGrp->GetAnimationIdx( m_stAddAttackAni );
	float fTimeRate  = m_fAddAttackAniRate;

	pGrp->SetActionAni( iAniID, FLOAT100, FLOAT100, FLOAT1, FLOAT1/fTimeRate, 0.0f, 0.0f, true );

	DWORD dwCurBaseIndex = pOwner->IncreaseWeaponIndexBase();
	DWORD dwPreDelay = m_dwAddAttackAniPreDelay;

	m_dwAddAttackFireTime = dwCurTime;
	m_dwMotionEndTime = dwCurTime;
	m_dwAddAttackFireTime += pGrp->GetAnimationEventTime_e( iAniID, "fire_s" ) * fTimeRate;
	m_dwMotionEndTime += ( pGrp->GetAnimationFullTime( iAniID ) * fTimeRate );

	if( pOwner->IsNeedProcess() )
	{
		//패킷을 보내자
		m_CurDirKey = pOwner->CheckCurDirKey();;

		SP2Packet kPacket( CUPK_SKILL_EXTRA_INFO );
		kPacket << pOwner->GetCharName();
		kPacket << GetName();

		kPacket << m_state;
		kPacket << (int)m_CurDirKey;

		P2PNetwork::SendToAllPlayingUser( kPacket );
		
	}
}


void ioChainTraceSkill::SetWaitAddAttackEndState( ioBaseChar *pOwner )
{
	m_state = SS_WAIT_END_ADD_ACTION;

	if( !pOwner )
		return;

	ioEntityGroup *pGrp = pOwner->GetGroup();
	if( !pGrp )
		return;

	DWORD dwCurTime = FRAMEGETTIME();

	WeaponInfoList weaponList;
	WeaponInfo weapon;
	weapon.m_iWeaponIdx = m_nAddAttackWeapon;
	weaponList.push_back( weapon );

	DWORD dwCurBaseIndex = pOwner->IncreaseWeaponIndexBase();

	for( size_t i = 0; i < m_vTargetList.size(); i++ )
	{
		const ioHashString& szTargetName = m_vTargetList[i];
		ioBaseChar *pTarget = pOwner->GetBaseChar( szTargetName );
		if( !pTarget )
			continue;

		if( pTarget->GetState() != CS_ETC_STATE && pTarget->GetState() != CS_FLOAT_STATE )
			continue;

		D3DXVECTOR3 vTargetPos = pTarget->GetMidPositionByRate();
		D3DXVECTOR3 vDir =	D3DXVECTOR3(0, 1, 0 );//	pOwner->GetTargetRot() * D3DXVECTOR3( 0, 0, -1 );

		m_stOwnerWeaponTargetName = szTargetName;

		pOwner->SkillFireExplicit( weaponList.front(), GetName(), dwCurBaseIndex++, 
			vTargetPos, vDir );
	}

	RemoveWeapon( pOwner );

	if( pOwner->IsNeedSendNetwork() )
	{
		SP2Packet kPacket( CUPK_SKILL_EXTRA_INFO );

		kPacket << pOwner->GetCharName();
		kPacket << GetName();
		kPacket << m_state;

		P2PNetwork::SendToAllPlayingUser( kPacket );
	}
}


void ioChainTraceSkill::SetEndState( ioBaseChar *pChar )
{
	m_state = SS_END;

	if( pChar->IsNeedSendNetwork() )
	{
		SP2Packet kPacket( CUPK_SKILL_EXTRA_INFO );

		kPacket << pChar->GetCharName();
		kPacket << GetName();
		kPacket << m_state;

		P2PNetwork::SendToAllPlayingUser( kPacket );
	}
}


bool ioChainTraceSkill::IsEnableTargetState( ioBaseChar *pTarget, bool bCheckTargetWound )
{
	if( !pTarget )
		return false;

	if( pTarget->IsSystemState() )
		return false;
		
	if( !pTarget->IsEnableTarget() ) 
		return false;

	if( bCheckTargetWound )
	{
		if( !pTarget->CheckTargetWoundType( m_TargetWoundType ) )
			return false;
	}

	return true;
}

void ioChainTraceSkill::ApplyAttributeToWeapon( ioWeapon *pWeapon, ioPlayStage *pStage )
{
	ioSkill::ApplyAttributeToWeapon( pWeapon, pStage );

	SetChargingRateForWeapon( pWeapon );

	ioUroborusWeapon4 *pUroborus4 = ToUroborusWeapon4( pWeapon );
	if( pUroborus4 )
	{
		DWORD weaponindex = pWeapon->GetWeaponIndex();
		m_vWeaponIndexList.push_back( weaponindex );

		D3DXVECTOR3 vWeaponPos = pWeapon->GetPosition();

		ioBaseChar *pChar = pWeapon->GetOwner();
		if( pChar )
		{
			D3DXVECTOR3 vDir = pChar->GetTargetRot() * D3DXVECTOR3( 0, 0, -1 );
			D3DXVec3Normalize( &vDir, &vDir );

			if( !m_Target.IsEmpty() )
			{
				ioBaseChar *pTargetChar = pChar->GetBaseChar( m_Target );
				if( pTargetChar )
				{
					D3DXVECTOR3 vDestPos = pTargetChar->GetWorldPosition();
					vDestPos.y = pTargetChar->GetMidHeightByRate();
					vDir = vDestPos - vWeaponPos;
					D3DXVec3Normalize( &vDir, &vDir );
					pUroborus4->SetMoveDir( vDir );

					if( m_state == SS_ACTION_HITED )
						pUroborus4->SetGuidArrowPosMode();
				}
				else
					pUroborus4->SetEndState( false, false, false );
			}
			else if( m_state != SS_START )
				pUroborus4->SetEndState( false, false, false );
		}
	}

	ioOwnerAttackWeapon *pOwnerWeapon = ToOwnerAttackWeapon(pWeapon);
	if( pOwnerWeapon )
	{
		pOwnerWeapon->SetTargetName( m_stOwnerWeaponTargetName, pStage );
	}

	/*ioContactWeapon *pContact = ToContactWeapon( pWeapon );
	if( pContact )
	{
		ioBaseChar *pChar = pWeapon->GetOwner();
		if( pChar && !m_Target.IsEmpty() )
		{
			ioBaseChar *pTargetChar = pChar->GetBaseChar( m_Target );
			if( pTargetChar )
			{
				D3DXVECTOR3 vPos = pTargetChar->GetWorldPosition();
				vPos.y = pTargetChar->GetMidHeightByRate();
				pContact->SetPosition( vPos );
			}
		}
	}*/
}

void ioChainTraceSkill::ApplyExtraSkillInfo( ioBaseChar *pChar, ioPlayStage *pStage, SP2Packet &rkPacket )
{
	if( !pChar ) return;
	if( !pStage ) return;

	int iState;
	rkPacket >> iState;

	switch( iState )
	{
	case SS_START:
		rkPacket >> m_Target;
		SetStartState( pChar );
		break;
	case SS_WAIT_END_ADD_ACTION:
		SetWaitAddAttackEndState(pChar);
		break;
	case SS_AUTOFIRE_SYNC:
		rkPacket >> m_Target;
		AutoFire(pChar);
		break;
	case SS_WAIT_EXTRA_TIME:
		SetWaitAddAttackEndState(pChar);
		break;
	case SS_EXTRA_ACTION:
		int nCurKey;
		rkPacket >> nCurKey;

		m_CurDirKey = (ioUserKeyInput::DirKeyInput)nCurKey;
		SetExtraAcitonState( pChar );
		break;;
	case SS_END:
		SetEndState(pChar);
		break;
	}
}



void ioChainTraceSkill::SetTarget( const ioHashString &szTargetName, ioBaseChar* pOwner, ioWeapon *pWeapon )
{
	m_vTargetList.push_back( szTargetName );
	m_fPrevHitHeight = pWeapon->GetPosition().y;
	m_vPrevHitPosition = pWeapon->GetPosition();

	if( m_state == SS_START || m_state == SS_ACTION_HITED )
		SetHitState( pOwner );
}


ioUserKeyInput::DirKeyInput ioChainTraceSkill::GetCurKey()
{
	return m_CurDirKey;
}


bool ioChainTraceSkill::IsProtected( int iDefenseBreakType ) const
{
	DWORD dwCurTime = FRAMEGETTIME();

	if( m_bDisableSkillProtect )
		return false;

	if(m_state == SS_PREDELAY)
		return true;

	if( m_dwProtectEndTime > dwCurTime )
		return true;

	//if( m_state == SS_START && m_dwFireEndTime && dwCurTime < m_dwFireEndTime )
		//return true;

	return false;
}


bool ioChainTraceSkill::IsSkillMotionEnd( DWORD dwActionStop, ioBaseChar* pOwner ) const
{
	DWORD dwCurTime = FRAMEGETTIME();

	if( m_state == SS_START && dwCurTime > m_dwMotionEndTime )
		return true;

	if( m_state == SS_END )
		return true;

	return false;
}


void ioChainTraceSkill::ProcessWeaponLine( ioBaseChar *pChar )
{
	if( !pChar )
		return;

	if( m_vTargetList.size() != m_vWeaponIndexList.size()  )
		return;

	bool bFound = false;

	int nIndexCnt = 0;
	for( ioHashStringVec::iterator iter = m_vTargetList.begin(); iter != m_vTargetList.end(); )
	{
		const ioHashString& stTargetDest = *iter;
		ioBaseChar* pTargetDest = pChar->GetBaseChar( stTargetDest );

		if( !pTargetDest || (pTargetDest->GetState() != CS_ETC_STATE && pTargetDest->GetState() != CS_FLOAT_STATE) )
		{
			m_vTargetList.erase( m_vTargetList.begin() + nIndexCnt );
			int nWeaponIndex = m_vWeaponIndexList[nIndexCnt];
			m_vWeaponIndexList.erase( m_vWeaponIndexList.begin() + nIndexCnt );

			ioWeapon *pWeapon = g_WeaponMgr.FindWeapon( pChar, nWeaponIndex );
			ioUroborusWeapon4 *pUroborus4 =	ToUroborusWeapon4( pWeapon );
			if( pUroborus4 )
				pUroborus4->SetEndState( false, false, false );

			continue;
		}

		iter++;
		nIndexCnt++;
	}

	for( size_t i = 0; i < m_vTargetList.size(); i++ )
	{
		const ioHashString& stTargetDest = m_vTargetList[i];
		ioBaseChar* pTargetDest = pChar->GetBaseChar( stTargetDest );

		DWORD weaponIndex = m_vWeaponIndexList[i];
		ioWeapon *pWeapon = g_WeaponMgr.FindWeapon( pChar, weaponIndex );
		ioUroborusWeapon4 *pUroborus4 =	ToUroborusWeapon4( pWeapon );
		if( !pUroborus4 )
			continue;

		//i == 0 일때는 시전자가 Src
		if( i > 0 )
		{
			const ioHashString& stTargetSrc = m_vTargetList[i-1];
			ioBaseChar* pTargetSrc = pChar->GetBaseChar( stTargetSrc );
			if( pTargetSrc && (pTargetSrc->GetState() == CS_ETC_STATE || pTargetSrc->GetState() == CS_FLOAT_STATE) )
			{
				D3DXVECTOR3 vPosSrc = pTargetSrc->GetMidPositionByRate();
				pUroborus4->SetGuidArrowPosMode( true, vPosSrc );
			}
		}
		else 
			pUroborus4->SetGuidArrowPosMode( false, ioMath::VEC3_ZERO );

		if( pTargetDest && (pTargetDest->GetState() == CS_ETC_STATE || pTargetDest->GetState() == CS_FLOAT_STATE) )
		{
			D3DXVECTOR3 vPosDest = pTargetDest->GetMidPositionByRate();
			pUroborus4->SetPosition( vPosDest );
		}

		bFound = true;
	}

	if( pChar->IsNeedProcess() && bFound == false )
		SetEndState(pChar);
}

//////////////////////////////////////////////////////////////////////////

/*float ioChainTraceSkill::GetAimedUpDownWeight( const D3DXVECTOR3 &vAimDir, float fMaxAngle )
{
	D3DXVECTOR3 vXZDir( vAimDir.x, 0.0f, vAimDir.z );
	D3DXVec3Normalize( &vXZDir, &vXZDir );

	float fAngle = D3DXVec3Dot( &vXZDir, &vAimDir );
	fAngle = max( -FLOAT1, min( fAngle, FLOAT1 ) );
	fAngle = acosf( fAngle );
	fAngle = RADtoDEG( fAngle );
	fAngle = max( 0.0f, min( fAngle, fMaxAngle ) );

	float fWeight = fAngle / fMaxAngle;

	return fWeight;
}


void ioChainTraceSkill::ChangeTrackingUD_Fire( ioBaseChar *pOwner )
{
	ioEntityGroup *pGrp = pOwner->GetGroup();
	if( !pGrp )
		return;

	D3DXVECTOR3 vAimedDir;
	ioBaseChar *pTarget = pOwner->GetBaseChar( m_Target );
	if( pTarget )
		vAimedDir = pTarget->GetMidPositionByRate() - pOwner->GetMidPositionByRate();
	else
		vAimedDir = pOwner->GetTargetRot() * D3DXVECTOR3( 0, 0, -1 );

	D3DXVec3Normalize( &vAimedDir, &vAimedDir );

	float fTimeRate  = FLOAT1;//pAttr->m_fAttackAniRate;
	DWORD dwPreDelay = 0;//pAttr->m_dwPreDelay;

	if( m_SuccubusAttackState == MFS_START )
		fTimeRate = m_CurAttackInfo.m_FirstAttack.m_fAttackAniRate;
	else if( m_SuccubusAttackState == MFS_ATTACK_HITED )
		fTimeRate = m_CurAttackInfo.m_fAttackLoopAniRate;

	DWORD dwCurTime = FRAMEGETTIME();

	float fWeight = GetAimedUpDownWeight( vAimedDir, 85.0f );
	if( vAimedDir.y >= 0.0f )
		AimedFireToUp( pGrp, fWeight, fTimeRate, dwPreDelay, true );
	else
		AimedFireToDown( pGrp, fWeight, fTimeRate, dwPreDelay, true );
}




void ioChainTraceSkill::AimedFireToUp( ioEntityGroup *pGrp, float fWeight, float fTimeRate, DWORD dwPreDelay, bool bHold )
{
	float fFadeOut = FLOAT100;
	if( !bHold )
		fFadeOut = 0.0f;

	bool bAniFx1 = false;
	bool bAniFx2 = true;

	if( fWeight > FLOAT05 )
	{
		bAniFx1 = true;
		bAniFx2 = false;
	}

	ioHashString szFireUp, szFireCenter;

	if( m_SuccubusAttackState == MFS_START )
	{
		szFireUp = m_CurAttackInfo.m_StartAniUp;
		szFireCenter = m_CurAttackInfo.m_StartAniCenter;
	}
	else if( m_SuccubusAttackState == MFS_ATTACK_HITED )
	{
		szFireUp = m_CurAttackInfo.m_AttackLoopAniUp;
		szFireCenter = m_CurAttackInfo.m_AttackLoopAniCenter;
	}

	if( fWeight > 0.0f )
	{
		if( m_SuccubusAttackState == MFS_START )
			pGrp->SetActionAni( szFireUp,100.0f, fFadeOut, fWeight, FLOAT1/fTimeRate,	0.0f, dwPreDelay,	bHold, bAniFx1 );
		else if( m_SuccubusAttackState == MFS_ATTACK_HITED )
			pGrp->SetLoopAni( szFireUp, FLOAT100, fWeight, FLOAT1/fTimeRate );

	}

	if( m_SuccubusAttackState == MFS_START )
		pGrp->SetActionAni( szFireCenter, FLOAT100, fFadeOut, FLOAT1 - fWeight, FLOAT1/fTimeRate, 0.0f, dwPreDelay, bHold, bAniFx2 );
	else if( m_SuccubusAttackState == MFS_ATTACK_HITED )
		pGrp->SetLoopAni( szFireCenter, FLOAT100, FLOAT1 - fWeight, FLOAT1/fTimeRate );
}


void ioChainTraceSkill::AimedFireToDown( ioEntityGroup *pGrp, float fWeight, float fTimeRate, DWORD dwPreDelay, bool bHold )
{
	float fFadeOut = FLOAT100;
	if( !bHold )
		fFadeOut = 0.0f;
	bool bAniFx1 = true;
	bool bAniFx2 = false;

	if( fWeight > FLOAT05 )
	{
		bAniFx1 = false;
		bAniFx2 = true;
	}

	ioHashString szFireDown, szFireCenter;

	if( m_SuccubusAttackState == MFS_START )
	{
		szFireDown = m_CurAttackInfo.m_StartAniDown;
		szFireCenter = m_CurAttackInfo.m_StartAniCenter;
	}else if( m_SuccubusAttackState == MFS_ATTACK_HITED )
	{
		szFireDown = m_CurAttackInfo.m_AttackLoopAniDown;
		szFireCenter = m_CurAttackInfo.m_AttackLoopAniCenter;
	}

	if( fWeight > 0.0f )
	{
		if( m_SuccubusAttackState == MFS_START )
			pGrp->SetActionAni( szFireCenter, FLOAT100, fFadeOut, FLOAT1 - fWeight, FLOAT1/fTimeRate, 0.0f, dwPreDelay, bHold, bAniFx1 );
		else if( m_SuccubusAttackState == MFS_ATTACK_HITED )
			pGrp->SetLoopAni( szFireCenter, FLOAT100, FLOAT1 - fWeight, FLOAT1/fTimeRate );
	}

	if( m_SuccubusAttackState == MFS_START )
		pGrp->SetActionAni( szFireDown, FLOAT100, fFadeOut, fWeight, FLOAT1/fTimeRate, 0.0f, dwPreDelay, bHold, bAniFx2 );
	else if( m_SuccubusAttackState == MFS_ATTACK_HITED )
		pGrp->SetLoopAni( szFireDown, FLOAT100, fWeight, FLOAT1/fTimeRate );
}
*/