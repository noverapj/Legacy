

#include "stdafx.h"

#include "ioBaseChar.h"
#include "ioPlayStage.h"

#include "ioFireSkill2.h"

#include "WeaponDefine.h"

ioFireSkill2::ioFireSkill2()
{
	m_bSetHandMesh = false;

	m_SkillState = SS_NONE;
	m_iCurFireCnt = 0;
	m_vTargetPos = ioMath::VEC3_ZERO;
}

ioFireSkill2::ioFireSkill2( const ioFireSkill2 &rhs )
: ioAttackSkill( rhs ),
m_TargetColType( rhs.m_TargetColType ),
m_TargetWoundType( rhs.m_TargetWoundType ),
m_fSkillRange( rhs.m_fSkillRange ),
m_fSkillAngle( rhs.m_fSkillAngle ),
m_fOffSet( rhs.m_fOffSet ),
m_fHeightOffSet( rhs.m_fHeightOffSet ),
m_fUpHeight( rhs.m_fUpHeight ),
m_fUnderHeight( rhs.m_fUnderHeight ),
m_fWidth( rhs.m_fWidth ),
m_iMaxFireCnt( rhs.m_iMaxFireCnt ),
m_dwSkillProtectTime( rhs.m_dwSkillProtectTime ),
m_FireUpAni( rhs.m_FireUpAni ),
m_FireDownAni( rhs.m_FireDownAni ),
m_fTargetAimRate( rhs.m_fTargetAimRate ),
m_fDownTargetAimRate( rhs.m_fDownTargetAimRate ),
m_szAimedMarker( rhs.m_szAimedMarker )
{
	m_bSetHandMesh = false;

	m_SkillState = SS_NONE;
	m_iCurFireCnt = 0;
	m_vTargetPos = ioMath::VEC3_ZERO;
}

ioFireSkill2::~ioFireSkill2()
{
}

void ioFireSkill2::LoadProperty( ioINILoader &rkLoader )
{
	ioAttackSkill::LoadProperty( rkLoader );

	char szBuf[MAX_PATH];

	m_TargetColType = (TargetColType)rkLoader.LoadInt_e( "target_collision_type", TCT_SPHERE );
	m_TargetWoundType = (TargetWoundType)rkLoader.LoadInt_e( "target_wound_type", TWT_ALL );

	m_dwSkillProtectTime = (DWORD)rkLoader.LoadInt_e( "skill_protect_time", 0 );

	m_fSkillRange = rkLoader.LoadFloat_e( "target_range", 0.0f );
	m_fSkillAngle = rkLoader.LoadFloat_e( "target_angle", 0.0f );
	m_fOffSet = rkLoader.LoadFloat_e( "target_offset", 0.0f );
	m_fHeightOffSet = rkLoader.LoadFloat_e( "target_height_offset", 0.0f );

	m_fUpHeight = rkLoader.LoadFloat_e( "target_up_height", 0.0f );
	m_fUpHeight = max( FLOAT1, m_fUpHeight );
	m_fUnderHeight = rkLoader.LoadFloat_e( "target_under_height", 0.0f );
	m_fUnderHeight = max( FLOAT1, m_fUnderHeight );
	m_fWidth = rkLoader.LoadFloat_e( "target_width", 0.0f );

	m_iMaxFireCnt = rkLoader.LoadInt_e( "max_fire_cnt", 1 );
	m_iMaxFireCnt = max( 1, m_iMaxFireCnt );

	rkLoader.LoadString_e( "fire_up_ani", "", szBuf, MAX_PATH );
	m_FireUpAni = szBuf;
	rkLoader.LoadString_e( "fire_down_ani", "", szBuf, MAX_PATH );
	m_FireDownAni = szBuf;

	m_fTargetAimRate = rkLoader.LoadFloat_e( "target_aim_rate", FLOAT05 );
	m_fDownTargetAimRate = rkLoader.LoadFloat_e( "down_target_aim_rate", FLOAT05 );

	rkLoader.LoadString_e( "aimed_marker", "", szBuf, MAX_PATH );
	m_szAimedMarker = szBuf;
}

ioSkill* ioFireSkill2::Clone()
{
	return new ioFireSkill2( *this );
}

