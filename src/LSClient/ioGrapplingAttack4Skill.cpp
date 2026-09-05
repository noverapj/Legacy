

#include "stdafx.h"

#include "ioNpcChar.h"
#include "ioPlayStage.h"

#include "ioGrapplingAttack4Skill.h"

#include "WeaponDefine.h"

ioGrapplingAttack4Skill::ioGrapplingAttack4Skill()
{
	ClearData();
}

ioGrapplingAttack4Skill::ioGrapplingAttack4Skill( const ioGrapplingAttack4Skill &rhs )
: ioAttackSkill( rhs ),
m_vWeaponToAirList( rhs.m_vWeaponToAirList ),
m_vWeaponToDownList( rhs.m_vWeaponToDownList ),
m_dwSkillProtectTime( rhs.m_dwSkillProtectTime ),
m_SkillLoopMotion( rhs.m_SkillLoopMotion ),
m_fSkillLoopMotionRate( rhs.m_fSkillLoopMotionRate ),
m_dwSkillLoopDuration( rhs.m_dwSkillLoopDuration ),
m_dwSkillLoopProtectTime( rhs.m_dwSkillLoopProtectTime ),
m_vAttackUp( rhs.m_vAttackUp ),
m_vAttackDown( rhs.m_vAttackDown ),
m_vAttackLeft( rhs.m_vAttackLeft ),
m_vAttackRight(rhs.m_vAttackRight ),
m_vAttackUpLeft( rhs.m_vAttackUpLeft ),
m_vAttackDownLeft(rhs.m_vAttackDownLeft ),
m_vAttackUpRight( rhs.m_vAttackUpRight ),
m_vAttackDownRight( rhs.m_vAttackDownRight ),
m_dwEnableKeyTime( rhs.m_dwEnableKeyTime ),
m_fTargetAngle( rhs.m_fTargetAngle ),
m_fTargetRange( rhs.m_fTargetRange ),
m_fCharHeightOffset( rhs.m_fCharHeightOffset )
{
	ClearData();
}

ioGrapplingAttack4Skill::~ioGrapplingAttack4Skill()
{
}

void ioGrapplingAttack4Skill::ClearData()
{
	m_SkillState			= SS_NONE;
	m_eInputDir				= AD_NONE;
	m_GrapplingTargetPosition	= GTP_NONE;

	m_dwGrapplingStartTime	= 0;
	m_dwMotionStartTime		= 0;
	m_dwSkillLoopStartTime	= 0;
	m_dwCurSkillProtectTime = 0;

	m_vTargetPosition = ioMath::VEC3_ZERO;
	m_TargetName.Clear();

	m_vAttackDir = ioMath::VEC3_ZERO;

	m_CurDirKey = ioUserKeyInput::DKI_NONE;
}

