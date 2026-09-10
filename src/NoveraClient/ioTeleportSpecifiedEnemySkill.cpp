

#include "stdafx.h"

#include "ioTeleportSpecifiedEnemySkill.h"

#include "ioBaseChar.h"
#include "ioPlayStage.h"
#include "WeaponDefine.h"
#include "ioCircleTraceDummy.h"
#include "ioCircleTraceDummy2.h"

ioTeleportSpecifiedEnemySkill::ioTeleportSpecifiedEnemySkill()
{
	m_State = SS_NONE;

	m_bSetHandMesh = false;

	m_dwFireStartTime = 0;
	m_dwActionStartTime = 0;
	m_dwLoopStartTime = 0;

	m_iCurWeaponCnt = 0;

	m_vTargetInfoList.clear();

	m_bChangeTargetPrev = false;
	m_bChangeTargetNext = false;

	m_dwCheckAimTime = 0;
	m_dwMakerID = -1;

	m_vTargetCharPos = ioMath::VEC3_ZERO;

	m_pTargetChar = NULL;

	m_dwCurAimLoopTime = 0;
	m_dwCurMagicCircleMaintenanceTime = 0;
}

ioTeleportSpecifiedEnemySkill::ioTeleportSpecifiedEnemySkill( const ioTeleportSpecifiedEnemySkill &rhs )
: ioNormalSkill( rhs ),
 m_szEnableCircle( rhs.m_szEnableCircle ),
 m_dwEnableKeyTime( rhs.m_dwEnableKeyTime ),
 m_fCircleRange( rhs.m_fCircleRange ),
 m_fCircleMoveSpeed( rhs.m_fCircleMoveSpeed ),
 m_fCircleHeightGap( rhs.m_fCircleHeightGap ),
 m_fCircleOffSet( rhs.m_fCircleOffSet ),
 m_SkillEndMotion( rhs.m_SkillEndMotion ),
 m_fEndMotionRate( rhs.m_fEndMotionRate ),
 m_StartEffect( rhs.m_StartEffect ),
 m_EndEffect( rhs.m_EndEffect ),
 m_RedGuidCircle( rhs.m_RedGuidCircle ),
 m_BlueGuidCircle( rhs.m_BlueGuidCircle ),
 m_LoopAnimation( rhs.m_LoopAnimation ),
 m_dwLoopProtectTime( rhs.m_dwLoopProtectTime ),
 m_iMaxWeaponCnt( rhs.m_iMaxWeaponCnt ),
 m_OnCircle( rhs.m_OnCircle ),
 m_OffCircle( rhs.m_OffCircle ),
 m_szStampCircleEffect( rhs.m_szStampCircleEffect ),
 m_dwAttackAttribute( rhs.m_dwAttackAttribute ),
 m_vWeaponOffset( rhs.m_vWeaponOffset ),
 m_CreateWeaponStyle( rhs.m_CreateWeaponStyle ),
 m_stDummyName( rhs.m_stDummyName ),
 m_bUseEnableMagicCircle( rhs.m_bUseEnableMagicCircle ),
 m_dwMagicCircleMaintenanceTime( rhs.m_dwMagicCircleMaintenanceTime ),
 m_bUseDestroyDummy( rhs.m_bUseDestroyDummy ),
 m_szAimedMarker( rhs.m_szAimedMarker),
 m_dwAimedTime( rhs.m_dwAimedTime ),
 m_dwAimLoopTime( rhs.m_dwAimLoopTime ),
 m_fAimRange( rhs.m_fAimRange ),
 m_szAimLoopAni( rhs.m_szAimLoopAni ),
 m_fAimLoopAniRate( rhs.m_fAimLoopAniRate )
{
	m_State = SS_NONE;

	m_bSetHandMesh = false;

	m_dwFireStartTime = 0;
	m_dwActionStartTime = 0;
	m_dwLoopStartTime = 0;

	m_iCurWeaponCnt = 0;

	m_CurCircleEffect = m_OnCircle;

	m_bOffCircle = false;
	m_bEnableSummon = true;

	m_vTargetInfoList.clear();

	m_bChangeTargetPrev = false;
	m_bChangeTargetNext = false;

	m_dwCheckAimTime = 0;
	m_dwMakerID = -1;

	m_vTargetCharPos = ioMath::VEC3_ZERO;

	m_pTargetChar = NULL;

	m_dwCurAimLoopTime = 0;
	m_dwCurMagicCircleMaintenanceTime = 0;
}

ioTeleportSpecifiedEnemySkill::~ioTeleportSpecifiedEnemySkill()
{
}

