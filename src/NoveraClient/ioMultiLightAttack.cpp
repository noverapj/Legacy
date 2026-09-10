#include "stdafx.h"

#include "ioMultiLightAttack.h"

#include "ioBaseChar.h"
#include "ioGrowthUpInfo.h"
#include "ioPlayStage.h"

#include "WeaponDefine.h"

ioMultiLightAttack::ioMultiLightAttack()
{
	m_bSetHandMesh = false;
}

ioMultiLightAttack::ioMultiLightAttack( const ioMultiLightAttack &rhs )
	: ioAttackSkill( rhs ),
m_OwnerBuffList( rhs.m_OwnerBuffList ),
m_iMaxWeaponWay( rhs.m_iMaxWeaponWay ),
m_FireAngleList( rhs.m_FireAngleList ),
m_SkillLoopMotion( rhs.m_SkillLoopMotion ),
m_fSkillLoopMotionRate( rhs.m_fSkillLoopMotionRate ),
m_dwLoopDuration( rhs.m_dwLoopDuration ),
m_fCircleRange( rhs.m_fCircleRange ),
m_fCircleMoveSpeed( rhs.m_fCircleMoveSpeed ),
m_fCircleHeightGap( rhs.m_fCircleHeightGap ),
m_fCircleOffSet( rhs.m_fCircleOffSet ),
m_Circle( rhs.m_Circle ),
m_bDisableGrowth( rhs.m_bDisableGrowth ),
m_fGuagePerTic( rhs.m_fGuagePerTic ),
m_dwTicTime( rhs.m_dwTicTime ),
m_SkillMapEffect( rhs.m_SkillMapEffect ),
m_bEndUseAutoTargetValue( rhs.m_bEndUseAutoTargetValue ),
m_bEndAutoTargetDownState( rhs.m_bEndAutoTargetDownState ),
m_fEndAutoTargetRange( rhs.m_fEndAutoTargetRange ),
m_fEndAutoTargetMinAngle( rhs.m_fEndAutoTargetMinAngle ),
m_fEndAutoTargetMaxAngle( rhs.m_fEndAutoTargetMaxAngle ),
m_dwTeleportEndTime( rhs.m_dwTeleportEndTime ),
m_dwCheckPullStateMaxTime( rhs.m_dwCheckPullStateMaxTime )

{
	ClearData();
}

ioMultiLightAttack::~ioMultiLightAttack()
{
	ClearData();
}

