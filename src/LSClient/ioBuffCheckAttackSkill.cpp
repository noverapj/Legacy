#include "StdAfx.h"
#include "ioBuffCheckAttackSkill.h"

#include "ioPlayStage.h"
#include "ioBaseChar.h"

#include "ioStatAttribute.h"

ioBuffCheckAttackSkill::ioBuffCheckAttackSkill(void)
{
	ClearData();
}

ioBuffCheckAttackSkill::ioBuffCheckAttackSkill( const ioBuffCheckAttackSkill &rhs ):
ioCountSkill( rhs ),
m_vNormalGroundAttackList( rhs.m_vNormalGroundAttackList ),
m_vCheckNormalGroundBuffList( rhs.m_vCheckNormalGroundBuffList ),
m_vNormalAirAttackList( rhs.m_vNormalAirAttackList ),
m_vCheckNormalAirBuffList( rhs.m_vCheckNormalAirBuffList ),
m_vJumpGroundAttackList( rhs.m_vJumpGroundAttackList ),
m_vCheckJumpGroundAttackList( rhs.m_vCheckJumpGroundAttackList ),
m_vJumpAirAttackList( rhs.m_vJumpAirAttackList ),
m_vCheckJumpAirAttackList( rhs.m_vCheckJumpAirAttackList )
{
	ClearData();
}

ioBuffCheckAttackSkill::~ioBuffCheckAttackSkill(void)
{
}

ioSkill* ioBuffCheckAttackSkill::Clone()
{
	return new ioBuffCheckAttackSkill( *this );
}

void ioBuffCheckAttackSkill::LoadProperty( ioINILoader &rkLoader )
{
	ioCountSkill::LoadProperty( rkLoader );
	char szBuf[MAX_PATH];
	char szKey[MAX_PATH];

	//Normal Attack Attribute
	int iMaxCount = rkLoader.LoadInt_e( "normal_ground_attack_count", 0 );
	m_vNormalGroundAttackList.clear();
	m_vNormalGroundAttackList.reserve( iMaxCount );
	for( int i = 0;i < iMaxCount ;i++)
	{
		AttackAttribute kAttribute;
		wsprintf_e( szBuf, "normal_ground_attack%d", i+1 );
		LoadAttackAttribute( szBuf, kAttribute, rkLoader );		
		m_vNormalGroundAttackList.push_back( kAttribute );
	}
	iMaxCount = rkLoader.LoadInt_e( "normal_air_attack_count", 0 );
	m_vNormalAirAttackList.clear();
	m_vNormalAirAttackList.reserve( iMaxCount );
	for( int i = 0;i < iMaxCount ;i++)
	{
		AttackAttribute kAttribute;
		wsprintf_e( szBuf, "normal_air_attack%d", i+1 );
		LoadAttackAttribute( szBuf, kAttribute, rkLoader );		
		m_vNormalAirAttackList.push_back( kAttribute );
	}

	//Jump Attack Attribute
	iMaxCount = rkLoader.LoadInt_e( "jump_ground_attack_count", 0 );
	m_vJumpGroundAttackList.clear();
	m_vJumpGroundAttackList.reserve( iMaxCount );
	for( int i = 0;i < iMaxCount ;i++)
	{
		AttackAttribute kAttribute;
		wsprintf_e( szBuf, "jump_ground_attack%d", i+1 );
		LoadAttackAttribute( szBuf, kAttribute, rkLoader );		
		m_vJumpGroundAttackList.push_back( kAttribute );
	}
	iMaxCount = rkLoader.LoadInt_e( "jump_air_attack_count", 0 );
	m_vJumpAirAttackList.clear();
	m_vJumpAirAttackList.reserve( iMaxCount );
	for( int i = 0;i < iMaxCount ;i++)
	{
		AttackAttribute kAttribute;
		wsprintf_e( szBuf, "jump_air_attack%d", i+1 );
		LoadAttackAttribute( szBuf, kAttribute, rkLoader );		
		m_vJumpAirAttackList.push_back( kAttribute );
	}

	//Normal Buff List
	m_vCheckNormalGroundBuffList.clear();
	iMaxCount = rkLoader.LoadInt_e( "normal_ground_check_buff_cnt", 0 );
	m_vCheckNormalGroundBuffList.reserve( iMaxCount );
	for( int i=0; i < iMaxCount; ++i )
	{
		wsprintf_e( szBuf, "normal_ground_check_buff%d", i+1 );
		rkLoader.LoadString( szBuf, "", szKey, MAX_PATH );
		m_vCheckNormalGroundBuffList.push_back( szKey );
	}
	m_vCheckNormalAirBuffList.clear();
	iMaxCount = rkLoader.LoadInt_e( "normal_air_check_buff_cnt", 0 );
	m_vCheckNormalAirBuffList.reserve( iMaxCount );
	for( int i=0; i < iMaxCount; ++i )
	{
		wsprintf_e( szBuf, "normal_air_check_buff%d", i+1 );
		rkLoader.LoadString( szBuf, "", szKey, MAX_PATH );
		m_vCheckNormalAirBuffList.push_back( szKey );
	}

	//Jump Buff List
	m_vCheckJumpGroundAttackList.clear();
	iMaxCount = rkLoader.LoadInt_e( "jump_ground_attack_buff_cnt", 0 );
	m_vCheckJumpGroundAttackList.reserve( iMaxCount );
	for( int i=0; i < iMaxCount; ++i )
	{
		wsprintf_e( szBuf, "jump_ground_attack_buff%d", i+1 );
		rkLoader.LoadString( szBuf, "", szKey, MAX_PATH );
		m_vCheckJumpGroundAttackList.push_back( szKey );
	}
	m_vCheckJumpAirAttackList.clear();
	iMaxCount = rkLoader.LoadInt_e( "jump_air_attack_buff_cnt", 0 );
	m_vCheckJumpAirAttackList.reserve( iMaxCount );
	for( int i=0; i < iMaxCount; ++i )
	{
		wsprintf_e( szBuf, "jump_air_attack_buff%d", i+1 );
		rkLoader.LoadString( szBuf, "", szKey, MAX_PATH );
		m_vCheckJumpAirAttackList.push_back( szKey );
	}
}

