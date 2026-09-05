#include "StdAfx.h"
#include "ioProtectCountAreaAttackSkill.h"

#include "ioPlayStage.h"
#include "ioBaseChar.h"

//기본 생성자
ioProtectCountAreaAttackSkill::ioProtectCountAreaAttackSkill()
{
	m_SkillState = SS_NONE;
	m_bSetHandMesh = false;

	m_dwCurSkillProtectTime = 0;
	
	m_dwCheckProtectTime = 0;
	m_bIsAreaAttackInit = false;
	m_bIsAreaCreateWeapon = false;
	m_dwAreaFireTime = 0;
	m_dwCheckNoWoundTime = 0;
}

//복사 생성자 ( Clone 에서 쓰임)
ioProtectCountAreaAttackSkill::ioProtectCountAreaAttackSkill( const ioProtectCountAreaAttackSkill &rhs )
	: ioAttackSkill( rhs ),
	m_dwSkillProtectTime( rhs.m_dwSkillProtectTime ),
	m_dwProtectTime( rhs.m_dwProtectTime ),
	m_szLookBuffName( rhs.m_szLookBuffName),
	m_szAreaWeaponName( rhs.m_szAreaWeaponName),
	m_szAreaAttackAnimation( rhs.m_szAreaAttackAnimation ),
	m_fAreaAttackAnimationRate( rhs.m_fAreaAttackAnimationRate ),
	m_szAreaAttackEffect( rhs.m_szAreaAttackEffect ),
	m_dwAreaAttackDelayTime( rhs.m_dwAreaAttackDelayTime ),
	m_bSetAreaFireDelayTime( rhs.m_bSetAreaFireDelayTime ),
	m_dwAreaFireDelayTime( rhs.m_dwAreaFireDelayTime ),
	m_bSetNoWound( rhs.m_bSetNoWound ),
	m_dwNoWoundTime( rhs.m_dwNoWoundTime ),
	m_OwnerBuffList( rhs.m_OwnerBuffList )
{	
	m_SkillState = SS_NONE;
	m_bSetHandMesh = false;

	m_dwCurSkillProtectTime = 0;
	
	m_dwCheckProtectTime = 0;
	m_bIsAreaAttackInit = false;
	m_bIsAreaCreateWeapon = false;
	m_dwAreaFireTime = 0;
	m_dwCheckNoWoundTime = 0;
}

//소멸자
ioProtectCountAreaAttackSkill::~ioProtectCountAreaAttackSkill()
{	
}

//ini 파일 읽기
void ioProtectCountAreaAttackSkill::LoadProperty( ioINILoader &rkLoader )
{
	ioAttackSkill::LoadProperty( rkLoader );

	//추가 데이터 읽기
	LoadProtectCountProperty( rkLoader );
	LoadBuffInfoProperty( rkLoader );
	m_dwSkillProtectTime = (DWORD)rkLoader.LoadInt_e( "skill_protect_time", 0 );
}