void ioMultiLightAttack::LoadProperty( ioINILoader &rkLoader )
{
	ioAttackSkill::LoadProperty( rkLoader );

	LoadOwnerBuffList( rkLoader );

	m_iMaxWeaponWay = rkLoader.LoadInt_e( "max_fire_cnt", 1 );
	m_iMaxWeaponWay = max( 1, m_iMaxWeaponWay );

	char szKey[MAX_PATH];
	m_FireAngleList.clear();
	for( int i =0 ; i < m_iMaxWeaponWay; ++i )
	{
		wsprintf_e( szKey, "fire_angle%d", i+1 );

		float fAngle = rkLoader.LoadFloat( szKey, 0.0f );
		m_FireAngleList.push_back( fAngle );
	}

	char szBuf[MAX_PATH];

	rkLoader.LoadString_e( "skill_loop_motion", "", szBuf, MAX_PATH );
	m_SkillLoopMotion = szBuf;	
	m_fSkillLoopMotionRate = rkLoader.LoadFloat_e( "skill_loop_motion_rate", FLOAT1 );	
	m_dwLoopDuration = rkLoader.LoadInt_e( "skill_loop_duration", 0 );

	m_fCircleRange = rkLoader.LoadFloat_e( "magic_circle_range", 0.0f );
	m_fCircleMoveSpeed = rkLoader.LoadFloat_e( "magic_circle_move_speed", 0.0f );
	m_fCircleHeightGap = rkLoader.LoadFloat_e( "magic_circle_height_gap", 0.0f );
	m_fCircleOffSet = rkLoader.LoadFloat_e( "magic_circle_offset", 0.0f );

	rkLoader.LoadString_e( "magic_circle", "", szBuf, MAX_PATH );
	m_Circle = szBuf;

	rkLoader.LoadString_e( "red_team_circle", "", szBuf, MAX_PATH );
	m_RedEffect = szBuf;
	rkLoader.LoadString_e( "blue_team_circle", "", szBuf, MAX_PATH );
	m_BlueEffect = szBuf;

	m_bDisableGrowth = rkLoader.LoadBool_e( "disable_growth", false );
	m_fGuagePerTic = rkLoader.LoadFloat_e( "gauge_per_tick", 0.0f );
	m_dwTicTime = (DWORD)rkLoader.LoadInt_e( "tick_time", 0 );

	rkLoader.LoadString_e( "skill_map_effect", "", szBuf, MAX_PATH );
	m_SkillMapEffect = szBuf;

	m_bEndUseAutoTargetValue = rkLoader.LoadBool_e( "skill_end_use_auto_target_value", false );
	m_bEndAutoTargetDownState = rkLoader.LoadBool_e( "skill_end_autotarget_down_state", false );
	m_fEndAutoTargetRange = rkLoader.LoadFloat_e( "skill_end_autotarget_range", 300.0f );
	m_fEndAutoTargetMinAngle = rkLoader.LoadFloat_e( "skill_end_autotarget_min_angle", 60.0f );
	m_fEndAutoTargetMaxAngle = rkLoader.LoadFloat_e( "skill_end_autotarget_max_angle", 150.0f );

	m_dwTeleportEndTime = rkLoader.LoadInt_e( "teleport_enable_time", 50 );
	m_dwCheckPullStateMaxTime = rkLoader.LoadInt_e( "check_max_time", 2500 );
}

void ioMultiLightAttack::LoadOwnerBuffList( ioINILoader &rkLoader )
{
	int iCnt = rkLoader.LoadInt_e( "owner_buff_cnt", 0 );
	m_OwnerBuffList.clear();

	if( iCnt <= 0 ) return;
	m_OwnerBuffList.reserve( iCnt );

	char szTitle[MAX_PATH], szBuf[MAX_PATH];
	for( int i=0; i < iCnt; ++i )
	{
		wsprintf_e( szTitle, "owner_buff%d", i+1 );
		rkLoader.LoadString( szTitle, "", szBuf, MAX_PATH );

		ioBuffInfo tBuffInfo;
		tBuffInfo.m_BuffName = szBuf;

		if( !tBuffInfo.m_BuffName.IsEmpty() )
		{
			tBuffInfo.m_iBuffType = g_BuffMaker.GetBuffType( tBuffInfo.m_BuffName );
			if( tBuffInfo.m_iBuffType != BT_NONE )
			{
				m_OwnerBuffList.push_back( tBuffInfo );
			}
		}
	}
}

void ioMultiLightAttack::ClearData()
{
	m_bSetHandMesh = false;
	m_State = SS_PRE;

	m_iCurWeaponWay = 0;

	// 성장관련 요소
	m_dwCheckTime = 0;
	m_fTicGaugeRateByGrowth = FLOAT1;
	m_dwTicTimeByGrowth = 0;

	m_vActiveWeapon.clear();

	m_dwSkillMapEffect = -1;

	m_dwCurTeleportEndTime = 0;
}

ioSkill* ioMultiLightAttack::Clone()
{
	return new ioMultiLightAttack( *this );
}

