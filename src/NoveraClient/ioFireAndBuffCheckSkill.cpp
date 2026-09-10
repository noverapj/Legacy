#include "StdAfx.h"
#include "ioFireAndBuffCheckSkill.h"

#include "ioPlayStage.h"
#include "ioBaseChar.h"

ioFireAndBuffCheckSkill::ioFireAndBuffCheckSkill(void)
{
}

ioFireAndBuffCheckSkill::ioFireAndBuffCheckSkill( const ioFireAndBuffCheckSkill &rhs ):
ioAttackSkill( rhs ),
m_vUpDownLinearComboList( rhs.m_vUpDownLinearComboList ),
m_iMaxAttackCount( rhs.m_iMaxAttackCount ),
m_szCheckBuff( rhs.m_szCheckBuff ),
m_RemoveBuff( rhs.m_RemoveBuff ),
m_TargetAttackType( rhs.m_TargetAttackType ),
m_WholeTargetAttack( rhs.m_WholeTargetAttack ),
m_bUsingTargetCheckTime( rhs.m_bUsingTargetCheckTime )
{

}

ioFireAndBuffCheckSkill::~ioFireAndBuffCheckSkill(void)
{
}

ioSkill* ioFireAndBuffCheckSkill::Clone()
{
	return new ioFireAndBuffCheckSkill( *this );
}

void ioFireAndBuffCheckSkill::ClearData()
{
	m_SkillState = SS_NONE;
	m_dwFireStartTime = 0;
	m_iCurAttackCount = 0;
	m_iCurComboIndex = 0;
	m_szTargetName.Clear();
	m_vTargetInfoList.clear();
	m_dwTargetCheckTime = 0;
	m_iCurWeaponInfo.clear();
}

void ioFireAndBuffCheckSkill::LoadProperty( ioINILoader &rkLoader )
{
	ioAttackSkill::LoadProperty( rkLoader );

	char szBuf[MAX_PATH],szKey[MAX_PATH];

	int iMaxCombo = rkLoader.LoadInt_e( "max_linear_combo", 0 );
	m_vUpDownLinearComboList.clear();
	if ( iMaxCombo > 0 )
	{
		m_vUpDownLinearComboList.reserve( iMaxCombo );
		for( int i = 0; i < iMaxCombo ; ++i )
		{
			UpDownAttackAttribute kUpDownAttribute;

			wsprintf_e( szBuf, "linear_combo%d", i+1 );
			LoadAttackAttribute( szBuf, kUpDownAttribute.m_Attribute, rkLoader );

			wsprintf_e( szBuf, "linear_combo_up_motion%d", i+1 );
			rkLoader.LoadStringA( szBuf, "", szKey, MAX_PATH );
			kUpDownAttribute.m_UpAnimation = szKey;

			wsprintf_e( szBuf, "linear_combo_down_motion%d", i+1 );
			rkLoader.LoadStringA( szBuf, "", szKey, MAX_PATH );
			kUpDownAttribute.m_DownAnimation = szKey;
			m_vUpDownLinearComboList.push_back( kUpDownAttribute );
		}
	}

	int iCnt = rkLoader.LoadInt_e( "remove_buff_cnt", 0 );
	m_RemoveBuff.clear();
	if( iCnt > 0 ) 
	{
		m_RemoveBuff.reserve( iCnt );
		for( int i=0; i < iCnt; ++i )
		{
			wsprintf_e( szBuf, "remove_buff%d", i+1 );
			rkLoader.LoadString( szBuf, "", szKey, MAX_PATH );

			m_RemoveBuff.push_back( szKey );
		}
	}

	rkLoader.LoadString_e( "check_buff", "", szBuf, MAX_PATH );
	m_szCheckBuff = szBuf;

	m_iMaxAttackCount = rkLoader.LoadInt_e( "max_target_attack_count", 0 );

	m_TargetAttackType = (TargetAttackType)rkLoader.LoadInt_e( "target_attack_type", TAT_SINGLE_COMBO );

	LoadAttackAttribute_e( "whole_attack", m_WholeTargetAttack, rkLoader );

	m_bUsingTargetCheckTime = rkLoader.LoadBool_e( "using_target_check_time", false );
}