void ioBuffCheckAttackSkill::ClearData()
{
	m_SkillState = SS_PRE;
	m_TargetPositionType = TPT_NONE;
	m_vAutoTargetDir = ioMath::VEC3_ZERO;
	m_iNormalAttackIndex = 0;
	m_iJumpAttackIndex = 0;
	m_vReservedSlidingList.clear();
	m_dwTrackingEndTime = 0;
}

bool ioBuffCheckAttackSkill::OnSkillStart( ioBaseChar *pOwner )
{
	ClearData();

	if( !ioCountSkill::OnSkillStart(pOwner) )
		return false;

	if ( !pOwner )
		return false;

	if( m_bUseItemMesh )
		pOwner->ShowWeaponItemMesh( false );

	if( !m_bSetHandMesh && m_bUseItemMesh && !m_HandMesh.IsEmpty() )
	{
		pOwner->ChangeSkillWeaponMesh( this, m_HandMesh, m_bVisibleEquipMesh );
		m_bSetHandMesh = true;
	}

	if( pOwner->IsNeedProcess() )
		CheckTargetPositionType( pOwner );

	return true;
}

void ioBuffCheckAttackSkill::CheckTargetPositionType( ioBaseChar *pOwner )
{
	if ( !pOwner )
		return;
	//AutoTarget Direction
	ioBaseChar* pTarget = pOwner->GetBaseChar( pOwner->GetAutoTargetName() );
	if( pTarget )
	{
		float fTargetGap = pTarget->GetWorldPosition().y - pTarget->GetBottomHeight();
		if ( fTargetGap > 0.0f )
		{
			m_TargetPositionType = TPT_JUMP;
			m_vAutoTargetDir = pTarget->GetMidPositionByRate() - pOwner->GetMidPositionByRate();
			D3DXVec3Normalize( &m_vAutoTargetDir, &m_vAutoTargetDir );
		}
		else
		{
			m_TargetPositionType = TPT_NORMAL;
			m_vAutoTargetDir = pTarget->GetMidPositionByRate() - pOwner->GetMidPositionByRate();
			if( GetSkillUseType() == SUT_NORMAL )
				m_vAutoTargetDir.y = 0.0f;
			D3DXVec3Normalize( &m_vAutoTargetDir, &m_vAutoTargetDir );
		}
	}
	else
	{
		if( GetSkillUseType() == SUT_JUMP )
			m_TargetPositionType = TPT_JUMP;
		else
			m_TargetPositionType = TPT_NORMAL;

		m_vAutoTargetDir = pOwner->GetTargetRot() * D3DXVECTOR3( 0.0f, 0.0f, -FLOAT1 );
		D3DXVec3Normalize( &m_vAutoTargetDir, &m_vAutoTargetDir );
	}
}