bool ioMultiLightAttack::OnSkillStart( ioBaseChar *pChar )
{
	if( !ioAttackSkill::OnSkillStart( pChar ) )
		return false;

	m_bSetHandMesh = false;

	// 기본 무기 손에서 제거
	if( m_bUseItemMesh )
	{
		pChar->ShowWeaponItemMesh( false );
	}

	ClearData();

	SetCameraBuff( pChar );

	m_dwCheckTime = FRAMEGETTIME();

	// 성장관련 요소 추가.
	if( !m_bDisableGrowth )
	{
		ioPlayStage *pStage = pChar->GetCreator();
		if( pStage )
		{
			ioGrowthNormalUpInfo *pUpInfo = NULL;

			pUpInfo = ToNormalUpInfo( GetGrowthUpInfoByType(GT_SKILL_COOL_TIME, pChar->GetCharName()) );
			if( pUpInfo )
				m_fTicGaugeRateByGrowth = pUpInfo->GetValue(pStage);

			m_fTicGaugeRateByGrowth = min( m_fTicGaugeRateByGrowth, FLOAT100 );
			m_fTicGaugeRateByGrowth = FLOAT1 + ( m_fTicGaugeRateByGrowth/FLOAT100 );


			pUpInfo = ToNormalUpInfo( GetGrowthUpInfoByType(GT_BUFF_TIC_TIME, pChar->GetCharName()) );
			if( pUpInfo )
				m_dwTicTimeByGrowth = pUpInfo->GetValue(pStage);			
		}
	}

	if( !m_bReduceGauge )
	{
		float fCurGauge = pChar->ReduceNeedGaugeBySkill( this, GetNeedGauge(pChar->GetCharName()) );
		m_bReduceGauge = true;
	}

	return true;
}

void ioMultiLightAttack::OnProcessState( ioBaseChar *pChar, ioPlayStage *pStage )
{
	DWORD dwCurTime = FRAMEGETTIME();

	float fCurTicGauge = m_fGuagePerTic / m_fTicGaugeRateByGrowth;
	int iTickGap = (int)( dwCurTime - m_dwCheckTime );
	int iCurTicTime = m_dwTicTime + m_dwTicTimeByGrowth;

	switch( m_State )
	{
	case SS_PRE:
		{
			DWORD dwPreDelay = GetPreDelayTime();
			if( m_dwPreDelayStartTime + dwPreDelay < FRAMEGETTIME() )
			{
				m_dwPreDelayStartTime = 0;

				AttackSkillFire( pChar );
				EndPreDelayEffect( pChar );				
			}
		}
		break;
	case SS_ACTION:
		{
			if( 0 < m_dwMotionEndTime && m_dwMotionEndTime < dwCurTime )
			{
				SetLoopState( pChar, pStage );
			}
		}
		break;
	case SS_LOOP:
		{		
			// 뱡향 변화 체크
			pChar->CheckChangeDirectionForMagicCircle();

			// MagicCircle 위치 갱신
			pChar->UpdateMagicCirclePosAll( m_fCircleMoveSpeed, m_fCircleRange, m_fCircleHeightGap, false );
			
			// MagicCircle 위치방향으로 캐릭터 회전
			if( pChar->CheckMagicCircle() && pChar->IsNeedProcess() )
			{
				m_vCirclePos = pChar->GetMagicCirclePos();
				pChar->SetTargetRotToTargetPos( m_vCirclePos, true, false );
			}

			if( pChar->IsNeedProcess() )
			{
				//키입력 여부 검사
				bool bReserveSkill = false;
				int iSkillNum = pChar->GetSkillKeyInput();
				if( iSkillNum >= 0 )
				{
					bReserveSkill = true;
				}

				if( pChar->IsAttackKey() || pChar->IsDefenseKey() || pChar->IsJumpKey() && !bReserveSkill )
				{
					SetPullState( pChar, pStage, true );
				}
			}
			else if( m_dwPullTime < dwCurTime )
			{
				m_vCirclePos = pChar->GetWorldPosition();
				SetPullState( pChar, pStage, true );
			}

			if( pChar->IsNeedProcess() && iTickGap >= iCurTicTime )
			{
				m_dwCheckTime = dwCurTime - ( iTickGap - iCurTicTime );
				if( fCurTicGauge != 0.0f )
				{
					if( pChar->ReduceCurSkillGaugeBySkill( GetOwnerItem(), fCurTicGauge ) <= 0.0f )
					{
						SetPullState( pChar, pStage, true );
					}
				}
			}
		}
		break;
	case SS_WEAPON_PULL:
		{
			if( CheckPullState( pChar ) )
			{
				SetTeleport( pChar, true );
			}
		}
		break;
	case SS_TELEPORT:
		{
			if( 0 < m_dwCurTeleportEndTime && m_dwCurTeleportEndTime < dwCurTime )
			{
				m_State = SS_END;

				RemoveWeapon( pChar );
				RemoveOwnerBuff( pChar );
			}
		}
		break;
	case SS_END:
		{

		}
		break;
	}
}

