

#include "stdafx.h"

#include "ioBaseChar.h"
#include "ioPlayStage.h"
#include "ioGrowthUpInfo.h"

#include "ioRangeSyncBuffSkill.h"

ioRangeSyncBuffSkill::ioRangeSyncBuffSkill()
{
}

ioRangeSyncBuffSkill::ioRangeSyncBuffSkill( const ioRangeSyncBuffSkill &rhs )
: ioRangeSkill( rhs )
{
}

ioRangeSyncBuffSkill::~ioRangeSyncBuffSkill()
{
}

void ioRangeSyncBuffSkill::LoadProperty( ioINILoader &rkLoader )
{
	ioRangeSkill::LoadProperty( rkLoader );
}

ioSkill* ioRangeSyncBuffSkill::Clone()
{
	return new ioRangeSyncBuffSkill( *this );
}

void ioRangeSyncBuffSkill::SetBuffList( ioBaseChar *pOwner, ioPlayStage *pStage )
{
	if( !pOwner )
	{
		LOG.PrintTimeAndLog( 0, "ioRangeBuffSkill::SetBuffList() - Owner is NULL" );
		return;
	}

	if( !pOwner->IsNeedProcess() )
		return;

	if( pOwner->IsNeedSendNetwork() )
	{
		SP2Packet kPacket( CUPK_SKILL_EXTRA_INFO );
		kPacket << pOwner->GetCharName();
		kPacket << GetName();
		kPacket << ESI_SET_BUFF;
		P2PNetwork::SendToAllPlayingUser( kPacket );
	}

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
		if( pChar->GetExperienceMode() == EMS_EXPERIENCE ) continue;
		if( pChar->IsSkipState() ) continue;

		D3DXVECTOR3 vStartPos = pOwner->GetMidPositionByRate();
		D3DXVECTOR3 vEndPos = pChar->GetMidPositionByRate();
		if( m_bCheckCollisionLine && !pStage->CheckCollisionLine( vStartPos, vEndPos ) )
			continue;

		if( !pChar->CheckTargetWoundType(m_TargetWoundType) )
			continue;

		bool bEnableTeamAttack = pChar->IsEnableTeamAttack();

		if( pOwner->GetCharName() == pChar->GetCharName() && !IsCollisioned(pChar) )
		{
			SetOwnerBuffList( pChar );
		}
		else if( pOwner->GetTeam() == pChar->GetTeam() && SetTeamBuffList( pOwner, pChar ) )	// Team
		{
			pOwner->CheckTeamBuffAttackCnt( pChar );
		}
		
		if( (bEnableTeamAttack || pOwner->GetTeam() != pChar->GetTeam()) && SetEnemyBuffList( pOwner, pChar ) )	// Enemy
		{
			//pOwner->CheckRealComboHit( pChar->GetCharName() );
			pOwner->CheckComboSkillHit();
			pOwner->CheckAttackCnt( pChar );
		}
	}

	pOwner->ReduceNeedGaugeBySkill( this, GetNeedGauge(pOwner->GetCharName()) );
	m_bReduceGauge = true;
}