bool ioFireAndBuffCheckSkill::OnSkillStart( ioBaseChar *pChar )
{
	if ( !pChar )
		return false;

	if( !ioAttackSkill::OnSkillStart( pChar ) )
		return false;

	if( m_bUseItemMesh )
		pChar->ShowWeaponItemMesh( false );

	if( !m_bSetHandMesh && m_bUseItemMesh && !m_HandMesh.IsEmpty() )
	{
		pChar->ChangeSkillWeaponMesh( this, m_HandMesh, m_bVisibleEquipMesh );
		m_bSetHandMesh = true;
	}

	SetCameraBuff( pChar );

	ClearData();

	return true;
}

bool ioFireAndBuffCheckSkill::IsSkillMotionEnd( DWORD dwActionStop, ioBaseChar* pOwner ) const
{
	if ( m_SkillState == SS_END && m_dwMotionEndTime < FRAMEGETTIME() )
		return true;
	return false;
}


void ioFireAndBuffCheckSkill::OnSkillEnd( ioBaseChar *pChar )
{
	ioPlayStage *pStage = pChar->GetCreator();
	if ( pStage )
	{
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

			if ( !pTarget->HasBuff( pChar->GetCharName(), m_szCheckBuff ) )
				continue;

			int nBuffCnt = (int)m_RemoveBuff.size();
			for ( int i=0; i<nBuffCnt; ++i )
				pTarget->RemoveBuff( m_RemoveBuff[i], true );	
		}
	}


	ioAttackSkill::OnSkillEnd( pChar );

	// 기본무기 장착
	if( m_bUseItemMesh )
		pChar->ShowWeaponItemMesh( true );

	// 스킬 중간 동작에 사용된 메쉬들 제거
	m_bSetHandMesh = false;
	if( !m_HandMesh.IsEmpty() )
		pChar->RestoreSkillWeaponMesh( this, m_HandMesh, m_bVisibleEquipMesh );

	EndCameraBuff( pChar );

	ClearData();
}

void ioFireAndBuffCheckSkill::OnProcessState( ioBaseChar *pChar, ioPlayStage *pStage )
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

				SetFireState( pChar );
			}
		}
		break;
	case SS_FIRE:
		{
			if( m_dwMotionEndTime && m_dwMotionEndTime < dwCurTime )
			{
				SetEndState( pChar );
				return;
			}

			if ( pChar->IsNeedProcess() )
			{
				if ( m_dwTargetCheckTime && m_dwTargetCheckTime < dwCurTime && !GetTargetList( pChar, pStage ) )
				{
					SetEndState( pChar, true );
					return;
				}

				if ( pChar->IsAttackKey() && GetTargetList( pChar, pStage ) )
				{
					if ( m_TargetAttackType == TAT_SINGLE_COMBO )
						SetLinearCombo( pChar );
					else if ( m_TargetAttackType == TAT_WHOLE_TARGET )
						SetWholeAttack( pChar );
					return;
				}
			}
			
		}
		break;
	case SS_LINEAR:
		{
			if( m_dwMotionEndTime && m_dwMotionEndTime < dwCurTime )
			{
				SetEndState( pChar );
				return;
			}

			if ( m_dwFireStartTime && m_dwFireStartTime < dwCurTime )
			{
				m_dwFireStartTime = 0;
				FireTarget( pChar );
			}

			if ( pChar->IsNeedProcess() )
			{
				if ( pChar->IsAttackKey() && CheckLinearAttack( pChar ) )
				{
					SetLinearCombo( pChar );
					return;
				}
			}
		}
		break;
	case SS_WHOLE_ATTACK:
		{
			if( m_dwMotionEndTime && m_dwMotionEndTime < dwCurTime )
			{
				SetEndState( pChar );
				return;
			}

			if ( m_dwFireStartTime && m_dwFireStartTime < dwCurTime )
			{
				m_dwFireStartTime = 0;
				FireWholeTarget( pChar, pStage );
			}
		}
		break;
	}
}

void ioFireAndBuffCheckSkill::SetFireState( ioBaseChar *pChar )
{
	if ( !pChar )
		return;

	m_SkillState = SS_FIRE;
	AttackSkillFire( pChar );

	if ( m_bUsingTargetCheckTime )
	{
		ioEntityGroup *pGrp = pChar->GetGroup();
		int iAniID = pGrp->GetAnimationIdx( GetAttribute().m_AttackAnimation );
		if( iAniID == -1 )
			return;
		m_dwTargetCheckTime = FRAMEGETTIME();
		m_dwTargetCheckTime += GetAttribute().m_dwPreDelay + pGrp->GetAnimationFullTime( iAniID ) * GetAttackTimeRate();
	}
}

