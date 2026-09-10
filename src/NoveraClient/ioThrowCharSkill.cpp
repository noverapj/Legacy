

#include "stdafx.h"

#include "ioPlayStage.h"

#include "ioThrowCharSkill.h"
#include "ioBaseChar.h"

#include "WeaponDefine.h"

ioThrowCharSkill::ioThrowCharSkill()
{
	ClearData();
}

ioThrowCharSkill::ioThrowCharSkill( const ioThrowCharSkill &rhs )
: ioAttackSkill( rhs ),
  m_iWeaponCount( rhs.m_iWeaponCount ),
  m_SkillLoopMotion( rhs.m_SkillLoopMotion ),
  m_fSkillLoopAniRate( rhs.m_fSkillLoopAniRate ),
  m_dwSkillLoopDuration( rhs.m_dwSkillLoopDuration ),
  m_dwSkillLoopProtectTime( rhs.m_dwSkillLoopProtectTime ),
  m_SkillActionMotion( rhs.m_SkillActionMotion ),
  m_fSkillActionAniRate( rhs.m_fSkillActionAniRate ),
  m_SkillEndMotion( rhs.m_SkillEndMotion ),
  m_fSkillEndAniRate( rhs.m_fSkillEndAniRate ),
  m_dwThrowSpeed( rhs.m_dwThrowSpeed ),
  m_fCheckTargetHeight( rhs.m_fCheckTargetHeight ),
  m_fThrowBackOffset( rhs.m_fThrowBackOffset ),
  m_CancelBuffList( rhs.m_CancelBuffList ),
  m_fTargetAngle( rhs.m_fTargetAngle ),
  m_fTargetRange( rhs.m_fTargetRange ),
  m_TargetWoundType( rhs.m_TargetWoundType ),
  m_fTargetUpHeight( rhs.m_fTargetUpHeight ),
  m_fTargetDownHeight( rhs.m_fTargetDownHeight )
{
	ClearData();
}

ioThrowCharSkill::~ioThrowCharSkill()
{
}

void ioThrowCharSkill::ClearData()
{
	m_SkillState = SS_NONE;
	
	m_ThrowTargetInfoList.clear();
	m_TargetNameList.clear();
	m_TargetNameListForWeapon.clear();

	m_bSetHandMesh = false;

	m_dwMotionEndTime	= 0;
	m_dwMotionStartTime	= 0;
	m_dwSkillLoopStartTime	= 0;
	m_dwFireStartTime = 0;

	m_iCurWeaponCount = 0;
	m_fCurAngle = 0.0f;
}

void ioThrowCharSkill::LoadProperty( ioINILoader &rkLoader )
{
	ioAttackSkill::LoadProperty( rkLoader );

	char szBuf[MAX_PATH], szKey[MAX_PATH];

	m_iWeaponCount = rkLoader.LoadInt_e( "attack_count", 0 );

	rkLoader.LoadString_e( "skill_loop_motion", "", szBuf, MAX_PATH );
	m_SkillLoopMotion = szBuf;
	m_dwSkillLoopDuration = (DWORD)rkLoader.LoadInt_e( "skill_loop_duration", 0 );
	m_fSkillLoopAniRate = rkLoader.LoadFloat_e( "skill_loop_ani_rate", 0.0f );
	m_dwSkillLoopProtectTime = (DWORD)rkLoader.LoadInt_e( "skill_loop_protect_time", 0 );

	rkLoader.LoadString_e( "skill_action_motion", "", szBuf, MAX_PATH );
	m_SkillActionMotion = szBuf;
	m_fSkillActionAniRate = rkLoader.LoadFloat_e( "skill_action_ani_rate", 0.0f );

	rkLoader.LoadString_e( "skill_end_motion", "", szBuf, MAX_PATH );
	m_SkillEndMotion = szBuf;
	m_fSkillEndAniRate = rkLoader.LoadFloat_e( "skill_end_ani_rate", 0.0f );

	m_dwThrowSpeed = (DWORD)rkLoader.LoadInt_e( "throw_speed", 0 );
	m_fCheckTargetHeight = rkLoader.LoadFloat_e( "check_target_height", 0.0f );
	m_fThrowBackOffset = rkLoader.LoadFloat_e( "throw_back_off_set", 0.0f );

	int iBuffCnt;
	m_CancelBuffList.clear();
	iBuffCnt = rkLoader.LoadInt_e( "cancel_buff_cnt", 0 );
	for( int i=0; i<iBuffCnt; ++i )
	{
		wsprintf_e( szKey, "cancel_buff%d", i+1 );
		rkLoader.LoadString( szKey, "", szBuf, MAX_PATH );
		m_CancelBuffList.push_back( szBuf );
	}

	m_fTargetAngle = rkLoader.LoadFloat_e( "target_angle", 0.0f );
	m_fTargetRange = rkLoader.LoadFloat_e( "target_range", 0.0f );
	m_fTargetUpHeight = rkLoader.LoadFloat_e( "target_up_height", 0.0f );
	m_fTargetDownHeight = rkLoader.LoadFloat_e( "target_down_height", 0.0f );
	m_TargetWoundType = (TargetWoundType)rkLoader.LoadInt_e( "target_wound_type", TWT_ALL );
}