void ioTeleportSpecifiedEnemySkill::LoadProperty( ioINILoader &rkLoader )
{
	ioNormalSkill::LoadProperty( rkLoader );

	char szBuf[MAX_PATH];
	rkLoader.LoadString_e( "enable_circle", "", szBuf, MAX_PATH );
	m_szEnableCircle = szBuf;

	m_dwEnableKeyTime = (DWORD)rkLoader.LoadInt_e( "enable_key_time", 0 );
	m_fCircleRange = rkLoader.LoadFloat_e( "magic_circle_range", 0.0f );
	m_fCircleMoveSpeed = rkLoader.LoadFloat_e( "magic_circle_move_speed", 0.0f );
	m_fCircleHeightGap = rkLoader.LoadFloat_e( "magic_circle_height_gap", 0.0f );
	m_fCircleOffSet = rkLoader.LoadFloat_e( "magic_circle_offset", 0.0f );

	//////////////////////////////////////////////////////////////////////////
	rkLoader.LoadString_e( "move_key_on_magic_circle", "", szBuf, MAX_PATH );
	m_OnCircle = szBuf;
	rkLoader.LoadString_e( "no_move_key_on_magic_circle", "", szBuf, MAX_PATH );
	m_OffCircle = szBuf;

	rkLoader.LoadString_e( "stamp_magic_circle", "", szBuf, MAX_PATH );
	m_szStampCircleEffect = szBuf;
	//////////////////////////////////////////////////////////////////////////
	rkLoader.LoadString_e( "skill_loop_motion", "", szBuf, MAX_PATH );
	m_LoopAnimation = szBuf;

	m_dwLoopProtectTime = (DWORD)rkLoader.LoadInt_e( "loop_protect_time", 0 );

	rkLoader.LoadString_e( "red_guid_circle", "", szBuf, MAX_PATH );
	m_RedGuidCircle = szBuf;
	rkLoader.LoadString_e( "blud_guid_circle", "", szBuf, MAX_PATH );
	m_BlueGuidCircle = szBuf;

	rkLoader.LoadString_e( "skill_end_motion", "", szBuf, MAX_PATH );
	m_SkillEndMotion = szBuf;
	m_fEndMotionRate = rkLoader.LoadFloat_e( "end_motion_rate", FLOAT1 );

	rkLoader.LoadString_e( "start_effect", "", szBuf, MAX_PATH );
	m_StartEffect = szBuf;
	rkLoader.LoadString_e( "end_effect", "", szBuf, MAX_PATH );
	m_EndEffect = szBuf;

	m_dwAttackAttribute = rkLoader.LoadInt_e( "weapon_attribute", 0 );
	m_iMaxWeaponCnt = rkLoader.LoadInt_e( "max_weapon_cnt", 0 );
	m_vWeaponOffset.clear();
	m_vWeaponOffset.reserve( max(m_iMaxWeaponCnt, 1) );
	char szKey[MAX_PATH];
	for( int i = 1; i <=m_iMaxWeaponCnt; i++  )
	{
		wsprintf_e( szKey, "weaponOffset%d", i );
		int nValue = rkLoader.LoadInt( szKey, 0 );;
		m_vWeaponOffset.push_back( nValue );
	}

	//////////////////////////////////////////////////////////////////////////
	m_CreateWeaponStyle = (CreateWeaponStyle)rkLoader.LoadInt_e( "create_weapon_style", CWS_NORMAL );
	//////////////////////////////////////////////////////////////////////////
	rkLoader.LoadString_e( "create_dummy_name", "", szBuf, MAX_PATH );
	m_stDummyName = szBuf;

	m_bUseEnableMagicCircle = rkLoader.LoadBool_e( "use_enable_magic_circle", false );

	m_bUseDestroyDummy = rkLoader.LoadBool_e( "use_destroy_dummy", false );

	// 처음 타겟 지정
	m_dwAimLoopTime = (DWORD)rkLoader.LoadInt_e( "aim_loop_time", 0 );
	rkLoader.LoadString_e( "aimed_marker", "", szBuf, MAX_PATH );
	m_szAimedMarker = szBuf;
	m_dwAimedTime = rkLoader.LoadInt_e( "aimed_time", 0 );

	m_fAimRange = rkLoader.LoadFloat_e( "aim_range", 0.0f );

	rkLoader.LoadString_e( "aim_loop_ani", "", szBuf, MAX_PATH );
	m_szAimLoopAni = szBuf;

	m_fAimLoopAniRate = rkLoader.LoadFloat_e( "aim_loop_ani_rate", FLOAT1 );

	// 마법진 유지 시간
	m_dwMagicCircleMaintenanceTime = (DWORD)rkLoader.LoadInt_e( "magic_circle_loop_time", 0 );
}

ioSkill* ioTeleportSpecifiedEnemySkill::Clone()
{
	return new ioTeleportSpecifiedEnemySkill( *this );
}

