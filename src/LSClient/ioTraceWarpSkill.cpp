

#include "stdafx.h"

#include "ioTraceWarpSkill.h"

#include "ioPlayStage.h"
#include "ioBaseChar.h"

#include "WeaponDefine.h"

ioTraceWarpSkill::ioTraceWarpSkill()
{
	ClearData();
}

ioTraceWarpSkill::ioTraceWarpSkill( const ioTraceWarpSkill &rhs )
: ioAttackSkill( rhs ),
  m_vTargetPosList( rhs.m_vTargetPosList ),
  m_fTargetRange( rhs.m_fTargetRange ),
  m_fTargetMinRange( rhs.m_fTargetMinRange ),
  m_iTargetLimitCount( rhs.m_iTargetLimitCount ),
  m_fTargetAngle( rhs.m_fTargetAngle ),
  m_TargetEffect( rhs.m_TargetEffect ),
  m_TargetWoundType( rhs.m_TargetWoundType ),
  m_WarpTargetWoundType( rhs.m_WarpTargetWoundType ),
  m_dwWaitTimeForWarp( rhs.m_dwWaitTimeForWarp ),
  m_szWarpBuffList( rhs.m_szWarpBuffList ),
  m_SkillLoopMotion( rhs.m_SkillLoopMotion ),
  m_fSkillLoopMotionRate( rhs.m_fSkillLoopMotionRate ),
  m_dwSkillLoopDuration( rhs.m_dwSkillLoopDuration ),
  m_dwSkillLoopProtectTime( rhs.m_dwSkillLoopProtectTime ),
  m_szWarpStartEffect( rhs.m_szWarpStartEffect ),
  m_szWarpEndEffect( rhs.m_szWarpEndEffect ),
  m_szWarpExceptTargetBuffList( rhs.m_szWarpExceptTargetBuffList ),
  m_bCheckDropZone( rhs.m_bCheckDropZone ),
  m_bSelectCollisionBuffEnable( rhs.m_bSelectCollisionBuffEnable )
{
	ClearData();
}

ioTraceWarpSkill::~ioTraceWarpSkill()
{
}

void ioTraceWarpSkill::ClearData()
{
	m_SkillState = SS_NONE;

	m_dwFireStartTime		= 0;
	m_dwSkillLoopStartTime	= 0;

	m_vTargetList.clear();
	m_vWarpTargetList.clear();

	m_bSetHandMesh = false;	
}

