

#include "stdafx.h"

#include "ioPlayStage.h"
#include "ioBaseChar.h"

#include "ioSelectTripleWeaponSkill.h"

#include "WeaponDefine.h"

ioSelectTripleWeaponSkill::ioSelectTripleWeaponSkill()
{
	ClearData();
}

ioSelectTripleWeaponSkill::ioSelectTripleWeaponSkill( const ioSelectTripleWeaponSkill &rhs )
: ioAttackSkill( rhs ),
 m_SkillLoopMotion( rhs.m_SkillLoopMotion ),
 m_dwSkillLoopDuration( rhs.m_dwSkillLoopDuration ),
 m_dwSkillProtectTime( rhs.m_dwSkillProtectTime ),
 m_fAimRange( rhs.m_fAimRange ),
 m_szAimMarkEffect( rhs.m_szAimMarkEffect ),
 m_TargetWoundType( rhs.m_TargetWoundType ),
 m_AttackInfo_A( rhs.m_AttackInfo_A ),
 m_AttackInfo_S( rhs.m_AttackInfo_S ),
 m_AttackInfo_D( rhs.m_AttackInfo_D ),
 m_iAWeaponUseCount( rhs.m_iAWeaponUseCount ),
 m_iSWeaponUseCount( rhs.m_iSWeaponUseCount ),
 m_iDWeaponUseCount( rhs.m_iDWeaponUseCount )
{
	ClearData();
}

ioSelectTripleWeaponSkill::~ioSelectTripleWeaponSkill()
{
	ClearData();
}

void ioSelectTripleWeaponSkill::ClearData()
{
	m_bSetHandMesh = false;

	SetSkillState( SS_NONE );

	m_TargetName.Clear();
	m_dwAimMarkEffectID = -1;
	m_CurDirKey   = ioUserKeyInput::DKI_NONE;
}

void ioSelectTripleWeaponSkill::LoadProperty( ioINILoader &rkLoader )
{
	ioAttackSkill::LoadProperty( rkLoader );

	char szBuf[MAX_PATH];

	rkLoader.LoadString_e( "skill_loop_motion", "", szBuf, MAX_PATH );
	m_SkillLoopMotion = szBuf;

	m_dwSkillLoopDuration = rkLoader.LoadInt_e( "skill_loop_duration", 0 );

	m_dwSkillProtectTime = (DWORD)rkLoader.LoadInt_e( "skill_protect_time", 0 );

	m_fAimRange = rkLoader.LoadFloat_e( "aim_target_range", 0.0f );

	rkLoader.LoadString_e( "aim_mark_effect", "", szBuf, MAX_PATH );
	m_szAimMarkEffect = szBuf;

	m_TargetWoundType =  (TargetWoundType)rkLoader.LoadInt_e( "target_wound_type", TWT_ALL );

	LoadAttackAttribute_e( "a_key_weapon", m_AttackInfo_A, rkLoader );
	LoadAttackAttribute_e( "s_key_weapon", m_AttackInfo_S, rkLoader );
	LoadAttackAttribute_e( "d_key_weapon", m_AttackInfo_D, rkLoader );

	m_iAWeaponUseCount = rkLoader.LoadInt_e( "a_weapon_use_count", 0 );
	m_iSWeaponUseCount = rkLoader.LoadInt_e( "s_weapon_use_count", 0 );
	m_iDWeaponUseCount = rkLoader.LoadInt_e( "d_weapon_use_count", 0 );
}

ioSkill* ioSelectTripleWeaponSkill::Clone()
{
	return new ioSelectTripleWeaponSkill( *this );
}

