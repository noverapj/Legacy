

#include "stdafx.h"

#include "ioFireAndLinearSkill.h"

#include "ioPlayStage.h"
#include "ioBaseChar.h"

#include "WeaponDefine.h"

ioFireAndLinearSkill::ioFireAndLinearSkill()
{
	m_bSetHandMesh = false;

	m_TargetCharList.clear();
	m_dwTartgetAttackFireTime = 0;
	m_vTargetPos = ioMath::VEC3_ZERO;
}

ioFireAndLinearSkill::ioFireAndLinearSkill( const ioFireAndLinearSkill &rhs )
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
	m_FireType( rhs.m_FireType ),
	m_SecFireList( rhs.m_SecFireList )
{
	m_bSetHandMesh = false;

	m_TargetCharList.clear();
	m_dwTartgetAttackFireTime = 0;
	m_vTargetPos = ioMath::VEC3_ZERO;

	m_FireState = FS_NONE;
	m_nSecFireListIndex = 0;
}

ioFireAndLinearSkill::~ioFireAndLinearSkill()
{
}

void ioFireAndLinearSkill::LoadProperty( ioINILoader &rkLoader )
{
	ioAttackSkill::LoadProperty( rkLoader );

	m_TargetColType = (TargetColType)rkLoader.LoadInt_e( "target_collision_type", TCT_SPHERE );
	m_TargetWoundType = (TargetWoundType)rkLoader.LoadInt_e( "target_wound_type", TWT_ALL );

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

	m_FireType = (FireType)rkLoader.LoadInt_e( "fire_type", FT_NORMAL );

	//////////////////////////////////////////////////////////////////////////
	char szBuf[MAX_PATH] = "";
	char szKey[MAX_PATH] = "";

	int iComboCnt = rkLoader.LoadInt_e( "linear_combo_cnt", 0 );
	m_SecFireList.clear();
	if( iComboCnt > 0 )
	{
		for( int i=0; i < iComboCnt; ++i )
		{
			AttackAttribute kComboAttack;

			wsprintf_e( szKey, "linear_combo%d", i+1 );
			LoadAttackAttribute( szKey, kComboAttack, rkLoader );

			m_SecFireList.push_back( kComboAttack );
		}
	}
}

ioSkill* ioFireAndLinearSkill::Clone()
{
	return new ioFireAndLinearSkill( *this );
}

bool ioFireAndLinearSkill::OnSkillStart( ioBaseChar *pChar )
{
	m_TargetCharList.clear();
	m_dwTartgetAttackFireTime = 0;
	m_vTargetPos = ioMath::VEC3_ZERO;

	if( !ioAttackSkill::OnSkillStart( pChar ) )
		return false;

	m_FireState = FS_FIRST_FIRE;
	m_bSetHandMesh = false;

	m_nSecFireListIndex = 0;

	// 기본 무기 손에서 제거
	if( m_bUseItemMesh )
	{
		pChar->ShowWeaponItemMesh( false );
	}

	return true;
}

void ioFireAndLinearSkill::OnProcessState( ioBaseChar *pChar, ioPlayStage *pStage )
{
	DWORD dwCurTime = FRAMEGETTIME();

	switch( m_FireType )
	{
	case FT_NORMAL:
		ioAttackSkill::OnProcessState( pChar, pStage );
		break;
	case FT_TARGET:
		{
			if( m_dwPreDelayStartTime == 0 )
			{
				CheckExtraAniJump( pChar );
				CheckAniRotate( pChar );

				if( m_dwTartgetAttackFireTime > 0 && m_dwTartgetAttackFireTime < dwCurTime )
				{
					m_dwTartgetAttackFireTime = 0;
					CheckTargetFire( pChar );
				}
				return;
			}

			DWORD dwPreDelay = GetPreDelayTime();

			if( m_dwPreDelayStartTime + dwPreDelay < dwCurTime )
			{
				m_dwPreDelayStartTime = 0;

				SetTargetAttackAction( pChar );
				EndPreDelayEffect( pChar );
			}
		}
		break;
	}

	//////////////////////////////////////////////////////////////////////////
	switch( m_FireState )
	{
	case FS_FIRST_FIRE:
		break;
	case FS_WAIT_FIRE:
		if( pChar->IsNeedProcess() && m_dwFireMotionEndTime < dwCurTime )
			SetNextLinearCombo( pChar );
		break;
	case FS_SEC_FIRE:
		CheckLinearCombo( pChar );
		break;
	}
	//////////////////////////////////////////////////////////////////////////
}