bool ioTeleportSpecifiedEnemySkill::OnSkillStart( ioBaseChar *pChar )
{
	if( !HasSkillMotion() )
		return false;

	m_dwEnableSwitchTime = 0;
	m_dwEnableSemiSwitchTime = 0;

	if( m_dwSwitchSkill > 0 )
		m_dwEnableSwitchTime = FRAMEGETTIME() + m_dwSwitchSkill;
	if( m_dwSemiSwitchSkill > 0 )
		m_dwEnableSemiSwitchTime = FRAMEGETTIME() + m_dwSemiSwitchSkill;

	m_dwSkillStartTime = FRAMEGETTIME();

	pChar->AttachEffect( m_SkillEffectName );

	// 기본 무기 손에서 제거
	if( m_bUseItemMesh )
	{
		pChar->ShowWeaponItemMesh( false );
	}

	m_bSetHandMesh = false;
	m_dwFireStartTime = 0;
	m_dwActionStartTime = 0;
	m_dwLoopStartTime = 0;

	m_iCurWeaponCnt = 0;

	m_vWeaponPosList.clear();
	m_vWeaponPosList.reserve(m_iMaxWeaponCnt);

	m_State = SS_NONE;

	m_dwGapTime = FRAMEGETTIME();

	// aim 유지 시간 셋팅
	m_dwCurAimLoopTime = m_dwGapTime + m_dwAimLoopTime;

	pChar->SetSkillProtection();

	if( !StartPreDelayAnimation( pChar ) )	// Delay Animation이 없다
	{
		SetLoopState( pChar );
	}

	SetCameraBuff( pChar );

	CheckSkillDesc( pChar );

	if( m_bInitJumpPower )
	{
		pChar->SetCurJumpPowerAmt( 0.0f );
		pChar->SetGravityAmt( 0.0f );
	}

	SetSkillNameChatBubble( pChar );

	//////////////////////////////////////////////////////////////////////////
	m_bSafeCirclePos = true;

	//기존에 이미 소환된 더미가 있으면 삭제
	if( m_bUseDestroyDummy )
		g_DummyCharMgr.DestroyDummyCharByName( pChar, m_stDummyName );

	return true;
}

void ioTeleportSpecifiedEnemySkill::OnSkillEnd( ioBaseChar *pChar )
{
	ioNormalSkill::OnSkillEnd( pChar );

	// 기본무기 장착
	if( m_bUseItemMesh )
	{
		pChar->ShowWeaponItemMesh( true );
	}

	ioPlayStage *pStage = g_SkillMaker.GetPlayStage();

	// 스킬 중간 동작에 사용된 메쉬들 제거
	m_bSetHandMesh = false;
	if( !m_HandMesh.IsEmpty() )
		pChar->RestoreSkillWeaponMesh( this, m_HandMesh, m_bVisibleEquipMesh );

	EndCameraBuff( pChar );

	pChar->SetMagicCircleTarget( "" );

	pChar->DestroyMagicCircle();

	DestroyEffect( pChar, pStage, m_dwMakerID );

	m_dwCurAimLoopTime = 0;
	m_dwCurMagicCircleMaintenanceTime = 0;
	DestroyEffect( pChar, pStage, m_dwMakerID );

	m_dwMotionEndTime = 0;
	m_dwMotionStartTime = 0;
	m_iCurWeaponCnt = 0;
	m_State = SS_NONE;
	m_dwMakerID = -1;

	m_pTargetChar = NULL;
	m_vTargetCharPos = ioMath::VEC3_ZERO;

	m_vWeaponPosList.clear();
}