bool ioSelectTripleWeaponSkill::OnSkillStart( ioBaseChar *pChar )
{
	if( !ioSkill::OnSkillStart( pChar ) )
		return false;

	ClearData();

	SetCameraBuff( pChar );

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

	m_dwWeaponBaseIndex = pChar->IncreaseWeaponIndexBase();

	if( !StartPreDelayAnimation( pChar ) )	// Delay Animation이 없다
	{
		SetLoopState( pChar, g_SkillMaker.GetPlayStage() );
	}
	else
	{
		DWORD dwTrackingTime = 0;
		ioEntityGroup *pGrp = pChar->GetGroup();
		if( pGrp )
		{
			ioHashString szPreDelay = GetPreDelayAniName();

			int iAniID = pGrp->GetAnimationIdx( szPreDelay );
			if( iAniID != -1 )
				dwTrackingTime = pGrp->GetAnimationEventTime_e( iAniID, "tracking_e" );
		}

		DWORD dwPreDelay = GetPreDelayTime();

		if( dwTrackingTime > 0 )
			dwTrackingTime += FRAMEGETTIME();
		else
			dwTrackingTime = FRAMEGETTIME() + dwPreDelay;

		if( IsUseAutoTargetValue() )
		{
			pChar->SetAutoTarget( ATT_SKILL );
			pChar->SetAutoTargetTrackingEndTime( dwTrackingTime );
		}
	}

	pChar->AttachEffect( m_SkillEffectName );

	if( !m_bReduceGauge )
	{
		pChar->ReduceNeedGaugeBySkill( this, GetOwnerItem()->GetMaxSkillGauge(pChar->GetCharName()) );
		m_bReduceGauge = true;
	}

	m_vAttackDir = pChar->GetTargetRot() * D3DXVECTOR3( 0.0f, 0.0f, -FLOAT1 );
	m_iCurActiveCount = m_iMaxActiveCount;
	return true;
}

void ioSelectTripleWeaponSkill::OnProcessState( ioBaseChar *pChar, ioPlayStage *pStage )
{
	DWORD dwCurTime = FRAMEGETTIME();

	switch( m_SkillState )
	{
	case SS_NONE:
		{
			DWORD dwPreDelay = GetPreDelayTime();
			if( m_dwPreDelayStartTime != 0 && m_dwPreDelayStartTime + dwPreDelay < dwCurTime )
			{
				m_dwPreDelayStartTime = 0;

				SetLoopState( pChar, pStage );
			}
		}
		break;
	case SS_LOOP:
		{
			CheckKeyInput( pChar );
			CheckTarget( pChar, pStage );
			UpdateAimMark( pChar, pStage );
			CheckDir( pChar, pStage );

			if( pChar->IsNeedProcess() )
			{
				if( pChar->IsJumpKey() )
				{
					SetAttackKeyType( AKT_JUMP );
					SetFireState( pChar );
					return;
				}
				else if( pChar->IsDefenseKey() )
				{
					SetAttackKeyType( AKT_DEFENSE );
					SetFireState( pChar );
					return;
				}
				else if( pChar->IsAttackKey() )
				{
					SetAttackKeyType( AKT_ATTACK );
					SetFireState( pChar );
					return;
				}
			}
		}
		break;
	case SS_FIRE:
		{
			if( m_dwMotionEndTime < FRAMEGETTIME() && m_iCurActiveCount > 0 )
				SetLoopState( pChar, pStage );
		}
		break;
	}
}

void ioSelectTripleWeaponSkill::OnSkillEnd( ioBaseChar *pChar )
{
	// 기본무기 장착
	if( m_bUseItemMesh )
		pChar->ShowWeaponItemMesh( true );

	if( m_bSetHandMesh && !m_HandMesh.IsEmpty() )
		pChar->RestoreSkillWeaponMesh( this, m_HandMesh, m_bVisibleEquipMesh );

	m_bSetHandMesh = false;

	EndPreDelayEffect( pChar );
	pChar->EndEffect( m_SkillEffectName, false );

	EndCameraBuff( pChar );

	m_bReduceGauge = false;
	pChar->SetSkillMoveEnable( false, false );
	pChar->SetMoveDirByRotate( pChar->GetTargetRot() );
	pChar->RestoreSkillProtection();
	pChar->SetCurMoveSpeed( 0.0f );

	SetSkillState( SS_NONE );
	m_SkillUseType = SUT_NORMAL;

	DestroyAimMark();

	if( !m_SkillEndEffectName.IsEmpty() )
		pChar->AttachEffect( m_SkillEndEffectName );
}