//버프 정보 설정
void ioProtectCountAreaAttackSkill::LoadBuffInfoProperty( ioINILoader &rkLoader )
{
	char szTitle[MAX_PATH], szBuf[MAX_PATH];
	
	ioBuffInfo tBuffInfo;
	m_OwnerBuffList.clear();

	int iCount = rkLoader.LoadInt_e( "owner_buff_cnt", 0 );

	for( int i=0; i<iCount; ++i )
	{
		wsprintf_e( szTitle, "owner_buff%d", i+1 );
		rkLoader.LoadString( szTitle, "", szBuf, MAX_PATH );
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

//충전 데이터 읽기
void ioProtectCountAreaAttackSkill::LoadProtectCountProperty( ioINILoader &rkLoader )
{
	char szBuf[MAX_PATH];

	//방어 시간 설정
	m_dwProtectTime = rkLoader.LoadInt_e( "defence_time", 0 );

	//바라보기 버프 설정
	rkLoader.LoadString_e( "defence_look_buff", "", szBuf, MAX_PATH );
	m_szLookBuffName = szBuf;

	//범위 무기 설정
	rkLoader.LoadString_e( "area_weapon_name", "", szBuf, MAX_PATH );
	m_szAreaWeaponName = szBuf;

	//애니메이션 설정
	rkLoader.LoadString_e( "area_attack_animation", "", szBuf, MAX_PATH );
	m_szAreaAttackAnimation = szBuf;

	m_fAreaAttackAnimationRate = rkLoader.LoadFloat_e( "area_attack_animation_rate", FLOAT1 );
	
	//이펙트 설정
	rkLoader.LoadString_e( "area_attack_effect", "", szBuf, MAX_PATH );
	m_szAreaAttackEffect = szBuf;

	//딜레이 시간
	m_dwAreaAttackDelayTime = rkLoader.LoadInt_e( "area_attack_delay_time", 0 );

	//시간에 따라 무기 생성?
	m_bSetAreaFireDelayTime = rkLoader.LoadBool_e( "set_area_weapon_fire_time", false );
	//무기 생성 딜레이 시간
	m_dwAreaFireDelayTime = rkLoader.LoadInt_e( "area_weapon_delay_time", 0 );
	//공격 애니메이션 동안 공격을 안받음
	m_bSetNoWound = rkLoader.LoadBool_e( "area_attack_nowound", false );
	//공격 안받는 시간 설정
	m_dwNoWoundTime = rkLoader.LoadInt_e( "area_attack_nowound_time", 0 );
}

//스킬 복사 생성 후 반환
ioSkill* ioProtectCountAreaAttackSkill::Clone()
{
	return new ioProtectCountAreaAttackSkill( *this );
}

//스킬 시작 부분 설정
bool ioProtectCountAreaAttackSkill::OnSkillStart( ioBaseChar *pChar )
{	
	if( !ioAttackSkill::OnSkillStart( pChar ) )
		return false;

	if( !m_SkillSoundName.IsEmpty() && pChar )
	{
		ioEntityGroup *pGrp = pChar->GetGroup();
		m_SkillSoundID = g_SoundMgr.PlaySound( m_SkillSoundName, pGrp->GetParentSceneNode() );
	}

	m_dwCurSkillProtectTime = 0;
	if( m_dwSkillProtectTime > 0 )
		m_dwCurSkillProtectTime = FRAMEGETTIME() + m_dwSkillProtectTime;

	m_SkillState = SS_NONE;
	m_bSetHandMesh = false;
	
	m_dwCheckProtectTime = 0;
	m_bIsAreaAttackInit = false;
	m_bIsAreaCreateWeapon = false;
	m_dwAreaFireTime = 0;
	m_dwCheckNoWoundTime = 0;
	
	if( m_bUseItemMesh )
	{
		pChar->ShowWeaponItemMesh( false );
	}

	if( !m_bSetHandMesh && m_bUseItemMesh && !m_HandMesh.IsEmpty() )
	{
		pChar->ChangeSkillWeaponMesh( this, m_HandMesh, m_bVisibleEquipMesh );
		m_bSetHandMesh = true;
	}

	//자신에게 생성하는 버프추가
	for (int i=0; i < (int)m_OwnerBuffList.size(); ++i)
	{
		ioBuffInfo pOwnerBuff = m_OwnerBuffList[i];
		pChar->AddNewBuff( pOwnerBuff.m_BuffName, 
			pChar->GetCharName(),
			GetOwnerItem()->GetName(),
			this);					
	}

	return true;
}

//스킬 발동 중 설정 (Loop 상태)
void ioProtectCountAreaAttackSkill::OnProcessState( ioBaseChar *pChar, ioPlayStage *pStage )
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
				AttackSkillFire( pChar );
				EndPreDelayEffect( pChar );
			}
		}
		break;
	case SS_PROTECT:
		{
			//방어 상태에서는 시간만 확인하여 상태를 변경 (종료조건1)
			if ( m_dwCheckProtectTime < dwCurTime )
			{
				//종료 상태로 변경
				m_SkillState = SS_END;
			}
		}
		break;
	case SS_AREA_ATTACK:
		{
			if ( !m_bIsAreaAttackInit )
			{
				//공격 상태 동기화
				SendAreaAttackState( pChar );
				//바라보기 버프
				AddLookBuff( pChar );
				//애니메이션 설정
				SetAreaAttackAnimation( pChar );
				m_bIsAreaAttackInit = true;
			}

			//무기 생성
			CheckCreateAreaWeapon( pChar );

			//마지막 애니메이션 설정이 완료된 후 모션 시간을 넘어서면 종료
			if ( m_dwMotionEndTime < dwCurTime )
			{
				//종료 상태로 변경
				m_SkillState = SS_END;
			}
		}
		break;
	}
}