bool ioFireSkill2::OnSkillStart( ioBaseChar *pChar )
{
	if( !ioAttackSkill::OnSkillStart( pChar ) )
		return false;

	m_bSetHandMesh = false;

	m_SkillState = SS_NONE;
	m_iCurFireCnt = 0;
	m_vTargetPos = ioMath::VEC3_ZERO;
	m_dwActionStartTime = 0;
	m_LastTargetName.Clear();

	// set marker
	pChar->SetTargetMarker( ioTargetMarker::MT_OTHER_MARKER,
							m_szAimedMarker,
							m_szAimedMarker,
							m_szAimedMarker,
							m_szAimedMarker,
							"",
							TT_OTHER );

	pChar->SetTargetMarkerState( ioTargetMarker::MS_NONE, TT_OTHER );

	// 기본 무기 손에서 제거
	if( m_bUseItemMesh )
	{
		pChar->ShowWeaponItemMesh( false );
	}

	return true;
}

void ioFireSkill2::SetSkillActionState( ioBaseChar *pChar, ioPlayStage *pStage )
{
	DWORD dwCurTime = FRAMEGETTIME();
	
	m_iCurFireCnt = 0;
	CheckNextTarget( pChar, pStage );

	if( !m_bSetHandMesh && m_bUseItemMesh && !m_HandMesh.IsEmpty() )
	{
		pChar->ChangeSkillWeaponMesh( this, m_HandMesh, m_bVisibleEquipMesh );
		m_bSetHandMesh = true;
	}

	m_SkillState = SS_ACTION;
	m_dwActionStartTime = dwCurTime;

	if( pChar->IsNeedSendNetwork() )
	{
		SP2Packet kPacket( CUPK_SKILL_EXTRA_INFO );
		kPacket << pChar->GetCharName();
		kPacket << GetName();
		kPacket << ST_STATE;
		kPacket << m_SkillState;
		kPacket << pChar->GetTargetRot();
		kPacket << m_vTargetPos;
		kPacket << m_iCurFireCnt;
		kPacket << m_LastTargetName;
		P2PNetwork::SendToAllPlayingUser( kPacket );
	}
}

void ioFireSkill2::OnProcessState( ioBaseChar *pChar, ioPlayStage *pStage )
{
	if( !pChar )
		return;

	DWORD dwCurTime = FRAMEGETTIME();
	DWORD dwPreDelay = GetPreDelayTime();

	switch( m_SkillState )
	{
	case SS_NONE:
		if( m_dwPreDelayStartTime + dwPreDelay < dwCurTime )
		{
			m_dwPreDelayStartTime = 0;

			SetSkillActionState( pChar, pStage );
			EndPreDelayEffect( pChar );
		}
		break;
	case SS_ACTION:
		if( pChar->IsNeedProcess() )
		{
			if( m_dwMotionEndTime < dwCurTime && m_iCurFireCnt < m_iMaxFireCnt )
			{
				CheckNextTarget( pChar, pStage );

				if( pChar->IsNeedSendNetwork() )
				{
					SP2Packet kPacket( CUPK_SKILL_EXTRA_INFO );
					kPacket << pChar->GetCharName();
					kPacket << GetName();
					kPacket << ST_FIRE;
					kPacket << pChar->GetTargetRot();
					kPacket << m_vTargetPos;
					kPacket << m_iCurFireCnt;
					kPacket << m_LastTargetName;
					P2PNetwork::SendToAllPlayingUser( kPacket );
				}
				return;
			}
		}
		break;
	}
}

void ioFireSkill2::OnSkillEnd( ioBaseChar *pChar )
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
	pChar->SetTargetMarkerState( ioTargetMarker::MS_NONE, TT_OTHER );
}

bool ioFireSkill2::CheckUseItemMesh()
{
	return m_bUseItemMesh;
}

void ioFireSkill2::ApplyAttributeToWeapon( ioWeapon *pWeapon, ioPlayStage *pStage )
{
	if( !pWeapon ) return;

	ioSkill::ApplyAttributeToWeapon( pWeapon, pStage );

	SetChargingRateForWeapon( pWeapon );

	ioArrowWeapon *pArrow = ToArrowWeapon( pWeapon );
	ioMissileWeapon *pMissile = ToMissileWeapon( pWeapon );
	ioBombMissileWeapon *pBombMissile = ToBombMissileWeapon( pWeapon );

	if( (pArrow || pMissile || pBombMissile) && m_vTargetPos != ioMath::VEC3_ZERO )
	{
		D3DXVECTOR3 vDir = m_vTargetPos - pWeapon->GetPosition();
		D3DXVec3Normalize( &vDir, &vDir );

		pWeapon->SetMoveDir( vDir );
	}

	ioSylphidMissileWeapon *pSylphid = ToSylphidMissileWeapon( pWeapon );
	if( pSylphid )
	{
		g_WeaponMgr.CheckSylphidMissileWeapon( pWeapon );
	}

	ioBaseChar *pChar = pWeapon->GetOwner();
	if( pChar )
	{
		pChar->SetTargetMarkerState( ioTargetMarker::MS_NONE, TT_OTHER );
	}
}