bool ioSelectTripleWeaponSkill::IsSkillMotionEnd( DWORD dwActionStop, ioBaseChar* pOwner ) const
{
	if( m_dwPreDelayStartTime > 0 )
		return false;

	switch( m_SkillState )
	{
	case SS_LOOP:
		return false;
	case SS_FIRE:
		if( m_iCurActiveCount > 0  )
			return false;

		if( m_dwMotionEndTime + dwActionStop > FRAMEGETTIME() )
			return false;

		return true;
	}

	return true;
}

bool ioSelectTripleWeaponSkill::IsProtected( int iDefenseBreakType ) const
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

	if( m_dwSkillProtectTime > 0 && m_dwSkillStartTime + m_dwSkillProtectTime > FRAMEGETTIME() )
		return true;

	return false;
}

bool ioSelectTripleWeaponSkill::IsEnableReserve() const
{
	if( m_dwPreDelayStartTime != 0 )
		return false;

	switch( m_SkillState )
	{
	case SS_LOOP:
		return false;
	case SS_FIRE:
		if( m_dwEnableReserveTime > 0 && m_dwEnableReserveTime < FRAMEGETTIME() )
			return true;

		break;
	}

	return false;
}

void ioSelectTripleWeaponSkill::ApplyExtraSkillInfo( ioBaseChar *pChar, ioPlayStage *pStage, SP2Packet &rkPacket )
{
	int iState;
	rkPacket >> iState;

	switch( iState )
	{
	case SNT_CHANGE:
		{
			rkPacket >> m_TargetName;
		}
		break;
	case SNT_ATTACK:
		{
			int iKeyType;
			rkPacket >> iKeyType;
			m_AttackKeyType = static_cast<AttackKeyType>( iKeyType );

			SetFireState( pChar );
		}
		break;
	}
}

void ioSelectTripleWeaponSkill::ApplyAttributeToWeapon( ioWeapon *pWeapon, ioPlayStage *pStage )
{
	if( !pWeapon ) return;

	ioSkill::ApplyAttributeToWeapon( pWeapon, pStage );

	ioTraceWeaponType2	*pTrace2 = ToTraceWeaponType2( pWeapon );
	if( pTrace2 )
	{
		pTrace2->SetMoveDir( m_vAttackDir );
		pTrace2->SetTraceTarget( m_TargetName );
	}

	ioTraceWeapon7 *pTrace7 = ToTraceWeapon7( pWeapon );
	if( pTrace7 )
		pTrace7->SetTraceTarget( m_TargetName );
}

void ioSelectTripleWeaponSkill::SetLoopState( ioBaseChar *pChar, ioPlayStage *pStage )
{
	if( !pChar ) return;
	ioEntityGroup *pGrp = pChar->GetGroup();
	if( !pGrp ) return;

	EndPreDelayEffect( pChar );

	SetSkillState( SS_LOOP );

	pGrp->ClearAllActionAni( FLOAT100 );
	pGrp->ClearAllLoopAni( FLOAT100 );

	pGrp->SetLoopAni( m_SkillLoopMotion, FLOAT100, FLOAT1 );
}

void ioSelectTripleWeaponSkill::SetFireState( ioBaseChar *pChar )
{
	if( !pChar ) return;

	if( m_AttackKeyType == AKT_JUMP )
	{
		if( m_iCurActiveCount < m_iAWeaponUseCount )
			return;

		m_iCurActiveCount -= m_iAWeaponUseCount;
		m_iCurActiveCount = max( 0, m_iCurActiveCount );
	}
	else if( m_AttackKeyType == AKT_DEFENSE )
	{
		if( m_iCurActiveCount < m_iSWeaponUseCount )
			return;

		m_iCurActiveCount -= m_iSWeaponUseCount;
		m_iCurActiveCount = max( 0, m_iCurActiveCount );
	}
	else if( m_AttackKeyType == AKT_ATTACK )
	{
		if( m_iCurActiveCount < m_iDWeaponUseCount )
			return;

		m_iCurActiveCount -= m_iDWeaponUseCount;
		m_iCurActiveCount = max( 0, m_iCurActiveCount );
	}

	AttackSkillFire( pChar );
	SetSkillState( SS_FIRE );
	DestroyAimMark();

	if( pChar->IsNeedSendNetwork() )
	{
		SP2Packet kPacket( CUPK_SKILL_EXTRA_INFO );
		kPacket << pChar->GetCharName();
		kPacket << GetName();
		kPacket << SNT_ATTACK;
		kPacket << static_cast<int>( m_AttackKeyType );
		P2PNetwork::SendToAllPlayingUser( kPacket );
	}
}