ioSkill* ioThrowCharSkill::Clone()
{
	return new ioThrowCharSkill( *this );
}

bool ioThrowCharSkill::OnSkillStart( ioBaseChar *pChar )
{
	if( !ioAttackSkill::OnSkillStart( pChar ) )
		return false;

	ClearData();

	// 기본 무기 손에서 제거
	if( m_bUseItemMesh )
	{
		pChar->ShowWeaponItemMesh( false );
	}

	if( !m_bSetHandMesh && m_bUseItemMesh && !m_HandMesh.IsEmpty() )
	{
		pChar->ChangeSkillWeaponMesh( this, m_HandMesh, m_bVisibleEquipMesh );
		m_bSetHandMesh = true;
	}

	return true;
}

void ioThrowCharSkill::OnProcessState( ioBaseChar *pChar, ioPlayStage *pStage )
{
	DWORD dwCurTime = FRAMEGETTIME();
	if( m_dwPreDelayStartTime == 0 )
	{
		CheckExtraAniJump( pChar );
		CheckAniRotate( pChar );
	}

	switch( m_SkillState )
	{
	case SS_NONE:
		{
			DWORD dwPreDelay = GetPreDelayTime();
			if( m_dwPreDelayStartTime > 0 && m_dwPreDelayStartTime + dwPreDelay < FRAMEGETTIME() )
			{
				m_dwPreDelayStartTime = 0;

				AttackSkillFire( pChar );
				EndPreDelayEffect( pChar );
			}
		}
		break;
	case SS_PRE_LOOP:
		{
			if( m_dwFireStartTime > 0 && m_dwFireStartTime <= dwCurTime )
			{
				FindTarget( pChar );

				int iCnt = min( (int)m_TargetNameListForWeapon.size(), m_iWeaponCount );
				for( int i=0; i<iCnt; ++i )
				{
					pChar->SkillFireExplicit( GetAttribute().m_vWeaponInfoList[0],
											GetName(),
											m_dwWeaponBaseIndex+i,
											pChar->GetMidPositionByRate() );
					m_iCurWeaponCount++;
				}

				if( iCnt == 0 )
				{
					pChar->SkillFireExplicit( GetAttribute().m_vWeaponInfoList[0],
											GetName(),
											m_dwWeaponBaseIndex,
											pChar->GetMidPositionByRate() );
				}
				SetSkillLoopState( pChar );
			}
		}
		break;
	case SS_LOOP:
		{
			if( m_dwSkillLoopStartTime > 0 && m_dwSkillLoopStartTime + m_dwSkillLoopDuration <= dwCurTime )
			{
				if( m_TargetNameList.empty() )
					SetSkillEndState( pChar );
				else
				{
					CheckThrowInfo( pChar );
					SetSkillActionState( pChar );
				}
			}
		}
		break;
	case SS_ACTION:
		{
			if( m_ThrowTargetInfoList.empty() && m_dwMotionEndTime <= dwCurTime )
				SetSkillEndState( pChar );

			CheckTargetChar( pChar );

			float fFrameGap = g_FrameTimer.GetCurFrameGap();
			float fRate = fFrameGap / m_dwThrowSpeed;
			float fRotateAngle = 360.0f * fRate;

			int iCnt = m_ThrowTargetInfoList.size();
			for( int i=0; i<iCnt; ++i )
			{
				ioBaseChar *pTarget = pStage->GetBaseChar( m_ThrowTargetInfoList[i].m_Name );
				if( !pTarget )	continue;

				D3DXVECTOR3 vCenterPos = m_ThrowTargetInfoList[i].m_vCenterPos;
				D3DXVECTOR3 vTargetPos = m_ThrowTargetInfoList[i].m_vTargetPos;
				D3DXVECTOR3 vDiff = vTargetPos - vCenterPos;

				float fLength = D3DXVec3Length( &vDiff );
				D3DXVec3Normalize( &vDiff, &vDiff );
				D3DXVECTOR3 vAxis = m_ThrowTargetInfoList[i].m_vAxis;

				D3DXQUATERNION qtRot;
				ioMath::CalcDirectionQuaternion( qtRot, -vDiff );
				D3DXQuaternionRotationAxis( &qtRot, &vAxis, DEGtoRAD( fRotateAngle ) );
				vDiff = qtRot * vDiff;
				D3DXVECTOR3 vNewPos;
				vNewPos = vCenterPos + ( vDiff * fLength );

				m_ThrowTargetInfoList[i].m_fAngle += fRotateAngle;
				float fCurAngle = m_ThrowTargetInfoList[i].m_fAngle;

				D3DXVECTOR3 vMoveAmt = vTargetPos - vNewPos;
				bool bCol = false;
				if( pStage->ReCalculateMoveVectorByTerrain( pTarget, &vMoveAmt, bCol ) )
				{
					pTarget->SetWorldPosition( vNewPos );
					m_ThrowTargetInfoList[i].m_vTargetPos = vNewPos;

					// 하드코딩 : 각도는 15도까지 바닥체크를 하지 않도록 한다.
					if( fCurAngle > 175.0f ||
						( CheckLanding( pTarget->GetCharName() ) && fCurAngle > 15.0f ) )
					{
						if( pTarget->IsNeedProcess() )
						{
							AddCancelBuffToTarget( pChar, pTarget );
						}
					}
				}
				else
				{
					if( fCurAngle < 45.0f )
					{
						pTarget->SetWorldPosition( vNewPos );
						m_ThrowTargetInfoList[i].m_vTargetPos = vNewPos;
					}
					else
					{
						if( pTarget->IsNeedProcess() )
						{
							AddCancelBuffToTarget( pChar, pTarget );
						}
					}
				}
			}
		}
		break;
	case SS_END:
		break;
	}
}