void ioTraceWarpSkill::LoadProperty( ioINILoader &rkLoader )
{
	ioAttackSkill::LoadProperty( rkLoader );

	char szBuf[MAX_PATH], szKey[MAX_PATH];

	m_fTargetAngle = rkLoader.LoadFloat_e( "target_angle", 0.0f );
	m_fTargetRange = rkLoader.LoadFloat_e( "target_max_range", 0.0f );
	m_fTargetMinRange = rkLoader.LoadFloat_e( "target_min_range", 0.0f );
	m_fTargetMinRange = min( m_fTargetMinRange, m_fTargetRange );
	m_iTargetLimitCount = rkLoader.LoadInt_e( "target_limit_count", 0 );

	rkLoader.LoadString_e( "target_effect", "", szBuf, MAX_PATH );
	m_TargetEffect = szBuf;

	m_TargetWoundType		= (TargetWoundType)rkLoader.LoadInt_e( "target_wound_type", TWT_ALL );
	m_WarpTargetWoundType	= (TargetWoundType)rkLoader.LoadInt_e( "warp_target_wound_type", TWT_ALL );

	int iCnt = rkLoader.LoadInt_e( "target_warp_position_count", 0 );
	m_vTargetPosList.clear();
	for( int i=0; i<iCnt; ++i )
	{
		D3DXVECTOR3 vTargetPosition = ioMath::VEC3_ZERO;
		wsprintf_e( szKey, "warp_offset_x%d", i+1 );
		vTargetPosition.x = rkLoader.LoadFloat( szKey, 0.0f );
		wsprintf_e( szKey, "warp_offset_y%d", i+1 );
		vTargetPosition.y = rkLoader.LoadFloat( szKey, 0.0f );
		wsprintf_e( szKey, "warp_offset_z%d", i+1 );
		vTargetPosition.z = rkLoader.LoadFloat( szKey, 0.0f );
		m_vTargetPosList.push_back( vTargetPosition );
	}

	m_szWarpBuffList.clear();
	m_dwWaitTimeForWarp = (DWORD)rkLoader.LoadInt_e( "wait_time_for_warp", 0 );
	iCnt = rkLoader.LoadInt_e( "target_warp_buff_count", 0 );
	for( int i=0; i<iCnt; ++i )
	{
		ioHashString szBuffName;
		wsprintf_e( szKey, "target_warp_buff%d", i+1 );
		rkLoader.LoadString( szKey, "", szBuf, MAX_PATH );
		szBuffName = szBuf;
        m_szWarpBuffList.push_back( szBuffName );
	}

	rkLoader.LoadString_e( "target_warp_start_effect", "", szBuf, MAX_PATH );	m_szWarpStartEffect = szBuf;

	rkLoader.LoadString_e( "target_warp_end_effect", "", szBuf, MAX_PATH );	m_szWarpEndEffect = szBuf;

	m_szWarpExceptTargetBuffList.clear();
	iCnt = rkLoader.LoadInt_e( "except_target_buff_count", 0 );
	for( int i=0; i<iCnt; ++i )
	{
		wsprintf_e( szKey, "except_target_buff%d", i+1 );
		rkLoader.LoadString( szKey, "", szBuf, MAX_PATH );

		m_szWarpExceptTargetBuffList.push_back( szBuf );
	}

	m_bCheckDropZone = rkLoader.LoadBool_e( "check_drop_zone", false );

	//LoopMotion
	rkLoader.LoadString_e( "skill_loop_motion", "", szBuf, MAX_PATH );	m_SkillLoopMotion = szBuf;

	m_fSkillLoopMotionRate		= rkLoader.LoadFloat_e( "skill_loop_motion_rate", FLOAT1 );
	m_dwSkillLoopDuration		= (DWORD)rkLoader.LoadInt_e( "skill_loop_duration", 1 );
	m_dwSkillLoopProtectTime	= rkLoader.LoadInt_e( "skill_loop_protect_time", 0 );

	m_bSelectCollisionBuffEnable = rkLoader.LoadBool_e( "select_collision_buff_enable", false );
}

ioSkill* ioTraceWarpSkill::Clone()
{
	return new ioTraceWarpSkill( *this );
}

bool ioTraceWarpSkill::OnSkillStart( ioBaseChar *pChar )
{
	if( !ioAttackSkill::OnSkillStart( pChar ) )
		return false;

	ClearData();

	m_CurTargetName.Clear();

	m_dwFireStartTime = 0;

	m_bSetHandMesh = false;

	// 기본 무기 손에서 제거
	if( m_bUseItemMesh )
	{
		pChar->ShowWeaponItemMesh( false );
	}

	// 동기화 되는 시간을 벌기 위해 predelay 시점에 타겟 선정 필요.
	FindTarget( pChar );

	return true;
}