void ioTeleportSpecifiedEnemySkill::OnProcessState( ioBaseChar *pChar, ioPlayStage *pStage )
{
	float fValue = 0.0f;
	DWORD dwCurTime = FRAMEGETTIME();

	switch( m_State )
	{
	case SS_NONE:
		{
			DWORD dwPreDelay = GetPreDelayTime();
			if( m_dwPreDelayStartTime + dwPreDelay < dwCurTime )
			{
				m_dwPreDelayStartTime = 0;
				
				SetLoopState( pChar );
			}
		}
		break;
	case SS_TARGET_SELECT_LOOP:
		{
			GetTargetList( pChar, pStage );
			SetTargetRotation( pChar );
			UpdateMaker( pChar, pStage );

			if ( CheckInputChangeTarget( pChar ) )
			{
				DestroyEffect( pChar, pStage, m_dwMakerID );
				return;
			}

			// 스킬 키 재입력 또는 타겟이 없는 경우 공격키 누르면 스킬 상태 해제
			int iSkillNum = pChar->GetSkillKeyInput();
			if( iSkillNum == 1 || ( pChar->IsAttackKey() && m_szAimedTarget.IsEmpty() ) )
				SetEndState( pChar );

			if ( CheckInputAttack( pChar ) && !m_szAimedTarget.IsEmpty() )
			{
				m_State = SS_MAGIC_CIRCLE_LOOP;
				m_pTargetChar = pStage->GetBaseChar( m_szAimedTarget );
				if( m_pTargetChar )
					m_vTargetCharPos = m_pTargetChar->GetMidPositionByRate();
				
				SetMagicCircle( pChar );

				// 마법진 유지 시간 체크 시작
				m_dwCurMagicCircleMaintenanceTime = m_dwMagicCircleMaintenanceTime + dwCurTime;

				if( pChar->IsNeedSendNetwork() )
				{
					SP2Packet kPacket( CUPK_SKILL_EXTRA_INFO );
					kPacket << pChar->GetCharName();
					kPacket << GetName();
					kPacket << SSS_MAGIC_CIRCLE;
					kPacket << m_szAimedTarget;
					P2PNetwork::SendToAllPlayingUser( kPacket );
				}
				m_szAimedTarget.Clear();
			}

			// 유지시간 체크
			if( m_dwCurAimLoopTime > 0 && m_dwCurAimLoopTime < dwCurTime )
			{
				m_dwCurAimLoopTime = 0;
				DestroyEffect( pChar, pStage, m_dwMakerID );
				SetEndState( pChar );
			}
		}
		break;
	case SS_MAGIC_CIRCLE_LOOP:
		{
			if( m_bUseEnableMagicCircle )
				ChangeCircleDropZoneAndPositionGap( pChar, pStage );

			int nSize = m_vWeaponPosList.size();
			float fCurRange = 0.f;
			if( m_vWeaponPosList.size() < m_vWeaponOffset.size() )
				fCurRange = m_vWeaponOffset[ nSize ];

			// 스킬 키 재입력 시 스킬 상태 해제
			int iSkillNum = pChar->GetSkillKeyInput();
			if( iSkillNum == 1 )
				SetEndState( pChar );

			if( !m_pTargetChar )
				return;

			// 뱡향 변화 체크
			pChar->CheckChangeDirectionForMagicCircle();

			// MagicCircle 위치 갱신
			D3DXVECTOR3 vCharPos = m_pTargetChar->GetWorldPosition();
			if( m_vWeaponPosList.size() )
			{
				pChar->UpdateMagicCircleToCirclePosAll( m_fCircleMoveSpeed, fCurRange, m_fCircleOffSet, 0, 0, 0, 
					m_vWeaponPosList[ m_vWeaponPosList.size()-1] , false, false );
			}
			else
			{
				pChar->UpdateMagicCircleToCirclePosAll( m_fCircleMoveSpeed, fCurRange, m_fCircleOffSet, 0, 0, 0, vCharPos, false, false );
			}

			// MagicCircle 위치방향으로 캐릭터 회전
			D3DXVECTOR3 vTargetPos = pChar->GetMagicCirclePos();
			pChar->SetTargetRotToTargetPos( vTargetPos, true, false );

			if( pChar->IsNeedProcess() )
			{
				if( pChar->IsAttackKey() && m_bEnableSummon )
				{
					AddPosition( pChar, pStage );
					SetActionState( pChar, pStage );
				}
			}

			// 유지시간 체크
			if( m_dwCurMagicCircleMaintenanceTime > 0 && m_dwCurMagicCircleMaintenanceTime < dwCurTime )
			{
				m_dwCurMagicCircleMaintenanceTime = 0;
				DestroyEffect( pChar, pStage, m_dwMakerID );
				SetEndState( pChar );
			}
		}
		break;
	case SS_ACTION:
		if( m_dwMotionEndTime < dwCurTime )
		{
			SetEndState( pChar );
		}
		else if( m_dwFireStartTime > 0 && m_dwFireStartTime < FRAMEGETTIME() )
		{
			if ( m_CreateWeaponStyle == CWS_CIRCLE_TRACE_DUMMY )
			{
				// 더미 Index 동기화를 위해
				if ( pChar->IsNeedProcess() )
				{
					int iCurIndex = g_DummyCharMgr.GetCurLocalIndex();
					CreateCircleTraceDummy( iCurIndex, pChar );
				}
			}

			m_dwFireStartTime = 0;
			DestroyEffect( pChar, pStage, m_dwMakerID );
		}
		break;
	case SS_END:
		break;
	}
}

bool ioTeleportSpecifiedEnemySkill::StartPreDelayAnimation( ioBaseChar *pChar )
{
	if( !ioNormalSkill::StartPreDelayAnimation( pChar ) )
		return false;

	return true;
}

void ioTeleportSpecifiedEnemySkill::SetMagicCircle( ioBaseChar *pChar )
{
	ioHashString szDummyCircle;
	if( !pChar->CheckMagicCircle() )
	{		
		pChar->CreateMagicCircle( m_OnCircle, m_OnCircle, szDummyCircle, m_vTargetCharPos, m_fCircleOffSet );
	}
	else
	{
		D3DXVECTOR3 vTargetPos = pChar->GetMagicCirclePos();
		pChar->CreateMagicCircle( m_OnCircle, m_OnCircle, szDummyCircle, m_fCircleOffSet );
		pChar->SetMagicCirclePos(false, vTargetPos);
	}
	
}