void ioSelectTripleWeaponSkill::AttackSkillFire( ioBaseChar *pChar )
{
	ioEntityGroup *pGrp = pChar->GetGroup();
	
	AttackAttribute stAttackAttribute;
	if( m_AttackKeyType == AKT_JUMP )
	{
		stAttackAttribute = m_AttackInfo_A;
	}
	else if( m_AttackKeyType == AKT_DEFENSE )
	{
		stAttackAttribute = m_AttackInfo_S;
	}
	else if( m_AttackKeyType == AKT_ATTACK )
	{
		stAttackAttribute = m_AttackInfo_D;
	}
	
	int iAniID = pGrp->GetAnimationIdx( stAttackAttribute.m_AttackAnimation );
	if( iAniID == -1 )	return;

	float fTimeRate  = stAttackAttribute.m_fAttackAniRate;
	DWORD dwPreDelay = stAttackAttribute.m_dwPreDelay;

	pGrp->ClearAllLoopAni( FLOAT100, true );
	pGrp->ClearAllActionAni( FLOAT100, true );

	if( m_bRefreshFireTimeList )
	{
		pChar->RefreshFireTimeListWithBase( m_dwWeaponBaseIndex,
											iAniID,
											stAttackAttribute.m_vWeaponInfoList,
											FTT_SKILL_ATTACK,
											fTimeRate,
											dwPreDelay );
	}

	m_dwMotionStartTime = m_dwMotionEndTime = FRAMEGETTIME();
	m_dwMotionEndTime += pGrp->GetAnimationFullTime( iAniID ) * fTimeRate;
	m_dwMotionEndTime += dwPreDelay + GetAttribute().m_dwEndDelay;

	DWORD dwCurTime = FRAMEGETTIME();

	m_dwFireStartTime = 0;
	m_dwFireStartTime = m_dwFireMotionEndTime = dwCurTime;

	m_dwFireStartTime += dwPreDelay;
	m_dwFireStartTime += pGrp->GetAnimationEventTime_e( iAniID, "fire_s" ) * fTimeRate;

	m_dwAttackEndTime = pGrp->GetAnimationEventTime_e( iAniID, "attack_end" ) * fTimeRate;
	if( m_dwAttackEndTime > 0 )
		m_dwAttackEndTime += dwCurTime;
	else
		m_dwAttackEndTime = 0;

	//
	SetExtraAniJump( pChar, iAniID, fTimeRate );

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

	m_dwEnableReserveTime = 0;
	DWORD dwReserveTime = pGrp->GetAnimationEventTime_e( iAniID, "key_reserve" ) * fTimeRate;
	if( dwReserveTime > 0 )
		m_dwEnableReserveTime = dwCurTime + dwReserveTime;

	if( m_dwFireStartTime >= m_dwFireMotionEndTime )
	{
		m_dwFireMotionEndTime = m_dwMotionEndTime;
	}

	pGrp->SetActionAni( iAniID, 0.0f, 0.0f, FLOAT1, FLOAT1/fTimeRate, 0.0f, dwPreDelay );

	pChar->SetReservedSliding( stAttackAttribute.m_vForceInfoList, iAniID, fTimeRate, dwPreDelay );

	if( GetOwnerItem() )
		pChar->ReduceNeedGaugeBySkill( this, GetOwnerItem()->GetMaxSkillGauge( GetOwnerItem()->GetOwnerName() ) );
}

bool ioSelectTripleWeaponSkill::CheckUseItemMesh()
{
	return m_bUseItemMesh;
}