void ioBuffCheckAttackSkill::OnProcessState( ioBaseChar *pOwner, ioPlayStage *pStage )
{
	switch( m_SkillState )
	{
	case SS_PRE: 
		{
			CheckPreState( pOwner, pStage );
		}
		break;
	case SS_NORMAL_ATTACK:
		{
			if( 0 < m_dwMotionEndTime && m_dwMotionEndTime < FRAMEGETTIME() )
			{
				SetEndState( pOwner );
				break;
			}
		}
		break;
	case SS_JUMP_ATTACK:
		{
			if( 0 < m_dwMotionEndTime && m_dwMotionEndTime < FRAMEGETTIME() )
			{
				SetEndState( pOwner );
				break;
			}
		}
		break;
	}

	TrackingTarget( pOwner );
	CheckReservedSliding( pOwner );
}

void ioBuffCheckAttackSkill::CheckPreState( ioBaseChar *pOwner, ioPlayStage *pStage )
{
	if ( !pOwner || !pStage )
		return;
	DWORD dwCurTime  = FRAMEGETTIME();
	DWORD dwPreDelay = GetPreDelayTime();

	if( m_dwPreDelayStartTime + dwPreDelay < dwCurTime )
	{
		m_dwPreDelayStartTime = 0;
		EndPreDelayEffect( pOwner );

		if( pOwner->IsNeedProcess() )
		{
			if( GetSkillUseType() == SUT_JUMP )
				SetJumpAttackState( pOwner, pStage );
			else
				SetNormalAttackState( pOwner, pStage );
		}
	}
}

void ioBuffCheckAttackSkill::SetNormalAttackState( ioBaseChar *pOwner, ioPlayStage *pStage )
{
	if ( !pOwner || !pStage )
		return;

	CheckNormalBuffPosition( pOwner, pStage );

	m_SkillState = SS_NORMAL_ATTACK;
	AttackSkillFire( pOwner );
	RemoveCheckBuff( pOwner );

	if ( pOwner->IsNeedSendNetwork() )
	{
		SP2Packet kPacket( CUPK_SKILL_EXTRA_INFO );
		kPacket << pOwner->GetCharName();
		kPacket << GetName();
		kPacket << static_cast<int>(m_SkillState);
		kPacket << m_TargetPositionType;
		kPacket << m_iNormalAttackIndex;
		kPacket << m_vAutoTargetDir;
		if ( IsUseAutoTargetValue() )
			pOwner->FillAutoTargetInfo( kPacket );
		P2PNetwork::SendToAllPlayingUser( kPacket );
	}
}

void ioBuffCheckAttackSkill::SetJumpAttackState( ioBaseChar *pOwner, ioPlayStage *pStage )
{
	if ( !pOwner || !pStage )
		return;
	CheckJumpBuffPosition( pOwner, pStage );

	m_SkillState = SS_JUMP_ATTACK;
	AttackSkillFire( pOwner );
	RemoveCheckBuff( pOwner );

	if ( pOwner->IsNeedSendNetwork() )
	{
		SP2Packet kPacket( CUPK_SKILL_EXTRA_INFO );
		kPacket << pOwner->GetCharName();
		kPacket << GetName();
		kPacket << static_cast<int>(m_SkillState);
		kPacket << m_TargetPositionType;
		kPacket << m_iJumpAttackIndex;
		kPacket << m_vAutoTargetDir;
		if ( IsUseAutoTargetValue() )
			pOwner->FillAutoTargetInfo( kPacket );
		P2PNetwork::SendToAllPlayingUser( kPacket );
	}
}

