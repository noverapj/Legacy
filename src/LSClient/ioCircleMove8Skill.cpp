#include "StdAfx.h"
#include "ioCircleMove8Skill.h"
#include "ioBaseChar.h"
#include "ioPlayStage.h"


ioCircleMove8Skill::ioCircleMove8Skill(void)
{
	ClearData();
}

ioCircleMove8Skill::ioCircleMove8Skill( const ioCircleMove8Skill &rhs ):
ioNormalSkill( rhs ),
m_szLoopAni( rhs.m_szLoopAni ),
m_fLoopAniRate( rhs.m_fLoopAniRate ),
m_dwLoopDuration( rhs.m_dwLoopDuration ),
m_fCircleRange( rhs.m_fCircleRange ),
m_fCircleMoveSpeed( rhs.m_fCircleMoveSpeed ),
m_fCircleHeightGap( rhs.m_fCircleHeightGap ),
m_fCircleOffSet( rhs.m_fCircleOffSet ),
m_ReadyCircle( rhs.m_ReadyCircle ),
m_RedGuidCircle( rhs.m_RedGuidCircle ),
m_BlueGuidCircle( rhs.m_BlueGuidCircle ),
m_fSearchRadius( rhs.m_fSearchRadius ),
m_fSearchHeight( rhs.m_fSearchHeight ),
m_iSearchMaxTargetCount( rhs.m_iSearchMaxTargetCount ),
m_szGroundEffectName( rhs.m_szGroundEffectName ),
m_vGroundDefaultOffsetList( rhs.m_vGroundDefaultOffsetList ),
m_TargetAttackWeaponInfo( rhs.m_TargetAttackWeaponInfo )
{
	ClearData();
}

ioCircleMove8Skill::~ioCircleMove8Skill(void)
{
}

ioSkill* ioCircleMove8Skill::Clone()
{
	return new ioCircleMove8Skill( *this );
}