void ioMultiLightAttack::AttackSkillFire( ioBaseChar *pChar )
{
	if( !pChar )
		return;

	m_State = SS_ACTION;
	ioAttackSkill::AttackSkillFire( pChar );

	pChar->SetInvisibleState( true, true );
	pChar->SetSpecialHide( true );
	AddOwnerBuff( pChar );

	if( !m_bSetHandMesh && m_bUseItemMesh && !m_HandMesh.IsEmpty() )
	{
		pChar->ChangeSkillWeaponMesh( this, m_HandMesh, m_bVisibleEquipMesh );
		m_bSetHandMesh = true;
	}	
}

void ioMultiLightAttack::SetLoopState( ioBaseChar *pChar, ioPlayStage *pStage )
{
	if( !pChar ) return;

	DWORD dwCurTime = FRAMEGETTIME();
	m_State = SS_LOOP;

	ioEntityGroup *pGrp = pChar->GetGroup();

	int iAniID = pGrp->GetAnimationIdx( m_SkillLoopMotion );
	float fAniRate = FLOAT1;
	if( m_fSkillLoopMotionRate > 0.0f )
		fAniRate = m_fSkillLoopMotionRate;

	pGrp->ClearAllLoopAni( FLOAT100, true );
	pGrp->ClearAllActionAni( FLOAT100, true );
	pGrp->SetLoopAni( iAniID, FLOAT100, FLOAT1, FLOAT1/fAniRate );

	m_dwPullTime = dwCurTime;
	m_dwPullTime += m_dwCheckPullStateMaxTime;

	if( !pChar->CheckMagicCircle() )
	{
		ioHashString szDummyCircle;
		pChar->CreateMagicCircle( m_Circle, m_Circle, szDummyCircle, m_fCircleOffSet );
	}
}

void ioMultiLightAttack::SetPullState( ioBaseChar *pChar, ioPlayStage *pStage, bool bSend )
{
	m_State = SS_WEAPON_PULL;

	ActiveWeaponList::iterator iter;
	for( iter = m_vActiveWeapon.begin(); iter != m_vActiveWeapon.end(); ++iter )
	{
		ioWeapon* pWeapon = g_WeaponMgr.FindWeapon( pChar, *iter );
		ioSelectPositionMoveWeapon* pSelectWepaon = ToSelectPositionMoveWeapon( pWeapon );
		if( pSelectWepaon )
		{	
			pSelectWepaon->SetTracePosition( m_vCirclePos );
		}
	}

	D3DXVECTOR3 vScale( FLOAT1, FLOAT1, FLOAT1 );
	ioMapEffect* pMapEffect = pStage->CreateMapEffect( m_SkillMapEffect, m_vCirclePos, vScale );
	if( pMapEffect )
	{
		m_dwSkillMapEffect = pMapEffect->GetUniqueID();
	}

	pChar->ReduceCurSkillGaugeBySkill( GetOwnerItem(), pChar->GetCurItemSkillGauge( GetOwnerItem()->GetName() ) );	
	pChar->ChangeMagicCircle( true );

	m_dwCurCheckPullStateStartTime = FRAMEGETTIME();

	if( pChar->IsNeedSendNetwork() && bSend )
	{
		SP2Packet kPacket( CUPK_SKILL_EXTRA_INFO );
		kPacket << pChar->GetCharName();
		kPacket << GetName();
		kPacket << SSC_PULL;
		kPacket << m_vCirclePos;
		P2PNetwork::SendToAllPlayingUser( kPacket );
	}
}