void ioGrapplingAttack4Skill::LoadProperty( ioINILoader &rkLoader )
{
	ioAttackSkill::LoadProperty( rkLoader );

	char szBuf[MAX_PATH] = "";
	char szKey[MAX_PATH] = "";

	// 공중 공격
	int iCnt;
	iCnt = rkLoader.LoadInt_e( "attack_type_air_count", 0 );
	m_vWeaponToAirList.clear();
	m_vWeaponToAirList.reserve( iCnt );
	for( int i=0; i<iCnt; ++i )
	{
		WeaponInfo	kInfo;
		kInfo.Init();

		wsprintf_e( szKey, "attack_type%d_air", i+1 );
		kInfo.m_iWeaponIdx = rkLoader.LoadInt( szKey, NO_WEAPON_ATTRIBUTE );

		wsprintf_e( szKey, "attack_type%d_air_resist", i+1 );
		kInfo.m_iResistanceIdx = rkLoader.LoadInt( szKey, NO_WEAPON_ATTRIBUTE );

		wsprintf_e( szKey, "attack_type%d_air_wounded", i+1 );
		rkLoader.LoadString( szKey, "", szBuf, MAX_PATH );
		kInfo.m_WoundedAnimation = szBuf;

		wsprintf_e( szKey, "attack_type%d_air_wounded_time", i+1 );
		kInfo.m_dwWoundedDuration = rkLoader.LoadInt( szKey, 0 );

		wsprintf_e( szKey, "attack_type%d_air_wounded_loop_ani", i+1 );
		kInfo.m_bWoundedLoopAni = rkLoader.LoadBool( szKey, false );

		m_vWeaponToAirList.push_back( kInfo );
	}

	// 다운 공격
	iCnt = rkLoader.LoadInt_e( "attack_type_down_count", 0 );
	m_vWeaponToDownList.clear();
	m_vWeaponToDownList.reserve( iCnt );
	for( int i=0; i<iCnt; ++i )
	{
		WeaponInfo	kInfo;
		kInfo.Init();

		wsprintf_e( szKey, "attack_type%d_down", i+1 );
		kInfo.m_iWeaponIdx = rkLoader.LoadInt( szKey, NO_WEAPON_ATTRIBUTE );

		wsprintf_e( szKey, "attack_type%d_down_resist", i+1 );
		kInfo.m_iResistanceIdx = rkLoader.LoadInt( szKey, NO_WEAPON_ATTRIBUTE );

		wsprintf_e( szKey, "attack_type%d_down_wounded", i+1 );
		rkLoader.LoadString( szKey, "", szBuf, MAX_PATH );
		kInfo.m_WoundedAnimation = szBuf;

		wsprintf_e( szKey, "attack_type%d_down_wounded_time", i+1 );
		kInfo.m_dwWoundedDuration = rkLoader.LoadInt( szKey, 0 );

		wsprintf_e( szKey, "attack_type%d_down_wounded_loop_ani", i+1 );
		kInfo.m_bWoundedLoopAni = rkLoader.LoadBool( szKey, false );

		m_vWeaponToDownList.push_back( kInfo );
	}

	m_dwSkillProtectTime = (DWORD)rkLoader.LoadInt_e( "skill_protect_time", 0 );

	// 스킬 루프 모션
	rkLoader.LoadString_e( "skill_loop_motion", "", szBuf, MAX_PATH );
	m_SkillLoopMotion = szBuf;
	m_fSkillLoopMotionRate = rkLoader.LoadFloat_e( "skill_loop_motion_rate", FLOAT1 );
	m_dwSkillLoopDuration = (DWORD)rkLoader.LoadInt_e( "skill_loop_duration", 0 );
	m_dwSkillLoopProtectTime = (DWORD)rkLoader.LoadInt_e( "skill_loop_protect_time", 0 );

	// 방향별 웨폰 정보
	LoadWeaponInfo( "attack_up", m_vAttackUp, rkLoader );
	LoadWeaponInfo( "attack_down", m_vAttackDown, rkLoader );
	LoadWeaponInfo( "attack_left", m_vAttackLeft, rkLoader );
	LoadWeaponInfo( "attack_right", m_vAttackRight, rkLoader );
	LoadWeaponInfo( "attack_up_left", m_vAttackUpLeft, rkLoader );
	LoadWeaponInfo( "attack_up_right", m_vAttackUpRight, rkLoader );
	LoadWeaponInfo( "attack_down_left", m_vAttackDownLeft, rkLoader );
	LoadWeaponInfo( "attack_down_right", m_vAttackDownRight, rkLoader );

	// 키 연타.
	m_dwEnableKeyTime = (DWORD)rkLoader.LoadInt_e( "enable_key_time", 0 );

	m_fTargetAngle = rkLoader.LoadFloat_e( "target_angle", 0.0f );
	m_fTargetRange = rkLoader.LoadFloat_e( "target_range", 0.0f );

	m_fCharHeightOffset = rkLoader.LoadFloat_e( "char_height_offset", FLOAT05 );
}