void ioCircleMove8Skill::LoadProperty( ioINILoader &rkLoader )
{
	ioNormalSkill::LoadProperty( rkLoader );

	char szBuf[MAX_PATH] = "";
	char szKey[MAX_PATH] = "";

	//Loop
	rkLoader.LoadString_e( "loop_animation", "", szBuf, MAX_PATH );
	m_szLoopAni = szBuf;
	m_fLoopAniRate = rkLoader.LoadFloat_e( "loop_animation_rate", FLOAT1 );
	m_dwLoopDuration = (DWORD)rkLoader.LoadInt_e( "loop_duration", 0 );

	//Magic Circle
	m_fCircleRange = rkLoader.LoadFloat_e( "magic_circle_range", 0.0f );
	m_fCircleMoveSpeed = rkLoader.LoadFloat_e( "magic_circle_move_speed", 0.0f );
	m_fCircleHeightGap = rkLoader.LoadFloat_e( "magic_circle_height_gap", 0.0f );
	m_fCircleOffSet = rkLoader.LoadFloat_e( "magic_circle_offset", 0.0f );

	rkLoader.LoadString_e( "ready_circle", "", szBuf, MAX_PATH );
	m_ReadyCircle = szBuf;
	rkLoader.LoadString_e( "red_guid_circle", "", szBuf, MAX_PATH );
	m_RedGuidCircle = szBuf;
	rkLoader.LoadString_e( "blud_guid_circle", "", szBuf, MAX_PATH );
	m_BlueGuidCircle = szBuf;

	//Action
	m_fSearchRadius = rkLoader.LoadFloat_e( "search_target_radius", 0 );
	m_fSearchHeight = rkLoader.LoadFloat_e( "search_target_height", 0 );
	m_iSearchMaxTargetCount = rkLoader.LoadInt_e( "search_target_max_count", 0 );

	rkLoader.LoadString_e( "ground_effect", "", szBuf, MAX_PATH );
	m_szGroundEffectName = szBuf;

	int iGroundInfoMaxCount = rkLoader.LoadInt_e( "ground_default_max_count", 0 );
	if ( iGroundInfoMaxCount > 0 )
	{
		m_vGroundDefaultOffsetList.reserve( iGroundInfoMaxCount );

		for ( int i=0; i<iGroundInfoMaxCount ; ++i )
		{
			DefaultGroundDirInfo kInfo;

			wsprintf_e( szKey, "ground_default_change_attack_type%d", i+1 );
			kInfo.DefaultWeaponInfo.m_iWeaponIdx = rkLoader.LoadInt( szKey, NO_WEAPON_ATTRIBUTE );

			wsprintf_e( szKey, "ground_default_change_attack_type%d_resist", i+1 );
			kInfo.DefaultWeaponInfo.m_iResistanceIdx = rkLoader.LoadInt( szKey, NO_WEAPON_ATTRIBUTE );

			wsprintf_e( szKey, "ground_default_change_attack_type%d_wounded", i+1 );
			rkLoader.LoadString( szKey, "", szBuf, MAX_PATH );
			kInfo.DefaultWeaponInfo.m_WoundedAnimation = szBuf;

			wsprintf_e( szKey, "ground_default_change_attack_type%d_wounded_time", i+1 );
			kInfo.DefaultWeaponInfo.m_dwWoundedDuration = rkLoader.LoadInt( szKey, 0 );

			wsprintf( szKey, "ground_default_change_attack_type%d_wounded_loop_ani", i+1 );
			kInfo.DefaultWeaponInfo.m_bWoundedLoopAni = rkLoader.LoadBool( szKey, false );

			wsprintf_e( szBuf, "ground_default_change_range%d", i+1 );
			kInfo.m_fChangeRange = rkLoader.LoadFloat( szBuf, 0.0f );

			wsprintf_e( szBuf, "ground_default_change_angle%d", i+1 );
			kInfo.m_fChangeAngle = rkLoader.LoadFloat( szBuf, 0.0f );

			m_vGroundDefaultOffsetList.push_back( kInfo );
		}
	}

	m_TargetAttackWeaponInfo.Init();
	m_TargetAttackWeaponInfo.m_iWeaponIdx = rkLoader.LoadInt_e( "target_attack_weapon_type", NO_WEAPON_ATTRIBUTE );
	m_TargetAttackWeaponInfo.m_iResistanceIdx = rkLoader.LoadInt_e( "target_attack_weapon_type_resist", NO_WEAPON_ATTRIBUTE );
	rkLoader.LoadString_e( "target_attack_weapon_type_wounded", "", szBuf, MAX_PATH );
	m_TargetAttackWeaponInfo.m_WoundedAnimation = szBuf;
	m_TargetAttackWeaponInfo.m_dwWoundedDuration = rkLoader.LoadInt_e( "target_attack_weapon_type_wounded_time", 0 );
	m_TargetAttackWeaponInfo.m_bWoundedLoopAni = rkLoader.LoadBool_e( "target_attack_weapon_type_wounded_loop_ani", false );
}

void ioCircleMove8Skill::ClearData()
{
	m_SkillState = SS_NONE;
	m_dwLoopStartTime = 0;
	m_vCirclePos = ioMath::VEC3_ZERO;
	m_dwFireStartTime = 0;
	m_GroundAttackInfoList.clear();
	m_vSearchTargetNameList.clear();
}

bool ioCircleMove8Skill::OnSkillStart( ioBaseChar *pOwner )
{
	if ( !ioNormalSkill::OnSkillStart( pOwner ) )
		return false;

	m_bSetHandMesh = false;
	// 기본 무기 손에서 제거
	if( m_bUseItemMesh )
		pOwner->ShowWeaponItemMesh( false );
	if( !m_bSetHandMesh && m_bUseItemMesh && !m_HandMesh.IsEmpty() )
	{
		pOwner->ChangeSkillWeaponMesh( this, m_HandMesh, m_bVisibleEquipMesh );
		m_bSetHandMesh = true;
	}

	if( !m_bReduceGauge )
	{
		pOwner->ReduceNeedGaugeBySkill( this, GetNeedGauge(pOwner->GetCharName()) );
		m_bReduceGauge = true;
	}

	ClearData();

	return true;
}

