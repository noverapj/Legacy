

#include "stdafx.h"

#include "ioMultiMagicCircleSkill.h"

#include "ioBaseChar.h"
#include "ioPlayStage.h"
#include "WeaponDefine.h"
#include "ioCircleTraceDummy.h"
#include "ioCircleTraceDummy2.h"

ioMultiMagicCircleSkill::ioMultiMagicCircleSkill()
{
	m_State = SS_NONE;

	m_bSetHandMesh = false;

	m_dwFireStartTime = 0;
	m_dwActionStartTime = 0;
	m_dwLoopStartTime = 0;

	m_iCurWeaponCnt = 0;
}

ioMultiMagicCircleSkill::ioMultiMagicCircleSkill( const ioMultiMagicCircleSkill &rhs )
: ioNormalSkill( rhs ),
 m_szEnableCircle( rhs.m_szEnableCircle ),
 m_dwEnableKeyTime( rhs.m_dwEnableKeyTime ),
 m_fCircleRange( rhs.m_fCircleRange ),
 m_fCircleMoveSpeed( rhs.m_fCircleMoveSpeed ),
 m_fCircleHeightGap( rhs.m_fCircleHeightGap ),
 m_fCircleOffSet( rhs.m_fCircleOffSet ),
 m_ReadyCircle( rhs.m_ReadyCircle ),
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
 m_bUseDecreaseActionGauge( rhs.m_bUseDecreaseActionGauge ),
 m_fReduceSkillGauge( rhs.m_fReduceSkillGauge ),
 m_bUseDestroyDummy( rhs.m_bUseDestroyDummy )
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
}

ioMultiMagicCircleSkill::~ioMultiMagicCircleSkill()
{
}

void ioMultiMagicCircleSkill::LoadProperty( ioINILoader &rkLoader )
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
	rkLoader.LoadString_e( "stamp_magic_circle", "", szBuf, MAX_PATH );
	m_ReadyCircle = szBuf;

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

	m_bUseDecreaseActionGauge = rkLoader.LoadBool_e( "use_decrease_end_action_gauge", false );
	m_fReduceSkillGauge = rkLoader.LoadFloat_e( "reduce_skill_gauge", 0.f );

	m_bUseDestroyDummy = rkLoader.LoadBool_e( "use_destroy_dummy", false );
}

ioSkill* ioMultiMagicCircleSkill::Clone()
{
	return new ioMultiMagicCircleSkill( *this );
}

bool ioMultiMagicCircleSkill::OnSkillStart( ioBaseChar *pChar )
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
	//m_iMaxWeaponCnt = max(2,4);//TEST

	pChar->ReduceNeedGaugeBySkill( this, m_fReduceSkillGauge );

	//기존에 이미 소환된 더미가 있으면 삭제
	if( m_bUseDestroyDummy )
		g_DummyCharMgr.DestroyDummyCharByName( pChar, m_stDummyName );

	return true;
}

void ioMultiMagicCircleSkill::OnSkillEnd( ioBaseChar *pChar )
{
	ioNormalSkill::OnSkillEnd( pChar );

	if( m_CreateWeaponStyle == CWS_NORMAL )
		CreateWeapon( pChar );
	else if( m_CreateWeaponStyle == CWS_MISSILE5 )
		CreateMissle5Weapon( pChar );
	else if( m_CreateWeaponStyle == CWS_DUMMY_CREATE || m_CreateWeaponStyle == CWS_CIRCLE_TRACE_DUMMY )
		;//CreateDummy( pChar );
	else
		CreateWeapon( pChar );

	// 기본무기 장착
	if( m_bUseItemMesh )
	{
		pChar->ShowWeaponItemMesh( true );
	}

	if( !m_dwEnableCircleIDList.empty() )
	{
		ioPlayStage *pStage = g_SkillMaker.GetPlayStage();
		if( pStage )
		{
			int iCnt = m_dwEnableCircleIDList.size();
			for( int i=0; i<iCnt; ++i)
			{
				pStage->DestroyMapEffect( m_dwEnableCircleIDList[i] );
			}
			m_dwEnableCircleIDList.clear();
		}
	}

	// 스킬 중간 동작에 사용된 메쉬들 제거
	m_bSetHandMesh = false;
	if( !m_HandMesh.IsEmpty() )
		pChar->RestoreSkillWeaponMesh( this, m_HandMesh, m_bVisibleEquipMesh );

	EndCameraBuff( pChar );

	pChar->SetMagicCircleTarget( "" );

	pChar->DestroyMagicCircle();

	m_dwMotionEndTime = 0;
	m_dwMotionStartTime = 0;
	m_iCurWeaponCnt = 0;
	m_State = SS_NONE;

	m_vWeaponPosList.clear();
}