void ioTraceWarpSkill::OnProcessState( ioBaseChar *pChar, ioPlayStage *pStage )
{
	DWORD dwCurTime = FRAMEGETTIME();
	DWORD dwPreDelay = GetPreDelayTime();

	if( m_dwPreDelayStartTime == 0 )
	{
		CheckExtraAniJump( pChar );
		CheckAniRotate( pChar );
	}

	switch( m_SkillState )
	{
	case SS_NONE:
		{
			if( m_dwPreDelayStartTime > 0 && m_dwPreDelayStartTime + dwPreDelay < FRAMEGETTIME() )
			{
				m_dwPreDelayStartTime = 0;

				AttackSkillFire( pChar );
				EndPreDelayEffect( pChar );
			}

			if( m_dwPreDelayStartTime == 0 )
			{
				if( m_SkillUseType == SUT_NORMAL )
				{
					m_CurTargetName.Clear();

					if( m_dwFireStartTime > 0 && m_dwFireStartTime < FRAMEGETTIME() )
					{
						// 타겟의 개수만큼 충돌체 생성
						// 리미트 걸 것.
						int iTargetCnt = m_vTargetList.size();
						iTargetCnt = min( iTargetCnt, m_iTargetLimitCount );
						int iCurMaxCnt = iTargetCnt;
						if( iCurMaxCnt == 0 )
							iCurMaxCnt = 1;

						int iCurFireCnt = 0;
						for(int i=0; i<iTargetCnt; ++i)
						{
							m_CurTargetName = m_vTargetList[i];

							D3DXVECTOR3 vPos = pChar->GetMidPositionByRate();
							pChar->SkillFireExplicit( GetAttribute().m_vWeaponInfoList.front(),
													  GetName(),
													  m_dwWeaponBaseIndex+i,
													  vPos);

							iCurFireCnt++;
						}

						if( iCurMaxCnt > 0 && iCurFireCnt < iCurMaxCnt )
						{
							m_CurTargetName.Clear();

							D3DXVECTOR3 vPos = pChar->GetMidPositionByRate();
							pChar->SkillFireExplicit( GetAttribute().m_vWeaponInfoList.front(),
													  GetName(),
													  m_dwWeaponBaseIndex+i,
													  vPos);
						}

						m_dwFireStartTime = 0;
					}

					if( m_dwMotionEndTime < dwCurTime )
					{
						SetSkillLoopState( pChar );
					}
				}
				else if(m_SkillUseType == SUT_JUMP )
				{
					SetSkillEndState( pChar );
				}
				return;
			}
		}
		break;
	case SS_LOOP:
		{
			if( m_dwMotionEndTime < dwCurTime )
			{
				SetSkillEndState( pChar );
				return;
			}

			if( m_dwMotionStartTime+m_dwWaitTimeForWarp < dwCurTime )
			{
				if( pChar->IsNeedProcess() )
				{
					SetWarpTarget( pChar, pStage, true );
					SetSkillEndState( pChar );
				}
			}
		}
		break;
	case SS_END:
		break;
	}
}

void ioTraceWarpSkill::OnSkillEnd( ioBaseChar *pChar )
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
}

bool ioTraceWarpSkill::IsSkillMotionEnd( DWORD dwActionStop, ioBaseChar* pOwner ) const
{
	DWORD dwCurTime = FRAMEGETTIME();

	if( m_dwPreDelayStartTime > 0 )
		return false;

	switch( m_SkillState )
	{
	case SS_NONE:
		return false;
	case SS_LOOP:
	case SS_END:
		if( m_dwMotionEndTime + dwActionStop < dwCurTime )
			return true;

		return false;
	}

	return false;
}

bool ioTraceWarpSkill::IsProtected( int iDefenseBreakType ) const
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
		if( m_dwFireMotionEndTime > FRAMEGETTIME() )
			return true;
	}

	switch( m_SkillState )
	{
	case SS_NONE:
		return true;
	case SS_LOOP:
	case SS_END:
		if( m_dwSkillLoopStartTime+m_dwSkillLoopProtectTime < dwCurTime )
			return false;

		return true;
	}

	return false;
}

bool ioTraceWarpSkill::CheckUseItemMesh()
{
	return m_bUseItemMesh;
}

