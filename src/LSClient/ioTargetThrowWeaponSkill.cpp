
#include "stdafx.h"

#include "ioBaseChar.h"
#include "ioPlayStage.h"
#include "ioGrowthUpInfo.h"

#include "ioTargetThrowWeaponSkill.h"

ioTargetThrowWeaponSkill::ioTargetThrowWeaponSkill()
{
	Init();
}

ioTargetThrowWeaponSkill::ioTargetThrowWeaponSkill( const ioTargetThrowWeaponSkill &rhs )
	: ioAttackSkill( rhs ),
	m_dwSkillProtectTime( rhs.m_dwSkillProtectTime ),
	m_szResultMarker( rhs.m_szResultMarker ),
	m_nMaxAttackCnt( rhs.m_nMaxAttackCnt ),
	m_vAttackList( rhs.m_vAttackList ),
	m_fWeaponTargetFloatPower( rhs.m_fWeaponTargetFloatPower ),
	m_fWeaponTargetFloatMinPower( rhs.m_fWeaponTargetFloatMinPower ),
	m_fWeaponDefaultFloatPower( rhs.m_fWeaponDefaultFloatPower ),
	m_fWeaponDefaultFloatMinPower( rhs.m_fWeaponDefaultFloatMinPower ),
	m_fTargetRange( rhs.m_fTargetRange ),
	m_nTargetMaxCnt( rhs.m_nTargetMaxCnt ),
	m_DefaultList( rhs.m_DefaultList )
{
	Init();
}

ioTargetThrowWeaponSkill::~ioTargetThrowWeaponSkill()
{
}

ioSkill* ioTargetThrowWeaponSkill::Clone()
{
	return new ioTargetThrowWeaponSkill( *this );
}

void ioTargetThrowWeaponSkill::LoadProperty( ioINILoader &rkLoader )
{
	ioAttackSkill::LoadProperty( rkLoader );

	char szBuf[MAX_PATH], szKey[MAX_PATH];

	m_dwSkillProtectTime = (DWORD)rkLoader.LoadInt_e( "skill_protect_time", 0 );

	rkLoader.LoadString_e( "result_target_marker", "", szBuf, MAX_PATH );
	m_szResultMarker = szBuf;

	m_vAttackList.clear();
	m_nMaxAttackCnt = rkLoader.LoadInt_e( "max_attack_cnt", 0 );
	for ( int i=0; i<m_nMaxAttackCnt; i++ )
	{
		wsprintf_e( szKey, "attack%d_info", i+1 );

		AttackAttribute sAttackInfo;
		LoadAttackAttribute( szKey, sAttackInfo, rkLoader );
		m_vAttackList.push_back( sAttackInfo );
	}

	m_fWeaponTargetFloatPower = rkLoader.LoadFloat_e( "weapon_target_float_power", 0.0f );
	m_fWeaponTargetFloatMinPower = rkLoader.LoadFloat_e( "weapon_target_float_min_power", 0.0f );

	m_fWeaponDefaultFloatPower = rkLoader.LoadFloat_e( "weapon_default_float_power", 0.0f );
	m_fWeaponDefaultFloatMinPower = rkLoader.LoadFloat_e( "weapon_default_float_min_power", 0.0f );

	m_fTargetRange = rkLoader.LoadFloat_e( "target_range", 0.0f );
	m_nTargetMaxCnt = rkLoader.LoadInt_e( "target_max_cnt", 0 );

	int nMaxDefaultCnt = rkLoader.LoadInt_e( "max_default_cnt", 0 );
	for ( int i=0; i<nMaxDefaultCnt; i++ )
	{
		D3DXVECTOR3 vTargetPos;
		wsprintf_e( szBuf, "default%d_x", i+1 );
		vTargetPos.x = rkLoader.LoadFloat( szBuf, 0.0f );

		wsprintf_e( szBuf, "default%d_y", i+1 );
		vTargetPos.y = rkLoader.LoadFloat( szBuf, 0.0f );

		wsprintf_e( szBuf, "default%d_z", i+1 );
		vTargetPos.z = rkLoader.LoadFloat( szBuf, 0.0f );

		m_DefaultList.push_back( vTargetPos );
	}
}

