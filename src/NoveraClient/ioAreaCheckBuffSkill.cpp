#include "StdAfx.h"
#include "ioAreaCheckBuffSkill.h"

#include "ioPlayStage.h"
#include "ioBaseChar.h"

//기본 생성자
ioAreaCheckBuffSkill::ioAreaCheckBuffSkill()
{
	m_SkillState = SS_NONE;
	m_bSetHandMesh = false;

	m_dwCurSkillProtectTime = 0;

	m_bSetAttackFire = false;

	m_dwAreaFireTime = 0;

	m_bIsBuffActionInit = false;
}

//복사 생성자 ( Clone 에서 쓰임)
ioAreaCheckBuffSkill::ioAreaCheckBuffSkill( const ioAreaCheckBuffSkill &rhs )
	: ioAttackSkill( rhs ),
	m_vBuffAction( rhs.m_vBuffAction ),
	m_dwSkillProtectTime( rhs.m_dwSkillProtectTime ),
	m_bIsCreateAreaWeapon( rhs.m_bIsCreateAreaWeapon ),
	m_AreaWeaponPos( rhs.m_AreaWeaponPos ),
	m_szAreaWeaponName( rhs.m_szAreaWeaponName ),
	m_szBuffActionEffect( rhs.m_szBuffActionEffect ),
	m_szCheckBuff( rhs.m_szCheckBuff ),
	m_EnemyBuffList( rhs.m_EnemyBuffList ),
	m_OwnerBuffList( rhs.m_OwnerBuffList ),
	m_TargetColType( rhs.m_TargetColType ),
	m_TargetWoundType( rhs.m_TargetWoundType ),
	m_fColRange( rhs.m_fColRange ),
	m_fColAngle( rhs.m_fColAngle ),
	m_fColUpHeight( rhs.m_fColUpHeight ),
	m_fColUnderHeight( rhs.m_fColUnderHeight ),
	m_fColWidth( rhs.m_fColWidth ),
	m_fColOffSet(rhs.m_fColOffSet),
	m_bSetReserveTime( rhs.m_bSetReserveTime ),
	m_dwDelayInputTime( rhs.m_dwDelayInputTime )
{	
	m_SkillState = SS_NONE;
	m_bSetHandMesh = false;

	m_dwCurSkillProtectTime = 0;

	m_bSetAttackFire = false;	

	m_dwAreaFireTime = 0;

	m_bIsBuffActionInit = false;

	m_bSetEnemyBuff = false;

	m_dwCheckInputTime = 0;
}

//소멸자
ioAreaCheckBuffSkill::~ioAreaCheckBuffSkill()
{	
}

//ini 파일 읽기
void ioAreaCheckBuffSkill::LoadProperty( ioINILoader &rkLoader )
{
	ioAttackSkill::LoadProperty( rkLoader );

	m_dwSkillProtectTime = (DWORD)rkLoader.LoadInt_e( "skill_protect_time", 0 );

	//추가 데이터 읽기
	//Area 무기를 생성할것인가
	m_bIsCreateAreaWeapon = rkLoader.LoadBool_e( "set_create_area_weapon", false );
	//Area 무기 이름 얻기
	char szKey[MAX_PATH] = "";
	rkLoader.LoadString_e( "area_weapon_name", "", szKey, MAX_PATH );
	m_szAreaWeaponName = szKey;
	//Area 무기 위치
	m_AreaWeaponPos.x = rkLoader.LoadFloat_e( "area_weapon_pos_x", 0.0f );
	m_AreaWeaponPos.y = rkLoader.LoadFloat_e( "area_weapon_pos_y", 0.0f );
	m_AreaWeaponPos.z = rkLoader.LoadFloat_e( "area_weapon_pos_z", 0.0f );

	//버프 액션 설정
	LoadBuffActionProperty( rkLoader );
	//타겟 충돌 설정
	LoadTargetInfoProperty( rkLoader );
	//버프 정보 설정
	LoadBuffInfoProperty( rkLoader );
}