bool ioRangeSyncBuffSkill::SetEnemyBuffList( ioBaseChar *pOwner, ioBaseChar *pChar )
{
	if( pChar->IsProtectState() )
	{
		int iType = pChar->CheckBlockEmoticonType();
		if( iType >= 0 && !IsBlocked( pChar ) )
		{
			m_vBlockedList.push_back( pChar->GetCharName() );
			pChar->SetProtectWound( iType );
		}

		return false;
	}

	if( m_EnemyBuffList.empty() )
		return false;

	pChar->SetRandomSeed( 0 );

	bool bResult = false;
	ioHashStringVec vBuffList;

	ioHashString szItemName;
	ioItem *pItem = GetOwnerItem();
	if( pItem )
	{
		szItemName = pItem->GetName();
	}

	float fValue = 0.0f;
	ioGrowthNormalUpInfo *pUpinfo = ToNormalUpInfo( GetGrowthUpInfoByType(GT_BUFF_DURATION, pChar->GetCharName()) );
	if( pUpinfo )
	{
		ioPlayStage *pStage = g_SkillMaker.GetPlayStage();
		fValue = pUpinfo->GetValue(pStage);
	}

	int iCnt = m_EnemyBuffList.size();
	for( int i=0; i < iCnt; ++i )
	{
		const ioHashString& szBuffName = m_EnemyBuffList[i].m_BuffName;

		if( pChar->IsHasCrown() && m_bDisableKingSkill )
		{
			continue;
		}
		else if( pChar->HasBuff( BT_ABSOLUTE_PROTECTION ) || pChar->HasBuff(BT_NOT_MOVE_PROTECT) )
			continue;
		else if( pChar->IsCanNoWound() )
			continue;

		if( !szBuffName.IsEmpty() )
		{
			ioBuff *pBuff = pChar->AddNewBuffWithAnimateTime( szBuffName, pOwner->GetCharName(), szItemName, this, GetSkillMotionTime() );
			if( pBuff )
			{
				if( m_bEnableAttacker )
				{
					pChar->SetLastAttackerName( pOwner->GetCharName(), GetName() );
				}

				pBuff->SetExtraDurationByGrowth( fValue );

				vBuffList.push_back( szBuffName );
				bResult = true;
			}
		}
	}

	if( bResult )
	{
		m_vCollisionedList.push_back( pChar->GetCharName() );

		int iCurBuffCnt = vBuffList.size();
		if( pChar->IsNeedSendNetwork() )
		{
			if( Help::CheckEnableTimeGapUser() )
			{
				LOG.PrintTimeAndLog( 0, "PC -SkillAddBuff14 : %s, %d", GetName().c_str(), FRAMEGETTIME() );
			}
			SP2Packet kPacket( CUPK_ADD_NEW_BUFF );
			kPacket << pChar->GetCharName();
			kPacket << pOwner->GetCharName();
			kPacket << szItemName;
			kPacket << pChar->GetRandomSeed();
			kPacket << true;					// 타격횟수 체크
			kPacket << 0.0f;
			kPacket << fValue;
			kPacket << 0.0f;
			kPacket << COWT_NONE;				// Check CloseOrderBuff(12.07.13 수정)
			kPacket << iCurBuffCnt;

			for( i=0; i < iCurBuffCnt; i++ )
			{
				kPacket << vBuffList[i];
			}

			kPacket << false;					// Use Force

			P2PNetwork::SendToAllPlayingUser( kPacket );

			SP2Packet kPacket2( CUPK_SKILL_EXTRA_INFO );
			kPacket2 << pOwner->GetCharName();
			kPacket2 << GetName();
			kPacket2 << ESI_COLLISION;
			kPacket2 << pChar->GetCharName();
			P2PNetwork::SendToAllPlayingUser( kPacket2 );
		}
	}

	return bResult;
}


void ioRangeSyncBuffSkill::OnProcessState( ioBaseChar *pChar, ioPlayStage *pStage )
{
	DWORD dwPreDelayTime = GetPreDelayTime();

	switch( m_RangeState )
	{
	case RS_NONE:
		if( m_dwPreDelayStartTime > 0 && m_dwPreDelayStartTime + dwPreDelayTime < FRAMEGETTIME() )
		{
			m_dwPreDelayStartTime = 0;
			EndPreDelayEffect( pChar );
			SetActionState( pChar );
		}
		break;
	case RS_ACTION:
		if( m_dwMotionStartTime > 0 && m_dwMotionEndTime < FRAMEGETTIME() )
		{
			m_dwMotionStartTime = 0;
			SetLoopState( pChar, pStage );
		}
		break;
	case RS_LOOP:
		if( m_dwLoopStartTime > 0 && m_dwLoopStartTime+m_dwLoopTime < FRAMEGETTIME() )
		{
			m_dwLoopStartTime = 0;
			SetEndState( pChar );
		}
		else if( m_dwLoopStartTime > 0 && ( m_dwDuration == 0 || m_dwLoopStartTime+m_dwDuration > FRAMEGETTIME() ) )
		{
			SetBuffList( pChar, pStage );
		}
		break;
	}
}