void ioTeleportSpecifiedEnemySkill::SetNormalSkill( ioBaseChar *pChar )
{
	ioEntityGroup *pGrp = pChar->GetGroup();
	ioHashString szSkillMotion = GetSkillMotionName();

	int iCurSkillAniID = pGrp->GetAnimationIdx( szSkillMotion );
	if( iCurSkillAniID == -1 )	return;

	DWORD dwCurTime = FRAMEGETTIME();

	float fAniRate = GetSkillMotionRate();
	float fTimeRate = fAniRate * m_fExtraAniRate;
	
	pGrp->ClearAllLoopAni( FLOAT100, true );
	pGrp->ClearAllActionAni( FLOAT100, true );
	pGrp->SetActionAni( iCurSkillAniID, FLOAT100, FLOAT100, FLOAT1, FLOAT1/fTimeRate );
	
	m_dwMotionStartTime = dwCurTime;
	DWORD dwDuration = pGrp->GetAnimationFullTime( iCurSkillAniID ) * fTimeRate;
	m_dwMotionEndTime = m_dwMotionStartTime + dwDuration;
	m_dwMotionEndTime += m_dwAnimationEndDelay;

	m_dwFireStartTime = m_dwFireMotionEndTime = m_dwActionStartTime = dwCurTime;
	m_dwFireStartTime += pGrp->GetAnimationEventTime_e( iCurSkillAniID, "fire_s" ) * fTimeRate;
	m_dwFireMotionEndTime += pGrp->GetAnimationEventTime_e( iCurSkillAniID, "firemotion_e" ) * fTimeRate;

	if( m_dwFireStartTime >= m_dwFireMotionEndTime )
		m_dwFireMotionEndTime = m_dwMotionEndTime;

	pChar->AttachEffect( m_StartEffect );
}

void ioTeleportSpecifiedEnemySkill::SetActionState( ioBaseChar *pChar, ioPlayStage *pStage )
{
	m_State = SS_ACTION;
	float fCurRange = m_fCircleRange;

	m_dwPreDelayStartTime = 0;
	if( pChar->CheckMagicCircle() )
	{
		pChar->UpdateMagicCirclePos( m_fCircleMoveSpeed, fCurRange, m_fCircleHeightGap, true, true );
	}
	
	SetNormalSkill( pChar );
	int nSkillPointSize = m_vWeaponPosList.size();

	if( pChar->IsNeedSendNetwork() )
	{
		SP2Packet kPacket( CUPK_SKILL_EXTRA_INFO );
		kPacket << pChar->GetCharName();
		kPacket << GetName();
		kPacket << SSS_ACTION;
		kPacket << nSkillPointSize;
		for( int i = 0; i < nSkillPointSize; i++ )//중간 참여자를 위해서 결과는 다시 처음부터 보낸다.
			kPacket << m_vWeaponPosList[i];
		
		if( m_CreateWeaponStyle == CWS_DUMMY_CREATE )
		{
			int iCurIndex = g_DummyCharMgr.GetCurLocalIndex();
			kPacket << iCurIndex;
		}
		P2PNetwork::SendToAllPlayingUser( kPacket );
	}
}

void ioTeleportSpecifiedEnemySkill::SetLoopState( ioBaseChar *pChar )
{
	if( m_LoopAnimation.IsEmpty() || !pChar )
	{
		m_dwLoopStartTime = 0;
		return;
	}

	// 사용 아이템 처리
	if( !m_bSetHandMesh && m_bUseItemMesh && !m_HandMesh.IsEmpty() )
	{
		pChar->ChangeSkillWeaponMesh( this, m_HandMesh, m_bVisibleEquipMesh );
		m_bSetHandMesh = true;
	}

	ioEntityGroup *pGrp = pChar->GetGroup();
	pGrp->ClearAllLoopAni( FLOAT100, true );
	pGrp->ClearAllActionAni( FLOAT100, true );
	pGrp->SetLoopAni( m_LoopAnimation, FLOAT100 );

	m_State = SS_TARGET_SELECT_LOOP;
	m_dwLoopStartTime = FRAMEGETTIME();

	EndPreDelayEffect( pChar );
}


void ioTeleportSpecifiedEnemySkill::SetEndState( ioBaseChar *pChar )
{
	if( m_State == SS_END )
		return;

	DWORD dwCurTime = FRAMEGETTIME();

	m_State = SS_END;
	if( pChar )
		pChar->DestroyMagicCircle();

	if( m_SkillEndMotion.IsEmpty() )
	{
		m_dwMotionStartTime = dwCurTime;
		m_dwMotionEndTime = m_dwMotionStartTime;
		return;
	}

	ioEntityGroup *pGrp = pChar->GetGroup();
	int iCurSkillAniID = pGrp->GetAnimationIdx( m_SkillEndMotion );
	if( iCurSkillAniID == -1 )	return;

	float fCurEndRate = m_fEndMotionRate;
	if( fCurEndRate <= 0.0f )
		fCurEndRate = FLOAT1;

	pGrp->ClearAllLoopAni( FLOAT100, true );
	pGrp->ClearAllActionAni( FLOAT100, true );
	pGrp->SetActionAni( iCurSkillAniID, FLOAT100, FLOAT100, FLOAT1/fCurEndRate );

	m_dwMotionStartTime = dwCurTime;
	m_dwMotionEndTime = m_dwMotionStartTime + pGrp->GetAnimationFullTime( iCurSkillAniID ) * fCurEndRate;
	m_dwMotionEndTime += m_dwAnimationEndDelay;

	pChar->AttachEffect( m_EndEffect );

	ioPlayStage* pStage = pChar->GetCreator();

	m_szAimedTarget.Clear();

	if( !m_bReduceGauge )
	{
		pChar->ReduceNeedGaugeBySkill( this, GetNeedGauge(pChar->GetCharName()) );
		m_bReduceGauge = true;
	}

	if( pStage )
	{
		DestroyEffect( pChar, pStage, m_dwMakerID );

		m_dwMakerID = -1;
	}
}

