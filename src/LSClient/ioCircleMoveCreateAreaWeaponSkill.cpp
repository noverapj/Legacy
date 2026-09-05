
#include "stdafx.h"

#include "ioCircleMoveCreateAreaWeaponSkill.h"

#include "ioBaseChar.h"
#include "ioPlayStage.h"

#include "ioGrowthUpInfo.h"

ioCircleMoveCreateAreaWeaponSkill::ioCircleMoveCreateAreaWeaponSkill()
{
	Init();
}

ioCircleMoveCreateAreaWeaponSkill::ioCircleMoveCreateAreaWeaponSkill( const ioCircleMoveCreateAreaWeaponSkill &rhs )
	: ioNormalSkill( rhs ),
	  m_fGaugePerTic( rhs.m_fGaugePerTic ),
	  m_dwTicTime( rhs.m_dwTicTime ),
	  m_fCircleRange( rhs.m_fCircleRange ),
	  m_fCircleMoveSpeed( rhs.m_fCircleMoveSpeed ),
	  m_fCircleHeightGap( rhs.m_fCircleHeightGap ),
	  m_fCircleOffSet( rhs.m_fCircleOffSet ),
	  m_ReadyCircle( rhs.m_ReadyCircle ),
	  m_dwSkillProtectTime( rhs.m_dwSkillProtectTime ),
	  m_szLoopAni( rhs.m_szLoopAni ),
	  m_nAreaWeaponMaxCnt( rhs.m_nAreaWeaponMaxCnt ),
	  m_szAreaWeaponName( rhs.m_szAreaWeaponName ),
	  m_OwnerBuffList( rhs.m_OwnerBuffList )
{	
	Init();
}

ioCircleMoveCreateAreaWeaponSkill::~ioCircleMoveCreateAreaWeaponSkill()
{
}

void ioCircleMoveCreateAreaWeaponSkill::LoadProperty( ioINILoader &rkLoader )
{
	ioNormalSkill::LoadProperty( rkLoader );

	char szBuf[MAX_PATH];
	//무적시간 설정
	m_dwSkillProtectTime    = (DWORD)rkLoader.LoadInt_e( "skill_protect_time", 0 );

	m_fGaugePerTic = rkLoader.LoadFloat_e( "gauge_per_tick", 0.0f );
	m_dwTicTime = (DWORD)rkLoader.LoadInt_e( "tick_time", 0 );

	//루프 설정
	rkLoader.LoadString_e( "loop_ani", "", szBuf, MAX_PATH );
	m_szLoopAni = szBuf;
	
	//마법진 설정
	m_fCircleRange = rkLoader.LoadFloat_e( "magic_circle_range", 0.0f );
	m_fCircleMoveSpeed = rkLoader.LoadFloat_e( "magic_circle_move_speed", 0.0f );
	m_fCircleHeightGap = rkLoader.LoadFloat_e( "magic_circle_height_gap", 0.0f );
	m_fCircleOffSet = rkLoader.LoadFloat_e( "magic_circle_offset", 0.0f );
	
	rkLoader.LoadString_e( "ready_circle", "", szBuf, MAX_PATH );
	m_ReadyCircle = szBuf;		

	LoadAreaWeaponList( rkLoader );
	LoadOwnerBuffList( rkLoader );
}

void ioCircleMoveCreateAreaWeaponSkill::LoadAreaWeaponList( ioINILoader &rkLoader )
{
	m_nAreaWeaponMaxCnt = rkLoader.LoadInt_e( "area_weapon_max_cnt", 0 );
	m_szAreaWeaponName.clear();

	if( m_nAreaWeaponMaxCnt <= 0 ) 
		return;

	m_szAreaWeaponName.reserve( m_nAreaWeaponMaxCnt );

	char szTitle[MAX_PATH], szBuf[MAX_PATH];
	for( int i=0; i < m_nAreaWeaponMaxCnt; ++i )
	{
		wsprintf_e( szTitle, "area_weapon_name%d", i+1 );
		rkLoader.LoadString( szTitle, "", szBuf, MAX_PATH );

		m_szAreaWeaponName.push_back( szBuf );
	}
}

ioSkill* ioCircleMoveCreateAreaWeaponSkill::Clone()
{
	return new ioCircleMoveCreateAreaWeaponSkill( *this );
}

