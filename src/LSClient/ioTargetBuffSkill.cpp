
#include "stdafx.h"

#include "ioPlayStage.h"
#include "ioBaseChar.h"

#include "ioTargetBuffSkill.h"

#include "BuffDefine.h"

ioTargetBuffSkill::ioTargetBuffSkill()
{
	ClearData();
}

ioTargetBuffSkill::ioTargetBuffSkill( const ioTargetBuffSkill &rhs )
	: ioBuffSkill( rhs ),
	m_SkillLoopMotion( rhs.m_SkillLoopMotion ),
	m_fSkillLoopAniRate( rhs.m_fSkillLoopAniRate ),
	m_dwLoopDuration( rhs.m_dwLoopDuration ),
	m_fTargetAngle( rhs.m_fTargetAngle ),
	m_fTargetRange( rhs.m_fTargetRange ),
	m_TargetWoundType( rhs.m_TargetWoundType ),
	m_szAimMark( rhs.m_szAimMark )
{
	ClearData();
}

ioTargetBuffSkill::~ioTargetBuffSkill()
{
	ClearData();
}

void ioTargetBuffSkill::ClearData()
{
	m_bSetHandMesh = false;

	m_SkillState = SS_NONE;
	m_dwLoopStartTime	= 0;

	m_dwAimMarkID	= -1;

	m_dwPreDelayStartTime = 0;
	m_szTargetName.Clear();

	m_dwFireStartTime = 0;
}

void ioTargetBuffSkill::LoadProperty( ioINILoader &rkLoader )
{
	ioBuffSkill::LoadProperty( rkLoader );

	char szBuf[MAX_PATH] = "";

	rkLoader.LoadString_e( "skill_loop_motion", "", szBuf, MAX_PATH );
	m_SkillLoopMotion = szBuf;
	m_fSkillLoopAniRate = rkLoader.LoadFloat_e( "skill_loop_ani_rate", FLOAT1 );
	m_dwLoopDuration = rkLoader.LoadInt_e( "skill_loop_duration", 0 );

	// Limit Angle
	m_fTargetAngle = rkLoader.LoadFloat_e( "target_angle", 0.0f );
	m_fTargetRange = rkLoader.LoadFloat_e( "target_range", 0.0f );
	
	m_TargetWoundType = (TargetWoundType)rkLoader.LoadInt_e( "target_wound_type", TWT_ALL );

	rkLoader.LoadString_e( "aim_mark_effect","", szBuf, MAX_PATH );
	m_szAimMark = szBuf;
}

ioSkill* ioTargetBuffSkill::Clone()
{
	return new ioTargetBuffSkill( *this );
}

bool ioTargetBuffSkill::OnSkillStart( ioBaseChar *pChar )
{
	if( !CheckEnableStart( pChar ) )
		return false;

	if( !HasSkillMotion() )
		return false;

	ClearData();

	pChar->SetSkillProtection();

	m_bReduceGauge = false;

	InitExtraAniJump();
	
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
	
	pChar->AttachEffect( m_SkillEffectName );

	CheckSkillDesc( pChar );

	if( !m_bReduceGauge )
	{
		pChar->ReduceNeedGaugeBySkill( this, GetNeedGauge(pChar->GetCharName()) );
		m_bReduceGauge = true;
	}

	return true;
}

void ioTargetBuffSkill::OnProcessState( ioBaseChar *pChar, ioPlayStage *pStage )
{
	DWORD dwCurTime = FRAMEGETTIME();

	switch( m_SkillState )
	{
	case SS_NONE:
		{
			DWORD dwPreDelay = GetPreDelayTime();
			if( m_dwPreDelayStartTime != 0 && m_dwPreDelayStartTime + dwPreDelay < dwCurTime )
			{
				EndPreDelayEffect( pChar );
				SetLoopState( pChar, pStage );

				m_dwPreDelayStartTime = 0;
			}
		}
		break;
	case SS_LOOP:
		{
			if( pChar->IsNeedProcess() )
			{
				CheckKeyInput( pChar );
				CheckTarget( pChar, pStage );
				UpdateAimMark( pChar, pStage );

				if( m_dwLoopStartTime > 0 && m_dwLoopStartTime+m_dwLoopDuration < dwCurTime )
				{
					SetFireState( pChar );
					return;
				}
				else if( pChar->IsAttackKey() )
				{
					SetFireState( pChar );
					return;
				}
			}
		}
		break;
	case SS_FIRE:
		{
			if( m_dwFireStartTime > 0 && m_dwFireStartTime < FRAMEGETTIME() )
			{
				m_dwFireStartTime = 0;

				SetOwnerBuff( pChar );
				SetEnemyBuff( pChar );
			}
		}
		break;
	}
}