void ioTeleportSpecifiedEnemySkill::AddPosition( ioBaseChar *pChar, ioPlayStage *pStage )
{
	if( !pChar )	return;
	if( !pStage )	return;

	if( m_iCurWeaponCnt < m_iMaxWeaponCnt )
	{
		m_vCirclePos = pChar->GetMagicCirclePos( m_fCircleOffSet, m_bCircleNoChangeY );
		m_vCirclePos.y = m_vTargetCharPos.y;
		m_vWeaponPosList.push_back( m_vCirclePos );
	}

	m_iCurWeaponCnt++;
}

void ioTeleportSpecifiedEnemySkill::SetTargetEffect( ioBaseChar *pChar, ioPlayStage *pStage, D3DXVECTOR3 vPos )
{
	if( !pChar )	return;
	if( !pStage )	return;

	if ( m_dwMakerID != -1 )
	{
		DestroyEffect( pChar, pStage, m_dwMakerID );

		m_dwMakerID = CreateEffect( pChar, pStage, m_szAimedMarker, vPos, D3DXVECTOR3(1.0f,1.0f,1.0f) );
	}
	else if ( m_dwMakerID == -1 )
	{
		m_dwMakerID = CreateEffect( pChar, pStage, m_szAimedMarker, vPos, D3DXVECTOR3(1.0f,1.0f,1.0f) );
	}
}

void ioTeleportSpecifiedEnemySkill::ApplyExtraSkillInfo( ioBaseChar *pChar, ioPlayStage *pStage, SP2Packet &rkPacket )
{
	int iSyncState;
	rkPacket >> iSyncState;

	switch( iSyncState )
	{
	case SSS_ASSIGN_TARGET:
		{
			D3DXVECTOR3 vPos;
			rkPacket >> m_szAimedTarget;
			rkPacket >> vPos;

			m_State = SS_TARGET_SELECT_LOOP;
			
			SetTargetRotation( pChar );
			SetTargetEffect( pChar, pStage, vPos );
		}
		break;
	case SSS_ACTION:
		{
			m_vWeaponPosList.clear();
			int nCnt = 0;
			rkPacket >> nCnt;

			m_State = SS_MAGIC_CIRCLE_LOOP;

			m_iCurWeaponCnt = nCnt;
			for( int i = 0; i < nCnt; i++ )
			{
				D3DXVECTOR3 vPos;
				rkPacket >> vPos;
				m_vWeaponPosList.push_back( vPos );
			}
			if( m_CreateWeaponStyle == CWS_DUMMY_CREATE )
			{
				int iCurIndex;
				rkPacket >> iCurIndex;
				g_DummyCharMgr.SetCurLocalIndex( iCurIndex );
			}
			SetActionState( pChar, pStage );
		}
		break;
	case SSS_CREAT_CIRCLE_TRACE_DUMMY:
		{
			int iDummyIndex;
			rkPacket >> iDummyIndex;

			m_State = SSS_CREAT_CIRCLE_TRACE_DUMMY;

			CreateCircleTraceDummy( iDummyIndex, pChar );
		}
		break;
	case SSS_MAGIC_CIRCLE:
		{
			m_State = SS_ACTION;

			rkPacket >> m_szAimedTarget;
			if( !m_szAimedTarget.IsEmpty() )
			{
				m_pTargetChar = pStage->GetBaseChar( m_szAimedTarget );
				m_vTargetCharPos = m_pTargetChar->GetMidPositionByRate();
			}
			SetMagicCircle( pChar );
		}
		break;
	}
}

bool ioTeleportSpecifiedEnemySkill::IsEndState() const
{
	if( m_State == SS_END )
		return true;

	return false;
}

bool ioTeleportSpecifiedEnemySkill::IsProtected( int iDefenseBreakType ) const
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

	switch( m_State )
	{
	case SS_NONE:
		return false;
	case SS_MAGIC_CIRCLE_LOOP:
		{
			if( m_dwLoopProtectTime > 0 &&
				m_dwLoopStartTime + m_dwLoopProtectTime > FRAMEGETTIME() )
				return true;

			if( m_dwFireMotionEndTime > FRAMEGETTIME() )
				return true;
		}
		break;
	case SS_ACTION:
	case SS_END:
		return false;
	}

	return false;
}