void ioThrowCharSkill::AttackSkillFire( ioBaseChar *pChar )
{
	ioEntityGroup *pGrp = pChar->GetGroup();
	int iAniID = pGrp->GetAnimationIdx( GetAttribute().m_AttackAnimation );
	if( iAniID == -1 )	return;

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

	float fAttackEnd = pGrp->GetAnimationEventTime_e( iAniID, "attack_end" ) * fTimeRate;
	if( fAttackEnd > 0.0f )
		m_dwAttackEndTime = dwCurTime + fAttackEnd;
	else
		m_dwAttackEndTime = 0;

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

	if( !m_bReduceGauge )
	{
		pChar->ReduceNeedGaugeBySkill( this, GetNeedGauge(pChar->GetCharName()) );
		m_bReduceGauge = true;
	}

	m_SkillState = SS_PRE_LOOP;
}

void ioThrowCharSkill::OnSkillEnd( ioBaseChar *pChar )
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

	ioHashStringVec::iterator iter = m_TargetNameList.begin();
	while( iter != m_TargetNameList.end() )
	{
		ioBaseChar *pTarget = pChar->GetBaseChar( iter->c_str() );
		if( !pTarget )
		{
			++iter;
			continue;
		}

		if( pTarget->HasBuff( BT_SKIP_STUN) )
			pTarget->RemoveBuff( BT_SKIP_STUN );

		++iter;
	}
	ClearData();
}