bool ioTargetThrowWeaponSkill::OnSkillStart( ioBaseChar *pChar )
{
	Init();

	if( m_dwSkillProtectTime > 0 )
		m_dwCurSkillProtectTime = FRAMEGETTIME() + m_dwSkillProtectTime;

	if( !ioAttackSkill::OnSkillStart( pChar ) )
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

	return true;
}

void ioTargetThrowWeaponSkill::OnProcessState( ioBaseChar *pChar, ioPlayStage *pStage )
{
	DWORD dwCurTime = FRAMEGETTIME();

	switch( m_SkillState )
	{
	case SS_None:
		{
			DWORD dwPreDelay = GetPreDelayTime();
			if( m_dwPreDelayStartTime > 0 && m_dwPreDelayStartTime + dwPreDelay < dwCurTime )
			{
				m_dwPreDelayStartTime = 0;
				EndPreDelayEffect( pChar );

				if ( pChar->IsNeedProcess() )
				{
					if (COMPARE( m_nCurAttackCnt, 0, m_nMaxAttackCnt ) )
						SetAttackState( pChar, pStage, m_vAttackList[m_nCurAttackCnt] );
					else
						m_SkillState = SS_End;
				}
			}
		}
		break;
	case SS_Action:
		{
			if ( pChar->IsNeedProcess() )
			{
				if ( m_dwMotionEndTime < dwCurTime )
				{
					if (COMPARE( m_nCurAttackCnt, 0, m_nMaxAttackCnt ) )
						SetAttackState( pChar, pStage, m_vAttackList[m_nCurAttackCnt] );
					else
						m_SkillState = SS_End;
				}
				else
				{
					if ( m_dwFireStart > 0 && m_dwFireStart < dwCurTime )
					{
						m_dwFireStart = 0;
						CreateWeapon( pChar, pStage );
					}
				}
			}
		}
		break;
	}
}