bool ioMultiLightAttack::CheckPullState( ioBaseChar* pChar )
{
	if( !pChar )
		return false;

	if( pChar->IsNeedProcess() )
	{
		DWORD dwCnt = 0;
		ActiveWeaponList::iterator iter;
		for( iter = m_vActiveWeapon.begin(); iter != m_vActiveWeapon.end(); ++iter )
		{
			ioWeapon* pWeapon = g_WeaponMgr.FindWeapon( pChar, *iter );
			if( pWeapon )
			{
				ioSelectPositionMoveWeapon* pSelectWepaon = ToSelectPositionMoveWeapon( pWeapon );
				if( pSelectWepaon && pSelectWepaon->isWeaponEnd() )
				{
					dwCnt++;
				}
			}
			else
			{
				//웨폰에 없다면 정상적인 종료를 위해 카운팅 증가
				dwCnt++;
			}
		}

		if( m_vActiveWeapon.size() == dwCnt )
		{
			return true;
		}
	}	
	
	//스킬 허용시간이 지났는데도 불구하고 아직도 체크 중이라면 강제로 다음단계 진행
	DWORD dwCheckTime = m_dwCurCheckPullStateStartTime + m_dwCheckPullStateMaxTime;
	if( 0 < dwCheckTime && dwCheckTime < FRAMEGETTIME() )
	{
		return true;
	}

	return false;
}

void ioMultiLightAttack::SetTeleport( ioBaseChar *pChar, bool bSend )
{
	pChar->SetWorldPosition( m_vCirclePos );
	m_State = SS_TELEPORT;

	m_dwCurTeleportEndTime = FRAMEGETTIME();
	m_dwCurTeleportEndTime += m_dwTeleportEndTime;
	
	pChar->SetInvisibleState( false, true );
	if( pChar->IsInvisibleFace() )
		pChar->ShowFaceMesh( false );
	pChar->SetSpecialHide( false );
	pChar->DestroyMagicCircle();

	if( pChar->IsNeedSendNetwork() && bSend )
	{
		SP2Packet kPacket( CUPK_SKILL_EXTRA_INFO );
		kPacket << pChar->GetCharName();
		kPacket << GetName();
		kPacket << SSC_TELEPORT;
		kPacket << m_vCirclePos;
		P2PNetwork::SendToAllPlayingUser( kPacket );
	}
}

void ioMultiLightAttack::AddOwnerBuff( ioBaseChar *pChar )
{
	if( !pChar )	return;
	if( m_OwnerBuffList.empty() ) return;

	int iCnt = m_OwnerBuffList.size();
	for( int i = 0; i < iCnt; ++i )
	{
		pChar->AddNewBuff( m_OwnerBuffList[i].m_BuffName, pChar->GetCharName(), "", this );
	}
}

void ioMultiLightAttack::RemoveOwnerBuff( ioBaseChar *pChar )
{
	if( !pChar )	return;
	if( m_OwnerBuffList.empty() ) return;
	int iCnt = m_OwnerBuffList.size();
	for( int i=0; i<iCnt; ++i)
	{
		pChar->RemoveBuff( m_OwnerBuffList[i].m_BuffName );
	}
}

void ioMultiLightAttack::RemoveWeapon( ioBaseChar* pChar )
{
	ActiveWeaponList::iterator iter;
	for( iter = m_vActiveWeapon.begin(); iter != m_vActiveWeapon.end(); ++iter )
	{
		ioWeapon* pWeapon = g_WeaponMgr.FindWeapon( pChar, *iter );
		ioSelectPositionMoveWeapon* pSelectWepaon = ToSelectPositionMoveWeapon( pWeapon );
		if( pSelectWepaon )
		{
			pSelectWepaon->SetWeaponDead();
		}
	}
}