void ioBuffCheckAttackSkill::OnSkillEnd( ioBaseChar *pOwner )
{
	ioCountSkill::OnSkillEnd( pOwner );

	pOwner->SetSkillMoveEnable( false, false );

	if( m_bUseItemMesh )
		pOwner->ShowWeaponItemMesh( true );

	if( !m_HandMesh.IsEmpty() )
		pOwner->RestoreSkillWeaponMesh( this, m_HandMesh, m_bVisibleEquipMesh );

	ClearData();
}

void ioBuffCheckAttackSkill::SetNextSkillState( ioBaseChar *pWounder, ioWeapon *pWeapon /*= NULL */ )
{
	if ( !pWounder || !pWeapon )
		return;

	ioBaseChar *pAttacker = pWeapon->GetOwner();
	if ( !pAttacker )
		return;

	if ( pWounder->IsNeedProcess() )
		AddCheckBuff( pWounder, pAttacker );
}

void ioBuffCheckAttackSkill::AddCheckBuff( ioBaseChar *pWounder, ioBaseChar *pAttacker )
{
	if ( !pWounder || !pAttacker )
		return;
	ioHashString szBuffName;
	switch( m_SkillState )
	{
	case SS_NORMAL_ATTACK:
		{
			if ( m_TargetPositionType == TPT_NORMAL )
			{
				if ( (int)m_vCheckNormalGroundBuffList.size() <= m_iNormalAttackIndex )
					break;

				szBuffName = m_vCheckNormalGroundBuffList[m_iNormalAttackIndex];
			}
			else if ( m_TargetPositionType == TPT_JUMP )
			{
				if ( (int)m_vCheckNormalAirBuffList.size() <= m_iJumpAttackIndex )
					break;

				szBuffName = m_vCheckNormalAirBuffList[m_iJumpAttackIndex];
			}
		}
		break;
	case SS_JUMP_ATTACK:
		{
			if ( m_TargetPositionType == TPT_NORMAL )
			{
				if ( (int)m_vCheckJumpGroundAttackList.size() <= m_iNormalAttackIndex )
					break;

				szBuffName = m_vCheckJumpGroundAttackList[m_iNormalAttackIndex];
			}
			else if ( (int)m_TargetPositionType == TPT_JUMP )
			{
				if ( (int)m_vCheckJumpAirAttackList.size() <= m_iJumpAttackIndex )
					break;

				szBuffName = m_vCheckJumpAirAttackList[m_iJumpAttackIndex];
			}
		}
		break;
	}

	if ( !szBuffName.IsEmpty() )
		pAttacker->AddNewBuff( szBuffName, pAttacker->GetCharName(), GetOwnerItem()->GetName(), this );

	SetEndState( pAttacker);
	if ( pWounder->IsNeedSendNetwork() )
	{
		SP2Packet kPacket( CUPK_SKILL_EXTRA_INFO );
		kPacket << pAttacker->GetCharName();
		kPacket << GetName();
		kPacket << static_cast<int>(SS_END);
		kPacket << szBuffName;
		P2PNetwork::SendToAllPlayingUser( kPacket );
	}
}

AttackAttribute& ioBuffCheckAttackSkill::GetAttribute()
{
	return const_cast<AttackAttribute&>( GetAttributeConst() );
}

const AttackAttribute& ioBuffCheckAttackSkill::GetAttributeConst() const
{
	switch( m_SkillState )
	{
	case SS_NORMAL_ATTACK:
		return GetNormalAttackAttribute();
	case SS_JUMP_ATTACK:
		return GetJumpAttackAttribute();
	}

	return ioCountSkill::GetAttributeConst();
}