void ioMultiMagicCircleSkill::OnProcessState( ioBaseChar *pChar, ioPlayStage *pStage )
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
				
				/*if( m_bDirectFire && pChar->CheckMagicCircle() )
					AddPosition( pChar, pStage );*/

				SetLoopState( pChar );
			}
		}
		break;
	case SS_LOOP:
		{
			if( m_bUseEnableMagicCircle )
				ChangeCircleDropZoneAndPositionGap( pChar, pStage );

			int nSize = m_vWeaponPosList.size();
			float fCurRange = 0.f;
			if( m_vWeaponPosList.size() < m_vWeaponOffset.size() )
				fCurRange = m_vWeaponOffset[ nSize ];	//m_fCircleRange;

			int iSkillNum = pChar->GetSkillKeyInput();
			if( iSkillNum == 0 )
			{
				if( m_vWeaponPosList.size() > 1 )
					SetActionState( pChar, pStage );
				else
					SetEndState( pChar );
			}

			// 뱡향 변화 체크
			pChar->CheckChangeDirectionForMagicCircle();

			// MagicCircle 위치 갱신
			D3DXVECTOR3 vCharPos = pChar->GetWorldPosition();
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
				bool bAddPosition = false;
				if( pChar->IsAttackKey() && m_dwGapTime < FRAMEGETTIME() && m_bEnableSummon )
				{					
					bAddPosition = true;
					AddPosition( pChar, pStage );
					m_dwGapTime += m_dwEnableKeyTime;

					if( pChar->IsNeedSendNetwork() && m_vWeaponPosList.size() )
					{
						SP2Packet kPacket( CUPK_SKILL_EXTRA_INFO );
						kPacket << pChar->GetCharName();
						kPacket << GetName();
						kPacket << SSS_KEY;
						P2PNetwork::SendToAllPlayingUser( kPacket );
					}
				}

				if( m_iCurWeaponCnt >= m_iMaxWeaponCnt )
				{
					SetActionState( pChar, pStage );
				}
				else if( bAddPosition )
				{
					//아직 셋팅할 구역이 있다면 Circle을 offset만큼 이동시켜 준다.
					D3DXVECTOR3 vCharPos = pChar->GetWorldPosition();
					D3DXVECTOR3 vDir = vTargetPos - vCharPos;
					D3DXVec3Normalize( &vDir, &vDir );
					
					vTargetPos = vTargetPos + vDir * m_fCircleOffSet;
					pChar->SetMagicCirclePos(false, vTargetPos);
				}
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
			if( m_CreateWeaponStyle == CWS_NORMAL )
				CreateWeapon( pChar );
			else if( m_CreateWeaponStyle == CWS_MISSILE5 )
				CreateMissle5Weapon( pChar );
			else if( m_CreateWeaponStyle == CWS_DUMMY_CREATE )
				CreateDummy( pChar );
			else if ( m_CreateWeaponStyle == CWS_CIRCLE_TRACE_DUMMY )
			{
				// 더미 Index 동기화를 위해
				if ( pChar->IsNeedProcess() )
				{
					int iCurIndex = g_DummyCharMgr.GetCurLocalIndex();
					CreateCircleTraceDummy( iCurIndex, pChar );
				}
			}
			else
				CreateWeapon( pChar );

			m_dwFireStartTime = 0;

			if( m_bUseDecreaseActionGauge && !m_bReduceGauge )
			{
				pChar->ReduceNeedGaugeBySkill( this, GetNeedGauge(pChar->GetCharName()) );
				m_bReduceGauge = true;
			}
		}
		break;
	case SS_END:
		break;
	}
}

