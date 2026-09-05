
#include "stdafx.h"

#include "ioRotateDirKeyAttackSkill.h"

#include "ioPlayStage.h"
#include "ioBaseChar.h"

#include "WeaponDefine.h"

ioRotateDirKeyAttackSkill::ioRotateDirKeyAttackSkill()
{
	m_SkillState = SS_None;

	m_bSetHandMesh = false;
	m_dwCurSkillProtectTime = 0;
	m_dwAttachWeaponIdx = -1;
	m_fCurAngle = 0.0f;
}

ioRotateDirKeyAttackSkill::ioRotateDirKeyAttackSkill( const ioRotateDirKeyAttackSkill &rhs )
	: ioAttackSkill( rhs ),
	m_dwSkillProtectTime( rhs.m_dwSkillProtectTime ),
	m_PreAttackInfo( rhs.m_PreAttackInfo ),
	m_AttackInfo( rhs.m_AttackInfo ),
	m_LastAttackInfo( rhs.m_LastAttackInfo ),
	m_szCheckBuff( rhs.m_szCheckBuff ),
	m_RemoveBuff( rhs.m_RemoveBuff ),
	m_dwRotateSpeed( rhs.m_dwRotateSpeed ),
	m_fMaxAngle( rhs.m_fMaxAngle )
{
	m_SkillState = SS_None;

	m_bSetHandMesh = false;
	m_dwCurSkillProtectTime = 0;
	m_dwAttachWeaponIdx = -1;
	m_fCurAngle = 0.0f;
}

ioRotateDirKeyAttackSkill::~ioRotateDirKeyAttackSkill()
{
}

void ioRotateDirKeyAttackSkill::LoadProperty( ioINILoader &rkLoader )
{
	ioAttackSkill::LoadProperty( rkLoader );

	char szBuf[MAX_PATH];

	m_dwSkillProtectTime = (DWORD)rkLoader.LoadInt_e( "skill_protect_time", 0 );

	LoadAttackAttribute( "Pre_Attack_Info", m_PreAttackInfo, rkLoader );
	LoadAttackAttribute( "Attack_Info", m_AttackInfo, rkLoader );
	LoadAttackAttribute( "Last_Attack_Info", m_LastAttackInfo, rkLoader );

	rkLoader.LoadString_e( "check_buff", "", szBuf, MAX_PATH );
	m_szCheckBuff = szBuf;

	LoadRemoveBuffList( rkLoader );

	m_dwRotateSpeed = rkLoader.LoadInt_e( "rotate_speed", 0 );
	m_fMaxAngle = rkLoader.LoadFloat_e( "max_angle", 0.0f );
}

ioSkill* ioRotateDirKeyAttackSkill::Clone()
{
	return new ioRotateDirKeyAttackSkill( *this );
}