void ioCircleMove8Skill::OnProcessState( ioBaseChar *pOwner, ioPlayStage *pStage )
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
				EndPreDelayEffect( pOwner );
				SetLoopState( pOwner );
				return;
			}
		}
		break;
	case SS_LOOP:
		{
			// 뱡향 변화 체크
			pOwner->CheckChangeDirectionForMagicCircle();

			// MagicCircle 위치 갱신
			pOwner->UpdateMagicCirclePosAll( m_fCircleMoveSpeed, m_fCircleRange, m_fCircleHeightGap, false );

			// MagicCircle 위치방향으로 캐릭터 회전
			D3DXVECTOR3 vTargetPos = pOwner->GetMagicCirclePos();
			pOwner->SetTargetRotToTargetPos( vTargetPos, true, false );

			if( pOwner->IsNeedProcess() && m_dwLoopStartTime > 0 && 
				m_dwLoopStartTime + m_dwLoopDuration < dwCurTime )
			{
				m_dwLoopStartTime = 0;
				SetEndState( pOwner );
				return;
			}

			//이부분은 좀 문제가 있어 보임( 동기화쪽에 문제가 될거 같음 )
			if( pOwner->IsNeedProcess() && pOwner->IsAttackKey() )
			{
				SetActionState( pOwner, pStage );
				return;
			}
		}
		break;
	case SS_ACTION:
		{
			if (  m_dwMotionEndTime < dwCurTime )
			{
				SetEndState( pOwner );
				return;
			}

			if ( m_dwFireStartTime > 0 && m_dwFireStartTime < dwCurTime )
			{
				m_dwFireStartTime = 0;
				CreateTargetGroundWeapon( pOwner, pStage );
				return;
			}
			
		}
		break;
	}
}

void ioCircleMove8Skill::SetLoopState( ioBaseChar *pOwner )
{
	if ( !pOwner )
		return;

	ioEntityGroup *pGrp = pOwner->GetGroup();
	if ( !pGrp )
		return;

	int nAniID = pGrp->GetAnimationIdx( m_szLoopAni );
	if( nAniID == -1 )	
		return;

	m_SkillState = SS_LOOP;

	pGrp->ClearAllLoopAni( FLOAT100, true );
	pGrp->ClearAllActionAni( FLOAT100, true );
	float fAniRate = FLOAT1;
	if ( fAniRate > 0.0f )
		fAniRate = m_fLoopAniRate;
	pGrp->SetLoopAni( nAniID, FLOAT100, FLOAT1, FLOAT1/fAniRate );

	CreateMagicCircle( pOwner );
	m_dwLoopStartTime = FRAMEGETTIME();
}

void ioCircleMove8Skill::CreateMagicCircle( ioBaseChar *pOwner )
{
	if( !pOwner->CheckMagicCircle() )
	{
		// 팀 체크
		if( pOwner->GetTeam() == TEAM_RED )
			pOwner->CreateMagicCircle( m_ReadyCircle, m_ReadyCircle, m_RedGuidCircle, m_fCircleOffSet );
		else if( pOwner->GetTeam() == TEAM_BLUE )
			pOwner->CreateMagicCircle( m_ReadyCircle, m_ReadyCircle, m_BlueGuidCircle, m_fCircleOffSet );
		else
		{
			if( pOwner->GetSingleConvertTeam() == TEAM_BLUE )
				pOwner->CreateMagicCircle( m_ReadyCircle, m_ReadyCircle, m_BlueGuidCircle, m_fCircleOffSet );
			else
				pOwner->CreateMagicCircle( m_ReadyCircle, m_ReadyCircle, m_RedGuidCircle, m_fCircleOffSet );
		}
	}
}