void ioFireAndLinearSkill::AttackSkillFire( ioBaseChar *pChar )
{
	ioAttackSkill::AttackSkillFire( pChar );

	SetSkillActionState( pChar );
}

void ioFireAndLinearSkill::SetSkillActionState( ioBaseChar *pChar )
{
	if( !m_bSetHandMesh && m_bUseItemMesh && !m_HandMesh.IsEmpty() )
	{
		pChar->ChangeSkillWeaponMesh( this, m_HandMesh, m_bVisibleEquipMesh );
		m_bSetHandMesh = true;
	}
}

void ioFireAndLinearSkill::OnSkillEnd( ioBaseChar *pChar )
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

bool ioFireAndLinearSkill::CheckUseItemMesh()
{
	return m_bUseItemMesh;
}

void ioFireAndLinearSkill::ApplyAttributeToWeapon( ioWeapon *pWeapon, ioPlayStage *pStage )
{
	ioSkill::ApplyAttributeToWeapon( pWeapon, pStage );

	SetChargingRateForWeapon( pWeapon );

	ioArrowWeapon *pArrow = ToArrowWeapon( pWeapon );
	ioMissileWeapon *pMissile = ToMissileWeapon( pWeapon );
	ioBombMissileWeapon *pBombMissile = ToBombMissileWeapon( pWeapon );
	ioTraceWeaponType2 *pTrace2 = ToTraceWeaponType2( pWeapon );

	if( (pArrow || pMissile || pBombMissile || pTrace2) && m_vTargetPos != ioMath::VEC3_ZERO )
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
}

void ioFireAndLinearSkill::FindTargetList( ioBaseChar *pOwner )
{
	m_TargetCharList.clear();

	if( !pOwner ) return;

	ioPlayStage *pStage = pOwner->GetCreator();
	if( !pStage ) return;

	BaseCharList vTargetList;

	D3DXVECTOR3 vPos = pOwner->GetMidPositionByRate();
	D3DXVECTOR3 vSightDir = pOwner->GetTargetRot() * D3DXVECTOR3( 0.0f, 0.0f, -FLOAT1 );
	D3DXVec3Normalize( &vSightDir, &vSightDir );

	if( m_fOffSet != 0.0f )
		vPos += vSightDir * m_fOffSet;

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
		vPos += vSightDir * m_fSkillRange * FLOAT05; 
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
		ioBaseChar *pTarget = (*iter);

		if( !pTarget )
			continue;
		
		if( pTarget == pOwner )
			continue;

		if( !pStage->IsCollisionCheckByMode() )
			continue;


		if( pTarget->IsSystemState() )
			continue;
		
		if( !pTarget->IsEnableTarget() ) 	
			continue;


		if( !pTarget->CheckTargetWoundType( m_TargetWoundType ) )
			continue;

		D3DXVECTOR3 vDiff = pTarget->GetWorldPosition() - pOwner->GetWorldPosition();
		float fDistanceSq = D3DXVec3LengthSq( &vDiff );

		BaseCharDistanceSq kTarget;
		kTarget.m_Name = pTarget->GetCharName();
		kTarget.m_fDistanceSq = fDistanceSq;
		kTarget.m_vPos = pTarget->GetMidPositionByRate();

		m_TargetCharList.push_back( kTarget );
	}

	if( !m_TargetCharList.empty() )
	{
		std::sort( m_TargetCharList.begin(), m_TargetCharList.end(), BaseCharDistanceSqSort() );
	}
}