void ioTargetBuffSkill::OnSkillEnd( ioBaseChar *pChar )
{
	ioSkill::OnSkillEnd( pChar );
	// 기본무기 장착
	if( m_bUseItemMesh )
		pChar->ShowWeaponItemMesh( true );

	if( m_bSetHandMesh && !m_HandMesh.IsEmpty() )
	{
		pChar->RestoreSkillWeaponMesh( this, m_HandMesh, m_bVisibleEquipMesh );
		m_bSetHandMesh = false;
	}

	pChar->EndEffect( m_SkillEffectName, false );

	EndCameraBuff( pChar );
	DestroyAimMark( pChar );

	ClearData();
}

bool ioTargetBuffSkill::IsSkillMotionEnd( DWORD dwActionStop, ioBaseChar* pOwner ) const
{
	if( m_dwPreDelayStartTime > 0 )
		return false;

	switch( m_SkillState )
	{
	case SS_LOOP:
		return false;
	case SS_FIRE:
		if( m_dwMotionEndTime + dwActionStop > FRAMEGETTIME() )
			return false;

		return true;
	}

	return true;
}

bool ioTargetBuffSkill::IsEnableReserve() const
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

void ioTargetBuffSkill::ApplyExtraSkillInfo( ioBaseChar *pChar, ioPlayStage *pStage, SP2Packet &rkPacket )
{
	rkPacket >> m_szTargetName;
	SetFireState( pChar );
}

void ioTargetBuffSkill::SetLoopState( ioBaseChar *pChar, ioPlayStage *pStage )
{
	if( !pChar ) return;
	ioEntityGroup *pGrp = pChar->GetGroup();
	if( !pGrp ) return;

	DWORD dwCurTime = FRAMEGETTIME();
	m_dwLoopStartTime = dwCurTime;

	m_SkillState = SS_LOOP;

	pGrp->ClearAllActionAni( FLOAT100 );
	pGrp->ClearAllLoopAni( FLOAT100 );

	pGrp->SetLoopAni( m_SkillLoopMotion, FLOAT100, FLOAT1, FLOAT1/m_fSkillLoopAniRate );
}

void ioTargetBuffSkill::SetFireState( ioBaseChar *pChar )
{
	// 공격키 누른후 오는 곳
	if( !pChar ) return;

	DestroyAimMark( pChar );

	m_SkillState = SS_FIRE;
		
	ioEntityGroup *pGrp = pChar->GetGroup();
	if( !pGrp ) return;

	int iAniId = pGrp->GetAnimationIdx( GetSkillMotionName() );
	if( iAniId == -1 )
		return;

	DWORD dwCurTime = FRAMEGETTIME();

	pGrp->ClearAllActionAni( FLOAT100 );
	pGrp->ClearAllLoopAni( FLOAT100 );

	pGrp->SetActionAni( iAniId, FLOAT100, FLOAT100, FLOAT1, FLOAT1 / GetSkillMotionRate() );

	m_dwMotionEndTime = dwCurTime + pGrp->GetAnimationFullTime( iAniId ) * GetSkillMotionRate();
	m_dwFireStartTime = dwCurTime + pGrp->GetAnimationEventTime_e( iAniId, "fire_s" ) * GetSkillMotionRate();
	m_dwFireMotionEndTime = dwCurTime + pGrp->GetAnimationEventTime_e( iAniId, "firemotion_e" ) * GetSkillMotionRate();

	if( pChar->IsNeedSendNetwork() )
	{
		SP2Packet kPacket( CUPK_SKILL_EXTRA_INFO );
		kPacket << pChar->GetCharName();
		kPacket << GetName();
		kPacket << m_szTargetName;
		P2PNetwork::SendToAllPlayingUser( kPacket );
	}
}