//버프 액션 관련 설정
void ioAreaCheckBuffSkill::LoadBuffActionProperty( ioINILoader &rkLoader )
{
	char szKey[MAX_PATH] = "";

	wsprintf_e( szKey, "buff_action" );
	LoadAttackAttribute( szKey, m_vBuffAction, rkLoader );

	rkLoader.LoadString_e( "buff_action_effect", "", szKey, MAX_PATH );
	m_szBuffActionEffect = szKey;
	
	m_bSetReserveTime = rkLoader.LoadBool_e( "set_reserve_time", true );

	m_dwDelayInputTime = rkLoader.LoadInt_e( "delay_input_time", 0 );	
}

//충돌 관련 설정
void ioAreaCheckBuffSkill::LoadTargetInfoProperty( ioINILoader &rkLoader )
{
	m_TargetColType = (TargetColType)rkLoader.LoadInt_e( "target_collision_type", TCT_SPHERE );
	m_TargetWoundType = (TargetWoundType)rkLoader.LoadInt_e( "target_wound_type", TWT_ALL );

	m_fColRange = rkLoader.LoadFloat_e( "target_range", 0.0f );
	m_fColAngle = rkLoader.LoadFloat_e( "target_angle", 0.0f );
	m_fColOffSet = rkLoader.LoadFloat_e( "target_offset", 0.0f );

	m_fColUpHeight = rkLoader.LoadFloat_e( "target_up_height", 0.0f );
	m_fColUpHeight = max( FLOAT1, m_fColUpHeight );
	m_fColUnderHeight = rkLoader.LoadFloat_e( "target_under_height", 0.0f );
	m_fColUnderHeight = max( FLOAT1, m_fColUnderHeight );

	m_fColWidth = rkLoader.LoadFloat_e( "target_width", 0.0f );
}

//버프 정보 설정
void ioAreaCheckBuffSkill::LoadBuffInfoProperty( ioINILoader &rkLoader )
{
	char szTitle[MAX_PATH], szBuf[MAX_PATH];

	wsprintf_e( szTitle, "check_buff" );
	rkLoader.LoadString( szTitle, "", szBuf, MAX_PATH );
	m_szCheckBuff = szBuf;

	ioBuffInfo tBuffInfo;
	m_EnemyBuffList.clear();
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
	
	iCount = rkLoader.LoadInt_e( "enemy_buff_cnt", 0 );

	for( int i=0; i<iCount; ++i )
	{
		wsprintf_e( szTitle, "enemy_buff%d", i+1 );
		rkLoader.LoadString( szTitle, "", szBuf, MAX_PATH );

		tBuffInfo.m_BuffName = szBuf;

		if( !tBuffInfo.m_BuffName.IsEmpty() )
		{
			tBuffInfo.m_iBuffType = g_BuffMaker.GetBuffType( tBuffInfo.m_BuffName );

			if( tBuffInfo.m_iBuffType != BT_NONE )
			{
				m_EnemyBuffList.push_back( tBuffInfo );
			}
		}
	}	
}

//스킬 복사 생성 후 반환
ioSkill* ioAreaCheckBuffSkill::Clone()
{
	return new ioAreaCheckBuffSkill( *this );
}