void ioMultiLightAttack::OnSkillEnd( ioBaseChar *pChar )
{
	ioAttackSkill::OnSkillEnd( pChar );

	if( GetOwnerItem() )
		GetOwnerItem()->ClearCurChargeRateForSkill();

	// 기본무기 장착
	if( m_bUseItemMesh )
	{
		pChar->ShowWeaponItemMesh( true );
	}


	if( !m_HandMesh.IsEmpty() )
	{
		pChar->RestoreSkillWeaponMesh( this, m_HandMesh, m_bVisibleEquipMesh );
	}

	EndCameraBuff( pChar );

	pChar->DestroyMagicCircle();
	pChar->ClearSkillReserveKey();

	if( m_dwSkillMapEffect != -1 )
	{
		pChar->EndMapEffect( m_dwSkillMapEffect, false );
		m_dwSkillMapEffect = -1;
	}

	RemoveOwnerBuff( pChar );
	RemoveWeapon( pChar );

	ClearData();

	pChar->SetInvisibleState( false, true );
	if( pChar->IsInvisibleFace() )
		pChar->ShowFaceMesh( false );

	pChar->SetSpecialHide( false );
	pChar->DestroyMagicCircle();
}

bool ioMultiLightAttack::IsSkillMotionEnd( DWORD dwActionStop, ioBaseChar* pOwner ) const
{
	if( m_dwPreDelayStartTime > 0 )
		return false;

	switch( m_State )
	{
	case SS_PRE:
	case SS_ACTION:
	case SS_LOOP:
	case SS_WEAPON_PULL:
	case SS_TELEPORT:
		return false;
	case SS_END:
		if( m_fSkillEndNextJumpAmt == 0.0f )
			return true;
		else 
			return false;		
	}
	return true;
}

bool ioMultiLightAttack::IsAttackEndState() const
{
	if( m_State == SS_END )
		return true;

	return false;
}

bool ioMultiLightAttack::CheckUseItemMesh()
{
	return m_bUseItemMesh;
}

void ioMultiLightAttack::ApplyAttributeToWeapon( ioWeapon *pWeapon, ioPlayStage *pStage )
{
	ioSkill::ApplyAttributeToWeapon( pWeapon, pStage );

	if( m_iMaxWeaponWay <= m_iCurWeaponWay )
		return;

	float fCurAngle = m_FireAngleList[m_iCurWeaponWay];
	fCurAngle = ioMath::ArrangeHead( fCurAngle );

	D3DXVECTOR3 vMoveDir;
	D3DXQUATERNION qtAngle;
	D3DXQuaternionRotationYawPitchRoll( &qtAngle, DEGtoRAD(fCurAngle), 0.0f, 0.0f );

	ioBaseChar *pOwner = pWeapon->GetOwner();
	if( pOwner )
	{
		vMoveDir = pOwner->GetTargetRot() * D3DXVECTOR3( 0.0f, 0.0f, -FLOAT1 );
		vMoveDir = qtAngle * vMoveDir;
	}
	else
	{
		vMoveDir = ioMath::UNIT_Z;
	}

	pWeapon->SetMoveDir( vMoveDir );
	m_iCurWeaponWay++;
	m_vActiveWeapon.push_back( pWeapon->GetWeaponIndex() );
}

void ioMultiLightAttack::ApplyExtraSkillInfo( ioBaseChar *pChar, ioPlayStage *pStage, SP2Packet &rkPacket )
{
	int iState;
	rkPacket >> iState;

	if( iState == SSC_PULL )
	{
		rkPacket >> m_vCirclePos;
		SetPullState( pChar, pStage, false );
	}
	else
	{
		rkPacket >> m_vCirclePos;
		SetTeleport( pChar , false );
	}
}

bool ioMultiLightAttack::IsNoDropState() const
{
	return true;
}