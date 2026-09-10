

#include "stdafx.h"

#include "ioNpcChar.h"
#include "ioPlayStage.h"

#include "ioGrapplingAttack3Skill.h"

#include "WeaponDefine.h"

ioGrapplingAttack3Skill::ioGrapplingAttack3Skill()
{
	ClearData();
}

ioGrapplingAttack3Skill::ioGrapplingAttack3Skill( const ioGrapplingAttack3Skill &rhs )
: ioAttackSkill( rhs ),
m_vWeaponToAirList( rhs.m_vWeaponToAirList ),
m_vWeaponToDownList( rhs.m_vWeaponToDownList ),
m_dwSkillProtectTime( rhs.m_dwSkillProtectTime ),
m_dwFastValue( rhs.m_dwFastValue ),
m_dwGrapplingMaxValue( rhs.m_dwGrapplingMaxValue ),
m_dwEnableGrapplingCancelTime( rhs.m_dwEnableGrapplingCancelTime ),
m_szCancelMotion( rhs.m_szCancelMotion ),
m_szTargetCancelMotion( rhs.m_szTargetCancelMotion ),
m_dwEnableKeyTime( rhs.m_dwEnableKeyTime ),
m_iMaxCombo( rhs.m_iMaxCombo ),
m_vAttackAttributeList( rhs.m_vAttackAttributeList ),
m_vCancelForce( rhs.m_vCancelForce ),
m_vTargetCancelForce( rhs.m_vTargetCancelForce ),
m_fTargetAngle( rhs.m_fTargetAngle ),
m_fTargetRange( rhs.m_fTargetRange ),
m_fTargetMoveSpeed( rhs.m_fTargetMoveSpeed ),
m_fGapRange( rhs.m_fGapRange ),
m_GuideEffect( rhs.m_GuideEffect ),
m_iMaxEffectCount( rhs.m_iMaxEffectCount ),
m_TargetEndBuff( rhs.m_TargetEndBuff )
{
	ClearData();
}

ioGrapplingAttack3Skill::~ioGrapplingAttack3Skill()
{
}

void ioGrapplingAttack3Skill::ClearData()
{
	m_SkillState				= SS_NONE;
	m_GrapplingTargetState		= GTS_NONE;
	m_GrapplingTargetPosition	= GTP_NONE;
	m_bCancelGrappling = false;

	m_dwCheckKeyTime		= 0;
	m_dwGrapplingStartTime	= 0;
	m_dwGrapplingOrigStartTime	= 0;
	m_dwMotionStartTime		= 0;

	m_dwCurGrapplingValue = 0;

	m_iCurCombo = 0;

	m_bSetMotion = false;

	m_bPreLeft		= false;

	m_vTargetPosition = ioMath::VEC3_ZERO;
	m_TargetName.Clear();

	m_fTargetCurRange = 0.0f;

	m_vAttackDir = ioMath::VEC3_ZERO;

	m_fMaxEffectRange = 0.0f;
}