bool ioMultiMagicCircleSkill::StartPreDelayAnimation( ioBaseChar *pChar )
{
	if( !ioNormalSkill::StartPreDelayAnimation( pChar ) )
		return false;

	return true;
}

void ioMultiMagicCircleSkill::SetMagicCircle( ioBaseChar *pChar )
{
	ioHashString szDummyCircle;
	if( !pChar->CheckMagicCircle() )
	{		
		pChar->CreateMagicCircle( m_OnCircle, m_OnCircle, szDummyCircle, m_fCircleOffSet );
	}
	else
	{
		D3DXVECTOR3 vTargetPos = pChar->GetMagicCirclePos();
		pChar->CreateMagicCircle( m_OnCircle, m_OnCircle, szDummyCircle, m_fCircleOffSet );
		pChar->SetMagicCirclePos(false, vTargetPos);
	}
	
}

void ioMultiMagicCircleSkill::SetNormalSkill( ioBaseChar *pChar )
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

void ioMultiMagicCircleSkill::SetActionState( ioBaseChar *pChar, ioPlayStage *pStage )
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

void ioMultiMagicCircleSkill::SetLoopState( ioBaseChar *pChar )
{
	if( m_LoopAnimation.IsEmpty() || !pChar )
	{
		m_dwLoopStartTime = 0;
		return;
	}

	SetMagicCircle( pChar );

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

	m_State = SS_LOOP;
	m_dwLoopStartTime = FRAMEGETTIME();

	EndPreDelayEffect( pChar );
}


void ioMultiMagicCircleSkill::SetEndState( ioBaseChar *pChar )
{
	if( m_State == SS_END )
		return;

	DWORD dwCurTime = FRAMEGETTIME();

	m_State = SS_END;
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
}

void ioMultiMagicCircleSkill::AddPosition( ioBaseChar *pChar, ioPlayStage *pStage )
{
	if( !pChar )	return;
	if( !pStage )	return;

	// 첫발이 찍힐 때 스킬게이지를 소모시킨다.
	if( !m_bReduceGauge && !m_bUseDecreaseActionGauge )
	{
		pChar->ReduceNeedGaugeBySkill( this, GetNeedGauge(pChar->GetCharName()) );
		m_bReduceGauge = true;
	}

	if( m_iCurWeaponCnt < m_iMaxWeaponCnt )
	{
		m_vCirclePos = pChar->GetMagicCirclePos( m_fCircleOffSet, m_bCircleNoChangeY );
		m_vWeaponPosList.push_back( m_vCirclePos );
	}
	
	m_iCurWeaponCnt++;

	if( !m_ReadyCircle.IsEmpty() )
	{	
		D3DXVECTOR3 vScale = D3DXVECTOR3( FLOAT1, FLOAT1, FLOAT1 );
		ioMapEffect *pMapEffect = pStage->CreateMapEffect( m_ReadyCircle, m_vCirclePos, vScale );
		if( pMapEffect )
		{
			DWORD dwID = pMapEffect->GetUniqueID();
			m_dwEnableCircleIDList.push_back(dwID);
		}
	}
}

void ioMultiMagicCircleSkill::ApplyExtraSkillInfo( ioBaseChar *pChar, ioPlayStage *pStage, SP2Packet &rkPacket )
{
	int iSyncState;
	rkPacket >> iSyncState;

	switch( iSyncState )
	{
	case SSS_KEY:
		{
			AddPosition( pChar, pStage );
		}
		break;
	case SSS_ACTION:
		{
			m_vWeaponPosList.clear();
			int nCnt = 0;
			rkPacket >> nCnt;
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

			CreateCircleTraceDummy( iDummyIndex, pChar );
		}
		break;
	}
}