bool ioThrowCharSkill::CheckUseItemMesh()
{
	return m_bUseItemMesh;
}

void ioThrowCharSkill::ApplyAttributeToWeapon( ioWeapon *pWeapon, ioPlayStage *pStage )
{
	ioSkill::ApplyAttributeToWeapon( pWeapon, pStage );

	SetChargingRateForWeapon( pWeapon );

	ioGuide2Weapon *pGuide2 = ToGuide2Weapon( pWeapon );
	if( pGuide2 )
	{
		D3DXVECTOR3 vDir = pGuide2->GetOwner()->GetMoveDir();
		pGuide2->SetMoveDir( vDir );
		if( !m_TargetNameListForWeapon.empty() )
			pGuide2->SetTraceTarget( m_TargetNameListForWeapon[m_iCurWeaponCount] );
	}
}

void ioThrowCharSkill::ApplyExtraSkillInfo( ioBaseChar *pChar, ioPlayStage *pStage, SP2Packet &rkPacket )
{
	if( !pChar )	return;
	if( !pStage )	return;
	ioHashString szTargetName;
	rkPacket >> szTargetName;

	ioBaseChar *pTarget = pStage->GetBaseChar( szTargetName );
	if( !pTarget )	return;
	AddCancelBuffToTarget( pChar, pTarget );
}

bool ioThrowCharSkill::IsProtected( int iDefenseBreakType ) const
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
	}

	if( m_dwSkillLoopStartTime + m_dwSkillLoopProtectTime > FRAMEGETTIME() )
		return true;

	return false;
}

bool ioThrowCharSkill::IsSkillMotionEnd( DWORD dwActionStop, ioBaseChar* pOwner ) const
{
	if( m_dwPreDelayStartTime > 0 )
		return false;

	if( m_SkillState == SS_ACTION )
		return false;

	if( m_dwMotionEndTime + dwActionStop > FRAMEGETTIME() )
		return false;

	return true;
}

void ioThrowCharSkill::SetInfoByApplyWeapon( ioBaseChar *pOwner, ioBaseChar *pTarget, DWORD dwWeaponBaseIndex, bool bBlock )
{
	if( bBlock ) return;
	if( !pOwner )	return;
	if( !pTarget )	return;
	//if( m_dwSkillLoopStartTime > 0 && m_dwSkillLoopStartTime + m_dwSkillLoopDuration <= dwCurTime )

	int iCnt = m_TargetNameList.size();
	for( int i=0; i<iCnt; ++i )
	{
		if( m_TargetNameList[i] == pTarget->GetCharName() )
			return;
	}

	m_TargetNameList.push_back( pTarget->GetCharName() );
}