void ioFireAndBuffCheckSkill::SetEndState( ioBaseChar *pChar, bool bSend /* = false */ )
{
	m_SkillState = SS_END;
	m_dwMotionEndTime = 0;
	if ( bSend && pChar->IsNeedSendNetwork() )
	{
		SP2Packet kPacket( CUPK_SKILL_EXTRA_INFO );
		kPacket << pChar->GetCharName();
		kPacket << GetName();
		kPacket << static_cast<BYTE>(SS_END);
		P2PNetwork::SendToAllPlayingUser( kPacket );
	}
}

bool ioFireAndBuffCheckSkill::GetTargetList( ioBaseChar *pOwner, ioPlayStage *pStage )
{
	if ( !pOwner || !pStage )
		return false;

	const BaseCharList &rkCharList = pStage->GetBaseCharList();
	BaseCharList::const_iterator iter;
	for( iter=rkCharList.begin() ; iter!=rkCharList.end() ; ++iter )
	{
		ioBaseChar *pTarget = *iter;
		if( !pTarget )
			continue;

		if( pOwner->GetCharName() == pTarget->GetCharName() )
			continue;

		if( pOwner->GetTeam() == pTarget->GetTeam() )
			continue;

		if( pTarget->IsSystemState() )
			continue;

		if( !pTarget->IsEnableTarget() )
			continue;

		if ( !pTarget->HasBuff( pOwner->GetCharName(), m_szCheckBuff ) )
			continue;

		//if( !pTarget->CheckTargetWoundType( m_TargetWoundType ) )
		//	continue;

		D3DXVECTOR3 vDiff = pTarget->GetWorldPosition() - pOwner->GetWorldPosition();
		float fDistSq = D3DXVec3LengthSq( &vDiff );

		tagBaseCharDistanceSq kInfo;
		kInfo.m_Name        = pTarget->GetCharName();
		kInfo.m_fDistanceSq = fDistSq;

		m_vTargetInfoList.push_back(kInfo);
	}

	if ( m_vTargetInfoList.empty() )
		return false;

	std::sort( m_vTargetInfoList.begin(), m_vTargetInfoList.end(), BaseCharDistanceSqSort2() );

	m_szTargetName = m_vTargetInfoList.front().m_Name;

	return true;
}

void ioFireAndBuffCheckSkill::SetLinearCombo( ioBaseChar *pChar )
{
	if ( !pChar || m_szTargetName.IsEmpty() )
		return;

	if ( m_vUpDownLinearComboList.empty() )
	{
		SetEndState( pChar );
		return;
	}

	if ( m_iCurComboIndex >= (int)m_vUpDownLinearComboList.size() )
		return;

	ioEntityGroup *pGrp = pChar->GetGroup();
	if ( !pGrp )
		return;

	const UpDownAttackAttribute& rkUpDownAttri = m_vUpDownLinearComboList[m_iCurComboIndex];
	const AttackAttribute& rkAttri = rkUpDownAttri.m_Attribute;
	int iAniID = pGrp->GetAnimationIdx( rkAttri.m_AttackAnimation );
	if( iAniID == -1 )
		return;

	m_iCurWeaponInfo.clear();
	m_iCurWeaponInfo = m_vUpDownLinearComboList[m_iCurComboIndex].m_Attribute.m_vWeaponInfoList;

	m_SkillState = SS_LINEAR;

	SetTargetRotation( pChar );

	pGrp->ClearAllLoopAni( FLOAT100, true );
	pGrp->ClearAllActionAni( FLOAT100, true );
	pChar->ClearReservedSliding();

	float fTimeRate  = FLOAT1;
	if ( rkAttri.m_fAttackAniRate != 0.0f )
		fTimeRate = rkAttri.m_fAttackAniRate;
	DWORD dwPreDelay = rkAttri.m_dwPreDelay;

	DWORD dwCurTime = FRAMEGETTIME();
	m_dwMotionStartTime = m_dwMotionEndTime = dwCurTime;
	m_dwMotionEndTime += dwPreDelay + pGrp->GetAnimationFullTime( iAniID ) * fTimeRate;
	m_dwMotionEndTime += rkAttri.m_dwEndDelay;

	m_dwFireStartTime = m_dwFireMotionEndTime = dwCurTime;
	m_dwFireStartTime += dwPreDelay;
	m_dwFireStartTime += pGrp->GetAnimationEventTime_e( iAniID, "fire_s" ) * fTimeRate;

	float fAttackEnd = pGrp->GetAnimationEventTime_e( iAniID, "attack_end" ) * fTimeRate;
	if( fAttackEnd > 0.0f )
		m_dwAttackEndTime = dwCurTime + dwPreDelay + fAttackEnd;
	else
		m_dwAttackEndTime = 0;

	m_dwFireMotionEndTime += dwPreDelay;
	m_dwFireMotionEndTime += pGrp->GetAnimationEventTime_e( iAniID, "firemotion_e" ) * fTimeRate;

	if( m_dwFireStartTime >= m_dwFireMotionEndTime )
		m_dwFireMotionEndTime = m_dwMotionEndTime;

	if ( rkUpDownAttri.m_DownAnimation.IsEmpty() || rkUpDownAttri.m_UpAnimation.IsEmpty() )
		pGrp->SetActionAni( iAniID, FLOAT100, FLOAT100, FLOAT1, FLOAT1/fTimeRate, 0.0f, dwPreDelay, true );
	else
		SetAttackAni( pGrp, rkAttri.m_AttackAnimation, rkUpDownAttri.m_UpAnimation, rkUpDownAttri.m_DownAnimation, fTimeRate, dwPreDelay );

	pChar->CheckCharColSkipSkillTime( iAniID, fTimeRate, dwPreDelay );
	pChar->SetReservedSliding( rkAttri.m_vForceInfoList, iAniID, fTimeRate, dwPreDelay );

	if ( pChar->IsNeedSendNetwork() )
	{
		SP2Packet kPacket( CUPK_SKILL_EXTRA_INFO );
		kPacket << pChar->GetCharName();
		kPacket << GetName();
		kPacket << static_cast<BYTE>(SS_LINEAR);
		kPacket << m_iCurComboIndex;
		kPacket << m_szTargetName;
		P2PNetwork::SendToAllPlayingUser( kPacket );
	}

	if ( pChar->IsNeedProcess() )
	{
		m_iCurComboIndex++;
		if ( m_iCurComboIndex >= (int)m_vUpDownLinearComboList.size() )
			m_iCurComboIndex = 0;
	}
	
}