void ioFireSkill2::CheckNextTarget( ioBaseChar *pChar, ioPlayStage *pStage )
{
	if( !pChar ) return;
	if( !pStage ) return;

	BaseCharList vTargetList;
	BaseCharDistanceSqList vNewCharList;

	D3DXVECTOR3 vPos = pChar->GetMidPositionByRate();
	D3DXVECTOR3 vSightDir = pChar->GetTargetRot() * D3DXVECTOR3( 0.0f, 0.0f, -FLOAT1 );
	D3DXVec3Normalize( &vSightDir, &vSightDir );

	if( m_fOffSet != 0.0f )
		vPos += vSightDir * m_fOffSet;

	// 충돌타입에 따라 리스트 가져오기
	switch( m_TargetColType )
	{
	case TCT_SPHERE:
		pChar->GetTargetCharListBySphere( vPos, m_fSkillRange, m_fSkillAngle, vTargetList );
		break;
	case TCT_CYLINDER:
		pChar->GetTargetCharListByCylinder( vPos, m_fSkillRange, m_fSkillAngle, m_fUpHeight, m_fUnderHeight, vTargetList );
		break;
	case TCT_BOX:
		// 사각형의 center을 vPos로 설정하게 되므로 여기서 캐릭터앞쪽에 생성하고 높이를 보정한다.
		vPos += vSightDir * m_fSkillRange * FLOAT05; 
		D3DXVECTOR3 vOffset( 0.0f, ( ( m_fUpHeight + m_fUnderHeight ) * FLOAT05 ) - m_fUpHeight, 0.0f );
		vPos -= vOffset;
		pChar->GetTargetCharListByBox( vPos, m_fSkillRange, m_fWidth, m_fUpHeight, m_fUnderHeight, vTargetList );
		break;
	}

	if( pChar->GetExperienceMode() == EMS_EXPERIENCE )
		vTargetList.clear();

	BaseCharList::iterator iter;
	for( iter = vTargetList.begin(); iter != vTargetList.end(); ++iter )
	{
		ioBaseChar *pTarget = (*iter);

		if( !pTarget )
			continue;

		if( pTarget == pChar )
			continue;

		if( pChar->GetTeam() == pTarget->GetTeam() )
			continue;

		if( pTarget->IsSystemState() )
			continue;
		
		if( !pTarget->IsEnableTarget() ) 	
			continue;

		if( !pTarget->CheckTargetWoundType(m_TargetWoundType) )
			continue;

		D3DXVECTOR3 vDiff = pTarget->GetWorldPosition() - pChar->GetWorldPosition();
		float fDistanceSq = D3DXVec3LengthSq( &vDiff );

		BaseCharDistanceSq kTarget;
		kTarget.m_Name = pTarget->GetCharName();
		kTarget.m_fDistanceSq = fDistanceSq;

		vNewCharList.push_back( kTarget );
	}

	if( vNewCharList.empty() )
	{
		m_vTargetPos = ioMath::VEC3_ZERO;
		m_LastTargetName.Clear();
	}
	else
	{
		std::sort( vNewCharList.begin(), vNewCharList.end(), BaseCharDistanceSqSort2() );

		ioHashString szCurTarget;
		if( m_LastTargetName.IsEmpty() )
		{
			szCurTarget = vNewCharList.front().m_Name;
		}
		else
		{
			BaseCharDistanceSqList::iterator new_iter = vNewCharList.begin();
			while( new_iter != vNewCharList.end() )
			{
				if( (*new_iter).m_Name != m_LastTargetName )
				{
					szCurTarget = (*new_iter).m_Name;
					break;
				}

				new_iter++;
			}

			if( szCurTarget.IsEmpty() )
				szCurTarget = m_LastTargetName;
		}

		ioBaseChar *pTarget = pStage->GetBaseChar( szCurTarget );
		if( pTarget )
		{
			m_LastTargetName = szCurTarget;

			if( CheckDownAimState( pTarget ) )
				m_vTargetPos = pTarget->GetMidPositionByRate(m_fDownTargetAimRate);
			else
				m_vTargetPos = pTarget->GetMidPositionByRate(m_fTargetAimRate);
		}
		else
		{
			m_vTargetPos = ioMath::VEC3_ZERO;
			m_LastTargetName.Clear();
		}
	}

	if( !m_LastTargetName.IsEmpty() )
	{
		pChar->SetTargetMarkerTarget( m_LastTargetName, false, TT_OTHER );
		pChar->SetTargetMarkerState( ioTargetMarker::MS_AIMED, TT_OTHER );
	}
	else
	{
		pChar->SetTargetMarkerState( ioTargetMarker::MS_NONE, TT_OTHER );
	}

	if( m_vTargetPos != ioMath::VEC3_ZERO )
	{
		pChar->SetTargetRotToTargetPos( m_vTargetPos, true );
	}

	AttackSkillFire( pChar );
	m_iCurFireCnt++;
}

