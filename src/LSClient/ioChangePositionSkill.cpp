

#include "stdafx.h"

#include "ioChangePositionSkill.h"
#include "ioNpcChar.h"
#include "ioPlayStage.h"

ioChangePositionSkill::ioChangePositionSkill()
{
	m_bSetHandMesh = false;
	m_SkillState = SS_NONE;
	m_vAttackDir = ioMath::VEC3_ZERO;
	m_dwCheckChangePosTime = 0;
	m_vOwnerPos = ioMath::VEC3_ZERO;
	m_vTargetPos = ioMath::VEC3_ZERO;
	m_dwSkillLoopStartTime = 0;
	m_dwSkillLoopEndTime = 0;
	m_szTargetName.Clear();
	m_bWaitChangePosition = false;
}

ioChangePositionSkill::ioChangePositionSkill( const ioChangePositionSkill &rhs )
: ioAttackSkill( rhs ),
  m_vLoopMotionList( rhs.m_vLoopMotionList ),
  m_dwChangePositionTime( rhs.m_dwChangePositionTime ),
  m_szChangePositionEffect( rhs.m_szChangePositionEffect ),
  m_dwSkillLoopProtectTime( rhs.m_dwSkillLoopProtectTime ),
  m_TargetBuffList( rhs.m_TargetBuffList )
{
	m_bSetHandMesh = false;
	m_SkillState = SS_NONE;
	m_vAttackDir = ioMath::VEC3_ZERO;
	m_dwCheckChangePosTime = 0;
	m_vOwnerPos = ioMath::VEC3_ZERO;
	m_vTargetPos = ioMath::VEC3_ZERO;
	m_dwSkillLoopStartTime = 0;
	m_dwSkillLoopEndTime = 0;
	m_szTargetName.Clear();
	m_bWaitChangePosition = false;
}

ioChangePositionSkill::~ioChangePositionSkill()
{
}

void ioChangePositionSkill::LoadProperty( ioINILoader &rkLoader )
{
	ioAttackSkill::LoadProperty( rkLoader );

	m_vLoopMotionList.clear();
	m_vLoopMotionList.reserve(3);
	char szKey[MAX_PATH] = "", szBuf[MAX_PATH] = "";
	// 기립, 점프, 다운 상태의 정보를 리스트에 담는다.
	// SkillUseType을 키값으로 사용한다.
	LoadLoopMotionInfo( "", rkLoader );
	LoadLoopMotionInfo( "jump_", rkLoader );
	LoadLoopMotionInfo( "down_", rkLoader );

	m_dwChangePositionTime = (DWORD)rkLoader.LoadInt_e( "change_position_time", 0 );

	rkLoader.LoadString_e( "change_position_effect", "", szBuf, MAX_PATH );
	m_szChangePositionEffect = szBuf;

	m_dwSkillLoopProtectTime = (DWORD)rkLoader.LoadInt_e( "skill_loop_protect_time", 0 );

	int iCount = rkLoader.LoadInt_e( "target_buff_cnt", 0 );
	for( int i=0; i<iCount; ++i )
	{
		wsprintf_e( szKey, "target_buff%d", i+1 );
		rkLoader.LoadString( szKey, "", szBuf, MAX_PATH );
		m_TargetBuffList.push_back( szBuf );
	}
}

void ioChangePositionSkill::LoadLoopMotionInfo( const char *szAttack, ioINILoader &rkLoader )
{
	char szKey[MAX_PATH], szBuf[MAX_PATH];
	
	LoopMotionInfo kInfo;
	kInfo.Init();

	wsprintf_e( szKey, "%sskill_loop_motion", szAttack );
	rkLoader.LoadString( szKey, "", szBuf, MAX_PATH );
	kInfo.m_SkillLoopMotionName = szBuf;

	wsprintf_e( szKey, "%sskill_loop_motion_rate", szAttack );
	kInfo.m_fSkillLoopMotionRate = rkLoader.LoadFloat( szKey, FLOAT1 );

	wsprintf_e( szKey, "%sskill_search_range", szAttack );
	kInfo.m_fSearchRange = rkLoader.LoadFloat( szKey, 0.0f );

	wsprintf_e( szKey, "%sskill_search_angle", szAttack );
	kInfo.m_fSearchAngle = rkLoader.LoadFloat( szKey, 0.0f );

	m_vLoopMotionList.push_back( kInfo );
}

