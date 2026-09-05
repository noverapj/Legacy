#include "StdAfx.h"
#include "ioTargetTeleportAttackSkill.h"

#include "ioPlayStage.h"
#include "ioBaseChar.h"
#include "ioZoneEffectWeapon.h"

ioTargetTeleportAttackSkill::ioTargetTeleportAttackSkill(void)
{
	ClearData();
}

ioTargetTeleportAttackSkill::ioTargetTeleportAttackSkill( const ioTargetTeleportAttackSkill &rhs ):
ioAttackSkill( rhs ),
m_bEnableDownAim( rhs.m_bEnableDownAim ),
m_fAimRange( rhs.m_fAimRange ),
m_szAimedMarker( rhs.m_szAimedMarker ),
m_szAimAni( rhs.m_szAimAni ),
m_dwLoopTime( rhs.m_dwLoopTime ),
m_dwAttackDelayTime( rhs.m_dwAttackDelayTime ),
m_vNoTargetTeleportOffset( rhs.m_vNoTargetTeleportOffset ),
m_vTargetTeleportOffset( rhs.m_vTargetTeleportOffset ),
m_LandAttackAttribute( rhs.m_LandAttackAttribute ),
m_vOwnerEndBuffList( rhs.m_vOwnerEndBuffList )
{
	ClearData();
}

ioTargetTeleportAttackSkill::~ioTargetTeleportAttackSkill(void)
{
}

ioSkill* ioTargetTeleportAttackSkill::Clone()
{
	return new ioTargetTeleportAttackSkill( *this );
}

void ioTargetTeleportAttackSkill::LoadProperty( ioINILoader &rkLoader )
{
	ioAttackSkill::LoadProperty( rkLoader );

	char szBuf[MAX_PATH], szKey[MAX_PATH];
	rkLoader.LoadString_e( "aim_ani", "", szBuf, MAX_PATH );
	m_szAimAni = szBuf;

	m_bEnableDownAim = rkLoader.LoadBool_e( "enable_down_aim", false );

	m_fAimRange = rkLoader.LoadFloat_e( "aim_range", 0.0f );

	rkLoader.LoadString_e( "aimed_marker", "", szBuf, MAX_PATH );
	m_szAimedMarker = szBuf;

	m_dwLoopTime = rkLoader.LoadInt_e( "loop_time", 0 );

	m_dwAttackDelayTime = rkLoader.LoadInt_e( "attack_delay_time", 0 );

	m_vNoTargetTeleportOffset.x = rkLoader.LoadFloat_e( "no_target_teleport_offset_x", 0.0f );
	m_vNoTargetTeleportOffset.y = rkLoader.LoadFloat_e( "no_target_teleport_offset_y", 0.0f );
	m_vNoTargetTeleportOffset.z = rkLoader.LoadFloat_e( "no_target_teleport_offset_z", 0.0f );

	m_vTargetTeleportOffset.x = rkLoader.LoadFloat_e( "target_teleport_offset_x", 0.0f );
	m_vTargetTeleportOffset.y = rkLoader.LoadFloat_e( "target_teleport_offset_y", 0.0f );
	m_vTargetTeleportOffset.z = rkLoader.LoadFloat_e( "target_teleport_offset_z", 0.0f );

	//end
	m_LandAttackAttribute.Init();
	LoadAttackAttribute_e( "skill_land_attack", m_LandAttackAttribute, rkLoader );

	int iCnt = rkLoader.LoadInt_e( "owner_end_buff_cnt", 0 );
	m_vOwnerEndBuffList.clear();
	if( iCnt > 0 )
	{
		m_vOwnerEndBuffList.reserve( iCnt );
		for( int i=0; i < iCnt; ++i )
		{
			wsprintf_e( szKey, "owner_end_buff%d", i+1 );
			rkLoader.LoadString( szKey, "", szBuf, MAX_PATH );
			ioHashString szBuffName = szBuf;

			m_vOwnerEndBuffList.push_back( szBuf );
		}
	}
}

void ioTargetTeleportAttackSkill::ClearData()
{
	m_SkillState = SS_NONE;
	m_dwMakerID = -1;
	m_szAimedTarget.Clear();
	m_bChangeTargetPrev = false;
	m_bChangeTargetNext = false;
	m_vAimDir = ioMath::VEC3_ZERO;
	m_bInputAttackKey = false;
	m_dwAttackKeyInputTime = 0;
	SetMotionTime( 0, 0 );
	m_bUseEndBuff = false;
}