void ioFireAndLinearSkill::SetTargetAttackAction( ioBaseChar *pOwner )
{
	if( !pOwner )
		return;

	ioEntityGroup *pGrp = pOwner->GetGroup();
	int iAniID = pGrp->GetAnimationIdx( GetAttribute().m_AttackAnimation );
	if( iAniID == -1 )	return;

	pOwner->AttachEffect( m_SkillEffectName );

	float fTimeRate  = GetAttackTimeRate();
	DWORD dwPreDelay = GetAttribute().m_dwPreDelay;

	pGrp->ClearAllLoopAni( FLOAT100, true );
	pGrp->ClearAllActionAni( FLOAT100, true );

	m_dwMotionStartTime = m_dwMotionEndTime = FRAMEGETTIME();
	m_dwMotionEndTime += pGrp->GetAnimationFullTime( iAniID ) * fTimeRate;
	m_dwMotionEndTime += dwPreDelay + GetAttribute().m_dwEndDelay;

	DWORD dwCurTime = FRAMEGETTIME();
	m_dwTartgetAttackFireTime = m_dwFireMotionEndTime = dwCurTime;

	m_dwTartgetAttackFireTime += dwPreDelay;
	m_dwTartgetAttackFireTime += pGrp->GetAnimationEventTime_e( iAniID, "fire_s" ) * fTimeRate;

	float fAttackEnd = pGrp->GetAnimationEventTime_e( iAniID, "attack_end" ) * fTimeRate;
	if( fAttackEnd > 0.0f )
		m_dwAttackEndTime = dwCurTime + fAttackEnd;
	else
		m_dwAttackEndTime = 0;

	//
	SetExtraAniJump( pOwner, iAniID, fTimeRate );

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
	pOwner->CheckCharColSkipSkillTime( iAniID, fTimeRate, dwPreDelay );

	m_dwFireMotionEndTime += dwPreDelay;
	m_dwFireMotionEndTime += pGrp->GetAnimationEventTime_e( iAniID, "firemotion_e" ) * fTimeRate;

	m_dwEnableReserveTime = dwCurTime;
	m_dwEnableReserveTime += pGrp->GetAnimationEventTime_e( iAniID, "key_reserve" ) * fTimeRate;

	if( m_dwTartgetAttackFireTime >= m_dwFireMotionEndTime )
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

	pOwner->SetReservedSliding( GetAttribute().m_vForceInfoList, iAniID, fTimeRate, dwPreDelay );

	if( !m_bReduceGauge )
	{
		pOwner->ReduceNeedGaugeBySkill( this, GetNeedGauge(pOwner->GetCharName()) );
		m_bReduceGauge = true;
	}

	SetSkillActionState( pOwner );
	FindTargetList( pOwner );
}

void ioFireAndLinearSkill::CheckTargetFire( ioBaseChar *pOwner )
{
	if( !pOwner )
		return;

	if( m_TargetCharList.empty() )
	{
		m_vTargetPos = ioMath::VEC3_ZERO;

		pOwner->SkillFireExplicit( GetAttribute().m_vWeaponInfoList.front(),
			GetName(),
			m_dwWeaponBaseIndex,
			pOwner->GetWorldPosition() );

		return;
	}

	BaseCharDistanceSqList::iterator iter = m_TargetCharList.begin();

	int iWeaponCnt = 0;
	while( iter != m_TargetCharList.end() )
	{
		m_vTargetPos = iter->m_vPos;

		pOwner->SkillFireExplicit( GetAttribute().m_vWeaponInfoList.front(),
			GetName(),
			m_dwWeaponBaseIndex+iWeaponCnt,
			pOwner->GetWorldPosition() );

		iWeaponCnt++;

		if( iWeaponCnt >= m_iMaxFireCnt )
			break;

		++iter;
	}
}