bool ioFireAndBuffCheckSkill::CheckLinearAttack( ioBaseChar *pOwner )
{
	if ( m_vUpDownLinearComboList.empty() )
		return false;

	if ( m_iMaxAttackCount <= m_iCurAttackCount )
		return false;

	if ( !CheckTargetList( pOwner ) )
		return false;

	return true;
}

bool ioFireAndBuffCheckSkill::CheckTargetList( ioBaseChar *pOwner )
{
	if ( m_vTargetInfoList.empty() )
		return false;

	ioHashString szBeforeTarget = m_szTargetName;
	BaseCharDistanceSqList::iterator iter = m_vTargetInfoList.begin();
	while( iter != m_vTargetInfoList.end() )
	{
		if ( (*iter).m_Name == szBeforeTarget )
			m_szTargetName.Clear();

		ioBaseChar *pTarget = pOwner->GetBaseChar( (*iter).m_Name );
		if ( !pTarget || !pTarget->HasBuff( pOwner->GetCharName(), m_szCheckBuff ) )
			iter = m_vTargetInfoList.erase( iter );
		else
		{
			if ( m_szTargetName.IsEmpty() && (*iter).m_Name != szBeforeTarget )
				m_szTargetName = (*iter).m_Name;
			++iter;
		}
	}

	if ( m_vTargetInfoList.empty() )
		return false;

	//만약 전에 공격했던 타겟이 마지막 타겟일 경우 처음 타겟으로 세팅.
	if ( m_szTargetName.IsEmpty() )
		m_szTargetName = m_vTargetInfoList.front().m_Name;

	return true;
}