bool ioTargetBuffSkill::CheckUseItemMesh()
{
	return m_bUseItemMesh;
}

bool ioTargetBuffSkill::IsProtected( int iDefenseBreakType ) const
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

	return false;
}

bool ioTargetBuffSkill::CheckExceptionCamera()
{
	if( !m_szTargetName.IsEmpty() )
		return true;

	return false;
}

D3DXVECTOR3 ioTargetBuffSkill::GetExceptionCameraPos( ioBaseChar *pChar )
{
	if( !pChar )
		return ioMath::VEC3_ZERO;

	ioBaseChar *pTarget = pChar->GetBaseChar( m_szTargetName );
	if( pTarget )
	{
		return pTarget->GetWorldPosition();
	}

	return pChar->GetWorldPosition();
}

void ioTargetBuffSkill::CheckKeyInput( ioBaseChar *pOwner )
{
	if( !pOwner ) return;

	if( pOwner->IsJumpKey() )
	{
		m_bJumpKey = true;
		m_bDefenseKey = false;
	}
	else if( pOwner->IsDefenseKey() )
	{
		m_bJumpKey = false;
		m_bDefenseKey = true;
	}
	else
	{
		m_bJumpKey = false;
		m_bDefenseKey = false;
	}
}

void ioTargetBuffSkill::CheckTarget( ioBaseChar *pChar, ioPlayStage *pStage )
{
	if( !pStage )	return;
	if( !pChar )	return;

	float fPrevDistSq = m_fTargetRange * m_fTargetRange;
	
	const BaseCharList &rkTargetList = pStage->GetBaseCharList();
	BaseCharDistanceSqList	vTargetInfoList;
	vTargetInfoList.clear();

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

		if( pTarget->IsSystemState() )
			continue;
			
		if( !pTarget->IsEnableTarget() )
			continue;

		if( !pTarget->CheckTargetWoundType( m_TargetWoundType ) )
			continue;

		D3DXVECTOR3 vDiff = pTarget->GetWorldPosition() - pChar->GetWorldPosition();
		float fDistSq = D3DXVec3LengthSq( &vDiff );

		if( fDistSq < fPrevDistSq )
		{
			D3DXVECTOR3 vDir = vDiff;
			D3DXVec3Normalize( &vDir, &vDir );

			D3DXVECTOR3 vSearchDir = pChar->GetTargetRot() * D3DXVECTOR3( 0.0f, 0.0f, -FLOAT1 );
			D3DXVec3Normalize( &vSearchDir, &vSearchDir );
			
			float fHalfCosine = 0.0f;

			fHalfCosine = cosf( DEGtoRAD( m_fTargetAngle ) * FLOAT05 );

			if( D3DXVec3Dot( &vSearchDir, &vDir ) >= fHalfCosine )
			{
				BaseCharDistanceSq kInfo;
				kInfo.m_Name = pTarget->GetCharName();
				kInfo.m_fDistanceSq = fDistSq;
				vTargetInfoList.push_back( kInfo );
			}
		}
	}
	if( !vTargetInfoList.empty() )
	{
		std::sort( vTargetInfoList.begin(), vTargetInfoList.end(), BaseCharDistanceSqSort2() );
	}
	if( m_szTargetName.IsEmpty() )
	{
		int iCnt = vTargetInfoList.size();
		for( int i=0; i<iCnt; ++i )
		{
			float fDistSq = vTargetInfoList[i].m_fDistanceSq;
			if( fDistSq < fPrevDistSq )	// Enemy
			{
				fPrevDistSq = fDistSq;
				m_szTargetName = vTargetInfoList[i].m_Name;
			}
		}

		if( !m_szTargetName.IsEmpty() )
		{
			DestroyAimMark( pChar );
		}
	}
	else if( !vTargetInfoList.empty() )
	{
		int iCurTarget = -1;

		int iCnt = vTargetInfoList.size();
		for( int i=0; i<iCnt; ++i )
		{
			if( m_szTargetName == vTargetInfoList[i].m_Name )
				iCurTarget = i;
		}

		if( iCurTarget != -1 )
		{
			bool bTargetChange = false;
			if( m_bJumpKey )
			{
				iCurTarget++;
				bTargetChange = true;
				if( iCurTarget >= iCnt )
					iCurTarget = 0;
			}
			else if( m_bDefenseKey )
			{
				bTargetChange = true;
				iCurTarget--;
				if( iCurTarget < 0 )
					iCurTarget = iCnt-1;
			}

			m_szTargetName = vTargetInfoList[iCurTarget].m_Name;

			if( bTargetChange )
			{
				DestroyAimMark( pChar );
			}
		}
		else
			m_szTargetName.Clear();
	}
	else
		m_szTargetName.Clear();

}