void ioGrapplingAttack4Skill::LoadWeaponInfo( const char *szWeapon, AttackInfo &rkAttackInfo, ioINILoader &rkLoader )
{
	rkAttackInfo.Init();

	char szBuf[MAX_PATH], szKey[MAX_PATH];

	wsprintf_e( szKey, "%s_motion", szWeapon );
	rkLoader.LoadString( szKey, "", szBuf, MAX_PATH );
	rkAttackInfo.m_vAttackAni = szBuf;
	if( rkAttackInfo.m_vAttackAni.IsEmpty() )	return;

	wsprintf_e( szKey, "%s_motion_rate", szWeapon );
	rkAttackInfo.m_fAttackAniRate = rkLoader.LoadFloat( szKey, FLOAT1 );

	wsprintf_e( szKey, "%s_motion_pre_delay", szWeapon );
	rkAttackInfo.m_dwPreDelay = (DWORD)rkLoader.LoadInt( szKey, 0 );

	wsprintf_e( szKey, "%s_motion_end_delay", szWeapon );
	rkAttackInfo.m_dwEndDelay = (DWORD)rkLoader.LoadInt( szKey, 0 );

	wsprintf_e( szKey, "%s_type_count", szWeapon );
	int iCnt = rkLoader.LoadInt( szKey, 0 );
	if( iCnt > 0 )
	{
		rkAttackInfo.m_vWeaponInfoList.clear();
		rkAttackInfo.m_vWeaponInfoList.reserve( iCnt );
	}

	for( int i=0; i<iCnt; ++i )
	{
		WeaponInfo	kInfo;
		kInfo.Init();

		wsprintf_e( szKey, "%s_type%d", szWeapon, i+1 );
		kInfo.m_iWeaponIdx = rkLoader.LoadInt( szKey, NO_WEAPON_ATTRIBUTE );

		wsprintf_e( szKey, "%s_type%d_resist", szWeapon, i+1 );
		kInfo.m_iResistanceIdx = rkLoader.LoadInt( szKey, NO_WEAPON_ATTRIBUTE );

		wsprintf_e( szKey, "%s_type%d_wounded", szWeapon, i+1 );
		rkLoader.LoadString( szKey, "", szBuf, MAX_PATH );
		kInfo.m_WoundedAnimation = szBuf;

		wsprintf_e( szKey, "%s_type%d_wounded_time", szWeapon, i+1 );
		kInfo.m_dwWoundedDuration = (DWORD)rkLoader.LoadInt( szKey, 0 );

		wsprintf_e( szKey, "%s_type%d_wounded_loop_ani", szWeapon, i+1 );
		kInfo.m_bWoundedLoopAni = rkLoader.LoadBool( szKey, false );

		rkAttackInfo.m_vWeaponInfoList.push_back( kInfo );
	}
}

ioSkill* ioGrapplingAttack4Skill::Clone()
{
	return new ioGrapplingAttack4Skill( *this );
}

bool ioGrapplingAttack4Skill::OnSkillStart( ioBaseChar *pChar )
{
	if( !ioAttackSkill::OnSkillStart( pChar ) )
		return false;

	ClearData();

	m_GrapplingTargetName.Clear();

	m_bSetHandMesh = false;

	// 기본 무기 손에서 제거
	if( m_bUseItemMesh )
	{
		pChar->ShowWeaponItemMesh( false );
	}

	if( !m_bReduceGauge )
	{
		pChar->ReduceNeedGaugeBySkill( this, GetNeedGauge(pChar->GetCharName()) );
		m_bReduceGauge = true;
	}

	SetCameraBuff( pChar );

	// 사용 아이템 처리
	if( !m_bSetHandMesh && m_bUseItemMesh && !m_HandMesh.IsEmpty() )
	{
		pChar->ChangeSkillWeaponMesh( this, m_HandMesh, m_bVisibleEquipMesh );
		m_bSetHandMesh = true;
	}

	FindTarget( pChar );

	m_dwCurSkillProtectTime = 0;
	if( m_dwSkillProtectTime > 0 )
		m_dwCurSkillProtectTime = FRAMEGETTIME() + m_dwSkillProtectTime;

	return true;
}

void ioGrapplingAttack4Skill::OnProcessState( ioBaseChar *pChar, ioPlayStage *pStage )
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
				AttackSkillFire( pChar );
			}
		}
		break;
	case SS_ACTION:
		break;
	case SS_GRAPPLING:
		{
			// Target 유효 체크
			ioBaseChar *pTarget = pStage->GetBaseChar( m_GrapplingTargetName );
			if( pTarget )
			{
				if( m_dwGrapplingStartTime + g_App.GetGrapplingCheckDelay() < dwCurTime &&
					pTarget->GetState() != CS_GRAPPLING_WOUNDED_SKILL )
				{
					SetSkillEndState();
					return;
				}
			}
			else
			{
				SetSkillEndState();
				return;
			}

			if( pChar->IsNeedProcess() )
			{
				CheckKeyInput( pChar );

				if( pChar->IsAttackKey() )
					GrapplingAttackFire( pChar );

				if( m_dwGrapplingStartTime+m_dwEnableKeyTime < dwCurTime )
					GrapplingAttackFire( pChar );
			}
		}
		break;
	case SS_END:
		break;
	}
}