//스킬 시작 부분 설정
bool ioAreaCheckBuffSkill::OnSkillStart( ioBaseChar *pChar )
{		
	m_dwEnableReserveTime = 0;
	m_dwFireMotionEndTime = 0;

	m_SkillState = SS_NONE;

	InitExtraAniJump();

	m_bSetAttackFire = false;

	if( !ioAttackSkill::OnSkillStart( pChar ) )
		return false;
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

	if( !m_SkillSoundName.IsEmpty() && pChar )
	{
		ioEntityGroup *pGrp = pChar->GetGroup();
		m_SkillSoundID = g_SoundMgr.PlaySound( m_SkillSoundName, pGrp->GetParentSceneNode() );
	}

	m_dwCurSkillProtectTime = 0;
	if( m_dwSkillProtectTime > 0 )
		m_dwCurSkillProtectTime = FRAMEGETTIME() + m_dwSkillProtectTime;
		

	m_bIsBuffActionInit = false;

	m_dwAreaFireTime = 0;

	if ( m_bIsCreateAreaWeapon )
	{
		m_bRefreshFireTimeList = false;
	}

	m_bSetEnemyBuff = false;

	m_dwCheckInputTime = 0;

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
void ioAreaCheckBuffSkill::OnProcessState( ioBaseChar *pChar, ioPlayStage *pStage )
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
	case SS_ACTION:
		{
			CheckExtraAniJump( pChar );
			CheckAniRotate( pChar );
			CheckCreateAreaWeapon( pChar );			
		}
		break;	
	case SS_CHECKBUFF:
		{			
			CheckBuff( pChar );			
		}
		break;
	}
		
	//스킬 상태가 버프확인 모션이 끝나게 된다면 종료 상태로 변경
	if ( (m_bIsBuffActionInit && m_dwMotionEndTime <= dwCurTime ) || 
		 (m_SkillState == SS_CHECKBUFF && m_dwCheckInputTime < dwCurTime) )
	{
		//종료 상태로 변경
		m_SkillState = SS_END;
	}
}

//스킬 종료 부분 설정
void ioAreaCheckBuffSkill::OnSkillEnd( ioBaseChar *pChar )
{
	//자신에게 생성하는 버프를 삭제
	for (int i=0; i < (int)m_OwnerBuffList.size(); ++i)
	{
		ioBuffInfo pOwnerBuff = m_OwnerBuffList[i];
		pChar->RemoveBuff( pOwnerBuff.m_BuffName, false);			
	}

	ioAttackSkill::OnSkillEnd( pChar );

	m_bSetAttackFire = false;

	pChar->EndEffect( m_szBuffActionEffect );

	// 기본무기 장착
	if( m_bUseItemMesh )
	{
		pChar->ShowWeaponItemMesh( true );
	}

	if( !m_HandMesh.IsEmpty() )
	{
		pChar->RestoreSkillWeaponMesh( this, m_HandMesh, m_bVisibleEquipMesh );
	}

	if( !m_SkillSoundName.IsEmpty() )
	{
		g_SoundMgr.StopSound( m_SkillSoundName, m_SkillSoundID );
	}
}

//메쉬 변경 설정
bool ioAreaCheckBuffSkill::CheckUseItemMesh()
{
	return m_bUseItemMesh;
}

//무기 속성 적용
void ioAreaCheckBuffSkill::ApplyAttributeToWeapon( ioWeapon *pWeapon, ioPlayStage *pStage )
{
	ioSkill::ApplyAttributeToWeapon( pWeapon, pStage );
}

//스킬 정보 추가 적용 (패킷을 받아 캐릭터의 상태를 처리, 이동, 회전, 특수 기능 동기화)
void ioAreaCheckBuffSkill::ApplyExtraSkillInfo( ioBaseChar *pChar, ioPlayStage *pStage, SP2Packet &rkPacket )
{	
	if( !pChar )
		return;

	int iState;
	rkPacket >> iState;

	switch( iState )
	{		
	case SS_CHECKHASBUFF:
		{
			//버프 액션 애니메이션 초기화
			if ( !m_bIsBuffActionInit )
			{
				//애니메이션 설정
				SetCheckBuffAnimation( pChar );

				m_bIsBuffActionInit = true;
			}
			//타겟에 대한 표시 버프 확인
			CheckTargetHasBuff( pChar );
		}
		break;
	}	
}

//스킬 종료 모션이 가능한가
bool ioAreaCheckBuffSkill::IsSkillMotionEnd( DWORD dwActionStop, ioBaseChar* pOwner ) const
{
	//버프 확인 상태에서는 종료 처리를 따로 함	
	if ( m_SkillState == SS_CHECKBUFF )
		return false;

	if( m_dwPreDelayStartTime > 0 )
		return false;

	if( m_dwMotionEndTime + dwActionStop >= FRAMEGETTIME() )
		return false;

	return true;
}