bool ioCircleMoveCreateAreaWeaponSkill::OnSkillStart( ioBaseChar *pChar )
{	
	if ( !ioNormalSkill::OnSkillStart( pChar ) )
		return false;

	Init();

	if( m_dwSkillProtectTime > 0 )
		m_dwCurSkillProtectTime = FRAMEGETTIME() + m_dwSkillProtectTime;

	SetCameraBuff( pChar );

	//기본 게이지 감소 (마법진 게이지)
	pChar->ReduceNeedGaugeBySkill( this, GetNeedGauge(pChar->GetCharName()) );
	m_bReduceGauge = true;	
	
	// 기본 무기 손에서 제거
	if( m_bUseItemMesh )
	{
		pChar->ShowWeaponItemMesh( false );
	}
	//무기 변경
	if( m_bUseItemMesh && !m_bSetHandMesh && !m_HandMesh.IsEmpty() )
	{
		pChar->ChangeSkillWeaponMesh( this, m_HandMesh, m_bVisibleEquipMesh );
		m_bSetHandMesh = true;
	}

	return true;
}

void ioCircleMoveCreateAreaWeaponSkill::OnProcessState( ioBaseChar *pChar, ioPlayStage *pStage )
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

				SetOwnerBuffList( pChar );
				m_dwCheckGaugeTime = FRAMEGETTIME();

				//루프 상태로 전환
				SetLoopState( pChar );
			}
		}
		break;
	case SS_LOOP:
		{
			if( m_dwCheckGaugeTime + m_dwTicTime <= dwCurTime )
			{
				m_dwCheckGaugeTime = dwCurTime;

				if( m_fGaugePerTic > 0.0f && pChar->IsNeedProcess() && pChar->ReduceCurSkillGaugeBySkill( GetOwnerItem(), m_fGaugePerTic ) <= 0.0f )
				{
					SetEndState( pChar );
					return;
				}
			}

			//마법진 갱신
			UpdateMagicCircle( pChar );
			//마법진 방향으로 회전
			CheckRotate( pChar );
			//키입력
			CheckInputKey( pChar );
		}
		break;
	case SS_ACTION:
		{
			if ( m_dwMotionEndTime < dwCurTime )
			{
				if ( m_nAreaWeaponCurCnt >= m_nAreaWeaponMaxCnt )
					SetEndState( pChar );
				else
					SetLoopState( pChar );
				return;
			}

			//무기 생성
			CreateWeapon( pChar );
		}
		break;
	}
}

void ioCircleMoveCreateAreaWeaponSkill::OnSkillEnd( ioBaseChar *pChar )
{
	ioNormalSkill::OnSkillEnd( pChar );

	EndCameraBuff( pChar );

	pChar->SetMagicCircleTarget( "" );
	pChar->DestroyMagicCircle();
	
	// 기본무기 장착
	if( m_bUseItemMesh )
		pChar->ShowWeaponItemMesh( true );

	// 스킬 중간 동작에 사용된 메쉬들 제거
	if( m_bSetHandMesh && !m_HandMesh.IsEmpty() )
	{
		pChar->RestoreSkillWeaponMesh( this, m_HandMesh, m_bVisibleEquipMesh );
		m_bSetHandMesh = false;
	}
}

bool ioCircleMoveCreateAreaWeaponSkill::IsProtected( int iDefenseBreakType ) const
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

	DWORD dwCurTime = FRAMEGETTIME();
	if( m_dwCurSkillProtectTime > dwCurTime )
		return true;
	
	return false;
}

bool ioCircleMoveCreateAreaWeaponSkill::IsEndState() const
{
	if( m_SkillState == SS_END )
		return true;

	return false;
}

bool ioCircleMoveCreateAreaWeaponSkill::IsSkillMotionEnd( DWORD dwActionStop, ioBaseChar* pOwner ) const
{
	if( m_SkillState == SS_END )
		return true;

	return false;
}

bool ioCircleMoveCreateAreaWeaponSkill::CheckUseItemMesh()
{
	return m_bUseItemMesh;
}

void ioCircleMoveCreateAreaWeaponSkill::ApplyAttributeToWeapon( ioWeapon *pWeapon, ioPlayStage *pStage )
{
	ioSkill::ApplyAttributeToWeapon( pWeapon, pStage );
}