void ioSelectTripleWeaponSkill::CheckKeyInput( ioBaseChar *pOwner )
{
	if( !pOwner ) return;
	if( !pOwner->IsNeedProcess() ) return;

	ioUserKeyInput::DirKeyInput eNewDirKey = pOwner->CheckCurDirKey();
	if( eNewDirKey != m_CurDirKey )
	{
		m_CurDirKey = eNewDirKey;
		if( eNewDirKey == ioUserKeyInput::DKI_RIGHT && !m_bRightKey )
		{
			m_bRightKey = true;
			m_bLeftKey  = false;
		}
		else if( eNewDirKey == ioUserKeyInput::DKI_LEFT && !m_bLeftKey )
		{
			m_bRightKey = false;
			m_bLeftKey  = true;
		}
		else if( eNewDirKey == ioUserKeyInput::DKI_NONE )
		{
			m_bRightKey = false;
			m_bLeftKey  = false;
		}
	}
}

void ioSelectTripleWeaponSkill::CheckTarget( ioBaseChar *pChar, ioPlayStage *pStage )
{
	if( !pStage )	return;
	if( !pChar )	return;

	if( !pChar->IsNeedProcess() )
		return;

	float fPrevDistSq = m_fAimRange * m_fAimRange;
	const BaseCharList &rkTargetList = pStage->GetBaseCharList();
	
	m_vTargetInfoList.clear();
	int iSize = rkTargetList.size();
	m_vTargetInfoList.reserve(iSize);
	
	BaseCharList::const_iterator iter;
	for( iter = rkTargetList.begin(); iter != rkTargetList.end(); ++iter )
	{
		ioBaseChar *pTarget = (*iter);

		if( pChar->GetCharName() == pTarget->GetCharName() )
			continue;

		if( pChar->GetTeam() == pTarget->GetTeam() )
			continue;

		if( !pTarget )
			continue;

		if( pTarget == pChar )
			continue;

		if( pTarget->IsSystemState() )
			continue;
			
		if( !pTarget->IsEnableTarget() )
			continue;

		if( !pTarget->CheckTargetWoundType( m_TargetWoundType ) )
			continue;
		
		D3DXVECTOR3 vDiff = pTarget->GetWorldPosition() - pChar->GetWorldPosition();
		float fDistSq = D3DXVec3LengthSq( &vDiff );

		tagBaseCharDistanceSq kInfo;
		kInfo.m_Name        = pTarget->GetCharName();
		kInfo.m_fDistanceSq = fDistSq;
		
		float fEnableDistSq = m_fAimRange * m_fAimRange;
		if( fDistSq < fEnableDistSq )	// Enemy
		{
			m_vTargetInfoList.push_back(kInfo);
		}
	}
	std::sort( m_vTargetInfoList.begin(), m_vTargetInfoList.end(), BaseCharDistanceSqSort2() );

	if( m_TargetName.IsEmpty() )
	{
		if( !m_vTargetInfoList.empty() )
		{
			m_TargetName = m_vTargetInfoList[0].m_Name;
		}

		if( !m_TargetName.IsEmpty() )
		{
			if( pChar->IsNeedSendNetwork() )
			{
				SP2Packet kPacket( CUPK_SKILL_EXTRA_INFO );
				kPacket << pChar->GetCharName();
				kPacket << GetName();
				kPacket << SNT_CHANGE;
				kPacket << m_TargetName;
				P2PNetwork::SendToAllPlayingUser( kPacket );
			}
		}
	}
	else if( !m_vTargetInfoList.empty() )
	{
		int iCurTarget = -1;

		int iCnt = m_vTargetInfoList.size();
		for( int i=0; i<iCnt; ++i )
		{
			if( m_TargetName == m_vTargetInfoList[i].m_Name )
				iCurTarget = i;
		}

		if( iCurTarget != -1 )
		{
			bool bTargetChange = false;
			if( m_bLeftKey )
			{
				iCurTarget++;
				m_bLeftKey = false;

				DestroyAimMark();
				if( iCurTarget >= iCnt )
					iCurTarget = 0;
			}
			else if( m_bRightKey )
			{
				iCurTarget--;
				m_bRightKey = false;

				DestroyAimMark();
				if( iCurTarget < 0 )
					iCurTarget = iCnt-1;
			}

			if( m_TargetName != m_vTargetInfoList[iCurTarget].m_Name )
			{
				m_TargetName = m_vTargetInfoList[iCurTarget].m_Name;
				bTargetChange = true;
			}

			if( bTargetChange )
			{
				if( pChar->IsNeedSendNetwork() )
				{
					SP2Packet kPacket( CUPK_SKILL_EXTRA_INFO );
					kPacket << pChar->GetCharName();
					kPacket << GetName();
					kPacket << SNT_CHANGE;
					kPacket << m_TargetName;
					P2PNetwork::SendToAllPlayingUser( kPacket );
				}
			}
		}
		else
		{
			DestroyAimMark();
			m_TargetName.Clear();
		}
	}
	else if( m_vTargetInfoList.empty() )
	{
		if( !m_TargetName.IsEmpty() )
		{
			m_TargetName.Clear();

			if( pChar->IsNeedSendNetwork() )
			{
				SP2Packet kPacket( CUPK_SKILL_EXTRA_INFO );
				kPacket << pChar->GetCharName();
				kPacket << GetName();
				kPacket << SNT_CHANGE;
				kPacket << m_TargetName;
				P2PNetwork::SendToAllPlayingUser( kPacket );
			}
		}
	}
}