void ioTraceWarpSkill::AttackSkillFire( ioBaseChar *pChar )
{
	ioEntityGroup *pGrp = pChar->GetGroup();
	int iAniID = pGrp->GetAnimationIdx( GetAttribute().m_AttackAnimation );
	if( iAniID == -1 )
		return;

	pChar->AttachEffect( m_SkillEffectName );

	float fTimeRate  = GetAttackTimeRate();
	DWORD dwPreDelay = GetAttribute().m_dwPreDelay;

	pGrp->ClearAllLoopAni( FLOAT100, true );
	pGrp->ClearAllActionAni( FLOAT100, true );

	m_dwMotionStartTime = m_dwMotionEndTime = FRAMEGETTIME();
	m_dwMotionEndTime += pGrp->GetAnimationFullTime( iAniID ) * fTimeRate;
	m_dwMotionEndTime += dwPreDelay + GetAttribute().m_dwEndDelay;

	DWORD dwCurTime = FRAMEGETTIME();
	DWORD dwFireStartTime;
	dwFireStartTime = m_dwFireMotionEndTime = dwCurTime;

	dwFireStartTime += dwPreDelay;
	dwFireStartTime += pGrp->GetAnimationEventTime_e( iAniID, "fire_s" ) * fTimeRate;
	m_dwFireStartTime = dwFireStartTime;

	m_dwAttackEndTime = pGrp->GetAnimationEventTime_e( iAniID, "attack_end" ) * fTimeRate;
	if( m_dwAttackEndTime > 0 )
		m_dwAttackEndTime += dwCurTime;
	else
		m_dwAttackEndTime = 0;

	if( m_SkillUseType == SUT_JUMP && m_bRefreshFireTimeList )
	{
		pChar->RefreshFireTimeListWithBase( m_dwWeaponBaseIndex,
											iAniID,
											GetAttribute().m_vWeaponInfoList,
											FTT_SKILL_ATTACK,
											fTimeRate,
											dwPreDelay );
	}

	//
	SetExtraAniJump( pChar, iAniID, fTimeRate );

	//
	m_dwAniRotateTime = 0;
	m_fAniRotateAngle = 0.0f;
	m_vAniRotateInfoList.clear();

	AniEventConstPtrList vAniRotateList;
	pGrp->GetAnimationEventConstPtrList( iAniID, "ani_rotate", vAniRotateList );

	AniEventConstPtrList::iterator iter_r = vAniRotateList.begin();
	while( iter_r != vAniRotateList.end() )
	{
		AniRotateInfo kInfo;

		kInfo.m_dwTime = (*iter_r)->fEventTime * fTimeRate;

		if( kInfo.m_dwTime > 0 )
			kInfo.m_dwTime += dwCurTime;
		else
			kInfo.m_dwTime = 0;

		if( !(*iter_r)->szSubInfo.IsEmpty() )
		{
			const StringVector &vParam = ioStringConverter::Split( (*iter_r)->szSubInfo.c_str(), " " );
			if( vParam.size() == 1 )
			{
				kInfo.m_fAngle = ioStringConverter::ParseFloat( vParam[0].c_str() );
			}
		}

		m_vAniRotateInfoList.push_back(kInfo);

		++iter_r;
	}

	if( !m_vAniRotateInfoList.empty() )
	{
		m_iCurAniRotate = 0;
		m_dwAniRotateTime = m_vAniRotateInfoList[m_iCurAniRotate].m_dwTime;
		m_fAniRotateAngle = m_vAniRotateInfoList[m_iCurAniRotate].m_fAngle;
	}
	//

	// Col Skip
	pChar->CheckCharColSkipSkillTime( iAniID, fTimeRate, dwPreDelay );

	m_dwFireMotionEndTime += dwPreDelay;
	m_dwFireMotionEndTime += pGrp->GetAnimationEventTime_e( iAniID, "firemotion_e" ) * fTimeRate;

	m_dwEnableReserveTime = dwCurTime;
	m_dwEnableReserveTime += pGrp->GetAnimationEventTime_e( iAniID, "key_reserve" ) * fTimeRate;

	if( dwFireStartTime >= m_dwFireMotionEndTime )
	{
		m_dwFireMotionEndTime = m_dwMotionEndTime;
	}

	if( GetAttribute().m_AttackEndAnimation.IsEmpty() )
	{
		pGrp->SetActionAni( iAniID, FLOAT100, FLOAT100, FLOAT1, FLOAT1/fTimeRate, 0.0f, dwPreDelay, true );
	}
	else
	{
		pGrp->SetActionAni( iAniID, FLOAT100, 0.0f, FLOAT1, FLOAT1/fTimeRate, 0.0f, dwPreDelay );

		float fWaitTime = pGrp->GetAnimationFullTime( iAniID ) * fTimeRate + dwPreDelay;
		float fEndTimeRate = GetAttribute().m_fAttackEndAniRate;

		pGrp->SetActionAni( GetAttribute().m_AttackEndAnimation,
							0.0f, FLOAT100,
							1.0f, FLOAT1/fEndTimeRate,
							fWaitTime, 0.0f, true );

		int iEndAniID = pGrp->GetAnimationIdx( GetAttribute().m_AttackEndAnimation );
		m_dwMotionEndTime += pGrp->GetAnimationFullTime( iEndAniID ) * fEndTimeRate;
	}

	pChar->SetReservedSliding( GetAttribute().m_vForceInfoList, iAniID, fTimeRate, dwPreDelay );
	pChar->ReduceNeedGaugeBySkill( this, GetNeedGauge(pChar->GetCharName()) );
	m_bReduceGauge = true;
}