bool ioRotateDirKeyAttackSkill::OnSkillStart( ioBaseChar *pChar )
{	
	m_SkillState = SS_None;

	m_bSetHandMesh = false;
	m_dwCurSkillProtectTime = 0;
	m_dwAttachWeaponIdx = -1;
	m_fCurAngle = 0.0f;

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

void ioRotateDirKeyAttackSkill::OnProcessState( ioBaseChar *pChar, ioPlayStage *pStage )
{
	DWORD dwCurTime = FRAMEGETTIME();

	switch( m_SkillState )
	{
	case SS_None:
		{
			DWORD dwPreDelay = GetPreDelayTime();
			if( m_dwPreDelayStartTime + dwPreDelay < dwCurTime )
			{
				m_dwPreDelayStartTime = 0;
				EndPreDelayEffect( pChar );
				SetState( pChar, m_PreAttackInfo, SS_Pre_Attack, true );
			}
		}
		break;
	case SS_Pre_Attack:
		{
			if ( m_dwAttachWeaponIdx != -1 )
			{
				ioWeapon *pWeapon = g_WeaponMgr.FindWeapon( pChar, m_dwAttachWeaponIdx );
				if ( !pWeapon )
				{
					SetEndState( pChar );
					return;
				}
			}

			ProcessCharRotate( pChar );

			if ( m_fCurAngle >= m_fMaxAngle )
				SetAttackState( pChar, pStage );
		}
		break;
	case SS_Attack:
		{
			if ( m_dwMotionEndTime < dwCurTime )
			{
				SetLastAttackState( pChar, pStage );
				return;
			}
		}
		break;
	case SS_Last_Attack:
		{
			if ( m_dwMotionEndTime < dwCurTime )
				SetEndState( pChar );
		}
		break;
	}
}

void ioRotateDirKeyAttackSkill::OnSkillEnd( ioBaseChar *pChar )
{
	ioAttackSkill::OnSkillEnd( pChar );

	EndCameraBuff( pChar );

	// 기본무기 장착
	if( m_bUseItemMesh )
		pChar->ShowWeaponItemMesh( true );

	if( !m_HandMesh.IsEmpty() )
		pChar->RestoreSkillWeaponMesh( this, m_HandMesh, m_bVisibleEquipMesh );

	m_bSetHandMesh = false;

	if ( m_dwAttachWeaponIdx != -1 )
	{
		ioWeapon *pWeapon = g_WeaponMgr.FindWeapon( pChar, m_dwAttachWeaponIdx );
		if ( pWeapon && pWeapon->IsLive() )
			pWeapon->SetWeaponDead();
	}

	ioPlayStage *pStage = pChar->GetCreator();
	if ( !pStage )
		return;

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

void ioRotateDirKeyAttackSkill::ApplyAttributeToWeapon( ioWeapon *pWeapon, ioPlayStage *pStage )
{
	ioSkill::ApplyAttributeToWeapon( pWeapon, pStage );	

	ioZoneEffectWeapon *pZone = ToZoneEffectWeapon( pWeapon );
	ioAdhesiveMissileWeapon4 *pAdhesive4 = ToAdhesiveMissileWeapon4( pWeapon );

	if( pAdhesive4 )
	{
		m_dwAttachWeaponIdx = pWeapon->GetWeaponIndex();
		pAdhesive4->SetState( ioAdhesiveMissileWeapon4::WS_ATTACH_MOVE );
	}
	else if( pZone )
	{
		ioBaseChar *pChar = pZone->GetOwner();
		if( pChar )
		{
			D3DXVECTOR3 vDir = pChar->GetTargetRot() * D3DXVECTOR3( 0.0f, 0.0f, -FLOAT1 );
			pZone->SetCollisionDir( vDir );
			pZone->SetZoneValue( false, true );
		}		
	}
	else if ( pWeapon )
	{
		ioBaseChar *pChar = pWeapon->GetOwner();
		if( pChar )
		{				
			D3DXVECTOR3 vPos = pChar->GetMidPositionByRate();
			D3DXVECTOR3 vFireDir = pChar->GetWorldOrientation() * D3DXVECTOR3( 0.0f, 0.0f, -FLOAT1 );
			if ( !m_szAimedTarget.IsEmpty() )
			{
				ioBaseChar *pTarget = pChar->GetBaseChar( m_szAimedTarget );
				if ( pTarget )
					vFireDir = pTarget->GetMidPositionByRate() - pWeapon->GetPosition();
			}

			D3DXVec3Normalize( &vFireDir, &vFireDir );
			pWeapon->SetMoveDir( vFireDir );
		}
	}
}

void ioRotateDirKeyAttackSkill::ApplyExtraSkillInfo( ioBaseChar *pChar, ioPlayStage *pStage, SP2Packet &rkPacket )
{
	if( !pChar || !pStage ) 
		return;

	int nState;
	rkPacket >> nState;

	switch( nState )
	{
	case SS_Attack:
		{
			D3DXVECTOR3 vPos;
			rkPacket >> vPos;
			pChar->SetWorldPosition( vPos );

			D3DXQUATERNION qtRot;
			rkPacket >> qtRot;
			pChar->SetTargetRotAndMoveDirChange( qtRot );

			SetState( pChar, m_AttackInfo, SS_Attack, false );

			g_WeaponMgr.CheckAdhesiveWeapon4( pChar->GetCharName() );
		}
		break;
	case SS_Last_Attack:
		{
			D3DXVECTOR3 vPos;
			rkPacket >> vPos;
			pChar->SetWorldPosition( vPos );

			D3DXQUATERNION qtRot;
			rkPacket >> qtRot;
			pChar->SetTargetRotAndMoveDirChange( qtRot );

			rkPacket >> m_szAimedTarget;

			SetState( pChar, m_LastAttackInfo, SS_Last_Attack, false );
		}
		break;
	case SS_END:
		m_SkillState = SS_END;
		break;
	}
}

bool ioRotateDirKeyAttackSkill::IsProtected( int iDefenseBreakType ) const
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

bool ioRotateDirKeyAttackSkill::IsSkillMotionEnd( DWORD dwActionStop, ioBaseChar* pOwner ) const
{	
	if( m_dwPreDelayStartTime > 0 )
		return false;

	switch( m_SkillState )
	{
	case SS_None:
	case SS_Pre_Attack:
	case SS_Attack:
	case SS_Last_Attack:
		return false;
	case SS_END:
		if( m_fSkillEndNextJumpAmt == 0.0f )
			return true;
		else
			return false;
	}

	if( m_dwMotionEndTime + dwActionStop >= FRAMEGETTIME() )
		return false;

	return true;
}

bool ioRotateDirKeyAttackSkill::IsAttackEndState() const
{
	if( m_SkillState == SS_END )
		return true;

	return false;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////

void ioRotateDirKeyAttackSkill::SetState( ioBaseChar *pChar, const AttackAttribute& AttackInfo, SkillState eState, bool bLoop )
{
	if ( !pChar )
		return;

	ioEntityGroup *pGrp = pChar->GetGroup();

	int nAniID = pGrp->GetAnimationIdx( AttackInfo.m_AttackAnimation );
	if( nAniID == -1 )	
		return;

	float fTimeRate  = AttackInfo.m_fAttackAniRate;
	DWORD dwPreDelay = AttackInfo.m_dwPreDelay;

	pGrp->ClearAllLoopAni( FLOAT100, true );
	pGrp->ClearAllActionAni( FLOAT100, true );

	if ( bLoop )
		pGrp->SetLoopAni( nAniID, FLOAT100, FLOAT1, FLOAT1/fTimeRate );
	else
		pGrp->SetActionAni( nAniID, FLOAT100, FLOAT100, FLOAT1, FLOAT1/fTimeRate, 0.0f, dwPreDelay );

	pChar->ClearReservedSliding();

	pChar->RefreshFireTimeList( nAniID, AttackInfo.m_vWeaponInfoList, FTT_SKILL_ATTACK, fTimeRate, dwPreDelay );

	DWORD dwCurTime = FRAMEGETTIME();
	m_dwMotionEndTime = dwCurTime;
	m_dwMotionEndTime += pGrp->GetAnimationFullTime( nAniID ) * fTimeRate;
	m_dwMotionEndTime += dwPreDelay;

	m_SkillState = eState;
}

void ioRotateDirKeyAttackSkill::SetAttackState( ioBaseChar *pChar, ioPlayStage *pStage )
{
	if ( !pChar || !pChar->IsNeedProcess() || !pStage )
		return;

	if (GetTargetList( pChar, pStage ))
	{
		ioUserKeyInput::DirKeyInput eNewDirKey = ioUserKeyInput::DKI_NONE;
		if ( pChar->IsSettedDirection() )
		{
			eNewDirKey = pChar->CheckCurDirKey();
			pChar->SetTargetRotToDirKey( eNewDirKey );
		}

		pChar->SetTargetRotToRotate( pChar->GetTargetRot(), true );

		SetState( pChar, m_AttackInfo, SS_Attack, false );

		g_WeaponMgr.CheckAdhesiveWeapon4( pChar->GetCharName() );

		if ( pChar->IsNeedSendNetwork() )
		{
			SP2Packet kPacket( CUPK_SKILL_EXTRA_INFO );
			kPacket << pChar->GetCharName();
			kPacket << GetName();
			kPacket << SS_Attack;
			kPacket << pChar->GetWorldPosition();
			kPacket << pChar->GetWorldOrientation();
			kPacket << m_szAimedTarget;
			P2PNetwork::SendToAllPlayingUser( kPacket );
		}
	}
	else
		SetEndState( pChar );
}

void ioRotateDirKeyAttackSkill::SetLastAttackState( ioBaseChar *pChar, ioPlayStage *pStage )
{
	if ( !pChar || !pChar->IsNeedProcess() || !pStage )
		return;

	if (GetTargetList( pChar, pStage ))
	{
		SetTargetRotation( pChar );
		SetState( pChar, m_LastAttackInfo, SS_Last_Attack, false );
		
		if ( pChar->IsNeedSendNetwork() )
		{
			SP2Packet kPacket( CUPK_SKILL_EXTRA_INFO );
			kPacket << pChar->GetCharName();
			kPacket << GetName();
			kPacket << SS_Last_Attack;
			kPacket << pChar->GetWorldPosition();
			kPacket << pChar->GetWorldOrientation();
			kPacket << m_szAimedTarget;
			P2PNetwork::SendToAllPlayingUser( kPacket );
		}	
	}
	else
		SetEndState( pChar );
}

void ioRotateDirKeyAttackSkill::SetEndState( ioBaseChar *pChar )
{
	if ( !pChar || !pChar->IsNeedProcess() )
		return;

	m_SkillState = SS_END;

	if ( pChar->IsNeedSendNetwork() )
	{
		SP2Packet kPacket( CUPK_SKILL_EXTRA_INFO );
		kPacket << pChar->GetCharName();
		kPacket << GetName();
		kPacket << SS_END;
		P2PNetwork::SendToAllPlayingUser( kPacket );
	}
}

////////////////////////////////////////////////////////////////////////////////////////////////////////

void ioRotateDirKeyAttackSkill::ProcessCharRotate( ioBaseChar *pChar )
{
	if( !pChar || m_dwRotateSpeed <= 0 )
		return;

	float fTimePerSec = g_FrameTimer.GetSecPerFrame();

	float fRate = 360.0f / m_dwRotateSpeed;
	float fRotateAngle = fRate * ( fTimePerSec * FLOAT1000 );

	fRotateAngle = ioMath::ArrangeHead( fRotateAngle );
	m_fCurAngle += fRotateAngle;

	D3DXQUATERNION qtNewRot;
	D3DXQuaternionRotationAxis( &qtNewRot, &ioMath::UNIT_Y, DEGtoRAD(fRotateAngle));
	D3DXQuaternionMultiply( &qtNewRot, &qtNewRot, &pChar->GetTargetRot() );

	D3DXVECTOR3 vDir = qtNewRot * D3DXVECTOR3( 0.0, 0.0, -FLOAT1 );

	pChar->SetTargetRotToDir( vDir, true );
}

bool ioRotateDirKeyAttackSkill::GetTargetList( ioBaseChar *pChar, ioPlayStage *pStage )
{
	if ( !pChar || !pChar->IsNeedProcess() || !pStage )
		return false;

	m_szAimedTarget.Clear();

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

		if ( !pTarget->HasBuff( pChar->GetCharName(), m_szCheckBuff ) )
			continue;

		if ( pTarget->IsApplyDownState( false ) )
			continue;

		if( !pTarget->IsEnableTarget( true, false ) )
			continue;

		if( pTarget->IsSystemState() )
			continue;
		
		D3DXVECTOR3 vDiff = pTarget->GetWorldPosition() - pChar->GetWorldPosition();
		float fDistSq = D3DXVec3LengthSq( &vDiff );

		tagBaseCharDistanceSq kInfo;
		kInfo.m_Name        = pTarget->GetCharName();
		kInfo.m_fDistanceSq = fDistSq;

		vTargetInfoList.push_back(kInfo);
	}

	if ( vTargetInfoList.empty() )
		return false;

	std::sort( vTargetInfoList.begin(), vTargetInfoList.end(), BaseCharDistanceSqSort2() );	
	m_szAimedTarget = vTargetInfoList[0].m_Name;

	return true;
}

void ioRotateDirKeyAttackSkill::SetTargetRotation( ioBaseChar *pChar )
{
	if ( !pChar )
		return;

	ioBaseChar *pTarget = pChar->GetBaseChar( m_szAimedTarget );
	if( pTarget )
	{
		D3DXVECTOR3 vAimedDir = pTarget->GetMidPositionByRate() - pChar->GetMidPositionByRate();
		pChar->SetTargetRotToDir( vAimedDir, true );		
	}
}

////////////////////////////////////////////////////////////////////////////////////////////////////////

void ioRotateDirKeyAttackSkill::LoadRemoveBuffList( ioINILoader &rkLoader )
{
	int iCnt = rkLoader.LoadInt_e( "remove_buff_cnt", 0 );
	m_RemoveBuff.clear();

	if( iCnt <= 0 ) 
		return;

	m_RemoveBuff.reserve( iCnt );

	char szTitle[MAX_PATH], szBuf[MAX_PATH];
	for( int i=0; i < iCnt; ++i )
	{
		wsprintf_e( szTitle, "remove_buff%d", i+1 );
		rkLoader.LoadString( szTitle, "", szBuf, MAX_PATH );

		m_RemoveBuff.push_back( szBuf );
	}
}