//스킬 사용시 무적 상태에 대한 처리
bool ioAreaCheckBuffSkill::IsProtected( int iDefenseBreakType ) const
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
//예약 가능한가
bool ioAreaCheckBuffSkill::IsEnableReserve() const
{
	if ( m_SkillState == SS_CHECKBUFF )
		return false;

	if( m_dwPreDelayStartTime != 0 )
		return false;

	if( m_dwEnableReserveTime == 0 || m_dwMotionStartTime >= m_dwEnableReserveTime )
		return false;

	if( m_dwEnableReserveTime < FRAMEGETTIME() )
		return true;

	return false;
}

//스킬이 점프 상태인가
bool ioAreaCheckBuffSkill::IsJumpState() const
{	
	return false;
}

//스킬이 떠있는 상태인가
bool ioAreaCheckBuffSkill::IsNoDropState() const
{	
	return false;
}

//패킷 (위치를 갱신) 을 보낼 수 있는 상태인가
bool ioAreaCheckBuffSkill::IsCanSendControl() const
{	
	if( m_bSetAttackFire )
		return false;

	return true;
}

//대쉬 확인이 가능한가
bool ioAreaCheckBuffSkill::IsEnableDashCheck( ioBaseChar *pChar )
{	
	switch( m_SkillState )
	{
	case SS_ACTION:
		return true;
	}

	return false;
}

//스킬 종료 상태인가
bool ioAreaCheckBuffSkill::IsEndState() const
{
	if( m_SkillState == SS_END )
		return true;

	return false;
}

//공격 완료 상태 (점프 스킬 사용 후 상태 변경) 
bool ioAreaCheckBuffSkill::IsAttackEndState() const
{
	return IsEndState();
}

//공격 스킬 발동
void ioAreaCheckBuffSkill::AttackSkillFire( ioBaseChar *pChar )
{
	//기본 무기 생성 및 애니메이션
	ioAttackSkill::AttackSkillFire( pChar );	

	//Area 무기를 생성한다면
	if ( m_bIsCreateAreaWeapon )
	{
		//Area 무기 생성 시간을 얻기 위해
		ioEntityGroup *pGrp = pChar->GetGroup();
		int iAniID = pGrp->GetAnimationIdx( GetAttribute().m_AttackAnimation );
		if( iAniID == -1 )	return;

		DWORD dwPreDelay = GetAttribute().m_dwPreDelay;

		DWORD dwCurTime = FRAMEGETTIME();
		m_dwAreaFireTime = dwCurTime;
		m_dwAreaFireTime += dwPreDelay;
		m_dwAreaFireTime += pGrp->GetAnimationEventTime( iAniID, "fire_s" ) * GetAttackTimeRate();
	}
			
	m_bSetAttackFire = true;
	m_SkillState = SS_ACTION;
}

//Area무기 생성 확인
void ioAreaCheckBuffSkill::CheckCreateAreaWeapon( ioBaseChar *pChar )
{
	//예외 처리
	//캐릭터가 없거나, 무기 생성을 안하거나, 무기 이름이 없거나
	if( !pChar || !m_bIsCreateAreaWeapon || m_szAreaWeaponName.IsEmpty() ) 
	{
		//스테이트 변경
		m_SkillState = SS_CHECKBUFF;
		return;
	}

	//Area무기가 생성될 시간이 지났다면 
	if ( m_dwAreaFireTime <= FRAMEGETTIME() )
	{			
		//각도, 위치
		D3DXVECTOR3 vPos = pChar->GetWorldPosition() + m_AreaWeaponPos;
		D3DXQUATERNION qtRot = pChar->GetTargetRot();

		//Area 무기 생성
		g_AreaWeaponMgr.CreateAreaWeapon( pChar->GetCharName(), "", GetName(), m_szAreaWeaponName, vPos, qtRot, ioAreaWeapon::CT_NORMAL );

		//스테이트 변경
		m_SkillState = SS_CHECKBUFF;

		if ( !m_bSetReserveTime )
		{
			m_dwCheckInputTime = FRAMEGETTIME() + m_dwDelayInputTime;
		}
	}
}