bool ioTargetTeleportAttackSkill::OnSkillStart( ioBaseChar *pChar )
{
	if ( !ioAttackSkill::OnSkillStart( pChar ) )
		return false;

	SetCameraBuff( pChar );

	// 기본 무기 손에서 제거
	if( m_bUseItemMesh )
		pChar->ShowWeaponItemMesh( false );

	// 사용 아이템 처리
	if( !m_bSetHandMesh && m_bUseItemMesh && !m_HandMesh.IsEmpty() )
	{
		pChar->ChangeSkillWeaponMesh( this, m_HandMesh, m_bVisibleEquipMesh );
		m_bSetHandMesh = true;
	}	

	if( !m_bReduceGauge )
	{
		pChar->ReduceNeedGaugeBySkill( this, GetNeedGauge(pChar->GetCharName()) );
		m_bReduceGauge = true;
	}

	for (int i=0; i < (int)m_vOwnerEndBuffList.size(); ++i)
		pChar->RemoveBuff( m_vOwnerEndBuffList[i], false);

	ClearData();
	return true;
}

void ioTargetTeleportAttackSkill::OnProcessState( ioBaseChar *pChar, ioPlayStage *pStage )
{
	DWORD dwCurTime = FRAMEGETTIME();

	switch( m_SkillState )
	{
	case SS_NONE:
		{
			DWORD dwPreDelay = GetPreDelayTime();
			if( m_dwPreDelayStartTime + dwPreDelay < dwCurTime )
			{
				m_dwPreDelayStartTime = 0;
				EndPreDelayEffect( pChar );

				SetAimState( pChar );
			}
		}
		break;
	case SS_AIM:
		{
			if ( m_dwLoopTime > 0 && m_dwMotionStartTime + m_dwLoopTime < dwCurTime )
			{
				SetEndState( pChar );
				return;
			}
			GetTargetList( pChar, pStage );
			SetTargetRotation( pChar );
			UpdateMaker( pChar, pStage );

			if ( CheckInputChangeTarget( pChar ) )
			{
				DestroyEffect( pChar, pStage, m_dwMakerID );
				return;
			}

			if ( CheckInputAttack( pChar ) )
			{
				SetTeleport( pChar, pStage );

				SetAttackState( pChar );
				return;
			}
		}
		break;
	case SS_TELEPORT_ATTACK:
		{
			if( CheckLanding( pChar ) )
			{
				SetLandAttackState( pChar );
				return;
			}
		}
		break;
	case SS_LAND_ATTACK:
		{
			if ( m_dwMotionEndTime < dwCurTime )
			{
				SetEndState( pChar );
				return;
			}
		}
		break;
	}
}

void ioTargetTeleportAttackSkill::SetAimState( ioBaseChar *pChar )
{
	if ( !pChar )
		return;

	ioEntityGroup *pGrp = pChar->GetGroup();
	if( !pGrp )
		return;

	pGrp->ClearAllLoopAni( FLOAT100, true );
	pGrp->ClearAllActionAni( FLOAT100, true );

	int iAniID = pGrp->GetAnimationIdx( m_szAimAni );
	if( iAniID == -1 )
		return;

	pGrp->SetLoopAni( iAniID, FLOAT100, FLOAT1 );

	m_SkillState = SS_AIM;

	m_dwMotionStartTime = FRAMEGETTIME();
}