void ioCircleMove8Skill::SetActionState( ioBaseChar *pOwner, ioPlayStage *pStage )
{
	m_SkillState = SS_ACTION;

	if ( pOwner->IsNeedProcess() )
	{
		m_vCirclePos = pOwner->GetMagicCirclePos( m_fCircleOffSet, m_bCircleNoChangeY );
		SearchTarget( pOwner, pStage );
	}

	//Set Ani
	SetNormalSkill( pOwner );

	CreateTargetGroundEffect( pOwner, pStage );

	if( pOwner->IsNeedProcess() &&  GetOwnerItem() )
		pOwner->SetCurItemSkillGauge(GetOwnerItem()->GetName() , 0.0f );

	if ( pOwner->IsNeedSendNetwork() )
	{
		int iTargetCount = m_vSearchTargetNameList.size();
		D3DXVECTOR3 vCreatePos;
		SP2Packet kPacket( CUPK_SKILL_EXTRA_INFO );
		kPacket << pOwner->GetCharName();
		kPacket << GetName();
		kPacket << SSS_ACTION;
		kPacket << pOwner->GetWorldPosition();
		kPacket << pOwner->GetTargetRot();
		kPacket << m_vCirclePos;
		kPacket << iTargetCount;
		for( int i=0; i<iTargetCount; ++i )
		{
			kPacket << m_vSearchTargetNameList[i].m_Name;
			kPacket << m_vSearchTargetNameList[i].m_vPos;
		}
		P2PNetwork::SendToAllPlayingUser( kPacket );
	}
}

void ioCircleMove8Skill::SetEndState( ioBaseChar *pOwner )
{
	m_SkillState = SS_END;

	if ( pOwner->IsNeedSendNetwork() )
	{
		D3DXVECTOR3 vCreatePos;
		SP2Packet kPacket( CUPK_SKILL_EXTRA_INFO );
		kPacket << pOwner->GetCharName();
		kPacket << GetName();
		kPacket << SSS_END;
		P2PNetwork::SendToAllPlayingUser( kPacket );
	}
}

void ioCircleMove8Skill::SearchTarget( ioBaseChar *pChar, ioPlayStage *pStage )
{
	if ( !pChar || !pStage )
		return;

	const BaseCharList &rkTargetList = pStage->GetBaseCharList();
	BaseCharList::const_iterator iter;
	int iTargetCount=0;
	for( iter = rkTargetList.begin(); iter != rkTargetList.end(); ++iter )
	{
		ioBaseChar *pTargetChar = (*iter);
		if( !pTargetChar )
			continue;

		if( pChar->GetCharName() == pTargetChar->GetCharName() )
			continue;

		if( pChar->GetTeam() == pTargetChar->GetTeam() )
			continue;

		if( pTargetChar->IsSystemState() )
			continue;

		if( !pTargetChar->IsEnableTarget() ) 
			continue;
		D3DXVECTOR3 vTargetPos = pTargetChar->GetWorldPosition();

		float fHeightLength = vTargetPos.y - m_vCirclePos.y;
		if( m_fSearchHeight > 0.0f && fabs(fHeightLength) > m_fSearchHeight )
			continue;

		vTargetPos.y = m_vCirclePos.y;
		D3DXVECTOR3 vDiff = m_vCirclePos - vTargetPos;
		float fLength = D3DXVec3Length( &vDiff );

		if( m_fSearchRadius > 0.0f && fLength > m_fSearchRadius )
			continue;

		if ( m_iSearchMaxTargetCount > 0 && iTargetCount > m_iSearchMaxTargetCount )
			continue;

		BaseCharDistanceSq vTargetInfo;
		vTargetInfo.m_Name = pTargetChar->GetCharName();
		vTargetInfo.m_vPos = vTargetPos;
		m_vSearchTargetNameList.push_back( vTargetInfo );
		iTargetCount++;
	}
}