ioSkill* ioChangePositionSkill::Clone()
{
	return new ioChangePositionSkill( *this );
}

bool ioChangePositionSkill::OnSkillStart( ioBaseChar *pChar )
{
	if( !ioAttackSkill::OnSkillStart( pChar ) )
		return false;

	m_bSetHandMesh = false;

	// 기본 무기 손에서 제거
	if( m_bUseItemMesh )
	{
		pChar->ShowWeaponItemMesh( false );
	}

	m_SkillState = SS_NONE;

	SkillUseType iType = GetSkillUseType();
	m_LoopMotionInfo.Init();
	m_LoopMotionInfo = m_vLoopMotionList[iType];

	m_vAttackDir = pChar->GetTargetRot() * D3DXVECTOR3( 0.0f, 0.0f, -FLOAT1 );

	m_dwCheckChangePosTime = 0;

	m_vOwnerPos = ioMath::VEC3_ZERO;
	m_vTargetPos = ioMath::VEC3_ZERO;

	m_dwSkillLoopStartTime = 0;
	m_dwSkillLoopEndTime = 0;

	m_szTargetName.Clear();

	m_bWaitChangePosition = false;

	return true;
}
void ioChangePositionSkill::OnProcessState( ioBaseChar *pChar, ioPlayStage *pStage )
{
	switch( m_SkillState )
	{
	case SS_NONE:
		{
			DWORD dwPreDelay = GetPreDelayTime();

			if( m_dwPreDelayStartTime + dwPreDelay < FRAMEGETTIME() )
			{
				m_dwPreDelayStartTime = 0;

				AttackSkillFire( pChar );
				EndPreDelayEffect( pChar );
			}
		}
		break;
	case SS_ACTION:
		if( m_dwMotionEndTime <= FRAMEGETTIME() )
		{
			SetSkillLoopState( pChar );
		}
		break;
	case SS_LOOP:
		ProcessSkillLoopState( pChar );
		break;
	case SS_LOOP_END:
		ProcessSkillLoopEndState( pChar );
		break;
	case SS_END:
		break;
	}

	if( m_dwPreDelayStartTime == 0 )
	{
		CheckExtraAniJump( pChar );
		CheckAniRotate( pChar );
		return;
	}
}

void ioChangePositionSkill::AttackSkillFire( ioBaseChar *pChar )
{
	CheckTarget( pChar );

	ioAttackSkill::AttackSkillFire( pChar );

	SetSkillActionState( pChar );
}

void ioChangePositionSkill::SetSkillActionState( ioBaseChar *pChar )
{
	if( !m_bSetHandMesh && m_bUseItemMesh && !m_HandMesh.IsEmpty() )
	{
		pChar->ChangeSkillWeaponMesh( this, m_HandMesh, m_bVisibleEquipMesh );
		m_bSetHandMesh = true;
	}
	m_SkillState = SS_ACTION;
}

void ioChangePositionSkill::OnSkillEnd( ioBaseChar *pChar )
{
	ioAttackSkill::OnSkillEnd( pChar );
	
	RemoveTargetBuff( pChar );

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
	m_SkillState = SS_NONE;
	m_szTargetName.Clear();
}

bool ioChangePositionSkill::CheckUseItemMesh()
{
	return m_bUseItemMesh;
}

void ioChangePositionSkill::ApplyExtraSkillInfo( ioBaseChar *pChar, ioPlayStage *pStage, SP2Packet &rkPacket )
{
	if( !pChar )	return;
	if( !pStage )	return;

	D3DXVECTOR3 vTargetPos = ioMath::VEC3_ZERO;
	D3DXVECTOR3 vOwnerPos = ioMath::VEC3_ZERO;
	D3DXQUATERNION	qtRot;
	D3DXQUATERNION	qtOwnerRot;
	ioHashString	szTargetName;

	rkPacket >> qtOwnerRot;
	rkPacket >> vOwnerPos;
	rkPacket >> szTargetName;
	rkPacket >> qtRot;
	rkPacket >> vTargetPos;
	m_szTargetName = szTargetName;

	ioBaseChar *pTarget = pChar->GetBaseChar( m_szTargetName );
	if( !pTarget )	return;

	pChar->SetWorldPosition( vTargetPos );
	pTarget->SetWorldPosition( vOwnerPos );

	pChar->SetTargetRotAndMoveDirChange( qtRot );
	pTarget->SetTargetRotAndMoveDirChange( qtOwnerRot );

	SetSkillEndState();
}