const AttackAttribute& ioBuffCheckAttackSkill::GetNormalAttackAttribute() const
{
	if ( m_TargetPositionType == TPT_NORMAL )
	{
		if ( (int)m_vNormalGroundAttackList.size() > m_iNormalAttackIndex )
		{
			return m_vNormalGroundAttackList[m_iNormalAttackIndex];
		}
	}
	else if ( m_TargetPositionType == TPT_JUMP )
	{
		if ( (int)m_vNormalAirAttackList.size() > m_iNormalAttackIndex )
		{
			return m_vNormalAirAttackList[m_iNormalAttackIndex];
		}
	}
	return ioCountSkill::GetAttributeConst();
}

const AttackAttribute& ioBuffCheckAttackSkill::GetJumpAttackAttribute() const
{
	if ( m_TargetPositionType == TPT_NORMAL )
	{
		if ( (int)m_vJumpGroundAttackList.size() > m_iJumpAttackIndex )
		{
			return m_vJumpGroundAttackList[m_iJumpAttackIndex];
		}
	}
	else if ( m_TargetPositionType == TPT_JUMP )
	{
		if ( (int)m_vJumpAirAttackList.size() > m_iJumpAttackIndex )
		{
			return m_vJumpAirAttackList[m_iJumpAttackIndex];
		}
	}
	
	return ioCountSkill::GetAttributeConst();
}

void ioBuffCheckAttackSkill::SetEndState( ioBaseChar *pOwner )
{
	if ( !pOwner )
		return;

	m_SkillState = SS_END;
	SetMotionTime( 0,0 );

	pOwner->StopAutoTargetTracking();
	pOwner->SetCurMoveSpeed( 0.0f );
	pOwner->SetForcePowerAmt( 0.0f );
	pOwner->SetExtraMoveSpeed( 0.0f );
}

bool ioBuffCheckAttackSkill::IsSkillMotionEnd( DWORD dwActionStop, ioBaseChar* pOwner ) const
{
	if ( m_SkillState == SS_END && m_dwMotionEndTime < FRAMEGETTIME() && m_fSkillEndNextJumpAmt == 0.0f )
		return true;

	return false;
}

bool ioBuffCheckAttackSkill::IsEndState() const
{
	return IsAttackEndState();
}

bool ioBuffCheckAttackSkill::IsAttackEndState() const
{
	if( m_SkillState == SS_END && m_dwMotionEndTime < FRAMEGETTIME() )
		return true;

	return false;
}

void ioBuffCheckAttackSkill::AttackSkillFire( ioBaseChar *pChar )
{
	ioEntityGroup *pGrp = pChar->GetGroup();
	int iAniID = pGrp->GetAnimationIdx( GetAttribute().m_AttackAnimation );
	if( iAniID == -1 )	return;

	pChar->AttachEffect( m_SkillEffectName );

	float fTimeRate  = GetAttackTimeRate();
	DWORD dwPreDelay = GetAttribute().m_dwPreDelay;

	if( m_bRefreshFireTimeList )
	{
		pChar->RefreshFireTimeListWithBase( m_dwWeaponBaseIndex,
											iAniID,
											GetAttribute().m_vWeaponInfoList,
											FTT_SKILL_ATTACK,
											fTimeRate,
											dwPreDelay );
	}

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

	if( IsUseAutoTargetValue() )
	{
		m_dwTrackingEndTime = dwCurTime + pGrp->GetAnimationEventTime_e( iAniID, "tracking_e" ) * fTimeRate + dwPreDelay;
		pChar->SetAutoTarget( ATT_SKILL );
		pChar->SetAutoTargetTrackingEndTime( m_dwTrackingEndTime );
	}

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

	SetReservedSliding( pChar, GetAttribute().m_vForceInfoList, iAniID, fTimeRate, dwPreDelay );
}