void ioCircleMove8Skill::SetNormalSkill( ioBaseChar *pChar )
{
	if ( !pChar )
		return;

	ioEntityGroup *pGrp = pChar->GetGroup();
	if ( !pGrp )
		return;

	int iAniID = pGrp->GetAnimationIdx( GetSkillMotionName() );
	if( iAniID == -1 )
		return;

	float fAniRate = GetSkillMotionRate();
	if ( fAniRate <= 0.0f )
		fAniRate = FLOAT1;

	DWORD dwCurTime = FRAMEGETTIME();

	pGrp->ClearAllLoopAni( FLOAT100, true );
	pGrp->ClearAllActionAni( FLOAT100, true );
	pGrp->SetActionAni( iAniID, FLOAT100, FLOAT100, FLOAT1, FLOAT1/fAniRate );

	m_dwMotionStartTime = m_dwFireStartTime = m_dwFireMotionEndTime = dwCurTime;

	m_dwMotionEndTime = m_dwMotionStartTime + pGrp->GetAnimationFullTime( iAniID ) * fAniRate;
	m_dwMotionEndTime += m_dwAnimationEndDelay;

	m_dwFireStartTime += pGrp->GetAnimationEventTime_e( iAniID, "fire_s" ) * fAniRate;
	m_dwFireMotionEndTime += pGrp->GetAnimationEventTime_e( iAniID, "firemotion_e" ) * fAniRate;
	if( m_dwFireStartTime >= m_dwFireMotionEndTime )
		m_dwFireMotionEndTime = m_dwMotionEndTime;

	m_dwEnableReserveTime = pGrp->GetAnimationEventTime_e( iAniID, "key_reserve" ) * fAniRate;
	if ( m_dwEnableReserveTime > 0 )
		m_dwEnableReserveTime += dwCurTime;
}

void ioCircleMove8Skill::CreateTargetGroundEffect( ioBaseChar *pOwner, ioPlayStage *pStage )
{
	if ( m_szGroundEffectName.IsEmpty() || !pOwner || !pStage )
		return;

	if ( !m_vSearchTargetNameList.empty() )
	{
		D3DXVECTOR3 vScale( FLOAT1, FLOAT1 ,1.0f );
		for ( int i=0 ; i<(int)m_vSearchTargetNameList.size() ; ++i )
		{
			ioMapEffect* pMapEffect = pStage->CreateMapEffect( m_szGroundEffectName, m_vSearchTargetNameList[i].m_vPos, vScale );
			if( pMapEffect )
			{
				GroundAttackInfo kInfo;
				kInfo.szTargetName = m_vSearchTargetNameList[i].m_Name;
				kInfo.vPos = pMapEffect->GetWorldPosition();
				kInfo.WeaopnData = m_TargetAttackWeaponInfo;
				kInfo.dwEffectID = pMapEffect->GetUniqueID();
				m_GroundAttackInfoList.push_back( kInfo );
			}
		}
	}
	else
	{
		
		D3DXVECTOR3 vScale( FLOAT1, FLOAT1 ,1.0f );
		D3DXVECTOR3 vCurDir = pOwner->GetWorldOrientation() * D3DXVECTOR3( 0.0f, 0.0f, -FLOAT1 );
		for ( int i=0 ; i<(int)m_vGroundDefaultOffsetList.size() ; ++i )
		{
			DefaultGroundDirInfo& rkInfo = m_vGroundDefaultOffsetList[i];

			float fAngle = ioMath::ArrangeHead( rkInfo.m_fChangeAngle );
			D3DXQUATERNION qtRot;
			D3DXQuaternionRotationAxis( &qtRot, &ioMath::UNIT_Y, DEGtoRAD(fAngle) );
			D3DXVECTOR3 vRotDir = qtRot * vCurDir;
			D3DXVECTOR3 vEffectPos = m_vCirclePos;
			vEffectPos += vRotDir * rkInfo.m_fChangeRange;
			ioMapEffect* pMapEffect = pStage->CreateMapEffect( m_szGroundEffectName, vEffectPos, vScale );
			if( pMapEffect )
			{
				GroundAttackInfo kInfo;
				kInfo.vPos = pMapEffect->GetWorldPosition();
				kInfo.dwEffectID = pMapEffect->GetUniqueID();
				kInfo.WeaopnData = rkInfo.DefaultWeaponInfo;
				m_GroundAttackInfoList.push_back( kInfo );
			}
		}
	}
}