void ioFireAndBuffCheckSkill::FireTarget( ioBaseChar *pOwner )
{
	if ( m_szTargetName.IsEmpty() )
		return;

	ioBaseChar *pTarget = pOwner->GetBaseChar( m_szTargetName );
	if ( !pTarget )
		return;

	if ( m_iCurWeaponInfo.empty() )
		return;

	m_dwWeaponBaseIndex = pOwner->IncreaseWeaponIndexBase();

	for ( int i=0 ; i< (int)m_iCurWeaponInfo.size() ; ++i )
	{
		FireTime kFireTime;
		kFireTime.dwStart = FRAMEGETTIME() + m_iCurWeaponInfo[i].m_dwCallTime;
		kFireTime.iAttributeIdx = m_iCurWeaponInfo[i].m_iWeaponIdx;
		kFireTime.iResistanceIdx = m_iCurWeaponInfo[i].m_iResistanceIdx;
		kFireTime.szWoundedAni  = m_iCurWeaponInfo[i].m_WoundedAnimation;
		kFireTime.dwWoundedAniDuration = m_iCurWeaponInfo[i].m_dwWoundedDuration;
		kFireTime.bLoopWoundedAni = m_iCurWeaponInfo[i].m_bWoundedLoopAni;
		kFireTime.dwWeaponIdx = ++m_dwWeaponBaseIndex;
		kFireTime.eFireTimeType = FTT_SKILL_ATTACK;

		D3DXVECTOR3 vPos = pOwner->GetMidPositionByRate();

		D3DXVECTOR3 vDir = pTarget->GetWorldPosition() - pOwner->GetWorldPosition();
		D3DXVec3Normalize( &vDir, &vDir );

		ioHashString szItemName;
		if ( GetOwnerItem() )
			szItemName = GetOwnerItem()->GetName();
		pOwner->ExtendFireExplicit( kFireTime, vPos, vDir, szItemName );
	}

	m_iCurAttackCount++;
}

void ioFireAndBuffCheckSkill::SetTargetRotation( ioBaseChar *pChar )
{
	if ( !pChar || m_szTargetName.IsEmpty() )
		return;
	m_vTargetDir = ioMath::VEC3_ZERO;
	ioBaseChar *pTarget = pChar->GetBaseChar( m_szTargetName );
	if ( !pTarget )
		return;

	m_vTargetDir = pTarget->GetMidPositionByRate() - pChar->GetMidPositionByRate();
	D3DXVec3Normalize( &m_vTargetDir, &m_vTargetDir );
	pChar->SetTargetRotToDir( m_vTargetDir, true );
}

float ioFireAndBuffCheckSkill::GetAimedUpDownWeight( const D3DXVECTOR3 &vAimDir, float fMaxAngle )
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

void ioFireAndBuffCheckSkill::SetAttackAni( ioEntityGroup *pGrp, const ioHashString& szCurAni, const ioHashString& szUpAni, const ioHashString& szDownAni, const float& fTimeRate, const DWORD dwPreDelay )
{
	if ( !pGrp )
		return;

	if ( m_vTargetDir == ioMath::VEC3_ZERO )
		return;

	float fAniWeight = GetAimedUpDownWeight( m_vTargetDir, 85.0f );
	if ( m_vTargetDir.y >= 0 )
	{
		bool bAniFx1 = false;
		bool bAniFx2 = true;

		if( fAniWeight > FLOAT05 )
		{
			bAniFx1 = true;
			bAniFx2 = false;
		}
		pGrp->SetActionAni( szUpAni, FLOAT100, FLOAT100, fAniWeight, FLOAT1/fTimeRate, 0.0f, dwPreDelay, true, bAniFx1 );
		pGrp->SetActionAni( szCurAni, FLOAT100, FLOAT100, FLOAT1 - fAniWeight, FLOAT1/fTimeRate, 0.0f, dwPreDelay, true, bAniFx2 );
	}
	else
	{
		bool bAniFx1 = true;
		bool bAniFx2 = false;

		if( fAniWeight > FLOAT05 )
		{
			bAniFx1 = false;
			bAniFx2 = true;
		}
		pGrp->SetActionAni( szCurAni, FLOAT100, FLOAT100, FLOAT1 - fAniWeight, FLOAT1/fTimeRate, 0.0f, dwPreDelay, true, bAniFx1 );
		pGrp->SetActionAni( szDownAni, FLOAT100, FLOAT100, fAniWeight, FLOAT1/fTimeRate, 0.0f, dwPreDelay, true, bAniFx2 );
	}
}