void ioChangePositionSkill::ApplyAttributeToWeapon( ioWeapon *pWeapon, ioPlayStage *pStage )
{
	ioSkill::ApplyAttributeToWeapon( pWeapon, pStage );

	SetChargingRateForWeapon( pWeapon );

	pWeapon->SetMoveDir( m_vAttackDir );
}

bool ioChangePositionSkill::IsSkillMotionEnd( DWORD dwActionStop, ioBaseChar* pOwner ) const
{
	DWORD dwCurTime = FRAMEGETTIME();

	if( m_dwPreDelayStartTime > 0 )
		return false;

	switch( m_SkillState )
	{
	case SS_NONE:
	case SS_ACTION:
	case SS_LOOP:
	case SS_LOOP_END:
		return false;
	case SS_END:
		if( m_dwCheckChangePosTime+g_App.GetGrapplingCheckDelay() <= FRAMEGETTIME() )
		{
			return true;
		}
	}

	return false;
}

bool ioChangePositionSkill::IsProtected( int iDefenseBreakType ) const
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

	if( m_dwSkillLoopProtectTime > 0 && m_dwSkillLoopStartTime+m_dwSkillLoopProtectTime > FRAMEGETTIME() )
		return true;

	return false;
}

bool ioChangePositionSkill::IsNoDropState() const
{
	if( m_SkillState == SS_LOOP ||
		m_SkillState == SS_LOOP_END ||
		m_SkillState == SS_ACTION )
		return true;

	return false;
}

bool ioChangePositionSkill::IsEndState() const
{
	if( m_SkillState == SS_END )
		return true;

	return false;
}

void ioChangePositionSkill::SetSkillLoopState( ioBaseChar *pChar )
{
	if( !pChar )	return;
	
	m_SkillState = SS_LOOP;

	ioHashString szLoopMotion = m_LoopMotionInfo.m_SkillLoopMotionName;
	float fTimeRate = m_LoopMotionInfo.m_fSkillLoopMotionRate;

	ioEntityGroup *pGrp = pChar->GetGroup();
	int iAni = pGrp->GetAnimationIdx( szLoopMotion );
	if( iAni == -1 )	return;

	pGrp->ClearAllActionAni( FLOAT100, true );
	pGrp->ClearAllLoopAni( FLOAT100, true );

	pGrp->SetLoopAni( iAni, FLOAT100, FLOAT1, FLOAT1/fTimeRate );

	m_dwSkillLoopStartTime = FRAMEGETTIME();
}

void ioChangePositionSkill::SetSkillLoopEndState( ioBaseChar *pChar )
{
	if( !pChar )	return;
	
	m_SkillState = SS_LOOP_END;

	m_dwSkillLoopEndTime = FRAMEGETTIME() + 550;
}

void ioChangePositionSkill::SetSkillEndState()
{
	m_SkillState = SS_END;
}

void ioChangePositionSkill::SetChangePosition( ioBaseChar *pOwner, ioBaseChar *pTarget )
{
	if( !pOwner )
	{
		SetSkillEndState();
		return;
	}
	if( !pTarget )
	{
		SetSkillEndState();
		return;
	}

	if( m_vOwnerPos == ioMath::VEC3_ZERO ||
		m_vTargetPos == ioMath::VEC3_ZERO )
	{
		SetSkillEndState();
		return;
	}

	if( !pOwner->IsNeedProcess() )	return;
	
	D3DXVECTOR3 vOwnerPos = m_vOwnerPos;
	D3DXVECTOR3 vTargetPos = m_vTargetPos;

	D3DXVECTOR3 vOwnerDir = vTargetPos - vOwnerPos;
	vOwnerDir.y = 0.0f;
	D3DXVec3Normalize( &vOwnerDir, &vOwnerDir );

	D3DXQUATERNION qtOwnerRot = pOwner->GetWorldOrientation();
	D3DXQUATERNION qtRot;
	ioMath::CalcDirectionQuaternion( qtRot, vOwnerDir );

	pOwner->SetWorldPosition( vTargetPos );
	pTarget->SetWorldPosition( vOwnerPos );
	
	pOwner->SetTargetRotAndMoveDirChange( qtRot );
	pTarget->SetTargetRotAndMoveDirChange( qtOwnerRot );

	SetSkillEndState();

	if( pOwner->IsNeedSendNetwork() )
	{
		SP2Packet kPacket( CUPK_SKILL_EXTRA_INFO );
		kPacket << pOwner->GetCharName();
		kPacket << GetName();
		kPacket << qtOwnerRot;
		kPacket << vOwnerPos;
		kPacket << m_szTargetName;
		kPacket << qtRot;
		kPacket << vTargetPos;
		P2PNetwork::SendToAllPlayingUser( kPacket );
	}
}