void ioCircleMoveCreateAreaWeaponSkill::ApplyExtraSkillInfo( ioBaseChar *pChar, ioPlayStage *pStage, SP2Packet &rkPacket )
{
	if ( !pChar || !pStage )
		return;

	int nState;
	rkPacket >> nState;

	switch( (SkillState)nState )
	{
	case SS_ACTION:
		{
			SetActionState( pChar );
		}
		break;
	case SS_CREATE_WEAPON:
		{
			rkPacket >> m_vCirclePos;
			rkPacket >> m_nAreaWeaponCurCnt;

			g_AreaWeaponMgr.CreateAreaWeapon( pChar->GetCharName(), "", GetName(), m_szAreaWeaponName[m_nAreaWeaponCurCnt],	m_vCirclePos, pChar->GetWorldOrientation(), ioAreaWeapon::CT_NORMAL );
		}
		break;
	case SS_END:
		{
			m_SkillState = SS_END;
		}
		break;
	}
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void ioCircleMoveCreateAreaWeaponSkill::Init()
{
	m_bSetHandMesh = false;
	m_SkillState = SS_NONE;

	m_dwCurSkillProtectTime = 0;
	m_dwCheckGaugeTime = 0;
	m_dwFireStartTime = 0;
	m_nAreaWeaponCurCnt = 0;
}

//성장 범위 얻기
float ioCircleMoveCreateAreaWeaponSkill::GetGrowthCircleRange( ioBaseChar *pChar )
{
	float fValue = 0.0f;

	if ( !pChar )
		return fValue;

	ioPlayStage *pStage = pChar->GetCreator();
	if ( !pStage )
		return fValue;

	ioGrowthUpInfo* pGrowthInfo = GetGrowthUpInfoByType( GT_MAGIC_CIRCLE_RANGE, pChar->GetCharName() );
	if ( pGrowthInfo )
	{
		ioGrowthNormalUpInfo *pUpInfo = ToNormalUpInfo( pGrowthInfo );
		if( pUpInfo )
			fValue = pUpInfo->GetValue(pStage);
	}

	return fValue;
}

//마법진 생성
void ioCircleMoveCreateAreaWeaponSkill::CreateMagicCircle( ioBaseChar *pChar )
{
	if ( !pChar )
		return;

	if( !pChar->CheckMagicCircle() )
	{
		ioHashString szDummyCircle;
		pChar->CreateMagicCircle( m_ReadyCircle, m_ReadyCircle, szDummyCircle, m_fCircleOffSet );
	}
}

//마법진 업데이트
void ioCircleMoveCreateAreaWeaponSkill::UpdateMagicCircle( ioBaseChar *pChar )
{
	if ( !pChar )
		return;
		
	if( pChar->CheckMagicCircle() )
	{
		float fCurRange = m_fCircleRange + GetGrowthCircleRange( pChar );

		//뱡향 체크
		pChar->CheckChangeDirectionForMagicCircle();
		//위치 갱신
		pChar->UpdateMagicCirclePosAll( m_fCircleMoveSpeed, fCurRange, m_fCircleHeightGap, false );
	}
}

//마법진 위치 얻기
void ioCircleMoveCreateAreaWeaponSkill::GetMagicCirclePos( ioBaseChar *pChar )
{
	if ( !pChar )
		return;

	if( pChar->CheckMagicCircle() )
	{
		float fCurRange = m_fCircleRange + GetGrowthCircleRange( pChar );
		pChar->UpdateMagicCirclePos( m_fCircleMoveSpeed, fCurRange, m_fCircleHeightGap, true, true );
	}

	m_vCirclePos = pChar->GetMagicCirclePos( m_fCircleOffSet, m_bCircleNoChangeY );
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////

//루프 상태 전환
void ioCircleMoveCreateAreaWeaponSkill::SetLoopState( ioBaseChar *pChar )
{
	if ( !pChar )
		return;

	ioEntityGroup *pGrp = pChar->GetGroup();
	int iAniID = pGrp->GetAnimationIdx( m_szLoopAni );
	if( iAniID == -1 )	
		return;
	
	pGrp->ClearAllLoopAni( FLOAT100, true );
	pGrp->ClearAllActionAni( FLOAT100, true );
	pGrp->SetLoopAni( iAniID, FLOAT100 );	

	//마법진 생성
	CreateMagicCircle( pChar );
	
	m_SkillState = SS_LOOP;	
}

//공격 상태로 전환
void ioCircleMoveCreateAreaWeaponSkill::SetActionState( ioBaseChar *pChar )
{
	if ( !pChar )
		return;

	ioEntityGroup *pGrp = pChar->GetGroup();
	ioHashString szSkillMotion = GetSkillMotionName();

	int iCurSkillAniID = pGrp->GetAnimationIdx( szSkillMotion );
	if( iCurSkillAniID == -1 )	
		return;

	float fAniRate = GetSkillMotionRate();
	float fTimeRate = fAniRate * m_fExtraAniRate;

	pGrp->ClearAllLoopAni( FLOAT100, true );
	pGrp->ClearAllActionAni( FLOAT100, true );
	pGrp->SetActionAni( iCurSkillAniID, FLOAT100, FLOAT100, FLOAT1, FLOAT1/fTimeRate );

	DWORD dwCurTime = FRAMEGETTIME();
	m_dwMotionEndTime = dwCurTime + pGrp->GetAnimationFullTime( iCurSkillAniID ) * fTimeRate;
	m_dwMotionEndTime += m_dwAnimationEndDelay;

	m_dwFireStartTime = dwCurTime;
	m_dwFireStartTime += pGrp->GetAnimationEventTime_e( iCurSkillAniID, "fire_s" ) * fTimeRate;
	
	m_SkillState = SS_ACTION;

	if( pChar->IsNeedSendNetwork() )
	{
		SP2Packet kPacket( CUPK_SKILL_EXTRA_INFO );
		kPacket << pChar->GetCharName();
		kPacket << GetName();
		kPacket << SS_ACTION;
		P2PNetwork::SendToAllPlayingUser( kPacket );
	}
}

//종료 상태 설정
void ioCircleMoveCreateAreaWeaponSkill::SetEndState( ioBaseChar *pChar )
{
	if ( !pChar || !pChar->IsNeedProcess() )
		return;

	m_SkillState = SS_END;
	
	//패킷 전송
	if( pChar->IsNeedSendNetwork() )
	{
		SP2Packet kPacket( CUPK_SKILL_EXTRA_INFO );
		kPacket << pChar->GetCharName();
		kPacket << GetName();
		kPacket << SS_END;
		P2PNetwork::SendToAllPlayingUser( kPacket );
	}
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////

//키입력 확인
void ioCircleMoveCreateAreaWeaponSkill::CheckInputKey( ioBaseChar *pChar )
{
	if ( !pChar || !pChar->IsNeedProcess() )
		return;	

	if( pChar->IsAttackKey() )
	{
		SetActionState( pChar );
		return;
	}
}

//마법진 방향으로 회전
void ioCircleMoveCreateAreaWeaponSkill::CheckRotate( ioBaseChar *pChar )
{
	if ( !pChar )
		return;

	//마법진 방향으로 캐릭터 회전
	D3DXVECTOR3 vTargetPos = pChar->GetMagicCirclePos();
	pChar->SetTargetRotToTargetPos( vTargetPos, true, false );
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void ioCircleMoveCreateAreaWeaponSkill::CreateWeapon( ioBaseChar *pChar )
{	
	if ( !pChar || !pChar->IsNeedProcess() )
		return;

	if ( m_nAreaWeaponCurCnt >= m_nAreaWeaponMaxCnt )
		return;

	DWORD dwCurTime = FRAMEGETTIME();
	if ( m_dwFireStartTime <= 0 || m_dwFireStartTime > dwCurTime )
		return;

	GetMagicCirclePos( pChar );
	g_AreaWeaponMgr.CreateAreaWeapon( pChar->GetCharName(), "", GetName(), m_szAreaWeaponName[m_nAreaWeaponCurCnt], m_vCirclePos, pChar->GetWorldOrientation(), ioAreaWeapon::CT_NORMAL );
	m_dwFireStartTime = 0;

	//패킷 전송
	if( pChar->IsNeedSendNetwork() )
	{
		SP2Packet kPacket( CUPK_SKILL_EXTRA_INFO );
		kPacket << pChar->GetCharName();
		kPacket << GetName();
		kPacket << SS_CREATE_WEAPON;
		kPacket << m_vCirclePos;
		kPacket << m_nAreaWeaponCurCnt;
		P2PNetwork::SendToAllPlayingUser( kPacket );
	}

	m_nAreaWeaponCurCnt++;
}

//버프 리스트 읽기
void ioCircleMoveCreateAreaWeaponSkill::LoadOwnerBuffList( ioINILoader &rkLoader )
{
	int iCnt = rkLoader.LoadInt_e( "owner_buff_cnt", 0 );
	m_OwnerBuffList.clear();

	if( iCnt <= 0 ) 
		return;

	m_OwnerBuffList.reserve( iCnt );

	char szTitle[MAX_PATH], szBuf[MAX_PATH];
	for( int i=0; i < iCnt; ++i )
	{
		wsprintf_e( szTitle, "owner_buff%d", i+1 );
		rkLoader.LoadString( szTitle, "", szBuf, MAX_PATH );

		m_OwnerBuffList.push_back( szBuf );
	}
}

void ioCircleMoveCreateAreaWeaponSkill::SetOwnerBuffList( ioBaseChar *pOwner )
{
	if ( !pOwner )
		return;

	if( m_OwnerBuffList.empty() ) 
		return;

	int nBuffCnt = m_OwnerBuffList.size();
	for( int i=0; i < nBuffCnt; ++i )
	{
		ioHashString szBuffName = m_OwnerBuffList[i];
		if( !szBuffName.IsEmpty() )
			pOwner->AddNewBuff( szBuffName, pOwner->GetCharName(), "", NULL );
	}
}