void ioGrapplingAttack4Skill::OnSkillEnd( ioBaseChar *pChar )
{
	ioAttackSkill::OnSkillEnd( pChar );

	// 기본무기 장착
	if( m_bUseItemMesh )
	{
		pChar->ShowWeaponItemMesh( true );
	}

	// 스킬 중간 동작에 사용된 메쉬들 제거
	m_bSetHandMesh = false;
	if( !m_HandMesh.IsEmpty() )
		pChar->RestoreSkillWeaponMesh( this, m_HandMesh, m_bVisibleEquipMesh );

	m_GrapplingTargetName.Clear();

	EndCameraBuff( pChar );

	ClearData();
}

void ioGrapplingAttack4Skill::ApplyExtraSkillInfo( ioBaseChar *pChar, ioPlayStage *pStage, SP2Packet &rkPacket )
{
	if( !pChar )	return;
	if( !pStage )	return;

	int iState;
	rkPacket >> iState;

	D3DXQUATERNION qtCharRot;
	rkPacket >> qtCharRot;

	pChar->SetTargetRotToRotate( qtCharRot, true, false );
	switch( iState )
	{
	case SSC_TARGET:
		{
			rkPacket >> m_TargetName;
		}
		break;
	case SSC_GRAPPLING:
		{
			rkPacket >> m_GrapplingTargetName;
			rkPacket >> m_vTargetPosition;

			m_dwGrapplingStartTime	= FRAMEGETTIME();
			m_SkillState = SS_GRAPPLING;

			SetSkillLoopMotion( pChar );

			ioBaseChar *pTarget = pStage->GetBaseChar( m_GrapplingTargetName );
			if( pTarget && pTarget->IsNeedProcess() )
			{
				pTarget->SetSkillGrapplingState( pChar->GetCharName(), GetSkillWoundGrapplingInfo(), GetName() );
				pTarget->SetWorldPosition( m_vTargetPosition );
			}
		}
		break;
	case SSC_ATTACK:
		{
			int eInputDir = AD_NONE;
			rkPacket >> eInputDir;
			m_eInputDir = (AttackDir)eInputDir;

			GrapplingAttackFire( pChar );

			ioBaseChar *pTarget = pStage->GetBaseChar( m_GrapplingTargetName );
			if( pTarget && pTarget->IsNeedProcess() )
			{
				pTarget->SetSkillGrapplingState( pChar->GetCharName(), GetSkillWoundGrapplingInfo(), GetName() );
			}
		}
		break;
	}
}

void ioGrapplingAttack4Skill::ApplyAttributeToWeapon( ioWeapon *pWeapon, ioPlayStage *pStage )
{
	ioSkill::ApplyAttributeToWeapon( pWeapon, pStage );

	if( pWeapon && pWeapon->CheckGrapplingState() == GPT_WRESTLING && !m_GrapplingTargetName.IsEmpty() )
	{
		pWeapon->SetGrapplingTargetName( m_GrapplingTargetName );
	}
	else if( pWeapon && pWeapon->CheckGrapplingState() == GPT_WRESTLING_ALL && !m_GrapplingTargetName.IsEmpty() )
	{
		pWeapon->SetGrapplingTargetName( m_GrapplingTargetName );
	}
	else if( pWeapon && pWeapon->CheckGrapplingState() == GPT_ATTACK && !m_GrapplingTargetName.IsEmpty() )
	{
		pWeapon->SetGrapplingTargetName( m_GrapplingTargetName );
	}

	//
	ioGuideWeapon *pGuideWeapon = ToGuideWeapon( pWeapon );
	if( pGuideWeapon )
	{
		pGuideWeapon->SetTraceTarget( m_TargetName );
	}

	ioZoneEffectWeapon *pZoneEffect = ToZoneEffectWeapon( pWeapon );
	if( pZoneEffect )
	{
		pZoneEffect->SetCollisionDir( m_vAttackDir );
		pZoneEffect->SetZoneValue( m_vTargetPosition, true );
	}
}