void ioCircleMove8Skill::CreateTargetGroundWeapon( ioBaseChar *pOwner, ioPlayStage *pStage )
{
	if ( !pOwner || !pStage )
		return;

	DWORD dwWeaponIndex = pOwner->IncreaseWeaponIndexBase();
	for ( int i=0; i<(int)m_GroundAttackInfoList.size() ; ++i )
	{
		GroundAttackInfo& rkInfo = m_GroundAttackInfoList[i];
		D3DXVECTOR3 vDir = ioMath::UNIT_Y;
		ioBaseChar *pTarget = pStage->GetBaseChar( rkInfo.szTargetName );
		if ( pTarget )
		{
			vDir = pTarget->GetWorldPosition() - rkInfo.vPos;
			D3DXVec3Normalize( &vDir, &vDir );
		}

		pOwner->SkillFireExplicit( rkInfo.WeaopnData, GetName(), dwWeaponIndex++, rkInfo.vPos, vDir );
	}
}

void ioCircleMove8Skill::OnSkillEnd( ioBaseChar *pOwner )
{
	ioNormalSkill::OnSkillEnd( pOwner );

	pOwner->DestroyMagicCircle();

	ioPlayStage *pStage =  pOwner->GetCreator();
	DestroyTargetGroundEffect( pOwner, pStage );

	// 기본무기 장착
	if( m_bUseItemMesh )
		pOwner->ShowWeaponItemMesh( true );

	if( !m_HandMesh.IsEmpty() )
		pOwner->RestoreSkillWeaponMesh( this, m_HandMesh, m_bVisibleEquipMesh );

	m_bSetHandMesh = false;
	ClearData();
}

bool ioCircleMove8Skill::IsSkillMotionEnd( DWORD dwActionStop, ioBaseChar* pOwner ) const
{
	if ( m_SkillState == SS_END )
		return true;

	return false;
}

void ioCircleMove8Skill::DestroyTargetGroundEffect( ioBaseChar *pOwner, ioPlayStage *pStage )
{
	if ( !pOwner || !pStage )
		return;

	for ( int i=0; i<(int)m_GroundAttackInfoList.size() ; ++i )
		pStage->DestroyMapEffect( m_GroundAttackInfoList[i].dwEffectID );

	m_GroundAttackInfoList.clear();
}

void ioCircleMove8Skill::ApplyExtraSkillInfo( ioBaseChar *pOwner, ioPlayStage *pStage, SP2Packet &rkPacket )
{
	if ( !pOwner || !pStage )
		return;

	int iState;
	rkPacket >> iState;

	if ( iState == SSS_ACTION )
	{
		D3DXVECTOR3 vPos;
		rkPacket >> vPos;
		pOwner->SetWorldPosition( vPos );

		D3DXQUATERNION qtRot;
		rkPacket >> qtRot;
		pOwner->SetTargetRotAndMoveDirChange( qtRot );
		
		rkPacket >> m_vCirclePos;

		int iTargetCount;
		rkPacket >> iTargetCount;
		if ( iTargetCount > 0 )
		{
			for ( int i=0 ; i<iTargetCount; ++i )
			{
				BaseCharDistanceSq kInfo;
				rkPacket >> kInfo.m_Name;
				rkPacket >> kInfo.m_vPos;
				m_vSearchTargetNameList.push_back( kInfo );
			}
		}

		SetActionState( pOwner, pStage );
	}
	else if ( iState == SSS_END )
		SetEndState( pOwner );
}