void ioTraceWarpSkill::SetSkillLoopState( ioBaseChar *pChar )
{
	if( !pChar )	return;

	m_SkillState = SS_LOOP;

	ioEntityGroup *pGrp = pChar->GetGroup();
	int iAniID = pGrp->GetAnimationIdx( m_SkillLoopMotion );
	if( iAniID == -1 )	return;

	float fTimeRate  = m_fSkillLoopMotionRate;

	pGrp->ClearAllLoopAni( FLOAT100, true );
	pGrp->ClearAllActionAni( FLOAT100, true );

	pGrp->SetLoopAni( iAniID, FLOAT100, FLOAT1, FLOAT1/fTimeRate );

	m_dwMotionStartTime = m_dwMotionEndTime = FRAMEGETTIME();
	m_dwMotionEndTime += m_dwSkillLoopDuration * fTimeRate;
	
	m_dwSkillLoopStartTime = FRAMEGETTIME();

	if( !m_bSetHandMesh && m_bUseItemMesh && !m_HandMesh.IsEmpty() )
	{
		pChar->ChangeSkillWeaponMesh( this, m_HandMesh, m_bVisibleEquipMesh );
		m_bSetHandMesh = true;
	}
}

void ioTraceWarpSkill::SetSkillEndState( ioBaseChar *pChar )
{
	if( !pChar )	return;
	if( m_SkillState == SS_END )	return;

	m_SkillState = SS_END;
}