void ioGrapplingAttack4Skill::SetSkillLoopMotion( ioBaseChar *pChar )
{
	if( !pChar )	return;

	ioEntityGroup *pGrp = pChar->GetGroup();
	if( !pGrp ) return;

	int iAniID = pGrp->GetAnimationIdx( m_SkillLoopMotion );
	if( iAniID == -1 )	return;

	m_dwSkillLoopStartTime = FRAMEGETTIME();

	pGrp->ClearAllLoopAni( FLOAT100, true );
	pGrp->ClearAllActionAni( FLOAT100, true );

	pGrp->SetLoopAni( iAniID, FLOAT100, FLOAT1, FLOAT1/m_fSkillLoopMotionRate );
}

void ioGrapplingAttack4Skill::FindTarget( ioBaseChar *pOwner )
{
	if( !pOwner )	return;

	ioPlayStage *pStage = pOwner->GetCreator();
	if( !pStage )	return;

	// target
	m_TargetName.Clear();

	D3DXQUATERNION qtRot = pOwner->GetTargetRot();
	D3DXVECTOR3 vDir = qtRot * D3DXVECTOR3( 0.0f, 0.0f, -FLOAT1 );
	D3DXVec3Normalize( &vDir, &vDir );

	float fPreDistSq = m_fTargetRange * m_fTargetRange;
	const BaseCharList &rkCharList = pStage->GetBaseCharList();
	BaseCharList::const_iterator iter;
	for( iter=rkCharList.begin() ; iter!=rkCharList.end() ; ++iter )
	{
		ioBaseChar *pCurChar = *iter;
		if( !pCurChar )	continue;

		if( pCurChar == pOwner )
			continue;

		if( pCurChar->GetTeam() == pOwner->GetTeam() )
			continue;

		if( !IsEnableTargetState( pCurChar ) )
			continue;

		D3DXVECTOR3 vDiff = pCurChar->GetMidPositionByRate() - pOwner->GetMidPositionByRate();
		float fLengthSq = D3DXVec3LengthSq( &vDiff );
		float fLength = D3DXVec3Length( &vDiff ); // 테스트코드

		if( fLengthSq <= fPreDistSq )
		{
			D3DXVec3Normalize( &vDiff, &vDiff );
			float fValue = D3DXVec3Dot( &vDir, &vDiff );
			float fAngleGap = RADtoDEG( acosf(fValue) );
			if( fAngleGap < m_fTargetAngle * FLOAT05 )
			{
				m_TargetName = pCurChar->GetCharName();
				fPreDistSq = fLengthSq;
			}
		}
	}

	CheckTargetState( m_TargetName );

	if( !m_TargetName.IsEmpty() )
	{
		ioBaseChar *pTarget = pOwner->GetBaseChar( m_TargetName );
		if( pTarget )
		{
			D3DXVECTOR3 vOwnerPos = pOwner->GetWorldPosition();
			D3DXVECTOR3 vTargetPos = pTarget->GetWorldPosition();
			D3DXVECTOR3 vDir = vTargetPos - vOwnerPos;
			if( vDir != ioMath::VEC3_ZERO )	
			{
				vDir.y = 0.0f;
				D3DXVec3Normalize( &vDir, &vDir );
				D3DXQUATERNION qtRot;
				ioMath::CalcDirectionQuaternion( qtRot, -vDir );
				pOwner->SetTargetRotToRotate( qtRot, true, false );
			}
		}
	}

	if( pOwner->IsNeedSendNetwork() )
	{
		if( !m_TargetName.IsEmpty() )
		{
			SP2Packet kPacket( CUPK_SKILL_EXTRA_INFO );
			kPacket << pOwner->GetCharName();
			kPacket << GetName();
			kPacket << SSC_TARGET;
			kPacket << pOwner->GetTargetRot();
			kPacket << m_TargetName;
			P2PNetwork::SendToAllPlayingUser( kPacket );
		}
	}
}

void ioGrapplingAttack4Skill::CheckTargetState( ioHashString &szTargetName )
{
	if( szTargetName.IsEmpty() )	return;
	
	ioPlayStage *pStage = g_SkillMaker.GetPlayStage();
	if( !pStage )	return;

	ioBaseChar *pTarget = pStage->GetBaseChar( szTargetName );
	if( !pTarget )	return;

	if( pTarget->IsFloating() || pTarget->IsFloatingState() )
	{
		m_GrapplingTargetPosition = GTP_AIR;
	}
	else if( pTarget->IsApplyDownState(false) )
	{
		m_GrapplingTargetPosition = GTP_DOWN;
	}
	else
	{
		m_GrapplingTargetPosition = GTP_STAND;
	}
}