void ioFireAndBuffCheckSkill::SetWholeAttack( ioBaseChar *pChar )
{
	ioEntityGroup *pGrp = pChar->GetGroup();
	if ( !pGrp )
		return;

	int iAniID = pGrp->GetAnimationIdx( m_WholeTargetAttack.m_AttackAnimation );
	if( iAniID == -1 )
		return;

	m_SkillState = SS_WHOLE_ATTACK;

	pGrp->ClearAllLoopAni( FLOAT100, true );
	pGrp->ClearAllActionAni( FLOAT100, true );
	pChar->ClearReservedSliding();

	float fTimeRate  = FLOAT1;
	if ( m_WholeTargetAttack.m_fAttackAniRate != 0.0f )
		fTimeRate = m_WholeTargetAttack.m_fAttackAniRate;
	DWORD dwPreDelay = m_WholeTargetAttack.m_dwPreDelay;

	DWORD dwCurTime = FRAMEGETTIME();
	m_dwMotionStartTime = m_dwMotionEndTime = dwCurTime;

	m_dwMotionEndTime += pGrp->GetAnimationFullTime( iAniID ) * fTimeRate;
	m_dwMotionEndTime += dwPreDelay + m_WholeTargetAttack.m_dwEndDelay;

	m_dwFireStartTime = m_dwFireMotionEndTime = dwCurTime;
	m_dwFireStartTime += dwPreDelay;
	m_dwFireStartTime += pGrp->GetAnimationEventTime_e( iAniID, "fire_s" ) * fTimeRate;

	float fAttackEnd = pGrp->GetAnimationEventTime_e( iAniID, "attack_end" ) * fTimeRate;
	if( fAttackEnd > 0.0f )
		m_dwAttackEndTime = dwCurTime + dwPreDelay + fAttackEnd;
	else
		m_dwAttackEndTime = 0;

	m_dwFireMotionEndTime += dwPreDelay;
	m_dwFireMotionEndTime += pGrp->GetAnimationEventTime_e( iAniID, "firemotion_e" ) * fTimeRate;

	if( m_dwFireStartTime >= m_dwFireMotionEndTime )
		m_dwFireMotionEndTime = m_dwMotionEndTime;

	pGrp->SetActionAni( iAniID, FLOAT100, FLOAT100, FLOAT1, FLOAT1/fTimeRate, 0.0f, dwPreDelay, true );
	
	pChar->CheckCharColSkipSkillTime( iAniID, fTimeRate, dwPreDelay );
	pChar->SetReservedSliding( m_WholeTargetAttack.m_vForceInfoList, iAniID, fTimeRate, dwPreDelay );

	if ( pChar->IsNeedSendNetwork() )
	{
		SP2Packet kPacket( CUPK_SKILL_EXTRA_INFO );
		kPacket << pChar->GetCharName();
		kPacket << GetName();
		kPacket << static_cast<BYTE>(SS_WHOLE_ATTACK);
		P2PNetwork::SendToAllPlayingUser( kPacket );
	}
}

void ioFireAndBuffCheckSkill::FireWholeTarget( ioBaseChar *pOwner, ioPlayStage *pStage )
{
	if ( !pOwner || !pStage )
		return;

	const WeaponInfoList& rkList = m_WholeTargetAttack.m_vWeaponInfoList;
	if ( rkList.empty() )
		return;
	m_dwWeaponBaseIndex = pOwner->IncreaseWeaponIndexBase();
	m_vTargetInfoList.clear();
	if( GetTargetList( pOwner, pStage ) )
	{
		D3DXVECTOR3 vOwnerPos = pOwner->GetMidPositionByRate();
		for ( int i=0 ; i<(int)m_vTargetInfoList.size() ; ++i )
		{
			ioBaseChar *pTarget = pOwner->GetBaseChar( m_vTargetInfoList[i].m_Name );
			if ( !pTarget )
				continue;

			for ( int j=0 ; j<(int)rkList.size() ; ++j )
			{
				D3DXVECTOR3 vDir = pTarget->GetMidPositionByRate() - vOwnerPos;
				D3DXVec3Normalize( &vDir, &vDir );

				pOwner->SkillFireExplicit( rkList[j],
					GetName(),
					m_dwWeaponBaseIndex,
					pOwner->GetMidPositionByRate(), vDir );
			}
		}
	}
	else
	{
		for ( int i=0 ; i<(int)rkList.size() ; ++i )
		{
			pOwner->SkillFireExplicit( rkList[i],
				GetName(),
				m_dwWeaponBaseIndex,
				pOwner->GetMidPositionByRate() );
		}
	}

	
	
}

void ioFireAndBuffCheckSkill::ApplyExtraSkillInfo( ioBaseChar *pChar, ioPlayStage *pStage, SP2Packet &rkPacket )
{
	if ( !pChar || !pStage )
		return;

	BYTE btState;
	rkPacket >> btState;
	if ( btState == SS_LINEAR )
	{
		rkPacket >> m_iCurComboIndex;
		m_szTargetName.Clear();
		rkPacket >> m_szTargetName;
		SetLinearCombo( pChar );
	}
	else if ( btState == SS_WHOLE_ATTACK )
		SetWholeAttack( pChar );
	else if ( btState == SS_END )
		SetEndState( pChar );
}