void ioThrowCharSkill::FindTarget( ioBaseChar *pChar )
{
	if( !pChar )	return;
	ioPlayStage *pStage = pChar->GetCreator();
	if( !pStage )	return;

	D3DXVECTOR3 vCurPos = pChar->GetMidPositionByRate();
	D3DXVECTOR3 vDiff= ioMath::VEC3_ZERO;

	float fCurAngle = ioMath::ArrangeHead( m_fTargetAngle );
	float fHalfValue = cosf( DEGtoRAD(fCurAngle*0.5f) );
	float fTargetRangeSq = m_fTargetRange * m_fTargetRange;

	const BaseCharList &rkCharList = pStage->GetBaseCharList();
	BaseCharList::const_iterator iter;

	for( iter=rkCharList.begin() ; iter!=rkCharList.end() ; ++iter )
	{
		ioBaseChar *pTarget = *iter;
		if( !pTarget ) continue;

		if( pTarget->GetTeam() == pChar->GetTeam() ) continue;
		if( pTarget->GetCharName() == pChar->GetCharName() ) continue;
		if( pTarget->GetExperienceMode() != EMS_NONE ) continue;

		if( !IsEnableTargetState( pTarget ) )
			continue;

		D3DXVECTOR3 vTargetPos = pTarget->GetMidPositionByRate();
		vDiff = vTargetPos - vCurPos;
		float fDiffRangeSq = D3DXVec3LengthSq( &vDiff );

		// 각도 체크
		if( fCurAngle != 0.0f )
		{
			D3DXQUATERNION qtRot = pChar->GetTargetRot();
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
		float fHeightGap = vTargetPos.y - vCurPos.y;
		if( fHeightGap > 0.0f && fHeightGap > m_fTargetUpHeight )
			continue;

		if( fHeightGap < 0.0f && fabs(fHeightGap) > m_fTargetDownHeight )
			continue;

		// 거리 체크
		if( fDiffRangeSq < fTargetRangeSq )
			m_TargetNameListForWeapon.push_back( pTarget->GetCharName() );
	}
}

bool ioThrowCharSkill::IsEnableTargetState( ioBaseChar *pTarget )
{
	if( !pTarget )
		return false;

	if( pTarget->IsSystemState() )
		return false;
		
	if( !pTarget->IsEnableTarget() )
		return false;

	if( !pTarget->CheckTargetWoundType(m_TargetWoundType) )
		return false;

	return true;
}

void ioThrowCharSkill::SetSkillLoopState( ioBaseChar *pChar )
{
	if( !pChar )	return;
	
	m_SkillState = SS_LOOP;

	ioEntityGroup *pGrp = pChar->GetGroup();
	int iAniID = pGrp->GetAnimationIdx( m_SkillLoopMotion );
	if( iAniID == -1 )	return;
	float fRate = m_fSkillLoopAniRate;

	pGrp->ClearAllActionAni( FLOAT100, true );
	pGrp->ClearAllLoopAni( FLOAT100, true );
	
	pGrp->SetLoopAni( iAniID, FLOAT100, FLOAT1, FLOAT1/fRate );
	
	m_dwMotionStartTime = m_dwMotionEndTime = m_dwSkillLoopStartTime = FRAMEGETTIME();
	m_dwMotionEndTime += m_dwSkillLoopDuration;
}

void ioThrowCharSkill::SetSkillActionState( ioBaseChar *pChar )
{
	if( !pChar )	return;
	
	m_SkillState = SS_ACTION;

	ioEntityGroup *pGrp = pChar->GetGroup();
	int iAniID = pGrp->GetAnimationIdx( m_SkillActionMotion );
	if( iAniID == -1 )	return;
	float fRate = m_fSkillActionAniRate;

	pGrp->ClearAllActionAni( FLOAT100, true );
	pGrp->ClearAllLoopAni( FLOAT100, true );
	
	pGrp->SetActionAni( iAniID, FLOAT100, FLOAT100, FLOAT1, FLOAT1/fRate );
	
	m_dwMotionStartTime = m_dwMotionEndTime = FRAMEGETTIME();
	m_dwMotionEndTime += pGrp->GetAnimationFullTime( iAniID ) * fRate;

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

		kInfo.m_dwTime = (*iter_r)->fEventTime * fRate;

		if( kInfo.m_dwTime > 0 )
			kInfo.m_dwTime += FRAMEGETTIME();
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

	m_dwFireMotionEndTime = FRAMEGETTIME();
	m_dwFireMotionEndTime += pGrp->GetAnimationEventTime_e( iAniID, "firemotion_e" ) * fRate;
}

void ioThrowCharSkill::SetSkillEndState( ioBaseChar *pChar )
{
	if( !pChar )	return;

	m_SkillState = SS_END;

	ioEntityGroup *pGrp = pChar->GetGroup();
	int iAniID = pGrp->GetAnimationIdx( m_SkillEndMotion );
	if( iAniID == -1 )	return;
	float fRate = m_fSkillEndAniRate;

	pGrp->ClearAllActionAni( FLOAT100, true );
	pGrp->ClearAllLoopAni( FLOAT100, true );

	pGrp->SetActionAni( iAniID, FLOAT100, FLOAT100, FLOAT1, FLOAT1/fRate );

	m_dwMotionStartTime = m_dwMotionEndTime = FRAMEGETTIME();
	m_dwMotionEndTime += pGrp->GetAnimationFullTime( iAniID ) * fRate;
}

void ioThrowCharSkill::CheckThrowInfo( ioBaseChar *pChar )
{
	if( !pChar )	return;

	D3DXVECTOR3 vDir = pChar->GetMoveDir();
	D3DXVECTOR3 vThrowBackPos = pChar->GetWorldPosition();
	vThrowBackPos -= vDir * m_fThrowBackOffset;

	int iCnt = m_TargetNameList.size();
	for( int i=0; i<iCnt; ++i )
	{
		ioBaseChar *pTarget = pChar->GetBaseChar( m_TargetNameList[i] );
		if( !pTarget )	continue;

		ThrowTargetInfo kInfo;
		kInfo.Init();

		kInfo.m_Name = m_TargetNameList[i];
		D3DXVECTOR3 vTargetPos = pTarget->GetWorldPosition();
		kInfo.m_vTargetPos = vTargetPos;
		kInfo.m_vCenterPos = ( vTargetPos + vThrowBackPos ) / 2;

		D3DXVECTOR3 vDiff = vTargetPos - vThrowBackPos;
		D3DXVec3Normalize( &vDiff, &vDiff );
		D3DXVec3Cross( &kInfo.m_vAxis, &vDiff, &ioMath::UNIT_Y );

		m_ThrowTargetInfoList.push_back( kInfo );
	}
}

void ioThrowCharSkill::CheckTargetChar( ioBaseChar *pChar )
{
	if( !pChar )	return;
	if( m_TargetNameList.empty() )	return;

	ioHashStringVec::iterator  iter = m_TargetNameList.begin();
	while( iter != m_TargetNameList.end() )
	{
		bool bDeleteTarget = false;
		ioHashString szTargetName = (*iter);
		ioBaseChar *pTarget = pChar->GetBaseChar( szTargetName );
		if( !pTarget )
		{
			bDeleteTarget = true;
		}

		if( !bDeleteTarget && !pTarget->HasBuff( BT_SKIP_STUN ) )
		{
			bDeleteTarget = true;
		}

		if( bDeleteTarget )
		{
			iter = m_TargetNameList.erase( iter );

			ThrowTargetInfoList::iterator TargetInfoIter = m_ThrowTargetInfoList.begin();
			while( TargetInfoIter != m_ThrowTargetInfoList.end() )
			{
				if( TargetInfoIter->m_Name == szTargetName )
					TargetInfoIter = m_ThrowTargetInfoList.erase( TargetInfoIter );
				else
					++TargetInfoIter;
			}
		}
		else
		{
			++iter;
		}
	}
}

bool ioThrowCharSkill::CheckLanding( const ioHashString &szTargetName )
{
	if( szTargetName.IsEmpty() ) return false;
	ioPlayStage *pStage = g_SkillMaker.GetPlayStage();
	if( !pStage )	return false;
	
	ioBaseChar *pTarget = pStage->GetBaseChar(szTargetName);
	if( !pTarget )	return false;

	float fHeightGap = pTarget->GetWorldPosition().y - pTarget->GetBottomHeight();

	if( fHeightGap > 0.0f + m_fCheckTargetHeight )
		return false;

	return true;
}

void ioThrowCharSkill::AddCancelBuffToTarget( ioBaseChar *pChar, ioBaseChar *pTarget )
{
	if( !pChar )	return;
	if( !pTarget )	return;

	int iBuffCnt = m_CancelBuffList.size();
	for( int j=0; j<iBuffCnt; ++j )
		pTarget->AddNewBuff( m_CancelBuffList[j], pChar->GetCharName(), "", NULL );

	if( pTarget->IsNeedSendNetwork() )
	{
		SP2Packet kPacket( CUPK_SKILL_EXTRA_INFO );
		kPacket << pChar->GetCharName();
		kPacket << GetName();
		kPacket << pTarget->GetCharName();
		P2PNetwork::SendToAllPlayingUser( kPacket );
	}
}