bool ioGrapplingAttack4Skill::IsEnableTargetState( ioBaseChar *pTarget )
{
	if( !pTarget )
		return false;

	if( pTarget->IsSystemState() )
		return false;
		
	if( !pTarget->IsEnableTarget() ) 	
		return false;

	return true;
}

bool ioGrapplingAttack4Skill::SetSkillGrapplingState( ioBaseChar *pChar, const ioHashString &szTargetName )
{
	if( !pChar )	return false;
	if( m_SkillState == SS_END )
		return false;

	m_SkillState = SS_GRAPPLING;

	// 충돌처리 후 체크된 타겟을 m_GrapplingTargetName로 지정한다.
	m_GrapplingTargetName	= szTargetName;
	m_dwGrapplingStartTime	= FRAMEGETTIME();

	ioBaseChar *pTarget = pChar->GetBaseChar( m_GrapplingTargetName );
	if( !pTarget ) return false;

	m_vTargetPosition = pTarget->GetWorldPosition();
	if( m_GrapplingTargetPosition != GTP_AIR )
	{
		m_vTargetPosition.y += m_fCharHeightOffset;
		pTarget->SetWorldPosition( m_vTargetPosition );
	}

	SetSkillLoopMotion( pChar );

	if( pChar->IsNeedSendNetwork() )
	{
		SP2Packet kPacket( CUPK_SKILL_EXTRA_INFO );
		kPacket << pChar->GetCharName();
		kPacket << GetName();
		kPacket << SSC_GRAPPLING;
		kPacket << pChar->GetTargetRot();
		kPacket << szTargetName;
		kPacket << m_vTargetPosition;
		P2PNetwork::SendToAllPlayingUser( kPacket );
	}

	// for NPC
	bool bNPC = false;
	if( ToNpcChar( pTarget ) )
	{
		bNPC = true;
	}

	// local or 훈련소 or NPC
	if( !P2PNetwork::IsNetworkPlaying() || !pChar->GetCreator()->IsNetworkMode() || bNPC )
	{
		D3DXQUATERNION qtCharRot = pChar->GetTargetRot();
		pTarget->SetSkillGrapplingState( pChar->GetCharName(), GetSkillWoundGrapplingInfo(), GetName() );
	}

	return true;
}

D3DXVECTOR3 ioGrapplingAttack4Skill::GetGrapplingTargetPos( ioBaseChar *pChar )
{
	if( m_vTargetPosition != ioMath::VEC3_ZERO )
		return m_vTargetPosition;

	return ioMath::VEC3_ZERO;
}

bool ioGrapplingAttack4Skill::IsSkillMotionEnd( DWORD dwActionStop, ioBaseChar* pOwner ) const
{
	DWORD dwCurTime = FRAMEGETTIME();

	if( m_dwPreDelayStartTime > 0 )
		return false;

	switch( m_SkillState )
	{
	case SS_GRAPPLING:
		return false;
	case SS_ACTION:
		if( m_dwMotionEndTime + 50 > FRAMEGETTIME() )
			return false;
		return true;
	case SS_END:
		if( m_dwMotionEndTime < dwCurTime )
		{
			return true;
		}
		return false;
	}
	return false;
}

bool ioGrapplingAttack4Skill::IsProtected( int iDefenseBreakType ) const
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
		if( m_dwFireMotionEndTime > dwCurTime )
			return true;
	}

	if( m_dwSkillLoopStartTime > 0 &&
		m_dwSkillLoopStartTime + m_dwSkillLoopProtectTime > dwCurTime )
		return true;

	if( m_dwCurSkillProtectTime > dwCurTime )
		return true;

	return false;
}

bool ioGrapplingAttack4Skill::IsEnableReserve() const
{
	if( m_dwPreDelayStartTime != 0 )
		return false;

	switch( m_SkillState )
	{
	case SS_END:
		if( m_dwEnableReserveTime == 0 || m_dwMotionStartTime >= m_dwEnableReserveTime )
			return false;

		if( m_dwEnableReserveTime < FRAMEGETTIME() )
			return true;
		break;
	}

	return false;
}

bool ioGrapplingAttack4Skill::CheckUseItemMesh()
{
	return m_bUseItemMesh;
}