void ioGrapplingAttack3Skill::LoadProperty( ioINILoader &rkLoader )
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

	// 콤보
	m_iMaxCombo = rkLoader.LoadInt_e( "max_combo", 1 );

	// 기립잡기 타격정보
	m_vAttackAttributeList.clear();
	m_vAttackAttributeList.reserve( m_iMaxCombo );
	for( int i=0; i<m_iMaxCombo; ++i )
	{
		AttackAttribute kInfo;
		kInfo.Init();
		wsprintf_e( szKey, "grappling_attack%d", i+1 );
		LoadAttackAttribute( szKey, kInfo, rkLoader );
		m_vAttackAttributeList.push_back( kInfo );
	}

	// 상태별 잡기캔슬 모션
	m_dwFastValue = (DWORD)rkLoader.LoadInt_e( "cancel_fast_value", 0 );
	m_dwGrapplingMaxValue = (DWORD)rkLoader.LoadInt_e( "grappling_cancel_max_value", 0 );
	m_dwEnableGrapplingCancelTime = (DWORD)rkLoader.LoadInt_e( "enable_grappling_cancel_time", 0 );

	rkLoader.LoadString_e( "grappling_cancel_motion", "", szBuf, MAX_PATH );
	m_szCancelMotion = szBuf;
	rkLoader.LoadString_e( "target_grappling_cancel_motion", "", szBuf, MAX_PATH );
	m_szTargetCancelMotion = szBuf;

	_ENCSTR("grappling_cancel", szEncGrapplingCancel);
	_ENCSTR("target_grappling_cancel", szEncTargetGrapplingCancel);
	LoadForceInfo( szEncGrapplingCancel, m_vCancelForce, rkLoader);
	LoadForceInfo( szEncTargetGrapplingCancel, m_vTargetCancelForce, rkLoader);

	// 키 연타.
	m_dwEnableKeyTime = (DWORD)rkLoader.LoadInt_e( "enable_key_time", 0 );

	//
	int iBuffCnt = rkLoader.LoadInt_e( "blow_cancel_buff_cnt", 0 );
	for( int i=0; i < iBuffCnt; ++i )
	{
		wsprintf_e( szKey, "blow_cancel_buff%d", i+1 );
		rkLoader.LoadString( szKey, "", szBuf, MAX_PATH );

		m_SkillGrapplingInfo.m_SwingCancelBuff.push_back( szBuf );
	}

	m_fTargetAngle = rkLoader.LoadFloat_e( "target_angle", 0.0f );
	m_fTargetRange = rkLoader.LoadFloat_e( "target_range", 0.0f );

	m_fTargetMoveSpeed = rkLoader.LoadFloat_e( "target_move_speed", 0.0f );
	m_fGapRange = rkLoader.LoadFloat_e( "target_move_gap_range", 0.0f );

	// 가이드 이펙트
	rkLoader.LoadString_e( "guide_effect", "", szBuf, MAX_PATH );
	m_GuideEffect = szBuf;
	m_iMaxEffectCount = rkLoader.LoadInt_e( "guide_max_effect", 1 );

	rkLoader.LoadString_e( "target_end_buff", "", szBuf, MAX_PATH );
	m_TargetEndBuff = szBuf;
}

void ioGrapplingAttack3Skill::LoadForceInfo( const char *szForce, ForceInfoList &rkForceInfoList, ioINILoader &rkLoader )
{
	rkForceInfoList.clear();

	char szKey[MAX_PATH];

	wsprintf_e( szKey, "%s_force_cnt", szForce );
	int iForceCnt = rkLoader.LoadInt( szKey, 0 );
	if( iForceCnt > 0 )
		rkForceInfoList.reserve( iForceCnt );

	for( int i=0; i < iForceCnt; ++i )
	{
		ForceInfo kInfo;

		wsprintf_e( szKey, "%s_force_move%d", szForce, i+1 );
		kInfo.m_fForceMovePower = rkLoader.LoadFloat( szKey, 0.0f );

		wsprintf_e( szKey, "%s_force_friction%d", szForce, i+1 );
		kInfo.m_fForceMoveFriction = rkLoader.LoadFloat( szKey, FLOAT1 );

		wsprintf_e( szKey, "%s_force_angle%d", szForce, i+1 );
		kInfo.m_fForceAngle = rkLoader.LoadFloat( szKey, 0.0f );

		rkForceInfoList.push_back( kInfo );
	}
}

ioSkill* ioGrapplingAttack3Skill::Clone()
{
	return new ioGrapplingAttack3Skill( *this );
}

bool ioGrapplingAttack3Skill::OnSkillStart( ioBaseChar *pChar )
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
	DestroyArrow();

	return true;
}