void ioTargetTeleportAttackSkill::GetTargetList( ioBaseChar *pChar, ioPlayStage *pStage )
{
	if ( !pChar || !pChar->IsNeedProcess() || !pStage )
		return;

	m_vTargetInfoList.clear();

	const BaseCharList &rkTargetList = pStage->GetBaseCharList();

	BaseCharList::const_iterator iter;
	for( iter = rkTargetList.begin(); iter != rkTargetList.end(); ++iter )
	{
		ioBaseChar *pTarget = (*iter);

		if( !pTarget ) 
			continue;

		if( pChar->GetCharName() == pTarget->GetCharName() )
			continue;

		if( pChar->GetTeam() == pTarget->GetTeam() )	
			continue;

		if( pTarget->HasHideBuff() )
			continue;

		if( !m_bEnableDownAim && pTarget->IsApplyDownState() )
			continue;

		if( pTarget->IsSystemState() )
			continue;
			
		if( !pTarget->IsEnableTarget() )		
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

	std::sort( m_vTargetInfoList.begin(), m_vTargetInfoList.end(), BaseCharDistanceSqSort() );	

	bool bSendPacket = false;
	if ( m_szAimedTarget.IsEmpty() )
	{
		if( !m_vTargetInfoList.empty() )
		{
			m_szAimedTarget = m_vTargetInfoList[0].m_Name;
			bSendPacket = true;
		}		
	}
	else if ( !m_vTargetInfoList.empty() )
	{
		int nCurTarget = -1;

		int nCnt = m_vTargetInfoList.size();
		for( int i=0; i<nCnt; ++i )
		{
			if( m_szAimedTarget == m_vTargetInfoList[i].m_Name )
				nCurTarget = i;
		}

		if ( nCurTarget != -1 )
		{
			if( m_bChangeTargetPrev )
			{
				nCurTarget--;
				m_bChangeTargetPrev = false;

				if( nCurTarget < 0 )
					nCurTarget = nCnt-1;
			}
			else if( m_bChangeTargetNext )
			{
				nCurTarget++;
				m_bChangeTargetNext = false;

				if( nCurTarget >= nCnt )
					nCurTarget = 0;
			}

			if( m_szAimedTarget != m_vTargetInfoList[nCurTarget].m_Name )
			{
				m_szAimedTarget = m_vTargetInfoList[nCurTarget].m_Name;
				bSendPacket = true;
			}
		}
		else if ( !m_szAimedTarget.IsEmpty() )
		{
			m_szAimedTarget.Clear();
			bSendPacket = true;
			m_bChangeTargetPrev = false;
			m_bChangeTargetNext = false;
		}

	}
	else
	{
		if ( !m_szAimedTarget.IsEmpty() )
		{
			m_szAimedTarget.Clear();
			bSendPacket = true;
			m_bChangeTargetPrev = false;
			m_bChangeTargetNext = false;
		}
	}
	if ( bSendPacket && pChar->IsNeedSendNetwork() )
	{
		SP2Packet kPacket( CUPK_SKILL_EXTRA_INFO );
		kPacket << pChar->GetCharName();
		kPacket << GetName();
		kPacket << static_cast<BYTE>(SST_TARGET);
		kPacket << m_szAimedTarget;
		P2PNetwork::SendToAllPlayingUser( kPacket );
	}
}

void ioTargetTeleportAttackSkill::SetTargetRotation( ioBaseChar *pChar )
{
	if ( !pChar )
		return;

	ioEntityGroup *pGrp = pChar->GetGroup();
	if ( !pGrp )
		return;

	ioBaseChar *pTarget = pChar->GetBaseChar( m_szAimedTarget );
	D3DXVECTOR3 vAimedDir;
	if( pTarget )
	{
		vAimedDir = pTarget->GetMidPositionByRate() - pChar->GetMidPositionByRate();
		pChar->SetTargetRotToDir( vAimedDir, true );		
	}
	else
		vAimedDir = pChar->GetTargetRot() * D3DXVECTOR3( 0.0f, 0.0f, -FLOAT1 );

	D3DXVec3Normalize( &vAimedDir, &vAimedDir );
	m_vAimDir = vAimedDir;
}

void ioTargetTeleportAttackSkill::UpdateMaker( ioBaseChar *pChar, ioPlayStage *pStage )
{
	if ( !pChar || !pStage )
		return;

	D3DXVECTOR3 vPos = pChar->GetMidPositionByRate() + m_vAimDir * m_fAimRange;

	if ( !m_szAimedTarget.IsEmpty() )
	{
		ioBaseChar *pTarget = pStage->GetBaseChar( m_szAimedTarget );
		if ( pTarget )
			vPos = pTarget->GetMidPositionByRate();
	}

	if ( m_dwMakerID == -1 )
		m_dwMakerID = CreateEffect( pChar, pStage, m_szAimedMarker, vPos, D3DXVECTOR3(1.0f,1.0f,1.0f) );

	ioMapEffect *pEffect = pStage->FindMapEffect( m_dwMakerID );
	if ( pEffect )		
		pEffect->SetWorldPosition( vPos );
}

bool ioTargetTeleportAttackSkill::CheckInputChangeTarget( ioBaseChar *pChar )
{
	if ( !pChar || !pChar->IsNeedProcess() )
		return false;

	if ( m_bInputAttackKey )
		return false;

	if ( pChar->IsDefenseKey() )
	{
		m_bChangeTargetNext = true;
		return true;
	}

	if ( pChar->IsJumpKey() )
	{
		m_bChangeTargetPrev = true;
		return true;
	}

	return false;
}

bool ioTargetTeleportAttackSkill::CheckInputAttack( ioBaseChar *pChar )
{
	if ( !pChar || !pChar->IsNeedProcess() )
		return false;

	if ( !m_bInputAttackKey )
	{
		if ( pChar->IsAttackKey() )
		{
			m_bInputAttackKey = true;
			m_dwAttackKeyInputTime = FRAMEGETTIME();
		}
	}
	else
	{
		if ( m_dwAttackKeyInputTime + m_dwAttackDelayTime < FRAMEGETTIME() )
			return true;
	}

	return false;
}

void ioTargetTeleportAttackSkill::SetTeleport( ioBaseChar *pChar, ioPlayStage *pStage )
{
	if ( !pChar || !pStage )
		return;

	DestroyEffect( pChar, pStage, m_dwMakerID );

	D3DXVECTOR3 vNewPos = pChar->GetWorldPosition() + pChar->GetWorldOrientation() * m_vNoTargetTeleportOffset;
	ioBaseChar *pTarget = pChar->GetBaseChar( m_szAimedTarget );
	if( pTarget )
		vNewPos = pTarget->GetWorldPosition() + pChar->GetWorldOrientation() * m_vTargetTeleportOffset;

	float fMapHeight = pStage->GetMapHeightOnlyTerrain( vNewPos.x, vNewPos.z );
	if ( vNewPos.y <= fMapHeight )
		vNewPos.y = fMapHeight;

	pChar->SetWorldPosition( vNewPos );

	SetTargetRotation( pChar );
}

void ioTargetTeleportAttackSkill::SetAttackState( ioBaseChar *pChar )
{
	if ( !pChar )
		return;
	ioEntityGroup *pGrp = pChar->GetGroup();
	if ( !pGrp )
		return;

	pChar->ClearReservedSliding();
	pChar->SetCurJumpPowerAmt( 0.0f );
	pChar->SetForcePowerAmt( 0.0f );
	pChar->SetGravityAmt( 0.0f );
	pChar->SetCurMoveSpeed( 0.0f );
	
	AttackAttribute AttackAttr = GetAttribute();
	int iAniID = pGrp->GetAnimationIdx( AttackAttr.m_AttackAnimation );
	if( iAniID == -1 )	
		return;

	float fTimeRate  = AttackAttr.m_fAttackAniRate;
	DWORD dwPreDelay = AttackAttr.m_dwPreDelay;

	pGrp->ClearAllLoopAni( FLOAT100, true );
	pGrp->ClearAllActionAni( FLOAT100, true );

	pGrp->SetActionAni( iAniID, FLOAT100, FLOAT100, FLOAT1, FLOAT1/fTimeRate, 0.0f, dwPreDelay );

	pChar->SetReservedSliding( AttackAttr.m_vForceInfoList, iAniID, fTimeRate, dwPreDelay );
	pChar->CheckCharColSkipSkillTime( iAniID, fTimeRate, dwPreDelay );

	pChar->RefreshFireTimeList( iAniID, AttackAttr.m_vWeaponInfoList, FTT_SKILL_ATTACK, fTimeRate, dwPreDelay );

	DWORD dwCurTime = FRAMEGETTIME();
	m_dwMotionEndTime = m_dwFireMotionEndTime = dwCurTime;
	m_dwMotionEndTime += dwPreDelay;
	m_dwMotionEndTime += pGrp->GetAnimationFullTime( iAniID ) * fTimeRate;

	m_dwFireMotionEndTime += dwPreDelay;
	m_dwFireMotionEndTime += pGrp->GetAnimationEventTime_e( iAniID, "firemotion_e" ) * fTimeRate;

	m_SkillState = SS_TELEPORT_ATTACK;
	m_bUseEndBuff = true;

	if ( pChar->IsNeedSendNetwork() )
	{
		SP2Packet kPacket( CUPK_SKILL_EXTRA_INFO );
		kPacket << pChar->GetCharName();
		kPacket << GetName();
		kPacket << static_cast<BYTE>( SST_TELEPORT_ATTACK );
		kPacket << pChar->GetWorldOrientation();
		kPacket << pChar->GetWorldPosition();
		P2PNetwork::SendToAllPlayingUser( kPacket );
	}
}

bool ioTargetTeleportAttackSkill::CheckLanding( ioBaseChar *pChar )
{
	if( !pChar ) return true;

	DWORD dwCurTime = FRAMEGETTIME();

	float fHeightGap = 0.0f;
	if( pChar->CheckGhostState() )
		fHeightGap = pChar->GetWorldPosition().y - pChar->GetPreHeightForGhostState();
	else
		fHeightGap = pChar->GetWorldPosition().y - pChar->GetBottomHeight();

	if( fHeightGap > 0.0f || !pChar->IsDownState() )
		return false;

	return true;
}

void ioTargetTeleportAttackSkill::SetNextSkillState( ioBaseChar *pWounder, ioWeapon *pWeapon /*= NULL */ )
{
	if ( !pWounder || !pWeapon )
		return;
	ioBaseChar *pAttacker = pWeapon->GetOwner();
	if ( !pAttacker )
		return;
	if( pWounder->IsNeedProcess() )
	{
		ioItem *pItem = GetOwnerItem();
		if ( !pItem || pItem->GetOwnerName().IsEmpty())
			return;
		SetLandAttackState( pAttacker );
	}
}

void ioTargetTeleportAttackSkill::SetLandAttackState( ioBaseChar *pChar )
{
	if ( !pChar )
		return;
	ioEntityGroup *pGrp = pChar->GetGroup();
	if ( !pGrp )
		return;

	pChar->ClearReservedSliding();
	pChar->SetCurJumpPowerAmt( 0.0f );
	pChar->SetForcePowerAmt( 0.0f );
	pChar->SetGravityAmt( 0.0f );
	pChar->SetCurMoveSpeed( 0.0f );
	SetMotionTime( 0,0 );
	int iAniID = pGrp->GetAnimationIdx( m_LandAttackAttribute.m_AttackAnimation );
	if( iAniID == -1 )	
		return;

	float fTimeRate  = m_LandAttackAttribute.m_fAttackAniRate;
	DWORD dwPreDelay = m_LandAttackAttribute.m_dwPreDelay;
	DWORD dwEndDelay = m_LandAttackAttribute.m_dwEndDelay;
	pGrp->ClearAllLoopAni( FLOAT100, true );
	pGrp->ClearAllActionAni( FLOAT100, true );

	pGrp->SetActionAni( iAniID, FLOAT100, FLOAT100, FLOAT1, FLOAT1/fTimeRate, 0.0f, dwPreDelay );

	pChar->SetReservedSliding( m_LandAttackAttribute.m_vForceInfoList, iAniID, fTimeRate, dwPreDelay );
	pChar->CheckCharColSkipSkillTime( iAniID, fTimeRate, dwPreDelay );

	pChar->RefreshFireTimeList( iAniID, m_LandAttackAttribute.m_vWeaponInfoList, FTT_SKILL_ATTACK, fTimeRate, dwPreDelay );

	DWORD dwCurTime = FRAMEGETTIME();
	m_dwMotionEndTime = m_dwFireMotionEndTime = dwCurTime;
	m_dwMotionEndTime += pGrp->GetAnimationFullTime( iAniID ) * fTimeRate;
	m_dwMotionEndTime += dwPreDelay + dwEndDelay;

	m_dwFireMotionEndTime += dwPreDelay;
	m_dwFireMotionEndTime += pGrp->GetAnimationEventTime_e( iAniID, "firemotion_e" ) * fTimeRate;

	m_SkillState = SS_LAND_ATTACK;
}

bool ioTargetTeleportAttackSkill::IsNoDropState() const
{
	if ( m_SkillState == SS_LAND_ATTACK )
		return true;

	return false;
}

void ioTargetTeleportAttackSkill::SetEndState( ioBaseChar *pChar )
{
	if ( !pChar )
		return;

	m_SkillState = SS_END;
}

DWORD ioTargetTeleportAttackSkill::CreateEffect( ioBaseChar *pChar, ioPlayStage *pStage, ioHashString& szEffect, D3DXVECTOR3& vPos, D3DXVECTOR3& vScale )
{
	if ( !pChar || !pStage || szEffect.IsEmpty() )
		return -1;

	ioMapEffect *pMaker = pStage->CreateMapEffect( szEffect, vPos, vScale );
	if ( pMaker )
		return pMaker->GetUniqueID();

	return -1;
}

void ioTargetTeleportAttackSkill::DestroyEffect( ioBaseChar *pChar, ioPlayStage *pStage, DWORD& dwEffectID )
{
	if ( !pChar || !pStage || dwEffectID == -1 )
		return;

	pStage->DestroyMapEffect( dwEffectID );
	dwEffectID = -1;
}

void ioTargetTeleportAttackSkill::OnSkillEnd( ioBaseChar *pChar )
{
	if ( !pChar )
		return;

	ioAttackSkill::OnSkillEnd( pChar );

	ioPlayStage *pStage = pChar->GetCreator();
	if ( pStage )
		DestroyEffect( pChar, pStage, m_dwMakerID );

	EndCameraBuff( pChar );

	// 기본무기 장착
	if( m_bUseItemMesh )
		pChar->ShowWeaponItemMesh( true );

	if( !m_HandMesh.IsEmpty() )
		pChar->RestoreSkillWeaponMesh( this, m_HandMesh, m_bVisibleEquipMesh );

	m_bSetHandMesh = false;

	ioItem *pItem = GetOwnerItem();
	if( !pItem )
		return;

	if ( m_bUseEndBuff )
	{
		int nBuffCnt = (int)m_vOwnerEndBuffList.size();
		for ( int i=0; i<nBuffCnt; ++i )
			pChar->AddNewBuff( m_vOwnerEndBuffList[i], pChar->GetCharName(), pItem->GetName(), this );			
	}
	
}

bool ioTargetTeleportAttackSkill::IsSkillMotionEnd( DWORD dwActionStop, ioBaseChar* pOwner ) const
{
	if ( m_SkillState == SS_END && m_dwMotionEndTime < FRAMEGETTIME() && m_fSkillEndNextJumpAmt == 0.0f )
		return true;

	return false;
}

bool ioTargetTeleportAttackSkill::IsAttackEndState() const
{
	if( m_SkillState == SS_END && m_dwMotionEndTime < FRAMEGETTIME() )
		return true;

	return false;
}

bool ioTargetTeleportAttackSkill::IsEndState() const
{
	return IsAttackEndState();
}

bool ioTargetTeleportAttackSkill::CheckExceptionCamera()
{
	if( m_SkillState == SS_AIM )
		return true;

	return false;
}

D3DXVECTOR3 ioTargetTeleportAttackSkill::GetExceptionCameraPos( ioBaseChar *pChar )
{
	if( !pChar )
		return ioMath::VEC3_ZERO;

	ioBaseChar *pTarget = pChar->GetBaseChar( m_szAimedTarget );
	if( pTarget )
		return pTarget->GetWorldPosition();

	D3DXVECTOR3 vPos = pChar->GetMidPositionByRate() + m_vAimDir * m_fAimRange;
	return vPos;
}

void ioTargetTeleportAttackSkill::ApplyExtraSkillInfo( ioBaseChar *pChar, ioPlayStage *pStage, SP2Packet &rkPacket )
{
	if( !pChar || !pStage ) 
		return;

	BYTE btState;
	rkPacket >> btState;

	switch( btState )
	{
	case SST_TARGET:
		rkPacket >> m_szAimedTarget;
		break;
	case SST_TELEPORT_ATTACK:
		{
			D3DXQUATERNION qtRot;
			rkPacket >> qtRot;

			pChar->SetTargetRotAndMoveDirChange( qtRot );

			D3DXVECTOR3 vPos;
			rkPacket >> vPos;

			pChar->SetWorldPosition( vPos );

			SetAttackState( pChar );
		}
		break;
	}
}

void ioTargetTeleportAttackSkill::ApplyAttributeToWeapon( ioWeapon *pWeapon, ioPlayStage *pStage )
{
	if ( !pWeapon || !pStage )
		return;

	ioSkill::ApplyAttributeToWeapon( pWeapon, pStage );

	ioZoneEffectWeapon *pZone = ToZoneEffectWeapon( pWeapon );
	if( pZone )
	{
		ioBaseChar *pChar = pZone->GetOwner();
		if( pChar )
		{
			D3DXVECTOR3 vDir = pChar->GetTargetRot() * D3DXVECTOR3( 0.0f, 0.0f, -FLOAT1 );
			pZone->SetCollisionDir( vDir );
			pZone->SetZoneValue( true, true );
		}
	}
}