void ioFireAndLinearSkill::SetNextSkillState( ioBaseChar *pWounder, ioWeapon *pWeapon  )
{
	m_FireState = FS_WAIT_FIRE;
	ioItem *pItem = GetOwnerItem();
	if ( pItem && pWounder && pWounder->IsNeedSendNetwork() )
	{
		SP2Packet kPacket( CUPK_SKILL_EXTRA_INFO );
		kPacket << pItem->GetOwnerName();
		kPacket << GetName();
		kPacket << (int)m_FireState;
		P2PNetwork::SendToAllPlayingUser( kPacket );
	}
}


bool ioFireAndLinearSkill::IsSkillMotionEnd( DWORD dwActionStop, ioBaseChar* pOwner ) const
{
	if( m_FireState == FS_WAIT_FIRE )
		return false;

	if( m_FireState == FS_FIRST_FIRE || m_FireState == FS_SEC_FIRE )
		return __super::IsSkillMotionEnd( dwActionStop, pOwner );
	
	return true;
}


void ioFireAndLinearSkill::SetChangeSecFireState( ioBaseChar *pOwner )
{
	m_FireState = FS_SEC_FIRE;
	m_nSecFireListIndex = 0;
}


void ioFireAndLinearSkill::CheckLinearCombo( ioBaseChar *pChar )
{
	DWORD dwCurTime = FRAMEGETTIME();
	if( dwCurTime > m_dwMotionEndTime )
		SetNextLinearCombo( pChar );
}