void ioTraceWarpSkill::SetWarpTarget( ioBaseChar *pChar, ioPlayStage *pStage, bool bSend /* = false */ )
{
	if( !pChar )	return;
	if( !pStage )	return;
	if( m_vWarpTargetList.empty() )	return;	

	int iWarpCount = m_vTargetPosList.size();
	
	Vector3Vec	vWarpPositionList;
	vWarpPositionList.reserve( iWarpCount );
	
	// 워프할 위치 정렬
	for( int i=0; i<iWarpCount; ++i )
	{
		D3DXVECTOR3 vWarpPosition = pChar->GetWorldPosition();
		if( m_vTargetPosList[i] != ioMath::VEC3_ZERO )
		{
			D3DXQUATERNION qtRot = pChar->GetTargetRot();
			D3DXVECTOR3 vOffSet = qtRot * m_vTargetPosList[i];

			vWarpPosition += vOffSet;
		}

		vWarpPosition.y = pStage->GetMapHeight( vWarpPosition.x, vWarpPosition.z, NULL, false );

		if( vWarpPosition.y <= 0.0f )
			vWarpPosition.y = pChar->GetMidHeightByRate();

		vWarpPosition.y += 25.0f; // 혹시나 맵에 걸려 떨어지는 상황 방지.

		vWarpPositionList.push_back(vWarpPosition);
	}

	// 드랍존을 고려하여 재정렬
	if( m_bCheckDropZone )
	{
		int iCount = 0;
		Vector3Vec vPosList;
		for( int i=0; i<iWarpCount; ++i )
		{
			D3DXVECTOR3 vPos = vWarpPositionList[i];
			float fMapHeight = pStage->GetMapHeightNoneEntity( vPos.x, vPos.z, vPos.y+3.0f );
			if( fMapHeight > 0.0f )
				vPosList.push_back(vPos);
		}

		if( !vPosList.empty() )
		{
			vWarpPositionList.clear();
			while( (int)vWarpPositionList.size() < iWarpCount )
			{
				if( (int)vPosList.size() <= iCount )
					iCount = 0;

				vWarpPositionList.push_back(vPosList[iCount]);
				iCount++;
			}
		}
	}

	// 위치 리스트를 기반으로 대상 이동.
	int iTargetCount = min( vWarpPositionList.size(), m_vWarpTargetList.size() );
	for( int i=0; i<iTargetCount; ++i )
	{
		ioBaseChar *pTarget = pStage->GetBaseChar( m_vWarpTargetList[i] );

		if( !pTarget )	continue;

		if( m_bSelectCollisionBuffEnable && !pTarget->HasBuff(BT_SELECT_COLLISION_BUFF) )
			continue;

		bool bCheck = false;
		bCheck = IsEnableTargetState( pTarget, true, true );

		if( bCheck )
		{
			D3DXVECTOR3 vTargetPos = pTarget->GetWorldPosition();
			D3DXVECTOR3 vTargetScale = pTarget->GetWorldScale();

			pTarget->SetWorldPosition( vWarpPositionList[i] );

			int iBuffCnt = m_szWarpBuffList.size();
			for( int j=0; j<iBuffCnt; ++j )
			{
				pTarget->AddNewBuff( m_szWarpBuffList[j], "", "", NULL );
			}

			pStage->CreateMapEffect( m_szWarpStartEffect, vTargetPos, vTargetScale );
			pStage->CreateMapEffect( m_szWarpEndEffect, vWarpPositionList[i], vTargetScale );
		}
	}

	if( pChar->IsNeedSendNetwork() && bSend )
	{
		if( !m_vWarpTargetList.empty() )
		{
			int iSize = m_vWarpTargetList.size();

			SP2Packet kPacket( CUPK_SKILL_EXTRA_INFO );
			kPacket << pChar->GetCharName();
			kPacket << GetName();
			kPacket << SSC_WARP_TARGET;
			kPacket << iSize;

			for( int i=0; i < iSize; ++i )
				kPacket << m_vWarpTargetList[i];

			P2PNetwork::SendToAllPlayingUser( kPacket );
		}
	}
}

void ioTraceWarpSkill::FindTarget( ioBaseChar *pOwner )
{
	ioHashString szTarget;

	if( !pOwner )	return;

	ioPlayStage *pStage = pOwner->GetCreator();
	if( !pStage )	return;
	
	// target list
	m_vTargetList.clear();

	D3DXQUATERNION qtRot = pOwner->GetTargetRot();
	D3DXVECTOR3 vDir = qtRot * D3DXVECTOR3( 0.0f, 0.0f, -FLOAT1 );
	D3DXVec3Normalize( &vDir, &vDir );

	const BaseCharList &rkCharList = pStage->GetBaseCharList();

	BaseCharList::const_iterator iter;
	for( iter=rkCharList.begin() ; iter!=rkCharList.end() ; ++iter )
	{
		ioBaseChar *pCurChar = *iter;

		if( pCurChar == pOwner )
			continue;

		if( pCurChar->GetTeam() == pOwner->GetTeam() )
			continue;

		if( !IsEnableTargetState( pCurChar, true ) )
			continue;

		D3DXVECTOR3 vDiff = pCurChar->GetMidPositionByRate() - pOwner->GetMidPositionByRate();
		float fLengthSq = D3DXVec3LengthSq( &vDiff );
		float fMaxRangeSq = m_fTargetRange * m_fTargetRange;
		float fMinRangeSq = m_fTargetMinRange * m_fTargetMinRange;
		if( !COMPARE( fLengthSq, fMinRangeSq, fMaxRangeSq ) )
			continue;

		D3DXVec3Normalize( &vDiff, &vDiff );
		float fValue = D3DXVec3Dot( &vDir, &vDiff );
		float fAngleGap = RADtoDEG( acosf(fValue) );
		if( fAngleGap >= m_fTargetAngle )
			continue;

		pCurChar->AttachEffect( m_TargetEffect );
		szTarget = pCurChar->GetCharName();
		m_vTargetList.push_back(szTarget);
	}

	if( pOwner->IsNeedSendNetwork() )
	{
		if( !m_vTargetList.empty() )
		{
			int iSize = m_vTargetList.size();

			SP2Packet kPacket( CUPK_SKILL_EXTRA_INFO );
			kPacket << pOwner->GetCharName();
			kPacket << GetName();
			kPacket << SSC_TARGET;
			kPacket << iSize;

			for( int i=0; i < iSize; ++i )
				kPacket << m_vTargetList[i];

			P2PNetwork::SendToAllPlayingUser( kPacket );
		}
	}
}