void ioFireSkill2::AttackSkillFire( ioBaseChar *pChar )
{
	ioEntityGroup *pGrp = pChar->GetGroup();
	int iAniID = pGrp->GetAnimationIdx( GetAttribute().m_AttackAnimation );
	if( iAniID == -1 )	return;

	pChar->AttachEffect( m_SkillEffectName );

	float fTimeRate  = GetAttackTimeRate();
	DWORD dwPreDelay = GetAttribute().m_dwPreDelay;

	pChar->RefreshFireTimeListWithBase( m_dwWeaponBaseIndex,
										iAniID,
										GetAttribute().m_vWeaponInfoList,
										FTT_SKILL_ATTACK,
										fTimeRate,
										dwPreDelay );

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

	if( m_vTargetPos == ioMath::VEC3_ZERO )
	{
		pGrp->SetActionAni( iAniID, FLOAT100, FLOAT100, FLOAT1, FLOAT1/fTimeRate, 0.0f, dwPreDelay, true );
	}
	else
	{
		D3DXVECTOR3 vDir = m_vTargetPos - pChar->GetMidPositionByRate();
		D3DXVec3Normalize( &vDir, &vDir );

		D3DXVECTOR3 vXZDir( vDir.x, 0.0f, vDir.z );
		D3DXVec3Normalize( &vXZDir, &vXZDir );

		float fAngle = D3DXVec3Dot( &vXZDir, &vDir );
		fAngle = max( -FLOAT1, min( fAngle, FLOAT1 ) );
		fAngle = acosf( fAngle );
		fAngle = RADtoDEG( fAngle );
		fAngle = max( 0.0f, min( fAngle, 85.0f ) );

		float fWeight = fAngle / 85.0f;
		if( vDir.y == 0.0f )
			pGrp->SetActionAni( iAniID, FLOAT100, FLOAT100, FLOAT1, FLOAT1/fTimeRate, 0.0f, dwPreDelay, true );
		else if( vDir.y > 0.0f )
			AimedFireToUp( pGrp, fWeight, fTimeRate, dwPreDelay, true );
		else
			AimedFireToDown( pGrp, fWeight, fTimeRate, dwPreDelay, true );
	}

	pChar->SetReservedSliding( GetAttribute().m_vForceInfoList, iAniID, fTimeRate, dwPreDelay );

	if( !m_bReduceGauge )
	{
		pChar->ReduceNeedGaugeBySkill( this, GetNeedGauge(pChar->GetCharName()) );
		m_bReduceGauge = true;
	}
}

void ioFireSkill2::AimedFireToUp( ioEntityGroup *pGrp,
								  float fWeight,
								  float fTimeRate,
								  DWORD dwPreDelay,
								  bool bHold )
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

	if( fWeight > 0.0f )
	{
		pGrp->SetActionAni( m_FireUpAni,
			FLOAT100, fFadeOut,
			fWeight, FLOAT1/fTimeRate,
			0.0f, dwPreDelay,
			bHold, bAniFx1 );
	}

	pGrp->SetActionAni( GetAttribute().m_AttackAnimation,
						FLOAT100, fFadeOut,
						1.0f - fWeight, FLOAT1/fTimeRate,
						0.0f, dwPreDelay,
						bHold, bAniFx2 );
}

