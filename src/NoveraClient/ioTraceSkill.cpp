

#include "stdafx.h"

#include "ioTraceSkill.h"

#include "ioPlayStage.h"
#include "ioBaseChar.h"

#include "WeaponDefine.h"

ioTraceSkill::ioTraceSkill()
{
	m_dwFireStartTime = 0;
	m_bSetHandMesh = false;
}

ioTraceSkill::ioTraceSkill( const ioTraceSkill &rhs )
: ioAttackSkill( rhs ),
  m_fTargetRange( rhs.m_fTargetRange ),
  m_fTargetMinRange( rhs.m_fTargetMinRange ),
  m_fTargetAngle( rhs.m_fTargetAngle ),
 m_TargetEffect( rhs.m_TargetEffect ),
 m_TargetWoundType( rhs.m_TargetWoundType )
{
	m_dwFireStartTime = 0;
	m_bSetHandMesh = false;
}

ioTraceSkill::~ioTraceSkill()
{
}

void ioTraceSkill::LoadProperty( ioINILoader &rkLoader )
{
	ioAttackSkill::LoadProperty( rkLoader );

	char szBuf[MAX_PATH];

	m_fTargetAngle = rkLoader.LoadFloat_e( "target_angle", 0.0f );
	m_fTargetRange = rkLoader.LoadFloat_e( "target_max_range", 0.0f );
	m_fTargetMinRange = rkLoader.LoadFloat_e( "target_min_range", 0.0f );
	m_fTargetMinRange = min( m_fTargetMinRange, m_fTargetRange );

	rkLoader.LoadString_e( "target_effect", "", szBuf, MAX_PATH );
	m_TargetEffect = szBuf;

	m_TargetWoundType = (TargetWoundType)rkLoader.LoadInt_e( "target_wound_type", TWT_ALL );
}

ioSkill* ioTraceSkill::Clone()
{
	return new ioTraceSkill( *this );
}

bool ioTraceSkill::OnSkillStart( ioBaseChar *pChar )
{
	if( !ioAttackSkill::OnSkillStart( pChar ) )
		return false;

	m_vTargetList.clear();
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

void ioTraceSkill::OnProcessState( ioBaseChar *pChar, ioPlayStage *pStage )
{
	if( m_dwPreDelayStartTime == 0 )
	{
		CheckExtraAniJump( pChar );
		CheckAniRotate( pChar );

		m_CurTargetName.Clear();

		if( m_dwFireStartTime > 0 && m_dwFireStartTime < FRAMEGETTIME() )
		{
			// 타겟의 개수만큼 충돌체 생성
			int iTargetCnt = m_vTargetList.size();
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
										  m_dwWeaponBaseIndex,
										  vPos);

				iCurFireCnt++;
			}

			if( iCurMaxCnt > 0 && iCurFireCnt < iCurMaxCnt )
			{
				m_CurTargetName.Clear();

				D3DXVECTOR3 vPos = pChar->GetMidPositionByRate();
				pChar->SkillFireExplicit( GetAttribute().m_vWeaponInfoList.front(),
										  GetName(),
										  m_dwWeaponBaseIndex,
										  vPos);
			}

			m_dwFireStartTime = 0;
		}

		return;
	}

	DWORD dwPreDelay = GetPreDelayTime();

	if( m_dwPreDelayStartTime + dwPreDelay < FRAMEGETTIME() )
	{
		m_dwPreDelayStartTime = 0;

		AttackSkillFire( pChar );
		EndPreDelayEffect( pChar );
	}
}

void ioTraceSkill::OnSkillEnd( ioBaseChar *pChar )
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

bool ioTraceSkill::CheckUseItemMesh()
{
	return m_bUseItemMesh;
}

void ioTraceSkill::AttackSkillFire( ioBaseChar *pChar )
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

	m_dwAttackEndTime = pGrp->GetAnimationEventTime_e( iAniID, "attack_end" ) * fTimeRate;
	if( m_dwAttackEndTime > 0 )
		m_dwAttackEndTime += dwCurTime;
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
	pChar->ReduceNeedGaugeBySkill( this, GetNeedGauge(pChar->GetCharName()) );
	m_bReduceGauge = true;
	//

	SetSkillActionState( pChar );
}

void ioTraceSkill::SetSkillActionState( ioBaseChar *pChar )
{
	if( !m_bSetHandMesh && m_bUseItemMesh && !m_HandMesh.IsEmpty() )
	{
		pChar->ChangeSkillWeaponMesh( this, m_HandMesh, m_bVisibleEquipMesh );
		m_bSetHandMesh = true;
	}
}

void ioTraceSkill::FindTarget( ioBaseChar *pOwner )
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

bool ioTraceSkill::IsEnableTargetState( ioBaseChar *pTarget, bool bCheckTargetWound )
{
	if( !pTarget )
		return false;

	if( pTarget->IsSystemState() )
		return false;
		
	if( !pTarget->IsEnableTarget() )
		return false;

	if( bCheckTargetWound )
	{
		if( !pTarget->CheckTargetWoundType(m_TargetWoundType) )
			return false;
	}

	return true;
}

void ioTraceSkill::ApplyAttributeToWeapon( ioWeapon *pWeapon, ioPlayStage *pStage )
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

void ioTraceSkill::ApplyExtraSkillInfo( ioBaseChar *pChar, ioPlayStage *pStage, SP2Packet &rkPacket )
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
	}

}