void ioTargetBuffSkill::UpdateAimMark( ioBaseChar *pChar, ioPlayStage *pStage )
{
	if( !pChar )
		return;
	
	if( !pStage )
		return;

	ioBaseChar *pTarget = pStage->GetBaseChar( m_szTargetName );
	if( !pTarget )
	{
		DestroyAimMark( pChar );
		return;
	}
	
	D3DXVECTOR3 vPos = pTarget->GetMidPositionByRate();
	D3DXVECTOR3 vScale = D3DXVECTOR3( FLOAT1, FLOAT1, FLOAT1 );

	if( m_dwAimMarkID == -1 )
	{			
		ioMapEffect *pMapEffect = pStage->CreateMapEffect( m_szAimMark, vPos, vScale );
		if( pMapEffect )
		{
			m_dwAimMarkID = pMapEffect->GetUniqueID();
		}
	}
	else if( m_dwAimMarkID != -1 )
	{
		ioMapEffect *pMapEffect = pStage->FindMapEffect( m_dwAimMarkID );
		if( pMapEffect )
			pMapEffect->SetWorldPosition( vPos );	

	}
}

void ioTargetBuffSkill::DestroyAimMark( ioBaseChar *pChar )
{
	if( !pChar )	return;

	if( !pChar->IsNeedProcess() )
		return;

	if( m_dwAimMarkID != -1 )
	{
		pChar->DestroyMapEffect( m_dwAimMarkID );
		m_dwAimMarkID = -1;
	}
}

void ioTargetBuffSkill::SetOwnerBuff( ioBaseChar *pChar )
{
	if( !pChar )
		return;

	int iCnt = m_OwnerBuffList.size();
	for( int i=0; i<iCnt; ++i )
	{
		ioBuffInfo kInfo = m_OwnerBuffList[i];

		if( GetOwnerItem() )
		{
			ioBuff *pBuff = pChar->AddNewBuff( kInfo.m_BuffName, pChar->GetCharName(), GetOwnerItem()->GetName(), this );
			ioConcentrateTargetBuff *pConcentrateTargetBuff = ToConcentrateTargetBuff(pBuff);
			if( pConcentrateTargetBuff )
			{
				pConcentrateTargetBuff->SetConcentrateCharName( pChar->GetCharName(), m_szTargetName );
			}

			ioAffectWoundedValueBuff *pAffectWoundedValueBuff = ToAffectWoundedValue( pBuff );
			if( pAffectWoundedValueBuff )
			{
				pAffectWoundedValueBuff->SetExceptCharName( m_szTargetName );
			}
		}
	}
}

void ioTargetBuffSkill::SetEnemyBuff( ioBaseChar *pChar )
{
	if( !pChar )
		return;

	ioBaseChar* pEnemy = pChar->GetCreator()->GetBaseChar( m_szTargetName );
	if( !pEnemy )
		return;

	int iCnt = m_EnemyBuffList.size();
#ifndef SHIPPING
	LOG.PrintConsoleLog( 0, "ioTargetBuffSkill::SetEnemyBuff - iCnt = %d", iCnt );
#endif
	for( int i = 0; i < iCnt; i++ )
	{
		ioBuffInfo kInfo = m_EnemyBuffList[i];

		if( GetOwnerItem() )
		{
			pEnemy->AddNewBuff( kInfo.m_BuffName, pChar->GetCharName(), GetOwnerItem()->GetName(), this );
		}
	}
}