void ioGrapplingAttack3Skill::OnProcessState( ioBaseChar *pChar, ioPlayStage *pStage )
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
					SetSkillCancelState( pChar, true );
					return;
				}
			}
			else
			{
				SetSkillCancelState( pChar, true );
				return;
			}

			if( m_iCurCombo < m_iMaxCombo )
			{
				if( pChar->IsNeedProcess() )
				{
					if( m_dwCheckKeyTime+m_dwEnableKeyTime < dwCurTime )
					{
						m_bCancelGrappling = true;
						SetSkillCancelState( pChar, true );
						return;
					}

					if( pChar->IsAttackKey() )
					{
						m_dwCheckKeyTime = dwCurTime;
						if( m_dwMotionEndTime < FRAMEGETTIME() )
							GrapplingAttackFire( pChar );
					}
				}
			}
			else
			{
				if( m_dwMotionEndTime < FRAMEGETTIME() )
					SetMoveTargetState( pChar );

				return;
			}

			ProcessGuideEffect( pChar, m_vTargetPosition );

			if( m_dwGrapplingOrigStartTime + m_dwEnableGrapplingCancelTime < dwCurTime )
				ProcessWoundGrapplingState( pChar, pStage );
		}
		break;
	case SS_CANCEL:
		{
			// GrapplingAttackFire()로 인한 타격동작과 곂쳐지지 않도록 약간의 딜레이를 둔다.
			if( m_dwMotionEndTime + g_App.GetGrapplingCheckDelay() < FRAMEGETTIME() )
			{
				CheckSkillGrapplingCancel( pChar );
				SetSkillEndState();
			}
		}
		break;
	case SS_MOVE_TARGET:
		{
			ProcessGuideEffect( pChar, m_vTargetPosition );
			ioBaseChar *pTarget = pChar->GetBaseChar( m_GrapplingTargetName );
			if( !pTarget )
			{
				SetSkillCancelState( pChar, true );
				return;
			}
			
			D3DXVECTOR3 vOwnerPos = pChar->GetWorldPosition();
			D3DXVECTOR3 vCurDir = vOwnerPos - m_vTargetPosition;
			D3DXVec3Normalize( &vCurDir, &vCurDir );

			float fTimePerSec = g_FrameTimer.GetSecPerFrame();
			float fMoveDist = m_fTargetMoveSpeed * fTimePerSec;

			m_fTargetCurRange += fMoveDist;

			if( m_fTargetCurRange < m_fTargetMaxRange )
			{
				m_vTargetPosition += (vCurDir * fMoveDist);
			}
			else
			{
				SetSkillCancelState( pChar, true );
			}
		}
		break;
	case SS_END:
		break;
	}
}

void ioGrapplingAttack3Skill::OnSkillEnd( ioBaseChar *pChar )
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

	DestroyArrow();
	ClearData();
}

void ioGrapplingAttack3Skill::ApplyExtraSkillInfo( ioBaseChar *pChar, ioPlayStage *pStage, SP2Packet &rkPacket )
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
			
			int iGrapplingState;
			rkPacket >> iGrapplingState;
			m_GrapplingTargetPosition = (GrapplingTargetPosition)iGrapplingState;
		}
		break;
	case SSC_GRAPPLING:
		{
			if( m_GrapplingTargetName.IsEmpty() )
			{
				m_dwGrapplingOrigStartTime = FRAMEGETTIME();
			}

			m_dwGrapplingStartTime	= FRAMEGETTIME();
			m_SkillState = SS_GRAPPLING;
			
			int iGrapplingTargetState;
			ioHashString szTargetName;

			rkPacket >> szTargetName;
			rkPacket >> iGrapplingTargetState;
			rkPacket >> m_SkillGrapplingInfo.m_bReverseTargetRot;
			rkPacket >> m_vTargetPosition;

			m_GrapplingTargetName = szTargetName;
			m_GrapplingTargetState = (GrapplingTargetState)iGrapplingTargetState;

			ioBaseChar *pTarget = pStage->GetBaseChar( m_GrapplingTargetName );
			if( pTarget && pTarget->IsNeedProcess() )
			{
				pTarget->SetSkillGrapplingState( pChar->GetCharName(), GetSkillWoundGrapplingInfo(), GetName() );

				m_vTargetPosition = pTarget->GetWorldPosition();
				D3DXVECTOR3 vDiff = pChar->GetWorldPosition() - pTarget->GetWorldPosition();
				m_fMaxEffectRange = D3DXVec3Length( &vDiff );
			}
		}
		break;
	case SSC_ATTACK:
		{
			GrapplingAttackFire( pChar );

			ioBaseChar *pTarget = pStage->GetBaseChar( m_GrapplingTargetName );
			if( pTarget && pTarget->IsNeedProcess() )
			{
				pTarget->SetSkillGrapplingState( pChar->GetCharName(), GetSkillWoundGrapplingInfo(), GetName() );
			}
		}
		break;
	case SSC_CANCEL:
		rkPacket >> m_bCancelGrappling;
		SetSkillCancelState( pChar, false );
		break;
	}
}

void ioGrapplingAttack3Skill::ApplyAttributeToWeapon( ioWeapon *pWeapon, ioPlayStage *pStage )
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

void ioGrapplingAttack3Skill::FindTarget( ioBaseChar *pOwner )
{
	if( !pOwner )	return;
	//if( !pOwner->IsNeedProcess() )	return;

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
			kPacket << (int)m_GrapplingTargetPosition;
			P2PNetwork::SendToAllPlayingUser( kPacket );
		}
	}
}