void ioSelectTripleWeaponSkill::CheckDir( ioBaseChar *pChar, ioPlayStage *pStage )
{
	if( !pChar )
		return;

	if( !pStage )
		return;

	if( m_TargetName.IsEmpty() )
		return;

	ioBaseChar *pTarget = pStage->GetBaseChar( m_TargetName );
	if( pTarget )
	{
		pChar->SetTargetRotToTargetPos( pTarget->GetWorldPosition(), true, false );
		m_vAttackDir = pChar->GetTargetRot() * D3DXVECTOR3( 0.0f, 0.0f, -FLOAT1 );
	}
}

void ioSelectTripleWeaponSkill::UpdateAimMark( ioBaseChar *pChar, ioPlayStage *pStage )
{
	if( !pStage )
	{
		DestroyAimMark();
		return;
	}

	if( !pChar )
	{
		DestroyAimMark();
		return;
	}

	if( m_TargetName.IsEmpty() )
	{
		DestroyAimMark();
		return;
	}

	ioBaseChar *pTarget = pStage->GetBaseChar( m_TargetName );
	if( !pTarget )
	{
		DestroyAimMark();
		return;
	}

	if( pTarget->IsNeedProcess() )
	{
		if( m_dwAimMarkEffectID == -1 )
		{
			ioMapEffect *pEffect = pStage->CreateMapEffect( m_szAimMarkEffect, pTarget->GetMidPositionByRate(), D3DXVECTOR3( FLOAT1, FLOAT1, FLOAT1 ) );
			if( pEffect )
				m_dwAimMarkEffectID = pEffect->GetUniqueID();
		}
		else
		{
			ioMapEffect *pEffect = pStage->FindMapEffect( m_dwAimMarkEffectID );
			if( pEffect )
			{
				pEffect->SetWorldPosition( pTarget->GetMidPositionByRate() );
			}
		}
	}

	if( pChar->IsNeedProcess() )
	{
		if( m_dwAimMarkEffectID == -1 )
		{
			ioMapEffect *pEffect = pStage->CreateMapEffect( m_szAimMarkEffect, pTarget->GetMidPositionByRate(), D3DXVECTOR3( FLOAT1, FLOAT1, FLOAT1 ) );
			if( pEffect )
				m_dwAimMarkEffectID = pEffect->GetUniqueID();
		}
		else
		{
			ioMapEffect *pEffect = pStage->FindMapEffect( m_dwAimMarkEffectID );
			if( pEffect )
			{
				pEffect->SetWorldPosition( pTarget->GetMidPositionByRate() );
			}
		}
	}
}

void ioSelectTripleWeaponSkill::DestroyAimMark()
{
	ioPlayStage *pStage = g_SkillMaker.GetPlayStage();
	if( !pStage )
		return;

	if( m_dwAimMarkEffectID != -1 )
	{
		pStage->DestroyMapEffect( m_dwAimMarkEffectID );
		m_dwAimMarkEffectID = -1;
	}
}