void ioMultiMagicCircleSkill::CreateDummy( ioBaseChar *pChar )
{
	if( !pChar )	return;

	ioPlayStage *pStage = pChar->GetCreator();
	if( pStage == 0 )
		return;

	int iCnt = m_vWeaponPosList.size();
	if( iCnt == 0 )
		return;

	for( int i = 0; i < iCnt; i++ )
	{
		int iCurIndex = g_DummyCharMgr.GetCurLocalIndex();
		D3DXVECTOR3 vPos = m_vWeaponPosList[i];
		ioDummyChar *pDummy = pStage->CreateDummyChar( m_stDummyName, iCurIndex,
			pChar->GetCharName(),
			vPos,
			0.f,
			0, true );
	}

	m_vWeaponPosList.clear();
}


void ioMultiMagicCircleSkill::CreateMissle5Weapon( ioBaseChar *pChar )
{
	if( !pChar )	return;
	if( m_vWeaponPosList.empty() )	return;

	ioPlayStage *pStage = pChar->GetCreator();

	int iCnt = m_vWeaponPosList.size();
	if( iCnt == 0 )
		return;

	D3DXQUATERNION qtRot;
	int iWeaponIndex = pChar->GetWeaponIndexBase();

	FireTime kFireTime;
	kFireTime.dwStart = FRAMEGETTIME();
	kFireTime.dwDuration = 1;
	kFireTime.iAttributeIdx = m_dwAttackAttribute;
	kFireTime.dwWeaponIdx = iWeaponIndex++;
	ioWeapon *pWeapon = pChar->ExtendFireExplicit( kFireTime, pChar->GetWorldPosition(), ioMath::VEC3_ZERO, "" );
	ioMissileWeapon5 *pMissile5Weapon = ToMissileWeapon5( pWeapon );

	if( pMissile5Weapon == 0 )
	{
		LOG.PrintTimeAndLog(0, "ioMultiMagicCircleSkill-->NoMissle5Weapon");
		assert( 0 );
	}

	for( int i = 0; i < iCnt; i++ )
	{
		D3DXVECTOR3 vPos = m_vWeaponPosList[i];
		pMissile5Weapon->AddMovePosition( vPos );
	}

	m_vWeaponPosList.clear();

	if( !m_dwEnableCircleIDList.empty() )
	{
		ioPlayStage *pStage = g_SkillMaker.GetPlayStage();
		if( pStage )
		{
			int iCnt = m_dwEnableCircleIDList.size();
			for( int i=0; i<iCnt; ++i)
			{
				pStage->DestroyMapEffect( m_dwEnableCircleIDList[i] );

			}
			m_dwEnableCircleIDList.clear();
		}
	}
}

void ioMultiMagicCircleSkill::CreateWeapon( ioBaseChar *pChar )
{
	if( !pChar )	return;
	if( m_vWeaponPosList.empty() )	return;

	ioPlayStage *pStage = pChar->GetCreator();

	int iCnt = m_vWeaponPosList.size();
	D3DXQUATERNION qtRot;
	int iWeaponIndex = pChar->GetWeaponIndexBase();
	
	for( int i=1; i<iCnt; ++i )
	{
		if( m_iMaxWeaponCnt > 0 )
		{
			//서로 점과의 방향을 구해서 돌리자
			D3DXVECTOR3 vMoveDir = m_vWeaponPosList[i] - m_vWeaponPosList[i-1];
			ioMath::CalcDirectionQuaternion( qtRot, -vMoveDir );
			float fLength = fabs( D3DXVec3Length( &vMoveDir ) );
			D3DXVec3Normalize( &vMoveDir, &vMoveDir );

			FireTime kFireTime;
			kFireTime.dwStart = FRAMEGETTIME();
			kFireTime.dwDuration = 1;
			kFireTime.iAttributeIdx = m_dwAttackAttribute;
			kFireTime.dwWeaponIdx = iWeaponIndex++;
			ioWeapon *pWeapon = pChar->ExtendFireExplicit( kFireTime, m_vWeaponPosList[i-1], vMoveDir, "" );
	
			if( pWeapon == 0 )
			{
				LOG.PrintTimeAndLog(0, "ioMultiMagicCircleSkill-->NoWeapon");
				assert( 0 );
			}

			ioFlamewaveWeapon  *pFlamewaveWeapon = ToFlamewaveWeapon(pWeapon);
			if( pFlamewaveWeapon )
				pFlamewaveWeapon->SetCustomRange(true, fLength );
		}
	}
	
	m_vWeaponPosList.clear();

	if( !m_dwEnableCircleIDList.empty() )
	{
		ioPlayStage *pStage = g_SkillMaker.GetPlayStage();
		if( pStage )
		{
			int iCnt = m_dwEnableCircleIDList.size();
			for( int i=0; i<iCnt; ++i)
			{
				pStage->DestroyMapEffect( m_dwEnableCircleIDList[i] );

			}
			m_dwEnableCircleIDList.clear();
		}
	}
}