void ioChangePositionSkill::ProcessSkillLoopState( ioBaseChar *pChar )
{
	if( !pChar )
	{
		SetSkillEndState();
		return;
	}

	if( m_dwCheckChangePosTime > 0 && m_dwCheckChangePosTime <= FRAMEGETTIME() )
	{
		ioBaseChar *pTarget = pChar->GetBaseChar( m_szTargetName );
		if( !pTarget )
		{
			SetSkillEndState();
			return;
		}

		RemoveTargetBuff( pChar );
		SetChangePosition( pChar, pTarget );
	}
}

void ioChangePositionSkill::ProcessSkillLoopEndState( ioBaseChar *pChar )
{
	if( !pChar )
	{
		SetSkillEndState();
		return;
	}

	DWORD dwCurTime = FRAMEGETTIME();

	if( m_dwSkillLoopEndTime > 0 && m_dwSkillLoopEndTime < dwCurTime )
	{
		SetSkillEndState();
		return;
	}

	ProcessSkillLoopState( pChar );
}

ioBaseChar *ioChangePositionSkill::CheckTarget( ioBaseChar *pOwner )
{
	if( !pOwner )	return NULL;

	ioPlayStage *pStage = pOwner->GetCreator();
	if( !pStage )	return NULL;

	float fPrevDistSq = m_LoopMotionInfo.m_fSearchRange * m_LoopMotionInfo.m_fSearchRange;
	
	ioBaseChar *pTargetChar = pOwner;
	const BaseCharList &rkTargetList = pStage->GetBaseCharList();
	BaseCharList::const_iterator iter;
	for( iter = rkTargetList.begin(); iter != rkTargetList.end(); ++iter )
	{
		ioBaseChar *pTarget = (*iter);
		if( !pTarget )
			continue;

		if( pTarget == pOwner )
			continue;

		if( !pStage->IsCollisionCheckByMode() )
			continue;

		if( pOwner->GetCharName() == pTarget->GetCharName() )
			continue;

		if( pOwner->GetTeam() == pTarget->GetTeam() )
			continue;

		if( pTarget->IsSystemState() )
			continue;
		
		if( !pTarget->IsEnableTarget() ) 
			continue;

		if( pTarget->HasHideBuff() )
			continue;

		if( pTarget->GetBlowWoundedState() != BW_BOUND_BLOWING &&
			pTarget->IsApplyDownState(false) )
		{
			continue;
		}

		D3DXVECTOR3 vDiff = pTarget->GetWorldPosition() - pOwner->GetWorldPosition();
		float fDistSq = D3DXVec3LengthSq( &vDiff );
		if( fDistSq < fPrevDistSq )	// Enemy
		{
			D3DXVECTOR3 vPos = pOwner->GetMidPositionByRate();
			D3DXVECTOR3 vTargetPos;

			vTargetPos = pTarget->GetMidPositionByRate();

			D3DXVECTOR3 vDir = vTargetPos - vPos;
			D3DXVec3Normalize( &vDir, &vDir );

			D3DXVECTOR3 vSearchDir = pOwner->GetTargetRot() * D3DXVECTOR3( 0.0f, 0.0f, -FLOAT1 );
			D3DXVec3Normalize( &vSearchDir, &vSearchDir );

			float fHalfCosine = cosf( DEGtoRAD( m_LoopMotionInfo.m_fSearchAngle ) * FLOAT05 );

			if( D3DXVec3Dot( &vSearchDir, &vDir ) >= fHalfCosine )
			{
				pTargetChar = pTarget;
				fPrevDistSq = fDistSq;
				m_vAttackDir = vDir;
			}
		}
	}

	if( pTargetChar == pOwner )
	{
		return NULL;
	}

	D3DXVECTOR3 vOwnerPos = pOwner->GetWorldPosition();
	D3DXVECTOR3 vTargetPos = pTargetChar->GetWorldPosition();

	D3DXVECTOR3 vOwnerDir = vTargetPos - vOwnerPos;
	vOwnerDir.y = 0.0f;
	D3DXVec3Normalize( &vOwnerDir, &vOwnerDir );

	D3DXQUATERNION qtOwnerRot = pOwner->GetWorldOrientation();
	D3DXQUATERNION qtRot;
	ioMath::CalcDirectionQuaternion( qtRot, -vOwnerDir );

	pOwner->SetTargetRotAndMoveDirChange( qtRot );

	return pTargetChar;
}