//스킬 종료 부분 설정
void ioProtectCountAreaAttackSkill::OnSkillEnd( ioBaseChar *pChar )
{
	//자신에게 생성하는 버프를 삭제
	for (int i=0; i < (int)m_OwnerBuffList.size(); ++i)
	{
		ioBuffInfo pOwnerBuff = m_OwnerBuffList[i];
		pChar->RemoveBuff( pOwnerBuff.m_BuffName, false);			
	}

	ioAttackSkill::OnSkillEnd( pChar );
			
	if( !m_SkillSoundName.IsEmpty() )
	{
		g_SoundMgr.StopSound( m_SkillSoundName, m_SkillSoundID );
	}

	//버프 삭제
	if ( !m_szLookBuffName.IsEmpty() && pChar->HasBuff( m_szLookBuffName ) )
	{
		pChar->RemoveBuff( m_szLookBuffName, false);	
	}
	
	pChar->EndEffect( m_szAreaAttackEffect );

	// 기본무기 장착
	if( m_bUseItemMesh )
	{
		pChar->ShowWeaponItemMesh( true );
	}

	if( !m_HandMesh.IsEmpty() )
	{
		pChar->RestoreSkillWeaponMesh( this, m_HandMesh, m_bVisibleEquipMesh );
	}
}

//메쉬 변경 설정
bool ioProtectCountAreaAttackSkill::CheckUseItemMesh()
{
	return m_bUseItemMesh;
}

//무기 속성 적용
void ioProtectCountAreaAttackSkill::ApplyAttributeToWeapon( ioWeapon *pWeapon, ioPlayStage *pStage )
{
	ioSkill::ApplyAttributeToWeapon( pWeapon, pStage );
}

//스킬 정보 추가 적용 (패킷을 받아 캐릭터의 상태를 처리, 이동, 회전, 특수 기능 동기화)
void ioProtectCountAreaAttackSkill::ApplyExtraSkillInfo( ioBaseChar *pChar, ioPlayStage *pStage, SP2Packet &rkPacket )
{	
	if( !pChar )
		return;

	int iState;
	rkPacket >> iState;

	switch( iState )
	{		
	case SS_AREA_ATTACK:
		{			
			m_SkillState = static_cast< SkillState >( iState );
			rkPacket >> m_szAttackerName;
		}
		break;
	}	
}

//스킬 종료 모션이 가능한가
bool ioProtectCountAreaAttackSkill::IsSkillMotionEnd( DWORD dwActionStop, ioBaseChar* pOwner ) const
{
	if ( m_SkillState == SS_END )
		return true;

	return false;
}