bool ioMultiMagicCircleSkill::IsEndState() const
{
	if( m_State == SS_END )
		return true;

	return false;
}

bool ioMultiMagicCircleSkill::IsProtected( int iDefenseBreakType ) const
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
	case SS_LOOP:
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

bool ioMultiMagicCircleSkill::IsSkillMotionEnd( DWORD dwActionStop, ioBaseChar* pOwner ) const
{
	if( m_State == SS_END && m_dwMotionEndTime + dwActionStop < FRAMEGETTIME() )
		return true;
	
	return false;
}

bool ioMultiMagicCircleSkill::CheckUseItemMesh()
{
	return m_bUseItemMesh;
}

void ioMultiMagicCircleSkill::ApplyAttributeToWeapon( ioWeapon *pWeapon, ioPlayStage *pStage )
{
	ioSkill::ApplyAttributeToWeapon( pWeapon, pStage );
}


void ioMultiMagicCircleSkill::ChangeCircleDropZoneAndPositionGap( ioBaseChar *pChar, ioPlayStage *pStage )
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

void ioMultiMagicCircleSkill::CreateCircleTraceDummy( int iDummyIndex, ioBaseChar *pChar )
{
	if( !pChar || m_vWeaponPosList.empty() )
		return;

	ioPlayStage *pStage = pChar->GetCreator();
	if ( !pStage )
		return;
	
	g_DummyCharMgr.DestroyDummyCharByName( pChar, m_stDummyName );

	ioDummyChar *pDummy = pStage->CreateDummyChar( m_stDummyName, iDummyIndex,
		pChar->GetCharName(),
		pChar->GetMidPositionByRate(),
		0.0f, 0, true );

	if ( !pDummy )
		return;

	ioCircleTraceDummy* pCircleTraceDummy = dynamic_cast<ioCircleTraceDummy*>( pDummy );
	ioCircleTraceDummy2* pCircleTraceDummy2 = dynamic_cast<ioCircleTraceDummy2*>( pDummy );
	if ( !pCircleTraceDummy && !pCircleTraceDummy2 )
		return;


	if( pCircleTraceDummy )
	{
		for( int i = 0; i < (int)m_vWeaponPosList.size(); i++ )
			pCircleTraceDummy->AddCirclePosition( m_vWeaponPosList[i] );
	}
	else if( pCircleTraceDummy2 )
	{
		for( int i = 0; i < (int)m_vWeaponPosList.size(); i++ )
			pCircleTraceDummy2->AddCirclePosition( m_vWeaponPosList[i] );
	}

	m_vWeaponPosList.clear();

	if( !m_dwEnableCircleIDList.empty() )
	{
		ioPlayStage *pStage = g_SkillMaker.GetPlayStage();
		if( pStage )
		{
			int iCnt = m_dwEnableCircleIDList.size();
			for( int i=0; i<iCnt; ++i)
			{
				pStage->DestroyMapEffect( m_dwEnableCircleIDList[i] );

			}
			m_dwEnableCircleIDList.clear();
		}
	}

	if( pChar->IsNeedSendNetwork() )
	{
		SP2Packet kPacket( CUPK_SKILL_EXTRA_INFO );
		kPacket << pChar->GetCharName();
		kPacket << GetName();
		kPacket << SSS_CREAT_CIRCLE_TRACE_DUMMY;
		kPacket << iDummyIndex;
		P2PNetwork::SendToAllPlayingUser( kPacket );
	}
}