//버프 확인 스테이트
void ioAreaCheckBuffSkill::CheckBuff( ioBaseChar *pChar )
{
	//예외처리
	if ( !pChar || m_bSetEnemyBuff )
		return;
	
	DWORD dwCurTime = FRAMEGETTIME();
	
	//입력 가능 시간이 현재 시간을 넘었으면서, 실행 가능 상태면
	if( pChar->IsNeedProcess() && 
		( ( m_bSetReserveTime && m_dwEnableReserveTime <= dwCurTime ) || 
		  ( !m_bSetReserveTime && m_dwCheckInputTime >= FRAMEGETTIME() ) 
		))
	{				
		//공격 키 입력 확인 후
		if( pChar->IsAttackKey() )
		{
			//버프 액션 애니메이션 초기화
			if ( !m_bIsBuffActionInit )
			{
				//애니메이션 설정
				SetCheckBuffAnimation( pChar );

				m_bIsBuffActionInit = true;
			}

			//표시 버프를 확인
			CheckTargetHasBuff( pChar );

			//패킷을 전송
			if( pChar->IsNeedSendNetwork() )
			{
				SP2Packet kPacket( CUPK_SKILL_EXTRA_INFO );
				kPacket << pChar->GetCharName();
				kPacket << GetName();
				kPacket << SS_CHECKHASBUFF;
				P2PNetwork::SendToAllPlayingUser( kPacket );
			}
			//버프를 설정했다.
			m_bSetEnemyBuff = true;
		}
	}	
}

//버프 확인 애니메이션 설정
void ioAreaCheckBuffSkill::SetCheckBuffAnimation( ioBaseChar *pChar )
{
	ioEntityGroup *pGrp = pChar->GetGroup();
	int iAniID = pGrp->GetAnimationIdx( m_vBuffAction.m_AttackAnimation );
	if( iAniID == -1 )	return;

	pChar->InitActionStopDelay( true );

	pChar->AttachEffect( m_szBuffActionEffect );

	float fTimeRate  = m_vBuffAction.m_fAttackAniRate;
	DWORD dwPreDelay = m_vBuffAction.m_dwPreDelay;
	
	pGrp->ClearAllLoopAni( FLOAT100, true );
	pGrp->ClearAllActionAni( FLOAT100, true );

	DWORD dwCurTime = FRAMEGETTIME();
	m_dwMotionEndTime = dwCurTime;
	m_dwMotionEndTime += pGrp->GetAnimationFullTime( iAniID ) * fTimeRate;
	m_dwMotionEndTime += dwPreDelay + m_vBuffAction.m_dwEndDelay;
	
	float fAttackEnd = pGrp->GetAnimationEventTime( iAniID, "attack_end" ) * fTimeRate;
	
	if( fAttackEnd > 0.0f )
		m_dwAttackEndTime = dwCurTime + fAttackEnd;
	else
		m_dwAttackEndTime = 0;
	
	if( m_vBuffAction.m_AttackEndAnimation.IsEmpty() )
	{
		pGrp->SetActionAni( iAniID, FLOAT100, FLOAT100, FLOAT1, FLOAT1/fTimeRate, 0.0f, dwPreDelay, true );
	}
	else
	{
		pGrp->SetActionAni( iAniID, FLOAT100, 0.0f, FLOAT1, FLOAT1/fTimeRate, 0.0f, dwPreDelay );

		float fWaitTime = pGrp->GetAnimationFullTime( iAniID ) * fTimeRate + dwPreDelay;
		float fEndTimeRate = m_vBuffAction.m_fAttackEndAniRate;

		pGrp->SetActionAni( m_vBuffAction.m_AttackEndAnimation,
			0.0f, FLOAT100,
			1.0f, FLOAT1/fEndTimeRate,
			fWaitTime, 0.0f, true );

		int iEndAniID = pGrp->GetAnimationIdx( m_vBuffAction.m_AttackEndAnimation );
		m_dwMotionEndTime += pGrp->GetAnimationFullTime( iEndAniID ) * fEndTimeRate;
	}

	// Col Skip
	pChar->CheckCharColSkipSkillTime( iAniID, fTimeRate, dwPreDelay );

	SetExtraAniJump( pChar, iAniID, fTimeRate );

	pChar->SetReservedSliding( m_vBuffAction.m_vForceInfoList, iAniID, fTimeRate, dwPreDelay );
	
	if( m_vBuffAction.m_vWeaponInfoList.size() > 0 )
	{
		pChar->RefreshFireTimeListWithBase( m_dwWeaponBaseIndex,
			iAniID,
			m_vBuffAction.m_vWeaponInfoList,
			FTT_SKILL_ATTACK,
			fTimeRate,
			dwPreDelay );
	}
}