bool ioTeleportSpecifiedEnemySkill::IsSkillMotionEnd( DWORD dwActionStop, ioBaseChar* pOwner ) const
{
	if( m_State == SS_END && m_dwMotionEndTime + dwActionStop < FRAMEGETTIME() )
		return true;
	
	return false;
}

bool ioTeleportSpecifiedEnemySkill::CheckUseItemMesh()
{
	return m_bUseItemMesh;
}

void ioTeleportSpecifiedEnemySkill::ApplyAttributeToWeapon( ioWeapon *pWeapon, ioPlayStage *pStage )
{
	ioSkill::ApplyAttributeToWeapon( pWeapon, pStage );
}


void ioTeleportSpecifiedEnemySkill::ChangeCircleDropZoneAndPositionGap( ioBaseChar *pChar, ioPlayStage *pStage )
{
	if( !pStage ) return;
	if( !pChar ) return;

	D3DXVECTOR3 vPos = pChar->GetMagicCirclePos();

	if( pStage->GetMapHeightNoneEntity( vPos.x, vPos.z, vPos.y + 3.0f ) > 0.0f )
	{
		if( m_bOffCircle && m_CurCircleEffect == m_OffCircle )
		{	
			ioHashString szDummyCircle;
			pChar->CreateMagicCircle( m_OnCircle, m_OnCircle, szDummyCircle, 0 );
			pChar->SetMagicCirclePos( false, vPos );
			m_bOffCircle = false;
			m_CurCircleEffect = m_OnCircle;		
		}

		m_bEnableSummon = true;
	}
	else
	{
		if( !m_bOffCircle && m_CurCircleEffect == m_OnCircle )
		{
			ioHashString szDummyCircle;
			pChar->CreateMagicCircle( m_OffCircle, m_OffCircle, szDummyCircle, 0 );
			pChar->SetMagicCirclePos( false, vPos );
			m_bOffCircle = true;
			m_CurCircleEffect = m_OffCircle;
		}

		m_bEnableSummon = false;
	}
}

void ioTeleportSpecifiedEnemySkill::CreateCircleTraceDummy( int iDummyIndex, ioBaseChar *pChar )
{
	if( !pChar || m_vWeaponPosList.empty() || !m_pTargetChar )
		return;

	ioPlayStage *pStage = pChar->GetCreator();
	if ( !pStage )
		return;
	
	g_DummyCharMgr.DestroyDummyCharByName( pChar, m_stDummyName );

	ioDummyChar *pDummy = pStage->CreateDummyChar( m_stDummyName, iDummyIndex,
		pChar->GetCharName(),
		m_vTargetCharPos,
		0.0f, 0, true );

	if ( !pDummy )
		return;

	D3DXVECTOR3 vDummyPos = pDummy->GetWorldPosition();

	ioCircleTraceDummy* pCircleTraceDummy = dynamic_cast<ioCircleTraceDummy*>( pDummy );
	if ( !pCircleTraceDummy )
		return;

	if( pCircleTraceDummy )
	{
		float fHeightOffset = pCircleTraceDummy->GetHeightOffset();

		vDummyPos.y += fHeightOffset;
		pCircleTraceDummy->SetWorldPosition( vDummyPos );

		for( int i = 0; i < (int)m_vWeaponPosList.size(); i++ )
		{
			m_vWeaponPosList[i].y += fHeightOffset;
			pCircleTraceDummy->AddCirclePosition( m_vWeaponPosList[i] );
		}
	}

	m_vWeaponPosList.clear();

	if( pChar->IsNeedSendNetwork() )
	{
		SP2Packet kPacket( CUPK_SKILL_EXTRA_INFO );
		kPacket << pChar->GetCharName();
		kPacket << GetName();
		kPacket << SSS_CREAT_CIRCLE_TRACE_DUMMY;
		kPacket << iDummyIndex;
		kPacket << m_pTargetChar->GetCharName();
		P2PNetwork::SendToAllPlayingUser( kPacket );
	}
	else
		g_DummyCharMgr.SetCurLocalIndex( iDummyIndex );		// 더미 인덱스 동기화
}

// 타겟 지정

void ioTeleportSpecifiedEnemySkill::GetTargetList( ioBaseChar *pChar, ioPlayStage *pStage )
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
		
		if( pTarget->IsSystemState() )
			continue;

		if( !pTarget->IsEnableTarget() )
			continue;

		D3DXVECTOR3 vDiff = pTarget->GetWorldPosition() - pChar->GetWorldPosition();
		if( D3DXVec3LengthSq( &vDiff ) < m_fAimRange * m_fAimRange )
		{
			BaseCharHP kInfo;
			kInfo.m_Name = pTarget->GetCharName();
			kInfo.m_fHP = pTarget->GetHP().m_fCurValue;
			kInfo.m_bPrisoner = pTarget->IsPrisonerMode();

			m_vTargetInfoList.push_back(kInfo);
		}
	}

	std::sort( m_vTargetInfoList.begin(), m_vTargetInfoList.end(), BaseCharHPSort() );	

	if( m_vTargetInfoList.empty() )
		m_szAimedTarget.Clear();
	else
	{
		ioMapEffect *pEffect = pStage->FindMapEffect( m_dwMakerID );
		if ( pEffect )		
			pEffect->SetVisible( true );
	}

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
}