void ioBuffCheckAttackSkill::SetReservedSliding( ioBaseChar *pOwner, const ForceInfoList &vForceInfoList, int iAniID, float fTimeRate, DWORD dwPreDelay, float fExtraForceRate /*= FLOAT1 */ )
{
	if ( !pOwner || iAniID == -1 )
		return;

	if ( vForceInfoList.empty() )
		return;

	pOwner->ClearReservedSliding();
	ioEntityGroup *pGrp = pOwner->GetGroup();
	if ( !pGrp )
		return;

	std::list<float> vTimeList;
	pGrp->GetAnimationEventTimeList_e( iAniID, "fire_sliding", vTimeList );

	int iListCnt = vForceInfoList.size();
	int iCurList = 0;

	std::list< float >::iterator iter = vTimeList.begin();
	while( iter != vTimeList.end() )
	{
		DWORD dwTime = (DWORD)*iter;
		dwTime *= fTimeRate;
		dwTime += dwPreDelay;

		float fForceAmt = 0.0f;
		float fForceFriction = FLOAT1;
		float fForceAngle = 0.0f;

		if( iListCnt > iCurList )
		{
			fForceAmt = vForceInfoList[iCurList].m_fForceMovePower;
			fForceAmt = g_StatAttribute.GetCalcStatAttribute( fForceAmt, SA_STR_FORCE_AMT_UP, pOwner );
			fForceAmt *= fExtraForceRate;

			ReservedSliding kSliding;
			kSliding.m_dwSlidingTime = dwTime + FRAMEGETTIME();
			kSliding.m_fSlidingForce = fForceAmt;
			kSliding.m_fSlidingFriction = vForceInfoList[iCurList].m_fForceMoveFriction;
			
			iCurList++;

			if ( kSliding.m_fSlidingForce != 0.0f )
				m_vReservedSlidingList.push_back( kSliding );
		}
		else
		{
			iCurList++;
		}

		iter++;
	}
}

void ioBuffCheckAttackSkill::CheckNormalBuffPosition( ioBaseChar *pOwner, ioPlayStage *pStage )
{
	if ( !pOwner || !pStage )
		return;
	
	m_iNormalAttackIndex = 0;

	if ( m_TargetPositionType == TPT_NORMAL )
	{
		for ( int i=0 ; i<(int)m_vCheckNormalGroundBuffList.size() ; ++i )
		{
			if( pOwner->HasBuff( m_vCheckNormalGroundBuffList[i] ) )
			{
				m_iNormalAttackIndex = i + 1;
				break;
			}
		}
	}
	else if ( m_TargetPositionType == TPT_JUMP )
	{
		for ( int i=0 ; i<(int)m_vCheckNormalAirBuffList.size() ; ++i )
		{
			if( pOwner->HasBuff( m_vCheckNormalAirBuffList[i] ) )
			{
				m_iNormalAttackIndex = i + 1;
				break;
			}
		}
	}
	
}

void ioBuffCheckAttackSkill::CheckJumpBuffPosition( ioBaseChar *pOwner, ioPlayStage *pStage )
{
	if ( !pOwner || !pStage )
		return;
	m_iJumpAttackIndex = 0;
	if ( m_TargetPositionType == TPT_NORMAL )
	{
		for ( int i=0 ; i<(int)m_vCheckJumpGroundAttackList.size() ; ++i )
		{
			if( pOwner->HasBuff( m_vCheckJumpGroundAttackList[i] ) )
			{
				m_iJumpAttackIndex = i + 1;
				break;
			}
		}
	}
	else if ( m_TargetPositionType == TPT_JUMP )
	{
		for ( int i=0 ; i<(int)m_vCheckJumpAirAttackList.size() ; ++i )
		{
			if( pOwner->HasBuff( m_vCheckJumpAirAttackList[i] ) )
			{
				m_iJumpAttackIndex = i + 1;
				break;
			}
		}
	}
}

void ioBuffCheckAttackSkill::RemoveCheckBuff( ioBaseChar *pOwner )
{
	int nBuffCnt = (int)m_vCheckNormalGroundBuffList.size();
	for ( int i=0; i<nBuffCnt; i++ )
		pOwner->RemoveBuff( m_vCheckNormalGroundBuffList[i] );

	nBuffCnt = (int)m_vCheckNormalAirBuffList.size();
	for ( int i=0; i<nBuffCnt; i++ )
		pOwner->RemoveBuff( m_vCheckNormalAirBuffList[i] );

	nBuffCnt = (int)m_vCheckJumpGroundAttackList.size();
	for ( int i=0; i<nBuffCnt; i++ )
		pOwner->RemoveBuff( m_vCheckJumpGroundAttackList[i] );

	nBuffCnt = (int)m_vCheckJumpAirAttackList.size();
	for ( int i=0; i<nBuffCnt; i++ )
		pOwner->RemoveBuff( m_vCheckJumpAirAttackList[i] );
}