void ioGrapplingAttack4Skill::AttackSkillFire( ioBaseChar *pChar )
{
	if( !pChar ) return;

	ioEntityGroup *pGrp = pChar->GetGroup();
	if( !pGrp ) return;

	int iAniID = pGrp->GetAnimationIdx( GetAttribute().m_AttackAnimation );
	if( iAniID == -1 )	return;

	m_SkillState = SS_ACTION;

	float fTimeRate  = GetAttackTimeRate();
	DWORD dwPreDelay = GetAttribute().m_dwPreDelay;

	WeaponInfoList vWeaponInfoList;
	vWeaponInfoList.clear();
	if( m_GrapplingTargetPosition == GTP_STAND || m_GrapplingTargetPosition == GTP_NONE )
		vWeaponInfoList = GetAttribute().m_vWeaponInfoList;
	else if( m_GrapplingTargetPosition == GTP_AIR )
		vWeaponInfoList = m_vWeaponToAirList;
	else if( m_GrapplingTargetPosition == GTP_DOWN )
		vWeaponInfoList = m_vWeaponToDownList;
	else
		return;

	if( m_bRefreshFireTimeList )
	{
		pChar->RefreshFireTimeListWithBase( m_dwWeaponBaseIndex,
											iAniID,
											vWeaponInfoList,
											FTT_SKILL_ATTACK,
											fTimeRate,
											dwPreDelay );
	}

	pGrp->ClearAllLoopAni( FLOAT100, true );
	pGrp->ClearAllActionAni( FLOAT100, true );

	pGrp->SetActionAni( iAniID, FLOAT100, FLOAT100, FLOAT1, FLOAT1/fTimeRate, 0.0f, dwPreDelay, true );
	pChar->AttachEffect( m_SkillEffectName );

	// Col Skip
	pChar->CheckCharColSkipSkillTime( iAniID, fTimeRate, dwPreDelay );

	pChar->SetReservedSliding( GetAttribute().m_vForceInfoList, iAniID, fTimeRate, dwPreDelay );

	m_dwMotionStartTime = m_dwMotionEndTime = m_dwFireMotionEndTime = FRAMEGETTIME();
	m_dwMotionEndTime += pGrp->GetAnimationFullTime( iAniID ) * fTimeRate;
	m_dwMotionEndTime += dwPreDelay + GetAttribute().m_dwEndDelay;
	m_dwFireMotionEndTime += dwPreDelay;
	m_dwFireMotionEndTime += pGrp->GetAnimationEventTime_e( iAniID, "firemotion_e" ) * fTimeRate;

	m_GrapplingTargetName.Clear();
}

void ioGrapplingAttack4Skill::SetSkillEndState()
{
	if( m_SkillState == SS_END )	return;

	m_SkillState = SS_END;
}

void ioGrapplingAttack4Skill::CheckKeyInput( ioBaseChar *pOwner )
{
	if( !pOwner ) return;

	if( pOwner->IsSettedDirection() )
	{
		ioUserKeyInput::DirKeyInput eNewDirKey = pOwner->CheckCurDirKey();
		if( eNewDirKey != m_CurDirKey )
		{
			m_eInputDir = AD_NONE;

			float fYawD = 180.0f + 45.0f * ( eNewDirKey - ioUserKeyInput::DKI_UP );
			fYawD = ioMath::ArrangeHead( fYawD );

			float fYawR = DEGtoRAD( fYawD );
			D3DXQUATERNION qtRotate;
			D3DXQuaternionRotationYawPitchRoll( &qtRotate, fYawR, 0.0f, 0.0f );

			D3DXVECTOR3 vTargetDir = qtRotate * D3DXVECTOR3( 0.0f, 0.0f, -FLOAT1 );
			D3DXVECTOR3 vDir = pOwner->GetMoveDir();
			D3DXQUATERNION qtCurRot = pOwner->GetTargetRot();

			if( D3DXVec3Dot( &vTargetDir, &vDir ) > -FLOAT1 )
			{
				float fCurYaw, fTargetYaw;
				fCurYaw = RADtoDEG( ioMath::QuaterToYaw( qtCurRot ) );
				fTargetYaw = RADtoDEG( ioMath::QuaterToYaw( qtRotate ) );

				fCurYaw = ioMath::ArrangeHead( fCurYaw );
				fTargetYaw = ioMath::ArrangeHead( fTargetYaw );

				float fRotAngle = ioMath::AngleGap( fCurYaw, fTargetYaw );
				if( fRotAngle <= 0.0f )
					fRotAngle = 360.0f + fRotAngle;

				float fCheckAngle = 22.5f;
				int eInputDir = AD_UP;
				while( m_eInputDir == AD_NONE )
				{
					if( fRotAngle < fCheckAngle )
					{
						m_eInputDir = (AttackDir)eInputDir;
					}
					else
					{
						fCheckAngle += 45.0f;
						eInputDir++;
						if( eInputDir >= AD_MAX )
							eInputDir = AD_UP;
					}
				}
			}

			m_CurDirKey = eNewDirKey;
		}
	}
	else
	{
		if( m_CurDirKey != ioUserKeyInput::DKI_NONE )
		{
			m_CurDirKey = ioUserKeyInput::DKI_NONE;
		}
	}
}