bool ioTeleportSpecifiedEnemySkill::CheckInputChangeTarget( ioBaseChar *pChar )
{
	if ( !pChar || !pChar->IsNeedProcess() )
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

bool ioTeleportSpecifiedEnemySkill::CheckInputAttack( ioBaseChar *pChar )
{
	if ( !pChar || !pChar->IsNeedProcess() )
		return false;

	if ( pChar->IsAttackKey() )
		return true;

	return false;
}

void ioTeleportSpecifiedEnemySkill::UpdateMaker( ioBaseChar *pChar, ioPlayStage *pStage )
{
	if ( !pChar || !pStage )
		return;

	D3DXVECTOR3 vPos = pChar->GetMidPositionByRate() + m_vAimDir * m_fAimRange;

	if ( !m_szAimedTarget.IsEmpty() )
	{
		ioBaseChar *pTarget = pStage->GetBaseChar( m_szAimedTarget );
		if ( pTarget )
		{
			vPos = pTarget->GetMidPositionByRate();
			SetAimedMaker( pChar, pStage, vPos );
		}
	}

	ioMapEffect *pEffect = pStage->FindMapEffect( m_dwMakerID );
	if ( pEffect )		
	{
		if ( !m_szAimedTarget.IsEmpty() )
			pEffect->SetWorldPosition( vPos );
		else
			pEffect->SetVisible( false );
	}
}

void ioTeleportSpecifiedEnemySkill::SetTargetRotation( ioBaseChar *pChar )
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

void ioTeleportSpecifiedEnemySkill::SetAimedMaker( ioBaseChar *pChar, ioPlayStage *pStage, D3DXVECTOR3 vPos )
{
	if ( !pChar || !pChar->IsNeedProcess() || !pStage )
		return;

	bool bSendNetwork = false;

	DWORD dwCurTime = FRAMEGETTIME();		
	if ( m_dwMakerID != -1 )
	{
		if ( m_dwCheckAimTime > 0 && m_dwCheckAimTime < dwCurTime )
		{
			bSendNetwork = true;
			DestroyEffect( pChar, pStage, m_dwMakerID );

			m_dwMakerID = CreateEffect( pChar, pStage, m_szAimedMarker, vPos, D3DXVECTOR3(1.0f,1.0f,1.0f) );
			m_dwCheckAimTime = m_dwAimedTime + dwCurTime;
		}
	}
	else if ( m_dwMakerID == -1 )
	{
		bSendNetwork = true;
		m_dwMakerID = CreateEffect( pChar, pStage, m_szAimedMarker, vPos, D3DXVECTOR3(1.0f,1.0f,1.0f) );
	}

	if ( bSendNetwork && pChar->IsNeedSendNetwork() )
	{
		SP2Packet kPacket( CUPK_SKILL_EXTRA_INFO );
		kPacket << pChar->GetCharName();
		kPacket << GetName();
		kPacket << SSS_ASSIGN_TARGET;
		kPacket << m_szAimedTarget;
		kPacket << vPos;
		P2PNetwork::SendToAllPlayingUser( kPacket );
	}
}

DWORD ioTeleportSpecifiedEnemySkill::CreateEffect( ioBaseChar *pChar, ioPlayStage *pStage, ioHashString& szEffect, D3DXVECTOR3& vPos, D3DXVECTOR3& vScale )
{
	if ( !pChar || !pStage || szEffect.IsEmpty() )
		return -1;

	ioMapEffect *pMaker = pStage->CreateMapEffect( szEffect, vPos, vScale );
	if ( pMaker )
		return pMaker->GetUniqueID();

	return -1;
}

void ioTeleportSpecifiedEnemySkill::DestroyEffect( ioBaseChar *pChar, ioPlayStage *pStage, DWORD& dwEffectID )
{
	if ( !pChar || !pStage || dwEffectID == -1 )
		return;

	pStage->DestroyMapEffect( dwEffectID );
	dwEffectID = -1;
}

//////////////////////////////////////////////////////////////////////////////////////////////////////

bool ioTeleportSpecifiedEnemySkill::CheckExceptionCamera()
{
	if( !m_szAimedTarget.IsEmpty() )
		return true;

	return false;
}

D3DXVECTOR3 ioTeleportSpecifiedEnemySkill::GetExceptionCameraPos( ioBaseChar *pChar )
{
	if( !pChar )
		return ioMath::VEC3_ZERO;

	ioBaseChar *pTarget = pChar->GetBaseChar( m_szAimedTarget );
	if( pTarget )
		return pTarget->GetWorldPosition();

	return pChar->GetWorldPosition();
}