void ioFireAndLinearSkill::SetNextLinearCombo( ioBaseChar *pChar )
{
	if( !pChar ) return;

	m_dwEnableReserveTime = 0;
	pChar->ClearSkillReserveKey();

	int iMaxCombo = m_SecFireList.size();
	if( !COMPARE( m_nSecFireListIndex, 0, iMaxCombo ) )
	{
		SetEndState( pChar );
		return;
	}

	/*m_SkillState = SS_COMBO;
	m_CurDirKey = ioUserKeyInput::DKI_NONE;*/

	/*if( pChar->IsNeedProcess() && pChar->IsSettedDirection() && m_bUseChangeDirection )
	{
		ioUserKeyInput::DirKeyInput eNewDirKey = pChar->CheckCurDirKey();
		m_CurDirKey = eNewDirKey;
		pChar->SetTargetRotToDirKey( eNewDirKey );
	}*/

	ioEntityGroup *pGrp = pChar->GetGroup();
	AttackAttribute kAttack = m_SecFireList[m_nSecFireListIndex];

	int iAniID = pGrp->GetAnimationIdx( kAttack.m_AttackAnimation );
	if( iAniID == -1 )	return;

	m_FireState = FS_SEC_FIRE;

	pChar->InitActionStopDelay( true );

	/*if( m_bNoGravityCombo )
	{
		pChar->SetGravityAmt( 0.0f );
		pChar->SetForcePowerAmt( 0.0f );
		pChar->SetCurJumpPowerAmt( 0.0f );
	}*/

	float fTimeRate = kAttack.m_fAttackAniRate;
	DWORD dwPreDelay = kAttack.m_dwPreDelay;

	pGrp->ClearAllLoopAni( FLOAT100, true );
	pGrp->ClearAllActionAni( FLOAT100, true );

	if( IsUseAutoTargetValue() )
	{
		pChar->SetAutoTarget( ATT_SKILL );
	}

	//if( m_bRefreshFireTimeList )
	{
		if( m_nSecFireListIndex == 0 )
			m_dwWeaponBaseIndex = pChar->IncreaseWeaponIndexBase();

		pChar->RefreshFireTimeListWithBase( m_dwWeaponBaseIndex+m_nSecFireListIndex,
			iAniID,
			kAttack.m_vWeaponInfoList,
			FTT_SKILL_ATTACK,
			fTimeRate,
			dwPreDelay );
	}

	DWORD dwCurTime = FRAMEGETTIME();
	m_dwMotionEndTime = dwCurTime;
	m_dwMotionEndTime += pGrp->GetAnimationFullTime( iAniID ) * fTimeRate;
	m_dwMotionEndTime += dwPreDelay + kAttack.m_dwEndDelay;

	DWORD dwFireStartTime;
	dwFireStartTime = dwCurTime;

	dwFireStartTime += dwPreDelay;
	dwFireStartTime += pGrp->GetAnimationEventTime_e( iAniID, "fire_s" ) * fTimeRate;

	float fAttackEnd = pGrp->GetAnimationEventTime_e( iAniID, "attack_end" ) * fTimeRate;
	if( fAttackEnd > 0.0f )
		m_dwAttackEndTime = dwCurTime + fAttackEnd;
	else
		m_dwAttackEndTime = 0;

	m_dwEnableReserveTime = dwCurTime;
	m_dwEnableReserveTime += pGrp->GetAnimationEventTime_e( iAniID, "key_reserve" ) * fTimeRate;

	if( kAttack.m_AttackEndAnimation.IsEmpty() )
	{
		pGrp->SetActionAni( iAniID, FLOAT100, FLOAT100, FLOAT1, FLOAT1/fTimeRate, 0.0f, dwPreDelay, true );
	}
	else
	{
		pGrp->SetActionAni( iAniID, FLOAT100, 0.0f, FLOAT1, FLOAT1/fTimeRate, 0.0f, dwPreDelay );

		float fWaitTime = pGrp->GetAnimationFullTime( iAniID ) * fTimeRate + dwPreDelay;
		float fEndTimeRate = kAttack.m_fAttackEndAniRate;

		pGrp->SetActionAni( kAttack.m_AttackEndAnimation,
			0.0f, FLOAT100,
			1.0f, FLOAT1/fEndTimeRate,
			fWaitTime, 0.0f, true );

		int iEndAniID = pGrp->GetAnimationIdx( kAttack.m_AttackEndAnimation );
		m_dwMotionEndTime += pGrp->GetAnimationFullTime( iEndAniID ) * fEndTimeRate;
	}

	pChar->CheckCharColSkipSkillTime( iAniID, fTimeRate, dwPreDelay );

	SetExtraAniJump( pChar, iAniID, fTimeRate );

	//
	DWORD dwTrackingTime = dwCurTime + dwPreDelay;
	dwTrackingTime += pGrp->GetAnimationEventTime_e( iAniID, "tracking_e" ) * fTimeRate;

	if( IsUseAutoTargetValue() )
	{
		pChar->SetAutoTargetTrackingEndTime( dwTrackingTime );
	}

	//
	pChar->SetReservedSliding( kAttack.m_vForceInfoList, iAniID, fTimeRate, dwPreDelay );

	//m_bUsedComboList = true;

	if( pChar->IsNeedSendNetwork() )
	{
		SP2Packet kPacket( CUPK_SKILL_EXTRA_INFO );
		kPacket << pChar->GetCharName();
		kPacket << GetName();
		kPacket << (int)m_FireState;
		kPacket << m_nSecFireListIndex;

		if( IsUseAutoTargetValue() )
		{
			pChar->FillAutoTargetInfo( kPacket );
		}
		P2PNetwork::SendToAllPlayingUser( kPacket );
	}

	//if ( !m_bUseEndCombo )
	{
		m_nSecFireListIndex++;
	}
}


void ioFireAndLinearSkill::SetEndState( ioBaseChar *pChar )
{
	m_FireState = FS_END;
}

void ioFireAndLinearSkill::ApplyExtraSkillInfo( ioBaseChar *pChar, ioPlayStage *pStage, SP2Packet &rkPacket )
{
	if( !pChar )	return;
	if( !pStage )	return;

	int iState;
	rkPacket >> iState;
	m_FireState = (FireState)iState;

	if( m_FireState == FS_SEC_FIRE )
	{
		rkPacket >> m_nSecFireListIndex;



		if( IsUseAutoTargetValue() )
		{
			pChar->ApplyAutoTargetInfo( rkPacket );
		}

		SetNextLinearCombo( pChar );
	}
	/*else
	{

	}*/
	
}