void ioRangeSyncBuffSkill::SetLoopState( ioBaseChar *pChar, ioPlayStage *pStage )
{
	ioEntityGroup *pGrp = pChar->GetGroup();
	int iAniID = pGrp->GetAnimationIdx( m_LoopAnimation );

	pGrp->ClearAllLoopAni( FLOAT100, true );
	pGrp->ClearAllActionAni( FLOAT100, true );

	if( iAniID == -1 )
	{
		m_dwEnableReserveTime = FRAMEGETTIME();
		LOG.PrintTimeAndLog( 0, "ioRangeBuffSkill::SetLoopState() - %s, Not Exist LoopAnimation",
								 GetName().c_str() );
	}
	else
	{
		pGrp->SetLoopAni( m_LoopAnimation, FLOAT100 );
		m_dwEnableReserveTime = FRAMEGETTIME();
		m_dwEnableReserveTime += pGrp->GetAnimationEventTime_e( iAniID, "key_reserve" );
	}

	D3DXVECTOR3 vPos = pChar->GetMidPositionByRate( FLOAT08 ) + m_vEffectDir * m_fOffSet;
	pChar->CreateMapEffectBySkill( m_RangeEffect, vPos );

	if( m_bSetHandMesh )
	{
		if( !m_StartHandMesh.IsEmpty() )
		{
			pChar->RestoreSkillWeaponMesh( this, m_StartHandMesh, m_bVisibleEquipMesh );
		}
		m_bSetHandMesh = false;
	}

	// 사용 아이템 처리
	if( !m_bSetHandMesh && m_bUseItemMesh && !m_LoopHandMesh.IsEmpty() )
	{
		pChar->ChangeSkillWeaponMesh( this, m_LoopHandMesh, m_bVisibleEquipMesh );
		m_bSetHandMesh = true;
	}

	m_dwLoopStartTime = FRAMEGETTIME();
	m_dwProtectTime = m_dwLoopStartTime + m_dwLoopProtectDuration;

	m_RangeState = RS_LOOP;
}

void ioRangeSyncBuffSkill::ApplyExtraSkillInfo( ioBaseChar *pOwner, ioPlayStage *pStage, SP2Packet &rkPacket )
{
	int iType;
	rkPacket >> iType;

	switch( iType )
	{
	case ESI_SET_BUFF:
		ApplySetBuffList( pOwner, pStage, rkPacket );
		break;
	case ESI_COLLISION:
		ApplySetCollisioned( pOwner, pStage, rkPacket );
		break;
	}
}

void ioRangeSyncBuffSkill::ApplySetBuffList( ioBaseChar *pOwner, ioPlayStage *pStage, SP2Packet &rkPacket )
{
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

		if( !pChar || IsCollisioned(pChar) ) continue;
		if( !pChar->IsNeedProcess() ) continue;
		if( !pStage->IsCollisionCheckByMode() ) continue;
		if( pChar->GetExperienceMode() == EMS_EXPERIENCE ) continue;

		D3DXVECTOR3 vStartPos = pOwner->GetMidPositionByRate();
		D3DXVECTOR3 vEndPos = pChar->GetMidPositionByRate();
		if( m_bCheckCollisionLine && !pStage->CheckCollisionLine( vStartPos, vEndPos ) )
			continue;

		if( !pChar->CheckTargetWoundType(m_TargetWoundType) )
			continue;

		bool bEnableTeamAttack = pChar->IsEnableTeamAttack();

		if( pOwner->GetCharName() == pChar->GetCharName() && !IsCollisioned(pChar) )
		{
			SetOwnerBuffList( pChar );
		}
		else if( pOwner->GetTeam() == pChar->GetTeam() && SetTeamBuffList( pOwner, pChar ) )	// Team
		{
			pOwner->CheckTeamBuffAttackCnt( pChar );
		}

		if( (bEnableTeamAttack || pOwner->GetTeam() != pChar->GetTeam()) && SetEnemyBuffList( pOwner, pChar ) )	// Enemy
		{
			//pOwner->CheckRealComboHit( pChar->GetCharName() );
			pOwner->CheckComboSkillHit();
			pOwner->CheckAttackCnt( pChar );
		}
	}
}