void ioGrapplingAttack4Skill::GrapplingAttackFire( ioBaseChar *pChar )
{
	if( !pChar )	return;
	if( m_SkillState == SS_END )
		return;

	m_SkillState = SS_END;
	float fTimeRate  = FLOAT1;
	DWORD dwPreDelay = 0;
	DWORD dwEndDelay = 0;

	AttackInfo vAttackInfo;
	vAttackInfo.Init();

	switch( m_eInputDir )
	{
	case AD_NONE:
	case AD_UP:
		vAttackInfo = m_vAttackUp;
		break;
	case AD_DOWN:
		vAttackInfo = m_vAttackDown;
		break;
	case AD_LEFT:
		vAttackInfo = m_vAttackLeft;
		break;
	case AD_RIGHT:
		vAttackInfo = m_vAttackRight;
		break;
	case AD_UP_LEFT:
		vAttackInfo = m_vAttackUpLeft;
		break;
	case AD_DOWN_LEFT:
		vAttackInfo = m_vAttackDownLeft;
		break;
	case AD_UP_RIGHT:
		vAttackInfo = m_vAttackUpRight;
		break;
	case AD_DOWN_RIGHT:
		vAttackInfo = m_vAttackDownRight;
		break;
	}

	ioEntityGroup *pGrp = pChar->GetGroup();
	int iAniID = pGrp->GetAnimationIdx( vAttackInfo.m_vAttackAni );
	if( iAniID == -1 )	return;

	fTimeRate  = vAttackInfo.m_fAttackAniRate;
	dwPreDelay = vAttackInfo.m_dwPreDelay;
	dwEndDelay = vAttackInfo.m_dwEndDelay;

	pChar->AttachEffect( m_SkillEffectName );

	if( m_bRefreshFireTimeList )
	{
		pChar->RefreshFireTimeListWithBase( m_dwWeaponBaseIndex,
											iAniID,
											vAttackInfo.m_vWeaponInfoList,
											FTT_SKILL_ATTACK,
											fTimeRate,
											dwPreDelay );
	}

	pGrp->ClearAllLoopAni( FLOAT100, true );
	pGrp->ClearAllActionAni( FLOAT100, true );

	m_dwMotionStartTime = m_dwMotionEndTime = FRAMEGETTIME();
	m_dwMotionEndTime += pGrp->GetAnimationFullTime( iAniID ) * fTimeRate;
	m_dwMotionEndTime += dwPreDelay + dwEndDelay;

	pGrp->SetActionAni( iAniID, FLOAT100, FLOAT100, FLOAT1, FLOAT1/fTimeRate, 0.0f, dwPreDelay, true );

	// Col Skip
	pChar->CheckCharColSkipSkillTime( iAniID, fTimeRate, dwPreDelay );

	// Ani Rotate
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
			kInfo.m_dwTime += FRAMEGETTIME();
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

	m_vAttackDir = pChar->GetTargetRot() * D3DXVECTOR3( 0.0f, 0.0f , -FLOAT1);

	if( pChar->IsNeedSendNetwork() )
	{
		SP2Packet kPacket( CUPK_SKILL_EXTRA_INFO );
		kPacket << pChar->GetCharName();
		kPacket << GetName();
		kPacket << SSC_ATTACK;
		kPacket << pChar->GetTargetRot();
		kPacket << m_eInputDir;
		P2PNetwork::SendToAllPlayingUser( kPacket );
	}
}