void ioChangePositionSkill::AddTargetBuff( ioBaseChar *pChar )
{
	if( !pChar )	return;
	if( m_szTargetName.IsEmpty() )	return;

	ioBaseChar *pTarget = pChar->GetBaseChar( m_szTargetName );

	if( pTarget )
	{
		int iCnt = m_TargetBuffList.size();
		for( int i=0; i<iCnt; ++i )
		{
			pTarget->AddNewBuff( m_TargetBuffList[i], "", "", this );
		}
	}
}

void ioChangePositionSkill::RemoveTargetBuff( ioBaseChar *pChar )
{
	if( !pChar )	return;
	if( m_szTargetName.IsEmpty() )	return;

	ioBaseChar *pTarget = pChar->GetBaseChar( m_szTargetName );

	if( pTarget )
	{
		int iCnt = m_TargetBuffList.size();
		for( int i=0; i<iCnt; ++i )
		{
			pTarget->RemoveBuff( m_TargetBuffList[i] );
		}
	}
}

void ioChangePositionSkill::SetCreateWeaponDependency( ioBaseChar *pOwner, ioPlayStage *pStage )
{
	if( !pOwner ) return;
	if( !pStage ) return;
	if( m_bWaitChangePosition )	return;

	SetSkillLoopEndState( pOwner );
	//SetSkillEndState();
}

void ioChangePositionSkill::SetInfoByApplyWeapon( ioBaseChar *pOwner, ioBaseChar *pTarget, DWORD dwWeaponBaseIndex, bool bBlock )
{
	if( bBlock ) return;
	if( !pOwner )	return;
	if( !pTarget )	return;
	if( m_bWaitChangePosition )	return;
	if( m_dwWeaponBaseIndex != dwWeaponBaseIndex )	return;

	ioPlayStage *pStage = pOwner->GetCreator();
	if( !pStage )return;

	ioNpcChar *pNpc = ToNpcChar( pTarget );
	if( pNpc && Help::IsMonsterDungeonMode(pStage->GetModeType()) )
		if( COMPARE( pNpc->GetNpcType(), NT_BIGTOWER, END_FIXING_NPC) )	
			return;
	
	m_szTargetName = pTarget->GetCharName();
	m_bWaitChangePosition = true;

	AddTargetBuff( pOwner );

	m_vOwnerPos = pOwner->GetWorldPosition();
	m_vTargetPos = pTarget->GetWorldPosition();

	D3DXVECTOR3 vOwnerDir = m_vTargetPos - m_vOwnerPos;
	D3DXVec3Normalize( &vOwnerDir, &vOwnerDir );

	D3DXVECTOR3 vTargetDir = m_vOwnerPos - m_vTargetPos;
	D3DXVec3Normalize( &vTargetDir, &vTargetDir );
	
	ioMapEffect *pOwnerEff = pStage->CreateMapEffect( m_szChangePositionEffect, m_vOwnerPos, pOwner->GetWorldScale() );
	if( pOwnerEff )
	{
		D3DXQUATERNION qtRot;
		ioMath::CalcDirectionQuaternion( qtRot, -vOwnerDir );
		pOwnerEff->SetWorldOrientation( qtRot );
	}

	ioMapEffect *pTargetEff = pStage->CreateMapEffect( m_szChangePositionEffect, m_vTargetPos, pTarget->GetWorldScale() );
	if( pTargetEff )
	{
		D3DXQUATERNION qtRot;
		ioMath::CalcDirectionQuaternion( qtRot, -vTargetDir );
		pTargetEff->SetWorldOrientation( qtRot );
	}
	
	m_dwCheckChangePosTime = m_dwChangePositionTime + FRAMEGETTIME();
}