void ioRangeSyncBuffSkill::ApplySetCollisioned( ioBaseChar *pOwner, ioPlayStage *pStage, SP2Packet &rkPacket )
{
	ioHashString szName;
	rkPacket >> szName;

	if( !szName.IsEmpty() )
		m_vCollisionedList.push_back( szName );
}

void ioRangeSyncBuffSkill::OnSkillEnd( ioBaseChar *pOwner )
{
	//UJ 130823, CPU 랙발생시켜서 state를 건너뛰고 gauge를 닳지 않게 하는 치팅을 수정
	if( !m_bReduceGauge && pOwner )
	{
		pOwner->ReduceNeedGaugeBySkill( this, GetNeedGauge(pOwner->GetCharName()) );
		m_bReduceGauge = true;
	}

	// 기본무기 장착
	if( m_bUseItemMesh )
	{
		pOwner->ShowWeaponItemMesh( true );
	}

	// 스킬 중간 동작에 사용된 메쉬들 제거
	m_bSetHandMesh = false; 

	if( !m_StartHandMesh.IsEmpty() )
		pOwner->RestoreSkillWeaponMesh( this, m_StartHandMesh, m_bVisibleEquipMesh );

	if( !m_LoopHandMesh.IsEmpty() )
		pOwner->RestoreSkillWeaponMesh( this, m_LoopHandMesh, m_bVisibleEquipMesh );

	if( !m_EndHandMesh.IsEmpty() )
		pOwner->RestoreSkillWeaponMesh( this, m_EndHandMesh, m_bVisibleEquipMesh );


	EndPreDelayEffect( pOwner );
	GetOwnerItem()->ClearTarget();

	pOwner->EndEffect( m_SkillEffectName );

	// m_vCollisionedList.clear() 보다 먼저해야한다
	RemoveBuffList( pOwner );

	m_vCollisionedList.clear();
	m_vBlockedList.clear();

	m_bReduceGauge = false;
	pOwner->SetMoveDirByRotate( pOwner->GetTargetRot() );
	pOwner->RestoreSkillProtection();

	m_SkillUseType = SUT_NORMAL;

	if( !m_SkillEndEffectName.IsEmpty() )
		pOwner->AttachEffect( m_SkillEndEffectName );
}

void ioRangeSyncBuffSkill::RemoveBuffList( ioBaseChar* pOwner )
{
	if( !pOwner ) return;

	const ioHashString& szOwnerName = pOwner->GetCharName();

	int iSize = m_vCollisionedList.size();
	for( int i=0; i < iSize; ++i )
	{
		const ioHashString& szName = m_vCollisionedList[i];

		ioBaseChar *pChar = pOwner->GetBaseChar( szName );
		if( pChar )
		{
			int iCnt = m_EnemyBuffList.size();
			for( int j=0; j < iCnt; ++j )
			{
				const ioHashString& szBuffName = m_EnemyBuffList[j].m_BuffName;
				pChar->RemoveBuffOwnerName( szBuffName, szOwnerName );
			}

			iCnt = m_OwnerBuffList.size();
			for( int j=0; j < iCnt; ++j )
			{
				const ioHashString& szBuffName = m_OwnerBuffList[j].m_BuffName;
				pChar->RemoveBuffOwnerName( szBuffName, szOwnerName );
			}

			iCnt = m_TeamBuffList.size();
			for( int j=0; j < iCnt; ++j )
			{
				const ioHashString& szBuffName = m_TeamBuffList[j].m_BuffName;
				pChar->RemoveBuffOwnerName( szBuffName, szOwnerName );
			}
		}
	}
}