//스킬 사용시 무적 상태에 대한 처리
bool ioProtectCountAreaAttackSkill::IsProtected( int iDefenseBreakType ) const
{	
	//공격 애니메이션 중에 피격을 안당하는 상태면서, 
	//시간 설정이 없거나, 
	//시간이 있으면서 현재 시간이 확인 시간을 지나지 않았다면 무적
	if ( m_SkillState == SS_AREA_ATTACK && m_bSetNoWound && 
		( m_dwNoWoundTime == 0 || ( m_dwNoWoundTime > 0 && (m_dwCheckNoWoundTime + m_dwNoWoundTime >= FRAMEGETTIME()) ) ) )
		return true;
	
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

//예약 가능한가
bool ioProtectCountAreaAttackSkill::IsEnableReserve() const
{	
	return false;
}

//스킬이 점프 상태인가
bool ioProtectCountAreaAttackSkill::IsJumpState() const
{	
	return false;
}

//스킬이 떠있는 상태인가
bool ioProtectCountAreaAttackSkill::IsNoDropState() const
{	
	return false;
}

//패킷 (위치를 갱신) 을 보낼 수 있는 상태인가
bool ioProtectCountAreaAttackSkill::IsCanSendControl() const
{	
	return true;
}

//대쉬 확인이 가능한가
bool ioProtectCountAreaAttackSkill::IsEnableDashCheck( ioBaseChar *pChar )
{		
	return false;
}

//스킬 종료 상태인가
bool ioProtectCountAreaAttackSkill::IsEndState() const
{
	if( m_SkillState == SS_END )
		return true;

	return false;
}

//공격 완료 상태 (점프 스킬 사용 후 상태 변경) 
bool ioProtectCountAreaAttackSkill::IsAttackEndState() const
{
	return IsEndState();
}

void ioProtectCountAreaAttackSkill::AttackSkillFire( ioBaseChar *pChar )
{
	ioAttackSkill::AttackSkillFire( pChar );
	m_SkillState = SS_PROTECT;

	m_dwCheckProtectTime = FRAMEGETTIME();
	m_dwCheckProtectTime += m_dwProtectTime;	
}

//추가한 함수
//스킬 상태 확인
bool ioProtectCountAreaAttackSkill::IsProtectState() const
{
	if ( m_SkillState == SS_PROTECT )
	{
		return true;
	}

	return false;
}

//스킬 상태 변경
void ioProtectCountAreaAttackSkill::SetAreaAttackState( ioHashString szAttacker )
{
	m_szAttackerName = szAttacker;
	m_SkillState = SS_AREA_ATTACK;
}

//버프 생성
void ioProtectCountAreaAttackSkill::AddLookBuff( ioBaseChar *pChar )
{
	if ( !pChar )
		return;

	//버프 이름이 있으면서 공격자 이름이 있으면 버프 생성
	if ( !m_szLookBuffName.IsEmpty() && !m_szAttackerName.IsEmpty() )
	{
		pChar->AddNewBuff( m_szLookBuffName, m_szAttackerName, GetOwnerItem()->GetName(), this);	
	}
}

//마지막 애니메이션 설정
void ioProtectCountAreaAttackSkill::SetAreaAttackAnimation( ioBaseChar *pChar )
{
	if ( !pChar )
		return;

	ioEntityGroup *pGrp = pChar->GetGroup();
	int iAniID = pGrp->GetAnimationIdx( m_szAreaAttackAnimation );
	if( iAniID == -1 )	return;

	pChar->InitActionStopDelay( true );

	pChar->AttachEffect( m_szAreaAttackEffect );

	float fTimeRate  = m_fAreaAttackAnimationRate;
	DWORD dwPreDelay = m_dwAreaAttackDelayTime;

	pGrp->ClearAllLoopAni( FLOAT100, true );
	pGrp->ClearAllActionAni( FLOAT100, true );

	DWORD dwCurTime = FRAMEGETTIME();
	m_dwMotionEndTime = dwCurTime;
	m_dwMotionEndTime += dwPreDelay;
	m_dwMotionEndTime += pGrp->GetAnimationFullTime( iAniID ) * fTimeRate;

	m_dwAreaFireTime = dwCurTime;
	m_dwAreaFireTime += dwPreDelay;

	if ( m_bSetAreaFireDelayTime )
	{
		m_dwAreaFireTime += m_dwAreaFireDelayTime;
	}
	else
	{
		m_dwAreaFireTime += pGrp->GetAnimationEventTime_e( iAniID, "fire_s" ) * fTimeRate;
	}
	
	pGrp->SetActionAni( iAniID, FLOAT100, FLOAT100, FLOAT1, FLOAT1/fTimeRate, 0.0f, dwPreDelay, true );
	
	// Col Skip
	pChar->CheckCharColSkipSkillTime( iAniID, fTimeRate, dwPreDelay );	

	m_dwCheckNoWoundTime = dwCurTime;
}

//범위 무기 생성
void ioProtectCountAreaAttackSkill::CheckCreateAreaWeapon( ioBaseChar *pChar )
{
	//예외 처리
	//캐릭터가 없거나, 무기 이름이 없거나, 무기를 한번 생성 했거나
	if( !pChar || m_szAreaWeaponName.IsEmpty() || m_bIsAreaCreateWeapon ) 
		return;

	//Area무기가 생성될 시간이 지났다면 
	if ( m_dwAreaFireTime <= FRAMEGETTIME() )
	{			
		//각도, 위치
		D3DXVECTOR3 vPos = pChar->GetWorldPosition();
		D3DXQUATERNION qtRot = pChar->GetTargetRot();

		//Area 무기 생성
		g_AreaWeaponMgr.CreateAreaWeapon( pChar->GetCharName(), "",GetName(), m_szAreaWeaponName, vPos, qtRot, ioAreaWeapon::CT_NORMAL );

		m_bIsAreaCreateWeapon = true;
	}
}

//공격 상태 동기화
void ioProtectCountAreaAttackSkill::SendAreaAttackState( ioBaseChar *pChar )
{
	if ( !pChar )
		return;

	if ( pChar->IsNeedProcess() && pChar->IsNeedSendNetwork() )
	{
		SP2Packet kPacket( CUPK_SKILL_EXTRA_INFO );
		kPacket << pChar->GetCharName();
		kPacket << GetName();
		kPacket << SS_AREA_ATTACK;
		kPacket << m_szAttackerName;
		P2PNetwork::SendToAllPlayingUser( kPacket );
	}
}
////////////////////////////////////////////////////////////////////////