void ioGrapplingAttack3Skill::CheckTargetState( ioHashString &szTargetName )
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

bool ioGrapplingAttack3Skill::IsEnableTargetState( ioBaseChar *pTarget )
{
	if( !pTarget )
		return false;


	if( pTarget->IsSystemState() )
		return false;
		
	if( !pTarget->IsEnableTarget() ) 	
		return false;

	return true;
}

bool ioGrapplingAttack3Skill::SetSkillGrapplingState( ioBaseChar *pChar, const ioHashString &szTargetName )
{
	if( !pChar )	return false;
	if( m_SkillState == SS_END || m_SkillState == SS_CANCEL )
		return false;

	DWORD dwCurTime = FRAMEGETTIME();

	// 충돌처리 후 체크된 타겟을 m_GrapplingTargetName로 지정한다.
	if( m_GrapplingTargetName.IsEmpty() )
	{
		m_GrapplingTargetName = szTargetName;
		m_dwGrapplingOrigStartTime	= dwCurTime;
		m_dwCheckKeyTime			= dwCurTime;
	}
	else if( m_GrapplingTargetName != szTargetName )
	{
		return false;
	}

	m_dwGrapplingStartTime	= dwCurTime;

	m_SkillState = SS_GRAPPLING;

	ioBaseChar *pTarget = pChar->GetBaseChar( m_GrapplingTargetName );
	if( !pTarget ) return false;

	m_vTargetPosition = pTarget->GetWorldPosition();
	
	D3DXVECTOR3 vDiff = pChar->GetWorldPosition() - pTarget->GetWorldPosition();
	m_fMaxEffectRange = D3DXVec3Length( &vDiff );

	D3DXVECTOR3 vTargetDir = pTarget->GetTargetRot() * D3DXVECTOR3( 0.0f, 0.0f, -FLOAT1 );
	D3DXVECTOR3 vDir = pChar->GetTargetRot() * D3DXVECTOR3( 0.0f, 0.0f, -FLOAT1 );
	float fDotValue = D3DXVec3Dot( &vTargetDir, &vDir );
	float fAngle = RADtoDEG(acosf(fDotValue));

	if( fAngle >=FLOAT90)
	{
		m_GrapplingTargetState = GTS_STAND_FRONT;
		m_SkillGrapplingInfo.m_bReverseTargetRot = true;
	}
	else
	{
		m_GrapplingTargetState = GTS_STAND_BACK;
		m_SkillGrapplingInfo.m_bReverseTargetRot = false;
	}

	if( pChar->IsNeedSendNetwork() )
	{
		SP2Packet kPacket( CUPK_SKILL_EXTRA_INFO );
		kPacket << pChar->GetCharName();
		kPacket << GetName();
		kPacket << SSC_GRAPPLING;
		kPacket << pChar->GetTargetRot();
		kPacket << szTargetName;
		kPacket << (int)m_GrapplingTargetState;
		kPacket << m_SkillGrapplingInfo.m_bReverseTargetRot;
		kPacket << m_vTargetPosition;
		P2PNetwork::SendToAllPlayingUser( kPacket );
	}

	// for NPC
	bool bNPC = false;
	bool bAttackerNPC = false;
	if( ToNpcChar( pTarget ) )
	{
		bNPC = true;
	}

	if( ToNpcChar( pChar ) )
	{
		bAttackerNPC = true;
	}

	// local or 훈련소 or NPC
	if( !P2PNetwork::IsNetworkPlaying() || !pChar->GetCreator()->IsNetworkMode() || (bNPC || bAttackerNPC) )
	{
		D3DXQUATERNION qtCharRot = pChar->GetTargetRot();
		pTarget->SetSkillGrapplingState( pChar->GetCharName(), GetSkillWoundGrapplingInfo(), GetName() );
	}

	return true;
}

D3DXVECTOR3 ioGrapplingAttack3Skill::GetGrapplingTargetPos( ioBaseChar *pChar )
{
	if( m_vTargetPosition != ioMath::VEC3_ZERO )
		return m_vTargetPosition;

	return ioMath::VEC3_ZERO;
}