void ioTargetThrowWeaponSkill::OnSkillEnd( ioBaseChar *pChar )
{
	ioAttackSkill::OnSkillEnd( pChar );

	EndCameraBuff( pChar );

	// 기본무기 장착
	if( m_bUseItemMesh )
		pChar->ShowWeaponItemMesh( true );

	if( !m_HandMesh.IsEmpty() )
		pChar->RestoreSkillWeaponMesh( this, m_HandMesh, m_bVisibleEquipMesh );

	m_bSetHandMesh = false;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////

bool ioTargetThrowWeaponSkill::IsProtected( int iDefenseBreakType ) const
{
	if( m_bDisableSkillProtect )
		return false;

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

	if( m_dwPreDelayStartTime == 0 )
	{		
		if( m_dwCurSkillProtectTime > dwCurTime )
			return true;
	}

	return false;
}

bool ioTargetThrowWeaponSkill::IsSkillMotionEnd( DWORD dwActionStop, ioBaseChar* pChar ) const
{
	if( m_dwPreDelayStartTime > 0 )
		return false;

	switch( m_SkillState )
	{
	case SS_None:
	case SS_Action:
		return false;
	case SS_End:
		if( m_fSkillEndNextJumpAmt == 0.0f )
			return true;
		else
			return false;
	}

	if( m_dwMotionEndTime + dwActionStop >= FRAMEGETTIME() )
		return false;

	return true;
}

void ioTargetThrowWeaponSkill::ApplyExtraSkillInfo( ioBaseChar *pChar, ioPlayStage *pStage, SP2Packet &rkPacket )
{
	if( !pChar || !pStage ) 
		return;

	int nState;
	rkPacket >> nState;

	switch( nState )
	{
	case SS_Action:
		{
			D3DXVECTOR3 vPos;
			rkPacket >> vPos;

			pChar->SetWorldPosition( vPos );

			D3DXQUATERNION qtRot;
			rkPacket >> qtRot;

			pChar->SetTargetRotAndMoveDirChange( qtRot );

			rkPacket >> m_nCurAttackCnt;

			if (COMPARE( m_nCurAttackCnt, 0, m_nMaxAttackCnt ) )
				SetAttackState( pChar, pStage, m_vAttackList[m_nCurAttackCnt] );
		}
		break;
	case SS_Create_Weapon:
		{
			rkPacket >> m_nCurAttackCnt;

			DWORD dwWeaponBaseIndex;
			rkPacket >> dwWeaponBaseIndex;

			bool bTarget;
			rkPacket >> bTarget;

			int nWeaponCnt;
			rkPacket >> nWeaponCnt;

			pChar->SetWeaponIndexBase( dwWeaponBaseIndex );
			pChar->IncreaseWeaponIndexBase();

			if ( !COMPARE( m_nCurAttackCnt, 0, m_nMaxAttackCnt ) )
				return;

			DWORD dwWeaponIndex = dwWeaponBaseIndex;
			const AttackAttribute &sAttribute = m_vAttackList[m_nCurAttackCnt];

			FireTime kFireTime;
			kFireTime.dwStart = FRAMEGETTIME();

			kFireTime.iAttributeIdx = sAttribute.m_vWeaponInfoList[0].m_iWeaponIdx;
			kFireTime.iResistanceIdx = sAttribute.m_vWeaponInfoList[0].m_iResistanceIdx;
			kFireTime.szWoundedAni  = sAttribute.m_vWeaponInfoList[0].m_WoundedAnimation;
			kFireTime.dwWoundedAniDuration = sAttribute.m_vWeaponInfoList[0].m_dwWoundedDuration;
			kFireTime.bLoopWoundedAni = sAttribute.m_vWeaponInfoList[0].m_bWoundedLoopAni;
			kFireTime.eFireTimeType = FTT_SKILL_ATTACK;
			
			for ( int i=0; i<nWeaponCnt; i++ )
			{
				kFireTime.dwWeaponIdx = dwWeaponIndex++;

				D3DXVECTOR3 vPos = pChar->GetMidPositionByRate();
				D3DXVECTOR3 vDir = pChar->GetWorldOrientation() * D3DXVECTOR3( 0, 0, -1 );

				ioWeapon *pWeapon = pChar->ExtendFireExplicit( kFireTime, vPos, vDir, "" );	
				if ( !pWeapon )
					continue;
				
				D3DXVECTOR3 vTargetPos;
				rkPacket >> vTargetPos;

				if ( bTarget )
					SetWeaponFloatValue( pChar, pStage, pWeapon, vTargetPos, bTarget );
				else
					SetWeaponFloatValue( pChar, pStage, pWeapon, vPos + pChar->GetWorldOrientation() * vTargetPos, bTarget );
			}

			m_nCurAttackCnt++;
		}
		break;
	}
}

////////////////////////////////////////////////////////////////////////////////////////////////////////

void ioTargetThrowWeaponSkill::Init()
{
	m_dwCurSkillProtectTime = 0;
	m_bSetHandMesh = false;

	m_SkillState = SS_None;
	m_nCurAttackCnt = 0;
	m_dwFireStart = 0;
}

void ioTargetThrowWeaponSkill::SetAttackState( ioBaseChar *pChar, ioPlayStage *pStage, const AttackAttribute &sAttribute )
{
	if( !pChar || !pStage )	
		return;

	ioEntityGroup *pGrp = pChar->GetGroup();
	int nAniID = pGrp->GetAnimationIdx( sAttribute.m_AttackAnimation );
	if( nAniID == -1 )	
		return;

	float fTimeRate  = sAttribute.m_fAttackAniRate;
	DWORD dwPreDelay = sAttribute.m_dwPreDelay;

	pGrp->ClearAllLoopAni( FLOAT100, true );
	pGrp->ClearAllActionAni( FLOAT100, true );

	pGrp->SetActionAni( nAniID, FLOAT100, FLOAT100, FLOAT1, FLOAT1/fTimeRate, 0.0f, dwPreDelay );

	pChar->ClearReservedSliding();
	pChar->SetReservedSliding( sAttribute.m_vForceInfoList, nAniID, fTimeRate, dwPreDelay );

	DWORD dwCurTime = FRAMEGETTIME();
	m_dwMotionEndTime = dwCurTime;
	m_dwMotionEndTime += pGrp->GetAnimationFullTime( nAniID ) * fTimeRate;
	m_dwMotionEndTime += dwPreDelay;

	m_dwFireStart = dwCurTime;
	m_dwFireStart += pGrp->GetAnimationEventTime_e( nAniID, "fire_s" ) * fTimeRate;
	m_dwFireStart += dwPreDelay;

	m_SkillState = SS_Action;

	if ( pChar->IsNeedSendNetwork() )
	{
		SP2Packet kPacket( CUPK_SKILL_EXTRA_INFO );
		kPacket << pChar->GetCharName();
		kPacket << GetName();
		kPacket << SS_Action;
		kPacket << pChar->GetWorldPosition();
		kPacket << pChar->GetWorldOrientation();
		kPacket << m_nCurAttackCnt;
		P2PNetwork::SendToAllPlayingUser( kPacket );
	}
}

void ioTargetThrowWeaponSkill::SetEndState( ioBaseChar *pChar )
{
	if ( !pChar )
		return;

	m_SkillState = SS_End;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////

DWORD ioTargetThrowWeaponSkill::CreateEffect( ioPlayStage *pStage, ioHashString& szEffect, D3DXVECTOR3& vPos, D3DXVECTOR3& vScale )
{
	if ( !pStage || szEffect.IsEmpty() )
		return -1;

	ioMapEffect *pMaker = pStage->CreateMapEffect( szEffect, vPos, vScale );
	if ( pMaker )
		return pMaker->GetUniqueID();

	return -1;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////

void ioTargetThrowWeaponSkill::CreateWeapon( ioBaseChar *pChar, ioPlayStage *pStage )
{	
	if ( !pChar || !pChar->IsNeedProcess() || !pStage )
		return;

	if ( !COMPARE( m_nCurAttackCnt, 0, m_nMaxAttackCnt ) )
		return;

	const AttackAttribute &sAttribute = m_vAttackList[m_nCurAttackCnt];

	GetTargetList( pChar, pStage );

	DWORD dwWeaponBaseIndex = pChar->IncreaseWeaponIndexBase();
	DWORD dwWeaponIndex = dwWeaponBaseIndex;

	FireTime kFireTime;
	kFireTime.dwStart = FRAMEGETTIME();

	kFireTime.iAttributeIdx = sAttribute.m_vWeaponInfoList[0].m_iWeaponIdx;
	kFireTime.iResistanceIdx = sAttribute.m_vWeaponInfoList[0].m_iResistanceIdx;
	kFireTime.szWoundedAni  = sAttribute.m_vWeaponInfoList[0].m_WoundedAnimation;
	kFireTime.dwWoundedAniDuration = sAttribute.m_vWeaponInfoList[0].m_dwWoundedDuration;
	kFireTime.bLoopWoundedAni = sAttribute.m_vWeaponInfoList[0].m_bWoundedLoopAni;
	kFireTime.eFireTimeType = FTT_SKILL_ATTACK;

	bool bTarget = false;
	Vector3Vec vTargetList = m_DefaultList;
	if ( !m_TargetList.empty() )
	{
		bTarget = true;
		vTargetList = m_TargetList;
	}

	for ( int i=0; i<(int)vTargetList.size(); i++ )
	{
		kFireTime.dwWeaponIdx = dwWeaponIndex++;

		D3DXVECTOR3 vPos = pChar->GetMidPositionByRate();
		D3DXVECTOR3 vDir = pChar->GetWorldOrientation() * D3DXVECTOR3( 0, 0, -1 );

		ioWeapon *pWeapon = pChar->ExtendFireExplicit( kFireTime, vPos, vDir, "" );	
		if ( !pWeapon )
			continue;

		D3DXVECTOR3 vTargetPos = vPos + pChar->GetWorldOrientation() * vTargetList[i];
		if ( bTarget )
			vTargetPos = vTargetList[i];

		SetWeaponFloatValue( pChar, pStage, pWeapon, vTargetPos, bTarget );
	}

	if ( pChar->IsNeedSendNetwork() )
	{
		SP2Packet kPacket( CUPK_SKILL_EXTRA_INFO );
		kPacket << pChar->GetCharName();
		kPacket << GetName();
		kPacket << SS_Create_Weapon;
		kPacket << m_nCurAttackCnt;
		kPacket << dwWeaponBaseIndex;
		kPacket << bTarget;
		kPacket << (int)vTargetList.size();
		for ( int i=0; i<(int)vTargetList.size(); i++ )
			kPacket << vTargetList[i];
		P2PNetwork::SendToAllPlayingUser( kPacket );
	}

	m_nCurAttackCnt++;
}

bool ioTargetThrowWeaponSkill::GetTargetList( ioBaseChar *pChar, ioPlayStage *pStage )
{
	if ( !pChar || !pChar->IsNeedProcess() || !pStage )
		return false;

	m_TargetList.clear();

	BaseCharDistanceSqList vTargetInfoList;
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
		
		if( !pTarget->IsEnableTarget( true, false ) )
			continue;

		if( pTarget->IsSystemState() )
			continue;

		D3DXVECTOR3 vDiff = pTarget->GetWorldPosition() - pChar->GetWorldPosition();
		float fDistSq = D3DXVec3LengthSq( &vDiff );
		float fEnableDistSq = m_fTargetRange * m_fTargetRange;

		if( fDistSq > fEnableDistSq )
			continue;

		tagBaseCharDistanceSq kInfo;
		kInfo.m_Name        = pTarget->GetCharName();
		kInfo.m_fDistanceSq = fDistSq;
		kInfo.m_vPos		= pTarget->GetWorldPosition();

		vTargetInfoList.push_back( kInfo );
	}

	if ( vTargetInfoList.empty() )
		return false;

	std::sort( vTargetInfoList.begin(), vTargetInfoList.end(), BaseCharDistanceSqSort2() );

	for ( int i=0; i<m_nTargetMaxCnt; i++ )
	{
		if ( COMPARE( i, 0, (int)vTargetInfoList.size() ) )
			m_TargetList.push_back( vTargetInfoList[i].m_vPos );
	}

	return true;
}

void ioTargetThrowWeaponSkill::SetWeaponFloatValue( ioBaseChar *pChar, ioPlayStage *pStage, ioWeapon *pWeapon, D3DXVECTOR3 vTargetPos, bool bTarget )
{
	if ( !pChar || !pStage || !pWeapon )
		return;

	vTargetPos.y = pStage->GetMapHeightNoneEntity( vTargetPos.x, vTargetPos.z );
	CreateEffect( pStage, m_szResultMarker, vTargetPos, D3DXVECTOR3( FLOAT1, FLOAT1, FLOAT1 ) );

	D3DXVECTOR3 vDir = vTargetPos - pChar->GetWorldPosition();
	vDir.y = 0.0f;

	float fRange = D3DXVec3Length( &vDir );
	D3DXVec3Normalize( &vDir, &vDir );

	D3DXVECTOR3 vCurWeaponMoveDir = vDir;
	float fCurWeaponFloatPower = 0.0f;
	float fCurWeaponMoveSpeed = 0.0f;

	float fWeaponFloatPower = m_fWeaponDefaultFloatPower;
	float fWeaponFloatMinPower = m_fWeaponDefaultFloatMinPower;

	if ( bTarget )
	{
		fWeaponFloatPower = m_fWeaponTargetFloatPower;
		fWeaponFloatMinPower = m_fWeaponTargetFloatMinPower;
	}

	if( fRange > 0.0f )
	{
		float fRate = fRange / m_fTargetRange;

		fCurWeaponFloatPower = fWeaponFloatPower * fRate;
		fCurWeaponFloatPower = max( fCurWeaponFloatPower, fWeaponFloatMinPower );

		float fWeaponMoveSpeed = 0.0f;
		float fFlightDuration = GetFlightDuration( pStage->GetRoomGravity(), fCurWeaponFloatPower );
		if( fFlightDuration > 0.0f )
			fWeaponMoveSpeed = m_fTargetRange / (fFlightDuration / FLOAT1000);

		fCurWeaponMoveSpeed = fWeaponMoveSpeed * fRate;
	}

	pWeapon->SetMoveDir( vCurWeaponMoveDir );
	pWeapon->SetFloatPower( fCurWeaponFloatPower );
	pWeapon->SetMoveSpeed( fCurWeaponMoveSpeed );
}

float ioTargetThrowWeaponSkill::GetFlightDuration( float fCurGravity, float fCurWeaponFloatPower )
{
	float fTopTime = 0.0f; 
	float fTopHeight = 0.0f;

	float fNewGravity = fCurGravity * m_fSkillGravityRate;
	if( fNewGravity > 0.0f )
	{
		fTopTime = fCurWeaponFloatPower / fNewGravity;
		fTopHeight = (fCurWeaponFloatPower*fTopTime) - (0.5f*fNewGravity*fTopTime*fTopTime);
	}
	return 2 * fTopTime * FLOAT1000;
}