void ioFireSkill2::AimedFireToDown( ioEntityGroup *pGrp,
								    float fWeight,
									float fTimeRate,
									DWORD dwPreDelay,
									bool bHold )
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

	if( fWeight > 0.0f )
	{
		pGrp->SetActionAni( m_FireDownAni,
							FLOAT100, fFadeOut,
							fWeight, FLOAT1/fTimeRate,
							0.0f, dwPreDelay,
							bHold, bAniFx2 );
	}

	pGrp->SetActionAni( GetAttribute().m_AttackAnimation,
						FLOAT100, fFadeOut,
						1.0f - fWeight, FLOAT1/fTimeRate,
						0.0f, dwPreDelay,
						bHold, bAniFx1 );
}

void ioFireSkill2::ApplyExtraSkillInfo( ioBaseChar *pChar, ioPlayStage *pStage, SP2Packet &rkPacket )
{
	if( !pChar ) return;
	if( !pStage ) return;

	int iType;
	rkPacket >> iType;

	switch( iType )
	{
	case ST_STATE:
		{
			int iState;
			rkPacket >> iState;

			D3DXQUATERNION qtRot;
			rkPacket >> qtRot;

			pChar->SetTargetRotToRotate( qtRot, true, false );

			switch( iState )
			{
			case SS_ACTION:
				{
					rkPacket >> m_vTargetPos;
					AttackSkillFire( pChar );

					rkPacket >> m_iCurFireCnt;
					rkPacket >> m_LastTargetName;

					if( !m_bSetHandMesh && m_bUseItemMesh && !m_HandMesh.IsEmpty() )
					{
						pChar->ChangeSkillWeaponMesh( this, m_HandMesh, m_bVisibleEquipMesh );
						m_bSetHandMesh = true;
					}

					m_SkillState = SS_ACTION;
					m_dwActionStartTime = FRAMEGETTIME();

					if( !m_LastTargetName.IsEmpty() )
					{
						pChar->SetTargetMarkerTarget( m_LastTargetName, false, TT_OTHER );
						pChar->SetTargetMarkerState( ioTargetMarker::MS_AIMED, TT_OTHER );
					}
					else
					{
						pChar->SetTargetMarkerState( ioTargetMarker::MS_NONE, TT_OTHER );
					}
				}
				break;
			}
		}
		break;
	case ST_FIRE:
		{
			D3DXQUATERNION qtRot;
			rkPacket >> qtRot;

			pChar->SetTargetRotToRotate( qtRot, true, false );

			rkPacket >> m_vTargetPos;
			AttackSkillFire( pChar );

			rkPacket >> m_iCurFireCnt;
			rkPacket >> m_LastTargetName;

			if( !m_LastTargetName.IsEmpty() )
			{
				pChar->SetTargetMarkerTarget( m_LastTargetName, false, TT_OTHER );
				pChar->SetTargetMarkerState( ioTargetMarker::MS_AIMED, TT_OTHER );
			}
			else
			{
				pChar->SetTargetMarkerState( ioTargetMarker::MS_NONE, TT_OTHER );
			}
		}
		break;
	}
}

bool ioFireSkill2::IsSkillMotionEnd( DWORD dwActionStop, ioBaseChar* pOwner ) const
{
	DWORD dwCurTime = FRAMEGETTIME();

	if( m_dwPreDelayStartTime > 0 )
		return false;

	switch( m_SkillState )
	{
	case SS_NONE:
		return false;
	case SS_ACTION:
		if( m_dwMotionEndTime + dwActionStop < dwCurTime && m_iCurFireCnt >= m_iMaxFireCnt )
		{
			return true;
		}
		return false;
	}

	return false;
}

bool ioFireSkill2::IsProtected( int iDefenseBreakType ) const
{
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

	switch( m_SkillState )
	{
	case SS_ACTION:
		if( m_dwActionStartTime+m_dwSkillProtectTime < dwCurTime )
			return false;
		return true;
	case SS_END:
		return false;
	}

	return true;
}

bool ioFireSkill2::CheckDownAimState( ioBaseChar *pTarget )
{
	if( !pTarget ) return false;

	CharState eState = pTarget->GetState();

	if( eState == CS_FROZEN && pTarget->GetBlowFrozenState() )
		return true;

	if( eState == CS_BLOW_WOUNDED && !pTarget->IsFloatingState() )
		return true;

	if( eState == CS_BOUND_BLOW_WOUNDED && !pTarget->IsFloatingState() )
		return true;

	if( eState == CS_ETC_STATE && pTarget->IsApplyDownState(false) )
		return true;

	if( eState == CS_USING_SKILL && pTarget->IsApplyDownState(false) )
		return true;

	if( eState == CS_STOP_MOTION && pTarget->GetBlowStopMotionState() )
		return true;

	return false;
}