void ioBuffCheckAttackSkill::ApplyExtraSkillInfo( ioBaseChar *pChar, ioPlayStage *pStage, SP2Packet &rkPacket )
{
	if( !pChar || !pStage ) 
		return;

	int nState;
	rkPacket >> nState;
	switch( nState )
	{
	case SS_NORMAL_ATTACK:
		{
			int iPositionType;
			rkPacket >> iPositionType;
			m_TargetPositionType = (TargetPositionType)iPositionType;
			rkPacket >> m_iNormalAttackIndex;
			rkPacket >> m_vAutoTargetDir;
			if ( IsUseAutoTargetValue() )
				pChar->ApplyAutoTargetInfo( rkPacket );
			m_SkillState = SS_NORMAL_ATTACK;
			AttackSkillFire( pChar );
			RemoveCheckBuff( pChar );
		}
		break;
	case SS_JUMP_ATTACK:
		{
			int iPositionType;
			rkPacket >> iPositionType;
			m_TargetPositionType = (TargetPositionType)iPositionType;
			rkPacket >> m_iJumpAttackIndex;
			rkPacket >> m_vAutoTargetDir;
			if ( IsUseAutoTargetValue() )
				pChar->ApplyAutoTargetInfo( rkPacket );

			m_SkillState = SS_JUMP_ATTACK;
			AttackSkillFire( pChar );
			RemoveCheckBuff( pChar );
		}
		break;
	case SS_END:
		{
			ioHashString szBuffName;
			rkPacket >> szBuffName;
			if ( !szBuffName.IsEmpty() )
				pChar->AddNewBuff( szBuffName, pChar->GetCharName(), GetOwnerItem()->GetName(), this );
			if ( m_SkillState != SS_END )
				SetEndState( pChar );
		}
		break;
	}
}

void ioBuffCheckAttackSkill::CheckReservedSliding( ioBaseChar *pOwner )
{
	if( !pOwner || m_vReservedSlidingList.empty() ) 
		return;

	ReservedSlidingList::iterator iter = m_vReservedSlidingList.begin();
	while( iter != m_vReservedSlidingList.end() )
	{
		ReservedSliding kSliding = (ReservedSliding)*iter;

		if( kSliding.m_dwSlidingTime > FRAMEGETTIME() )
			break;

		pOwner->SetForcePower( m_vAutoTargetDir, kSliding.m_fSlidingForce, kSliding.m_fSlidingFriction );
		iter = m_vReservedSlidingList.erase( m_vReservedSlidingList.begin() );
	}
}

void ioBuffCheckAttackSkill::TrackingTarget( ioBaseChar *pOwner )
{
	if ( !pOwner )
		return;
	if ( m_dwTrackingEndTime > FRAMEGETTIME() )
	{
		//AutoTarget Direction
		ioBaseChar* pTarget = pOwner->GetBaseChar( pOwner->GetAutoTargetName() );
		if( pTarget )
		{
			m_vAutoTargetDir = pTarget->GetMidPositionByRate() - pOwner->GetMidPositionByRate();
			if( GetSkillUseType() == SUT_NORMAL &&  m_TargetPositionType == TPT_NORMAL )
				m_vAutoTargetDir.y = 0.0f;
			D3DXVec3Normalize( &m_vAutoTargetDir, &m_vAutoTargetDir );
		}
		else
		{
			m_vAutoTargetDir = pOwner->GetTargetRot() * D3DXVECTOR3( 0.0f, 0.0f, -FLOAT1 );
			D3DXVec3Normalize( &m_vAutoTargetDir, &m_vAutoTargetDir );
		}
	}
}

bool ioBuffCheckAttackSkill::IsProtected( int iDefenseBreakType ) const
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
	return false;
}