bool ioGrapplingAttack3Skill::IsSkillMotionEnd( DWORD dwActionStop, ioBaseChar* pOwner ) const
{
	DWORD dwCurTime = FRAMEGETTIME();

	if( m_dwPreDelayStartTime > 0 )
		return false;

	switch( m_SkillState )
	{
	case SS_GRAPPLING:
	case SS_CANCEL:
		return false;
	case SS_ACTION:
		if( m_dwMotionEndTime + 50 > dwCurTime )
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

bool ioGrapplingAttack3Skill::IsProtected( int iDefenseBreakType ) const
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

	if( m_dwGrapplingOrigStartTime>0 && m_dwGrapplingOrigStartTime+m_dwSkillProtectTime > dwCurTime )
	{
		return true;
	}

	if( m_dwPreDelayStartTime == 0 )
	{
		if( m_dwFireMotionEndTime > FRAMEGETTIME() )
			return true;
	}

	return false;
}

bool ioGrapplingAttack3Skill::IsEnableReserve() const
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

bool ioGrapplingAttack3Skill::CheckUseItemMesh()
{
	return m_bUseItemMesh;
}

void ioGrapplingAttack3Skill::AttackSkillFire( ioBaseChar *pChar )
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

void ioGrapplingAttack3Skill::SetSkillCancelState( ioBaseChar *pChar, bool bSendNet )
{
	if( !pChar ) return;

	m_SkillState = SS_CANCEL;

	if( pChar->IsNeedSendNetwork() && bSendNet )
	{
		SP2Packet kPacket( CUPK_SKILL_EXTRA_INFO );
		kPacket << pChar->GetCharName();
		kPacket << GetName();
		kPacket << SSC_CANCEL;
		kPacket << pChar->GetTargetRot();
		kPacket << m_bCancelGrappling;
		P2PNetwork::SendToAllPlayingUser( kPacket );
	}
}

void ioGrapplingAttack3Skill::SetMoveTargetState( ioBaseChar *pChar )
{
	if( !pChar )	return;
	if( m_fTargetMoveSpeed <= 0.0f )
	{
		SetSkillCancelState( pChar, true );
		return;
	}

	m_SkillState = SS_MOVE_TARGET;
	D3DXVECTOR3 vDiff = pChar->GetWorldPosition() - m_vTargetPosition;
	m_fTargetMaxRange = D3DXVec3Length( &vDiff ) - m_fGapRange;
}

void ioGrapplingAttack3Skill::SetSkillEndState()
{
	if( m_SkillState == SS_END )	return;

	DestroyArrow();
	m_SkillState = SS_END;
	if( m_iCurCombo >= m_iMaxCombo )
	{
		ioPlayStage *pStage = g_SkillMaker.GetPlayStage();
		if( pStage && !m_TargetEndBuff.IsEmpty() )
		{
			ioBaseChar *pTarget = pStage->GetBaseChar( m_GrapplingTargetName );
			if( pTarget )
			{
				pTarget->AddNewBuff( m_TargetEndBuff, "", "", NULL );
			}
		}
	}
}

void ioGrapplingAttack3Skill::GrapplingAttackFire( ioBaseChar *pChar )
{
	if( !pChar )	return;
	if( m_SkillState == SS_END || m_SkillState == SS_CANCEL )
		return;
	if( !COMPARE( m_iCurCombo, 0, m_iMaxCombo ) )
		return;

	float fTimeRate  = FLOAT1;
	DWORD dwPreDelay = 0;

	ioEntityGroup *pGrp = pChar->GetGroup();
	int iAniID = pGrp->GetAnimationIdx( m_vAttackAttributeList[m_iCurCombo].m_AttackAnimation );
	if( iAniID == -1 )	return;

	pChar->AttachEffect( m_SkillEffectName );

	fTimeRate  = m_vAttackAttributeList[m_iCurCombo].m_fAttackAniRate;
	dwPreDelay = m_vAttackAttributeList[m_iCurCombo].m_dwPreDelay;

	if( m_bRefreshFireTimeList )
	{
		pChar->RefreshFireTimeListWithBase( m_dwWeaponBaseIndex + m_iCurCombo,
											iAniID,
											m_vAttackAttributeList[m_iCurCombo].m_vWeaponInfoList,
											FTT_SKILL_ATTACK,
											fTimeRate,
											dwPreDelay );
	}

	pGrp->ClearAllLoopAni( FLOAT100, true );
	pGrp->ClearAllActionAni( FLOAT100, true );

	m_dwMotionStartTime = m_dwMotionEndTime = FRAMEGETTIME();
	m_dwMotionEndTime += pGrp->GetAnimationFullTime( iAniID ) * fTimeRate;
	m_dwMotionEndTime += dwPreDelay + m_vAttackAttributeList[m_iCurCombo].m_dwEndDelay;

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

	pChar->SetReservedSliding( m_vAttackAttributeList[m_iCurCombo].m_vForceInfoList, iAniID, fTimeRate, dwPreDelay );

	m_iCurCombo++;

	m_vAttackDir = pChar->GetTargetRot() * D3DXVECTOR3( 0.0f, 0.0f , -FLOAT1);

	if( pChar->IsNeedSendNetwork() )
	{
		SP2Packet kPacket( CUPK_SKILL_EXTRA_INFO );
		kPacket << pChar->GetCharName();
		kPacket << GetName();
		kPacket << SSC_ATTACK;
		kPacket << pChar->GetTargetRot();
		P2PNetwork::SendToAllPlayingUser( kPacket );
	}
}

void ioGrapplingAttack3Skill::CheckSkillGrapplingCancel( ioBaseChar *pChar )
{
	if( !pChar )
		return;
	if( m_GrapplingTargetName.IsEmpty()	)
		return;
	if( !m_bCancelGrappling )
		return;

	ioBaseChar *pTarget = pChar->GetBaseChar( m_GrapplingTargetName );
	ioEntityGroup *pTargetGrp = NULL;
	if( pTarget )
	{
		pTargetGrp = pTarget->GetGroup();

		int iAniID = pTargetGrp->GetAnimationIdx( m_szTargetCancelMotion );
		if( iAniID != -1 )
		{
			D3DXQUATERNION qtRot;
			D3DXVECTOR3 vDir = pChar->GetTargetRot() * D3DXVECTOR3( 0.0f, 0.0f, -FLOAT1 );
			ioMath::CalcDirectionQuaternion( qtRot, vDir );
			pTarget->SetTargetRotToRotate( qtRot, true, false );

			pTarget->SetWoundedState( m_szTargetCancelMotion, 0, FLOAT1, 0, false );
			pTarget->SetReservedSliding( m_vTargetCancelForce, iAniID, FLOAT1, 0 );
		}
	}

	ioEntityGroup *pGrp = pChar->GetGroup();
	if( pGrp )
	{
		pGrp->ClearAllActionAni( FLOAT100, true );
		pGrp->ClearAllLoopAni( FLOAT100, true );

		int iAniID = pGrp->GetAnimationIdx( m_szCancelMotion );
		if( iAniID != -1 )
		{
			pGrp->SetActionAni( iAniID, FLOAT100, FLOAT100, FLOAT1, FLOAT1 );

			m_dwMotionEndTime = FRAMEGETTIME();
			m_dwMotionEndTime += pGrp->GetAnimationFullTime( iAniID );

			pChar->SetReservedSliding( m_vCancelForce, iAniID, FLOAT1, 0 );
		}
	}
}

void ioGrapplingAttack3Skill::ProcessWoundGrapplingState( ioBaseChar *pChar, ioPlayStage *pStage )
{
	if( !pChar )	return;
	if( !pStage	)	return;
	if( m_GrapplingTargetName.IsEmpty() )
		return;

	if( m_bCancelGrappling )
		return;

	if( m_GrapplingTargetState == GTS_STAND_BACK )
		return;

	ioBaseChar *pTarget = pStage->GetBaseChar( m_GrapplingTargetName );

	if( pTarget && pTarget->IsNeedProcess() )
	{
		if( pTarget->IsPrisonerMode() )
		{
			m_bCancelGrappling = false;
			return;
		}

		if(	( !m_bPreLeft && pTarget->CheckCurDirKey() == ioUserKeyInput::DKI_LEFT ) ||
			( m_bPreLeft && pTarget->CheckCurDirKey() == ioUserKeyInput::DKI_RIGHT ) )			
		{
			IORandom eRandom;
			eRandom.Randomize();

			DWORD dwCurFastValue = eRandom.Random( m_dwFastValue );
			dwCurFastValue++;

			m_dwCurGrapplingValue += dwCurFastValue;

			m_bPreLeft ^= true;
		}

		if( m_dwCurGrapplingValue >= m_dwGrapplingMaxValue )
		{
			// cancel
			m_bCancelGrappling = true;

			SetSkillCancelState( pChar, false );
			if( pTarget->IsNeedSendNetwork() )
			{
				SP2Packet kPacket( CUPK_SKILL_EXTRA_INFO );
				kPacket << pChar->GetCharName();
				kPacket << GetName();
				kPacket << SSC_CANCEL;
				kPacket << pChar->GetTargetRot();
				kPacket << m_bCancelGrappling;
				P2PNetwork::SendToAllPlayingUser( kPacket );
			}
			return;
		}
	}
}

bool ioGrapplingAttack3Skill::isEnableCancel()
{
	if( m_GrapplingTargetState == GTS_STAND_BACK )
		return false;

	return true;
}

void ioGrapplingAttack3Skill::ProcessGuideEffect( ioBaseChar *pOwner, D3DXVECTOR3 vTargetPos )
{
	if( !pOwner )	return;
	if( vTargetPos == ioMath::VEC3_ZERO )
		return;
	if( !CheckArrowCreated( pOwner ) )
		return;

	ArrangeArrows( pOwner, vTargetPos );
}

void ioGrapplingAttack3Skill::DestroyArrow()
{
	ioPlayStage *pStage = g_SkillMaker.GetPlayStage();
	if( !pStage )	return;
	ArrowList::iterator iter = m_EffectList.begin();
	for( ; iter!=m_EffectList.end() ; ++iter )
	{
		ioMapEffect *pMapEffect = pStage->FindMapEffect( *iter );
		if( pMapEffect )
			pMapEffect->EndEffectForce();
	}
	m_EffectList.clear();
}

bool ioGrapplingAttack3Skill::CheckArrowCreated( ioBaseChar *pOwner )
{
	if( !pOwner )	return false;
	if( !m_EffectList.empty() )
		return true;

	ioPlayStage *pStage = pOwner->GetCreator();
	if( !pStage )	return false;

	D3DXVECTOR3 vScale( FLOAT1, FLOAT1, FLOAT1 );

	m_EffectList.reserve( m_iMaxEffectCount );
	for( int i=0 ; i<m_iMaxEffectCount ; i++ )
	{
		ioMapEffect *pEffect = pStage->CreateMapEffect( m_GuideEffect, vScale );
		if( pEffect )
		{
			m_EffectList.push_back( pEffect->GetUniqueID() );
		}
	}

	return !m_EffectList.empty();
}

void ioGrapplingAttack3Skill::ArrangeArrows( ioBaseChar *pOwner, D3DXVECTOR3 vTargetPos )
{
	if( !pOwner )	return;
	
	ioPlayStage *pStage = pOwner->GetCreator();
	if( !pStage )	return;
	
	if( m_iMaxEffectCount <= 0 || m_EffectList.empty() )
		return;
	
	ioBaseChar *pTarget = pOwner->GetBaseChar( m_GrapplingTargetName );
	if( !pTarget )	return;

	D3DXVECTOR3 vStartPos = pOwner->GetMidPositionByRate();
	
	vTargetPos = pTarget->GetMidPositionByRate();
	D3DXVECTOR3 vDiff = vTargetPos - vStartPos;

	float fTargetLength = D3DXVec3Length( &vDiff );
	D3DXVec3Normalize( &vDiff, &vDiff );

	// Update Arrow Pos and Rotation
	D3DXVECTOR3 vCurPos = vStartPos;
	float fGapLength = min( m_fMaxEffectRange, fTargetLength ) / m_iMaxEffectCount;

	float fCurRate = FLOAT1;
	if( m_fMaxEffectRange > 0.0f )
		fCurRate = fTargetLength / m_fMaxEffectRange;

	float fCurScale = max( 0.1f, fCurRate );

	ArrowList::iterator iter = m_EffectList.begin();
	for( ; iter!=m_EffectList.end() ; ++iter )
	{
		vCurPos += vDiff * fGapLength;

		ioMapEffect *pMapEff = pStage->FindMapEffect( *iter );
		if( pMapEff )
		{
			pMapEff->SetWorldPosition( vCurPos );

			ioEffect *pEff = pMapEff->GetEffect();
			if( pEff )
			{
				D3DXVECTOR3 vScale( FLOAT1, FLOAT1, fCurScale );
				pEff->GetParentNode()->SetScale( vScale );

				D3DXQUATERNION qtRot = Help::ConvertDirToQuaternion( vDiff );
				pEff->GetParentNode()->SetOrientation( qtRot );
			}
		}
	}
}