bool ioTraceWarpSkill::IsEnableTargetState( ioBaseChar *pTarget, bool bCheckTargetWound, bool bWarp /* = false */ )
{
	if( !pTarget )
		return false;

	if( pTarget->IsSystemState() )
		return false;

	if( !pTarget->IsEnableTarget() )
		return false;

	if( bWarp )
	{
		int iBuffCnt = m_szWarpExceptTargetBuffList.size();
		for( int i=0; i<iBuffCnt; ++i )
		{
			if( pTarget->HasBuff( m_szWarpExceptTargetBuffList[i] ) )
				return false;
		}
	}
	if( bCheckTargetWound )
	{
		TargetWoundType CheckWoundType;

		if( bWarp )
			CheckWoundType = m_WarpTargetWoundType;
		else
			CheckWoundType = m_TargetWoundType;

		if( !pTarget->CheckTargetWoundType( CheckWoundType ) )
			return false;
	}
	return true;
}

void ioTraceWarpSkill::SetWarpTargetList( const ioHashString &szTargetName )
{
	if( szTargetName.IsEmpty() )	return;
	
	ioHashStringVec::iterator iter = std::find( m_vWarpTargetList.begin(),
												m_vWarpTargetList.end(),
												szTargetName );

	if( iter == m_vWarpTargetList.end() )
		m_vWarpTargetList.push_back( szTargetName );
}

void ioTraceWarpSkill::ApplyAttributeToWeapon( ioWeapon *pWeapon, ioPlayStage *pStage )
{
	ioSkill::ApplyAttributeToWeapon( pWeapon, pStage );

	SetChargingRateForWeapon( pWeapon );
	
	//
	ioTraceWeaponType2 *pTraceWeapon = ToTraceWeaponType2( pWeapon );
	if( pTraceWeapon )
	{
		pTraceWeapon->SetTraceTarget( m_CurTargetName );
	}
}

void ioTraceWarpSkill::ApplyExtraSkillInfo( ioBaseChar *pChar, ioPlayStage *pStage, SP2Packet &rkPacket )
{
	if( !pChar ) return;
	if( !pStage ) return;

	int iState;
	rkPacket >> iState;

	switch( iState )
	{
	case SSC_TARGET:
		{
			int iSize;
			rkPacket >> iSize;

			m_vTargetList.clear();
			for( int i=0; i < iSize; ++i )
			{
				ioHashString szTargetName;
				rkPacket >> szTargetName;

				m_vTargetList.push_back( szTargetName );
			}
		}
		break;
	case SSC_WARP_TARGET:
		{
			int iSize;
			rkPacket >> iSize;

			m_vWarpTargetList.clear();

			for( int i=0; i < iSize; ++i )
			{
				ioHashString szTargetName;
				rkPacket >> szTargetName;

				m_vWarpTargetList.push_back( szTargetName );
			}
			SetWarpTarget( pChar, pStage );
			SetSkillEndState( pChar );
		}
		break;
	}

}