//실제 타겟에서 표시버프가 있는가 확인
void ioAreaCheckBuffSkill::CheckTargetHasBuff( ioBaseChar *pChar )
{		
	if( !pChar ) 
		return;

	if( m_EnemyBuffList.empty() )
		return;
	
	//타겟 리스트
	BaseCharList vTargetList;
	//위치 설정
	D3DXVECTOR3 vPos = pChar->GetMidPositionByRate();
	D3DXVECTOR3 vSightDir = pChar->GetTargetRot() * D3DXVECTOR3( 0.0f, 0.0f, -FLOAT1 );
	D3DXVec3Normalize( &vSightDir, &vSightDir );

	if( m_fColOffSet != 0.0f )
		vPos += vSightDir * m_fColOffSet;

	// 충돌타입에 따라 리스트 가져오기
	switch( m_TargetColType )
	{
	case TCT_SPHERE:
		pChar->GetTargetCharListBySphere( vPos, m_fColRange, m_fColAngle, vTargetList );
		break;
	case TCT_CYLINDER:
		pChar->GetTargetCharListByCylinder( vPos, m_fColRange, m_fColAngle, m_fColUpHeight, m_fColUnderHeight, vTargetList );
		break;
	case TCT_BOX:
		// 사각형의 center을 vPos로 설정하게 되므로 여기서 캐릭터앞쪽에 생성하고 높이를 보정한다.
		vPos += vSightDir * m_fColRange * FLOAT05; 
		D3DXVECTOR3 vOffset( 0.0f, ( ( m_fColUpHeight + m_fColUnderHeight ) * FLOAT05 ) - m_fColUpHeight, 0.0f );
		vPos -= vOffset;
		pChar->GetTargetCharListByBox( vPos, m_fColRange, m_fColWidth, m_fColUpHeight, m_fColUnderHeight, vTargetList );
		break;
	}

	//타겟 리스트가 비어있다면 종료
	if ( vTargetList.empty() )
		return;

	int nTargetCnt = vTargetList.size();	

	int nBuffCnt = m_EnemyBuffList.size();

	for ( int i=0; i < nTargetCnt; ++i )
	{
		ioBaseChar* pTarget = vTargetList[i];
		//같은 팀이면 확인하지 않음
		if ( pChar->GetTeam() == pTarget->GetTeam() )
			continue;

		//표시 버프를 갖고 있다면
		if ( pTarget->HasBuff( m_szCheckBuff ) )
		{
			if( pTarget->IsProtectState() )
				continue;
						
			//해당 버프를 생성
			for( int i=0; i < nBuffCnt; ++i )
			{
				ioHashString szBuffName = m_EnemyBuffList[i].m_BuffName;
				
				if( !szBuffName.IsEmpty() )
				{
					if( pTarget->IsHasCrown() && m_bDisableKingSkill )
					{
						continue;
					}

					pTarget->AddNewBuff( szBuffName, pChar->GetCharName(), GetOwnerItem()->GetName(), this );					
				}
			}
		}
	}
}
////////////////////////////////////////////////////////////////////////